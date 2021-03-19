/*
 ============================================================================
 Name        : RFtermBT.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Bluetooth client.
 ============================================================================
 */

// INCLUDE FILES
#include <StringLoader.h>
#include <coemain.h>
#include "RFtermBtServiceSearcher.h"
#include "RFtermConstants.h"
#include "Bt.pan"
#include "RFtermBt.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRFtermBt::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBt* CRFtermBt::NewL()
	{
	CRFtermBt* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBt* CRFtermBt::NewLC()
	{
	CRFtermBt* self = new (ELeave) CRFtermBt();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::CRFtermBt()
// Constructor.
// ----------------------------------------------------------------------------
//
CRFtermBt::CRFtermBt() :
	CActive(CActive::EPriorityStandard),
	iState(EWaitingToGetDevice)
	{
	CActiveScheduler::Add(this);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::~CRFtermBt()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermBt::~CRFtermBt()
	{
	// Close() will wait forever for Read to complete
	if (State() == EConnected)
		{
		if (iActiveSocket)
			{
			iActiveSocket->CancelRead();
			}
		}
	Cancel();

	delete iBatteryStatus;
	iBatteryStatus = NULL;

	iBTPhysicalLinkAdapter.Close();

	iSocket.Close();
	iAcceptedSocket.Close();
	iSocketServer.Close();

	delete iMessage;
	iMessage = NULL;

	delete iServiceSearcher;
	iServiceSearcher = NULL;

	delete iRemoteDevice;
	iRemoteDevice = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CRFtermBt::ConstructL()
	{
	iServiceSearcher = CRFtermBtServiceSearcher::NewL();
	iServiceSearcher->SetObserver(iObserver);
	User::LeaveIfError(iSocketServer.Connect());
	iBatteryStatus = CRFtermBatteryStatus::NewL(this);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CRFtermBt::DoCancel()
	{
	if (iState == EGettingService)
		{
		// Interrupt the work of CRFtermBtServiceSearcher.
		// iStatus of CRFtermBt is used by CRFtermBtServiceSearcher.
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNotFound);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CRFtermBt::RunL()
	{

	HBufC* textResource = NULL;

	if (iStatus == KErrHCILinkDisconnection)
		// this error happens if connected server
		// sudently loses link (powered down for example)
		{
		textResource = StringLoader::LoadLC(R_ERR_LOST_CONNECTION);
		NotifyL(*textResource);
		CleanupStack::PopAndDestroy(textResource);
		if (State() != EDisconnected)
			{
			NotifyDeviceIsDisconnectedL();
			
			iSocket.CancelAll();
			iSocket.Close();
			}
		SetState(EWaitingToGetDevice);
		return;
		}

	else if (iStatus != KErrNone)
		{
		switch (State())
			{
			case EGettingDevice:
				if (iStatus == KErrCancel)
					{
					textResource = StringLoader::LoadLC(R_ERR_NO_DEVICE_SELECTED);
					NotifyL(*textResource);
					CleanupStack::PopAndDestroy(textResource);
					}
				else
					{
					textResource = StringLoader::LoadLC(R_ERR_CANT_GET_DEVICE_LIST);
					NotifyL(*textResource);
					CleanupStack::PopAndDestroy(textResource);
					}
				SetState(EWaitingToGetDevice);
				break;
				
			case EGettingService:
			case EGettingConnection:
				textResource = StringLoader::LoadLC(R_ERR_CONNECTION_ERROR);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				SetState(EWaitingToGetDevice);
				break;
				
			case EConnected:
				textResource = StringLoader::LoadLC(R_ERR_LOST_CONNECTION);
				NotifyL(*textResource);
				DisconnectFromServerL();
				CleanupStack::PopAndDestroy(textResource);
				SetState(EDisconnecting);
				break;
				
			case ESendingMessage:
				textResource = StringLoader::LoadLC(R_ERR_MESSAGE_FAILED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				DisconnectFromServerL();
				SetState(EDisconnecting);
				break;
				
			case EDisconnecting:
				if (iStatus == KErrDisconnected)
					{
					textResource = StringLoader::LoadLC(R_STR_DISCONNECT_COMPLETE);
					NotifyL(*textResource);
					CleanupStack::PopAndDestroy(textResource);

					SetState(EWaitingToGetDevice);
					}
				else
					{
					textResource = StringLoader::LoadLC(R_ERR_FAILED_TO_DISCONNECT);
					NotifyL(*textResource);
					CleanupStack::PopAndDestroy(textResource);

					Panic(ERFtermUnableToDisconnect);
					}
				break;
			
			case EWaitingToGetDevice:
				textResource = StringLoader::LoadLC(R_STR_DISCONNECTED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				break;
				
			default:
				Panic(ERFtermInvalidLogicState);
				break;
			}
		}

	else
		{
		switch (State())
			{
			case EGettingDevice:
				// found a device now search for a suitable service
				NotifyL(iServiceSearcher->ResponseParams().DeviceName());
				SetState(EGettingService);
				iStatus = KRequestPending;  // this means that the RunL 
											// can not be called until
											// this program does something 
											// to iStatus
				iServiceSearcher->FindServiceL(iStatus);
				SetActive();
				break;
				
			case EGettingService:
				textResource = StringLoader::LoadLC(R_STR_FOUND_SERVICE);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				SetState(EGettingConnection);
				ConnectToServerL();
				break;
				
			case EGettingConnection:
				textResource = StringLoader::LoadLC(R_STR_CONNECTED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);

				SetState(EConnected);
				PreventLowPowerModes();
				RequestData();
				NotifyDeviceIsConnectedL();
				break;
				
			case EConnected:
				textResource = HBufC::NewLC(iBuffer.Length());
				textResource->Des().Copy(iBuffer); // Convert buf8 to buf16
				if (iObserver)
					{
					iObserver->HandleBtDataL(*textResource);
					}
				iBuffer.Zero();
				CleanupStack::PopAndDestroy(textResource);
				RequestData();
				break;
				
			case ESendingMessage:
				SetState(EConnected);
				RequestData();
				break;
				
			case EDisconnecting:
				textResource = StringLoader::LoadLC(R_STR_DISCONNECT_COMPLETE);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy (textResource);
				iSocket.Close();
				SetState(EWaitingToGetDevice);
				break;
			
			case EWaitingToGetDevice:
				break;
				
			default:
				Panic(ERFtermInvalidLogicState);
				break;
			};
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::SetState()
// 
// ----------------------------------------------------------------------------
//
void CRFtermBt::SetState(TRFtermState aState)
	{
	iState = aState;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::State()
// 
// ----------------------------------------------------------------------------
//
TInt CRFtermBt::State()
	{
	return iState;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsReadyToSend()
// True if the client can send a message.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::IsReadyToSend()
	{
	return (State() == EConnected);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsConnected()
// ETrue if the client is fully connected to the server.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::IsConnected()
	{
	return ((State() == EConnected) || (State() == ESendingMessage));
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsConnecting()
// True if is establishing a connection.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::IsConnecting()
	{
	return ((State() == EGettingDevice)
		||
		(State() == EGettingService)
		||
		(State() == EGettingConnection)
		);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CRFtermBt::ConnectL()
	{
	if (State() == EWaitingToGetDevice && !IsActive())
		{
		SetState(EGettingDevice);
		iServiceSearcher->SelectDeviceByDiscoveryL(iStatus);
		SetActive();
		}
	else
		{
		HBufC* errClientBusy = StringLoader::LoadLC(R_STR_CLIENT_BUSY);
		NotifyL(*errClientBusy);
		CleanupStack::PopAndDestroy(errClientBusy);
		
		User::Leave(KErrInUse);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CRFtermBt::DisconnectL()
	{
	if (IsConnected())
		{
		DisconnectFromServerL();
		SetState(EDisconnecting);
		}
	else
		{
		HBufC* errNoConn = StringLoader::LoadLC(R_ERR_NO_CONN);
		NotifyL(*errNoConn);
		CleanupStack::PopAndDestroy(errNoConn);
		User::Leave(KErrDisconnected);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::DisconnectFromServerL()
// Disconnects from the service
// ----------------------------------------------------------------------------
//
void CRFtermBt::DisconnectFromServerL()
	{
	NotifyDeviceIsDisconnectedL();

	// Terminate all operations
	iSocket.CancelAll();
	Cancel();

	HBufC* strReleasingConn = StringLoader::LoadLC(R_STR_RELEASING_CONN);
	NotifyL(*strReleasingConn);
	CleanupStack::PopAndDestroy(strReleasingConn);
	iSocket.Shutdown(RSocket::ENormal, iStatus);
	SetActive();
	}

// ----------------------------------------------------------------------------
// CRFtermBt::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CRFtermBt::ConnectToServerL()
	{
	HBufC* strConnecting = StringLoader::LoadLC(R_STR_CONNECTING);
	NotifyL(*strConnecting);
	CleanupStack::PopAndDestroy(strConnecting);

	User::LeaveIfError(iSocket.Open(iSocketServer, KStrRFCOMM));

	TBTSockAddr address;
	address.SetBTAddr(iServiceSearcher->BTDevAddr());
	address.SetPort(iServiceSearcher->Port());

	iSocket.Connect(address, iStatus);

	iActiveSocket = &iSocket;

	#ifdef __WINS__
		// Fix to allow emulator client to connect to server
		User::After(1);
	#endif

	SetActive();
	}

// ----------------------------------------------------------------------------
// CRFtermBt::SendMessageL()
// Send a message to a service on a remote machine.
// ----------------------------------------------------------------------------
//
void CRFtermBt::SendMessageL(const TDesC& aText)
	{
	if (State() != EConnected)
		{
		User::Leave(KErrDisconnected);
		}

	// stop reading socket
	if (iActiveSocket)
		{
		iActiveSocket->CancelRead();
		}

	if (IsActive())
		{
		Cancel();
		}
	SetState(ESendingMessage);

	// convert 16-bit to 8-bit data
	delete iMessage;
	iMessage = HBufC8::NewL(aText.Length());
	iMessage->Des().Copy(aText);

	if (iObserver)
		{
		iObserver->HandleBtDataL(aText);
		}

	if (iActiveSocket)
		{
		iActiveSocket->Write(*iMessage, iStatus);
		}

	SetActive();
	}

// ----------------------------------------------------------------------------
// CRFtermBt::RequestData()
// Request data from the client.
// ----------------------------------------------------------------------------
//
void CRFtermBt::RequestData()
	{
	if (iActiveSocket)
		{
		iActiveSocket->RecvOneOrMore(iBuffer, 0, iStatus, iLen);
		}
	SetActive();
	}

// ----------------------------------------------------------------------------
// CRFtermBt::SetObserver()
// Connect an observer for notifications.
// ----------------------------------------------------------------------------
//
void CRFtermBt::SetObserver(MRFtermBtObserver* aObserver)
	{
	iObserver = aObserver;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::NotifyL()
// Send to observer a log message.
// ----------------------------------------------------------------------------
//
void CRFtermBt::NotifyL(const TDesC& aMessage)
	{
	if (iObserver)
		{
		iObserver->HandleBtNotifyL(aMessage);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::NotifyDeviceIsConnectedL()
// Send to observer information about connected BT device.
// ----------------------------------------------------------------------------
//
void CRFtermBt::NotifyDeviceIsConnectedL()
	{
	if (iObserver)
		{
		RHostResolver hostResolver;
		TProtocolDesc pInfo;
		_LIT(KLinkMan, "BTLinkManager");
		TProtocolName name(KLinkMan);
		User::LeaveIfError(iSocketServer.FindProtocol(name, pInfo));
		// Open an appropriate host resolver
		User::LeaveIfError(hostResolver.Open(iSocketServer, pInfo.iAddrFamily, pInfo.iProtocol));
		// Set up inquiry address
		TBTSockAddr socketAddress;
		iActiveSocket->RemoteName(socketAddress);
		TInquirySockAddr inquiryAddr(socketAddress);
		inquiryAddr.SetAction(KHostResCache);
		TNameEntry remoteDevInfo;
		hostResolver.GetByAddress(inquiryAddr, remoteDevInfo);

		delete iRemoteDevice;
		TBTDevAddr remoteDevAddr = static_cast<TBTSockAddr>(socketAddress).BTAddr();
		iRemoteDevice = CBTDevice::NewL(remoteDevAddr);
		HBufC8* nameBuf = HBufC8::NewLC(remoteDevInfo().iName.Length());
		nameBuf->Des().Copy(remoteDevInfo().iName);
		iRemoteDevice->SetDeviceNameL(*nameBuf);
		CleanupStack::PopAndDestroy(nameBuf);

		iObserver->HandleBtDeviceChangeL(iRemoteDevice);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::NotifyDeviceIsDisconnectedL()
// Send to observer BT device disconnect notify.
// ----------------------------------------------------------------------------
//
void CRFtermBt::NotifyDeviceIsDisconnectedL()
	{
	delete iRemoteDevice;
	iRemoteDevice = NULL;
	if (iObserver)
		{
		iObserver->HandleBtDeviceChangeL(iRemoteDevice);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::AllowLowPowerModes()
// Enable low power modes to save the battery.
// ----------------------------------------------------------------------------
void CRFtermBt::AllowLowPowerModes()
	{
	TInt error = iBTPhysicalLinkAdapter.Open(iSocketServer, *iActiveSocket);
	if (error == KErrNone)
		{
		iBTPhysicalLinkAdapter.AllowLowPowerModes(EAnyLowPowerMode);
		iBTPhysicalLinkAdapter.Close();
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::PreventLowPowerModes()
// Disable low power modes to reach max bandwidth.
// ----------------------------------------------------------------------------
void CRFtermBt::PreventLowPowerModes()
	{
	if (iBatteryStatus->IsOK())
		{
		TInt error = iBTPhysicalLinkAdapter.Open(iSocketServer, *iActiveSocket);
		if (error == KErrNone)
			{
			iBTPhysicalLinkAdapter.PreventLowPowerModes(EAnyLowPowerMode);
			iBTPhysicalLinkAdapter.Close();
			}
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::HandleBatteryStatusChangeL()
// Battery status change notify.
// ----------------------------------------------------------------------------
//
void CRFtermBt::HandleBatteryStatusChangeL()
	{
	if (IsConnected())
		{
		if (iBatteryStatus->IsOK())
			{
			PreventLowPowerModes();
			}
		else
			{
			AllowLowPowerModes();
			}
		}
	}

// End of File

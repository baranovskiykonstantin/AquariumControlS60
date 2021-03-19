/*
 ============================================================================
 Name        : BtClient.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Bluetooth client.
 ============================================================================
 */

// INCLUDE FILES
#include <StringLoader.h>
#include <coemain.h>
#include "BtServiceSearcher.h"
#include "BtClientConstants.h"
#include "BtClient.pan"
#include "BtClient.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBtClient::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtClient* CBtClient::NewL()
	{
	CBtClient* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CBtClient::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtClient* CBtClient::NewLC()
	{
	CBtClient* self = new (ELeave) CBtClient();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CBtClient::CBtClient()
// Constructor.
// ----------------------------------------------------------------------------
//
CBtClient::CBtClient() :
	CActive(CActive::EPriorityStandard),
	iState(EWaitingToGetDevice)
	{
	CActiveScheduler::Add(this);
	}

// ----------------------------------------------------------------------------
// CBtClient::~CBtClient()
// Destructor.
// ----------------------------------------------------------------------------
//
CBtClient::~CBtClient()
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
// CBtClient::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CBtClient::ConstructL()
	{
	iServiceSearcher = CBtServiceSearcher::NewL();
	iServiceSearcher->SetObserver(iObserver);
	User::LeaveIfError(iSocketServer.Connect());
	iBatteryStatus = CBatteryStatus::NewL(this);
	}

// ----------------------------------------------------------------------------
// CBtClient::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CBtClient::DoCancel()
	{
	if (iState == EGettingService)
		{
		// Interrupt the work of CBtServiceSearcher.
		// iStatus of CBtClient is used by CBtServiceSearcher.
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNotFound);
		}
	}

// ----------------------------------------------------------------------------
// CBtClient::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CBtClient::RunL()
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

					Panic(EBtClientUnableToDisconnect);
					}
				break;
			
			case EWaitingToGetDevice:
				textResource = StringLoader::LoadLC(R_STR_DISCONNECTED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				break;
				
			default:
				Panic(EBtClientInvalidLogicState);
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
				Panic(EBtClientInvalidLogicState);
				break;
			};
		}
	}

// ----------------------------------------------------------------------------
// CBtClient::SetState()
// 
// ----------------------------------------------------------------------------
//
void CBtClient::SetState(TBtClientState aState)
	{
	iState = aState;
	}

// ----------------------------------------------------------------------------
// CBtClient::State()
// 
// ----------------------------------------------------------------------------
//
TInt CBtClient::State()
	{
	return iState;
	}

// ----------------------------------------------------------------------------
// CBtClient::IsReadyToSend()
// True if the client can send a message.
// ----------------------------------------------------------------------------
//
TBool CBtClient::IsReadyToSend()
	{
	return (State() == EConnected);
	}

// ----------------------------------------------------------------------------
// CBtClient::IsConnected()
// ETrue if the client is fully connected to the server.
// ----------------------------------------------------------------------------
//
TBool CBtClient::IsConnected()
	{
	return ((State() == EConnected) || (State() == ESendingMessage));
	}

// ----------------------------------------------------------------------------
// CBtClient::IsConnecting()
// True if is establishing a connection.
// ----------------------------------------------------------------------------
//
TBool CBtClient::IsConnecting()
	{
	return ((State() == EGettingDevice)
		||
		(State() == EGettingService)
		||
		(State() == EGettingConnection)
		);
	}

// ----------------------------------------------------------------------------
// CBtClient::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CBtClient::ConnectL()
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
// CBtClient::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CBtClient::DisconnectL()
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
// CBtClient::DisconnectFromServerL()
// Disconnects from the service
// ----------------------------------------------------------------------------
//
void CBtClient::DisconnectFromServerL()
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
// CBtClient::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CBtClient::ConnectToServerL()
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
// CBtClient::SendMessageL()
// Send a message to a service on a remote machine.
// ----------------------------------------------------------------------------
//
void CBtClient::SendMessageL(const TDesC& aText)
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
// CBtClient::RequestData()
// Request data from the client.
// ----------------------------------------------------------------------------
//
void CBtClient::RequestData()
	{
	if (iActiveSocket)
		{
		iActiveSocket->RecvOneOrMore(iBuffer, 0, iStatus, iLen);
		}
	SetActive();
	}

// ----------------------------------------------------------------------------
// CBtClient::SetObserver()
// Connect an observer for notifications.
// ----------------------------------------------------------------------------
//
void CBtClient::SetObserver(MBtClientObserver* aObserver)
	{
	iObserver = aObserver;
	}

// ----------------------------------------------------------------------------
// CBtClient::NotifyL()
// Send to observer a log message.
// ----------------------------------------------------------------------------
//
void CBtClient::NotifyL(const TDesC& aMessage)
	{
	if (iObserver)
		{
		iObserver->HandleBtNotifyL(aMessage);
		}
	}

// ----------------------------------------------------------------------------
// CBtClient::NotifyDeviceIsConnectedL()
// Send to observer information about connected BT device.
// ----------------------------------------------------------------------------
//
void CBtClient::NotifyDeviceIsConnectedL()
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
// CBtClient::NotifyDeviceIsDisconnectedL()
// Send to observer BT device disconnect notify.
// ----------------------------------------------------------------------------
//
void CBtClient::NotifyDeviceIsDisconnectedL()
	{
	delete iRemoteDevice;
	iRemoteDevice = NULL;
	if (iObserver)
		{
		iObserver->HandleBtDeviceChangeL(iRemoteDevice);
		}
	}

// ----------------------------------------------------------------------------
// CBtClient::AllowLowPowerModes()
// Enable low power modes to save the battery.
// ----------------------------------------------------------------------------
void CBtClient::AllowLowPowerModes()
	{
	TInt error = iBTPhysicalLinkAdapter.Open(iSocketServer, *iActiveSocket);
	if (error == KErrNone)
		{
		iBTPhysicalLinkAdapter.AllowLowPowerModes(EAnyLowPowerMode);
		iBTPhysicalLinkAdapter.Close();
		}
	}

// ----------------------------------------------------------------------------
// CBtClient::PreventLowPowerModes()
// Disable low power modes to reach max bandwidth.
// ----------------------------------------------------------------------------
void CBtClient::PreventLowPowerModes()
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
// CBtClient::HandleBatteryStatusChangeL()
// Battery status change notify.
// ----------------------------------------------------------------------------
//
void CBtClient::HandleBatteryStatusChangeL()
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
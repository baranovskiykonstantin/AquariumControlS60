/*
 ============================================================================
 Name        : BtClient.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Bluetooth client.
 ============================================================================
 */

// INCLUDE FILES
#include <stringloader.h>
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
		// Try to save, ignore error
		TRAPD(error, iServiceSearcher->SaveBtDeviceInfoL());
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

	HideWaitDialog();
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
	// Try to restore connection with
	// the previously used BT device
	if (iServiceSearcher->LoadBtDeviceInfoL())
		{
		ShowWaitDialog();
		SetState(EGettingDevice);
		SetActive();
		// Device is gotten from the previous session,
		// go to getting service
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
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
		NotifyL(R_ERR_LOST_CONNECTION, ETrue);
		if (IsConnecting() || IsConnected())
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
					NotifyL(R_ERR_NO_DEVICE_SELECTED);
					}
				else
					{
					NotifyL(R_ERR_CANT_GET_DEVICE_LIST, ETrue);
					}
				SetState(EWaitingToGetDevice);
				break;
				
			case EGettingService:
			case EGettingConnection:
				NotifyL(R_ERR_CONNECTION_ERROR, ETrue);
				SetState(EWaitingToGetDevice);
				break;
				
			case EConnected:
				NotifyL(R_ERR_LOST_CONNECTION, ETrue);
				DisconnectFromServerL();
				SetState(EDisconnecting);
				break;
				
			case ESendingMessage:
				NotifyL(R_ERR_MESSAGE_FAILED, ETrue);
				DisconnectFromServerL();
				SetState(EDisconnecting);
				break;
				
			case EDisconnecting:
				if (iStatus == KErrDisconnected)
					{
					NotifyL(R_STR_DISCONNECT_COMPLETE);
					SetState(EWaitingToGetDevice);
					}
				else
					{
					NotifyL(R_ERR_FAILED_TO_DISCONNECT, ETrue);
					Panic(EBtClientUnableToDisconnect);
					}
				break;
			
			case EWaitingToGetDevice:
				NotifyL(R_STR_DISCONNECTED);
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
				{
				ShowWaitDialog();

				_LIT(KPrepend, "\n(");
				_LIT(KByteSeparator, ":");
				_LIT(KAppend, ")");
				TBuf<20> addr;
				iServiceSearcher->ResponseParams().BDAddr().GetReadable(addr, KPrepend, KByteSeparator, KAppend);
				addr.UpperCase();

				HBufC* conTo = StringLoader::LoadLC(R_WAITDIALOG_CONNECTING_TO);
				HBufC* label = HBufC::NewLC(conTo->Length() + iServiceSearcher->ResponseParams().DeviceName().Length() + addr.Length());
				label->Des().Copy(*conTo);
				label->Des().Append(iServiceSearcher->ResponseParams().DeviceName());
				label->Des().Append(addr);
				SetWaitDialogLabel(*label);
				CleanupStack::PopAndDestroy(2); // label, conTo

				// found a device now search for a suitable service
				NotifyL(iServiceSearcher->ResponseParams().DeviceName());
				SetState(EGettingService);
				iStatus = KRequestPending;  // this means that the RunL 
											// can not be called until
											// this program does something 
											// to iStatus
				iServiceSearcher->FindServiceL(iStatus);
				SetActive();
				}
				break;
				
			case EGettingService:
				NotifyL(R_STR_FOUND_SERVICE);
				SetState(EGettingConnection);
				ConnectToServerL();
				break;
				
			case EGettingConnection:
				NotifyL(R_STR_CONNECTED);
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
				NotifyL(R_STR_DISCONNECT_COMPLETE);
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
		NotifyL(R_STR_CLIENT_BUSY, ETrue);
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
		NotifyL(R_ERR_NO_CONN, ETrue);
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

	NotifyL(R_STR_RELEASING_CONN);
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
	NotifyL(R_STR_CONNECTING);

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
void CBtClient::NotifyL(const TDesC& aMessage, TBool aIsError)
	{
	if (aIsError)
		HideWaitDialog();

	if (iObserver)
		{
		iObserver->HandleBtNotifyL(aMessage, aIsError);
		}
	}
void CBtClient::NotifyL(TInt aMessageResourceId, TBool aIsError)
	{
	HBufC* textResource = StringLoader::LoadLC(aMessageResourceId);
	NotifyL(*textResource, aIsError);
	CleanupStack::PopAndDestroy(textResource);
	}

// ----------------------------------------------------------------------------
// CBtClient::NotifyDeviceIsConnectedL()
// Send to observer information about connected BT device.
// ----------------------------------------------------------------------------
//
void CBtClient::NotifyDeviceIsConnectedL()
	{
	HideWaitDialog();
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
	HideWaitDialog();
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

// ----------------------------------------------------------------------------
// CBtClient::ShowWaitDialog()
// Hide connection notification.
// ----------------------------------------------------------------------------
//
void CBtClient::ShowWaitDialog()
	{
	if(iWaitDialog)
		return;
	iWaitDialog = new(ELeave)CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iWaitDialog)));
	iWaitDialog->SetCallback(this);
	iWaitDialog->ExecuteLD(R_WAIT_DIALOG);
	}

// ----------------------------------------------------------------------------
// CBtClient::SetWaitDialogLabel()
// Hide connection notification.
// ----------------------------------------------------------------------------
//
void CBtClient::SetWaitDialogLabel(const TDesC& aLabel)
	{
	if(iWaitDialog)
		{
		iWaitDialog->SetTextL(aLabel);
		}
	}

// ----------------------------------------------------------------------------
// CBtClient::HideWaitDialog()
// Hide connection notification.
// ----------------------------------------------------------------------------
//
void CBtClient::HideWaitDialog()
	{
	if(iWaitDialog)
		{
		iWaitDialog->ProcessFinishedL();
		iWaitDialog = NULL;
		}
	}

// ----------------------------------------------------------------------------
// CBtClient::DialogDismissedL()
// Callback method Gets called when a wait dialog is dismissed.
// ----------------------------------------------------------------------------
//
void CBtClient::DialogDismissedL(TInt aButtonId)
	{
	if (EAknSoftkeyCancel == aButtonId)
		{
		iSocket.Close();
		Cancel();
		SetState(EWaitingToGetDevice);
		}
	}

// End of File

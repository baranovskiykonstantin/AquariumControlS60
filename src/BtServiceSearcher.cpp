/*
 ============================================================================
 Name        : BtServiceSearcher.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Searching for a service on a remote machine.
 ============================================================================
 */

#include <bt_sock.h>
#include <f32file.h>
#include <s32file.h>
#include <s32strm.h>
#include <StringLoader.h>
#include "BtClientConstants.h"
#include "BtServiceSearcher.h"
#include "BtServiceSearcher.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBtServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtServiceSearcher* CBtServiceSearcher::NewL()
	{
	CBtServiceSearcher* self = CBtServiceSearcher::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtServiceSearcher* CBtServiceSearcher::NewLC()
	{
	CBtServiceSearcher* self = new (ELeave) CBtServiceSearcher();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::CBtServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CBtServiceSearcher::CBtServiceSearcher()
	: iIsDeviceSelectorConnected(EFalse),
	iServiceClass(KServiceClass),
	iPort(-1)
	{
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::~CBtServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CBtServiceSearcher::~CBtServiceSearcher()
	{
	iProtocolArray.Close();

	if (iIsDeviceSelectorConnected)
		{
		iDeviceSelector.CancelNotifier(KDeviceSelectionNotifierUid);
		iDeviceSelector.Close();
		}
	delete iSdpSearchPattern;
	iSdpSearchPattern = NULL;

	delete iAgent;
	iAgent = NULL;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::ConstructL()
	{
	// no implementation required
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::SelectDeviceByDiscoveryL()
// Select a device.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::SelectDeviceByDiscoveryL(
	TRequestStatus& aObserverRequestStatus)
	{
	if (iIsDeviceSelectorConnected)
		{
		iDeviceSelector.CancelNotifier(KDeviceSelectionNotifierUid);
		}
	else
		{
		User::LeaveIfError(iDeviceSelector.Connect());
		iIsDeviceSelectorConnected = ETrue;
		}

	iSelectionFilter().SetUUID(ServiceClass());

	iDeviceSelector.StartNotifierAndGetResponse(
		aObserverRequestStatus,
		KDeviceSelectionNotifierUid,
		iSelectionFilter,
		iResponse);
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::FindServiceL()
// Find a service on the specified device.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::FindServiceL(TRequestStatus& aObserverRequestStatus)
	{
	if (!iResponse().IsValidBDAddr())
		{
		User::Leave(KErrNotFound);
		}
	iHasFoundService = EFalse;

	// delete any existing agent and search pattern
	delete iSdpSearchPattern;
	iSdpSearchPattern = NULL;

	delete iAgent;
	iAgent = NULL;

	iAgent = CSdpAgent::NewL(*this, BTDevAddr());

	iSdpSearchPattern = CSdpSearchPattern::NewL();

	iSdpSearchPattern->AddL(ServiceClass());
	// return code is the position in the list that the UUID is inserted at
	// and is intentionally ignored

	iAgent->SetRecordFilterL(*iSdpSearchPattern);

	iStatusObserver = &aObserverRequestStatus;

	iAgent->NextRecordRequestL();
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::NextRecordRequestComplete()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::NextRecordRequestComplete(
	TInt aError,
	TSdpServRecordHandle aHandle,
	TInt aTotalRecordsCount)
	{
	// Canceled by BtClient. Quit.
	if (*iStatusObserver != KRequestPending)
		return;

	TRAPD(error,
		NextRecordRequestCompleteL(aError, aHandle, aTotalRecordsCount));

	if (error != KErrNone)
		{
		Panic(EBtServiceSearcherNextRecordRequestComplete);
		}
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::NextRecordRequestCompleteL()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::NextRecordRequestCompleteL(
	TInt aError,
	TSdpServRecordHandle aHandle,
	TInt aTotalRecordsCount)
	{
	if (aError == KErrEof)
		{
		Finished();
		return;
		}

	if (aError != KErrNone)
		{
		HBufC* errNRRC = StringLoader::LoadLC(R_ERR_NRRC_ERROR, aError);
		NotifyL(*errNRRC);
		CleanupStack::PopAndDestroy(); // errNRRC
		Finished(aError);
		return;
		}

	if (aTotalRecordsCount == 0)
		{
		NotifyL(R_ERR_NRRC_NO_RECORDS);
		Finished(KErrNotFound);
		return;
		}

	//  Request its attributes
	iAgent->AttributeRequestL(aHandle, KSdpAttrIdProtocolDescriptorList);
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::AttributeRequestResult()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::AttributeRequestResult(
	TSdpServRecordHandle aHandle,
	TSdpAttributeID aAttrID,
	CSdpAttrValue* aAttrValue)
	{
	// Canceled by BtClient. Quit.
	if (*iStatusObserver != KRequestPending)
		return;

	TRAPD(error, AttributeRequestResultL(aHandle, aAttrID, aAttrValue));
	if (error != KErrNone)
		{
		Panic(EBtServiceSearcherAttributeRequestResult);
		}
	// Delete obsolete local atribute pointer.
	delete aAttrValue;
	aAttrValue = NULL;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::AttributeRequestResultL()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::AttributeRequestResultL(
	TSdpServRecordHandle /*aHandle*/,
	TSdpAttributeID aAttrID,
	CSdpAttrValue* aAttrValue)
	{
	__ASSERT_ALWAYS(aAttrID == KSdpAttrIdProtocolDescriptorList,
					User::Leave(KErrNotFound));
	TSdpAttributeParser parser(ProtocolList(), *this);

	// Validate the attribute value, and extract the RFCOMM channel
	aAttrValue->AcceptVisitorL(parser);

	if (parser.HasFinished())
		{
		// Found a suitable record so change state
		iHasFoundService = ETrue;
		}
	}

// -----------------------------------------------------------------------------
// CBtServiceSearcher::AttributeRequestComplete()
// Process the attribute request completion.
// -----------------------------------------------------------------------------
//
void CBtServiceSearcher::AttributeRequestComplete(TSdpServRecordHandle aHandle,
	TInt aError)
	{
	// Canceled by BtClient. Quit.
	if (*iStatusObserver != KRequestPending)
		return;

	TRAPD(error, AttributeRequestCompleteL(aHandle, aError));
	if (error != KErrNone)
		{
		Panic(EBtServiceSearcherAttributeRequestComplete);
		}
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::AttributeRequestCompleteL()
// Process the attribute request completion.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::AttributeRequestCompleteL(
	TSdpServRecordHandle /*aHandle*/,
	TInt aError)
	{
	if (aError != KErrNone)
		{
		HBufC* errCantGetAttribute = StringLoader::LoadLC(R_ERR_CANT_GET_ATTRIBUTE, aError);
		NotifyL(*errCantGetAttribute);
		CleanupStack::PopAndDestroy(); // errCantGetAttribute
		}
	else if (!HasFinishedSearching())
		{
		// have not found a suitable record so request another
		iAgent->NextRecordRequestL();
		}
	else
		{
		NotifyL(R_ERR_ATTR_REQ_COM);
		Finished();
		}
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::Finished()
// The search has finished and notify the observer
// that the process is complete.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::Finished(TInt aError /* default = KErrNone */)
	{
	if (aError == KErrNone && !HasFoundService())
		{
		aError = KErrNotFound;
		}
	// if != KRequestPending -> canceled by BtClient
	if (*iStatusObserver == KRequestPending)
		User::RequestComplete(iStatusObserver, aError);
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::HasFinishedSearching()
// Is the instance still wanting to search.
// ----------------------------------------------------------------------------
//
TBool CBtServiceSearcher::HasFinishedSearching() const
	{
	return (*iStatusObserver != KRequestPending);
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::BTDevAddr()
// Returns the bluetooth device address.
// ----------------------------------------------------------------------------
//
const TBTDevAddr& CBtServiceSearcher::BTDevAddr()
	{
	return iResponse().BDAddr();
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::ResponseParams()
// Returns information about the device selected by the user.
// ----------------------------------------------------------------------------
//
const TBTDeviceResponseParams& CBtServiceSearcher::ResponseParams()
	{
	return iResponse();
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::HasFoundService()
// True if a service has been found.
// ----------------------------------------------------------------------------
//
TBool CBtServiceSearcher::HasFoundService() const
	{
	return iHasFoundService;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CBtServiceSearcher::ServiceClass() const
	{
	return iServiceClass;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::ProtocolList()
// The list of Protocols required by the service.
// ----------------------------------------------------------------------------
//
 RArray <TSdpAttributeParser::TSdpAttributeNode>& CBtServiceSearcher
::ProtocolList()
	{
	TSdpAttributeParser::TSdpAttributeNode attrib;
	attrib.SetCommand(TSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::ECheckValue);
	attrib.SetType(ETypeUUID);
	attrib.SetValue(KL2CAP);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::ECheckValue);
	attrib.SetType(ETypeUUID);
	attrib.SetValue(KRFCOMM);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::EReadValue);
	attrib.SetType(ETypeUint);
	attrib.SetValue(KRfcommChannel);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TSdpAttributeParser::EFinished);
	iProtocolArray.Append(attrib);

	return iProtocolArray;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::FoundElementL(TInt aKey, CSdpAttrValue& aValue)
	{
	__ASSERT_ALWAYS(aKey == static_cast<TInt>(KRfcommChannel),
		Panic(EBtServiceSearcherProtocolRead));
	iPort = aValue.Uint();
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CBtServiceSearcher::Port()
	{
	return iPort;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::SetObserver()
// Connect an observer for notifications.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::SetObserver(MBtClientObserver* aObserver)
	{
	iObserver = aObserver;
	}

// ----------------------------------------------------------------------------
// CBtServiceSearcher::NotifyL()
// Send to observer a log message.
// ----------------------------------------------------------------------------
//
void CBtServiceSearcher::NotifyL(const TDesC& aMessage)
	{
	if (iObserver)
		{
		iObserver->HandleBtNotifyL(aMessage, ETrue);
		}
	}
void CBtServiceSearcher::NotifyL(TInt aErrorResourceId)
	{
	HBufC* textResource = StringLoader::LoadLC(aErrorResourceId);
	NotifyL(*textResource);
	CleanupStack::PopAndDestroy(textResource);
	}

// -----------------------------------------------------------------------------
// CBtServiceSearcher::LoadBtDeviceInfoL()
// Restore BT device info from the previous session.
// -----------------------------------------------------------------------------
//
TBool CBtServiceSearcher::LoadBtDeviceInfoL()
	{
	TBTDevAddr bdAddr;
	TBuf8<6> bdAddrStr;
	TBuf<KMaxBluetoothNameLen> deviceName;
	TBTDeviceClass deviceClass;

	TFileName fileName;
	RFs fs;
	RFileReadStream readStream;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Prepare private path
	User::LeaveIfError(fs.CreatePrivatePath(EDriveC));
	User::LeaveIfError(fs.PrivatePath(fileName));
	fileName += KBtDeviceFile;
	// Read from stream
	TInt fsError = readStream.Open(fs, fileName, EFileRead);
	TInt loadingError = KErrNone;
	// If file existed, try to read it.
	if (KErrNone == fsError)
		{
		TRAP(loadingError, readStream >> bdAddrStr);
		if (KErrNone == loadingError)
			TRAP(loadingError, readStream >> deviceName);
		if (KErrNone == loadingError)
			TRAP(loadingError, readStream >> deviceClass);
		if (KErrNone == loadingError)
			{
			bdAddr = TBTDevAddr(bdAddrStr);
			iResponse().SetDeviceAddress(bdAddr);
			iResponse().SetDeviceName(deviceName);
			iResponse().SetDeviceClass(deviceClass);
			}
		}
	else
		{
		loadingError = fsError;
		}
	readStream.Release();
	// Use file only once.
	fs.Delete(fileName);
	CleanupStack::PopAndDestroy(); // fs
	return (KErrNone == loadingError);
	}

// -----------------------------------------------------------------------------
// CBtServiceSearcher::SaveBtDeviceInfoL()
// Save BT device info to file for the next session.
// -----------------------------------------------------------------------------
//
void CBtServiceSearcher::SaveBtDeviceInfoL()
	{
	TFileName fileName;
	RFs fs;
	RFileWriteStream writeStream;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Prepare private path
	User::LeaveIfError(fs.CreatePrivatePath(EDriveC));
	User::LeaveIfError(fs.PrivatePath(fileName));
	fileName += KBtDeviceFile;
	TInt error = writeStream.Open(fs, fileName, EFileWrite);
	// Setting file did not exist, create one.
	if (error != KErrNone)
		{
		User::LeaveIfError(writeStream.Create(fs, fileName, EFileWrite));
		}
	writeStream.PushL();
	writeStream << iResponse().BDAddr().Des();
	writeStream << iResponse().DeviceName();
	writeStream << iResponse().DeviceClass();
	writeStream.CommitL();
	writeStream.Pop();
	writeStream.Release();
	CleanupStack::PopAndDestroy(); // fs
	}

// End of File

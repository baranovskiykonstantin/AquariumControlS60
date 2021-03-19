/*
 ============================================================================
 Name        : BatteryStatus.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Battery status notifier.
 ============================================================================
 */

// INCLUDE FILES
#include "BatteryStatus.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBatteryStatus::CBatteryStatus()
// Constructor.
// ----------------------------------------------------------------------------
//
CBatteryStatus::CBatteryStatus(MBatteryStatusObserver* aObserver) :
	iBatteryLevelValue(EBatteryLevelUnknown),
	iChargingStatusValue(EChargingStatusNotConnected),
	iObserver(aObserver)
	{
	}

// ----------------------------------------------------------------------------
// CBatteryStatus::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBatteryStatus* CBatteryStatus::NewL(MBatteryStatusObserver* aObserver)
	{
	CBatteryStatus* self = new(ELeave) CBatteryStatus(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CBatteryStatus::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CBatteryStatus::ConstructL()
	{
	iBatteryLevel = CBatteryLevel::NewL(this);
	iChargingStatus = CChargingStatus::NewL(this);
	}

// ----------------------------------------------------------------------------
// CBatteryStatus::~CBatteryStatus()
// Destructor.
// ----------------------------------------------------------------------------
//
CBatteryStatus::~CBatteryStatus()
	{
	delete iChargingStatus;
	iChargingStatus = NULL;
	
	delete iBatteryLevel;
	iBatteryLevel = NULL;
	}

// ----------------------------------------------------------------------------
// CBatteryStatus::IsOK()
// Get status of the battery.
// ----------------------------------------------------------------------------
//
TBool CBatteryStatus::IsOK()
	{
	if (iBatteryLevelValue > EBatteryLevelLevel2 || iChargingStatusValue >= EChargingStatusCharging)
		{
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CBatteryStatus::HandleBatteryLevelChangeL()
// Battery level change notify.
// ----------------------------------------------------------------------------
//
void CBatteryStatus::HandleBatteryLevelChangeL(EPSHWRMBatteryLevel aBatteryLevel)
	{
	TBool prevStatus = IsOK();
	iBatteryLevelValue = aBatteryLevel;
	TBool newStatus = IsOK();
	if (iObserver && prevStatus != newStatus)
		{
		iObserver->HandleBatteryStatusChangeL();
		}
	}

// ----------------------------------------------------------------------------
// CBatteryStatus::HandleChargingStatusChangeL()
// Charging status change notify.
// ----------------------------------------------------------------------------
//
void CBatteryStatus::HandleChargingStatusChangeL(EPSHWRMChargingStatus aChargingStatus)
	{
	TBool prevStatus = IsOK();
	iChargingStatusValue = aChargingStatus;
	TBool newStatus = IsOK();
	if (iObserver && prevStatus != newStatus)
		{
		iObserver->HandleBatteryStatusChangeL();
		}
	}

// End of File

/*
 ============================================================================
 Name        : BatteryStatus.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Battery status notifier.
 ============================================================================
 */

#ifndef __BATTERYSTATUS_H__
#define __BATTERYSTATUS_H__

#include <BatteryLevel.h>
#include <ChargingStatus.h>

/**
 * MBatteryStatusObserver mixin.
 * Handles the battery status change.
 */
class MBatteryStatusObserver
	{

public:

	/**
	 * HandleBatteryStatusChangeL()
	 * Battery status change notify.
	 * 
	 * Derived classes provide the implementation.
	 */
	virtual void HandleBatteryStatusChangeL() = 0;

	};

/**
* CBatteryStatus
* Checks the battery status and sends the notification of battery status change
* to MBatteryStatusObserver.
*/
class CBatteryStatus :
	public CBase,
	private MBatteryLevelObserver,
	private MChargingStatusObserver
	{

public: // Constructors and destructor

	static CBatteryStatus* NewL(MBatteryStatusObserver* aObserver);
	virtual ~CBatteryStatus();

private: // Constructors

	CBatteryStatus(MBatteryStatusObserver* aObserver);
	void ConstructL();

private: // From MBatteryLevelObserver

	void HandleBatteryLevelChangeL(EPSHWRMBatteryLevel aBatteryLevel);

private: // From MChargingStatusObserver

	void HandleChargingStatusChangeL(EPSHWRMChargingStatus aChargingStatus);

public: // New public functions

	/**
	 * IsOK()
	 * Get status of the battery.
	 */
	TBool IsOK();

private: // Data

	/**
	 * iBatteryLevel
	 */
	CBatteryLevel* iBatteryLevel;
	EPSHWRMBatteryLevel iBatteryLevelValue;

	/**
	 * iChargingStatus
	 */
	CChargingStatus* iChargingStatus;
	EPSHWRMChargingStatus iChargingStatusValue;

	/**
	 * iObserver
	 * Handles the battery status change.
	 * Non-owning pointer.
	 */
	MBatteryStatusObserver* iObserver;

	};

#endif /* __BATTERYSTATUS_H__ */

// End of File

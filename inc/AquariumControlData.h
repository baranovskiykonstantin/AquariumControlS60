/*
 ============================================================================
 Name        : AquariumControlData.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : CAquariumControlData declaration
 ============================================================================
 */

#ifndef __AQUARIUMCONTROLDATA_H__
#define __AQUARIUMCONTROLDATA_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 * TAquariumHeatState
 */
enum TAquariumDeviceState {
	EOff,
	EOn
};

/**
 * TAquariumHeatState
 */
enum TAquariumDeviceMode {
	EAuto,
	EManual
};

/**
 * TAquariumHeatState
 */
enum TAquariumDispalyMode {
	ETime,
	ETemperature
};

/**
 *  CAquariumControlData
 * 
 */
class CAquariumControlData : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CAquariumControlData();

	/**
	 * Two-phased constructor.
	 */
	static CAquariumControlData* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CAquariumControlData* NewLC();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CAquariumControlData();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

public:
	// Connection status.
	// ETure if the connection is established
	// and a complete aquarium status has received.
	// Resets to EFalse if the connection is lost.
	TBool iIsConnected;

	// Date
	TUint iDay;
	TUint iMonth;
	TUint iYear;
	TUint iDayOfWeek;

	// Time
	TUint iHours;
	TUint iMinutes;
	TUint iSeconds;
	TInt  iTimeCorrection;

	// Light
	TAquariumDeviceState	iLightState;
	TAquariumDeviceMode		iLightMode;
	TUint					iLightLevel;
	TUint					iLightCurrentLevel;
	TUint					iLightRise;
	// Light ON time
	TUint iLightOnHours;
	TUint iLightOnMinutes;
	TUint iLightOnSeconds;
	// Light OFF time
	TUint iLightOffHours;
	TUint iLightOffMinutes;
	TUint iLightOffSeconds;

	// Heat
	TUint					iTemp;
	TAquariumDeviceState	iHeatState;
	TAquariumDeviceMode		iHeatMode;
	TUint					iHeatLow;
	TUint					iHeatHigh;

	// Display
	TAquariumDispalyMode	iDisplayMode;
	};

#endif // __AQUARIUMCONTROLDATA_H__

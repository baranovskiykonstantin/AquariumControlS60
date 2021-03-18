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

// CONSTANTS
const TInt KUnknownTemp(127);

// CLASS DECLARATION

/**
 * TAquariumConnectionStatus
 */
enum TAquariumConnectionStatus {
	EStatusDisconnected,
	EStatusConnecting,
	EStatusConnected,
	EStatusPaused
};

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

public:
	// New functions

	/**
	 * Parse a line of the status message from aquarium controller
	 * and extract data from it.
	 * Status message example:
	 *     Date: 01.01.17 Friday
	 *     Time: 13:29:59 (-3 sec at 12:00:00)
	 *     Temp: 22
	 *     Heat: OFF auto (20-22)
	 *     Light: ON manual (10:00:00-20:00:00) 43/50% 10min
	 *     Display: time
	 * @param aLine A text line from status message.
	 */
	void ParseLineL(const TDesC& aLine);

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
	// Connection status
	TAquariumConnectionStatus iConnectionStatus;

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
	TInt					iTemp;
	TAquariumDeviceState	iHeatState;
	TAquariumDeviceMode		iHeatMode;
	TUint					iHeatLow;
	TUint					iHeatHigh;

	// Display
	TAquariumDispalyMode	iDisplayMode;
	};

#endif // __AQUARIUMCONTROLDATA_H__

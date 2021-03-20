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
private: // Flags to track the status integrity.
	enum TStatusLine {
		EStatusLineNULL,
		EStatusLineDate,
		EStatusLineTime,
		EStatusLineTemp,
		EStatusLineHeat,
		EStatusLineLight,
		EStatusLineDisplay
	};

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
	 * @param aLine A text line from the status message.
	 */
	void ParseLineL(const TDesC& aLine);

	/**
	 * Check if the status has been processed fully.
	 * @return ETrue if all lines of the status were parsed.
	 */
	TBool IsStatusOk();

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
	TUint iHour;
	TUint iMinute;
	TUint iSecond;
	TInt  iTimeCorrection;

	// Light
	TAquariumDeviceState	iLightState;
	TAquariumDeviceMode		iLightMode;
	TUint					iLightLevel;
	TUint					iLightCurrentLevel;
	TUint					iLightRise;
	// Light ON time
	TUint iLightOnHour;
	TUint iLightOnMinute;
	TUint iLightOnSecond;
	// Light OFF time
	TUint iLightOffHour;
	TUint iLightOffMinute;
	TUint iLightOffSecond;

	// Heat
	TInt					iTemp;
	TAquariumDeviceState	iHeatState;
	TAquariumDeviceMode		iHeatMode;
	TUint					iHeatLow;
	TUint					iHeatHigh;

	// Display
	TAquariumDispalyMode	iDisplayMode;

	// Flag to track the status integrity.
	TStatusLine iProcessedLine;
	};

#endif // __AQUARIUMCONTROLDATA_H__

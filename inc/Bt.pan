/*
 ============================================================================
 Name        : Bt.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains bluetooth panic codes.
 ============================================================================
 */

#ifndef __BT_PAN__
#define __BT_PAN__

/** RFterm application panic codes */
enum TBtPanics
	{
	ERFtermInvalidLogicState = 1,
	ERFtermUnableToDisconnect
	// add further panics here
	};

/** Panic Category */
_LIT(KPanicBt, "Bluetooth");

inline void Panic(TBtPanics aReason)
	{
	User::Panic(KPanicBt, aReason);
	}

#endif // __BT_PAN__

// End of File

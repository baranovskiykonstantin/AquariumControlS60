/*
 ============================================================================
 Name        : BtClient.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains bluetooth panic codes.
 ============================================================================
 */

#ifndef __BTCLIENT_PAN__
#define __BTCLIENT_PAN__

/** BtClient panic codes */
enum TBtPanics
	{
	EBtClientInvalidLogicState = 1,
	EBtClientUnableToDisconnect
	// add further panics here
	};

/** Panic Category */
_LIT(KPanicBt, "Bluetooth");

inline void Panic(TBtPanics aReason)
	{
	User::Panic(KPanicBt, aReason);
	}

#endif // __BTCLIENT_PAN__

// End of File

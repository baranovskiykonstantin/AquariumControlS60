/*
 ============================================================================
 Name        : AquariumControl.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __AQUARIUMCONTROL_PAN__
#define __AQUARIUMCONTROL_PAN__

/** AquariumControl application panic codes */
enum TAquariumControlPanics
	{
	EAquariumControlViewAppUi = 1,
	EAquariumControlViewTabId
	
	// add further panics here
	};

inline void Panic(TAquariumControlPanics aReason)
	{
	_LIT(applicationName, "AquariumControl");
	User::Panic(applicationName, aReason);
	}

#endif // __AQUARIUMCONTROL_PAN__

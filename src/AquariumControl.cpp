/*
 ============================================================================
 Name        : AquariumControl.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "AquariumControlApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CAquariumControlApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}


/*
 ============================================================================
 Name        : AquariumControlApplication.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "AquariumControl.hrh"
#include "AquariumControlDocument.h"
#include "AquariumControlApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAquariumControlApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CAquariumControlApplication::CreateDocumentL()
	{
	// Create an AquariumControl document, and return a pointer to it
	return CAquariumControlDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CAquariumControlApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CAquariumControlApplication::AppDllUid() const
	{
	// Return the UID for the AquariumControl application
	return KUidAquariumControlApp;
	}

// End of File

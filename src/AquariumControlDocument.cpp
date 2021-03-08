/*
 ============================================================================
 Name        : AquariumControlDocument.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : CAquariumControlDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "AquariumControlAppUi.h"
#include "AquariumControlDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAquariumControlDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAquariumControlDocument* CAquariumControlDocument::NewL(CEikApplication& aApp)
	{
	CAquariumControlDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CAquariumControlDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAquariumControlDocument* CAquariumControlDocument::NewLC(CEikApplication& aApp)
	{
	CAquariumControlDocument* self =
			new (ELeave) CAquariumControlDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CAquariumControlDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAquariumControlDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CAquariumControlDocument::CAquariumControlDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAquariumControlDocument::CAquariumControlDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CAquariumControlDocument::~CAquariumControlDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CAquariumControlDocument::~CAquariumControlDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CAquariumControlDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CAquariumControlDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CAquariumControlAppUi;
	}

// End of File

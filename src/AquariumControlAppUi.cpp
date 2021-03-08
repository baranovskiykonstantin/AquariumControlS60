/*
 ============================================================================
 Name        : AquariumControlAppUi.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : CAquariumControlAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>

#include <aquariumcontrol_0xa291b5a5.rsg>

#include "AquariumControl.hrh"
#include "AquariumControl.pan"
#include "AquariumControlApplication.h"
#include "AquariumControlAppUi.h"
#include "AquariumControlAppView.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAquariumControlAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAquariumControlAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// Create view object
	iAppView = CAquariumControlAppView::NewL(ClientRect());
	}
// -----------------------------------------------------------------------------
// CAquariumControlAppUi::CAquariumControlAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAquariumControlAppUi::CAquariumControlAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppUi::~CAquariumControlAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CAquariumControlAppUi::~CAquariumControlAppUi()
	{
	if (iAppView)
		{
		delete iAppView;
		iAppView = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CAquariumControlAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case EAbout:
			{
			CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
			dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
			HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
			dlg->QueryHeading()->SetTextL(*title);
			CleanupStack::PopAndDestroy(); //title
			HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
			dlg->SetMessageTextL(*msg);
			CleanupStack::PopAndDestroy(); //msg
			dlg->RunLD();
			}
			break;

		default:
			Panic(EAquariumControlUi);
			break;
		}
	}

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CAquariumControlAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect(ClientRect());
	}

// End of File

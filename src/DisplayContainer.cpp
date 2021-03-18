/*
 ============================================================================
 Name        : DisplayContainer.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application display view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <aknviewappui.h>
#include <stringloader.h>
#include <aquariumcontrol_0xa291b5a5.rsg>
#include "AquariumControl.hrh"
#include "AquariumControlViewAppUi.h"
#include "AquariumControlData.h"
#include "DisplayContainer.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDisplayContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDisplayContainer* CDisplayContainer::NewL(const TRect& aRect)
	{
	CDisplayContainer* self = new (ELeave) CDisplayContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CDisplayContainer::CDisplayContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDisplayContainer::CDisplayContainer()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDisplayContainer::UpdateListBoxL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CDisplayContainer::UpdateListBoxL()
	{
	if (!iListBox || !iListBoxItems)
		return;

	// Get access to aquarium data
	const CAquariumControlData* data = ((CAquariumControlViewAppUi*) iAvkonViewAppUi)->AquariumData();

	if (data->iConnectionStatus == EStatusPaused)
		return;

	// Remove all items
	iListBoxItems->Reset();

	if (data->iConnectionStatus == EStatusConnected)
		{
		// Create items again
		TBuf<64> itemText;
		HBufC* itemTitle;
		HBufC* itemValue;

		// Show
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_DISPLAY);
		switch (data->iDisplayMode)
			{
			case (TAquariumDispalyMode) ETime:
				itemValue = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_DISPLAY_TIME);
				break;
			case (TAquariumDispalyMode) ETemperature:
				itemValue = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_DISPLAY_TEMP);
				break;
			default:
				itemValue = KNullDesC().AllocLC();
				break;
			}
		itemText.Format(KListBoxItemFormat, itemTitle, itemValue);
		iListBoxItems->AppendL(itemText);
		CleanupStack::PopAndDestroy(itemValue);
		CleanupStack::PopAndDestroy(itemTitle);

		}

	iListBox->HandleItemAdditionL();
	}

// -----------------------------------------------------------------------------
// CDisplayContainer::HandleListBoxItemActivationL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CDisplayContainer::HandleListBoxItemActivationL(TInt aItem)
	{
	// The order of the listbox items and the order of
	// command ids (TAquariumControlIds) must be the same.
	iAvkonViewAppUi->HandleCommandL(EAquariumControlSwitchDisplayMode + aItem);
	}

// End of File

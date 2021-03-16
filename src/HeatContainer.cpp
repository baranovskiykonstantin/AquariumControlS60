/*
 ============================================================================
 Name        : HeatContainer.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application heat view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <aknviewappui.h>
#include <akntoolbar.h>
#include <stringloader.h>
#include <aquariumcontrol_0xa291b5a5.rsg>
#include "AquariumControl.hrh"
#include "AquariumControlViewAppUi.h"
#include "AquariumControlData.h"
#include "HeatContainer.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHeatContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHeatContainer* CHeatContainer::NewL(const TRect& aRect)
	{
	CHeatContainer* self = new (ELeave) CHeatContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CHeatContainer::CHeatContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHeatContainer::CHeatContainer()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CHeatContainer::UpdateListBoxL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CHeatContainer::UpdateListBoxL()
	{
	if (!iListBox || !iListBoxItems)
		return;

	// Remove all items
	iListBoxItems->Reset();

	// Get access to aquarium data
	const CAquariumControlData* data = ((CAquariumControlViewAppUi*) iAvkonViewAppUi)->AquariumData();

	if (data->iIsConnected)
		{
		// Create items again
		TBuf<64> itemText;
		HBufC* itemTitle;
		TBuf<32> itemValue;

		// Formats
		_LIT(KTempFormat, "%i \u00b0C");

		// Water temp
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_TEMP);
		if (data->iTemp == KUnknownTemp)
			{
			HBufC* unknown = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_TEMP_UNKNOWN);
			itemText.Format(KListBoxItemFormat, itemTitle, unknown);
			CleanupStack::PopAndDestroy(unknown);
			}
		else
			{
			itemValue.Format(KTempFormat, data->iTemp);
			itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
			}
		iListBoxItems->AppendL(itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// State
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_STATE);
		HBufC* state;
		switch (data->iHeatState)
			{
			case (TAquariumDeviceState) EOn:
				state = iEikonEnv->AllocReadResourceLC(R_STATE_ON);
				break;
			case (TAquariumDeviceState) EOff:
				state = iEikonEnv->AllocReadResourceLC(R_STATE_OFF);
				break;
			default:
				state = KNullDesC().AllocLC();
				break;
			}
		itemText.Format(KListBoxItemFormat, itemTitle, state);
		iListBoxItems->AppendL(itemText);
		CleanupStack::PopAndDestroy(state);
		CleanupStack::PopAndDestroy(itemTitle);

		// Mode
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_MODE);
		HBufC* mode;
		switch (data->iHeatMode)
			{
			case (TAquariumDeviceMode) EAuto:
				mode = iEikonEnv->AllocReadResourceLC(R_MODE_AUTO);
				break;
			case (TAquariumDeviceMode) EManual:
				mode = iEikonEnv->AllocReadResourceLC(R_MODE_MANUAL);
				break;
			default:
				mode = KNullDesC().AllocLC();
				break;
			}
		itemText.Format(KListBoxItemFormat, itemTitle, mode);
		iListBoxItems->AppendL(itemText);
		CleanupStack::PopAndDestroy(mode);
		CleanupStack::PopAndDestroy(itemTitle);

		// Minimum temperature
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_LO);
		itemValue.Format(KTempFormat, data->iHeatLow);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		iListBoxItems->AppendL(itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Maximum temperature
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_HI);
		itemValue.Format(KTempFormat, data->iHeatHigh);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		iListBoxItems->AppendL(itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		}

	iListBox->HandleItemAdditionL();
	}

// -----------------------------------------------------------------------------
// CLightContainer::HandleListBoxItemActivationL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CHeatContainer::HandleListBoxItemActivationL(TInt aItem)
	{
	// The order of the listbox items and the order of
	// command ids (TAquariumControlIds) must be the same.
	iAvkonViewAppUi->HandleCommandL(EAquariumControlWaterTemp + aItem);
	}

// End of File

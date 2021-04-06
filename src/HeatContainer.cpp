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

	// Get access to aquarium data
	const CAquariumControlData* data = ((CAquariumControlViewAppUi*) iAvkonViewAppUi)->AquariumData();

	if (data->iConnectionStatus == EStatusPaused)
		return;

	if (data->iConnectionStatus == EStatusConnected)
		{
		// Create/update items
		TBool isItemAddition = (iListBoxItems->Count() == 0);
		TBuf<64> itemText;
		HBufC* itemTitle;
		TBuf<32> itemValue;

		// Formats
		_LIT(KTempFormat, "%i \u00b0C");

		// Water temp
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_TEMP);
		if (data->iTemp == KUnknownTemp)
			{
			iEikonEnv->ReadResourceL(itemValue, R_LISTBOX_ITEM_HEAT_TEMP_UNKNOWN);
			}
		else
			{
			itemValue.Format(KTempFormat, data->iTemp);
			}
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(0, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// State
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_STATE);
		switch (data->iHeatState)
			{
			case (TAquariumDeviceState) EOn:
				iEikonEnv->ReadResourceL(itemValue, R_STATE_ON);
				break;
			case (TAquariumDeviceState) EOff:
				iEikonEnv->ReadResourceL(itemValue, R_STATE_OFF);
				break;
			default:
				itemValue.Zero();
				break;
			}
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(1, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Mode
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_MODE);
		switch (data->iHeatMode)
			{
			case (TAquariumDeviceMode) EAuto:
				iEikonEnv->ReadResourceL(itemValue, R_MODE_AUTO);
				break;
			case (TAquariumDeviceMode) EManual:
				iEikonEnv->ReadResourceL(itemValue, R_MODE_MANUAL);
				break;
			default:
				itemValue.Zero();
				break;
			}
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(2, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Minimum temperature
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_LO);
		itemValue.Format(KTempFormat, data->iHeatLow);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(3, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Maximum temperature
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_HI);
		itemValue.Format(KTempFormat, data->iHeatHigh);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(4, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		if (isItemAddition)
			iListBox->HandleItemAdditionL();
		else
			iListBox->DrawDeferred();
		}
	else
		{
		// Remove all items
		if (iListBoxItems->Count() > 0)
			{
			iListBoxItems->Reset();
			iListBox->HandleItemRemovalL();
			iListBox->DrawNow();
			}
		}
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

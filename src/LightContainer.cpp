/*
 ============================================================================
 Name        : LightContainer.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application light view implementation
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
#include "LightContainer.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLightContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLightContainer* CLightContainer::NewL(const TRect& aRect)
	{
	CLightContainer* self = new (ELeave) CLightContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CLightContainer::CLightContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CLightContainer::CLightContainer()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CLightContainer::UpdateListBoxL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CLightContainer::UpdateListBoxL()
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
		_LIT(KStateFormat, "%S (%u%%)");
		_LIT(KTimeFormat, "%u:%02u:%02u");
		_LIT(KLevelFormat, "%u %%");
		_LIT(KRiseFormat, "%u %S");

		// State
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_STATE);
		HBufC* state;
		switch (data->iLightState)
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
		if (data->iLightState == EOn && data->iLightCurrentLevel != data->iLightLevel)
			{
			itemValue.Format(KStateFormat, state, data->iLightCurrentLevel);
			itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
			}
		else if (data->iLightState == EOff && data->iLightCurrentLevel != 0)
			{
			itemValue.Format(KStateFormat, state, data->iLightCurrentLevel);
			itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
			}
		else
			{
			itemText.Format(KListBoxItemFormat, itemTitle, state);
			}
		UpdateListBoxItemL(0, itemText);
		CleanupStack::PopAndDestroy(state);
		CleanupStack::PopAndDestroy(itemTitle);

		// Mode
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_MODE);
		switch (data->iLightMode)
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
		UpdateListBoxItemL(1, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Turn on time
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_ON_TIME);
		itemValue.Format(KTimeFormat, data->iLightOnHour, data->iLightOnMinute, data->iLightOnSecond);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(2, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Turn off time
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_OFF_TIME);
		itemValue.Format(KTimeFormat, data->iLightOffHour, data->iLightOffMinute, data->iLightOffSecond);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(3, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Brightness
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_LEVEL);
		itemValue.Format(KLevelFormat, data->iLightLevel);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(4, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Rise time
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_RISE);
		HBufC* riseUnits = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_RISE_UNITS);
		itemValue.Format(KRiseFormat, data->iLightRise, riseUnits);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(5, itemText);
		CleanupStack::PopAndDestroy(riseUnits);
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
void CLightContainer::HandleListBoxItemActivationL(TInt aItem)
	{
	// The order of the listbox items and the order of
	// command ids (TAquariumControlIds) must be the same.
	iAvkonViewAppUi->HandleCommandL(EAquariumControlSwitchLightState + aItem);
	}

// End of File

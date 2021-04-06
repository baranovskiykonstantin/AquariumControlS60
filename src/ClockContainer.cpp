/*
 ============================================================================
 Name        : ClockContainer.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application clock view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <aknviewappui.h>
#include <stringloader.h>
#include <aquariumcontrol_0xa291b5a5.rsg>
#include "AquariumControl.hrh"
#include "AquariumControlViewAppUi.h"
#include "AquariumControlData.h"
#include "ClockContainer.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CClockContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CClockContainer* CClockContainer::NewL(const TRect& aRect)
	{
	CClockContainer* self = new (ELeave) CClockContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CClockContainer::CClockContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CClockContainer::CClockContainer()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CClockContainer::UpdateListBoxL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CClockContainer::UpdateListBoxL()
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
		_LIT(KTimeFormat, "%u:%02u:%02u");
		_LIT(KTimeCorFormat, "%i %S");
		_LIT(KDateFormat, "%02u.%02u.20%02u");
		
		// Constants
		_LIT(KSignPlus, "+");

		// Time
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_TIME);
		itemValue.Format(KTimeFormat, data->iHour, data->iMinute, data->iSecond);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(0, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Time correction
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_TIMECOR);
		HBufC* timeCorUnits = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_TIMECORUNITS);
		itemValue.Format(KTimeCorFormat, data->iTimeCorrection, timeCorUnits);
		if (data->iTimeCorrection >= 0)
			itemValue.Insert(0, KSignPlus);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(1, itemText);
		CleanupStack::PopAndDestroy(timeCorUnits);
		CleanupStack::PopAndDestroy(itemTitle);

		// Date
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_DATE);
		itemValue.Format(KDateFormat, data->iDay, data->iMonth, data->iYear);
		itemText.Format(KListBoxItemFormat, itemTitle, &itemValue);
		UpdateListBoxItemL(2, itemText);
		CleanupStack::PopAndDestroy(itemTitle);

		// Day of week
		itemTitle = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_DAYOFWEEK);
		if (data->iDayOfWeek > 0 && data->iDayOfWeek < 8)
			{
			TDayName dayOfWeek = TDayName((TDay)(data->iDayOfWeek - 1));
			itemText.Format(KListBoxItemFormat, itemTitle, &dayOfWeek);
			}
		else
			{
			itemText.Format(KListBoxItemFormat, itemTitle, &KNullDesC);
			}
		UpdateListBoxItemL(3, itemText);
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
// CClockContainer::HandleListBoxItemActivationL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CClockContainer::HandleListBoxItemActivationL(TInt aItem)
	{
	// The order of the listbox items and the order of
	// command ids (TAquariumControlIds) must be the same.
	iAvkonViewAppUi->HandleCommandL(EAquariumControlSetTime + aItem);
	}

// End of File

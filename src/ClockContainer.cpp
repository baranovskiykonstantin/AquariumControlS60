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
#include "AquariumControl.hrh"
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
	if (iListBox && iListBoxItems)
		{
		// Remove all items
		iListBoxItems->Reset();
		// Create items again
		iListBoxItems->AppendL(_L("\tTime\t12:00\t\t"));
		iListBoxItems->AppendL(_L("\tTime correction\t+5 sec\t\t"));
		iListBoxItems->AppendL(_L("\tDate\t10.03.2021\t\t"));
		iListBoxItems->AppendL(_L("\tDay of week\tWednesday\t\t"));
		iListBox->HandleItemAdditionL();
		}
	}

// -----------------------------------------------------------------------------
// CClockContainer::HandleListBoxItemActivationL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CClockContainer::HandleListBoxItemActivationL(TInt aItem)
	{
	switch (aItem)
		{
		case 0:
			iAvkonViewAppUi->HandleCommandL(EAquariumControlSetTime);
		break;

		case 2:
			iAvkonViewAppUi->HandleCommandL(EAquariumControlSetDate);
		break;

		default:
			// Do nothing
		break;
		}
	}

// End of File

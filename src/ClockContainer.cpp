/*
 ============================================================================
 Name        : ClockContainer.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application clock view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <aknviewappui.h> 
#include <aknlists.h> 
#include "AquariumControl.hrh"
#include "ClockContainer.h"

// Constants
_LIT(KListBoxEmptyText, "???");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CClockContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CClockContainer* CClockContainer::NewL(const TRect& aRect)
	{
	CClockContainer* self = CClockContainer::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CClockContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CClockContainer* CClockContainer::NewLC(const TRect& aRect)
	{
	CClockContainer* self = new (ELeave) CClockContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CClockContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CClockContainer::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Create a listbox
	iListBox = new (ELeave) CAknDoubleStyleListBox();
	// Construct listbox
	iListBox->ConstructL(this, EAknListBoxSelectionList);
	iListBox->SetContainerWindowL(*this);
	iListBox->SetListBoxObserver(this);
/*
	// Set scrollbars
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto);
*/

	// Set the listbox empty text
	iListBox->View()->SetListEmptyTextL(KListBoxEmptyText);

	// Create listbox item array
	iListBoxItems = new (ELeave) CDesCArrayFlat(4);
	// Add items to listbox
	iListBox->Model()->SetItemTextArray(iListBoxItems);
	// Listbox deletes the item array
	iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	// Activate Listbox
	iListBox->SetRect(Rect());
	iListBox->ActivateL();
	UpdateListBoxL();

	// Activate the window, which makes it ready to be drawn
	ActivateL();
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
// CClockContainer::~CClockContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CClockContainer::~CClockContainer()
	{
	if (iListBox)
		{
		delete iListBox;
		}
	}

// -----------------------------------------------------------------------------
// CClockContainer::CountComponentControls()
// Gets the number of controls contained in a compound control.
// -----------------------------------------------------------------------------
//
TInt CClockContainer::CountComponentControls() const
	{
	return 1;
	}

// -----------------------------------------------------------------------------
// CClockContainer::ComponentControl()
// Gets an indexed component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CClockContainer::ComponentControl(TInt aIndex) const
	{
	switch(aIndex)
		{
		case 0:
			return iListBox;
		default:
			return NULL;
		}
	}

// -----------------------------------------------------------------------------
// CClockContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CClockContainer::SizeChanged()
	{
	if (iListBox)
		{
		iListBox->SetRect(Rect());
		}
	}

// -----------------------------------------------------------------------------
// CClockContainer::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CClockContainer::HandlePointerEventL(
		const TPointerEvent& aPointerEvent)
	{
	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// -----------------------------------------------------------------------------
// CClockContainer::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse
CClockContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
	{
	if (aKeyEvent.iCode != EKeyLeftArrow || aKeyEvent.iCode != EKeyRightArrow)
		// Forward key events to listbox
		return iListBox->OfferKeyEventL(aKeyEvent, aType);
	
	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// CClockContainer::HandleListBoxEventL()
// Handles list box events.
// -----------------------------------------------------------------------------
//
void CClockContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemClicked)
		{
		HandleListBoxItemActivationL(iListBox->CurrentItemIndex());
		}
	}

// -----------------------------------------------------------------------------
// CClockContainer::UpdateListBoxL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CClockContainer::UpdateListBoxL()
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

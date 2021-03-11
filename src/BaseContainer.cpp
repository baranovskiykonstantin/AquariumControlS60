/*
 ============================================================================
 Name        : BaseContainer.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Implementation of base class of view's containers.
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <aknviewappui.h> 
#include "AquariumControl.hrh"
#include "BaseContainer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBaseContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBaseContainer::ConstructL(const TRect& aRect)
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
// CBaseContainer::CBaseContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CBaseContainer::CBaseContainer()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CBaseContainer::~CBaseContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CBaseContainer::~CBaseContainer()
	{
	if (iListBox)
		{
		delete iListBox;
		}
	}

// -----------------------------------------------------------------------------
// CBaseContainer::CountComponentControls()
// Gets the number of controls contained in a compound control.
// -----------------------------------------------------------------------------
//
TInt CBaseContainer::CountComponentControls() const
	{
	return 1;
	}

// -----------------------------------------------------------------------------
// CBaseContainer::ComponentControl()
// Gets an indexed component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CBaseContainer::ComponentControl(TInt aIndex) const
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
// CBaseContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CBaseContainer::SizeChanged()
	{
	if (iListBox)
		{
		iListBox->SetRect(Rect());
		}
	}

// -----------------------------------------------------------------------------
// CBaseContainer::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CBaseContainer::HandlePointerEventL(
		const TPointerEvent& aPointerEvent)
	{
	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// -----------------------------------------------------------------------------
// CBaseContainer::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse
CBaseContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
	{
	if (aKeyEvent.iCode != EKeyLeftArrow && aKeyEvent.iCode != EKeyRightArrow)
		{
		// Forward key events to listbox
		return iListBox->OfferKeyEventL(aKeyEvent, aType);
		}
	
	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// CBaseContainer::HandleListBoxEventL()
// Handles list box events.
// -----------------------------------------------------------------------------
//
void CBaseContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemClicked)
		{
		HandleListBoxItemActivationL(iListBox->CurrentItemIndex());
		}
	}

// -----------------------------------------------------------------------------
// CBaseContainer::UpdateListBoxL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CBaseContainer::UpdateListBoxL()
	{

	}

// -----------------------------------------------------------------------------
// CBaseContainer::HandleListBoxItemActivationL()
// Handles list box item activation.
// -----------------------------------------------------------------------------
//
void CBaseContainer::HandleListBoxItemActivationL(TInt /*aItem*/)
	{

	}

// End of File

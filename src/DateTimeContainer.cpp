/*
 ============================================================================
 Name        : DateTimeContainer.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "DateTimeContainer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDateTimeContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDateTimeContainer* CDateTimeContainer::NewL(const TRect& aRect)
	{
	CDateTimeContainer* self = CDateTimeContainer::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CDateTimeContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDateTimeContainer* CDateTimeContainer::NewLC(const TRect& aRect)
	{
	CDateTimeContainer* self = new (ELeave) CDateTimeContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CDateTimeContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDateTimeContainer::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CDateTimeContainer::CDateTimeContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDateTimeContainer::CDateTimeContainer()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDateTimeContainer::~CDateTimeContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CDateTimeContainer::~CDateTimeContainer()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDateTimeContainer::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CDateTimeContainer::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// CDateTimeContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CDateTimeContainer::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CDateTimeContainer::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CDateTimeContainer::HandlePointerEventL(
		const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// End of File

/*
 ============================================================================
 Name        : AquariumControlAppView.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "AquariumControlAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAquariumControlAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAquariumControlAppView* CAquariumControlAppView::NewL(const TRect& aRect)
	{
	CAquariumControlAppView* self = CAquariumControlAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAquariumControlAppView* CAquariumControlAppView::NewLC(const TRect& aRect)
	{
	CAquariumControlAppView* self = new (ELeave) CAquariumControlAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAquariumControlAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppView::CAquariumControlAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAquariumControlAppView::CAquariumControlAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppView::~CAquariumControlAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CAquariumControlAppView::~CAquariumControlAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CAquariumControlAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// CAquariumControlAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CAquariumControlAppView::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CAquariumControlAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CAquariumControlAppView::HandlePointerEventL(
		const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// End of File

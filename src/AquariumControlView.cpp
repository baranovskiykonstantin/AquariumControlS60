/*
 ============================================================================
 Name        : AquariumControlView.cpp
 Author      : Konstantin Baranovskiy
 Version	 : 1.0
 Copyright   : GPLv3
 Description : The view class implementation.
 ============================================================================
 */

#include <aknviewappui.h>
#include <avkon.hrh>
#include <aquariumcontrol_0xa291b5a5.rsg>
#include "AquariumControl.hrh"
#include "AquariumControl.pan"
#include "BaseContainer.h"
#include "ClockContainer.h"
#include "AquariumControlView.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CAquariumControlView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CAquariumControlView::ConstructL(CAknTabGroup* aTabGroup,
		TAquariumControlViewTabIds aTabId)
	{
	switch (aTabId)
		{
		// construct from resources
		case EAquariumControlClockViewTab:
			BaseConstructL(R_CLOCK_VIEW);
			break;
			
		default:
			Panic(EAquariumControlViewTabId);
			break;
		}
	iTabGroup = aTabGroup;
	iId = aTabId;
	}

// ---------------------------------------------------------
// CAquariumControlView::~CAquariumControlView()
// Default destructor
// ---------------------------------------------------------
//
CAquariumControlView::~CAquariumControlView()
	{
	if (iContainer)
		{
		AppUi()->RemoveFromViewStack(*this, iContainer);
		}

	delete iContainer;
	}

// ---------------------------------------------------------
// TUid CAquariumControlView::Id()
// This returns the view ID
// ---------------------------------------------------------
//
TUid CAquariumControlView::Id() const
	{
	TUid uid = {iId};
	return uid;
	}

// ---------------------------------------------------------
// CAquariumControlView::HandleCommandL(TInt aCommand)
// Here we handle commands for this view. 
// Each view has their own HandleCommandL()
// ---------------------------------------------------------
//
void CAquariumControlView::HandleCommandL(TInt aCommand)
	{
	AppUi()->HandleCommandL(aCommand);
	}

// ---------------------------------------------------------
// CAquariumControlView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CAquariumControlView::HandleClientRectChange()
	{
	if (iContainer)
		{
		iContainer->SetRect(ClientRect());
		}
	}

// ---------------------------------------------------------
// CAquariumControlView::DoActivateL(...)
// This is called when a view needs to be activated. 
// This creates container with its controls.
// It also receives messages sent to the view.
// aPrevViewId is passed s.t. we can return to the previous view.
// aCustomMessageId is an ID for the message.
// aCustomMessage is the actual message.
// ---------------------------------------------------------
//
void CAquariumControlView::DoActivateL(
		const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
		const TDesC8& /*aCustomMessage*/)
	{
	if (!iContainer)
		{
		switch (iId)
			{
			case EAquariumControlClockViewTab:
				iContainer = CClockContainer::NewL(ClientRect());
				break;

			default:
				Panic(EAquariumControlViewTabId);
				break;
			}

		iContainer->SetMopParent(this);
		AppUi()->AddToStackL(*this, iContainer);
		}

	// Message handling would take place here.
	}

// ---------------------------------------------------------
// CAquariumControlView::HandleCommandL(TInt aCommand)
// This is called when a view needs to be deactivated. 
// This destroys container with its controls.
// ---------------------------------------------------------
//
void CAquariumControlView::DoDeactivate()
	{
	if (iContainer)
		{
		AppUi()->RemoveFromViewStack(*this, iContainer);
		}
	
	delete iContainer;
	iContainer = NULL;
	}


// End of File

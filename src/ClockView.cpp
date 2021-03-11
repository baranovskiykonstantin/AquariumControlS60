/*
 ============================================================================
 Name        : ClockView.cpp
 Author      : Konstantin Baranovskiy
 Version	 : 1.0
 Copyright   : GPLv3
 Description : The clock view class implementation.
 ============================================================================
 */

#include <aknviewappui.h>
#include <avkon.hrh>
#include <aquariumcontrol_0xa291b5a5.rsg>
#include "AquariumControl.hrh"
#include "ClockContainer.h"
#include "ClockView.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CClockView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CClockView::ConstructL(CAknTabGroup* aTabGroup)
	{
	// construct from resources
	BaseConstructL(R_CLOCK_VIEW);
	iTabGroup = aTabGroup;
	}

// ---------------------------------------------------------
// CClockView::~CClockView()
// Default destructor
// ---------------------------------------------------------
//
CClockView::~CClockView()
	{
	if (iContainer)
		{
		AppUi()->RemoveFromViewStack(*this, iContainer);
		}

	delete iContainer;
	}

// ---------------------------------------------------------
// TUid CClockView::Id()
// This returns the view ID
// ---------------------------------------------------------
//
TUid CClockView::Id() const
	{
	return KViewId;
	}

// ---------------------------------------------------------
// CClockView::HandleCommandL(TInt aCommand)
// Here we handle commands for this view. 
// Each view has their own HandleCommandL()
// ---------------------------------------------------------
//
void CClockView::HandleCommandL(TInt aCommand)
	{
	AppUi()->HandleCommandL(aCommand);
	}

// ---------------------------------------------------------
// CClockView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CClockView::HandleClientRectChange()
	{
	if (iContainer)
		{
		iContainer->SetRect(ClientRect());
		}
	}

// ---------------------------------------------------------
// CClockView::DoActivateL(...)
// This is called when a view needs to be activated. 
// This creates container with its controls.
// It also receives messages sent to the view.
// aPrevViewId is passed s.t. we can return to the previous view.
// aCustomMessageId is an ID for the message.
// aCustomMessage is the actual message.
// ---------------------------------------------------------
//
void CClockView::DoActivateL(
		const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
		const TDesC8& /*aCustomMessage*/)
	{
	if (!iContainer)
		{
		iContainer = CClockContainer::NewL(ClientRect());
		iContainer->SetMopParent(this);
		AppUi()->AddToStackL(*this, iContainer);
		}

	// Message handling would take place here.
	}

// ---------------------------------------------------------
// CClockView::HandleCommandL(TInt aCommand)
// This is called when a view needs to be deactivated. 
// This destroys container with its controls.
// ---------------------------------------------------------
//
void CClockView::DoDeactivate()
	{
	if (iContainer)
		{
		AppUi()->RemoveFromViewStack(*this, iContainer);
		}
	
	delete iContainer;
	iContainer = NULL;
	}


// End of File

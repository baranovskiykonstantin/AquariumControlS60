/*
 ============================================================================
 Name        : DateTimeView.cpp
 Author      : Konstantin Baranovskiy
 Version	 : 1.0
 Copyright   : GPLv3
 Description : The date/time view class implementation.
 ============================================================================
 */

#include <aknviewappui.h>
#include <avkon.hrh>
#include <aquariumcontrol_0xa291b5a5.rsg>
#include "AquariumControl.hrh"
#include "DateTimeContainer.h"
#include "DateTimeView.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDateTimeView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CDateTimeView::ConstructL(CAknTabGroup* aTabGroup)
	{
	// construct R_MYVIEW_VIEW1 resources
	BaseConstructL(R_DATETIME_VIEW);
	iTabGroup=aTabGroup;
	}

// ---------------------------------------------------------
// CDateTimeView::~CDateTimeView()
// Default destructor
// ---------------------------------------------------------
//
CDateTimeView::~CDateTimeView()
	{
	if (iContainer)
		{
		AppUi()->RemoveFromViewStack(*this, iContainer);
		}

	delete iContainer;
	}

// ---------------------------------------------------------
// TUid CDateTimeView::Id()
// This returns the view ID
// ---------------------------------------------------------
//
TUid CDateTimeView::Id() const
	{
	return KViewId;
	}

// ---------------------------------------------------------
// CDateTimeView::HandleCommandL(TInt aCommand)
// Here we handle commands for this view. 
// Each view has their own HandleCommandL()
// ---------------------------------------------------------
//
void CDateTimeView::HandleCommandL(TInt aCommand)
	{
	AppUi()->HandleCommandL(aCommand);
	}

// ---------------------------------------------------------
// CDateTimeView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CDateTimeView::HandleClientRectChange()
	{
	if (iContainer)
		{
		iContainer->SetRect(ClientRect());
		}
	}

// ---------------------------------------------------------
// CDateTimeView::DoActivateL(...)
// This is called when a view needs to be activated. 
// This creates container with its controls.
// It also receives messages sent to the view.
// aPrevViewId is passed s.t. we can return to the previous view.
// aCustomMessageId is an ID for the message.
// aCustomMessage is the actual message.
// ---------------------------------------------------------
//
void CDateTimeView::DoActivateL(
		const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
		const TDesC8& /*aCustomMessage*/)
	{
	if (!iContainer)
		{
		iContainer = CDateTimeContainer::NewL(ClientRect());
		iContainer->SetMopParent(this);
		AppUi()->AddToStackL(*this, iContainer);
		}

	// Message handling would take place here.
	}

// ---------------------------------------------------------
// CDateTimeView::HandleCommandL(TInt aCommand)
// This is called when a view needs to be deactivated. 
// This destroys container with its controls.
// ---------------------------------------------------------
//
void CDateTimeView::DoDeactivate()
	{
	if (iContainer)
		{
		AppUi()->RemoveFromViewStack(*this, iContainer);
		}
	
	delete iContainer;
	iContainer = NULL;
	}


// End of File

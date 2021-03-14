/*
 ============================================================================
 Name        : AquariumControlViewAppUi.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : CAquariumControlViewAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>

#include <aquariumcontrol_0xa291b5a5.rsg>

#include "AquariumControl.hrh"
#include "AquariumControl.pan"
#include "AquariumControlApplication.h"
#include "AquariumControlView.h"
#include "AquariumControlData.h"
#include "AquariumControlViewAppUi.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknViewAppUi::EAknEnableSkin);

	// Aquarium data
	iData = CAquariumControlData::NewL();

	// Show tabs for main views from resources
	CEikStatusPane* sp = StatusPane();

	// Fetch pointer to the default navi pane control
	iNaviPane = (CAknNavigationControlContainer*)sp->ControlL(
		TUid::Uid(EEikStatusPaneUidNavi));

	// Tabgroup has been read from resource and it was pushed to the navi pane.
	// Get pointer to the navigation decorator with the ResourceDecorator() function.
	// Application owns the decorator and it has responsibility to delete the object.
	iDecoratedTabGroup = iNaviPane->ResourceDecorator();
	if (iDecoratedTabGroup)
		{
		iTabGroup = (CAknTabGroup*) iDecoratedTabGroup->DecoratedControl();
		}

	CAquariumControlView* clockView = new (ELeave) CAquariumControlView;
	CleanupStack::PushL(clockView);
	clockView->ConstructL(iTabGroup, EAquariumControlClockViewTab);
	AddViewL(clockView);					// transfer ownership to CAknViewAppUi
	CleanupStack::Pop(clockView);
	iClockViewId = clockView->Id();	// view id to get view from CAknViewAppUi

	CAquariumControlView* lightView = new (ELeave) CAquariumControlView;
	CleanupStack::PushL(lightView);
	lightView->ConstructL(iTabGroup, EAquariumControlLightViewTab);
	AddViewL(lightView);
	CleanupStack::Pop(lightView);
	iLightViewId = lightView->Id();

	SetDefaultViewL(*clockView);
	}
// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CAquariumControlViewAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAquariumControlViewAppUi::CAquariumControlViewAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::~CAquariumControlViewAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CAquariumControlViewAppUi::~CAquariumControlViewAppUi()
	{
	delete iDecoratedTabGroup;
	delete iData;
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case EAquariumControlConnect:
			iData->iIsConnected = ETrue;
			
			UpdateViewsL();
			break;

		case EAquariumControlDisconnect:
			iData->iIsConnected = EFalse;
			UpdateViewsL();
			break;

		case EAquariumControlSetTime:
		case EAquariumControlSetTimeCorrection:
			iEikonEnv->InfoMsg(_L("Set Time"));
			break;

		case EAquariumControlSetDate:
		case EAquariumControlSetDayOfWeek:
			iEikonEnv->InfoMsg(_L("Set Date"));
			break;

		case EAquariumControlSetLightSwitchState:
			{
			switch (iData->iLightState)
				{
				case (TAquariumDeviceState) EOn:
					iData->iLightState = (TAquariumDeviceState) EOff;
					break;
				case (TAquariumDeviceState) EOff:
					iData->iLightState = (TAquariumDeviceState) EOn;
					break;
				default:
					iData->iLightState = (TAquariumDeviceState) EOff;
					break;
				}
			}
			UpdateViewsL();
			break;
		case EAquariumControlSetLightSwitchMode:
			{
			switch (iData->iLightMode)
				{
				case (TAquariumDeviceMode) EAuto:
					iData->iLightMode = (TAquariumDeviceMode) EManual;
					break;
				case (TAquariumDeviceMode) EManual:
					iData->iLightMode = (TAquariumDeviceMode) EAuto;
					break;
				default:
					iData->iLightMode = (TAquariumDeviceMode) EAuto;
					break;
				}
			}
			UpdateViewsL();
			break;
		case EAquariumControlSetLightOnTime:
		case EAquariumControlSetLightOffTime:
		case EAquariumControlSetLightLevel:
		case EAquariumControlSetLightRise:
		case EAquariumControlSetLightStateOn:
		case EAquariumControlSetLightStateOff:
		case EAquariumControlSetLightStateAuto:
			iEikonEnv->InfoMsg(_L("Light"));
			break;

		case EAbout:
			{
			CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
			dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
			HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
			dlg->QueryHeading()->SetTextL(*title);
			CleanupStack::PopAndDestroy(); //title
			HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
			dlg->SetMessageTextL(*msg);
			CleanupStack::PopAndDestroy(); //msg
			dlg->RunLD();
			}
			break;

		default:
			Panic(EAquariumControlViewAppUi);
			break;
		}
	}

// ------------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleForegroundEventL(TBool aForeground)
//  This function is called by the  framework when the screen loses or gains focus.
//   i.e. when it goes to the background or to the foreground. Incoming call
//   softnote is an example.
// This event applies to the entire application, all views.
// ------------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleForegroundEventL(TBool aForeground)
	{
	if (aForeground==TRUE)
	iEikonEnv->InfoMsg(_L("Foreground true"));
	else
	iEikonEnv->InfoMsg(_L("Foreground false"));
	}

// ----------------------------------------------------
// CAquariumControlViewAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Here we handle key events: Right and left arrow key
//   to change view.
// ----------------------------------------------------
//
TKeyResponse CAquariumControlViewAppUi::HandleKeyEventL(
		const TKeyEvent& aKeyEvent, TEventCode /*aType*/)
	{
	if (iTabGroup == NULL)
		{
		return EKeyWasNotConsumed;
		}

	TInt active = iTabGroup->ActiveTabIndex();
	TInt count = iTabGroup->TabCount();

	switch (aKeyEvent.iCode)
		{
		case EKeyLeftArrow:
			if (active > 0)
				{
				active--;
				iTabGroup->SetActiveTabByIndex(active);
				// ActivateLocalViewL() is used to change the view.
				// To change view from another application we would use ActivateViewL()
				// Send an empty message
				ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
				}
			break;
		case EKeyRightArrow:
			if((active + 1) < count)
				{
				active++;
				iTabGroup->SetActiveTabByIndex(active);
				// ActivateLocalViewL() is used to change the view.
				// To change view from another application we would use ActivateViewL()
				ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
				}
			break;
		default:
			return EKeyWasNotConsumed;
		}

	return EKeyWasConsumed;
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleResourceChangeL( TInt aType )
// Called by framework when layout is changed.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleResourceChangeL(TInt aType)
	{
	CAknAppUi::HandleResourceChangeL(aType);

	// ADDED FOR SCALABLE UI SUPPORT
	// *****************************
	if (aType == KEikDynamicLayoutVariantSwitch)
		{
		((CAquariumControlView*) View(iClockViewId))->HandleClientRectChange();
		((CAquariumControlView*) View(iLightViewId))->HandleClientRectChange();
		}

	}

// ---------------------------------------------------------------------------
// CAquariumControlViewAppUi::DynInitMenuPaneL()
// This function is called by the EIKON framework just before it displays a 
// menu pane.
// ---------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::DynInitMenuPaneL(TInt aResourceId,
		CEikMenuPane* aMenuPane)
	{
	if(aResourceId == R_COMMON_MENU)
		{
		if (iData->iIsConnected)
			aMenuPane->SetItemDimmed(EAquariumControlConnect, ETrue);
		else
			aMenuPane->SetItemDimmed(EAquariumControlDisconnect, ETrue);
		}
	}

// ---------------------------------------------------------------------------
// CAquariumControlViewAppUi::UpdateViewsL()
// Update all views.
// ---------------------------------------------------------------------------
//
inline void CAquariumControlViewAppUi::UpdateViewsL()
	{
	((CAquariumControlView*) View(iClockViewId))->UpdateL();
	((CAquariumControlView*) View(iLightViewId))->UpdateL();
	}

// End of File

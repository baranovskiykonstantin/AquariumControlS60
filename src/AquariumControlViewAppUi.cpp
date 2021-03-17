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
#include <aknquerydialog.h>
#include <aknlistquerydialog.h>
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
	CAknNavigationControlContainer* naviPane = (CAknNavigationControlContainer*)sp->ControlL(
		TUid::Uid(EEikStatusPaneUidNavi));

	// Tabgroup has been read from resource and it was pushed to the navi pane.
	// Get pointer to the navigation decorator with the ResourceDecorator() function.
	// Application owns the decorator and it has responsibility to delete the object.
	iDecoratedTabGroup = naviPane->ResourceDecorator();
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

	CAquariumControlView* heatView = new (ELeave) CAquariumControlView;
	CleanupStack::PushL(heatView);
	heatView->ConstructL(iTabGroup, EAquariumControlHeatViewTab);
	AddViewL(heatView);
	CleanupStack::Pop(heatView);
	iHeatViewId = heatView->Id();

	CAquariumControlView* displayView = new (ELeave) CAquariumControlView;
	CleanupStack::PushL(displayView);
	displayView->ConstructL(iTabGroup, EAquariumControlDisplayViewTab);
	AddViewL(displayView);
	CleanupStack::Pop(displayView);
	iDisplayViewId = displayView->Id();

	SetDefaultViewL(*clockView);

	iTimer = CPeriodic::NewL(EPriorityLow);
	const TInt tickInterval = 1000000;
	iTimer->Start(tickInterval, tickInterval, TCallBack(TimerCallBack, this));

	// TEST
	iData->ParseLineL(_L8("Date: 01.02.2017 Friday"));
	iData->ParseLineL(_L8("Time: 13:29:59 (-3 sec at 12:00:00)"));
	iData->ParseLineL(_L8("Temp: 21"));
	iData->ParseLineL(_L8("Heat: OFF auto (20-22)"));
	iData->ParseLineL(_L8("Light: ON manual (10:00:00-20:00:00) 43/50% 10min"));
	iData->ParseLineL(_L8("Display: time"));
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
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	iTimer = NULL;
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleCommandL(TInt aCommand)
	{
	TBool updatingIsNeeded(EFalse);
	if (iData->iConnectionStatus == EConnected)
		iData->iConnectionStatus = EPaused;
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case EAquariumControlConnect:
			iData->iConnectionStatus = EConnected;
			
			updatingIsNeeded = ETrue;
			break;
		case EAquariumControlDisconnect:
			iData->iConnectionStatus = EDisconnected;
			updatingIsNeeded = ETrue;
			break;

		case EAquariumControlSetTime:
			{
			TInt variant;
			TInt answer;
			CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&variant);
			answer = dlg->ExecuteLD(R_SETTIMEVARIANTS_QUERY_DIALOG);
			if (EAknSoftkeyOk == answer)
				{
				TTime time;
				if (variant == 0)
					{
					time.HomeTime();
					iData->iHours = time.DateTime().Hour();
					iData->iMinutes = time.DateTime().Minute();
					iData->iSeconds = time.DateTime().Second();
					updatingIsNeeded = ETrue;
					iEikonEnv->InfoMsg(_L("Set current"));
					}
				else if (variant == 1)
					{
					_LIT(KTimeStrFormat, ":%02u%02u%02u.");
					TBuf<8> timeStr;
					timeStr.Format(KTimeStrFormat, iData->iHours, iData->iMinutes, iData->iSeconds);
					time.Set(timeStr);
					CAknTimeQueryDialog* dlg = new (ELeave) CAknTimeQueryDialog(time);
					answer = dlg->ExecuteLD(R_SETTIME_QUERY_DIALOG);
					if (EAknSoftkeyOk == answer)
						{
						iData->iHours = time.DateTime().Hour();
						iData->iMinutes = time.DateTime().Minute();
						iData->iSeconds = time.DateTime().Second();
						updatingIsNeeded = ETrue;
						iEikonEnv->InfoMsg(_L("Set custom"));
						}
					}
				else
					{
					break;
					}
				}
			}
			break;
		case EAquariumControlSetTimeCorrection:
			iEikonEnv->InfoMsg(_L("Set Time"));
			break;
		case EAquariumControlSetDate:
		case EAquariumControlSetDayOfWeek:
			iEikonEnv->InfoMsg(_L("Set Date"));
			break;

		case EAquariumControlSwitchLightState:
			{
			switch (iData->iLightState)
				{
				case (TAquariumDeviceState) EOn:
					iData->iLightState = EOff;
					break;
				case (TAquariumDeviceState) EOff:
					iData->iLightState = EOn;
					break;
				default:
					iData->iLightState = EOff;
					break;
				}
			}
			updatingIsNeeded = ETrue;
			break;
		case EAquariumControlSwitchLightMode:
			{
			switch (iData->iLightMode)
				{
				case (TAquariumDeviceMode) EAuto:
					iData->iLightMode = EManual;
					break;
				case (TAquariumDeviceMode) EManual:
					iData->iLightMode = EAuto;
					break;
				default:
					iData->iLightMode = EAuto;
					break;
				}
			}
			updatingIsNeeded = ETrue;
			break;
		case EAquariumControlSetLightOnTime:
		case EAquariumControlSetLightOffTime:
		case EAquariumControlSetLightLevel:
		case EAquariumControlSetLightRise:
			iEikonEnv->InfoMsg(_L("Light"));
			break;
		case EAquariumControlSetLightStateOn:
		case EAquariumControlSetLightStateOff:
		case EAquariumControlSetLightModeAuto:
			iEikonEnv->InfoMsg(_L("Light toolbar"));
			break;

		case EAquariumControlWaterTemp:
			// dummy
			break;
		case EAquariumControlSwitchHeatState:
			{
			switch (iData->iHeatState)
				{
				case (TAquariumDeviceState) EOn:
					iData->iHeatState = EOff;
					break;
				case (TAquariumDeviceState) EOff:
					iData->iHeatState = EOn;
					break;
				default:
					iData->iHeatState = EOff;
					break;
				}
			}
			updatingIsNeeded = ETrue;
			break;
		case EAquariumControlSwitchHeatMode:
			{
			switch (iData->iHeatMode)
				{
				case (TAquariumDeviceMode) EAuto:
					iData->iHeatMode = EManual;
					break;
				case (TAquariumDeviceMode) EManual:
					iData->iHeatMode = EAuto;
					break;
				default:
					iData->iHeatMode = EAuto;
					break;
				}
			}
			updatingIsNeeded = ETrue;
			break;
		case EAquariumControlSetHeatLow:
		case EAquariumControlSetHeatHigh:
			iEikonEnv->InfoMsg(_L("Heat"));
			break;
		case EAquariumControlSetHeatStateOn:
		case EAquariumControlSetHeatStateOff:
		case EAquariumControlSetHeatModeAuto:
			iEikonEnv->InfoMsg(_L("Heat toolbar"));
			break;

		case EAbout:
			{
			CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
			dlg->ExecuteLD(R_ABOUT_QUERY_DIALOG);
			}
			break;

		case EAquariumControlSwitchDisplayMode:
			{
			switch (iData->iDisplayMode)
				{
				case (TAquariumDispalyMode) ETime:
					iData->iDisplayMode = ETemperature;
					break;
				case (TAquariumDispalyMode) ETemperature:
					iData->iDisplayMode = ETime;
					break;
				default:
					iData->iDisplayMode = ETime;
					break;
				}
			}
			updatingIsNeeded = ETrue;
			break;

		default:
			Panic(EAquariumControlViewAppUi);
			break;
		}
	if (iData->iConnectionStatus == EPaused)
		iData->iConnectionStatus = EConnected;
	if (updatingIsNeeded)
		UpdateViewsL();
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
	if (aForeground == TRUE)
		{
		if (iData->iConnectionStatus == EPaused)
			iData->iConnectionStatus = EConnected;
		}
	else
		{
		if (iData->iConnectionStatus == EConnected)
			iData->iConnectionStatus = EPaused;
		}
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
		((CAquariumControlView*) View(iHeatViewId))->HandleClientRectChange();
		((CAquariumControlView*) View(iDisplayViewId))->HandleClientRectChange();
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
		if (iData->iConnectionStatus == EDisconnected)
			aMenuPane->SetItemDimmed(EAquariumControlDisconnect, ETrue);
		else
			aMenuPane->SetItemDimmed(EAquariumControlConnect, ETrue);
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
	((CAquariumControlView*) View(iHeatViewId))->UpdateL();
	((CAquariumControlView*) View(iDisplayViewId))->UpdateL();
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::TimerCallBack()
// Update views.
// ----------------------------------------------------------------------------
//
TInt CAquariumControlViewAppUi::TimerCallBack(TAny* aObject)
	{
	CAquariumControlViewAppUi* self = static_cast<CAquariumControlViewAppUi*> (aObject);
	TInt error(KErrNone);
	if (self->iData->iConnectionStatus == EConnected)
		{
		self->iData->iSeconds += 1;
		if (self->iData->iSeconds > 59)
			self->iData->iSeconds = 0;
		TRAP(error, self->UpdateViewsL());
		}
	return error;
	}

// End of File

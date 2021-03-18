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

// CONSTANTS
_LIT(KStatusCmd, "status\n");

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

	// Bluetooth
	iBtClient = CRFtermBt::NewL();
	iBtClient->SetObserver(this);
	// check whether BT is available or not
	RSdp sdpSession;
	if (sdpSession.Connect() == KErrNone)
		{
		sdpSession.Close();
		iBtAvailable = ETrue;
		}
	else
		{
		iBtAvailable = EFalse;
		}

	// Empty by default
	iBtDataTail = HBufC::New(0);

	// TEST
/*
	HandleBtDataL(_L("Date: 01.02.17 Friday\r\nTime: 13:29:59 (-3 sec at 12:00:00)\r\nTemp: 21\r\nHeat: OFF"));
	HandleBtDataL(_L(" auto (20-22)\r\nLight: ON manual (10:00:00-20:00:00) 43/50% 10min\r\nDisplay: time\r\n"));

	iData->ParseLineL(_L8("Date: 01.02.17 Friday"));
	iData->ParseLineL(_L8("Time: 13:29:59 (-3 sec at 12:00:00)"));
	iData->ParseLineL(_L8("Temp: 21"));
	iData->ParseLineL(_L8("Heat: OFF auto (20-22)"));
	iData->ParseLineL(_L8("Light: ON manual (10:00:00-20:00:00) 43/50% 10min"));
	iData->ParseLineL(_L8("Display: time"));
*/
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
	delete iBtClient;
	delete iBtDataTail;
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleCommandL(TInt aCommand)
	{
	TBool updatingIsNeeded(EFalse);
	PauseUpdating();
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case EAquariumControlConnect:
			if (!iBtAvailable)
				ShowBTNotAvailableNoteL();
			else
				iBtClient->ConnectL();
			break;
		case EAquariumControlDisconnect:
			if (iBtAvailable)
				iBtClient->DisconnectL();
			break;

		case EAquariumControlSetTime:
			updatingIsNeeded = CommandSetTime();
			break;
		case EAquariumControlSetTimeCorrection:
			updatingIsNeeded = CommandSetTimeCor();
			break;
		case EAquariumControlSetDate:
			updatingIsNeeded = CommandSetDate();
			break;
		case EAquariumControlSetDayOfWeek:
			updatingIsNeeded = CommandSetDayOfWeek();
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
			updatingIsNeeded = CommandSetLightOnTime();
			break;
		case EAquariumControlSetLightOffTime:
			updatingIsNeeded = CommandSetLightOffTime();
			break;
		case EAquariumControlSetLightLevel:
			updatingIsNeeded = CommandSetLightLevel();
			break;
		case EAquariumControlSetLightRise:
			updatingIsNeeded = CommandSetLightRise();
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
			updatingIsNeeded = CommandSetHeatLow();
			break;
		case EAquariumControlSetHeatHigh:
			updatingIsNeeded = CommandSetHeatHigh();
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
	ResumeUpdating();
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
		ResumeUpdating();
	else
		PauseUpdating();
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
		if (iData->iConnectionStatus == EStatusDisconnected)
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
	if (self->iData->iConnectionStatus == EStatusConnected)
		{
		TRAP(error, self->iBtClient->SendMessageL(KStatusCmd));
		}
	return error;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::PauseUpdating()
// Pause view updating.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::PauseUpdating()
	{
	if (iData->iConnectionStatus == EStatusConnected)
		iData->iConnectionStatus = EStatusPaused;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::ResumeUpdating()
// Resume view updating.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::ResumeUpdating()
	{
	if (iData->iConnectionStatus == EStatusPaused)
		iData->iConnectionStatus = EStatusConnected;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetTime()
// SetTime command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetTime()
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
			iEikonEnv->InfoMsg(_L("Set current"));
			return ETrue;
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
				iEikonEnv->InfoMsg(_L("Set custom"));
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetTimeCor()
// SetTimeCor command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetTimeCor()
	{
	TInt answer;
	TInt timeCorr(iData->iTimeCorrection);
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_TIMECOR);
	CAknNumberQueryDialog* dlg = new (ELeave) CAknNumberQueryDialog(timeCorr);
	dlg->PrepareLC(R_SETINT_QUERY_DIALOG);
	dlg->SetPromptL(*title);
	dlg->SetMinimumAndMaximum(-59, 59);
	answer = dlg->RunLD();
	CleanupStack::PopAndDestroy(title);
	if (EAknSoftkeyOk == answer)
		{
		iData->iTimeCorrection = timeCorr;
		iEikonEnv->InfoMsg(_L("Set TimeCorr"));
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetDate()
// SetDate command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetDate()
	{
	TInt variant;
	TInt answer;
	CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&variant);
	answer = dlg->ExecuteLD(R_SETDATEVARIANTS_QUERY_DIALOG);
	if (EAknSoftkeyOk == answer)
		{
		TTime date;
		if (variant == 0)
			{
			date.HomeTime();
			iData->iDay = date.DateTime().Day() + 1;
			iData->iMonth = date.DateTime().Month() + 1;
			iData->iYear = date.DateTime().Year() - 2000;
			iEikonEnv->InfoMsg(_L("Set current"));
			return ETrue;
			}
		else if (variant == 1)
			{
			_LIT(KTimeStrFormat, "20%02u%02u%02u:.");
			TBuf<10> dateStr;
			dateStr.Format(KTimeStrFormat, iData->iYear, iData->iMonth - 1 , iData->iDay - 1);
			date.Set(dateStr);
			CAknTimeQueryDialog* dlg = new (ELeave) CAknTimeQueryDialog(date);
			answer = dlg->ExecuteLD(R_SETDATE_QUERY_DIALOG);
			if (EAknSoftkeyOk == answer)
				{
				iData->iDay = date.DateTime().Day() + 1;
				iData->iMonth = date.DateTime().Month() + 1;
				iData->iYear = date.DateTime().Year() - 2000;
				iEikonEnv->InfoMsg(_L("Set custom"));
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetDayOfWeek()
// SetDayOfWeek command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetDayOfWeek()
	{
	TInt dayOfWeek;
	TInt answer;
	CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&dayOfWeek);
	answer = dlg->ExecuteLD(R_SETDAYOFWEEK_QUERY_DIALOG);
	if (EAknSoftkeyOk == answer)
		{
		iData->iDayOfWeek = dayOfWeek + 1;
		iEikonEnv->InfoMsg(_L("Set DayOfWeek"));
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightOnTime()
// SetLightOnTime command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetLightOnTime()
	{
	_LIT(KTimeStrFormat, ":%02u%02u%02u.");
	_LIT(KMinTimeStr, ":000000.");
	TInt answer;
	TTime timeOn;
	TTime minTime, maxTime;
	TBuf<8> timeOnStr;
	TBuf<8> maxTimeStr;
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_ON_TIME);
	timeOnStr.Format(KTimeStrFormat, iData->iLightOnHours, iData->iLightOnMinutes, iData->iLightOnSeconds);
	maxTimeStr.Format(KTimeStrFormat, iData->iLightOffHours, iData->iLightOffMinutes, iData->iLightOffSeconds);
	timeOn.Set(timeOnStr);
	minTime.Set(KMinTimeStr);
	maxTime.Set(maxTimeStr);
	maxTime -= TTimeIntervalSeconds(1);
	CAknTimeQueryDialog* dlg = new (ELeave) CAknTimeQueryDialog(timeOn);
	dlg->PrepareLC(R_SETTIME_QUERY_DIALOG);
	dlg->SetPromptL(*title);
	dlg->SetMinimumAndMaximum(minTime, maxTime);
	answer = dlg->RunLD();
	CleanupStack::PopAndDestroy(title);
	if (EAknSoftkeyOk == answer)
		{
		iData->iLightOnHours = timeOn.DateTime().Hour();
		iData->iLightOnMinutes = timeOn.DateTime().Minute();
		iData->iLightOnSeconds = timeOn.DateTime().Second();
		iEikonEnv->InfoMsg(_L("Set TimeOn"));
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightOffTime()
// SetLightOffTime command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetLightOffTime()
	{
	_LIT(KTimeStrFormat, ":%02u%02u%02u.");
	_LIT(KMaxTimeStr, ":235959.");
	TInt answer;
	TTime timeOff;
	TTime minTime, maxTime;
	TBuf<8> timeOffStr;
	TBuf<8> minTimeStr;
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_OFF_TIME);
	timeOffStr.Format(KTimeStrFormat, iData->iLightOffHours, iData->iLightOffMinutes, iData->iLightOffSeconds);
	minTimeStr.Format(KTimeStrFormat, iData->iLightOnHours, iData->iLightOnMinutes, iData->iLightOnSeconds);
	timeOff.Set(timeOffStr);
	minTime.Set(minTimeStr);
	maxTime.Set(KMaxTimeStr);
	minTime += TTimeIntervalSeconds(1);
	CAknTimeQueryDialog* dlg = new (ELeave) CAknTimeQueryDialog(timeOff);
	dlg->PrepareLC(R_SETTIME_QUERY_DIALOG);
	dlg->SetPromptL(*title);
	dlg->SetMinimumAndMaximum(minTime, maxTime);
	answer = dlg->RunLD();
	CleanupStack::PopAndDestroy(title);
	if (EAknSoftkeyOk == answer)
		{
		iData->iLightOffHours = timeOff.DateTime().Hour();
		iData->iLightOffMinutes = timeOff.DateTime().Minute();
		iData->iLightOffSeconds = timeOff.DateTime().Second();
		iEikonEnv->InfoMsg(_L("Set TimeOn"));
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightLevel()
// SetLightLevel command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetLightLevel()
	{
	TInt answer;
	TInt level(iData->iLightLevel);
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_LEVEL);
	CAknNumberQueryDialog* dlg = new (ELeave) CAknNumberQueryDialog(level);
	dlg->PrepareLC(R_SETINT_QUERY_DIALOG);
	dlg->SetPromptL(*title);
	dlg->SetMinimumAndMaximum(0, 100);
	answer = dlg->RunLD();
	CleanupStack::PopAndDestroy(title);
	if (EAknSoftkeyOk == answer)
		{
		iData->iLightLevel = level;
		iEikonEnv->InfoMsg(_L("Set Level"));
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightRise()
// SetLightRise command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetLightRise()
	{
	TInt answer;
	TInt rise(iData->iLightRise);
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_RISE);
	CAknNumberQueryDialog* dlg = new (ELeave) CAknNumberQueryDialog(rise);
	dlg->PrepareLC(R_SETINT_QUERY_DIALOG);
	dlg->SetPromptL(*title);
	dlg->SetMinimumAndMaximum(0, 30);
	answer = dlg->RunLD();
	CleanupStack::PopAndDestroy(title);
	if (EAknSoftkeyOk == answer)
		{
		iData->iLightRise = rise;
		iEikonEnv->InfoMsg(_L("Set Rise"));
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetHeatLow()
// SetHeatLow command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetHeatLow()
	{
	TInt answer;
	TInt low(iData->iHeatLow);
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_LO);
	CAknNumberQueryDialog* dlg = new (ELeave) CAknNumberQueryDialog(low);
	dlg->PrepareLC(R_SETINT_QUERY_DIALOG);
	dlg->SetPromptL(*title);
	dlg->SetMinimumAndMaximum(18, iData->iHeatHigh);
	answer = dlg->RunLD();
	CleanupStack::PopAndDestroy(title);
	if (EAknSoftkeyOk == answer)
		{
		iData->iHeatLow = low;
		iEikonEnv->InfoMsg(_L("Set HeatLow"));
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetHeatHigh()
// SetHeatHigh command handler.
// ----------------------------------------------------------------------------
//
TBool CAquariumControlViewAppUi::CommandSetHeatHigh()
	{
	TInt answer;
	TInt high(iData->iHeatHigh);
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_HEAT_HI);
	CAknNumberQueryDialog* dlg = new (ELeave) CAknNumberQueryDialog(high);
	dlg->PrepareLC(R_SETINT_QUERY_DIALOG);
	dlg->SetPromptL(*title);
	dlg->SetMinimumAndMaximum(iData->iHeatLow, 35);
	answer = dlg->RunLD();
	CleanupStack::PopAndDestroy(title);
	if (EAknSoftkeyOk == answer)
		{
		iData->iHeatHigh = high;
		iEikonEnv->InfoMsg(_L("Set HeatHigh"));
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::ShowBTNotAvailableNoteL()
// Show note if BT is not available 
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::ShowBTNotAvailableNoteL()
	{
	HBufC* textResource = StringLoader::LoadLC(R_ERR_NO_BT);
	CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
	errorNote->ExecuteLD(*textResource);
	CleanupStack::PopAndDestroy(textResource);
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleBtDeviceChangeL()
// Update the information about connected BT.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleBtDeviceChangeL(CBTDevice* aRemoteDevice)
	{
	if (aRemoteDevice)
		{
		iData->iConnectionStatus = EStatusConnected;
		iBtClient->SendMessageL(KStatusCmd);
		}
	else
		{
		iData->iConnectionStatus = EStatusDisconnected;
		UpdateViewsL();
		}
	
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleBtNotifyL()
// Show log message from BT client.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleBtNotifyL(const TDesC& aMessage)
	{
	CAknInformationNote* errorNote = new (ELeave) CAknInformationNote;
	errorNote->ExecuteLD(aMessage);
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleBtDataL()
// Handle received data from BT client.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleBtDataL(const TDesC& aData)
	{
	_LIT(KNewLineMark, "\r\n");
	HBufC* fullData = HBufC::NewLC(iBtDataTail->Length() + aData.Length());
	fullData->Des().Copy(*iBtDataTail);
	fullData->Des().Append(aData);

	TPtrC unprocessed(*fullData);
	TPtrC line;

	// skip command echo
	line.Set(unprocessed.Left(KStatusCmd().Length()));
	if (line == KStatusCmd)
		{
		// Echo has additional '\r' character. Skit it too.
		TInt dataLength = unprocessed.Length() - KStatusCmd().Length() - 1;
		unprocessed.Set(unprocessed.Right(dataLength));
		}

	TInt rnPos = unprocessed.Find(KNewLineMark);
	while (KErrNotFound != rnPos)
		{
		line.Set(unprocessed.Left(rnPos));
		rnPos += KNewLineMark().Length();
		unprocessed.Set(unprocessed.Right(unprocessed.Length() - rnPos));
		rnPos = unprocessed.Find(KNewLineMark);
		iData->ParseLineL(line);
		}

	delete iBtDataTail;
	iBtDataTail = HBufC::NewL(unprocessed.Length());
	iBtDataTail->Des().Copy(unprocessed);

	UpdateViewsL();

	CleanupStack::PopAndDestroy(fullData);
	}

// -----------------------------------------------------------------------------
// CAquariumControlViewAppUi::HandleBtFileSendingFinishL()
// Show note if file has been successfully sent.
// -----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::HandleBtFileSendingFinishL()
	{
	
	}

// End of File

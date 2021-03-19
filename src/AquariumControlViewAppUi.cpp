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
_LIT(KStatusCmd, "status\r");

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
	iBtClient = CBtClient::NewL();
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
	_LIT(KCmdLightOn, "light on\r");
	_LIT(KCmdLightOff, "light off\r");
	_LIT(KCmdLightAuto, "light auto\r");
	_LIT(KCmdHeatOn, "heat on\r");
	_LIT(KCmdHeatOff, "heat off\r");
	_LIT(KCmdHeatAuto, "heat auto\r");
	_LIT(KCmdDisplayTime, "display time\r");
	_LIT(KCmdDisplayTemp, "display temp\r");
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
			CommandSetTime();
			break;
		case EAquariumControlSetTimeCorrection:
			CommandSetTimeCor();
			break;
		case EAquariumControlSetDate:
			CommandSetDate();
			break;
		case EAquariumControlSetDayOfWeek:
			CommandSetDayOfWeek();
			break;

		case EAquariumControlSwitchLightState:
			{
			switch (iData->iLightState)
				{
				case (TAquariumDeviceState) EOn:
					iBtClient->SendMessageL(KCmdLightOff);
					break;
				case (TAquariumDeviceState) EOff:
					iBtClient->SendMessageL(KCmdLightOn);
					break;
				default:
					iBtClient->SendMessageL(KCmdLightOff);
					break;
				}
			}
			break;
		case EAquariumControlSwitchLightMode:
			{
			switch (iData->iLightMode)
				{
				case (TAquariumDeviceMode) EAuto:
					if (iData->iLightState == EOn)
						iBtClient->SendMessageL(KCmdLightOn);
					else
						iBtClient->SendMessageL(KCmdLightOff);
					break;
				case (TAquariumDeviceMode) EManual:
					iBtClient->SendMessageL(KCmdLightAuto);
					break;
				default:
					iBtClient->SendMessageL(KCmdLightAuto);
					break;
				}
			}
			break;
		case EAquariumControlSetLightOnTime:
			CommandSetLightOnTime();
			break;
		case EAquariumControlSetLightOffTime:
			CommandSetLightOffTime();
			break;
		case EAquariumControlSetLightLevel:
			CommandSetLightLevel();
			break;
		case EAquariumControlSetLightRise:
			CommandSetLightRise();
			break;
		case EAquariumControlSetLightStateOn:
			iBtClient->SendMessageL(KCmdLightOn);
			break;
		case EAquariumControlSetLightStateOff:
			iBtClient->SendMessageL(KCmdLightOff);
			break;
		case EAquariumControlSetLightModeAuto:
			iBtClient->SendMessageL(KCmdLightAuto);
			break;

		case EAquariumControlWaterTemp:
			// dummy
			break;
		case EAquariumControlSwitchHeatState:
			{
			switch (iData->iHeatState)
				{
				case (TAquariumDeviceState) EOn:
					iBtClient->SendMessageL(KCmdHeatOff);
					break;
				case (TAquariumDeviceState) EOff:
					iBtClient->SendMessageL(KCmdHeatOn);
					break;
				default:
					iBtClient->SendMessageL(KCmdHeatOff);
					break;
				}
			}
			break;
		case EAquariumControlSwitchHeatMode:
			{
			switch (iData->iHeatMode)
				{
				case (TAquariumDeviceMode) EAuto:
					if (iData->iHeatState == EOn)
						iBtClient->SendMessageL(KCmdHeatOn);
					else
						iBtClient->SendMessageL(KCmdHeatOff);
					break;
				case (TAquariumDeviceMode) EManual:
					iBtClient->SendMessageL(KCmdHeatAuto);
					break;
				default:
					iBtClient->SendMessageL(KCmdHeatAuto);
					break;
				}
			}
			break;
		case EAquariumControlSetHeatLow:
			CommandSetHeatLow();
			break;
		case EAquariumControlSetHeatHigh:
			CommandSetHeatHigh();
			break;
		case EAquariumControlSetHeatStateOn:
			iBtClient->SendMessageL(KCmdHeatOn);
			break;
		case EAquariumControlSetHeatStateOff:
			iBtClient->SendMessageL(KCmdHeatOff);
			break;
		case EAquariumControlSetHeatModeAuto:
			iBtClient->SendMessageL(KCmdHeatAuto);
			break;

		case EAquariumControlSwitchDisplayMode:
			{
			switch (iData->iDisplayMode)
				{
				case (TAquariumDispalyMode) ETime:
					iBtClient->SendMessageL(KCmdDisplayTemp);
					break;
				case (TAquariumDispalyMode) ETemperature:
					iBtClient->SendMessageL(KCmdDisplayTime);
					break;
				default:
					iBtClient->SendMessageL(KCmdDisplayTime);
					break;
				}
			}
			break;

		case EAbout:
			{
			CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
			dlg->ExecuteLD(R_ABOUT_QUERY_DIALOG);
			}
			break;

		default:
			Panic(EAquariumControlViewAppUi);
			break;
		}
	ResumeUpdating();
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
void CAquariumControlViewAppUi::CommandSetTime()
	{
	_LIT(KCmdFormat, "time %02u:%02u:%02u\r");
	TBuf<14> cmd;
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
			cmd.Format(KCmdFormat,
					time.DateTime().Hour(),
					time.DateTime().Minute(),
					time.DateTime().Second());
			iBtClient->SendMessageL(cmd);
			}
		else if (variant == 1)
			{
			_LIT(KTimeStrFormat, ":%02u%02u%02u.");
			TBuf<8> timeStr;
			timeStr.Format(KTimeStrFormat,
					iData->iHour,
					iData->iMinute,
					iData->iSecond);
			time.Set(timeStr);
			CAknTimeQueryDialog* dlg = new (ELeave) CAknTimeQueryDialog(time);
			answer = dlg->ExecuteLD(R_SETTIME_QUERY_DIALOG);
			if (EAknSoftkeyOk == answer)
				{
				cmd.Format(KCmdFormat,
						time.DateTime().Hour(),
						time.DateTime().Minute(),
						time.DateTime().Second());
				iBtClient->SendMessageL(cmd);
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetTimeCor()
// SetTimeCor command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetTimeCor()
	{
	_LIT(KCmdFormatP, "time +%02i\r");
	_LIT(KCmdFormatN, "time -%02i\r");
	TBuf<9> cmd;
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
		if (timeCorr < 0)
			cmd.Format(KCmdFormatN, -timeCorr);
		else
			cmd.Format(KCmdFormatP, timeCorr);
		iBtClient->SendMessageL(cmd);
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetDate()
// SetDate command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetDate()
	{
	_LIT(KCmdFormat, "date %02u.%02u.%02u %1u\r");
	TBuf<16> cmd;
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
			cmd.Format(KCmdFormat,
					date.DateTime().Day() + 1,
					date.DateTime().Month() + 1,
					date.DateTime().Year() - 2000,
					iData->iDayOfWeek);
			iBtClient->SendMessageL(cmd);
			}
		else if (variant == 1)
			{
			_LIT(KTimeStrFormat, "20%02u%02u%02u:.");
			TBuf<10> dateStr;
			dateStr.Format(KTimeStrFormat,
					iData->iYear,
					iData->iMonth - 1,
					iData->iDay - 1);
			date.Set(dateStr);
			CAknTimeQueryDialog* dlg = new (ELeave) CAknTimeQueryDialog(date);
			answer = dlg->ExecuteLD(R_SETDATE_QUERY_DIALOG);
			if (EAknSoftkeyOk == answer)
				{
				cmd.Format(KCmdFormat,
						date.DateTime().Day() + 1,
						date.DateTime().Month() + 1,
						date.DateTime().Year() - 2000,
						iData->iDayOfWeek);
				iBtClient->SendMessageL(cmd);
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetDayOfWeek()
// SetDayOfWeek command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetDayOfWeek()
	{
	_LIT(KCmdFormat, "date %02u.%02u.%02u %1u\r");
	TBuf<16> cmd;
	TInt dayOfWeek;
	TInt answer;
	CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&dayOfWeek);
	answer = dlg->ExecuteLD(R_SETDAYOFWEEK_QUERY_DIALOG);
	if (EAknSoftkeyOk == answer)
		{
		cmd.Format(KCmdFormat,
				iData->iDay,
				iData->iMonth,
				iData->iYear,
				dayOfWeek + 1);
		iBtClient->SendMessageL(cmd);
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightOnTime()
// SetLightOnTime command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetLightOnTime()
	{
	_LIT(KTimeStrFormat, ":%02u%02u%02u.");
	_LIT(KMinTimeStr, ":000000.");
	_LIT(KCmdFormat, "light %02u:%02u:%02u-%02u:%02u:%02u\r");
	TBuf<24> cmd;
	TInt answer;
	TTime timeOn;
	TTime minTime, maxTime;
	TBuf<8> timeOnStr;
	TBuf<8> maxTimeStr;
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_ON_TIME);
	timeOnStr.Format(KTimeStrFormat,
			iData->iLightOnHour,
			iData->iLightOnMinute,
			iData->iLightOnSecond);
	maxTimeStr.Format(KTimeStrFormat,
			iData->iLightOffHour,
			iData->iLightOffMinute,
			iData->iLightOffSecond);
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
		cmd.Format(KCmdFormat,
				timeOn.DateTime().Hour(),
				timeOn.DateTime().Minute(),
				timeOn.DateTime().Second(),
				iData->iLightOffHour,
				iData->iLightOffMinute,
				iData->iLightOffSecond);
		iBtClient->SendMessageL(cmd);
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightOffTime()
// SetLightOffTime command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetLightOffTime()
	{
	_LIT(KTimeStrFormat, ":%02u%02u%02u.");
	_LIT(KMaxTimeStr, ":235959.");
	_LIT(KCmdFormat, "light %02u:%02u:%02u-%02u:%02u:%02u\r");
	TBuf<24> cmd;
	TInt answer;
	TTime timeOff;
	TTime minTime, maxTime;
	TBuf<8> timeOffStr;
	TBuf<8> minTimeStr;
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_LISTBOX_ITEM_LIGHT_OFF_TIME);
	timeOffStr.Format(KTimeStrFormat,
			iData->iLightOffHour,
			iData->iLightOffMinute,
			iData->iLightOffSecond);
	minTimeStr.Format(KTimeStrFormat,
			iData->iLightOnHour,
			iData->iLightOnMinute,
			iData->iLightOnSecond);
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
		cmd.Format(KCmdFormat,
				iData->iLightOnHour,
				iData->iLightOnMinute,
				iData->iLightOnSecond,
				timeOff.DateTime().Hour(),
				timeOff.DateTime().Minute(),
				timeOff.DateTime().Second());
		iBtClient->SendMessageL(cmd);
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightLevel()
// SetLightLevel command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetLightLevel()
	{
	_LIT(KCmdFormat, "light level %03u\r");
	TBuf<16> cmd;
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
		cmd.Format(KCmdFormat, level);
		iBtClient->SendMessageL(cmd);
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetLightRise()
// SetLightRise command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetLightRise()
	{
	_LIT(KCmdFormat, "light rise %02u\r");
	TBuf<14> cmd;
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
		cmd.Format(KCmdFormat, rise);
		iBtClient->SendMessageL(cmd);
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetHeatLow()
// SetHeatLow command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetHeatLow()
	{
	_LIT(KCmdFormat, "heat %02u-%02u\r");
	TBuf<11> cmd;
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
		cmd.Format(KCmdFormat, low, iData->iHeatHigh);
		iBtClient->SendMessageL(cmd);
		}
	}

// ----------------------------------------------------------------------------
// CAquariumControlViewAppUi::CommandSetHeatHigh()
// SetHeatHigh command handler.
// ----------------------------------------------------------------------------
//
void CAquariumControlViewAppUi::CommandSetHeatHigh()
	{
	_LIT(KCmdFormat, "heat %02u-%02u\r");
	TBuf<11> cmd;
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
		cmd.Format(KCmdFormat, iData->iHeatLow, high);
		iBtClient->SendMessageL(cmd);
		}
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
void CAquariumControlViewAppUi::HandleBtNotifyL(const TDesC& aMessage, TBool aIsError)
	{
	if (aIsError)
		{
		CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
		errorNote->ExecuteLD(aMessage);
		}
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

// End of File

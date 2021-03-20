/*
 ============================================================================
 Name        : AquariumControlViewAppUi.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __AQUARIUMCONTROLVIEWAPPUI_h__
#define __AQUARIUMCONTROLVIEWAPPUI_h__

// INCLUDES
#include <e32base.h> // CPeriodic
#include <aknviewappui.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include "BtClient.h"
#include "BtClientObserver.h"

// FORWARD DECLARATIONS
class CAquariumControlData;

// CLASS DECLARATION
/**
 * CAquariumControlViewAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CAquariumControlViewAppUi : public CAknViewAppUi, MBtClientObserver
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CAquariumControlViewAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CAquariumControlViewAppUi();

	/**
	 * ~CAquariumControlViewAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CAquariumControlViewAppUi();

public:
	// Data getter.
	CAquariumControlData* AquariumData();

private:
	// Functions from base classes

	/**
	 * From CEikViewAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	* From CEikAppUi, handles key events.
	* @param aKeyEvent Event to handled.
	* @param aType Type of the key event. 
	* @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
	*/
	virtual TKeyResponse HandleKeyEventL(
		const TKeyEvent& aKeyEvent,TEventCode aType);

	void HandleForegroundEventL(TBool aForeground);

	/**
	* From CEikAppUi, handles key events.
	* @param aType The type of resources that have changed
	*/
	virtual void HandleResourceChangeL(TInt aType);

	/**
	 * From MEikMenuObserver, DynInitMenuPaneL.
	 * This function is called by the EIKON framework just before it
	 * displays a menu pane.
	 * @param aResourceId The ID of command which defined in resource file.
	 * @param aMenuPane Pointer to menu-pane object.
	 */
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:
	// New methods

	/**
	 * Update all views.
	 */
	void UpdateViewsL();

	/**
	 * Called by iTimer.
	 */
	static TInt TimerCallBack(TAny* aObject);

	/**
	 * Pause view updating.
	 */
	inline void PauseUpdating();
	inline void ResumeUpdating();

	/**
	 * Command handlers.
	 * @return EFalse if command canceled by user, ETrue otherwise.
	 */
	void CommandSetTime();
	void CommandSetTimeCor();
	void CommandSetDate();
	void CommandSetDayOfWeek();
	void CommandSetLightOnTime();
	void CommandSetLightOffTime();
	void CommandSetLightLevel();
	void CommandSetLightRise();
	void CommandSetHeatLow();
	void CommandSetHeatHigh();

	/**
	* ShowErrorNoteL.
	* Show note with error message.
	* @param aMessageResourceId Resource ID of the error message
	*/
	void ShowErrorNoteL(TInt aMessageResourceId);

	/**
	 * From MBtClientObserver
	 */
	void HandleBtDeviceChangeL(CBTDevice* aRemoteDevice);
	void HandleBtNotifyL(const TDesC& aMessage, TBool aIsError);
	void HandleBtDataL(const TDesC& aData);

private:
	// Data

	/**
	 * Tabs
	 */
	CAknTabGroup*				iTabGroup;
	CAknNavigationDecorator*	iDecoratedTabGroup;
	CAquariumControlData*		iData;

	/**
	 * Views
	 */
	TUid iClockViewId;
	TUid iLightViewId;
	TUid iHeatViewId;
	TUid iDisplayViewId;

	/**
	 * iTimer is used to update views periodically
	 */
	CPeriodic* iTimer;

	/** 
	 * iBtClient
	 * the BT engine 
	 * Owned by ViewAppUi
	 */
	CBtClient* iBtClient;

	/**
	 * iBtAvailable, ETrue if an SDP session can be opened, EFalse otherwise.
	 */
	TBool iBtAvailable;

	/**
	 * Bluetooth data is processed line by line.
	 * iBtDataTail contains the last part of text
	 * that does not end with "\r\n".
	 * This part will be prefixed to the next
	 * portion of bluetooth data.
	 */
	HBufC* iBtDataTail;

	};

inline CAquariumControlData* CAquariumControlViewAppUi::AquariumData() { return iData; }

#endif // __AQUARIUMCONTROLVIEWAPPUI_h__
// End of File

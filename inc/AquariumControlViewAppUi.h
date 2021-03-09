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
#include <aknviewappui.h>
#include <akntabgrp.h>
#include <aknnavide.h>

// FORWARD DECLARATIONS
class CDateTimeView;

// CLASS DECLARATION
/**
 * CAquariumControlViewAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CAquariumControlViewAppUi : public CAknViewAppUi
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
	virtual void HandleResourceChangeL( TInt aType );

private:
	// Data

	CAknNavigationControlContainer*	iNaviPane;
	CAknTabGroup*					iTabGroup;
	CAknNavigationDecorator*		iDecoratedTabGroup;
	TUid 							iViewDateTimeId;

	};

#endif // __AQUARIUMCONTROLVIEWAPPUI_h__
// End of File

/*
 ============================================================================
 Name        : AquariumControlAppUi.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __AQUARIUMCONTROLAPPUI_h__
#define __AQUARIUMCONTROLAPPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CAquariumControlAppView;

// CLASS DECLARATION
/**
 * CAquariumControlAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CAquariumControlAppUi : public CAknAppUi
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CAquariumControlAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CAquariumControlAppUi();

	/**
	 * ~CAquariumControlAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CAquariumControlAppUi();

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is changed.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;

private:
	// Data

	/**
	 * The application view
	 * Owned by CAquariumControlAppUi
	 */
	CAquariumControlAppView* iAppView;

	};

#endif // __AQUARIUMCONTROLAPPUI_h__
// End of File

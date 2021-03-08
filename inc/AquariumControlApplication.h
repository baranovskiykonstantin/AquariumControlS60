/*
 ============================================================================
 Name        : AquariumControlApplication.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __AQUARIUMCONTROLAPPLICATION_H__
#define __AQUARIUMCONTROLAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "AquariumControl.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidAquariumControlApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CAquariumControlApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CAquariumControlApplication is the application part of the
 * AVKON application framework for the AquariumControl example application.
 */
class CAquariumControlApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidAquariumControlApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CAquariumControlDocument document object. The returned
	 * pointer in not owned by the CAquariumControlApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __AQUARIUMCONTROLAPPLICATION_H__
// End of File

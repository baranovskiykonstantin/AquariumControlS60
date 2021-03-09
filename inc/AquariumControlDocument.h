/*
 ============================================================================
 Name        : AquariumControlDocument.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __AQUARIUMCONTROLDOCUMENT_h__
#define __AQUARIUMCONTROLDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CAquariumControlViewAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CAquariumControlDocument application class.
 * An instance of class CAquariumControlDocument is the Document part of the
 * AVKON application framework for the AquariumControl example application.
 */
class CAquariumControlDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CAquariumControlDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CAquariumControlDocument.
	 */
	static CAquariumControlDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CAquariumControlDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CAquariumControlDocument.
	 */
	static CAquariumControlDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CAquariumControlDocument
	 * Virtual Destructor.
	 */
	virtual ~CAquariumControlDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CAquariumControlAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CAquariumControlDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CAquariumControlDocument(CEikApplication& aApp);

	};

#endif // __AQUARIUMCONTROLDOCUMENT_h__
// End of File

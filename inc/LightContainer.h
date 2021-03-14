/*
 ============================================================================
 Name        : LightContainer.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares the light view's controls class for application.
 ============================================================================
 */

#ifndef __LIGHTCONTAINER_H__
#define __LIGHTCONTAINER_H__

// INCLUDES
#include "BaseContainer.h"

// CLASS DECLARATION
class CLightContainer : public CBaseContainer
	{

public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CLightContainer object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CLightContainer.
	 */
	static CLightContainer* NewL(const TRect& aRect);

private:
	
	/**
	 * CClcokContainer.
	 * C++ default constructor.
	 */
	CLightContainer();

public:

	/**
	 * From CBaseContainer, UpdateListBox.
	 * Update listbox items.
	 */
	virtual void UpdateListBoxL();

protected:

	/**
	 * From CBaseContainer, HandleListBoxItemActivationL.
	 * Process listbox item activation by its positional number.
	 * @param aItem The activated item number (starting from 0).
	 */
	virtual void HandleListBoxItemActivationL(TInt aItem);

	};

#endif // __LIGHTCONTAINER_H__
// End of File

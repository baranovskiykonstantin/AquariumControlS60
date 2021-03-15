/*
 ============================================================================
 Name        : DisplayContainer.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares the display view's controls class for application.
 ============================================================================
 */

#ifndef __DISPLAYCONTAINER_H__
#define __DISPLAYCONTAINER_H__

// INCLUDES
#include "BaseContainer.h"

// CLASS DECLARATION
class CDisplayContainer : public CBaseContainer
	{

public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CDisplayContainer object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CDisplayContainer.
	 */
	static CDisplayContainer* NewL(const TRect& aRect);

private:
	
	/**
	 * CDisplayContainer.
	 * C++ default constructor.
	 */
	CDisplayContainer();

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

#endif // __DISPLAYCONTAINER_H__
// End of File

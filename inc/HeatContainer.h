/*
 ============================================================================
 Name        : HeatContainer.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares the heat view's controls class for application.
 ============================================================================
 */

#ifndef __HEATCONTAINER_H__
#define __HEATCONTAINER_H__

// INCLUDES
#include "BaseContainer.h"

// CLASS DECLARATION
class CHeatContainer : public CBaseContainer
	{

public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CHeatContainer object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CHeatContainer.
	 */
	static CHeatContainer* NewL(const TRect& aRect);

private:
	
	/**
	 * CHeatContainer.
	 * C++ default constructor.
	 */
	CHeatContainer();

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

#endif // __HEATCONTAINER_H__
// End of File

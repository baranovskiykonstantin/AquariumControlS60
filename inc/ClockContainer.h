/*
 ============================================================================
 Name        : ClockContainer.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares the clock view's controls class for application.
 ============================================================================
 */

#ifndef __CLOCKCONTAINER_H__
#define __CLOCKCONTAINER_H__

// INCLUDES
#include "BaseContainer.h"

// CLASS DECLARATION
class CClockContainer : public CBaseContainer
	{

public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CClockContainer object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CClockContainer.
	 */
	static CClockContainer* NewL(const TRect& aRect);

private:
	
	/**
	 * CClcokContainer.
	 * C++ default constructor.
	 */
	CClockContainer();

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

#endif // __CLOCKCONTAINER_H__
// End of File

/*
 ============================================================================
 Name        : BaseContainer.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares the base class for containers.
 ============================================================================
 */

#ifndef __BASECONTAINER_H__
#define __BASECONTAINER_H__

// INCLUDES
#include <coecntrl.h>
#include <badesca.h>
#include <eiklbo.h> 
#include <aknlists.h> 

// CLASS DECLARATION
class CBaseContainer : public CCoeControl, MEikListBoxObserver 
	{
public:
	// New virtual methods.
	// Must be implemented by inheritor.

	/**
	 * UpdateListBox.
	 * Update listbox items.
	 */
	virtual void UpdateListBoxL() = 0;

protected:
	// New virtual methods.
	// Must be implemented by inheritor.

	/**
	 * HandleListBoxItemActivationL.
	 * Process listbox item activation by its positional number.
	 * @param aItem The activated item number (starting from 0).
	 */
	virtual void HandleListBoxItemActivationL(TInt aItem) = 0;

public:
	// Destructor

	/**
	 * ~CBaseContainer
	 * Virtual Destructor.
	 */
	virtual ~CBaseContainer();

public:
	// Functions from base classes

	/**
	 * From CCoeControl, CountComponentControls
	 * Gets the number of controls contained in a compound control.
	 * @return The number of component controls contained by this control.
	 */
	TInt CountComponentControls() const;

	/**
	 * From CCoeControl, ComponentControl
	 * Gets an indexed component of a compound control.
	 * @param aIndex The index of the control.
	 * @return The component control with an index of aIndex.
	 */
	CCoeControl *ComponentControl(TInt aIndex) const;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

	/**
	 * From CoeControl, HandlePointerEventL.
	 * Called by framework when a pointer touch event occurs.
	 * Note: although this method is compatible with earlier SDKs, 
	 * it will not be called in SDKs without Touch support.
	 * @param aPointerEvent the information about this event
	 */
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

protected:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CBaseContainer object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

	/**
	 * CBaseContainer.
	 * C++ default constructor.
	 */
	CBaseContainer();

	/**
	 * From CoeControl, OfferKeyEventL.
	 * Handles key events.
	 * @param aKeyEvent The key event.
	 * @param aType The type of key event: EEventKey, EEventKeyUp or
	 *              EEventKeyDown
	 */
	TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

	/**
	 * From MEikListBoxObserver
	 */
	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

protected: // Data

	/**
	* Pointer to listbox control.
	*
	* Owned.
	*/
	CAknDoubleStyleListBox* iListBox;
	
	/**
	* Pointer to listbox items.
	*
	* Owned by iListBox.
	*/
	CDesCArrayFlat* iListBoxItems;

	};

#endif // __BASECONTAINER_H__
// End of File

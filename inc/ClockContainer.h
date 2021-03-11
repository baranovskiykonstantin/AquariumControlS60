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
#include <coecntrl.h>
#include <badesca.h>
#include <eiklbo.h> 

// FORWARD DECLARATIONS
class CAknDoubleStyleListBox;

// CLASS DECLARATION
class CClockContainer : public CCoeControl, MEikListBoxObserver 
	{
public:
	// New virtual methods.
	// Must be implemented by inheritor.

	/**
	 * UpdateListBox.
	 * Update listbox items.
	 */
	virtual void UpdateListBoxL();

protected:
	// New virtual methods.
	// Must be implemented by inheritor.

	/**
	 * HandleListBoxItemActivationL.
	 * Process listbox item activation by its positional number.
	 * @param aItem The activated item number (starting from 0).
	 */
	virtual void HandleListBoxItemActivationL(TInt aItem);

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

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CClockContainer object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CClockContainer.
	 */
	static CClockContainer* NewLC(const TRect& aRect);

	/**
	 * ~CClockContainer
	 * Virtual Destructor.
	 */
	virtual ~CClockContainer();

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

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CClockContainer object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

	/**
	 * CClockContainer.
	 * C++ default constructor.
	 */
	CClockContainer();

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

private: // Data

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

#endif // __CLOCKCONTAINER_H__
// End of File

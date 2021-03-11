/*
 ============================================================================
 Name        : ClockView.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares the clock view class for application.
 ============================================================================
 */

#ifndef __CLOCKVIEW_H__
#define __CLOCKVIEW_H__

// INCLUDES
#include <aknview.h>
#include <akntabgrp.h>        // For handling tabs

// CONSTANTS
// UID of view
const TUid KViewId = {1};

// FORWARD DECLARATIONS
class CClockContainer;

// CLASS DECLARATION

/**
 *  CClockView
 * 
 */
class CClockView : public CAknView
	{
public: // Constructors and destructor

	/**
	* EPOC default constructor.
	*/
	void ConstructL(CAknTabGroup*);

	/**
	* Destructor.
	*/
	~CClockView();

public: // Functions from base classes
	
	/**
	* Return Uid
	*/
	TUid Id() const;

	/**
	* Handle Commands
	*/
	void HandleCommandL(TInt aCommand);

	/**
	* Handle Size changes
	*/
	void HandleClientRectChange();

private:

	/**
	* From AknView, Activates view
	*/
	void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
		const TDesC8& aCustomMessage);

	/**
	* From AknView, Deactivates view
	*/
	void DoDeactivate();
	

private: // Data
	CClockContainer* iContainer;
	CAknTabGroup* iTabGroup;

	};

#endif // __CLOCKVIEW_H__

/*
 ============================================================================
 Name        : AquariumControlView.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares the view class for application.
 ============================================================================
 */

#ifndef __AQUARIUMCONTROLVIEW_H__
#define __AQUARIUMCONTROLVIEW_H__

// INCLUDES
#include <aknview.h>
#include <akntabgrp.h>        // For handling tabs
#include <akntoolbarobserver.h>
#include "AquariumControl.hrh"

// FORWARD DECLARATIONS
class CBaseContainer;

// CLASS DECLARATION

/**
 *  CAquariumControlView
 * 
 */
class CAquariumControlView : public CAknView, MAknToolbarObserver
	{
public: // Constructors and destructor

	/**
	* EPOC default constructor.
	*/
	void ConstructL(CAknTabGroup* aTabGroup, TAquariumControlViewTabIds aTabId);

	/**
	* Destructor.
	*/
	~CAquariumControlView();

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

public:
	// New methods
	
	/**
	 * Update view accordingly to data change
	 */
	void UpdateL();

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

	/**
	 * From MEikMenuObserver, DynInitMenuPaneL.
	 * This function is called by the EIKON framework just before it
	 * displays a menu pane.
	 * @param aResourceId The ID of command which defined in resource file.
	 * @param aMenuPane Pointer to menu-pane object.
	 */
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	/**
	 * From MAknToolbarObserver, OfferToolbarEventL
	 * Handles toolbar events for a certain toolbar item.
	 * @param aCommand The command ID of some toolbar item.
	 */
	void OfferToolbarEventL(TInt aCommand);

private: // Data
	TInt iId;
	CBaseContainer* iContainer; // owned
	CAknTabGroup* iTabGroup; // not owned

	};

#endif // __AQUARIUMCONTROLVIEW_H__

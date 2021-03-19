/*
 ============================================================================
 Name        : BtClientObserver.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Bluetooth client observer.
 ============================================================================
 */

#ifndef __BTCLIENTOBSERVER_H__
#define __BTCLIENTOBSERVER_H__

#include <btdevice.h>

/**
 * MBtClientObserver mixin.
 * All information from BT client is sent
 * to observer using this interface.
 */
class MBtClientObserver
	{

public:

	/**
	 * HandleBtDeviceChangeL()
	 * BT device connection/disconnection notify.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aRemoteDevice Contains the name and address of connected device.
	 */
	virtual void HandleBtDeviceChangeL(CBTDevice* aRemoteDevice) = 0;

	/**
	 * HandleBtNotifyL()
	 * Notify/error message from BT client.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aMessage Message from BT client.
	 */
	virtual void HandleBtNotifyL(const TDesC& aMessage) = 0;

	/**
	 * HandleBtDataL()
	 * Received data from remote BT device.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aData Received BT data.
	 */
	virtual void HandleBtDataL(const TDesC& aData) = 0;

	};

#endif /* __BTCLIENTOBSERVER_H__ */

// End of File

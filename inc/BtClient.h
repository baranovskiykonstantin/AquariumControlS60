/*
 ============================================================================
 Name        : BtClient.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Bluetooth client.
 ============================================================================
 */

#ifndef __BTCLIENT_H__
#define __BTCLIENT_H__

#include <es_sock.h>
#include <bt_sock.h>
#include <btextnotifiers.h>
#include <btsdp.h>
#include <btdevice.h>
#include "BtServiceSearcher.h"
#include "BtClientObserver.h"
#include "BtClientConstants.h"
#include "BatteryStatus.h"

/**
* TBtClientState
* The state of the active object, determines behaviour within
* the RunL method.
* EWaitingToGetDevice waiting for the user to select a device
* EGettingDevice searching for a device
* EGettingService searching for a service
* EGettingConnection connecting to a service on a remote machine
* EConnected connected to a service on a remote machine
* ESendingMessage sending a message to the remote machine
* EWaitingForMessage wainting for message from remote machine
* EDisconnecting disconnecting from remote machine
*/

enum TBtClientState
	{
	EWaitingToGetDevice,
	EGettingDevice,
	EGettingService,
	EGettingConnection,
	EConnected,
	EDisconnected,
	ESendingMessage,
	EWaitingForMessage,
	EDisconnecting
	};

/**
* CBtClient
* Connects and sends messages to a remote machine using bluetooth
*/
class CBtClient :
	public CActive,
	public MBatteryStatusObserver
	{

public: // Constructors and destructor

	/**
	* NewL()
	* Construct a CBtClient
	* @param aLog the log to send output to
	* @return a pointer to the created instance of CBtClient
	*/
	static CBtClient* NewL();

	/**
	* NewLC()
	* Construct a CBtClient
	* @param aLog the log to send output to
	* @return a pointer to the created instance of CBtClient
	*/
	static CBtClient* NewLC();

	/**
	* CBtClient()
	* Constructs this object
	* @param aLog the log to send output to
	*/
	CBtClient();

	/**
	* ~CBtClient()
	* Destroy the object and release all memory objects.
	* Close any open sockets.
	*/
	virtual ~CBtClient();

public: // New functions

	/**
	* ConnectL()
	* Connect to an available service on a remote machine
	*/
	void ConnectL();

	/**
	* DisconnectL()
	* Disconnect from remote machine
	*/
	void DisconnectL();

	/**
	* SendMessageL()
	* Sends a message to a service on a remote machine.
	* @param aText text to send
	*/
	void SendMessageL(const TDesC& aText);

	/**
	* SetState()
	* Set state
	* @param aState the state of the engine
	*/
	void SetState(TBtClientState aState);

	/**
	* State()
	* Get State
	* @return state
	*/
	TInt State();

	/**
	* IsConnected()
	* @return ETrue if the client is fully connected to the server.
	*/
	TBool IsConnected();

	/**
	* IsConnecting()
	* @return ETrue if the client is establishing a
	* connection to the server.
	*/
	TBool IsConnecting();

	/**
	* IsReadyToSend()
	* @return ETrue if the client can send a message.
	*/
	TBool IsReadyToSend();

	/**
	 * SetObserver()
	 * Assing an observer to receive log messages.
	 */
	void SetObserver(MBtClientObserver* aObserver);

protected: // from CActive

	/**
	* DoCancel()
	* Cancel any outstanding requests
	*/
	void DoCancel();

	/**
	* RunL()
	* Respond to an event
	*/
	void RunL();

private: // Functions from base classes

	/**
	* ConstructL()
	* Performs second phase construction of this object
	*/
	void ConstructL();

private: // From MBatteryStatusObserver

	/**
	 * HandleBatteryStatusChangeL()
	 * Battery status change notification.
	 */
	void HandleBatteryStatusChangeL();

private: // New private functions

	/**
	* ConnectToServerL
	* Connects to the service
	*/
	void ConnectToServerL();

	/**
	* DisonnectFromServerL()
	* Disconnects from the service
	*/
	void DisconnectFromServerL();

	/**
	* RequestData()
	* Request data from the client, this object is notified
	* when data becomes available
	*/
	void RequestData();

	/**
	* SetSecurityWithChannelL
	* Sets the security on the channel port
	* And returns the channel available.
	* @param aAuthentication is authentication used
	* @param aEncryption is encryption used
	* @param aAuthorisation is authorisation used
	* @param aDenied is denied used
	* @return an available channel
	*/
	void SetSecurityWithChannelL(TBool aAuthentication,
		TBool aEncryption,
		TBool aAuthorisation,
		TBool aDenied,
		TInt& aChannel
	);

	/**
	* SetSecurityOnChannelL()
	* Sets the security on the channel port
	* @param aAuthentication is authentication used
	* @param aEncryption is encryption used
	* @param aAuthorisation is authorisation used
	* @param aChannel used channel ID
	*/
	static void SetSecurityOnChannelL(TBool aAuthentication,
		TBool aEncryption,
		TBool aAuthorisation,
		TInt aChannel
	);

	/**
	 * NotifyL()
	 * Send log message to observer.
	 */
	void NotifyL(const TDesC& aMessage);

	/**
	 * NotifyDeviceIsConnectedL()
	 * Send to observer information about connected BT device.
	 */
	void NotifyDeviceIsConnectedL();

	/**
	 * NotifyDeviceIsDisconnectedL()
	 * Send to observer BT device disconnect notify.
	 */
	void NotifyDeviceIsDisconnectedL();

	/**
	 * AllowLowPowerModes()
	 * Enable low power modes to save the battery.
	 */
	void AllowLowPowerModes();

	/**
	 * PreventLowPowerModes()
	 * Disable low power modes to reach max bandwidth.
	 */
	void PreventLowPowerModes();

private: // Data

	/**
	* iState the current state of the client
	*/
	TBtClientState iState;

	/**
	* iServiceSearcher searches for service this
	* client can connect to.
	* Owned by CBtClient
	*/
	CBtServiceSearcher* iServiceSearcher;

	/**
	* iMessage a copy of the message to send
	* Owned by CBtClient
	*/
	HBufC8* iMessage;

	/**
	* iSocketServer
	* a connection to the socket server
	*/
	RSocketServ iSocketServer;

	/**
	* iSocket
	* a socket to connect with
	*/
	RSocket iSocket;

	/**
	* iServiceClass
	* the service class UUID to search for
	*/
	TUUID iServiceClass;

	/**
	* iBuffer
	* the buffer to read data to
	*/
	TBuf8<KBtTextBufLength> iBuffer;

	/**
	* iAcceptedSocket
	* the socket that connections are accepted to
	*/
	RSocket iAcceptedSocket;

	/**
	* iLen
	* length of data read
	*/
	TSockXfrLength iLen;

	/**
	* iActiveSocket
	* the active socket
	*/
	RSocket* iActiveSocket;

	/**
	 * iRemoteDevice
	 * information about remote BT device
	 */
	CBTDevice* iRemoteDevice;

	/**
	* iObserver the handler of log messages
	*/
	MBtClientObserver* iObserver;

	/**
	 * iBatteryStatus
	 * the battery status checker
	 */
	CBatteryStatus* iBatteryStatus;

	/**
	 * iBTPhysicalLinkAdapter
	 * enable/disable low power modes.
	 */
	RBTPhysicalLinkAdapter iBTPhysicalLinkAdapter;

	};

#endif /* __BTCLIENT_H__ */

// End of File

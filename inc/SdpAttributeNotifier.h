/*
 ============================================================================
 Name        : SdpAttributeNotifier.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Read selected SDP Attributes.
 ============================================================================
 */

#ifndef __SDPATTRIBUTENOTIFIER_H__
#define __SDPATTRIBUTENOTIFIER_H__

class CSdpAttrValue;

/**
* MSdpAttributeNotifier
* An instance of MSdpAttributeNotifier is used to read selected SDP Attribute
* elements from an attribute value.
*/
class MSdpAttributeNotifier
	{

public: // New functions

	/**
	* FoundElementL
	* Read the data element
	* @param aKey a key that identifies the element
	* @param aValue the data element
	*/
	virtual void FoundElementL(TInt aKey, CSdpAttrValue& aValue) = 0;

	};

#endif // __SDPATTRIBUTENOTIFIER_H__

// End of File

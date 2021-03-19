/*
 ============================================================================
 Name        : Attributeparser.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains SDP attr panic codes.
 ============================================================================
 */

#ifndef __SDPATTRIBUTEPARSER_PAN__
#define __SDPATTRIBUTEPARSER_PAN__

#include <e32std.h>

/** Panic Category */
_LIT(KPanicSAP, "SAP");

/** SDP attr panic codes */
enum TSdpAttributeParserPanics
	{
	ESdpAttributeParserInvalidCommand = 1,
	ESdpAttributeParserNoValue,
	ESdpAttributeParserValueIsList,
	ESdpAttributeParserValueTypeUnsupported
	};

inline void Panic(TSdpAttributeParserPanics aReason)
	{
	User::Panic(KPanicSAP, aReason);
	}

#endif // __SDPATTRIBUTEPARSER_PAN__

// End of File

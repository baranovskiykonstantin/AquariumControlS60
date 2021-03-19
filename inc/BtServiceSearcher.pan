/*
 ============================================================================
 Name        : BtServiceSearcher.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains service searcher panic codes.
 ============================================================================
 */

#ifndef __BTSERVICESEARCHER_PAN__
#define __BTSERVICESEARCHER_PAN__

#include <e32std.h>

/** BtServiceSearcher application panic codes */
enum TBtServiceSearcherPanics 
	{
	EBtServiceSearcherNextRecordRequestComplete = 1,
	EBtServiceSearcherAttributeRequestResult,
	EBtServiceSearcherAttributeRequestComplete,
	EBtServiceSearcherProtocolRead
	};


/** Panic Category */
_LIT(KPanicServiceSearcher, "ServiceSearcher");


inline void Panic(TBtServiceSearcherPanics aReason)
	{
	User::Panic(KPanicServiceSearcher, aReason);
	}

#endif // __BTSERVICESEARCHER_PAN__

// End of File

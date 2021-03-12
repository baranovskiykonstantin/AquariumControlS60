/*
 ============================================================================
 Name        : AquariumControlData.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : CAquariumControlData implementation
 ============================================================================
 */

#include "AquariumControlData.h"

CAquariumControlData::CAquariumControlData() :
		iIsConnected(EFalse),
		iDayOfWeek(1)
	{
	}

CAquariumControlData::~CAquariumControlData()
	{
	}

CAquariumControlData* CAquariumControlData::NewLC()
	{
	CAquariumControlData* self = new (ELeave) CAquariumControlData();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CAquariumControlData* CAquariumControlData::NewL()
	{
	CAquariumControlData* self = CAquariumControlData::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CAquariumControlData::ConstructL()
	{

	}

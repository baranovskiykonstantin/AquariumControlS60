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
		iDayOfWeek(1),
		iTemp(KUnknownTemp)
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

void CAquariumControlData::ParseLineL(const TDesC8& aLine)
	{
	// Line IDs
	_LIT8(KDate, "Date:");
	_LIT8(KTime, "Time:");
	_LIT8(KTemp, "Temp:");
	_LIT8(KHeat, "Heat:");
	_LIT8(KLight, "Light:");
	_LIT8(KDisplay, "Display:");
	// State
	_LIT8(KStateOn, "ON");
	_LIT8(KStateOff, "OFF");
	// Mode
	_LIT8(KModeAuto, "auto");
	_LIT8(KModeManual, "manual");

	TLex8 intParser;
	TLex8 parser(aLine);
	parser.Mark();
	parser.SkipCharacters();

	if (parser.MarkedToken() == KDate)
		{
		// Day of week
		_LIT8(KMonday, "Monday");
		_LIT8(KTuesday, "Tuesday");
		_LIT8(KWednesday, "Wednesday");
		_LIT8(KThursday, "Thursday");
		_LIT8(KFriday, "Friday");
		_LIT8(KSaturday, "Saturday");
		_LIT8(KSunday, "Sunday");
		// Date format
		_LIT8(KDateFormat, "??.??.20??");

		TUint day, month, year, dayOfWeek;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (KErrNotFound == parser.MarkedToken().Match(KDateFormat))
			return;
		parser.UnGetToMark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(day);
		parser.Inc(); // skip a point
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(month);
		parser.Inc(3); // skip ".20"
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(year);

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (parser.MarkedToken() == KMonday)
			dayOfWeek = 1;
		else if (parser.MarkedToken() == KTuesday)
			dayOfWeek = 2;
		else if (parser.MarkedToken() == KWednesday)
			dayOfWeek = 3;
		else if (parser.MarkedToken() == KThursday)
			dayOfWeek = 4;
		else if (parser.MarkedToken() == KFriday)
			dayOfWeek = 5;
		else if (parser.MarkedToken() == KSaturday)
			dayOfWeek = 6;
		else if (parser.MarkedToken() == KSunday)
			dayOfWeek = 7;
		else
			return;

		// Accept values only if all line
		// has been parsed successfully.
		iDay = day;
		iMonth = month;
		iYear = year;
		iDayOfWeek = dayOfWeek;
		}
	else if (parser.MarkedToken() == KTime)
		{
		// Time format
		_LIT8(KTimeFormat, "??:??:??");

		TUint hour, min, sec;
		TInt corr;
		TBool corrIsNegative;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (KErrNotFound == parser.MarkedToken().Match(KTimeFormat))
			return;
		parser.UnGetToMark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
intParser.Val(hour);
		parser.Inc(); // skip a colon
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(min);
		parser.Inc(); // skip a colon
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(sec);

		parser.SkipSpace();
		if (parser.Get() != '(')
			return;
		if (parser.Peek() == '+')
			corrIsNegative = EFalse;
		else if (parser.Peek() == '-')
			corrIsNegative = ETrue;
		else
			return;
		parser.Inc();
		parser.Mark();
		while (!parser.Eos() && parser.Peek().IsDigit())
			parser.Inc();
		if (!parser.MarkedToken().Length())
			return;
		intParser.Assign(parser.MarkedToken());
		intParser.Val(corr);
		if (corrIsNegative)
			corr *= -1;

		// Accept values only if all line
		// has been parsed successfully.
		iHours = hour;
		iMinutes = min;
		iSeconds = sec;
		iTimeCorrection = corr;
		}
	else if (parser.MarkedToken() == KTemp)
		{
		TInt temp;
		TBool tempIsNegative(EFalse);

		parser.SkipSpace();
		if (parser.Eos())
			return;
		if (parser.Peek() == '-')
			{
			tempIsNegative = ETrue;
			parser.Inc();
			}
		if (parser.Eos())
			return;
		if (parser.Peek() == '-')
			{
			// second minus -> temperature is unknown
			temp = KUnknownTemp;
			}
		else
			{
			parser.Mark();
			while (!parser.Eos() && parser.Peek().IsDigit())
				parser.Inc();
			if (!parser.MarkedToken().Length())
				return;
			intParser.Assign(parser.MarkedToken());
			intParser.Val(temp);
			if (tempIsNegative)
				temp *= -1;
			}

		// Accept values only if all line
		// has been parsed successfully.
		iTemp = temp;
		}
	else if (parser.MarkedToken() == KHeat)
		{
		// Heater thresholds format
		// "?-" and "?)" brings some strange behavior.
		// Escaping the question marks with slash solves the issue.
		_LIT8(KHeatFormat, "(?\?-?\?)");

		TAquariumDeviceState state;
		TAquariumDeviceMode mode;
		TUint low, high;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (parser.MarkedToken() == KStateOff)
			state = EOff;
		else if (parser.MarkedToken() == KStateOn)
			state = EOn;
		else
			return;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (parser.MarkedToken() == KModeAuto)
			mode = EAuto;
		else if (parser.MarkedToken() == KModeManual)
			mode = EManual;
		else
			return;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (KErrNotFound == parser.MarkedToken().Match(KHeatFormat))
			return;
		parser.UnGetToMark();
		parser.Inc(); // skip '('
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(low);
		parser.Inc(); // skip '-'
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(high);

		// Accept values only if all line
		// has been parsed successfully.
		iHeatState = state;
		iHeatMode = mode;
		iHeatLow = low;
		iHeatHigh = high;
		}
	else if (parser.MarkedToken() == KLight)
		{
		// Light thresholds format
		// "?-" and "?)" brings some strange behavior.
		// Escaping the question marks with slash solves the issue.
		_LIT8(KLightFormat, "(??:??:?\?-??:??:?\?)");

		TAquariumDeviceState state;
		TAquariumDeviceMode mode;
		TUint hourOn, minOn, secOn;
		TUint hourOff, minOff, secOff;
		TUint curLevel, topLevel, rise;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (parser.MarkedToken() == KStateOff)
			state = EOff;
		else if (parser.MarkedToken() == KStateOn)
			state = EOn;
		else
			return;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (parser.MarkedToken() == KModeAuto)
			mode = EAuto;
		else if (parser.MarkedToken() == KModeManual)
			mode = EManual;
		else
			return;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (KErrNotFound == parser.MarkedToken().Match(KLightFormat))
			return;
		parser.UnGetToMark();
		parser.Inc(); // skip '('
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(hourOn);
		parser.Inc(); // skip ':'
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(minOn);
		parser.Inc(); // skip ':'
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(secOn);
		parser.Inc(); // skip '-'
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(hourOff);
		parser.Inc(); // skip ':'
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(minOff);
		parser.Inc(); // skip ':'
		parser.Mark();
		parser.Inc(2);
		intParser.Assign(parser.MarkedToken());
		intParser.Val(secOff);
		parser.Inc(); // skip ')'

		parser.SkipSpace();
		if (parser.Eos())
			return;
		parser.Mark();
		while (!parser.Eos() && parser.Peek().IsDigit())
			parser.Inc();
		if (!parser.MarkedToken().Length())
			return;
		intParser.Assign(parser.MarkedToken());
		intParser.Val(curLevel);
		if (parser.Eos())
			return;
		if (parser.Get() != '/')
			return;
		parser.Mark();
		while (!parser.Eos() && parser.Peek().IsDigit())
			parser.Inc();
		if (!parser.MarkedToken().Length())
			return;
		intParser.Assign(parser.MarkedToken());
		intParser.Val(topLevel);
		if (parser.Eos())
			return;
		if (parser.Get() != '%')
			return;

		parser.SkipSpace();
		if (parser.Eos())
			return;
		parser.Mark();
		while (!parser.Eos() && parser.Peek().IsDigit())
			parser.Inc();
		if (!parser.MarkedToken().Length())
			return;
		intParser.Assign(parser.MarkedToken());
		intParser.Val(rise);

		// Accept values only if all line
		// has been parsed successfully.
		iLightState = state;
		iLightMode = mode;
		iLightOnHours = hourOn;
		iLightOnMinutes = minOn;
		iLightOnSeconds = secOn;
		iLightOffHours = hourOff;
		iLightOffMinutes = minOff;
		iLightOffSeconds = secOff;
		iLightCurrentLevel = curLevel;
		iLightLevel = topLevel;
		iLightRise = rise;
		}
	else if (parser.MarkedToken() == KDisplay)
		{
		// Display mode
		_LIT8(KDisplayTime, "time");
		_LIT8(KDisplayTemp, "temp");

		TAquariumDispalyMode mode;

		parser.SkipSpaceAndMark();
		parser.SkipCharacters();
		if (parser.MarkedToken() == KDisplayTime)
			mode = ETime;
		else if (parser.MarkedToken() == KDisplayTemp)
			mode = ETemperature;
		else
			return;

		// Accept values only if all line
		// has been parsed successfully.
		iDisplayMode = mode;
		}
	}


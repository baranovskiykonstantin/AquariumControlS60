/*
 ============================================================================
 Name        : RFtermConstants.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Common constants.
 ============================================================================
 */

#ifndef __RFTERMCONSTANTS_H__
#define __RFTERMCONSTANTS_H__

#include <bt_sock.h>
#include <aquariumcontrol_0xa291b5a5.rsg>
#include "AquariumControl.hrh"

const TInt KRfcommChannel = 1;
const TInt KServiceClass = 0x1101; // SerialPort

const TUid KAppUid = {_UID3};

_LIT(KStrRFCOMM, "RFCOMM");

const TInt KListeningQueSize = 1;
const TInt KRFtermTextBufLength = 128;

const TInt KRFtermTimeOut = 20000000; // 20 seconds time-out

#endif /* __RFTERMCONSTANTS_H__ */

// End of File

#ifndef _KDPRINT_H
#define _KDPRINT_H
//------------------------------------------------------------------------------------------------------------------------
#include "..\..\kdshared.h"

BOOL KDPrintInit();
void KDPrintUnInit();
void _KDPrint(IN PCHAR Str2Output);
void KDPrint(PCHAR FormatStr, ...);

//------------------------------------------------------------------------------------------------------------------------
#endif
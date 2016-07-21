#ifndef _KDPRINT_H
#define _KDPRINT_H
//------------------------------------------------------------------------------------------------------------------------
#include "..\kdshared.h"

NTSTATUS KDPrintInit(IN HANDLE hSectionMutant, IN HANDLE hSection);
VOID KDPrintUnInit();
VOID _KDPrint(IN PCHAR Str2Output);
VOID KDPrint(PCHAR FormatStr, ...);

//------------------------------------------------------------------------------------------------------------------------
#endif
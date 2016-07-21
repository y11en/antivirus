#ifndef __RUNPROC_IO
#define __RUNPROC_IO

#include "osspec.h"
#include "debug.h"

//+ ------------------------------------------------------------------------------------------

void
GlobalProcCheckInit();

void
GlobalProcCheckDone();

BOOLEAN
CheckProc_IsAllowProcessExecuting();

NTSTATUS
CheckProc_ProcessFunc(HANDLE ProcessID, ULONG Request);

void
CheckProc_RemoveFromList(HANDLE ProcessID);

#endif // __RUNPROC_IO
#ifndef __RUNPROC_IO
#define __RUNPROC_IO

#include "../debug.h"
#include "../osspec.h"

//+ ------------------------------------------------------------------------------------------

void
GlobalProcCheckInit();

void
GlobalProcCheckDone();

BOOLEAN
CheckProc_IsAllowProcessExecuting();

NTSTATUS
CheckProc_ProcessFunc(ULONG ProcessID, ULONG Request);

void
CheckProc_RemoveFromList(ULONG ProcessID);

#endif // __RUNPROC_IO
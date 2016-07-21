#ifndef __TSP_H_
#define __TSP_H_

#include "osspec.h"

BOOLEAN
TSPInit(VOID);

VOID
SetTspPAEMode(BOOLEAN bPAE);

BOOLEAN
TSPDone(VOID);

VOID
TSPProcCrash(PVOID ProcId);

VOID
TSPThreadCrash(PVOID ThId);

NTSTATUS
TSPRegister(ULONG maxSize,ORIG_HANDLE *phOrig);

NTSTATUS
TSPUnregister(ORIG_HANDLE hOrig);

NTSTATUS
TSPLock(PTSPRS_LOCK pls);

NTSTATUS
TSPThreadEnter(ORIG_HANDLE hOrig);

NTSTATUS
TSPThreadLeave(ORIG_HANDLE hOrig);

VOID
TSPSetMinShadow(ULONG MinShadow);

BOOLEAN
TSPIsAllowed(VOID);
//+ ------------------------------------------------------------------------------------------

BOOLEAN
PatchSwapC(VOID);

BOOLEAN
HookSetProcessAffinity(VOID);

BOOLEAN
TspExistRegisterdProcess(PVOID ProcId);

//+ ------------------------------------------------------------------------------------------

extern BOOLEAN TSPInitedOk;

#endif // __TSP_H_
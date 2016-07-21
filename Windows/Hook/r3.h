#ifndef _R3_HOOK_H
#define _R3_HOOK_H

#include "osspec.h"
#include "syscalls.h"

//+ ------------------------------------------------------------------------------------------

extern BOOLEAN R3Hooked;
void SetR3HookUsed(BOOLEAN bEnabled);
//+ ------------------------------------------------------------------------------------------
BOOLEAN
R3HookInit(VOID);

VOID
LoadSputniks();

#ifdef _HOOK_VXD_
	BOOLEAN
	R3Hook(VOID);
#else
	BOOLEAN __stdcall
	R3Hook(PWCHAR DllName,PVOID DllBase);
#endif

#endif // _R3_HOOK_H
#ifndef __selfprot
#define __selfprot

#include <windows.h>
#include "fssync.h"

void* __cdecl pfFSSYNC_MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag );

void __cdecl pfFSSYNC_MemFree (
	PVOID pOpaquePtr,
	void** pptr);

// -
BOOL
SelfProt_StartThread (
	PVOID pClientContext,
	_tpfProactivNotify pfnCallback,
	PVOID pUserContext
	);

void
SelfProt_StopThread (
	PVOID pClientContext
	);

#endif // __selfprot
#ifndef __HOOKSYMB_H
#define __HOOKSYMB_H

typedef struct _SYM {
	WCHAR*	wcsName;
	DWORD	dwNameLen;
	DWORD	dwHash;
	DWORD	Unknown2;
	DWORD	Unknown3;
	DWORD	Unknown4;
} SYM, *PSYM;

typedef struct _VBS_SEP {
	PVOID	IStaticEntryPoint;
	PVOID	pProc;
	DWORD	Unknown;
} VBS_SEP, *PVBS_SEP;

typedef struct _SYM_HOOK {
	WCHAR*	wcsName;
	DWORD	dwNameLen;
	DWORD	dwHash;
	PVOID	pHookProc;
	PVOID	pHookAddr;
	PVOID	pProc;
	PVOID	pSYM;
	PVOID	pSEP;
} SYM_HOOK, *PSYM_HOOK;

DWORD SetSymbolsHooks(HMODULE hModule, DWORD nHookCount, SYM_HOOK* pHooks);

#endif
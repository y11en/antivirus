#ifndef _HOOK_EXPORTS_H_
#define _HOOK_EXPORTS_H_

#include <windows.h>
#include "hookbase.h"

#define FLAG_HOOK_IMPORT   1
#define FLAG_HOOK_EXPORT   2
#define FLAG_HOOK_USE_NAME 4

HOOKINFO* HookImportInModule(HMODULE hModule, CHAR* szFuncName, VOID* HookProc, DWORD dwFlags);
HOOKINFO* HookExportInModule(HMODULE hModule, char* szFuncName, PVOID pHookFunc, DWORD dwFlags);
BOOL HookProcInProcess(CHAR* szFuncName, VOID* HookProc, DWORD dwFlags);

#endif // _HOOK_EXPORTS_H_
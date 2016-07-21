#ifndef __EXP_HOOKS_H__
#define __EXP_HOOKS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

extern DWORD   g_dwTlsIndexEventDefinition;

BOOL InitExportHooks(HMODULE hThisModule, BOOL bGUIPatchSet);
void DoneExportHooks();

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__EXP_HOOKS_H__
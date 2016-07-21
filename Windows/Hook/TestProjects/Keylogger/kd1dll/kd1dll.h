#ifndef _KD1DLL_H
#define _KD1DLL_H
//-------------------------------------------------------------

__declspec(dllimport) BOOL __cdecl SetGlobalHook(HMODULE hModule);
__declspec(dllimport) void __cdecl UnsetGlobalHook();

typedef BOOL (__cdecl *fSetGlobalHook)(HMODULE hModule);
typedef void (__cdecl *fUnsetGlobalHook)();
//-------------------------------------------------------------
#endif

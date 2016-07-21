#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

HRESULT RegisterSputnikDll(CHAR* szDllName, CHAR* szSputnikDllName);
HRESULT UnregisterSputnikDll(CHAR* szDllName, CHAR* szSputnikDllName);
HRESULT RegisterSputnikDllByHandle(CHAR* szDllName, HMODULE hSputnikDllHandle);
HRESULT UnregisterSputnikDllByHandle(CHAR* szDllName, HMODULE hSputnikDllHandle);
BOOL SputnikReloadSettings(ULONG SettingsMask, PCHAR szModuleNamePath);

#ifdef __cplusplus
} // extern "C" {
#endif

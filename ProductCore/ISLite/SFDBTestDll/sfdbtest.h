#ifndef __SFDBTESTDLL_H_
#define __SFDBTESTDLL_H_

#ifdef __cplusplus
extern "C" {
#endif

DWORD __declspec(dllexport) __cdecl Init();
DWORD __declspec(dllexport) __cdecl InitSFDB(char* szSFDBDataBase);
DWORD __declspec(dllexport) __cdecl Done();
DWORD __declspec(dllexport) __cdecl GetStatus(char* szFileName, tBYTE* pOptionsData, DWORD dwOptionsSize, DWORD* pdwData1, DWORD* pdwData2, tQWORD *pqwHash);
DWORD __declspec(dllexport) __cdecl GetStatusEx(char* szFileName, tBYTE* pOptionsData, DWORD dwOptionsSize, char** pszDate1, char** pszDate2, tQWORD* pqwHash);
DWORD __declspec(dllexport) __cdecl DeleteStatus(char* szFileName, tBYTE* pOptionsData, DWORD dwOptionsSize);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SFDBTESTDLL_H_
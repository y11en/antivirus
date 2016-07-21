#ifndef __WTH_WINTRUST_H
#define __WTH_WINTRUST_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL InitWinTrustApi();
void WTHlpFreeCertName(LPWSTR pString);
BOOL WTHlpGetCertName(HANDLE hWVTStateData, DWORD dwType, DWORD dwFlags, void* pvTypePara, LPWSTR* pString);
void WTHlpFreeStateData(HANDLE hWVTStateData);
BOOL WTHlpVerifyEmbeddedSignature(HANDLE hFile, LPCWSTR pwszSourceFile, HANDLE* phWVTStateData, LONG* lpWVTResult);
BOOL WTHlpCalcHashFromFileHandle(IN HANDLE hFile, IN OUT DWORD* hash_size, OUT BYTE* hash);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __WTH_WINTRUST_H


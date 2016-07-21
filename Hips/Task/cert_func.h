#ifndef _HIPS_CERT_FUNC_H_
#define _HIPS_CERT_FUNC_H_

//#include <windows.h>
#include "local_inc.h"
#include <wintrust.h>
#include <softpub.h>

BOOL InitWinTrustApi();

BOOL WTHlpGetCertName(HANDLE hWVTStateData, 
					 DWORD dwType,
					 DWORD dwFlags,
					 void* pvTypePara,
					 LPWSTR* pString
);
void WTHlpFreeStateData(HANDLE hWVTStateData);
BOOL WTHlpVerifyEmbeddedSignature(HANDLE hFile, LPCWSTR pwszSourceFile, HANDLE* phWVTStateData);
void WTHlpFreeCertName(LPWSTR pString);

#endif //_HIPS_CERT_FUNC_H_
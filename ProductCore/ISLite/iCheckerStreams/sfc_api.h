
/*++

Module Name:

    sfc_api.h

Abstract:

    Header file for SFC interfaces.

Author:

	Mike 02-apr-2003

Revision History:

--*/



#ifndef _SFC_API_
#define _SFC_API_

#include "sfc.h"
#include "sfc_int.h"

#ifdef __cplusplus
extern "C" {

#endif

// public SFC interfaces
extern BOOL (WINAPI *fSfcGetNextProtectedFile)(HANDLE RpcHandle,PPROTECTED_FILE_DATA pFPD);
extern BOOL (WINAPI *fSfcIsFileProtected)(IN HANDLE RpcHandle, IN LPCWSTR ProtFileName);

// new APIs which are not currently supported, but are stubbed out
extern BOOL (WINAPI *fSfpVerifyFile)(IN LPCSTR pszFileName, IN LPSTR pszError, IN DWORD dwErrSize);

// private SFC interfaces
extern DWORD (WINAPI *fSfcFileException)(HANDLE RpcHandle, LPCWSTR pwcszFileName, DWORD dwDisableActionMask);
extern VOID (WINAPI *fSfcTerminateWatcherThread)(VOID);
extern BOOL (WINAPI *fSfcInitiateScan)(HANDLE RpcHandle, DWORD dwScanMode);

BOOL InitSfcApi(VOID);
VOID DoneSfcApi(VOID);

#ifdef __cplusplus
}
#endif

#endif // _SFC_API_

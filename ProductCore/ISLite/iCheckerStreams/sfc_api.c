// sfc.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "sfc_api.h"

HANDLE g_hSfcLib = NULL;
BOOL (WINAPI *fSfcGetNextProtectedFile)(HANDLE RpcHandle,PPROTECTED_FILE_DATA pFPD) = NULL;
BOOL (WINAPI *fSfcIsFileProtected)(IN HANDLE RpcHandle, IN LPCWSTR ProtFileName) = NULL;

// new APIs which are not currently supported, but are stubbed out
BOOL (WINAPI *fSfpVerifyFile)(IN LPCSTR pszFileName, IN LPSTR pszError, IN DWORD dwErrSize) = NULL;

// private API
DWORD (WINAPI *fSfcFileException)(HANDLE RpcHandle, LPCWSTR pwcszFileName, DWORD dwDisableActionMask) = NULL;
VOID (WINAPI *fSfcTerminateWatcherThread)(VOID) = NULL;
BOOL (WINAPI *fSfcInitiateScan)(HANDLE RpcHandle, DWORD dwScanMode) = NULL;

CHAR* GetSfcLibName()
{
	int i,k=0x55;
	static CHAR szSfcLibName[8];

	// tricks to hide dll name
	szSfcLibName[0] = 's' ^ 0x55;
	szSfcLibName[1] = 'f' ^ 0x54;
	szSfcLibName[2] = 'c' ^ 0x53;
	szSfcLibName[3] = '.' ^ 0x52;
	szSfcLibName[4] = 'd' ^ 0x51;
	szSfcLibName[5] = 'l' ^ 0x50;
	szSfcLibName[6] = 'l' ^ 0x4F;
	szSfcLibName[7] = 0   ^ 0x4E;
	for (i=0; i<sizeof(szSfcLibName); i++)
		szSfcLibName[i] ^= k--;
	return szSfcLibName;
}

BOOL InitSfcApi(VOID)
{
	if (g_hSfcLib)
		return TRUE;
	g_hSfcLib = LoadLibrary(GetSfcLibName());
	if (g_hSfcLib == NULL)
		return FALSE;

	*(FARPROC*)& fSfcTerminateWatcherThread    = GetProcAddress(g_hSfcLib, (LPCSTR)2);
	*(FARPROC*)& fSfcFileException             = GetProcAddress(g_hSfcLib, (LPCSTR)5);
	*(FARPROC*)& fSfcInitiateScan              = GetProcAddress(g_hSfcLib, (LPCSTR)6);
	*(FARPROC*)& fSfcGetNextProtectedFile      = GetProcAddress(g_hSfcLib, (LPCSTR)"SfcGetNextProtectedFile");
	*(FARPROC*)& fSfcIsFileProtected           = GetProcAddress(g_hSfcLib, (LPCSTR)"SfcIsFileProtected");
	
	return TRUE;
}

VOID DoneSfcApi(VOID)
{
	if (g_hSfcLib == NULL)
		return;
	FreeLibrary(g_hSfcLib);
	g_hSfcLib = NULL;

	*(FARPROC*)& fSfcTerminateWatcherThread    = NULL;
	*(FARPROC*)& fSfcFileException             = NULL;
	*(FARPROC*)& fSfcInitiateScan              = NULL;
	*(FARPROC*)& fSfcGetNextProtectedFile      = NULL;
	*(FARPROC*)& fSfcIsFileProtected           = NULL;
	
	return;
}



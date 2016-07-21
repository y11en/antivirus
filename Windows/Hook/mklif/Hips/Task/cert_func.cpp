#include "cert_func.h"

static GUID GUID_WINTRUST_ACTION_GENERIC_VERIFY_V2 = WINTRUST_ACTION_GENERIC_VERIFY_V2;

typedef CRYPT_PROVIDER_DATA* (WINAPI *tWTHelperProvDataFromStateData)(HANDLE hStateData);
typedef CRYPT_PROVIDER_SGNR* (WINAPI *tWTHelperGetProvSignerFromChain)(CRYPT_PROVIDER_DATA *pProvData,
                                                                       DWORD idxSigner,
                                                                       BOOL fCounterSigner,
                                                                       DWORD idxCounterSigner);
typedef LONG (WINAPI *tWinVerifyTrust)(HWND hwnd, GUID *pgActionID, LPVOID pWVTData);
typedef DWORD (WINAPI *tCertGetNameStringW)(
    IN PCCERT_CONTEXT pCertContext,
    IN DWORD dwType,
    IN DWORD dwFlags,
    IN void *pvTypePara,
    OUT OPTIONAL LPWSTR pszNameString,
    IN DWORD cchNameString
    );
typedef DWORD (WINAPI *tCertNameToStrW)(
    IN DWORD dwCertEncodingType,
    IN PCERT_NAME_BLOB pName,
    IN DWORD dwStrType,
    OUT OPTIONAL LPWSTR psz,
    IN DWORD csz
    );

BOOL bWinTrustInited = FALSE;
BOOL bWinTrustInitSuccess = FALSE;
HMODULE hWinTrust = NULL;
HMODULE hCrypt32 = NULL;
tWinVerifyTrust fWinVerifyTrust = NULL;
tWTHelperProvDataFromStateData fWTHelperProvDataFromStateData = NULL;
tWTHelperGetProvSignerFromChain fWTHelperGetProvSignerFromChain = NULL;
tCertNameToStrW fCertNameToStrW = NULL;
tCertGetNameStringW fCertGetNameStringW = NULL;

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
BOOL InitWinTrustApi()
{
	if (bWinTrustInited)
		return bWinTrustInitSuccess;
	bWinTrustInited = TRUE;
	if (!hWinTrust)
		hWinTrust = LoadLibrary("wintrust.dll");
	if (!hWinTrust)
		return FALSE;
	fWinVerifyTrust = (tWinVerifyTrust)GetProcAddress(hWinTrust, "WinVerifyTrust");
	if (!fWinVerifyTrust)
		return FALSE;
	fWTHelperGetProvSignerFromChain = (tWTHelperGetProvSignerFromChain)GetProcAddress(hWinTrust, "WTHelperGetProvSignerFromChain");
	if (!fWTHelperGetProvSignerFromChain)
		return FALSE;
	fWTHelperProvDataFromStateData = (tWTHelperProvDataFromStateData)GetProcAddress(hWinTrust, "WTHelperProvDataFromStateData");
	if (!fWTHelperProvDataFromStateData)
		return FALSE;
	if (!hCrypt32)
		hCrypt32 = LoadLibrary("crypt32.dll");
	if (!hCrypt32)
		return FALSE;
	fCertNameToStrW = (tCertNameToStrW)GetProcAddress(hCrypt32, "CertNameToStrW");
	if (!fCertNameToStrW)
		return FALSE;
	fCertGetNameStringW = (tCertGetNameStringW)GetProcAddress(hCrypt32, "CertGetNameStringW");
	if (!fCertGetNameStringW)
		return FALSE;
	bWinTrustInitSuccess = TRUE;
	return TRUE;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
BOOL WTHlpVerifyEmbeddedSignature(HANDLE hFile, LPCWSTR pwszSourceFile, HANDLE* phWVTStateData)
{
    LONG lStatus;
    WINTRUST_FILE_INFO FileData;
    WINTRUST_DATA WinTrustData;

	if (!InitWinTrustApi())
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS::WTHlpVerifyEmbeddedSignature InitWinTrustApi FAIL"));
		return FALSE;
	}

    memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WinTrustData);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; 
    WinTrustData.dwProvFlags = WTD_REVOCATION_CHECK_NONE; // WTD_HASH_ONLY_FLAG | WTD_REVOCATION_CHECK_NONE; // WTD_SAFER_FLAG;
	WinTrustData.dwStateAction = phWVTStateData ? WTD_STATEACTION_VERIFY : WTD_STATEACTION_IGNORE;
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;

	memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.hFile = hFile;
    FileData.pcwszFilePath = pwszSourceFile;
    WinTrustData.pFile = &FileData;

    lStatus = fWinVerifyTrust(
		NULL,
		&GUID_WINTRUST_ACTION_GENERIC_VERIFY_V2,
		&WinTrustData);

	if (ERROR_SUCCESS == lStatus)
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS::WTHlpVerifyEmbeddedSignature fWinVerifyTrust return OK"));
		if (phWVTStateData)
			*phWVTStateData = WinTrustData.hWVTStateData;
		return TRUE;
	}
	PR_TRACE((g_root, prtERROR, TR "HIPS::WTHlpVerifyEmbeddedSignature fWinVerifyTrust FAIL"));

	if (WinTrustData.hWVTStateData)
	{
    	WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
		lStatus = fWinVerifyTrust(
			NULL,
			&GUID_WINTRUST_ACTION_GENERIC_VERIFY_V2,
			&WinTrustData);
	}

    return FALSE;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
void WTHlpFreeCertName(LPWSTR pString)
{
	HeapFree(GetProcessHeap(), 0, pString);
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
BOOL WTHlpGetCertName(HANDLE hWVTStateData, 
					 DWORD dwType,
					 DWORD dwFlags,
					 void* pvTypePara,
					 LPWSTR* pString
)
{
	PCCERT_CONTEXT pCertContext = NULL;
	PCERT_INFO pCertInfo = NULL;
	DWORD cbSize;
	LPWSTR pszSignerCertName;

	if (!pString)
		return TRUE;
	CRYPT_PROVIDER_DATA* pcdata = fWTHelperProvDataFromStateData(hWVTStateData);
	if (!pcdata)
		return FALSE;
	CRYPT_PROVIDER_SGNR* psigner = fWTHelperGetProvSignerFromChain(pcdata, 0, FALSE, 0);
	if (!psigner)
		return FALSE;
	if (psigner->csCertChain && psigner->pasCertChain)
		pCertContext = psigner->pasCertChain->pCert;
	if (!pCertContext)
		return FALSE;
	cbSize = fCertGetNameStringW(pCertContext, dwType, dwFlags, pvTypePara, 0, 0);
	if (!cbSize)
		return FALSE;
	pszSignerCertName = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (cbSize+0x10)*sizeof(WCHAR));
	if (!pszSignerCertName)
		return FALSE;
	if (!fCertGetNameStringW(pCertContext, dwType, dwFlags, pvTypePara, pszSignerCertName, cbSize))
	{
		pszSignerCertName[cbSize] = 0;
		WTHlpFreeCertName(pszSignerCertName);
		return FALSE;
	}
	*pString = pszSignerCertName;
	return TRUE;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
void WTHlpFreeStateData(HANDLE hWVTStateData)
{
    LONG lStatus;
    WINTRUST_DATA WinTrustData;
	memset(&WinTrustData, 0, sizeof(WinTrustData));
	WinTrustData.cbStruct = sizeof(WinTrustData);
	WinTrustData.hWVTStateData = hWVTStateData;
	WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
	lStatus = fWinVerifyTrust(
		NULL,
		&GUID_WINTRUST_ACTION_GENERIC_VERIFY_V2,
		&WinTrustData);
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
PWCHAR SkipDriveLetter(PWCHAR pPath)
{
	if (!pPath)
		return NULL;
	if (pPath[0] && pPath[1]==':' && pPath[2]=='\\')
		return pPath+2;
	return pPath;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
PWCHAR GetFilePart(PWCHAR pPath)
{
	// get file name without file path
	if (!pPath)
		return NULL;
	PWCHAR ptr = pPath;
	PWCHAR pname = ptr;
	while (*ptr)
	{
		if (*ptr == '\\')
			pname = ptr+1;
		ptr++;
	}
	return pname;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
PWCHAR GetFilePartWithoutExt(PWCHAR pPath, PWCHAR pOutPath)
{
	// get file name without file path
	if (!pPath)
		return NULL;
	PWCHAR ptr = pPath;
	PWCHAR pname = ptr;
	while (*ptr)
	{
		if (*ptr == '\\')
			pname = ptr+1;
		ptr++;
	}
	wcscpy(pOutPath, pname);
	if (ptr = wcsrchr(pOutPath, L'.')) {
		*ptr = 0;
	}
	else {
		return 0;
	}
	return pOutPath;
}

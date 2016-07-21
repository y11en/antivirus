#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <softpub.h>
#include <tchar.h>

#include "wthlp.h"
#include "sha1.h"

typedef CRYPT_PROVIDER_SGNR* (WINAPI *tWTHelperGetProvSignerFromChain)(CRYPT_PROVIDER_DATA *pProvData,
                                                                       DWORD idxSigner,
                                                                       BOOL fCounterSigner,
                                                                       DWORD idxCounterSigner);
typedef CRYPT_PROVIDER_DATA* (WINAPI *tWTHelperProvDataFromStateData)(HANDLE hStateData);
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

BOOL InitWinTrustApi()
{
	if (bWinTrustInited)
		return bWinTrustInitSuccess;
	bWinTrustInited = TRUE;
	if (!hWinTrust)
		hWinTrust = LoadLibrary(_T("wintrust.dll"));
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
		hCrypt32 = LoadLibrary(_T("crypt32.dll"));
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

static GUID GUID_WINTRUST_ACTION_GENERIC_VERIFY_V2 = WINTRUST_ACTION_GENERIC_VERIFY_V2;

void WTHlpFreeCertName(LPWSTR pString)
{
	HeapFree(GetProcessHeap(), 0, pString);
}

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

BOOL WTHlpVerifyEmbeddedSignature(HANDLE hFile, LPCWSTR pwszSourceFile, HANDLE* phWVTStateData, LONG* lpWVTResult)
{
    LONG lStatus;
    WINTRUST_FILE_INFO FileData;
    WINTRUST_DATA WinTrustData;

	if (lpWVTResult)
		*lpWVTResult = TRUST_E_FAIL;

	if (!InitWinTrustApi())
		return FALSE;

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
	if (lpWVTResult)
		*lpWVTResult = lStatus;

//	PR_TRACE((g_root, prtNOTIFY, TR "WVT(%08X, '%S') res=%08X", hFile, pwszSourceFile ? pwszSourceFile : L"NULL", lStatus));

	if (ERROR_SUCCESS == lStatus)
	{
		if (phWVTStateData)
			*phWVTStateData = WinTrustData.hWVTStateData;
		return TRUE;
	}

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

PBYTE _iAddSha1HashData(SHA1Context* sha_context, PBYTE pData, PBYTE pToData, DWORD cbSkip)
{
	size_t cbSize = pToData - pData;
	SHA1Input(sha_context, pData, (unsigned int)cbSize);
	return pData + cbSize + cbSkip;
}

BOOL _iCalcHashFromMappedFile(IN PVOID pFileData, DWORD dwFileSize, IN OUT DWORD* hash_size, OUT BYTE* hash)
{
	IMAGE_DOS_HEADER* pMZ;
	IMAGE_NT_HEADERS* pPE;
	IMAGE_SECTION_HEADER* pSec;
	SHA1Context sha;
	DWORD dwSecurityData, dwSecuritySize;
	PBYTE pNextData;
	int i;

	if (hash_size)
	{
		if (*hash_size < 20)
		{
			*hash_size = 20;
			return FALSE;
		}
		*hash_size = 20;
	}

	if (!hash)
		return FALSE;

	pNextData = (PBYTE)pFileData;
	pMZ = (IMAGE_DOS_HEADER*)pFileData;
	if (pMZ->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;
	if (dwFileSize < (ULONG)pMZ->e_lfanew + sizeof(IMAGE_NT_HEADERS))
		return FALSE;
	pPE = (IMAGE_NT_HEADERS*)((PBYTE)pFileData + (ULONG)pMZ->e_lfanew);
	if (pPE->Signature != IMAGE_NT_SIGNATURE)
		return FALSE;
	if (pPE->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) // TODO: IMAGE_FILE_MACHINE_AMD64
		return FALSE;
	SHA1Reset(&sha);
	pNextData = _iAddSha1HashData(&sha, pNextData, (PBYTE)&pPE->OptionalHeader.CheckSum, sizeof(pPE->OptionalHeader.CheckSum));
	pNextData = _iAddSha1HashData(&sha, pNextData, (PBYTE)&pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY], sizeof(IMAGE_DATA_DIRECTORY));
	dwSecurityData = pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress;
	dwSecuritySize = pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
	// validate security data
	if (!dwSecurityData && dwSecuritySize)
		return FALSE;
	if (dwSecurityData)
	{
		WORD i;
		WORD nSections;
		if (dwSecurityData + dwSecuritySize > dwFileSize)
			return FALSE;
		nSections = pPE->FileHeader.NumberOfSections;
		pSec = (IMAGE_SECTION_HEADER*)((PBYTE)pPE + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + pPE->FileHeader.SizeOfOptionalHeader);
		if ((PBYTE)pSec + nSections*sizeof(IMAGE_SECTION_HEADER) > (PBYTE)pFileData + dwFileSize)
			return FALSE;
		for (i=0; i<nSections; i++, pSec++)
		{
			if (0 == pSec->PointerToRawData)
				continue;
			if (pSec->PointerToRawData + pSec->SizeOfRawData > dwSecurityData)
				return FALSE;
		}
		pNextData = _iAddSha1HashData(&sha, pNextData, (PBYTE)pFileData + dwSecurityData, dwSecuritySize);
	}
	pNextData = _iAddSha1HashData(&sha, pNextData, (PBYTE)pFileData + dwFileSize, 0);
	if (dwFileSize & 7)
	{
		BYTE fill[8] = {0};
		_iAddSha1HashData(&sha, fill, fill + 8 - (dwFileSize & 7), 0);
	}

	if (!SHA1Result(&sha))
		return FALSE;
	for (i=0; i<5; i++)
	{
		PBYTE p = (PBYTE)&sha.Message_Digest[i];
		*hash++ = p[3];
		*hash++ = p[2];
		*hash++ = p[1];
		*hash++ = p[0];
	}
	return TRUE;
}

BOOL WTHlpCalcHashFromFileHandle(IN HANDLE hFile, IN OUT DWORD* hash_size, OUT BYTE* hash)
{
	HANDLE hMapping;
	LPVOID pData;
	BOOL bRes;
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0xFFFFFFFF)
		return FALSE;
	if (dwFileSize < sizeof(IMAGE_DOS_HEADER)+sizeof(IMAGE_NT_HEADERS))
		return FALSE;
	hMapping = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, dwFileSize, NULL);
	if (hMapping == NULL)
		return FALSE;
	pData = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
	CloseHandle(hMapping);
	if (!pData)
		return FALSE;
	bRes = _iCalcHashFromMappedFile(pData, dwFileSize, hash_size, hash);
	UnmapViewOfFile(pData);
	return bRes;
}

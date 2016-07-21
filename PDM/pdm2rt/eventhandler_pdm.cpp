#include "eventhandler_pdm.h"
#include <plugin/p_hash_md5.h>
#include "pdm2imp.h"

#include <wintrust.h>
#include <softpub.h>

void
TraceBeforeRequest (
	ULONG RefCount,
	PVOID pEventHdr
	);

struct tRecordData {
	tDWORD dwFlags;
	tDWORD dwReserved;
};
#define RD_FLAGS_TRUSTED 1

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
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
BOOL WTHlpVerifyEmbeddedSignature(HANDLE hFile, LPCWSTR pwszSourceFile, HANDLE* phWVTStateData)
{
    LONG lStatus;
    WINTRUST_FILE_INFO FileData;
    WINTRUST_DATA WinTrustData;

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

	PR_TRACE((g_root, prtNOTIFY, TR "WVT(%08X, '%S') res=%08X", hFile, pwszSourceFile ? pwszSourceFile : L"NULL", lStatus));

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
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
bool CalcImageHashMD5(cObj* hParent, cEnvironmentHelper * pEnvHelper, tcstring ImagePath, void* pHash, int nHashSize, DWORD nSizeLimit)
{
	tERROR error;
	hHASH hHash;

	uint64_t nFileSize64;
	size_t nFileSize;
	cAutoEnvMem<BYTE>  pBuff(pEnvHelper);

	cFile ImageFile(pEnvHelper, ImagePath);

	nFileSize64 = ImageFile.Size();
	if (nFileSize64 == 0)
		return false;
	nFileSize = (size_t)nFileSize64;

	pBuff.Alloc(nFileSize + 1);	//+1 for NULL terminating
	if (!pBuff)
		return false;

	uint32_t nBytesRead;
	if (false == ImageFile.Read(0, pBuff, (uint32_t)nFileSize, &nBytesRead))
		return false;

	nFileSize = (nBytesRead > nSizeLimit) ? nSizeLimit : nBytesRead;

	error = hParent->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5);
	if (PR_FAIL(error))	
	{
		PR_TRACE((hParent, prtERROR, TR "intche:\tCannot create MD5 hash, %terr", error));
		return false;
	}
	error = hHash->Update(pBuff, nFileSize);

	DWORD dwHash[4];
	if (PR_SUCC(error)) {
		error = hHash->GetHash((tBYTE*)&dwHash, 16);
	}
	if (PR_SUCC(error)) {
		switch(nHashSize)
		{
		case 16:
			memcpy(pHash, &dwHash, 16);
			break;
		case 8:
			dwHash[0] ^= dwHash[2];
			dwHash[1] ^= dwHash[3];
			memcpy(pHash, &dwHash, 8);
			break;
		default:
			error = errPARAMETER_INVALID;
		}
	}
	hHash->sysCloseObject();
	return PR_SUCC(error);
}

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
tERROR AddStringToHash(hHASH hHash, tcstring pwcString, tBOOL bLowerCase)
{
	tERROR error;
	tchar wc;
	tUINT size;
	tcstring _ps = pwcString;
	if (!pwcString)
		return errOK;
	while (wc = *pwcString)
	{
		if (bLowerCase)
		{
			if (wc >= 'A' && wc <= 'Z')
				wc |= 0x20; // lower-case
		}
		size = wc <= 0xFF ? 1 : sizeof(wc);
		if (PR_FAIL(error = hHash->Update((tBYTE*) &wc, size)))
			return error;
		pwcString++;
	}
#ifdef _DEBUG
	{
		tDWORD nHash[4];
		hHash->GetHash((tBYTE*)&nHash, 16);
		PR_TRACE((g_root, prtIMPORTANT, TR "AddStringToHash: %S %d %08X %08X %08X %08X", _ps, bLowerCase, nHash[0], nHash[1], nHash[2], nHash[3]));
	}
#endif
	return errOK;
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
static PWCHAR SkipDriveLetter(PWCHAR pPath)
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
static PWCHAR GetFilePart(PWCHAR pPath)
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
bool CheckExcludeEventData(hSECUREFILEDB hSFDB, hHASH hHash, enPdm2EventType nEventType, BYTE pHash[16], tcstring pName1, ...)
{
	// calc id + names

	char sTrace[0x1000];
	int len = 0;
	va_list args;
	sTrace[0] = 0;
	if (!pHash && !pName1) {
		PR_TRACE((g_root, prtERROR, TR "CheckExcludeEventData !pHash && !pName1"));
		return false;
	}
	hHash->Reset();
	if (pHash)
	{
		DWORD* pdwhash = (DWORD*)pHash;
		len += _sprintf_p(sTrace+len, sizeof(sTrace) - 1 - len, "%08X%08X%08X%08X ", pdwhash[0], pdwhash[1], pdwhash[2], pdwhash[3]);
		hHash->Update(pHash, 16);
	}
	if (pName1)
	{
		va_start(args, pName1);
		len += _sprintf_p(sTrace+len, sizeof(sTrace) - 1 - len, "\"");
		AddStringToHash(hHash, pName1, cTRUE);
		len += _sprintf_p(sTrace+len, sizeof(sTrace) - 1 - len, "%S", pName1);
		while (pName1 = va_arg(args, PWCHAR))
		{
			AddStringToHash(hHash, pName1, cTRUE);
			len += _sprintf_p(sTrace+len, sizeof(sTrace) - 1 - len, "%S", pName1);
		}
		len += _sprintf_p(sTrace+len, sizeof(sTrace) - 1 - len, "\"");
		va_end(args);
	}
	if (nEventType)
	{
		hHash->Update((tBYTE*) &nEventType, sizeof(nEventType));
		len += _sprintf_p(sTrace+len, sizeof(sTrace) - 1 - len, " ev:%d", nEventType);
	}
	// IsTrustedHash
	{

		tERROR error;

		tDWORD nHash[4];
		tRecordData rd;
		memset(&rd, 0, sizeof(rd));
		error = hHash->GetHash((tBYTE*)&nHash, sizeof(nHash));
		if (PR_FAIL(error))
			return false;
		nHash[0] ^= nHash[2];
		nHash[1] ^= nHash[3];
		error = hSFDB->GetRecord(&nHash, &rd);
		if (PR_FAIL(error))
		{
			PR_TRACE((hSFDB, prtIMPORTANT, TR "%08X%08X NF %terr %s", nHash[1], nHash[0], error, sTrace));
			return false;
		}
		PR_TRACE((hSFDB, prtIMPORTANT, TR "%08X%08X DATA:%08X,%08X T=%d %s", nHash[1], nHash[0], rd.dwFlags, rd.dwReserved, rd.dwFlags & RD_FLAGS_TRUSTED, sTrace));
		if (rd.dwFlags & RD_FLAGS_TRUSTED)
			return true;
		return false;
	}
	return false;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
bool CheckExcludeCertAttr(hSECUREFILEDB hSFDB, hHASH hHash, enPdm2EventType nEventType, PWCHAR pCertAttrName, PWCHAR pCertAttrValue, PWCHAR pImagePath )
{
	if (!pCertAttrValue || !pCertAttrName)
		return NULL;
	if (CheckExcludeEventData(hSFDB, hHash, nEventType, NULL, L"Cert::", pCertAttrName, L"=", pCertAttrValue, 0 ))
		return true;
	if (pImagePath)
	{
		if (CheckExcludeEventData(hSFDB, hHash, nEventType, NULL, L"Cert::", pCertAttrName, L"=", pCertAttrValue, L":", pImagePath, 0))
			return true;
		if (CheckExcludeEventData(hSFDB, hHash, PDM2_EVENT_UNKNOWN, NULL, L"Cert::", pCertAttrName, L"=", pCertAttrValue, L":", pImagePath, 0))
			return true;
	}
	if (0 == wcscmp(pCertAttrValue, L"Microsoft Corp"))
	{
		if (CheckExcludeCertAttr(hSFDB, hHash, nEventType, pCertAttrName, L"Microsoft Corporation", pImagePath ))
			return true;
	}
	return false;
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
bool CheckExcludeCertAttrFull(hSECUREFILEDB hSFDB, hHASH hHash, enPdm2EventType nEventType, PWCHAR pCertAttrName, PWCHAR pCertAttrValue, PWCHAR pFullName )
{
	if (!pFullName)
		return false;

	PWCHAR pFilePath = SkipDriveLetter(pFullName);
	PWCHAR pFileName = GetFilePart(pFilePath);

	if (CheckExcludeCertAttr(hSFDB, hHash, nEventType, pCertAttrName, pCertAttrValue, pFilePath))
		return true;
	if (CheckExcludeCertAttr(hSFDB, hHash, nEventType, pCertAttrName, pCertAttrValue, pFileName))
		return true;
	return false;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
bool CheckExcludeBySignature(cEvent& event, hSECUREFILEDB hSFDB, hHASH hHash, cEnvironmentHelper * pEnvHelper, LPWSTR pFullName, enPdm2EventType nEventType)
{
	if (!pFullName)
		return false;

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(pFullName))
		return false;
	LPWSTR pName = NULL;
	HANDLE hWVTStateData;

	if (WTHlpVerifyEmbeddedSignature(0, pFullName, &hWVTStateData)) {
		bool bExclude = false;
		DWORD dwStrType = CERT_X500_NAME_STR;
		if (WTHlpGetCertName(hWVTStateData, CERT_NAME_RDN_TYPE, 0,  &dwStrType, &pName))
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "Certificate subject: %S", pName));
			WTHlpFreeCertName(pName);
		}
		if (WTHlpGetCertName(hWVTStateData, CERT_NAME_RDN_TYPE, CERT_NAME_ISSUER_FLAG,  &dwStrType, &pName))
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "Certificate issuer: %S", pName));
			WTHlpFreeCertName(pName);
		}
		if (!bExclude && WTHlpGetCertName(hWVTStateData, CERT_NAME_ATTR_TYPE, 0,  szOID_ORGANIZATION_NAME, &pName))
		{
			bExclude = CheckExcludeCertAttrFull(hSFDB, hHash, nEventType, L"O", pName, pFullName);
			WTHlpFreeCertName(pName);
		} 
		else if (!bExclude && WTHlpGetCertName(hWVTStateData, CERT_NAME_ATTR_TYPE, 0,  szOID_COMMON_NAME, &pName))
		{
			bExclude = CheckExcludeCertAttrFull(hSFDB, hHash, nEventType, L"CN", pName, pFullName);
			WTHlpFreeCertName(pName);
		}

		WTHlpFreeStateData(hWVTStateData);
		if (bExclude)
			return true;
	}
	//	!!! temporarily not activated this functionality
	//if (!pPDM->m_bWasOneCheck_MicrosoftSign)
	//{
	//	PR_TRACE((g_root, prtIMPORTANT, TR "MSSign service not ready %X", paskitem->m_pProcess->m_Flags));
	//	//if (paskitem->m_pProcess->IsType(_SPRC_FLAG_SVCHOST)
	//	//	|| paskitem->m_pProcess->IsType(_SPRC_FLAG_SCM))
	//	if (g_Service.IsThis(paskitem->m_pProcess->m_pwchImagePath, L"%SystemRoot%\\system32\\svchost.exe")
	//		|| g_Service.IsThis(paskitem->m_pProcess->m_pwchImagePath, L"%SystemRoot%\\system32\\services.exe"))
	//	{
	//		PR_TRACE((g_root, prtIMPORTANT, TR "type sys"));
	//		return true;
	//	}
	//}
	//else
	//{
	//	if (VTIsMicrosoftSigned(pPDM->m_hVerify, pFullName))
	//	{
	//		if (CheckExcludeCertAttrFull(pPDM->m_hSFDB_KL, hHash, nEventType, L"O", L"Microsoft Corporation", pFullName))
	//			return true;
	//	}	
	//}
	return false;
}
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
bool cEventHandlerPDM::CheckExcludeEvent(cEvent& event, unsigned int l_EventType)
{
	tERROR error;
	hHASH  hHash = NULL;
	BYTE nImageHash[16];
	bool bImageHashMD5;

	cProcess pProcess = event.GetProcess();
	cProcess pParentProcess;
	if (!pProcess) 
	{
		PR_TRACE(( g_root, prtERROR, TR "cEventHandlerPDM::CheckExcludeEvent:: event.GetProcess() return NULL" ));
		return false;
	}

	enPdm2EventType nEventType = (enPdm2EventType)l_EventType;

	if (nEventType == PDM2_EVENT_RDR) 
	{
		pProcess = pProcess->get_parent();
		pParentProcess = pProcess;
		if (!pProcess)
		{
			cProcess pTempProcess = event.GetProcess();
			PR_TRACE((g_root, prtERROR, TR "CheckExcludeEvent: REPORT_EVENT_RDR, cannot get parent of %d", pTempProcess->m_pid));
			return false;
		}
	}

	PR_TRACE((g_root, prtIMPORTANT, TR "CheckExcludeEvent %d '%S'", 
		nEventType,
		(pProcess->m_image.getFull()) ? pProcess->m_image.getFull() : L"NULL"
		));

	//	!!! there is no trusted list
	//if (pProcess->IsType(_SPRC_FLAG_USER_TRUST))
	//{
	//	PR_TRACE((g_root, prtIMPORTANT, TR "CheckExcludeEvent: process is trusted"));
	//	return true;
	//}

	bImageHashMD5 = CalcImageHashMD5(
						m_pThis,
						event.m_pEventMgr->m_pEnvironmentHelper,
						pProcess->m_image.getFull(),
						nImageHash, 16, 2*1024*1024);


	if (!pProcess->m_image.getFull() && !bImageHashMD5)
	{
		return false;
	}

	error = m_pThis->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5);
	if (PR_FAIL(error))	
	{
		PR_TRACE((g_root, prtERROR, TR "Cannot create hash object (sysCreateObjectQuick), %terr", error));
		return false;
	}

	cAutoObj<cHash> _hash(hHash);

	// calc image hash
	if (bImageHashMD5)
	{
		if (CheckExcludeEventData(m_hSFDB_KL, hHash, PDM2_EVENT_UNKNOWN, nImageHash, 0)) {
			return true;
		}
		if (CheckExcludeEventData(m_hSFDB_KL, hHash, nEventType, nImageHash, 0)) {
			return true;
		}
	}
	
	// now calc names
	PWCHAR pImagePath = SkipDriveLetter((PWCHAR)pProcess->m_image.getFull());
	PWCHAR pImageFile = GetFilePart(pImagePath);

	if (CheckExcludeEventData(m_hSFDB_KL, hHash, nEventType, NULL, pImagePath, 0)) {
		return true;
	}
	if (CheckExcludeEventData(m_hSFDB_KL, hHash, nEventType, NULL, pImageFile, 0)) {
		return true;
	}
	
	
	//	!!!	
	//	logic part (paskitem->m_RequestProps.m_pwchUserDescription) not implemented in PDM2
	//	that is why this block not need
	//
	//if (paskitem->m_RequestProps.m_pwchUserDescription && *paskitem->m_RequestProps.m_pwchUserDescription)
	//{
	//	PWCHAR pDllPath = SkipDriveLetter(paskitem->m_RequestProps.m_pwchUserDescription);
	//	PWCHAR pDllFile = GetFilePart(pDllPath);
	//	if (CheckExcludeEventData(pPDM->m_hSFDB_KL, hHash, nEventType, NULL, pDllPath, 0))
	//		return true;
	//	if (CheckExcludeEventData(pPDM->m_hSFDB_KL, hHash, nEventType, NULL, pDllFile, 0))
	//		return true;
	//	if (CheckExcludeEventData(pPDM->m_hSFDB_KL, hHash, nEventType, NULL, pImagePath, L":", pDllPath, 0))
	//		return true;
	//}
	//
	//if (paskitem->m_RequestProps.m_ImagePath && *paskitem->m_RequestProps.m_ImagePath)
	//{
	//	PWCHAR pDllPath = SkipDriveLetter(paskitem->m_RequestProps.m_ImagePath);
	//	PWCHAR pDllFile = GetFilePart(pDllPath);
	//	if (CheckExcludeEventData(pPDM->m_hSFDB_KL, hHash, nEventType, NULL, pDllPath, 0))
	//		return true;
	//	if (CheckExcludeEventData(pPDM->m_hSFDB_KL, hHash, nEventType, NULL, pDllFile, 0))
	//		return true;
	//	if (CheckExcludeEventData(pPDM->m_hSFDB_KL, hHash, nEventType, NULL, pImagePath, L":", pDllPath, 0))
	//		return true;
	//}
	//if (paskitem->m_pwchKeyName)
	//{
	//	HKEY hRootKey = GetRootKey(paskitem->m_pwchKeyName);
	//	WCHAR* pSubKey = GetSubkeyName(paskitem->m_pwchKeyName);
	//	if (pSubKey && hRootKey == HKEY_USERS)
	//	{
	//		// skip user SID
	//		pSubKey = wcschr(pSubKey, '\\');
	//		if (pSubKey)
	//			pSubKey++;
	//	}
	//	//PR_TRACE((g_root, prtIMPORTANT, TR "CheckExcludeEvent '%S' '%S' \\ '%S', %d", paskitem->m_pwchKeyName, pSubKey ? pSubKey : L"NULL", paskitem->m_pwchValueName ? paskitem->m_pwchValueName : L"NULL", nEventType));
	//	if (CheckExcludeEventData(pPDM->m_hSFDB_KL, hHash, nEventType, NULL, pSubKey, paskitem->m_pwchValueName ? L"\\" : NULL, paskitem->m_pwchValueName, 0))
	//		return true;
	//}
	
	bool bMsSign = CheckExcludeBySignature(event, m_hSFDB_KL, hHash, event.m_pEventMgr->m_pEnvironmentHelper, (PWCHAR)pProcess->m_image.getFull(), nEventType);
	if (bMsSign) {
		return true;
	}

	//if (nEventType == REPORT_EVENT_REGMODIFY)
	//{
	//	if (paskitem->m_pValue && paskitem->m_ValueLen > 4)
	//	{
	//		cStringObj sValue;
	//		sValue.assign(paskitem->m_pValue, cCP_UNICODE, paskitem->m_ValueLen);
	//		if (CheckExcludeBySignature(pPDM, paskitem, hHash, (PWCHAR)sValue.data(), nEventType))
	//			return true;
	//	}
	//}
	return false;
	
}

bool
cEventHandlerPDM::MakeRequest (
	cEvent& event,
	unsigned int EventType,
	unsigned int EventSubType
	)
{
	if (CheckExcludeEvent(event, EventType)) {
		return true;
	}

	PEventContext pEventContext = (PEventContext) event.m_context;

	if (!pEventContext)
		return false;

	cProcess pProcess = event.GetProcess();

	if (!pProcess)
		return false;

	if (!pEventContext->m_pRefData)
	{
		pEventContext->m_pRefData = (PRefData) pfMemAlloc( NULL, sizeof(SingleEvent), 0 );
		if (!pEventContext->m_pRefData)
			return false;

		pEventContext->m_pRefData->m_RefCount = 0;
		pEventContext->m_pRefData->m_VerdictFlags = 0;

		if (!OpenThreadToken (
			GetCurrentThread(),
			TOKEN_QUERY | TOKEN_IMPERSONATE,
			TRUE,
			&pEventContext->m_pRefData->m_hTokenImpersonate
			))
		{
			// error get impersonalization token
			pEventContext->m_pRefData->m_hTokenImpersonate = NULL;
		}
	}

	pEventContext->m_EventType = (tPdm2EventType) EventType;
	pEventContext->m_EventSubType = (tPdm2EventSubType) EventSubType;
	pEventContext->m_pProcess = pProcess;

	TraceBeforeRequest( pEventContext->m_pRefData->m_RefCount, pEventContext->m_SE.m_pEventHdr );

	tTaskId task_info;
	if ( PR_SUCC( pEventContext->m_pInternal->m_ThPoolInfo->AddTask (
		&task_info,
		ThreadInfoCallback,
		pEventContext,
		sizeof(EventContext),
		0
		)))
	{
		pProcess->AddRef();

		// not synchronized - working on one thread
		pEventContext->m_pRefData->m_RefCount++;
		
		event.SetVerdict( evtVerdict_Pending );

		return true;
	}

	if (!pEventContext->m_pRefData->m_RefCount)
	{
		if (pEventContext->m_pRefData->m_hTokenImpersonate)
		{
			CloseHandle( pEventContext->m_pRefData->m_hTokenImpersonate );
			pEventContext->m_pRefData->m_hTokenImpersonate = NULL;
		}

		pfMemFree( NULL, (void**) &pEventContext->m_pRefData );
	}

	PR_TRACE(( g_root, prtERROR, TR "MakeRequest failed" ));

	return false;
}

cEventHandlerPDM::cEventHandlerPDM()
{
}

cEventHandlerPDM::~cEventHandlerPDM()
{
}


//tVerdict
void
cEventHandlerPDM::OnSelfCopy (
	cEvent& event,
	cFile& copy
	)
{
	PR_TRACE(( g_root, prtERROR, TR "OnSelfCopy to %S", copy.get() ));

	return;// evtVerdict_Default;
}
// === Detect ==========================================================================================

//tVerdict
void
cEventHandlerPDM::OnSelfCopyMultiple (
	cEvent& event,
	cFile& copy
	)
{
	PR_TRACE(( g_root, prtERROR, TR "OnSelfCopyMultiple" ));

	MakeRequest( event, PDM2_EVENT_SC_MULTIPLE );
}

//tVerdict
void
cEventHandlerPDM::OnSelfCopyToNetwork (
	cEvent& event,
	cFile& copy
	)
{
	PR_TRACE(( g_root, prtERROR, TR "OnSelfCopyToNetwork" ));

	MakeRequest( event, PDM2_EVENT_SCN);
}

//tVerdict
void
cEventHandlerPDM::OnSelfCopyToPeerToPeerFolder (
	cEvent& event,
	cFile& copy
	)
{
	PR_TRACE(( g_root, prtERROR, TR "OnSelfCopyToPeerToPeerFolder" ));

	MakeRequest( event, PDM2_EVENT_P2P_SCN );
}

//tVerdict
void
cEventHandlerPDM::OnRegAvpImageFileExecOptions (
	cEvent& event,
	const cRegKey& key,
	tcstring sValueName,
	tcstring sValueData
	)
{
	PR_TRACE(( g_root, prtERROR, TR "OnRegAvpImageFileExecOptions" ));
	return;// evtVerdict_Default;
}

//tVerdict
void
cEventHandlerPDM::OnSelfCopyAutoRun (
	cEvent& event,
	const cRegKey& key,
	tcstring sValueName,
	tcstring sValueData,
	tcstring sValueDataPart,
	tcstring sFilename
	)
{
	PR_TRACE(( g_root, prtERROR, TR "OnSelfCopyAutoRun" ));

	MakeRequest( event, PDM2_EVENT_SC_AR );
}

//tVerdict
void
cEventHandlerPDM::OnSelfCopyToStartup (
	cEvent& event,
	tcstring sFilename
	)
{
	PR_TRACE(( g_root, prtERROR, TR "OnSelfCopyToStartup" ));

	MakeRequest( event, PDM2_EVENT_SC2STARTUP );
}

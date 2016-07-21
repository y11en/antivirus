#include "verifytrust.h"
#ifdef _PRAGUE_TRACE_
#include <Prague/prague.h>
#include <tchar.h>
#else
#define PR_TRACE()
#endif

typedef struct tag_VerifyTrustData
{
	BOOL m_bAPIResolved;
	HMODULE	m_hWintrustDll;
	HMODULE	m_hCrypt32Dll;
	BOOL (WINAPI *fCryptCATAdminAcquireContext)(OUT HCATADMIN *phCatAdmin, IN const GUID *pgSubsystem, IN DWORD dwFlags);
	BOOL (WINAPI *fCryptCATAdminCalcHashFromFileHandle)(IN HANDLE hFile, IN OUT DWORD *pcbHash, OUT OPTIONAL BYTE *pbHash, IN DWORD dwFlags);
	HCATINFO (WINAPI *fCryptCATAdminEnumCatalogFromHash)(IN HCATADMIN hCatAdmin, IN BYTE *pbHash, IN DWORD cbHash, IN DWORD dwFlags, IN OUT HCATINFO *phPrevCatInfo); 
	BOOL (WINAPI *fCryptCATCatalogInfoFromContext)(IN HCATINFO hCatInfo, IN OUT CATALOG_INFO *psCatInfo, IN DWORD dwFlags); 
	BOOL (WINAPI *fCryptCATAdminReleaseCatalogContext)(IN HCATADMIN hCatAdmin, IN HCATINFO hCatInfo, IN DWORD dwFlags);
	BOOL (WINAPI *fCryptCATAdminReleaseContext)(IN HCATADMIN hCatAdmin, IN DWORD dwFlags);
	HCERTSTORE (WINAPI *fCertOpenStore)(IN LPCSTR lpszStoreProvider, IN DWORD dwEncodingType, IN HCRYPTPROV hCryptProv, IN DWORD dwFlags, IN const void *pvPara);
	PCCERT_CONTEXT (WINAPI *fCertEnumCertificatesInStore)(IN HCERTSTORE hCertStore, IN PCCERT_CONTEXT pPrevCertContext);
	BOOL (WINAPI *fCertGetCertificateChain)(IN OPTIONAL HCERTCHAINENGINE hChainEngine, IN PCCERT_CONTEXT pCertContext, IN OPTIONAL LPFILETIME pTime, IN OPTIONAL HCERTSTORE hAdditionalStore, IN PCERT_CHAIN_PARA pChainPara, IN DWORD dwFlags, IN LPVOID pvReserved, OUT PCCERT_CHAIN_CONTEXT* ppChainContext);
	PCCERT_CHAIN_CONTEXT (WINAPI *fCertFindChainInStore)(HCERTSTORE hCertStore, DWORD dwCertEncodingType, DWORD dwFindFlags, DWORD dwFindType, const void* pvFindPara, PCCERT_CHAIN_CONTEXT pPrevChainContext);
	BOOL (WINAPI *fCertVerifyCertificateChainPolicy)(IN LPCSTR pszPolicyOID, IN PCCERT_CHAIN_CONTEXT pChainContext, IN PCERT_CHAIN_POLICY_PARA pPolicyPara, IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus);
	VOID (WINAPI *fCertFreeCertificateChainEngine)(IN HCERTCHAINENGINE hChainEngine);
	VOID (WINAPI *fCertFreeCertificateChain)(IN PCCERT_CHAIN_CONTEXT pChainContext);
	BOOL (WINAPI *fCertFreeCertificateContext)(IN PCCERT_CONTEXT pCertContext);
	BOOL (WINAPI *fCertCloseStore)(IN HCERTSTORE hCertStore, DWORD dwFlags);
    LONG (WINAPI *fWinVerifyTrust)(HWND hwnd, GUID *pguidAction, LPVOID pvData);
	DWORD (WINAPI *fCertGetNameStringA)(PCCERT_CONTEXT pCertContext, DWORD dwType, DWORD dwFlags, void* pvTypePara, LPSTR pszNameString, DWORD cchNameString);
	BOOL m_bCacheContext;
	HCATADMIN m_hCatAdmin;
	LONG m_nCatAdminRefCount;
	WCHAR** m_pCatCache;
	DWORD m_nCatCacheCount;
	CRITICAL_SECTION m_sync;
	OSVERSIONINFOA vi;
} tVerifyTrustData;

void WINAPI VerifyTrustDone(HVERIFY hVerify)
{
	tVerifyTrustData* d = hVerify;
	if (!d)
		return;
	if (d->m_hCatAdmin)
		d->fCryptCATAdminReleaseContext(d->m_hCatAdmin, 0);
	if (d->m_pCatCache)
	{
		DWORD i;
		for (i=0; i<d->m_nCatCacheCount; i++)
		{
			if (!d->m_pCatCache)
				break;
			if (!d->m_pCatCache[i])
				continue;
			HeapFree(GetProcessHeap(), 0, d->m_pCatCache[i]);
		}
		HeapFree(GetProcessHeap(), 0, d->m_pCatCache);
	}
	if (d->m_hWintrustDll)
		FreeLibrary(d->m_hWintrustDll);
	if (d->m_hCrypt32Dll)
		FreeLibrary(d->m_hCrypt32Dll);
	DeleteCriticalSection(&d->m_sync);
	HeapFree(GetProcessHeap(), 0, d);
}

#define _RESOLVE_API(module, name) \
	*(void**)&d->f##name = GetProcAddress(module, #name); \
	if (!d->f##name) \
		return FALSE; \

static BOOL iResolveAPI2(tVerifyTrustData* d)
{
	d->m_hWintrustDll = LoadLibraryA("WINTRUST");
	if (!d->m_hWintrustDll)
		return FALSE;
	d->m_hCrypt32Dll = LoadLibraryA("CRYPT32");
	if (!d->m_hCrypt32Dll)
		return FALSE;
	
	memset(&d->vi, 0, sizeof(OSVERSIONINFOA));
	d->vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if ( !GetVersionExA(&d->vi) )
		return FALSE;

	_RESOLVE_API(d->m_hWintrustDll, WinVerifyTrust);
	_RESOLVE_API(d->m_hWintrustDll, CryptCATAdminAcquireContext);
	_RESOLVE_API(d->m_hWintrustDll, CryptCATAdminCalcHashFromFileHandle);
	_RESOLVE_API(d->m_hWintrustDll, CryptCATAdminEnumCatalogFromHash);
	_RESOLVE_API(d->m_hWintrustDll, CryptCATCatalogInfoFromContext);
	_RESOLVE_API(d->m_hWintrustDll, CryptCATAdminReleaseCatalogContext);
	_RESOLVE_API(d->m_hWintrustDll, CryptCATAdminReleaseContext);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertOpenStore);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertEnumCertificatesInStore);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertGetCertificateChain);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertFindChainInStore);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertVerifyCertificateChainPolicy);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertFreeCertificateChainEngine);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertFreeCertificateChain);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertFreeCertificateContext);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertCloseStore);
	_RESOLVE_API(d->m_hCrypt32Dll,  CertGetNameStringA);
	return TRUE;
}

static BOOL iResolveAPI(tVerifyTrustData* d)
{
	if (d->m_bAPIResolved)
		return TRUE;
	EnterCriticalSection(&d->m_sync);
	d->m_bAPIResolved = iResolveAPI2(d);
	LeaveCriticalSection(&d->m_sync);
	return d->m_bAPIResolved;
}


HVERIFY WINAPI VerifyTrustInit(BOOL bCacheContext)
{
	tVerifyTrustData* d;
	d = (tVerifyTrustData*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tVerifyTrustData));
	d->m_bCacheContext = bCacheContext;
	InitializeCriticalSection(&d->m_sync);
	PR_TRACE((0, prtNOTIFY, TR "Inited"));
	return d;
}


BOOL WINAPI iIsMicrosoftSignedStore(HVERIFY hVerify, const WCHAR* wcsFName)
{
	tVerifyTrustData* d = hVerify;
	BOOL bRes = FALSE;
	HCERTSTORE hStore = NULL;

	if (!d)
		return FALSE;

	__try
	{
		hStore = d->fCertOpenStore(CERT_STORE_PROV_FILENAME_W, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_STORE_OPEN_EXISTING_FLAG|CERT_STORE_READONLY_FLAG, wcsFName);
	}
	__except(TRUE)
	{
		hStore = NULL;
		PR_TRACE((0, prtIMPORTANT, TR "fCertOpenStore - exception"));
	}

	if (!hStore)
	{
		PR_TRACE((0, prtIMPORTANT, TR "Open file '%S' as secure store failed with %d", wcsFName, GetLastError()));
	}
	else
	{	
		CERT_ENHKEY_USAGE        EnhkeyUsage;
		CERT_USAGE_MATCH         CertUsage;  
		CERT_CHAIN_PARA          ChainPara;
		PCCERT_CONTEXT           pCertContext;
		EnhkeyUsage.cUsageIdentifier = 0;
		EnhkeyUsage.rgpszUsageIdentifier=NULL;
		CertUsage.dwType = USAGE_MATCH_TYPE_AND;
		CertUsage.Usage  = EnhkeyUsage;
		ChainPara.cbSize = sizeof(CERT_CHAIN_PARA);
		ChainPara.RequestedUsage = CertUsage;

		__try
		{
			pCertContext = d->fCertEnumCertificatesInStore(hStore, NULL);
		}
		__except(TRUE)
		{
			pCertContext = NULL;
			PR_TRACE((0, prtIMPORTANT, TR "fCertEnumCertificatesInStore - exception"));
		}
		while (pCertContext)
		{
			BOOL bCert;
			PCCERT_CHAIN_CONTEXT     pChainContext = NULL;
			CERT_CHAIN_FIND_BY_ISSUER_PARA find_para;
			CHAR szNameString[128];
			if(!d->fCertGetNameStringA(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, szNameString, sizeof(szNameString)))
				strcpy(szNameString, "<unknown>");
			

			memset(&find_para, 0, sizeof(find_para));
			find_para.cbSize = sizeof(find_para);

			__try
			{
				bCert = d->fCertGetCertificateChain(NULL, pCertContext, NULL, NULL, &ChainPara, CERT_CHAIN_DISABLE_AUTH_ROOT_AUTO_UPDATE | CERT_CHAIN_CACHE_ONLY_URL_RETRIEVAL /*| CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY*/, NULL, &pChainContext);
			}
			__except(TRUE)
			{
				bCert = FALSE;
				PR_TRACE((0, prtIMPORTANT, TR "fCertGetCertificateChain - exception"));
			}
			
			if (!bCert)
			{
				//PR_TRACE((0, prtERROR, TR "GetCertificateChain for '%s' failed with result %d", szNameString, GetLastError()));
			}
			else
			{
				BOOL bWin2000 = (d->vi.dwMajorVersion == 5 && d->vi.dwMinorVersion == 0);
				CERT_CHAIN_POLICY_PARA params;
				CERT_CHAIN_POLICY_STATUS status;
				memset(&status, 0, sizeof(status));
				status.cbSize = sizeof(status);
				memset(&params, 0, sizeof(params));
				params.cbSize = sizeof(params);
				
				if (!d->fCertVerifyCertificateChainPolicy(bWin2000 ? CERT_CHAIN_POLICY_BASE :  CERT_CHAIN_POLICY_MICROSOFT_ROOT, pChainContext, &params, &status))
				{
					//PR_TRACE((0, prtERROR, TR "VerifyCertificateChainPolicy for '%s' failed with %d", szNameString, GetLastError()));
				}
				else
				{
					if (status.dwError == 0)
					{
						//PR_TRACE((0, prtIMPORTANT, TR "VerifyCertificateChainPolicy for '%s' succeeded with result %d", szNameString, status.dwError));
						if (bWin2000 && 0 != strcmp(szNameString, "Microsoft Root Authority"))
							bRes = FALSE;
						else
							bRes = TRUE;
					}
					else
					{
						//PR_TRACE((0, prtIMPORTANT, TR "VerifyCertificateChainPolicy for '%s' failed with result %d", szNameString, status.dwError));
					}
				}
				d->fCertFreeCertificateChain(pChainContext);
			}
			if (bRes)
				break;
			pCertContext = d->fCertEnumCertificatesInStore(hStore, pCertContext);
		}	
		if (pCertContext)
			d->fCertFreeCertificateContext(pCertContext);
		d->fCertCloseStore(hStore, 0);
	}
	return bRes;
}

BOOL WINAPI VTIsMicrosoftSigned(HVERIFY hVerify, const WCHAR* wcsFName, wchar_t ** l_ppOutCatFileName)
{
	BOOL bRes = FALSE;
	HANDLE hFile;
	BYTE my_hash[0x100];
	DWORD hash_size = sizeof(my_hash);
	HCATADMIN hCatAdmin = NULL;
	tVerifyTrustData* d = hVerify;

	if (!hVerify)
		return FALSE;
	
	if (!d)
		return FALSE;

	if (!iResolveAPI(d))
		return FALSE;

	hFile = CreateFileW(wcsFName, FILE_GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		PR_TRACE((0, prtERROR, TR "VTIsMicrosoftSigned: open file '%S' failed with %d", wcsFName, GetLastError()));
		return FALSE;
	}
	if (d->m_bCacheContext)
	{
		EnterCriticalSection(&d->m_sync);
		if (!d->m_hCatAdmin)
		{
			if (!d->fCryptCATAdminAcquireContext(&d->m_hCatAdmin, NULL, 0))
				d->m_hCatAdmin = NULL;
		}
		if (d->m_hCatAdmin)
			InterlockedIncrement(&d->m_nCatAdminRefCount);
		LeaveCriticalSection(&d->m_sync);
		hCatAdmin = d->m_hCatAdmin;
	}
	else
	{
		if (!d->fCryptCATAdminAcquireContext(&hCatAdmin, NULL, 0))
			hCatAdmin = NULL;
	}
	if (!hCatAdmin)
	{
		PR_TRACE((0, prtERROR, TR "VTIsMicrosoftSigned: Cannot aquire hCatAdmin error=%d", GetLastError()));
	}
	else
	{
		if (!d->fCryptCATAdminCalcHashFromFileHandle(hFile, &hash_size, my_hash, 0))
		{
			PR_TRACE((0, prtERROR, TR "VTIsMicrosoftSigned: CalcHash for '%S' failed with 0x%x (size %d)", 
				wcsFName, GetLastError(), hash_size));
		}
		else
		{
			HCATINFO hCatInfo;
			CATALOG_INFO ci;
			hCatInfo = d->fCryptCATAdminEnumCatalogFromHash(hCatAdmin, my_hash, hash_size, 0, 0);
			if (!hCatInfo)
			{
				PR_TRACE((0, prtIMPORTANT, TR "VTIsMicrosoftSigned: hash for '%S' not found in catalogs, err=%d", wcsFName, GetLastError()));
			}
			else
			{
				ci.cbStruct = sizeof ci;
				if (!d->fCryptCATCatalogInfoFromContext(hCatInfo, &ci, 0))
				{
					PR_TRACE((0, prtERROR, TR "VTIsMicrosoftSigned: CatalogInfoFromContext failed"));
				}
				else
				{
					DWORD i;
					PR_TRACE((0, prtIMPORTANT, TR "VTIsMicrosoftSigned: hash for '%S' found in catalog '%S'", wcsFName, ci.wszCatalogFile));
					//l_ppOutCatFileName
					for (i=0; i<d->m_nCatCacheCount; i++)
					{
						if (!d->m_pCatCache)
							break;
						if (!d->m_pCatCache[i])
							continue;
						if (0 == wcscmp(d->m_pCatCache[i], ci.wszCatalogFile))
						{
							PR_TRACE((0, prtIMPORTANT, TR "VTIsMicrosoftSigned: catalog '%S' found in trusted catalogs cache", ci.wszCatalogFile));
							bRes = TRUE;
							break;
						}
					}
					if (!bRes)
					{
						bRes = iIsMicrosoftSignedStore(hVerify, ci.wszCatalogFile);
						if (bRes)
						{
							d->m_nCatCacheCount;
							if (!d->m_pCatCache)
								d->m_pCatCache = (WCHAR**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WCHAR*));
							else
								d->m_pCatCache = (WCHAR**)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, d->m_pCatCache, sizeof(WCHAR*)*(d->m_nCatCacheCount+1));
							if (d->m_pCatCache)
							{
								d->m_pCatCache[d->m_nCatCacheCount] = (WCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WCHAR)*(wcslen(ci.wszCatalogFile)+1));
								if (d->m_pCatCache[d->m_nCatCacheCount])
									wcscpy(d->m_pCatCache[d->m_nCatCacheCount], ci.wszCatalogFile);
								d->m_nCatCacheCount++;
							}
						}
					}
				}
				d->fCryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
			}
		}
		if (!d->m_bCacheContext)
			d->fCryptCATAdminReleaseContext(hCatAdmin, 0);
		else
			InterlockedDecrement(&d->m_nCatAdminRefCount);

	}
	CloseHandle(hFile);
	if (bRes)
		PR_TRACE((0, prtIMPORTANT, TR "VTIsMicrosoftSigned: file '%S' is trusted", wcsFName));
	return bRes;
}

void WINAPI VTDropCachedContext(HVERIFY hVerify)
{
	tVerifyTrustData* d = hVerify;
	if (!d)
		return;
	if (!d->m_bCacheContext)
		return;
	EnterCriticalSection(&d->m_sync);
	if (d->m_hCatAdmin && d->m_nCatAdminRefCount==0)
	{
		d->fCryptCATAdminReleaseContext(d->m_hCatAdmin, 0);
		d->m_hCatAdmin = NULL;
	}
	LeaveCriticalSection(&d->m_sync);
}

/*
bool WinVerifyTrust(const WCHAR * pwszFilePath, BOOL fUseUI)
{
    bool fRet = false;
    HRESULT hr;
    GUID guidPublishedSoftware = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA wintrustdata;
    WINTRUST_FILE_INFO fileinfo;
//    DWORD cb;

    memset(&wintrustdata, 0x00, sizeof wintrustdata);

    wintrustdata.cbStruct            = sizeof wintrustdata;
    wintrustdata.fdwRevocationChecks = WTD_REVOKE_NONE;
    wintrustdata.dwStateAction       = WTD_STATEACTION_IGNORE;
    wintrustdata.hWVTStateData       = NULL;
    wintrustdata.pwszURLReference    = NULL;
    wintrustdata.dwUIChoice          = fUseUI ? WTD_UI_ALL : WTD_UI_NONE;
    wintrustdata.dwProvFlags         = WTD_SAFER_FLAG | WTD_REVOCATION_CHECK_NONE;
    wintrustdata.dwUnionChoice       = WTD_CHOICE_FILE;
    wintrustdata.pFile               = &fileinfo;

    // Valid dwProvFlags are:
    // WTD_USE_IE4_TRUST_FLAG
    // WTD_NO_IE4_CHAIN_FLAG 
    // WTD_NO_POLICY_USAGE_FL
    // WTD_REVOCATION_CHECK_NONE
    // WTD_REVOCATION_CHECK_END_CERT
    // WTD_REVOCATION_CHECK_CHAIN
    // WTD_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT

	GUID subject = WIN_TRUST_SUBJTYPE_RAW_FILEEX ;
	memset(&fileinfo, 0x00, sizeof fileinfo);
    fileinfo.cbStruct          = sizeof fileinfo;
    fileinfo.pcwszFilePath     = pwszFilePath;
    fileinfo.pgKnownSubject    = NULL; //&subject;
    fileinfo.hFile             = NULL;

    hr = fWinVerifyTrust(0, &guidPublishedSoftware, &wintrustdata);
    if (SUCCEEDED(hr))
	    fRet = true;

	return fRet;
}

*/
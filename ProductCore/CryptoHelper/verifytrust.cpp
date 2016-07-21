// -------------------------------------------

#include <Prague/prague.h>
#include <stdio.h>
#include "verifytrust.h"

// -------------------------------------------
cStrObj GetDefaultProvider();

CMSVerifyTrust::CMSVerifyTrust()
{
#define _RESOLVE_API(module, name) if( m_bAPIResolved ) \
	if( !(*(void**)&f##name = GetProcAddress(module, #name)) ) m_bAPIResolved = FALSE;

	m_hCatAdmin = NULL;
	m_hWintrustDll = LoadLibraryA("WINTRUST");
	m_hCrypt32Dll = LoadLibraryA("CRYPT32");
	m_bAPIResolved = m_hWintrustDll && m_hCrypt32Dll;

	_RESOLVE_API(m_hWintrustDll, WinVerifyTrust);
	_RESOLVE_API(m_hWintrustDll, CryptCATAdminAcquireContext);
	_RESOLVE_API(m_hWintrustDll, CryptCATAdminCalcHashFromFileHandle);
	_RESOLVE_API(m_hWintrustDll, CryptCATAdminEnumCatalogFromHash);
	_RESOLVE_API(m_hWintrustDll, CryptCATCatalogInfoFromContext);
	_RESOLVE_API(m_hWintrustDll, CryptCATAdminReleaseCatalogContext);
	_RESOLVE_API(m_hWintrustDll, CryptCATAdminReleaseContext);
	_RESOLVE_API(m_hCrypt32Dll,  CertOpenStore);
	_RESOLVE_API(m_hCrypt32Dll,  CertEnumCertificatesInStore);
	_RESOLVE_API(m_hCrypt32Dll,  CertGetCertificateChain);
	_RESOLVE_API(m_hCrypt32Dll,  CertFindChainInStore);
	_RESOLVE_API(m_hCrypt32Dll,  CertVerifyCertificateChainPolicy);
	_RESOLVE_API(m_hCrypt32Dll,  CertFreeCertificateChainEngine);
	_RESOLVE_API(m_hCrypt32Dll,  CertFreeCertificateChain);
	_RESOLVE_API(m_hCrypt32Dll,  CertFreeCertificateContext);
	_RESOLVE_API(m_hCrypt32Dll,  CertCloseStore);
	_RESOLVE_API(m_hCrypt32Dll,  CertGetNameStringA);
	_RESOLVE_API(m_hCrypt32Dll,  CryptGetMessageSignerCount);
	_RESOLVE_API(m_hCrypt32Dll,  CryptVerifyMessageSignature);

	if( m_bAPIResolved)
		fCryptCATAdminAcquireContext(&m_hCatAdmin, NULL, 0);

	PR_TRACE((0, prtIMPORTANT, "VerifyTrust\tCMSVerifyTrust()"));
}

CMSVerifyTrust::~CMSVerifyTrust()
{
	if( m_hCatAdmin)
		fCryptCATAdminReleaseContext(m_hCatAdmin, 0);

	if( m_hWintrustDll)
		FreeLibrary(m_hWintrustDll);

	if( m_hCrypt32Dll)
		FreeLibrary(m_hCrypt32Dll);

	PR_TRACE((0, prtIMPORTANT, "VerifyTrust\t~CMSVerifyTrust()"));
}

BOOL CMSVerifyTrust::VerifySertContext(PCCERT_CONTEXT ppCertContext)
{
    CHAR szNameString[256] = ""; 
    fCertGetNameStringA( ppCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, szNameString, 128 );

	CERT_CHAIN_PARA ChainPara;
	ChainPara.cbSize = sizeof( CERT_CHAIN_PARA );
	ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
	ChainPara.RequestedUsage.Usage.cUsageIdentifier = 0;
	ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = NULL;
	
	PCCERT_CHAIN_CONTEXT pChainContext = NULL;
    if( !fCertGetCertificateChain(NULL, ppCertContext, NULL, NULL, &ChainPara, CERT_CHAIN_DISABLE_AUTH_ROOT_AUTO_UPDATE | CERT_CHAIN_CACHE_ONLY_URL_RETRIEVAL /*| CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY*/, NULL, &pChainContext) )
	{
		PR_TRACE((0, prtERROR, "VerifyTrust\tGetCertificateChain for '%s' failed with result %d", szNameString, GetLastError()));
		return FALSE;
	}

    CERT_CHAIN_POLICY_PARA params;
    CERT_CHAIN_POLICY_STATUS status;
    status.cbSize = sizeof( CERT_CHAIN_POLICY_STATUS );
	memset(&params, 0, sizeof( CERT_CHAIN_POLICY_PARA ));
    params.cbSize = sizeof( CERT_CHAIN_POLICY_PARA );
	params.dwFlags = CERT_CHAIN_POLICY_ALLOW_UNKNOWN_CA_FLAG;

	BOOL bRes = FALSE;
	if( !fCertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_MICROSOFT_ROOT, pChainContext, &params, &status))
		PR_TRACE((0, prtERROR, "VerifyTrust\tVerifyCertificateChainPolicy for '%s' failed with %d", szNameString, GetLastError()));
	else if( status.dwError != 0)
		PR_TRACE((0, prtIMPORTANT, "VerifyTrust\tVerifyCertificateChainPolicy for '%s' failed with result %d", szNameString, status.dwError));
	else
	{
		PR_TRACE((0, prtIMPORTANT, "VerifyTrust\tVerifyCertificateChainPolicy for '%s' succeeded with result %d", szNameString, status.dwError));
		bRes = TRUE;
	}
	
	fCertFreeCertificateChain(pChainContext);
	return bRes;
}

BOOL CMSVerifyTrust::VerifyFile1(const WCHAR* wcsFName)
{
	BOOL      bRes = FALSE;
	HANDLE    hFile = NULL, hFileMap = NULL;
    DATA_BLOB SignedBlob = {NULL, 0};

	WIN32_FILE_ATTRIBUTE_DATA rcFileAttributes;
	if( !GetFileAttributesExW(wcsFName, GetFileExInfoStandard, &rcFileAttributes) )
		return FALSE;

	SignedBlob.cbData = rcFileAttributes.nFileSizeLow;
	hFile = CreateFileW(wcsFName, FILE_GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if( hFile != INVALID_HANDLE_VALUE )
		hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, SignedBlob.cbData, NULL);

	if( hFileMap )
		SignedBlob.pbData = (PBYTE)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);

	if( SignedBlob.pbData )
	{
		CRYPT_VERIFY_MESSAGE_PARA pVerifyPara;
		pVerifyPara.cbSize = sizeof( CRYPT_VERIFY_MESSAGE_PARA );
		pVerifyPara.dwMsgAndCertEncodingType = X509_ASN_ENCODING | PKCS_7_ASN_ENCODING;
		pVerifyPara.hCryptProv = 0;
		pVerifyPara.pfnGetSignerCertificate = NULL;
		pVerifyPara.pvGetArg = NULL;
		PCRYPT_VERIFY_MESSAGE_PARA ppVerifyPara = &pVerifyPara;
		
		LONG sigcnt = fCryptGetMessageSignerCount( pVerifyPara.dwMsgAndCertEncodingType,
			SignedBlob.pbData, SignedBlob.cbData );
		
		for(int i = 0; i < sigcnt && !bRes; i++)
		{
			PCCERT_CONTEXT ppCertContext;
			BOOL bSigValid = fCryptVerifyMessageSignature( ppVerifyPara, i,
				SignedBlob.pbData, SignedBlob.cbData, NULL, NULL, &ppCertContext );

			if( ppCertContext )
			{
				if( bSigValid )
					bRes = VerifySertContext(ppCertContext);
				fCertFreeCertificateContext(ppCertContext);
			}
		}

		UnmapViewOfFile(SignedBlob.pbData);
	}

	if( hFileMap )
		CloseHandle(hFileMap);

	if( hFile != INVALID_HANDLE_VALUE )
		CloseHandle(hFile);
    return bRes;
}

BOOL CMSVerifyTrust::VerifyFile2(const WCHAR* wcsFName)
{
	BOOL bRes = FALSE;
	HCERTSTORE hStore = fCertOpenStore(CERT_STORE_PROV_FILENAME_W, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
			CERT_STORE_OPEN_EXISTING_FLAG|CERT_STORE_READONLY_FLAG, wcsFName);

	if( !hStore )
	{
		PR_TRACE((0, prtIMPORTANT, "VerifyTrust\tOpen file '%S' as secure store failed with %d", wcsFName, GetLastError()));
		return FALSE;
	}

	PCCERT_CONTEXT pCertContext = NULL;
	while( !bRes && (pCertContext = fCertEnumCertificatesInStore(hStore, pCertContext)) )
		bRes = VerifySertContext(pCertContext);

	if( pCertContext )
		fCertFreeCertificateContext(pCertContext);

	fCertCloseStore(hStore, 0);
	return bRes;
}

BOOL CMSVerifyTrust::IsMicrosoftSigned(const WCHAR* wcsFName)
{
	BOOL         bRes = FALSE;
	HCATINFO     hCatInfo = NULL;
	HCERTSTORE   hStore = NULL;
	HANDLE       hFile = INVALID_HANDLE_VALUE;
	BYTE         bfHash[0x100];
	DWORD        dwHash = sizeof(bfHash), pos;
	cStrObj      strFile;

	if( !m_hCatAdmin )
		return FALSE;

	hFile = CreateFileW(wcsFName, FILE_GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if( hFile == INVALID_HANDLE_VALUE)
	{
		PR_TRACE((0, prtERROR, "VerifyTrust\tVTIsMicrosoftSigned: open file '%S' failed with %d", wcsFName, GetLastError()));
		goto cleanup;
	}

	if( !fCryptCATAdminCalcHashFromFileHandle(hFile, &dwHash, bfHash, 0) )
	{
		PR_TRACE((0, prtERROR, "VerifyTrust\tVTIsMicrosoftSigned: CalcHash for '%S' failed with 0x%x (size %d)", wcsFName, GetLastError(), dwHash));
		goto cleanup;
	}

	if( !(hCatInfo = fCryptCATAdminEnumCatalogFromHash(m_hCatAdmin, bfHash, dwHash, 0, 0)) )
	{
		DWORD* pdwHash = (DWORD*)bfHash;
		PR_TRACE((0, prtIMPORTANT, "VerifyTrust\tVTIsMicrosoftSigned: hash for '%S' not found in catalogs, hash=%08X%08X%08X%08X, err=%d", wcsFName, pdwHash[0], pdwHash[1], pdwHash[2], pdwHash[3], GetLastError()));
		goto cleanup;
	}

	CATALOG_INFO ci;
	ci.cbStruct = sizeof ci;
	if( !fCryptCATCatalogInfoFromContext(hCatInfo, &ci, 0) )
	{
		PR_TRACE((0, prtERROR, "VerifyTrust\tVTIsMicrosoftSigned: CatalogInfoFromContext failed"));
		goto cleanup;
	}

	strFile = ci.wszCatalogFile;
	pos = m_vecCache.find(strFile);
	if( !(bRes = (pos != -1)) )
		if( bRes = VerifyFile2(strFile.data()) ) 
			m_vecCache.push_back(strFile);

cleanup:
	if( hCatInfo )
		fCryptCATAdminReleaseCatalogContext(m_hCatAdmin, hCatInfo, 0);

	if( hFile != INVALID_HANDLE_VALUE )
		CloseHandle(hFile);

	if( bRes )
		PR_TRACE((0, prtIMPORTANT, "VerifyTrust\tVTIsMicrosoftSigned: file '%S' is trusted", wcsFName));

	return bRes;
}

// -------------------------------------------

#define MY_ENCODING_TYPE (X509_ASN_ENCODING|PKCS_7_ASN_ENCODING)

CMSDataStore::CMSDataStore(const CHAR* szStorage, DWORD dwVersion)
{
	m_dwProps = (dwVersion << 6) + CERT_FIRST_USER_PROP_ID;
	m_hStore = NULL;
	m_hCertCtx = NULL;

	CHAR szCertStr[100];
	sprintf(szCertStr, "CN=%s", szStorage);

	DWORD cbName;
	if( !CertStrToName(MY_ENCODING_TYPE, szCertStr, CERT_OID_NAME_STR, NULL, NULL, &cbName, NULL) )
	{
		PR_TRACE((0, prtERROR, "crypt\tDataStore: CertStrToName %s failed with 0x%x", szCertStr, GetLastError()));
		return;
	}

	CERT_BLOB blob;
	blob.cbData = cbName;
	blob.pbData = (BYTE*)malloc(cbName);
	if( !CertStrToName(MY_ENCODING_TYPE, szCertStr, CERT_OID_NAME_STR, NULL, blob.pbData, &blob.cbData, NULL) )
	{
		PR_TRACE((0, prtERROR, "crypt\tDataStore: CertStrToName failed with 0x%x", GetLastError()));
		free(blob.pbData);
		return;
	}

	PR_TRACE((0, prtIMPORTANT, "crypt\tDataStore: Try call CertOpenSystemStore"));
	m_hStore = CertOpenSystemStore(NULL, "SPC");

	if( m_hStore )
		m_hCertCtx = CertFindCertificateInStore(m_hStore, MY_ENCODING_TYPE, 0,
			CERT_FIND_ISSUER_NAME, &blob, NULL);
	else
		PR_TRACE((0, prtERROR, "crypt\tDataStore: CertOpenSystemStore failed with 0x%x", GetLastError()));

	if( m_hCertCtx )
	{
		PR_TRACE((0, prtIMPORTANT, "crypt\tDataStore: Cert found"));
		free(blob.pbData);
		return;
	}

	PR_TRACE((0, prtIMPORTANT, "crypt\tDataStore: Cert not found"));

	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL;
	PCCERT_CONTEXT pCertCtx = NULL;
	CERT_PUBLIC_KEY_INFO* pbPublicKeyInfo = NULL;
	PBYTE pbSignedEncodedCert = NULL, pbEncodedCert = NULL;
	DWORD cbPublicKeyInfo = 0, cbEncodedCertSize = 0;

	DWORD dwOSVersion = GetVersion();
	cStrObj prov = GetDefaultProvider();
	PR_TRACE((0, prtIMPORTANT, "crypt\tDataStore: Provider is %S", prov.data()));
	if( !(dwOSVersion & 0x80000000) && (DWORD)(LOBYTE(LOWORD(dwOSVersion))) > 4 )
	{
		CryptAcquireContext(&hCryptProv, NULL, prov.empty() ? NULL : (LPCSTR)prov.c_str(cCP_ANSI), PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
		PR_TRACE((0, prtIMPORTANT, "crypt\tDataStore: Acquire VERIFYCONTEXT %i", GetLastError()));
	}

	if( !hCryptProv )
	{
		CryptAcquireContext(&hCryptProv, szStorage, prov.empty() ? NULL : (LPCSTR)prov.c_str(cCP_ANSI), PROV_RSA_FULL, 0);
		PR_TRACE((0, prtIMPORTANT, "crypt\tDataStore: Acquire 0 %i", GetLastError()));
	}
	if( !hCryptProv )
	{
		CryptAcquireContext(&hCryptProv, szStorage, prov.empty() ? NULL : (LPCSTR)prov.c_str(cCP_ANSI), PROV_RSA_FULL, CRYPT_NEWKEYSET);
		PR_TRACE((0, prtIMPORTANT, "crypt\tDataStore: Acquire NEWKEYSET %i", GetLastError()));
	}

	if( !hCryptProv )
	{
		PR_TRACE((0, prtERROR, "crypt\tDataStore: Failed to acquire context"));
		free(blob.pbData);
		return;
	}

	if( !CryptGetUserKey(hCryptProv, AT_SIGNATURE, &hKey) )
	{
		if( GetLastError() == NTE_NO_KEY )
		{
			if( !CryptGenKey(hCryptProv, AT_SIGNATURE, CRYPT_EXPORTABLE, &hKey) )
				PR_TRACE((g_root, prtERROR, "crypt\tDataStore: CryptGenKey failed with 0x%x", GetLastError()));
		}
		else
			PR_TRACE((g_root, prtERROR, "crypt\tDataStore: CryptGetUserKey failed with 0x%x", GetLastError()));
	}

	if( hKey && !CryptExportPublicKeyInfo(hCryptProv, AT_SIGNATURE, MY_ENCODING_TYPE, NULL, &cbPublicKeyInfo) )
		PR_TRACE((0, prtERROR, "crypt\tDataStore: CryptExportPublicKeyInfo failed with 0x%x", GetLastError()));
	else
	{
		pbPublicKeyInfo = (CERT_PUBLIC_KEY_INFO*)malloc(cbPublicKeyInfo);
		CryptExportPublicKeyInfo(hCryptProv, AT_SIGNATURE, MY_ENCODING_TYPE, pbPublicKeyInfo, &cbPublicKeyInfo);
	}

	if( hKey )
		CryptDestroyKey(hKey);

	if( pbPublicKeyInfo )
	{
		SYSTEMTIME st; GetSystemTime(&st);

		CRYPT_ALGORITHM_IDENTIFIER sa;
		memset(&sa, 0, sizeof(CRYPT_ALGORITHM_IDENTIFIER));
		sa.pszObjId = szOID_OIWSEC_sha1RSASign;

		CERT_INFO ci;
		memset(&ci, 0, sizeof(CERT_INFO));
		ci.dwVersion = CERT_V3;
		ci.Issuer = blob;
		ci.Subject = blob;
		ci.SerialNumber = blob;
		SystemTimeToFileTime(&st, &ci.NotAfter);
		ci.NotBefore = ci.NotAfter;
		ci.SignatureAlgorithm = sa;
		ci.SubjectPublicKeyInfo = *pbPublicKeyInfo;

/*		if( CryptSignAndEncodeCertificate(hCryptProv, AT_SIGNATURE, MY_ENCODING_TYPE, X509_CERT_TO_BE_SIGNED, &ci,
			&sa, NULL, NULL, &cbEncodedCertSize) )
		{
			pbEncodedCert = (PBYTE)malloc(cbEncodedCertSize);
			CryptSignAndEncodeCertificate(hCryptProv, AT_SIGNATURE, MY_ENCODING_TYPE, X509_CERT_TO_BE_SIGNED, &ci,
				&sa, NULL, pbEncodedCert, &cbEncodedCertSize);
		}
		else
			PR_TRACE((0, prtERROR, "crypt\tDataStore: CryptSignAndEncodeCertificate failed with 0x%x", GetLastError()));
*/
		if( CryptEncodeObject(MY_ENCODING_TYPE, X509_CERT_TO_BE_SIGNED, &ci, NULL, &cbEncodedCertSize) )
		{
			pbSignedEncodedCert = (PBYTE)malloc(cbEncodedCertSize);
			CryptEncodeObject(MY_ENCODING_TYPE, X509_CERT_TO_BE_SIGNED, &ci, pbSignedEncodedCert, &cbEncodedCertSize);
		}
		else
			PR_TRACE((0, prtERROR, "crypt\tDataStore: CryptEncodeObject failed with 0x%x", GetLastError()));

		if( pbSignedEncodedCert )
		{
			CERT_BLOB blob;
			blob.cbData = cbEncodedCertSize;
			blob.pbData = pbSignedEncodedCert;
			if( !CryptQueryObject(CERT_QUERY_OBJECT_BLOB, &blob, CERT_QUERY_CONTENT_FLAG_ALL, 
					CERT_QUERY_FORMAT_FLAG_ALL, 0, NULL, NULL, NULL, NULL, NULL, (const void**)&pCertCtx) )
				PR_TRACE((0, prtERROR, "crypt\tDataStore: CryptQueryObject failed with 0x%x", GetLastError()));
		}

/*		if( pbSignedEncodedCert )
		{
			CERT_SIGNED_CONTENT_INFO csci;
			memset(&csci, 0, sizeof(CERT_SIGNED_CONTENT_INFO));
			csci.ToBeSigned.cbData = cbEncodedCertSize;
			csci.ToBeSigned.pbData = pbSignedEncodedCert;
			csci.SignatureAlgorithm = sa;

			if( CryptEncodeObject(MY_ENCODING_TYPE, X509_CERT, &csci, NULL, &cbEncodedCertSize) )
			{
				pbEncodedCert = (PBYTE)malloc(cbEncodedCertSize);
				CryptEncodeObject(MY_ENCODING_TYPE, X509_CERT, &csci, pbEncodedCert, &cbEncodedCertSize);
			}
			else
			{
				PR_TRACE((0, prtERROR, "crypt\tDataStore: CryptEncodeObject(X509_CERT) failed with 0x%x", GetLastError()));
				FILE* f = fopen("c:\\crypt.cer", "w");
				fwrite(csci.ToBeSigned.pbData, csci.ToBeSigned.cbData, 1, f);
				fclose(f);
			}
		}

		if( pbEncodedCert )
			if( !(pCertCtx = CertCreateCertificateContext(MY_ENCODING_TYPE, pbEncodedCert, cbEncodedCertSize)) )
				PR_TRACE((0, prtERROR, "crypt\tDataStore: CertCreateCertificateContext failed with 0x%x", GetLastError()));
*/
		if( pCertCtx )
		{
			if( !CertAddCertificateContextToStore(m_hStore, pCertCtx, CERT_STORE_ADD_NEW, &m_hCertCtx) )
				PR_TRACE((0, prtERROR, "crypt\tDataStore: CertAddCertificateContextToStore failed with 0x%x", GetLastError()));

			CertFreeCertificateContext(pCertCtx);
		}
	}

	if( hCryptProv )
		CryptReleaseContext(hCryptProv, 0);

	if( pbPublicKeyInfo )
		free(pbPublicKeyInfo);

	if( pbSignedEncodedCert )
		free(pbSignedEncodedCert);

	if( pbEncodedCert )
		free(pbEncodedCert);

	free(blob.pbData);
}

CMSDataStore::~CMSDataStore()
{
	if( m_hCertCtx )
		CertFreeCertificateContext(m_hCertCtx);

	if( m_hStore )
		CertCloseStore(m_hStore, 0);
}

BOOL CMSDataStore::GetData(DWORD dwId, PVOID data, DWORD* size)
{
	if( !m_hCertCtx )
		return FALSE;

	if( CertGetCertificateContextProperty(m_hCertCtx, m_dwProps + dwId, data, size) )
		return TRUE;
	GetLastError();
	return FALSE;
}

BOOL CMSDataStore::StoreData(DWORD dwId, PVOID data, DWORD size)
{
	if( !m_hCertCtx )
		return FALSE;

	CERT_BLOB blob;
	blob.cbData = size;
	blob.pbData = (PBYTE)data;
	return CertSetCertificateContextProperty(m_hCertCtx, m_dwProps + dwId, 0, &blob);
}

tDWORD CMSDataStore::GetFirstID()
{
	tDWORD dwRet = 0;

	if (m_hCertCtx)
	{
		dwRet = CertEnumCertificateContextProperties(m_hCertCtx, 0);
		while (dwRet > 0 && (dwRet < m_dwProps || dwRet > CERT_LAST_USER_PROP_ID))
		{
			dwRet = CertEnumCertificateContextProperties(m_hCertCtx, dwRet);
		}
		if (dwRet > 0)
			dwRet -= m_dwProps;
	}
	
	return dwRet;
}

tDWORD CMSDataStore::GetNextID(tDWORD prevID)
{
	tDWORD dwRet = 0;

	if (m_hCertCtx)
	{
		dwRet = m_dwProps + prevID;
		do
		{
			dwRet = CertEnumCertificateContextProperties(m_hCertCtx, dwRet);
		}
		while (dwRet > 0 && (dwRet < m_dwProps || dwRet > CERT_LAST_USER_PROP_ID));
		if (dwRet > 0)
			dwRet -= m_dwProps;
	}
	
	return dwRet;
}

tDWORD CMSDataStore::GetPastLastID()
{
	return 0;
}


// -------------------------------------------

void XORString(tBYTE* szFrom, tDWORD sizeFrom, tBYTE* szTo, tDWORD* pSizeTo)
{
	static char g_szXOR[] = "rcsUpPRl349xAMc0VpjiiERMaT57aDNZNP9jpsdWJ31iPUaEwuycVy4E58LtP1fiE=?8?.$??.¬??.";

	tDWORD i = 0;
	for (; i < sizeFrom && i < *pSizeTo; i++)
	{
		if (i < sizeof(g_szXOR))
			szTo[i] = szFrom[i] ^ g_szXOR[i];
		else
			szTo[i] = szFrom[i] ^ 33;
	}
	*pSizeTo = i;
}






cSerRegistrar     * g_serializable_registrar_head = NULL;

struct cHelperStoreData : public cSerializable
{
	DECLARE_IID( cHelperStoreData, cSerializable, PID_CRYPTOHELPER, cnst_cHelperStoreData );

	cVector<cHelperStoreDataItem>	m_items;
};





CHelperDataStore::CHelperDataStore(const cStrObj& sFileName) : m_sFileName(sFileName)
{
	tERROR err = errOK;
	for(const cSerRegistrar* tmp = g_serializable_registrar_head; PR_SUCC(err) && tmp; tmp = tmp->m_next)
		err = ::g_root->RegisterSerializableDescriptor(tmp->m_dsc);
}

CHelperDataStore::~CHelperDataStore()
{
}

void CHelperDataStore::Load()
{
	cAutoObj<cRegistry> reg;
	cHelperStoreData data;
	tERROR err = ::g_root->sysCreateObject((hOBJECT*)&reg, IID_REGISTRY, PID_DTREG);
	if (PR_SUCC(err))
		err = m_sFileName.copy(reg, pgOBJECT_NAME);
	if (PR_SUCC(err))
		err = reg->sysCreateObjectDone();
	if (PR_SUCC(err))
	{
		cHelperStoreData* pData = &data;
		err = g_root->RegDeserialize((cSerializable **)&pData, reg, NULL, data.eIID);
	}
	if (PR_SUCC(err) && !data.m_items.empty())
	{
		m_itemsMap.clear();
		for (tSIZE_T idx = 0; idx < data.m_items.size(); ++idx)
		{
			cHelperStoreDataItem& storeItem = data.m_items.at(idx);
			XORHelperStoreDataItem(storeItem);

			m_itemsMap.insert(std::make_pair(storeItem.m_dwId, storeItem));
		}
	}
}

void CHelperDataStore::Save()
{
	cHelperStoreData data;
	for (CItemsMap::const_iterator item = m_itemsMap.begin(); item != m_itemsMap.end(); ++item)
	{
		data.m_items.push_back(item->second);

		cHelperStoreDataItem& storeItem = data.m_items.at(data.m_items.size() - 1);
		XORHelperStoreDataItem(storeItem);
	}
	if (!data.m_items.empty())
	{
		cAutoObj<cRegistry> reg;
		tERROR err = ::g_root->sysCreateObject((hOBJECT*)&reg, IID_REGISTRY, PID_DTREG);
		if (PR_SUCC(err))
		{
			reg->propSetDWord(pgCLEAN, cTRUE);
			reg->propSetDWord(pgOBJECT_RO, cFALSE);
			reg->propSetDWord(pgSAVE_RESULTS_ON_CLOSE, cTRUE);
			err = m_sFileName.copy(reg, pgOBJECT_NAME);
		}
		if (PR_SUCC(err))
			err = reg->sysCreateObjectDone();
		if (PR_SUCC(err))
			err = g_root->RegSerialize(&data, data.eIID, reg, NULL);
	}
}

void CHelperDataStore::ReadMSStore(CMSDataStore& store)
{
	tDWORD dwID = store.GetFirstID();
	if (dwID != store.GetPastLastID())
	{
        DWORD dwSize = 0;
	    tBYTE* btBuff = NULL;
		do 
		{
			if (PR_SUCC(store.GetData(dwID, NULL, &dwSize))
				&& (btBuff = reinterpret_cast<tBYTE*>(malloc(static_cast<size_t>(dwSize))))
				&& (PR_SUCC(store.GetData(dwID, btBuff, &dwSize))))
			{
				StoreData(dwID, btBuff, dwSize, TRUE);
			}
            if (btBuff)
			{
                free(btBuff);
				btBuff = NULL;
			}
			dwID = store.GetNextID(dwID);
		}
		while (dwID != store.GetPastLastID());
	}
}

BOOL CHelperDataStore::GetData(DWORD dwId, PVOID data, DWORD* size)
{
	BOOL bRet = FALSE;

	CItemsMap::const_iterator item = m_itemsMap.find(dwId);
	if (item != m_itemsMap.end())
	{
		if (*size == 0)
		{
			*size = item->second.m_dwSize;
			bRet = TRUE;
		}
		else if (data && *size > 0 && *size <= item->second.m_dwSize)
		{
			memcpy(data, item->second.m_data.data(), item->second.m_dwSize);
			*size = item->second.m_dwSize;
			bRet = TRUE;
		}
		else
		{
			*size = 0;
		}
	}

	return bRet;
}

BOOL CHelperDataStore::StoreData(DWORD dwId, PVOID data, DWORD size, BOOL bUpdateIfExists)
{
	BOOL bRet = FALSE;

	if (data)
	{
		CItemsMap::iterator item = m_itemsMap.find(dwId);
		if (item == m_itemsMap.end())
		{
			cHelperStoreDataItem newItem;
			newItem.m_dwId = dwId;
			item = m_itemsMap.insert(std::make_pair(dwId, newItem)).first;
			item->second.m_dwSize = size;
			if (item->second.m_data.size() < item->second.m_dwSize)
				item->second.m_data.resize(item->second.m_dwSize);
			memcpy(item->second.m_data.data(), data, item->second.m_dwSize);
		}
		else if (bUpdateIfExists)
		{
			item->second.m_dwSize = size;
			if (item->second.m_data.size() < item->second.m_dwSize)
				item->second.m_data.resize(item->second.m_dwSize);
			memcpy(item->second.m_data.data(), data, item->second.m_dwSize);
		}
		bRet = TRUE;
	}

	return bRet;
}

void CHelperDataStore::XORHelperStoreDataItem(cHelperStoreDataItem& item)
{
	tDWORD size = 0;
	tBYTE* pData = 0;

	size = sizeof(tDWORD);
	pData = reinterpret_cast<tBYTE*>(&item.m_dwId);
	XORString(pData, size, pData, &size);
	size = sizeof(tDWORD);
	pData = reinterpret_cast<tBYTE*>(&item.m_dwSize);
	XORString(pData, size, pData, &size);
	if (!item.m_data.empty())
	{
		size = item.m_data.size() * sizeof(item.m_data.at(0));
		pData = reinterpret_cast<tBYTE*>(item.m_data.data());
		XORString(pData, size, pData, &size);
	}
}


//////////////////////////////////////////////////////////////////////////

#undef  __pr_serializable_h
#define __DECLARE_SERIALIZABLE

#include <Prague/pr_serializable.h>

IMPLEMENT_SERIALIZABLE_BEGIN( cHelperStoreDataItem, 0 )
	SER_VALUE( m_dwId, tid_DWORD, "ItemID" )
	SER_VALUE( m_dwSize,  tid_DWORD, "ItemSize" )
	SER_VECTOR( m_data,  tid_CHAR, "ItemData" )
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( cHelperStoreData, 0 )
	SER_VECTOR( m_items,  cHelperStoreDataItem::eIID, "Items" )
IMPLEMENT_SERIALIZABLE_END()

#undef __DECLARE_SERIALIZABLE


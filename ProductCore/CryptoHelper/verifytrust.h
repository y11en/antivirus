#ifndef _VERIFY_TRUST_C_
#define _VERIFY_TRUST_C_

#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <softpub.h>
#include <Mscat.h>
#include <map>

#include <Prague/pr_vector.h>
#include <ProductCore/plugin/p_cryptohelper.h>

// -------------------------------------------

class CMSVerifyTrust
{
public:
	CMSVerifyTrust();
	~CMSVerifyTrust();

	BOOL IsMicrosoftSigned(const WCHAR* wcsFName);

private:
	BOOL VerifyFile1(const WCHAR* wcsFName);
	BOOL VerifyFile2(const WCHAR* wcsFName);
	BOOL VerifySertContext(PCCERT_CONTEXT ppCertContext);

private:
	BOOL      m_bAPIResolved;
	HMODULE	  m_hWintrustDll;
	HMODULE	  m_hCrypt32Dll;
	HCATADMIN m_hCatAdmin;
	cVector<cStrObj> m_vecCache;

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
	LONG (WINAPI *fCryptGetMessageSignerCount)(DWORD dwMsgEncodingType, const BYTE *pbSignedBlob, DWORD cbSignedBlob);
	BOOL (WINAPI *fCryptVerifyMessageSignature)(PCRYPT_VERIFY_MESSAGE_PARA pVerifyPara, DWORD dwSignerIndex, const BYTE *pbSignedBlob, DWORD cbSignedBlob, BYTE *pbDecoded, DWORD *pcbDecoded, PCCERT_CONTEXT *ppSignerCert);
};

// -------------------------------------------

class CMSDataStore
{
public:
	CMSDataStore(const CHAR* szStorage, DWORD dwVersion);
	~CMSDataStore();

	BOOL GetData(DWORD dwId, PVOID data, DWORD* size);
	BOOL StoreData(DWORD dwId, PVOID data, DWORD size);

	tDWORD GetFirstID();
	tDWORD GetNextID(tDWORD prevID);
	tDWORD GetPastLastID();

private:
	HCERTSTORE     m_hStore;
	PCCERT_CONTEXT m_hCertCtx;
	DWORD          m_dwProps;
};

// -------------------------------------------

void XORString(tBYTE* szFrom, tDWORD sizeFrom, tBYTE* szTo, tDWORD* pSizeTo);



enum _eCryptoHelperStructs_IID
{
	cnst_cHelperStoreDataItem = 0x001,
	cnst_cHelperStoreData = 0x002
};

struct cHelperStoreDataItem : public cSerializable
{
	cHelperStoreDataItem() : 
		m_dwId(0),
		m_dwSize(0)
	{}

	DECLARE_IID( cHelperStoreDataItem, cSerializable, PID_CRYPTOHELPER, cnst_cHelperStoreDataItem );

	tDWORD			m_dwId;
	tDWORD			m_dwSize;
	cVector<tCHAR>	m_data;
};



class CHelperDataStore
{
public:
	CHelperDataStore(const cStrObj& sFileName);
	~CHelperDataStore();

	void Load();
	void Save();
	void ReadMSStore(CMSDataStore& store);
	BOOL GetData(DWORD dwId, PVOID data, DWORD* size);
	BOOL StoreData(DWORD dwId, PVOID data, DWORD size, BOOL bUpdateIfExists);

protected:
	typedef std::map<tDWORD, cHelperStoreDataItem> CItemsMap;

	void XORHelperStoreDataItem(cHelperStoreDataItem& item);

	cStrObj			m_sFileName;
	CItemsMap		m_itemsMap;
};


#endif // _VERIFY_TRUST_C_
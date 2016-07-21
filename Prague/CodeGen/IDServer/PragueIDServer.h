// PragueIDServer.h : Declaration of the CPragueIDServer

#ifndef __PRAGUEIDSERVER_H_
#define __PRAGUEIDSERVER_H_

#include "resource.h"       // main symbols
#include "sql\_sql.h"
#include <property/property.h>
#include <Stuff\RegStorage.h>
#include "ApartThread.h"


// как мы получаем адрес SQL'ного сервака где лежит база "Prague" с идентификаторами
// 1. берем значение переменной окружения "PragueIDServer"
// 2. ищем в ини-файле "PragueIDServer.ini" в текущем папке ключ "SQLAddr" в секции "PragueIDServer"
// 3. ищем в ини-файле "PragueIDServer.Ini" в папке стартования PVE ключ "SQLAddr" в секции "PragueIDServer"
// 4. берем значение "SQLAddr" в ключе "hklm\software\kasperskylab\PragueIDServer"
// 5. берем дефалтовое значение PRAGUE_SERVER_DEF_ADDR
#define PRAGUE_SERVER_DEF_ADDR      "db6.avp.ru" //"mom-db.avp.ru" //"medvedev\\medvedev"
#define PRAGUE_ID_SERVER_KEY        "PragueIDServer"
#define PRAGUE_SERVER_CFG_FILE      PRAGUE_ID_SERVER_KEY ".ini"
#define PRAGUE_REG_SERVER_KEY       "software\\kasperskylab\\" PRAGUE_ID_SERVER_KEY
#define PRAGUE_SERVER_ADDR_VAL      "SQLAddr"
#define PRAGUE_ID_CONTAINER_VALUE   "IDContainer"

int get_prague_server_address( char* addr_buff, int buff_size );

/////////////////////////////////////////////////////////////////////////////

class CIdServer
{
public:
	virtual	HRESULT GetVendorName(DWORD dwSize, char *pVendorName) = 0;
	virtual HRESULT GetVendorMnemonicID(DWORD dwSize, char *pMnemonicID) = 0;
	virtual HRESULT GetErrorString(HRESULT hErr, char **pErrString) = 0;
	virtual HRESULT CheckIDInUse(PgIDCategories eCategory, DWORD dwID) = 0;
	virtual HRESULT SetIDInUse(PgIDCategories eCategory, DWORD dwID) = 0;
	virtual HRESULT ReleaseID(PgIDCategories eCategory, DWORD dwID) = 0;
	virtual HRESULT GetID(PgIDCategories eCategory, DWORD *pdwID) = 0;
};

class CRegistryIdServer : public CIdServer
{
	CRegStorage		m_RegStorage;
	HDATA			m_hRootData;
	HPROP			m_hCurIdProp;
	DWORD			m_hCurIdIndex;
	
	void	SaveContainer();
	HDATA	FindCategoryRoot( PgIDCategories eCategory );
	
public:
	DWORD	FindFirstId(PgIDCategories eCategory);
	DWORD	FindNextId();
	
	static BOOL PerformRegister();

	CRegistryIdServer();
	~CRegistryIdServer();
	
	virtual HRESULT GetVendorName(DWORD dwSize, char *pVendorName);
	virtual HRESULT GetVendorMnemonicID(DWORD dwSize, char *pMnemonicID);
	virtual HRESULT GetErrorString(HRESULT hErr, char **pErrString);
	virtual HRESULT CheckIDInUse(PgIDCategories eCategory, DWORD dwID);
	virtual HRESULT SetIDInUse(PgIDCategories eCategory, DWORD dwID);
	virtual HRESULT ReleaseID(PgIDCategories eCategory, DWORD dwID);
	virtual HRESULT GetID(PgIDCategories eCategory, DWORD *pdwID);
};

class CResourceReleaser;
class CSqlIdServer : public CIdServer
{
	CSql	m_oSql;
	int		m_nUserId;
	CResourceReleaser* m_pReleaser;
	
	//  operations
	BOOL	GetUserLogin		(TCHAR* o_strUserLogin, DWORD i_dwBufLen);
	BOOL	Connect				();

public:
	BOOL	m_bSqlFailure;
	
	CSqlIdServer();
	~CSqlIdServer();
	
	int		GetUserId			();
	BOOL	GetCurrentUserId	();
	BOOL	GetIntSqlSelect		(int& o_nRet, TCHAR* i_strSql, ...);
	BOOL	GetStringSqlSelect	(TCHAR* o_strRet, DWORD i_dwBufLen, TCHAR* i_strSql, ...);
	
	virtual HRESULT GetVendorName(DWORD dwSize, char *pVendorName);
	virtual HRESULT GetVendorMnemonicID(DWORD dwSize, char *pMnemonicID);
	virtual HRESULT GetErrorString(HRESULT hErr, char **pErrString);
	virtual HRESULT CheckIDInUse(PgIDCategories eCategory, DWORD dwID);
	virtual HRESULT SetIDInUse(PgIDCategories eCategory, DWORD dwID);
	virtual HRESULT ReleaseID(PgIDCategories eCategory, DWORD dwID);
	virtual HRESULT GetID(PgIDCategories eCategory, DWORD *pdwID);

	friend class CResourceReleaser;
};

/////////////////////////////////////////////////////////////////////////////
// CPragueIDServer
class ATL_NO_VTABLE CPragueIDServer : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPragueIDServer, &CLSID_PragueIDServer>,
	public IPragueIDServer
{
	BOOL		m_bSqlSource;
	CIdServer*	m_pIdServer;

	HRESULT UpdateSqlFromRegistry();
	void	SetSqlIdServer();
	void	SetRegistryIdServer();
	void	ReleaseServer();

public:
	CPragueIDServer();
	~CPragueIDServer();

DECLARE_REGISTRY_RESOURCEID(IDR_PRAGUEIDSERVER)
DECLARE_NOT_AGGREGATABLE(CPragueIDServer)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPragueIDServer)
	COM_INTERFACE_ENTRY(IPragueIDServer)
END_COM_MAP()

// IPragueIDServer
public:
	STDMETHOD(GetVendorName)(/*[in]*/ DWORD dwSize, /*[in,out,size_is(dwSize)]*/ BYTE *pVendorName);
	STDMETHOD(GetVendorMnemonicID)(/*[in]*/ DWORD dwSize, /*[out,size_is(,dwSize)]*/ BYTE *pMnemonicID);
	STDMETHOD(GetErrorString)(/*[in]*/ HRESULT hErr, /*[out,string]*/ BYTE **pErrString);
	STDMETHOD(CheckIDInUse)(/*[in]*/ PgIDCategories eCategory, /*[in]*/ DWORD dwID);
	STDMETHOD(SetIDInUse)(/*[in]*/ PgIDCategories eCategory, /*[in]*/ DWORD dwID);
	STDMETHOD(ReleaseID)(/*[in]*/ PgIDCategories eCategory, /*[in]*/ DWORD dwID);
	STDMETHOD(GetID)(/*[in]*/ PgIDCategories eCategory, /*[out]*/ DWORD *pdwID);
	STDMETHOD(ForceRegistryUse)(/*[in]*/ DWORD dwSet);
};

class CResourceReleaser : public CApartThread
{
	CSqlIdServer*	m_pSqlServer;
	DWORD			m_dwStartTicks;

public:
	CResourceReleaser(CSqlIdServer* i_pSqlServer);
	virtual void Run();
};


#endif //__PRAGUEIDSERVER_H_

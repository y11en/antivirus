// PragueIDServer.cpp : Implementation of CPragueIDServer
#include "stdafx.h"
#include "..\IDServerInterface.h"
#include "..\PIDSError.h"
#include <datatreeutils/dtutils.h>
#include <Stuff\CPointer.h>
#include "SerDeser.h"
#include "AVPPisID.h"
#include "PragueIDServer.h"
#include "RegisterDlg.h"



CResourceReleaser::CResourceReleaser(CSqlIdServer* i_pSqlServer)
{
	m_pSqlServer = i_pSqlServer;
}
void CResourceReleaser::Run()
{
	m_dwStartTicks = GetTickCount();

	while(!IsRestartQueried())
	{
		DWORD dwCurTime = GetTickCount();

		if(1000*25 < (dwCurTime - m_dwStartTicks))
		{
			//m_pSqlServer->m_oSql.DropConnect();
			return;
		}

		Sleep(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPragueIDServer
// ---
CPragueIDServer::CPragueIDServer() 
{
	m_bSqlSource = TRUE;
	m_pIdServer = NULL;

	SetSqlIdServer();
}

// ---
CPragueIDServer::~CPragueIDServer() 
{
	ReleaseServer();
}

HRESULT CPragueIDServer::UpdateSqlFromRegistry()
{
	static BOOL s_bUpadtedFromRegistry = FALSE;
	if(s_bUpadtedFromRegistry)
		return S_OK;
	s_bUpadtedFromRegistry = TRUE; 

	CRegistryIdServer oRegIds;
	CSqlIdServer* pSqlIds = (CSqlIdServer*)m_pIdServer;
				   
	if(!pSqlIds->GetCurrentUserId())
		return pSqlIds->m_bSqlFailure ? PIDS_E_SQL : PIDS_E_USER_NOT_REGISTERED;
	
	// check if database not yet updated
	int tmp;
	if(!pSqlIds->GetIntSqlSelect(tmp, _T("select UpdatedFromRegistry from users where userid=%d"), pSqlIds->GetUserId()))
		return PIDS_E_SQL;
	
	if(1 == tmp)
		return S_OK;

	DWORD i;

	for(i=oRegIds.FindFirstId(pic_Plugin); i != -1; i=oRegIds.FindNextId())
	{
		HRESULT hRes = pSqlIds->CheckIDInUse(pic_Plugin, i);
		if(S_OK != hRes && S_FALSE != hRes)
			return PIDS_E_SQL;

		if(S_FALSE == hRes)
			pSqlIds->SetIDInUse(pic_Plugin, i);
	}

	for(i=oRegIds.FindFirstId(pic_Interface); i != -1; i=oRegIds.FindNextId())
	{
		HRESULT hRes = pSqlIds->CheckIDInUse(pic_Interface, i);
		if(S_OK != hRes && S_FALSE != hRes)
			return PIDS_E_SQL;

		if(S_FALSE == hRes)
			pSqlIds->SetIDInUse(pic_Interface, i);
	}
	
	if(!pSqlIds->GetIntSqlSelect(tmp, _T("update users set UpdatedFromRegistry=1 where userid=%d"), pSqlIds->GetUserId()))
		return PIDS_E_SQL;

	return S_OK;
}

void	CPragueIDServer::SetSqlIdServer()
{
	ReleaseServer();
	m_bSqlSource = TRUE;
	m_pIdServer = new CSqlIdServer;
}

void	CPragueIDServer::SetRegistryIdServer()
{
	ReleaseServer();
	m_bSqlSource = FALSE;
	m_pIdServer = new CRegistryIdServer;
}

void	CPragueIDServer::ReleaseServer()
{
	if(m_bSqlSource && m_pIdServer)
		delete ((CSqlIdServer*)m_pIdServer);

	if(!m_bSqlSource && m_pIdServer)
		delete ((CRegistryIdServer*)m_pIdServer);

	m_pIdServer = NULL;
}
							   
// ---
STDMETHODIMP CPragueIDServer::ForceRegistryUse(DWORD dwSet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// set registry data source
	if(m_bSqlSource && dwSet)
		SetRegistryIdServer();
	else
	if(!m_bSqlSource && !dwSet)
		SetSqlIdServer();

	return S_OK;
}

// ---
STDMETHODIMP CPragueIDServer::GetID( PgIDCategories eCategory, DWORD *pdwID ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return m_pIdServer->GetID(eCategory, pdwID);
}

// ---
STDMETHODIMP CPragueIDServer::ReleaseID( PgIDCategories eCategory, DWORD dwID ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return m_pIdServer->ReleaseID(eCategory, dwID);
}

// ---
STDMETHODIMP CPragueIDServer::SetIDInUse( PgIDCategories eCategory, DWORD dwID ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return m_pIdServer->SetIDInUse(eCategory, dwID);
}

// ---
STDMETHODIMP CPragueIDServer::CheckIDInUse( PgIDCategories eCategory, DWORD dwID ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return m_pIdServer->CheckIDInUse(eCategory, dwID);	
}

// ---
STDMETHODIMP CPragueIDServer::GetVendorMnemonicID( DWORD dwSize, BYTE *pMnemonicID ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return m_pIdServer->GetVendorMnemonicID(dwSize, (char*)pMnemonicID);
}

// ---
STDMETHODIMP CPragueIDServer::GetVendorName( DWORD dwSize, BYTE *pVendorName ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if(m_bSqlSource && m_pIdServer)
	{
		HRESULT hRes = UpdateSqlFromRegistry();
		if(FAILED(hRes))
			return hRes;
	}
	return m_pIdServer->GetVendorName(dwSize, (char*)pVendorName);
}

// ---
STDMETHODIMP CPragueIDServer::GetErrorString( HRESULT hErr, BYTE **pErrString ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return m_pIdServer->GetErrorString(hErr, (char**)pErrString);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//
//		CRegistryIdServer
//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// ---
struct CatTableItem {
	PgIDCategories  m_eCategory;
	AVP_dword			  m_pAddr[2];
};

// ---
static CatTableItem gCatTable[] = {
	{ pic_Interface,	{ PI_PID_INTERFACEID, 0 } },
	{ pic_Plugin,			{ PI_PID_PLUGINID,		0 } },
};


static int gCatTableCount = sizeof(gCatTable) / sizeof(gCatTable[0]);

/////////////////////////////////////////////////////////////////////////////
// CPragueIDServer
// ---
CRegistryIdServer::CRegistryIdServer() :
	m_RegStorage( PRAGUE_ID_SERVER_KEY ),
	m_hRootData( NULL ) {

	BYTE *pBuffer = NULL;
	DWORD dwSize = 0;
	if ( m_RegStorage.GetValue(PRAGUE_ID_CONTAINER_VALUE, (BYTE *)NULL, dwSize) ) {
		pBuffer = new BYTE[dwSize];
		if ( m_RegStorage.GetValue(PRAGUE_ID_CONTAINER_VALUE, pBuffer, dwSize) ) {
			m_hRootData = ::DeserializeDTree( pBuffer, dwSize );
		}
		delete [] pBuffer;
	}
}

// ---
CRegistryIdServer::~CRegistryIdServer() {
	if ( m_hRootData )
		::DATA_Remove( m_hRootData, NULL );
	m_hRootData = NULL;
}

// ---
void CRegistryIdServer::SaveContainer() 
{
	DWORD dwSize = 0;
	AVP_char *pBuffer = ::SerializeDTree( m_hRootData, dwSize );
	if ( pBuffer ) {
		m_RegStorage.PutValue( PRAGUE_ID_CONTAINER_VALUE, (BYTE *)pBuffer, dwSize );
		delete [] pBuffer;
	}
}

// ---
BOOL CRegistryIdServer::PerformRegister() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CRegisterDlg rcRegister;
	return rcRegister.DoModal() == IDOK;
}

// ---
HDATA	CRegistryIdServer::FindCategoryRoot( PgIDCategories eCategory ) 
{
	for ( int i=0; i<gCatTableCount; i++ ) {
		if ( gCatTable[i].m_eCategory == eCategory ) {
			return ::DATA_Find( m_hRootData, gCatTable[i].m_pAddr );
		}
	}
	return NULL;
}

// ---
static void GetIDRange( HDATA hData, DWORD &dwFrom, DWORD &dwTo ) 
{
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, PI_PID_IDRANGE );
	if ( hProp ) {
		dwFrom = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, 0 );
		dwTo   = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, 1 );
	}
}

// ---
HRESULT CRegistryIdServer::GetID( PgIDCategories eCategory, DWORD *pdwID ) 
{
	if ( pdwID ) {
		if ( m_hRootData ) {
			HPROP hProp;
			switch ( eCategory ) {
				case pic_Vendor : 
					hProp = ::DATA_Find_Prop( m_hRootData, NULL, NULL );
					if ( hProp ) 
						*pdwID = ::DTUT_GetPropNumericValueAsDWord( hProp );
					return S_OK;
				default :
					HDATA hData = FindCategoryRoot( eCategory );
					if ( hData ) {
						hProp = ::DATA_Find_Prop( hData, NULL, NULL );
						if ( hProp ) {
							DWORD dwFrom, dwTo;
							::GetIDRange( hData, dwFrom, dwTo );

							AVP_dword dwCount = ::PROP_Arr_Count( hProp );
							AVP_dword i, d;
							for ( i=0,d=dwFrom; i<dwCount && d<dwTo; i++,d++ ) {
								AVP_dword dwContID = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, i );
								if ( dwContID != d ) {
									dwContID = d;
									::PROP_Arr_Insert( hProp, i, &dwContID, sizeof(dwContID) );
									*pdwID = dwContID;
									SaveContainer();
									return S_OK;
								}
							}
							if ( d < dwTo ) {
								::DTUT_SetPropArrayNumericValue( hProp, PROP_ARR_LAST, d );
								*pdwID = d;
								SaveContainer();
								return S_OK;
							}
							else
								return PIDS_E_EMPTYRANGE;
						}
						return PIDS_E_INTERNALERROR;
					}
					return PIDS_E_INCORRECTCATEGORY;
			}
		}
		return PIDS_E_OUTOFORDER;
	}
  return E_INVALIDARG;
}

// ---
HRESULT CRegistryIdServer::ReleaseID( PgIDCategories eCategory, DWORD dwID ) 
{
	if ( m_hRootData ) {
		HDATA hData = FindCategoryRoot( eCategory );
		if ( hData ) {
			DWORD dwFrom, dwTo;
			::GetIDRange( hData, dwFrom, dwTo );
			if ( dwID >= dwFrom && dwID < dwTo ) {
				HPROP hProp = ::DATA_Find_Prop( hData, NULL, NULL );
				if ( hProp ) {
					AVP_dword dwCount = ::PROP_Arr_Count( hProp );
					for ( AVP_dword i=0; i<dwCount; i++ ) {
						AVP_dword dwContID = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, i );
						if ( dwContID == dwID ) {
							::PROP_Arr_Remove( hProp, i, 1 );
							SaveContainer();
							return S_OK;
						}
					}
					return S_OK;
				}
				return PIDS_E_INTERNALERROR;
			}
			return PIDS_E_OUTOFRANGE;
		}
		return PIDS_E_INCORRECTCATEGORY;
	}

	return PIDS_E_OUTOFORDER;
}

// ---
HRESULT CRegistryIdServer::SetIDInUse( PgIDCategories eCategory, DWORD dwID ) 
{
	if ( m_hRootData ) {
		HDATA hData = FindCategoryRoot( eCategory );
		if ( hData ) {
			DWORD dwFrom, dwTo;
			::GetIDRange( hData, dwFrom, dwTo );
			if ( dwID >= dwFrom && dwID < dwTo ) {
				HPROP hProp = ::DATA_Find_Prop( hData, NULL, NULL );
				if ( hProp ) {
					AVP_dword dwCount = ::PROP_Arr_Count( hProp );
					for ( AVP_dword i=0; i<dwCount; i++ ) {
						AVP_dword dwContIDLow = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, i );
						if ( dwContIDLow == dwID ) {
							return S_OK;
						}
						if ( i < (dwCount - 1) ) {
							AVP_dword dwContIDHigh = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, i + 1 );
							if ( dwContIDLow < dwID && dwID < dwContIDHigh ) {
								::PROP_Arr_Insert( hProp, i + 1, &dwID, sizeof(dwID) );
								SaveContainer();
								return S_OK;
							}
						}
					}
					::DTUT_SetPropArrayNumericValue( hProp, PROP_ARR_LAST, dwID );
					SaveContainer();
					return S_OK;
				}
				return PIDS_E_INTERNALERROR;
			}
			return PIDS_E_OUTOFRANGE;
		}
		return PIDS_E_INCORRECTCATEGORY;
	}

	return PIDS_E_OUTOFORDER;
}

// ---
HRESULT CRegistryIdServer::CheckIDInUse( PgIDCategories eCategory, DWORD dwID ) 
{
	if ( m_hRootData ) {
		HDATA hData = FindCategoryRoot( eCategory );
		if ( hData ) {
			DWORD dwFrom, dwTo;
			::GetIDRange( hData, dwFrom, dwTo );
			if ( dwID >= dwFrom && dwID < dwTo ) 
			{
				HPROP hProp = ::DATA_Find_Prop( hData, NULL, NULL );
				if ( hProp ) {
					AVP_dword dwCount = ::PROP_Arr_Count( hProp );
					for ( AVP_dword i=0; i<dwCount; i++ ) 
					{
						AVP_dword dwContID = 
							::DTUT_GetPropArrayNumericValueAsDWord( hProp, i );

						if ( dwContID == dwID ) 
							return S_OK;
					}
					return S_FALSE;
				}
				return PIDS_E_INTERNALERROR;
			}
			return PIDS_E_OUTOFRANGE;
		}
		return PIDS_E_INCORRECTCATEGORY;
	}

	return PIDS_E_OUTOFORDER;
}

// ---
HRESULT CRegistryIdServer::GetVendorMnemonicID( DWORD dwSize, char *pMnemonicID ) 
{
	if ( m_hRootData ) {
		HPROP hProp = ::DATA_Find_Prop( m_hRootData, NULL, PI_PID_VENDORMNEMONICID );
		if ( hProp ) {
			CAPointer<char> pName = ::DTUT_GetPropValueAsString( hProp, NULL );
			lstrcpyn( (char *)pMnemonicID, pName, dwSize );
			return S_OK;
		}
		return PIDS_E_INTERNALERROR;
	}

	return PIDS_E_OUTOFORDER;
}

// ---
HRESULT CRegistryIdServer::GetVendorName( DWORD dwSize, char *pVendorName ) 
{
	if ( m_hRootData ) {
		HPROP hProp = ::DATA_Find_Prop( m_hRootData, NULL, PI_PID_VENDORNAME );
		if ( hProp ) {
			CAPointer<char> pName = ::DTUT_GetPropValueAsString( hProp, NULL );
			lstrcpyn( (char *)pVendorName, pName, dwSize );
			return S_OK;
		}
		return PIDS_E_INTERNALERROR;
	}

	return PIDS_E_OUTOFORDER;
}

// ---
HRESULT CRegistryIdServer::GetErrorString( HRESULT hErr, char **pErrString ) 
{
  if ( !pErrString )
    return E_INVALIDARG;

	*pErrString = (char*)::CoTaskMemAlloc( 0x200 );
  if ( *pErrString ) {
    const char* pMsg = 0;
    switch( hErr ) {
			case PIDS_E_INTERNALERROR          : pMsg = "Internal interface error."; break;
			case PIDS_E_OUTOFORDER             : pMsg = "Interface is not ready for work due to some reason."; break;
			case PIDS_E_INCORRECTCATEGORY      : pMsg = "Incorrect ID category."; break;
			case PIDS_E_EMPTYRANGE             : pMsg = "No free ID value."; break;
			case PIDS_E_OUTOFRANGE             : pMsg = "The value is out of available range."; break;
			case PIDS_E_USER_NOT_REGISTERED    : pMsg = "User not registered"; break;
			case PIDS_E_SQL                    : pMsg = "SQL error"; break;
			default                            : pMsg = "unknown error"; break;
    }

    if ( pMsg ) {
      ::lstrcpy( (char*)*pErrString, pMsg );
      return S_OK;
    }
    else 
			if ( ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hErr, 0, (char*)*pErrString, 0x200, 0) )
				return S_OK;
			else
				return HRESULT_FROM_WIN32( ::GetLastError() );
  }
  else
    return E_OUTOFMEMORY;
}

DWORD	CRegistryIdServer::FindFirstId(PgIDCategories eCategory)
{
	m_hCurIdProp = NULL;
	m_hCurIdIndex = -1;

	if ( !m_hRootData ) 
		return -1;

	HDATA hData = FindCategoryRoot( eCategory );

	if(NULL == hData)
		return -1;

	m_hCurIdProp = ::DATA_Find_Prop( hData, NULL, NULL );
	if(NULL == m_hCurIdProp)
		return -1;

	AVP_dword dwCount = ::PROP_Arr_Count( m_hCurIdProp );

	if(0 == dwCount)
		return -1;

	m_hCurIdIndex = 0;

	return ::DTUT_GetPropArrayNumericValueAsDWord( m_hCurIdProp, m_hCurIdIndex++ );
}

DWORD CRegistryIdServer::FindNextId()
{
	if(NULL == m_hCurIdProp)
		return -1;

	AVP_dword dwCount = ::PROP_Arr_Count( m_hCurIdProp );

	if(m_hCurIdIndex >= dwCount)
		return -1;

	return ::DTUT_GetPropArrayNumericValueAsDWord( m_hCurIdProp, m_hCurIdIndex++ );
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//
//		CSqlIdServer
//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CSqlIdServer::CSqlIdServer()
{
	m_nUserId = -1;

	m_pReleaser = new CResourceReleaser(this);
	m_pReleaser->Restart();

	m_oSql.IgnoreNoDataResult();
}

CSqlIdServer::~CSqlIdServer()
{
	m_pReleaser->Terminate();
	delete m_pReleaser;
}

int CSqlIdServer::GetUserId()
{
	return m_nUserId;
}

BOOL CSqlIdServer::GetCurrentUserId	()
{
	if(m_nUserId != -1)
		return TRUE;
	
	TCHAR strUserLogin[128];
	if(GetUserLogin(strUserLogin, 128))
		GetIntSqlSelect(m_nUserId, _T("select UserId from Users where UserLogin='%s'"), strUserLogin);

	return m_nUserId != -1;
}

// ---
HRESULT CSqlIdServer::GetID( PgIDCategories eCategory, DWORD *pdwID ) 
{
	if(!GetCurrentUserId())
		return m_bSqlFailure ? PIDS_E_SQL : PIDS_E_USER_NOT_REGISTERED;

	int& nRet = * (int*)pdwID;

	switch(eCategory)
	{
	case pic_Vendor:
		if(!GetIntSqlSelect(nRet, _T("select VendorId from users where userid=%d"), m_nUserId))
			return PIDS_E_SQL;
		return S_OK;
		
	case pic_Plugin:
		if(!GetIntSqlSelect(nRet, _T("exec sp_AddNewPluginId %d"), m_nUserId))
			return PIDS_E_SQL;
	case pic_Interface:
		if(pic_Interface == eCategory)
		if(!GetIntSqlSelect(nRet, _T("exec sp_AddNewInterfaceId %d"), m_nUserId))
			return PIDS_E_SQL;

		if(-10==nRet)
			return PIDS_E_USER_NOT_REGISTERED;
		if(-1==nRet)
			return PIDS_E_SQL;
		
		return S_OK;
	}

	return PIDS_E_INCORRECTCATEGORY;
}

// ---
HRESULT CSqlIdServer::ReleaseID( PgIDCategories eCategory, DWORD dwID ) 
{
	if(!GetCurrentUserId())
		return m_bSqlFailure ? PIDS_E_SQL : PIDS_E_USER_NOT_REGISTERED;
		
	int nRet;

	switch(eCategory)
	{
	case pic_Plugin:
		if(!GetIntSqlSelect(nRet, _T("exec sp_ReleasePluginId %d"), m_nUserId, dwID))
			return PIDS_E_SQL;
		return S_OK;
	case pic_Interface:
		if(!GetIntSqlSelect(nRet, _T("exec sp_ReleaseInterfaceId %d"), m_nUserId, dwID))
			return PIDS_E_SQL;
		return S_OK;
	}
	
	return PIDS_E_INCORRECTCATEGORY;
}

// ---
HRESULT CSqlIdServer::SetIDInUse( PgIDCategories eCategory, DWORD dwID ) 
{
	if(!GetCurrentUserId())
		return m_bSqlFailure ? PIDS_E_SQL : PIDS_E_USER_NOT_REGISTERED;
		
	int nRet;
	switch(eCategory)
	{
	case pic_Plugin:
		if(!GetIntSqlSelect(nRet, _T("exec sp_ReservePluginId %d, %d"), m_nUserId, dwID))
			return PIDS_E_SQL;

	case pic_Interface:
		if(pic_Interface == eCategory)
		if(!GetIntSqlSelect(nRet, _T("exec sp_ReserveInterfaceId %d, %d"), m_nUserId, dwID))
			return PIDS_E_SQL;

		if(-10 == nRet)
			return PIDS_E_USER_NOT_REGISTERED;
			
		if(-5 == nRet)
			return PIDS_E_OUTOFRANGE;
			
		if(-1 == nRet)
			return PIDS_E_SQL;

		return (1==nRet) ? S_OK : S_FALSE;
	}
	
	return PIDS_E_INCORRECTCATEGORY;
}

// ---
HRESULT CSqlIdServer::CheckIDInUse( PgIDCategories eCategory, DWORD dwID ) 
{
	if(!GetCurrentUserId())
		return m_bSqlFailure ? PIDS_E_SQL : PIDS_E_USER_NOT_REGISTERED;
		
	int nRet;
	switch(eCategory)
	{
	case pic_Plugin:
		if(!GetIntSqlSelect(nRet, _T("exec sp_CheckPluginId %d, %d"), m_nUserId, dwID))
			return PIDS_E_SQL;
		return nRet ? S_OK : S_FALSE;
		
	case pic_Interface:
		if(!GetIntSqlSelect(nRet, _T("exec sp_CheckInterfaceId %d, %d"), m_nUserId, dwID))
			return PIDS_E_SQL;
		return nRet ? S_OK : S_FALSE;
	}
	
	return PIDS_E_INCORRECTCATEGORY;
}

// ---
HRESULT CSqlIdServer::GetVendorMnemonicID( DWORD dwSize, char *pMnemonicID ) 
{
	if(!GetCurrentUserId())
		return m_bSqlFailure ? PIDS_E_SQL : PIDS_E_USER_NOT_REGISTERED;
		
	if(!GetStringSqlSelect((char*)pMnemonicID, dwSize, 
		_T("select VendorMnemonicId from users where userid=%d"), m_nUserId))
		return PIDS_E_SQL;

	return S_OK;
}

// ---
HRESULT CSqlIdServer::GetVendorName( DWORD dwSize, char *pVendorName ) 
{
	if(!GetCurrentUserId())
		return m_bSqlFailure ? PIDS_E_SQL : PIDS_E_USER_NOT_REGISTERED;
		
	if(!GetStringSqlSelect((char*)pVendorName, dwSize, 
		_T("select VendorName from users where userid=%d"), m_nUserId))
		return PIDS_E_SQL;
	
	return S_OK;
}

// ---
HRESULT CSqlIdServer::GetErrorString( HRESULT hErr, char **pErrString ) 
{
	if ( !pErrString )
		return E_INVALIDARG;

	*pErrString = (char*)::CoTaskMemAlloc( 4000 );
	if (! *pErrString ) 
		return E_OUTOFMEMORY;
	
	char strErrorBuffer[4000] = "Unknown SQL error";
	if ( *pErrString ) {
		const char* pMsg = 0;
		switch( hErr ) {
			case PIDS_E_INTERNALERROR       : pMsg = "Internal error"; break;
			case PIDS_E_OUTOFORDER          : pMsg = "out of order"; break;
			case PIDS_E_INCORRECTCATEGORY   : pMsg = "Incorrect ID category."; break;
			case PIDS_E_EMPTYRANGE          : pMsg = "No free ID value."; break;
			case PIDS_E_OUTOFRANGE          : pMsg = "The value is out of available range."; break;
			case PIDS_E_USER_NOT_REGISTERED : pMsg = "User not registered in database."; break;	
			case PIDS_E_SQL                 : pMsg = strErrorBuffer; m_oSql.GetErrorString( strErrorBuffer, sizeof(strErrorBuffer) ); break;
			default                         : pMsg = "Unknown error"; break;
		}

		if ( pMsg ) 
			::lstrcpy( (char*)*pErrString, pMsg );
		else 
		{
			if ( !::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hErr, 0, (char*)*pErrString, 0x200, 0) )
				return HRESULT_FROM_WIN32( ::GetLastError() );
		}
	}

	return S_OK;
}

BOOL CSqlIdServer::GetUserLogin(TCHAR* o_strUserLogin, DWORD i_dwBufLen)
{
	return GetUserName(o_strUserLogin, &i_dwBufLen);
}

BOOL CSqlIdServer::GetIntSqlSelect(int& o_nRet, TCHAR* i_strSql, ...)
{
	if(!Connect())
		return FALSE;
	
	TCHAR strSql[1024];
	
	va_list ptr;						  
	va_start(ptr, i_strSql);
	_vsnprintf(strSql, 1024, i_strSql, ptr);
	va_end(ptr);
	
	CSqlRecordset oRS;		    
	if (!m_oSql.DoSqlQuery(strSql))
	{		
		m_bSqlFailure = TRUE;
		m_pReleaser->Restart();
		return FALSE;
	}

	if(!m_oSql.MoveNext(oRS))
	{
		m_pReleaser->Restart();
		return FALSE;
	}
	
	SFieldValue* pFld = oRS[DWORD(0)];
	
	if( !pFld || 
		pFld->vt != SFieldValue::VT_I4 || 
		!pFld->Value.pintVal)
	{
		m_bSqlFailure = TRUE;
		m_pReleaser->Restart();
		return FALSE;
	}	

	m_pReleaser->Restart();
	
	o_nRet = *pFld->Value.plVal;
	return TRUE;
}

BOOL CSqlIdServer::GetStringSqlSelect(TCHAR* o_strRet, DWORD i_dwBufLen, TCHAR* i_strSql, ...)
{
	if(!Connect())
		return FALSE;

	TCHAR strSql[1024];

	va_list ptr;						  
	va_start(ptr, i_strSql);
	_vsnprintf(strSql, 1024, i_strSql, ptr);
	va_end(ptr);

	CSqlRecordset oRS;		    
	if (!m_oSql.DoSqlQuery(strSql))
	{
		m_bSqlFailure = TRUE;
		m_pReleaser->Restart();
		return FALSE;
	}
	
	if (! m_oSql.MoveNext(oRS))
	{
		m_pReleaser->Restart();
		return FALSE;
	}
	
	SFieldValue* pFld = oRS[DWORD(0)];
		
	if( !pFld || 
		pFld->vt != SFieldValue::VT_CREF || 
		!pFld->Value.pcVal)
	{
		m_bSqlFailure = TRUE;
		m_pReleaser->Restart();
		return FALSE;
	}

	lstrcpyn(o_strRet, pFld->Value.pcVal, i_dwBufLen);

	m_pReleaser->Restart();
	return TRUE;
}

inline int _lstrcmp(char* szOne, char* szTwo)
{
	int nLen1 = lstrlenA(szOne);
	int nLen2 = lstrlenA(szTwo);
	
	if(nLen1 != nLen2)
		return 1;

	for(int i=0; i<nLen1; i++)
		if(szOne[i] != szTwo[i])
			return 1;

	return 0;
}

BOOL CSqlIdServer::Connect()
{
	m_pReleaser->QueryRestart();

	m_bSqlFailure = FALSE;

	if( !m_oSql.IsConnected() ) {
		char addr[MAX_PATH];
		if ( !get_prague_server_address(addr,sizeof(addr)) )
			m_bSqlFailure = TRUE;
		else
			m_bSqlFailure = !m_oSql.Trusted_Connect2( addr, _T("Prague"), _T("PragueIdServer"), NULL, TRUE);
	}
	
	return !m_bSqlFailure;
}


// ---
int get_prague_server_address( char* addr, int buff_size ) {
	int ret = 0;

	// 1. берем значение переменной окружения "PragueIDServer"
	ret = GetEnvironmentVariable( PRAGUE_ID_SERVER_KEY, addr, buff_size );
	if ( ret )
		return ret;

	// 2. ищем ини-файл "PragueIDServer.cfg" в текущем папке ( ключ "SQLAddr" в секции "PragueIDServer" )
	char path[2*MAX_PATH];
	DWORD len = GetCurrentDirectory( sizeof(path), path );
	if ( len ) {
		if ( path[len-1] != '\\' )
			path[len++] = '\\';
		memcpy( path+len, PRAGUE_SERVER_CFG_FILE, sizeof(PRAGUE_SERVER_CFG_FILE) );
		ret = GetPrivateProfileString( PRAGUE_ID_SERVER_KEY, PRAGUE_SERVER_ADDR_VAL, "", addr, buff_size, path );
		if ( ret )
			return ret;
	}

	// 3. ищем ини-файл "PragueIDServer.cfg" в папке откуда стартанул PVE ( ключ "SQLAddr" в секции "PragueIDServer" )
	len = GetModuleFileName( 0, path, sizeof(path) );
	if ( len ) {
		char* slash = strrchr( path, '\\' );
		if ( slash ) {
			strcpy( ++slash, PRAGUE_SERVER_CFG_FILE );
			ret = GetPrivateProfileString( PRAGUE_ID_SERVER_KEY, PRAGUE_SERVER_ADDR_VAL, "", addr, buff_size, path );
			if ( ret )
				return ret;
		}
	}

	// 4. берем значение value "SQLAddr" в ключе "PragueIDServer" в реестре
	DWORD dwRet = buff_size;
	CRegStorage	reg( HKEY_LOCAL_MACHINE, PRAGUE_REG_SERVER_KEY );
	if ( reg.GetValue(PRAGUE_SERVER_ADDR_VAL,(BYTE *)addr,dwRet) )
		return dwRet;

	// 5. берем дефалтовое значение PRAGUE_SERVER_DEF_ADDR
	if ( buff_size >= sizeof(PRAGUE_SERVER_DEF_ADDR) )
		memcpy( addr, PRAGUE_SERVER_DEF_ADDR, sizeof(PRAGUE_SERVER_DEF_ADDR) );
	return sizeof(PRAGUE_SERVER_DEF_ADDR);
}


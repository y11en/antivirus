// PragueIServer.cpp : Implementation of CPragueIServer
#include "stdafx.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include <property/property.h>
#include <Stuff\RegStorage.h>
#include <StuffIO\IOUtil.h>

#include "..\PGIServerInterface.h"
#include "..\PISrvError.h"
#include "PragueIServer.h"
#include "BrowseForFolder.h"

#define PRIS_REG_KEY									 _T("PragueInterfaceServer")
#define PRIS_REG_STARTINGFOLDER_VALUE	 _T("StartingFolder")


TCHAR *LoadAndFormatString( UINT uiStrID, ... );

// ---
static BOOL PerformFolderNameDialog( HWND hWndParent, CPathStr &rcFolderName ) {
	CBrowseForFolder rcDlg( hWndParent );

	char pCurrDir[_MAX_PATH];
	::GetModuleFileName( NULL, pCurrDir, sizeof(pCurrDir)	);

	if ( *rcFolderName ) 
		::IOSFullPathFromRelativePath( pCurrDir, rcFolderName, pCurrDir, sizeof(pCurrDir) );

	rcDlg.SetFlags( BIF_RETURNONLYFSDIRS );
	CAPointer<TCHAR> pTitle = ::LoadAndFormatString( IDS_SELECTFOLDERTITLE );
	rcDlg.SetTitle( pTitle );
	rcDlg.SetSelectedFolder(pCurrDir);

	if ( rcDlg.SelectFolder() ) {
		rcFolderName = rcDlg.GetSelectedFolder(); 
		if ( *(rcFolderName + lstrlen(rcFolderName) - 1) == L'\\' )
			*(rcFolderName + lstrlen(rcFolderName) - 1) = 0;
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPragueIServer

// ---
CPragueIServer::CPragueIServer() :
  m_hWndParent( NULL ), 
	m_StartingFolders( 3, 1, false ),
	m_Extentions( 3, 1, false),
	m_iface_cache(8,8) {

  ::DATA_Init_Library( malloc, free );
  ::DTUT_Init_Library( malloc, free );
	::KLDT_Init_Library( malloc, free );
	::LoadIO();

	m_StartingFolders.Add( (cMyPathStr*)&m_PrtFolder ); // Prt
	m_StartingFolders.Add( (cMyPathStr*)&m_ImpFolder );	// Imp
	m_StartingFolders.Add( (cMyPathStr*)&m_DscFolder );	// Dsc

//	m_Extentions.Add( _T("*.prt") );
//	m_Extentions.Add( _T("*.imp") );
//	m_Extentions.Add( _T("*.dsc") );

	m_Extentions.Add( _T("*.prt") ); 
	m_Extentions.Add( _T("*.prt") ); 
	m_Extentions.Add( _T("*.prt") );
}

// ---
CPragueIServer::~CPragueIServer() {
	m_PrtFolder = NULL;
	m_ImpFolder = NULL;
	m_DscFolder = NULL;
	for( uint i=0,c=m_iface_cache.Count(); i<c; ++i ) {
		::DATA_Remove( m_iface_cache[i], 0 );
		m_iface_cache[i] = 0;
	}
}


// ---
STDMETHODIMP CPragueIServer::SetUIParent( HWND hWndParent ) {
	m_hWndParent = hWndParent;

	return S_OK;
}

// ---
STDMETHODIMP CPragueIServer::Init() {
	*m_StartingFolders[0] = _T("");

	TCHAR pBuffer[_MAX_PATH] = _T("");
	CRegStorage rcRegStorage( PRIS_REG_KEY );
	rcRegStorage.GetValue( PRIS_REG_STARTINGFOLDER_VALUE, pBuffer, _MAX_PATH );

	*m_StartingFolders[0] = pBuffer;

	if ( !*LPCTSTR(*m_StartingFolders[0]) ) {
		if ( !m_hWndParent )
			return PISRV_E_UIOWNER;

		if ( ::PerformFolderNameDialog( m_hWndParent, *m_StartingFolders[0]) ) {
			rcRegStorage.PutValue( PRIS_REG_STARTINGFOLDER_VALUE, *m_StartingFolders[0] );
		}
		else
		 return PISRV_E_CONNECTION;
	}

	if ( ::IOSFileExists(*m_StartingFolders[0]) != IOS_ERR_OK )
		 return PISRV_E_CONNECTION;

//	*m_StartingFolders[2] = *m_StartingFolders[1] = *m_StartingFolders[0];
	*m_StartingFolders[2] = CPathStr("o:\\ppp\\include\\prt");
	*m_StartingFolders[1] = CPathStr("o:\\prague\\include\\prt");

	return CheckIntegrity();
}

// ---
STDMETHODIMP CPragueIServer::ReInit() {
	if ( ::PerformFolderNameDialog( m_hWndParent, *m_StartingFolders[0]) ) {
		CRegStorage rcRegStorage( PRIS_REG_KEY );
		rcRegStorage.PutValue( PRIS_REG_STARTINGFOLDER_VALUE, *m_StartingFolders[0] );
	}
	else
		return S_FALSE;

	if ( ::IOSFileExists(*m_StartingFolders[0]) != IOS_ERR_OK )
		 return PISRV_E_CONNECTION;

//	*m_StartingFolders[2] = *m_StartingFolders[1] = *m_StartingFolders[0];
	*m_StartingFolders[2] = CPathStr("o:\\ppp\\include\\prt");
	*m_StartingFolders[1] = CPathStr("o:\\prague\\include\\prt");

	return CheckIntegrity();
}

// ---
STDMETHODIMP CPragueIServer::Done() {
	return S_OK;
}



// ---
STDMETHODIMP CPragueIServer::LoadContainer(BYTE *pContainerName, BOOL bLockOnLoad, DWORD *pdwSize, BYTE **ppInterfaces) {
	CPathStr rcContainerName( (TCHAR *)pContainerName );
	if ( *rcContainerName )
		return LoadContainerInterfacesTree(rcContainerName, bLockOnLoad, itByContainer, pdwSize, ppInterfaces);

	return E_INVALIDARG;
}

// ---
STDMETHODIMP CPragueIServer::OpenContainer(BYTE **ppContainerName, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces) {
	if ( *m_StartingFolders[0] ) {
		if ( !m_hWndParent )
			return PISRV_E_UIOWNER;

		CPathStr rcContainerName(_MAX_PATH);
		rcContainerName =	( (TCHAR *)*ppContainerName );
		
		HRESULT hError = OpenContainerInterfacesTree(rcContainerName, bLockOnLoad, eIType, pdwSize, ppInterfaces);

		::CoTaskMemFree( *ppContainerName );
		*ppContainerName = (BYTE *)::CoTaskMemAlloc( lstrlen(rcContainerName) + 1 );
		lstrcpy( (char *)*ppContainerName,	rcContainerName );

		return hError;
	}

	return PISRV_E_HASNOTCONNECTED;
}

// ---
STDMETHODIMP CPragueIServer::SaveContainer(BYTE **ppContainerName, BOOL bSaveAs, BOOL bLockOnSave, DWORD dwSize, BYTE *pInterfaces) {
	if ( *m_StartingFolders[0] ) {
		CPathStr rcContainerName(_MAX_PATH);
		rcContainerName =	( (TCHAR *)*ppContainerName );

		HRESULT hError = SaveContainerInterfacesTree( bSaveAs, rcContainerName, bLockOnSave, itByContainer, dwSize, pInterfaces );

		::CoTaskMemFree( *ppContainerName );
		*ppContainerName = (BYTE *)::CoTaskMemAlloc( lstrlen(rcContainerName) + 1 );
		lstrcpy( (char *)*ppContainerName,	rcContainerName );

		return hError;
	}

	return PISRV_E_HASNOTCONNECTED;
}

// ---
STDMETHODIMP CPragueIServer::EnumInterfaces(ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces) {
	return LoadAllInterfacesTree(eIType, pdwSize, ppInterfaces);
}

// ---
STDMETHODIMP CPragueIServer::LoadInterface(DWORD dwUIDSize, BYTE *pUID, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterface) {
//	if ( dwUIDSize == sizeof(CNodeUID) || !pUID || !*pUID ) {
		CNodeUID rcUID( (TCHAR *)pUID, dwUIDSize );
		return LoadInterfaceTree( rcUID, bLockOnLoad, eIType, pdwSize, ppInterface);
//	}

	return E_INVALIDARG;
}

// ---
STDMETHODIMP CPragueIServer::SaveDescriptionDB(BYTE *pContainerName, DWORD dwSize, BYTE *pInterfaces) {
	CPathStr rcContainerName( (TCHAR *)pContainerName );
	return SaveInterfacesDB( rcContainerName, dwSize, pInterfaces );
}


// ---
STDMETHODIMP CPragueIServer::GetErrorString( HRESULT hError, BYTE **pErrString ) {
  if ( !pErrString )
    return E_INVALIDARG;

	*pErrString = (BYTE*)::CoTaskMemAlloc( _MAX_PATH << 1 );
  if ( *pErrString ) {
    CAPointer<TCHAR> rcMsg;
    switch( hError ) {
			case PISRV_E_INTERNALERROR           : rcMsg = ::LoadAndFormatString(IDS_E_INTERNALERROR); break;
      case PISRV_E_OUTOFORDER              : rcMsg = ::LoadAndFormatString(IDS_E_OUTOFORDER); break;
      case PISRV_E_HASNOTCONNECTED         : rcMsg = ::LoadAndFormatString(IDS_E_HASNOTCONNECTED); break;
      case PISRV_E_MEMORYALLOCATIONERROR   : rcMsg = ::LoadAndFormatString(IDS_E_MEMORYALLOCATIONERROR); break;
			case PISRV_E_DIFFERENTTYPES					 : rcMsg = ::LoadAndFormatString(IDS_E_DIFFERENTTYPES); break;
			case PISRV_E_SAVECONTAINER           : rcMsg = ::LoadAndFormatString(IDS_E_SAVECONTAINER, m_rcLastErrorStr); break;
			case PISRV_E_SAVERESERVECOPY         : rcMsg = ::LoadAndFormatString(IDS_E_SAVERESERVECOPY, m_rcLastErrorStr); break;
			case PISRV_E_CONNECTION							 : rcMsg = ::LoadAndFormatString(IDS_E_CONNECTION, LPCTSTR(*m_StartingFolders[0])); break;
			case PISRV_E_INTNOTFOUND						 : rcMsg = ::LoadAndFormatString(IDS_E_INTNOTFOUND, m_rcLastErrorStr); break;
			case PISRV_E_SAVEDESCRIPTIONDB			 : rcMsg = ::LoadAndFormatString(IDS_E_SAVEDESCRIPTIONDB, m_rcLastErrorStr); break;
			case PISRV_E_UIOWNER								 : rcMsg = ::LoadAndFormatString(IDS_E_UIOWNER); break;
    }

    if ( rcMsg && *rcMsg ) {
      ::lstrcpy( (char*)*pErrString, rcMsg );
      return S_OK;
    }
    else 
			if ( ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hError, 0, (char*)*pErrString, 0x200, 0) )
				return S_OK;
			else
				return HRESULT_FROM_WIN32( ::GetLastError() );
  }
  else
    return E_OUTOFMEMORY;
}

// ---
STDMETHODIMP CPragueIServer::LockContainer( BYTE *pContainerName, BOOL bLock ) {
	return S_OK;
}

// ---
STDMETHODIMP CPragueIServer::SetStartingFolder( BYTE *pStartingFolder, ItType eIType ) {
	*m_StartingFolders[FOLDER_INDEX(eIType)] = (TCHAR *)pStartingFolder;
	return S_OK;
}

// ---
STDMETHODIMP CPragueIServer::EnumContainers(ItType eIType, DWORD *pdwSize, BYTE **ppContainers) {
	return LoadAllContainersTree(eIType, pdwSize, ppContainers);
}

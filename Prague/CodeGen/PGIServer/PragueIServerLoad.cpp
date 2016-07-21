////////////////////////////////////////////////////////////////////
//
//  PragueIServerLoad.cpp
//  Load interfaces stuff implementation
//  Prague Interfaces Server
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include <property/property.h>
#include <AVPComID.h>
#include <Stuff\FFind.h>
#include <Stuff\PathStr.h>
#include <Stuff\PArray.h>
#include <StuffIO\IOUtil.h>

#include "..\AVPPveID.h"

#include "..\PGIServerInterface.h"
#include "..\PISrvError.h"
#include "PragueIServer.h"

TCHAR *LoadAndFormatString( UINT uiStrID, ... );

// ---
static BOOL IsAlreadyAttached( HDATA hDataToCheck, HDATA hRootData ) {
	if ( hDataToCheck && hRootData ) {
		CNodeUID rcUIDToCheck;
		if ( ::GetNodeUID(hDataToCheck, rcUIDToCheck) ) {

			HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
			while ( hCurrData ) {
				CNodeUID rcCurrUID;
				if ( ::GetNodeUID(hCurrData, rcCurrUID) &&
						 ::CompareNodesUID(rcUIDToCheck, rcCurrUID) ) {
					return TRUE;
				}
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}
	}
	return FALSE;
}

// ---
HRESULT CPragueIServer::AttachContainerInterfaces( LPCTSTR pContainerName, HDATA *phRootData, BOOL bSkipLinked, ItType eIType, CAliasContainer &rcAliases, BOOL bFullContainer ) {
	HRESULT hError = S_OK;
	HDATA hResultData = NULL;
	if ( ::KLDT_DeserializeUsingSWM(pContainerName, &hResultData) ) {
		if ( eIType == itAny || eIType == itByContainer || eIType == ::GetTreeSType(hResultData) ) {
			HDATA hWorkRoot = *phRootData;
			if ( hWorkRoot == NULL )
				hWorkRoot = *phRootData = ::DATA_Copy( NULL, NULL, hResultData, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
			else {
				if ( bFullContainer ) {
					hWorkRoot = ::DATA_Copy( NULL, NULL, hResultData, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
					AVP_dword nID = ::CreateDataId( *phRootData );
					::DATA_Replace_ID( hWorkRoot, NULL, AVP_word(nID) );

					if ( !::DATA_Attach(*phRootData, NULL, hWorkRoot, DATA_IF_ROOT_INCLUDE) ) {
						::DATA_Remove( hWorkRoot, NULL );
						hWorkRoot = NULL;
					}
				}
			}

			if ( hWorkRoot ) {
				HDATA hCurrData = ::DATA_Get_First( hResultData, NULL );
				while ( hCurrData ) {
					HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
					if ( (!::IsLinkedToFile(hCurrData) || !bSkipLinked) && !::IsAlreadyAttached(hCurrData, hWorkRoot) ) {
						if ( !::IsAlias(hCurrData) ) {
							::DATA_Detach( hCurrData, NULL );

							AVP_dword nID = ::CreateDataId( hWorkRoot );
							::DATA_Replace_ID( hCurrData, NULL, AVP_word(nID) );

							::SetDataPropWithCheck( hCurrData, VE_PID_IF_CONTAINERNAME, *m_StartingFolders[0], pContainerName );
							
							if ( !::DATA_Attach(hWorkRoot, NULL, hCurrData, DATA_IF_ROOT_INCLUDE) ) {
								hError =  PISRV_E_INTERNALERROR;
								break;
							}
						}
						else {
							CNodeUID rcCurrUID;
							if ( ::GetNodeUID(hCurrData, rcCurrUID) )
								rcAliases.Add( rcCurrUID );
						}
					}
					hCurrData = hNextData;
				}
			}
			else 
				hError =  PISRV_E_INTERNALERROR;
		}
		::DATA_Remove( hResultData, NULL );
	}

	return hError;
}


// ---
HRESULT CPragueIServer::AttachContainerInterfacesByAlias( LPCTSTR pContainerName, HDATA *phRootData, ItType eIType, CAliasContainer &rcAliases ) {
	HRESULT hError = S_OK;
	HDATA hResultData = NULL;
	if ( ::KLDT_DeserializeUsingSWM(pContainerName, &hResultData) ) {
		if ( eIType == itAny || eIType == itByContainer || eIType == ::GetTreeSType(hResultData) ) {
			HDATA hCurrData = ::DATA_Get_First( hResultData, NULL );
			while ( hCurrData && rcAliases.Count() ) {
				HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
				CNodeUID rcCurrUID;
				if ( ::GetNodeUID(hCurrData, rcCurrUID) ) {
					UINT uiIndex = rcAliases.FindIt( rcCurrUID );
					if ( uiIndex != SYS_MAX_UINT ) {
						if ( !::IsLinkedToFile(hCurrData) && !::IsAlreadyAttached(hCurrData, *phRootData) ) {
							if ( !::IsAlias(hCurrData) ) {
								if ( *phRootData == NULL ) {
									if ( !(*phRootData = ::DATA_Copy(NULL, NULL, hCurrData, DATA_IF_ROOT_INCLUDE)) ) {
										hError = PISRV_E_INTERNALERROR;
										break;
									}
									::SetDataPropWithCheck( *phRootData, VE_PID_IF_CONTAINERNAME, *m_StartingFolders[0], pContainerName );
									rcAliases.Flush();
								}
								else {
									::DATA_Detach( hCurrData, NULL );

									AVP_dword nID = ::CreateDataId( *phRootData );
									::DATA_Replace_ID( hCurrData, NULL, AVP_word(nID) );

									::SetDataPropWithCheck( hCurrData, VE_PID_IF_CONTAINERNAME, *m_StartingFolders[0], pContainerName );

									if ( !::DATA_Attach(*phRootData, NULL, hCurrData, DATA_IF_ROOT_INCLUDE) ) {
										hError = PISRV_E_INTERNALERROR;
										break;
									}
									rcAliases.RemoveInd( uiIndex );
								}
							}
						}
					}
				}
				hCurrData = hNextData;
			}
		}
		::DATA_Remove( hResultData, NULL );
	}

	return hError;
}


// ---
HRESULT CPragueIServer::EnumFoldersForInterfaces( LPCTSTR pFolderName, ItType eIType, HDATA hRootData, CAliasContainer &rcAliases ) {
	CPathStr pPathWildcard( pFolderName );

	pPathWildcard ^= m_Extentions[FOLDER_INDEX(eIType)];

	CAvpFileFind finder;
	bool bWorking = !!finder.FindFile( pPathWildcard );

	HRESULT hError = S_OK;

	while ( bWorking && hError == S_OK ) {
		bWorking = !!finder.FindNextFile();

		CPathStr pFullName( pFolderName );
		CAPointer<TCHAR> pFileName = finder.GetFileName();
		pFullName ^= pFileName;

		if ( finder.IsDirectory() && !finder.IsDots() ) 
			hError = EnumFoldersForInterfaces(pFullName, eIType, hRootData, rcAliases);

		if ( !finder.IsDirectory() && !finder.IsDots() ) 
			hError = AttachContainerInterfaces(pFullName, &hRootData, TRUE, eIType, rcAliases);
	}

	return S_OK;
}

// ---
HRESULT CPragueIServer::EnumFoldersForInterfacesAliases( LPCTSTR pFolderName, ItType eIType, HDATA *phRootData, CAliasContainer &rcAliases ) {
	if ( rcAliases.Count() ) {
		CPathStr pPathWildcard( pFolderName );

		pPathWildcard ^= m_Extentions[FOLDER_INDEX(eIType)];

		CAvpFileFind finder;
		bool bWorking = !!finder.FindFile( pPathWildcard );

		HRESULT hError = S_OK;

		while ( bWorking && rcAliases.Count() && hError == S_OK ) {
			bWorking = !!finder.FindNextFile();

			CPathStr pFullName( pFolderName );
			CAPointer<TCHAR> pFileName = finder.GetFileName();
			pFullName ^= pFileName;

			if ( finder.IsDirectory() && !finder.IsDots() ) 
				hError = EnumFoldersForInterfacesAliases(pFullName, eIType, phRootData, rcAliases);

			if ( !finder.IsDirectory() && !finder.IsDots() ) {
				hError = AttachContainerInterfacesByAlias(pFullName, phRootData, eIType, rcAliases);
			}
		}
	}
	return S_OK;
}


// ---
HRESULT CPragueIServer::LoadAllInterfacesTree( ItType eIType, HDATA &hRootData ) {
	HRESULT hError = PISRV_E_INTERNALERROR;

	hRootData = ::DATA_Add( NULL, NULL, VE_PID_ROOT, 0, 0 );
	if ( hRootData ) {
		for (int i=0; i<3; i++)
		{
			CPathStr rcStartingFolder = *m_StartingFolders[i/*FOLDER_INDEX(eIType)*/];
			CAliasContainer rcAliases;
			hError = EnumFoldersForInterfaces( rcStartingFolder, eIType, hRootData, rcAliases );

			if ( SUCCEEDED(hError) )
				hError = EnumFoldersForInterfacesAliases( rcStartingFolder, eIType, &hRootData, rcAliases );
		}
	}
	return hError;
}


// ---
HRESULT CPragueIServer::LoadAllInterfacesTree(ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces) {
	HRESULT hError = E_INVALIDARG;

	if ( pdwSize && ppInterfaces ) {
		HDATA hRootData = NULL;

		hError = LoadAllInterfacesTree( eIType, hRootData );

		if ( SUCCEEDED(hError) && hRootData ) {
			if ( SUCCEEDED(hError) )
				*ppInterfaces = (BYTE *)::SerializeDTree( hRootData, *pdwSize );

			::DATA_Remove( hRootData, NULL );
		}
	}
	return hError;
}

// ---
HRESULT CPragueIServer::EnumFoldersForContainers( LPCTSTR pFolderName, ItType eIType, HDATA hRootData ) {
	CPathStr pPathWildcard( pFolderName );

	pPathWildcard ^= m_Extentions[FOLDER_INDEX(eIType)];

	CAvpFileFind finder;
	bool bWorking = !!finder.FindFile( pPathWildcard );

	HRESULT hError = S_OK;

	while ( bWorking && hError == S_OK ) {
		bWorking = !!finder.FindNextFile();

		CPathStr pFullName( pFolderName );
		CAPointer<TCHAR> pFileName = finder.GetFileName();
		pFullName ^= pFileName;

		if ( finder.IsDirectory() && !finder.IsDots() ) 
			hError = EnumFoldersForContainers(pFullName, eIType, hRootData);

		if ( !finder.IsDirectory() && !finder.IsDots() ) 
			hError = LoadContainerInterfacesTree(pFullName, FALSE, eIType, &hRootData, TRUE );
	}

	return S_OK;
}

// ---
HRESULT CPragueIServer::LoadAllContainersTree( ItType eIType, HDATA &hRootData ) {
	HRESULT hError = PISRV_E_INTERNALERROR;

	hRootData = ::DATA_Add( NULL, NULL, VE_PID_ROOT, 0, 0 );
	if ( hRootData ) {
		CPathStr rcStartingFolder = *m_StartingFolders[FOLDER_INDEX(eIType)];
		hError = EnumFoldersForContainers( rcStartingFolder, eIType, hRootData );
	}
	return hError;
}


// ---
HRESULT CPragueIServer::LoadAllContainersTree(ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces) {
	HRESULT hError = E_INVALIDARG;

	if ( pdwSize && ppInterfaces ) {
		HDATA hRootData = NULL;

		hError = LoadAllContainersTree( eIType, hRootData );

		if ( SUCCEEDED(hError) && hRootData ) {
			if ( SUCCEEDED(hError) )
				*ppInterfaces = (BYTE *)::SerializeDTree( hRootData, *pdwSize );

			::DATA_Remove( hRootData, NULL );
		}
	}
	return hError;
}

// ---
HRESULT CPragueIServer::LoadContainerInterfacesTree(LPCTSTR pContainerName, BOOL bLockOnLoad, ItType eIType, HDATA *phRootData, BOOL bFullContainer ) {
	CAliasContainer rcAliases;

	HRESULT hError = AttachContainerInterfaces( pContainerName, phRootData, FALSE, eIType, rcAliases, bFullContainer );

	if ( SUCCEEDED(hError) ) {
		if ( eIType == itByContainer )
			eIType = ::GetTreeSType( *phRootData );

		CPathStr rcStartingFolder = *m_StartingFolders[FOLDER_INDEX(eIType)];

		hError = EnumFoldersForInterfacesAliases( rcStartingFolder, eIType, phRootData, rcAliases );
	}

	return hError;
}

// ---
HRESULT CPragueIServer::LoadContainerInterfacesTree(LPCTSTR pContainerName, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces) {
	HRESULT hError = E_INVALIDARG;

	if ( pdwSize && ppInterfaces ) {
		HDATA hRootData = NULL;

		hError = LoadContainerInterfacesTree(pContainerName, bLockOnLoad, eIType, &hRootData );

		if ( SUCCEEDED(hError) )
			hError = LockContainer( (BYTE *)pContainerName, bLockOnLoad );

		if ( SUCCEEDED(hError) )
			*ppInterfaces = (BYTE *)::SerializeDTree( hRootData, *pdwSize );

		::DATA_Remove( hRootData, NULL );
	}

	return hError;
}

// ---
HRESULT CPragueIServer::LoadInterfaceTree( CNodeUID &rcUID, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterface) {
	HRESULT hError = E_INVALIDARG;

	if ( pdwSize && ppInterface ) {
		HDATA hRootData = NULL;
		for( uint i=0,c=m_iface_cache.Count(); !hRootData && i<c; ++i ) {
			HDATA curr = m_iface_cache[i];
			CNodeUID rcCurrUID;
			if ( ::GetNodeUID(curr, rcCurrUID) && ::CompareNodesUID(rcUID, rcCurrUID) )
				hRootData = curr;
		}

		if ( hRootData ) 
			hError = S_OK;
		else {
			CAliasContainer rcAliases;

			rcAliases.Add( rcUID );
			hError = PISRV_E_INTNOTFOUND;
			for (int i=0; i<3 && hError == PISRV_E_INTNOTFOUND; i++)
			{
				CPathStr rcStartingFolder = *m_StartingFolders[i/*FOLDER_INDEX(eIType)*/];

				hError = EnumFoldersForInterfacesAliases( rcStartingFolder, eIType, &hRootData, rcAliases );

				if ( FAILED(hError) )
					m_rcLastErrorStr = rcUID;
				else if ( !hRootData ) {
					m_rcLastErrorStr = rcUID;
					hError = PISRV_E_INTNOTFOUND;
				}
				else
					m_iface_cache.Add( hRootData );
			}
		}

		if ( SUCCEEDED(hError) && hRootData ) {
			CAPointer<TCHAR> pContainerName;
			HPROP hNameProp = ::DATA_Find_Prop( hRootData, NULL, VE_PID_IF_CONTAINERNAME );
			if ( hNameProp ) 
				pContainerName = ::DTUT_GetPropValueAsString( hNameProp, NULL );
			hError = LockContainer( (BYTE *)(LPCTSTR(pContainerName)), bLockOnLoad );
		}

		if ( SUCCEEDED(hError) ) 
			*ppInterface = (BYTE *)::SerializeDTree( hRootData, *pdwSize );

	}

	return hError;
}


// ---
BOOL CPragueIServer::PerformFileOpenDlg( ItType eIType, LPTSTR pContainerName ) {
	DWORD dwExtID; 
	DWORD dwFilterID; 
	switch ( eIType ) {
		case itAny :
		default    :
			dwExtID = IDS_IPP_DEFEXTENTION;
			dwFilterID = IDS_IP_FILTER;
			break;
		case itPrototype :
			dwExtID = IDS_IPP_DEFEXTENTION;
			dwFilterID = IDS_IPP_FILTER;
			break;
		case itImplementation :
			dwExtID = IDS_IPI_DEFEXTENTION;
			dwFilterID = IDS_IPI_FILTER;
			break;
	}

	CAPointer<TCHAR> rcExtString = ::LoadAndFormatString( dwExtID );
	CAPointer<TCHAR> rcFltrString = ::LoadAndFormatString( dwFilterID );

	char pCurrDir[_MAX_PATH];

	if ( !pContainerName || !*pContainerName ) {
		lstrcpy( pCurrDir, *m_StartingFolders[0] );
	}
	else {
		char pFileName[_MAX_PATH];
		::IOSFullPathFromRelativePath( *m_StartingFolders[0], pContainerName, pFileName, sizeof(pFileName) );
		::IOSDirExtract( pFileName, pCurrDir, sizeof(pCurrDir) );
	}

	return ::PerformFileOpenDlg(m_hWndParent, pCurrDir, rcFltrString, rcExtString, pContainerName, _MAX_PATH);
}

// ---
HRESULT CPragueIServer::OpenContainerInterfacesTree(LPTSTR pContainerName, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces) {
	HRESULT hError = E_INVALIDARG;

	if ( pdwSize && ppInterfaces ) {
		if ( PerformFileOpenDlg(eIType, pContainerName) ) 
			return LoadContainerInterfacesTree( pContainerName, bLockOnLoad, eIType, pdwSize, ppInterfaces );
		return S_FALSE;
	}

	return hError;
}



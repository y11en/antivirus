////////////////////////////////////////////////////////////////////
//
//  PragueIServerSave.cpp
//  Save interfaces stuff implementation
//  Prague Interfaces Server
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include <property/property.h>
#include <AVPComID.h>
#include <Stuff\FFind.h>
#include <Stuff\SArray.h>
#include <Stuff\PathStr.h>
#include <StuffIO\IOUtil.h>

#include "..\AVPPveID.h"

#include "..\PGIServerInterface.h"
#include "..\PISrvError.h"
#include "PragueIServer.h"

TCHAR *LoadAndFormatString( UINT uiStrID, ... );

// ---
HRESULT CPragueIServer::SaveBakFile( LPCTSTR pContainerName ) {
	if ( pContainerName && *pContainerName ) {
		CPathStr rcNewFileName = pContainerName;
		rcNewFileName += ".bak";

		::SetFileAttributes( rcNewFileName, ::GetFileAttributes(rcNewFileName) & ~FILE_ATTRIBUTE_READONLY );

		if ( ::IOSFileExists(pContainerName) == IOS_ERR_OK &&
			   ::IOSCopyFile(pContainerName, rcNewFileName) != IOS_ERR_OK ) {
			m_rcLastErrorStr = rcNewFileName;
			return PISRV_E_SAVERESERVECOPY;
		}
		return S_OK;
	}
	return E_INVALIDARG;
}

// ---
HRESULT CPragueIServer::SaveInterfaces( LPCTSTR pContainerName, HDATA hRootData ) {
	HRESULT hError;
	if ( (hError = SaveBakFile(pContainerName)) == S_OK ) {
	
		if (!CheckOutFile(pContainerName))
			return S_FALSE;
		
		AVP_bool bResult = ::KLDT_SerializeUsingSWM( pContainerName, hRootData, "" );
		if ( !bResult ) {
			m_rcLastErrorStr = pContainerName;
			hError =  PISRV_E_SAVECONTAINER;
		}
	}
	return hError;
}


// ---
HRESULT CPragueIServer::PerformFileSaveAs( LPTSTR pContainerName, ItType eIType, HDATA hRootData ) {
	if ( !m_hWndParent )
		return PISRV_E_UIOWNER;

	HRESULT hError = S_FALSE;//PISRV_E_INTERNALERROR;

	DWORD dwExtID; 
	DWORD dwFilterID; 
	switch ( eIType ) {
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

	if ( ::PerformFileSaveDlg(m_hWndParent, pCurrDir, rcFltrString, rcExtString, pContainerName, _MAX_PATH) ) {
		if ( ::IOSFileExists(pContainerName) == IOS_ERR_OK ) {
			HDATA hError = NULL;
			AVP_bool nResult = ::KLDT_DeserializeUsingSWM( pContainerName, &hError );
			if ( nResult ) {
				AVP_dword nWorkMode;
				::DATA_Get_Val( hError, NULL, 0, &nWorkMode, sizeof(nWorkMode) );
				::DATA_Remove( hError, NULL );
				if ( AVP_dword(eIType) != nWorkMode ) {
						return PISRV_E_DIFFERENTTYPES;
				}
			}
		}

		hError = SaveInterfaces( pContainerName, hRootData );
	}

	return hError;
}


// ---
static BOOL PreprocessSavingContainer( HDATA hRootData ) {
	CAliasContainer rcAliases;
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
		
		if ( ::IsLinkedToFile(hCurrData) ) {
			CNodeUID rcCurrUID;
			if ( ::GetNodeUID(hCurrData, rcCurrUID) )
				rcAliases.Add( rcCurrUID );
			::DATA_Remove( hCurrData, NULL );
			hCurrData = NULL;
		}

		if ( hCurrData && ::IsLinkedByInterface(hCurrData) ) {
			::DATA_Remove( hCurrData, NULL );
			hCurrData = NULL;
		}

		hCurrData = hNextData;
	}

	if ( rcAliases.Count() ) {
		hCurrData = ::DATA_Get_First( hRootData, NULL );
		while ( hCurrData ) {
			HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
			if ( !::IsLinkedToFile(hCurrData) ) {
				HPROP hLinkedProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_LINKEDUNIQUEID );
				if ( !hLinkedProp )
					hLinkedProp = ::DATA_Add_Prop( hCurrData, NULL, VE_PID_LINKEDUNIQUEID, 0, 0 );
				if ( hLinkedProp ) {
					CNodeUID rcCurrUID;
					AVP_dword dwLength = ::PROP_Arr_Count( hLinkedProp );
					AVP_dword i;
					for ( i=0; i<dwLength; i++ ) {
						::PROP_Arr_Get_Items( hLinkedProp, i, &rcCurrUID, sizeof(CNodeUID) );
						UINT uiIndex = rcAliases.FindIt( rcCurrUID );
						if ( uiIndex != SYS_MAX_UINT ) 
							rcAliases.RemoveInd( uiIndex );
					}

					for ( i=0; i<rcAliases.Count(); i++ ) {
						::PROP_Arr_Insert( hLinkedProp, PROP_ARR_LAST, rcAliases[i], sizeof(CNodeUID) );
					}
				}
				else
					return FALSE;
			}
			hCurrData = hNextData;
		}
	}

	return TRUE;
}



// ---
HRESULT CPragueIServer::SaveContainerInterfacesTree(BOOL bSaveAs, LPTSTR pContainerName, BOOL bLockOnSave, ItType eIType, DWORD dwSize, BYTE *pInterfaces) {
	HRESULT hError = E_INVALIDARG;

	if ( dwSize && pInterfaces ) {
		hError = PISRV_E_INTERNALERROR;
		HDATA hRootData = ::DeserializeDTree( pInterfaces, dwSize );
		if ( hRootData ) {
			if ( ::PreprocessSavingContainer(hRootData) ) {

				if ( eIType == itByContainer )
					eIType = ::GetTreeSType( hRootData );

				if ( !pContainerName || !*pContainerName || bSaveAs ) 
					hError = PerformFileSaveAs( pContainerName, eIType, hRootData );
				else 
					hError = SaveInterfaces( pContainerName, hRootData );

				if ( hError == S_OK )
					hError = LockContainer( (BYTE *)pContainerName, bLockOnSave );
			}
			::DATA_Remove( hRootData, NULL );
		}
	}

	return hError;
}


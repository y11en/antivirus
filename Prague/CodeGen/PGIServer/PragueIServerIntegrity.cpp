////////////////////////////////////////////////////////////////////
//
//  PragueIServerIntegrity.cpp
//  Save interfaces stuff implementation
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
HRESULT CPragueIServer::AttachContainerIntegrity( LPCTSTR pContainerName, HDATA hRootData ) {
	HRESULT hError = S_OK;
	HDATA hResultData = NULL;
	if ( ::KLDT_DeserializeUsingSWM(pContainerName, &hResultData) ) {
		if ( itPrototype == ::GetTreeSType(hResultData) ) {
			HDATA hCurrData = ::DATA_Get_First( hResultData, NULL );
			while ( hCurrData ) {
				HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
				if ( !::IsLinkedToFile(hCurrData) ) {
					if ( !::IsAlias(hCurrData) ) {
						::DATA_Detach( hCurrData, NULL );

						AVP_dword nID = ::CreateDataId( hRootData );
						::DATA_Replace_ID( hCurrData, NULL, AVP_word(nID) );

						::SetDataPropWithCheck( hCurrData, VE_PID_IF_CONTAINERNAME, *m_StartingFolders[0], pContainerName );
						
						if ( !::DATA_Attach(hRootData, NULL, hCurrData, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN) ) {
							hError =  PISRV_E_INTERNALERROR;
							break;
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
HRESULT CPragueIServer::EnumFoldersForCheckIntegrity( LPCTSTR pFolderName, HDATA hRootData ) {
	CPathStr pPathWildcard( pFolderName );
	pPathWildcard ^= "*.prt";

	CAvpFileFind finder;
	bool bWorking = !!finder.FindFile( pPathWildcard );

	HRESULT hError = S_OK;

	while ( bWorking && hError == S_OK ) {
		bWorking = !!finder.FindNextFile();

		CPathStr pFullName( pFolderName );
		CAPointer<TCHAR> pFileName = finder.GetFileName();
		pFullName ^= pFileName;

		if ( finder.IsDirectory() && !finder.IsDots() ) 
			hError = EnumFoldersForCheckIntegrity(pFullName, hRootData);

		if ( !finder.IsDirectory() && !finder.IsDots() ) 
			hError = AttachContainerIntegrity(pFullName, hRootData);
	}

	return S_OK;
}




// ---
HRESULT CPragueIServer::CheckIntegrity() {
	HRESULT hError = PISRV_E_INTERNALERROR;

	HDATA hRootData = ::DATA_Add( NULL, NULL, VE_PID_ROOT, 0, 0 );

	if ( hRootData ) {
		hError = EnumFoldersForCheckIntegrity( *m_StartingFolders[0], hRootData );

		if ( SUCCEEDED(hError) ) {
			CAliasContainer rcAliases;
			CPArray<TCHAR> rcContainers;
			CPArray<TCHAR> rcInterfaces;

			HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
			while ( hCurrData ) {
				CNodeUID rcCurrUID;
				if ( ::GetNodeUID(hCurrData, rcCurrUID) ) {
					CAPointer<TCHAR> pContainerName;
					CAPointer<TCHAR> pInterfaceName;
					HPROP hNameProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_CONTAINERNAME );
					if ( hNameProp )
						pContainerName = ::DTUT_GetPropValueAsString( hNameProp, NULL );

					hNameProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_INTERFACENAME );
					if ( hNameProp )
						pInterfaceName = ::DTUT_GetPropValueAsString( hNameProp, NULL );

					UINT uiUndex = rcAliases.FindIt(rcCurrUID);
					if ( uiUndex == SYS_MAX_UINT ) {
						rcAliases.Add( rcCurrUID );
						rcInterfaces.Add( pInterfaceName.Relinquish() ); 
						rcContainers.Add( pContainerName.Relinquish() ); 
					}
					else {
						CAPointer<TCHAR> rcError = ::LoadAndFormatString( IDS_DUPLICATEINT, LPCTSTR(rcCurrUID), LPCTSTR(pInterfaceName), LPCTSTR(pContainerName), LPCTSTR(rcInterfaces[uiUndex]), LPCTSTR(rcContainers[uiUndex]) );
						CAPointer<TCHAR> rcTitle = ::LoadAndFormatString( IDS_PROJNAME );
						::MessageBox( m_hWndParent, rcError, rcTitle, MB_OK | MB_ICONEXCLAMATION );
					}
				}
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}

			::DATA_Remove( hRootData, NULL );

			hError = S_OK;
		}
	}
	return hError;
}



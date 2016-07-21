// VisualEditorLoadSave.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <serialize/KLDTSer.h>
#include <datatreeutils/dtutils.h>
#include <Stuff\CPointer.h>
#include <StuffIO\IOUtil.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "ImportDefFileDlg.h"
#include "NewInterfaceNameDlg.h"
#include "SynchroTools.h"
#include "PVE_DT_UTILS.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ---
void PrepareFilterIndex( OPENFILENAME &pOfn, const char *pDefExt ) {
  CString rcExt("*.");
  rcExt += pDefExt;
  char *p;
  int   i;
  for ( p = (char *)pOfn.lpstrFilter, i = 0; *p; i++ ) {
    p += lstrlen( p ) + 1;
    if ( strstr( p, rcExt ) ) {
      pOfn.nFilterIndex = i + 1;
      break;
    }
    p += strlen( p ) + 1;
  }
}


// ---
void CVisualEditorDlg::SetWindowTitle(BOOL i_bUpdateRegistry) {
	CString rcString;
	if ( m_FileName.IsEmpty() )
		rcString.LoadString( IDS_MAINTITLE );
	else {
		CString strFileName;
		strFileName.Format(_T("%s%s"), m_FileName, IsModified() ? _T("* "):_T(""));

		rcString.Format( IDS_WINDOWTITLE, strFileName );

		if(i_bUpdateRegistry)
		{
			CString rcInitDirRegValue;
			switch ( m_nWorkMode ) {
			case wmt_IDefinition :
				rcInitDirRegValue = PRT_INITDIR_REG_VALUE;
				break;
			case wmt_IImplementation :
				rcInitDirRegValue = IMP_INITDIR_REG_VALUE;
				break;
			}
			char pCurrDir[_MAX_PATH];
			::IOSDirExtract( rcString, pCurrDir, sizeof(pCurrDir) );
			m_RegStorage.PutValue( rcInitDirRegValue, pCurrDir );
		}
	}

	CString strOldTitle;
	GetWindowText(strOldTitle);

	if(strOldTitle != rcString)
		SetWindowText( rcString );
}


// ---
void CVisualEditorDlg::OnAppExit() {

	// if user choose to stay in and correct default plugin name or plugin id
	if(!CheckPluginDefaultName())
		return;
		
	if ( IsModified() ) {
		int nOK = ::DlgMessageBox( this, IDS_ERR_SAVEREQUEST, MB_ICONQUESTION | MB_YESNOCANCEL );
		switch ( nOK ) {
			case IDYES :
				if ( !PerformFileSave() )
					return;
				break;
			case IDCANCEL :
				return;
		}
	}
/*
	else {
		if ( ::DlgMessageBox(this, IDS_ERR_CANCLOSE, MB_ICONQUESTION | MB_YESNO) == IDNO )
			return;
	}
*/
	if ( m_pIIServer ) {
		TCHAR *pFileName = (TCHAR *)::CoTaskMemAlloc( m_FileName.GetLength() + 1 );
		lstrcpy( pFileName,	m_FileName );
		m_pIIServer->LockContainer( (BYTE *)(LPCTSTR(m_FileName)), FALSE );
	  ::CoTaskMemFree( pFileName );
	}

	CDialog::OnOK();
}


// ---
void CVisualEditorDlg::OnFileExit() {
	OnAppExit();
}


// ---
BOOL CVisualEditorDlg::SaveChangedRequest( BOOL bStrictSave ) {

	if(!CheckPluginDefaultName())
		return false;

	if ( IsModified() ) {
		int nOK = ::DlgMessageBox( this, IDS_ERR_SAVEREQUEST, MB_ICONQUESTION | MB_YESNOCANCEL );
		switch ( nOK ) {
			case IDYES :
				return PerformFileSave(); 
			case IDNO :
				return !bStrictSave;
			case IDCANCEL :
				return false;
		}
	}
	return true;
}

void CVisualEditorDlg::OnUpdateFileNew(CCmdUI* pCmdUI){
	pCmdUI->Enable(!m_bTechnicalEditting);
}

// ---
void CVisualEditorDlg::OnFileNew() {
	if ( SaveChangedRequest() ) {
		m_nWorkMode = wmt_IDefinition;
		InitNewTree();
	}
}

// ---
void CVisualEditorDlg::OnUpdateFileNewImp(CCmdUI* pCmdUI){
	pCmdUI->Enable(!m_bTechnicalEditting);
}

// ---
void CVisualEditorDlg::OnFileNewImp() {
	if ( SaveChangedRequest() ) {
		m_nWorkMode = wmt_IImplementation;
		InitNewTree();
	}
}

// ---
void CVisualEditorDlg::OnFileOpen() {
	if ( SaveChangedRequest() ) {
		CString rcOldName = m_FileName;
		if ( LoadInterfaces(m_FileName, true) ) {
			switch ( m_nWorkMode ) {
				case wmt_IDefinition :
					m_IDefinitionFileName = m_FileName;
					break;
				case wmt_IImplementation :
					m_IImplementationFileName = m_FileName;
					break;
			}
			UpdateData( FALSE );
			ShakeRecentMenu();
			SetWindowTitle();
		}
		else
			m_FileName = rcOldName;
	}
}

// ---
static void SetInterfaceOneItemExpanded( HDATA hOldData, HDATA hNewData, HDATA hStopData ) {
	AVP_bool nValue = FALSE;
	HPROP hExpandedProp = ::DATA_Find_Prop( hOldData, NULL, VE_PID_NODEEXPANDED );
	if ( hExpandedProp ) {
		::PROP_Get_Val( hExpandedProp, &nValue, sizeof(nValue) );

		AVP_dword *pSeq = NULL;
		::DATA_Make_Sequence( hOldData, NULL, hStopData, &pSeq );

		if ( hNewData = ::DATA_Find(hNewData, pSeq) ) {
			hExpandedProp = ::DATA_Find_Prop( hNewData, NULL, VE_PID_NODEEXPANDED );
			if ( !hExpandedProp )
				hExpandedProp = ::DATA_Add_Prop( hNewData, NULL, VE_PID_NODEEXPANDED, 0, 0 );

			if ( hExpandedProp )
				::PROP_Set_Val( hExpandedProp, nValue, 0 );
		}
		::DATA_Remove_Sequence( pSeq );
	}
}


// ---
static void SetInterfaceSubItemsExpanded( HDATA hOldData, HDATA hNewData, HDATA hStopData ) {
	if ( hOldData ) {
		::SetInterfaceOneItemExpanded( hOldData, hNewData, hStopData );
		HDATA hCurrData = ::DATA_Get_First( hOldData, NULL );
		while ( hCurrData ) {										 
			::SetInterfaceSubItemsExpanded( hCurrData, hNewData, hStopData );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
}

// ---
void CVisualEditorDlg::CheckLinkedInterfaces( HDATA hRootData, BOOL bRecursiveImported /*= FALSE*/ ) {
	HDATA hOldData = ::DATA_Copy( NULL, NULL, hRootData, DATA_IF_ROOT_INCLUDE );

	CPArray<char> pFileNames( 0, 1, true );
	CPArray<char> pRelativeFileNames( 0, 1, true );
	CSArray<CNodeUID> pNodesUIDs;
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
		if ( bRecursiveImported || !::IsLinkedToFile(hCurrData) ) {
			HPROP hFileProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_IMPORTFILENAME );
			if ( hFileProp ) {
				CAPointer<char> pFileName = ::GetPropValueAsString( hFileProp, NULL );
				BOOL bFound = false;
				for ( int i=0,c=pFileNames.Count(); i<c && !bFound; i++ ) {
					if ( !lstrcmpi(pFileNames[i], pFileName) ) 
						bFound = true;
				}
				if ( !bFound ) {
					pFileNames.Add( pFileName.Relinquish() );

					CAPointer<char> pRelativeFileName = lstrcpy( new char[1], "" );
					HPROP hFileProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_IMPORTRELATIVEFILENAME );
					if ( hFileProp ) 
						pRelativeFileName = ::GetPropValueAsString( hFileProp, NULL );

					pRelativeFileNames.Add( pRelativeFileName.Relinquish() );
				}
				::DATA_Remove( hCurrData, NULL );
			}
			else {
				HPROP hLinkedProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_LINKEDUNIQUEID );
				if ( hLinkedProp ) {
					CNodeUID rcCurrUID;
					AVP_dword dwLength = ::PROP_Arr_Count( hLinkedProp );
					for ( AVP_dword i=0; i<dwLength; i++ ) {
						::PROP_Arr_Get_Items( hLinkedProp, i, &rcCurrUID, sizeof(CNodeUID) );

						UINT uiIndex = pNodesUIDs.FindIt( rcCurrUID );
						if ( uiIndex == SYS_MAX_UINT ) 
							pNodesUIDs.Add( rcCurrUID );
					}
				}
			}
		}

		hCurrData = hNextData;
	}

	int i, c;
	for ( i=0,c=pFileNames.Count(); i<c; i++ ) 
		SetModified( IsModified() | LoadImportInterfacesDefinition( pFileNames[i], pRelativeFileNames[i], hRootData, TRUE ) );

	for ( i=0,c=pNodesUIDs.Count(); i<c; i++ ) 
		SetModified( IsModified() | LoadImportInterfacesDefinitionByUID(pNodesUIDs[i], hRootData) );

	for ( i=0,c=pNodesUIDs.Count(); i<c; i++ ) 
		SetModified( IsModified() | UpdateInterfacesIncludesByUID(pNodesUIDs[i], hRootData) );

	if ( hOldData ) {
		::SetInterfaceSubItemsExpanded( hOldData, hRootData, hOldData );

		::DATA_Remove( hOldData, NULL );
	}
}

// ---
BOOL CVisualEditorDlg::LoadInterfaces( CString rcFileName, BOOL bOpenOrLoad ) {
	if ( m_pIIServer ) {
		DWORD dwSize;
		BYTE *pContainer;

		HRESULT hError = S_FALSE;
		if ( bOpenOrLoad ) {
			ItType eIType = itAny;//::WType2IType( m_nWorkMode );
			TCHAR *pFileName = (TCHAR *)::CoTaskMemAlloc( rcFileName.GetLength() + 1 );
			lstrcpy( pFileName,	rcFileName );

		  hError = m_pIIServer->OpenContainer( (BYTE **)&pFileName, TRUE, eIType, &dwSize, &pContainer );

			rcFileName = pFileName;
			::CoTaskMemFree( pFileName );
		}
		else {
			TCHAR *pFileName = (TCHAR *)::CoTaskMemAlloc( rcFileName.GetLength() + 1 );
			lstrcpy( pFileName,	rcFileName );

			hError = m_pIIServer->LoadContainer( (BYTE *)pFileName, TRUE, &dwSize, &pContainer );

			::CoTaskMemFree( pFileName );
		}

		if ( hError == S_OK ) {
			HDATA hResult = ::DeserializeDTree( pContainer, dwSize );
			if ( hResult ) {
				::DATA_Get_Val( hResult, NULL, 0, &m_nWorkMode, sizeof(m_nWorkMode) );
				m_FileName = rcFileName;
				SetModified( false );
				CheckLinkedInterfaces( hResult, TRUE );
				
				//BOOL bChanged = IsModified();
				// during initialization tree can be modified, so don't clear modified flag
				InitNewTree( hResult );
				//SetModified( bChanged );

				return true;
			}
		}
		else 
			DisplayPISError( hError );
	}

	return false;
}

// ---
void CVisualEditorDlg::OnFileSave() {
	PerformFileSave();
}

// ---
void CVisualEditorDlg::OnFileSaveAs() {
	PerformFileSaveAs();
}

// ---
BOOL CVisualEditorDlg::PerformFileSave() {
	if ( m_FileName.IsEmpty() ) 
		return PerformFileSaveAs();
	else 
		return SaveInterfaces( false );
}

// ---
BOOL CVisualEditorDlg::PerformFileSaveAs() {
	BOOL bResult = SaveInterfaces( true );

	if ( bResult ) {
		switch ( m_nWorkMode ) {
			case wmt_IDefinition :
				m_IDefinitionFileName = m_FileName;
				break;
			case wmt_IImplementation :
				m_IImplementationFileName = m_FileName;
				break;
		}

		UpdateData( FALSE );
	}

	return bResult;
}


// ---
BOOL CVisualEditorDlg::SaveInterfaces( BOOL bSaveAs, LPCTSTR pszFileName ) {
	if ( m_hTree && m_pIIServer ) {
		DWORD dwSize = 0;
		BYTE  *pBuffer = (BYTE *)::SerializeDTree( m_hTree, dwSize );
		TCHAR *pFileName = (TCHAR *)::CoTaskMemAlloc( m_FileName.GetLength() + 1 );
		lstrcpy( pFileName,	m_FileName );

		HRESULT hError = m_pIIServer->SaveContainer( (BYTE **)&pFileName, bSaveAs, TRUE, dwSize, pBuffer );
		
		::CoTaskMemFree( pBuffer );

		if ( hError == S_OK ) {
			m_FileName = pFileName;

			SetModified( false );

			switch ( m_nWorkMode ) {
				case wmt_IDefinition :
					break;
				case wmt_IImplementation :
					SaveDescriptionDB();
					break;
			}

			::CoTaskMemFree( pFileName );

			ShakeRecentMenu();
			SetWindowTitle();
		}
		else {
			DisplayPISError( hError );
			return false;
		}
	}
		
	return true;
}

// --- 
void CVisualEditorDlg::CheckDuplicateIntName( HDATA hRootData, const char *pIntName ) {
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_INTERFACENAME );
		if ( hProp ) {
			CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
			if ( !lstrcmpi(pValue, pIntName) ) {
				CString rcString;
				rcString.Format( IDS_ERR_DUPLICATEINTNAME, pIntName );
				::DlgMessageBox( this, rcString );
				break;
			}
		}
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
}

// ---
BOOL CVisualEditorDlg::CheckAlreadyImported( HDATA hRootData, CNodeUID &rcUID ) {
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		CNodeUID rcCurrUID;
		if ( ::GetNodeUID(hCurrData, rcCurrUID) && ::CompareNodesUID(rcUID, rcCurrUID) ) 
			return true;
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	return false;
}


// ---
BOOL CVisualEditorDlg::CheckAlreadyImported( HDATA hRootData, const char *pFileName ) {
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		HPROP hFileProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_IMPORTFILENAME );
		if ( hFileProp ) {
			CAPointer<char> pCheckFileName = ::GetPropValueAsString( hFileProp, NULL );
			if ( !lstrcmpi(pCheckFileName, pFileName) ) {
				CString rcString;
				rcString.Format( IDS_ERR_ALREADYIMPORTED, pFileName );
				::DlgMessageBox( this, rcString );
				return true;
			}
		}
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	return false;
}


// ---
void CVisualEditorDlg::AttachLinkedInterface( HDATA hRootData, HDATA hInterfaceData ) {
	CNodeUID rcInterfaceUID;
	if ( ::GetNodeUID(hInterfaceData, rcInterfaceUID) && !CheckAlreadyImported(hRootData, rcInterfaceUID) ) {
		AVP_dword nID = ::CreateDataId( hRootData );
		::DATA_Replace_ID( hInterfaceData, NULL, AVP_word(nID) );
		::DATA_Add_Prop( hInterfaceData, NULL, VE_PID_IF_LINKEDBYINTERFACE, 0, 0 );
		::DATA_Attach( hRootData, NULL, hInterfaceData,  DATA_IF_ROOT_INCLUDE );
	}
}

// ---
BOOL CVisualEditorDlg::UpdateInterfacesIncludesByUID( CNodeUID &rcUID, HDATA hRootData ) {
	BOOL bModified = false;
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		CNodeUID rcInterfaceUID;
		if ( ::GetNodeUID(hCurrData, rcInterfaceUID) && ::CompareNodesUID(rcInterfaceUID, rcUID) ) {
			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_INCLUDEFILE ) ) {
				CAPointer<char> pValue = ::GetPropValueAsString( hValueProp, NULL );

				HPROP hIncludeUidsProp;
				HPROP hIncludeNamesProp;
				HDATA hUpdateCurrData = ::DATA_Get_First( hRootData, NULL );

				while ( hUpdateCurrData) {
					BOOL bFound = false;
					if ( !::IsLinkedToFile(hUpdateCurrData) ) {
						if ( !(hIncludeNamesProp = ::DATA_Find_Prop(hUpdateCurrData, NULL, VE_PID_IF_INCLUDENAMES)) ) {
							hIncludeNamesProp = ::DATA_Add_Prop(hUpdateCurrData, NULL, VE_PID_IF_INCLUDENAMES, 0, 0 );
							bModified = true;
						}
						if ( !(hIncludeUidsProp = ::DATA_Find_Prop(hUpdateCurrData, NULL, VE_PID_IF_INCLUDEUIDS)) ) {
							hIncludeUidsProp = ::DATA_Add_Prop( hUpdateCurrData, NULL, VE_PID_IF_INCLUDEUIDS, 0, 0 );
							bModified = true;
						}
						if ( hIncludeUidsProp ) {
							CNodeUID rcUpdateUID;
							AVP_dword dwCount = ::PROP_Arr_Count( hIncludeUidsProp );
							for ( AVP_dword i=0; i<dwCount; i++ ) {
								::PROP_Arr_Get_Items( hIncludeUidsProp, i, &rcUpdateUID, sizeof(rcUpdateUID) );
								if ( ::CompareNodesUID(rcInterfaceUID, rcUpdateUID) ) {
									if ( hIncludeNamesProp ) {
										bModified |= !!::SetDataPropArrayWithCheck( hIncludeNamesProp, i, pValue );
										//::SetPropArrayValue( hValueProp, i, pValue, NULL );
										bFound = true;
										break;
									}
								}
							}
							if ( !bFound ) {
						    ::SetPropArrayValue( hIncludeUidsProp, PROP_ARR_LAST, LPCTSTR(rcInterfaceUID), NULL );
								bModified = true;
							}
						}

						bFound = false;

						if ( hIncludeNamesProp ) {
							AVP_dword dwCount = ::PROP_Arr_Count( hIncludeNamesProp );
							for ( AVP_dword i=0; i<dwCount; i++ ) {
								CAPointer<char> pUpdateValue = ::GetPropArrayValueAsString( hIncludeNamesProp, i, NULL );
								if ( !lstrcmpi(pUpdateValue, pValue) ) {
									if ( hIncludeUidsProp ) {
										int dwCount = ::PROP_Arr_Count( hIncludeUidsProp );
										for ( dwCount=i-dwCount; dwCount>=0; dwCount-- ) {
											::SetPropArrayValue( hIncludeUidsProp, PROP_ARR_LAST, LPCTSTR(""), NULL );
											bModified = true;
										}
										bModified |= !!::SetDataPropArrayWithCheck( hIncludeUidsProp, i, LPCTSTR(rcInterfaceUID) );
										bFound = true;
										break;
									}
								}
							}
							if ( !bFound ) {
								::SetPropArrayValue( hIncludeNamesProp, PROP_ARR_LAST, pValue, NULL );
								bModified = true;
							}
						}
					}
					hUpdateCurrData = ::DATA_Get_Next( hUpdateCurrData, NULL );
				}
			}
		}
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	return bModified;
}

// ---
BOOL CVisualEditorDlg::LoadImportInterfacesDefinitionByUID( CNodeUID &rcUID, HDATA hRootData ) {
	if ( CheckAlreadyImported(hRootData, rcUID) )
		return false;

	if ( m_pIIServer ) {
		DWORD dwSize;
		BYTE *pContainer;
		HRESULT hError = m_pIIServer->LoadInterface( sizeof(rcUID), (BYTE *)&rcUID, FALSE, itPrototype, &dwSize, &pContainer );
		if ( SUCCEEDED(hError) ) {
			HDATA hResult = ::DeserializeDTree( pContainer, dwSize );
			if ( hResult ) {
				AttachLinkedInterface( hRootData, hResult );
				return false;
			}
		}
		else 
			DisplayPISError( hError );
	}

	return false;
}


// ---
BOOL CVisualEditorDlg::LoadImportInterfacesDefinition( const char *pImpFileName, const char *pRelativeFileName, HDATA hRootData, BOOL bLinkToFile ) {
	if ( bLinkToFile && CheckAlreadyImported(hRootData, pImpFileName) )
		return false;

	char pFileName[_MAX_PATH];
	lstrcpy( pFileName, pImpFileName );

	if ( ::IOSFileExists(pFileName) != IOS_ERR_OK ) {
		BOOL bFound = false;
		char pFullPath[_MAX_PATH];
		::IOSFullPathFromRelativePath( m_FileName, pRelativeFileName, pFullPath, sizeof(pFullPath) );
		if ( ::IOSFileExists(pFullPath) == IOS_ERR_OK ) {
			lstrcpy( pFileName, pFullPath );
			bFound = true;
		}

		if ( !bFound ) {
			CString rcString;
			rcString.Format( IDS_ERR_FILEEXISTS, LPCSTR(pFileName) );
			if ( ::DlgMessageBox(this, rcString, MB_YESNO ) != IDYES )
				return false;

			CString rcFileName = pFileName;
			BOOL bLinked = TRUE;
			if ( GetImportFileName(rcFileName, bLinked) ) 
				lstrcpy( pFileName, rcFileName );
			else
				return false;
		}
	}


	HDATA hResult = NULL;
	if ( !::KLDT_DeserializeUsingSWM(pFileName, &hResult) ) {
		CString rcString;
		rcString.Format( IDS_ERR_READFILE, LPCSTR(pFileName) );
		::DlgMessageBox(this, rcString );
	}

	if ( hResult ) {
		AVP_dword nFileType;
		::DATA_Get_Val( hResult, NULL, 0, &nFileType, sizeof(nFileType) );
		if ( nFileType != wmt_IDefinition ) {
			::DlgMessageBox( this, IDS_ERR_INCORRECTFILETYPE );
			::DATA_Remove( hResult, NULL );
			hResult = NULL;
		}
	}

	BOOL bChanged = false;
	if ( hResult ) {
		HDATA hCurrData = ::DATA_Get_First( hResult, NULL );
		while ( hCurrData ) {
			if ( !::DATA_Find_Prop(hCurrData, NULL, VE_PID_IF_IMPORTFILENAME) ) {
				// Don't attach already linked interfaces
				HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_INTERFACENAME );
				if ( hProp ) {
					CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
					CheckDuplicateIntName( hRootData, pValue );
					HDATA hNewData = ::DATA_Copy( NULL, NULL, hCurrData, DATA_IF_ROOT_INCLUDE );
					if ( hNewData ) {
						AVP_dword nID = ::CreateDataId( hRootData );
						::DATA_Replace_ID( hNewData, NULL, AVP_word(nID) );

						bChanged = !!::SetDataPropWithCheck( hNewData, VE_PID_IF_IMPORTED, TRUE );

						if ( bLinkToFile ) {
							bChanged = !!::SetDataPropWithCheck( hNewData, VE_PID_IF_IMPORTFILENAME, pFileName );

							char pRelativePath[_MAX_PATH];
							::IOSRelativePathFromFullPath( m_FileName, pFileName, pRelativePath, sizeof(pRelativePath) );
							bChanged = !!::SetDataPropWithCheck( hNewData, VE_PID_IF_IMPORTRELATIVEFILENAME, pRelativePath );
						}
						else {
							hProp = ::DATA_Find_Prop( hNewData, NULL, VE_PID_IF_IMPORTFILENAME );
							if ( hProp ) 
								::DATA_Remove_Prop_H( hNewData, NULL, hProp );
						}

						::DATA_Attach( hRootData, NULL, hNewData,  DATA_IF_ROOT_INCLUDE );
					}
				}
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}

		::DATA_Remove( hResult, NULL );
	}
	return bChanged;
}


// ---
void CVisualEditorDlg::LoadImportDefinitionFile( CString &rcFileName, BOOL bLinkToFile ) {
	if ( LoadImportInterfacesDefinition(rcFileName, rcFileName, m_hTree, bLinkToFile) ) {
		InitNewTree( m_hTree );
		SetModified( true );
	}
}


// ---
BOOL CVisualEditorDlg::GetImportFileName( CString &rcFileName, BOOL &bLinked ) {
	CString rcExtString;
	rcExtString.LoadString( IDS_IPP_DEFEXTENTION );

	CString rcFltrString;
	rcFltrString.LoadString( IDS_IPP_FILTER );

	CString rcInitDirRegValue = PRT_INITDIR_REG_VALUE;

	CImportDefFileDlg rcOpenDlg( TRUE, rcExtString, NULL, OFN_HIDEREADONLY | OFN_ENABLETEMPLATE  | OFN_PATHMUSTEXIST, rcFltrString, this );
	rcOpenDlg.m_bLinkToFile = bLinked;

	rcOpenDlg.m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_IMPORTDEF_DLG);
	rcOpenDlg.m_ofn.hInstance = ::AfxGetApp()->m_hInstance;

	char pCurrDir[_MAX_PATH];
	char pFileName[_MAX_PATH];

	if ( rcFileName.IsEmpty() ) {
		if ( !m_RegStorage.GetValue(rcInitDirRegValue, pCurrDir, sizeof(pCurrDir)) ) 
			::IOSGetCurrentDirectory( pCurrDir, sizeof(pCurrDir) );

		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	else {
		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;

		::IOSFileExtract( rcFileName, pFileName, _MAX_PATH );
		rcOpenDlg.m_ofn.lpstrFile = pFileName;
		rcOpenDlg.m_ofn.nMaxFile = sizeof(pFileName);
	}


	::PrepareFilterIndex( rcOpenDlg.m_ofn, rcExtString );

	BOOL bResult = rcOpenDlg.DoModal() == IDOK;

	if ( bResult ) {
		rcFileName = rcOpenDlg.GetPathName();
		bLinked = rcOpenDlg.m_bLinkToFile;

		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		m_RegStorage.PutValue( rcInitDirRegValue, pCurrDir );

	}

	return bResult;
}


// ---
BOOL CVisualEditorDlg::EnumAndSelectInterfaces( HDATA *hResultData, PreprocesEnumedInterfacesFunc pPreprocessFunc/* = NULL*/, HDATA hSourceData /*= NULL*/, BOOL bFlushSelected /*= TRUE*/  ) {
	if ( m_pIIServer ) {
		*hResultData = NULL;

		if ( !hSourceData ) {
			DWORD dwSize;
			BYTE *pContainer;
			HRESULT hError = m_pIIServer->EnumInterfaces( itPrototype, &dwSize, &pContainer );

			if ( SUCCEEDED(hError) ) 
				*hResultData = ::DeserializeDTree( pContainer, dwSize );
			else 
				DisplayPISError( hError );
		}
		else 
			*hResultData = ::DATA_Copy( NULL, NULL, hSourceData, DATA_IF_ROOT_INCLUDE );

		if ( *hResultData ) {
			if ( pPreprocessFunc )
				(this->*pPreprocessFunc)( *hResultData );

			CSelectLoadedInterfacesDlg rcDlg( *hResultData, this, bFlushSelected );
			int nResult = rcDlg.DoModal();
			switch ( nResult ) {
				case IDOK : 
					return TRUE;
			default :
				::DATA_Remove( *hResultData, NULL );
				*hResultData = NULL;
				break;
			}
		}
	}
	return FALSE;
}


// ---
void CVisualEditorDlg::LinkInterfaceDefinition( HDATA hTargetData, HDATA hInterfacesToLink ) {
	if ( hTargetData ) {
		HPROP hLinkedProp = ::DATA_Find_Prop( hTargetData, NULL, VE_PID_LINKEDUNIQUEID );
		if ( !hLinkedProp )
			hLinkedProp = ::DATA_Add_Prop( hTargetData, NULL, VE_PID_LINKEDUNIQUEID, 0, 0 );

		if ( hLinkedProp ) {
			HDATA hCurrData = ::DATA_Get_First( hInterfacesToLink, NULL );
			while ( hCurrData ) {
				HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );

				CNodeUID rcCurrUID;
				if ( ::GetNodeUID(hCurrData, rcCurrUID) ) {

					BOOL bFound = FALSE;
					AVP_dword dwLength = ::PROP_Arr_Count( hLinkedProp );
					for ( AVP_dword i=0; i<dwLength && !bFound; i++ ) {
						CNodeUID rcItemUID;
						::PROP_Arr_Get_Items( hLinkedProp, i, &rcItemUID, sizeof(CNodeUID) );
						if ( ::CompareNodesUID(rcCurrUID, rcItemUID) )
							bFound = TRUE;
					}

					if ( !bFound ) 
						::PROP_Arr_Insert( hLinkedProp, PROP_ARR_LAST, &rcCurrUID, sizeof(CNodeUID) );
				}

				hCurrData = hNextData;
			}
		}
	}
}

void CVisualEditorDlg::OnUpdateFileImportDef(CCmdUI* pCmdUI) {
	pCmdUI->Enable(!m_bTechnicalEditting);
}
	
// ---
void CVisualEditorDlg::OnFileImportDef() {
	HDATA hResult = NULL;
	if ( EnumAndSelectInterfaces(&hResult) ) {
		CWaitCursor wait;

// Link to all interfaces

		HDATA hCurrData = ::DATA_Get_First( m_hTree, NULL );
		while ( hCurrData ) {
			if( !IsPluginStaticInterfaceRoot(hCurrData) && 
				!::IsLinkedToFile(hCurrData) ) 
				LinkInterfaceDefinition( hCurrData, hResult );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}

		hCurrData = ::DATA_Get_First( hResult, NULL );
		while ( hCurrData ) {
			HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
			AttachLinkedInterface( m_hTree, hCurrData );

			hCurrData = hNextData;
		}

		CheckLinkedInterfaces( m_hTree );

		int iSelected = m_Interfaces.GetSelectedIndex();

		InitNewTree( m_hTree );
		SetModified( true );

		::SetFocus( m_Interfaces );
		m_Interfaces.SelectNode( iSelected );

		::DATA_Remove( hResult, NULL );
	}


}

// ---
void CVisualEditorDlg::PreprocessForAddDef( HDATA hRootData ) {
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
		if ( ::IsAlreadyAttached(hCurrData, m_hTree) )
			::DATA_Remove( hCurrData, NULL );
		hCurrData = hNextData;
	}
}


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceMnemonicIDStuff( HDATA hData ) {
	CAPointer<char> pLabel;
	HPROP hValueProp;

	pLabel  = ::CreateMnemonicIDLabel( hData );
	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_MNEMONICID ) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(LPCSTR(pLabel)), 0 );
	else 
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_MNEMONICID, AVP_dword(LPCSTR(pLabel)), 0 );
}

// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceDigitalIDStuff( HDATA hData ) {
	HPROP hValueProp;

	AVP_dword nIDId = 0;

	if ( m_pIIDServer ) {
		HRESULT hr = m_pIIDServer->GetID( pic_Interface, &nIDId );
		if ( !SUCCEEDED(hr) )
			DisplayPIDSError( hr );
	}

	if ( !nIDId ) {
		AVP_dword nID = ::DATA_Get_Id( hData, NULL );
		nIDId = GET_AVP_PID_ID(nID);
	}

	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID ) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(nIDId), 0 );
	else 
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_DIGITALID, AVP_dword(nIDId), 0 );
}

// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceRevisionIDStuff( HDATA hData ) {
	HPROP hValueProp;
	AVP_dword nID = 1;

	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_REVISIONID ) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(nID), 0 );
	else 
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_REVISIONID, AVP_dword(nID), 0 );
}

// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceCreationDateStuff( HDATA hData ) {
	HPROP hValueProp;

	CTime rcDate( CTime::GetCurrentTime() );
	SYSTEMTIME rcSysTime;
	rcDate.GetAsSystemTime( rcSysTime );

	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_CREATIONDATE ) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(&rcSysTime), 0 );
	else 
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_CREATIONDATE, AVP_dword(&rcSysTime), 0 );
}


// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceObjectTypeStuff( HDATA hData ) {
	CAPointer<char> pLabel;
	HPROP hValueProp;

	pLabel  = ::CreateObjectTypeLabel( hData );
	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_OBJECTTYPE ) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(LPCSTR(pLabel)), 0 );
	else 
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_OBJECTTYPE, AVP_dword(LPCSTR(pLabel)), 0 );

	CIObject *pObject = m_IOTable.Add( pLabel );
	if ( pObject ) {
		pObject->m_bInterface = TRUE;
		HPROP hIDProp;
		if ( hIDProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID ) ) {
			AVP_dword dwID;
			::PROP_Get_Val( hIDProp, &dwID, sizeof(dwID) );
			pObject->m_dwObjectID = dwID;
		}
	}
}


// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceIncludeFileStuff( HDATA hData ) {
	CAPointer<char> pLabel;
	HPROP hValueProp;

	pLabel  = ::CreateIncludeFileLabel( hData );
	if ( hValueProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IF_INCLUDEFILE) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(LPCSTR(pLabel)), 0 );
	else
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_INCLUDEFILE, AVP_dword(LPCSTR(pLabel)), 0 );
}

// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceAutorNameStuff( HDATA hData ) {
	HPROP hValueProp;

	TCHAR pName[_MAX_PATH] = {0};
	if ( m_pIIDServer ) {
		HRESULT hr = m_pIIDServer->GetVendorName( sizeof(pName), (BYTE *)pName );
		if ( !SUCCEEDED(hr) )
			DisplayPIDSError( hr );
	}

	if ( hValueProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IF_AUTORNAME) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(pName), 0 );
	else
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_AUTORNAME, AVP_dword(pName), 0 );
}


// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceVendorIDStuff( HDATA hData ) {
	CAPointer<char> pLabel;
	HPROP hValueProp;

	AVP_dword nIDId = 1;
	if ( m_pIIDServer ) {
		HRESULT hr = m_pIIDServer->GetID( pic_Vendor, &nIDId );
		if ( !SUCCEEDED(hr) )
			DisplayPIDSError( hr );
	}

	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_VENDORID ) ) 
		::PROP_Set_Val( hValueProp, AVP_dword(nIDId), 0 );
	else 
		::DATA_Add_Prop( hData, NULL, VE_PID_IF_VENDORID, AVP_dword(nIDId), 0 );
}

// ---
void CVisualEditorDlg::CreateOrUpdateInterfaceStuff( HDATA hData ) {
	CreateOrUpdateInterfaceMnemonicIDStuff( hData );
	CreateOrUpdateInterfaceDigitalIDStuff( hData );
	CreateOrUpdateInterfaceRevisionIDStuff( hData );
	CreateOrUpdateInterfaceCreationDateStuff( hData );
	CreateOrUpdateInterfaceObjectTypeStuff( hData );
	CreateOrUpdateInterfaceIncludeFileStuff( hData );
	CreateOrUpdateInterfaceAutorNameStuff( hData );
	CreateOrUpdateInterfaceVendorIDStuff( hData );
}

// ---
void CVisualEditorDlg::OnFileNewdefByBase() {
	HDATA hResult = NULL;
	if ( EnumAndSelectInterfaces(&hResult/*Not needed, PreprocessForAddDef*/) ) {
		HDATA hCurrData = ::DATA_Get_First( hResult, NULL );
		while ( hCurrData ) {
			HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
			
			HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_INTERFACENAME );
			CAPointer<char> pValue;
			if ( hProp )
				pValue = ::GetPropValueAsString( hProp, NULL );

			CString rcName = pValue;

			CNewInterfaceNameDlg rcDlg( rcName, this );
			if ( rcDlg.DoModal() == IDOK ) {
				if ( hProp )
					::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(rcName)), 0 );
				else
					::DATA_Add_Prop( hCurrData, NULL, VE_PID_IF_INTERFACENAME, AVP_dword(LPCTSTR(rcName)), 0 );

				CreateOrUpdateInterfaceStuff( hCurrData );
				::AddHeaderGuidProp( hCurrData );
				::SetNodeUID( hCurrData, TRUE );

				AVP_dword nID = ::CreateDataId( m_hTree );
				::DATA_Replace_ID( hCurrData, NULL, AVP_word(nID) );

				HDATA hInsertData = ::FindFirstImportedData( m_hTree );
				if ( hInsertData ) 
					::DATA_Insert( hInsertData, NULL, hCurrData );
				else
					::DATA_Attach( m_hTree, NULL, hCurrData, DATA_IF_ROOT_INCLUDE );
			}

			hCurrData = hNextData;
		}

		CheckLinkedInterfaces( m_hTree, TRUE );

		int iSelected = m_Interfaces.GetSelectedIndex();

		InitNewTree( m_hTree );
		SetModified( true );

		::SetFocus( m_Interfaces );
		m_Interfaces.SelectNode( iSelected );

		::DATA_Remove( hResult, NULL );
	}
}


// ---
void CVisualEditorDlg::OnFileAddDef() {
	HDATA hResult = NULL;
	if ( EnumAndSelectInterfaces(&hResult, &CVisualEditorDlg::PreprocessForAddDef) ) {
		HDATA hCurrData = ::DATA_Get_First( hResult, NULL );
		while ( hCurrData ) {
			HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
			
			AVP_dword nID = ::CreateDataId( m_hTree );
			::DATA_Replace_ID( hCurrData, NULL, AVP_word(nID) );

			HDATA hInsertData = ::FindFirstImportedData( m_hTree );
			if ( hInsertData ) 
				::DATA_Insert( hInsertData, NULL, hCurrData );
			else
			  ::DATA_Attach( m_hTree, NULL, hCurrData, DATA_IF_ROOT_INCLUDE );

			hCurrData = hNextData;
		}

		CheckLinkedInterfaces( m_hTree, TRUE );

		int iSelected = m_Interfaces.GetSelectedIndex();

		InitNewTree( m_hTree );
		SetModified( true );

		::SetFocus( m_Interfaces );
		m_Interfaces.SelectNode( iSelected );

		::DATA_Remove( hResult, NULL );
	}
}

// ---
void CVisualEditorDlg::SetImpTreePrototypeFile( HDATA hImpTreeData, CString &rcFileName ) {
	HPROP hFileNameProp = ::DATA_Find_Prop( hImpTreeData, NULL, VE_PID_PROTOTYPEFILENAME );
	if ( !hFileNameProp ) 
		::DATA_Add_Prop( hImpTreeData, NULL, VE_PID_PROTOTYPEFILENAME, AVP_dword(LPCSTR(rcFileName)), 0 );
	else
		::PROP_Set_Val( hFileNameProp, AVP_dword(LPCSTR(rcFileName)), 0 );

	char pRelativePath[_MAX_PATH];
	::IOSRelativePathFromFullPath( m_FileName, rcFileName, pRelativePath, sizeof(pRelativePath) );

	hFileNameProp = ::DATA_Find_Prop( hImpTreeData, NULL, VE_PID_PROTOTYPERELATIVEFILENAME );
	if ( !hFileNameProp ) 
		::DATA_Add_Prop( hImpTreeData, NULL, VE_PID_PROTOTYPERELATIVEFILENAME, AVP_dword(LPCSTR(pRelativePath)), 0 );
	else
		::PROP_Set_Val( hFileNameProp, AVP_dword(LPCSTR(pRelativePath)), 0 );
}

// ---
HDATA CVisualEditorDlg::SynchronizeImpAndDef( HDATA hProtoData ) 
{
	if ( !hProtoData ) 
		return NULL;

	HDATA hWorkTree  = ::DATA_Copy( NULL, NULL, m_hTree, DATA_IF_ROOT_INCLUDE );
	HDATA hNewImp = ::DATA_Copy( NULL, NULL, m_hTree, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
	
	// Сканируем Def на интерфейсы
	HDATA hProtoInt = ::DATA_Get_First( hProtoData, NULL );
	while ( hProtoInt ) 
	{
		//-------------------------------------------------------------------------------
		// Ищем такой же - структуру задает Def
		HDATA hImpInt = ::FindInterfaceByInterface( hWorkTree, hProtoInt );
		
		HDATA hNewInt;
		if ( hImpInt ) 
		{
			// Если такой есть, то его копируем
			hNewInt = ::DATA_Copy( NULL, NULL, hImpInt, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
			::DTUT_ExchangeDataPropValues( hProtoInt, hNewInt );
			
			HPROP hNewIncludesProp = ::DATA_Find_Prop( hNewInt, NULL, VE_PID_IF_INCLUDENAMES );
			HPROP hImpIncludesProp = ::DATA_Find_Prop( hImpInt, NULL, VE_PID_IF_INCLUDENAMES );
			if ( hNewIncludesProp && hImpIncludesProp )
				::DTUT_ExchangePropValue( hImpIncludesProp, hNewIncludesProp );
		}
		else
			// Если нет - копируем с Def
			hNewInt = ::DATA_Copy( NULL, NULL, hProtoInt, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
		
		::DATA_Attach( hNewImp, NULL, hNewInt, DATA_IF_ROOT_INCLUDE );

		//-------------------------------------------------------------------------------
		// sub trees synchronization
		
		UINT arrDataTypes[] = 
		{
			VE_PID_IF_TYPES,
			VE_PID_IF_CONSTANTS,
			VE_PID_IF_ERRORCODES,
			VE_PID_PL_MESSAGES,
			VE_PID_IF_METHODS,
			VE_PID_IF_DATASTRUCTURES,
			VE_PID_IF_SYSTEMMETHODS,
			VE_PID_IF_PROPERTIES,
		};

		int nArrDataTypesSize = sizeof(arrDataTypes) / sizeof(arrDataTypes[0]);
		for(int i=0; i<nArrDataTypesSize; i++)
			SyncDatasCommon(hNewInt, hProtoInt, hImpInt, arrDataTypes[i]);

		//-------------------------------------------------------------------------------
		// remove synchronized interface
		if ( hImpInt ) 
			::DATA_Remove( hImpInt, NULL );

		hProtoInt = ::DATA_Get_Next( hProtoInt, NULL );
	}
		
	// Оставшиеся интерфейсы перекладываем в новое дерево
	hProtoInt = ::DATA_Get_First( hWorkTree, NULL );
	while ( hProtoInt ) 
	{
		HDATA hNextData = ::DATA_Get_Next( hProtoInt, NULL );
		DATA_Detach(hProtoInt, NULL);

		//DataAttachWithNextDataId(hNewImp, hProtoInt);
		::DATA_Attach( hNewImp, NULL, hProtoInt, DATA_IF_ROOT_INCLUDE );
		hProtoInt = hNextData;
	}
		
	::DATA_Remove( hWorkTree, NULL );
	SetModified( true );
		
	return hNewImp;
}

// ---
void CVisualEditorDlg::SynchronizeImpAndDef( CString &rcFileName ) {
	if ( ::IOSFileExists(rcFileName) == IOS_ERR_OK ) {
		HDATA hResult = NULL;
		AVP_bool nResult = ::KLDT_DeserializeUsingSWM( rcFileName, &hResult );
		if ( nResult ) {
			AVP_dword nWorkMode;
			::DATA_Get_Val( hResult, NULL, 0, &nWorkMode, sizeof(nWorkMode) );
			if ( nWorkMode != wmt_IDefinition ) {
				::DlgMessageBox( this, IDS_ERR_INCORRECTFILETYPE, MB_OK );
			}
			else {
				if ( ::CompareTreeUniqueIDs(hResult, m_hTree) ) {
					HDATA hNewImp = SynchronizeImpAndDef( hResult );

					InitNewTree( hNewImp );
					SetImpTreePrototypeFile( m_hTree, rcFileName );

					CString rcText;
					rcText.Format( IDS_FILESYNCHRONIZATIONCOMPLETED, LPCTSTR(rcFileName) );
					::DlgMessageBox( this, rcText, MB_OK );
				}
				else {
					CString rcText;
					rcText.Format( IDS_ERR_INCOMPATIBLEFILES, LPCTSTR(rcFileName), LPCTSTR(m_FileName) );
					::DlgMessageBox( this, rcText, MB_OK );
				}
			}
		}
		else {
			CString rcText;
			rcText.Format( IDS_ERR_READFILE, LPCTSTR(rcFileName) );
			::DlgMessageBox( this, rcText, MB_OK );
		}

		::DATA_Remove( hResult, NULL );
	}
}
 
// ---
BOOL CVisualEditorDlg::GetSynchonizeFileName( CString &rcFileName ) {
	CString rcExtString;
	rcExtString.LoadString( IDS_IPP_DEFEXTENTION );

	CString rcFltrString;
	rcFltrString.LoadString( IDS_IPP_FILTER );

	CString rcInitDirRegValue = PRT_INITDIR_REG_VALUE;

	CFileDialog rcOpenDlg( TRUE, rcExtString, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, rcFltrString, this );

	char pCurrDir[_MAX_PATH];
	char pFileName[_MAX_PATH];

	if ( rcFileName.IsEmpty() ) {
		if ( !m_RegStorage.GetValue(rcInitDirRegValue, pCurrDir, sizeof(pCurrDir)) ) 
			::IOSGetCurrentDirectory( pCurrDir, sizeof(pCurrDir) );

		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	else {
		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;

		::IOSFileExtract( rcFileName, pFileName, _MAX_PATH );
		rcOpenDlg.m_ofn.lpstrFile = pFileName;
		rcOpenDlg.m_ofn.nMaxFile = sizeof(pFileName);
	}


	::PrepareFilterIndex( rcOpenDlg.m_ofn, rcExtString );

	BOOL bResult = rcOpenDlg.DoModal() == IDOK;

	if ( bResult ) {
		rcFileName = rcOpenDlg.GetPathName();

		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		m_RegStorage.PutValue( rcInitDirRegValue, pCurrDir );
	}

	return bResult;
}


// ---
void CVisualEditorDlg::OnFileSynchronize() {
	if ( m_pIIServer ) {
		CSArray<CNodeUID> pNodesUIDs;

		HDATA hCurrData = ::DATA_Get_First( m_hTree, NULL );
		while ( hCurrData ) {
			if ( !::IsLinkedToFile(hCurrData) && !IsPluginStaticInterfaceRoot(hCurrData)) {
				CNodeUID rcCurrUID;
				if ( ::GetNodeUID(hCurrData, rcCurrUID) ) {
					UINT uiIndex = pNodesUIDs.FindIt( rcCurrUID );
					if ( uiIndex == SYS_MAX_UINT ) 
						pNodesUIDs.Add( rcCurrUID );
				}
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}

		if ( pNodesUIDs.Count() ) {
			HDATA hRootData = ::DATA_Copy( NULL, NULL, m_hTree, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
			if ( hRootData ) {
				for ( int i=0,c=pNodesUIDs.Count(); i<c; i++ ) {
					DWORD dwSize;
					BYTE *pContainer;
					HRESULT hError = m_pIIServer->LoadInterface( sizeof(CNodeUID), (BYTE *)&pNodesUIDs[i], FALSE, itPrototype, &dwSize, &pContainer );
					if ( SUCCEEDED(hError) ) {
						HDATA hResult = ::DeserializeDTree( pContainer, dwSize );
						if ( hResult ) {
							AVP_dword nID = ::CreateDataId( hRootData );
							::DATA_Replace_ID( hResult, NULL, AVP_word(nID) );
							::SetDataPropWithCheck( hResult, VE_PID_IF_SELECTED, TRUE );
							::DATA_Attach( hRootData, NULL, hResult,  DATA_IF_ROOT_INCLUDE );
						}
					}
					else 
						DisplayPISError( hError );
				}

				HDATA hResultData = NULL;
				if ( EnumAndSelectInterfaces(&hResultData, NULL, hRootData, FALSE) && hResultData ) 
				{
					UpdateWindow();

					int iSelected = m_Interfaces.GetSelectedIndex();

					HDATA hNewImp = SynchronizeImpAndDef( hResultData );
					if ( hNewImp ) {
						for ( int i=pNodesUIDs.MaxIndex(); i>=0; i-- ) {
							HDATA hCurrData = ::DATA_Get_First( hNewImp, NULL );
							HDATA hFirstData = hCurrData;
							while ( hCurrData ) {
								HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
								if ( !::IsLinkedToFile(hCurrData) ) {
									CNodeUID rcCurrUID;
									if ( ::GetNodeUID(hCurrData, rcCurrUID) && 
										   ::CompareNodesUID(rcCurrUID, pNodesUIDs[i]) &&
											 hFirstData != hCurrData ) {
										::DATA_Detach( hCurrData, NULL );
										::DATA_Insert( hFirstData, NULL, hCurrData );
									}
								}
								hCurrData = hNextData;
							}
						}
						InitNewTree( hNewImp );
					}
					
					CheckLinkedInterfaces( hResultData, TRUE );

					if(hNewImp)
						SetModified(true);

					::SetFocus( m_Interfaces );
					m_Interfaces.SelectNode( iSelected );

					CString rcText;
					rcText.LoadString( IDS_SYNCHRONIZATIONCOMPLETED );
					::DlgMessageBox( this, rcText, MB_OK );

					::DATA_Remove( hResultData, NULL );
				}

				::DATA_Remove( hRootData, NULL );
			}
		}
		else {
			::DlgMessageBox( this, IDS_NOUIDSFORSYNCHRONIZATION, MB_OK );
		}
	}
}


/* OLD TECHNOLOGY
// ---
void CVisualEditorDlg::OnFileSynchronize() {
	CAPointer<char> pFileName = lstrcpy( new char[1], "" );
	CAPointer<char> pRelativeFileName = lstrcpy( new char[1], "" );
	CString rcFileName;

	HPROP hFileProp = ::DATA_Find_Prop( m_hTree, NULL, VE_PID_PROTOTYPEFILENAME );
	if ( hFileProp ) 
		pFileName = ::GetPropValueAsString( hFileProp, NULL );

	hFileProp = ::DATA_Find_Prop( m_hTree, NULL, VE_PID_PROTOTYPERELATIVEFILENAME );
	if ( hFileProp ) 
		pRelativeFileName = ::GetPropValueAsString( hFileProp, NULL );

	BOOL bFound = false;
	if ( lstrlen(pFileName) ) {
		if ( ::IOSFileExists(pFileName) != IOS_ERR_OK ) {
			char pFullPath[_MAX_PATH];
			::IOSFullPathFromRelativePath( m_FileName, pRelativeFileName, pFullPath, sizeof(pFullPath) );
			if ( ::IOSFileExists(pFullPath) == IOS_ERR_OK ) {
				pFileName = new char[_MAX_PATH];
				lstrcpy( pFileName, pFullPath );
			}

			if ( !bFound ) {
				CString rcString;
				rcString.Format( IDS_ERR_FILEEXISTS, LPCSTR(pFileName) );
				if ( ::DlgMessageBox(this, rcString, MB_YESNO ) != IDYES )
					return;

				rcFileName = pFileName;
				if ( !GetSynchonizeFileName(rcFileName) ) 
					return;
			}
		}
		bFound = true;
	}

	rcFileName = pFileName;
	if ( bFound || GetSynchonizeFileName(rcFileName) ) {
		int iSelected = m_Interfaces.GetSelectedIndex();

		SynchronizeImpAndDef( rcFileName );

		::SetFocus( m_Interfaces );
		m_Interfaces.SelectNode( iSelected );
	}
}
*/


// ---
void CVisualEditorDlg::OnUpdateFileSrcgen(CCmdUI* pCmdUI) {
	CString rcString;
	switch ( m_nWorkMode ) {
		case wmt_IDefinition :
			rcString.LoadString( IDS_FILE_GENERATEHEADER );
			pCmdUI->SetText( rcString );
			break;
		case wmt_IImplementation : 
			rcString.LoadString( IDS_FILE_GENERATESOURCE );
			pCmdUI->SetText( rcString );
			break;
	}
	pCmdUI->Enable( !m_bTechnicalEditting );
}
/*
// ---
void CVisualEditorDlg::OnUpdateFileImportdef(CCmdUI* pCmdUI) {
	pCmdUI->Enable( m_nWorkMode == wmt_IDefinition );
}
*/

// ---
void CVisualEditorDlg::OnUpdateSynchronize(CCmdUI* pCmdUI) {
	pCmdUI->Enable( !m_bTechnicalEditting && m_nWorkMode == wmt_IImplementation );
}

// ---
void CVisualEditorDlg::OnUpdateFileAddDef(CCmdUI* pCmdUI) {
	pCmdUI->Enable( !m_bTechnicalEditting && m_nWorkMode == wmt_IImplementation );
}


// ---
void CVisualEditorDlg::OnUpdateFileNewdefByBase(CCmdUI* pCmdUI) {
	pCmdUI->Enable( !m_bTechnicalEditting && m_nWorkMode == wmt_IDefinition );

}
// ---
void CVisualEditorDlg::OnFileReinitIServer() {
	if ( m_pIIServer ) {
		HRESULT hError;
		if ( !SUCCEEDED(hError = m_pIIServer->ReInit()) )
			DisplayPISError( hError );
	}
}

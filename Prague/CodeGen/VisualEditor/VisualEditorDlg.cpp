// VisualEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include <avpcontrols/wascmess.h>
#include "../avpcontrols/WASCRes.rh"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include <stuff\cpointer.h>
#include <stuff\pathstr.h>
#include <stuff\layout.h>
#include <stuff\comutils.h>

#include "largecommentdlg.h"
#include "visualeditor.h"
#include "visualeditordlg.h"
#include "propmemberdlg.h"
#include "renameintrelateddlg.h"
#include "choosenamefromlistdlg.h"
#include "CodePages.h"
#include "methdlg.h"

#include <Pr_prop.h>
#include <Pr_types.h>
#include <hashutil/hashutil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const IID IID_IPragueIDServer = {0x60FE9D11,0x06EA,0x11D4,{0x96,0xB1,0x00,0xD0,0xB7,0x1D,0xDD,0xF6}};
const CLSID CLSID_PragueIDServer = {0x60FE9D12,0x06EA,0x11D4,{0x96,0xB1,0x00,0xD0,0xB7,0x1D,0xDD,0xF6}};

const IID IID_IPragueIServer = {0xE635B364,0xB186,0x11D4,{0x96,0xB2,0x44,0x45,0x53,0x54,0x00,0x00}};
const CLSID CLSID_PragueIServer = {0x661A3E92,0xB16B,0x11D4,{0x96,0xB2,0x44,0x45,0x53,0x54,0x00,0x00}};
static UINT g_nIndicators[] = {
	IDC_SB_TEXT,
};


// ---
tLangId gLang;
tCHAR*  gLangId[] = { "all", "rus", "eng" };

// ---------------------------------------------------------------------------------------------------------

#define WM_KICKIDLE         0x036A  // (params unused) causes idles to kick in

BEGIN_MESSAGE_MAP(CVisualEditorDlg, CDialog)
  //{{AFX_MSG_MAP(CVisualEditorDlg)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_SIZE()
  ON_WM_INITMENUPOPUP()
  ON_MESSAGE( WM_KICKIDLE, OnRestoreDlgState )
  ON_MESSAGE( CTTM_POSTLABELEDIT, OnCTTPostLabelEdit )
  ON_MESSAGE( CTTM_LOADCOMBOCONTENTS, OnCTTLoadComboContents )
  ON_MESSAGE( CTTM_WINDOWSHUTDOWN, OnCTTWindowShutDown )
  ON_MESSAGE( CTTM_LOADUPDOWNCONTEXT, OnCTTLoadUpDownContext )
  ON_MESSAGE( CTTM_BEGINLABELEDIT, OnCTTBeginLabelEdit )
  ON_MESSAGE( CTTM_CANCELLABELEDIT, OnCTTCancelLabelEdit )
  ON_MESSAGE( CTTM_CHECKEDCHANGING, OnCTTCheckedChanging )
  ON_MESSAGE( CTTM_REMOVEITEM, OnCTTRemoveItem )
  ON_MESSAGE( CTTM_ITEMSELCHANGING, OnCTTItemSelChanging )
  ON_MESSAGE( CTTM_STATECHANGED, OnCTTStateChanged )
  ON_MESSAGE( CTTM_ENDLABELEDIT, OnCTTEndLabelEdit )
  ON_MESSAGE( CTTM_QUERYDATE, OnCTTQueryDate )
  ON_MESSAGE( CTTM_QUERYDATEFORMAT, OnCTTQueryDateFormat )
  ON_MESSAGE( CTTM_ENDDATELABELEDIT, OnCTTEndDateLabelEdit )
  ON_MESSAGE( CTTM_QUERYTIME, OnCTTQueryTime )
  ON_MESSAGE( CTTM_QUERYTIMEFORMAT, OnCTTQueryTimeFormat )
  ON_MESSAGE( CTTM_ENDTIMELABELEDIT, OnCTTEndTimeLabelEdit )
  ON_MESSAGE( CTTM_QUERYDATETIME, OnCTTQueryDateTime )
  ON_MESSAGE( CTTM_QUERYDATETIMEFORMAT, OnCTTQueryDateTimeFormat )
  ON_MESSAGE( CTTM_ENDDATETIMELABELEDIT, OnCTTEndDateTimeLabelEdit )
  ON_MESSAGE( CTTM_PREPROCESSCONTEXTMENU, OnCTTPreptocessContextMenu )
  ON_MESSAGE( CTTM_DOCONTEXTMENUCOMMAND, OnCTTDoContextMenuCommand )
  ON_MESSAGE( CTTM_ITEMEXPANDING, OnCTTItemExpanding )
  ON_MESSAGE( CTTM_LOADIPADDRESS, OnCTTLoadIPAddress )
  ON_MESSAGE( CTTM_ENDADDRESSLABELEDIT, OnCTTEndAddressLabelEdit )
  ON_MESSAGE( CTTM_GETMENUTOOLBAR, OnCTTGetMenuToolbar )
  ON_MESSAGE( CTTM_DOEXTERNALEDITING, OnCTTDoExternalEditing )
  ON_MESSAGE( CTTM_VSCROLLUP, OnCTTVScrollUp )
  ON_MESSAGE( CTTM_VSCROLLDOWN, OnCTTVScrollDown )
  ON_MESSAGE( CTTM_VSCROLLUP, OnCTTVScrollUp )
  ON_MESSAGE( CTTM_VSCROLLDOWN, OnCTTVScrollDown )
  ON_UPDATE_COMMAND_UI(ID_FILE_NEW,           OnUpdateFileNew)
  ON_UPDATE_COMMAND_UI(ID_FILE_NEWIMP,        OnUpdateFileNewImp)
  ON_UPDATE_COMMAND_UI(ID_FILE_IMPORTDEF,     OnUpdateFileImportDef)
  ON_UPDATE_COMMAND_UI(ID_EDIT_COPY,          OnUpdateEditCopy)
  ON_UPDATE_COMMAND_UI(ID_EDIT_CUT,           OnUpdateEditCut)
  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE,         OnUpdateEditPaste)
  ON_UPDATE_COMMAND_UI(ID_FILE_SRCGEN,        OnUpdateFileSrcgen)
  ON_UPDATE_COMMAND_UI(ID_FILE_SYNCHRONIZE,   OnUpdateSynchronize)
  ON_UPDATE_COMMAND_UI(ID_FILE_ADDDEF,        OnUpdateFileAddDef)
  ON_UPDATE_COMMAND_UI(ID_FILE_NEWDEF_BYBASE, OnUpdateFileNewdefByBase)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS,            OnUpdateOptions)
  ON_WM_TIMER()
  ON_COMMAND(ID_FILE_EXIT,                    OnFileExit)
  ON_COMMAND(ID_FILE_NEW,                     OnFileNew)
  ON_COMMAND(ID_FILE_OPEN,                    OnFileOpen)
  ON_COMMAND(ID_FILE_SAVE,                    OnFileSave)
  ON_COMMAND(ID_FILE_SAVEAS,                  OnFileSaveAs)
  ON_COMMAND(ID_FILE_NEWIMP,                  OnFileNewImp)
  ON_COMMAND(ID_FILE_SRCGEN,                  OnFileSrcgen)
  ON_COMMAND(ID_FILE_IMPORTDEF,               OnFileImportDef)
  ON_COMMAND(ID_FILE_SYNCHRONIZE,             OnFileSynchronize)
  ON_COMMAND(ID_FILE_REINITISERVER,           OnFileReinitIServer)
  ON_COMMAND(ID_EDIT_CUT,                     OnEditCut)
  ON_COMMAND(ID_EDIT_COPY,                    OnEditCopy)
  ON_COMMAND(ID_EDIT_PASTE,                   OnEditPaste)
  ON_COMMAND(ID_FILE_ADDDEF,                  OnFileAddDef)
  ON_COMMAND(ID_FILE_NEWDEF_BYBASE,           OnFileNewdefByBase)
  ON_COMMAND(IDOK,                            OnOK)
  ON_COMMAND(ID_OPTIONS,                      OnOptions)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CVisualEditorDlg dialog

CVisualEditorDlg::CVisualEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVisualEditorDlg::IDD, pParent),
	m_RegStorage( CFG_REG_KEY ),
  m_types( 0, false )
{
	//{{AFX_DATA_INIT(CVisualEditorDlg)
	m_nWorkMode = wmt_IDefinition;
	m_hAccelTable = NULL;
	m_bStateRestored = false;
	m_bChanged = false;
	m_nAppID = AVP_APID_GLOBAL;
	m_hTree = NULL;
	m_hDefinitionData = NULL;
	m_hCCTree = NULL;
	m_hCutData = NULL;
	m_nInterfacesClipFormat = 0;
	m_nDescriptionsClipFormat = 0;
	m_FileName = _T("");
	m_IDefinitionFileName = _T(""); 
	m_IImplementationFileName = _T("");
	//}}AFX_DATA_INIT

	m_uiIdleTimerId = 100;

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_IDescriptions.SetEnableAutoScanForEnable( true );
	//m_IDescriptions.SetEnableOpOnDisabled( false );
	m_IDescriptions.SetEnableOpOnDisabled( true );
	m_IDescriptions.SetAcceptNotChangedText( false );
	
	m_Interfaces.SetEnableAutoScanForEnable( false );
	m_Interfaces.SetShowSelectionAlways( true );
	m_Interfaces.SetAcceptNotChangedText( false );
	
	::DATA_Init_Library( malloc, free );
	::DTUT_Init_Library( malloc, free );
	::KLDT_Init_Library( malloc, free );
	::LoadIO();

	LoadTypesTable();

	gpMainDlg = this;

	m_bTechnicalEditting = TRUE;
	m_bLocalEditting = FALSE;

	DWORD dwTechEditMode;

	// if there is a value, setting to false
	if ( m_RegStorage.GetValue(TECH_EDITING_MODE_REG_VALUE, dwTechEditMode) ) 
		m_bTechnicalEditting = FALSE;

	if ( m_RegStorage.GetValue(LOCAL_EDITING_MODE_REG_VALUE, dwTechEditMode) &&
		dwTechEditMode) 
		m_bLocalEditting = TRUE;

	m_Interfaces.m_bTechnicalEditingMode = m_bTechnicalEditting;
	m_IDescriptions.m_bTechnicalEditingMode = m_bTechnicalEditting;

	if(m_bTechnicalEditting || m_bLocalEditting)
		m_pIIDServer = NULL;
	else
		m_pIIDServer = (IPragueIDServer *)::COMLoadCOMObjectID( CLSID_PragueIDServer, IID_IPragueIDServer );

	m_pIIServer = (IPragueIServer *)::COMLoadCOMObjectID( CLSID_PragueIServer, IID_IPragueIServer );

	VERIFY(m_oOptions.Load());
}

// ---
void CVisualEditorDlg::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVisualEditorDlg)
	DDX_Control(pDX, IDC_INTERFACES_TREE, m_Interfaces);
	DDX_Control(pDX, IDC_IF_DESCRIPTION_TREE, m_IDescriptions);
	DDX_Control(pDX, IDC_SPLITTER, m_Splitter);
	//}}AFX_DATA_MAP
}


// ---
void CVisualEditorDlg::DisplayPIDSError( HRESULT hr ) {
	if ( !SUCCEEDED(hr) && m_pIIDServer ) {
		CString rcError;
		BYTE *pText = NULL;
		if ( SUCCEEDED(m_pIIDServer->GetErrorString(hr, &pText)) ) {
			rcError.Format( IDS_ERR_PIDS_ERROR, pText );
			::DlgMessageBox( this, rcError );
			::CoTaskMemFree( pText );
		}
	}
}


// ---
void CVisualEditorDlg::DisplayPISError( HRESULT hr ) {
	if ( !SUCCEEDED(hr) && m_pIIServer ) {
		CString rcError;
		BYTE *pText = NULL;
		if ( SUCCEEDED(m_pIIServer->GetErrorString(hr, &pText)) ) {
			rcError.Format( IDS_ERR_PIS_ERROR, pText );
			::DlgMessageBox( this, rcError );
			::CoTaskMemFree( pText );
		}
	}
}

// ---
BOOL CVisualEditorDlg::DestroyWindow() {

	m_RegStorage.PutValue( SPLIT_TLR_REG_VALUE, m_Splitter.GetTopLeftToRightPercent() );
	m_RegStorage.PutValue( SPLIT_LTB_REG_VALUE, m_Splitter.GetLeftTopToBottomPercent() );
  
  if ( gLang >= li_last )
    gLang = li_all;
  m_RegStorage.PutValue( COMM_LANG_REG_VALUE, gLangId[gLang] );
  
	WINDOWPLACEMENT rcPlacement;
	rcPlacement.length = sizeof(rcPlacement);

	GetWindowPlacement( &rcPlacement );
	m_RegStorage.PutValue( WINDOWPLACEMENT_REG_VALUE, (BYTE *)&rcPlacement, sizeof(rcPlacement) );

	CString rcValueName;
	for ( int i=0,c=m_RecentFiles.Count(); i<c; i++ ) {
		rcValueName.Format( "RecentFile%d", i );
		m_RegStorage.PutValue( rcValueName, m_RecentFiles[i] );
	}

	m_Interfaces.ForEach( ::DeleteNodesData, NULL, -1 );
	m_IDescriptions.ForEach( ::DeleteNodesData, NULL, -1 );

	m_Interfaces.DeleteItemWithSubtree( -1 );
	m_IDescriptions.DeleteItemWithSubtree( -1 );

	if ( m_hDefinitionData ) {
		::DATA_Remove( m_hDefinitionData, NULL );
		m_hDefinitionData = NULL;
	}

	if ( m_hTree ) {
		::DATA_Remove( m_hTree, NULL );
		m_hTree = NULL;
	}

	if ( m_pIIDServer )
		m_pIIDServer->Release();

	if ( m_pIIServer ) {
		m_pIIServer->Done();
		m_pIIServer->Release();
	}

	if ( ghSMRootData ) {
		::DATA_Remove( ghSMRootData, NULL );
		ghSMRootData = NULL;
	}

	return CDialog::DestroyWindow();
}


// ---
BOOL CVisualEditorDlg::PreTranslateMessage( MSG* pMsg ) {
	if ( m_hAccelTable ) {
/*
		HWND hWndFocus = ::GetFocus();
		if ( hWndFocus == m_Interfaces.GetSafeHwnd() || hWndFocus == m_IDescriptions.GetSafeHwnd() ) {
*/
			if ( ::TranslateAccelerator(*this, m_hAccelTable, pMsg) )
				return TRUE;
//		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


// ---
static AVP_dword SetOneIDInUse( HDATA hData, void* pUserData ) {

	if(NULL == pUserData)
		return 0;

	HPROP hProp;

	hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID );
	if ( hProp ) {
		DWORD dwID = ::GetPropNumericValueAsDWord( hProp );
		((IPragueIDServer *)pUserData)->SetIDInUse( pic_Interface, dwID );
	}

	hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_PL_DIGITALID );
	if ( hProp ) {
		DWORD dwID = ::GetPropNumericValueAsDWord( hProp );
		((IPragueIDServer *)pUserData)->SetIDInUse( pic_Plugin, dwID );
	}
		
	return 0;
}

// ---
void CVisualEditorDlg::ScanTreeForSetIDInUse() {
	if ( m_hTree && m_pIIDServer ) {
		::DATA_For_Each( m_hTree, NULL, DATA_IF_ROOT_INCLUDE, SetOneIDInUse, m_pIIDServer );
	}
}


// ---
void CVisualEditorDlg::SetObjectIsUsed( const TCHAR *pObjectName ) {
	CIObject *pIObject = m_IOTable.Find( pObjectName );
	if ( pIObject) 
		pIObject->m_dwRefCount++;
}


// ---
AVP_dword CVisualEditorDlg::AcceptUsedObject( const TCHAR *pObjectName, AVP_dword &dwType ) {
	AVP_dword nNewType = 0;
	CIObject *pIObject = m_IOTable.Find( pObjectName );
	if ( pIObject) {
		pIObject->m_dwRefCount++;
		::SaveUsedObject( pIObject, m_hCCTree );
		nNewType = /*::MaxTypeID() + */pIObject->m_dwObjectID;
		dwType = (pIObject->m_bInterface ? VE_NTT_INTERFACE : VE_NTT_USERTYPE);
	}
	else {
		// !!! Petrovitch -- nNewType = ::GTypeName2TypeID( pObjectName );
		nNewType = ::PropTypeName2PropTypeID( pObjectName );
		dwType = VE_NTT_GLOBALTYPE;
	}

	return nNewType;
}
			
// ---
void CVisualEditorDlg::RefuseUsedObject( HDATA hData, AVP_dword dwTypePropID ) {
	if ( hData ) {
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, dwTypePropID );
		if ( hProp ) {
			CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
			CIObject *pIObject = m_IOTable.Find( pValue );
			if ( pIObject && pIObject->m_dwRefCount ) {
				pIObject->m_dwRefCount--;
				if ( !pIObject->m_dwRefCount && !m_IOTable.IsIncludeNameUsed(pIObject) ) {
					::RemoveUsedObject( pIObject, m_hCCTree );
				}
			}
		}
	}
}

// ---
void CVisualEditorDlg::ReloadIncludesTree() {
	int nItemInd = m_IDescriptions.FirstThat( ::FindForNodeType, (void *)sdt_Includes, -1 );
	if ( nItemInd >= 0 ) {
		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
		CCheckTreeItem *item = items[nItemInd];
		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = CPPD(item)->m_hProp;

		int nCount = ::PROP_Arr_Count( hProp );

		if ( nCount >= CONTENTS_SCROLL_SIZE ) 
			CPPD(items[nItemInd])->m_nScrollPos++;

		ReloadContentsList( items, nItemInd, hData, hProp );
	}
}

// ---
void CVisualEditorDlg::FlushDataTree( HDATA hRootData ) {
//	if ( m_hTree != hRootData ) {
		// Удалить данные в элементах поддерева
		m_IDescriptions.ForEach( ::DeleteNodesData, NULL, -1 );
		m_Interfaces.ForEach( ::DeleteNodesData, NULL, -1 );
//	}

	// Уничтожить поддерево элементов
	m_Interfaces.DeleteItemWithSubtree( -1 );
	m_IDescriptions.DeleteItemWithSubtree( -1 );

	m_hCutData = NULL;

	m_IOTable.Flush();

	if ( m_hTree && m_hTree != hRootData ) {
		::DATA_Remove( m_hTree, NULL );
		m_hTree = NULL;
	}
}

			
// ---
void CVisualEditorDlg::InitNewTree( HDATA hRootData ) {
	if ( !hRootData ) 
		CreateNewInterfaceTreeStuff();
	else 
	{
		FlushDataTree( hRootData );
		m_hTree = hRootData;

		m_hCCTree = NULL;
		m_nAppID = AVP_APID_GLOBAL;
		SetModified( false );

		LoadInterfaceDescriptionNodes( m_hTree );

		//ScanTreeForSetIDInUse();
	}
}

// ---------------------------------------------------------------------------------------------------------

// ---
BOOL CVisualEditorDlg::CheckPluginID( HDATA hData, int nItemInd, const char *pText ) {
	if ( hData ) {
		if ( m_pIIDServer ) {
			DWORD dwID = ::A2I( pText );
			HRESULT hr = m_pIIDServer->CheckIDInUse( pic_Plugin, dwID );
			if ( !SUCCEEDED(hr) ) {
				DisplayPIDSError( hr );
				return false;
			}
			else
				if ( hr == S_OK ) {
					::DlgMessageBox( this, IDS_ERR_IDALREADYUSED, MB_ICONSTOP | MB_OK );
					return false;
				}
		}
	}
	return true;
}

// ---
static BOOL CheckDuplicateInterfaceProp( HDATA hData, const char *pName, const char *pID, const char *pRevision ) {
	HDATA hParentData = ::DATA_Get_Dad(hData, NULL);
	if ( pName && pID && pRevision && hParentData ) {
		HDATA hCurrData = ::DATA_Get_First( hParentData, NULL );
		while ( hCurrData ) {
			if ( hCurrData != hData ) {
				CAPointer<char> pCurrName;
				CAPointer<char> pCurrID;
				CAPointer<char> pCurrRevision;

				HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_INTERFACENAME );
				if ( hProp ) 
					pCurrName = ::GetPropValueAsString( hProp, NULL );

				hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_DIGITALID );
				if ( hProp ) 
					pCurrID = ::GetPropValueAsString( hProp, NULL );

				hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_REVISIONID );
				if ( hProp ) 
					pCurrRevision = ::GetPropValueAsString( hProp, NULL );

				if ( LPCSTR(pCurrName)			&& !lstrcmpi(pName, pCurrName) &&
						 LPCSTR(pCurrID)				&& !lstrcmpi(pID, pCurrID) &&
						 LPCTSTR(pCurrRevision) && !lstrcmpi(pRevision, pCurrRevision) )
					return TRUE;
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
	return FALSE;
}

// ---
BOOL CVisualEditorDlg::CheckInterfaceName( HDATA hData, int nItemInd, const char *pText ) {
	if ( hData ) {
		CAPointer<char> pID;
		CAPointer<char> pRevision;
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID );
		if ( hProp ) 
			pID = ::GetPropValueAsString( hProp, NULL );

		hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_REVISIONID );
		if ( hProp ) 
			pRevision = ::GetPropValueAsString( hProp, NULL );

		if ( ::CheckDuplicateInterfaceProp(hData, pText, pID, pRevision) ) {
			::DlgMessageBox( this, IDS_ERR_IFALREADYEXISTS, MB_ICONSTOP | MB_OK );
			return false;
		}
		else
			return true;
	}
	return true;
}

// ---
BOOL CVisualEditorDlg::CheckInterfaceName( HDATA hData ) {
	if ( hData ) {
		CAPointer<char> pID;
		CAPointer<char> pRevision;
		CAPointer<char> pName;
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_INTERFACENAME );
		if ( hProp )
			pName = ::GetPropValueAsString( hProp, NULL );

		hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID );
		if ( hProp ) 
			pID = ::GetPropValueAsString( hProp, NULL );

		hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_REVISIONID );
		if ( hProp ) 
			pRevision = ::GetPropValueAsString( hProp, NULL );

		if ( ::CheckDuplicateInterfaceProp(hData, pName, pID, pRevision) ) {
			::DlgMessageBox( this, IDS_ERR_IFEXISTS, MB_ICONEXCLAMATION | MB_OK );
			return false;
		}
		else
			return true;
	}
	return true;
}

// ---
BOOL CVisualEditorDlg::CheckInterfaceID( HDATA hData, int nItemInd, const char *pText ) {
	if ( hData ) {
		CAPointer<char> pName;
		CAPointer<char> pID;
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_INTERFACENAME );
		if ( hProp ) 
			pName = ::GetPropValueAsString( hProp, NULL );

		hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_REVISIONID );
		if ( hProp ) 
			pID = ::GetPropValueAsString( hProp, NULL );

		if ( ::CheckDuplicateInterfaceProp(hData, pName, pText, pID) ) {
			::DlgMessageBox( this, IDS_ERR_IFALREADYEXISTS, MB_ICONSTOP | MB_OK );
			return false;
		}
		else
			if ( m_pIIDServer ) {
				DWORD dwID = ::A2I( pText );
				HRESULT hr = m_pIIDServer->CheckIDInUse( pic_Interface, dwID );
				if ( !SUCCEEDED(hr) ) {
					DisplayPIDSError( hr );
					return false;
				}
				else
					if ( hr == S_OK ) {
						::DlgMessageBox( this, IDS_ERR_IDALREADYUSED, MB_ICONSTOP | MB_OK );
						return false;
					}
			}
	}
	return true;
}

// ---
BOOL CVisualEditorDlg::CheckInterfaceRevisionID( HDATA hData, int nItemInd, const char *pText ) {
	if ( hData ) {
		CAPointer<char> pName;
		CAPointer<char> pID;
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_INTERFACENAME );
		if ( hProp ) 
			pName = ::GetPropValueAsString( hProp, NULL );

		hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID );
		if ( hProp ) 
			pID = ::GetPropValueAsString( hProp, NULL );

		if ( ::CheckDuplicateInterfaceProp(hData, pName, pID, pText) ) {
			::DlgMessageBox( this, IDS_ERR_IFALREADYEXISTS, MB_ICONSTOP | MB_OK );
			return false;
		}
		else
			return true;
	}
	return true;
}


// ---
static BOOL CheckDuplicateSingleProp( HDATA hParentData, HDATA hCheckData, AVP_dword dwPropID, const char *pValue ) {
	if ( pValue && hParentData ) {
		HDATA hCurrData = ::DATA_Get_First( hParentData, NULL );
		while ( hCurrData ) {
			if ( hCheckData != hCurrData ) {
				CAPointer<char> pCurrValue;

				HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, dwPropID );
				if ( hProp ) 
					pCurrValue = ::GetPropValueAsString( hProp, NULL );

				if ( LPCSTR(pCurrValue) && !lstrcmpi(pValue, pCurrValue) )
					return TRUE;
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
	return FALSE;
}


// ---
static BOOL CheckDuplicateInterfaceMethodProp( HDATA hParentData, HDATA hCheckData, const char *pName ) {
	return ::CheckDuplicateSingleProp( hParentData, hCheckData, VE_PID_IFM_METHODNAME, pName );
}


// ---
BOOL CVisualEditorDlg::CheckInterfaceMethodName( HDATA hData, int nItemInd, const char *pText ) {
	if ( !::IsSeparator(hData) ) {
		HDATA hParentData;
		if ( hData ) 
			hParentData = ::DATA_Get_Dad(hData, NULL);
		else 
			hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

		if ( ::CheckDuplicateInterfaceMethodProp(hParentData, hData, pText) ) {
			::DlgMessageBox( this, IDS_ERR_IFMALREADYEXISTS, MB_ICONSTOP | MB_OK );
			return false;
		}
	}
	return true;
}

// ---
static BOOL CheckDuplicateInterfaceMethodParamProp( HDATA hParentData, HDATA hCheckData, const char *pName ) {
	return ::CheckDuplicateSingleProp( hParentData, hCheckData, VE_PID_IFMP_PARAMETERNAME, pName );
}


// ---
BOOL CVisualEditorDlg::CheckInterfaceMethodParamName( HDATA hData, int nItemInd, const char *pText ) {
	HDATA hParentData;
	if ( hData ) 
		hParentData = ::DATA_Get_Dad(hData, NULL);
	else 
		hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

	if ( ::CheckDuplicateInterfaceMethodParamProp(hParentData, hData, pText) ) {
		::DlgMessageBox( this, IDS_ERR_IFMPALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}

	return true;
}

// ---
static BOOL CheckDuplicateInterfacePropertyProp( HDATA hParentData, HDATA hCheckData, const char *pName ) {
	return ::CheckDuplicateSingleProp( hParentData, hCheckData, VE_PID_IFP_PROPNAME, pName );
}


// ---
BOOL CVisualEditorDlg::CheckInterfacePropertyName( HDATA hData, int nItemInd, const char *pText ) {
	HDATA hParentData;
	if ( hData ) 
		hParentData = ::DATA_Get_Dad(hData, NULL);
	else 
		hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

	if ( ::CheckDuplicateInterfacePropertyProp(hParentData, hData, pText) ) {
		::DlgMessageBox( this, IDS_ERR_IFPALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}

	return true;
}

// ---
BOOL CVisualEditorDlg::CheckInterfaceMessageClassName( HDATA hData, int nItemInd, const char *pText ) 
{
	HDATA hParentData = hData ?  
		::DATA_Get_Dad(hData, NULL) : ::FindParentHData( &m_IDescriptions, nItemInd );

	if( ::CheckDuplicateSingleProp( hParentData, hData, VE_PID_IFM_MESSAGECLASSNAME, pText ) )
	{
		::DlgMessageBox( this, IDS_ERR_IFCNALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}
	
	return true;
}

// ---
BOOL CVisualEditorDlg::CheckInterfaceMessageName( HDATA hData, int nItemInd, const char *pText ) 
{
	HDATA hParentData = hData ?  
		::DATA_Get_Dad(hData, NULL) : ::FindParentHData( &m_IDescriptions, nItemInd );
	
	if( ::CheckDuplicateSingleProp( hParentData, hData, VE_PID_IFM_MESSAGENAME, pText ) )
	{
		::DlgMessageBox( this, IDS_ERR_IFMNALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}
	
	return true;
}

// ---
static BOOL CheckDuplicateInterfacePropertyIDProp( HDATA hParentData, HDATA hCheckData, const char *pName ) {
	return ::CheckDuplicateSingleProp( hParentData, hCheckData, VE_PID_IFP_DIGITALID, pName );
}


// ---
BOOL CVisualEditorDlg::CheckInterfacePropertyID( HDATA hData, int nItemInd, const char *pText ) {
	AVP_dword nID = A2I( pText );
	if ( nID < pUSER_MIN_PROP_ID || nID > pUSER_MAX_PROP_ID ) {
		CString rcString;
		rcString.Format( IDS_ERR_IFPIDOUTOFRANGE, nID, pUSER_MIN_PROP_ID, pUSER_MAX_PROP_ID );
		::DlgMessageBox( this, rcString, MB_ICONSTOP | MB_OK );
		return false;
	}

	HDATA hParentData;
	if ( hData ) 
		hParentData = ::DATA_Get_Dad(hData, NULL);
	else 
		hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

	if ( ::CheckDuplicateInterfacePropertyIDProp(hParentData, hData, pText) ) {
		::DlgMessageBox( this, IDS_ERR_IFPIDALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}

	return true;
}

// ---
BOOL CVisualEditorDlg::CheckInterfaceObjectTypeName( HDATA hData, int nItemInd, const char *pText ) {
	HDATA hParentData;
	if ( hData ) 
		hParentData = ::DATA_Get_Dad(hData, NULL);
	else 
		hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

	if ( ::CheckDuplicateSingleProp(hParentData, hData, VE_PID_IF_OBJECTTYPE, pText) ) {
		::DlgMessageBox( this, IDS_ERR_IFONAMEALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}
/*
	if ( m_IOTable.Find(pText) ) {
		BOOL bResult = ::DlgMessageBox( this, IDS_ERR_IFONAMEUSED, MB_YESNO ) == IDYES;
		return false;
	}
*/
	return true;
}

// ---
static BOOL CheckDuplicateInterfaceTypeProp( HDATA hParentData, HDATA hCheckData, const char *pName ) {
	return ::CheckDuplicateSingleProp( hParentData, hCheckData, VE_PID_IFT_TYPENAME, pName );
}


// ---
BOOL CVisualEditorDlg::CheckInterfaceTypeName( HDATA hData, int nItemInd, const char *pText ) {
	HDATA hParentData;
	if ( hData ) 
		hParentData = ::DATA_Get_Dad(hData, NULL);
	else 
		hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

	if ( ::CheckDuplicateInterfaceTypeProp(hParentData, hData, pText) ) {
		::DlgMessageBox( this, IDS_ERR_IFTALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}

	return true;
}

// ---
BOOL CVisualEditorDlg::CheckTypeNameIsUsed( const char *pText ) {
	CIObject *pObject = m_IOTable.Find( pText );
	if ( pObject && pObject->m_dwRefCount ) {
		CString rcMessage;
		rcMessage.Format( IDS_ERR_TYPENAMEUSED, pText );
		return ::DlgMessageBox( this, rcMessage, MB_YESNO ) == IDYES;
	}
	return true;
}

// ---
BOOL CVisualEditorDlg::CheckTypeNameIsUsedForDelete( const char *pText, BOOL* pbUsed ) {
	if(pbUsed)	*pbUsed = false;
	CIObject *pObject = m_IOTable.Find( pText );
	if ( pObject && pObject->m_dwRefCount ) {
		if(pbUsed) *pbUsed = true;
		CString rcMessage;
		rcMessage.Format( IDS_ERR_TYPENAMEUSEDDEL, pText );
		return ::DlgMessageBox( this, rcMessage, MB_YESNO ) == IDYES;
	}
	return true;
}


// ---
static BOOL CheckDuplicateInterfaceConstantProp( HDATA hParentData, HDATA hCheckData, const char *pName ) {
	return ::CheckDuplicateSingleProp( hParentData, hCheckData, VE_PID_IFC_CONSTANTNAME, pName );
}


// ---
BOOL CVisualEditorDlg::CheckInterfaceConstantName( HDATA hData, int nItemInd, const char *pText ) {
	if ( !::IsSeparator(hData) ) {
		HDATA hParentData;
		if ( hData ) 
			hParentData = ::DATA_Get_Dad(hData, NULL);
		else 
			hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

		if ( ::CheckDuplicateInterfaceConstantProp(hParentData, hData, pText) ) {
			::DlgMessageBox( this, IDS_ERR_IFCALREADYEXISTS, MB_ICONSTOP | MB_OK );
			return false;
		}
	}
	return true;
}

// ---
static BOOL CheckDuplicateInterfaceErrorCodeProp( HDATA hParentData, HDATA hCheckData, const char *pName ) {
	return ::CheckDuplicateSingleProp( hParentData, hCheckData, VE_PID_IFC_CONSTANTNAME, pName );
}

							  
// ---
BOOL CVisualEditorDlg::CheckInterfaceErrorCodeName( HDATA hData, int nItemInd, const char *pText ) {
	HDATA hParentData;
	if ( hData ) 
		hParentData = ::DATA_Get_Dad(hData, NULL);
	else 
		hParentData = ::FindParentHData( &m_IDescriptions, nItemInd );

	if ( ::CheckDuplicateInterfaceErrorCodeProp(hParentData, hData, pText) ) {
		::DlgMessageBox( this, IDS_ERR_IFCALREADYEXISTS, MB_ICONSTOP | MB_OK );
		return false;
	}

	return true;
}

// ---
struct MinMaxTableItem {
	AVP_dword     m_dwType;
	AVP_longlong  m_nMinValue;
	AVP_qword     m_nMaxValue;
};

// ---
static MinMaxTableItem gMinMaxTable[] = {
	{	avpt_short		, SHRT_MIN	, SHRT_MAX		},
	{ avpt_long			, LONG_MIN	,	LONG_MAX		},
	{	avpt_byte			, 0					, UCHAR_MAX		},
	{	avpt_word			,	0					,	USHRT_MAX		},
	{	avpt_dword		, 0					,	ULONG_MAX		},
	{	avpt_qword		, 0					,	_UI64_MAX		},
	{	avpt_longlong	, _I64_MIN	,	_I64_MAX		},
	{	avpt_bool 		, 0					,	1        		},
	{	avpt_int			,	INT_MIN		, INT_MAX			},
	{	avpt_uint			, 0					, UINT_MAX		},
};

static int gMinMaxTableCount = sizeof(gMinMaxTable) / sizeof(gMinMaxTable[0]);

// ---
BOOL CVisualEditorDlg::CheckPropertyDefValue( HDATA hData, int nItemInd, const char *pText ) {
	HPROP hTypeProp;
	if ( hTypeProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IFP_TYPE) ) {
		AVP_dword dwPragueType = ::GetPropNumericValueAsDWord(hTypeProp);
		for ( int i=0,c=gPropTypes.Count(); i<c; i++ ) {
			if ( gPropTypes[i]->m_nType == dwPragueType ) {
				AVP_dword dwDTType = gPropTypes[i]->m_nDTType;
				if ( NUMERIC_TYPE(dwDTType) ) {
					CAPointer<char> pValue = ::GetConstantValueAsString( m_hTree, pText );
					if ( !pValue )
						pValue = ::StrDuplicate( pText );
					if ( ::CheckConvertable(pValue) ) {
						int nBase = ::GetConvertBase( pValue );
						if ( nBase > 0 ) {
							char *pEndPtr;
							AVP_longlong nValue;
							switch ( dwDTType ) {
								case avpt_qword    :
								case avpt_longlong :
									nValue = ::A2I64( pValue, &pEndPtr, nBase );
									break;
								default :
									nValue = strtol( pValue, &pEndPtr, nBase );
									break;
							}
							if ( !*pEndPtr ) {
								for ( int i=0; i<gMinMaxTableCount; i++ ) {
									if ( gMinMaxTable[i].m_dwType == dwDTType ) {
										switch ( dwDTType ) {
											case avpt_qword    :
											case avpt_longlong :
												break;
											default :
												if ( !(AVP_long(nValue) >= AVP_long(gMinMaxTable[i].m_nMinValue) && 
															 AVP_dword(nValue) <= AVP_dword(gMinMaxTable[i].m_nMaxValue)) ) {
													CString rcString;
													rcString.Format( IDS_ERR_VALUEOUTOFRANGE, AVP_dword(gMinMaxTable[i].m_nMinValue), AVP_dword(gMinMaxTable[i].m_nMaxValue) );
													::DlgMessageBox( this, rcString, MB_ICONSTOP | MB_OK );
													return false;
												}
										}
										return true;
									}
								}
							}
						}
					}
					// !!!Petrovitch ::DlgMessageBox( this, IDS_ERR_INCORRECTVALUE, MB_ICONSTOP | MB_OK );
					// !!!Petrovitch return false;
					return true;
				}
				else {
					switch ( dwDTType ) {
						case avpt_char :
						case avpt_wchar :
							if ( lstrlen(pText) > 1 ) {
								::DlgMessageBox( this, IDS_ERR_INCORRECTVALUE, MB_ICONSTOP | MB_OK );
								return false;
							}
							break;
						case avpt_date     :
						case avpt_time     :
						case avpt_datetime :
							break;
						case avpt_bin      :
							if ( !::CheckBinaryValue(pText) ) {
								::DlgMessageBox( this, IDS_ERR_INCORRECTVALUE, MB_ICONSTOP | MB_OK );
								return false;
							}
							break;
					}
				}
			}
		}
	}
	return true;
}

// ---
BOOL CVisualEditorDlg::CheckPropType( HDATA hData, int nItemInd, AVP_dword dwNewType ) {
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFP_TYPE );
	if ( hProp ) {
		AVP_dword dwType = ::GetPropNumericValueAsDWord( hProp );
		if ( dwType != dwNewType ) {
			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IFPSS_VARIABLEDEFVALUE) ) {
				if ( ::DlgMessageBox(this, IDS_ERR_DISCARDDEFVALUE, MB_YESNO) != IDYES )
					return false;
				else {
					::SetPropValue( hValueProp, "", NULL );

					CAPointer<char> pLabel = ::GetPropValueAsString( hValueProp, NULL );
					pLabel = ::CreateDisplayLabelSTR( IDS_PROPDEFVALUE_PREFIX, pLabel );
					::SetParentChildNodeText( m_IDescriptions, nItemInd, sdt_Property, VE_PID_IFPSS_VARIABLEDEFVALUE_ID, pLabel );
					
					SetPropTypifiedDefValue( hData );
				}
			}
		}
	}
	return true;
}

// ---------------------------------------------------------------------------------------------------------

// ---
static AVP_dword g_dwExchangeIDTable[][6] = {
	{ VE_PID_IF_METHODS		, VE_PID_IFM_RESULTTYPE	, VE_PID_IFM_RESULTTYPENAME	, VE_PID_IFMP_TYPE, VE_PID_IFMP_TYPENAME, 0 },
	{ VE_PID_IF_TYPES			, VE_PID_IFT_BASETYPE		, VE_PID_IFT_BASETYPENAME		, 0								, 0										, 0 },
	{ VE_PID_IF_CONSTANTS	, VE_PID_IFC_BASETYPE		, VE_PID_IFT_BASETYPENAME		, 0								, 0										, 0 },
};

static int g_nExchangeIDTableCount = sizeof(g_dwExchangeIDTable) / sizeof(g_dwExchangeIDTable[0]);

// ---
static void ExchangeNodeTypeByID( HDATA hData, CIObject *pObject, int nRowIndex, int nColIndex ) {
	if ( g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 1] ) {
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 2] );
		if ( hProp ) {
			CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
			if ( !lstrcmpi(pValue, pObject->m_rcObjectName) ) {
				hProp =  ::DATA_Find_Prop( hData, NULL, g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 1] );
				if ( hProp )
					::PROP_Set_Val( hProp, pObject->m_dwObjectID, sizeof(pObject->m_dwObjectID) );
			}
		}
	}
}


// ---
static void ExchangeNodeTypesByID( HDATA hData, CIObject *pObject, int nRowIndex, int nColIndex ) {
	if ( g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 1] ) {
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) {
			::ExchangeNodeTypeByID( hCurrData, pObject, nRowIndex, nColIndex );
			::ExchangeNodeTypesByID( hData, pObject, nRowIndex, nColIndex + 1 );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
}

// ---
void CVisualEditorDlg::ExchangeTypesByInterfaceID( CIObject *pObject ) {
	HDATA hCurrData = ::DATA_Get_First( m_hTree, NULL );
	while ( hCurrData ) {
		for ( int i=0; i<g_nExchangeIDTableCount; i++ ) {
			MAKE_ADDR1( nAddr, g_dwExchangeIDTable[i][0] );
			HDATA hData = ::DATA_Find( hCurrData, nAddr );
			if ( hData )
				::ExchangeNodeTypesByID( hData, pObject, i, 0 );
		}

		if ( m_hCCTree == hCurrData )
			ReloadInterfaceCCTree( hCurrData );

		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
}

// ---
static void ExchangeNodeTypeByObject( HDATA hData, CIObject *pObject, int nRowIndex, int nColIndex ) {
	if ( g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 1] ) {
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 1] );
		if ( hProp ) {
			AVP_dword dwValue = ::GetPropNumericValueAsDWord( hProp );
			if ( dwValue == pObject->m_dwObjectID ) {
				hProp =  ::DATA_Find_Prop( hData, NULL, g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 2] );
				if ( hProp )
					::PROP_Set_Val( hProp, AVP_dword(LPCSTR(pObject->m_rcObjectName)), 0 );
			}
		}
	}
}


// ---
static void ExchangeNodeTypesByObject( HDATA hData, CIObject *pObject, int nRowIndex, int nColIndex ) {
	if ( g_dwExchangeIDTable[nRowIndex][nColIndex * 2 + 1] ) {
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) {
			::ExchangeNodeTypeByObject( hCurrData, pObject, nRowIndex, nColIndex );
			::ExchangeNodeTypesByObject( hData, pObject, nRowIndex, nColIndex + 1 );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
}

// ---
void CVisualEditorDlg::ExchangeTypesByInterfaceObject( CIObject *pObject ) {
	HDATA hCurrData = ::DATA_Get_First( m_hTree, NULL );
	while ( hCurrData ) {
		for ( int i=0; i<g_nExchangeIDTableCount; i++ ) {
			MAKE_ADDR1( nAddr, g_dwExchangeIDTable[i][0] );
			HDATA hData = ::DATA_Find( hCurrData, nAddr );
			if ( hData )
				::ExchangeNodeTypesByID( hData, pObject, i, 0 );
		}

		if ( m_hCCTree == hCurrData )
			ReloadInterfaceCCTree( hCurrData );

		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
}

// ---------------------------------------------------------------------------------------------------------
// ---
void CVisualEditorDlg::SetPropTypifiedDefValue( HDATA hData ) {
	CAPointer<char> pLabel;
	HPROP hValueProp;
	HPROP hTypeProp;
	if ( hValueProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IFPSS_VARIABLEDEFVALUE) ) {
		pLabel = ::GetPropValueAsString( hValueProp, NULL );
		if ( hTypeProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IFP_TYPE) ) {
			AVP_dword dwPragueType = ::GetPropNumericValueAsDWord(hTypeProp);
			for ( int i=0,c=gPropTypes.Count(); i<c; i++ ) {
				if ( gPropTypes[i]->m_nType == dwPragueType ) {
					HPROP hTypifiedValueProp = ::FindPropByID( hData, VE_PID_IFPSS_VARTYPIFIEDDEFVALUE_ID );
					if ( hTypifiedValueProp && GET_AVP_PID_TYPE(::PROP_Get_Id(hTypifiedValueProp)) != gPropTypes[i]->m_nDTType ) {
						::DATA_Remove_Prop_H( hData, NULL, hTypifiedValueProp );
						hTypifiedValueProp = NULL;
					}
					if ( !hTypifiedValueProp ) {
						AVP_dword dwPID = MAKE_AVP_PID(VE_PID_IFPSS_VARTYPIFIEDDEFVALUE_ID, VE_APP_ID, gPropTypes[i]->m_nDTType, 0);
						hTypifiedValueProp = ::DATA_Add_Prop( hData, NULL, dwPID, 0, 0 );
					}
					CAPointer<char> pValue = ::GetConstantValueAsString( m_hTree, pLabel );
					if ( pValue ) {
						int nSizeOf = gPropTypes[i]->m_nSizeOf;
						if ( nSizeOf >= 0 ) {
							int nBase = ::GetConvertBase( pValue );
							char *pEndPtr;
							AVP_qword nValue = ::A2I64( pValue, &pEndPtr, nBase );

							nValue >>= (nSizeOf * CHAR_BIT);
							if ( nValue ) {
								CString rcString;
								rcString.Format( IDS_ERR_VALUEOUTOFLENGTH, pValue );
								::DlgMessageBox( this, rcString, MB_ICONEXCLAMATION | MB_OK );
							}
						}
					  ::SetPropValue( hTypifiedValueProp, pValue, NULL );
					}
					else
					  ::SetPropValue( hTypifiedValueProp, pLabel, NULL );
					/* не выполняем обратного преобразования
					pLabel = ::GetPureValueString( hTypifiedValueProp );
					::SetPropValue( hValueProp, pLabel, NULL );
					*/
					break;
				}
			}
		}
	}
}
// ---------------------------------------------------------------------------------------------------------

// ---
static AVP_dword g_dwExchangeTypeTable[] = {
	VE_PID_IFP_TYPENAME_ID,
	VE_PID_IFM_RESULTTYPENAME_ID,
	VE_PID_IFMP_TYPENAME_ID,
	VE_PID_IFDM_TYPENAME_ID,
	VE_PID_IFT_BASETYPENAME_ID,
	VE_PID_IFC_BASETYPENAME_ID,
};

static int g_nExchangeTypeTableCount = sizeof(g_dwExchangeTypeTable) / sizeof(g_dwExchangeTypeTable[0]);

// ---
void CVisualEditorDlg::ExchangeTypesByTypeName( LPCTSTR pOldTypeName, LPCTSTR pNewTypeName ) {
	m_IOTable.Update( pOldTypeName, pNewTypeName );

	BOOL bResult = TRUE;
	CCTItemsArray &items = *m_IDescriptions.GetItemsArray();

	CPointer<CTTMessageInfo> info = new CTTMessageInfo;
	info->m_Value.m_pItemText = ::StrDuplicate( pNewTypeName );
	info->m_nWindowID = IDC_IF_DESCRIPTION_TREE;
	for ( int i=0,c=items.Count(); i<c; i++ ) {
		CCheckTreeItem *item = items[i];
		if ( !IS_EMPTY_DATA(item) &&
			CPPD(item)->m_hProp &&
			CPPD(item)->m_hProp != ::DATA_Find_Prop(CPPD(item)->m_hData, NULL, 0) ) {

			for ( int j=0; j<g_nExchangeTypeTableCount; j++ ) {
				if ( GET_AVP_PID_ID(::PROP_Get_Id(CPPD(item)->m_hProp)) == g_dwExchangeTypeTable[j] ) {
					HPROP hProp = CPPD(item)->m_hProp;
					if ( hProp ) {
						CAPointer<TCHAR> pLabel = ::GetPropValueAsString( hProp, NULL );
						if ( !lstrcmp(pLabel, pOldTypeName ) ) {
							info->m_nItemIndex = i;
							OnCTTPostLabelEdit( WPARAM(&bResult), LPARAM((CTTMessageInfo*)info) );
						}
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------

// ---
static AVP_dword g_dwExchangeConstantTable[] = {
	VE_PID_IFPSS_VARIABLEDEFVALUE_ID,
};

static int g_nExchangeConstantTableCount = sizeof(g_dwExchangeConstantTable) / sizeof(g_dwExchangeConstantTable[0]);

// ---
void CVisualEditorDlg::ExchangeConstantsByConstantName( LPCTSTR pOldConstantName, LPCTSTR pNewConstantName ) {
	m_IOTable.Update( pOldConstantName, pNewConstantName );
	
	BOOL bResult = TRUE;
	CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
	
	CPointer<CTTMessageInfo> info = new CTTMessageInfo;
	info->m_Value.m_pItemText = ::StrDuplicate( pNewConstantName );
	info->m_nWindowID = IDC_IF_DESCRIPTION_TREE;
	for ( int i=0,c=items.Count(); i<c; i++ ) {
		CCheckTreeItem *item = items[i];
		if ( !IS_EMPTY_DATA(item) &&
			CPPD(item)->m_hProp &&
			CPPD(item)->m_hProp != ::DATA_Find_Prop(CPPD(item)->m_hData, NULL, 0) ) {
			
			for ( int j=0; j<g_nExchangeConstantTableCount; j++ ) {
				if ( GET_AVP_PID_ID(::PROP_Get_Id(CPPD(item)->m_hProp)) == g_dwExchangeConstantTable[j] ) {
					HPROP hProp = CPPD(item)->m_hProp;
					if ( hProp ) {
						CAPointer<TCHAR> pLabel = ::GetPropValueAsString( hProp, NULL );
						if ( !lstrcmp(pLabel, pOldConstantName) ) {
							info->m_nItemIndex = i;
							OnCTTPostLabelEdit( WPARAM(&bResult), LPARAM((CTTMessageInfo*)info) );
						}
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------


// ---
void CVisualEditorDlg::LoadContentsList( CCTItemsArray &items, 
	HDATA hData, HPROP hProp, int nFirstItemLevel, int nScrollPos, BOOL bReadOnly ) 
{
	CControlTreeItem *item;

	CString rcString;

	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType  = GET_AVP_PID_TYPE(nID);

	int nCount = ::PROP_Arr_Count( hProp );

	if ( nCount >= CONTENTS_SCROLL_SIZE ) {
		item = new CControlTreeItem;
		item->SetLevel( nFirstItemLevel );
		item->SetVScrollUpAlias();
		items.Add( item );
	}

	for ( int i=nScrollPos; i<(nScrollPos+CONTENTS_SCROLL_SIZE); i++ ) 
	{
		CPPDescriptor *pDescr;

		item = new CControlTreeItem;

		item->SetLevel( nFirstItemLevel );

		if ( bReadOnly )
			item->SetSimpleStringAlias();
		else
			::SetItemEditType( hProp, item );

		pDescr = new CPPDescriptor( hData, hProp );
		pDescr->m_nArrayIndex = i;
		item->SetData( (void *)pDescr, false );

		items.Add( item );

		if ( i < nCount ) {
			CAPointer<char> pValue = ::GetPropArrayValueAsString( hProp, i, NULL );
			item->SetText( LPCTSTR(pValue) );
			item->SetCanBeRemoved( !bReadOnly );

			pDescr->m_nSpecialType = sdt_ScrollListValue;
		}
		else {
			if ( !bReadOnly ) {
				rcString.LoadString( IDS_WAS_PP_NEWVALUE );
				item->SetText( rcString );
				item->SetCanBeRemoved( false );
				
				pDescr->m_nSpecialType = sdt_ScrollListNewValue;
			}
			else {
				items.RemoveAt( items.GetUpperBound() );
				if ( !items.GetOwns() )
					delete item;
			}
			break;
		}
	}

	if ( nCount >= CONTENTS_SCROLL_SIZE ) {
		item = new CControlTreeItem;
		item->SetLevel( nFirstItemLevel );
		item->SetVScrollDownAlias();
		items.Add( item );
	}
}

// ---
void CVisualEditorDlg::ReloadContentsList( CCTItemsArray &items, int nParentInd, HDATA hData, HPROP hProp ) {

	CCheckTreeItem *item = items[nParentInd];

	int nFirstItemLevel = item->GetLevel() + 1;
	int nScrollPos      = CPPD(item)->m_nScrollPos;

	m_IDescriptions.ForEach( ::DeleteNodesData, NULL, nParentInd );
	
	CCTItemsArray newItems( false );

	LoadContentsList( newItems, hData, hProp, nFirstItemLevel, nScrollPos, false );

	int	nSelected = m_IDescriptions.GetSelectedIndex();

	m_IDescriptions.UpdateItemsArray( nParentInd, newItems );

	if ( nSelected < items.GetSize() ) {
		CControlTreeItem *item = (CControlTreeItem *)items[nSelected];

		if ( item->IsVScrollUpAlias() )
			nSelected++;

		if ( item->IsVScrollDownAlias() )
			nSelected--;

		m_IDescriptions.SelectNode( nSelected );
	}
}
// ---------------------------------------------------------------------------------------------------------

// ---
void CVisualEditorDlg::SetErrorCodeDisplayString( HDATA hErrorCodeData ) {
	if ( hErrorCodeData ) {
		int nMethodInd = m_IDescriptions.FirstThat( ::FindForNodeHDATA, (void *)hErrorCodeData, -1 );
		if ( nMethodInd >= 0 ) {
			HPROP hProp = ::DATA_Find_Prop( hErrorCodeData, NULL, VE_PID_IFE_ERRORCODENAME );
			CAPointer<char> pLabel = ::GetErrorCodeNameDisplayString( hErrorCodeData, hProp );
			m_IDescriptions.SetTreeItemText( nMethodInd, LPCSTR(pLabel) );
		}
	}
}

// ---
void CVisualEditorDlg::SetMethodDisplayString( HDATA hMethodData ) {
	if ( hMethodData ) {
		int nMethodInd = m_IDescriptions.FirstThat( ::FindForNodeHDATA, (void *)hMethodData, -1 );
		if ( nMethodInd >= 0 ) {
			CAPointer<char> pLabel = ::GetMethodNameDisplayString( hMethodData );
			m_IDescriptions.SetTreeItemText( nMethodInd, LPCSTR(pLabel) );
		}
	}
}

// ---
void CVisualEditorDlg::SetPropertyDisplayString( HDATA hPropertyData ) {
	if ( hPropertyData ) {
		int nPropertyInd = m_IDescriptions.FirstThat( ::FindForNodeHDATA, (void *)hPropertyData, -1 );
		if ( nPropertyInd >= 0 ) {
			HPROP hProp = ::DATA_Find_Prop( hPropertyData, NULL, VE_PID_IFP_PROPNAME );
			CAPointer<char> pLabel = ::GetPropertyNameDisplayString( hPropertyData, hProp, m_nWorkMode );
			m_IDescriptions.SetTreeItemText( nPropertyInd, LPCSTR(pLabel) );
		}
	}
}

// ---
void CVisualEditorDlg::SetConstantDisplayString( HDATA hConstantData ) {
	if ( hConstantData ) {
		int nConstantInd = m_IDescriptions.FirstThat( ::FindForNodeHDATA, (void *)hConstantData, -1 );
		if ( nConstantInd >= 0 ) {
			HPROP hProp = ::DATA_Find_Prop( hConstantData, NULL, VE_PID_IFC_CONSTANTNAME );
			CAPointer<char> pLabel = ::GetConstantNameDisplayString( hConstantData, hProp, m_nWorkMode );
			m_IDescriptions.SetTreeItemText( nConstantInd, LPCSTR(pLabel) );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVisualEditorDlg message handlers
// ---
void CVisualEditorDlg::OnOK() {
}

// ---
void CVisualEditorDlg::OnCancel() {
	// Не отдавать папе - не завершаемся по Escape
}

// ---
void CVisualEditorDlg::OnSysCommand( UINT nID, LONG lParam ) {
	UINT nItemID = (nID & 0xFFF0);

	switch ( nItemID ) {
		case SC_CLOSE :
			OnAppExit();
			return;
	}

	CDialog::OnSysCommand( nID, lParam );
}



// ---
BOOL CVisualEditorDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// check idserver sql and user registration error
	if(m_pIIDServer)		
	{
		m_pIIDServer->ForceRegistryUse(m_oOptions.m_bUseRegistryNotSql);

		//char strUserName[2000];

		// in this call performing registry update in addition, so its must be done first
		//HRESULT hRes = m_pIIDServer->GetVendorName(2000, (BYTE*)strUserName);
		// in case of error release interface and disable all dependent operations
		//if(FAILED(hRes))
		//{
		//	DisplayPIDSError( hRes );
		//	m_pIIDServer->Release();
		//	m_pIIDServer = NULL;
		//}
	}

	if ( m_pIIServer ) {   
		HRESULT hError = m_pIIServer->SetUIParent( *this );
		if ( SUCCEEDED(hError) )
			hError = m_pIIServer->Init();
		DisplayPISError( hError );
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon
  
  CString rcString;
  rcString.LoadString( IDS_INTERFACESCLIPBOARDFORMAT );
  m_nInterfacesClipFormat = ::RegisterClipboardFormat( rcString );  
  
  rcString.LoadString( IDS_DESCRIPTIONSCLIPBOARDFORMAT );
  m_nDescriptionsClipFormat = ::RegisterClipboardFormat( rcString );  
  
  // --------------------------------------------------------------------------------------------------------
  m_StatusBar.CreateStatusBar( this, g_nIndicators, sizeof(g_nIndicators)/sizeof(g_nIndicators[0]), SBPS_NOBORDERS | SBPS_STRETCH );
  RepositionBars( AFX_IDW_STATUS_BAR, AFX_IDW_STATUS_BAR, 0 );
  
  COLORREF rcColor = ::GetSysColor(COLOR_3DFACE);
  m_StatusBar.SetBkColor( 0, rcColor, rcColor );
  
  m_StatusBar.SetBarStyle( m_StatusBar.GetBarStyle() | CBRS_BORDER_TOP | CBRS_BORDER_3D );
  // --------------------------------------------------------------------------------------------------------
  
  ///	
  InitNewTree();
  ///
  
  ::SetWindowLong( m_Splitter.GetSafeHwnd(), GWL_ID, IDC_SPLITTER );
  
  HINSTANCE hInst = AfxFindResourceHandle( MAKEINTRESOURCE(IDR_DIALOG_ACCELERATORS), RT_ACCELERATOR );
  m_hAccelTable = ::LoadAccelerators( hInst, MAKEINTRESOURCE(IDR_DIALOG_ACCELERATORS) );
  
  WINDOWPLACEMENT rcPlacement;
  rcPlacement.length = sizeof(rcPlacement);
  
  DWORD dwSize = sizeof(rcPlacement);
  if ( m_RegStorage.GetValue(WINDOWPLACEMENT_REG_VALUE, (BYTE *)&rcPlacement, dwSize) )
    SetWindowPlacement( &rcPlacement );
  
  char val[10];
  gLang = li_all;
  if ( m_RegStorage.GetValue(COMM_LANG_REG_VALUE,val,sizeof(val)) ) {
    for( uint i=0; i<countof(gLangId); i++ ) {
      if ( !_strcmpi(gLangId[i],val) ) {
        gLang = (tLangId)i;
        break;
      }
    }
  }

  DWORD nRegValue;

	if ( m_RegStorage.GetValue(SPLIT_TLR_REG_VALUE, nRegValue) ) {
		if ( nRegValue > 100 )
			nRegValue = 50;
		m_Splitter.SetTopLeftToRightPercent( (short)nRegValue );
	}

	if ( m_RegStorage.GetValue(SPLIT_LTB_REG_VALUE, nRegValue) ) {
		if ( nRegValue > 100 )
			nRegValue = 50;
		m_Splitter.SetLeftTopToBottomPercent( (short)nRegValue );
	}

	m_Interfaces.SetFocus();
	m_Interfaces.SelectNode( 0 );

  m_IDescriptions.SetEnableOpOnDisabled( false );

	SetTimer(m_uiIdleTimerId, 250, NULL);

	return FALSE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVisualEditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVisualEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// ---
void CVisualEditorDlg::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	
	RepositionBars(AFX_IDW_STATUS_BAR, AFX_IDW_STATUS_BAR, 0);
	
  static RULE rules[] = {
    // Action    Act-on                   Relative-to           Offset
    // ------    ------                   -----------           ------
    {  lSTRETCH, lRIGHT(IDC_SPLITTER),				lRIGHT(lPARENT)							,-5  },
    {  lSTRETCH, lBOTTOM(IDC_SPLITTER),				lTOP(AFX_IDW_STATUS_BAR)		,-5  },

    {  lEND																																		 }
	};

  Layout_ComputeLayout( GetSafeHwnd(), rules );
}




// ---
void CVisualEditorDlg::OnInitMenuPopup( CMenu* pMenu, UINT nIndex, BOOL bSysMenu ) {
	if ( bSysMenu )
		return;     // don't support system menu

	ASSERT(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
	else 
		if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL) {
			CWnd* pParent = GetTopLevelParent();
				// child windows don't have menus -- need to go to the top!
			if (pParent != NULL && (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL) {
				int nIndexMax = ::GetMenuItemCount(hParentMenu);
				for ( int nIndex = 0; nIndex < nIndexMax; nIndex++ ) {
					if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
					{
						// when popup is found, m_pParentMenu is containing menu
						state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
						break;
					}
				}
			}
		}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0 || 
			  (state.m_nID >= ID_FIRSTRECENTFILE && state.m_nID < (ID_FIRSTRECENTFILE + MAX_RECENT_FILES)))
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1) {
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1) {

				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else {
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if ( nCount < state.m_nIndexMax ) {
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID) {

				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}

	CDialog::OnInitMenuPopup( pMenu, nIndex, bSysMenu );
}



// ---
LRESULT CVisualEditorDlg::OnCTTWindowShutDown( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		// Пока так - потом сделать удаление данных от другого дерева
		CCTItemsArray *items = m_IDescriptions.GetItemsArray();

		if ( items ) {
			for ( int i=0; i<items->GetSize(); i++ )
				delete CPPD((*items)[i]);
		}
	}

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		// Пока так - потом сделать удаление данных от другого дерева
		CCTItemsArray *items = m_Interfaces.GetItemsArray();

		if ( items ) {
			for ( int i=0; i<items->GetSize(); i++ )
				delete CPPD((*items)[i]);
		}
	}
	return 0;
}


// ---
void CVisualEditorDlg::GetNotificationData(LPARAM i_lParam,
		CTTMessageInfo*& o_info,
		CVEControlTreeBase*& o_pTreeCtrl,
		CCTItemsArray*&	o_pCtrlItems,
		int&			o_nItemIndex,
		CCheckTreeItem *& o_pItem,
		HDATA& o_hData,
		HPROP& o_hProp)
{
	o_pCtrlItems = NULL;
	o_pTreeCtrl = NULL;
	o_nItemIndex = 0;
	o_pItem = NULL;
	o_hData = NULL;
	o_hProp = NULL;

	o_info = (CTTMessageInfo *)i_lParam;

	if( o_info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) 
		o_pTreeCtrl = &m_IDescriptions;
	else
	if( o_info->m_nWindowID == IDC_INTERFACES_TREE ) 
		o_pTreeCtrl = &m_Interfaces;
	else
	{
		ASSERT(FALSE);
		return;
	}

	o_pCtrlItems	= o_pTreeCtrl->GetItemsArray();
	o_nItemIndex	= o_info->m_nItemIndex;
	o_pItem =		(*o_pCtrlItems)[o_nItemIndex];

	if(!IS_EMPTY_DATA(o_pItem))
	{
		o_hData = CPPD(o_pItem)->m_hData;
		o_hProp = CPPD(o_pItem)->m_hProp;
	}
}

// ---
LRESULT CVisualEditorDlg::OnCTTLoadUpDownContext( WPARAM wParam, LPARAM lParam ) {
	return 0;
}
   

// ---
LRESULT CVisualEditorDlg::OnCTTPostLabelEdit( WPARAM wParam, LPARAM lParam ) 
{
	CTTMessageInfo *info = (CTTMessageInfo  *)lParam;

// -------------------------------------------------------------------------------------------
	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		int nItemInd         = info->m_nItemIndex;

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
		CCheckTreeItem *item = items[nItemInd];

		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = CPPD(item)->m_hProp;

		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item)  && IS_NODE_TYPE(item) ) {
			if ( !hData ) {

				CreateInterfaceTypeNodeContents( items, nItemInd );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else {
				CAPointer<TCHAR> pLabel = ::GetPropValueAsString( hProp, NULL );
				ExchangeTypesByTypeName( pLabel, info->m_Value.m_pItemText );
				::DATA_Set_Val( hData, NULL, VE_PID_IFT_TYPENAME, AVP_dword(info->m_Value.m_pItemText), 0 );
					
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}
		// Отредактирован BaseTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFT_BASETYPENAME_ID) ) {

			RefuseUsedObject( hData, VE_PID_IFT_BASETYPENAME );

			::DATA_Set_Val( hData, NULL, VE_PID_IFT_BASETYPENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			AVP_dword dwTypeType = 0;
			AVP_dword nNewType = AcceptUsedObject( info->m_Value.m_pItemText, dwTypeType );
			
			::DATA_Set_Val( hData, NULL, VE_PID_IFT_BASETYPE, AVP_dword(nNewType), 0 );
			::SetDataPropWithCheck( hData, VE_PID_NODETYPETYPE, dwTypeType );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_BASETYPENAME_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}


		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item)  && IS_NODE_CONSTANT(item) ) {
			if ( !hData ) 
			{
				CreateInterfaceConstantNodeContents( items, nItemInd );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else 
			{
				CAPointer<TCHAR> pLabel = ::GetPropValueAsString( hProp, NULL );
				ExchangeConstantsByConstantName( pLabel, info->m_Value.m_pItemText );

				::DATA_Set_Val( hData, NULL, VE_PID_IFC_CONSTANTNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				SetConstantDisplayString( hData );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактировано имя класса сообщений
		if ( IS_NODE_LABEL(item)  && IS_NODE_MESSAGE_CLASS(item) ) 
		{
			if ( !hData ) 
				CreateInterfaceMessageClassNodeContents( items, nItemInd );
			else 
				::PROP_Set_Val( hProp, AVP_dword( info->m_Value.m_pItemText ), 0 );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактировано имя сообщениR
		if ( IS_NODE_LABEL(item)  && IS_NODE_MESSAGE(item) ) 
		{
			if ( !hData ) 
				CreateInterfaceMessageNodeContents( items, nItemInd );				
			else 
				::PROP_Set_Val( hProp, AVP_dword( info->m_Value.m_pItemText ), 0 );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован BaseTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFC_BASETYPENAME_ID) ) {

			RefuseUsedObject( hData, VE_PID_IFC_BASETYPENAME );

			::DATA_Set_Val( hData, NULL, VE_PID_IFC_BASETYPENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			AVP_dword dwTypeType = 0;
			AVP_dword nNewType = AcceptUsedObject( info->m_Value.m_pItemText, dwTypeType );
			
			::DATA_Set_Val( hData, NULL, VE_PID_IFC_BASETYPE, AVP_dword(nNewType), 0 );
			::SetDataPropWithCheck( hData, VE_PID_NODETYPETYPE, dwTypeType );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_CONSTTYPENAME_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			SetConstantDisplayString( hData );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован Value узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFC_VALUE_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFC_VALUE, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_VALUE_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			SetConstantDisplayString( hData );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактировано имя класса сообщений
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGECLASSNAME_ID) ) {
			
			::PROP_Set_Val( hProp, AVP_dword(info->m_Value.m_pItemText), 0 );
			
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;

			return 0;
		}

		// Отредактирован идентификатор класса сообщений
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGECLASSID_ID) ) {
			
			::PROP_Set_Val( hProp, AVP_dword( A2I(info->m_Value.m_pItemText) ), 0 );
			
			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_IDENTIFICATOR_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );
			
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован идентификатор сообщения
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGEID_ID) ) {
			
			::PROP_Set_Val( hProp, AVP_dword( A2I(info->m_Value.m_pItemText) ), 0 );
			
			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_IDENTIFICATOR_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );
			
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

    // Отредактирован send point сообщения
    if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGE_SEND_POINT_ID) ) {
      ::DATA_Set_Val( hData, NULL, VE_PID_IFM_MESSAGE_SEND_POINT, AVP_dword(info->m_Value.m_pItemText), 0 );
      // Восстановить отображение узла
      CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, VE_PID_IFM_MESSAGE_SEND_POINT );
      pLabel = ::CreateDisplayLabelSTR( IDS_MSG_SEND_POINT_PREFIX, pLabel );
      m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );
      *(reinterpret_cast<BOOL *>(wParam)) = TRUE;
      return 0;
    }
    
    // Отредактирован контекст сообщения
    if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGECONTEXT_ID) ) {
      ::DATA_Set_Val( hData, NULL, VE_PID_IFM_MESSAGECONTEXT, AVP_dword(info->m_Value.m_pItemText), 0 );
      // Восстановить отображение узла
      CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, VE_PID_IFM_MESSAGECONTEXT );
      pLabel = ::CreateDisplayLabelSTR( IDS_MSG_CONTEXT_PREFIX, pLabel );
      m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );
      *(reinterpret_cast<BOOL *>(wParam)) = TRUE;
      return 0;
    }
    
		// Отредактированы данные сообщения
    if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGEDATA_ID) ) {
      ::DATA_Set_Val( hData, NULL, VE_PID_IFM_MESSAGEDATA, AVP_dword(info->m_Value.m_pItemText), 0 );
      // Восстановить отображение узла
      CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, VE_PID_IFM_MESSAGEDATA );
      pLabel = ::CreateDisplayLabelSTR( IDS_MSG_DATA_PREFIX, pLabel );
      m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );
      *(reinterpret_cast<BOOL *>(wParam)) = TRUE;
      return 0;
    }

		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item)  && IS_NODE_ERRORCODE(item) ) {
			if ( !hData ) {

				CreateInterfaceErrorCodeNodeContents( items, nItemInd );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else {
				::DATA_Set_Val( hData, NULL, VE_PID_IFE_ERRORCODENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				SetErrorCodeDisplayString( hData );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item)  && IS_NODE_METHOD(item) ) {
			if ( !hData ) 
			{
				CreateInterfaceMethodNodeContents( items, nItemInd );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else {
				::DATA_Set_Val( hData, NULL, VE_PID_IFM_METHODNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				SetMethodDisplayString( hData );

				// update crc
				CPubMethodInfo method( hData, false );
				method.SetMethodID( ::iCountCRC32str(info->m_Value.m_pItemText) );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактирован InterfaceSubtypeID узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_SUBTYPEID_ID))  {

			::DATA_Remove_Prop_H( hData, NULL, hProp );

			if ( *info->m_Value.m_pItemText >= L'0' && *info->m_Value.m_pItemText <= L'9' ) {
				AVP_dword nID = A2I( info->m_Value.m_pItemText );
				hProp = ::DATA_Add_Prop( hData, NULL, VE_PID_IF_SUBTYPEID, nID, 0 );
			}
			else 
			{
				CAPointer<char> strId = GetConstantValueAsString(m_hTree, info->m_Value.m_pItemText);
				AVP_dword nID = A2I( strId );
				hProp = ::DATA_Add_Prop( hData, NULL, VE_PID_IF_SUBTYPEID, nID, 0 );
			}

			delete CPPD(item);
			CPPDescriptor *pDescr = new CPPDescriptor( hData, hProp );
			item->SetData( (void *)pDescr, false );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_SUBTYPEID_PREFIX, info->m_Value.m_pItemText);
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован ShortComment узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_SHORTCOMMENT_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_SHORTCOMMENT, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_SHORTCOMMENT_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован LargeComment узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_LARGECOMMENT_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_LARGECOMMENT, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, VE_PID_LARGECOMMENT );
			pLabel = ::CreateDisplayLabelSTR( IDS_LARGECOMMENT_PREFIX, pLabel );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован BehaviorComment узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_BEHAVIORCOMMENT_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_BEHAVIORCOMMENT, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, VE_PID_BEHAVIORCOMMENT );
			pLabel = ::CreateDisplayLabelSTR( IDS_BEHAVIORCOMMENT_PREFIX, pLabel );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован ResultComment узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_VALUECOMMENT_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_VALUECOMMENT, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, VE_PID_VALUECOMMENT );
			pLabel = ::CreateDisplayLabelSTR( IDS_VALUECOMMENT_PREFIX, pLabel );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован ResultTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_RESULTTYPENAME_ID) ) {

			RefuseUsedObject( hData, VE_PID_IFM_RESULTTYPENAME );

			::DATA_Set_Val( hData, NULL, VE_PID_IFM_RESULTTYPENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			AVP_dword dwTypeType = 0;
			AVP_dword nNewType = AcceptUsedObject( info->m_Value.m_pItemText, dwTypeType );
			
			::DATA_Set_Val( hData, NULL, VE_PID_IFM_RESULTTYPE, AVP_dword(nNewType), 0 );
			::SetDataPropWithCheck( hData, VE_PID_NODETYPETYPE, dwTypeType );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_RESULTNAME_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			SetMethodDisplayString( hData );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован ResultDefValue узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_RESULTDEFVALUE_ID) ) {

			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFM_RESULTDEFVALUE ) ) 
				::SetPropValue( hValueProp, info->m_Value.m_pItemText, NULL );
			else {
				hValueProp = ::DATA_Add_Prop( hData, NULL, VE_PID_IFM_RESULTDEFVALUE, AVP_dword(0), 0 );
				::SetPropValue( hValueProp, info->m_Value.m_pItemText, NULL );
			}

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::GetPropValueAsString( hValueProp, NULL );
			pLabel = ::CreateDisplayLabelSTR( IDS_RESULTDEFVALUE_PREFIX, pLabel );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

	
		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item)  && IS_NODE_METHODPARAM(item) ) {
			if ( !hData ) 
			{
				CreateInterfaceMethodParamNodeContents( items, nItemInd );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else 
			{
				::DATA_Set_Val( hData, NULL, VE_PID_IFMP_PARAMETERNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				SetMethodDisplayString( ::DATA_Get_Dad(hData, NULL) );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактирован ParamTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFMP_TYPENAME_ID) ) {

			RefuseUsedObject( hData, VE_PID_IFMP_TYPENAME );

			::DATA_Set_Val( hData, NULL, VE_PID_IFMP_TYPENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			AVP_dword dwTypeType;
			AVP_dword nNewType = AcceptUsedObject( info->m_Value.m_pItemText, dwTypeType );
			
			::DATA_Set_Val( hData, NULL, VE_PID_IFMP_TYPE, AVP_dword(nNewType), 0 );
			::SetDataPropWithCheck( hData, VE_PID_NODETYPETYPE, dwTypeType );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PARAMTYPENAME_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			SetMethodDisplayString( ::DATA_Get_Dad(hData, NULL) );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован LastCallPropReadFuncName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFLCP_READFUNC_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFLCP_READFUNC, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLSREADFUNC_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			::SetParentChildNodeText( m_IDescriptions, nItemInd, sdt_PropScopeLocal, VE_PID_IFLCP_READFUNC_ID, pLabel );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован LastCallPropWriteFuncName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFLCP_WRITEFUNC_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFLCP_WRITEFUNC, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLSWRITEFUNC_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			::SetParentChildNodeText( m_IDescriptions, nItemInd, sdt_PropScopeLocal, VE_PID_IFLCP_WRITEFUNC_ID, pLabel );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item) && IS_NODE_PROPERTY(item) ) 
		{
			if ( !hData ) 
			{
				CreateInterfacePropertyNodeContents( items, nItemInd );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else 
			{
				::DATA_Set_Val( hData, NULL, VE_PID_IFP_PROPNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				BOOL bOldPredefinedProp = false;
				HPROP hPredefinedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFP_PREDEFINED );
				if ( hPredefinedProp )
					bOldPredefinedProp = !!::GetPropNumericValueAsDWord( hPredefinedProp );

				BOOL bPredefinedProp = !!::PropName2PropID( info->m_Value.m_pItemText );

				if ( bPredefinedProp || bPredefinedProp != bOldPredefinedProp ) 
				{
					SetDwordProp(hData, VE_PID_IFP_PREDEFINED, bPredefinedProp);
					m_IDescriptions.ForEach( ::DeleteNodesData, NULL, nItemInd );

					CCTItemsArray newItems( false );
					int nFirstItemLevel = item->GetLevel() + 1;
					LoadInterfacePropertyStuff( hData, newItems, nFirstItemLevel, false, false);
					m_IDescriptions.UpdateItemsArray( nItemInd, newItems );
				}

				SetPropertyDisplayString( hData );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактирован DigitalID узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_DIGITALID_ID) ) {

			AVP_dword nID = A2I( info->m_Value.m_pItemText );

			::DATA_Set_Val( hData, NULL, VE_PID_IFP_DIGITALID, nID, 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelINT( IDS_DIGITALID_PREFIX, nID );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PropertyTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_TYPENAME_ID) ) {

			RefuseUsedObject( hData, VE_PID_IFP_TYPENAME );

			::DATA_Set_Val( hData, NULL, VE_PID_IFP_TYPENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			AVP_dword dwTypeType = 0;
			AVP_dword nNewType = AcceptUsedObject( info->m_Value.m_pItemText, dwTypeType );
			
			//AVP_dword nNewType = ::PropTypeName2PropTypeID(info->m_Value.m_pItemText);
			
			::DATA_Set_Val( hData, NULL, VE_PID_IFP_TYPE, AVP_dword(nNewType), 0 );
			::SetDataPropWithCheck( hData, VE_PID_NODETYPETYPE, dwTypeType );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PROPTYPENAME_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			int nDefValueInd = ::FindParentChildNodeIndex( m_IDescriptions, nItemInd, sdt_Property, VE_PID_IFPSS_VARIABLEDEFVALUE_ID );
			if ( nDefValueInd >= 0 ) {
				CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
				int nDefValueLevel = items[nDefValueInd]->GetLevel();

				m_IDescriptions.ForEach( ::DeleteNodesData, NULL, nDefValueInd );
				delete CPPD(items[nDefValueInd]);

				//::FullyDeleteItem( m_IDescriptions, nDefValueInd, false );
				CCTItemsArray newItems( false );
				CreatePropertyNode(newItems, nDefValueLevel, hData, VE_PID_IFPSS_VARIABLEDEFVALUE);
				m_IDescriptions.UpdateItemsArray( nDefValueInd, newItems, true );
			}

			//SetPropTypifiedDefValue( hData );

			SetPropertyDisplayString( hData );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PropertyLocalScopeReadFuncName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPLS_READFUNC_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFPLS_READFUNC, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLSREADFUNC_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			::SetParentChildNodeText( m_IDescriptions, nItemInd, sdt_PropScopeLocal, VE_PID_IFPLS_READFUNC_ID, pLabel );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PropertyLocalScopeWriteFuncName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPLS_WRITEFUNC_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFPLS_WRITEFUNC, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLSWRITEFUNC_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			::SetParentChildNodeText( m_IDescriptions, nItemInd, sdt_PropScopeLocal, VE_PID_IFPLS_WRITEFUNC_ID, pLabel );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PropertyValiableName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPSS_VARIABLENAME_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFPSS_VARIABLENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PROPVARIABLE_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PropertyValiableDefValue узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPSS_VARIABLEDEFVALUE_ID) ) {

			RefuseUsedObject( hData, VE_PID_IFPSS_VARIABLEDEFVALUE );

			::DATA_Set_Val( hData, NULL, VE_PID_IFPSS_VARIABLEDEFVALUE, AVP_dword(info->m_Value.m_pItemText), 0 );
			
			SetPropTypifiedDefValue( hData );

			AVP_dword dwTypeType = 0;
			AcceptUsedObject( info->m_Value.m_pItemText, dwTypeType );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::GetPropValueAsString( hProp, NULL );
			pLabel = ::CreateDisplayLabelSTR( IDS_PROPDEFVALUE_PREFIX, pLabel );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item)  && IS_NODE_DATASTRUCTURE(item) ) {
			if ( !hData ) 
			{
				CreateInterfaceDataStructureNodeContents( items, nItemInd );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else {
				::DATA_Set_Val( hData, NULL, VE_PID_IFD_DATASTRUCTURENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item)  && IS_NODE_DATASTRUCTUREMEMBER(item) ) {
			if ( !hData ) 
			{
				CreateInterfaceDataStructureMemberNodeContents( items, nItemInd );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else {
				::DATA_Set_Val( hData, NULL, VE_PID_IFDM_MEMBERNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактирован DataStructureMemberTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFDM_TYPENAME_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFDM_TYPENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_DSMTYPENAME_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован DataStructureMemberArrayBound узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFDM_ARRAYBOUND_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IFDM_ARRAYBOUND, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_DSMARRAYBOUND_PREFIX, info->m_Value.m_pItemText );
			m_IDescriptions.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован Array
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_ScrollListNewValue) ) {
			::SetPropArrayValue( hProp, -1, info->m_Value.m_pItemText, NULL );
			
			int nParentInd = m_IDescriptions.GetParentIndex( nItemInd );
			int nCount = ::PROP_Arr_Count( hProp );

			if ( nCount >= CONTENTS_SCROLL_SIZE ) 
				CPPD(items[nParentInd])->m_nScrollPos++;

			ReloadContentsList( items, nParentInd, hData, hProp );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован Array
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_ScrollListValue) ) {
			::SetPropArrayValue( hProp, CPPD(item)->m_nArrayIndex, info->m_Value.m_pItemText, NULL );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

	}


// -------------------------------------------------------------------------------------------
	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		int nItemInd         = info->m_nItemIndex;

		CCTItemsArray &items = *m_Interfaces.GetItemsArray();
		CCheckTreeItem *item = items[nItemInd];
		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = CPPD(item)->m_hProp;

		// Отредактирован PluginName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_PLUGINNAME_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_PL_PLUGINNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLNAME_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			int nParentIndex = m_Interfaces.GetParentIndex( nItemInd );
			int nIndex = m_Interfaces.FirstThat( ::FindForNodeID, (void *)VE_PID_PL_MNEMONICID_ID, nParentIndex );
			if ( nIndex >= 0 ) {
				// Заменить отображение узла
				CAPointer<char> pMnemonicIDLabel = ::CreatePluginMnemonicIDLabel( hData );
				::DATA_Set_Val( hData, NULL, VE_PID_PL_MNEMONICID, AVP_dword(LPCSTR(pMnemonicIDLabel)), 0 );
				pMnemonicIDLabel = ::CreateDisplayLabelSTR( IDS_MNEMONICID_PREFIX, pMnemonicIDLabel );
				m_Interfaces.SetTreeItemText( nIndex, LPCSTR(pMnemonicIDLabel) );
			}

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginMnemonicID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_MNEMONICID_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_PL_MNEMONICID, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_MNEMONICID_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginDigitalID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_DIGITALID_ID) ) {

			AVP_dword nID = 0;
			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_PL_DIGITALID ) ) 
				nID = ::GetPropNumericValueAsDWord( hValueProp );

			if ( m_pIIDServer )
				m_pIIDServer->ReleaseID( pic_Plugin, nID );

			nID = ::A2I( info->m_Value.m_pItemText );

			::DATA_Set_Val( hData, NULL, VE_PID_PL_DIGITALID, nID, 0 );

			if ( m_pIIDServer )
				m_pIIDServer->SetIDInUse( pic_Plugin, nID );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelINT( IDS_PLDIGITALID_PREFIX, nID );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginVersionID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_VERSIONID_ID) ) {

			AVP_dword nID = A2I( info->m_Value.m_pItemText );

			::DATA_Set_Val( hData, NULL, VE_PID_PL_VERSIONID, nID, 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelINT( IDS_PLVERSION_PREFIX, nID );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginCodePageID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_CODEPAGEID_ID) ) {

			CCodePages oCP;
			AVP_dword nID = oCP.GetCodePageByName(info->m_Value.m_pItemText);

			::DATA_Set_Val( hData, NULL, VE_PID_PL_CODEPAGEID, nID, 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLCODEPAGE_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginCodePageID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_DATEFORMATID_ID) ) {

			AVP_dword nID = A2I( info->m_Value.m_pItemText );

			::DATA_Set_Val( hData, NULL, VE_PID_PL_DATEFORMATID, nID, 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelINT( IDS_PLDATEFORMAT_PREFIX, nID );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginAuthorName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_AUTHORNAME_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_PL_AUTHORNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLAUTHORNAME_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginProjectName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_PROJECTNAME_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_PL_PROJECTNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLPROJECTNAME_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован PluginSubprojectName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_SUBPROJECTNAME_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_PL_SUBPROJECTNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_PLSUBPROJECTNAME_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактировано имя узла
		if ( IS_NODE_LABEL(item) && IS_NODE_INTERFACE(item)) {
			if ( !hData ) {

				CreateInterfaceNodeContents( items, nItemInd );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
			else {
				::DATA_Set_Val( hData, NULL, VE_PID_IF_INTERFACENAME, AVP_dword(info->m_Value.m_pItemText), 0 );

				CSArray<CRelatedIDs> rcRelatedInfo;
				CRelatedIDs rcRelatedID;

				rcRelatedID.m_dwStrID = IDS_MNEMONICID_PREFIX;
				rcRelatedID.m_bValue = TRUE;
				rcRelatedInfo.Add( rcRelatedID );

				rcRelatedID.m_dwStrID = IDS_OBJECTTYPE_PREFIX;
				rcRelatedID.m_bValue = TRUE;
				rcRelatedInfo.Add( rcRelatedID );
				
				rcRelatedID.m_dwStrID = IDS_INCLUDEFILE_PREFIX;
				rcRelatedID.m_bValue = TRUE;
				rcRelatedInfo.Add( rcRelatedID );

				if ( CRenameIntRelatedDlg(rcRelatedInfo, this).DoModal() == IDOK ) {
					if ( rcRelatedInfo[0].m_bValue ) {
						int nIndex = m_Interfaces.FirstThat( ::FindForNodeID, (void *)VE_PID_IF_MNEMONICID_ID, nItemInd );
						if ( nIndex >= 0 ) {
							// Заменить отображение узла
							CAPointer<char> pMnemonicIDLabel = ::CreateMnemonicIDLabel( hData );
							::DATA_Set_Val( hData, NULL, VE_PID_IF_MNEMONICID, AVP_dword(LPCSTR(pMnemonicIDLabel)), 0 );
							pMnemonicIDLabel = ::CreateDisplayLabelSTR( IDS_MNEMONICID_PREFIX, pMnemonicIDLabel );
							m_Interfaces.SetTreeItemText( nIndex, LPCSTR(pMnemonicIDLabel) );
						}
					}
					
					if ( rcRelatedInfo[1].m_bValue ) {
						int nIndex = m_Interfaces.FirstThat( ::FindForNodeID, (void *)VE_PID_IF_OBJECTTYPE_ID, nItemInd );
						if ( nIndex >= 0 ) {
							// Заменить отображение узла
							CAPointer<char> pObjectTypeLabel = ::CreateObjectTypeLabel( hData );
							::DATA_Set_Val( hData, NULL, VE_PID_IF_OBJECTTYPE, AVP_dword(LPCSTR(pObjectTypeLabel)), 0 );
							pObjectTypeLabel = ::CreateDisplayLabelSTR( IDS_OBJECTTYPE_PREFIX, pObjectTypeLabel );
							m_Interfaces.SetTreeItemText( nIndex, LPCSTR(pObjectTypeLabel) );
						}
					}
					
					if ( rcRelatedInfo[2].m_bValue ) {
						int nIndex = m_Interfaces.FirstThat( ::FindForNodeID, (void *)VE_PID_IF_INCLUDEFILE_ID, nItemInd );
						if ( nIndex >= 0 ) {
							// Заменить отображение узла
							CAPointer<char> pObjectTypeLabel = ::CreateIncludeFileLabel( hData );
							::DATA_Set_Val( hData, NULL, VE_PID_IF_INCLUDEFILE, AVP_dword(LPCSTR(pObjectTypeLabel)), 0 );
							pObjectTypeLabel = ::CreateDisplayLabelSTR( IDS_INCLUDEFILE_PREFIX, pObjectTypeLabel );
							m_Interfaces.SetTreeItemText( nIndex, LPCSTR(pObjectTypeLabel) );
						}
					}
				}
				
				::SetNodeUID( hData, true );
	
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				return 0;
			}
		}

		// Отредактирован MnemonicID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_MNEMONICID_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IF_MNEMONICID, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_MNEMONICID_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован DigitalID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_DIGITALID_ID) ) {

			AVP_dword nID = 0;
			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID ) ) 
				nID = ::GetPropNumericValueAsDWord( hValueProp );

			if ( m_pIIDServer )
				m_pIIDServer->ReleaseID( pic_Interface, nID );

			nID = ::A2I( info->m_Value.m_pItemText );

			AVP_dword nOldID = ::GetPropNumericValueAsDWord( hProp );
			CIObject *pObject = m_IOTable.Update( nOldID, nID );
			ExchangeTypesByInterfaceID( pObject );

			::DATA_Set_Val( hData, NULL, VE_PID_IF_DIGITALID, nID, 0 );

			if ( m_pIIDServer )
				m_pIIDServer->SetIDInUse( pic_Interface, nID );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelINT( IDS_DIGITALID_PREFIX, nID );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован RevisionID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_REVISIONID_ID) ) {

			AVP_dword nID = A2I( info->m_Value.m_pItemText );

			::DATA_Set_Val( hData, NULL, VE_PID_IF_REVISIONID, nID, 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelINT( IDS_REVISIONID_PREFIX, nID );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован CreationDate узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_CREATIONDATE_ID) ) {

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::GetPropValueAsString( hProp, NULL );
			pLabel = ::CreateDisplayLabelSTR( IDS_CREATIONDATE_PREFIX, pLabel );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован IncludeFile узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_INCLUDEFILE_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IF_INCLUDEFILE, AVP_dword(info->m_Value.m_pItemText), 0 );

			SetObjectIncludeName( hData, info->m_Value.m_pItemText, true );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_INCLUDEFILE_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован OutputFile узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_OUTPUTFILE_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IF_OUTPUTFILE, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_OUTPUTFILE_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован ObjectType узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_OBJECTTYPE_ID) ) {
			CAPointer<char> pOldName = ::GetPropValueAsString(hProp, NULL);
			CIObject *pObject = m_IOTable.Update( pOldName, info->m_Value.m_pItemText );
			ExchangeTypesByInterfaceObject( pObject );

			::DATA_Set_Val( hData, NULL, VE_PID_IF_OBJECTTYPE, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_OBJECTTYPE_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован ShortComment узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_SHORTCOMMENT_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_SHORTCOMMENT, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_SHORTCOMMENT_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован LargeComment узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_LARGECOMMENT_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_LARGECOMMENT, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, VE_PID_LARGECOMMENT );
			pLabel = ::CreateDisplayLabelSTR( IDS_LARGECOMMENT_PREFIX, pLabel );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован AutorName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_AUTORNAME_ID) ) {

			::DATA_Set_Val( hData, NULL, VE_PID_IF_AUTORNAME, AVP_dword(info->m_Value.m_pItemText), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::CreateDisplayLabelSTR( IDS_AUTORNAME_PREFIX, info->m_Value.m_pItemText );
			m_Interfaces.SetTreeItemText( nItemInd, LPCSTR(pLabel) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

	}
// -------------------------------------------------------------------------------------------
	return 0;
}

// ---
LRESULT CVisualEditorDlg::OnCTTBeginLabelEdit( WPARAM wParam, LPARAM lParam ) 
{
	CTTMessageInfo*		info;
	CVEControlTreeBase* pTreeCtrl;
	CCTItemsArray*		pCtrlItems;
	int					nItemIndex;
	CCheckTreeItem*		pItem;

	HDATA hData;
	HPROP hProp;

	GetNotificationData(lParam, info, pTreeCtrl, pCtrlItems, nItemIndex, pItem, hData, hProp);

	if(NULL == pTreeCtrl)
		return 0;

	int nArrayPosition = CPPD(pItem)->m_nArrayIndex;

	PrepareDisplayString(info, hData, hProp, nArrayPosition, FALSE);

	*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
	return 0;
}


// ---
// validation of the user input
LRESULT CVisualEditorDlg::OnCTTEndLabelEdit( WPARAM wParam, LPARAM lParam ) 
{
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	int nItemInd         = info->m_nItemIndex;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) 
	{
		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
		HDATA hData = CPPD(items[nItemInd])->m_hData;
		HPROP hProp = CPPD(items[nItemInd])->m_hProp;

		// Отредактирован InterfaceSubTypeID узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_SUBTYPEID_ID) ) 
		{
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, true) ) 
			{
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован TypeName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Type) ||
			 ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFT_TYPENAME_ID)  ) 
		{
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) 
			{
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}

			if ( hProp ) 
			{
				CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
				if ( !CheckTypeNameIsUsed(pValue) ) 
				{
					*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
					return 0;
				}
			}

			*(reinterpret_cast<BOOL *>(wParam)) = 
				CheckInterfaceTypeName( hData, nItemInd, info->m_Value.m_pItemText );

			return 0;
		}

		// Отредактирован ConstantName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Constant) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFC_CONSTANTNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceConstantName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован ErrorCodeName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_ErrorCode) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFE_ERRORCODENAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceErrorCodeName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован MethodName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Method) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_METHODNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceMethodName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован ParameterName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_MethodParam) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFMP_PARAMETERNAME_ID)  ) {
			if ( !::IsEllipsis(info->m_Value.m_pItemText) && 
				   !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceMethodParamName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован PropertyName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Property) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_PROPNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfacePropertyName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактировано имя класса сообщений
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_MessageClass) ||
			::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGECLASSNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceMessageClassName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}
		
		// Отредактировано имя сообщения
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Message) ||
			::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_MESSAGENAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceMessageName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован PropertyTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_TYPENAME_ID) ) {
			// !!!Petrovitch -- AVP_dword nNewType = ::GTypeName2TypeID(info->m_Value.m_pItemText);
			AVP_dword nNewType = ::PropTypeName2PropTypeID( info->m_Value.m_pItemText );
			*(reinterpret_cast<BOOL *>(wParam)) = CheckPropType( hData, nItemInd, nNewType );
			return 0;
		}

		// Отредактирован PropertyDigitalID узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_DIGITALID_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfacePropertyID( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Редактировался PropertySharedScopeValiableDefValue
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPSS_VARIABLEDEFVALUE_ID)  ) {
			/*
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*/
			*(reinterpret_cast<BOOL *>(wParam)) = CheckPropertyDefValue( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован StructureName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_DataStructure) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFD_DATASTRUCTURENAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован StructureMemberName узла
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_DataStructureMember) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFDM_MEMBERNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован StructureMemberTypeName узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFDM_TYPENAME_ID)  ) {
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован StructureMemberArrayBound узла
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFDM_ARRAYBOUND_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		return 0;
	}

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {
		CCTItemsArray &items = *m_Interfaces.GetItemsArray();
		HDATA hData = CPPD(items[nItemInd])->m_hData;
		HPROP hProp = CPPD(items[nItemInd])->m_hProp;

		// Отредактирован PluginName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_PLUGINNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактирован PluginMnemonicID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_MNEMONICID_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактирован PluginDigitalID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_DIGITALID_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, true) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckPluginID( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}
		
		// Отредактирован PluginVersionID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_VERSIONID_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, true) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактирован PluginCodePageID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_CODEPAGEID_ID)  ) {
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактирован PluginDateFormatID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_DATEFORMATID_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, true) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактирован PluginAuthorName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_AUTHORNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактирован PluginProjectName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_PROJECTNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Отредактирован PluginSubprojectName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_PL_SUBPROJECTNAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Отредактирован InterfaceName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_INTERFACENAME_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, false) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован DigitalID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_DIGITALID_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, true) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceID( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован RevisionID узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_REVISIONID_ID)  ) {
			if ( !::CheckEditedValue(this, info->m_Value.m_pItemText, true) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceRevisionID( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		// Отредактирован ObjectName узла
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_OBJECTTYPE_ID)  ) {
			CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
			if ( !CheckTypeNameIsUsed(pValue) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}
			*(reinterpret_cast<BOOL *>(wParam)) = CheckInterfaceObjectTypeName( hData, nItemInd, info->m_Value.m_pItemText );
			return 0;
		}

		return 0;
	}

	return 0;
}

// ---
LRESULT CVisualEditorDlg::OnCTTCancelLabelEdit( WPARAM wParam, LPARAM lParam ) 
{
	CTTMessageInfo*		info;
	CVEControlTreeBase* pTreeCtrl;
	CCTItemsArray*		pCtrlItems;
	int					nItemIndex;
	CCheckTreeItem*		pItem;

	HDATA hData;
	HPROP hProp;

	GetNotificationData(lParam, info, pTreeCtrl, pCtrlItems, nItemIndex, pItem, hData, hProp);

	if(NULL == pTreeCtrl)
		return 0;

	PrepareDisplayString(info, hData, hProp, CPPD(pItem)->m_nArrayIndex, TRUE);

	*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
	return 0;
}


// ---
LRESULT CVisualEditorDlg::OnCTTLoadComboContents( WPARAM wParam, LPARAM lParam ) {

	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		int nItemInd   = info->m_nItemIndex;

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
		CCheckTreeItem *item = items[nItemInd];
		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = CPPD(item)->m_hProp;

		CStringList *pContList = (CStringList *)info->m_Value.m_pStringList;

		CString rcString;

		int i,c;
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_RESULTTYPENAME_ID) ) {
			for ( i=0,c=gTypes.Count(); i<c; i++ )
				pContList->AddTail( gTypes[i]->m_pName );

			for ( i=0,c=m_IOTable.Count(); i<c; i++ )
				pContList->AddTail( m_IOTable[i]->m_rcObjectName );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Property) ||
			   ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_PROPNAME_ID) ) {
			for ( i=0,c=gSoftProps.Count(); i<c; i++ )
				pContList->AddTail( gSoftProps[i]->m_pName );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_TYPENAME_ID) ) {
			for ( i=0,c=gPropTypes.Count(); i<c; i++ )
				pContList->AddTail( gPropTypes[i]->m_pName );

			for ( i=0,c=m_IOTable.Count(); i<c; i++ )
				if ( !m_IOTable[i]->m_bInterface )
					pContList->AddTail( m_IOTable[i]->m_rcObjectName );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFMP_TYPENAME_ID) ) {
			for ( i=0,c=gTypes.Count(); i<c; i++ )
				pContList->AddTail( gTypes[i]->m_pName );

			for ( i=0,c=m_IOTable.Count(); i<c; i++ ) 
				pContList->AddTail( m_IOTable[i]->m_rcObjectName );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFT_BASETYPENAME_ID) ) {
			HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFT_TYPENAME );
			CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
			for ( i=0,c=gTypes.Count(); i<c; i++ )
				pContList->AddTail( gTypes[i]->m_pName );

			for ( i=0,c=m_IOTable.Count(); i<c; i++ ) {
				if ( lstrcmp( pValue, m_IOTable[i]->m_rcObjectName) )
					pContList->AddTail( m_IOTable[i]->m_rcObjectName );
			}

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFC_BASETYPENAME_ID) ) {
			for ( i=0,c=gTypes.Count(); i<c; i++ )
				pContList->AddTail( gTypes[i]->m_pName );

			for ( i=0,c=m_IOTable.Count(); i<c; i++ ) 
				pContList->AddTail( m_IOTable[i]->m_rcObjectName );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPSS_VARIABLEDEFVALUE_ID) ) {
			HPROP hTypeProp;
			if ( hTypeProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IFP_TYPE) ) {
				AVP_dword dwPragueType = ::GetPropNumericValueAsDWord(hTypeProp);
				::CollectConstantNames( m_hTree, *pContList, dwPragueType );
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
	}

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		int nItemInd   = info->m_nItemIndex;
		
		CCTItemsArray &items = *m_Interfaces.GetItemsArray();
		CCheckTreeItem *item = items[nItemInd];
		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = CPPD(item)->m_hProp;
		
		CStringList *pContList = (CStringList *)info->m_Value.m_pStringList;
		
		// plugin code page
		if ( ::IsIDProlog(m_Interfaces, nItemInd,  VE_PID_PL_CODEPAGEID_ID) ){
			
			::CollectCodePages( *pContList );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
	}
	return 0;
}


// Сообщение об намерении установить Checked
// WPARAM		- LPBOOL - указатель на результат - не используется
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
// ---
LRESULT CVisualEditorDlg::OnCTTCheckedChanging( WPARAM wParam, LPARAM lParam ) {

	if(m_bTechnicalEditting){
		// changing forbidden
		*((BOOL*)wParam) = FALSE;
		return 0;
	}

	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
		
		int nItemInd         = info->m_nItemIndex;
 		CCheckTreeItem *item = items[nItemInd];
		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = CPPD(item)->m_hProp;

		// Check SysMethodName
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_SELECTED_ID) ) {
			AVP_bool dwSelected = info->m_Value.m_bSetRemove;
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFM_SELECTED );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, dwSelected, 0 );
		}
							   
		// Check PropertyScopeName
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFP_SCOPE_ID) ) {
			AVP_dword dwValue = VE_IFP_LOCAL;

			int nType = CPPD(item)->m_nSpecialType;
			nType &= ~sdt_Label;
			switch ( nType ) {
				case sdt_PropScopeLocal :
					dwValue = VE_IFP_LOCAL;
					break;
				case sdt_PropScopeShared :
					dwValue = VE_IFP_SHARED;
					break;
			}

			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFP_SCOPE ) ) 
				::SetPropNumericValue( hValueProp, dwValue );
			else 
				::DATA_Add_Prop( hData, NULL, VE_PID_IFP_SCOPE, dwValue, 0 );
		}

		// Check LocalScopeWritableDuringInit
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPLS_WRITABLEDURINGINIT_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPLS_WRITABLEDURINGINIT );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFPLS_WRITABLEDURINGINIT, AVP_dword(info->m_Value.m_bSetRemove), 0 );

			SetPropertyDisplayString( hData );
		}

		// Check LocalScopeRequired
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPLS_REQUIRED_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPLS_REQUIRED );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFPLS_REQUIRED, AVP_dword(info->m_Value.m_bSetRemove), 0 );

//			::SetParentChildNodeChecked( m_IDescriptions, nItemInd, sdt_PropScopeLocal, VE_PID_IFPLS_REQUIRED_ID, info->m_Value.m_bSetRemove );
			SetPropertyDisplayString( hData );
		}

		// Check LocalScopeWrite
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPLS_MEMBER_MODE_ID) ) {
			AVP_dword dwValue = VE_IFPLS_MEMBER_READ;

			int nType = CPPD(item)->m_nSpecialType & ~sdt_Label;
			switch ( nType ) {
				case sdt_PropLocalMemberRead :
					dwValue = VE_IFPLS_MEMBER_READ;
					break;
				case sdt_PropLocalMemberWrite :
					dwValue = VE_IFPLS_MEMBER_WRITE;
					break;
				case sdt_PropLocalMemberReadWrite :
					dwValue = VE_IFPLS_MEMBER_READWRITE;
					break;
			}

			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPLS_MEMBER_MODE ) ) 
				::SetPropNumericValue( hValueProp, dwValue );
			else 
				::DATA_Add_Prop( hData, NULL, VE_PID_IFPLS_MEMBER_MODE, dwValue, 0 );
		}
		// Check PropertyLocalMode
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPLS_MODE_ID) ) 
		{
			AVP_dword dwValue = VE_IFPLS_NONE;

			int nType = CPPD(item)->m_nSpecialType;
			nType &= ~ sdt_Label;
			switch ( nType ) {
				case sdt_PropLocalNone :
					dwValue = VE_IFPLS_NONE;
					break;
				case sdt_PropLocalRead :
					dwValue = VE_IFPLS_READ;
					break;
				case sdt_PropLocalWrite :
					dwValue = VE_IFPLS_WRITE;
					break;
				case sdt_PropLocalReadWrite :
					dwValue = VE_IFPLS_READWRITE;
					break;
			}

			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPLS_MODE ) ) 
				::SetPropNumericValue( hValueProp, dwValue );
			else 
				::DATA_Add_Prop( hData, NULL, VE_PID_IFPLS_MODE, dwValue, 0 );
		}
/*
		// Check SharedScopePointer
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPSS_POINTER_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPSS_POINTER );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFPSS_POINTER, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		}
*/
		// Check StructureMemberIsArray
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFDM_ISARRAY_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFDM_ISARRAY );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFDM_ISARRAY, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		}

		// Check MethodParamIsPointer
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFMP_ISPOINTER_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_ISPOINTER );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFMP_ISPOINTER, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			
			AVP_bool bChecked = info->m_Value.m_bSetRemove;
			if ( !bChecked ) {
        HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_TYPE );
        AVP_dword value = ::GetPropNumericValueAsDWord( hCheckedProp );
        bChecked = TYPE_IS_POINTER(value);
        if ( !bChecked ) {
          hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_ISCONST );
					if ( hCheckedProp )
						::PROP_Set_Val( hCheckedProp, AVP_dword(bChecked), 0 );
					else
						::DATA_Add_Prop( hData, NULL, VE_PID_IFMP_ISCONST, AVP_dword(bChecked), 0 );
        }
      }
			
			SetMethodDisplayString( ::DATA_Get_Dad(hData, NULL) );
		}
		
		
		// Check MethodParamIsPointer
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFMP_IS_DOUBLE_POINTER_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_IS_DOUBLE_POINTER );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFMP_IS_DOUBLE_POINTER, AVP_dword(info->m_Value.m_bSetRemove), 0 );

			AVP_bool bChecked = info->m_Value.m_bSetRemove;
			if ( !bChecked ) {
        HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_TYPE );
        AVP_dword value = ::GetPropNumericValueAsDWord( hCheckedProp );
        bChecked = TYPE_IS_POINTER(value);
        if ( !bChecked ) {
          hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_ISCONST );
				  if ( hCheckedProp )
					  ::PROP_Set_Val( hCheckedProp, AVP_dword(bChecked), 0 );
				  else
					  ::DATA_Add_Prop( hData, NULL, VE_PID_IFMP_ISCONST, AVP_dword(bChecked), 0 );
        }
      }

			SetMethodDisplayString( ::DATA_Get_Dad(hData, NULL) );
		}


    // Check MethodParamIsConst
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFMP_ISCONST_ID) ) {
			AVP_bool bPointer = FALSE;
			HPROP hPointerProp;
			if ( hPointerProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_ISPOINTER ) ) 
				bPointer = ::GetPropNumericValueAsDWord( hPointerProp );
      if ( !bPointer ) {
        hPointerProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_TYPE ); 
        if ( hPointerProp ) {
          AVP_dword value = ::GetPropNumericValueAsDWord( hPointerProp );
          bPointer = TYPE_IS_POINTER(value);
        }
      }

			AVP_bool bChecked = bPointer ? info->m_Value.m_bSetRemove : FALSE;

			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_ISCONST );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(bChecked), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFMP_ISCONST, AVP_dword(bChecked), 0 );

			if ( !bPointer )
				m_IDescriptions.SetItemChecked( nItemInd, CCheckTreeItem::ie_Unchecked, false ); 

			SetMethodDisplayString( ::DATA_Get_Dad(hData, NULL) );
		}


		// Check Static
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_STATIC_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_STATIC );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IF_STATIC, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		}

    // Check System
    if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_SYSTEM_ID) ) {
      HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_SYSTEM );
      if ( hCheckedProp )
        ::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
      else
        ::DATA_Add_Prop( hData, NULL, VE_PID_IF_SYSTEM, AVP_dword(info->m_Value.m_bSetRemove), 0 );
    }
    
    // Check TransferPropUP
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_TRANSFERPROPUP_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_TRANSFERPROPUP );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IF_TRANSFERPROPUP, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		}

		// Check NonUpdatable
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_NONSWAPABLE_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_NONSWAPABLE );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IF_NONSWAPABLE, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		}

    // Check ProtectedBy, ProtectedByCS, ProtectedByMutex
    if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_PROTECTED_BY_ID) ) {
      BOOL sync;
      BOOL by_cs;

      switch( CPPD(item)->m_nArrayIndex ) {
        case 0:
          sync = !!info->m_Value.m_bSetRemove;
          by_cs = items[nItemInd+1]->IsChecked();
          break;
        case 1:
          sync = items[nItemInd-1]->IsChecked();
          by_cs = !!info->m_Value.m_bSetRemove;
          break;
        case 2:
          sync = items[nItemInd-2]->IsChecked();
          by_cs = !info->m_Value.m_bSetRemove;
          break;
        default:
          sync = false;
          break;
      }
      AVP_dword val;
      if ( !sync ) 
        val = VE_IFF_PROTECTED_BY_NONE;
      else if ( by_cs )
        val = VE_IFF_PROTECTED_BY_CS;
      else
        val = VE_IFF_PROTECTED_BY_MUTEX;
      
      HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_PROTECTED_BY );
      if ( hCheckedProp )
        ::PROP_Set_Val( hCheckedProp, val, 0 );
      else
        ::DATA_Add_Prop( hData, NULL, VE_PID_IF_PROTECTED_BY, val, 0 );
    }
    
    // Check LastCallProp
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IF_LASTCALLPROP_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_LASTCALLPROP );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IF_LASTCALLPROP, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		}
		
		// Check ErrorIsWarning
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFE_ISWARNING_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFE_ISWARNING );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_IFE_ISWARNING, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			SetErrorCodeDisplayString( hData );
		}

		// Check Plugin AutoStart
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_PL_AUTOSTART_ID) ) {
			HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_PL_AUTOSTART );
			if ( hCheckedProp )
				::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
			else
				::DATA_Add_Prop( hData, NULL, VE_PID_PL_AUTOSTART, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		}
	}

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		CCTItemsArray &items = *m_Interfaces.GetItemsArray();
		
		int nItemInd         = info->m_nItemIndex;
 		CCheckTreeItem *item = items[nItemInd];
		HDATA hData = CPPD(items[nItemInd])->m_hData;
		HPROP hProp = CPPD(items[nItemInd])->m_hProp;
	}
	return 0;
}


// ---
void CVisualEditorDlg::CheckAndRemoveLinkedInterfaces( HDATA hData, int nItemInd ) {
	int iSelected = m_Interfaces.GetSelectedIndex();

	HPROP hFileProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_IMPORTFILENAME );
	if ( hFileProp ) {
		CAPointer<char> pFileName = ::GetPropValueAsString( hFileProp, NULL );
		CString rcString;
		rcString.Format( IDS_ERR_REMOVELINKED, pFileName );
		if ( ::DlgMessageBox(this, rcString, MB_YESNO) == IDYES ) {
			SetModified( true );

			FlushDataTree( m_hTree );
			/*
			::FullyDeleteItem( m_IDescriptions, -1 );
			m_hCCTree = NULL;
			::FullyDeleteItem( m_Interfaces, nItemInd );
			*/
			HDATA hCurrData = ::DATA_Get_First( m_hTree, NULL );
			while ( hCurrData ) {
				HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
				HPROP hFileProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_IMPORTFILENAME );
				if ( hFileProp ) {
					CAPointer<char> pSiblingFileName = ::GetPropValueAsString( hFileProp, NULL );
					if ( !lstrcmpi(pSiblingFileName, pFileName) ) 
						::DATA_Remove( hCurrData, NULL );
				}
				hCurrData = hNextData;
			}
		}

		InitNewTree( m_hTree );
		m_Interfaces.SetFocus();
		m_Interfaces.SelectNode( iSelected );
	}
	else {
		CNodeUID rcInterfaceUID;
		if ( ::GetNodeUID(hData, rcInterfaceUID) ) {
			FlushDataTree( m_hTree );

			::DATA_Remove( hData, NULL );

			HDATA hCurrData = ::DATA_Get_First( m_hTree, NULL );
			while ( hCurrData ) {
				if ( !::IsLinkedToFile(hCurrData) ) {
					HPROP hValueProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_LINKEDUNIQUEID );
					if ( hValueProp ) {
						CNodeUID rcUpdateUID;
						AVP_dword dwCount = ::PROP_Arr_Count( hValueProp );
						for ( AVP_dword i=0; i<dwCount; i++ ) {
							::PROP_Arr_Get_Items( hValueProp, i, &rcUpdateUID, sizeof(rcUpdateUID) );
							if ( ::CompareNodesUID(rcInterfaceUID, rcUpdateUID) ) {
								SetModified( true );
								::PROP_Arr_Remove( hValueProp, i, 1 );
								break;
							}
						}
					}
				}
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}

			InitNewTree( m_hTree );
			m_Interfaces.SetFocus();
			m_Interfaces.SelectNode( iSelected );
		}
	}
}


// ---
LRESULT CVisualEditorDlg::OnCTTRemoveItem( WPARAM wParam, LPARAM lParam ) {

	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
		
		int nItemInd         = info->m_nItemIndex;
 		CCheckTreeItem *item = items[nItemInd];
		HDATA hData					 = CPPD(item)->m_hData;
		HPROP hProp					 = CPPD(item)->m_hProp;

		// Удаляется sdt_Type
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Type) ) {

			CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
			BOOL bTypeIsUsed;
			if ( !CheckTypeNameIsUsedForDelete(pValue, &bTypeIsUsed) ) {
				*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
				return 0;
			}

			RefuseUsedObject( hData, VE_PID_IFT_BASETYPENAME );
			::FullyDeleteItem( m_IDescriptions, nItemInd );

			if(bTypeIsUsed)
			{
				CChooseNameFromListDlg oDlg;
				oDlg.m_strTitle.LoadString(IDS_TITLE_CHOOSE_TYPE_DLG);
				FillTypeNamesList(oDlg.m_oNamesList, pValue);
				if(IDOK == oDlg.DoModal())
					ExchangeTypesByTypeName(pValue, oDlg.m_strNameChoosen);
			}

			ReloadIncludesTree();
			
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Удаляется sdt_Constant
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Constant) ) {

			RefuseUsedObject( hData, VE_PID_IFC_BASETYPENAME );
			::FullyDeleteItem( m_IDescriptions, nItemInd );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Удаляется sdt_MessageClass
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_MessageClass) ) {
			
			::FullyDeleteItem( m_IDescriptions, nItemInd );
			
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Удаляется sdt_Message
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Message) ) {
			
			::FullyDeleteItem( m_IDescriptions, nItemInd );
			
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Удаляется sdt_ErrorCode
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_ErrorCode) ) {

			::FullyDeleteItem( m_IDescriptions, nItemInd );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Удаляется sdt_Property
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Property) ) {

			RefuseUsedObject( hData, VE_PID_IFP_TYPENAME );
			::FullyDeleteItem( m_IDescriptions, nItemInd );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Удаляется sdt_Method
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_Method) ) {

			RefuseUsedObject( hData, VE_PID_IFMP_TYPENAME );
			::FullyDeleteItem( m_IDescriptions, nItemInd );

			ReloadIncludesTree();

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Удаляется sdt_MethodParam
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_MethodParam) ) {
			HDATA hMethodData = ::DATA_Get_Dad( hData, NULL );

			RefuseUsedObject( hData, VE_PID_IFMP_TYPENAME );
			::FullyDeleteItem( m_IDescriptions, nItemInd );

			ReloadIncludesTree();

			SetMethodDisplayString( hMethodData );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

		// Удаляется sdt_DataStructure
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_DataStructure) ) {

			::FullyDeleteItem( m_IDescriptions, nItemInd );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		
		// Удаляется sdt_DataStructure
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_DataStructureMember) ) {

			::FullyDeleteItem( m_IDescriptions, nItemInd );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
		

		// Удаляется элемент Contents
		if ( ::IsTypeProlog(m_IDescriptions, nItemInd, sdt_ScrollListValue) ) {
			::PROP_Arr_Remove( hProp, CPPD(item)->m_nArrayIndex, 1 );
			
			int nParentInd = m_IDescriptions.GetParentIndex( nItemInd );

			if ( CPPD(items[nParentInd])->m_nScrollPos ) 
				CPPD(items[nParentInd])->m_nScrollPos--;

			ReloadContentsList( items, nParentInd, CPPD(item)->m_hData, hProp );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}

	}

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		CCTItemsArray &items = *m_Interfaces.GetItemsArray();
		
		int nItemInd         = info->m_nItemIndex;
 		CCheckTreeItem *item = items[nItemInd];
		HDATA hData					 = CPPD(item)->m_hData;

		// Удаляется sdt_Interface
		if ( ::IsTypeProlog(m_Interfaces, nItemInd, sdt_Interface) ) {
			if ( IsLinkedToFile(hData) ) {
				CheckAndRemoveLinkedInterfaces( hData, nItemInd );
			}
			else {
				HPROP hValueProp;
				if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_OBJECTTYPE) ) {
					CAPointer<char> pValue = ::GetPropValueAsString( hValueProp, NULL );
					if ( !CheckTypeNameIsUsedForDelete(pValue) ) {
						*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
						return 0;
					}
				}

				AVP_dword nIDId = 0;
				if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID ) ) {
					nIDId = ::GetPropNumericValueAsDWord( hValueProp );
				}

				if ( m_pIIDServer )
					m_pIIDServer->ReleaseID( pic_Interface, nIDId );

				::FullyDeleteItem( m_IDescriptions, -1 );

				m_hCCTree = NULL;

				::FullyDeleteItem( m_Interfaces, nItemInd );
			}
			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
	}

	return 0;
}


// ---
void CVisualEditorDlg::ReloadInterfaceCCTree( HDATA hRootData ) {
	// Удалить данные в элементах поддерева
	m_IDescriptions.ForEach( ::DeleteNodesData, NULL, -1 );
	// Очистить дерево
	m_IDescriptions.DeleteItemWithSubtree( -1 );

	m_hCCTree = hRootData;

	if ( m_hCCTree ) 
	{
		// Найти Stop-property в папе
		HPROP hProp = ::DATA_Find_Prop( m_hCCTree, NULL, VE_PID_IF_LASTINTERFACESLEVEL );
		if ( hProp ) 
			LoadInterfaceDescriptionTreeNodes( m_hCCTree );
		else 
			CreateNewDescriptionTreeStuff();
	}
}

// ---
void CVisualEditorDlg::ReloadPluginCCTree( HDATA hRootData ) {
	// Удалить данные в элементах поддерева
	m_IDescriptions.ForEach( ::DeleteNodesData, NULL, -1 );
	// Очистить дерево
	m_IDescriptions.DeleteItemWithSubtree( -1 );

	m_hCCTree = hRootData;

	if ( m_hCCTree ) {
		// Найти Stop-property в папе
		HPROP hProp = ::DATA_Find_Prop( m_hCCTree, NULL, VE_PID_IF_LASTINTERFACESLEVEL );
		if ( hProp ) {
			LoadPluginDescriptionTreeNodes( m_hCCTree );
		}
	}
}

// ---
LRESULT CVisualEditorDlg::OnCTTItemSelChanging( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	CWaitCursor wait;

	m_StatusBar.SetPaneText( 0, _T(""), TRUE );

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		if ( info->m_Value.m_bSetRemove ) {
			CCTItemsArray *items = m_Interfaces.GetItemsArray();

			if ( items && items->GetSize() ) {
				int nItemInd         = info->m_nItemIndex;
 				CCheckTreeItem *item = (*items)[nItemInd];

				if ( !IS_EMPTY_DATA(item) ) {
					HDATA hData = CPPD(item)->m_hData;

					HPROP hValueProp;
					if ( hData && (hValueProp = ::DATA_Find_Prop(hData, NULL, VE_PID_SHORTCOMMENT)) ) {
						CAPointer<char> pLabel = ::GetPropValueAsString( hValueProp, NULL );
						m_StatusBar.SetPaneText( 0, pLabel, TRUE );
					}
					
					if ( //CPPD(item)->m_nSpecialType != sdt_FullFilledItem &&
							//CPPD(item)->m_nSpecialType != sdt_FullFilledRootItem &&
							m_hCCTree != hData )	{

						DWORD dwType = GET_NODE_TYPE(item);
						int   nParentInd = nItemInd;
						while ( !dwType && nParentInd >= 0 ) {
							nParentInd = m_Interfaces.GetParentIndex( nParentInd );
							if ( nParentInd >= 0 )
								dwType = GET_NODE_TYPE((*items)[nParentInd]);
						}
						switch ( dwType ) {
							case sdt_Interface :
								ReloadInterfaceCCTree( hData );
								break;
							case sdt_Plugin :
								ReloadPluginCCTree( hData );
								break;
						}

						return 0;
					}
				}
			}
		}
		return 0;
	}
	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {
		if ( info->m_Value.m_bSetRemove ) {
			CCTItemsArray *items = m_IDescriptions.GetItemsArray();
			if ( items && items->GetSize() ) {
				int nItemInd         = info->m_nItemIndex;
				CCheckTreeItem *item = (*items)[nItemInd];
				
				if ( !IS_EMPTY_DATA(item) ) {
					HDATA hData = CPPD(item)->m_hData;
					
					HPROP hValueProp;
					if ( hData && (hValueProp = ::DATA_Find_Prop(hData, NULL, VE_PID_SHORTCOMMENT)) ) {
						CAPointer<char> pLabel = ::GetPropValueAsString( hValueProp, NULL );
						m_StatusBar.SetPaneText( 0, pLabel, TRUE );
					}
				}
			}
		}
		return 0;
	}
	
	return 0;
}



// ---
LRESULT CVisualEditorDlg::OnCTTStateChanged( WPARAM wParam, LPARAM lParam ) {
	SetModified( true );
	EnableEditableStuff();
	return 0;
}


// ---
static void AnalizeDate( AVP_date &nValue, SYSTEMTIME *pSysTime ) {
	AVP_date nEmptyValue;
	::ZeroMemory( &nEmptyValue, sizeof(nEmptyValue) );

	if ( !memcmp(&nValue, &nEmptyValue, sizeof(nValue)) ) {
		SYSTEMTIME rcSysTime;
		CTime::GetCurrentTime().GetAsSystemTime( rcSysTime );
		::CopyMemory( pSysTime, &rcSysTime, sizeof(rcSysTime) );
	}
	else
		::CopyMemory( pSysTime, &nValue, sizeof(nValue) );
}

// ---
LRESULT CVisualEditorDlg::OnCTTQueryDate( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		int nItemInd         = info->m_nItemIndex;

		CCTItemsArray &items = *m_Interfaces.GetItemsArray();
 		CCheckTreeItem *item = items[nItemInd];
		HPROP hProp					 = CPPD(item)->m_hProp;

		// Вход в редактирование CreationDate
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_CREATIONDATE_ID) ) {
			AVP_date nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );

			SYSTEMTIME rcSysTime;
			::AnalizeDate( nValue, &rcSysTime );
			::CopyMemory( info->m_Value.m_aDateValue, &rcSysTime, sizeof(info->m_Value.m_aDateValue) );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
	}

	return 0;
}


// ---
LRESULT CVisualEditorDlg::OnCTTQueryDateFormat( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	info->m_Value.m_pDateFormat = new TCHAR[_MAX_PATH];
	::GetDateEditFormat( info->m_Value.m_pDateFormat, _MAX_PATH );

	*(reinterpret_cast<BOOL *>(wParam)) = TRUE;

	return 0;
}

// ---
LRESULT CVisualEditorDlg::OnCTTEndDateLabelEdit( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {

		int nItemInd         = info->m_nItemIndex;

		CCTItemsArray &items = *m_Interfaces.GetItemsArray();
 		CCheckTreeItem *item = items[nItemInd];
		HPROP hProp					 = CPPD(item)->m_hProp;

		SYSTEMTIME rcSysTime;
		::CopyMemory( &rcSysTime, info->m_Value.m_aDateValue, sizeof(info->m_Value.m_aDateValue) );

		// Редактировался CreationDate
		if ( ::IsIDProlog(m_Interfaces, nItemInd, VE_PID_IF_CREATIONDATE_ID) ) {
			::SetPropDateValue( hProp, &rcSysTime );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			return 0;
		}
	}

	return 0;
}



// ---
LRESULT CVisualEditorDlg::OnCTTQueryTime( WPARAM wParam, LPARAM lParam ) {
	return 0;
}


// ---
LRESULT CVisualEditorDlg::OnCTTQueryTimeFormat( WPARAM wParam, LPARAM lParam ) {
	return 0;
}

// ---
LRESULT CVisualEditorDlg::OnCTTEndTimeLabelEdit( WPARAM wParam, LPARAM lParam ) {
	return 0;
}



// ---
static void AnalizeDateTime( AVP_datetime &nValue, SYSTEMTIME *pSysTime ) {
	AVP_datetime nEmptyValue;
	::ZeroMemory( &nEmptyValue, sizeof(nEmptyValue) );

	if ( !memcmp(&nValue, &nEmptyValue, sizeof(nValue)) ) {
		SYSTEMTIME rcSysTime;
		CTime::GetCurrentTime().GetAsSystemTime( rcSysTime );
		::CopyMemory( pSysTime, &rcSysTime, sizeof(rcSysTime) );
	}
	else
		::CopyMemory( pSysTime, &nValue, sizeof(nValue) );
}

// ---
LRESULT CVisualEditorDlg::OnCTTQueryDateTime( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		int nItemInd         = info->m_nItemIndex;

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
 		CCheckTreeItem *item = items[nItemInd];
		HPROP hProp					 = CPPD(item)->m_hProp;
		HDATA hData					 = CPPD(item)->m_hData;

		// Вход в редактирование VariableDefValue
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPSS_VARIABLEDEFVALUE_ID) ) {
			AVP_dword dwDTType = ::GetPropDTTypeByPragueType( hData );
			HPROP hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(dwDTType) );
			if ( hValueProp ) {
				AVP_datetime nValue;
				::PROP_Get_Val( hValueProp, &nValue, sizeof(nValue) );

				SYSTEMTIME rcSysTime;
				::AnalizeDateTime( nValue, &rcSysTime );
				::CopyMemory( info->m_Value.m_aDateTimeValue, &rcSysTime, sizeof(info->m_Value.m_aDateTimeValue) );

				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			}
			return 0;
		}
	}

	return 0;
}


// ---
LRESULT CVisualEditorDlg::OnCTTQueryDateTimeFormat( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	info->m_Value.m_pDateTimeFormat = new TCHAR[_MAX_PATH];
	::GetDateTimeEditFormat( info->m_Value.m_pDateTimeFormat, _MAX_PATH );

	*(reinterpret_cast<BOOL *>(wParam)) = TRUE;

	return 0;
}

// ---
LRESULT CVisualEditorDlg::OnCTTEndDateTimeLabelEdit( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		int nItemInd         = info->m_nItemIndex;

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
 		CCheckTreeItem *item = items[nItemInd];
		HPROP hProp					 = CPPD(item)->m_hProp;
		HDATA hData					 = CPPD(item)->m_hData;

		SYSTEMTIME rcSysTime;
		::CopyMemory( &rcSysTime, info->m_Value.m_aDateTimeValue, sizeof(info->m_Value.m_aDateTimeValue) );

		// Вход в редактирование VariableDefValue
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPSS_VARIABLEDEFVALUE_ID) ) {
			AVP_dword dwDTType = ::GetPropDTTypeByPragueType( hData );
			HPROP hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(dwDTType) );
			if ( hValueProp ) {
				::SetPropDateValue( hValueProp, &rcSysTime );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			}
			return 0;
		}
	}

	return 0;
}



// Запрос на обработку меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если обработка выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_hMenu - Handle Context-меню
// LRESULT	- не используется
// UINT const CTTM_PREPROCESSCONTEXTMENU
// ---
LRESULT CVisualEditorDlg::OnCTTPreptocessContextMenu( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( ::GetMenuItemCount(info->m_Value.m_hMenu) )
		::AppendMenu( info->m_Value.m_hMenu, MF_SEPARATOR, 0, 0 );

	CString rcMenuItemName;
//	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {
		rcMenuItemName.LoadString( IDS_MENU_EDIT_CUT );
		::AppendMenu( info->m_Value.m_hMenu, MF_STRING | (!IsEditCutEnable() ? MF_GRAYED : 0), ID_EDIT_CUT, rcMenuItemName );
//	}

	rcMenuItemName.LoadString( IDS_MENU_EDIT_COPY );
	::AppendMenu( info->m_Value.m_hMenu, MF_STRING | (!IsEditCopyEnable() ? MF_GRAYED : 0), ID_EDIT_COPY, rcMenuItemName );

	rcMenuItemName.LoadString( IDS_MENU_EDIT_PASTE );
	::AppendMenu( info->m_Value.m_hMenu, MF_STRING | (!IsEditPasteEnable() ? MF_GRAYED : 0), ID_EDIT_PASTE, rcMenuItemName );

	*(reinterpret_cast<BOOL *>(wParam)) = TRUE;

	return 0;
}

// Запрос на выполнение команды меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если команда выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_Command - идентификатор команды меню
// LRESULT	- не используется
// UINT const CTTM_DOCONTEXTMENUCOMMAND
// ---
LRESULT CVisualEditorDlg::OnCTTDoContextMenuCommand( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	switch( info->m_Value.m_iCommandId ) {
		case ID_EDIT_CUT :
			OnEditCut();
			break;
		case ID_EDIT_COPY :
			OnEditCopy();
			break;
		case ID_EDIT_PASTE :
			OnEditPaste();
			break;
	}

	*(reinterpret_cast<BOOL *>(wParam)) = TRUE;

	return 0;
}


// Сообщение о раскрытии узла
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - расрытие, FALSE - закрытие
// LRESULT	- не используется
// UINT const CTTM_ITEMEXPANDING
// ---
LRESULT CVisualEditorDlg::OnCTTItemExpanding( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {
		CCTItemsArray *items = m_IDescriptions.GetItemsArray();
		CCheckTreeItem *item = (*items)[info->m_nItemIndex];
		if ( !IS_EMPTY_DATA(item) && /*IS_NODE_LABEL(item) && */CPPD(item)->m_hData ) {
			HDATA hData = CPPD(item)->m_hData;
			HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_NODEEXPANDED );
			if ( !hProp )
				hProp = ::DATA_Add_Prop( hData, NULL, VE_PID_NODEEXPANDED, 0, 0 );

			::PROP_Set_Val( hProp, AVP_dword(info->m_Value.m_bSetRemove), sizeof(AVP_bool) );
		}
	}

	if ( info->m_nWindowID == IDC_INTERFACES_TREE ) {
		CCTItemsArray *items = m_Interfaces.GetItemsArray();
		CCheckTreeItem *item = (*items)[info->m_nItemIndex];
		if ( !IS_EMPTY_DATA(item) && /*IS_NODE_LABEL(item) && */CPPD(item)->m_hData ) {
			HDATA hData = CPPD(item)->m_hData;
			HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_NODEEXPANDED );
			if ( !hProp )
				hProp = ::DATA_Add_Prop( hData, NULL, VE_PID_NODEEXPANDED, 0, 0 );

			if ( hProp )
				::PROP_Set_Val( hProp, AVP_dword(info->m_Value.m_bSetRemove), sizeof(AVP_bool) );
		}
	}
	return 0;
}



// Запрос на загрузку IP адреса
// WPARAM		- LPBOOL - указатель на результат - TRUE - если загрузка выполнена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo
// 		        m_Value.m_aIPValue - значения полей IP адреса
// LRESULT	- не используется
// UINT const CTTM_LOADIPADDRESS
// ---
LRESULT CVisualEditorDlg::OnCTTLoadIPAddress( WPARAM wParam, LPARAM lParam ) {
	return 0;
}


// Сообщение о завершении редактирования текста узла типа "IP Address"
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//						m_Value.m_aIPValue[4] - значения полей IP адреса
// LRESULT	- не используется
// UINT const CTTM_ENDADDRESSLABELEDIT
// ---
LRESULT CVisualEditorDlg::OnCTTEndAddressLabelEdit( WPARAM wParam, LPARAM lParam ) {
	return 0;
}


// Сообщение о запросе toolbar ID для меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если ID выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_ToolBarStuff[2]	- значение toolbar ID[0] and Handle[1]
// LRESULT	- не используется
// UINT const CTTM_GETMENUTOOLBAR		
// ---
LRESULT CVisualEditorDlg::OnCTTGetMenuToolbar( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	info->m_Value.m_ToolBarStuff[0] = IDR_MENUTOOLBAR;
	*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
	return 0;
}

// ---
BOOL CVisualEditorDlg::EditLargeComment( CTTMessageInfo *info, DWORD dwTitleID, DWORD dwPrefixID, HDATA hData, HPROP hProp, BOOL bReadOnly, CWnd *pParent /*NULL*/ ) {
	CAPointer<char>	pValue = ::GetPropValueAsString(hProp, NULL);

	CString rcText = pValue;
	CLargeCommentDlg rcDialog( rcText, dwTitleID, bReadOnly, pParent );

	if ( rcDialog.DoModal() == IDOK ) {
		::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(rcText)), 0 );

		// Восстановить отображение узла
		CAPointer<char> pLabel = ::CreateLargeCommentLabel( hData, ::PROP_Get_Id(hProp) );
		pLabel = ::CreateDisplayLabelSTR( dwPrefixID, pLabel );

		delete [] info->m_Value.m_pItemText;
		info->m_Value.m_pItemText = pLabel.Relinquish();

		return TRUE;
	}
	return FALSE;
}


// ---
BOOL CVisualEditorDlg::EditLocalScopeMember( CTTMessageInfo *info, DWORD dwPrefixID, HDATA hData, HPROP hProp ) {
	MAKE_ADDR1(nAddr, VE_PID_IF_DATASTRUCTURES);
	HDATA hStructData = ::DATA_Find( m_hCCTree, nAddr );
	if ( hStructData ) {
		CAPointer<char>	pValue = ::GetPropValueAsString(hProp, NULL);

		CString rcMemberText = pValue;

		CPropMemberDlg rcDialog( rcMemberText, hStructData, this );

		if ( rcDialog.DoModal() == IDOK ) {
			::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(rcMemberText)), 0 );

			// Восстановить отображение узла
			CAPointer<char> pLabel = ::GetPropValueAsString( hProp, NULL );
			pLabel = ::CreateDisplayLabelSTR( dwPrefixID, pLabel );

			delete [] info->m_Value.m_pItemText;
			info->m_Value.m_pItemText = pLabel.Relinquish();

			return TRUE;
		}
	}
	return FALSE;
}

CVisualEditorDlg::SExtEditTitle CVisualEditorDlg::s_arrExtEditTitles[] = 
{
	{VE_PID_IFM_MESSAGE_SEND_POINT, IDS_LET_MSG_SEND_POINT},
	{VE_PID_IFM_MESSAGECONTEXT, IDS_LET_MSG_CONTEXT},
	{VE_PID_IFM_MESSAGEDATA, IDS_LET_MSG_DATA},
	{VE_PID_LARGECOMMENT, IDS_LET_LARGECOMMENT},
	{VE_PID_BEHAVIORCOMMENT, IDS_LET_BEHAVIORCOMMENT},
	{VE_PID_VALUECOMMENT, IDS_LET_VALUECOMMENT},
	{VE_PID_IMPLCOMMENT, IDS_LET_IMPLCOMMENT},
};

int CVisualEditorDlg::GetExtEditTitleId(int i_nPropId)
{
	int nTblSize = sizeof(s_arrExtEditTitles) / sizeof(s_arrExtEditTitles[0]);

	for(int i=0; i<nTblSize; i++)
		if(GET_AVP_PID_ID(s_arrExtEditTitles[i].m_nPropId) == UINT(i_nPropId))
			return s_arrExtEditTitles[i].m_nStrId;

	return -1;
}

LRESULT CVisualEditorDlg::OnCTTDoExternalEditing( WPARAM wParam, LPARAM lParam ) 
{
	CTTMessageInfo *info;
	CVEControlTreeBase* pTreeCtrl;
	CCTItemsArray*	pCtrlItems;
	int				nItemIndex;
	CCheckTreeItem *pItem;
	HDATA hData;
	HPROP hProp;

	GetNotificationData(lParam, info, pTreeCtrl, pCtrlItems, nItemIndex, pItem, hData, hProp);

	if(NULL == pTreeCtrl)
		return 0;

	BOOL bExternalAlias = ((CControlTreeItem*)pItem)->IsExclusiveExternalEditingAlias();
	AVP_dword dwPropId = GetTreeNodePropId(*pTreeCtrl, nItemIndex);

	if(VE_PID_IFPLS_MEMBER_ID == dwPropId)
		bExternalAlias = FALSE;

	BOOL bReadOnly = FALSE;
	if(IsNativeData(hData) && IsImplementation())
		bReadOnly = TRUE;

	if(dwPropId == VE_PID_IMPLCOMMENT_ID)
		bReadOnly = FALSE;

	if(bExternalAlias)
	{
		int nStrTitleId = GetExtEditTitleId(dwPropId);
		int nStrPrefixId = 0;

		MD::SPropMetadata* pPropMetadata = FindPropMetadata(dwPropId, TRUE);
		if(pPropMetadata)
			nStrPrefixId = pPropMetadata->m_nPrefixStrId;

		*(reinterpret_cast<BOOL *>(wParam)) = 
			EditLargeComment( info, nStrTitleId, nStrPrefixId, hData, hProp, bReadOnly );

		return 0;
	}

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) 
	{
		int nItemInd = info->m_nItemIndex;

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
		CCheckTreeItem *item = items[nItemInd];
		HDATA hData = CPPD(items[nItemInd])->m_hData;
		HPROP hProp = CPPD(items[nItemInd])->m_hProp;

		// Вход в редактирование PropertyLocalScopeMember
		if ( ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFPLS_MEMBER_ID) ) {
			*(reinterpret_cast<BOOL *>(wParam)) = EditLocalScopeMember( info, IDS_PLSMEMBER_PREFIX, hData, hProp );
			return 0;
		}
		
		// Вход в редактирование MethodName
		if ( IS_NODE_METHOD(item) || ::IsIDProlog(m_IDescriptions, nItemInd, VE_PID_IFM_METHODNAME_ID) ) {
			CPubMethodInfo mi( hData, false );
			if ( mi.IsSeparator() ) {
				::GetPureInfoValueString( info, hProp );
				*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
			}
			else {
				CMethDlg dlg( this, hData );
				if ( IDOK == dlg.DoModal() ) {
					if ( !hData ) {
						CreateInterfaceMethodNodeContents( items, nItemInd );
						hData = CPPD(items[nItemInd])->m_hData;
					}
					dlg.update( hData );
					CCTItemsArray& newItems = *new CCTItemsArray( true );
					
					LoadOneInterfaceMethodStuff( hData, newItems, item->GetLevel(), false);
					::FullyDeleteItem( m_IDescriptions, nItemInd );
					m_IDescriptions.UpdateItemsArray( nItemInd, newItems, /*BOOL bRootInserted*/ true );
					//UpdateInterfaceMethodNodeContents( info );
					ReloadIncludesTree();
					*(reinterpret_cast<BOOL *>(wParam)) = TRUE;
				}
				else
					*(reinterpret_cast<BOOL *>(wParam)) = FALSE;
			}
			
			return 0;
		}
	}

	return 0;
}


// ---
void CVisualEditorDlg::UpdateContentsScrollItems( HDATA hData, HPROP hProp, int nScrollPos, LPARAM lParam ) {

	CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
	
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	int nFirstInd = LOWORD(info->m_Value.m_nScrollGroup);
	int nLastInd  = HIWORD(info->m_Value.m_nScrollGroup);
	int nCount		= ::PROP_Arr_Count( hProp );

	for ( int i=nFirstInd,j=nScrollPos; i<=nLastInd && j<nCount; i++,j++ ) {
		CCheckTreeItem *item = items[i];
		CAPointer<char> pValue = ::GetPropArrayValueAsString( hProp, j, NULL );
		item->SetText( LPCTSTR(pValue) );
		CPPD(item)->m_nArrayIndex = j;
	}	

	if ( (nScrollPos + CONTENTS_SCROLL_SIZE) > nCount ) {
		CCheckTreeItem *item = items[nLastInd];
		CString rcString;
		rcString.LoadString( IDS_WAS_PP_NEWVALUE );
		item->SetText( rcString );
		CPPD(item)->m_nSpecialType = sdt_ScrollListNewValue;
		CPPD(item)->m_nArrayIndex = nCount;
	}
}


// ---
LRESULT CVisualEditorDlg::OnCTTVScrollUp( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();

		int nParentInd = m_IDescriptions.GetParentIndex( info->m_nItemIndex );

		CCheckTreeItem *item = items[nParentInd];
		CPPDescriptor *pDescr = CPPD(item);

		if ( pDescr->m_nScrollPos ) {

			pDescr->m_nScrollPos--;

			UpdateContentsScrollItems( pDescr->m_hData, pDescr->m_hProp, pDescr->m_nScrollPos, lParam );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;

			return 0;
		}
	}
	*(reinterpret_cast<BOOL *>(wParam)) = FALSE;

	return 0;
}


// ---
LRESULT CVisualEditorDlg::OnCTTVScrollDown( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	if ( info->m_nWindowID == IDC_IF_DESCRIPTION_TREE ) {

		CCTItemsArray &items = *m_IDescriptions.GetItemsArray();

		int nParentInd = m_IDescriptions.GetParentIndex( info->m_nItemIndex );
		int nCount = ::PROP_Arr_Count( CPPD(items[nParentInd])->m_hProp );

		CCheckTreeItem *item = items[nParentInd];
		CPPDescriptor *pDescr = CPPD(item);

		if ( pDescr->m_nScrollPos <= nCount - CONTENTS_SCROLL_SIZE ) {

			pDescr->m_nScrollPos++;

			UpdateContentsScrollItems( pDescr->m_hData, pDescr->m_hProp, pDescr->m_nScrollPos, lParam );

			*(reinterpret_cast<BOOL *>(wParam)) = TRUE;

			return 0;
		}
	}

	*(reinterpret_cast<BOOL *>(wParam)) = FALSE;

	return 0;
}

//---
// fill the list oTypeNames with names of types, 
// remove pExcludeThisType from m_IOTable array
void	CVisualEditorDlg::FillTypeNamesList(CStringList& oTypeNames, TCHAR* pExcludeThisType)
{
	oTypeNames.RemoveAll();		

	int i, c;
	for ( i=0,c=gPropTypes.Count(); i<c; i++ )
	{
		CString strTypeName = gPropTypes[i]->m_pName;
		if(pExcludeThisType && strTypeName == pExcludeThisType)
				continue;
		oTypeNames.AddTail( strTypeName );
	}
	
	for ( i=0,c=m_IOTable.Count(); i<c; i++ )
	{
			CString strTypeName = m_IOTable[i]->m_rcObjectName;
			if(m_IOTable[i]->m_bInterface)
				continue;

			if(pExcludeThisType && strTypeName == pExcludeThisType)
			{
				m_IOTable.RemoveInd(i);
				continue;
			}

			oTypeNames.AddTail( strTypeName );
	}
}

//---
void CVisualEditorDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);

	// update modified flag in title
	if(m_uiIdleTimerId == nIDEvent)
		SetWindowTitle(FALSE);
}



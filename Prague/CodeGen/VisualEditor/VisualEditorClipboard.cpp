// VisualEditorClipboard.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <Stuff\CPointer.h>
#include <StuffIO\IOUtil.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "ImportDefFileDlg.h"
#include "PVE_DT_UTILS.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ---
inline BOOL EnabledTreeTableHas(AVP_dword i_nSubTreeId){

	AVP_dword arrEnabledTreesTable[] = {
			VE_PID_IF_DATASTRUCTURES,
			VE_PID_IF_DATASTRUCTUREMEMBERS,
			VE_PID_IF_CONSTANTS,
			VE_PID_IF_PROPERTIES,
	};
	
	int nEnabledTreesTableCount = sizeof(arrEnabledTreesTable) / sizeof(arrEnabledTreesTable[0]);
	for(int i=0; i<nEnabledTreesTableCount; i++)	
		if(arrEnabledTreesTable[i] == i_nSubTreeId)
			return true;

	return false;
}

inline HDATA GetAncestorWithProp(HDATA i_hData, AVP_dword i_PropId){

	if(NULL == i_hData)
		return NULL;
	HDATA hParentData = ::DATA_Get_Dad( i_hData, NULL );
	while ( hParentData ) {
		if ( ::DATA_Find_Prop( hParentData, NULL, i_PropId ) ) 
			return hParentData;
		i_hData = hParentData;
		hParentData = ::DATA_Get_Dad( i_hData, NULL );
	}
	return NULL;
}


// ---
static BOOL CheckRecursion( HDATA hParentData, HDATA hCheckData ) {
	HDATA hCurrData = ::DATA_Get_First( hParentData, NULL );
	while ( hCurrData ) {
		if ( hCurrData == hCheckData ||
			   ::CheckRecursion(hCurrData, hCheckData) )
			return true;
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	return false;
}


// --- 
HDATA CVisualEditorDlg::DeserializeClipboardData( UINT nClipFormat) {
	HDATA hData = NULL;
	HANDLE hMem = ::GetClipboardData( nClipFormat );

	if ( hMem ) {
		void *pMem  = ::GlobalLock( hMem );

		AVP_Serialize_Data rcSData;
		memset( &rcSData, 0, sizeof(rcSData) );
		
		uint32 dwLength = ::GlobalSize( hMem );

		hData = ::DATA_Deserialize( &rcSData, pMem, dwLength, &dwLength );

		::GlobalUnlock( hMem );
	}

	return hData;
}


// ---
void CVisualEditorDlg::FillAndSetClipboard( HDATA hRootData, UINT nClipFormat ) {
	BeginWaitCursor();

	AVP_Serialize_Data rcSData;
	memset( &rcSData, 0, sizeof(rcSData) );

	AVP_dword nSerData;
	::DATA_Serialize( hRootData, NULL, &rcSData, NULL, 0, &nSerData );

	HANDLE hMem = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, nSerData );
	void *pMem  = ::GlobalLock( hMem );

	::DATA_Serialize( hRootData, NULL, &rcSData, pMem, nSerData, &nSerData );

	::GlobalUnlock( hMem );

	EndWaitCursor();

	if ( ::SetClipboardData( nClipFormat, hMem ) == NULL )  {
		::DlgMessageBox( this, IDS_ERR_SETCLIPDATA );    
		::GlobalFree( hMem );
	} 
}

// ---
BOOL CVisualEditorDlg::CheckTypeIsEqual( HDATA hAttachData, CCheckTreeItem *item ) {
	AVP_dword dwType = 0;
	HPROP hTypeProp = ::DATA_Find_Prop( hAttachData, NULL, VE_PID_NODECLIPTYPE );
	if ( hTypeProp ) 
		::PROP_Get_Val( hTypeProp, &dwType, sizeof(hTypeProp) );
	
	if ( !IS_NODE_EQUAL(item, (int)dwType) ) {
		::DlgMessageBox( this, IDS_ERR_INCORRECTPASTE, MB_ICONEXCLAMATION | MB_OK );
		::DATA_Remove( hAttachData, NULL );
		return false;
	}

	return true;
}


// ---
void CVisualEditorDlg::AttachInterfaceDataTree( HDATA hAttachData ) {
	CCTItemsArray &items = *m_Interfaces.GetItemsArray();
	int nItemInd = m_Interfaces.GetSelectedIndex();
 	CCheckTreeItem *item = items[nItemInd];
	HDATA hTargetData = !IS_EMPTY_DATA(item) ? CPPD(item)->m_hData : NULL;

	if ( CheckTypeIsEqual(hAttachData, item) ) {
		HDATA hParentData;

		if ( hTargetData )
			hParentData = ::DATA_Get_Dad( hTargetData, NULL );
		else {
			int nParentInd = m_Interfaces.GetParentIndex( nItemInd );
			if ( nParentInd >= 0 )
				hParentData = CPPD(items[nParentInd])->m_hData;
			else 
				hParentData = m_hTree;
		}

		AVP_dword nSourceID = VE_PID_INTERFACE(0);

		AVP_dword nReplaceIDId = ::DTUT_FindUniquePID( hParentData, nSourceID );
		::DATA_Replace_ID( hAttachData, NULL, AVP_word(GET_AVP_PID_ID(nReplaceIDId)) );

		if ( !m_hCutData ) {
			AVP_dword nIDId = 0;

			if ( m_pIIDServer ) {
				HRESULT hr = m_pIIDServer->GetID( pic_Interface, &nIDId );
				if ( !SUCCEEDED(hr) )
					DisplayPIDSError( hr );
			}

			if ( !nIDId ) {
				AVP_dword nID = ::DATA_Get_Id( hAttachData, NULL );
				nIDId = GET_AVP_PID_ID(nID);
			}

			HPROP hIDProp;
			if ( hIDProp = ::DATA_Find_Prop(hAttachData, NULL, VE_PID_IF_DIGITALID) ) 
				::PROP_Set_Val( hIDProp, AVP_dword(nIDId), 0 );
			else 
				::DATA_Add_Prop( hAttachData, NULL, VE_PID_IF_DIGITALID, AVP_dword(nIDId), 0 );
		}

		if ( m_nWorkMode == wmt_IDefinition ) 
			::SetNodeUID( hAttachData, TRUE );


		HDATA hCurrData = hTargetData 
											? ::DATA_Insert( hTargetData, NULL, hAttachData )
											: ::DATA_Attach( hParentData, NULL, hAttachData, DATA_IF_ROOT_INCLUDE );
		if ( hCurrData ) {
			if ( m_hCutData ) {
				::DATA_Remove( m_hCutData, NULL );
				m_hCutData = NULL;
			}

			// Удалить данные в элементах поддерева
			m_IDescriptions.ForEach( ::DeleteNodesData, NULL, -1 );
			// Очистить дерево
			m_IDescriptions.DeleteItemWithSubtree( -1 );

			InitNewTree( m_hTree );

			nItemInd = m_Interfaces.FirstThat( ::FindForNodeHDATA, (void *)hCurrData, -1 );

			m_Interfaces.SetFocus();
			m_Interfaces.SelectNode( nItemInd );
			SetModified( true );

			CheckInterfaceName( hCurrData );
		}
/*
		if ( !IS_EMPTY_DATA(item) && CPPD(item)->m_hData ) {
			HDATA hData = CPPD(item)->m_hData;
			HDATA hCurrData = ::DATA_Get_First( hData, NULL );

			while ( hCurrData ) {
				::DATA_Remove( hCurrData, NULL );
				hCurrData = ::DATA_Get_First( hData, NULL );
			}

	//		::DATA_Attach( hData, NULL, hAttachData, DATA_IF_ROOT_EXCLUDE );
			hCurrData = ::DATA_Get_First( hAttachData, NULL );
			while ( hCurrData ) {
				::DATA_Attach( hData, NULL, hCurrData, 0 );
				hCurrData = ::DATA_Get_First( hAttachData, NULL );
			}
			::DATA_Remove( hAttachData, NULL );


			m_hCCTree = hData;

			// Установить Stop-property в папу
			if ( !::DATA_Find_Prop(m_hCCTree, NULL, VE_PID_IF_LASTINTERFACESLEVEL) )
				::DATA_Add_Prop( m_hCCTree, NULL, VE_PID_IF_LASTINTERFACESLEVEL, 0, 0 );

			// Удалить данные в элементах поддерева
			m_IDescriptions.ForEach( ::DeleteNodesData, NULL, -1 );
			// Очистить дерево
			m_IDescriptions.DeleteItemWithSubtree( -1 );

			if ( m_hCCTree )
				LoadInterfaceDescriptionTreeNodes( m_hCCTree );

			SetModified( true );
		}
		else 
			::MessageBeep( 0 );
*/
	}
}


// ---
static void CheckAndRemoveNativeType( HDATA hCurrData, int nWorkMode, AVP_dword dwPID ) {
	if ( nWorkMode == wmt_IImplementation ) {
		HPROP hNativeProp = ::DATA_Find_Prop( hCurrData, NULL, dwPID );
		if ( hNativeProp ) 
			::DATA_Remove_Prop_H( hCurrData, NULL, hNativeProp );
	}
}


// ---
static void CheckErrorCodeValue( HDATA hNewData ) {
	AVP_dword dwMaxValue = 0;
	HDATA hParentData = ::DATA_Get_Dad( hNewData, NULL );
	HPROP hMaxValueProp = ::DATA_Find_Prop( hParentData, NULL, VE_PID_IFE_ERRORCODEVALUE );
	if ( hMaxValueProp ) 
		dwMaxValue = ::GetPropNumericValueAsDWord( hMaxValueProp );

	HPROP hValueProp = ::DATA_Find_Prop( hNewData, NULL, VE_PID_IFE_ERRORCODEVALUE );
	if ( hValueProp ) {
		AVP_dword dwValue = ::GetPropNumericValueAsDWord( hValueProp );

		if ( dwValue > dwMaxValue ) {
			dwValue = ++dwMaxValue;
			::PROP_Set_Val( hValueProp, dwValue, 0 );
		}
		else {
			HDATA hCurrData = ::DATA_Get_First( hParentData, NULL );
			while ( hCurrData ) {
				if ( hCurrData != hNewData ) {
					HPROP hCurrProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IFE_ERRORCODEVALUE );
					if ( hCurrProp ) {
						AVP_dword dwCurrValue = ::GetPropNumericValueAsDWord( hCurrProp );
						if ( dwCurrValue == dwValue ) {
							dwValue = ++dwMaxValue;
							::PROP_Set_Val( hValueProp, dwValue, 0 );
							break;
						}
					}
				}
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}
		::PROP_Set_Val( hMaxValueProp, dwMaxValue, 0 );
	}	
}

// ---
void CVisualEditorDlg::AttachDescriptionDataTree( HDATA hAttachData ) {
	CCTItemsArray &items = *m_IDescriptions.GetItemsArray();
	int nItemInd = m_IDescriptions.GetSelectedIndex();
 	CCheckTreeItem *item = items[nItemInd];
	HDATA hTargetData = CPPD(item)->m_hData;

	if ( CheckTypeIsEqual(hAttachData, item) ) {
		if ( m_hCutData && hTargetData ) {
			if ( ::CheckRecursion(m_hCutData, hTargetData) ) {
				::DlgMessageBox( this,IDS_ERR_CLIPRECURSION, MB_ICONEXCLAMATION | MB_OK );
				::DATA_Remove( hAttachData, NULL );
				return;
			}
		}
		
		HDATA hParentData;

		if ( hTargetData )
			hParentData = ::DATA_Get_Dad( hTargetData, NULL );
		else {
			int nParentInd = m_IDescriptions.GetParentIndex( nItemInd );
			if ( nParentInd >= 0 )
				hParentData = CPPD(items[nParentInd])->m_hData;
			else 
				hParentData = m_hCCTree;
		}

		AVP_dword nSourceID;
		switch ( GET_NODE_TYPE(item) ) {
			case sdt_Method :
				nSourceID = VE_PID_IF_METHOD(0);
				break;
			case sdt_MethodParam :
				nSourceID = VE_PID_IFM_PARAMETER(0);
				break;
			case sdt_Property :
				nSourceID = VE_PID_IF_PROPERTY(0);
				break;
			case sdt_Type :
				nSourceID = VE_PID_IF_TYPE(0);
				break;
			case sdt_Constant :
				nSourceID = VE_PID_IF_CONSTANT(0);
				break;
			case sdt_DataStructure :
				nSourceID = VE_PID_IF_DATASTRUCTURE(0);
				break;
			case sdt_DataStructureMember :
				nSourceID = VE_PID_IFD_MEMBER(0);
				break;
			case sdt_ErrorCode :
				nSourceID = VE_PID_IF_ERRORCODE(0);
				break;
		}

		AVP_dword nReplaceIDId = ::DTUT_FindUniquePID( hParentData, nSourceID );
		::DATA_Replace_ID( hAttachData, NULL, AVP_word(GET_AVP_PID_ID(nReplaceIDId)) );

		HDATA hCurrData = hTargetData 
											? ::DATA_Insert( hTargetData, NULL, hAttachData )
											: ::DATA_Attach( hParentData, NULL, hAttachData, DATA_IF_ROOT_INCLUDE );
		if ( hCurrData ) {
			if ( m_hCutData ) {
				::DATA_Remove( m_hCutData, NULL );
				m_hCutData = NULL;
			}

			switch ( GET_NODE_TYPE(item) ) {
				case sdt_MethodParam :
				case sdt_DataStructure :
				case sdt_DataStructureMember :
					break;
				case sdt_Method :
				case sdt_Property :
					::CheckAndRemoveNativeType( hCurrData, m_nWorkMode, VE_PID_IFP_NATIVE );
					break;
				case sdt_Type :
					::CheckAndRemoveNativeType( hCurrData, m_nWorkMode, VE_PID_IFT_NATIVE );
					break;
				case sdt_Constant :
					::CheckAndRemoveNativeType( hCurrData, m_nWorkMode, VE_PID_IFC_NATIVE );
					break;
				case sdt_ErrorCode : 
					::CheckAndRemoveNativeType( hCurrData, m_nWorkMode, VE_PID_IFE_NATIVE );
					::CheckErrorCodeValue( hCurrData );
					break;
			}

			// process NewSomething nodes, where there is no hData memeber set
			if(NULL == hTargetData){
				CCheckTreeItem *item = items[m_IDescriptions.GetParentIndex(nItemInd)];
				hTargetData = CPPD(item)->m_hData;
			}
			
			// Удалить данные в элементах поддерева
			m_IDescriptions.ForEach( ::DeleteNodesData, NULL, -1 );
			// Очистить дерево
			m_IDescriptions.DeleteItemWithSubtree( -1 );

			HDATA hInterfaceRoot = GetAncestorWithProp(hTargetData, VE_PID_IF_LASTINTERFACESLEVEL);

			if(IsPluginStaticInterfaceRoot(hInterfaceRoot))
				ReloadPluginCCTree(m_hCCTree);
			else
				LoadInterfaceDescriptionTreeNodes( m_hCCTree );

			SetModified( true );
		}
	}
}


// ---
static BOOL IsPasteEnable( CCheckTree &rcTree, UINT uiFormat, int nWorkMode ) {
	CCTItemsArray *items = rcTree.GetItemsArray();
	if ( items && items->GetSize() ) {
		int nItemInd = rcTree.GetSelectedIndex();
 		CCheckTreeItem *item = (*items)[nItemInd];
/*
		return ( !IS_EMPTY_DATA(item) && 
							IS_NODE_LABEL(item) &&
							//CPPD(item)->m_hData && 
							::IsClipboardFormatAvailable(uiFormat) );
*/
		if ( ::IsClipboardFormatAvailable(uiFormat) && !IS_EMPTY_DATA(item) && IS_NODE_LABEL(item) ) {
			HDATA hData = CPPD(item)->m_hData;
			if ( hData ) {
				if ( nWorkMode == wmt_IImplementation ) {
					HDATA hParentData = ::DATA_Get_Dad( hData, NULL );
					while ( hParentData ) {
						HPROP hProp = ::DATA_Find_Prop( hParentData, NULL, VE_PID_IF_LASTINTERFACESLEVEL );
						if ( hProp ) {
							AVP_dword dwPID = ::DATA_Get_Id( hData, NULL );
							if(!IsPluginStaticInterfaceRoot(hParentData))
								return EnabledTreeTableHas(dwPID);
							return true;
						}
						hData = hParentData;
						hParentData = ::DATA_Get_Dad( hData, NULL );
					}
				}
			}
			return true;
		}
	}
	return false;
}


// ---
BOOL CVisualEditorDlg::IsEditPasteEnable() {

	if(m_bTechnicalEditting)
		return false;
	
	HWND hWndFocus = ::GetFocus();
	if ( hWndFocus ) {
		if ( hWndFocus == m_Interfaces.GetSafeHwnd() ) {
			if ( m_nWorkMode == wmt_IImplementation )
				return false;

			return ::IsPasteEnable( m_Interfaces, m_nInterfacesClipFormat, m_nWorkMode );
		}
		if ( hWndFocus == m_IDescriptions.GetSafeHwnd() ) {
			return ::IsPasteEnable( m_IDescriptions, m_nDescriptionsClipFormat, m_nWorkMode );
		}
	}
	return false;
}

// ---
static BOOL CheckIsNativeType( HDATA hCurrData, AVP_dword dwPID ) {
	HPROP hNativeProp = ::DATA_Find_Prop( hCurrData, NULL, dwPID );
	if ( hNativeProp ) 
		return !!::GetPropNumericValueAsDWord( hNativeProp );
	return FALSE;
}


// ---
static BOOL IsImpAndNodeNative( CCheckTree &rcTree, int nWorkMode ) {
	if ( nWorkMode == wmt_IImplementation ) {
		CCTItemsArray *items = rcTree.GetItemsArray();
		if ( items && items->GetSize() ) {
			int nItemInd = rcTree.GetSelectedIndex();
			CCheckTreeItem *item = (*items)[nItemInd];
			if ( !IS_EMPTY_DATA(item) && IS_NODE_LABEL(item) ) {
				HDATA hData = CPPD(item)->m_hData;
				if ( hData ) {
					HPROP hNativeProp;
					
					hNativeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFP_NATIVE );
					if ( hNativeProp ) 
						return true;
					hNativeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFC_NATIVE );
					if ( hNativeProp ) 
						return true;
					hNativeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFE_NATIVE );
					if ( hNativeProp ) 
						return true;
					hNativeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFT_NATIVE );
					if ( hNativeProp ) 
						return true;
					hNativeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFM_NATIVE );
					if ( hNativeProp ) 
						return true;
					hNativeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMC_NATIVE );
					if ( hNativeProp ) 
						return true;
				}
			}
		}
	}
	return false;
}

	
// ---
static BOOL IsCopyEnable( CCheckTree &rcTree, int nWorkMode ) {
	CCTItemsArray *items = rcTree.GetItemsArray();
	if ( items && items->GetSize() ) {
		int nItemInd = rcTree.GetSelectedIndex();
 		CCheckTreeItem *item = (*items)[nItemInd];
		if ( !IS_EMPTY_DATA(item) && IS_NODE_LABEL(item) ) {
			HDATA hData = CPPD(item)->m_hData;
			if ( hData ) {
				if ( nWorkMode == wmt_IImplementation ) {
					HDATA hParentData = ::DATA_Get_Dad( hData, NULL );
					while ( hParentData ) {
						HPROP hProp = ::DATA_Find_Prop( hParentData, NULL, VE_PID_IF_LASTINTERFACESLEVEL );
						if ( hProp ) {
							AVP_dword dwPID = ::DATA_Get_Id( hData, NULL );
							if(!IsPluginStaticInterfaceRoot(hParentData))
								return EnabledTreeTableHas(dwPID);
							return true;
						}
						hData = hParentData;
						hParentData = ::DATA_Get_Dad( hData, NULL );
					}
				}
				return true;
			}
		}
	}
	return false;
}

// ---
BOOL CVisualEditorDlg::IsEditCopyEnable() {

	if(m_bTechnicalEditting)
		return false;

	HWND hWndFocus = ::GetFocus();
	if ( hWndFocus ) {
		if ( hWndFocus == m_Interfaces.GetSafeHwnd() ) {
			if ( m_nWorkMode == wmt_IImplementation )
				return false;

			return ::IsCopyEnable( m_Interfaces, m_nWorkMode );
		}
		if ( hWndFocus == m_IDescriptions.GetSafeHwnd() ) {
			return ::IsCopyEnable( m_IDescriptions, m_nWorkMode );
		}
	}
	return false;
}


// ---
BOOL CVisualEditorDlg::IsEditCutEnable() {

	if(m_bTechnicalEditting)
		return false;
	
	HWND hWndFocus = ::GetFocus();
	if ( hWndFocus ) {
		if ( hWndFocus == m_Interfaces.GetSafeHwnd() ) {
			if ( m_nWorkMode == wmt_IImplementation )
				return false;

			return ::IsCopyEnable( m_Interfaces, m_nWorkMode );
		}
		if ( hWndFocus == m_IDescriptions.GetSafeHwnd() ) {
			return !IsImpAndNodeNative(m_IDescriptions, m_nWorkMode) && ::IsCopyEnable( m_IDescriptions, m_nWorkMode );
		}
	}
	return false;
}


// ---
void CVisualEditorDlg::OnEditCut() {
	if ( IsEditCopyEnable() ) {
		HWND hWndFocus = ::GetFocus();

		if ( hWndFocus == m_Interfaces.GetSafeHwnd() ) {
			CCTItemsArray *items = m_Interfaces.GetItemsArray();
			int nItemInd = m_Interfaces.GetSelectedIndex();
 			CCheckTreeItem *item = (*items)[nItemInd];
			m_hCutData = CPPD(item)->m_hData;
			OnEditCopy();
			return;
		}

		if ( hWndFocus == m_IDescriptions.GetSafeHwnd() ) {
			CCTItemsArray *items = m_IDescriptions.GetItemsArray();
			int nItemInd = m_IDescriptions.GetSelectedIndex();
 			CCheckTreeItem *item = (*items)[nItemInd];
			m_hCutData = CPPD(item)->m_hData;
			OnEditCopy();
			return;
		}
	}
	::MessageBeep( 0 );
}

// ---
void CVisualEditorDlg::OnEditCopy() {
	if ( IsEditCopyEnable() ) {
		if ( !OpenClipboard() )  {
			::DlgMessageBox( this, IDS_ERR_OPENCLIPBOARD );
			return; 
		}  

		if( !EmptyClipboard() )  {
			::DlgMessageBox( this,IDS_ERR_EMPTYCLIPBOARD );
			return; 
		}  

		HWND hWndFocus = ::GetFocus();
		if ( hWndFocus == m_Interfaces.GetSafeHwnd() && m_hCCTree ) {
			CCTItemsArray *items = m_Interfaces.GetItemsArray();
			int nItemInd = m_Interfaces.GetSelectedIndex();
 			CCheckTreeItem *item = (*items)[nItemInd];

			AVP_dword dwType = GET_NODE_TYPE(item);
			HPROP hTypeProp = ::DATA_Find_Prop( m_hCCTree, NULL, VE_PID_NODECLIPTYPE );
			SetModified( true );
			if ( !hTypeProp ) 
				::DATA_Add_Prop( m_hCCTree, NULL, VE_PID_NODECLIPTYPE, dwType, 0 );
			else 
				::PROP_Set_Val( hTypeProp, dwType, 0 );
			FillAndSetClipboard( m_hCCTree, m_nInterfacesClipFormat );
		}

		if ( hWndFocus == m_IDescriptions.GetSafeHwnd() ) {
			CCTItemsArray *items = m_IDescriptions.GetItemsArray();
			int nItemInd = m_IDescriptions.GetSelectedIndex();
 			CCheckTreeItem *item = (*items)[nItemInd];
			HDATA hData = CPPD(item)->m_hData;

			AVP_dword dwType = GET_NODE_TYPE(item);
			HPROP hTypeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_NODECLIPTYPE );
			if ( !hTypeProp ) 
				::DATA_Add_Prop( hData, NULL, VE_PID_NODECLIPTYPE, dwType, 0 );
			else 
				::PROP_Set_Val( hTypeProp, dwType, 0 );
			FillAndSetClipboard( hData, m_nDescriptionsClipFormat );
		}
  
		CloseClipboard();

		return;
	}
	::MessageBeep( 0 );
}

// ---
void CVisualEditorDlg::OnEditPaste() {
	if ( IsEditPasteEnable() ) {
		if ( !OpenClipboard() )  {
			::DlgMessageBox( this, IDS_ERR_OPENCLIPBOARD );
			return; 
		}  

		BeginWaitCursor();

		HWND hWndFocus = ::GetFocus();
		if ( hWndFocus == m_Interfaces.GetSafeHwnd() ) {
			HDATA hData = DeserializeClipboardData( m_nInterfacesClipFormat );
			if ( hData ) 
				AttachInterfaceDataTree( hData );
		}

		if ( hWndFocus == m_IDescriptions.GetSafeHwnd() ) {
			HDATA hData = DeserializeClipboardData( m_nDescriptionsClipFormat );
			if ( hData ) 
				AttachDescriptionDataTree( hData );
		}

		CloseClipboard();

		EndWaitCursor();

		return;
	}
	::MessageBeep( 0 );
}


// ---
void CVisualEditorDlg::OnUpdateEditCopy(CCmdUI* pCmdUI) {
	pCmdUI->Enable( IsEditCopyEnable() );
}

// ---
void CVisualEditorDlg::OnUpdateEditCut(CCmdUI* pCmdUI) {
	pCmdUI->Enable( IsEditCutEnable() );
}

// ---
void CVisualEditorDlg::OnUpdateEditPaste(CCmdUI* pCmdUI) {
	pCmdUI->Enable( IsEditPasteEnable() );
}


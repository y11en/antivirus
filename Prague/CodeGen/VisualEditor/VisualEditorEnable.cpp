// VisualEditorEnable.cpp : implementation file
//

#include "stdafx.h"
#include <StuffIO\Utils.h>
#include "resource.h"
#include <Stuff\CPointer.h>
#include "VisualEditorDlg.h"
#include <avpprpid.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ---
static void EnableFunc( CTreeWnd &owner, HTREEITEM cur, int nItemInd, void *pParam ) {
	CCTItemsArray &items = *((CControlTree &)owner).GetItemsArray();
	CCheckTreeItem *item = items[nItemInd];

	if ( !IS_EMPTY_DATA(item) ) {
		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = CPPD(item)->m_hProp;

		((CVisualEditorDlg *)pParam)->EnableInterfacePropertyLocalWriteStuff( hData, hProp, nItemInd );
	}
}


// ---
void CVisualEditorDlg::EnableEditableStuff() {
  m_IDescriptions.ForEach( EnableFunc, this ); 
}


// ---
void CVisualEditorDlg::EnableInterfacePropertyLocalWriteStuff( HDATA hNodeData, HPROP hProp, int nIndex ) {
	if ( hProp ) {
    if (::PROP_Get_Id(hProp) == VE_PID_IFPLS_MEMBER_MODE ) {
			CAPointer<char> pLabel;
			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( hNodeData, NULL, VE_PID_IFPLS_MEMBER ) ) {
				pLabel = ::GetPropValueAsString( hValueProp, NULL );

				m_IDescriptions.SetItemEnable( nIndex, pLabel && *pLabel, false );
			}
		}
		if (::PROP_Get_Id(hProp) == VE_PID_IFMP_ISCONST ) {
			HPROP hValueProp;
      AVP_bool bChecked = FALSE;
      if ( 0 != (hValueProp = ::DATA_Find_Prop( hNodeData, NULL, VE_PID_IFMP_ISPOINTER )) )
				bChecked = ::GetPropNumericValueAsDWord( hValueProp );
      if ( !bChecked && (0 != (hValueProp = ::DATA_Find_Prop( hNodeData, NULL, VE_PID_IFMP_TYPE ))) ) {
        AVP_dword val = ::GetPropNumericValueAsDWord( hValueProp );
        bChecked = TYPE_IS_POINTER(val);
      }
			m_IDescriptions.SetItemEnable( nIndex, !!bChecked, false );
			if ( !bChecked )
				m_IDescriptions.SetItemChecked( nIndex, CCheckTreeItem::ie_Unchecked, false ); 
		}
	}
}

bool FindNodeWithDataProp( CTreeWnd &owner, HTREEITEM cur, int ind, void *par )
{
	CControlTreeItem* pItem = (CControlTreeItem*) owner.GetItemData(cur);
	if(!pItem)
		return false;

	CPPDescriptor* pDescr = (CPPDescriptor*) pItem->GetData();
	CPPDescriptor* pDescrValid = (CPPDescriptor*) par;
	
	if(!pDescr ||!pDescrValid)
		return false;
		
	if(pDescrValid->m_hData == pDescr->m_hData && pDescrValid->m_hProp == pDescr->m_hProp)
		return true;

	return false;
}

// returns false if user selected to stay in and correct names
BOOL	CVisualEditorDlg::CheckPluginDefaultName()
{					     
	CAPointer<char> pLabel;
	HPROP hValueProp = ::DATA_Find_Prop(m_hTree, NULL, VE_PID_PL_PLUGINNAME);
			  
	// plugin name
	if(hValueProp)
	{
		pLabel = ::GetPropValueAsString( hValueProp, NULL );
		if(pLabel == CString(_T("Plugin")) && 
		   IDNO == AfxMessageBox(IDS_DEFAULT_PLUGIN_NAME_WARNING, MB_YESNO))
		{
			// we need to select plugin name node

			CPPDescriptor oValidDescr;
			oValidDescr.m_hData = m_hTree;
			oValidDescr.m_hProp = hValueProp;
			int nPluginNameIdx = m_Interfaces.FirstThat(FindNodeWithDataProp, &oValidDescr, -1);

			// its a hadr way to get HTREEITEM
			CCTItemsArray* pItems = m_Interfaces.GetItemsArray();
			CControlTreeItem* pItem = NULL;
			if(-1 != nPluginNameIdx)
				pItem = (CControlTreeItem*)(*pItems)[nPluginNameIdx]; 
			
			HTREEITEM hPluginName = NULL;
			if(pItem)
				hPluginName = pItem->GetTreeItem();

			if(hPluginName)
			{
				m_Interfaces.Select(hPluginName, TVGN_CARET);
				m_Interfaces.EnsureVisible(hPluginName);
				m_Interfaces.SetFocus();
			}

			return false;
		}
	}
		   
	hValueProp = ::DATA_Find_Prop(m_hTree, NULL, VE_PID_PL_MNEMONICID);
	
	if(hValueProp)
	{
		pLabel = ::GetPropValueAsString( hValueProp, NULL );
		if(pLabel == CString(_T("PID_PLUGIN")) && 
			IDNO == AfxMessageBox(IDS_DEFAULT_PLUGIN_PID_WARNING, MB_YESNO))
		{
			// we need to select plugin PID node
			
			CPPDescriptor oValidDescr;
			oValidDescr.m_hData = m_hTree;
			oValidDescr.m_hProp = hValueProp;
			int nPluginPIDIdx = m_Interfaces.FirstThat(FindNodeWithDataProp, &oValidDescr, -1);
			
			// its a hadr way to get HTREEITEM
			CCTItemsArray* pItems = m_Interfaces.GetItemsArray();
			CControlTreeItem* pItem = NULL;
			if(-1 != nPluginPIDIdx)
				pItem = (CControlTreeItem*)(*pItems)[nPluginPIDIdx]; 
			
			HTREEITEM hPluginPID = NULL;
			if(pItem)
				hPluginPID = pItem->GetTreeItem();
			
			if(hPluginPID)
			{
				m_Interfaces.Select(hPluginPID, TVGN_CARET);
				m_Interfaces.EnsureVisible(hPluginPID);
				m_Interfaces.SetFocus();
			}
			
			return false;
		}
	}

	return true;
}

// PageAntispam.cpp : Implementation of CPageAntispam

#include "stdafx.h"
#pragma warning(disable: 4146)
#import "dll\mso9.dll" rename_namespace("Office"), named_guids, implementation_only rename("RGB", "msoRGB") rename("DocumentProperties", "msoDocumentProperties")
#include "OutlookAddin.h"
#include "PageAntispam.h"

#ifdef SubclassWindow
#undef SubclassWindow
#endif

inline int MsgBox(CItemBase * pItem, LPCTSTR strTextId, LPCTSTR strCaptId = NULL, UINT uType = MB_OK|MB_ICONINFORMATION, cSerializable * pBind = NULL)
{
	if( !pItem )
		return 0;

	return pItem->m_pRoot->ShowMsgBox(pItem, strTextId,
		strCaptId ? strCaptId : "dlg_caption", uType, pBind);
}

/////////////////////////////////////////////////////////////////////////////
// CPageAntispam

STDMETHODIMP  CPageAntispam::GetControlInfo(LPCONTROLINFO pCI )
{
	return S_OK;
};

// IOleObject
STDMETHODIMP CPageAntispam::SetClientSite(IOleClientSite *pClientSite)
{
	// call default ATL implementation
	HRESULT result = IOleObjectImpl<CPageAntispam>::SetClientSite(pClientSite);
	if (result != S_OK)
		return result;
	
	// pClientSite may be NULL when container has being destructed
	if (pClientSite != NULL)
	{
		CComQIPtr<Outlook::PropertyPageSite> pPropertyPageSite(pClientSite);
		result = pPropertyPageSite.CopyTo(&m_pPropPageSite);
	}
	m_fDirty = false;
	return result;
}

STDMETHODIMP CPageAntispam::get_Dirty(VARIANT_BOOL *Dirty)
{
	*Dirty = m_fDirty.boolVal;
	return S_OK;
}

STDMETHODIMP CPageAntispam::Apply()
{
	return SaveSettings() ? S_OK : E_FAIL;
}

STDMETHODIMP CPageAntispam::GetPageInfo(BSTR *HelpFile,LONG *HelpContext)
{
	if (HelpFile == NULL)
		return E_POINTER;
	if (HelpContext == NULL)
		return E_POINTER;
	return E_NOTIMPL;	
}

STDMETHODIMP CPageAntispam::OnAmbientPropertyChange(DISPID dispid)
{
	if (dispid == DISPID_AMBIENT_BACKCOLOR)
	{
		//SetBackgroundColorFromAmbient();
		FireViewChange();
	}
	return IOleControlImpl<CPageAntispam>::OnAmbientPropertyChange(dispid);
}

// ---------------------- CPageAntispam kav6 se ----------------------

HRESULT CPageAntispam::ControlQueryInterface(const IID& iid, void** ppv)
{
	return _InternalQueryInterface(iid, ppv);
}

HWND CPageAntispam::CreateControlWindow(HWND hWndParent, RECT& rcPos)
{
	CRootItem * pRoot = CGuiLoader::Instance().GetRoot();
	if( !pRoot )
		return NULL;

	m_hWndParent = hWndParent;
	
	CItemBase * pItem = NULL;
	pRoot->LoadItem(this, pItem, "OutlookPlugun.Antispam");
	if( !pItem )
		return NULL;
	
	return m_hWnd = pItem->GetWindow();
}

void CPageAntispam::SetDirty(bool bDirty)
{
	m_fDirty = true;
	m_pPropPageSite->OnStatusChange();
}

/////////////////////////////////////////////////////////////////////////////
// CPageAntispamSink
void CPageAntispamSink::OnCreate()
{
	TBaseDlg::OnCreate();
#if _BASEGUI_VER >= 0x0200
	Item()->AddDataSource(&m_Status);
#else
	m_pBinding->AddDataSource(&m_Status);
#endif
}

void CPageAntispamSink::OnInit()
{
	if(!IsInited())
	{
		Show("CAlgo", m_bAdvView);
		Show("CA_Line", m_bAdvView);
		Show("CAlgoGroup", m_bAdvView);
		
		GetStatus(m_Status);

		m_pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
		if(m_pAS)
			m_pAS->GetSettings(&m_Settings);

#if _BASEGUI_VER >= 0x0200
#else
		CComboItem *pCombo;
		if(pCombo = (CComboItem *)GetItem("CS_Action", GUI_ITEMT_COMBO))
			pCombo->Fill("AS_Actions");
		if(pCombo = (CComboItem *)GetItem("PS_Action", GUI_ITEMT_COMBO))
			pCombo->Fill("AS_Actions");
#endif
	}

	TBaseDlg::OnInit();
}

bool CPageAntispamSink::ValidateRule(AntispamPluginRule_t &Rule)
{
	return (Rule.AntispamPluginAction == AntispamPluginAction_Move || Rule.AntispamPluginAction == AntispamPluginAction_Copy) && Rule.FolderForAction.szName.empty();
}

bool CPageAntispamSink::SaveSettings()
{
	if(ValidateRule(m_Settings.CertainSpamRule) || ValidateRule(m_Settings.PossibleSpamRule))
	{
		MsgBox(*this, "FolderMissing", NULL, MB_OK|MB_ICONERROR);
		return false;
	}
	m_pAS->SetSettings(&m_Settings);
	return true;
}

bool CPageAntispamSink::OnOk()
{
	if(!SaveSettings())
		return false;
	
	return TBaseDlg::OnOk();
}

bool CPageAntispamSink::OnCanClose(bool bUser)
{
	if ( m_bAskOnCancel && MsgBox(*this, "ShowSettingsAgain", NULL, MB_YESNO|MB_ICONINFORMATION ) == IDNO )
	{
		// Сохраняем настройки по умолчанию
		if (m_pAS) m_pAS->SetSettings(&m_Settings);
	}
	
	return TBaseDlg::OnCanClose(bUser);
}

bool CPageAntispamSink::OnClicked(CItemBase* pItem)
{
	FolderInfo_t *pFolder = NULL;
	sswitch(pItem->m_strItemId.c_str())
	scase("CS_Browse") pFolder = &m_Settings.CertainSpamRule.FolderForAction;  sbreak
	scase("PS_Browse") pFolder = &m_Settings.PossibleSpamRule.FolderForAction; sbreak
	scase("Link")      if(m_pAS) m_pAS->ShowProductASSettings(); sbreak
	send

	if(pFolder)
	{
		UpdateData(false);
		if(CBrowseForFolder(m_pAS, pFolder).DoModal(Item(), "OutlookPlugun.Antispam.Browse") == DLG_ACTION_OK)
		{
			UpdateData(true);
			SetDirty();
		}
	}
	
	return TBaseDlg::OnClicked(pItem);
}

void CPageAntispamSink::OnChangedData(CItemBase* pItem)
{
	TBaseDlg::OnChangedData(pItem);
	SetDirty();
}

void CPageAntispamSink::OnTimerRefresh(tDWORD nTimerSpin)
{
	if(nTimerSpin % 10)
		return;

	cAntispamPluginStatus Status; GetStatus(Status);
	if(m_Status.m_bKavStarted != Status.m_bKavStarted || m_Status.m_bAsEnabled != Status.m_bAsEnabled)
	{
		m_Status = Status;
		UpdateData(&m_Status, true);
	}
}

HWND CPageAntispamSink::OnGetParent()
{
	return m_hWndParent;
}

void CPageAntispamSink::GetStatus(cAntispamPluginStatus &Status)
{
	Status.m_bKavStarted = CAntispamOUSupportClass::IsAVPTaskManagerEnabled();
	Status.m_bAsEnabled  = CAntispamOUSupportClass::IsAntispamEngineEnabled();
}

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

void CFolderTree::OnInited()
{
	ExpandItem(NULL);
	cTreeItem *pChildItem = Item()->GetChildItem(NULL);
	if(pChildItem)
		Item()->ExpandItem(pChildItem);
}

bool CFolderTree::OnItemExpanding(cTreeItem * pItem, tUINT& nChildren)
{
	return ExpandItem(pItem);
}

bool CFolderTree::ExpandItem(cTreeItem * hItem)
{
	FolderInfo_t * pFolder = NULL;
	if( hItem )
	{
		if( Item()->GetChildItem(hItem) )
			return true;
		pFolder = &m_aFolders[((TVIDATA *)hItem)->m_nFolder];
	}

	tDWORD i = m_aFolders.size();

	HRESULT res;
	if( pFolder )
	{
		FolderInfo_t copy = *pFolder;
		res = m_pAS->GetFolderInfoList(&copy, &m_aFolders);
	}
	else
		res = m_pAS->GetFolderInfoList(NULL, &m_aFolders);
	
	for(; i < m_aFolders.size(); i++)
	{
		pFolder = &m_aFolders[i];
		Item()->AppendItem(hItem, new TVIDATA(pFolder->szName, i, pFolder->bHasChildren));
	}
	
	return true;
}

bool CFolderTree::GetFolder(FolderInfo_t &FolderInfo)
{
	cTreeItem * hItem = Item()->GetSelectionItem();
	if( !hItem )
		return false;

	FolderInfo = m_aFolders[((TVIDATA *)hItem)->m_nFolder];
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseForFolder

bool CBrowseForFolder::OnOk()
{
	if( m_Tree && m_Tree.GetFolder(*m_pFldInfo) )
		return TBase::OnOk();
	return false;
}

// SettingsDlg.cpp: implementation of the CSettingsDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SettingsDlg.h"

static int MsgBox(CItemBase * pItem, LPCTSTR strTextId, LPCTSTR strCaptId = NULL, UINT uType = MB_OK|MB_ICONINFORMATION, cSerializable * pBind = NULL)
{
	if( !pItem )
		return 0;

	return pItem->m_pRoot->ShowMsgBox(pItem, strTextId,
		strCaptId ? strCaptId : "dlg_caption", uType, pBind);
}

/////////////////////////////////////////////////////////////////////////////
// CPageAntispamSink

void CPageAntispamSink::OnInit()
{
	if( !IsInited() )
	{
		Show("CAlgo", false);
		Show("CA_Line", false);
		Show("CAlgoGroup", false);

#if _BASEGUI_VER >= 0x0200
		Item()->AddDataSource(&m_Status);
#else
		m_pBinding->AddDataSource(&m_Status);
#endif
	}

#if _BASEGUI_VER >= 0x0200
#else
	if( CComboItem *pCombo = (CComboItem *)GetItem("CS_Action", GUI_ITEMT_COMBO) )
		pCombo->Fill("AS_Actions");
	if( CComboItem *pCombo = (CComboItem *)GetItem("PS_Action", GUI_ITEMT_COMBO) )
		pCombo->Fill("AS_Actions");
#endif

	UpdateStatus();
	TBaseDlg::OnInit();
}

bool CPageAntispamSink::ValidateRule(AntispamPluginRule_t &Rule)
{
	return (Rule.AntispamPluginAction == AntispamPluginAction_Move ||
		Rule.AntispamPluginAction == AntispamPluginAction_Copy) &&
		Rule.FolderForAction.szName.empty();
}

bool CPageAntispamSink::OnOk()
{
	if( !TBaseDlg::OnOk() )
		return false;

	if( ValidateRule(m_Settings.CertainSpamRule) || ValidateRule(m_Settings.PossibleSpamRule) )
	{
		MsgBox(*this, "FolderMissing");
		return false;
	}

	return true;
}

bool CPageAntispamSink::OnCanClose(bool bUser)
{
	if (!COESettings::IsOEPluginInstalled() && MsgBox(*this, "ShowSettingsAgain", 
		NULL, MB_YESNO | MB_ICONINFORMATION) == IDNO)
		m_bShouldSaveAnyway = true;
	
	return TBaseDlg::OnCanClose(bUser);
}

bool CPageAntispamSink::OnClicked(CItemBase* pItem)
{
	FolderInfo_t * pFolder = NULL;
	sswitch(pItem->m_strItemId.c_str())
	scase("CS_Browse") pFolder = &m_ser->CertainSpamRule.FolderForAction;  sbreak;
	scase("PS_Browse") pFolder = &m_ser->PossibleSpamRule.FolderForAction; sbreak;
	scase("Link")      _Module.ShowAntiSpamSettings(); sbreak
	send
		
	if( pFolder )
	{
		UpdateData(false);
		if(CBrowseForFolder(m_arrLocalFolders, pFolder, m_hTreeImageList).DoModal(Item(), "OutlookPlugun.Antispam.Browse") == DLG_ACTION_OK)
		{
			UpdateData(true);
		}
	}
	
	return TBaseDlg::OnClicked(pItem);
}

void CPageAntispamSink::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( nTimerSpin % 10 )
		return;
	
	UpdateStatus();
	UpdateData(&m_Status, true);
}

HWND CPageAntispamSink::OnGetParent()
{
	return m_hWndParent;
}

void CPageAntispamSink::UpdateStatus()
{
	m_Status.m_bKavStarted = _Module.IsKAVRunning();
	m_Status.m_bAsEnabled  = _Module.IsAntiSpamTaskRunning();
}

//////////////////////////////////////////////////////////////////////////

COESettings::SpamActions _SpamActions_N2O(AntispamPluginAction_t nAction)
{
	switch( nAction )
	{
	case AntispamPluginAction_Move:     return COESettings::SA_MOVETOFOLDER;
	case AntispamPluginAction_Copy:     return COESettings::SA_COPYTOFOLDER;
	case AntispamPluginAction_Delete:   return COESettings::SA_DELETE;
	case AntispamPluginAction_KeepAsIs: return COESettings::SA_LEAVEASIS;
	}

	return COESettings::SA_UNKNOWN;
}

AntispamPluginAction_t _SpamActions_O2N(COESettings::SpamActions nAction)
{
	switch( nAction )
	{
	case COESettings::SA_MOVETOFOLDER:  return AntispamPluginAction_Move;
	case COESettings::SA_COPYTOFOLDER:  return AntispamPluginAction_Copy;
	case COESettings::SA_DELETE:        return AntispamPluginAction_Delete;
	case COESettings::SA_LEAVEASIS:     return AntispamPluginAction_KeepAsIs;
	}

	return (AntispamPluginAction_t)-1;
}

void CPageAntispamSink::_FolderId_O2N(__int64 nId, FolderInfo_t& pFolder) const
{
	pFolder.szName = "";
	
	COETricks::FOLDERS_LIST::const_iterator it = m_arrLocalFolders.begin();
	for(; it != m_arrLocalFolders.end(); ++it)
		if( it->m_dwID == nId )
		{
			pFolder.szName = it->m_strName;
			break;
		}

	SBinary _Id; _Id.cb = sizeof(nId); _Id.lpb = (tBYTE *)&nId;
	pFolder.EntryID = _Id;
}

__int64 CPageAntispamSink::_FolderId_N2O(const FolderInfo_t& pFolder) const
{
	__int64 nId = 0;

	if( pFolder.EntryID.cb == sizeof(nId) )
		nId = *(__int64 *)pFolder.EntryID.buff.m_ptr;

	return nId;
}

void CPageAntispamSink::SetSettings(IN const COESettings& settings)
{
	COESettings::SpamActions nAction;
	bool bMarkAsRead;
	__int64 dwFolderID;
	
	settings.GetSpamSettings(nAction, bMarkAsRead, dwFolderID);
	m_Settings.CertainSpamRule.AntispamPluginAction = _SpamActions_O2N(nAction);
	m_Settings.CertainSpamRule.bMarkAsRead = bMarkAsRead;
	_FolderId_O2N(dwFolderID, m_Settings.CertainSpamRule.FolderForAction);
	
	settings.GetProbSpamSettings(nAction, bMarkAsRead, dwFolderID);
	m_Settings.PossibleSpamRule.AntispamPluginAction = _SpamActions_O2N(nAction);
	m_Settings.PossibleSpamRule.bMarkAsRead = bMarkAsRead;
	_FolderId_O2N(dwFolderID, m_Settings.PossibleSpamRule.FolderForAction);

//	m_Settings.bUseRequestLogic = ;
//	m_Settings.dwRequestStoreDaysCount = ;
//	m_Settings.RequestList = ;
}

void CPageAntispamSink::GetSettings(OUT COESettings& settings) const
{
	settings.SetSpamSettings(
		_SpamActions_N2O(m_Settings.CertainSpamRule.AntispamPluginAction),
		!!m_Settings.CertainSpamRule.bMarkAsRead,
		_FolderId_N2O(m_Settings.CertainSpamRule.FolderForAction));
	
	settings.SetProbSpamSettings(
		_SpamActions_N2O(m_Settings.PossibleSpamRule.AntispamPluginAction),
		!!m_Settings.PossibleSpamRule.bMarkAsRead,
		_FolderId_N2O(m_Settings.PossibleSpamRule.FolderForAction));
}

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

bool CFolderTree::_FolderExists(__int64 dwFolderID)
{
	COETricks::FOLDERS_LIST::const_iterator it = m_aLocalFolders.begin();
	for (; it != m_aLocalFolders.end(); ++it)
	{
		if (it->m_dwID == dwFolderID)
			return true;
	}
	
	return false;
}

bool CFolderTree::_FolderHasChildren(__int64 dwFolderID)
{
	COETricks::FOLDERS_LIST::const_iterator it = m_aLocalFolders.begin();
	for (; it != m_aLocalFolders.end(); ++it)
	{
		if (it->m_dwID == dwFolderID)
			continue;

		if (it->m_dwParentID == dwFolderID)
			return true;
	}

	return false;
}

void CFolderTree::GetFolderInfoList(IN COETricks::TFolder* pFolderInfo, OUT COETricks::FOLDERS_LIST* pFolderInfoList)
{
	COETricks::FOLDERS_LIST::const_iterator it = m_aLocalFolders.begin();
	for (; it != m_aLocalFolders.end(); ++it)
	{
		if( pFolderInfo && ( it->m_dwParentID == pFolderInfo->m_dwID ) ||
			!pFolderInfo && !_FolderExists(it->m_dwParentID))
		{
			pFolderInfoList->push_back(*it);
		}
	}
}

void CFolderTree::OnInited()
{
	Item()->SetImageList(m_hTreeImageList);
	ExpandItem(NULL);
	Item()->ExpandItem(Item()->GetChildItem(NULL));
}

bool CFolderTree::OnItemExpanding(cTreeItem * pItem, tUINT& nChildren)
{
	return ExpandItem(pItem);
}

bool CFolderTree::ExpandItem(cTreeItem * hItem)
{
	COETricks::TFolder * pFolder = NULL;
	if( hItem )
	{
		if( Item()->GetChildItem(hItem) )
			return true;
		pFolder = &m_aFolders[((TVIDATA *)hItem)->m_nFolder];
	}

	size_t i = m_aFolders.size();

	if( pFolder )
	{
		COETricks::TFolder copy = *pFolder;
		GetFolderInfoList(&copy, &m_aFolders);
	}
	else
		GetFolderInfoList(NULL, &m_aFolders);
	
	for(; i < m_aFolders.size(); i++)
	{
		pFolder = &m_aFolders[i];
		Item()->AppendItem(hItem, new TVIDATA(pFolder->m_strName, (tUINT)i, _FolderHasChildren(pFolder->m_dwID), pFolder->m_nImage));
	}
	
	return true;
}

bool CFolderTree::GetFolder(COETricks::TFolder &FolderInfo)
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
	COETricks::TFolder folder;
	if( m_Tree && m_Tree.GetFolder(folder) )
	{
		SBinary _Id; _Id.cb = sizeof(folder.m_dwID); _Id.lpb = (tBYTE *)&folder.m_dwID;
		m_pFldInfo->szName = folder.m_strName;
		m_pFldInfo->EntryID = _Id;
		return TBase::OnOk();
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////

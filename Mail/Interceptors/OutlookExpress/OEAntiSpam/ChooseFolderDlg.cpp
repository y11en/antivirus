// ChooseFolderDlg.cpp: implementation of the CChooseFolderDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChooseFolderDlg.h"
#include <set>

LRESULT CChooseFolderDlg::OnCancel(WORD, WORD wID, HWND, BOOL&)
{
	EndDialog(wID);
	return S_OK;
}

LRESULT CChooseFolderDlg::OnOK(WORD, WORD wID, HWND, BOOL&)
{
	if (m_hTree)
	{
		HTREEITEM hSelected = (HTREEITEM) ::SendMessage(m_hTree, TVM_GETNEXTITEM, TVGN_CARET, 0);
		if (hSelected)
		{
			TVITEM tvi;
			tvi.mask = TVIF_PARAM;
			tvi.hItem = hSelected;
			
			if (::SendMessage(m_hTree, TVM_GETITEM, 0, (LPARAM) &tvi))
				m_dwChosenFolderID = tvi.lParam;
		}
	}

	EndDialog(wID);
	return S_OK;
}

void CChooseFolderDlg::_FillTreeView()
{
	::SendMessage(m_hTree, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM) m_hTreeImageList);

	COETricks::FOLDERS_LIST arrFolders(m_arrLocalFolders);

	typedef std::map<DWORD, HTREEITEM> CREATED_FOLDERS;
	CREATED_FOLDERS arrCreatedFolders;
	
	while (arrFolders.size() > 0)
	{
		std::set<DWORD> arrDeleted;

		// add all folders that do not have parents in our arrFolders array
		COETricks::FOLDERS_LIST::const_iterator it = arrFolders.begin();
		for (; it != arrFolders.end(); ++it)
		{
			bool bHasChildren = _FolderHasChildren(arrFolders, it->m_dwID);
			bool bHasParent = _FolderExists(arrFolders, it->m_dwParentID);
			
			if (bHasParent)
				continue;	// don't process this node yet

			HTREEITEM hParent = TVI_ROOT;
			CREATED_FOLDERS::iterator itCreated = arrCreatedFolders.find(it->m_dwParentID);
			if (itCreated != arrCreatedFolders.end())
				hParent = itCreated->second;

			HTREEITEM hItem = _AddTreeItem(hParent, it->m_strName, it->m_dwID, bHasChildren, it->m_nImage);

			arrCreatedFolders[it->m_dwID] = hItem;
			arrDeleted.insert(it->m_dwID);

//			ATLTRACE(_T(" -- Added folder '%s' (%d, htree = %d), parent = %d (htree = %d)\n"),
//				it->m_strName.c_str(), it->m_dwID, hItem, it->m_dwParentID, hParent);
		}

		std::set<DWORD>::const_iterator itDeleted = arrDeleted.begin();
		for (; itDeleted != arrDeleted.end(); ++itDeleted)
			_DeleteFolderByID(arrFolders, *itDeleted);

//		ATLTRACE(_T(" -- \n"));
	}

	CREATED_FOLDERS::const_iterator it = arrCreatedFolders.begin();
	for (; it != arrCreatedFolders.end(); ++it)
		::SendMessage(m_hTree, TVM_EXPAND, TVE_EXPAND, (LPARAM) it->second);
}

LRESULT CChooseFolderDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// fill our tree control
	m_hTree = GetDlgItem(IDC_FOLDERS);
	_FillTreeView();
	CenterWindow();

	tstring strCaption = _Module.LoadString("OutlookPlugun.Antispam.Browse", "dlg_caption");
	SetWindowText(strCaption.c_str());
	
	return S_OK;
}

HTREEITEM CChooseFolderDlg::_AddTreeItem(HTREEITEM hParent, const cStrObj& strText, LPARAM lParam, 
										 bool bHasChildren, int nImage)
{
	if (!m_hTree)
		return NULL;

	TVINSERTSTRUCTA tv;
	tv.hParent = hParent;
	tv.hInsertAfter = TVI_LAST;
	tv.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tv.item.lParam = lParam;
	tv.item.cchTextMax = strText.size();
	tv.item.cChildren = bHasChildren ? 1 : 0;
	tv.item.iImage = nImage;
	tv.item.iSelectedImage = nImage;
	
	if( COETricks::m_bAnsi )
	{
		const cStrBuff& strAnsi = strText.c_str(cCP_ANSI);
		tv.item.pszText = (LPSTR)strAnsi.ptr();
		return (HTREEITEM) ::SendMessage(m_hTree, TVM_INSERTITEMA, 0, (LPARAM)&tv);
	}

	tv.item.pszText = (LPSTR)strText.data();
	return (HTREEITEM) ::SendMessage(m_hTree, TVM_INSERTITEMW, 0, (LPARAM)&tv);
}

bool CChooseFolderDlg::_FolderHasChildren(const COETricks::FOLDERS_LIST& arrFolders, DWORD dwFolderID)
{
	COETricks::FOLDERS_LIST::const_iterator it = arrFolders.begin();
	for (; it != arrFolders.end(); ++it)
	{
		if (it->m_dwID == dwFolderID)
			continue;

		if (it->m_dwParentID == dwFolderID)
			return true;
	}

	return false;
}

bool CChooseFolderDlg::_FolderExists(const COETricks::FOLDERS_LIST& arrFolders, DWORD dwFolderID)
{
	COETricks::FOLDERS_LIST::const_iterator it = arrFolders.begin();
	for (; it != arrFolders.end(); ++it)
	{
		if (it->m_dwID == dwFolderID)
			return true;
	}
	
	return false;
}

void CChooseFolderDlg::_DeleteFolderByID(COETricks::FOLDERS_LIST& arrFolders, DWORD dwFolderID)
{
	COETricks::FOLDERS_LIST::iterator it = arrFolders.begin();
	for (; it != arrFolders.end(); ++it)
	{
		if (it->m_dwID == dwFolderID)
		{
			arrFolders.erase(it);
			return;
		}
	}
}


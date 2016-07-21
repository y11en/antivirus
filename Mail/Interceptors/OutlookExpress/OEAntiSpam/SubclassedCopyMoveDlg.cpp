// SubclassedCopyMoveDlg.cpp: implementation of the CSubclassedCopyMoveDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SubclassedCopyMoveDlg.h"
#include "OETricks.h"

void CSubclassedCopyMoveDlg::AfterInitDialog()
{
	HWND hTreeCtrl = _GetTreeCtrl();
	if (!hTreeCtrl)
	{
		// wrong dialog?
		PostMessage(WM_COMMAND, IDCANCEL, 0);
		return;
	}

	HTREEITEM hRoot = TreeView_GetRoot(hTreeCtrl);
	HTREEITEM hLocalFolders = COETricks::m_bVista ? hRoot : TreeView_GetChild(hTreeCtrl, hRoot);
	if (!hLocalFolders)
	{
		PostMessage(WM_COMMAND, IDCANCEL, 0);	// dunno what happened
		return;
	}

	FOLDERS_LIST arrFolders;
	_EnumFolders(hTreeCtrl, hLocalFolders, arrFolders);

	// select our target folder
	// then press OK
	FOLDERS_LIST::const_iterator it = arrFolders.find(m_dwFolderID);
	if (it == arrFolders.end())
	{
		// there's no love...
		PostMessage(WM_COMMAND, IDCANCEL, 0);
		return;
	}

	TreeView_SelectItem(hTreeCtrl, it->second);
	PostMessage(WM_COMMAND, IDOK, 0);
}

HWND CSubclassedCopyMoveDlg::_GetTreeCtrl() const
{
	HWND hThorTree = FindWindowEx(m_hWnd, NULL, _T("ThorTreeViewWndClass"), NULL);
	if (!hThorTree)
		return NULL;

	return FindWindowEx(hThorTree, NULL, _T("SysTreeView32"), NULL);
}

bool CSubclassedCopyMoveDlg::_EnumFolders(HWND hTreeCtrl, HTREEITEM hRootFolder, OUT FOLDERS_LIST& arrFolders)
{
	TreeView_Expand(hTreeCtrl, hRootFolder, TVE_EXPAND); // to prevent the infamous crash 

	HTREEITEM hCurFolder = TreeView_GetChild(hTreeCtrl, hRootFolder);
	if (!hCurFolder)
		return false;

	for (; hCurFolder; hCurFolder = TreeView_GetNextSibling(hTreeCtrl, hCurFolder))
	{
		TCHAR szBuf[256];
		
		TVITEM item;
		item.hItem = hCurFolder;
		item.mask = TVIF_TEXT | TVIF_PARAM;
		item.pszText = szBuf;
		item.cchTextMax = 256;
		TreeView_GetItem(hTreeCtrl, &item);
		
		CTLeafInfo LeafInfo((void *)item.lParam);
		if(LeafInfo.IsNull())
			continue;

		arrFolders[LeafInfo.GetFolderId()] = hCurFolder;
		
		_EnumFolders(hTreeCtrl, hCurFolder, arrFolders);
	}

	return true;
}

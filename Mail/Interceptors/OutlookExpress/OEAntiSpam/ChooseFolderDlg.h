// ChooseFolderDlg.h: interface for the CChooseFolderDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHOOSEFOLDERDLG_H__D50C6A61_D5DD_4E2D_BEDE_9D1E05D05B6D__INCLUDED_)
#define AFX_CHOOSEFOLDERDLG_H__D50C6A61_D5DD_4E2D_BEDE_9D1E05D05B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "OETricks.h"
#include <map>

class CChooseFolderDlg : public CDialogImpl<CChooseFolderDlg>
{
public:
	CChooseFolderDlg(HIMAGELIST hTreeImageList, const COETricks::FOLDERS_LIST& arrFolders) : 
		m_dwChosenFolderID(0),
		m_hTreeImageList(hTreeImageList),
		m_hTree(NULL)
	{
		m_arrLocalFolders = arrFolders;
	}

	DWORD GetChosenFolderID() const
	{
		return m_dwChosenFolderID;
	}

	void SetLocalFolders(const COETricks::FOLDERS_LIST& arrFolders)
	{
		m_arrLocalFolders = arrFolders;
	}

	enum { IDD = IDD_CHOOSEFOLDER };

	BEGIN_MSG_MAP(CChooseFolderDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()
		
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	DWORD	m_dwChosenFolderID;
	HIMAGELIST	m_hTreeImageList;
	COETricks::FOLDERS_LIST	m_arrLocalFolders;

	HWND m_hTree;
	HTREEITEM _AddTreeItem(HTREEITEM hParent, const cStrObj& strText, LPARAM lParam, bool bHasChildren, int nImage);
	void _FillTreeView();

	static bool _FolderHasChildren(const COETricks::FOLDERS_LIST& arrFolders, DWORD dwFolderID);
	static bool _FolderExists(const COETricks::FOLDERS_LIST& arrFolders, DWORD dwFolderID);
	static void _DeleteFolderByID(COETricks::FOLDERS_LIST& arrFolders, DWORD dwFolderID);
};

#endif // !defined(AFX_CHOOSEFOLDERDLG_H__D50C6A61_D5DD_4E2D_BEDE_9D1E05D05B6D__INCLUDED_)

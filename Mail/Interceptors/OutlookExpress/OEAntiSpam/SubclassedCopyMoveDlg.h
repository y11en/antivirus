// SubclassedCopyMoveDlg.h: interface for the CSubclassedCopyMoveDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBCLASSEDCOPYMOVEDLG_H__28216E1E_8C5F_4437_B38A_66C597A39492__INCLUDED_)
#define AFX_SUBCLASSEDCOPYMOVEDLG_H__28216E1E_8C5F_4437_B38A_66C597A39492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SubclassedBase.h"
#include <map>
#include <commctrl.h>

class CSubclassedCopyMoveDlg : public CSubclassedBase
{
public:
	virtual void AfterInitDialog();
	
	void SetTargetFolderID(__int64 dwFolderID)
	{
		m_dwFolderID = dwFolderID;
	}
	
private:
	__int64 m_dwFolderID;	// where to copy/move

	HWND _GetTreeCtrl() const;

	typedef std::map<__int64, HTREEITEM> FOLDERS_LIST;
	bool _EnumFolders(HWND hTreeCtrl, HTREEITEM hRootFolder, OUT FOLDERS_LIST& arrFolders);
};

#endif // !defined(AFX_SUBCLASSEDCOPYMOVEDLG_H__28216E1E_8C5F_4437_B38A_66C597A39492__INCLUDED_)

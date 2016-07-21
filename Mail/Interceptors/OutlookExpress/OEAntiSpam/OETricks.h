// OETricks.h: interface for the COETricks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OETRICKS_H__61071A00_89C3_4D71_8BD9_082B5E013A9D__INCLUDED_)
#define AFX_OETRICKS_H__61071A00_89C3_4D71_8BD9_082B5E013A9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <commctrl.h>

const int OE_WM_PROPERTIES		= 40122;
const int OE_WM_CREATEFOLDER	= 40288;
const int OE_WM_DELETEMESSAGE	= 40434;
const int OE_WM_COPYTOFOLDER	= 40126;
const int OE_WM_MOVETOFOLDER	= 40127;
const int OE_WM_MARKREAD		= 40186;
const int OE_WM_MARKUNREAD		= 40187;
const int OE_WM_SWITCHIDENTITY	= 40117;

const int OE_WM_VIEWSOURCE		= 137;

namespace OETricks
{
	const int OE_LOCALFOLDERS_ID = 1;
	const int OE_INBOX_ID		= 4;
	const int OE_OUTBOX_ID		= 5;
	const int OE_SENTITEMS_ID	= 6;
	const int OE_DELETED_ID		= 7;
	const int OE_DRAFTS_ID		= 8;

	const int OE_INBOX_ID_VISTA		= 3;
	const int OE_OUTBOX_ID_VISTA	= 4;
	const int OE_SENTITEMS_ID_VISTA	= 5;
	const int OE_DELETED_ID_VISTA	= 6;
	const int OE_DRAFTS_ID_VISTA	= 7;
}

class CMainWindow;

class COETricks  
{
public:
	COETricks(CMainWindow* pMainWnd);

	void Init(HWND hMainWnd)
	{
		m_hMainWnd = hMainWnd;
	}

	HWND GetTreeCtrl() const;
	HWND GetMsgListView() const;
	HTREEITEM GetLocalFoldersItem(HWND hTreeCtrl) const;

	bool SetFocusToMessageView() const;
	bool CreateSpamFolders(OPTIONAL bool bResetCounter = true);	// nevermind the parameter
	
	// returns immediately after initiating the message selecting procedure,
	// sends WM_MSGPROP_DONE to the main window when the sending of all messages is complete
	bool SendSelectedMessagesToMC(bool bIsSpam, OUT UINT& nTotalMessages);

	bool SaveSelection();	// selection of msgs in the message view window
	bool RestoreSelection();

	__int64 GetCurrentFolderID() const;

	bool MoveSelectedMessages(__int64 dwMoveTo);
	bool CopySelectedMessages(__int64 dwCopyTo);
	bool DeleteSelectedMessages();
	bool MarkSelectedRead();
	bool MarkSelectedUnread();
	
	struct TFolder
	{
		cStrObj	m_strName;
		__int64	m_dwID;
		__int64	m_dwParentID;
		int		m_nImage;
	};

	typedef std::vector<TFolder> FOLDERS_LIST;

	bool GetLocalFolders(OUT FOLDERS_LIST& arrFolders);
	bool GetLocalFolders(HWND hTreeCtrl, OUT FOLDERS_LIST& arrFolders);
	HIMAGELIST GetTreeImageList() const;

	// returns 0 if the folder was not found
	bool GetSpamFoldersIDfromInbox(OUT __int64& dwSpamFolderID, OUT __int64& dwProbSpamFolderID);

	static int GetInboxId()
		{ return m_bVista ? OETricks::OE_INBOX_ID_VISTA : OETricks::OE_INBOX_ID; }
	static int GetOutboxId()
		{ return m_bVista ? OETricks::OE_OUTBOX_ID_VISTA : OETricks::OE_OUTBOX_ID; }
	static int GetDeletedId()
	{ return m_bVista ? OETricks::OE_DELETED_ID_VISTA : OETricks::OE_DELETED_ID; }

public:
	static bool m_bAnsi;
	static bool m_bVista;
	
private:
	CMainWindow* m_pMainWnd;
	HWND m_hMainWnd;
	mutable HWND m_hThorTreeCtrl;

	bool UnwindFolder(HWND hTreeCtrl, HTREEITEM hFolder, OUT FOLDERS_LIST& arrFolders);

	typedef std::vector<int> STORED_ITEMS;
	STORED_ITEMS m_arrStoredItems;
};

struct TLeafInfo
{
	__int3264 m_dwUnk1;
	__int3264 m_dwUnk2;
	__int3264 m_dwFolderID;
	__int3264 m_dwParentID;
};

struct TLeafInfoVista
{
	__int3264 m_dwUnk1;
	__int3264 m_dwUnk2;
	__int64 m_dwFolderID;
	__int64 m_dwParentID;
};

class CTLeafInfo
{
	void *m_p;
public:
	CTLeafInfo(void *p) : m_p(p)
	{
	}
	__int64 GetFolderId() const
	{
		return m_p ? (COETricks::m_bVista ? ((TLeafInfoVista *)m_p)->m_dwFolderID :
						((TLeafInfo *)m_p)->m_dwFolderID) : 0;
	}
	__int64 GetParentId() const
	{
		return m_p ? (COETricks::m_bVista ? ((TLeafInfoVista *)m_p)->m_dwParentID :
			((TLeafInfo *)m_p)->m_dwParentID) : 0;
	}
	bool IsNull() const
	{
		return m_p == NULL;
	}
};

#endif // !defined(AFX_OETRICKS_H__61071A00_89C3_4D71_8BD9_082B5E013A9D__INCLUDED_)

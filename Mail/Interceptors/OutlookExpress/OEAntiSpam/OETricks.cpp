// OETricks.cpp: implementation of the COETricks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OETricks.h"
#include "resource.h"
#include "WndInterceptor.h"
#include "MainWindow.h"
#include "Tracing.h"
#include "Utility.h"

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

bool COETricks::m_bAnsi = false;
bool COETricks::m_bVista = false;

COETricks::COETricks(CMainWindow* pMainWnd) :
	m_hMainWnd(NULL), m_hThorTreeCtrl(NULL), m_pMainWnd(pMainWnd)
{
	OSVERSIONINFO osVer; memset(&osVer, 0, sizeof(osVer));
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if( ::GetVersionEx((OSVERSIONINFO *)&osVer) )
	{
		m_bAnsi = osVer.dwPlatformId != VER_PLATFORM_WIN32_NT;
		m_bVista = osVer.dwMajorVersion > 5;
	}
}

HWND COETricks::GetTreeCtrl() const
{
	// didn't find it yet, look again
	HWND hTopFrame = FindWindowEx(m_hMainWnd, NULL, _T("Outlook Express Navigation Pane"), NULL);
	if (!hTopFrame)
		return NULL;
	
	HWND hChildFrame;
	if (m_bVista)
		hChildFrame = hTopFrame;
	else
		hChildFrame = FindWindowEx(hTopFrame, NULL, _T("Outlook Express Pane Frame"), NULL);

	for (; hChildFrame != NULL; hChildFrame = FindWindowEx(hTopFrame, hChildFrame, _T("Outlook Express Pane Frame"), NULL))
	{
		HWND hThorTree = FindWindowEx(hChildFrame, NULL, _T("ThorTreeViewWndClass"), NULL);
		if (!hThorTree)
			continue;
		
		HWND hTreeCtrl = FindWindowEx(hThorTree, NULL, _T("SysTreeView32"), NULL);
		if (!hTreeCtrl)
			continue;

		// found it!
		m_hThorTreeCtrl = hTreeCtrl;
		break;	// no more ThorTree windows
	}
	
	return m_hThorTreeCtrl;
}

HTREEITEM COETricks::GetLocalFoldersItem(HWND hTreeCtrl) const
{
	HTREEITEM hRoot = TreeView_GetRoot(hTreeCtrl);
	if (!hRoot)
		return NULL;

	if (m_bVista)
		return hRoot;

	return TreeView_GetChild(hTreeCtrl, hRoot);	// Local Folders
}

bool COETricks::CreateSpamFolders(bool bResetCounter)
{
	// it should go like this:
	// 1. Select the Inbox folder (id = 4)
	// 2. Iteratively:
	//    a) see if a folder is already created
	//    b) call the 'Create folder' window if there's no folder - PostMessage(WM_COMMAND, OE_WM_CREATEFOLDER, NULL)
	//    c) remember the id of newly created folder
	
	// see if we need to create the folders
	ASTRACE(_T("Creating spam folders"));

	__int64 dwSpamFolderID = 0;
	__int64 dwProbSpamFolderID = 0;
	GetSpamFoldersIDfromInbox(dwSpamFolderID, dwProbSpamFolderID);
	bool bSpamFolderPresent = dwSpamFolderID ? true : false;
	bool bProbSpamFolderPresent = dwProbSpamFolderID ? true : false;

	if (bSpamFolderPresent && bProbSpamFolderPresent)
	{
		::PostMessage(m_hMainWnd, WM_CREATEFOLDER_DONE, NULL, NULL);
		return true;	// the folders are already there
	}

	HWND hTreeCtrl = GetTreeCtrl();
	if (!hTreeCtrl)
		return false;

	HTREEITEM hLocalFolders = GetLocalFoldersItem(hTreeCtrl);	// Local Folders
	if (!hLocalFolders)
		return false;

	HTREEITEM hInbox = TreeView_GetChild(hTreeCtrl, hLocalFolders);
	if (!hInbox)
		return false;

	while (hInbox)
	{
		// check if this is an Inbox
		TVITEM item;
		item.hItem = hInbox;
		item.mask = TVIF_PARAM;
		TreeView_GetItem(hTreeCtrl, &item);
		CTLeafInfo LeafInfo((void *)item.lParam);
		if(!LeafInfo.IsNull() && LeafInfo.GetFolderId() == GetInboxId())
			break;

		hInbox = TreeView_GetNextSibling(hTreeCtrl, hInbox);
	}

	TreeView_Expand(hTreeCtrl, hLocalFolders, TVE_EXPAND); // to prevent WinMe + IE5.5 crash 

	if (hInbox)
		TreeView_SelectItem(hTreeCtrl, hInbox);

	// before initiating folders creation, make the necessary preparations
	if (!bSpamFolderPresent || !bProbSpamFolderPresent)
	{
		CWndInterceptor& interceptor = CWndInterceptor::GetInstance();
		if (bResetCounter)
			interceptor.SetCreateFolderCounter(1);

		if (!bSpamFolderPresent)
		{
			tstring strSpamFolder = _Module.LoadString(IDS_SPAMFOLDER);
			interceptor.AddCreateFolderTask(strSpamFolder.c_str());
		}

		if (!bProbSpamFolderPresent)
		{
			tstring strProbableSpamFolder = _Module.LoadString(IDS_PROBSPAMFOLDER);
			interceptor.AddCreateFolderTask(strProbableSpamFolder.c_str());
		}

		::PostMessage(m_hMainWnd, WM_COMMAND, OE_WM_CREATEFOLDER, NULL);
	}

	return true;
}

HIMAGELIST COETricks::GetTreeImageList() const
{
	HWND hTreeCtrl = GetTreeCtrl();
	if (!hTreeCtrl)
		return NULL;

	return (HIMAGELIST) ::SendMessage(hTreeCtrl, TVM_GETIMAGELIST, TVSIL_NORMAL, 0);
}

bool COETricks::GetLocalFolders(OUT FOLDERS_LIST& arrFolders)
{
	HWND hTreeCtrl = GetTreeCtrl();
	if (!hTreeCtrl)
		return false;

	return GetLocalFolders(hTreeCtrl, arrFolders);
}

bool COETricks::GetLocalFolders(HWND hTreeCtrl, OUT FOLDERS_LIST& arrFolders)
{
	HTREEITEM hLocalFolders = GetLocalFoldersItem(hTreeCtrl);	// Local Folders
	if (!hLocalFolders)
		return false;
	
	HTREEITEM hCurFolder = TreeView_GetChild(hTreeCtrl, hLocalFolders);
	if (!hCurFolder)
		return false;
	
	for (; hCurFolder; hCurFolder = TreeView_GetNextSibling(hTreeCtrl, hCurFolder))
	{
		{
			CHAR szBuf[512];
			
			TVITEMA item;
			item.hItem = hCurFolder;
			item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
			item.pszText = szBuf;
			item.cchTextMax = sizeof(szBuf)/2;
			
			if( m_bAnsi ) SNDMSG(hTreeCtrl, TVM_GETITEMA, 0, (LPARAM)(TV_ITEMA FAR *)&item);
			else          SNDMSG(hTreeCtrl, TVM_GETITEMW, 0, (LPARAM)(TV_ITEMA FAR *)&item);
			
			CTLeafInfo LeafInfo((void *)item.lParam);
			if (LeafInfo.IsNull() || (LeafInfo.GetFolderId() == GetOutboxId()))
				continue;
			
			TFolder folder;
			folder.m_strName.assign(szBuf, m_bAnsi ? cCP_ANSI : cCP_UNICODE);
			folder.m_dwID = LeafInfo.GetFolderId();
			folder.m_dwParentID = LeafInfo.GetParentId();
			folder.m_nImage = item.iImage;
			arrFolders.push_back(folder);
		}
		
		UnwindFolder(hTreeCtrl, hCurFolder, arrFolders);
	}
	
	return true;
}

bool COETricks::UnwindFolder(HWND hTreeCtrl, HTREEITEM hFolder, OUT FOLDERS_LIST& arrFolders)
{
	HTREEITEM hCurFolder = TreeView_GetChild(hTreeCtrl, hFolder);
	if (!hCurFolder)
		return false;
	
	for (; hCurFolder; hCurFolder = TreeView_GetNextSibling(hTreeCtrl, hCurFolder))
	{
		{
			CHAR szBuf[512];
			
			TVITEMA item;
			item.hItem = hCurFolder;
			item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
			item.pszText = szBuf;
			item.cchTextMax = sizeof(szBuf)/2;
			
			if( m_bAnsi ) SNDMSG(hTreeCtrl, TVM_GETITEMA, 0, (LPARAM)(TV_ITEMA FAR *)&item);
			else          SNDMSG(hTreeCtrl, TVM_GETITEMW, 0, (LPARAM)(TV_ITEMA FAR *)&item);
			
			CTLeafInfo LeafInfo((void *)item.lParam);
			if(LeafInfo.IsNull() || (LeafInfo.GetFolderId() == GetDeletedId()))
				continue;
			
			TFolder folder;
			folder.m_strName.assign(szBuf, m_bAnsi ? cCP_ANSI : cCP_UNICODE);
			folder.m_dwID = LeafInfo.GetFolderId();
			folder.m_dwParentID = LeafInfo.GetParentId();
			folder.m_nImage = item.iImage;
			arrFolders.push_back(folder);
		}
		
		UnwindFolder(hTreeCtrl, hCurFolder, arrFolders);
	}

	return true;
}

bool COETricks::SetFocusToMessageView() const
{
	HWND hMsgView;
	if (m_bVista)
		hMsgView = GetMsgListView();
	else
		hMsgView = FindWindowEx(m_hMainWnd, NULL, TEXT("Outlook Express Message View"), NULL);
	::SetFocus(hMsgView);
	return true;
}

bool COETricks::GetSpamFoldersIDfromInbox(__int64& dwSpamFolderID, __int64& dwProbSpamFolderID)
{
	FOLDERS_LIST arrLocalFolders;
	GetLocalFolders(arrLocalFolders);

	dwSpamFolderID = 0;
	dwProbSpamFolderID = 0;

	// look for the Spam and Probable Spam folders
	tstring strSpamFolder = _Module.LoadString(IDS_SPAMFOLDER);
	tstring strProbableSpamFolder = _Module.LoadString(IDS_PROBSPAMFOLDER);
	
	// make folder names uppercase
	Utility::MakeStrUpper(strSpamFolder);
	Utility::MakeStrUpper(strProbableSpamFolder);
	
	FOLDERS_LIST::const_iterator it = arrLocalFolders.begin();
	for (; it != arrLocalFolders.end(); ++it)
	{
		tstring strName(it->m_strName.c_str(cCP_ANSI));
		Utility::MakeStrUpper(strName);
		
		if ((strName == strSpamFolder) && (it->m_dwParentID == GetInboxId()))
			dwSpamFolderID = it->m_dwID;
		else if ((strName == strProbableSpamFolder) && (it->m_dwParentID == GetInboxId()))
			dwProbSpamFolderID = it->m_dwID;
	}

	USES_CONVERSION;
	tstring msg("SpamFolderID = ");
	msg += A2CT((boost::lexical_cast<std::string>(dwSpamFolderID)).c_str());
	msg += _T(", ProbSpamFolderID = ");
	msg += A2CT((boost::lexical_cast<std::string>(dwProbSpamFolderID)).c_str());
	ASTRACE(msg.c_str());

	return true;
}

HWND COETricks::GetMsgListView() const
{
	HWND hMsgView = FindWindowEx(m_hMainWnd, NULL, _T("Outlook Express Message View"), NULL);
	if (!hMsgView)
		return NULL;
	
	HWND hMsgList = FindWindowEx(hMsgView, NULL, _T("Outlook Express Message List"), NULL);
	if (!hMsgList)
		return NULL;
	
	if (m_bVista)
		return FindWindowEx(hMsgList, NULL, _T("SysListView32"), NULL);

	return FindWindowEx(hMsgList, NULL, _T("ATL:SysListView32"), NULL);
}

bool COETricks::SendSelectedMessagesToMC(bool bIsSpam, UINT& nTotalMessages)
{
	SetFocusToMessageView();
	SaveSelection();

	CWndInterceptor& interceptor = CWndInterceptor::GetInstance();
	// 1. Get message view
	// 2. Enumerate selected items
	HWND hListView = GetMsgListView();
	if (!hListView)
		return false;

	int nItem = (int)::SendMessage(hListView, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED, 0));
	int nFirstItem = nItem;

	interceptor.ClearMsgPropTaskArray();
	nTotalMessages = 0;
	for (; nItem >= 0; nItem = (int)::SendMessage(hListView, LVM_GETNEXTITEM, nItem, MAKELPARAM(LVNI_SELECTED, 0)))
	{
		++nTotalMessages;
		interceptor.AddMsgPropTask(bIsSpam, hListView, nItem);
	}
	
	if (nFirstItem >= 0)
	{
		// there are selected items
		// remove items selection (for now)
		LVITEM item;
		item.iItem = -1;
		item.iSubItem = 0;
		item.mask = LVIF_STATE;
		item.state = 0;
		item.stateMask = LVIS_SELECTED;
		::SendMessage(hListView, LVM_SETITEMSTATE, -1, (LPARAM) &item);

		// select the first item and initiate the MsgProp sequence
		item.iItem = nFirstItem;
		item.state = LVIS_SELECTED;
		::SendMessage(hListView, LVM_SETITEMSTATE, nFirstItem, (LPARAM) &item);

		::PostMessage(m_hMainWnd, WM_COMMAND, OE_WM_PROPERTIES, NULL);
	}

	return true;
}

bool COETricks::SaveSelection()
{
	HWND hListView = GetMsgListView();
	if (!hListView)
		return false;

	m_arrStoredItems.swap(STORED_ITEMS());
	int nItem = (int)::SendMessage(hListView, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED, 0));
	for (; nItem >= 0; nItem = (int)::SendMessage(hListView, LVM_GETNEXTITEM, nItem, MAKELPARAM(LVNI_SELECTED, 0)))
		m_arrStoredItems.push_back(nItem);

	return true;
}

bool COETricks::RestoreSelection()
{
	HWND hListView = GetMsgListView();
	if (!hListView)
		return false;
	
	STORED_ITEMS::const_iterator it = m_arrStoredItems.begin();
	for (; it != m_arrStoredItems.end(); ++it)
	{
		LVITEM item;
		item.iItem = *it;
		item.iSubItem = 0;
		item.mask = LVIF_STATE;
		item.state = LVIS_SELECTED;
		item.stateMask = LVIS_SELECTED;
		::SendMessage(hListView, LVM_SETITEMSTATE, *it, (LPARAM) &item);
	}
	
	return true;
}

__int64 COETricks::GetCurrentFolderID() const
{
	HWND hTreeCtrl = GetTreeCtrl();
	if (!hTreeCtrl)
		return 0;
	
	HTREEITEM hLocalFolders = GetLocalFoldersItem(hTreeCtrl);	// Local Folders
	if (!hLocalFolders)
		return 0;
	
	HTREEITEM hSelected = (HTREEITEM) ::SendMessage(hTreeCtrl, TVM_GETNEXTITEM, TVGN_CARET, 0);
	if (!hSelected)
		return 0;

	TVITEM item;
	item.hItem = hSelected;
	item.mask = TVIF_PARAM;
	TreeView_GetItem(hTreeCtrl, &item);
	
	CTLeafInfo LeafInfo((void *)item.lParam);
	if (LeafInfo.IsNull())
		return 0;

	return LeafInfo.GetFolderId();
}

bool COETricks::MoveSelectedMessages(__int64 dwMoveTo)
{
	USES_CONVERSION;
	tstring msg(_T("Moving selected messages to folder "));
	msg += A2CT((boost::lexical_cast<std::string>(dwMoveTo)).c_str());
	ASTRACE(msg.c_str());

	CWndInterceptor& interceptor = CWndInterceptor::GetInstance();
	interceptor.AddCopyMoveMsgTask(dwMoveTo);

	if (m_pMainWnd)
		m_pMainWnd->PrepareToCopyMoveMessages();

	::PostMessage(m_hMainWnd, WM_COMMAND, OE_WM_MOVETOFOLDER, 0);
	return true;
}

bool COETricks::CopySelectedMessages(__int64 dwCopyTo)
{
	USES_CONVERSION;
	tstring msg(_T("Copying selected messages to folder "));
	msg += A2CT((boost::lexical_cast<std::string>(dwCopyTo)).c_str());
	ASTRACE(msg.c_str());

	CWndInterceptor& interceptor = CWndInterceptor::GetInstance();
	interceptor.AddCopyMoveMsgTask(dwCopyTo);

	if (m_pMainWnd)
		m_pMainWnd->PrepareToCopyMoveMessages();

	::PostMessage(m_hMainWnd, WM_COMMAND, OE_WM_COPYTOFOLDER, 0);
	return true;
}

bool COETricks::DeleteSelectedMessages()
{
	ASTRACE(_T("Deleting selected messages"));
	::PostMessage(m_hMainWnd, WM_COMMAND, OE_WM_DELETEMESSAGE, 0);
	return true;
}

bool COETricks::MarkSelectedRead()
{
	ASTRACE(_T("Marking selected messages as read"));
	::PostMessage(m_hMainWnd, WM_COMMAND, OE_WM_MARKREAD, 0);
	return true;
}

bool COETricks::MarkSelectedUnread()
{
	ASTRACE(_T("Marking selected messages as unread"));
	::PostMessage(m_hMainWnd, WM_COMMAND, OE_WM_MARKUNREAD, 0);
	return true;
}

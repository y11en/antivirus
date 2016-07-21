// WndInterceptor.cpp: implementation of the CWndInterceptor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WndInterceptor.h"
#include "OETricks.h"
#include "SubclassedBase.h"
#include "MainWindow.h"
#include "Tracing.h"

CSWMRG CWndInterceptor::m_InstGuard;
CWndInterceptor* CWndInterceptor::m_pInstance = NULL;

HHOOK CWndInterceptor::m_hLocalHook = NULL;
bool CWndInterceptor::m_bWindowWasSubclassed = false;
CSubclassedBase* CWndInterceptor::m_pSubclass = NULL;

CSubclassedCreateDlg CWndInterceptor::m_CreateDlg;
CSubclassedMsgPropDlg CWndInterceptor::m_MsgPropDlg;
CSubclassedCopyMoveDlg CWndInterceptor::m_CopyMoveDlg;

CWndInterceptor::CWndInterceptor() :
	m_nCreateFolderCounter(0)
{
}

CWndInterceptor::~CWndInterceptor()
{
}

CWndInterceptor& CWndInterceptor::GetInstance()
{
	if (!m_pInstance)
	{
		CAutoLockWrite locked(m_InstGuard);

		// double-checking technique to prevent multi-creation of the singleton (from different threads)
		if (!m_pInstance)  
		{
			CWndInterceptor* pInterceptor = new CWndInterceptor;
			m_pInstance = pInterceptor;
		}
	}

	return *m_pInstance;
}

void CWndInterceptor::Destroy()
{
	if (m_pInstance)
	{
		CAutoLockWrite locker(m_InstGuard);
		
		// double-checking technique to prevent multi-destruction of the singleton (from different threads)
		if (m_pInstance)  
		{
			CWndInterceptor* pMain = m_pInstance;
			m_pInstance = NULL;
			delete pMain;
		}
	}
}

void CWndInterceptor::_ClearLocalHook()
{
	if (m_hLocalHook)
		UnhookWindowsHookEx(m_hLocalHook);
	
	m_hLocalHook = NULL;
	m_bWindowWasSubclassed = false;
}

bool CWndInterceptor::AddCreateFolderTask(LPCTSTR szFolderName)
{
	CAutoLockWrite locker(m_accessor);
	m_arrCreateFolderTasks.push_back(szFolderName);
	return true;
}

void CWndInterceptor::BeforeCreateFolderDialog(HWND hMainWnd)
{
	ASTRACE(_T("Before create folder dialog"));

	{
		// the WM_CREATEFOLDER message is for some reason being sent twice for the first dialog
		// so we should skip the first message and handle the second (only for the first time!)
		// hence the need of m_nCreateFolderCounter variable
		CAutoLockRead locker(m_accessor);
		if ((m_arrCreateFolderTasks.size() <= 0) || (m_nCreateFolderCounter == 0))
			return;
	}

	// start the handler
	CAutoLockWrite locker(m_accessor);
	tstring strFolder = m_arrCreateFolderTasks.front();
	m_arrCreateFolderTasks.pop_front();

	// set the local hook on windows creation (only for this thread)
	m_CreateDlg.SetDesiredFolderName(strFolder.c_str());
	m_pSubclass = &m_CreateDlg;
	m_hLocalHook = SetWindowsHookEx(WH_CBT, CWndInterceptor::LocalCBTProc, NULL, GetCurrentThreadId());
}

void CWndInterceptor::AfterCreateFolderDialog(HWND hMainWnd)
{
	ASTRACE(_T("After create folder dialog"));

	CAutoLockWrite locker(m_accessor);
	if (m_nCreateFolderCounter == 0)
	{
		// we're just starting the counter, now all messages will be processed
		m_nCreateFolderCounter = 1;
		return;	// it's the very first message
	}

	_ClearLocalHook();

	// finish the handler (start another?)
	if (m_arrCreateFolderTasks.size() > 0)
		::PostMessage(hMainWnd, WM_COMMAND, OE_WM_CREATEFOLDER, NULL);
	else
		::PostMessage(hMainWnd, WM_CREATEFOLDER_DONE, NULL, NULL);
}

LRESULT CALLBACK CWndInterceptor::LocalCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	switch (nCode)
	{
	case HCBT_CREATEWND:
		{
			HWND hWnd = reinterpret_cast<HWND>(wParam);
			TCHAR szClass[256] = {0};
			GetClassName(hWnd, szClass, sizeof(szClass) - 1);
			if (*szClass == _T('\0'))
				return CallNextHookEx(m_hLocalHook, nCode, wParam, lParam);
			
			tstring strClassName(szClass);
			if (strClassName != _T("#32770"))	// handle only dialogs
				return CallNextHookEx(m_hLocalHook, nCode, wParam, lParam);

			if (m_bWindowWasSubclassed)
			{
				// this is some unexpected dialog (most probably a messagebox)
				// close it nahren
				// ASTRACE(_T("LocalCBT: Unexpected dialog"));
				::PostMessage(hWnd, WM_COMMAND, IDCANCEL, NULL);
				return CallNextHookEx(m_hLocalHook, nCode, wParam, lParam);
			}

			// subclass this wnd
			m_bWindowWasSubclassed = true;
			if (m_pSubclass)
				m_pSubclass->SubclassWindow(hWnd);
		}
		break;
	}
	
	return CallNextHookEx(m_hLocalHook, nCode, wParam, lParam);
}

bool CWndInterceptor::AddMsgPropTask(bool bIsSpam, HWND hListView, int nItemIndex)
{
	CAutoLockWrite locker(m_accessor);
	m_arrMsgPropTasks.push_back(nItemIndex);
	m_hMsgListView = hListView;
	m_bIsSpam = bIsSpam;
	return true;
}

void CWndInterceptor::ClearMsgPropTaskArray()
{
	CAutoLockWrite locker(m_accessor);
	m_arrMsgPropTasks.clear();
}

void CWndInterceptor::BeforeMsgPropDialog(HWND hMainWnd)
{
	ASTRACE(_T("Before MessageProp dialog"));

	{
		CAutoLockRead locker(m_accessor);
		if (m_arrMsgPropTasks.size() <= 0)
			return;	// no MsgProp tasks
	}

	// start the handler
	CAutoLockWrite locker(m_accessor);
	m_arrMsgPropTasks.pop_front();
	
	// set the local hook on windows creation (only for this thread)
	m_MsgPropDlg.SetIsSpam(m_bIsSpam);
	m_pSubclass = &m_MsgPropDlg;
	m_hLocalHook = SetWindowsHookEx(WH_CBT, CWndInterceptor::LocalCBTProc, NULL, GetCurrentThreadId());
}

void CWndInterceptor::AfterMsgPropDialog(HWND hMainWnd, HANDLE hStopEvent)
{
	ASTRACE(_T("After MessageProp dialog"));
	ASTRACE(_T(""));

	CAutoLockWrite locker(m_accessor);
	_ClearLocalHook();

	bool bWasCancelled = (WaitForSingleObject(hStopEvent, 0) == WAIT_OBJECT_0);
	if ((m_arrMsgPropTasks.size() <= 0) || bWasCancelled)
	{
		::PostMessage(hMainWnd, WM_MSGPROP_DONE, m_bIsSpam ? TRUE : FALSE, bWasCancelled ? TRUE : FALSE);
		return;	// no more messages or user aborted the processing
	}
	
	// select the next item in listview
	int nItemIndex = m_arrMsgPropTasks.front();
	
	// remove items selection (for now)
	LVITEM item;
	item.iItem = -1;
	item.iSubItem = 0;
	item.mask = LVIF_STATE;
	item.state = 0;
	item.stateMask = LVIS_SELECTED;
	::SendMessage(m_hMsgListView, LVM_SETITEMSTATE, -1, (LPARAM) &item);

	item.iItem = nItemIndex;
	item.state = LVIS_SELECTED;
	::SendMessage(m_hMsgListView, LVM_SETITEMSTATE, nItemIndex, (LPARAM) &item);

	// ignite!
	::PostMessage(hMainWnd, WM_COMMAND, OE_WM_PROPERTIES, NULL);
}

bool CWndInterceptor::AddCopyMoveMsgTask(__int64 dwFolderID)
{
	m_CopyMoveDlg.SetTargetFolderID(dwFolderID);
	return true;
}

void CWndInterceptor::BeforeCopyMoveDialog(HWND hMainWnd)
{
	ASTRACE(_T("Before CopyMove dialog"));

	// set the local hook on windows creation (only for this thread)
	CAutoLockWrite locker(m_accessor);
	m_pSubclass = &m_CopyMoveDlg;
	m_hLocalHook = SetWindowsHookEx(WH_CBT, CWndInterceptor::LocalCBTProc, NULL, GetCurrentThreadId());
}

void CWndInterceptor::AfterCopyMoveDialog(HWND hMainWnd)
{
	ASTRACE(_T("After CopyMove dialog"));

	CAutoLockWrite locker(m_accessor);
	_ClearLocalHook();
}

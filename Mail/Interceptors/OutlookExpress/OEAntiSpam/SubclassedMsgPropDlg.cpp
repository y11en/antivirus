// SubclassedMsgPropDlg.cpp: implementation of the CSubclassedMsgPropDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SubclassedMsgPropDlg.h"
#include "OETricks.h"
#include "Tracing.h"

namespace
{
	DWORD TIMER_ID = 1234;
}

CSubclassedMsgSourceDlg CSubclassedMsgPropDlg::m_SourceDlg;
HHOOK CSubclassedMsgPropDlg::m_hLocalHook;

void CSubclassedMsgPropDlg::AfterInitDialog()
{
	if (!SendMessage(PSM_SETCURSEL, 1, NULL))
	{
		PostMessage(WM_COMMAND, IDOK, 0);
		return;
	}

	// prepare for the message source dialog
	// set the local hook that will wait for the Message Source dialog to appear
	// then wait for that dialog to close
	// when it is closed, close this "Message Properties" dialog
	m_hSyncEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_SourceDlg.SetFinishedEvent(m_hSyncEvent);
	m_SourceDlg.SetIsSpam(m_bIsSpam);

	m_hLocalHook = SetWindowsHookEx(WH_CBT, CSubclassedMsgPropDlg::LocalCBTProc, NULL, GetCurrentThreadId());

	SetTimer(TIMER_ID, 10);
	PostMessage(WM_COMMAND, OE_WM_VIEWSOURCE, NULL);
}

void CSubclassedMsgPropDlg::BeforeDestroy()
{
	if (m_hLocalHook)
		UnhookWindowsHookEx(m_hLocalHook);
}

LRESULT CALLBACK CSubclassedMsgPropDlg::LocalCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
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
			
			// subclass this wnd
			m_SourceDlg.SubclassWindow(hWnd);
		}
		break;
	}
	
	return CallNextHookEx(m_hLocalHook, nCode, wParam, lParam);
}

void CSubclassedMsgPropDlg::BeforeTimer(UINT nTimerID)
{
	if (nTimerID == TIMER_ID)
	{
		DWORD dwRes = WaitForSingleObject(m_hSyncEvent, 0);
		if (dwRes == WAIT_OBJECT_0)
			PostMessage(WM_COMMAND, IDCANCEL, 0);
	}
}


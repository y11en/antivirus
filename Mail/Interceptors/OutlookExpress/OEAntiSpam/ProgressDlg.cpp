// ProgressDlg.cpp: implementation of the CProgressDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProgressDlg.h"
#include <process.h>
#include <commctrl.h>

CProgressDlg::CProgressDlg() :
	m_nTotalMessages(0),
	m_nCurrentMessages(0)
{
	m_hStartedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hMustStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

LRESULT CProgressDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetEvent(m_hMustStopEvent);
	EndDialog(IDCANCEL);
	return S_OK;
}

LRESULT CProgressDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hProgress = GetDlgItem(IDC_PROGRESS);
	if (hProgress)
		::SendMessage(hProgress, PBM_SETRANGE32, (WPARAM) 0, (LPARAM) m_nTotalMessages);

	HWND hText = GetDlgItem(IDC_PROGRESS_TEXT);
	if (hText)
		::SendMessage(hText, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(_T("0 / 0")));

	hText = GetDlgItem(IDC_TEXT_PROCESSING);
	if (hText)
	{
		tstring strText = _Module.LoadString("OutlookPlugun.Antispam.Browse", 
			m_bIsSpam ? "progress_spam" : "progress_notspam");

		::SendMessage(hText, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(strText.c_str()));
	}
	
	hText = GetDlgItem(IDCANCEL);
	if (hText)
	{
		tstring strText = _Module.LoadString("Global", "Cancel");
		if (strText.length() > 0)
			::SendMessage(hText, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(strText.c_str()));
	}
	
	CenterWindow();
	SetEvent(m_hStartedEvent);
	return S_OK;
}

HWND CProgressDlg::CreateDialogInThread(bool bIsSpam, HWND hParent)
{
	m_bIsSpam = bIsSpam;
	m_nTotalMessages = m_nCurrentMessages = 0;

	ResetEvent(m_hStartedEvent);
	ResetEvent(m_hMustStopEvent);

	m_hParent = hParent;

	UINT nDummy;
	CloseHandle(reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, DialogThread, this, 0, &nDummy)));
	WaitForSingleObject(m_hStartedEvent, 5000);
	return m_hWnd;
}

void CProgressDlg::IncrementMessages()
{
	++m_nCurrentMessages;
	if (IsWindow())
		PostMessage(WM_REFRESH_PROGRESS, 0, 0);
}

void CProgressDlg::SetTotalMessages(UINT nTotalMessages)
{
	m_nTotalMessages = nTotalMessages;
	if (IsWindow())
		PostMessage(WM_REFRESH_PROGRESS, 0, 0);
}

LRESULT CProgressDlg::OnRefreshProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hProgress = GetDlgItem(IDC_PROGRESS);
	if (hProgress)
	{
		::SendMessage(hProgress, PBM_SETRANGE32, (WPARAM) 0, (LPARAM) m_nTotalMessages);
		::SendMessage(hProgress, PBM_SETPOS, m_nCurrentMessages, 0);
		
		HWND hText = GetDlgItem(IDC_PROGRESS_TEXT);
		if (hText)
		{
			TCHAR buf[100];
			_sntprintf(buf, 100, _T("%d / %d"), m_nCurrentMessages, m_nTotalMessages);
			::SendMessage(hText, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(buf));
		}
	}
	
	return S_OK;
}

UINT WINAPI CProgressDlg::DialogThread(LPVOID pParam)
{
	CProgressDlg* pThis = reinterpret_cast<CProgressDlg*>(pParam);
	if (!pThis)
		return 1;

	pThis->DoModal(GetDesktopWindow());
	return 233;
}

LRESULT CProgressDlg::OnShouldClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return S_OK;
}

void CProgressDlg::CloseDialog()
{
	if (IsWindow())
		PostMessage(WM_SHOULD_CLOSE, 0, 0);
}

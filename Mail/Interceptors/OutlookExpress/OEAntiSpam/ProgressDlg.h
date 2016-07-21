// ProgressDlg.h: interface for the CProgressDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESSDLG_H__DC692819_A144_4F5F_A6E1_6F046D64394C__INCLUDED_)
#define AFX_PROGRESSDLG_H__DC692819_A144_4F5F_A6E1_6F046D64394C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "../../TheBatAntispam/EnsureCleanup.h"

namespace
{
	const int WM_REFRESH_PROGRESS = WM_APP + 20;
	const int WM_SHOULD_CLOSE = WM_APP + 21;
}

class CDummyDlg : public CDialogImpl<CDummyDlg>
{
public:
	enum { IDD = IDD_DLG_DUMMY };

	BEGIN_MSG_MAP(CDummyDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();
		return S_OK;
	}
};

class CProgressDlg : public CDialogImpl<CProgressDlg>
{
public:
	CProgressDlg();

	HWND CreateDialogInThread(bool bIsSpam, HWND hParent);
	void CloseDialog();	// and end the thread
	void SetTotalMessages(UINT nTotalMessages);
	void IncrementMessages();

	HANDLE GetMustStopEvent()
	{
		return m_hMustStopEvent;
	}

	enum { IDD = IDD_DLG_PROGRESS };

	BEGIN_MSG_MAP(CProgressDlg)
		MESSAGE_HANDLER(WM_REFRESH_PROGRESS, OnRefreshProgress)
		MESSAGE_HANDLER(WM_SHOULD_CLOSE, OnShouldClose)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

protected:
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRefreshProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShouldClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CEnsureCloseHandle m_hStartedEvent;
	CEnsureCloseHandle m_hMustStopEvent;	// OnCancel() must set this event

	UINT m_nTotalMessages;
	UINT m_nCurrentMessages;
	HWND m_hParent;
	bool m_bIsSpam;

	static UINT WINAPI DialogThread(LPVOID pParam);
};

#endif // !defined(AFX_PROGRESSDLG_H__DC692819_A144_4F5F_A6E1_6F046D64394C__INCLUDED_)

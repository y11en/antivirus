#pragma once

#include <windows.h>

extern LANGID g_LangID;
#define NN_LANG_ID g_LangID
#define NN_LANG_R
#include <common\ResourceLang.h>

#include <atlbase.h>

extern CComModule _Module;

#include <atlcom.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlctrls.h>

#include "kavlog.h"
#include "resource.h"

#define countof(array) (sizeof (array) / sizeof (*array))

#define LOG_REFRESH_TIMER    0x5d7c7931
#define LOG_REFRESH_INTERVAL 500

//////////////////////////////////////////////////////////////////////////
// CLogSettingsDlg

class CLogSettingsDlg : public CDialogImpl<CLogSettingsDlg>
{
public:
	typedef CDialogImpl<CLogSettingsDlg> TBase;

	CLogSettingsDlg(CTraceManager *pTM) : m_pTM(pTM) {}
	~CLogSettingsDlg() {}

	enum { IDD = IDD_DIALOG_LOG_PARAMETERS };
	enum { usTraceState = 0x01, usTracesExists = 0x02, usAll = usTraceState | usTracesExists };

protected:
	BEGIN_MSG_MAP(CLogSettingsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_ID_HANDLER(IDC_BUTTON_SWITCH_TRACE, OnBtnSwitchTrace)
		COMMAND_ID_HANDLER(IDC_BUTTON_SEND_2KL, OnBtnSend2KL)
		COMMAND_ID_HANDLER(IDC_COMBO_TRACE_LEVEL, OnComboTraceLevel)
		COMMAND_ID_HANDLER(IDC_COMPONENTS, OnCustomizeComponents)
 		COMMAND_ID_HANDLER(IDOK, OnClose)
 		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
		COMMAND_ID_HANDLER(IDC_BUTTON_CREATE_DUMP, OnBnClickedButtonCreateDump)
	END_MSG_MAP()
	
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBtnSwitchTrace(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBtnSend2KL(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnComboTraceLevel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCustomizeComponents(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);	
	LRESULT OnBnClickedButtonCreateDump(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);	
	void    UpdateState(UINT nFlags = usAll);

protected:
	CComboBox      m_Combo;
	
protected:
	CTraceManager *m_pTM;

protected:
	void    SaveTraces(bool bCanceled = false);
	void    EnableDlgItem(WORD wID, BOOL bEnable) { HWND hItemWnd = GetDlgItem(wID); if(hItemWnd) ::EnableWindow(hItemWnd, bEnable); }
};

//////////////////////////////////////////////////////////////////////////
// CZipTraceDlg

class CZipTraceDlg : public CDialogImpl<CZipTraceDlg>
{
public:
	typedef CDialogImpl<CZipTraceDlg> TBase;

	CZipTraceDlg(CTraceManager *pTM) : m_pTM(pTM), m_hZipThread(NULL), m_bCanceled(false), m_bZipped(false) {}
	~CZipTraceDlg() {}

	enum { IDD = IDD_DIALOG_WAIT };

protected:
	BEGIN_MSG_MAP(CZipTraceDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
 		COMMAND_ID_HANDLER(IDOK, OnClose)
 		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
	END_MSG_MAP()
	
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
	DWORD   ZipProc();
	static DWORD WINAPI ZipProcLink(LPVOID lpParameter);

protected:
	void    EnableDlgItem(WORD wID, BOOL bEnable) { HWND hItemWnd = GetDlgItem(wID); if(hItemWnd) ::EnableWindow(hItemWnd, bEnable); }

protected:
	CTraceManager *m_pTM;
	HANDLE         m_hZipThread;
	volatile bool  m_bCanceled;
	volatile bool  m_bZipped;
};

//////////////////////////////////////////////////////////////////////////
// CSrfRequestDlg

class CSrfRequestDlg : public CDialogImpl<CSrfRequestDlg>
{
public:
	typedef CDialogImpl<CSrfRequestDlg> TBase;

	CSrfRequestDlg(CTraceManager *pTM) : m_pTM(pTM) {}
	~CSrfRequestDlg() {}

	enum { IDD = IDD_DIALOG_REQUET_SRF };

protected:
	BEGIN_MSG_MAP(CLogSettingsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
 		COMMAND_ID_HANDLER(IDOK, OnClose)
 		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
	END_MSG_MAP()
	
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
	CTraceManager *m_pTM;
};

//////////////////////////////////////////////////////////////////////////
// CSrfRequestDlg

class CComponentCustomizeDlg : public CDialogImpl<CComponentCustomizeDlg>
{
public:
	typedef CDialogImpl<CComponentCustomizeDlg> TBase;

	CComponentCustomizeDlg(CTraceManager *pTM) : m_pTM(pTM) {}
	~CComponentCustomizeDlg() {}

	enum { IDD = IDD_DIALOG_COMP_CUST };

protected:
	BEGIN_MSG_MAP(CLogSettingsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		NOTIFY_HANDLER(IDC_COMPONENTS, LVN_ITEMCHANGED, OnComponentSelected)
		COMMAND_HANDLER(IDC_LEVEL, EN_CHANGE, OnLevelChanged)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnComponentSelected(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnLevelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
	CListViewCtrl  m_Components;
	CEdit          m_Level;

protected:
	void SetItemLevel(int index, int level);
	CTraceManager *m_pTM;
};

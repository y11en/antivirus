#include "stdafx.h"
#include "gui.h"

class cUILANG
{
	LANGID m_LangID;
public:
	cUILANG() : m_LangID(0)
	{
		typedef LANGID (WINAPI *tGetUserDefaultUILanguage)(void);
		HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
		if(hKernel32)
		{
			tGetUserDefaultUILanguage fnGetUserDefaultUILanguage = (tGetUserDefaultUILanguage)GetProcAddress(hKernel32, "GetUserDefaultUILanguage");
			if(fnGetUserDefaultUILanguage)
				m_LangID = fnGetUserDefaultUILanguage();
		}
		if(m_LangID == 0)
			m_LangID = PRIMARYLANGID(GetUserDefaultLangID());
	}
	LANGID GetLangID() const
	{
		return m_LangID;
	}
};

static cUILANG g_LanguageID;

LANGID g_LangID = g_LanguageID.GetLangID();

//////////////////////////////////////////////////////////////////////////
// CLogSettingsDlg

LRESULT CLogSettingsDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_Combo.Attach(GetDlgItem(IDC_COMBO_TRACE_LEVEL));

	// fill combo in
	char *aTraceLevls[] = {"Fatal (100)", "Danger (200)", "Error (300)", "Important (400)", "Notify (500)", "Not important (600)", "Spam (700)"};
	for(int i = 0; i < countof(aTraceLevls); i++)
		m_Combo.AddString(aTraceLevls[i]);
	
	SetDlgItemText(IDC_EDIT_PRODUCT_LOCATION, m_pTM->GetProductLocation().c_str());
	SetDlgItemText(IDC_EDIT_PRODUCT_VERSION,  m_pTM->GetProductVersion().c_str());

	GetDlgItem(IDC_COMPONENTS).ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_CREATE_DUMP).EnableWindow(m_pTM->IsDumpEnabled());

	SetIcon(::LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_ICON_KAV)));
	CenterWindow(GetParent());
	UpdateState();
	SetTimer(LOG_REFRESH_TIMER, LOG_REFRESH_INTERVAL);
	return 0;
}

LRESULT CLogSettingsDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == LOG_REFRESH_TIMER )
		UpdateState(usTracesExists);
	return 0;
}

LRESULT CLogSettingsDlg::OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT CLogSettingsDlg::OnBtnSwitchTrace(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pTM->EnableTrace(!m_pTM->IsTraceEnabled());
	UpdateState(usTraceState);
	return 0;
}

LRESULT CLogSettingsDlg::OnBtnSend2KL(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if( CZipTraceDlg(m_pTM).DoModal() == IDOK )
	{
		if( CSrfRequestDlg(m_pTM).DoModal() == IDOK )
		{
			if( m_pTM->Send2KL() )
				MessageBox(LoadRS(IDS_SEND_STATUS_OK).c_str(), LoadRS(IDS_SEND_STATUS_CAPTION).c_str(), MB_OK | MB_ICONINFORMATION);
			else
				SaveTraces();
		}
		else
			SaveTraces(true);
	}
	return 0;
}

LRESULT CLogSettingsDlg::OnComboTraceLevel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if( wNotifyCode == CBN_SELCHANGE )
		m_pTM->SetLevel((m_Combo.GetCurSel() + 1) * 100);
	return 0;
}

LRESULT CLogSettingsDlg::OnCustomizeComponents(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CComponentCustomizeDlg dlg(m_pTM);
	dlg.DoModal();
	return 0;
}

void CLogSettingsDlg::UpdateState(UINT nFlags)
{
	if( nFlags & usTraceState )
	{
		bool bTraceEnabled = m_pTM->IsTraceEnabled();
		SetDlgItemText(IDC_EDIT_TRACE_STATE,    LoadRS(bTraceEnabled ? IDS_TRACES_STATE_ON : IDS_TRACES_STATE_OFF).c_str());
		SetDlgItemText(IDC_BUTTON_SWITCH_TRACE, LoadRS(bTraceEnabled ? IDS_TRACES_TURN_OFF : IDS_TRACES_TURN_ON).c_str());
		
		m_Combo.EnableWindow(!bTraceEnabled);
		m_Combo.SetCurSel(m_pTM->GetLevel() / 100 - 1);
	}
	if( nFlags & usTracesExists )
	{
		bool bTracesExists = m_pTM->IsAnyTracesExists();
		SetDlgItemText(IDC_STATIC_TRACES_STATE, LoadRS(bTracesExists ? IDS_TRACES_EXISTS : IDS_TRACES_ABSENT).c_str());
		EnableDlgItem(IDC_BUTTON_SEND_2KL,      bTracesExists);
	}
}

void CLogSettingsDlg::SaveTraces(bool bCanceled)
{
	string strZippedTraces = m_pTM->GetZippedTraces();
	if( strZippedTraces.empty() )
		return;

	string &strErrorMsg = LoadRS(bCanceled ? IDS_SEND_STATUS_CANCELED : IDS_SEND_STATUS_FAILED);
	if( MessageBox(strErrorMsg.c_str(), LoadRS(IDS_SAVE_CONFR_CAPTION).c_str(), MB_YESNO | MB_ICONQUESTION) == IDYES )
	{
		string &strZippedLogs = m_pTM->GetZippedTraces();
		string &strLogFile    = GetFileFormPath(strZippedLogs);

		char strBuffer[5 * MAX_PATH];
		strcpy(strBuffer, strLogFile.c_str());

		OPENFILENAME ofn = {0, };
		ofn.lStructSize = sizeof OPENFILENAME;
		ofn.hwndOwner   = m_hWnd;
		ofn.lpstrFilter = "Archived logs\0*.zip\0";
		ofn.lpstrFile   = strBuffer;
		ofn.nMaxFile    = countof(strBuffer);
		ofn.Flags       = OFN_CREATEPROMPT | OFN_ENABLESIZING | OFN_LONGNAMES;
 		if( GetSaveFileName(&ofn) )
		{
			CopyFile(strZippedLogs.c_str(), ofn.lpstrFile, FALSE);
		}
	}
}

LRESULT CLogSettingsDlg::OnBnClickedButtonCreateDump(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode == BN_CLICKED)
	{
		m_pTM->TryToCreateDump();
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////////
// CZipTraceDlg

LRESULT CZipTraceDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD nThreadId;
	m_hZipThread = CreateThread(NULL, 0, CZipTraceDlg::ZipProcLink, this, 0, &nThreadId);

	m_bZipped = false;
	m_bCanceled = false;
	
	SetIcon(::LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_ICON_KAV)));
	CenterWindow(GetParent());
	return 0;
}

LRESULT CZipTraceDlg::OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if( wID == IDCANCEL )
	{
		EnableDlgItem(IDCANCEL, FALSE);
		m_bCanceled = true;
	}
	
	if( m_hZipThread )
	{
		WaitForSingleObject(m_hZipThread, INFINITE);
		CloseHandle(m_hZipThread);
		m_hZipThread = NULL;
	}
	
	EndDialog(m_bCanceled ? IDCANCEL : IDOK);
	return 0;
}

DWORD CZipTraceDlg::ZipProc()
{
	m_bZipped = m_pTM->ZipTraces();

	if( !m_bCanceled )
		::PostMessage(m_hWnd, WM_COMMAND, MAKELONG(m_bZipped ? IDOK : IDCANCEL, 0), 0);

	return 0;
}

DWORD CZipTraceDlg::ZipProcLink(LPVOID lpParameter)
{
	return ((CZipTraceDlg *)lpParameter)->ZipProc();
}


//////////////////////////////////////////////////////////////////////////
// CLogSettingsDlg

LRESULT CSrfRequestDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_pTM->GetSRF() )
		SetDlgItemInt(IDC_EDIT_SRF, m_pTM->GetSRF());
	
	SetIcon(::LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_ICON_KAV)));
	CenterWindow(GetParent());
	return 0;
}

LRESULT CSrfRequestDlg::OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if( wID == IDOK )
		m_pTM->SetSRF(GetDlgItemInt(IDC_EDIT_SRF));
	EndDialog(wID);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// CComponentCustomizeDlg

LRESULT CComponentCustomizeDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_Components.Attach(GetDlgItem(IDC_COMPONENTS));
	m_Level.Attach(GetDlgItem(IDC_LEVEL));

	m_Components.InsertColumn(0, LoadRS(IDS_COMPONENT).c_str(), LVCFMT_LEFT, 150, 0);
	m_Components.InsertColumn(1, LoadRS(IDS_LEVEL).c_str(), LVCFMT_LEFT, 100, 0);
	for (UINT i = 0; i < m_pTM->GetComponentCount(); ++i)
	{
		string name = m_pTM->GetComponentName(i);
		UINT level = m_pTM->GetComponentLevel(i);
		int index = m_Components.InsertItem(
			LVIF_PARAM|LVIF_TEXT, 0xFFFF, name.c_str(), 0, 0, 0, (LPARAM) i);
		SetItemLevel(index, level);
		ListView_SetExtendedListViewStyle(m_Components, LVS_EX_FULLROWSELECT);
	}
	return 0;
}

void CComponentCustomizeDlg::SetItemLevel(int index, int level)
{
	if (level == 0)
	{
		char buf[256];
		strcpy(buf, LoadRS(IDS_DEFAULT).c_str());
		ListView_SetItemText(m_Components, index, 1, buf);
	}
	else
	{
		char buf[128];
		_itoa(level, buf, 10);
		ListView_SetItemText(m_Components, index, 1, buf);
	}
}

LRESULT CComponentCustomizeDlg::OnComponentSelected(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	LPNMLISTVIEW pnmv = (LPNMLISTVIEW) pnmh;
	if (0 != (pnmv->uOldState & LVIS_SELECTED)
		|| 0 == (pnmv->uNewState & LVIS_SELECTED))
		return 0;
	UINT index = (UINT)pnmv->lParam;
	UINT level = m_pTM->GetComponentLevel(index);
	char buf[128];
	if (level)
		m_Level.SetWindowText(_itoa(level, buf, 10));
	else
		m_Level.SetWindowText("");
	return 0;
}

LRESULT CComponentCustomizeDlg::OnLevelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LVITEM item = {};
	item.mask = LVIF_PARAM|LVIF_STATE;
	if (!m_Components.GetSelectedItem(&item))
		return 0;
	m_Components.GetItem(&item);

	char level[128];
	m_Level.GetWindowText(level, _countof(level));
	int nLevel = atoi(level);
	m_pTM->SetComponentLevel((UINT) item.lParam, nLevel);

	SetItemLevel(item.iItem, nLevel);

	return 0;
}

LRESULT CComponentCustomizeDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDOK);
	return 0;
}
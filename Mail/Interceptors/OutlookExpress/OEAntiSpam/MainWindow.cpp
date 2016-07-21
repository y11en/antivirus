// MainWindow.cpp: implementation of the CMainWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainWindow.h"
#include "resource.h"
#include "MyComModule.h"
#include "WndInterceptor.h"
#include "Tracing.h"
#include "SettingsDlg.h"

#include <ProductCore/GuiLoader.h>

#include <commctrl.h>

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

namespace
{
	// the timer that is needed for proper installation of plugin
	// (it waits for the OE to fully initialize before starting the installation)
	const int CHECKINST_TIMERID = 1200;	
	const int CHECKINST_TIMEOUT = 500;	// twice per second

	// these two timers are needed for toolbar buttons enabling/disabling
	// CHECKAS timer will check for the AntiSpam task
	const int CHECKAS_TIMERID = 1201;
	const int CHECKAS_TIMEOUT = 1000;

	// CHECKLIST timer will check if there are some selected messages that can be sent to AS
	const int CHECKLIST_TIMERID = 1202;
	const int CHECKLIST_TIMEOUT = 250;	// four times per second

	const LPCTSTR PGPOE_DLL = _T("PGPoe.dll");
	const int OURBAND_ID = 50008;
}

CMainWindow::CMainWindow() : 
	m_hToolbar(NULL),
	m_bIsInstalling(false),
	m_bAntiSpamIsRunning(false),
	m_bIsConfigShown(false),
	m_nStyle(WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS),
	m_bShouldProcessCopyMoveDialog(false),
	m_OETricks(this)
{
}

CMainWindow::~CMainWindow()
{
	if (IsWindow())
		UnsubclassWindow(TRUE);
}

HBITMAP _ConvertIconToBitmap(HICON hImg)
{
    HDC hdcScreen = GetDC(NULL);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, 16, 16);
    HGDIOBJ old = SelectObject(hdc, hbmp);
	
    HBRUSH hbrush=GetSysColorBrush(COLOR_BTNFACE);
    DrawIconEx(hdc, 0, 0, hImg, 16, 16, 0, hbrush,DI_NORMAL);
    DeleteObject(hbrush);
    SelectObject(hdc,old);
    DeleteDC(hdc);
    ReleaseDC(NULL,hdcScreen);
	return hbmp;
}

bool _FillToolbar(HWND hToolbar)
{
	if (!hToolbar)
		return false;

	//   There is an incompatibility with PGP Desktop 9 (maybe with other versions too),
	// which has its own OE-plugin. When our toolbar has an imagelist, 
	// PGP mistakes our toolbar for the OE native toolbar and cannot add its button for some reason.
	// PGP fails to obtain the correct imagelist and hangs in an infinite loop.
	//   To appease the effect of this incompatibility, we don't use imagelists when PGP plugin
	// is detected and use them otherwise.

	CEnsureFreeLibrary hPGP = LoadLibrary(PGPOE_DLL);
	bool bIsPGPpresent = hPGP.IsValid() ? true : false;

    TBBUTTON but;
    ZeroMemory(&but,sizeof(but));
    but.fsState = TBSTATE_ENABLED; 
    but.fsStyle = TBSTYLE_AUTOSIZE | TBSTYLE_BUTTON;
	
	TBADDBITMAP addbmp;
	addbmp.hInst = 0;

	// first button - "Is spam"
	HBITMAP hCoolImg = (HBITMAP) LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_ISSPAM), IMAGE_BITMAP, 16, 14, LR_DEFAULTCOLOR);
	addbmp.nID = (UINT_PTR) hCoolImg;

	int iImage = bIsPGPpresent ? I_IMAGENONE : (int)SendMessage(hToolbar, TB_ADDBITMAP, (WPARAM) 1, (LPARAM) static_cast<LPTBADDBITMAP>(&addbmp)); 
	but.iBitmap = iImage;
	but.idCommand = ID_ISSPAM;
	but.fsState = 0;
	tstring str;
	std::wstring wstr;
	if(COETricks::m_bVista)
	{
		wstr = _Module.LoadStringW("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsSpamCaption");
		if (wstr.empty())
			wstr = L"Spam";
		but.iString = (INT_PTR)wstr.c_str();
		SendMessage(hToolbar, TB_ADDBUTTONSW, (WPARAM)1, (LPARAM) (LPTBBUTTON)&but); 
	}
	else
	{
		str = _Module.LoadString("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsSpamCaption");
		if (str.size() <= 0)
			str = _T("Spam");
		but.iString = (INT_PTR)str.c_str();
		SendMessage(hToolbar, TB_ADDBUTTONS, (WPARAM)1, (LPARAM) (LPTBBUTTON)&but); 
	}

	// second button - "Is ham"
	hCoolImg = (HBITMAP) LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_ISNOTSPAM), IMAGE_BITMAP, 16, 14, LR_DEFAULTCOLOR);
	addbmp.nID = (UINT_PTR) hCoolImg;
	iImage = bIsPGPpresent ? I_IMAGENONE : (int)SendMessage(hToolbar, TB_ADDBITMAP, (WPARAM) 1, (LPARAM) static_cast<LPTBADDBITMAP>(&addbmp)); 
	but.iBitmap = iImage;
	but.idCommand = ID_ISNOTSPAM;
	if(COETricks::m_bVista)
	{
		wstr = _Module.LoadStringW("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsHamCaption");
		if (wstr.empty())
			wstr = L"Not spam";
		but.iString = (INT_PTR)wstr.c_str();
		SendMessage(hToolbar, TB_ADDBUTTONSW, (WPARAM)1, (LPARAM) (LPTBBUTTON)&but); 
	}
	else
	{
		str = _Module.LoadString("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsHamCaption");
		if (str.size() <= 0)
			str = _T("Not spam");
		but.iString = (INT_PTR)str.c_str();
		SendMessage(hToolbar, TB_ADDBUTTONS, (WPARAM)1, (LPARAM) (LPTBBUTTON)&but); 
	}

	// third button - "Configure"
	HICON hImg = (HICON) LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_CONFIGURE), IMAGE_ICON, 16, 16, 0);
	addbmp.nID = (UINT_PTR) _ConvertIconToBitmap(hImg);
	iImage = bIsPGPpresent ? I_IMAGENONE : (int)SendMessage(hToolbar, TB_ADDBITMAP, (WPARAM) 1, (LPARAM) static_cast<LPTBADDBITMAP>(&addbmp)); 
	but.iBitmap = iImage;
	but.idCommand = ID_CONFIG;
	but.fsState = TBSTATE_ENABLED;
	if(COETricks::m_bVista)
	{
		wstr = _Module.LoadStringW("Report_Body.ReportDetails.antispam.Events.menu", "ASConfigCaption");
		if (wstr.empty())
			wstr = L"Configure";
		but.iString = (INT_PTR)wstr.c_str();
		SendMessage(hToolbar, TB_ADDBUTTONSW, (WPARAM)1, (LPARAM) (LPTBBUTTON)&but);
	}
	else
	{
		str = _Module.LoadString("Report_Body.ReportDetails.antispam.Events.menu", "ASConfigCaption");
		if (str.size() <= 0)
			str = _T("Configure");
		but.iString = (INT_PTR)str.c_str();
		SendMessage(hToolbar, TB_ADDBUTTONS, (WPARAM)1, (LPARAM) (LPTBBUTTON)&but);
	}

	return true;
}

LRESULT CMainWindow::OnAfterSubclass(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ASTRACE(_T("Initializing"));
	if(!_Module.LocInit())
		return -1;
	m_OETricks.Init(m_hWnd);
	m_OESettings.Initialize();
	ASTRACE(_T(""));

	HWND hSizableRebar = FindWindowEx(m_hWnd, NULL, TEXT("SizableRebar"), NULL);
	HWND hReBarWindow = FindWindowEx(hSizableRebar, NULL, TEXT("ReBarWindow32"), NULL);

	// create the toolbar here
	m_hToolbar = CreateWindowEx(TBSTYLE_EX_HIDECLIPPEDBUTTONS, TOOLBARCLASSNAME, _T("KAV OEAntiSpam"), 
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		TBSTYLE_TRANSPARENT | TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TOOLTIPS |
		CCS_NODIVIDER | CCS_ADJUSTABLE | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_TOP, 
		110, 110, 100, 100, hReBarWindow, 0, NULL /*Aplication Instance*/, 0);

	SendMessage(m_hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

	_FillToolbar(m_hToolbar);

	SIZE size;
	size.cx = size.cy = 0;
	SendMessage(m_hToolbar, TB_GETMAXSIZE, 0, reinterpret_cast<LPARAM>(&size));

	CRebarPos::TBand band;

	bool bPosLoaded = false;
	tstring strRegName = COESettings::GetSettingsRegName();
	CRegKey key;
	if (key.Create(HKEY_CURRENT_USER, strRegName.c_str(), NULL, REG_OPTION_NON_VOLATILE, KEY_READ) == ERROR_SUCCESS)
	{
		if (m_RebarPos.LoadFromRegistry(key))
		{
			bPosLoaded = true;
			m_RebarPos.GetBand(OURBAND_ID, band);
		}
	}

	REBARBANDINFO rbBand;
	ZeroMemory(&rbBand, sizeof(REBARBANDINFO));
	rbBand.cbSize = sizeof(REBARBANDINFO);
	rbBand.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE | RBBIM_ID;
	rbBand.fStyle = band.m_bUseFullLine ? RBBS_BREAK : 0 /*| RBBS_USECHEVRON*/;
	rbBand.hwndChild = m_hToolbar;
	rbBand.cxMinChild = 30;
	rbBand.cyMinChild = HIWORD(SendMessage(m_hToolbar, TB_GETBUTTONSIZE, 0,0));
	rbBand.cx = (band.m_dwCX <= 0) ? (size.cx + 15) : band.m_dwCX;
	rbBand.cxIdeal = size.cx;
	rbBand.wID = OURBAND_ID;
	LRESULT nResult = SendMessage(hReBarWindow, RB_INSERTBAND, (WPARAM) bPosLoaded ? band.m_dwIndex : -1, 
		(LPARAM) &rbBand);

	if (bPosLoaded)
		m_RebarPos.ApplyToRebar(hReBarWindow);

	// if the plugin is not installed, set the timer, that will choose the right time to install
	// (that is, when the tree control is populated)
	// installation suggests creating spam-folders and default rules 
	if (!COESettings::IsOEPluginInstalled())
	{
		ASTRACE(_T("Plugin was not installed, starting installation process"));
		SetTimer(CHECKINST_TIMERID, CHECKINST_TIMEOUT);
	}
	else
		m_OESettings.LoadSettings();

	SetTimer(CHECKAS_TIMERID, CHECKAS_TIMEOUT);
	SetTimer(CHECKLIST_TIMERID, CHECKLIST_TIMEOUT);

	m_nStyle = GetWindowLong(GWL_STYLE);

	return S_OK;
}

LRESULT CMainWindow::OnIsSpam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Dummy.Create(m_hWnd);
	m_Dummy.ShowWindow(SW_SHOW);

	m_ProgressDlg.CreateDialogInThread(true, m_hWnd);
	UINT nTotalMessages = 0;
	if (m_OETricks.SendSelectedMessagesToMC(true, nTotalMessages))
	{
		m_ProgressDlg.SetTotalMessages(nTotalMessages);
		m_nStyle = GetWindowLong(GWL_STYLE);
		SetWindowLong(GWL_STYLE, m_nStyle & ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX));

		HMENU hSysMenu = GetSystemMenu(FALSE);
		if (hSysMenu)
		{
			MENUITEMINFO mii;
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STATE;
			mii.fState = MFS_GRAYED;
			
			SetMenuItemInfo(hSysMenu, SC_CLOSE, FALSE, &mii);
		}
	}

	return S_OK;
}

LRESULT CMainWindow::OnIsNotSpam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Dummy.Create(m_hWnd);
	m_Dummy.ShowWindow(SW_SHOW);

	m_ProgressDlg.CreateDialogInThread(false, m_hWnd);
	UINT nTotalMessages = 0;
	if (m_OETricks.SendSelectedMessagesToMC(false, nTotalMessages))
	{
		m_ProgressDlg.SetTotalMessages(nTotalMessages);
		m_nStyle = GetWindowLong(GWL_STYLE);
		SetWindowLong(GWL_STYLE, m_nStyle & ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX));
		
		HMENU hSysMenu = GetSystemMenu(FALSE);
		if (hSysMenu)
		{
			MENUITEMINFO mii;
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STATE;
			mii.fState = MFS_GRAYED;
			
			SetMenuItemInfo(hSysMenu, SC_CLOSE, FALSE, &mii);
		}
	}

	return S_OK;
}

bool CMainWindow::_CanInstallNow()
{
	// see if the tree control has been populated
	HWND hTreeCtrl = m_OETricks.GetTreeCtrl();
	if (!hTreeCtrl)
		return false;

	// see how many leafs are there (we expect at least 5)
	UINT nLeafs = TreeView_GetCount(hTreeCtrl);
	if (nLeafs >= 5)
		return true;

	return false;
}

LRESULT CMainWindow::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (wParam)
	{
	case CHECKINST_TIMERID:
		if (_CanInstallNow())
		{
			_InstallPlugin();
			KillTimer(CHECKINST_TIMERID);
		}
		break;

	case CHECKLIST_TIMERID:
		{
			HWND hMsgView = m_OETricks.GetMsgListView();
			if (!hMsgView)
				_EnableButtons(false);
			else
			{
				// see if anything is selected
				LRESULT nSelectedCount = ::SendMessage(hMsgView, LVM_GETSELECTEDCOUNT, 0, 0L);
				_EnableButtons((nSelectedCount > 0) ? true : false);
			}
		}
		break;

	case CHECKAS_TIMERID:
		{
			bool bAntiSpamIsRunning = _Module.IsAntiSpamTaskRunning();
			
			if (!m_bIsConfigShown && !m_bAntiSpamIsRunning && bAntiSpamIsRunning && !COESettings::IsOEPluginInstalled())
			{
				// the antispam task has been turned on!
				PostMessage(WM_COMMAND, ID_CONFIG, 0);
			}

			if (m_bAntiSpamIsRunning ^ bAntiSpamIsRunning)
			{
				// rewrite the rules (to overwrite possible user changes) - only if the settings were already saved once
				if (COESettings::IsOEPluginInstalled())
				{
					m_OESettings.SetEnabled(bAntiSpamIsRunning);
					m_OESettings.SaveSettings();
				}
			}

			m_bAntiSpamIsRunning = bAntiSpamIsRunning;
		}
		break;
	}

	return S_OK;
}

bool CMainWindow::_InstallPlugin()
{
	// first, create folders
	ASTRACE(_T("Installation phase 1"));
	PostMessage(WM_CREATEFOLDER_DONE, 0, 0);
//	m_OETricks.CreateSpamFolders(false);	
	m_bIsInstalling = true;	// the rest of installation will be done in OnCreateFolderDone()

	return true;
}

LRESULT CMainWindow::OnOECreateFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CWndInterceptor& interceptor = CWndInterceptor::GetInstance();
	interceptor.BeforeCreateFolderDialog(m_hWnd);
	LRESULT lRes = DefWindowProc(WM_COMMAND, MAKELONG(wID, wNotifyCode), (LPARAM) hWndCtl);
	interceptor.AfterCreateFolderDialog(m_hWnd);

	bHandled = TRUE;
	return lRes;
}

LRESULT CMainWindow::OnOEProperties(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// increase the number of processed messages
	m_ProgressDlg.IncrementMessages();

	CWndInterceptor& interceptor = CWndInterceptor::GetInstance();
	interceptor.BeforeMsgPropDialog(m_hWnd);
	LRESULT lRes = DefWindowProc(WM_COMMAND, MAKELONG(wID, wNotifyCode), (LPARAM) hWndCtl);
	interceptor.AfterMsgPropDialog(m_hWnd, m_ProgressDlg.GetMustStopEvent());

	bHandled = TRUE;
	return lRes;
}

LRESULT CMainWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_Module.LocDeinit();
	bHandled = FALSE;
	return S_OK;
}

bool CMainWindow::_DestroyOurToolband()
{
	HWND hSizableRebar = FindWindowEx(m_hWnd, NULL, TEXT("SizableRebar"), NULL);
	HWND hReBarWindow = FindWindowEx(hSizableRebar, NULL, TEXT("ReBarWindow32"), NULL);
	if (!hReBarWindow)
		return false;

	m_RebarPos.FillFromRebar(hReBarWindow);

	tstring strRegName = COESettings::GetSettingsRegName();
	CRegKey key;
	if (key.Create(HKEY_CURRENT_USER, strRegName.c_str(), NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE) == ERROR_SUCCESS)
		m_RebarPos.SaveToRegistry(key);

	LRESULT n = SendMessage(hReBarWindow, RB_IDTOINDEX, OURBAND_ID, 0);
	if (n < 0)
		return false;

	SendMessage(hReBarWindow, RB_DELETEBAND, n, 0);
	::DestroyWindow(m_hToolbar);
	return true;
}

LRESULT CMainWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_DestroyOurToolband();
	bHandled = FALSE;
	return S_OK;
}

LRESULT CMainWindow::OnCreateFolderDone(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bIsInstalling)
	{
		ASTRACE(_T("Installation phase 2"));

		m_bIsInstalling = false;

		// create rules
		__int64 dwSpamFolderID;
		__int64 dwProbSpamFolderID;
		m_OETricks.GetSpamFoldersIDfromInbox(dwSpamFolderID, dwProbSpamFolderID);
		m_OESettings.SetDefaultSettings(dwSpamFolderID, dwProbSpamFolderID);
		
		// finally, create registry key to indicate that the plugin has been successfully installed
//		m_OESettings.SaveSettings();	// the settings will be saved in OnConfig()
		ASTRACE(_T("Installation finished"));

		// see if AntiSpam task is available
		if (!m_bIsConfigShown && _Module.IsAntiSpamTaskRunning())
			PostMessage(WM_COMMAND, ID_CONFIG, 0);
	}

	return S_OK;
}

LRESULT CMainWindow::OnMsgPropDone(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// do action from the settings
	bool bIsSpam = (wParam != FALSE);
	bool bWasCancelled = (lParam != FALSE);
	
	COESettings::SpamActions nSpamAction;
	bool bSpamMarkAsRead;
	__int64 dwSpamFolderID;
	m_OESettings.GetSpamSettings(nSpamAction, bSpamMarkAsRead, dwSpamFolderID);

	COESettings::SpamActions nProbAction;
	bool bProbMarkAsRead;
	__int64 dwProbFolderID;
	m_OESettings.GetProbSpamSettings(nProbAction, bProbMarkAsRead, dwProbFolderID);

	USES_CONVERSION;
	tstring msg;
	
	if (bWasCancelled)
		msg = _T("The training was cancelled");
	else
	{
		msg = _T("Done training Antispam, applying settings (spam act=");
		msg += A2CT((boost::lexical_cast<std::string>(nSpamAction)).c_str());
		msg += _T(", mark=");
		msg += A2CT((boost::lexical_cast<std::string>(bSpamMarkAsRead)).c_str());
		msg += _T(", folder=");
		msg += A2CT((boost::lexical_cast<std::string>(dwSpamFolderID)).c_str());
		msg += _T(") (prob act=");
		msg += A2CT((boost::lexical_cast<std::string>(nProbAction)).c_str());
		msg += _T(", mark=");
		msg += A2CT((boost::lexical_cast<std::string>(bProbMarkAsRead)).c_str());
		msg += _T(", folder=");
		msg += A2CT((boost::lexical_cast<std::string>(dwProbFolderID)).c_str());
		msg += _T(")");
	}

	ASTRACE(msg.c_str());
	ASTRACE(_T(""));

	// here's the algorithm:
	// reselect all messages
	m_OETricks.RestoreSelection();
	__int64 dwCurrentFolderID = m_OETricks.GetCurrentFolderID();

	if (!bWasCancelled)
	{
		// if it's a spam, use the spam settings for actions
		if (bIsSpam)
		{
			if (bSpamMarkAsRead)
				m_OETricks.MarkSelectedRead();
			
			switch(nSpamAction)
			{
			case COESettings::SA_LEAVEASIS:
				break;
				
			case COESettings::SA_MOVETOFOLDER:
				if (dwCurrentFolderID != dwSpamFolderID)	// if we're in the same folder, do nothing
					m_OETricks.MoveSelectedMessages(dwSpamFolderID);
				break;
				
			case COESettings::SA_COPYTOFOLDER:
				if (dwCurrentFolderID != dwSpamFolderID)	// if we're in the same folder, do nothing
					m_OETricks.CopySelectedMessages(dwSpamFolderID);
				break;
				
			case COESettings::SA_DELETE:
				m_OETricks.DeleteSelectedMessages();
				break;
			}
		}
		else
		{
			// not spam
			bool bMoveMsgs = false;
			if ((nSpamAction == COESettings::SA_MOVETOFOLDER) && (dwCurrentFolderID == dwSpamFolderID))
				bMoveMsgs = true;
			
			if ((nProbAction == COESettings::SA_MOVETOFOLDER) && (dwCurrentFolderID == dwProbFolderID))
				bMoveMsgs = true;
			
			if (bMoveMsgs)
				m_OETricks.MoveSelectedMessages(COETricks::GetInboxId());
		}
	}
	
	m_ProgressDlg.CloseDialog();
	m_Dummy.DestroyWindow();

	::SetForegroundWindow(m_hWnd);
	m_OETricks.SetFocusToMessageView();

	// enable Minimize, Maximize and Close buttons
	HMENU hSysMenu = GetSystemMenu(FALSE);
	if (hSysMenu)
	{
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_ENABLED;
	
		SetMenuItemInfo(hSysMenu, SC_CLOSE, FALSE, &mii);
	}
	  
	SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) & ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX) |
																m_nStyle & (WS_MAXIMIZEBOX|WS_MINIMIZEBOX));
	SendMessage(WM_NCACTIVATE, 1, 0);

	return S_OK;
}

LRESULT CMainWindow::OnConfigure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CRootItem* pRoot = _Module.m_GuiLoader.GetRoot();
	if( !pRoot )
		return S_OK;

	m_bIsConfigShown = true;

	COETricks::FOLDERS_LIST arrFolders;
	m_OETricks.GetLocalFolders(arrFolders);
	
	CPageAntispamSink dlg(m_hWnd, m_OETricks.GetTreeImageList(), arrFolders);
	dlg.SetSettings(m_OESettings);

	if (dlg.DoModal(pRoot, "OutlookPlugun.AntispamDlg") == DLG_ACTION_OK)
	{
		COESettings tmp_settings;
		dlg.GetSettings(tmp_settings);
		
		bool bIsInstalled = COESettings::IsOEPluginInstalled();
		if (!bIsInstalled || (bIsInstalled && (m_OESettings != tmp_settings)))
		{
			m_OESettings = tmp_settings;
			m_OESettings.SaveSettings();

			if(COETricks::m_bVista)
			{
				std::wstring strBody;
				strBody = _Module.LoadStringW("OutlookPlugun.AntispamDlg", "WinMailNeedRestart");
				std::wstring strTitle = _Module.LoadStringW("OutlookPlugun.AntispamDlg", "dlg_caption");
				::MessageBoxW(m_hWnd, strBody.c_str(), strTitle.c_str(), MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				tstring strBody;
				strBody = _Module.LoadString("OutlookPlugun.AntispamDlg", "OENeedRestart");
				tstring strTitle = _Module.LoadString("OutlookPlugun.AntispamDlg", "dlg_caption");
				MessageBox(strBody.c_str(), strTitle.c_str(), MB_OK | MB_ICONINFORMATION);
			}
		}
	}
	else
	{
		if (dlg.ShouldSaveAnyway())
			m_OESettings.SaveSettings();
	}

	m_bIsConfigShown = false;

	return S_OK;
}

LRESULT CMainWindow::OnOECopyMove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!m_bShouldProcessCopyMoveDialog)
	{
		bHandled = FALSE;
		return S_OK;
	}

	m_bShouldProcessCopyMoveDialog = false;

	CWndInterceptor& interceptor = CWndInterceptor::GetInstance();
	interceptor.BeforeCopyMoveDialog(m_hWnd);
	LRESULT lRes = DefWindowProc(WM_COMMAND, MAKELONG(wID, wNotifyCode), (LPARAM) hWndCtl);
	interceptor.AfterCopyMoveDialog(m_hWnd);
	
	bHandled = TRUE;
	return lRes;
}

void CMainWindow::_EnableButtons(bool bEnable)
{
	if (bEnable)
		bEnable = m_bAntiSpamIsRunning;	// is there's no antispam task, disable the buttons

	::SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_ISSPAM, MAKELONG(bEnable ? TRUE : FALSE, 0));	
	::SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_ISNOTSPAM, MAKELONG(bEnable ? TRUE : FALSE, 0));	
}


// Create_AuditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Create_Audit.h"
#include "Create_AuditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD g_wuNewData		= RegisterWindowMessage("Audit_NewDataArrived");
DWORD g_wuTrayNotify	= RegisterWindowMessage("Audit_TrayNotification");

// -----------------------------------------------------------------------------------------

BOOL TrayMessage(HWND hDlg, UINT uFlags, DWORD dwMessage, UINT uID, HICON hIcon, CHAR *pszTip)
{
	BOOL res;
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = hDlg;
	tnd.uID = uID;
	tnd.uFlags = uFlags;
	tnd.uCallbackMessage = g_wuTrayNotify;
	tnd.hIcon = hIcon;
	if(pszTip)
		lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip));
	else
		tnd.szTip[0] = '\0';
	res = Shell_NotifyIcon(dwMessage, &tnd);
	if(hIcon)
		DestroyIcon(hIcon);
	
	return res;
}

// -----------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CCreate_AuditDlg dialog

CCreate_AuditDlg::CCreate_AuditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreate_AuditDlg::IDD, pParent)
{
	CRegStorage RegStorage(HKEY_CURRENT_USER, "Software\\KasperskyLab\\Create_Audit");
	
	char path[MAX_PATH];
	DWORD szlen = sizeof(path);
	if (RegStorage.GetValue("Path", (LPTSTR) path, szlen) == FALSE)
		lstrcpy(path,  "\0");

	DWORD AutoHide = 0;
	RegStorage.GetValue("AutoHide", AutoHide);
	DWORD AutoStart = 0;
	RegStorage.GetValue("AutoStart", AutoStart);

	//{{AFX_DATA_INIT(CCreate_AuditDlg)
	m_ePath = path;
	m_chAutoHide = AutoHide;
	m_chAutoStart = AutoStart;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hEvWaiterThread = NULL;
}

void CCreate_AuditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreate_AuditDlg)
	DDX_Text(pDX, IDC_EDIT_PATH, m_ePath);
	DDV_MaxChars(pDX, m_ePath, 1024);
	DDX_Check(pDX, IDC_CHECK_AUTOHIDE, m_chAutoHide);
	DDX_Check(pDX, IDC_CHECK_AUTOSTART, m_chAutoStart);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCreate_AuditDlg, CDialog)
	//{{AFX_MSG_MAP(CCreate_AuditDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_REGISTERED_MESSAGE(g_wuNewData, OnNewData)
	ON_REGISTERED_MESSAGE(g_wuTrayNotify, OnTrayNotification)
	ON_COMMAND(IDMI_EXIT, OnExit)
	ON_COMMAND(IDMI_SHOW, OnShow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreate_AuditDlg message handlers

BOOL CCreate_AuditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_OSVer.dwOSVersionInfoSize = sizeof(m_OSVer);
	GetVersionEx(&m_OSVer);

	if (m_OSVer.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		MessageBox("Can't start - platform not NT", "Error", MB_ICONINFORMATION);
		OnCancel();
	}

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	
	lstrcpy(m_ThreadParam.m_AppReg.m_LogFileName, "");
	m_ThreadParam.m_hWnd = m_hWnd;
	m_ThreadParam.m_AppReg.m_AppID = AVPG_Driver_Specific;
	m_ThreadParam.m_AppReg.m_Priority = AVPG_INFOPRIORITY;
	m_ThreadParam.m_pOSVer = &m_OSVer;

	m_hEvWaiterThread = StartClient(&m_ThreadParam);
	if (m_hEvWaiterThread == NULL)
	{
		MessageBox("Can't start client", "Error", MB_ICONINFORMATION);
		OnCancel();
	}

	TrayMessage(m_hWnd, NIF_MESSAGE | NIF_ICON | NIF_TIP, NIM_ADD, IDR_MAINFRAME, 
		(HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0), "Audit_Create");
	
	if (m_chAutoHide)
		ShowWindow(SW_HIDE);

	if (m_chAutoStart)
		OnButtonStart();
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCreate_AuditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCreate_AuditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CCreate_AuditDlg::DestroyWindow() 
{
	UpdateData();
	CRegStorage RegStorage(HKEY_CURRENT_USER, "Software\\KasperskyLab\\Create_Audit");
	RegStorage.PutValue("Path", m_ePath);
	
	RegStorage.PutValue("AutoHide", (DWORD) m_chAutoHide);
	RegStorage.PutValue("AutoStart", (DWORD) m_chAutoStart);
	
	
	if (m_hEvWaiterThread != NULL)
		StopClient(m_hEvWaiterThread, &m_ThreadParam);	

	TrayMessage(m_hWnd, NIF_MESSAGE, NIM_DELETE, IDR_MAINFRAME, NULL, NULL);
	
	return CDialog::DestroyWindow();
}

void CCreate_AuditDlg::OnButtonStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_ePath.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_PATH)->SetFocus();
		return;
	}
	
	m_ePath += "*";

	if (AddFilters(&m_ThreadParam) == FALSE)
		return;

	GetDlgItem(IDC_EDIT_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	//GetDlgItem(IDC_BUTTON_STOP)->EnableWindow();

	ShowWindow(SW_HIDE);
}

void CCreate_AuditDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_PATH)->EnableWindow();
	GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow();	

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
}

long CCreate_AuditDlg::OnNewData(WPARAM wParam, LPARAM lParam)
{
	// wparam user, lparam filename
	return 0;
}

long CCreate_AuditDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_LBUTTONDOWN:
		/*SetForegroundWindow(hWnd);
		PostMessage(hWnd, WM_COMMAND, IDMI_SHOWSETTINGS, 0);*/
		return 0;
	case WM_RBUTTONDOWN: 
		{
			POINT pt;
			HMENU hSysMenu,hSubMenu;
			
			hSysMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_TRAY));
			GetCursorPos(&pt);
			SetForegroundWindow();
			hSubMenu = GetSubMenu(hSysMenu,0);
			
			SetMenuDefaultItem(hSubMenu, IDMI_SHOW, MF_BYCOMMAND);
			TrackPopupMenu(hSubMenu,TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x,pt.y, 0, m_hWnd,NULL);
			DestroyMenu(hSysMenu);
		}
		break;
	}
	
	return 0;
}

void CCreate_AuditDlg::OnExit() 
{
	// TODO: Add your command handler code here
	OnOK();
}

void CCreate_AuditDlg::OnShow() 
{
	// TODO: Add your command handler code here
	ShowWindow(SW_SHOW);
}

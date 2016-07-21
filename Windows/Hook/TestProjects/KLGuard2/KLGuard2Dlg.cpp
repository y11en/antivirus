// KLGuard2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "KLGuard2.h"
#include "KLGuard2Dlg.h"

#include "DHistory.h"
#include "DSandBoxSettings.h"
#include "DKnownProcesses.h"
#include "DAbout.h"
#include "DGenericSettings.h"

#include "ListColumnPositions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//+ ------------------------------------------------------------------------------------------
UINT nHyperlinkPressed = RegisterWindowMessage(_T("klguard_HyperlinkPressed"));
UINT WU_ALERT = RegisterWindowMessage(_T("klguard_GlobalAlert"));
//+ ------------------------------------------------------------------------------------------

void GlobalAlert()
{
	SendMessage(AfxGetMainWnd()->m_hWnd, WU_ALERT, 0, 0);
}

#define _L_Start_time			0
#define _L_Proc					1
#define _L_PID					2
#define _L_Files				3
#define _L_SysReg				4
#define _L_Inet					5
#define _L_ParentID				6
#define _L_HasInvisibleWindow	7
#define _L_ImagePath			8

/////////////////////////////////////////////////////////////////////////////
// CKLGuard2Dlg dialog

CKLGuard2Dlg::CKLGuard2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKLGuard2Dlg::IDD, pParent)
{
	CGenericSettings* pGenericSettings = gpProcList->GetGenericSettings();
	//{{AFX_DATA_INIT(CKLGuard2Dlg)
	m_chEducationMode = pGenericSettings->m_bEducationMode;
	m_chStartKnownProcesses = pGenericSettings->m_bStartOnlyKnownProcesses;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_KL);

	m_pSandBox = NULL;
	m_pGeneriSettings = NULL;
	m_pKnownProcesses = NULL;
	m_pAbout = NULL;

	m_TrayBlinkCounter = 0;
}

void CKLGuard2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKLGuard2Dlg)
	DDX_Control(pDX, IDC_LIST_PROCESSES, m_lProcesses);
	DDX_Check(pDX, IDC_CHECK_EDUCATIONMODE, m_chEducationMode);
	DDX_Check(pDX, IDC_CHECK_START_KNOWN_PROCESSES, m_chStartKnownProcesses);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKLGuard2Dlg, CDialog)
	//{{AFX_MSG_MAP(CKLGuard2Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_REGISTERED_MESSAGE(nHyperlinkPressed, OnHyperLink)
	ON_WM_TIMER()
	ON_MESSAGE(WM_NOTIFYICON, OnTrayIocn)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESSES, OnDblclkListProcesses)
	ON_BN_CLICKED(IDC_CHECK_EDUCATIONMODE, OnCheckEducationmode)
	ON_BN_CLICKED(IDC_CHECK_START_KNOWN_PROCESSES, OnCheckStartKnownProcesses)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_REGISTERED_MESSAGE(WU_ALERT, OnAlert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKLGuard2Dlg message handlers

BOOL CKLGuard2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	ListView_SetExtendedListViewStyle(m_lProcesses.m_hWnd, LVS_EX_FULLROWSELECT);

	HINSTANCE CurrentInstance = AfxGetInstanceHandle();
	
	m_pSandBox = CreateHyperlinkControl(this->m_hWnd, WU_SAND_BOX, IDS_BUTTON_SANDBOX, 
		CurrentInstance, CurrentInstance, IDC_RECT_SANDBOX, 0);

	m_pGeneriSettings = CreateHyperlinkControl(this->m_hWnd, WU_GENERIC_SETTINGS, IDS_BUTTON_GENERIC_SETTINGS, 
		CurrentInstance, CurrentInstance, IDC_RECT_GENERIC_SETTINGS, 0);
	
	m_pKnownProcesses = CreateHyperlinkControl(this->m_hWnd, WU_KNOWNPROC, IDS_STRING_KNOWNPROCESSES, 
		CurrentInstance, CurrentInstance, IDC_RECT_KNOWPROCESSES, 0);

	m_pAbout = CreateHyperlinkControl(this->m_hWnd, WU_ABOUT, IDS_BUTTON_ABOUT, 
		CurrentInstance, CurrentInstance, IDC_RECT_ABOUT, 0);

	m_pAbout->SetDrawTestFormat(DT_RIGHT | DT_WORDBREAK);

	TrayMessage(this->m_hWnd, NIF_MESSAGE | NIF_ICON | NIF_TIP, NIM_ADD, _TRAY_ICON_ID,
		(HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_KL),IMAGE_ICON, 16, 16, 0), _APP_NAME);
	
	SetTimer(_TIMER_MAINWND, 1000, 0);
	SetTimer(_TIMER_TRAYBLINK, 500, 0);
	SetTimer(_TIMER_TIMERSTEP, 1000 * 30, 0);

	m_lProcesses.InsertColumn(_L_Start_time, _T("Start time"), LVCFMT_LEFT, 100);
	m_lProcesses.InsertColumn(_L_Proc, _T("Proc"), LVCFMT_LEFT, 100);
	m_lProcesses.InsertColumn(_L_PID, _T("PID"), LVCFMT_LEFT, 80);
	m_lProcesses.InsertColumn(_L_Files, _T("Files"), LVCFMT_LEFT, 80);
	m_lProcesses.InsertColumn(_L_SysReg, _T("SysReg"), LVCFMT_LEFT, 60);
	m_lProcesses.InsertColumn(_L_Inet, _T("Inet"), LVCFMT_LEFT, 60);
	m_lProcesses.InsertColumn(_L_ParentID, _T("ParentID"), LVCFMT_LEFT, 80);
	m_lProcesses.InsertColumn(_L_HasInvisibleWindow, _T("InvisibleWnd"), LVCFMT_LEFT, 80);
	m_lProcesses.InsertColumn(_L_ImagePath, _T("ImagePath"), LVCFMT_LEFT, 600);

	if (!gpProcList->StartLoop())
	{
		TRACE0("start main loop failed");
		MessageBox(_T("Start failed"), _T("KLGuard v2 main init"), MB_ICONERROR);
		PostMessage(WM_QUIT);
	}

	if (!m_Processes.Start())
	{
		MessageBox(_T("Can't init tasks"), _T("KLGuard v2"), MB_ICONERROR);
		PostMessage(WM_QUIT);
	}

	ModifyTrayIcon(this->m_hWnd, IDI_ICON_KL);

	RestorePosition();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKLGuard2Dlg::OnPaint() 
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
HCURSOR CKLGuard2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CKLGuard2Dlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_Processes.Stop();
	gpProcList->StopLoop();

	KillTimer(_TIMER_MAINWND);
	KillTimer(_TIMER_TRAYBLINK);
	KillTimer(_TIMER_TIMERSTEP);

	TrayMessage(this->m_hWnd, NIF_MESSAGE, NIM_DELETE, _TRAY_ICON_ID, NULL, NULL);

	SavePosition();

	//! is need to free this resource
/*	delete m_pSandBox;
	delete m_pKnownProcesses;
	delete m_pGeneriSettings;*/

	return CDialog::DestroyWindow();
}

BOOL CKLGuard2Dlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(LOWORD(wParam) == IDCANCEL)
	{
		ShowWindow(SW_HIDE);
		return TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CKLGuard2Dlg::OnOK() 
{
	// TODO: Add extra validation here
	ShowWindow(SW_HIDE);
	
	//CDialog::OnOK();
}

long CKLGuard2Dlg::OnTrayIocn(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_LBUTTONDOWN:
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
		break;
	case WM_RBUTTONDOWN:
		{
			CWnd* pWnd = GetForegroundWindow();
			SetForegroundWindow();
			
			POINT pt;
			HMENU hSysMenu, hSubMenu;
			
			hSysMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_TRAY));
			GetCursorPos(&pt);
			hSubMenu = GetSubMenu(hSysMenu, 0);

			//SetMenuDefaultItem(hSubMenu, TRAY_SHOW, MF_BYCOMMAND);

			TrackPopupMenu(hSubMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x,pt.y, 0, this->m_hWnd, NULL);
			
			DestroyMenu(hSysMenu);

			pWnd->SetForegroundWindow();
		}
		break;
	}

	return TRUE;
}

void CKLGuard2Dlg::OnExit() 
{
	// TODO: Add your command handler code here
	PostMessage(WM_QUIT);	
}

void CKLGuard2Dlg::OnDblclkListProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UINT selcount = m_lProcesses.GetSelectedCount();
	if (selcount == 0)
		return;

	UINT idx = m_lProcesses.GetNextItem(-1, LVIS_SELECTED);
	SYSTEMTIME Systime = *((LPSYSTEMTIME) m_lProcesses.GetItemData(idx));

	_pProcList* pList = gpProcList->AcquireProcList();

	CSingleProcess* pSingle = gpProcList->FindProcessByTime(&Systime);
	CString strtmp;
	if (pSingle == NULL)
		gpProcList->ReleaseProcList();
	else
	{
		_StrHistory DrvRegHistory = pSingle->m_DrvRegHistory.m_History;
		_StrHistory DrvFileHistory = pSingle->m_DrvFileHistory.m_History;
		_StrHistory DrvSelfHistory = pSingle->m_DrvSelfHistory.m_History;

		strtmp.Format(_T("%#x (%d) '%s'"), pSingle->m_ProcessId, pSingle->m_ProcessId, pSingle->m_ProcName);
		CDHistory DHistory(pSingle->m_ProcessId, &strtmp, pSingle->GetImagePath(), &DrvRegHistory, &DrvFileHistory, &DrvSelfHistory, this);

		gpProcList->ReleaseProcList();

		DHistory.DoModal();
	}

	*pResult = 0;
}

//+ ------------------------------------------------------------------------------------------
long CKLGuard2Dlg::OnHyperLink(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case WU_SAND_BOX:
		{
			CDSandBoxSettings DSandBoxSettings(gpProcList->GetSandBoxSettings(), this);
			DSandBoxSettings.DoModal();
		}
		break;
	case WU_KNOWNPROC:
		{
			CDKnownProcesses DKnownProcesses(&gpProcList->m_KnownProcessList, this);
			DKnownProcesses.DoModal();
		}
		break;
	case WU_ABOUT:
		{
			CDAbout DAbout(this);
			DAbout.DoModal();
		}
		break;
	case WU_GENERIC_SETTINGS:
		{
			CDGenericSettings DGenericSettings(gpProcList->GetGenericSettings(), this);
			DGenericSettings.DoModal();
		}
		break;
	}
	return 0;
}

void CKLGuard2Dlg::OnCheckEducationmode() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	gpProcList->GetGenericSettings()->m_bEducationMode = !!m_chEducationMode;
	gpProcList->GetGenericSettings()->GS_SaveSettings();
}

void CKLGuard2Dlg::OnCheckStartKnownProcesses() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	gpProcList->GetGenericSettings()->m_bStartOnlyKnownProcesses = !!m_chStartKnownProcesses;
	gpProcList->GetGenericSettings()->GS_SaveSettings();
}

void CKLGuard2Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_lProcesses.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on										Relative-to				Offset
			// ------    ------										-----------				------
			{lMOVE,		lRIGHT(IDOK),								lRIGHT(lPARENT)			, -3},
			{lMOVE,		lBOTTOM(IDOK),								lBOTTOM(lPARENT)		, -3},
			{lMOVE,		lRIGHT(WU_ABOUT),							lRIGHT(lPARENT)			, -3},

			{lMOVE,		lBOTTOM(IDC_CHECK_START_KNOWN_PROCESSES),	lBOTTOM(lPARENT)		, -4},
			{lMOVE,		lBOTTOM(WU_KNOWNPROC),						lBOTTOM(lPARENT)		, -3},
			{lMOVE,		lBOTTOM(IDC_CHECK_EDUCATIONMODE),			lBOTTOM(lPARENT)		, -4},

			{lSTRETCH,	lRIGHT(IDC_LIST_PROCESSES),					lRIGHT(lPARENT)			, -3},
			{lSTRETCH,	lBOTTOM(IDC_LIST_PROCESSES),				lTOP(IDOK)				, -5},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}	
}

void CKLGuard2Dlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI)
{
	CWnd::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = 640;
	lpMMI->ptMinTrackSize.y = 480;
}

void CKLGuard2Dlg::SavePosition()
{
	//	TRACE("\nOwner save pos\n");
	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);

	g_pReestr->SetBinaryValue(_T("Position"), (DWORD)&wpl, sizeof(wpl));

	CListColumnPositions list_columns(_T("Processes_List"), &m_lProcesses);
	list_columns.SavePositions();
}

void CKLGuard2Dlg::RestorePosition()
{
	WINDOWPLACEMENT wpl;
	UINT dw = 0;

	if (g_pReestr->GetBinaryValue(_T("Position"), (DWORD) &wpl, sizeof(wpl)))
		SetWindowPlacement(&wpl);

	CListColumnPositions list_columns(_T("Processes_List"), &m_lProcesses);
	list_columns.RestorePositions();
}
//+ ----------------------------------------------------------------------------------------
void CKLGuard2Dlg::TimerMain()
{
	if (gpProcList->m_JobSandBoxWarnings.IsExistEvents())
		gpProcList->m_JobSandBoxWarnings.ShowWarnigns();

	unsigned int mod_flags = gpProcList->GetModifiedFlags();
	if (mod_flags == _PROCLIST_MODIFIED_NONE)
		return;

	int cou = 0;
	CString strtmp;

	_pProcList* pList = gpProcList->AcquireProcList();
	
	if (mod_flags & _PROCLIST_MODIFIED_ITEMS)
	{
		m_lProcesses.DeleteAllItems();
	
	//+ ------------------------------------------------------------------------------------------
		for (_pProcList::iterator _it = pList->begin(); _it != pList->end(); ++_it)
		{
			CSingleProcess* pSingle = *_it;
			
			if (pSingle->m_SysTime.wYear == 1601)
				strtmp = _T("n/a");
			else
				strtmp.Format(_T("%02d-%02d %02d:%02d:%02d"), pSingle->m_SysTime.wMonth, pSingle->m_SysTime.wDay,
					pSingle->m_SysTime.wHour, pSingle->m_SysTime.wMinute, pSingle->m_SysTime.wSecond);

			m_lProcesses.InsertItem(LVIF_TEXT, cou, strtmp, 0, 0, 0, 0);
			
			m_lProcesses.SetItemText(cou, _L_Proc, pSingle->m_ProcName);
			
			strtmp.Format(_T("%#6x (%4d) %s"), pSingle->m_ProcessId, pSingle->m_ProcessId,
				pSingle->m_bAlive == true ? _T("alive") : _T(""));

			m_lProcesses.SetItemText(cou, _L_PID, strtmp);
			m_lProcesses.SetItemData(cou, (DWORD) &pSingle->m_SysTime);

			strtmp.Format(_T("%#6x (%4d)"), pSingle->m_ParentId, pSingle->m_ParentId);
			m_lProcesses.SetItemText(cou, _L_ParentID, strtmp);

			strtmp.Format(_T("%s"), pSingle->m_bHasInvisibleWindows == true ? _T("YES") : _T(""));
			m_lProcesses.SetItemText(cou, _L_HasInvisibleWindow, strtmp);

			m_lProcesses.SetItemText(cou, _L_ImagePath, *pSingle->GetImagePath());

			strtmp.Format(_T("%d"), pSingle->m_DrvFileHistory.m_EventsCount);
			m_lProcesses.SetItemText(cou, _L_Files, strtmp);

			strtmp.Format(_T("%d"), pSingle->m_DrvRegHistory.m_EventsCount);
			m_lProcesses.SetItemText(cou, _L_SysReg, strtmp);

			cou++;
		}
	}
	else
	{
		for (_pProcList::iterator _it = pList->begin(); _it != pList->end(); ++_it)
		{
			CSingleProcess* pSingle = *_it;
			
			strtmp.Format(_T("%s"), pSingle->m_bHasInvisibleWindows == true ? _T("YES") : _T(""));
			m_lProcesses.SetItemText(cou, _L_HasInvisibleWindow, strtmp);

			m_lProcesses.SetItemText(cou, _L_ImagePath, *pSingle->GetImagePath());

			strtmp.Format(_T("%d"), pSingle->m_DrvFileHistory.m_EventsCount);
			m_lProcesses.SetItemText(cou, _L_Files, strtmp);

			strtmp.Format(_T("%d"), pSingle->m_DrvRegHistory.m_EventsCount);
			m_lProcesses.SetItemText(cou, _L_SysReg, strtmp);

			cou++;
		}
	}
//+ ------------------------------------------------------------------------------------------
	gpProcList->SetModifiedFlag(_PROCLIST_MODIFIED_NONE);
	gpProcList->ReleaseProcList();
}

void CKLGuard2Dlg::TimerTrayBlink()
{
	if (InterlockedExchange(&m_TrayBlinkCounter, m_TrayBlinkCounter) == 0)
		return;

	LONG current = InterlockedDecrement(&m_TrayBlinkCounter);

	if (current % 2)
		TrayMessage(this->m_hWnd, NIF_ICON, NIM_MODIFY, _TRAY_ICON_ID,
		(HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_ALERT), IMAGE_ICON, 16, 16, 0), _APP_NAME);
	else
		TrayMessage(this->m_hWnd, NIF_ICON, NIM_MODIFY, _TRAY_ICON_ID,
			(HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_KL), IMAGE_ICON, 16, 16, 0), _APP_NAME);
}

void CKLGuard2Dlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case _TIMER_MAINWND:
		TimerMain();
		break;
	case _TIMER_TRAYBLINK:
		TimerTrayBlink();
		break;
	case _TIMER_TIMERSTEP:
		gpProcList->TimeoutStep();
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

long CKLGuard2Dlg::OnAlert(WPARAM wParam, LPARAM lParam)
{
	if (InterlockedExchange(&m_TrayBlinkCounter, m_TrayBlinkCounter) < _tray_blink_count * 2)
		InterlockedExchangeAdd(&m_TrayBlinkCounter, _tray_blink_count * 2);

	return 0;
}
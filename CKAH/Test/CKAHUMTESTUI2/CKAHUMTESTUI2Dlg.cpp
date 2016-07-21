// CKAHUMTESTUI2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "CKAHUMTESTUI2Dlg.h"
#include "PacketRulesDlg.h"
#include "ApplicationRuleDlg.h"
#include "ApplicationRulesDlg.h"
#include "CRConsoleDlg.h"
#include "BanometDlg.h"
#include "HostStatDlg.h"
#include "ActiveConnDlg.h"
#include "winsock.h"
#include <atlconv.h>
#include <stuff/StdString.h>
#include "Utils.h"
#include <CKAH/ckahiptoa.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//static CCKAHRegStorageImpl g_RegStorageImpl (HKEY_LOCAL_MACHINE, L"SOFTWARE\\KasperskyLab\\Components\\10a");

static bool g_LoggerEnabled = true;

#define WM_USER_DELAYEDLOG (WM_USER + 0x100)
#define WM_USER_SHOWCREATERULEDLG (WM_USER + 0x101)
#define WM_USER_DELAYEDTRACE (WM_USER + 0x102)

static void CALLBACK Logger (CKAHUM::LogLevel Level, LPCSTR szString)
{
	if(!g_LoggerEnabled)
		return;

	TCHAR szLogString[1024];
	szLogString[1023] = _T('\0');
	_sntprintf(szLogString, 1023, _T("[%s] %s"), Level == CKAHUM::lError ? _T("E") : (Level == CKAHUM::lWarning ? _T("W") : _T("I")), szString);
    if (AfxGetApp()->m_pMainWnd)
        AfxGetApp()->m_pMainWnd->PostMessage (WM_USER_DELAYEDTRACE, (WPARAM)_tcsdup (szLogString), 0);
    else
        ASSERT(0);
	
#ifdef _DEBUG
	OutputDebugString(szLogString);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CCKAHUMTESTUI2Dlg dialog

CCKAHUMTESTUI2Dlg::CCKAHUMTESTUI2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCKAHUMTESTUI2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCKAHUMTESTUI2Dlg)
	m_ReloadPath = _T("o:\\ckah\\ckah.set");
	m_SetParamID = 0;
	m_SetParamNum = 0;
	m_SetParamValue = 0;
	m_GetParamID = 0;
	m_GetParamNum = 0;
	m_GetParamCountID = 0;
	m_Deny = TRUE;
	m_Notify = FALSE;
	m_Log = TRUE;
	m_Ban = FALSE;
	m_SetIDSParamID = 0;
	m_BanTime = 3600;
	m_nCRConsolePort = 110;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hRules = NULL;
	m_hConnectionList = NULL;
	m_nLastSliderPos = 0;

#ifdef _UNICODE
	CHAR szPath[] = "o\0:\0\\\0\x56\x52\x3d\x5e\xa1\x4e\xa1\x4e\xa1\x4e\\\0c\0k\0a\0h\0.\0s\0e\0t\0\0";
	m_ReloadPath = (WCHAR *)szPath;
#else
	HKEY hKey = NULL;

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, _T("SOFTWARE\\KasperskyLab\\Components\\10a\\LastSet"),
			0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		TCHAR szDirectory[MAX_PATH];

		DWORD dwType, dwSize = sizeof (szDirectory);
		if (RegQueryValueEx (hKey, _T("Directory"), NULL, &dwType, (LPBYTE)szDirectory, &dwSize) == ERROR_SUCCESS &&
				dwType == REG_SZ)
		{
			m_ReloadPath = szDirectory;
			if (!m_ReloadPath.IsEmpty () && m_ReloadPath[m_ReloadPath.GetLength () - 1] != _T('\\'))
				m_ReloadPath += _T('\\');
			m_ReloadPath += _T("ckah.set");
		}

		RegCloseKey (hKey);
	}
#endif
}

CCKAHUMTESTUI2Dlg::~CCKAHUMTESTUI2Dlg ()
{
	if (m_hRules)
		CKAHFW::PacketRules_Delete (m_hRules);
	if (m_hConnectionList)
		CKAHFW::ConnectionList_Delete (m_hConnectionList);

	g_LoggerEnabled = false;

    CKAHUM::Deinitialize(Logger);
}

void CCKAHUMTESTUI2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCKAHUMTESTUI2Dlg)
	DDX_Control(pDX, IDC_TRACELOG, m_TraceLog);
	DDX_Control(pDX, IDC_FWLEVEL, m_FirewallLevelSlider);
	DDX_Control(pDX, IDC_OpenConnectionList, m_OpenConnectionListCtrl);
	DDX_Control(pDX, IDC_OpenPortList, m_OpenPortListCtrl);
	DDX_Control(pDX, IDC_NETLIST, m_NetListCtrl);
	DDX_Control(pDX, IDC_IPADDRESS2, m_UnbanMaskCtrl);
	DDX_Control(pDX, IDC_IPADDRESS1, m_UnbanIPCtrl);
	DDX_Control(pDX, IDC_ATTACKLIST, m_AttackList);
	DDX_Text(pDX, IDC_RELOADPATH, m_ReloadPath);
	DDX_Text(pDX, IDC_SetParamID, m_SetParamID);
	DDX_Text(pDX, IDC_SetParamNum, m_SetParamNum);
	DDX_Text(pDX, IDC_SetParamValue, m_SetParamValue);
	DDX_Text(pDX, IDC_GetParamID, m_GetParamID);
	DDX_Text(pDX, IDC_GetParamNum, m_GetParamNum);
	DDX_Text(pDX, IDC_GetParamCountID, m_GetParamCountID);
	DDX_Check(pDX, IDC_SetIDSPar_Deny, m_Deny);
	DDX_Check(pDX, IDC_SetIDSPar_Notify, m_Notify);
	DDX_Check(pDX, IDC_SetIDSPar_Log, m_Log);
	DDX_Check(pDX, IDC_SetIDSPar_Ban, m_Ban);
	DDX_Text(pDX, IDC_SetIDSParamID, m_SetIDSParamID);
	DDX_Text(pDX, IDC_EDIT1, m_BanTime);
	DDX_Text(pDX, IDC_CR_CONSOLE_PORT, m_nCRConsolePort);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCKAHUMTESTUI2Dlg, CDialog)
	//{{AFX_MSG_MAP(CCKAHUMTESTUI2Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_RELOAD, OnReload)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_RESUME, OnResume)
	ON_BN_CLICKED(IDC_SETPARAM, OnSetparam)
	ON_BN_CLICKED(IDC_GETPARAM, OnGetparam)
	ON_BN_CLICKED(IDC_GETPARAMCOUNT, OnGetparamcount)
	ON_BN_CLICKED(IDC_Clear, OnClear)
	ON_BN_CLICKED(IDC_SETIDSPARAM, OnSetidsparam)
	ON_BN_CLICKED(IDC_Unban, OnUnban)
	ON_BN_CLICKED(IDC_STM_START, OnStmStart)
	ON_BN_CLICKED(IDC_STM_STOP, OnStmStop)
	ON_BN_CLICKED(IDC_STM_Pause, OnSTMPause)
	ON_BN_CLICKED(IDC_STM_Resume, OnSTMResume)
	ON_BN_CLICKED(IDC_FW_START, OnFwStart)
	ON_BN_CLICKED(IDC_FW_STOP, OnFwStop)
	ON_BN_CLICKED(IDC_FW_PAUSE, OnFwPause)
	ON_BN_CLICKED(IDC_FW_RESUME, OnFwResume)
	ON_BN_CLICKED(IDC_CKAHUMPAUSE, OnCkahumpause)
	ON_BN_CLICKED(IDC_CKAHUMRESUME, OnCkahumresume)
	ON_BN_CLICKED(IDC_UpdateNetList, OnUpdateNetList)
	ON_BN_CLICKED(IDC_UpdateOpenPortList, OnUpdateOpenPortList)
	ON_BN_CLICKED(IDC_UpdateOpenConnectionList, OnUpdateOpenConnectionList)
	ON_BN_CLICKED(IDC_BreakConnection, OnBreakConnection)
	ON_BN_CLICKED(IDC_FW_PACKETRULES, OnFwPacketrules)
	ON_BN_CLICKED(IDC_FW_APPRULES, OnFwApprules)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CKAHUMENABLE, OnCkahumenable)
	ON_BN_CLICKED(IDC_CKAHUMDISABLE, OnCkahumdisable)
	ON_MESSAGE(WM_USER_DELAYEDLOG, OnDelayedLog)
	ON_MESSAGE(WM_USER_DELAYEDTRACE, OnDelayedTrace)
	ON_MESSAGE(WM_USER_SHOWCREATERULEDLG, OnShowCreateRuleDlg)
	ON_BN_CLICKED(IDC_CR_START, OnCrStart)
	ON_BN_CLICKED(IDC_CR_STOP, OnCrStop)
	ON_BN_CLICKED(IDC_CR_Pause, OnCRPause)
	ON_BN_CLICKED(IDC_CR_Resume, OnCRResume)
	ON_BN_CLICKED(IDC_CR_Console, OnCRConsole)
	ON_BN_CLICKED(IDC_Clear2, OnClear2)
	ON_BN_CLICKED(IDC_GetBannedList, OnGetBannedList)
	ON_BN_CLICKED(IDC_HOST_STAT, OnHostStat)
	ON_BN_CLICKED(IDC_ConnList, OnConnList)
	ON_BN_CLICKED(IDC_CR_Watchdog, OnCRWatchdog)
	ON_BN_CLICKED(IDC_CR_Watchdog2, OnCRExcludes)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_StmChange, OnStmChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCKAHUMTESTUI2Dlg message handlers

VOID CALLBACK OnNetChangeCallback (LPVOID lpContext)
{
	CCKAHUMTESTUI2Dlg *pThis = (CCKAHUMTESTUI2Dlg *)lpContext;

	pThis->UpdateNetList ();
}

BOOL CCKAHUMTESTUI2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_UnbanIPCtrl.SetAddress (0);
	m_UnbanMaskCtrl.SetAddress (0);

	CKAHUM::Initialize (HKEY_LOCAL_MACHINE, L"SOFTWARE\\KasperskyLab\\protected\\AVP7\\CKAHUM", Logger);

	UpdateNetList ();
	CKAHFW::SetNetworkChangeCallback (OnNetChangeCallback, this);
	
//	m_OpenPortListCtrl.SetHorizontalExtent (1000);
//	m_OpenConnectionListCtrl.SetHorizontalExtent (1000);

	m_FirewallLevelSlider.SetRange (0, 4);
	m_FirewallLevelSlider.SetPos (m_nLastSliderPos = (int)CKAHFW::wmAllowNotFiltered);
	m_FirewallLevelSlider.EnableWindow (FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCKAHUMTESTUI2Dlg::OnPaint() 
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
HCURSOR CCKAHUMTESTUI2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCKAHUMTESTUI2Dlg::SetResult (CKAHUM::OpResult result)
{
	CString s;

	bool bBeep = true;

	switch (result)
	{
	case CKAHUM::srOK:
		s = "srOK";
		bBeep = false;
		break;
	case CKAHUM::srNeedReload:
		s = "srNeedReload";
		break;
	case CKAHUM::srInvalidArg:
		s = "srInvalidArg";
		break;
	case CKAHUM::srManifestNotFound:
		s = "srManifestNotFound";
		break;
	case CKAHUM::srWrongManifestFormat:
		s = "srWrongManifestFormat";
		break;
	case CKAHUM::srErrorReadingPersistentManifest:
		s = "srErrorReadingPersistentManifest";
		break;
	case CKAHUM::srNeedReboot:
		s = "srNeedReboot";
		break;
	case CKAHUM::srNotStarted:
		s = "srNotStarted";
		break;
	case CKAHUM::srAlreadyStarted:
		s = "srAlreadyStarted";
		break;
	case CKAHUM::srOpFailed:
		s = "srOpFailed";
		break;
	case CKAHUM::srNoPersistentManifest:
		s = "srNoPersistentManifest";
		break;
	default:
		s = "Unknown";
	}

	SetDlgItemText (IDC_Result, s);
	if (bBeep)
		MessageBeep (-1);
}

void CALLBACK AttackNotifyCallback (LPVOID lpCallbackParam, const CKAHIDS::AttackNotify *pAttackNotify)
{
	CString strMessage;

	FILETIME ft;
	SYSTEMTIME st;
	TCHAR szTime[0x100];
	
	FileTimeToLocalFileTime ((FILETIME *)&pAttackNotify->Time, &ft);
	FileTimeToSystemTime (&ft, &st);

	GetTimeFormat (LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 0x100);

	strMessage.Format (_T("[%s] 0x%08x, %ls, 0x%08x, %hs, %d, 0x%08x"), szTime,
						pAttackNotify->AttackID,
						pAttackNotify->AttackDescription,
						pAttackNotify->Proto,
						CKAHUM_IPTOA(pAttackNotify->AttackerIP),
						pAttackNotify->LocalPort,
						pAttackNotify->ResponseFlags);

	CCKAHUMTESTUI2Dlg *pThis = (CCKAHUMTESTUI2Dlg *)lpCallbackParam;

	pThis->AddStringToLog (strMessage);
}

void CCKAHUMTESTUI2Dlg::OnStart() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHIDS::Start (AttackNotifyCallback, this);
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnReload() 
{
	USES_CONVERSION;

	UpdateData ();
	ClearResult ();
	CKAHUM::OpResult result = CKAHUM::Reload (T2W((LPTSTR)(LPCTSTR)m_ReloadPath));
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCkahumenable() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHUM::EnableBaseDrivers (true);
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCkahumdisable() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHUM::EnableBaseDrivers (false);
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnStop() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHIDS::Stop ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnPause() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHIDS::Pause ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnResume() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHIDS::Resume ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnSetparam() 
{
	UpdateData ();

	ClearResult ();
	CKAHUM::OpResult result = CKAHIDS::SetIDSParam (m_SetParamID, m_SetParamNum, m_SetParamValue);
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnGetparam() 
{
	UpdateData ();

	ClearResult ();
	ULONG Value = 0;
	CKAHUM::OpResult result = CKAHIDS::GetIDSParam (m_GetParamID, m_GetParamNum, &Value);
	SetResult (result);
	if (result == CKAHUM::srOK)
	{
		SetDlgItemInt (IDC_GetParamValue, Value, FALSE);
	}
}

void CCKAHUMTESTUI2Dlg::OnGetparamcount() 
{
	UpdateData ();

	ClearResult ();
	ULONG Value = 0;
	CKAHUM::OpResult result = CKAHIDS::GetIDSParamCount (m_GetParamCountID, &Value);
	SetResult (result);
	if (result == CKAHUM::srOK)
	{
		SetDlgItemInt (IDC_GetParamCountValue, Value, FALSE);
	}
}

void CCKAHUMTESTUI2Dlg::OnSetidsparam() 
{
	UpdateData ();

	ClearResult ();
	ULONG ResponseFlags = (m_Deny ? FLAG_ATTACK_DENY : 0) |
							(m_Notify ? FLAG_ATTACK_NOTIFY : 0) |
							(m_Log ? FLAG_ATTACK_LOG : 0) |
							(m_Ban ? FLAG_ATTACK_BAN :0);
	CKAHUM::OpResult result = CKAHIDS::SetIDSAttackNotifyParam (m_SetIDSParamID, ResponseFlags, m_BanTime);
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::ClearResult ()
{
	SetDlgItemText (IDC_Result, _T(""));
}

void CCKAHUMTESTUI2Dlg::OnClear() 
{
	m_AttackList.ResetContent ();	
}

void CCKAHUMTESTUI2Dlg::OnUnban() 
{
	UpdateData ();
	ClearResult ();
	CKAHUM::IP ip(0), mask(0);
    m_UnbanIPCtrl.GetAddress (ip.v4);
	m_UnbanMaskCtrl.GetAddress (mask.v4);
    CKAHUM::OpResult result = CKAHIDS::UnbanHost (&ip, &mask);
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnStmStart() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHSTM::Start ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnStmStop() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHSTM::Stop ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnSTMPause() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHSTM::Pause ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnSTMResume() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHSTM::Resume ();
	SetResult (result);
}

void CALLBACK PacketRuleCallback (LPVOID lpContext, const CKAHFW::PacketRuleNotify *pNotify)
{
	CString strMessage;

	FILETIME ft;
	SYSTEMTIME st;
	TCHAR szTime[0x100];
	
	FileTimeToLocalFileTime ((FILETIME *)&pNotify->Time, &ft);
	FileTimeToSystemTime (&ft, &st);

	GetTimeFormat (LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 0x100);

	CString From (CKAHUM_IPTOA (pNotify->LocalAddress)), To (CKAHUM_IPTOA (pNotify->RemoteAddress)), Proto;
	if (pNotify->Proto == 6 || pNotify->Proto == 17)
	{
		From.Format (_T("%s:%d"), CString (From), pNotify->TCPUDPLocalPort);
		To.Format (_T("%s:%d"), CString (To), pNotify->TCPUDPRemotePort);
	}

	if (pNotify->Proto == 6)
		Proto = _T("TCP");
	else if (pNotify->Proto == 17)
		Proto = _T("UDP");
	else if (pNotify->Proto == 1)
		Proto.Format (_T("ICMP (%d,%d)"), pNotify->ICMPType, pNotify->ICMPCode);
	else
		Proto.Format (_T("proto %d"), pNotify->Proto);

	strMessage.Format (_T("[%s %c%c] %s, %s %s %s %s packet %s by '%ls' rule"), szTime, 
		pNotify->Notify ? _T('n') : _T(' '), pNotify->Log ? _T('l') : _T(' '),
		Proto, From, pNotify->IsIncoming ? _T("<-") : _T("->"), To,
        pNotify->IsStream ? (pNotify->IsStreamIncoming ? _T("(<<)") : _T("(>>)")) : _T("(\?\?)"), 
		pNotify->IsBlocking ? _T("blocked") : _T("allowed"), pNotify->RuleName);

	CCKAHUMTESTUI2Dlg *pThis = (CCKAHUMTESTUI2Dlg *)lpContext;

	pThis->AddStringToLog (strMessage);
}

CKAHFW::RuleCallbackResult ProcessPopup (const CKAHFW::ApplicationRuleNotify *pNotify)
{
	CKAHFW::RuleCallbackResult result = CKAHFW::crPass;

	CString strNotify;

	CString From, To;
	From.Format (_T("%s:%d"), CKAHUM_IPTOA (pNotify->LocalAddress), pNotify->LocalPort);
	To.Format (_T("%s:%d"), CKAHUM_IPTOA (pNotify->RemoteAddress), pNotify->RemotePort);

	strNotify.Format (_T("%ls(%ls): %s %s %s %s %s"),
		pNotify->ApplicationName, pNotify->CommandLine, 
		From, pNotify->IsIncoming ? _T("<-") : _T("->"), To,
        pNotify->IsStream ? (pNotify->IsStreamIncoming ? _T("(<<)") : _T("(>>)")) : _T("(\?\?)"), 
		pNotify->Proto == CKAHFW::PROTO_TCP ? _T("connection") : ((pNotify->Proto == CKAHFW::PROTO_UDP) ? _T("datagram") : _T("?")));

	CString strMessage (strNotify);
	strMessage += _T("\n\nCreate application rule?");

//	if (AfxMessageBox (strMessage, MB_YESNO | MB_SYSTEMMODAL) == IDYES)
	if (MessageBox (NULL, strMessage, "Application notification", MB_YESNO | MB_SYSTEMMODAL) == IDYES)
	{
		result = (CKAHFW::RuleCallbackResult)AfxGetMainWnd ()->SendMessage (WM_USER_SHOWCREATERULEDLG, (WPARAM)pNotify);
	}
	else
	{
		CString strMessage;
		strMessage.Format (_T("%s\n\nAllow %s?"), strNotify, pNotify->Proto == CKAHFW::PROTO_TCP ? _T("connection") : ((pNotify->Proto == CKAHFW::PROTO_UDP) ? _T("datagram") : _T("?")));
//		result = AfxMessageBox (strMessage, MB_YESNO | MB_SYSTEMMODAL) == IDYES ? CKAHFW::crPass : CKAHFW::crBlock;
		result = MessageBox (NULL, strMessage, "Application notification", MB_YESNO | MB_SYSTEMMODAL) == IDYES ? CKAHFW::crPass : CKAHFW::crBlock;
	}

	return result;
}

LRESULT CCKAHUMTESTUI2Dlg::OnShowCreateRuleDlg (WPARAM wParam, LPARAM lParam)
{
	const CKAHFW::ApplicationRuleNotify *pNotify = (const CKAHFW::ApplicationRuleNotify *)wParam;
	CKAHFW::RuleCallbackResult result = CKAHFW::crBlock;

	CKAHFW::HAPPRULE hRule = CKAHFW::ApplicationRule_Create ();

	if (hRule)
	{
		GUID guid;

		if (CoCreateGuid (&guid) == S_OK)
		{
			wchar_t wszGuid[50];
			if (StringFromGUID2 (guid, wszGuid, 50))
				CKAHFW::ApplicationRule_SetName (hRule, wszGuid);
		}

		CKAHFW::ApplicationRule_SetApplicationName (hRule, pNotify->ApplicationName);
		CKAHFW::ApplicationRule_SetIsBlocking (hRule, false);

		CKAHFW::HAPPRULEELEMENT hElem = CKAHFW::ApplicationRuleElement_Create ();
		if (hElem)
		{
			CKAHFW::ApplicationRuleElement_SetIsActive (hElem, true);
			CKAHFW::ApplicationRule_AddElementToBack (hRule, hElem);
			CKAHFW::ApplicationRuleElement_Delete (hElem);
		}

		CApplicationRuleDlg ApplicationRuleDlg (hRule);

		if (ApplicationRuleDlg.DoModal () == IDOK)
		{
			CKAHFW::HAPPRULES hRules = NULL;
			CKAHFW::GetApplicationRules (&hRules);
			CKAHFW::ApplicationRules_AddItemToFront (hRules, ApplicationRuleDlg.m_hRule);
			CKAHFW::SetApplicationRules (hRules);
			CKAHFW::ApplicationRules_Delete (hRules);

			bool bIsBlocking = true;
			CKAHFW::ApplicationRule_GetIsBlocking (ApplicationRuleDlg.m_hRule, &bIsBlocking);
			if (!bIsBlocking)
				result = CKAHFW::crPass;
		}

		CKAHFW::ApplicationRule_Delete (hRule);
	}
	
	return (LRESULT)result;
}

CKAHFW::RuleCallbackResult CALLBACK ApplicationRuleCallback (LPVOID lpContext, const CKAHFW::ApplicationRuleNotify *pNotify)
{
	CKAHFW::RuleCallbackResult result = CKAHFW::crPass;

	CString strMessage;

	FILETIME ft;
	SYSTEMTIME st;
	TCHAR szTime[0x100];
	
	FileTimeToLocalFileTime ((FILETIME *)&pNotify->Time, &ft);
	FileTimeToSystemTime (&ft, &st);

	GetTimeFormat (LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 0x100);

	CString From, To;
	From.Format (_T("%s:%d"), CKAHUM_IPTOA (pNotify->LocalAddress), pNotify->LocalPort);
	To.Format (_T("%s:%d"), CKAHUM_IPTOA (pNotify->RemoteAddress), pNotify->RemotePort);

	if (pNotify->Popup)
	{
		strMessage.Format (_T("[%s] %ls: %s %s %s %s %s?.."), szTime,
			pNotify->ApplicationName,
			From, pNotify->IsIncoming ? _T("<-") : _T("->"), To,
            pNotify->IsStream ? (pNotify->IsStreamIncoming ? _T("(<<)") : _T("(>>)")) : _T("(\?\?)"), 
			pNotify->Proto == CKAHFW::PROTO_TCP ? _T("connection") : ((pNotify->Proto == CKAHFW::PROTO_UDP) ? _T("datagram") : _T("?")));

		if (!strMessage.IsEmpty ())
		{
			CCKAHUMTESTUI2Dlg *pThis = (CCKAHUMTESTUI2Dlg *)lpContext;
			pThis->AddStringToLog (strMessage);
		}

		result = ProcessPopup (pNotify);

		if (!strMessage.IsEmpty ())
		{
			strMessage += (result == CKAHFW::crPass) ? _T(" passed") : _T(" blocked");

			CCKAHUMTESTUI2Dlg *pThis = (CCKAHUMTESTUI2Dlg *)lpContext;
			pThis->AddStringToLog (strMessage);
		}
	}
	else
	{
		strMessage.Format (_T("[%s %c%c] %s %s %s %s %s %s by '%ls' rule"), szTime,
			pNotify->Notify ? _T('n') : _T(' '), pNotify->Log ? _T('l') : _T(' '),
			From, pNotify->IsIncoming ? _T("<-") : _T("->"), To,
            pNotify->IsStream ? (pNotify->IsStreamIncoming ? _T("(<<)") : _T("(>>)")) : _T("(\?\?)"), 
			pNotify->Proto == 6 ? _T("connection") : ((pNotify->Proto == 17) ? _T("datagram") : _T("?")),
			pNotify->IsBlocking ? _T("blocked") : _T("allowed"), pNotify->RuleName);

		if (!strMessage.IsEmpty ())
		{
			CCKAHUMTESTUI2Dlg *pThis = (CCKAHUMTESTUI2Dlg *)lpContext;
			pThis->AddStringToLog (strMessage);
		}
	}
	
	return result;
}
/*
struct HashThreadParam
{
	CString wszAppPath;
	UCHAR Hash[FW_HASH_SIZE];
};

static DWORD CALLBACK AsyncUpdateRulesWithNewHashThread(LPVOID lpParameter)
{
	HashThreadParam *pParam = (HashThreadParam *)lpParameter;

	CKAHFW::HAPPRULES hRules = NULL;
	
	CKAHUM::OpResult result = CKAHFW::GetApplicationRules (&hRules);

	bool bNeedSave = false;

	if (result == CKAHUM::srOK)
	{
		int n = CKAHFW::ApplicationRules_GetCount (hRules);

		CStdStringW szLongAppPath, szRuleLongAppPath;
		GetLongPathName(CStdStringW(pParam->wszAppPath), szLongAppPath);
		
		for (int i = 0; i < n; ++i)
		{
			CKAHFW::HAPPRULE hRule = NULL;
			if (CKAHFW::ApplicationRules_GetItem (hRules, i, &hRule) != CKAHUM::srOK)
				continue;

			ULONG nSize = CKAHFW::ApplicationRule_GetApplicationName (hRule, NULL, 0);
			if (nSize)
			{
				WCHAR *pName = new WCHAR[nSize];
				if (pName)
				{
					if (CKAHFW::ApplicationRule_GetApplicationName (hRule, pName, nSize) == nSize - 1)
					{
						GetLongPathName(CStdStringW(pName), szRuleLongAppPath);
						if(szRuleLongAppPath.CompareNoCase(szLongAppPath) == 0)
						{
							CKAHFW::ApplicationRule_SetApplicationChecksum (hRule, pParam->Hash);
							bNeedSave = true;
						}
					}

					delete []pName;
				}
			}

			CKAHFW::ApplicationRule_Delete (hRule);
		}
	}

	if(bNeedSave)
		CKAHFW::SetApplicationRules (hRules);

	CKAHFW::ApplicationRules_Delete (hRules);

	delete pParam;

	return 0;
}

static void AsyncUpdateRulesWithNewHash(LPCWSTR szApplicationName, const UCHAR Hash[FW_HASH_SIZE])
{
	HashThreadParam *pParam = new HashThreadParam;
	if(!pParam)
	{
		ASSERT(0);
		return;
	}

	pParam->wszAppPath = szApplicationName;
	memcpy(pParam->Hash, Hash, FW_HASH_SIZE);
	CloseHandle(CreateThread(NULL, 0, AsyncUpdateRulesWithNewHashThread, pParam, 0, NULL));
}
*/
CKAHFW::RuleCallbackResult CALLBACK ChecksumChangedCallback (LPVOID lpContext, const CKAHFW::ChecksumChangedNotify *pNotify)
{
	CKAHFW::RuleCallbackResult result = CKAHFW::crPass;

	CString strMessage;

	FILETIME ft;
	SYSTEMTIME st;
	TCHAR szTime[0x100];
	
	FileTimeToLocalFileTime ((FILETIME *)&pNotify->Time, &ft);
	FileTimeToSystemTime (&ft, &st);

	GetTimeFormat (LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 0x100);

	strMessage.Format (_T("[%s] Checksum of '%ls [%d]' has been changed"), szTime, pNotify->ApplicationName, pNotify->PID);

	CCKAHUMTESTUI2Dlg *pThis = (CCKAHUMTESTUI2Dlg *)lpContext;
	pThis->AddStringToLog (strMessage);

	result = AfxMessageBox(strMessage + _T(". Allow its activity?"), MB_YESNO | MB_ICONQUESTION) == IDYES ? CKAHFW::crPass : CKAHFW::crBlock;

    if (result == CKAHFW::crPass)
    {
        result = AfxMessageBox(strMessage + _T(". Save new checksum?"), MB_YESNO | MB_ICONQUESTION) == IDYES ? CKAHFW::crPassModifyChecksum : CKAHFW::crPass;
    }

    strMessage.Format(_T("[%s] Activity of '%ls' is %s by user"), szTime, pNotify->ApplicationName, (result == CKAHFW::crPassModifyChecksum) ? _T("allowed/saved") : (result == CKAHFW::crPass) ? _T("allowed") : _T("blocked"));
	pThis->AddStringToLog (strMessage);
/*
	if(result == CKAHFW::crPass)
		AsyncUpdateRulesWithNewHash(pNotify->ApplicationName, pNotify->ApplicationNewChecksum);
*/
	return result;
}

void CCKAHUMTESTUI2Dlg::OnFwStart() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHFW::Start (PacketRuleCallback, this,
												ApplicationRuleCallback, this,
												ChecksumChangedCallback, this);
	if (result == CKAHUM::srOK || result == CKAHUM::srAlreadyStarted)
	{
		CKAHFW::WorkingMode wm = CKAHFW::wmAllowNotFiltered;
		if (CKAHFW::GetWorkingMode (&wm) == CKAHUM::srOK)
		{
			m_FirewallLevelSlider.SetPos (m_nLastSliderPos = (int)wm);
			m_FirewallLevelSlider.EnableWindow (TRUE);
		}
	}

	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnFwStop() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHFW::Stop ();
	m_FirewallLevelSlider.EnableWindow (FALSE);
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnFwPause() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHFW::Pause ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnFwResume() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHFW::Resume ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCkahumpause() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHUM::Pause ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCkahumresume() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHUM::Resume ();
	SetResult (result);
}

CKAHUM::OpResult CCKAHUMTESTUI2Dlg::UpdateNetList ()
{
	CKAHFW::HNETWORKLIST hNetList = NULL;
	CKAHUM::OpResult result = CKAHFW::GetNetworkList (&hNetList);
	
	m_NetListCtrl.ResetContent ();

	if (result == CKAHUM::srOK)
	{
		int size;

		if ((result = CKAHFW::NetworkList_GetSize (hNetList, &size)) == CKAHUM::srOK)
		{
			for (int j = 0; j < size; ++j)
			{
				CKAHFW::Network network;

				if ((result = CKAHFW::NetworkList_GetItem (hNetList, j, &network)) == CKAHUM::srOK)
				{
					CString s;
                    s.Format (_T("%s%s - %s"), 
                    network.HasIPv4 ? CKAHUM_IPNETTOA(CKAHUM::IP(network.IPv4), CKAHUM::IP(network.Maskv4)) : "",
                    network.HasIPv6 ? CKAHUM_IPNETTOA(CKAHUM::IP(network.IPv6), CKAHUM::IP(network.Maskv6)) : "",
                    network.AdapterDesc);

					m_NetListCtrl.AddString (s);
				}
			}
		}

		CKAHFW::NetworkList_Delete (hNetList);
	}

	return result;
}

void CCKAHUMTESTUI2Dlg::OnUpdateNetList() 
{
	ClearResult ();
	CKAHUM::OpResult result = UpdateNetList ();
	SetResult (result);
}

static CString GetProtoDescription (UCHAR proto)
{
	TCHAR szI[32];

	_itot (proto, szI, 10);

	CString strRet = szI;

	switch (proto)
	{
     case 1:
          strRet = "ICMP";
          break;
     case 2:
          strRet = "IGMP, RGMP";
          break;
     case 3:
          strRet = "GGP";
          break;
     case 4:
          strRet = "IP in IP encapsulation";
          break;
     case 6:
          strRet = "TCP";
          break;
     case 9:
          strRet = "IGRP";
          break;
     case 17:
          strRet = "UDP";
          break;
     case 47:
          strRet = "GRE";
          break;
     case 50:
          strRet = "ESP";
          break;
     case 51:
          strRet = "AH";
          break;
     case 53:
          strRet = "IP with Encryption";
          break;
	}

	return strRet;
}

CKAHUM::OpResult CCKAHUMTESTUI2Dlg::UpdateOpenPortList ()
{
	CKAHFW::HOPENPORTLIST hOpenPortList = NULL;
	CKAHUM::OpResult result = CKAHFW::GetOpenPortList (&hOpenPortList);
	
	m_OpenPortListCtrl.ResetContent ();

	if (result == CKAHUM::srOK)
	{
		int size;

		if ((result = CKAHFW::OpenPortList_GetSize (hOpenPortList, &size)) == CKAHUM::srOK)
		{
			for (int j = 0; j < size; ++j)
			{
				CKAHFW::OpenPort OpenPort;

				if ((result = CKAHFW::OpenPortList_GetItem (hOpenPortList, j, &OpenPort)) == CKAHUM::srOK)
				{
					CString s;
					s.Format (_T("%s, %s:%d (%I64d/%I64d, %d s), %ls %ls (%d)"), GetProtoDescription ((UCHAR)OpenPort.Proto), CKAHUM_IPTOA(OpenPort.LocalIP), 
								OpenPort.LocalPort, OpenPort.InBytes, OpenPort.OutBytes, OpenPort.ActiveTime, OpenPort.AppName, OpenPort.CmdLine, OpenPort.PID);

					m_OpenPortListCtrl.AddString (s);
				}
 			}
		}

		CKAHFW::OpenPortList_Delete (hOpenPortList);
	}

	return result;
}

void CCKAHUMTESTUI2Dlg::OnUpdateOpenPortList() 
{
	ClearResult ();
	CKAHUM::OpResult result = UpdateOpenPortList ();
	SetResult (result);
}

CKAHUM::OpResult CCKAHUMTESTUI2Dlg::UpdateOpenConnectionList ()
{
	if (m_hConnectionList)
	{
		CKAHFW::ConnectionList_Delete (m_hConnectionList);
		m_hConnectionList = NULL;
	}

	CKAHUM::OpResult result = CKAHFW::GetConnectionList (&m_hConnectionList);
	
	m_OpenConnectionListCtrl.ResetContent ();

	if (result == CKAHUM::srOK)
	{
		int size;

		if ((result = CKAHFW::ConnectionList_GetSize (m_hConnectionList, &size)) == CKAHUM::srOK)
		{
			for (int j = 0; j < size; ++j)
			{
				CKAHFW::Connection Connection;

				if ((result = CKAHFW::ConnectionList_GetItem (m_hConnectionList, j, &Connection)) == CKAHUM::srOK)
				{
					CString s;
					s.Format (_T("%s, %s:%d %s %s:%d(%I64d/%I64d, %d s), %ls %ls (%d)"), GetProtoDescription ((UCHAR)Connection.Proto),
								CKAHUM_IPTOA (Connection.LocalIP), Connection.LocalPort,
								Connection.IsIncoming ? _T("<-") : _T("->"),
								CKAHUM_IPTOA (Connection.RemoteIP), Connection.RemotePort,
                                Connection.InBytes, Connection.OutBytes, Connection.ActiveTime,
								Connection.AppName, Connection.CmdLine, Connection.PID);

					m_OpenConnectionListCtrl.AddString (s);
				}
			}
		}
	}

	return result;
}

void CCKAHUMTESTUI2Dlg::OnUpdateOpenConnectionList() 
{
	ClearResult ();
	CKAHUM::OpResult result = UpdateOpenConnectionList ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnBreakConnection() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHUM::srOpFailed;
	int nSelIndex = m_OpenConnectionListCtrl.GetCurSel ();
	
	if (nSelIndex != LB_ERR)
	{
		CKAHFW::Connection Connection;

		if (CKAHFW::ConnectionList_GetItem (m_hConnectionList, nSelIndex, &Connection) == CKAHUM::srOK)
		{
			result = CKAHFW::BreakConnection (Connection.Proto, &Connection.LocalIP,
										Connection.LocalPort, &Connection.RemoteIP, Connection.RemotePort);
		}
	}	
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnFwPacketrules() 
{
	ClearResult ();

	CKAHFW::HPACKETRULES hRules = NULL;
	
	CKAHUM::OpResult result = CKAHFW::GetPacketRules (&hRules);

	if (result == CKAHUM::srOK)
	{
		CPacketRulesDlg PacketRulesDlg (hRules, this);
		
		if (PacketRulesDlg.DoModal () == IDOK)
		{
			result = CKAHFW::SetPacketRules (PacketRulesDlg.m_hRules);
		}
	}

	CKAHFW::PacketRules_Delete (hRules);

	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnFwApprules() 
{
	ClearResult ();

	CKAHFW::HAPPRULES hRules = NULL;
	
	CKAHUM::OpResult result = CKAHFW::GetApplicationRules (&hRules);

	if (result == CKAHUM::srOK)
	{
		CApplicationRulesDlg ApplicationRulesDlg (hRules, this);
		
		if (ApplicationRulesDlg.DoModal () == IDOK)
		{
			result = CKAHFW::SetApplicationRules (ApplicationRulesDlg.m_hRules);
		}
	}

	CKAHFW::ApplicationRules_Delete (hRules);

	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (nSBCode == SB_ENDSCROLL && pScrollBar->GetSafeHwnd () == m_FirewallLevelSlider.GetSafeHwnd ())
	{
		if (m_nLastSliderPos != m_FirewallLevelSlider.GetPos ())
		{
			ClearResult ();
			CKAHUM::OpResult result = CKAHFW::SetWorkingMode ((CKAHFW::WorkingMode)(m_nLastSliderPos = m_FirewallLevelSlider.GetPos ()));
			SetResult (result);
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CCKAHUMTESTUI2Dlg::OnDelayedLog (WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szString = (LPCTSTR)wParam;

	m_AttackList.AddString (szString);
	m_AttackList.SetTopIndex (m_AttackList.GetCount () - 1);

	free ((LPVOID)szString);

	return 0;
}

LRESULT CCKAHUMTESTUI2Dlg::OnDelayedTrace (WPARAM wParam, LPARAM lParam)
{
	LPCTSTR szString = (LPCTSTR)wParam;

	m_TraceLog.AddString (szString);
	m_TraceLog.SetTopIndex (m_TraceLog.GetCount () - 1);

	free ((LPVOID)szString);

	return 0;
}

void CCKAHUMTESTUI2Dlg::AddStringToLog (LPCTSTR string)
{
	PostMessage (WM_USER_DELAYEDLOG, (WPARAM)_tcsdup (string), 0);
}

void CCKAHUMTESTUI2Dlg::OnCrStart() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHCR::Start ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCrStop() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHCR::Stop ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCRPause() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHCR::Pause ();
	SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCRResume() 
{
	ClearResult ();
	CKAHUM::OpResult result = CKAHCR::Resume ();
	SetResult (result);
}


void CCKAHUMTESTUI2Dlg::OnCRWatchdog() 
{
    ClearResult ();
    CKAHUM::OpResult result = CKAHCR::SetWatchdogTimeout(5);
    SetResult (result);
}

void CCKAHUMTESTUI2Dlg::OnCRConsole() 
{
	UpdateData();

    static int ports = 10000;

	CCRConsoleDlg *pConsoleDlg = new CCRConsoleDlg (&m_CrExludeList, ports++, m_nCRConsolePort, this);

	if(pConsoleDlg && pConsoleDlg->m_status)
		pConsoleDlg->Create(CCRConsoleDlg::IDD, this);
	else
		delete pConsoleDlg;
}

void CCKAHUMTESTUI2Dlg::OnClear2() 
{
	m_TraceLog.ResetContent ();	
}

void CCKAHUMTESTUI2Dlg::OnGetBannedList() 
{
	// TODO: Add your control notification handler code here
	CBanometDlg BanometDlg;
		
	if (BanometDlg.DoModal() == IDOK)
	{
		
	}
}

void CCKAHUMTESTUI2Dlg::OnHostStat() 
{
	// TODO: Add your control notification handler code here
    CHostStatDlg HostStatDlg;

    if (HostStatDlg.DoModal() == IDOK)
	{
		
	}
	
}

void CCKAHUMTESTUI2Dlg::OnConnList() 
{
	// TODO: Add your control notification handler code here
    CActiveConnDlg  Dlg;

    if (Dlg.DoModal() == IDOK)
	{
		
	}	
}

void CCKAHUMTESTUI2Dlg::OnCRExcludes() 
{
	// TODO: Add your control notification handler code here
	CRdrExcl        Dlg(&m_CrExludeList);

    if ( Dlg.DoModal() == IDOK )
    {
    }
}

void CCKAHUMTESTUI2Dlg::OnStmChange() 
{	
	CKAHFW::HNETWORKLIST hNetList = NULL;	

	CKAHUM::OpResult result = CKAHFW::GetNetworkList (&hNetList);

	if (result == CKAHUM::srOK)
	{
		int index = m_NetListCtrl.GetCurSel();		
		
		CKAHFW::Network network;
		
		if ((result = CKAHFW::NetworkList_GetItem (hNetList, index, &network)) == CKAHUM::srOK)
		{
            if (network.HasIPv4)
            {
                CKAHSTM::StmNet Net = { CKAHUM::IP(network.IPv4) , CKAHUM::IP(network.Maskv4) };
    			
			    ClearResult ();
			    result = CKAHSTM::AddNet( &Net );
			    SetResult (result);
            }
            if (network.HasIPv6)
            {
                CKAHSTM::StmNet Net = { CKAHUM::IP(network.IPv6) , CKAHUM::IP(network.Maskv6) };
    			
			    ClearResult ();
			    result = CKAHSTM::AddNet( &Net );
			    SetResult (result);
            }
		}

		CKAHFW::NetworkList_Delete (hNetList);
	}
}

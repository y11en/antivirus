// CKAHGENDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHGEN.h"
#include "CKAHGENDlg.h"
#include "../../../../../windows/hook/Plugins/Include/mad_api.h"
#define USER_MODE
#include "winioctl.h"
#include "../../../../../windows/Hook/klsdk/klstatus.h"
#include "../../../../../windows/hook/klsdk/PlugApi.h"
#include "../../../../../windows/hook/klsdk/BaseAPI.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCKAHGENDlg dialog

CCKAHGENDlg::CCKAHGENDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCKAHGENDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCKAHGENDlg)
	m_AttackID = 0x00010001;
	m_Proto = 1;
	m_Port = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCKAHGENDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCKAHGENDlg)
	DDX_Control(pDX, IDC_IP, m_IPCtrl);
	DDX_Text(pDX, IDC_EDIT2, m_AttackID);
	DDX_Text(pDX, IDC_EDIT3, m_Proto);
	DDX_Text(pDX, IDC_EDIT4, m_Port);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCKAHGENDlg, CDialog)
	//{{AFX_MSG_MAP(CCKAHGENDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_EMULATE, OnEmulate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCKAHGENDlg message handlers

BOOL CCKAHGENDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	char host[256];
	gethostname (host, sizeof (host));

	hostent *phe = gethostbyname (host);

	m_IPCtrl.SetAddress (htonl (*(DWORD *)phe->h_addr_list[0]));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCKAHGENDlg::OnPaint() 
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
HCURSOR CCKAHGENDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCKAHGENDlg::OnEmulate() 
{
	UpdateData ();

	TCHAR szName[] = _T("\\\\.\\mad");
	HANDLE hPlugin;

	if (g_bIsNT)
		hPlugin = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hPlugin = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);

	if (hPlugin == INVALID_HANDLE_VALUE)
	{
		CString str;
		str.Format (_T("Failed to open 'mad' plugin (0x%08x)"), GetLastError ());
		AfxMessageBox (str, MB_OK | MB_ICONSTOP);
		return;
	}

	std::vector<BYTE> vec_buf (sizeof (PLUGIN_MESSAGE) + sizeof (MAD_ATTACK), 0);

	PLUGIN_MESSAGE *pPluginMessage = (PLUGIN_MESSAGE *)&vec_buf[0];

	strcpy (pPluginMessage->PluginName, MAD_PLUGINNAME);
	pPluginMessage->MessageID = MAD_MSGID_ATTACK;
	pPluginMessage->MessageSize = sizeof (MAD_ATTACK);
	
	MAD_ATTACK *pMADAttack = (MAD_ATTACK *)&vec_buf[sizeof (PLUGIN_MESSAGE)];

	pMADAttack->AttackID = m_AttackID;
	pMADAttack->Proto = m_Proto;
	m_IPCtrl.GetAddress (pMADAttack->AttackerIP);
	pMADAttack->LocalPort = m_Port;	

	DWORD dwBytesReturned = 0;
	if (!DeviceIoControl (hPlugin, PLUG_IOCTL, &vec_buf[0], vec_buf.size (), NULL, 0, &dwBytesReturned, NULL))
	{
		CString str;
		str.Format (_T("DeviceIoControl failed (0x%08x)"), GetLastError ());
		AfxMessageBox (str, MB_OK | MB_ICONSTOP);
	}

	CloseHandle (hPlugin);
}

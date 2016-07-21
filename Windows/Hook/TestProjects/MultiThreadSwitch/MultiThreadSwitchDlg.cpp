// MultiThreadSwitchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiThreadSwitch.h"
#include "MultiThreadSwitchDlg.h"


#include <Hook\FsdrvLib.h>
#include "OwnThreads.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LONG TotalThreads;
extern DWORD InCheck;
extern DWORD gEnteredRegs;
extern LONG LastInChTime;
extern DWORD gRegsCnt;
extern DWORD gRegSize;
extern DWORD gmin_number_of_shadows;
DWORD EvExit = 0;
HANDLE hPauseEvent;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiThreadSwitchDlg dialog

CMultiThreadSwitchDlg::CMultiThreadSwitchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiThreadSwitchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMultiThreadSwitchDlg)
	m_RegsCnt=3;
	m_RegSize=0x100;
	m_ThCnt=10;
	
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMultiThreadSwitchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultiThreadSwitchDlg)
	DDX_Text(pDX, IDC_EDIT_COUNT, m_eCount);
	DDV_MinMaxInt(pDX, m_eCount, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_COUNT_REGS, m_RegsCnt);
	DDV_MinMaxInt(pDX, m_RegsCnt, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_COUNT_TH, m_ThCnt);
	DDV_MinMaxInt(pDX, m_ThCnt, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_REG_SIZE, m_RegSize);
	DDV_MinMaxDWord(pDX, m_RegSize, 0, 32768);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMultiThreadSwitchDlg, CDialog)
	//{{AFX_MSG_MAP(CMultiThreadSwitchDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SS, OnButtonSs)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiThreadSwitchDlg message handlers

BOOL CMultiThreadSwitchDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initlialization here
	if (FSDrvInterceptorInitTsp() == FALSE)
	{
		MessageBox("Can't init fsdrvlib!", "Error", MB_ICONINFORMATION);
		PostMessage(WM_QUIT);
	}
	hPauseEvent=CreateEvent(NULL,TRUE,TRUE,NULL);
	
	SetTimer(1,200/*ms*/,NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


BOOL CMultiThreadSwitchDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1);
	DoneOrig();
	FSDrvInterceptorDone();
	CloseHandle(hPauseEvent);
	
	return CDialog::DestroyWindow();
}


void CMultiThreadSwitchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMultiThreadSwitchDlg::OnPaint() 
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
HCURSOR CMultiThreadSwitchDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMultiThreadSwitchDlg::OnButtonSs() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	GetDlgItem(IDC_EDIT_COUNT_REGS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_REG_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_COUNT_TH)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SS)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	gmin_number_of_shadows=m_ThCnt/2;
	if(InitOrigs(m_RegsCnt,m_RegSize*4096))
		StartThreads(m_ThCnt);
	else {
		MessageBox("Can't init register orig!", "Error", MB_ICONINFORMATION);
		PostMessage(WM_QUIT);
	}

	GetDlgItem(IDOK)->EnableWindow(TRUE);
				
}

void CMultiThreadSwitchDlg::OnTimer(UINT nIDEvent) 
{
static DWORD OldRegs;
static DWORD OldUsedMem;
static DWORD OldInChk;
static LONG OldLastInChTime;
DWORD UsedMem;
// TODO: Add your message handler code here and/or call default
CHAR str[128];

	if(OldRegs !=gRegsCnt && gRegsCnt) {
		wsprintf(str,"  %d  ",gRegsCnt);
		SetDlgItemText(IDC_EDIT_COUNT_REGS,str);
		OldRegs =gRegsCnt;
	}

	if(gRegsCnt) {
		if(gEnteredRegs < gmin_number_of_shadows*gRegsCnt)
			UsedMem=gRegsCnt*(gmin_number_of_shadows+1)*gRegSize/1024;
		else
			UsedMem=(gRegsCnt+gEnteredRegs)*gRegSize/1024;
	} else {
		UpdateData();
		UsedMem=m_RegsCnt*m_RegSize*4*(m_ThCnt +1 );
	}
	if(OldUsedMem !=UsedMem) {
		wsprintf(str,"%d",UsedMem);
		SetDlgItemText(IDC_UsedMem,str);
		OldUsedMem =UsedMem;
	}
	if(OldInChk!=InCheck) {
		wsprintf(str,"  %d  ",InCheck);
		SetDlgItemText(IDC_ThInCh,str);
		OldInChk=InCheck;
	}
	if(OldLastInChTime!=LastInChTime) {
		wsprintf(str," %d ",LastInChTime);
		SetDlgItemText(IDC_InChTime,str);
		OldLastInChTime=LastInChTime;
	}
	CDialog::OnTimer(nIDEvent);
	if(EvExit && TotalThreads==0)
		CDialog::OnOK();
}

void CMultiThreadSwitchDlg::OnOK() 
{
	// TODO: Add extra validation here
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	EvExit = 1;
}

void CMultiThreadSwitchDlg::OnButtonPause() 
{
static BOOL paused=FALSE;
	paused=!paused;
	if(paused) {
		ResetEvent(hPauseEvent);
		SetDlgItemText(IDC_BUTTON_PAUSE,"Start");
//		SetProcessWorkingSetSizeEx(GetCurrentProcess(),-1,-1,QUOTA_LIMITS_HARDWS_DISABLE);
		SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
	}else{
		SetEvent(hPauseEvent);
		SetDlgItemText(IDC_BUTTON_PAUSE,"Pause");
	}
	// TODO: Add your control notification handler code here
}

// virt_guiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "virt_gui.h"
#include "virt_guiDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cvirt_guiDlg dialog




Cvirt_guiDlg::Cvirt_guiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cvirt_guiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cvirt_guiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cvirt_guiDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_INIT_VIRT, &Cvirt_guiDlg::OnBnClickedBtnInitVirt)
	ON_BN_CLICKED(IDC_BTN_APPLY, &Cvirt_guiDlg::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_RESET, &Cvirt_guiDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_ADD, &Cvirt_guiDlg::OnBnClickedBtnAdd)
END_MESSAGE_MAP()


// Cvirt_guiDlg message handlers

BOOL Cvirt_guiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cvirt_guiDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cvirt_guiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cvirt_guiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cvirt_guiDlg::OnBnClickedBtnInitVirt()
{
	// TODO: Add your control notification handler code here
	//DebugBreak();
	HRESULT hResult = virt_cl.Init();
	if ( IS_ERROR( hResult ) )
	{
		MessageBox( L"virt_cl.Init - Error");
	}
}

void Cvirt_guiDlg::OnBnClickedBtnApply()
{
	// TODO: Add your control notification handler code here

	//DebugBreak();
	HRESULT hResult = virt_cl.Apply();
	if ( IS_ERROR( hResult ) )
	{
		MessageBox( L"virt_cl.Apply - Error");
	}
}

void Cvirt_guiDlg::OnBnClickedBtnReset()
{
	// TODO: Add your control notification handler code here
	//DebugBreak();
	HRESULT hResult = virt_cl.Reset();
	if ( IS_ERROR( hResult ) )
	{
		MessageBox( L"virt_cl.Reset - Error");
	}
}

void Cvirt_guiDlg::OnBnClickedBtnAdd()
{
	// TODO: Add your control notification handler code here
	
	//DebugBreak();
	HRESULT hResult = virt_cl.AddApplToSB(L"C:\\Far\\Far.exe", L"C:\\SB1" );
	if ( IS_ERROR( hResult ) )
	{
		MessageBox( L"virt_cl.AddAppToSB - Error");
	}
}

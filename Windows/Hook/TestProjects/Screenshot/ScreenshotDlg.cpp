// ScreenshotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Screenshot.h"
#include "ScreenshotDlg.h"

#include "demo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenshotDlg dialog




CScreenshotDlg::CScreenshotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenshotDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenshotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_IMG, m_ImgCtrl);
}

BEGIN_MESSAGE_MAP(CScreenshotDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDB_TEST1, &CScreenshotDlg::OnBnClickedBtnshoot)
	ON_BN_CLICKED(IDB_TEST2, &CScreenshotDlg::OnBnClickedTest2)
END_MESSAGE_MAP()


// CScreenshotDlg message handlers

BOOL CScreenshotDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScreenshotDlg::OnPaint()
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
HCURSOR CScreenshotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CScreenshotDlg::OnBnClickedBtnshoot()
{
	ScreenshotDemo1(m_ImgCtrl.m_hWnd);
}

void CScreenshotDlg::OnBnClickedTest2()
{
	ScreenshotDemo2(m_ImgCtrl.m_hWnd);
}

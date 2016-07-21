// EventsGaugeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EventsGauge.h"
#include "EventsGaugeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventsGaugeDlg dialog
#define FILTER_BUFFER_SIZE 4096

BOOL FilterTransmit(HANDLE hDevice, FILTER_TRANSMIT* pInFilter, FILTER_TRANSMIT* pOutFilter)
{
	DWORD BytesRet = 0;
	return DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, pInFilter, FILTER_BUFFER_SIZE, pOutFilter, FILTER_BUFFER_SIZE, &BytesRet, NULL);
}

ULONG GetFiltersCount(HANDLE hDevice, ULONG AppID)
{
	ULONG fc = 0;
	BOOL bRet;

	
	BYTE pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;
	
	_pFilter->m_AppID = AppID;
	
	_pFilter->m_FltCtl = FLTCTL_FIRST;
	
	bRet = FilterTransmit(hDevice, _pFilter, _pFilter);
	if (bRet == FALSE)
		return fc;
	
	_pFilter->m_FltCtl = FLTCTL_NEXT;
	while (bRet)
	{
		fc++;
		bRet = FilterTransmit(hDevice, _pFilter, _pFilter);
	}
	
	return fc;
}

CEventsGaugeDlg::CEventsGaugeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventsGaugeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventsGaugeDlg)
	m_eAppID = 0;
	m_eQueueLen = 0;
	m_eMarkedQueueLen = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEventsGaugeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventsGaugeDlg)
	DDX_Text(pDX, IDC_EDIT_APP_ID, m_eAppID);
	DDV_MinMaxUInt(pDX, m_eAppID, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_QUEUE_LEN, m_eQueueLen);
	DDV_MinMaxUInt(pDX, m_eQueueLen, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_MARKED_QUEUE_LEN, m_eMarkedQueueLen);
	DDV_MinMaxUInt(pDX, m_eMarkedQueueLen, 0, 4294967295);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventsGaugeDlg, CDialog)
	//{{AFX_MSG_MAP(CEventsGaugeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventsGaugeDlg message handlers

BOOL CEventsGaugeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return FALSE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEventsGaugeDlg::OnPaint() 
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
HCURSOR CEventsGaugeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CEventsGaugeDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(_TIMER);
	return CDialog::DestroyWindow();
}

void CEventsGaugeDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	HDSTATE Activity;
	char msg[MAX_PATH];

	lstrcpy(msg, "EG: ");
	if (IDriverGetState(FSDrvGetDeviceHandle(), m_eAppID, &Activity))
	{
		m_eQueueLen = Activity.QLen;
		m_eMarkedQueueLen = Activity.QLen - Activity.QUnmarkedLen;
		
		switch (Activity.Activity)
		{
		case _AS_Dead:
			lstrcat(msg, "Dead");
			break;
		case _AS_Sleep:
			lstrcat(msg, "Sleep");
			break;
		case _AS_Active:
			lstrcat(msg, "Active");
			break;
		case _AS_NotRegistered:
			lstrcat(msg, "Not registered");
			break;
		}
	}
	else
		lstrcat(msg, "Not registered");

	char tmp[MAX_PATH];
	wsprintf(tmp, "(fc : %d)", GetFiltersCount(FSDrvGetDeviceHandle(), m_eAppID));
	lstrcat(msg, tmp);
	
	UpdateData(FALSE);
	SetWindowText(msg);
	
	CDialog::OnTimer(nIDEvent);
}

void CEventsGaugeDlg::OnButtonGo() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	SetTimer(_TIMER, 100, NULL);
	
	GetDlgItem(IDC_BUTTON_GO)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_APP_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_APP_ID)->SetFocus();
}

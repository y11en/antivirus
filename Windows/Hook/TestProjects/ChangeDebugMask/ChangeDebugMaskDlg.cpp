// ChangeDebugMaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChangeDebugMask.h"
#include "ChangeDebugMaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define  NullStr "\0"
//#include "../../debug.c"
//#include <hook\TDIPF_IOCTL.H>

CHAR NullStr[] = "\0";
WCHAR NullStrU[] = L"\0";

CHAR UnknownStr[] = "-?-";
WCHAR UnknownStrW[] = L"-?-";

CHAR *DebCatName[] = {
	NullStr,
		"SYS",
		"FILE",
		"DISK",
		"REG",
		"R3",
		"FWALL",
		"I21",
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		"IOCTL",
		"CLIENT",
		"EVENT",
		"FILTER",
		"INVTH",
		"LOG",
		"DOG",	
		"NAMEC",	
		"TSP",
		"OBJC",
		"Low level disk IO",
		"Filtering Params",
};

CHAR *DebugLevelName[] = {
	"FATAL_ERROR",
	"ERROR",
	"WARNING",
	"IMPORTANT",
	"NOTIFY",
	"INFO",
	"SPAM"
};


#ifndef _DEBUG
#define _DEBUG
#define __MY_DEBUG
#endif

//#include "..\..\..\Personal Firewall\Drivers\Attach\kldebug.h"
//#include "..\..\..\Personal Firewall\Drivers\Pid_Hook\kldebug.h"

#ifdef __MY_DEBUG
#undef _DEBUG
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskDlg dialog

CChangeDebugMaskDlg::CChangeDebugMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeDebugMaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeDebugMaskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChangeDebugMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeDebugMaskDlg)
	DDX_Control(pDX, IDC_COMBO_DRIVER, m_cbDriver);
	DDX_Control(pDX, IDC_LIST_MASK, m_lMask);
	DDX_Control(pDX, IDC_COMBO_DEBUGLEVEL, m_cbLevel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeDebugMaskDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeDebugMaskDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_UNCHECKALL, OnButtonUncheckall)
	ON_BN_CLICKED(IDC_BUTTON_CHECKALL, OnButtonCheckall)
	ON_CBN_SELCHANGE(IDC_COMBO_DRIVER, OnSelchangeComboDriver)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_RESTDEFAULT, OnButtonRestoreDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskDlg message handlers

BOOL CChangeDebugMaskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	int idx;
	idx = m_cbDriver.AddString(AVPGNAME);
	m_cbDriver.SetItemData(idx, 0);
	m_cbDriver.SetCurSel(idx);
	
	idx = m_cbDriver.AddString("klpid");
	m_cbDriver.SetItemData(idx, FLTTYPE_PID);
	ListView_SetExtendedListViewStyle(m_lMask.m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	m_lMask.InsertColumn(0, "Category", LVCFMT_LEFT, 180);
	m_lMask.InsertColumn(1, "Comments", LVCFMT_LEFT, 180);

	ReloadSettings(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChangeDebugMaskDlg::OnPaint() 
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
HCURSOR CChangeDebugMaskDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CChangeDebugMaskDlg::OnOK() 
{
	// TODO: Add extra validation here
	OnButtonApply();
	
	CDialog::OnOK();
}

void CChangeDebugMaskDlg::OnButtonUncheckall() 
{
	// TODO: Add your control notification handler code here
	int count = m_lMask.GetItemCount();
	for (int cou = 0; cou < count; cou++)
		m_lMask.SetCheck(cou, FALSE);
}

void CChangeDebugMaskDlg::OnButtonCheckall() 
{
	// TODO: Add your control notification handler code here
	int count = m_lMask.GetItemCount();
	for (int cou = 0; cou < count; cou++)
		m_lMask.SetCheck(cou, TRUE);
}

void CChangeDebugMaskDlg::OnSelchangeComboDriver() 
{
	// TODO: Add your control notification handler code here
	ReloadSettings(m_cbDriver.GetItemData(m_cbDriver.GetCurSel()));
}

void CChangeDebugMaskDlg::ReloadSettings(DWORD dwID)
{
	int idx;
	HANDLE hDriver = FSDrvGetDeviceHandle();
	DWORD Level = 3;
	DWORD Mask = 0xffffffff;

	PCHAR* pchDebCatName;
	PCHAR* pchDebugLevelName;
	int CatSize = 0;
	int LevelSize = 0;

	m_lMask.DeleteAllItems();
	m_cbLevel.ResetContent();

	/*switch (dwID)
	{
	case FLTTYPE_PID:
		//pchDebCatName = KlpidDebCatName;
		CatSize = sizeof(KlpidDebCatName) / sizeof (KlpidDebCatName[0]);
		pchDebugLevelName = KlpidDebugLevelName;
		LevelSize = sizeof(KlpidDebugLevelName) / sizeof (KlpidDebugLevelName[0]);
		//
		break;
	default:*/
		{
			pchDebCatName = DebCatName;
			CatSize = sizeof(DebCatName) / sizeof (DebCatName[0]);
			pchDebugLevelName = DebugLevelName;
			LevelSize = sizeof(DebugLevelName) / sizeof (DebugLevelName[0]);
		}
/*		break;
	}*/
	if (dwID == 0)
	{
		IDriverGetDebugInfoMask(hDriver, &Mask);
		IDriverGetDebugInfoLevel(hDriver, &Level);
	}
	else
	{
		/*CHAR buff[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DWORD)];
		PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) buff;
		pRequest->m_DrvID = dwID;
		pRequest->m_BufferSize = sizeof(DWORD);

		pRequest->m_IOCTL = IOCTL_GET_DEBUG_LEVEL;
		if (IDriverExternalDrvRequest(hDriver, pRequest, pRequest) == TRUE)
			Level = *(DWORD*) pRequest->m_Buffer;
		
		pRequest->m_IOCTL = IOCTL_GET_DEBUG_MASK;
		if (IDriverExternalDrvRequest(hDriver, pRequest, pRequest) == TRUE)
			Mask = *(DWORD*) pRequest->m_Buffer;*/
	}

	int count = 0;
	for (int cou = 0; cou < CatSize; cou++)
	{
		if (lstrcmp(pchDebCatName[cou], NullStr) != 0)
		{
			idx = m_lMask.InsertItem(count++, pchDebCatName[cou]);
			if (idx != -1)
			{
				m_lMask.SetItemData(idx, cou);
				if ((Mask & (1 << cou)) != 0)
					m_lMask.SetCheck(idx, TRUE);
			}
		}
		
	}
	
	for (cou = 0; cou < LevelSize; cou++)
	{
		m_cbLevel.AddString(pchDebugLevelName[cou]);
	}
	
	m_cbLevel.SetCurSel(Level);
	
	count = m_lMask.GetItemCount();
	
	char msg[MAX_PATH];
	wsprintf(msg, "From driver: Mask %#x, Level: %d\n", Mask, Level);
	OutputDebugString(msg);
}

void CChangeDebugMaskDlg::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	DWORD dwID = m_cbDriver.GetItemData(m_cbDriver.GetCurSel());
	HANDLE hDriver = FSDrvGetDeviceHandle();
	
	int Level = m_cbLevel.GetCurSel();
	
	int count = m_lMask.GetItemCount();
	int idx;
	DWORD Mask = 0;
	for (int cou = 0; cou < count; cou++)
	{
		if (m_lMask.GetCheck(cou))
		{
			idx = m_lMask.GetItemData(cou);
			Mask |= (1 << idx);
		}
	}

	if (dwID == 0)
	{
		IDriverChangeDebugInfoLevel(hDriver, Level);
		IDriverChangeDebugInfoMask(hDriver, Mask);
	}
	else
	{
		/*CHAR buff[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DWORD)];
		PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) buff;
		pRequest->m_DrvID = dwID;
		pRequest->m_BufferSize = sizeof(DWORD);
		
		pRequest->m_IOCTL = IOCTL_SET_DEBUG_LEVEL;
		*(DWORD*) pRequest->m_Buffer = Level;
		IDriverExternalDrvRequest(hDriver, pRequest, pRequest);

		*(DWORD*) pRequest->m_Buffer = Mask;			
		pRequest->m_IOCTL = IOCTL_SET_DEBUG_MASK;
		IDriverExternalDrvRequest(hDriver, pRequest, pRequest);*/
	}
	
	char msg[MAX_PATH];	
	wsprintf(msg, "To driver: Mask %#x, Level: %d\n", Mask, Level);
	OutputDebugString(msg);
}

void CChangeDebugMaskDlg::OnButtonRestoreDefaults() 
{
	// TODO: Add your control notification handler code here
	DWORD dwID = m_cbDriver.GetItemData(m_cbDriver.GetCurSel());
	HANDLE hDriver = FSDrvGetDeviceHandle();
	
	int Level = 3;
	DWORD Mask = (DWORD)-1;
	
	if (dwID == 0)
	{
		IDriverChangeDebugInfoLevel(hDriver, Level);
//		IDriverChangeDebugInfoMask(hDriver, Mask);
	}
	else
	{
		/*CHAR buff[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DWORD)];
		PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) buff;
		pRequest->m_DrvID = dwID;
		pRequest->m_BufferSize = sizeof(DWORD);
		
		pRequest->m_IOCTL = IOCTL_SET_DEBUG_LEVEL;
		*(DWORD*) pRequest->m_Buffer = Level;
		IDriverExternalDrvRequest(hDriver, pRequest, pRequest);
		
		*(DWORD*) pRequest->m_Buffer = Mask;			
		pRequest->m_IOCTL = IOCTL_SET_DEBUG_MASK;
		IDriverExternalDrvRequest(hDriver, pRequest, pRequest);*/
	}
	ReloadSettings(dwID);
	char msg[MAX_PATH];
	wsprintf(msg, "To driver: Mask %#x, Level: %d\n", Mask, Level);
	OutputDebugString(msg);
	
}

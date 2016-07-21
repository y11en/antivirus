// DConfig.cpp : implementation file
//

#include "stdafx.h"
#include "avpgcfg.h"
#include "DConfig.h"
#include "DDefaultFilters.h"
#include "DChangeAppID.h"
#include "DAuditWizard.h"
#include "DClientInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDConfig dialog


CDConfig::CDConfig(CWnd* pParent /*=NULL*/, HANDLE hDevice)
	: CDialog(CDConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDConfig)
	//}}AFX_DATA_INIT
	m_hDevice = hDevice;
	m_MinSize.x = 0;
	m_pActivePage = NULL;
}


void CDConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDConfig)
	DDX_Control(pDX, IDCANCEL, m_bCancel);
	DDX_Control(pDX, ID_APPLY_NOW, m_bApply);
	DDX_Control(pDX, IDC_AVPCOOLBAR, m_axCoolBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDConfig, CDialog)
	//{{AFX_MSG_MAP(CDConfig)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_HELP, OnHelp)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNow)
	ON_COMMAND(IDMI_EXIT, OnExit)
	ON_COMMAND(IDMI_WIZARD_DEF_FILTERS, OnSimpleWizard)
	ON_COMMAND(IDMI_WIZARD, OnWizard)
	ON_COMMAND(IDMI_CHANGEAPPID, OnChangeappid)
	ON_WM_CLOSE()
	ON_COMMAND(IDMI_CLIENTINFO, OnClientinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDConfig message handlers

BOOL CDConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_pFiltersStorage = new CFiltersStorage(m_hDevice);
	m_pFiltersStorage->ReloadFiltersArray();
	
	// TODO: Add extra initialization here

	::SetWindowLong(m_axCoolBar, GWL_ID, IDC_AVPCOOLBAR);

	/*m_pLog = NULL;
	m_pLog = new CDLog;
	m_pLog->Create(IDD_DIALOG_LOG, this);
	m_pLog->ShowWindow(SW_SHOW);
	m_pActivePage = m_pLog;*/

	m_pDTreeMode = new CDTreeMode(m_pFiltersStorage);
	m_pDTreeMode->m_hWndApply = m_bApply.m_hWnd;
	m_pDTreeMode->m_hWndCancel = m_bCancel.m_hWnd;
	m_pDTreeMode->Create(IDD_DIALOG_TREEMODE, this);
	m_pDTreeMode->ShowWindow(SW_SHOW);
	m_pActivePage = m_pDTreeMode;

/*	m_pListMode = new CDListMode(m_pFiltersStorage);
	m_pListMode->m_hWndApply = m_bApply.m_hWnd;
	m_pListMode->m_hWndCancel = m_bCancel.m_hWnd;
	m_pListMode->Create(IDD_DIALOG_LISTMODE, this);
	m_pListMode->ShowWindow(SW_HIDE);*/


	GetDlgItem(ID_APPLY_NOW)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	RestorePosition();

	long FolderIndex;
	m_axCoolBar.AddFolder(IDS_AVPI_SIMPLEMODE, NULL, &FolderIndex);

	HICON hIcon;
/*	hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_ICON_CHANGEDEFFILTERS));
	m_axCoolBar.AddFolderItem(FolderIndex, (long)hIcon, IDS_AVPI_CHANGEDEFFILTERS, NULL, 0);
*/
	/*hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_ICON_LOG));
	m_axCoolBar.AddFolderItem(FolderIndex, (long)hIcon, IDS_LOG, NULL, 0);*/

// -----------------------------------------------------------------------------------------

//	m_axCoolBar.AddFolder(IDS_AVPI_ADVANCEDMODE, NULL, &FolderIndex);

	hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_ICON_TREEVIEW));
	m_axCoolBar.AddFolderItem(FolderIndex, (long)hIcon, IDS_TREEMODE, NULL, 0);

//	hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_ICON_LISTVIEW));
//	m_axCoolBar.AddFolderItem(FolderIndex, (long)hIcon, IDS_LISTMODE, NULL, 0);

//	hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_ICON_DYNAMICINFO));
//	m_axCoolBar.AddFolderItem(FolderIndex, (long)hIcon, IDS_DYNAMICINFO, NULL, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDConfig::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pDTreeMode->DestroyWindow();
	delete m_pDTreeMode;

/*	m_pListMode->DestroyWindow();
	delete m_pListMode;*/

/*	if (m_pLog != NULL)
	{
		m_pLog->DestroyWindow();
		delete m_pLog;
	}*/

	m_pFiltersStorage->ResetState();
	delete m_pFiltersStorage;

	return CDialog::DestroyWindow();
}

void CDConfig::SavePosition()
{
//	TRACE("\nOwner save pos\n");
	CRect rect;
	DWORD Zoomed;
	CString strtmp;

	strtmp = _AVP_GUARD;

	if (!IsIconic())
	{
		if (IsZoomed())
			Zoomed = 1;
		else
		{
			GetWindowRect(&rect);
			g_pReestr->SetValue(strtmp + "WTop", rect.top);
			g_pReestr->SetValue(strtmp + "WLeft", rect.left);
			g_pReestr->SetValue(strtmp + "WBottom", rect.bottom);
			g_pReestr->SetValue(strtmp + "WRight", rect.right);
		}
		g_pReestr->SetValue(strtmp + "Zoomed", Zoomed);
	}
}

void CDConfig::RestorePosition()
{
//	TRACE("\nOwner restore pos \n");
	CRect rect;
	DWORD Zoomed = 0;
	CString strtmp;

	strtmp = _AVP_GUARD;

	GetWindowRect(&rect);
	rect.bottom++;
	m_MinSize.x = rect.Width();
	m_MinSize.y = rect.Height();
	
	rect.top = g_pReestr->GetValue(strtmp + "WTop", rect.top);
	rect.left = g_pReestr->GetValue(strtmp + "WLeft", rect.left);
	rect.bottom = g_pReestr->GetValue(strtmp + "WBottom", rect.bottom);
	rect.right = g_pReestr->GetValue(strtmp + "WRight", rect.right);

	SetWindowPos(NULL, 
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	Zoomed = g_pReestr->GetValue(strtmp + "Zoomed", 0);
	if (Zoomed)
		ShowWindow(SW_SHOWMAXIMIZED);
}

void CDConfig::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI)
{
	CWnd::OnGetMinMaxInfo(lpMMI);
	if (m_MinSize.x != 0)
	{
		lpMMI->ptMinTrackSize.x = m_MinSize.x;
		lpMMI->ptMinTrackSize.y = m_MinSize.y;
	}
}

void CDConfig::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_axCoolBar.m_hWnd))
	{
		static RULE rules[] = {    // Action    Act-on                   Relative-to           Offset
		// ------    ------                   -----------           ------
		{lSTRETCH, lBOTTOM(IDC_AVPCOOLBAR),     lBOTTOM(lPARENT)            ,-5},

		{lMOVE,    lRIGHT(IDB_HELP),			lRIGHT(lPARENT)				,-5},
		{lMOVE,    lBOTTOM(IDB_HELP),			lBOTTOM(lPARENT)			,-5},

		{lMOVE,    lRIGHT(ID_APPLY_NOW),		lLEFT(IDB_HELP)				,-5},
		{lMOVE,    lBOTTOM(ID_APPLY_NOW),		lBOTTOM(IDB_HELP)			,0},

		{lMOVE,    lRIGHT(IDCANCEL),			lLEFT(ID_APPLY_NOW)			,-5},
		{lMOVE,    lBOTTOM(IDCANCEL),			lBOTTOM(ID_APPLY_NOW)		,0},

		{lMOVE,    lRIGHT(IDOK),				lLEFT(IDCANCEL)				,-5},
		{lMOVE,    lBOTTOM(IDOK),				lBOTTOM(IDCANCEL)			,0},

		{lSTRETCH, lRIGHT(IDC_STATIC_FRAME),	lRIGHT(lPARENT)				,-5},
		{lSTRETCH, lBOTTOM(IDC_STATIC_FRAME),	lTOP(IDCANCEL)				,-5},

		{lEND}};

		Layout_ComputeLayout( GetSafeHwnd(), rules);

		// Resize the active page
		TabResize();

	}		
}

void CDConfig::TabResize()
{
	CRect TabRect;
	GetDlgItem(IDC_STATIC_FRAME)->GetWindowRect(&TabRect);
	ScreenToClient(&TabRect);
	TabRect.DeflateRect(1, 1);

	m_pDTreeMode->SetWindowPos(NULL, TabRect.left, TabRect.top, TabRect.Width(), TabRect.Height(), SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE);
//	m_pListMode->SetWindowPos(NULL, TabRect.left, TabRect.top, TabRect.Width(), TabRect.Height(), SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE);
/*	if (m_pLog != NULL)
		m_pLog->SetWindowPos(NULL, TabRect.left, TabRect.top, TabRect.Width(), TabRect.Height(), SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE);
*/
}

void CDConfig::OnApplyNow()
{
	// TODO: Add your control notification handler code here
	m_pDTreeMode->UpdateFilter(_REQUEST_APLLY);
//	m_pListMode->UpdateFilter(_REQUEST_APLLY);
}

void CDConfig::OnOK() 
{
	// TODO: Add extra validation here
	SavePosition();	
	m_pDTreeMode->UpdateFilter(_REQUEST_SAVE);
//	m_pListMode->UpdateFilter(_REQUEST_SAVE);
	CDialog::OnOK();
}

void CDConfig::OnCancel() 
{
	// TODO: Add extra cleanup here
	SavePosition();
	m_pDTreeMode->UpdateFilter(_REQUEST_CANCEL);
//	m_pListMode->UpdateFilter(_REQUEST_CANCEL);
	CDialog::OnCancel();
}

void CDConfig::OnHelp() 
{
	// TODO: Add your control notification handler code here
	
}

void CDConfig::OnExit() 
{
	// TODO: Add your command handler code here
	OnOK();
}

// -----------------------------------------------------------------------------------------

BEGIN_EVENTSINK_MAP(CDConfig, CDialog)
    //{{AFX_EVENTSINK_MAP(CDConfig)
	ON_EVENT(CDConfig, IDC_AVPCOOLBAR, 1 /* CBRGetLocalisedString */, OnCBRGetLocalisedStringAvpcoolbar, VTS_I4 VTS_PBSTR)
	ON_EVENT(CDConfig, IDC_AVPCOOLBAR, 3 /* CBRFolderChange */, OnCBRFolderChangeAvpcoolbar, VTS_I4)
	ON_EVENT(CDConfig, IDC_AVPCOOLBAR, 2 /* CBRItemClick */, OnCBRItemClickAvpcoolbar, VTS_I4 VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDConfig::OnCBRGetLocalisedStringAvpcoolbar(long nStringID, BSTR FAR* pString) 
{
	// TODO: Add your control notification handler code here
	CString strtmp;
	if (g_pLocalize->GetLocalizeString(nStringID, &strtmp))
	{
		::SysFreeString(*pString);
		*pString = strtmp.AllocSysString();
	}
}

void CDConfig::OnCBRFolderChangeAvpcoolbar(long nFolderIndex) 
{
	// TODO: Add your control notification handler code here
}

void CDConfig::OnCBRItemClickAvpcoolbar(long nFolderIndex, long nItemIndex, long nItemID) 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd = NULL;
	switch (nFolderIndex)
	{
	case 0:	// simple mode
		{
/*			switch (nItemIndex)
			{
/*			case 0:
				pWnd = m_pLog;
				break;
			case 1:
				pWnd = m_pDTreeMode;
				m_pDTreeMode->StateChanged(false); // криво
				break;
			case 3:
				pWnd = m_pListMode;
				m_pListMode->StateChanged(false); // аналогично :(
				break;
			}
			*/
			pWnd = m_pDTreeMode;
			m_pDTreeMode->StateChanged(false); // криво
		}
	}

	TabResize();
	if (pWnd != NULL)
	{
		if (m_pActivePage != pWnd)
		{
			if (m_pActivePage != NULL)
			{
				m_pActivePage->ShowWindow(SW_HIDE);
				m_pActivePage->EnableWindow(FALSE);
			}
			pWnd->EnableWindow();
			HWND hWndNext = ::GetNextDlgTabItem(pWnd->m_hWnd, NULL, FALSE );
			if ( hWndNext != NULL )	
				::SetFocus( hWndNext );
			pWnd->ShowWindow(SW_SHOW);
			m_pActivePage = pWnd;
		}
	}

	OnApplyNow();
}

void CDConfig::OnSimpleWizard() 
{
	// TODO: Add your command handler code here
	CDDefaultFilters DDefaultFilters(m_pFiltersStorage);
	DDefaultFilters.DoModal();
}


void CDConfig::OnWizard() 
{
	// TODO: Add your command handler code here
	CDAuditWizard DAWizard;
	if (DAWizard.DoModal() == IDOK)
	{
		m_pDTreeMode->StateChanged(true);
	}
}

void CDConfig::OnChangeappid() 
{
	// TODO: Add your command handler code here
	CDChangeAppID DChangeAppID;
	if (DChangeAppID.DoModal() == IDOK)
	{
		AppReg.m_AppID = DChangeAppID.m_eAppID;
		TRACE("AVPG: New AppID = %d\n", AppReg.m_AppID);

		m_pDTreeMode->StateChanged(true);
	}
}

void CDConfig::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnOK();	
}

void CDConfig::OnClientinfo() 
{
	// TODO: Add your command handler code here
	CDClientInfo DClientInfo(m_hDevice, this);	
	DClientInfo.DoModal();
}

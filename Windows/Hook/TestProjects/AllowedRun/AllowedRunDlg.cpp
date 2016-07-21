// AllowedRunDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AllowedRun.h"
#include "AllowedRunDlg.h"

#include "DFolders.h"
#include "DApplications.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static RULE gRules[] = {
	// Action    Act-on                   Relative-to           Offset
	// ------    ------                   -----------           ------
	{lMOVE,		lLEFT(IDC_BUTTON_ADDNEW),		lLEFT(lPARENT)				, 3},
	{lMOVE,		lBOTTOM(IDC_BUTTON_ADDNEW),		lBOTTOM(lPARENT)			, -7},

	{lMOVE,		lLEFT(IDC_BUTTON_DELETE),		lRIGHT(IDC_BUTTON_ADDNEW)	, 14},
	{lMOVE,		lBOTTOM(IDC_BUTTON_DELETE),		lBOTTOM(IDC_BUTTON_ADDNEW)	, 0},

	{lMOVE,		lLEFT(IDC_BUTTON_EDIT),			lRIGHT(IDC_BUTTON_DELETE)	, 14},
	{lMOVE,		lBOTTOM(IDC_BUTTON_EDIT),		lBOTTOM(lPARENT)			, -7},	

	{lMOVE,		lRIGHT(IDC_BUTTON_EXIT),		lRIGHT(lPARENT)				, -7},
	{lMOVE,		lBOTTOM(IDC_BUTTON_EXIT),		lBOTTOM(lPARENT)			, -7},
	
	{lSTRETCH,	lRIGHT(IDC_TAB_MAIN),			lRIGHT(lPARENT)				, -7},
	{lSTRETCH,	lBOTTOM(IDC_TAB_MAIN),			lTOP(IDC_BUTTON_ADDNEW)		, -5},
	{lEND},
};

/////////////////////////////////////////////////////////////////////////////
// CAllowedRunDlg dialog

CAllowedRunDlg::CAllowedRunDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAllowedRunDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAllowedRunDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAccel = NULL;

	m_PHash_TabWindow = 0;
}

void CAllowedRunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAllowedRunDlg)
	DDX_Control(pDX, IDC_TAB_MAIN, m_tabMain);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAllowedRunDlg, CDialog)
	//{{AFX_MSG_MAP(CAllowedRunDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, OnSelchangeTabMain)
	ON_COMMAND(IDA_NEXTTAB, OnNexttab)
	ON_COMMAND(IDA_PREVTAB, OnPrevtab)
	ON_COMMAND(IDA_ENTER, OnEnter)
	ON_BN_CLICKED(IDC_BUTTON_ADDNEW, OnButtonAddnew)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnExit)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllowedRunDlg message handlers

BOOL CAllowedRunDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_COMMON));
	gpFilters->ReloadFiltersArray();

	int iPanel = 0;
	
	_Hash_Init_Library(_MM_Alloc, _MM_Free, _MM_Compare, NULL/*_MM_FreeItemValue*/);
	m_PHash_TabWindow = _Hash_InitNew(1);

	CDFolders *pDFolders = new CDFolders;
	pDFolders->Create(IDD_DIALOG_FOLDERS, this);
	_Hash_AddItem(m_PHash_TabWindow, (void*) (CDialog*) pDFolders, sizeof(pDFolders), NULL);
	m_tabMain.InsertItem(iPanel++, _T("Folders"));
	pDFolders->ShowWindow(SW_SHOW);	

	CDApplications *pDApplications = new CDApplications;
	pDApplications->Create(IDD_DIALOG_APP, this);
	_Hash_AddItem(m_PHash_TabWindow, (void*) (CDialog*) pDApplications, sizeof(pDApplications), NULL);
	m_tabMain.InsertItem(iPanel++, _T("Applications"));
	pDApplications->ShowWindow(SW_HIDE);	

	RestorePosition();
	TabResize();

	pDFolders->SetFocus();
	
	return FALSE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAllowedRunDlg::OnPaint() 
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
HCURSOR CAllowedRunDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAllowedRunDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI -> ptMinTrackSize.x = 640;
	lpMMI -> ptMinTrackSize.y = 480;
}

void CAllowedRunDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	Layout_ComputeLayout(GetSafeHwnd(), gRules);
	TabResize();
}

BOOL CAllowedRunDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog *pDialog;
	PHashTreeItem pItem = _Hash_GetFirst(m_PHash_TabWindow);
	while (pItem != NULL)
	{
		pDialog = (CDialog*) pItem->m_pData;
		pDialog->DestroyWindow();
		delete pDialog;
		pItem = _Hash_GetNext(pItem);
	}

	_Hash_Done(m_PHash_TabWindow, 0);
	m_PHash_TabWindow = 0;

	SavePosition();
	
	return CDialog::DestroyWindow();
}

void CAllowedRunDlg::SavePosition()
{
	//	TRACE("\nOwner save pos\n");
	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);

	AfxGetApp()->WriteProfileBinary(_T("Position"), _T("Placement"), (LPBYTE)&wpl, sizeof(wpl));
}

void CAllowedRunDlg::RestorePosition()
{
	WINDOWPLACEMENT* pwpl = NULL;
	UINT dw = 0;

	if (AfxGetApp()->GetProfileBinary(_T("Position"), _T("Placement"), (LPBYTE*)&pwpl, &dw))
	{
		if (dw == sizeof(WINDOWPLACEMENT))
			SetWindowPlacement((WINDOWPLACEMENT*)pwpl);

		delete [] pwpl;
	}

	Layout_ComputeLayout(GetSafeHwnd(), gRules);
}

void CAllowedRunDlg::TabResize()
{
	if (!IsWindow(m_tabMain.m_hWnd))
		return;

	if (m_tabMain.GetItemCount() != 0)
	{
		CRect rect;
		CSize size;

		int edge = GetSystemMetrics(SM_CXEDGE) * 2;
		CDialog* pDialog;

		m_tabMain.GetItemRect(0, &rect);
		size.cy = rect.Height() * m_tabMain.GetRowCount();
		m_tabMain.GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.DeflateRect(edge, edge, 3, edge);
		rect.top += size.cy + 1;
		m_TabRect = rect;
		
		PHashTreeItem pItem = _Hash_GetFirst(m_PHash_TabWindow);
		while (pItem != NULL)
		{
			pDialog = (CDialog*) pItem->m_pData;
			pDialog->SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE);
			pItem = _Hash_GetNext(pItem);
		}
	}
}

void CAllowedRunDlg::SelectTab(int nNewTab)
{
	CDialog* pDShow = NULL;
	int cou = 0;

	CDialog *pDialog;
	
	PHashTreeItem pItem = _Hash_GetFirst(m_PHash_TabWindow);
	while (pItem != NULL)
	{
		pDialog = (CDialog*) pItem->m_pData;
		pDialog->ShowWindow(SW_HIDE);
		if (cou == nNewTab)
			pDShow = pDialog;
		
		cou++;
		pItem = _Hash_GetNext(pItem);
	}
	if (pDShow != NULL)
		pDShow->ShowWindow(SW_SHOW);
}


void CAllowedRunDlg::OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int cur_tab = TabCtrl_GetCurSel(pNMHDR->hwndFrom);
	SelectTab(cur_tab);
	
	*pResult = 0;
}

BOOL CAllowedRunDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_hAccel != NULL)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CAllowedRunDlg::OnNexttab() 
{
	// TODO: Add your command handler code here
	if (m_PHash_TabWindow->m_ItemsCount == 0)
		return;

	int cur_tab = m_tabMain.GetCurSel() + 1;
	if ((unsigned int)cur_tab == m_PHash_TabWindow->m_ItemsCount)
		cur_tab = 0;
	SelectTab(cur_tab);
	m_tabMain.SetCurSel(cur_tab);
}

void CAllowedRunDlg::OnPrevtab() 
{
	// TODO: Add your command handler code here
	if (m_PHash_TabWindow->m_ItemsCount == 0)
		return;

	int cur_tab = m_tabMain.GetCurSel() - 1;
	if (cur_tab == -1 )
		cur_tab = m_PHash_TabWindow->m_ItemsCount - 1;

	SelectTab(cur_tab);
	m_tabMain.SetCurSel(cur_tab);	
}

void CAllowedRunDlg::OnButtonAddnew() 
{
	// TODO: Add your control notification handler code here
	SendMsgToCurrentPage(WU_ADDNEW);
}

void CAllowedRunDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	SendMsgToCurrentPage(WU_DELETE);
}

void CAllowedRunDlg::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	SendMsgToCurrentPage(WU_EDIT);
}

void CAllowedRunDlg::SendMsgToCurrentPage(DWORD wuMessage)
{
	int cou = 0;

	CDialog *pDialog;
	
	PHashTreeItem pItem = _Hash_GetFirst(m_PHash_TabWindow);
	while (pItem != NULL)
	{
		pDialog = (CDialog*) pItem->m_pData;
		if (cou == m_tabMain.GetCurSel())
		{
			pDialog->SendMessage(wuMessage, 0, 0);
			return;
		}
		
		cou++;
		pItem = _Hash_GetNext(pItem);
	}
}

void CAllowedRunDlg::OnOK()
{

}

void CAllowedRunDlg::OnEnter() 
{
	// TODO: Add your command handler code here
	
}

void CAllowedRunDlg::OnExit() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

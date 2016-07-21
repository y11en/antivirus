// DConfig.cpp : implementation file
//

#include "stdafx.h"
#include "IGuard.h"
#include "DConfig.h"
#include "DClientProperty.h"
#include "DInfo.h"
#include "DInfoForUser.h"
#include "DAppID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int _MM_Compare(void* p1, void* p2, int len)
{
	int ret = p1 == p2 ? 0 : 1;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CDConfig dialog


CDConfig::CDConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hAccel = NULL;
}


void CDConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDConfig)
	DDX_Control(pDX, IDC_TAB_MAIN, m_tabMain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDConfig, CDialog)
	//{{AFX_MSG_MAP(CDConfig)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, OnSelchangeTabMain)
	ON_COMMAND(IDA_INSERT, OnInsert)
	ON_COMMAND(IDA_INSERT_CTRL, OnInsertCtrl)
	ON_COMMAND(IDA_CHANGE_APPID, OnChangeAppid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDConfig message handlers
void CDConfig::StoreWindowPosition()
{
	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);
	
	g_pRegStorage->PutValue("MainWindow", (BYTE*) &wpl, sizeof(wpl));
}

void CDConfig::RestoreWindowPosition()
{
	WINDOWPLACEMENT wpl;
	DWORD dw = sizeof(wpl);
	if (g_pRegStorage->GetValue("MainWindow", (BYTE*)&wpl, dw))
		SetWindowPlacement(&wpl);
}

BOOL CDConfig::DestroyWindow() 
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

	StoreWindowPosition();

	return CDialog::DestroyWindow();
}

void CDConfig::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_tabMain.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on							Relative-to				Offset
			// ------    ------							-----------				------
			{lMOVE,		lBOTTOM(IDOK),					lBOTTOM(lPARENT)			, -3},
			{lMOVE,		lRIGHT(IDOK),					lRIGHT(lPARENT)			, -3},
			{lSTRETCH,	lRIGHT(IDC_TAB_MAIN),		lRIGHT(lPARENT)			, -3},
			{lSTRETCH,	lBOTTOM(IDC_TAB_MAIN),		lTOP(IDOK)					, -5},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	

		TabResize();
	}
}

void CDConfig::TabResize()
{
	if (m_tabMain.GetItemCount() != 0)
	{
		CRect rect;
		CSize size;

		CDialog* pDialog;
		
		m_tabMain.GetItemRect(0, &rect);
		size.cy = rect.Height() * m_tabMain.GetRowCount();
		m_tabMain.GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.DeflateRect(4, 4, 3, 4);
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

BOOL CDConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int iPanel = 0;
	
	_Hash_Init_Library(_MM_Alloc, _MM_Free, _MM_Compare, NULL/*_MM_FreeItemValue*/);
	m_PHash_TabWindow = _Hash_InitNew(1);

	CDFilters *pDFilters = new CDFilters;
	pDFilters->Create(IDD_DIALOG_FILTERS, this);
	_Hash_AddItem(m_PHash_TabWindow, (void*) (CDialog*) pDFilters, sizeof(pDFilters), NULL);
	m_tabMain.InsertItem(iPanel++, "Filters");
	pDFilters->ShowWindow(SW_SHOW);	
	m_pDFilters = pDFilters;

	CDClientProperty* pDClientProperty = new CDClientProperty;
	pDClientProperty->Create(IDD_DIALOG_CLIENT_PROPERTY, this);
	_Hash_AddItem(m_PHash_TabWindow, (void*) (CDialog*) pDClientProperty, sizeof(pDClientProperty), NULL);
	m_tabMain.InsertItem(iPanel++, "Client property");
	pDClientProperty->ShowWindow(SW_HIDE);
	
	CDInfo* pDInfo = new CDInfo;
	pDInfo->Create(IDD_DIALOG_INFO, this);
	_Hash_AddItem(m_PHash_TabWindow, (void*) (CDialog*) pDInfo, sizeof(pDInfo), NULL);
	m_tabMain.InsertItem(iPanel++, "Info");
	pDInfo->ShowWindow(SW_HIDE);

	CDInfoForUser* pDInfoForUser = new CDInfoForUser;
	pDInfoForUser->Create(IDD_DIALOG_INFOFORUSER, this);
	_Hash_AddItem(m_PHash_TabWindow, (void*) (CDialog*) pDInfoForUser, sizeof(pDInfoForUser), NULL);
	m_tabMain.InsertItem(iPanel++, "User guide");
	pDInfoForUser->ShowWindow(SW_HIDE);
	
	RestoreWindowPosition();

	TabResize();

/*	DWORD dwStyle = GetWindowLong(this->m_hWnd, GWL_EXSTYLE);
	dwStyle |= WS_EX_APPWINDOW;
	LONG res = SetWindowLong(this->m_hWnd, dwStyle, GWL_EXSTYLE);*/

	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_FILTERS));

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDConfig::OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int cur_tab = TabCtrl_GetCurSel(pNMHDR->hwndFrom);
	
	SelectTab(cur_tab);
	
	*pResult = 0;
}

void CDConfig::SelectTab(int nNewTab)
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

BOOL CDConfig::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_hAccel != NULL)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDConfig::OnInsert() 
{
	// TODO: Add your command handler code here
	m_pDFilters->InsertNewFilter();	
}

void CDConfig::OnInsertCtrl() 
{
	// TODO: Add your command handler code here
	m_pDFilters->InsertNewFilter();
}

void CDConfig::OnChangeAppid() 
{
	// TODO: Add your command handler code here
//#ifdef _DEBUG
	ULONG AppID = AppReg.m_AppID;
	CDAppID	DAppID(&AppID, this);

	if (DAppID.DoModal() != IDOK)
		return;

	AppReg.m_AppID = AppID;
//#endif
}

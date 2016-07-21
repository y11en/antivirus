// DClientProperty.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DClientProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDClientProperty dialog


CDClientProperty::CDClientProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDClientProperty::IDD, pParent)
{
	m_eLogPlace = AppReg.m_LogFileName;
	m_chAllowCacheDeadlock = (AppReg.m_ClientFlags & _CLIENT_FLAG_CACHDEADLOCK) != 0 ? TRUE : FALSE;
	m_chClientType = (AppReg.m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE) != 0 ? FALSE : TRUE;
	m_chSaveFilters = (AppReg.m_ClientFlags & _CLIENT_FLAG_SAVE_FILTERS) != 0 ? TRUE : FALSE;
	m_chUseBlueScreen = (AppReg.m_ClientFlags & _CLIENT_FLAG_USE_BLUE_SCREEN_REQUEST) != 0 ? TRUE : FALSE;
	m_chUseDriverCache = (AppReg.m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE) != 0 ? TRUE : FALSE;
	m_chWithoutCascade = (AppReg.m_ClientFlags & _CLIENT_FLAG_WITHOUTCASCADE) != 0 ? TRUE : FALSE;
	m_chWithoutWatchDog = (AppReg.m_ClientFlags & _CLIENT_FLAG_WITHOUTWATCHDOG) != 0 ? TRUE : FALSE;

	m_eCacheSize = AppReg.m_CacheSize;	
	//{{AFX_DATA_INIT(CDClientProperty)
	//}}AFX_DATA_INIT
}


void CDClientProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDClientProperty)
	DDX_Text(pDX, IDC_EDIT_LOG_PLACE, m_eLogPlace);
	DDV_MaxChars(pDX, m_eLogPlace, 270);
	DDX_Check(pDX, IDC_CHECK_ALLOW_CACHE_DEADLOCK, m_chAllowCacheDeadlock);
	DDX_Check(pDX, IDC_CHECK_CLIENT_POPUP, m_chClientType);
	DDX_Check(pDX, IDC_CHECK_SAVE_FILTERS, m_chSaveFilters);
	DDX_Check(pDX, IDC_CHECK_USE_BLUE_SCREEN_REQ, m_chUseBlueScreen);
	DDX_Check(pDX, IDC_CHECK_USING_DRIVER_CACHE, m_chUseDriverCache);
	DDX_Check(pDX, IDC_CHECK_WITHOUT_CASCADE_PROCESSING, m_chWithoutCascade);
	DDX_Check(pDX, IDC_CHECK_WITHOUT_WATCHDOG, m_chWithoutWatchDog);
	DDX_Text(pDX, IDC_EDIT_CACHE_SIZE, m_eCacheSize);
	DDV_MinMaxInt(pDX, m_eCacheSize, 128, 65535);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDClientProperty, CDialog)
	//{{AFX_MSG_MAP(CDClientProperty)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_EN_CHANGE(IDC_EDIT_LOG_PLACE, OnChangeEditLogPlace)
	ON_BN_CLICKED(IDC_CHECK_ALLOW_CACHE_DEADLOCK, OnCheckAllowCacheDeadlock)
	ON_BN_CLICKED(IDC_CHECK_CLIENT_POPUP, OnCheckClientPopup)
	ON_BN_CLICKED(IDC_CHECK_SAVE_FILTERS, OnCheckSaveFilters)
	ON_BN_CLICKED(IDC_CHECK_USE_BLUE_SCREEN_REQ, OnCheckUseBlueScreenReq)
	ON_BN_CLICKED(IDC_CHECK_USING_DRIVER_CACHE, OnCheckUsingDriverCache)
	ON_BN_CLICKED(IDC_CHECK_WITHOUT_CASCADE_PROCESSING, OnCheckWithoutCascadeProcessing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDClientProperty message handlers

BOOL CDClientProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDClientProperty::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
//	if (IsWindow(m_tabMain.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on							Relative-to				Offset
			// ------    ------							-----------				------
			{lSTRETCH,	lRIGHT(IDC_EDIT_LOG_PLACE),		lRIGHT(lPARENT)			, -3},
			{lMOVE,		lRIGHT(IDC_BUTTON_APPLY),			lRIGHT(lPARENT)			, -3},
			{lMOVE,		lBOTTOM(IDC_BUTTON_APPLY),			lBOTTOM(lPARENT)			, -3},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}
}

BOOL CDClientProperty::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if((LOWORD(wParam) == IDCANCEL) || (LOWORD(wParam) == IDOK))
		return TRUE;
	
	return CDialog::OnCommand(wParam, lParam);
}


void CDClientProperty::EnableApply(void)
{
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow();
}

void CDClientProperty::OnChangeEditLogPlace() 
{
	EnableApply();
}

void CDClientProperty::OnCheckAllowCacheDeadlock() 
{
	EnableApply();
}

void CDClientProperty::OnCheckClientPopup() 
{
	EnableApply();
}

void CDClientProperty::OnCheckSaveFilters() 
{
	EnableApply();
}

void CDClientProperty::OnCheckUseBlueScreenReq() 
{
	EnableApply();
}

void CDClientProperty::OnCheckUsingDriverCache() 
{
	EnableApply();
}

void CDClientProperty::OnCheckWithoutCascadeProcessing() 
{
	EnableApply();
}

void CDClientProperty::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	
	DWORD dwValue;

	g_pRegStorage->PutValue("LogFileName", m_eLogPlace);

	dwValue = m_chAllowCacheDeadlock;
	g_pRegStorage->PutValue("AllowCacheDeadlock", dwValue);
	
	dwValue = m_chClientType;
	g_pRegStorage->PutValue("ClientType", dwValue);
	
	dwValue = m_chSaveFilters;
	g_pRegStorage->PutValue("SaveFilters", dwValue);
	
	dwValue = m_chUseBlueScreen;
	g_pRegStorage->PutValue("UseBlueScreen", dwValue);
	
	dwValue = m_chUseDriverCache;
	g_pRegStorage->PutValue("UseDriverCache", dwValue);
	
	dwValue = m_chWithoutCascade;
	g_pRegStorage->PutValue("WithoutCascade", dwValue);
	
	dwValue = m_chWithoutWatchDog;
	g_pRegStorage->PutValue("WithoutWatchDog", dwValue);
	
	MessageBox("You should restart application for aplly changes", "Message", MB_ICONINFORMATION);
	
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(FALSE);
}
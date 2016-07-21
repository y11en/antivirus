// DClientInfo.cpp : implementation file
//

#include "stdafx.h"
#include "avpgcfg.h"
#include "DClientInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDClientInfo dialog


#define _Timer	401

CDClientInfo::CDClientInfo(HANDLE hDevice, CWnd* pParent /*=NULL*/)
	: CDialog(CDClientInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDClientInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hDevice = hDevice;
}


void CDClientInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDClientInfo)
	DDX_Control(pDX, IDC_COMBO_STATUS, m_cbStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDClientInfo, CDialog)
	//{{AFX_MSG_MAP(CDClientInfo)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_WM_TIMER()
	ON_CBN_DROPDOWN(IDC_COMBO_STATUS, OnDropdownComboStatus)
	ON_CBN_SELENDOK(IDC_COMBO_STATUS, OnSelendokComboStatus)
	ON_CBN_SELENDCANCEL(IDC_COMBO_STATUS, OnSelendcancelComboStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDClientInfo message handlers

BOOL CDClientInfo::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(_Timer);
	return CDialog::DestroyWindow();
}

BOOL CDClientInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	OnButtonRefresh();

	SetTimer(_Timer, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDClientInfo::OnButtonRefresh() 
{
	// TODO: Add your control notification handler code here
	CString strtmp;

	strtmp.Format("Cleint info for AppID %d", AppReg.m_AppID);
	
	SetWindowText(strtmp);

	HDSTATE Activity;
	Activity.AppID = AppReg.m_AppID;
	Activity.Activity = _AS_DontChange;
	
	DWORD BytesRet;
	
	if (DeviceIoControl(m_hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
	{
		strtmp.Format("%d", Activity.QLen);
		SetDlgItemText(IDC_STATIC_TOTALEVENTS, strtmp);
		strtmp.Format("%d", Activity.QUnmarkedLen);
		SetDlgItemText(IDC_STATIC_UNMARKEDEVENTS, strtmp);
		strtmp.Format("%d", Activity.QLen - Activity.QUnmarkedLen);
		SetDlgItemText(IDC_STATIC_MARKEDEVENTS, strtmp);
		
		m_cbStatus.SetCurSel(Activity.Activity);
	}
	else
	{
		strtmp = "unresolved";
		SetDlgItemText(IDC_STATIC_TOTALEVENTS, strtmp);
		SetDlgItemText(IDC_STATIC_UNMARKEDEVENTS, strtmp);
		SetDlgItemText(IDC_STATIC_MARKEDEVENTS, strtmp);
	}

	ADDITIONALSTATE AddState;
	if (DeviceIoControl(m_hDevice, IOCTLHOOK_GetDriverInfo, NULL, 0, &AddState, sizeof(AddState), &BytesRet, NULL))
	{
		strtmp.Format("%d", AddState.ReplyWaiterEnters);
		SetDlgItemText(IDC_STATIC_REPLYWAITERENTERS, strtmp);
		strtmp.Format("%d", AddState.DeadlockCount);
		SetDlgItemText(IDC_STATIC_DEADLOCKCOUNT, strtmp);
		strtmp.Format("%d", AddState.FilterEventEntersCount);
		SetDlgItemText(IDC_STATIC_FILTEREVENTENTERSCOUNT, strtmp);
	}
	else
	{
		strtmp = "unresolved";
		SetDlgItemText(IDC_STATIC_REPLYWAITERENTERS, strtmp);
		SetDlgItemText(IDC_STATIC_DEADLOCKCOUNT, strtmp);
		SetDlgItemText(IDC_STATIC_FILTEREVENTENTERSCOUNT, strtmp);
	}
}

void CDClientInfo::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	OnButtonRefresh();	
	CDialog::OnTimer(nIDEvent);
}

void CDClientInfo::OnDropdownComboStatus() 
{
	// TODO: Add your control notification handler code here
	KillTimer(_Timer);
}

void CDClientInfo::OnSelendokComboStatus() 
{
	// TODO: Add your control notification handler code here
	HDSTATE Activity;
	
	Activity.Activity = m_cbStatus.GetCurSel();
	Activity.AppID = AppReg.m_AppID;
	if (Activity.Activity == 1)
		Activity.Activity = _AS_Sleep;
	else if (Activity.Activity == 2)
		Activity.Activity = _AS_Active;
	else
		Activity.Activity = _AS_DontChange;
	
	DWORD BytesRet;
	
	DeviceIoControl(m_hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL);
		

	SetTimer(_Timer, 100, NULL);
}

void CDClientInfo::OnSelendcancelComboStatus() 
{
	// TODO: Add your control notification handler code here
	SetTimer(_Timer, 100, NULL);
}

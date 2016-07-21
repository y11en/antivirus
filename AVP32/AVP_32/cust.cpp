// cust.cpp : implementation file
//

#include "stdafx.h"
#include "Avp32.h"
#include "cust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CStringArray Mess;

/////////////////////////////////////////////////////////////////////////////
// CAvpCustomize dialog


CAvpCustomize::CAvpCustomize(CWnd* pParent /*=NULL*/)
	: CDialog(CAvpCustomize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAvpCustomize)
	m_CustDeleteAllMessage = FALSE;
	m_CustNextDiskPrompt = FALSE;
	m_CustPopupAfterFinish = FALSE;
	m_CustRedundantMessage = FALSE;
	m_CustSingleClick = FALSE;
	m_CustStatAfterBegin = FALSE;
	m_CustStatAfterFinish = FALSE;
	m_CustOtherMessages = FALSE;
	m_Sound = FALSE;
	m_UpdateInterval = 14;
	m_UpdateCheck = TRUE;
	//}}AFX_DATA_INIT
}


void CAvpCustomize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvpCustomize)
	DDX_Check(pDX, IDC_CUST_DELETE_ALL_MESSAGE, m_CustDeleteAllMessage);
	DDX_Check(pDX, IDC_CUST_NEXT_DISK_PROMPT, m_CustNextDiskPrompt);
	DDX_Check(pDX, IDC_CUST_POPUP_AFTER_FINISH, m_CustPopupAfterFinish);
	DDX_Check(pDX, IDC_CUST_REDUNDANT_MESSAGE, m_CustRedundantMessage);
	DDX_Check(pDX, IDC_CUST_SINGLE_CLICK, m_CustSingleClick);
	DDX_Check(pDX, IDC_CUST_STAT_AFTER_BEGIN, m_CustStatAfterBegin);
	DDX_Check(pDX, IDC_CUST_STAT_AFTER_FINISH, m_CustStatAfterFinish);
	DDX_Check(pDX, IDC_CUST_OTHER_MESSAGES, m_CustOtherMessages);
	DDX_Check(pDX, IDC_SOUND, m_Sound);
	DDX_Text(pDX, IDC_CUST_UPDATE_INTERVAL, m_UpdateInterval);
	DDV_MinMaxUInt(pDX, m_UpdateInterval, 1, 365);
	DDX_Check(pDX, IDC_CUST_UPDATE_CHECK, m_UpdateCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAvpCustomize, CDialog)
	//{{AFX_MSG_MAP(CAvpCustomize)
	ON_BN_CLICKED(IDC_CUST_UPDATE_CHECK, OnCustUpdateCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvpCustomize message handlers

BOOL CAvpCustomize::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	SetHelpID(HIDD(IDD_CUSTOMIZE));

	SetWindowText(MS( IDS_CUSTOMIZE_DLG ));
	GetDlgItem(IDC_SOUND)->SetWindowText(MS( IDS_TAB3_SOUND ));
	GetDlgItem(IDC_CUST_SINGLE_CLICK)->SetWindowText(MS( IDS_CUST_SINGLE_CLICK ));
	GetDlgItem(IDC_CUST_NEXT_DISK_PROMPT)->SetWindowText(MS( IDS_CUST_NEXT_DISK_PROMPT ));
	GetDlgItem(IDC_CUST_REDUNDANT_MESSAGE)->SetWindowText(MS( IDS_CUST_REDUNDANT_MESSAGE ));
	GetDlgItem(IDC_CUST_DELETE_ALL_MESSAGE)->SetWindowText(MS( IDS_CUST_DELETE_ALL_MESSAGE ));
	GetDlgItem(IDC_CUST_STAT_AFTER_BEGIN)->SetWindowText(MS( IDS_CUST_STAT_AFTER_BEGIN ));
	GetDlgItem(IDC_CUST_STAT_AFTER_FINISH)->SetWindowText(MS( IDS_CUST_STAT_AFTER_FINISH ));
	GetDlgItem(IDC_CUST_POPUP_AFTER_FINISH)->SetWindowText(MS( IDS_CUST_POPUP_AFTER_FINISH ));
	GetDlgItem(IDC_CUST_OTHER_MESSAGES)->SetWindowText(MS( IDS_CUST_OTHER_MESSAGES ));
	GetDlgItem(IDC_CUST_UPDATE_CHECK)->SetWindowText(MS( IDS_CUST_UPDATE_CHECK ));
	GetDlgItem(IDC_CUST_UPDATE_CHECK_DAYS)->SetWindowText(MS( IDS_CUST_UPDATE_CHECK_DAYS ));
	GetDlgItem(IDC_CUST_UPDATE_INTERVAL)->EnableWindow(m_UpdateCheck);

	GetDlgItem(IDOK)->SetWindowText(MS( IDS_OK_BUTTON ));
	GetDlgItem(IDCANCEL)->SetWindowText(MS( IDS_CANCEL ));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAvpCustomize::OnCustUpdateCheck() 
{
	GetDlgItem(IDC_CUST_UPDATE_INTERVAL)->EnableWindow(IsDlgButtonChecked(IDC_CUST_UPDATE_CHECK));
	GetDlgItem(IDC_CUST_UPDATE_INTERVAL)->EnableWindow(IsDlgButtonChecked(IDC_CUST_UPDATE_CHECK_DAYS));
}

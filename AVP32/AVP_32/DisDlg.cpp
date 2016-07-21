// DisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Avp32.h"
#include "DisDlg.h"
#include "Avp32Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//extern CStringArray Mess;

/////////////////////////////////////////////////////////////////////////////
// CDisinfectDlg dialog

CDisinfectDlg::CDisinfectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisinfectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisinfectDlg)
	m_All = FALSE;
	m_CopyFolder = FALSE;
	m_sObject = _T("");
	m_sVirus = _T("");
	m_Action = 1;
	//}}AFX_DATA_INIT
}


void CDisinfectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisinfectDlg)
	DDX_Control(pDX, IDC_STOP, m_sStop);
	DDX_Check(pDX, IDC_ALL, m_All);
	DDX_Check(pDX, IDC_COPYINFECTED, m_CopyFolder);
	DDX_Text(pDX, IDC_OBJECT, m_sObject);
	DDX_Text(pDX, IDC_VIRUS, m_sVirus);
	DDX_Radio(pDX, IDC_R_DISINFECT, m_Action);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisinfectDlg, CDialog)
	//{{AFX_MSG_MAP(CDisinfectDlg)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisinfectDlg message handlers

void CDisinfectDlg::OnStop() 
{
	EndDialog(IDC_STOP);
}


HBRUSH CDisinfectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
//	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	CBrush brush;
/*
	if(nCtlColor==CTLCOLOR_DLG || nCtlColor==CTLCOLOR_STATIC )
		brush.CreateSolidBrush(0x000000FF);
	else
*/
		brush.FromHandle(CDialog::OnCtlColor(pDC, pWnd, nCtlColor));
	return HBRUSH(brush);	
//	return hbr;
}

BOOL CDisinfectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(MS( IDS_DISINFECT_DIALOG ));
	GetDlgItem(IDC_ACTION_FRAME)->SetWindowText(MS( IDS_ACTION ));
	GetDlgItem(IDC_INFECTED_BY)->SetWindowText(MS( IDS_INFECTED_BY ));
	GetDlgItem(IDC_STOP)->SetWindowText(MS( IDS_STOP ));
	GetDlgItem(IDCANCEL)->SetWindowText(MS( IDS_CANCEL ));
	GetDlgItem(IDC_DISINFECT_R)->SetWindowText(MS( IDS_DISINFECT_R ));
	GetDlgItem(IDC_DISINFECT_D)->SetWindowText(MS( IDS_DISINFECT_D ));
	GetDlgItem(IDC_DISINFECT_DEL)->SetWindowText(MS( IDS_DISINFECT_DEL ));
	GetDlgItem(IDC_COPYINFECTED)->SetWindowText(MS( IDS_COPYINFECTED ));
	GetDlgItem(IDC_ALL)->SetWindowText(MS( IDS_APPLY_TOALL ));

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

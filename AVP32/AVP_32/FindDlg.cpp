// FindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Avp32.h"
#include "FindDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CStringArray Mess;

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog


CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDlg)
	m_Case = FALSE;
	m_String = _T("");
	m_WWord = FALSE;
	//}}AFX_DATA_INIT
}


void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDlg)
	DDX_Check(pDX, IDC_CASE, m_Case);
	DDX_Text(pDX, IDC_STRING, m_String);
	DDV_MaxChars(pDX, m_String, 256);
	DDX_Check(pDX, IDC_W_WORD, m_WWord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	//{{AFX_MSG_MAP(CFindDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDlg message handlers

BOOL CFindDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(MS( IDS_FIND ));
	GetDlgItem(IDC_CASE)->SetWindowText(MS( IDS_CASE ));
	GetDlgItem(IDC_W_WORD)->SetWindowText(MS( IDS_W_WORD ));
	GetDlgItem(IDOK)->SetWindowText(MS( IDS_FINDNEXT ));
	GetDlgItem(IDCANCEL)->SetWindowText(MS( IDS_CANCEL ));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

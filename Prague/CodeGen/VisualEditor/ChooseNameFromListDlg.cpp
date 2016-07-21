// ChooseNameFromListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "visualeditor.h"
#include "ChooseNameFromListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseNameFromListDlg dialog


CChooseNameFromListDlg::CChooseNameFromListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseNameFromListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseNameFromListDlg)
	m_strNameChoosen = _T("");
	//}}AFX_DATA_INIT
}


void CChooseNameFromListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseNameFromListDlg)
	DDX_LBString(pDX, IDC_LIST_OF_NAMES, m_strNameChoosen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseNameFromListDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseNameFromListDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseNameFromListDlg message handlers

BOOL CChooseNameFromListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_strTitle);

	CListBox* pListBox = (CListBox*) GetDlgItem(IDC_LIST_OF_NAMES);

	POSITION pos = m_oNamesList.GetHeadPosition();
	while(pos)
		pListBox->AddString(m_oNamesList.GetNext(pos));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseNameFromListDlg::OnOk() 
{
	CDialog::OnOK();
}

// DChanges.cpp : implementation file
//

#include "stdafx.h"
#include "allowedrun.h"
#include "DChanges.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDChanges dialog


CDChanges::CDChanges(CString* pNew, CString* pOld, CWnd* pParent /*=NULL*/)
	: CDialog(CDChanges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDChanges)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_New = *pNew;
	m_Old = *pOld;
}


void CDChanges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDChanges)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDChanges, CDialog)
	//{{AFX_MSG_MAP(CDChanges)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDChanges message handlers

BOOL CDChanges::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetDlgItemText(IDC_EDIT_NEW, m_New);
	SetDlgItemText(IDC_EDIT_OLD, m_Old);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

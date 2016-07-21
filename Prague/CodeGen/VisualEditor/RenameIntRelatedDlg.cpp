// RenameIntRelatedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "visualeditor.h"
#include "RenameIntRelatedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameIntRelatedDlg dialog


CRenameIntRelatedDlg::CRenameIntRelatedDlg(CSArray<CRelatedIDs> &rcRelatedInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CRenameIntRelatedDlg::IDD, pParent),
	m_rcRelatedInfo( rcRelatedInfo ),
	m_RelatedButtons( 3, 1, false )
{
	//{{AFX_DATA_INIT(CRenameIntRelatedDlg)
	//}}AFX_DATA_INIT

	m_RelatedButtons.Add( &m_RelatedCheck1Ctrl );
	m_RelatedButtons.Add( &m_RelatedCheck2Ctrl );
	m_RelatedButtons.Add( &m_RelatedCheck3Ctrl );
}


// ---
void CRenameIntRelatedDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameIntRelatedDlg)
	DDX_Control(pDX, IDC_RELATED_1_CHECK, m_RelatedCheck1Ctrl);
	DDX_Control(pDX, IDC_RELATED_2_CHECK, m_RelatedCheck2Ctrl);
	DDX_Control(pDX, IDC_RELATED_3_CHECK, m_RelatedCheck3Ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameIntRelatedDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameIntRelatedDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameIntRelatedDlg message handlers
// ---
BOOL CRenameIntRelatedDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString rcString;

	for ( int i=0; i<min((int)m_rcRelatedInfo.Count(),3); i++) {
		rcString.LoadString( m_rcRelatedInfo[i].m_dwStrID );
		m_RelatedButtons[i]->SetWindowText( rcString );
		m_RelatedButtons[i]->SetCheck( m_rcRelatedInfo[i].m_bValue );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ---
void CRenameIntRelatedDlg::OnOK() {
	for ( int i=0; i<min((int)m_rcRelatedInfo.Count(),3); i++) {
		m_rcRelatedInfo[i].m_bValue = m_RelatedButtons[i]->GetCheck();;
	}
	
	CDialog::OnOK();
}

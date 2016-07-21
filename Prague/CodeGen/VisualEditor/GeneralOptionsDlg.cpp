// GeneralOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VisualEditor.h"
#include "GeneralOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralOptionsDlg dialog


CGeneralOptionsDlg::CGeneralOptionsDlg(BOOL &rbInSingleFile, CWnd &rcFrame, CWnd* pParent /*=NULL*/)
	: CDialog(CGeneralOptionsDlg::IDD, pParent),
	m_bInSingleFile(rbInSingleFile),
	m_rcFrame( rcFrame )
{
	//{{AFX_DATA_INIT(CGeneralOptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGeneralOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralOptionsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


// ---
void  CGeneralOptionsDlg::SetState( int s ) {
	CFAutoParam::SetState( s );
	switch ( m_State ) {
		case ps_nrm :
			if ( !GetSafeHwnd() ) {
				Create( CGeneralOptionsDlg::IDD, m_pParentWnd );

				CRect rcFrameRect;
				m_rcFrame.GetWindowRect( &rcFrameRect );
				::MapWindowPoints( NULL, *this, (POINT *)&rcFrameRect, 2 );

				SetWindowPos( NULL, rcFrameRect.left, rcFrameRect.top, 
														rcFrameRect.Width(), rcFrameRect.Height(), SWP_NOACTIVATE | SWP_NOZORDER );
				ShowWindow( SW_SHOW );
			}
			break;
		case ps_grd :
		case ps_und :
			if ( GetSafeHwnd() ) {
				DestroyWindow();
			}
			break;
	}
}


BEGIN_MESSAGE_MAP(CGeneralOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CGeneralOptionsDlg)
	ON_BN_CLICKED(IDC_CGG_SINGLEFILE_RBTN, OnSingleFile)
	ON_BN_CLICKED(IDC_CGG_DIFFERENTFILE_RBTN, OnDifferentFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralOptionsDlg message handlers
// ---
BOOL CGeneralOptionsDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	if ( m_bInSingleFile )
		CheckDlgButton( IDC_CGG_SINGLEFILE_RBTN, BST_CHECKED );
	else
		CheckDlgButton( IDC_CGG_DIFFERENTFILE_RBTN, BST_CHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ---
void CGeneralOptionsDlg::OnSingleFile() {
	m_bInSingleFile = TRUE;
}


// ---
void CGeneralOptionsDlg::OnDifferentFile() {
	m_bInSingleFile = FALSE;
}


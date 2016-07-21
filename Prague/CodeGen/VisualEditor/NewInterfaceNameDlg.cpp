// NewInterfaceNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "visualeditor.h"
#include "NewInterfaceNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewInterfaceNameDlg dialog


CNewInterfaceNameDlg::CNewInterfaceNameDlg(CString &rcInterfaceName, CWnd* pParent /*=NULL*/)
	: CDialog(CNewInterfaceNameDlg::IDD, pParent),
	m_rcInterfaceName( rcInterfaceName )
{
	//{{AFX_DATA_INIT(CNewInterfaceNameDlg)
	m_IntName = m_rcInterfaceName;
	//}}AFX_DATA_INIT
}


void CNewInterfaceNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewInterfaceNameDlg)
	DDX_Text(pDX, IDC_NIN_NAMEEDIT, m_IntName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewInterfaceNameDlg, CDialog)
	//{{AFX_MSG_MAP(CNewInterfaceNameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewInterfaceNameDlg message handlers
// ---
void CNewInterfaceNameDlg::OnOK() {
	UpdateData( TRUE );
	m_rcInterfaceName = m_IntName;

	CDialog::OnOK();
}

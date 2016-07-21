// DApp.cpp : implementation file
//

#include "stdafx.h"
#include "allowedrun.h"
#include "DApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDApp dialog


CDApp::CDApp(CString* pApp, CWnd* pParent /*=NULL*/)
	: CDialog(CDApp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDApp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pApp = pApp;
}

void CDApp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDApp)
	DDX_Control(pDX, IDC_COMBO_APP, m_cbApp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDApp, CDialog)
	//{{AFX_MSG_MAP(CDApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDApp message handlers

void CDApp::OnOK() 
{
	// TODO: Add extra validation here
	CString Process;
	Process.Empty();
	m_cbApp.GetWindowText(Process);

	if (Process.IsEmpty())
	{
		m_cbApp.SetFocus();
		return;
	}

	*m_pApp = Process;
	
	CDialog::OnOK();
}

BOOL CDApp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_cbApp.SetWindowText(*m_pApp);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

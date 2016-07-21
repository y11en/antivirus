// DSandBoxWarnings.cpp : implementation file
//

#include "stdafx.h"
#include "klguard2.h"
#include "DSandBoxWarnings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CDSandBoxWarnings dialog


CDSandBoxWarnings::CDSandBoxWarnings(CStrHistory *pStrHistory, CWnd* pParent /*=NULL*/)
	: CDialog(CDSandBoxWarnings::IDD, pParent)
{
	m_pStrHistory = pStrHistory;

	m_pStrHistory->PopEvent(&m_eWanring);
	m_WarningsRemain = m_pStrHistory->GetEventCount();

	//{{AFX_DATA_INIT(CDSandBoxWarnings)
	//}}AFX_DATA_INIT
}


void CDSandBoxWarnings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSandBoxWarnings)
	DDX_Text(pDX, IDC_EDIT_WARNING, m_eWanring);
	DDV_MaxChars(pDX, m_eWanring, 4096);
	DDX_Text(pDX, IDC_EDIT_WARNINGSREMAIN, m_WarningsRemain);
	DDV_MinMaxUInt(pDX, m_WarningsRemain, 0, 99999999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSandBoxWarnings, CDialog)
	//{{AFX_MSG_MAP(CDSandBoxWarnings)
	ON_BN_CLICKED(IDC_BUTTON_ALLOK, OnButtonAllok)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSandBoxWarnings message handlers

BOOL CDSandBoxWarnings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetTimer(_TIMER_SAND_BOX, 1000, 0);
	
	SetForegroundWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDSandBoxWarnings::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(_TIMER_SAND_BOX);
	return CDialog::DestroyWindow();
}

void CDSandBoxWarnings::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	m_WarningsRemain = m_pStrHistory->GetEventCount();
	UpdateData(FALSE);
	
	CDialog::OnTimer(nIDEvent);
}

void CDSandBoxWarnings::OnOK() 
{
	// TODO: Add extra validation here
	if (m_pStrHistory->GetEventCount() == 0)
		CDialog::OnOK();

	m_pStrHistory->PopEvent(&m_eWanring);
	m_WarningsRemain = m_pStrHistory->GetEventCount();

	UpdateData(FALSE);
}

void CDSandBoxWarnings::OnButtonAllok() 
{
	// TODO: Add your control notification handler code here
	while (m_pStrHistory->GetEventCount() > 0)
		m_pStrHistory->PopEvent(&m_eWanring);
	
	CDialog::OnOK();
}


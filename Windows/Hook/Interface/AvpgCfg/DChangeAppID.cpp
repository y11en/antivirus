// DChangeAppID.cpp : implementation file
//

#include "stdafx.h"
#include "avpgcfg.h"
#include "DChangeAppID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDChangeAppID dialog


CDChangeAppID::CDChangeAppID(CWnd* pParent /*=NULL*/)
	: CDialog(CDChangeAppID::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDChangeAppID)
	m_eAppID = 0;
	//}}AFX_DATA_INIT
}


void CDChangeAppID::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDChangeAppID)
	DDX_Text(pDX, IDC_EDIT_APPID, m_eAppID);
	DDV_MinMaxUInt(pDX, m_eAppID, 0, 0xffffffff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDChangeAppID, CDialog)
	//{{AFX_MSG_MAP(CDChangeAppID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDChangeAppID message handlers

BOOL CDChangeAppID::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	((CEdit*)GetDlgItem(IDC_EDIT_APPID))->SetSel(0, -1);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

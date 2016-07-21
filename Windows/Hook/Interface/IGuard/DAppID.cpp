// DAppID.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DAppID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDAppID dialog


CDAppID::CDAppID(ULONG* pAppID, CWnd* pParent /*=NULL*/)
	: CDialog(CDAppID::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDAppID)
	m_eAppID = *pAppID;
	//}}AFX_DATA_INIT
	m_pAppID = pAppID;
}


void CDAppID::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDAppID)
	DDX_Text(pDX, IDC_EDIT_APPID, m_eAppID);
	DDV_MinMaxDWord(pDX, m_eAppID, 0, 4294967295);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDAppID, CDialog)
	//{{AFX_MSG_MAP(CDAppID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDAppID message handlers

void CDAppID::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	*m_pAppID = m_eAppID;
	
	CDialog::OnOK();
}

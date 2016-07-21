// RuleTimeElt.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "RuleTimeEltDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CRuleTimeEltDlg dialog


CRuleTimeEltDlg::CRuleTimeEltDlg(CKAHFW::HRULETIMEELEMENT hTimeElt, CWnd* pParent /*=NULL*/)
	: CDialog(CRuleTimeEltDlg::IDD, pParent),
    m_hTimeElt(NULL)
{
	//{{AFX_DATA_INIT(CRuleTimeEltDlg)
	m_TimeMax = 0;
	m_TimeMin = 0;
	//}}AFX_DATA_INIT
	RuleTimeElement_Duplicate (hTimeElt, &m_hTimeElt);
}

CRuleTimeEltDlg::~CRuleTimeEltDlg()
{
    RuleTimeElement_Delete (m_hTimeElt);
}

void CRuleTimeEltDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuleTimeEltDlg)
	DDX_Text(pDX, IDC_TimeMax, m_TimeMax);
	DDX_Text(pDX, IDC_TimeMin, m_TimeMin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRuleTimeEltDlg, CDialog)
	//{{AFX_MSG_MAP(CRuleTimeEltDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuleTimeEltDlg message handlers

BOOL CRuleTimeEltDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    RuleTimeElementType Type;

    RuleTimeElement_GetType(m_hTimeElt, &Type);
    RuleTimeElement_GetRange(m_hTimeElt, &m_TimeMin, &m_TimeMax);

	UpdateData (FALSE);

    CheckRadioButton (IDC_TimeSeconds, IDC_TimeYears, IDC_TimeSeconds + Type);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRuleTimeEltDlg::OnOK() 
{
	if (!UpdateData ())
		return;

	RuleTimeElementType Type = (RuleTimeElementType)(GetCheckedRadioButton (IDC_TimeSeconds, IDC_TimeYears) - IDC_TimeSeconds);

    RuleTimeElement_SetType(m_hTimeElt, Type);
    RuleTimeElement_SetRange(m_hTimeElt, m_TimeMin, m_TimeMax);
	
	CDialog::OnOK();
}

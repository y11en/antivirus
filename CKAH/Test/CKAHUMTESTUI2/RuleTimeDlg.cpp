// RuleTime.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "RuleTimeDlg.h"
#include "RuleTimeEltDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CRuleTimeDlg dialog


CRuleTimeDlg::CRuleTimeDlg(CKAHFW::HRULETIME hTime, CWnd* pParent /*=NULL*/)
	: CDialog(CRuleTimeDlg::IDD, pParent),
    m_hTime (NULL)
{
	//{{AFX_DATA_INIT(CRuleTimeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	RuleTime_Duplicate (hTime, &m_hTime);
}

CRuleTimeDlg::~CRuleTimeDlg()
{
    RuleTime_Delete (m_hTime);
}

void CRuleTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuleTimeDlg)
	DDX_Control(pDX, IDC_TimeList, m_TimeList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRuleTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CRuleTimeDlg)
	ON_BN_CLICKED(IDC_AddTimeElt, OnAddTimeElt)
	ON_BN_CLICKED(IDC_EditTimeElt, OnEditTimeElt)
	ON_BN_CLICKED(IDC_DeleteTimeElt, OnDeleteTimeElt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuleTimeDlg message handlers

BOOL CRuleTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RefreshList ();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

extern TCHAR *g_TimeTypeStr[] =
{
    _T("sec"),
    _T("daysec"),
    _T("min"),
    _T("hour"),
    _T("day"),
    _T("weekday"),
    _T("month"),
    _T("year")
};

void CRuleTimeDlg::RefreshList ()
{
	m_TimeList.ResetContent ();

	int n = RuleTime_GetCount (m_hTime);

	for (int i = 0; i < n; ++i)
	{
        HRULETIMEELEMENT hTimeElt = NULL;
        if (RuleTime_GetItem (m_hTime, i, &hTimeElt) != srOK)
            continue;

        CString TimeElt;

        CKAHFW::RuleTimeElementType type;
        RuleTimeElement_GetType (hTimeElt, &type); 

        ULONG minVal, maxVal;
        RuleTimeElement_GetRange (hTimeElt, &minVal, &maxVal);

        TimeElt.Format("%d-%d %s ", minVal, maxVal, g_TimeTypeStr[type]);

        m_TimeList.AddString (TimeElt.IsEmpty () ? _T("<no time>") : TimeElt);

        RuleTimeElement_Delete (hTimeElt);
	}
}

void CRuleTimeDlg::OnAddTimeElt() 
{
	HRULETIMEELEMENT hTimeElt = RuleTimeElement_Create ();

	CRuleTimeEltDlg RuleTimeEltDlg (hTimeElt, this);

	if (RuleTimeEltDlg.DoModal () == IDOK)
	{
		RuleTime_AddItemToBack (m_hTime, RuleTimeEltDlg.m_hTimeElt);
		RefreshList ();
	}

	RuleTimeElement_Delete (hTimeElt);
}

void CRuleTimeDlg::OnEditTimeElt() 
{
	int nSel = m_TimeList.GetCurSel ();

	if (nSel != LB_ERR)
	{
        HRULETIMEELEMENT hTimeElt = NULL;

		if (RuleTime_GetItem (m_hTime, nSel, &hTimeElt) != srOK)
			return;

		CRuleTimeEltDlg RuleTimeEltDlg (hTimeElt, this);

		if (RuleTimeEltDlg.DoModal () == IDOK)
		{
			RuleTime_DeleteItem (m_hTime, nSel);
			RuleTime_InsertItem (m_hTime, nSel, RuleTimeEltDlg.m_hTimeElt);
			RefreshList ();
		}

		RuleTimeElement_Delete (hTimeElt);
	}	
}

void CRuleTimeDlg::OnDeleteTimeElt() 
{
	int nSel = m_TimeList.GetCurSel ();

	if (nSel != LB_ERR)
	{
		RuleTime_DeleteItem (m_hTime, nSel);
		RefreshList ();
	}	
}

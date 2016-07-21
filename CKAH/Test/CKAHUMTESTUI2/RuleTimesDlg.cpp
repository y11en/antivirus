// RuleTimes.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "RuleTimesDlg.h"
#include "RuleTimeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CRuleTimesDlg dialog


CRuleTimesDlg::CRuleTimesDlg(CKAHFW::HRULETIMES hTimes, CWnd* pParent /*=NULL*/)
	: CDialog(CRuleTimesDlg::IDD, pParent),
    m_hTimes (NULL)
{
	//{{AFX_DATA_INIT(CRuleTimesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	RuleTimes_Duplicate (hTimes, &m_hTimes);
}

CRuleTimesDlg::~CRuleTimesDlg()
{
    RuleTimes_Delete (m_hTimes);
}

void CRuleTimesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuleTimesDlg)
	DDX_Control(pDX, IDC_TimesList, m_TimesList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRuleTimesDlg, CDialog)
	//{{AFX_MSG_MAP(CRuleTimesDlg)
	ON_BN_CLICKED(IDC_AddTime, OnAddTime)
	ON_BN_CLICKED(IDC_EditTime, OnEditTime)
	ON_BN_CLICKED(IDC_DeleteTime, OnDeleteTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuleTimesDlg message handlers

BOOL CRuleTimesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RefreshList ();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

extern TCHAR *g_TimeTypeStr[];

void CRuleTimesDlg::RefreshList ()
{
	m_TimesList.ResetContent ();

	int n = RuleTimes_GetCount (m_hTimes);

	for (int i = 0; i < n; ++i)
	{
		HRULETIME hTime = NULL;
		if (RuleTimes_GetItem (m_hTimes, i, &hTime) != srOK)
			continue;

        int m = RuleTime_GetCount (hTime);

		CString Time;

        for (int j = 0; j < m; ++j)
        {
            HRULETIMEELEMENT hTimeElt = NULL;
            if (RuleTime_GetItem (hTime, j, &hTimeElt) != srOK)
                continue;

            CString TimeElt;

            CKAHFW::RuleTimeElementType type;
            RuleTimeElement_GetType (hTimeElt, &type); 

            ULONG minVal, maxVal;
            RuleTimeElement_GetRange (hTimeElt, &minVal, &maxVal);

            TimeElt.Format("%d-%d %s ", minVal, maxVal, g_TimeTypeStr[type]);
            Time += TimeElt;

            RuleTimeElement_Delete (hTimeElt);
        }

        m_TimesList.AddString (Time.IsEmpty () ? _T("<no time>") : Time);

		RuleTime_Delete (hTime);
	}
}

void CRuleTimesDlg::OnAddTime() 
{
	HRULETIME hTime = RuleTime_Create ();

	CRuleTimeDlg RuleTimeDlg (hTime, this);

	if (RuleTimeDlg.DoModal () == IDOK)
	{
		RuleTimes_AddItemToBack (m_hTimes, RuleTimeDlg.m_hTime);
		RefreshList ();
	}

	RuleTime_Delete (hTime);
}

void CRuleTimesDlg::OnEditTime() 
{
	int nSel = m_TimesList.GetCurSel ();

	if (nSel != LB_ERR)
	{
		HRULETIME hTime = NULL;

		if (RuleTimes_GetItem (m_hTimes, nSel, &hTime) != srOK)
			return;

		CRuleTimeDlg RuleTimeDlg (hTime, this);

		if (RuleTimeDlg.DoModal () == IDOK)
		{
			RuleTimes_DeleteItem (m_hTimes, nSel);
			RuleTimes_InsertItem (m_hTimes, nSel, RuleTimeDlg.m_hTime);
			RefreshList ();
		}

		RuleTime_Delete (hTime);
	}	
}

void CRuleTimesDlg::OnDeleteTime() 
{
	int nSel = m_TimesList.GetCurSel ();

	if (nSel != LB_ERR)
	{
		RuleTimes_DeleteItem (m_hTimes, nSel);
		RefreshList ();
	}	
}

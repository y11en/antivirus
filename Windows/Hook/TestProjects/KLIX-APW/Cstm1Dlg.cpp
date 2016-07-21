// cstm1dlg.cpp : implementation file
//

#include "stdafx.h"
#include "appwizard.h"
#include "cstm1dlg.h"
#include "appwizardaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg dialog
CCustom1Dlg::CCustom1Dlg()
	: CAppWizStepDlg(CCustom1Dlg::IDD)
{
	//{{AFX_DATA_INIT(CCustom1Dlg)
	m_bKLICK = TRUE;
	m_bKLIN = TRUE;
	m_strPluginDescription = _T("Default plugin");
	//}}AFX_DATA_INIT
}

void CCustom1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustom1Dlg)
	DDX_Check(pDX, IDC_CLICK, m_bKLICK);
	DDX_Check(pDX, IDC_KLIN, m_bKLIN);
	DDX_Text(pDX, IDC_PLUGIN_DESCRIPTION, m_strPluginDescription);
	DDV_MaxChars(pDX, m_strPluginDescription, 100);
	//}}AFX_DATA_MAP
}

BOOL CCustom1Dlg::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

	if(FALSE == m_bKLICK && FALSE == m_bKLIN)
	{
		m_bKLICK	= 
		m_bKLIN		=  TRUE;
	}

	LPCTSTR strVal;

	strVal = m_bKLICK ? _T("1") : _T("");
	Appwizardaw.SetMacro(_T("KLICK"), strVal);

	strVal = m_bKLIN ? _T("1") : _T("");
	Appwizardaw.SetMacro(_T("KLIN"), strVal);

	Appwizardaw.SetMacro(_T("PluginDescription"), m_strPluginDescription);

	return TRUE;	
}

BEGIN_MESSAGE_MAP(CCustom1Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CCustom1Dlg)
	ON_BN_CLICKED(IDC_CLICK, OnKlick)
	ON_BN_CLICKED(IDC_KLIN, OnKlin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg message handlers
void CCustom1Dlg::OnKlick() 
{
}

void CCustom1Dlg::OnKlin() 
{
}

/////////////////////////////////////////////////////////////////////////////

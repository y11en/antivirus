// DWarningNoFilters.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DWarningNoFilters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDWarningNoFilters dialog


CDWarningNoFilters::CDWarningNoFilters(CWnd* pParent /*=NULL*/)
	: CDialog(CDWarningNoFilters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDWarningNoFilters)
	m_chDontShow = FALSE;
	//}}AFX_DATA_INIT
}


void CDWarningNoFilters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDWarningNoFilters)
	DDX_Check(pDX, IDC_CHECK_DONTSHOW, m_chDontShow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDWarningNoFilters, CDialog)
	//{{AFX_MSG_MAP(CDWarningNoFilters)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDWarningNoFilters message handlers

BOOL CDWarningNoFilters::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();

	DWORD dwWarn = m_chDontShow;
	g_pRegStorage->PutValue("NoFilterWarning", dwWarn);
			
	return CDialog::DestroyWindow();
}

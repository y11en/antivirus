// PPOptions.cpp : implementation file
//

#include "stdafx.h"
#include "visualeditor.h"
#include "PPOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPPOptions property page

IMPLEMENT_DYNCREATE(CPPOptions, CPropertyPage)

CPPOptions::CPPOptions() : CPropertyPage(CPPOptions::IDD)
{
	//{{AFX_DATA_INIT(CPPOptions)
	m_bUseRegistryNotSql = FALSE;
	//}}AFX_DATA_INIT
}

CPPOptions::~CPPOptions()
{
}

void CPPOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPPOptions)
	DDX_Check(pDX, IDC_USE_REGISTRY_NOT_SQL, m_bUseRegistryNotSql);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPPOptions, CPropertyPage)
	//{{AFX_MSG_MAP(CPPOptions)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPPOptions message handlers

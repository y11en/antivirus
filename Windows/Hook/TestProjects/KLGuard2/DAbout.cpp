// DAbout.cpp : implementation file
//

#include "stdafx.h"
#include "klguard2.h"
#include "DAbout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDAbout dialog


CDAbout::CDAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDAbout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDAbout)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDAbout, CDialog)
	//{{AFX_MSG_MAP(CDAbout)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDAbout message handlers

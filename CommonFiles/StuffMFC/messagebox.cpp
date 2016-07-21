// messagebox.cpp : implementation file
//

#include "stdafx.h"
#include "..\..\AVPServ\AVPMan\AVPCCCfg\avpcccfg.h"
#include "messagebox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageBox dialog


CMessageBox::CMessageBox(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageBox, CDialog)
	//{{AFX_MSG_MAP(CMessageBox)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageBox message handlers

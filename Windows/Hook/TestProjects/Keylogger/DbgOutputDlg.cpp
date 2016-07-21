// DbgOutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "keylogger.h"
#include "DbgOutputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDbgOutputDlg dialog
IMPLEMENT_DYNCREATE(CDbgOutputDlg, CDialog)

CDbgOutputDlg::CDbgOutputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbgOutputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDbgOutputDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	CDialog::Create(IDD, pParent);
}


void CDbgOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbgOutputDlg)
	DDX_Control(pDX, IDC_ED_DBGOUTPUT, m_EdDbgOutput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDbgOutputDlg, CDialog)
	//{{AFX_MSG_MAP(CDbgOutputDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDbgOutputDlg message handlers

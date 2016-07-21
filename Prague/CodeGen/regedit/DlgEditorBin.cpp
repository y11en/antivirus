//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegEdit.h"
#include "DlgEditorBin.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
DlgEditorBin::DlgEditorBin(CWnd* pParent /*=NULL*/)
	: CDialog(DlgEditorBin::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgEditorBin)
	//}}AFX_DATA_INIT
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void DlgEditorBin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditorBin)
	//}}AFX_DATA_MAP
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BEGIN_MESSAGE_MAP(DlgEditorBin, CDialog)
	//{{AFX_MSG_MAP(DlgEditorBin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// DlgEditorBin message handlers

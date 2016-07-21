// CRConsoleEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "CRConsoleEdit.h"
#include "CRConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCRConsoleEdit

CCRConsoleEdit::CCRConsoleEdit()
{
}

CCRConsoleEdit::~CCRConsoleEdit()
{
}


BEGIN_MESSAGE_MAP(CCRConsoleEdit, CEdit)
	//{{AFX_MSG_MAP(CCRConsoleEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCRConsoleEdit message handlers

void CCRConsoleEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (nChar == '\t')
    {
    }
    else if (nChar == '\r')
    {
        ((CCRConsoleDlg*)GetParent())->SrvChar('\r');
        ((CCRConsoleDlg*)GetParent())->SrvChar('\n');
    }
    else
    {
        ((CCRConsoleDlg*)GetParent())->SrvChar(nChar);
    }
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

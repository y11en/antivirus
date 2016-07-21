// LogCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Avp32.h"
#include "LogCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLogCtrl

CLogCtrl::CLogCtrl()
{
}

CLogCtrl::~CLogCtrl()
{
}


BEGIN_MESSAGE_MAP(CLogCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CLogCtrl)
	ON_WM_SIZE()
	ON_COMMAND(ID_STOP,OnLayout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogCtrl message handlers

void CLogCtrl::OnSize(UINT nType, int cx, int cy) 
{
	static ox;
	
	CListCtrl::OnSize(nType, cx, cy);
	
	if(cx != ox){
		ox=cx;
		PostMessage(WM_COMMAND,ID_STOP);
	}
}


void CLogCtrl::OnLayout() 
{
	CRect cr;
	GetClientRect(cr);
	SetColumnWidth(0,cr.right - GetColumnWidth(1) );
}




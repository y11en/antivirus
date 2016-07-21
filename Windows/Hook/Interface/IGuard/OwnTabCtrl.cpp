// OwnTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "OwnTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COwnTabCtrl

COwnTabCtrl::COwnTabCtrl()
{
	m_bCanErase = false;
}

COwnTabCtrl::~COwnTabCtrl()
{
}


BEGIN_MESSAGE_MAP(COwnTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(COwnTabCtrl)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COwnTabCtrl message handlers

BOOL COwnTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect = GetPageRect(); 

	CRect clientRect; 
	GetClientRect(&clientRect); 

	//just paint the edge of the client; 
	//not paint the childRect region 
	pDC->ExcludeClipRect(rect); 
	pDC->FillSolidRect(&clientRect,GetSysColor(COLOR_BTNFACE)); 

	return TRUE;

//	return CTabCtrl::OnEraseBkgnd(pDC);
}

CRect COwnTabCtrl::GetPageRect()
{
	CRect rect;

	CSize size;
	GetItemRect(0, &rect);
	size.cy = rect.Height() * GetRowCount();
	GetClientRect(&rect);
	rect.DeflateRect(4, 4, 3, 4);
	rect.top += size.cy + 1;

	return rect;
}

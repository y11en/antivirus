// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "gui.h"
#include "ListCtrlEx.h"


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{
	CXListCtrl();
}


/*CListCtrlEx::~CListCtrlEx()
{
	~CXListCtrl();
}*/


BEGIN_MESSAGE_MAP(CListCtrlEx, CXListCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CListCtrlEx message handlers



void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int a=GetDlgCtrlID();
	if (GetDlgCtrlID()==IDC_LIST_RUL)
	{
		//int a;
		//a=0;
		*pls=LIST_RUL;
	}
	else if (GetDlgCtrlID()==IDC_LIST_APL)
	{
		//int a;
		//a=0;
		*pls=LIST_APL;
	}
	else
	{
		*pls=LIST_NULL;
	}
	CXListCtrl::OnLButtonDown(nFlags, point);
}

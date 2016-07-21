// FlatListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FlatListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatListCtrl

CFlatListCtrl::CFlatListCtrl()
{
}

CFlatListCtrl::~CFlatListCtrl()
{
}


BEGIN_MESSAGE_MAP(CFlatListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFlatListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlatListCtrl message handlers

void CFlatListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	VERIFY(m_wndFlatHeader.SubclassWindow(::GetDlgItem(m_hWnd,0)));
}

BOOL CFlatListCtrl::GetSortMode(int nColumn)
{
	BOOL bSortAscending;

	// If clicked on already sorted column, reverse sort order
	if (nColumn == m_wndFlatHeader.GetSortColumn(&bSortAscending))
		bSortAscending = !bSortAscending;
	else
        bSortAscending = TRUE;

	m_wndFlatHeader.SetSortColumn(nColumn, bSortAscending);

	return bSortAscending;
}

void CFlatListCtrl::SetSortColumn(int nColumn, BOOL bAscending)
{
	m_wndFlatHeader.SetSortColumn(nColumn, bAscending);
}

void CFlatListCtrl::ResetSortColumn()
{
	m_wndFlatHeader.SetSortColumn(-1, TRUE);
}


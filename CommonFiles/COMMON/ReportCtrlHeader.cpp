// ReportCtrlHeader.cpp : implementation file
//

#include "stdafx.h"
#include "ReportCtrlHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportCtrlHeader

CReportCtrlHeader::CReportCtrlHeader()
{
}

CReportCtrlHeader::~CReportCtrlHeader()
{
}

BEGIN_MESSAGE_MAP(CReportCtrlHeader, CHeaderCtrl)
	//{{AFX_MSG_MAP(CReportCtrlHeader)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CReportCtrlHeader::IsOnNonResizableDivider(POINT pt)
{
	HDHITTESTINFO hdhtiHotItem;
	int iHotIndex = -1;

	hdhtiHotItem.pt = pt;
	ScreenToClient(&hdhtiHotItem.pt);
	iHotIndex = SendMessage(HDM_HITTEST, 0, (LPARAM)(&hdhtiHotItem));

	if ( iHotIndex == -1 || (hdhtiHotItem.flags & (HHT_ONDIVIDER|HHT_ONDIVOPEN)) == 0 )
	{
		return false;
	}

	if ( GetLastColumnIndex () == iHotIndex )
	{
		return true;
	}

	if ( std::find(m_NonResizableHdrIndices.begin(), m_NonResizableHdrIndices.end(), iHotIndex) != m_NonResizableHdrIndices.end() )
		return true;


	int right_index;
	for( int i = GetItemCount (); (right_index = OrderToIndex (i - 1)) != iHotIndex && i>0; --i)
		if ( std::find(m_NonResizableHdrIndices.begin(), m_NonResizableHdrIndices.end(), right_index) == m_NonResizableHdrIndices.end() )
			return false;

//	int left_index;
//	int nCount = GetItemCount ();
//	for( i = 0; (left_index = OrderToIndex (i)) != iHotIndex && i<nCount; ++i)
//		if ( std::find(m_NonResizableHdrIndices.begin(), m_NonResizableHdrIndices.end(), left_index) == m_NonResizableHdrIndices.end() )
//			return false;

	return true;


//	std::list<int>::const_iterator i;
//	for ( i = m_NonResizableHdrIndices.begin(); i != m_NonResizableHdrIndices.end(); ++i )
//	{
//		if ( *i == iHotIndex )
//		{
//			return true;
//		}
//	}

//	return false;
}

bool CReportCtrlHeader::IsOnHeader(POINT pt)
{
	HDHITTESTINFO hdhtiHotItem;
	int iHotIndex = -1;

	hdhtiHotItem.pt = pt;
	ScreenToClient(&hdhtiHotItem.pt);
	iHotIndex = SendMessage(HDM_HITTEST, 0, (LPARAM)(&hdhtiHotItem));

	return ( iHotIndex != -1 && (hdhtiHotItem.flags & HHT_ONHEADER) );
}

bool CReportCtrlHeader::IsOnNonResizableHeader(POINT pt)
{
	HDHITTESTINFO hdhtiHotItem;
	int iHotIndex = -1;

	hdhtiHotItem.pt = pt;
	ScreenToClient(&hdhtiHotItem.pt);
	iHotIndex = SendMessage(HDM_HITTEST, 0, (LPARAM)(&hdhtiHotItem));

	if ( iHotIndex == -1 || (hdhtiHotItem.flags & HHT_ONHEADER) == 0 )
	{
		return false;
	}

	if ( GetLastColumnIndex () == iHotIndex )
	{
		return true;
	}

	std::list<int>::const_iterator i;

	for ( i = m_NonResizableHdrIndices.begin(); i != m_NonResizableHdrIndices.end(); ++i )
	{
		if ( *i == iHotIndex )
		{
			return true;
		}
	}

	return false;
}

bool CReportCtrlHeader::IsSortEnabled(POINT pt)
{
	HDHITTESTINFO hdhtiHotItem;
	int iHotIndex = -1;

	hdhtiHotItem.pt = pt;
	ScreenToClient(&hdhtiHotItem.pt);
	iHotIndex = SendMessage(HDM_HITTEST, 0, (LPARAM)(&hdhtiHotItem));
	
	if ( iHotIndex == -1 )
	{
		return false;
	}

	std::list<int>::const_iterator i;

	for ( i = m_DisabledSortHdrIndices.begin(); i != m_DisabledSortHdrIndices.end(); ++i )
	{
		if ( *i == iHotIndex )
		{
			return false;
		}
	}

	return true;
}

BOOL CReportCtrlHeader::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return ( IsOnNonResizableDivider(CPoint(::GetMessagePos())) ) ? TRUE : CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
}

void CReportCtrlHeader::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pt(::GetMessagePos());
	
	if ( !IsOnNonResizableDivider(pt) && 
			(IsSortEnabled(pt) || !IsOnHeader(pt) ) ) 
	{
		CHeaderCtrl::OnLButtonDown(nFlags, point);
	}
}

void CReportCtrlHeader::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if ( !IsOnNonResizableDivider(CPoint(::GetMessagePos())) ) 
	{
		CHeaderCtrl::OnLButtonDblClk(nFlags, point);
	}
}

int CReportCtrlHeader::GetLastColumnIndex()
{
	int nHdrItems = GetItemCount ();
	return (nHdrItems > 0) ? OrderToIndex (nHdrItems - 1) : -1;
}

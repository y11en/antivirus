#include "stdafx.h"
#include "common.h"
#include "ReportListCtrl.h"
#include "WinVerDiff.h"



//******************************************************************************
// 
//******************************************************************************
BEGIN_MESSAGE_MAP(CReportListCtrl, CCRoboListCtrl)
    //{{AFX_MSG_MAP(CReportListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnHdrChanged)
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnHdrChanged)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CReportListCtrl::CReportListCtrl ()
{
	m_nSortItem = -1;
	m_eSortDirection = eSortNone;
	
	m_bXP = false;
	m_bDisableErasing = false;
	
	//check windows XP
	OSVERSIONINFO info;
	ZeroMemory(&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx (&info))
		m_bXP = (info.dwMajorVersion > 5) ||
		(info.dwMajorVersion == 5 && info.dwMinorVersion > 0);

	if (!m_bXP)
	{
		VERIFY(m_bmSortUp.LoadMappedBitmap(IDB_COMMON_SORT_UP));
		VERIFY(m_bmSortDown.LoadMappedBitmap(IDB_COMMON_SORT_DOWN));		
	}
	
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CReportListCtrl::~CReportListCtrl ()
{
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CReportListCtrl::ESortDirection CReportListCtrl::SetSortMark ( int nHdrItem, CReportListCtrl::ESortDirection eSort /* = eSortAuto*/ )
{
	if(nHdrItem < 0)
		return m_eSortDirection;
	CHeaderCtrl *pHeaderCtrl = GetHeaderCtrl();
	if(pHeaderCtrl && nHdrItem >= pHeaderCtrl->GetItemCount())
		return m_eSortDirection;

	if  ( m_nSortItem == nHdrItem )
    {
        m_eSortDirection = ( eSort == eSortAuto ) ? NextSort ( m_eSortDirection ) : eSort;
    }
	else
	{
		SetColumnMark ( m_nSortItem, eSortNone );
		m_nSortItem = nHdrItem;
		m_eSortDirection = ( eSort == eSortAuto ) ? eSortSmallToBig : eSort;
	}
	SetColumnMark ( m_nSortItem, m_eSortDirection );

	return m_eSortDirection;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CReportListCtrl::ESortDirection CReportListCtrl::NextSort ( CReportListCtrl::ESortDirection eCurrSort )
{
	switch ( eCurrSort )
	{
	case eSortNone:
        return eSortSmallToBig;

    case eSortSmallToBig:
        return eSortBigToSmall;

    case eSortBigToSmall:
        return eSortSmallToBig;
	}

	ASSERT(FALSE);
	return eSortNone;
}
 
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CReportListCtrl::SetColumnMark ( int nSortItem, ESortDirection eSort )
{
	ASSERT (eSort != eSortAuto);

    if  ( nSortItem == -1 )
    {
        return;
    }

	CHeaderCtrl & hdr = *GetHeaderCtrl ();
	int nCount = hdr.GetItemCount ();
	
	HDITEM hdi;
	hdi.mask = HDI_FORMAT;
	VERIFY ( hdr.GetItem ( nSortItem, &hdi ) );

	if (m_bXP)
		hdi.fmt &= ~( HDF_SORTUP | HDF_SORTDOWN );
	else
		hdi.fmt &= ~(HDF_BITMAP|HDF_BITMAP_ON_RIGHT);

	
	switch ( eSort )
	{
	case eSortNone:
		break;

    case eSortSmallToBig:
		if (m_bXP)
			hdi.fmt |= HDF_SORTUP;
		else
		{
			hdi.mask |= HDI_BITMAP;
			hdi.fmt |= HDF_BITMAP|HDF_BITMAP_ON_RIGHT;
			hdi.hbm = m_bmSortUp;
		}
		break;

    case eSortBigToSmall: 
		if (m_bXP)
			hdi.fmt |= HDF_SORTDOWN;
		else
		{
			hdi.mask |= HDI_BITMAP;
			hdi.fmt |= HDF_BITMAP|HDF_BITMAP_ON_RIGHT;
			hdi.hbm = m_bmSortDown;
		}
		break;

    default:
		ASSERT (false);
	}
	VERIFY ( hdr.SetItem ( nSortItem, &hdi ) );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int CReportListCtrl::GetSortColumn ()
{
	return m_nSortItem;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CReportListCtrl::ESortDirection CReportListCtrl::GetSortDirection ()
{
	return m_eSortDirection;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CReportListCtrl::PreSubclassWindow ()
{
	ASSERT ( ::IsWindow (GetSafeHwnd  ()) );
	LONG lStyle = ::GetWindowLong  (GetSafeHwnd  (), GWL_STYLE);
	ASSERT ( lStyle & LVS_OWNERDATA );
	ASSERT ( lStyle & LVS_REPORT );
	
	ModifyStyle ( 0, /*LVS_SINGLESEL |*/ LVS_SHOWSELALWAYS );
	
	ListView_SetExtendedListViewStyle ( GetSafeHwnd (), ListView_GetExtendedListViewStyle ( GetSafeHwnd () ) | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
	
	SetScrollRange  (SB_VERT, 0, 100);
	
	CCRoboListCtrl::PreSubclassWindow ();
}


void CReportListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (IsRButtonSelected())
		CCRoboListCtrl::OnRButtonDown(nFlags, point);
}

void CReportListCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	OnReportContextMenu(point);
	
	if (IsRButtonSelected())
		CCRoboListCtrl::OnRButtonUp(nFlags, point);
}

void CReportListCtrl::OnReportContextMenu(CPoint& pt)
{
}

bool CReportListCtrl::IsRButtonSelected()
{
	return true;
}

void CReportListCtrl::FitLastListCtrlColumn ()
{
     CHeaderCtrl * pHeaderCtrl = GetHeaderCtrl ();

     int nHdrItem = pHeaderCtrl -> GetItemCount ();
     
     if   ( nHdrItem < 1 )
     {
          return;
     }
     
     //m_nLastColumnIndex = pHeaderCtrl -> OrderToIndex ( nHdrItem - 1 );
     
     //	SetColumnWidth ( nHdrItem - 1, LVSCW_AUTOSIZE_USEHEADER );
     
     // посчитаем ширину без последнего столбца
     int nWidth = 0, i;

     for  ( i = 0; i < nHdrItem - 1; ++i )
     {
          nWidth += GetColumnWidth ( pHeaderCtrl -> OrderToIndex ( i ) );
     }
     
     // будем считать вручную
     CRect rcWindow, rcClient;
     // GetWindowRect - без скролбаров
     GetWindowRect ( &rcWindow );
     GetClientRect ( &rcClient );
     
     // определим, будет ли выведен вертикальный скролбар
     // GetStyle() & WS_VSCROLL не работает из-за возможно установленного SetRedraw(FALSE)
     CRect rcFirstItem, rcLastItem;
     rcFirstItem.SetRectEmpty ();
     rcLastItem.SetRectEmpty ();
     
     GetItemRect ( 0, &rcFirstItem, LVIR_BOUNDS );
     GetItemRect ( GetItemCount () - 1, &rcLastItem, LVIR_BOUNDS );
     
     bool bHasVScroll = ( !rcLastItem.IsRectEmpty() && rcLastItem.bottom > rcClient.bottom ) ||
          ( !rcFirstItem.IsRectEmpty () && rcFirstItem.top < rcClient.top );
     
     int nLastWidth = rcWindow.Width () - nWidth - 2 * GetSystemMetrics ( SM_CXEDGE );

     if   ( bHasVScroll )
     {
          nLastWidth -= GetSystemMetrics ( SM_CXHSCROLL );
     }
     
     if ( nLastWidth < 1 )
     {
          nLastWidth = 1;
     }
     
     m_bDisableErasing = true;
     
     int nLastColumnIndex = pHeaderCtrl -> OrderToIndex ( nHdrItem - 1 );
     
     CRect rcSubItem;
     
     // посчитаем максимальный размер строк в столбце
     // вообще-то здесь надо учитывать не только строки, но и иконки и чекбоксы
	 int nWidthAddend = 16;
//	 if (pHeaderCtrl->GetItemCount () == 1)
	 {
		 bool bHasCheckBox = ((ListView_GetExtendedListViewStyle (m_hWnd) & LVS_EX_CHECKBOXES) != 0);
		 bool bHasImageList = (GetImageList (LVSIL_NORMAL) != NULL || 
								GetImageList (LVSIL_SMALL) != NULL ||
								GetImageList (LVSIL_STATE) != NULL);
		 
		 if (bHasCheckBox)
			 nWidthAddend += 18;//GetSystemMetrics (SM_CXMENUCHECK);

		 if (bHasImageList)
			 nWidthAddend += 9;//GetSystemMetrics (SM_CXSMICON);
	 }

     for  ( i = 0; i < GetItemCount (); ++i )
     {
          int nStrWidth = GetStringWidth ( GetItemText ( i, nLastColumnIndex ) ) + nWidthAddend;
          nLastWidth = max ( nLastWidth, nStrWidth );
     }
     
     if   ( GetColumnWidth ( nLastColumnIndex ) != nLastWidth )
     {
          SetColumnWidth ( nLastColumnIndex, nLastWidth );
     }
     m_bDisableErasing = false;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CReportListCtrl::OnHdrChanged ( NMHDR * pNMHDR, LRESULT * pResult )
{
     NMHEADER * pHdr = ( NMHEADER * ) pNMHDR;
     
     *pResult = 0;
     
     if   ( m_bDisableErasing || (pHdr->pitem->mask & HDI_WIDTH) == 0 )
     {
          Default ();
          return;
     }
     
     SetRedraw ( FALSE );
     
     Default ();
     
     FitLastListCtrlColumn ();
     
     SetRedraw ( TRUE );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
BOOL CReportListCtrl::OnEraseBkgnd ( CDC * pDC )
{
     if   ( m_bDisableErasing )
     {
          return TRUE;
     }
     return CCRoboListCtrl::OnEraseBkgnd ( pDC );
}

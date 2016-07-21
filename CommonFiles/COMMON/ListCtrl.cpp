//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "ListCtrl.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void COMMONEXPORT GetSelectedItems ( CListCtrl & ListCtrl, std::list < int > & List )
{
     if   ( ListCtrl.GetSelectedCount () )
     {
          for  ( int i = ListCtrl.GetItemCount (); i; )
          {
               if   ( ListCtrl.GetItemState ( --i, LVIS_SELECTED ) )
               {
                    List.push_back ( i );
               }
          }
     }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int COMMONEXPORT GetFocusedItem ( CListCtrl & ListCtrl )
{
     for  ( int i = 0; i < ListCtrl.GetItemCount (); i++ )
     {
          if   ( ListCtrl.GetItemState ( i, LVIS_FOCUSED ) )
          {
               return i;
          }
     }
     return -1;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void COMMONEXPORT SelectItem ( CListCtrl & ListCtrl, int Item )
{
     ListCtrl.SetItemState ( Item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void COMMONEXPORT SelectOneItem ( CListCtrl & ListCtrl, int Item )
{
     ClearSelection ( ListCtrl );
     SelectItem ( ListCtrl, Item );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void COMMONEXPORT ClearSelection ( CListCtrl & ListCtrl )
{
     for  ( int i = ListCtrl.GetItemCount (); i; )
     {
          ListCtrl.SetItemState ( --i, 0, LVIS_SELECTED );
     }
}

//******************************************************************************
//
//******************************************************************************
BEGIN_MESSAGE_MAP(CCRoboListCtrl, CListCtrl)
    //{{AFX_MSG_MAP(CCRoboListCtrl)
    //}}AFX_MSG_MAP
    ON_REGISTERED_MESSAGE(WM_SETHWND, OnSetHWND)
    ON_REGISTERED_MESSAGE(WM_GETLISTITEMS, OnGetListItems)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCRoboListCtrl::CCRoboListCtrl ()
{
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCRoboListCtrl::~CCRoboListCtrl ()
{
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
LRESULT CCRoboListCtrl::OnSetHWND ( WPARAM wParam, LPARAM lParam )
{
    m_hHideWnd = ( HWND ) wParam;
    
    return 0 ;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
LRESULT CCRoboListCtrl::OnGetListItems ( WPARAM wParam, LPARAM lParam )
{
    if  ( m_hHideWnd == NULL )
    {
        return TRUE;
    }
    
    if  ( lParam == -1 )
    {
        lParam = GetItemCount ();
    }
    
    CHeaderCtrl * pHdrCtrl = GetHeaderCtrl ();
    
    if  ( pHdrCtrl == NULL )
    {
        return TRUE;
    }
    
    CEP_USES_SERIALIZE ((lParam - wParam)*(SIZEOF_ROWNUMBER + 
		pHdrCtrl -> GetItemCount ()*(SIZEOF_COLUMNNUMBER + 8*SIZEOF_COLUMNITEMNUMBER + 6*SIZEOF_INT + 2*SIZEOF_STRING)));
    
    for ( int nRow = wParam; nRow < lParam; ++nRow )
    {
        CEP_SER_ROWNUMBER(nRow) ;
        
        for ( int nColumn = 0; nColumn < pHdrCtrl -> GetItemCount (); ++nColumn )
        {
            CEP_SER_COLUMNNUMBER(nColumn) ;
            
            TCHAR szText [ 512 ] = _T("");
            
            LVITEM item;
			ZeroMemory(&item, sizeof (item));
			
            item.iItem = nRow;
            item.iSubItem = nColumn;
            item.pszText = szText;
            item.cchTextMax = sizeof ( szText );
            item.mask = LVIF_TEXT | LVIF_IMAGE;

            if  ( GetItem ( &item ) == FALSE )
            {
                break;
            }
            
            CRect rect;
            GetItemRect ( nRow, &rect, LVIR_BOUNDS );

            CEP_SER_COLUMNITEMNUMBER ( 0 );
            CEP_SER_INT ( item.iImage );

            CEP_SER_COLUMNITEMNUMBER ( 1 );
            CEP_SER_STRING ( szText );

            CEP_SER_COLUMNITEMNUMBER ( 2 );
            CEP_SER_INT ( GetCheck ( nRow ) ?  2 : 0 );

            CEP_SER_COLUMNITEMNUMBER ( 3 );
            CEP_SER_INT ( rect.left );

            CEP_SER_COLUMNITEMNUMBER ( 4 );
            CEP_SER_INT ( rect.top );

            CEP_SER_COLUMNITEMNUMBER ( 5 );
            CEP_SER_INT ( rect.right );

            CEP_SER_COLUMNITEMNUMBER ( 6 );
            CEP_SER_INT ( rect.bottom );

            TCHAR szTooltip [ 512 ] = _T("");
            CEP_SER_COLUMNITEMNUMBER ( 7 );
            CEP_SER_STRING ( szTooltip );
        }
    }
    COPYDATASTRUCT cds;
    cds.dwData = 0;
    cds.cbData = CEP_SER_DATASIZE ();
    cds.lpData = CEP_SER_DATA ();
    ::SendMessage ( m_hHideWnd, WM_COPYDATA, ( WPARAM ) m_hWnd, ( LPARAM ) &cds );
    
    return 0;
}

//******************************************************************************
// 
//******************************************************************************
BEGIN_MESSAGE_MAP(CCListCtrlEx, CCRoboListCtrl)
    //{{AFX_MSG_MAP(CCListCtrlEx)
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnHdrChanged)
	ON_WM_SIZE()
     ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnHdrChanged)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
    //ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    //ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCListCtrlEx::CCListCtrlEx ()
:    m_bDisableErasing ( false )
{
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCListCtrlEx::~CCListCtrlEx ()
{
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCListCtrlEx::FitLastListCtrlColumn ()
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
	 if (pHeaderCtrl->GetItemCount () == 1)
	 {
		 bool bHasCheckBox = ((ListView_GetExtendedListViewStyle (m_hWnd) & LVS_EX_CHECKBOXES) != 0);
		 bool bHasImageList = (GetImageList (LVSIL_NORMAL) != NULL || 
								GetImageList (LVSIL_SMALL) != NULL ||
								GetImageList (LVSIL_STATE) != NULL);
		 
		 if (bHasCheckBox)
			 nWidthAddend += GetSystemMetrics (SM_CXMENUCHECK);

		 if (bHasImageList)
			 nWidthAddend += GetSystemMetrics (SM_CXSMICON);
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
void CCListCtrlEx::OnHdrChanged ( NMHDR * pNMHDR, LRESULT * pResult )
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
void CCListCtrlEx::OnSize ( UINT nType, int cx, int cy )
{
     CCRoboListCtrl::OnSize ( nType, cx, cy );
     
	 // Обработку OnSize дожен делать родитель
	 //
	 // Сначала выставить CCListCtrlEx.SetRedraw (FALSE);
	 // Затем сделать Layout_ComputeLayout и FitLastListCtrlColumn
	 // Сделать CCListCtrlEx.SetRedraw (TRUE);

/*
     SetRedraw ( FALSE );
     
     FitLastListCtrlColumn ();
     
     SetRedraw ( TRUE );
*/
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
BOOL CCListCtrlEx::OnEraseBkgnd ( CDC * pDC )
{
     if   ( m_bDisableErasing )
     {
          return TRUE;
     }
     return CCRoboListCtrl::OnEraseBkgnd ( pDC );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCListCtrlEx::PreSubclassWindow ()
{
	CCRoboListCtrl::PreSubclassWindow ();

    DWORD dwExStyle = ListView_GetExtendedListViewStyle ( GetSafeHwnd () );
    ListView_SetExtendedListViewStyle ( GetSafeHwnd (), dwExStyle | LVS_EX_INFOTIP | LVS_EX_LABELTIP );
}

//==============================================================================

// ReportViewList.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include <StuffMFC\VMWaitCursor.h>
#include "ReportViewList.h"
#include "WASCRes.rh"
#include "WASCMess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const UINT nGetMenuItemBmp							= ::RegisterWindowMessage(_T("ODMGetMenuItemBmp"));

// ---
static TCHAR *DuplicateString( const TCHAR *s ) {
  return s ? _tcscpy(new TCHAR[_tcsclen(s)+1], s) : _tcscpy(new TCHAR[1], _T(""));
}

/////////////////////////////////////////////////////////////////////////////
// CReportViewListItem

// --- 
CReportViewListItem::CReportViewListItem() :
	m_SmallIcon( NULL ), 
	m_SmallIconIndex( -1 ),
	fManuallyChecked( false ),
	fOwnsIcons( false )
{
}

// --- 
CReportViewListItem::~CReportViewListItem() {
}


// --- 
int	CReportViewListItem::GetImageIndex() {
	return CCheckListItem::GetImageIndex(); 
}

// ---
void CReportViewListItem::Empty() {
	GetSubItemsTexts().RemoveAll();
	GetText() = _T("");
	SetCurrImageIndex( -1 );
	SetIconIndexes( -1 );
}

/////////////////////////////////////////////////////////////////////////////
// CReportViewList

CReportViewList::CReportViewList() :
	m_bItemFound( false ),
	m_bItemFoundInd( -1 ),
	m_nLastSelectedSubItem( -1 ),
	m_nSmallImageSize( ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CXSMICON) ),
	fUseItemImages( false ),	    // Использовать иконы на элементах
	fDontAddItemToList( false ),  // Не добавлать элемент в лист немедленно
	fExternalFillingItem( false ) // Данные элементов хранятся где-то снаружи
{
	HINSTANCE hInst = AfxFindResourceHandle( MAKEINTRESOURCE(IDR_WAS_REPORTVIEWLIST), RT_ACCELERATOR );
	SetAccelTable( ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_WAS_REPORTVIEWLIST)) );

	m_TitleTip.SetWrapLongText( TRUE );

	SetDisplayNoItemsString();
}

// ---
CReportViewList::~CReportViewList() {
}

// ---
COLORREF CReportViewList::SetBkColor( COLORREF crColor ) {
	if ( m_SmallImageList.GetSafeHandle() )
		m_SmallImageList.SetBkColor( crColor );

	return CCheckList::SetBkColor( crColor );
}


// ---
void CReportViewList::CreateImageList() {
  if ( IsUseCheckSemantic() ) {
		SetStateImageType();
		m_ImageList.Create( IDB_WAS_REPORTLISTIMAGE, 25, 1, RGB(255, 255, 255) );
		SetImageList( &m_ImageList, LVSIL_STATE );
	}
	CreateStandartImageList();
}

// ---
void CReportViewList::CreateStandartImageList() {
	if ( IsUseItemImages() ) {
		m_SmallImageList.Create( m_nSmallImageSize.cx, m_nSmallImageSize.cy, ILC_COLOR24 | ILC_MASK, 25, 1);
		m_SmallImageList.SetBkColor( GetBkColor() );
		SetImageList( &m_SmallImageList, LVSIL_SMALL );
	}
}


// ---
void CReportViewList::SetSortByColumns( bool bSet ) {
	if ( GetHeaderCtrl() )
		GetHeaderCtrl()->ModifyStyle( !bSet ? HDS_BUTTONS : 0, 
																	 bSet ? HDS_BUTTONS : 0 );
}


// ---
static void GetIconCompareInfo( HICON hIcon ) {
	ICONINFO stIconInfo;
	::GetIconInfo( hIcon, &stIconInfo );

	BITMAP stMaskBmp;
	::GetObject( stIconInfo.hbmMask, sizeof(BITMAP), &stMaskBmp );
	int bmpMaskBitsSize = stMaskBmp.bmWidthBytes * 8 * stMaskBmp.bmHeight;

	CAPointer<uint8> bmpMaskBits = new uint8[bmpMaskBitsSize];
	::GetBitmapBits( stIconInfo.hbmMask, bmpMaskBitsSize, bmpMaskBits );

	BITMAP stColorBmp;
	::GetObject( stIconInfo.hbmColor, sizeof(BITMAP), &stColorBmp );
	int bmpColorBitsSize = stColorBmp.bmWidthBytes * 8 * stColorBmp.bmHeight;

	CAPointer<uint8> bmpColorBits = new uint8[bmpColorBitsSize];
	::GetBitmapBits( stIconInfo.hbmColor, bmpColorBitsSize, bmpColorBits );

	::DeleteObject( stIconInfo.hbmColor );
	::DeleteObject( stIconInfo.hbmMask );
}


// ---
void CReportViewList::SetItemIcons( int iItemIndex ) {
	if ( IsUseItemImages() ) {
		CReportViewListItem * nsItem = (CReportViewListItem *)(*m_Items)[iItemIndex]; 

		int iSmall = nsItem->GetSmallIconIndex();
/*
		// Прокачка видеодрайвера
		HICON hIcon = AfxGetApp()->LoadIcon(1);
		::GetIconCompareInfo( hIcon );
*/
		if ( nsItem->GetSmallIcon() ) {
			// Найти Image элемента среди уже имеющихся
			int iCacheIndex;

			if ( (iCacheIndex = m_SmallIconCache.FindIt(nsItem->GetSmallIcon())) != -1 ) {
				iSmall = iCacheIndex;
				if ( nsItem->IsOwnsIcons() )
					m_SmallIconCache.Condemn( nsItem->GetSmallIcon() );
			}
			else {
				iSmall = m_SmallImageList.Add( nsItem->GetSmallIcon() );

				int iNewIndex = m_SmallIconCache.Add( nsItem->GetSmallIcon() );
				m_SmallIconCache.SetItemIndex( iNewIndex, iSmall );
			}
		}

		if ( !IsDontAddItemToList() )
			SetItemImage( iItemIndex, iSmall );

		nsItem->SetIconIndexes( iSmall );
		nsItem->SetIcons( NULL );
	}
}


// ---
void CReportViewList::AddNewItem( CCheckListItem *pItem, bool bEnsureVisible, bool bAddToList ) {
	if ( !IsDontAddItemToList()  ) {
		if ( !m_Items || !m_Items->GetSize() )
			FlushItemCache();

		CCheckList::AddNewItem( pItem, bEnsureVisible, bAddToList );

		if ( m_Items->GetSize() == 1 ) {
			SetColumnsWidth();
			CorrectColumnsWidth();
		}
	}
	else {
		if ( !m_Items ) 
			m_Items = new CCLItemsArray( true );

		if ( !m_Items->GetSize() )
			FlushItemCache();

		m_Items->Add( pItem );

		if ( bAddToList ) 
			AddItemsToList( bEnsureVisible, true );
	}
}

// ---
void CReportViewList::AddItemsToList( bool bEnsureVisible, bool bInternal ) {
	if ( m_Items && m_Items->GetSize() ) {
		int nIndex = m_Items->GetSize() - 1;

		ListView_SetItemCountEx( *this, nIndex + 1, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL  );

		if ( bEnsureVisible || !nIndex )	{
			SetItemState( nIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
			EnsureVisible( nIndex, FALSE );
		}

		if ( !bInternal ) {
			SetColumnsWidth();
			CorrectColumnsWidth();
		}
	}
}


// ---
void CReportViewList::AddItem( int iItemIndex ) {

	CCheckList::AddItem( iItemIndex );

	SetItemIcons( iItemIndex );
}


// ---
void CReportViewList::CheckItem( int iItemIndex ) {
	CReportViewListItem *item = (CReportViewListItem *)(*m_Items)[iItemIndex];

	RPVMessageInfo info;

	info.m_nItemIndex = iItemIndex;
	info.m_nSubItemIndex = 0;
	info.m_Value.m_bSetRemove = !item->IsChecked();

	item->SetManuallyChecked( true );

	BOOL bResult = TRUE;

	GetMessageReceiver()->SendMessage( RPVM_MANUALLYSETREMOVECHECKED, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult ) {
	  SetItemChecked( iItemIndex, CCheckListItem::CieChecked(!item->IsChecked()) );
		RECT rcRect;
		GetItemRect( iItemIndex, &rcRect, LVIR_BOUNDS );
		InvalidateRect( &rcRect );
	}
}

// ---
void CReportViewList::FlushItemCache() {
	if ( m_Items && m_Items->GetSize() ) {
		for ( int i=0,c=m_ItemsCache.Count(); i<c; i++ ) {
			CReportViewListItem *pItem = (CReportViewListItem *)(*m_Items)[m_ItemsCache[i].m_nIndex]; 
			pItem->Empty();
		}
	}
	m_ItemsCache.Flush();
}

// ---
bool CReportViewList::PrepareCachedItem( int nItemIndex ) {
	bool bFound = false;
	for ( int i=0,c=m_ItemsCache.Count(); i<c; i++ ) {
		if ( m_ItemsCache[i].m_nIndex == nItemIndex ) {
			m_ItemsCache[i].m_bUsed = true;
			bFound = true;
			break;
		}
	}

	if ( !bFound ) {
		RPVMessageInfo info;

		info.m_nItemIndex = nItemIndex;
		info.m_nSubItemIndex = 0;
		
		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( RPVM_DOFILLITEM, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			m_ItemsCache.Add( CReportViewListCacheItem(nItemIndex) );

			SetItemIcons( nItemIndex );

			if ( m_Items->GetSize() == 1 ) {
				SetColumnsWidth();
				CorrectColumnsWidth();
			}
		}
	}

	return !bFound;
}

// ---
void CReportViewList::PrepareItemsCache( int nIndexFrom, int nIndexTo ) {
	bool bFound = false;

	int i, c;
	for ( i=0,c=m_ItemsCache.Count(); i<c; i++ ) 
		m_ItemsCache[i].m_bUsed = false;

	for ( i=nIndexFrom; i<=nIndexTo; i++ ) 
		bFound |= PrepareCachedItem( i );

	if ( bFound ) {
		for ( i=m_ItemsCache.MaxIndex(); i>=0; i-- ) {
			if ( !m_ItemsCache[i].m_bUsed ) {
				CReportViewListItem *pItem = (CReportViewListItem *)(*m_Items)[m_ItemsCache[i].m_nIndex]; 
				pItem->Empty();
				m_ItemsCache.RemoveInd( i );
			}
		}
	}
	for ( i=0,c=m_ItemsCache.Count(); i<c; i++ ) 
		m_ItemsCache[i].m_bUsed = true;
}

#define CListCtrl CCheckList
BEGIN_MESSAGE_MAP(CReportViewList, CListCtrl)
#undef CListCtrl
	//{{AFX_MSG_MAP(CReportViewList)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnOdCacheHint)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING,OnItemChanging)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_REGISTERED_MESSAGE(nGetMenuItemBmp, OnGetMenuItemBmp)
	ON_COMMAND(IDS_WAS_FINDITEM, OnFindItem)
	ON_COMMAND(IDS_WAS_FINDNEXTITEM, OnFindNextItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportViewList message handlers
// ---
void CReportViewList::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;

	if ( m_Items ) {
		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
		int nNewIndex = pDispInfo->item.iItem;
		if ( nNewIndex < m_Items->GetSize() ) {
			CReportViewListItem *pItem;
			
			if ( IsExternalFillingItem() ) 
				PrepareCachedItem( nNewIndex );

			pItem	= (CReportViewListItem *)(*m_Items)[nNewIndex]; 

			if ( pDispInfo->item.mask & LVIF_TEXT ) {
				if ( pDispInfo->item.iSubItem ) {
					const TCHAR *pSubText = pItem->GetSubItemText(pDispInfo->item.iSubItem - 1);
					if ( pSubText )
						_tcsncpy( pDispInfo->item.pszText, pSubText, pDispInfo->item.cchTextMax );
					else
						_tcsncpy( pDispInfo->item.pszText, _T(""), pDispInfo->item.cchTextMax );
				}
				else {
					_tcsncpy( pDispInfo->item.pszText, pItem->GetText(), pDispInfo->item.cchTextMax );
				}
			}

      if( pDispInfo->item.mask & LVIF_IMAGE ) {
        pDispInfo->item.iImage = pItem->GetSmallIconIndex();

				if ( IsUseCheckSemantic() ) {
					pDispInfo->item.mask |= LVIF_STATE;
					pDispInfo->item.stateMask = LVIS_STATEIMAGEMASK;
					pDispInfo->item.state = INDEXTOSTATEIMAGEMASK( pItem->GetImageIndex() + 2 );
				}
			}

      if( pDispInfo->item.mask & LVIF_INDENT ) {
         pDispInfo->item.iIndent = 0; 
			}

			if( pDispInfo->item.mask & LVIF_STATE && 
					pDispInfo->item.stateMask == LVIS_STATEIMAGEMASK && 
					IsUseCheckSemantic() ) {
				pDispInfo->item.state = INDEXTOSTATEIMAGEMASK( pItem->GetImageIndex() + 2 );
			}
		}
	}
}

// ---
void CReportViewList::OnOdCacheHint( NMHDR* pNMHDR, LRESULT* pResult ) {
	NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;
	
	PrepareItemsCache( pCacheHint->iFrom, pCacheHint->iTo );

	*pResult = 0;
}


// ---
void CReportViewList::OnDestroy() {
	m_bDestroyed = true;

	GetMessageReceiver()->SendMessage( RPVM_WINDOWSHUTDOWN, WPARAM(0), LPARAM(0) );

	CCheckList::OnDestroy();
}

// ---
void CReportViewList::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult ) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if ( m_Columns && m_Columns->GetSize() ) {
		CVMWaitCursor rcWait; 

		FlushItemCache();

		CCheckListColumn *pColumn = (*m_Columns)[pNMListView->iSubItem];

		RPVMessageInfo info;

		info.m_nItemIndex = pNMListView->iItem;
		info.m_nSubItemIndex = pNMListView->iSubItem;
		info.m_Value.m_bSortAscending = !pColumn->IsSortAscending();

		BOOL bResult = FALSE;
		GetMessageReceiver()->SendMessage( RPVM_DOSORTITEMS, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult )
			pColumn->SetSortAscending( !pColumn->IsSortAscending() );
	}
	*pResult = 0;
}

// ---
bool CReportViewList::CreatePopupMenu( CPoint& point, CPointer<CODMenu> &menu, int &nItem, int &nSubItem ) {
	menu = new CODMenu(this);
	menu->SetParentWnd( this );
	menu->SetBitmapBackground(RGB(0xc0,0xc0,0xc0));

	POSITION pos = GetFirstSelectedItemPosition();
	if ( pos ) 
    nItem = GetNextSelectedItem( pos );

	LVHITTESTINFO rcHti;
	rcHti.pt = point;
	ScreenToClient( &rcHti.pt );
	SubItemHitTest( &rcHti );
	m_nLastSelectedSubItem = nSubItem = rcHti.iSubItem;

	RPVMessageInfo info;

	info.m_nItemIndex = nItem;
	info.m_nSubItemIndex = nSubItem;
	info.m_Value.m_hMenu = (HMENU)NULL;

	BOOL bResult = FALSE;

	GetMessageReceiver()->SendMessage( RPVM_CREATECONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult && info.m_Value.m_hMenu )
		menu->Attach( info.m_Value.m_hMenu );
	else
	  menu->CreatePopupMenu();

	if (point.x == -1 && point.y == -1) {
		//keystroke invocation      
		if ( nItem >= 0 && m_Items && m_Items->GetSize() ) {
			CReportViewListItem * nsItem = (CReportViewListItem *)(*m_Items)[nItem]; 

			CRect itemRect;
			GetItemRect( nItem, &itemRect, LVIR_ICON );
			ClientToScreen( itemRect );
  
			point = itemRect.TopLeft(); 
			point.Offset( itemRect.Width() >> 1, itemRect.Height() >> 1 );    
		} 
		else {
			point.x = point.y = 0;
			ClientToScreen( &point );
		}
	}

	if ( m_Items && m_Items->GetSize() ) {
		CString rcMenuItemName;

		rcMenuItemName.LoadString( IDS_WAS_FINDITEM );
		menu->AppendMenu( MF_STRING, IDS_WAS_FINDITEM, rcMenuItemName );

		rcMenuItemName.LoadString( IDS_WAS_FINDNEXTITEM );
		menu->AppendMenu( MF_STRING, IDS_WAS_FINDNEXTITEM, rcMenuItemName );
	}


	info.m_nItemIndex = nItem;
	info.m_Value.m_hMenu = (HMENU)*menu;

	bResult = TRUE;

	GetMessageReceiver()->SendMessage( RPVM_PREPROCESSCONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult && menu->GetMenuItemCount() ) {
		menu->SynchronizeMenu();

		menu->LoadToolbar( IDR_WAS_TOOLBAR );

		bResult = FALSE;
		info.m_Value.m_ToolBarStuff[0] = 0;
		info.m_Value.m_ToolBarStuff[1] = 0;

		GetMessageReceiver()->SendMessage( RPVM_GETMENUTOOLBAR, WPARAM(&bResult), LPARAM(&info) );
		if ( bResult && info.m_Value.m_ToolBarStuff[0] ) {
			HINSTANCE hOldResModule = ::AfxGetResourceHandle();
			if ( info.m_Value.m_ToolBarStuff[1] )
				::AfxSetResourceHandle( HINSTANCE(info.m_Value.m_ToolBarStuff[1]) ); 

			menu->LoadToolbar( info.m_Value.m_ToolBarStuff[0] );

			::AfxSetResourceHandle( hOldResModule ); 
		}

		menu->SynchronizeIcons();
//		GetMessageReceiver()->SendMessage( WM_INITMENUPOPUP, (WPARAM)(HMENU)*menu, 0 );
	}
	else
		menu = NULL;

	return !!menu;
}


// ---
void CReportViewList::CreateAndTrackPopupMenu( CPoint& point ) {
	CPointer<CODMenu> menu;

	int nItem = -1;
	int nSubItem = -1;

	if ( CreatePopupMenu(point, menu, nItem, nSubItem) ) {

		UINT nSelected = menu->TrackPopupMenu( TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTALIGN|TPM_RIGHTBUTTON,
																							point.x, point.y, this ); 

		switch ( nSelected ) 	{
			case IDS_WAS_FINDITEM    :
					OnFindItem();
					break;
			case IDS_WAS_FINDNEXTITEM :
					OnFindNextItem();
					break;
			case 0:
					//user selected nothing
					break;
			default: {
					RPVMessageInfo info;

					info.m_nItemIndex = nItem;
					info.m_nSubItemIndex = nSubItem;
					info.m_Value.m_iCommandId = nSelected;

					BOOL bResult = TRUE;

					GetMessageReceiver()->SendMessage( RPVM_DOCONTEXTMENUCOMMAND, WPARAM(&bResult), LPARAM(&info) );
			}
					break;
		}
	}
}

// ---
void CReportViewList::OnContextMenu( CWnd* pWnd, CPoint point ) {
	CRect rcClient;
	GetClientRect( &rcClient );
	CPoint rcRPoint( point );
	ScreenToClient( &rcRPoint );
	if ( rcClient.PtInRect(rcRPoint) )
		CreateAndTrackPopupMenu( point );
	else
		CCheckList::OnContextMenu( pWnd, point );
}


// ---
void CReportViewList::FindItem() {
	if ( m_Items && m_Items->GetSize() ) {
		RPVMessageInfo info;
		info.m_nSubItemIndex = 0;
		info.m_Value.m_pContext = LPTSTR(LPCTSTR(m_FindContext));
		m_bItemFound = false;

		int nSelected = GetSelectedIndex();
		if ( nSelected < 0 )
			nSelected = m_bItemFoundInd;
		bool bTop = false;
		nSelected = nSelected + 1 < m_Items->GetSize() ? nSelected : -1;
		for ( int i=nSelected + 1,c=m_Items->GetSize(); i<c; i++ ) {
			info.m_nItemIndex = i;

			BOOL bResult = FALSE;

			GetMessageReceiver()->SendMessage( RPVM_CHECKITEMFORFIND, WPARAM(&bResult), LPARAM(&info) );

			if ( bResult ) {
				m_bItemFound = true;
				m_bItemFoundInd = i;
				SetFocus();
				SetItemState( i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
				EnsureVisible( i, FALSE );
				break;
			}

			if ( bTop && 
					((nSelected >= 0 && i == nSelected) || 
					(nSelected < 0   && i == 0)) )
				break;

			if ( i + 1 >= m_Items->GetSize() ) {
				i = -1;
				bTop = true;
			}
		}
	}
}

// ---
void CReportViewList::OnFindItem() {
	if ( m_Items && m_Items->GetSize() ) {
		RPVMessageInfo info;

		info.m_nItemIndex = GetSelectedIndex();
		info.m_nSubItemIndex = 0;
		info.m_Value.m_pContext = ::DuplicateString( m_FindContext );

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( RPVM_GETFINDITEMCONTEXT, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			m_FindContext = info.m_Value.m_pContext;

			if ( m_FindContext.GetLength() ) 
				FindItem();
		}
		delete [] info.m_Value.m_pContext;
	}
}

// ---
void CReportViewList::OnFindNextItem() {
	if ( m_FindContext.GetLength() ) 
		FindItem();
	else
		OnFindItem();
}

// ---
void CReportViewList::FindFirstItem( const TCHAR *pFindContext ) {
	m_FindContext = pFindContext;

	if ( m_FindContext.GetLength() ) {
		m_bItemFoundInd = -1;
		FindItem();
	}
}


// ---
void CReportViewList::FindNextItem() {
	OnFindNextItem();
}


// ---
void CReportViewList::OnSetFocus( CWnd* pOldWnd ) {
	CCheckList::OnSetFocus(pOldWnd);
	
	RPVMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_nSubItemIndex = 0;
	info.m_Value.m_bSetRemove = TRUE;

	GetMessageReceiver()->SendMessage( RPVM_FOCUSCHANGING, WPARAM(0), LPARAM(&info) );
}

// ---
void CReportViewList::OnKillFocus(CWnd* pNewWnd) {
	CCheckList::OnKillFocus(pNewWnd);

	RPVMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_nSubItemIndex = 0;
	info.m_Value.m_bSetRemove = FALSE;

	GetMessageReceiver()->SendMessage( RPVM_FOCUSCHANGING, WPARAM(0), LPARAM(&info) );
}

// ---
LRESULT CReportViewList::OnGetMenuItemBmp( WPARAM wParam, LPARAM lParam ) {
	RPVMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_nSubItemIndex = m_nLastSelectedSubItem;
	info.m_Value.m_MenuItemStuff[0] = wParam;
	info.m_Value.m_MenuItemStuff[1] = NULL;

	BOOL bResult = FALSE;
	GetMessageReceiver()->SendMessage( RPVM_GETMENUITEMBITMAP, WPARAM(&bResult), LPARAM(&info) );
	if ( bResult )
		*(HBITMAP *)lParam = (HBITMAP)info.m_Value.m_MenuItemStuff[1];

	return 0;
}

// --- 
void CReportViewList::DrawCellFocusRect( CPoint &point ) {
/*
  int nColumn;
  CRect rcRect;

  //the function below is provided in CListCtrl inPlace editing
  int nIndex = GetRowColumnIndex( point, &nColumn );
  if ( nIndex == -1 )
		return;

  int nOffset = 0;
  for ( int i = 0; i < nColumn; i++ )
    nOffset += GetColumnWidth(i);

  //Get the rectangle of the label and the icon
  GetItemRect( nIndex, &rcRect, LVIR_BOUNDS );
  rcRect.left += nOffset + 4;

  //Get the columnWidth of the selected nColumn
  rcRect.right = rcRect.left + GetColumnWidth( nColumn );

  Update(nIndex);
  CClientDC dc(this);    //this is the pointer of the current view
  dc.DrawFocusRect( rcRect );
*/
}


// --- 
void CReportViewList::OnLButtonDown( UINT nFlags, CPoint point ) {
	CCheckList::OnLButtonDown( nFlags, point );

	DrawCellFocusRect( point );
}

// --- 
void CReportViewList::OnRButtonDown( UINT nFlags, CPoint point ) {
	CCheckList::OnRButtonDown( nFlags, point );

	DrawCellFocusRect( point );
}


// ---
void CReportViewList::OnPaint() {
	CCheckList::OnPaint();
}

// ---
void CReportViewList::OnItemChanging( NMHDR* pNMHDR, LRESULT* pResult ) {
	CCheckList::OnItemChanging( pNMHDR, pResult );
	if ( !*pResult ) {
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if ( (pNMListView->uChanged & LVIF_STATE) && 
			(pNMListView->uOldState & LVIS_FOCUSED) &&
			!pNMListView->uNewState ) {
			
			RPVMessageInfo info;
			
			info.m_nItemIndex = pNMListView->iItem;
			info.m_Value.m_bSetRemove = FALSE;
			
			GetMessageReceiver()->SendMessage( RPVM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );
		}
		
		*pResult = 0;
	}
}

// ---
void CReportViewList::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) {
	CCheckList::OnItemChanged( pNMHDR, pResult );
	if ( !*pResult ) {
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if ( (pNMListView->uChanged & LVIF_STATE) && 
			(pNMListView->uNewState & LVIS_FOCUSED) &&
			!pNMListView->uOldState ) {
			
			RPVMessageInfo info;
			
			info.m_nItemIndex = pNMListView->iItem;
			info.m_Value.m_bSetRemove = TRUE;
			
			GetMessageReceiver()->SendMessage( RPVM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );
		}
		
		*pResult = 0;
	}
}



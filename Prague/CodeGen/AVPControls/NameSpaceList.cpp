// NameSpaceList.cpp : implementation file
//

#include "stdafx.h"
#include <StuffIO\Utils.h>
#include "NameSpaceList.h"
#include "WASCRes.rh"
#include <Wascmess.h>
#include <ODMenu.h>        // CODMenu class declaration

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameSpaceListItem

// --- 
CNameSpaceListItem::CNameSpaceListItem() :
	m_LargeIcon( NULL ), 
	m_LargeIconIndex( -1 ),
	m_SmallIcon( NULL ), 
	m_SmallIconIndex( -1 ),
	fStrict( ie_NonStrict ),
	fManuallyChecked( false ),
	fOwnsIcons( false )
{
}

// ---
bool CNameSpaceListItem::operator == ( const CCheckListItem &rItem ) {
	CNameSpaceListItem &rnsItem = (CNameSpaceListItem &)rItem;
	return CCheckListItem::operator == ( rItem )				&&
				 m_LargeIcon			== rnsItem.m_LargeIcon			&&
				 m_SmallIcon			== rnsItem.m_SmallIcon			&&
				 m_LargeIconIndex == rnsItem.m_LargeIconIndex &&
				 m_SmallIconIndex == rnsItem.m_SmallIconIndex &&
				 fStrict					== rnsItem.fStrict					&&  // Strict/Non Strict/Some Strict 
				 fManuallyChecked == rnsItem.fManuallyChecked;    // Включен вручную
}
     

// --- 
void CNameSpaceListItem::Assign( const CCheckListItem &rItem ) {
	CNameSpaceListItem &rnsItem = (CNameSpaceListItem &)rItem;

	CCheckListItem::Assign( rItem );

	m_LargeIcon				= rnsItem.m_LargeIcon ;
	m_SmallIcon				= rnsItem.m_SmallIcon ;
	m_LargeIconIndex	= rnsItem.m_LargeIconIndex ;
	m_SmallIconIndex	= rnsItem.m_SmallIconIndex ;
	fStrict						= rnsItem.fStrict ;  // Strict/Non Strict/Some Strict 
  fManuallyChecked  = rnsItem.fManuallyChecked;    // Включен вручную
}


// ---
CCheckListItem *CNameSpaceListItem::Duplicate() {
	CNameSpaceListItem *newItem = new CNameSpaceListItem;
	*newItem = *this;
	return newItem;
}


// --- 
CNameSpaceListItem::~CNameSpaceListItem() {
}


// --- 
int	CNameSpaceListItem::GetImageIndex() {
//	return CCheckListItem::GetImageIndex()// + ii_LastIndex * GetStrict(); 
	int nIndexShift = 0;
/*
	if ( IsHasCheckedDescendants() )
		nIndexShift = iti_LastIndex;

	if ( IsHasStrictCheckedDescendants() )
		nIndexShift = iti_LastIndex << 1;
*/
	if ( IsEnabled() ) {
		return int(( GetChecked() == ie_Checked
								?	iti_Checked
								: iti_Unchecked
							 ) + nIndexShift) + 
							     ((iti_LastIndex * 3) * (int)IsStrict());
	}
	else {
		nIndexShift /= 2;
		return int(( GetChecked() == ie_Checked
								?	iti_Checked
								: iti_Unchecked
							 ) + nIndexShift) + 
							     ((iti_LastIndex * 3) * 2);
	}

/*
	return int(( GetChecked() == ie_Checked
							?	iti_Checked
							: iti_Unchecked
						 ) + nIndexShift) + (iti_LastIndex * 3 * (int)IsStrict());
*/
}


/////////////////////////////////////////////////////////////////////////////
// CNameSpaceList
// ---
CNameSpaceList::CNameSpaceList() :
	m_nLargeImageSize(32, 32),//( ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON) ),
	m_nSmallImageSize(16, 16)//( ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CXSMICON) )
{
	// ImageList в классе CCheckList будет типа State
	SetStateImageType( true );	
}

// ---
CNameSpaceList::~CNameSpaceList() {
}

// ---
void CNameSpaceList::PreSubclassWindow() {
	CCheckList::PreSubclassWindow();

	if ( ::IsDlgClassName(*GetParent()) ) 
		SetShowSelectionAlways( fShowSelectionAlways );
}


// ---
COLORREF CNameSpaceList::SetBkColor( COLORREF crColor ) {
	if ( m_NameLargeImageList.GetSafeHandle() )
		m_NameLargeImageList.SetBkColor( crColor );

	if ( m_NameSmallImageList.GetSafeHandle() )
		m_NameSmallImageList.SetBkColor( crColor );

	return CCheckList::SetBkColor( crColor );
}


// ---
void CNameSpaceList::SetImageListSize( SIZE nLargeSize, SIZE nSmallSize ) {
	m_nLargeImageSize = nLargeSize;
	m_nSmallImageSize = nSmallSize;

	if ( m_NameLargeImageList.GetSafeHandle() && m_NameSmallImageList.GetSafeHandle()	) {
		SetImageList( NULL, LVSIL_NORMAL );
		SetImageList( NULL, LVSIL_SMALL );

		m_NameLargeImageList.DeleteImageList();
		m_NameSmallImageList.DeleteImageList();

		CreateStandartImageList();
	}
}

// ---
void CNameSpaceList::CreateImageList() {

	m_ImageList.Create( IDB_WAS_NAMELISTIMAGE, 25, 1, RGB(255, 255, 255) );
	SetImageList( &m_ImageList, LVSIL_STATE );

	CreateStandartImageList();
}

// ---
void CNameSpaceList::CreateStandartImageList() {
	m_NameLargeImageList.Create( m_nLargeImageSize.cx, m_nLargeImageSize.cy,
															 ILC_COLOR24 | ILC_MASK, 25, 1);

	m_NameLargeImageList.SetBkColor( GetBkColor() );

	m_NameSmallImageList.Create( m_nSmallImageSize.cx, m_nSmallImageSize.cy, 
															 ILC_COLOR24 | ILC_MASK, 25, 1);

	m_NameSmallImageList.SetBkColor( GetBkColor() );
/*
  CBitmap largeBmp;
  CBitmap smallBmp;
  CBitmap *oldBmp;
  CBitmap *newBmp;
  CDC dc;

  dc.CreateCompatibleDC( NULL );

  largeBmp.CreateBitmap( 32, 32, 1, 24, NULL );

  oldBmp = dc.SelectObject( &largeBmp );

  dc.FillSolidRect( 0, 0, 31, 31, GetSysColor(COLOR_WINDOW) );

	newBmp = dc.SelectObject( oldBmp );

  m_NameLargeImageList.Add( newBmp, COLORREF(0x00) );


  smallBmp.CreateBitmap( 16, 16, 1, 24, NULL );

  oldBmp = dc.SelectObject( &smallBmp );

  dc.FillSolidRect( 0, 0, 15, 15, GetSysColor(COLOR_WINDOW) );

	newBmp = dc.SelectObject( oldBmp );

  dc.DeleteDC ();

  m_NameSmallImageList.Add( newBmp, COLORREF(0x00) );
*/
	int nImage = m_NameSmallImageList.Add( AfxGetApp()->LoadIcon(IDI_WAS_NAMETREEOVERLAYIMAGE) );
	m_NameSmallImageList.SetOverlayImage( nImage, 1 );

	nImage = m_NameLargeImageList.Add( AfxGetApp()->LoadIcon(IDI_WAS_NAMETREEOVERLAYIMAGE) );
	m_NameLargeImageList.SetOverlayImage( nImage, 1 );

	SetImageList( &m_NameLargeImageList, LVSIL_NORMAL );
	SetImageList( &m_NameSmallImageList, LVSIL_SMALL );
}

// ---
void CNameSpaceList::FlushItemImages() {
	int i, c;
	for ( i=0,c=m_NameLargeImageList.GetImageCount(); i<c; i++ ) 
		m_NameLargeImageList.Remove( i );
	m_LargeIconCache.Flush();

	for ( i=0,c=m_NameSmallImageList.GetImageCount(); i<c; i++ ) 
		m_NameSmallImageList.Remove( i );
	m_SmallIconCache.Flush();
}


// ---
void CNameSpaceList::SetItemIcons( int iItemIndex ) {
	if ( iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) {
		CNameSpaceListItem * nsItem = (CNameSpaceListItem *)(*m_Items)[iItemIndex]; 

		int iLarge = nsItem->GetLargeIconIndex();
		int iSmall = nsItem->GetSmallIconIndex();

		if ( nsItem->GetLargeIcon() ) {
			// Найти Image элемента среди уже имеющихся
			int iCacheIndex;

			if ( (iCacheIndex = m_LargeIconCache.FindIt(nsItem->GetLargeIcon())) != -1 ) {
				iLarge = iCacheIndex;
				/*
				if ( nsItem->IsOwnsIcons() )
					m_LargeIconCache.Condemn( nsItem->GetLargeIcon() );
				*/
			}
			else {
				iLarge = m_NameLargeImageList.Add( nsItem->GetLargeIcon() );
	//			SetImageList( &m_NameLargeImageList, LVSIL_NORMAL );

				int iNewIndex = m_LargeIconCache.Add( nsItem->GetLargeIcon() );
				m_LargeIconCache.SetItemIndex( iNewIndex, iLarge );
			}
		}

		if ( nsItem->GetSmallIcon() ) {
			// Найти Image элемента среди уже имеющихся
			int iCacheIndex;

			if ( (iCacheIndex = m_SmallIconCache.FindIt(nsItem->GetSmallIcon())) != -1 ) {
				iSmall = iCacheIndex;
				/*
				if ( nsItem->IsOwnsIcons() )
					m_SmallIconCache.Condemn( nsItem->GetSmallIcon() );
				*/
			}
			else {
				iSmall = m_NameSmallImageList.Add( nsItem->GetSmallIcon() );
	//			SetImageList( &m_NameSmallImageList, LVSIL_SMALL );

				int iNewIndex = m_SmallIconCache.Add( nsItem->GetSmallIcon() );
				m_SmallIconCache.SetItemIndex( iNewIndex, iSmall );
			}
		}


		SetItemImage( iItemIndex, iLarge );
		SetItemImage( iItemIndex, iSmall );

		nsItem->SetIconIndexes( iSmall, iLarge );
		nsItem->SetIcons( NULL, NULL );
	}
}


// ---
void CNameSpaceList::SetItemImages( CCheckListItem *clItem, int nItemIndex ) {

	CCheckList::SetItemImages( clItem, nItemIndex );
/*
	CNameSpaceListItem *nsItem = (CNameSpaceListItem *)clItem;

	int nOverLayInd = ( nsItem->GetStrict() == CNameSpaceListItem::ie_Strict ) ? 1 : 0;
	SetItemState( nItemIndex, INDEXTOOVERLAYMASK(nOverLayInd), LVIS_OVERLAYMASK );
*/
}


// ---
void CNameSpaceList::AddItem( int iItemIndex ) {

	CCheckList::AddItem( iItemIndex );

	SetItemIcons( iItemIndex );
}


// ---
void CNameSpaceList::CheckItem( int iItemIndex ) {
	if ( iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) {
		CNameSpaceListItem *item = (CNameSpaceListItem *)(*m_Items)[iItemIndex];

		NSLMessageInfo info;

		info.m_nItemIndex = iItemIndex;
		info.m_Value.m_bSetRemove = !item->IsChecked();

		item->SetManuallyChecked( true );

		BOOL bResult = TRUE;

		GetMessageReceiver()->SendMessage( NSLM_MANUALLYSETREMOVECHECKED, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult )
			SetItemChecked( iItemIndex, CCheckListItem::CieChecked(!item->IsChecked()) );
	}
}

// Установить Strict
// ---
void CNameSpaceList::StrictItem( int iItemIndex, bool strict ) {
	if ( iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) {
		NSLMessageInfo info;

		info.m_nItemIndex = iItemIndex;
		info.m_Value.m_bSetRemove = strict;

		BOOL bResult = TRUE;

		GetMessageReceiver()->SendMessage( NSLM_MANUALLYSETREMOVESTRICT, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult )
			SetItemStrict( iItemIndex, strict
																 ? CNameSpaceListItem::ie_Strict 
																 : CNameSpaceListItem::ie_NonStrict );
	}
}


// Установить признак Strict на элемент
// ---
void CNameSpaceList::SetItemStrict( int iItemIndex, 
																		CNameSpaceListItem::CieStrict fHowToStrict, 
																		bool bSendMessage ) {

	if ( m_Items && m_Items->GetSize() && iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) {
		CNameSpaceListItem *item = (CNameSpaceListItem *)(*m_Items)[iItemIndex];

		item->SetStrict( fHowToStrict );

			// Установить картинки
		SetItemImages( item, iItemIndex );

		if ( bSendMessage ) {
			NSLMessageInfo info;

			info.m_nItemIndex = iItemIndex;

			GetMessageReceiver()->SendMessage( NSLM_STATECHANGED, WPARAM(0), LPARAM(&info) );
		}
	}
}


// Установить признак Checked на элемент
// ---
void CNameSpaceList::SetItemChecked( int iItemIndex, 
																		 CCheckListItem::CieChecked fHowToCheck,
																		 bool bSendMessage ) {
	if ( m_Items && m_Items->GetSize() && iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) {
		CCheckList::SetItemChecked( iItemIndex, fHowToCheck, false );

		if ( bSendMessage ) {
			NSLMessageInfo info;

			info.m_nItemIndex = iItemIndex;

			GetMessageReceiver()->SendMessage( NSLM_STATECHANGED, WPARAM(0), LPARAM(&info) );
		}
	}
}


#define CListCtrl CCheckList
BEGIN_MESSAGE_MAP(CNameSpaceList, CListCtrl)
#undef CListCtrl
	//{{AFX_MSG_MAP(CNameSpaceList)
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameSpaceList message handlers
// ---
int CNameSpaceList::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
	if (CCheckList::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if ( !::IsDlgClassName(*GetParent()) ) 
		SetShowSelectionAlways( fShowSelectionAlways );

	return 0;
}

// ---
void CNameSpaceList::OnRButtonDown( UINT nFlags, CPoint point ) {
	// but don't call default.... otherwise WM_CONTEXTMENU is NOT sent
  //CCheckList::OnRButtonDown( nFlags, point );

	UINT flags;
  int iItemIndex = HitTest( point, &flags );
	
	::SetFocus( *this );
	if ( iItemIndex < 0 )
		iItemIndex = m_nLastSelectedInd;
	
	SetItemSelected( iItemIndex );
	m_nLastSelectedInd = iItemIndex;
	
}

// ---
void CNameSpaceList::OnContextMenu( CWnd* pWnd, CPoint point ) {
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
void CNameSpaceList::InsideImageClickImitate( int nItemOnWhich ) {
	CImageList *pImgNormal = GetImageList( LVSIL_NORMAL );		
	ASSERT( pImgNormal );

	IMAGEINFO imageInfoNormal;
	pImgNormal->GetImageInfo( 0, &imageInfoNormal );		

	CImageList *pImgState = GetImageList( LVSIL_STATE );		
	ASSERT( pImgState );

	IMAGEINFO imageInfoState;
	pImgState->GetImageInfo( 0, &imageInfoState );		

	CRect rect;
	GetItemRect( nItemOnWhich, &rect, LVIR_ICON );
	rect.right = (rect.left - 2) - (imageInfoNormal.rcImage.right - 2);		
	rect.left -= (imageInfoState.rcImage.right + 2) + (imageInfoNormal.rcImage.right + 2);					

	CPoint point = rect.TopLeft(); 
	point.Offset( rect.Width() >> 1, rect.Height() >> 1 );    

	PostMessage( WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y) );
	PostMessage( WM_LBUTTONUP  , 0, MAKELPARAM(point.x, point.y) );
}


#define SETCHECKED(type) ((GetViewType() == type) ? MF_CHECKED : 0)

// ---
void CNameSpaceList::CreateViewMenu( HMENU menu ) {
	CString rcMenuItemName;

	rcMenuItemName.LoadString( IDS_WAS_VIEWLARGEICONS );
	::AppendMenu( menu,MF_STRING | SETCHECKED(LVS_ICON), IDS_WAS_VIEWLARGEICONS, rcMenuItemName );

	rcMenuItemName.LoadString( IDS_WAS_VIEWSMALLICONS );
	::AppendMenu( menu,MF_STRING | SETCHECKED(LVS_SMALLICON), IDS_WAS_VIEWSMALLICONS, rcMenuItemName );

	rcMenuItemName.LoadString( IDS_WAS_VIEWLIST );
	::AppendMenu( menu,MF_STRING | SETCHECKED(LVS_LIST), IDS_WAS_VIEWLIST, rcMenuItemName );

	rcMenuItemName.LoadString( IDS_WAS_VIEWDETAILS );
	::AppendMenu( menu,MF_STRING | SETCHECKED(LVS_REPORT), IDS_WAS_VIEWDETAILS, rcMenuItemName );
}


// ---
bool CNameSpaceList::CreatePopupMenu( CPoint& point, CPArray<CODMenu> &menus, int &nItem ) {
	menus.Add( new CODMenu(this) );
	menus[0]->CreatePopupMenu();

	POSITION pos = GetFirstSelectedItemPosition();
	if ( pos ) {
    nItem = GetNextSelectedItem( pos );
		CNameSpaceListItem * nsItem = (CNameSpaceListItem *)(*m_Items)[nItem]; 

		if (point.x == -1 && point.y == -1) {
		 //keystroke invocation      
			CRect itemRect;
			GetItemRect( nItem, &itemRect, LVIR_ICON );
			ClientToScreen( itemRect );
  
			point = itemRect.TopLeft(); 
			point.Offset( itemRect.Width() >> 1, itemRect.Height() >> 1 );    
		} 

		
		CString rcMenuItemName;

		if ( nsItem->IsChecked() ) {
			rcMenuItemName.LoadString( IDS_WAS_UNCHECKNODE );
			menus[0]->AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_UNCHECKNODE, rcMenuItemName );
		}
		else {
			rcMenuItemName.LoadString( IDS_WAS_CHECKNODE );
			menus[0]->AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_CHECKNODE, rcMenuItemName );
		}

		if ( nsItem->GetStrict() == CNameSpaceListItem::ie_Strict ) {
			rcMenuItemName.LoadString( IDS_WAS_REMOVESTRICT );
			menus[0]->AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_REMOVESTRICT, rcMenuItemName );
		}
		else {
			rcMenuItemName.LoadString( IDS_WAS_SETSTRICT );
			menus[0]->AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_SETSTRICT, rcMenuItemName );
		}

		menus.Add( new CODMenu(this) );

		menus[1]->CreatePopupMenu();

		CreateViewMenu( *menus[1] );

		menus[0]->AppendMenu( MF_SEPARATOR );

		rcMenuItemName.LoadString( IDS_WAS_VIEW );

		menus[0]->AppendMenu( MF_STRING | MF_POPUP, (UINT)(HMENU)*menus[1], rcMenuItemName );

		MENUITEMINFO rcMii;
		::ZeroMemory( &rcMii, sizeof(rcMii) );
		rcMii.cbSize = sizeof(rcMii);
		rcMii.wID = IDS_WAS_VIEW;
		rcMii.fMask = MIIM_ID;

		::SetMenuItemInfo( *menus[0], menus[0]->GetMenuItemCount() - 1, TRUE, &rcMii );          
	}
	else {
		CreateViewMenu( (HMENU)*menus[0] );
		if (point.x == -1 && point.y == -1) {
			point.x = point.y = 0;
			ClientToScreen( &point );
		}
	}

	NSLMessageInfo info;

	info.m_nItemIndex = nItem;
	info.m_Value.m_hMenu = (HMENU)*menus[0];

	BOOL bResult = TRUE;

	GetMessageReceiver()->SendMessage( NSLM_PREPROCESSCONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult && menus[0]->GetMenuItemCount() ) {
		// Все меню должны быть синхронизированы до загрузки toolbar
		int i, c;
		for ( i=0,c=menus.Count(); i<c; i++ ) 
			menus[i]->SynchronizeMenu();

		for ( i=0,c=menus.Count(); i<c; i++ ) 
			menus[i]->LoadToolbar( IDR_WAS_TOOLBAR );

		bResult = FALSE;
		info.m_Value.m_ToolBarStuff[0] = 0;
		info.m_Value.m_ToolBarStuff[1] = 0;

		GetMessageReceiver()->SendMessage( NSLM_GETMENUTOOLBAR, WPARAM(&bResult), LPARAM(&info) );
		if ( bResult && info.m_Value.m_ToolBarStuff[0] ) {
			HINSTANCE hOldResModule = ::AfxGetResourceHandle();
			if ( info.m_Value.m_ToolBarStuff[1] )
				::AfxSetResourceHandle( HINSTANCE(info.m_Value.m_ToolBarStuff[1]) ); 

			for ( int i=0,c=menus.Count(); i<c; i++ ) 
				menus[i]->LoadToolbar( info.m_Value.m_ToolBarStuff[0] );

			::AfxSetResourceHandle( hOldResModule ); 
		}

//		GetMessageReceiver()->SendMessage( WM_INITMENUPOPUP, (WPARAM)(HMENU)*menus[0], 0 );
	}
	else
		menus.Flush();

	return !!menus.Count();
}


// ---
void CNameSpaceList::CreateAndTrackPopupMenu( CPoint& point ) {
	CPArray<CODMenu> menus;

	int nItem = -1;

	if ( CreatePopupMenu(point, menus, nItem) ) {

		UINT nSelected = menus[0]->TrackPopupMenu( TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTALIGN|TPM_RIGHTBUTTON,
																							point.x, point.y, this ); 

		switch ( nSelected ) 	{
			case IDS_WAS_CHECKNODE   :
			case IDS_WAS_UNCHECKNODE : 
					//if ( GetSelectedCount() == 1 ) {
//					InsideImageClickImitate( nItem );
					CheckItem( nItem );
					break;
			case IDS_WAS_SETSTRICT    :
			case IDS_WAS_REMOVESTRICT : 
					StrictItem( nItem, nSelected == IDS_WAS_SETSTRICT );
					break;
			case IDS_WAS_VIEWLARGEICONS :
					SetViewType( LVS_ICON );
					break;
			case IDS_WAS_VIEWSMALLICONS :
					SetViewType( LVS_SMALLICON );
					break;
			case IDS_WAS_VIEWLIST :
					SetViewType( LVS_LIST );
					break;
			case IDS_WAS_VIEWDETAILS :
					SetViewType( LVS_REPORT );
					break;
			case 0:
					//user selected nothing
					break;
			default: {
					NSLMessageInfo info;

					info.m_nItemIndex = nItem;
					info.m_Value.m_iCommandId = nSelected;

					BOOL bResult = TRUE;

					GetMessageReceiver()->SendMessage( NSLM_DOCONTEXTMENUCOMMAND, WPARAM(&bResult), LPARAM(&info) );
			}
					break;
		}
	}
}

// ---
void CNameSpaceList::OnItemChanging( NMHDR* pNMHDR, LRESULT* pResult ) {
	CCheckList::OnItemChanging( pNMHDR, pResult );
	if ( !*pResult ) {
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if ( (pNMListView->uChanged & LVIF_STATE) && 
				 (pNMListView->uOldState & LVIS_FOCUSED) &&
				 !pNMListView->uNewState ) {

			BeginWaitCursor(); 

			NSLMessageInfo info;

			info.m_nItemIndex = pNMListView->iItem;
			info.m_Value.m_bSetRemove = false;

			GetMessageReceiver()->SendMessage( NSLM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );
		}

		*pResult = 0;
	}
}

// ---
void CNameSpaceList::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) {
	CCheckList::OnItemChanged( pNMHDR, pResult );
	if ( !*pResult ) {
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if ( (pNMListView->uChanged & LVIF_STATE) && 
				 (pNMListView->uNewState & LVIS_FOCUSED) &&
				 !pNMListView->uOldState ) {

			NSLMessageInfo info;

			info.m_nItemIndex = pNMListView->iItem;
			info.m_Value.m_bSetRemove = true;

			GetMessageReceiver()->SendMessage( NSLM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );

			EndWaitCursor(); 
		}

		*pResult = 0;
	}
}


// ---
void CNameSpaceList::OnDestroy() {
	GetMessageReceiver()->SendMessage( NSLM_WINDOWSHUTDOWN, WPARAM(0), LPARAM(0) );

	CCheckList::OnDestroy();
}


// ---
void CNameSpaceList::OnKillFocus( CWnd* pNewWnd ) {
	CCheckList::OnKillFocus(pNewWnd);

	NSLMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_Value.m_bSetRemove = FALSE;

	GetMessageReceiver()->SendMessage( NSLM_FOCUSCHANGING, WPARAM(HWND(*pNewWnd)), LPARAM(&info) );
}


// ---
void CNameSpaceList::OnSetFocus(CWnd* pOldWnd) {
	CCheckList::OnSetFocus(pOldWnd);

	NSLMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_Value.m_bSetRemove = TRUE;

	GetMessageReceiver()->SendMessage( NSLM_FOCUSCHANGING, WPARAM(HWND(*pOldWnd)), LPARAM(&info) );
}

// ---
void CNameSpaceList::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ) {
	*pResult = 0;
	LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW)pNMHDR;
	switch ( lpNMCustomDraw->nmcd.dwDrawStage ) {
		case CDDS_PREPAINT :
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT : {
			*pResult = CDRF_DODEFAULT;
			CNameSpaceListItem *nsItem = (CNameSpaceListItem *)GetItemData( lpNMCustomDraw->nmcd.dwItemSpec );
			if ( nsItem ) {
				if ( !nsItem->IsEnabled() )
					lpNMCustomDraw->clrText = ::GetSysColor( COLOR_GRAYTEXT );
				HFONT hFont = nsItem->GetFont();
				if ( hFont ) {
					HFONT hOldFont = (HFONT)::SelectObject( lpNMCustomDraw->nmcd.hdc, hFont );
					if ( !m_hOldFont )
						m_hOldFont = hOldFont;
					*pResult = CDRF_NEWFONT;
				}
			}
			break;
		}
	}
}


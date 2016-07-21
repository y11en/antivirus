// AboutViewList.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include "AboutViewList.h"
#include "WASCRes.rh"
#include "WASCMess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcsclen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif



// ---
static TCHAR *DuplicateString( const TCHAR *s ) {
  return s ? _tcscpy(new TCHAR[TCSCLEN(s)+1], s) : _tcscpy(new TCHAR[1], _T(""));
}

/////////////////////////////////////////////////////////////////////////////
// CAboutViewListItem

// --- 
CAboutViewListItem::CAboutViewListItem() :
	m_SmallIcon( NULL ), 
	m_SmallIconIndex( -1 ),
	m_Data( NULL ),
	fOwnsData( false ),
	fOwnsIcons( false )
{
}

// --- 
CAboutViewListItem::~CAboutViewListItem() {
	if ( fOwnsData )
		delete m_Data;
}


// --- 
int	CAboutViewListItem::GetImageIndex() {
	return CCheckListItem::GetImageIndex(); 
}

// ---
void CAboutViewListItem::Empty() {
	GetSubItemsTexts().RemoveAll();
	GetText() = _T("");
	SetCurrImageIndex( -1 );
}

/////////////////////////////////////////////////////////////////////////////
// CAboutViewList

CAboutViewList::CAboutViewList() :
	m_nSmallImageSize( ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CXSMICON) ),
	fUseItemImages( false )	    // Использовать иконы на элементах
{
	HINSTANCE hInst = AfxFindResourceHandle( MAKEINTRESOURCE(IDR_WAS_ABOUTVIEWLIST), RT_ACCELERATOR );
	SetAccelTable( ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_WAS_ABOUTVIEWLIST)) );
}

// ---
CAboutViewList::~CAboutViewList() {
}

// ---
COLORREF CAboutViewList::SetBkColor( COLORREF crColor ) {
	if ( m_SmallImageList.GetSafeHandle() )
		m_SmallImageList.SetBkColor( crColor );

	return CCheckList::SetBkColor( crColor );
}


// ---
void CAboutViewList::CreateImageList() {
/*
  if ( IsUseCheckSemantic() ) {
		SetStateImageType();
		m_ImageList.Create( IDB_WAS_ABOUTLISTIMAGE, 25, 1, RGB(255, 255, 255) );
		SetImageList( &m_ImageList, LVSIL_STATE );
	}
*/
	CreateStandartImageList();
}

// ---
void CAboutViewList::CreateStandartImageList() {
	if ( IsUseItemImages() ) {
		m_SmallImageList.Create( m_nSmallImageSize.cx, m_nSmallImageSize.cy, 
																 ILC_COLOR24 | ILC_MASK, 25, 1);

		m_SmallImageList.SetBkColor( GetBkColor() );

		SetImageList( &m_SmallImageList, LVSIL_SMALL );
	}
}


// ---
void CAboutViewList::SetItemIcons( int iItemIndex ) {
	if ( IsUseItemImages() ) {
		CAboutViewListItem * nsItem = (CAboutViewListItem *)(*m_Items)[iItemIndex]; 

		int iSmall = nsItem->GetSmallIconIndex();

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

		SetItemImage( iItemIndex, iSmall );

		nsItem->SetIconIndexes( iSmall );
		nsItem->SetIcons( NULL );
	}
}


// ---
void CAboutViewList::AddNewItem( CCheckListItem *pItem, bool bEnsureVisible ) {
	CCheckList::AddNewItem( pItem, bEnsureVisible );
/* За установку ширины колонок отвечает host
	if ( m_Items->GetSize() == 1 )
		SetColumnsWidth();
*/
}


// ---
void CAboutViewList::AddItem( int iItemIndex ) {

	CCheckList::AddItem( iItemIndex );

	SetItemIcons( iItemIndex );
}


#define CListCtrl CCheckList
BEGIN_MESSAGE_MAP(CAboutViewList, CListCtrl)
#undef CListCtrl
	//{{AFX_MSG_MAP(CAboutViewList)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDS_WAS_FINDITEM, OnFindItem)
	ON_COMMAND(IDS_WAS_FINDNEXTITEM, OnFindNextItem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutViewList message handlers
// ---
void CAboutViewList::OnDestroy() {
	GetMessageReceiver()->SendMessage( ABTM_WINDOWSHUTDOWN, WPARAM(0), LPARAM(0) );

	CCheckList::OnDestroy();
}

// ---
bool CAboutViewList::CreatePopupMenu( CPoint& point, CPointer<CODMenu> &menu, int &nItem ) {
	menu = new CODMenu(this);

	RPVMessageInfo info;

	info.m_nItemIndex = nItem;
	info.m_Value.m_hMenu = (HMENU)NULL;

	BOOL bResult = FALSE;

	GetMessageReceiver()->SendMessage( ABTM_CREATECONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult && info.m_Value.m_hMenu )
		menu->Attach( info.m_Value.m_hMenu );
	else
	  menu->CreatePopupMenu();

	POSITION pos = GetFirstSelectedItemPosition();
	if ( pos ) {
    nItem = GetNextSelectedItem( pos );
		CAboutViewListItem * nsItem = (CAboutViewListItem *)(*m_Items)[nItem]; 

		if (point.x == -1 && point.y == -1) {
		 //keystroke invocation      
			CRect itemRect;
			GetItemRect( nItem, &itemRect, LVIR_ICON );
			ClientToScreen( itemRect );
  
			point = itemRect.TopLeft(); 
			point.Offset( itemRect.Width() >> 1, itemRect.Height() >> 1 );    
		} 
	}
	else {
		if (point.x == -1 && point.y == -1) {
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

	GetMessageReceiver()->SendMessage( ABTM_PREPROCESSCONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult && menu->GetMenuItemCount() ) {
		menu->SynchronizeMenu();

		menu->LoadToolbar( IDR_WAS_TOOLBAR );

		bResult = FALSE;
		info.m_Value.m_ToolBarStuff[0] = 0;
		info.m_Value.m_ToolBarStuff[1] = 0;

		GetMessageReceiver()->SendMessage( ABTM_GETMENUTOOLBAR, WPARAM(&bResult), LPARAM(&info) );
		if ( bResult && info.m_Value.m_ToolBarStuff[0] ) {
			HINSTANCE hOldResModule = ::AfxGetResourceHandle();
			if ( info.m_Value.m_ToolBarStuff[1] )
				::AfxSetResourceHandle( HINSTANCE(info.m_Value.m_ToolBarStuff[1]) ); 

			menu->LoadToolbar( info.m_Value.m_ToolBarStuff[0] );

			::AfxSetResourceHandle( hOldResModule ); 
		}

//		GetMessageReceiver()->SendMessage( WM_INITMENUPOPUP, (WPARAM)(HMENU)*menu, 0 );
	}
	else
		menu = NULL;

	return !!menu;
}


// ---
void CAboutViewList::CreateAndTrackPopupMenu( CPoint& point ) {
	CPointer<CODMenu> menu;

	int nItem = -1;

	if ( CreatePopupMenu(point, menu, nItem) ) {

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
					info.m_Value.m_iCommandId = nSelected;

					BOOL bResult = TRUE;

					GetMessageReceiver()->SendMessage( ABTM_DOCONTEXTMENUCOMMAND, WPARAM(&bResult), LPARAM(&info) );
			}
					break;
		}
	}
}

// ---
void CAboutViewList::OnContextMenu( CWnd* pWnd, CPoint point ) {
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
void CAboutViewList::FindItem() {
	if ( m_Items && m_Items->GetSize() ) {
		RPVMessageInfo info;
		info.m_Value.m_pContext = LPTSTR(LPCTSTR(m_FindContext));

		int nSelected = GetSelectedIndex();
		bool bTop = false;
		nSelected = nSelected + 1 < m_Items->GetSize() ? nSelected : -1;
		for ( int i=nSelected + 1,c=m_Items->GetSize(); i<c; i++ ) {
			info.m_nItemIndex = i;

			BOOL bResult = FALSE;

			GetMessageReceiver()->SendMessage( ABTM_CHECKITEMFORFIND, WPARAM(&bResult), LPARAM(&info) );

			if ( bResult ) {
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
void CAboutViewList::OnFindItem() {
	if ( m_Items && m_Items->GetSize() ) {
		RPVMessageInfo info;

		info.m_nItemIndex = GetSelectedIndex();
		info.m_Value.m_pContext = ::DuplicateString( m_FindContext );

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( ABTM_GETFINDITEMCONTEXT, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			m_FindContext = info.m_Value.m_pContext;

			if ( m_FindContext.GetLength() ) 
				FindItem();
		}
		delete [] info.m_Value.m_pContext;
	}
}

// ---
void CAboutViewList::OnFindNextItem() {
	if ( m_FindContext.GetLength() ) 
		FindItem();
}

// ---
void CAboutViewList::FindFirstItem( const char *pFindContext ) {
	m_FindContext = pFindContext;

	if ( m_FindContext.GetLength() ) 
		FindItem();
}


// ---
void CAboutViewList::FindNextItem() {
	OnFindNextItem();
}

// ---
void CAboutViewList::OnItemChanging( NMHDR* pNMHDR, LRESULT* pResult ) {
	CCheckList::OnItemChanging( pNMHDR, pResult );
	if ( !*pResult ) {
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if ( (pNMListView->uChanged & LVIF_STATE) && 
			(pNMListView->uOldState & LVIS_FOCUSED) &&
			!pNMListView->uNewState ) {
			
			ABTMessageInfo info;
			
			info.m_nItemIndex = pNMListView->iItem;
			info.m_Value.m_bSetRemove = false;
			
			GetMessageReceiver()->SendMessage( ABTM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );
		}
		
		*pResult = 0;
	}
}

// ---
void CAboutViewList::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) {
	CCheckList::OnItemChanged( pNMHDR, pResult );
	if ( !*pResult ) {
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if ( (pNMListView->uChanged & LVIF_STATE) && 
			(pNMListView->uNewState & LVIS_FOCUSED) &&
			!pNMListView->uOldState ) {
			
			ABTMessageInfo info;
			
			info.m_nItemIndex = pNMListView->iItem;
			info.m_Value.m_bSetRemove = true;
			
			GetMessageReceiver()->SendMessage( ABTM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );
		}
		
		*pResult = 0;
	}
}



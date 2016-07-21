// StatisticViewList.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include "StatisticViewList.h"
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
// CStatisticViewListItem

// --- 
CStatisticViewListItem::CStatisticViewListItem() :
	m_SmallIcon( NULL ), 
	m_SmallIconIndex( -1 ),
	fOwnsIcons( false )
{
}

// --- 
CStatisticViewListItem::~CStatisticViewListItem() {
}


// --- 
int	CStatisticViewListItem::GetImageIndex() {
	return CCheckListItem::GetImageIndex(); 
}

// ---
void CStatisticViewListItem::Empty() {
	GetSubItemsTexts().RemoveAll();
	GetText() = _T("");
	SetCurrImageIndex( -1 );
}

/////////////////////////////////////////////////////////////////////////////
// CStatisticViewList

CStatisticViewList::CStatisticViewList() :
	m_nSmallImageSize( ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CXSMICON) ),
	fUseItemImages( false )	    // Использовать иконы на элементах
{
	HINSTANCE hInst = AfxFindResourceHandle( MAKEINTRESOURCE(IDR_WAS_STATISTICVIEWLIST), RT_ACCELERATOR );
	SetAccelTable( ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_WAS_STATISTICVIEWLIST)) );

	SetDisplayNoItemsString();
}

// ---
CStatisticViewList::~CStatisticViewList() {
}

// ---
COLORREF CStatisticViewList::SetBkColor( COLORREF crColor ) {
	if ( m_SmallImageList.GetSafeHandle() )
		m_SmallImageList.SetBkColor( crColor );

	return CCheckList::SetBkColor( crColor );
}


// ---
void CStatisticViewList::CreateImageList() {
  if ( IsUseCheckSemantic() ) {
		SetStateImageType();
		m_ImageList.Create( IDB_WAS_STATISTICLISTIMAGE, 25, 1, RGB(255, 255, 255) );
		SetImageList( &m_ImageList, LVSIL_STATE );
	}
	CreateStandartImageList();
}

// ---
void CStatisticViewList::CreateStandartImageList() {
	if ( IsUseItemImages() ) {
		m_SmallImageList.Create( m_nSmallImageSize.cx, m_nSmallImageSize.cy, 
																 ILC_COLOR24 | ILC_MASK, 25, 1);

		m_SmallImageList.SetBkColor( GetBkColor() );

		SetImageList( &m_SmallImageList, LVSIL_SMALL );
	}
}


// ---
void CStatisticViewList::SetItemIcons( int iItemIndex ) {
	if ( IsUseItemImages() ) {
		CStatisticViewListItem * nsItem = (CStatisticViewListItem *)(*m_Items)[iItemIndex]; 

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
void CStatisticViewList::AddNewItem( CCheckListItem *pItem, bool bEnsureVisible ) {
	CCheckList::AddNewItem( pItem, bEnsureVisible );

	if ( m_Items->GetSize() == 1 ) {
		SetColumnsWidth();
		CorrectColumnsWidth();
	}
}


// ---
void CStatisticViewList::AddItem( int iItemIndex ) {

	CCheckList::AddItem( iItemIndex );

	SetItemIcons( iItemIndex );
}


#define CListCtrl CCheckList
BEGIN_MESSAGE_MAP(CStatisticViewList, CListCtrl)
#undef CListCtrl
	//{{AFX_MSG_MAP(CStatisticViewList)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDS_WAS_FINDITEM, OnFindItem)
	ON_COMMAND(IDS_WAS_FINDNEXTITEM, OnFindNextItem)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticViewList message handlers
// ---
void CStatisticViewList::OnDestroy() {
	GetMessageReceiver()->SendMessage( STVM_WINDOWSHUTDOWN, WPARAM(0), LPARAM(0) );

	CCheckList::OnDestroy();
}

// ---
bool CStatisticViewList::CreatePopupMenu( CPoint& point, CPointer<CODMenu> &menu, int &nItem ) {
	menu = new CODMenu(this);

	STVMessageInfo info;

	info.m_nItemIndex = nItem;
	info.m_Value.m_hMenu = (HMENU)NULL;

	BOOL bResult = FALSE;

	GetMessageReceiver()->SendMessage( STVM_CREATECONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult && info.m_Value.m_hMenu )
		menu->Attach( info.m_Value.m_hMenu );
	else
	  menu->CreatePopupMenu();

	POSITION pos = GetFirstSelectedItemPosition();
	if ( pos ) {
    nItem = GetNextSelectedItem( pos );
		CStatisticViewListItem * nsItem = (CStatisticViewListItem *)(*m_Items)[nItem]; 

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

	GetMessageReceiver()->SendMessage( STVM_PREPROCESSCONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult && menu->GetMenuItemCount() ) {
		menu->SynchronizeMenu();

		menu->LoadToolbar( IDR_WAS_TOOLBAR );

		bResult = FALSE;
		info.m_Value.m_ToolBarStuff[0] = 0;
		info.m_Value.m_ToolBarStuff[1] = 0;

		GetMessageReceiver()->SendMessage( STVM_GETMENUTOOLBAR, WPARAM(&bResult), LPARAM(&info) );
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
void CStatisticViewList::CreateAndTrackPopupMenu( CPoint& point ) {
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
					STVMessageInfo info;

					info.m_nItemIndex = nItem;
					info.m_Value.m_iCommandId = nSelected;

					BOOL bResult = TRUE;

					GetMessageReceiver()->SendMessage( STVM_DOCONTEXTMENUCOMMAND, WPARAM(&bResult), LPARAM(&info) );
			}
					break;
		}
	}
}

// ---
void CStatisticViewList::OnContextMenu( CWnd* pWnd, CPoint point ) {
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
void CStatisticViewList::FindItem() {
	if ( m_Items && m_Items->GetSize() ) {
		STVMessageInfo info;
		info.m_Value.m_pContext = LPTSTR(LPCTSTR(m_FindContext));

		int nSelected = GetSelectedIndex();
		bool bTop = false;
		nSelected = nSelected + 1 < m_Items->GetSize() ? nSelected : -1;
		for ( int i=nSelected + 1,c=m_Items->GetSize(); i<c; i++ ) {
			info.m_nItemIndex = i;

			BOOL bResult = FALSE;

			GetMessageReceiver()->SendMessage( STVM_CHECKITEMFORFIND, WPARAM(&bResult), LPARAM(&info) );

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
void CStatisticViewList::OnFindItem() {
	if ( m_Items && m_Items->GetSize() ) {
		STVMessageInfo info;

		info.m_nItemIndex = GetSelectedIndex();
		info.m_Value.m_pContext = ::DuplicateString( m_FindContext );

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( STVM_GETFINDITEMCONTEXT, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			m_FindContext = info.m_Value.m_pContext;

			if ( m_FindContext.GetLength() ) 
				FindItem();
		}
		delete [] info.m_Value.m_pContext;
	}
}

// ---
void CStatisticViewList::OnFindNextItem() {
	if ( m_FindContext.GetLength() ) 
		FindItem();
	else
		OnFindItem();
}

// ---
void CStatisticViewList::FindFirstItem( const TCHAR *pFindContext ) {
	m_FindContext = pFindContext;

	if ( m_FindContext.GetLength() ) 
		FindItem();
}


// ---
void CStatisticViewList::FindNextItem() {
	OnFindNextItem();
}


// ---
void CStatisticViewList::OnKillFocus( CWnd* pNewWnd ) {
	CCheckList::OnKillFocus(pNewWnd);

	STVMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_Value.m_bSetRemove = FALSE;

	GetMessageReceiver()->SendMessage( STVM_FOCUSCHANGING, WPARAM(HWND(*pNewWnd)), LPARAM(&info) );
}


// ---
void CStatisticViewList::OnSetFocus( CWnd* pOldWnd ) {
	CCheckList::OnSetFocus(pOldWnd);

	STVMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_Value.m_bSetRemove = FALSE;

	GetMessageReceiver()->SendMessage( STVM_FOCUSCHANGING, WPARAM(HWND(*pOldWnd)), LPARAM(&info) );
}

// ---
void CStatisticViewList::OnPaint() {
	CCheckList::OnPaint();
}

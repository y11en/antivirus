// CheckList.cpp : implementation file
//

#pragma warning( disable : 4786 )

#include "stdafx.h"
#include <tchar.h>
#include "WASCRes.rh"
#include <WASCMess.h>
#include <StuffIO\Utils.h>
#include <Stuff\SArray.h>
#include <Stuff\PArray.h>

#include "CheckList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define HDC_MINWIDTH (0)
#define HDC_WIDTHISTRUE(cx) (cx > HDC_MINWIDTH)

static const UINT nLoadLocalisedStringMessage = ::RegisterWindowMessage(_T("AVPControlsLoadLocalisedStringMessage"));


// Class to convert non-dc related painting messages to a
// messages with a dc. I will be glad when MicroSlop come
// out with a version of ListCtrl that supports DCs which
// include scroll bar and header redraw.
class CWMPaintHook {
public:
  struct CWndInfo {
    CWndInfo( HWND hWnd, HDC hdc, WNDPROC pPrevFunc ) : m_hWnd(hWnd), m_hdc(hdc), m_pPrevFunc(pPrevFunc), m_sCount(0) {};
		
    HWND    m_hWnd;
    HDC     m_hdc;
    WNDPROC m_pPrevFunc;
    short   m_sCount;
  };
	

  struct WNDINFO_MAP : public CSArray<CWndInfo> {
		public :
			CWndInfo *Find( HWND hWnd ) {
				for ( int i=0,c=Count(); i<c; i++ ) {
					if ( (*this)[i].m_hWnd == hWnd )
						return &(*this)[i];
				}
				return NULL;
			}
	};
	
	public:
		CWMPaintHook()  { _ASSERT(0); }
		CWMPaintHook(HWND hWnd, HDC hdc) : m_hWnd(hWnd) {
			WNDPROC pPrevFunc = (WNDPROC)SetWindowLong( hWnd, GWL_WNDPROC, (long)MsgProc );
			
			CWndInfo *pWndInfo = m_wndInfo.Find( hWnd );
			if ( !pWndInfo )
				m_wndInfo.Add( CWndInfo(hWnd, hdc, pPrevFunc) );
			else
				pWndInfo->m_sCount++;
		}
		
		~CWMPaintHook() {
			CWndInfo *pWndInfo = m_wndInfo.Find( m_hWnd );
			if ( pWndInfo )	{
				if ( pWndInfo->m_sCount )
					pWndInfo->m_sCount--;
				else {
					VERIFY((WNDPROC)SetWindowLong(m_hWnd, GWL_WNDPROC, (long)pWndInfo->m_pPrevFunc) == MsgProc);
					m_wndInfo.RemoveObj( *pWndInfo );
				}
			}
		}
		
	private:
		static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			CWndInfo *pWndInfo = m_wndInfo.Find( hWnd );
			if ( pWndInfo )	{
				switch ( uMsg )	{
					case WM_PAINT:
					case WM_NCPAINT:
						return ::CallWindowProc( pWndInfo->m_pPrevFunc, hWnd, WM_PRINT, (WPARAM)pWndInfo->m_hdc, lParam );
						
					case WM_ERASEBKGND:
						if ( (WPARAM)pWndInfo->m_hdc == wParam )
							return ::CallWindowProc( pWndInfo->m_pPrevFunc, hWnd, WM_ERASEBKGND, (WPARAM)pWndInfo->m_hdc, lParam );
						else
							return TRUE;
						
					default:
						return ::CallWindowProc( pWndInfo->m_pPrevFunc, hWnd, uMsg, wParam, lParam );
				}
			}
			return 0;
		}
		
	private:
		HWND    m_hWnd;
		static WNDINFO_MAP  m_wndInfo;
};

CWMPaintHook::WNDINFO_MAP CWMPaintHook::m_wndInfo;

// ---
HFONT CCLFontCache::Find( DWORD dwStyle ) {
	for ( int i=0,c=Count(); i<c; i++ ) {
		if ( (*this)[i].m_dwStyle == dwStyle )
			return (*this)[i].m_hFont;
	}
	return NULL;
}

// ---
void CCLFontCache::Flush() {
	for ( int i=0,c=Count(); i<c; i++ ) {
		::DeleteObject( (*this)[i].m_hFont );
	}
}

// ---
CCheckListItem::CCheckListItem() :
	m_Data( NULL ),
	m_Font( NULL ),
  fChecked( ie_Unchecked ),
	fEnabled( true ),						      // Разрешен
	fNode( false ),
	fOwnsData( false ),
  m_Text( _T("") ),
	m_ImageIndex( ii_LeafUnchecked ),
	m_CurrImageIndex( -1 ) {
}

// ---
CCheckListItem::~CCheckListItem() {
	if ( fOwnsData )
		delete m_Data;
}


// ---
bool operator == ( const CStringList &rFirst, const CStringList &rSecond ) {
	for( POSITION pos1 = rFirst.GetHeadPosition(), pos2=rSecond.GetHeadPosition(); pos1 != NULL && pos2 != NULL; ) 
		if ( rFirst.GetNext(pos1) != rSecond.GetNext(pos2) )
			return false;
	return true;
}

// ---
bool CCheckListItem::operator == ( const CCheckListItem &rItem ) {
  return m_Text						== rItem.m_Text						&&    // Текст узла
				 m_Data						== rItem.m_Data						&&
				 m_SubItemsTexts	== rItem.m_SubItemsTexts  &&		// Тексты Subitems
				 m_ImageIndex			== rItem.m_ImageIndex			&&		// Индекс в Image-листе
				 m_CurrImageIndex == rItem.m_CurrImageIndex &&		// Текущий индекс в Image-листе
				 fChecked					== rItem.fChecked					&&    // Включен/выключен/немного включен
				 fOwnsData				== rItem.fOwnsData				&&  // Владеет данными (сам удаляет)
				 fNode						== rItem.fNode;									// Является узлом
}
      
// ---
void Assign( CStringList &rFirst, const CStringList &rSecond ) {
	rFirst.RemoveAll();

	for( POSITION pos = rSecond.GetHeadPosition(); pos != NULL; ) 
		rFirst.AddTail( rSecond.GetNext(pos) );
}

// --- 
void CCheckListItem::Assign( const CCheckListItem &rItem ) {
  ::Assign( m_SubItemsTexts, rItem.m_SubItemsTexts );		// Тексты Subitems

	m_Data						= rItem.m_Data					;
  m_Text						= rItem.m_Text					;   // Текст узла
  m_ImageIndex			= rItem.m_ImageIndex		;		// Индекс в Image-листе
  m_CurrImageIndex	= rItem.m_CurrImageIndex;		// Текущий индекс в Image-листе
  fChecked					= rItem.fChecked				;   // Включен/выключен/немного включен
  fNode							= rItem.fNode						;		// Является узлом
	fOwnsData					= rItem.fOwnsData;  // Владеет данными (сам удаляет)

}


// ---
CCheckListItem *CCheckListItem::Duplicate() {
	return (new CCheckListItem)->operator = ( *this );
}


// --- 
void CCheckListItem::SetText( const CString &setText ) {
  m_Text = setText;
}

// --- 
CString &CCheckListItem::GetText() {
  return m_Text;
}

      
// --- 
void CCheckListItem::SetSubItemsTexts( const CStringList &setText ) {
	m_SubItemsTexts.RemoveAll(); 
	m_SubItemsTexts.AddTail( const_cast<CStringList *>(&setText) );
}
 
// --- 
CStringList	&CCheckListItem::GetSubItemsTexts() {
	return m_SubItemsTexts;
}

// ---
const TCHAR *CCheckListItem::GetSubItemText( int nIndex ) {
/*
	int j = 0;
	for( POSITION pos = m_SubItemsTexts.GetHeadPosition(); pos != NULL; j++ ) {
		LPCTSTR columnName = m_SubItemsTexts.GetNext( pos );
		if ( j == nIndex ) 
			return columnName;
	}
*/
	POSITION pos = m_SubItemsTexts.FindIndex( nIndex );
	if ( pos )
		return m_SubItemsTexts.GetAt( pos );

	return NULL;
}


// ---
int CCheckListItem::GetSubItemsCount() {
	return m_SubItemsTexts.GetCount();
}


// ---
int CCheckListItem::GetImageIndex() {
	return int( IsNode()
							? IsChecked()
								? GetChecked() == ie_SomeChecked
									? ii_NodeSomeChecked
									:	ii_NodeChecked
								: ii_NodeUnchecked
							:	IsChecked()
								? ii_LeafChecked 
								: ii_LeafUnchecked
						); 
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// ---
CCLItemsArray::CCLItemsArray( bool owns ) : 
	CPSyncArray<CCheckListItem>( 100, 1, owns ) {
}

// ---
CCLItemsArray::~CCLItemsArray() {
	RemoveAll();
}


// ---
void CCLItemsArray::RemoveAt( int nIndex ) { 
	CPSyncArray<CCheckListItem>::RemoveInd( nIndex );
}


// ---
int CCLItemsArray::FindIt( CCheckListItem *item ) {
	for ( int i=0, c=GetSize(); i<c; i++ )
		if ( (*this)[i] == item )
			return i;
	return -1;
}


// ---
void CCLItemsArray::InsertAt( int nIndex, CCLItemsArray *pNewArray ) {
	for ( int i=0,c=pNewArray->GetSize(); i<c; i++ )
		Insert( nIndex++, (*pNewArray)[i] );
}

// ---
void CCLItemsArray::InsertAt( int nIndex, CCheckListItem *pNewItem ) {
	Insert( nIndex, pNewItem );
}


// ---
bool CCLItemsArray::operator == ( const CCLItemsArray &rItems ) {
	if ( GetSize() == rItems.GetSize() ) {
		for ( int i=0, c=GetSize(); i<c; i++ )
			if ( *(*this)[i] != *rItems[i] )
				return false;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// ---
CCheckListColumn::CCheckListColumn() :
	m_Name( _T("") ),
	m_Alignment( ca_Left ),
	fSortAscending( false ),
	fExternalPercent( false ),
	m_WidthPercent( -1 ) {
}


// --- 
bool CCheckListColumn::operator == ( const CCheckListColumn &rItem ) {
	// Не проверять m_WidthPercent - на это заложена логика в использующих классах
	return m_Name				== rItem.m_Name &&
				 m_Alignment	== rItem.m_Alignment;
}


// --- 
void CCheckListColumn::SetName( const CString &rName ) {
  m_Name = rName;
}


// --- 
CString &CCheckListColumn::GetName() {
  return m_Name;
}

      
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// ---
CCLColumnsArray::~CCLColumnsArray() {
	RemoveAll();
}


// ---
void CCLColumnsArray::RemoveAt( int nIndex ) { 
	CPSyncArray<CCheckListColumn>::RemoveInd( nIndex );
}


// ---
void CCLColumnsArray::InsertAt( int nIndex, CCLColumnsArray *pNewArray ) {
	for ( int i=0,c=pNewArray->GetSize(); i<c; i++ )
		Insert( nIndex++, (*pNewArray)[i] );
}

// ---
void CCLColumnsArray::InsertAt( int nIndex, CCheckListColumn *pNewColumn ) {
	Insert( nIndex, pNewColumn );
}


// ---
int CCLColumnsArray::FindIt( CCheckListColumn *item ) {
	for ( int i=0, c=GetSize(); i<c; i++ )
		if ( (*this)[i] == item )
			return i;
	return -1;
}


// ---
bool CCLColumnsArray::operator == ( const CCLColumnsArray &rItems ) {
	if ( GetSize() == rItems.GetSize() ) {
		for ( int i=0, c=GetSize(); i<c; i++ )
			if ( *(*this)[i] != *rItems[i] )
				return false;
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CCheckList

static const UINT nDefferedPosChanging = ::RegisterWindowMessage(_T("AVPControlsDefferedPosChanging"));
static const UINT nDefferedPosChanged = ::RegisterWindowMessage(_T("AVPControlsDefferedPosChanged"));

CCheckList::CCheckList() :
	m_hOldFont( NULL ),
	m_MsgReceiver( NULL ),
	m_hAccelTable( NULL ),
	m_bAccelTableShouldBeDestroyed( false ),
	m_Items( NULL ),
	m_Columns( NULL ),
	m_bInterfaceLocked( false ),
	m_nInterfaceLockCount( 0 ),
	m_nLastSelectedInd( -1 ),
	m_bSizeOscillate( false ), 
	m_bDynamicResizeHeader( false ),
	m_bColumnWidthSetup( false ),
	m_bTrackInProgress( false ),
	m_bDefferedSetColumns( true ),
	m_bFirstChanceSetSize( true ),
	m_bDestroyed( false ),
	fStateImageType( false ),
	fOwnsItemsArray( true ),					// Владеет (сам уничтожает) массив элементов
	fOwnsColumnsArray( true ),				// Владеет (сам уничтожает) массив заголовков
	fHeaderWasChanged( false ),				// Состояние Header изменено
	fSetupImagesAtLoad( true ),				// Устанавливать иконы при загрузке
	fUseCheckSemantic( true ),				// Использовать семантику Check-листа
	fEnableChangeItemsState( true ),	// Разрешено изменение состояния элементов
	fShowSelectionAlways( false ),		// Удерживать селектирование
	fShowHeaderOnEmpty( false ),			// Удерживать Header на пустом листе
	fProportionalResizeHeader( false ), // Пропорциональное изменение ширины колонок Header
	fChangeColumnTextOnly( false ),		// Только замена текста колонок Header
	fUseTitleTip( true ),							// Использовать TitleTip
	fDisplayNoItemsString( false ),    // Использовать строку "There are no items to show in this view"
	fForceAntiFlickering( false )			// Использовать технологию "анти-бликирования"
{
	for ( int i=0; i<E_COUNT; i++ )	{
		CEventSemaphore *pEvent = new CEventSemaphore( true, true );
		m_pEvents.Add( pEvent );
		m_hEvents[i] = (HANDLE)*pEvent;
	}
}

CCheckList::~CCheckList() {
	if ( IsOwnsItemsArray() )
		delete m_Items;

	if ( IsOwnsColumnsArray() )
		delete m_Columns;

	if ( m_bAccelTableShouldBeDestroyed )
		::DestroyAcceleratorTable( m_hAccelTable );
}

// ---
void CCheckList::StartupInitiation() {
	::SendMessage( *this, WM_SETFONT, ::SendMessage(::GetParent(*this), WM_GETFONT, 0, 0), 0 );

	EnableToolTips( FALSE );

	m_TitleTip.Create( this );

	CreateImageList();

	LoadListWindow( true );
}


// ---
void CCheckList::PreSubclassWindow() {
	CListCtrl::PreSubclassWindow();

	if ( ::IsDlgClassName(*GetParent()) ) {
		StartupInitiation();
	}
}


// ---
BOOL CCheckList::PreTranslateMessage( MSG* pMsg ) {
	m_TitleTip.PreTranslateMessage( pMsg );

	if ( m_hAccelTable && (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) )
		if ( ::TranslateAccelerator(*this, m_hAccelTable, pMsg) )
			return TRUE;

	return CListCtrl::PreTranslateMessage( pMsg );
}

// ---
void CCheckList::SetAccelTable( HACCEL	hAccelTable, bool bShouldBeDestroyed ) {
	m_hAccelTable = hAccelTable;
	m_bAccelTableShouldBeDestroyed = bShouldBeDestroyed;
}


// ---
void CCheckList::LockInterface( bool bLock ) {
	if ( bLock ) {
		m_bInterfaceLocked = true;
		m_nInterfaceLockCount++;
	}
	else {
		if ( m_nInterfaceLockCount )
			m_nInterfaceLockCount--;
		if ( !m_nInterfaceLockCount )
			m_bInterfaceLocked = false;
	}
}

// ---
CWnd *CCheckList::GetMessageReceiver() {
	if ( m_MsgReceiver )
		return m_MsgReceiver;

	return GetParent();
}


// ---
COLORREF CCheckList::SetBkColor( COLORREF crColor ) {
	if ( m_ImageList.GetSafeHandle() )
		m_ImageList.SetBkColor( crColor );

	return CListCtrl::SetBkColor( crColor );
}


// ---
void CCheckList::SetShowSelectionAlways( bool bShowAlways ) {
	fShowSelectionAlways = bShowAlways;

	if ( m_hWnd )	{
		if ( ModifyStyle( !fShowSelectionAlways ? LVS_SHOWSELALWAYS : 0, 
											 fShowSelectionAlways ? LVS_SHOWSELALWAYS : 0 ) ) {
			int nSelectedIndex = GetSelectedIndex();
			CRect itemRect;
			GetItemRect( nSelectedIndex, &itemRect, LVIR_BOUNDS/*LVIR_LABEL*/ );
			InvalidateRect( itemRect );
			UpdateWindow();
		}
	}
}


// ---
void CCheckList::SetUseTitleTip( bool set ) {
	fUseTitleTip = set;
	if ( !fUseTitleTip ) 
		m_TitleTip.Hide();
}


// ---
void CCheckList::CreateImageList() {
  if ( IsUseCheckSemantic() ) {
		m_ImageList.Create( IsStateImageType() 
												? IDB_WAS_CHECKLISTSTATEIMAGE 
												: IDB_WAS_CHECKLISTIMAGE,
												20, 1, RGB(255, 255, 255) );
		if ( IsStateImageType() ) 
			SetImageList( &m_ImageList, LVSIL_STATE );
		else {
			SetImageList( &m_ImageList, LVSIL_NORMAL );
			SetImageList( &m_ImageList, LVSIL_SMALL );
		}
  }
}



// ---
void CCheckList::SetItemImage( int iItemIndex, int iItemImageIndex ) {
	LVITEM lvItem;
	memset( &lvItem, 0, sizeof(lvItem) );

	lvItem.mask = LVIF_IMAGE;
	lvItem.iItem = iItemIndex;
	lvItem.iImage = iItemImageIndex;

	SetItem( &lvItem);
}

// ---
void CCheckList::SetItemImages( CCheckListItem *item, int iItemIndex ) {

  int imageIndex = item->GetImageIndex();
	if ( item->GetCurrImageIndex() != imageIndex ) {

		if ( IsStateImageType() )
			// "0"-image must be blank
			SetItemState( iItemIndex, INDEXTOSTATEIMAGEMASK(imageIndex + 2), LVIS_STATEIMAGEMASK );
		else 
			SetItemImage( iItemIndex, imageIndex );

		item->SetCurrImageIndex( imageIndex );
	}
}


// Установить новый массив дескрипторов
// ---
void CCheckList::SetItemsArray( CCLItemsArray *itemsArray, CCLColumnsArray *headerNames ) {
	bool bNeedReloadHeader = false;

	if ( headerNames ) {
		
		bNeedReloadHeader = !(m_Columns && *m_Columns == *headerNames);
		
		if ( IsOwnsColumnsArray() && m_Columns != headerNames )
			delete m_Columns;

		m_Columns = headerNames;
	}

	if ( itemsArray ) {
		bool bNeedReload = !m_Items || *m_Items != *itemsArray || !itemsArray->GetSize();

		if ( IsOwnsItemsArray() && m_Items )
			delete m_Items;
/*
		m_Items = NULL;

		if ( bNeedReload ) {
			bool bDisplayNoItemsString = IsDisplayNoItemsString();
			SetDisplayNoItemsString( false );
			ShowColumnHeader( false );
			SetDisplayNoItemsString( bDisplayNoItemsString );
		}
*/
		m_Items = itemsArray;

		if ( bNeedReload )
			LoadListWindow( bNeedReloadHeader );
	}
}

// ---
void CCheckList::Synchronize() {
	while ( !m_bDestroyed && ::WaitForMultipleObjects(E_COUNT, m_hEvents, TRUE, 0) == WAIT_TIMEOUT ) {
		MSG rcMsg;
		if ( ::PeekMessage(&rcMsg, NULL, 0, 0, PM_REMOVE) )	{
			::TranslateMessage( &rcMsg );
			::DispatchMessage( &rcMsg );
		}
	}
	//while ( m_bTrackInProgress || m_bDynamicResizeHeader || m_bColumnWidthSetup || m_bSizeOscillate );
}


// ---
void CCheckList::Lock( int nIndex ) {
	m_pEvents[nIndex]->Reset();
}

// ---
void CCheckList::Unlock( int nIndex ) {
	m_pEvents[nIndex]->Set();
}

// ---
void CCheckList::SetColumnsArray( CCLColumnsArray *headerNames ) {

	//Synchronize();

	bool bNeedReloadHeader = !(m_Columns && *m_Columns == *headerNames);
	
	if ( IsOwnsColumnsArray() )
		delete m_Columns;

	m_Columns = headerNames;

	LoadHeader( bNeedReloadHeader );

	if ( bNeedReloadHeader || IsChangeColumnTextOnly() )
		SetColumnsWidth();

	ShowColumnHeader( true );
}


// ---
void CCheckList::SetItemText( int nIndex, CString &rcItemText ) {
	//Synchronize();

	if ( m_Items && nIndex < m_Items->GetSize() ) {
		(*m_Items)[nIndex]->SetText( rcItemText );
		CListCtrl::SetItemText( nIndex, 0, rcItemText );
	}
}

// ---
void CCheckList::SetItemSubTexts( int iItemIndex, CStringList &rcStrList ) {

	//Synchronize();

	int j = 1;
	for( POSITION pos = rcStrList.GetHeadPosition(); pos != NULL; j++ ) {
		LPCTSTR columnName = rcStrList.GetNext( pos );
		CListCtrl::SetItemText( iItemIndex, j, columnName );
	}
}

// ---
void CCheckList::AddItem( int iItemIndex ) {
	if ( iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) {
		CCheckListItem *clItem = (*m_Items)[iItemIndex];

		// Первое добавление - обязательно должен перерисоваться
		clItem->SetCurrImageIndex( -1 );

		InsertItem( iItemIndex, clItem->GetText() );

		CStringList &itemColumns = clItem->GetSubItemsTexts();
		SetItemSubTexts( iItemIndex, itemColumns );

		SetItemData( iItemIndex, (DWORD)clItem );

		if ( IsSetupImagesAtLoad() )
			SetItemImages( clItem, iItemIndex );
	}
}


// ---
void CCheckList::AddNewItem( CCheckListItem *pItem, bool bEnsureVisible, bool bAddToList ) {
	if ( !m_Items ) 
		m_Items = new CCLItemsArray( true );

	m_Items->Add( pItem );

	if ( bAddToList ) {
		int nIndex = m_Items->GetSize() - 1;

		if ( !(nIndex % 200) )
			SetItemCount( nIndex + 200 );

		AddItem( nIndex );

		if ( bEnsureVisible || !nIndex )	{
			SetItemState( nIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
			EnsureVisible( nIndex, FALSE );
		}
	}
}

// ---
void CCheckList::RemoveItems( int iItemFromIndex, int nRemoveCount ) {
	SetRedraw( FALSE );

	if ( m_Items && m_Items->GetSize() ) {
		for ( ; nRemoveCount && m_Items->GetSize(); nRemoveCount-- ) {
			m_Items->RemoveAt( iItemFromIndex );
			DeleteItem( iItemFromIndex );
		}
	}

	SetRedraw( TRUE );
}


// ---
void CCheckList::AddItemsToList( bool bEnsureVisible ) {
}

// Убрать/восстановить Header
void CCheckList::ShowColumnHeader( bool bShow ) {
  ModifyStyle( LVS_NOCOLUMNHEADER, !bShow ? LVS_NOCOLUMNHEADER : 0 );
	Invalidate();
	UpdateWindow();
}


// ---
inline long LRound( double fValue ) {
	return long(fValue + (fValue > 0 ? 0.5 : -0.5));
}

// ---
void CCheckList::LoadHeader( bool bNeedReloadHeader ) {
	if ( bNeedReloadHeader || IsChangeColumnTextOnly() ) {

		if ( m_Columns && m_Columns->GetSize() ) {
			ShowColumnHeader( true );
			
			SetRedraw(FALSE);

			// Это работает даже в режиме не-Report
			CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
		//		CHeaderCtrl* pHeader = GetHeaderCtrl();

			int iExtraSpace = GetStringWidth(_T("X")) << 1;
			CRect rcClientRect;
			GetClientRect( rcClientRect );

			if ( !IsChangeColumnTextOnly() ) {
				int nColumnCount = pHeader->GetItemCount();
				for ( int n=0; n<nColumnCount; n++ )
					DeleteColumn( 0 );

				for( int i=0,c=m_Columns->GetSize(); i<c; i++ ) {
					CCheckListColumn *pColumn = (*m_Columns)[i];
					LPCTSTR headerName = pColumn->GetName();
					int headerNameWidth;
					int iColumnPercent = pColumn->GetWidthPercent();
					if ( iColumnPercent >= 0 && pColumn->IsExternalPercent() ) {
						iColumnPercent = iColumnPercent > 100 ? 100 : iColumnPercent;
						headerNameWidth = LRound( rcClientRect.Width() * iColumnPercent / 100. );
					}
					else 
						headerNameWidth = GetStringWidth( headerName ) + iExtraSpace;
					InsertColumn( i, headerName, pColumn->GetAlignment(), headerNameWidth, i );
				}
			}
			else {
				for( int i=0,c=m_Columns->GetSize(); i<c; i++ ) {
					CCheckListColumn *pColumn = (*m_Columns)[i];
					LPCTSTR headerName = pColumn->GetName();
					int headerNameWidth;
					int iColumnPercent = pColumn->GetWidthPercent();
					if ( iColumnPercent >= 0 && pColumn->IsExternalPercent() ) {
						iColumnPercent = iColumnPercent > 100 ? 100 : iColumnPercent;
						headerNameWidth = LRound( rcClientRect.Width() * iColumnPercent / 100. );
					}
					else 
						headerNameWidth = GetStringWidth( headerName ) + iExtraSpace;

					int justColumnWidth = GetColumnWidth( i );
					if ( headerNameWidth < justColumnWidth )
						headerNameWidth = justColumnWidth;

					SetItemWidth( i, headerNameWidth );
					SetHeaderItem( i, headerName, pColumn->GetAlignment() );
				}
			}

			SetHeaderWasChanged( false );

			SetRedraw(TRUE);
		}
		else
			ShowColumnHeader( false );
	}
}

// Загрузить окно
// ---
void CCheckList::LoadListWindow( bool bNeedReloadHeader ) {
	SetRedraw( FALSE );

//	int nSelectedIndex = GetSelectedIndex();

	LoadHeader( bNeedReloadHeader );

	DeleteAllItems();

	if ( m_Items && m_Items->GetSize() ) {
		ShowColumnHeader( true );
		
		for ( int i=0,c=m_Items->GetSize(); i<c; i++ ) 
			AddItem( i );
	}
	else
		ShowColumnHeader( IsShowHeaderOnEmpty() );

	SetColumnsWidth();
/*
	if ( nSelectedIndex >= 0 )
		SetItemSelected( nSelectedIndex );
*/
	SetRedraw( TRUE );
}


// ---
void CCheckList::SetItemSelected( int ind ) {
	int nSelectedIndex = GetSelectedIndex();

	if ( nSelectedIndex >= 0 )
		SetItemState( nSelectedIndex, 0, LVIS_SELECTED | LVIS_FOCUSED );

	SetItemState( ind, LVIS_FOCUSED | LVIS_SELECTED, LVIS_SELECTED | LVIS_FOCUSED );
}


// Получить индекс селектированного элемента
// ---
int CCheckList::GetSelectedIndex() {
	POSITION pos = GetFirstSelectedItemPosition();
	if ( pos != NULL )
		return GetNextSelectedItem( pos );
	return -1;
}

// ---
void CCheckList::SetItemWidth( int iItemIndex, int iColumnWidth ) {
	// Так можно устанавливать даже в режиме не-Report
  CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);

	SetHeaderItemWidth( pHeader, iItemIndex, iColumnWidth );
}

// ---
void CCheckList::SetHeaderItemWidth( CHeaderCtrl* pHeader, int iItemIndex, int iColumnWidth ) {
	HDITEM stHDItem;
	memset( &stHDItem, 0, sizeof(stHDItem) );
	stHDItem.mask = HDI_WIDTH;
	stHDItem.cxy = iColumnWidth;

	pHeader->SetItem( iItemIndex, &stHDItem );
	SetColumnWidth( iItemIndex, iColumnWidth );
}

// ---
void CCheckList::SetHeaderItem( int iItemIndex, LPCTSTR pColumnText, int nFormat ) {
	// Так можно устанавливать даже в режиме не-Report
  CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);

	HDITEM stHDItem;
	memset( &stHDItem, 0, sizeof(stHDItem) );
	stHDItem.mask = HDI_TEXT | HDI_FORMAT;
	stHDItem.pszText = (LPTSTR)pColumnText;
	stHDItem.cchTextMax = lstrlen( pColumnText );
	stHDItem.fmt = nFormat;

	pHeader->SetItem( iItemIndex, &stHDItem );

	LVCOLUMN stLVColumn;
	memset( &stLVColumn, 0, sizeof(stLVColumn) );
	stLVColumn.mask = LVCF_FMT | LVCF_TEXT;
	stLVColumn.pszText = (LPTSTR)pColumnText;
	stLVColumn.fmt = nFormat;
	stLVColumn.iSubItem = iItemIndex;

	SetColumn( iItemIndex, &stLVColumn );
}


// ---
void CCheckList::SetColumnsWidth() {
	CRect rcClientRect;
	GetClientRect( rcClientRect );
	
	SetColumnsByWidth( rcClientRect.Width() );
}


// ---
void CCheckList::SetColumnsByWidth( int iClientWidth ) {
	// Это работает даже не в режиме Report
  CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);

	if ( pHeader && pHeader->GetItemCount() && !IsHeaderWasChanged() && (IsShowHeaderOnEmpty() || (m_Items && m_Items->GetSize())) ) {
		int nImageExtraSpace = 0;

		m_bColumnWidthSetup = true;
		Lock( E_COLUMNWIDTHSETUP );

		CImageList *pImgNormal = GetImageList( LVSIL_SMALL );		
		if ( pImgNormal && pImgNormal->GetImageCount() ) {
			IMAGEINFO imageInfoNormal;
			pImgNormal->GetImageInfo( 0, &imageInfoNormal );		
			nImageExtraSpace += (imageInfoNormal.rcImage.right + 2);					
		}

		CImageList *pImgState = GetImageList( LVSIL_STATE );		
		if ( pImgState && pImgState->GetImageCount() ) {
			IMAGEINFO imageInfoState;
			pImgState->GetImageInfo( 0, &imageInfoState );		
			nImageExtraSpace += (imageInfoState.rcImage.right + 2);					
		}

		bool bDirectPercent = false;

		int nPadding = GetStringWidth( _T("X") ) << 1;

		CSArray<int>	pColumnPercents;
		CSArray<int>	pColumnWidths;

		int j, n;
		for ( j=0,n=pHeader->GetItemCount(); j<n; j++ ) {
			CCheckListColumn *pColumn = (*m_Columns)[j];

			int iColumnWidth = GetColumnWidth( j );
			int iColumnPercent = pColumn->GetWidthPercent();

			if ( iColumnPercent >= 0 && pColumn->IsExternalPercent() ) {
				iColumnPercent = iColumnPercent > 100 ? 100 : iColumnPercent;
				iColumnWidth = LRound( iClientWidth * iColumnPercent / 100. );

				if ( iColumnWidth < 0 )
					iColumnWidth = 0;

				bDirectPercent = true;
			}
			else {
				if ( m_Items ) {
					for ( int i=0,c=min(m_Items->GetSize(), 100); i<c; i++ ) {
						const TCHAR *pText = j ? (*m_Items)[i]->GetSubItemText( j - 1 )	: (*m_Items)[i]->GetText();
						int iItemWidth = GetStringWidth( pText ) + nPadding + (!j ? nImageExtraSpace : 0);
//						int iItemWidth = GetStringWidth( GetItemText(i, j) ) + nPadding + (!j ? nImageExtraSpace : 0);
						if ( iColumnWidth < iItemWidth )
							iColumnWidth = iItemWidth;
					}
					iColumnPercent = iColumnWidth * 100 / iClientWidth;
				}
			}
			
			pColumnPercents.Add( iColumnPercent );
			pColumnWidths.Add( iColumnWidth );
		}

		if ( bDirectPercent && IsProportionalResizeHeader() ) {
			int iWidthSum = 0;
			for ( int i=0,n=pColumnWidths.Count(); i<n; i++ ) 
				iWidthSum += pColumnWidths[i];

			if ( iWidthSum ) {
				int iCorrectIndex = 0;
				int nExtraWidth = iWidthSum - iClientWidth;
				pColumnWidths[iCorrectIndex] -= nExtraWidth;
				if ( !HDC_WIDTHISTRUE(pColumnWidths[iCorrectIndex]) )
					pColumnWidths[iCorrectIndex] = HDC_MINWIDTH;

				pColumnPercents[iCorrectIndex] = pColumnWidths[iCorrectIndex] * 100 / iClientWidth;
			}
		}

		for ( j=0,n=pColumnWidths.Count(); j<n; j++ ) {
			(*m_Columns)[j]->SetWidthPercent( pColumnPercents[j] );
			SetHeaderItemWidth( pHeader, j, pColumnWidths[j] );
			/*
			CString rcString;
			rcString.Format( "%d = %d = %d", j, pColumnPercents[j], pColumnWidths[j] );
			AfxMessageBox( rcString );
			*/
		}

		// Первая же загрузка - и последняя
		SetHeaderWasChanged( !bDirectPercent && !!m_Items && !!m_Items->GetSize() );

		m_bColumnWidthSetup = false;
		Unlock( E_COLUMNWIDTHSETUP );
	}
}

// ---
void CCheckList::CorrectColumnsWidth() {
	if ( IsProportionalResizeHeader() ) {
		m_bColumnWidthSetup = true;
		Lock( E_COLUMNWIDTHSETUP );

		CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
		bool bDirectPercent = false;
		for ( int j=0,n=pHeader->GetItemCount(); j<n && !bDirectPercent; j++ ) {
			CCheckListColumn *pColumn = (*m_Columns)[j];
			int iColumnPercent = pColumn->GetWidthPercent();

			bDirectPercent = ( iColumnPercent >= 0 && pColumn->IsExternalPercent() );
		}

		if ( !bDirectPercent ) {
			CRect rcClientRect;
			GetClientRect( rcClientRect );
			int iClientWidth = rcClientRect.Width();

			CSArray<int>	pColumnPercents;
			CSArray<int>	pColumnWidths;
			int nPercentSum = 0;

			int j, n;
			for ( j=0,n=pHeader->GetItemCount(); j<n; j++ ) {
				CCheckListColumn *pColumn = (*m_Columns)[j];

				int iColumnWidth = GetColumnWidth( j );
				int iColumnPercent = iColumnWidth * 100 / iClientWidth;

				nPercentSum += iColumnPercent;

				pColumnWidths.Add( iColumnWidth );
				pColumnPercents.Add( iColumnPercent );
			}

			nPercentSum = LRound((100. - nPercentSum) / pColumnWidths.Count());

			for ( j=0,n=pColumnWidths.Count(); j<n; j++ ) {
				pColumnPercents[j] += nPercentSum;
				pColumnWidths[j] = LRound( iClientWidth * pColumnPercents[j] / 100. );
			}

			int iWidthSum = 0;
			for ( j=0,n=pColumnWidths.Count(); j<n; j++ ) 
				iWidthSum += pColumnWidths[j];

			if ( iWidthSum ) {
				int iCorrectIndex = 0;
				int nExtraWidth = iWidthSum - iClientWidth;
				pColumnWidths[iCorrectIndex] -= nExtraWidth;
				if ( !HDC_WIDTHISTRUE(pColumnWidths[iCorrectIndex]) )
					pColumnWidths[iCorrectIndex] = HDC_MINWIDTH;

				pColumnPercents[iCorrectIndex] = LRound( pColumnWidths[iCorrectIndex] * 100. / iClientWidth );
			}

			for ( j=0,n=pColumnWidths.Count(); j<n; j++ ) {
				CCheckListColumn *pColumn = (*m_Columns)[j];
				pColumn->SetWidthPercent( pColumnPercents[j] );
				pColumn->SetExternalPercent();
				SetHeaderItemWidth( pHeader, j, pColumnWidths[j] );
			}

			SetHeaderWasChanged( true );
		}

		m_bColumnWidthSetup = false;
		Unlock( E_COLUMNWIDTHSETUP );
	}
}


// Установить новый массив дескрипторов, начиная с заданного индекса
// ---
void CCheckList::UpdateItemsArray( int fromIndex, CCLItemsArray &itemsArray ) {
}


// ---
void CCheckList::SetItemChecked( int iItemIndex, CCheckListItem::CieChecked fHowToCheck, bool bSendMessage ) {
	if ( m_Items && m_Items->GetSize() && iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) {
		(*m_Items)[iItemIndex]->SetCheck( fHowToCheck );

		SetItemImages( (*m_Items)[iItemIndex], iItemIndex );
	}
}


// ---
void CCheckList::CheckItem( int iItemIndex ) {
	if ( m_Items && m_Items->GetSize() && iItemIndex >=0 && iItemIndex <= m_Items->MaxIndex() ) 
	  SetItemChecked( iItemIndex, CCheckListItem::CieChecked(!(*m_Items)[iItemIndex]->IsChecked()) );
}


// Установить тип отображения
// ---
bool CCheckList::SetViewType( DWORD dwViewType ) {
	if ( GetViewType() != dwViewType ) {
		int nSelectedIndex = GetSelectedIndex();

		bool ok = !!ModifyStyle( LVS_TYPEMASK, dwViewType & LVS_TYPEMASK );

		if ( GetViewType() == LVS_REPORT && m_Items && m_Items->GetSize() ) 
			ShowColumnHeader( true );

		if ( nSelectedIndex >= 0 )
			SetItemSelected( nSelectedIndex );

		return ok;
	}

	return false;
}

// Получить тип отображения
// ---
uint32 CCheckList::GetViewType() {
	return GetStyle() & LVS_TYPEMASK;
}


// ---
void CCheckList::SetProportionalResizeHeader( bool set )	{ 
	fProportionalResizeHeader = set;

	CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
	if ( pHeader && pHeader->GetSafeHwnd() ) {
		
		if ( IsProportionalResizeHeader() ) {
			pHeader->ModifyStyle( HDS_FULLDRAG, 0 );
		}
		
	}
}

// ---
CString CCheckList::GetTrueItemText( int nRow, int nCol ) {
	 // Get the header control 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
   _ASSERTE(pHeader);    

	 // get the current number of columns 
  int nCount = pHeader->GetItemCount();
  
	// find the actual column requested. We will compare    
	// against hi.iOrder
  for ( int x=0; x<nCount; x++ ) {        
		HD_ITEM hi = {0};
    hi.mask = HDI_ORDER;        
		BOOL bRet = pHeader->GetItem( x, &hi );
     _ASSERTE(bRet);        
		 if ( hi.iOrder == nCol ) {
       // Found it, get the associated text
       return GetItemText( nRow, x );        
		 }    
	}    

	_ASSERTE(FALSE);
  return _T("We better never fall through to here!");
}


// ---
int CCheckList::GetTrueColumnWidth( int nCurrentPosition ) const {
	 // Get the header control 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
  _ASSERTE(pHeader);    

	 // get the current number of columns 
	int nCount = pHeader->GetItemCount();

  for ( int x=0; x<nCount; x++ ) {        
		HD_ITEM hi = {0};
    hi.mask = HDI_WIDTH | HDI_ORDER;
    BOOL bRet = pHeader->GetItem( x, &hi );        
		_ASSERTE(bRet);
    if ( hi.iOrder == nCurrentPosition )            
			return hi.cxy;    
	}
  _ASSERTE(FALSE);    
	return 0; // We would never fall through to here!
}


// ---
int CCheckList::GetRowColumnIndex( CPoint &rcPoint, RECT *pCellRect, int *pnColumn ) const { 
	int nColNum = 0; 
	int nRow = HitTest( rcPoint, NULL ); 
	if ( pnColumn ) 
		*pnColumn = 0; 

	// Make sure that the ListView is in LVS_REPORT 
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT ) 
	 return nRow; 

	//in case the hit test returns -1, loop through all the 
	// rows to get the correct nColumn 
	nRow = GetTopIndex(); 

	int nBottom = nRow + GetCountPerPage(); 
	if ( nBottom > GetItemCount() ) 
		nBottom = GetItemCount(); 

	// Get the number of columns 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem( 0 ); 
	int nColumnCount = pHeader->GetItemCount(); 

	//Loop through the visible rows 
	for ( ;nRow <= nBottom; nRow++ ) { 
		// Get bounding rcRect of item and check whether rcPoint falls in it. 
		CRect rcRect; 
		GetItemRect( nRow, &rcRect, LVIR_BOUNDS ); 
		if( rcRect.PtInRect(rcPoint) ) { 
			// Now find the nColumn 
			for( int nColNum = 0; nColNum < nColumnCount; nColNum++ ) { 
				int nColWidth = GetTrueColumnWidth( nColNum ); 
				if( rcPoint.x >= rcRect.left && rcPoint.x <= (rcRect.left + nColWidth ) ) { 
					RECT rectClient;					
					GetClientRect( &rectClient );
					if ( rcPoint.x > rectClient.right )						
						return -1;					

					if( pnColumn ) 
						*pnColumn = nColNum; 

					rcRect.right = rcRect.left + nColWidth;
					if( rcRect.right > rectClient.right ) 						
						rcRect.right = rectClient.right;

					if ( pCellRect )
						*pCellRect = rcRect;					
					return nRow; 
				} 
				rcRect.left += nColWidth; 
			} 
		} 
	} 
	return -1; 
} 

// ---
void CCheckList::SetNodeStyle( CCheckListItem *item, DWORD dwStyle ) {
	if ( dwStyle ) {
		HFONT hFont = m_FontCache.Find( dwStyle );
		if ( !hFont ) {
			CFont *pFont = GetFont();
			if ( pFont ) {
				LOGFONT rcLogfont;
				pFont->GetLogFont( &rcLogfont );
				rcLogfont.lfWeight = (dwStyle & CLIS_BOLD) ? FW_BOLD : FW_NORMAL;
				rcLogfont.lfItalic = (dwStyle & CLIS_ITALIC) ? TRUE : FALSE;
				rcLogfont.lfUnderline = (dwStyle & CLIS_UNDERLINE) ? TRUE : FALSE;
				rcLogfont.lfStrikeOut = (dwStyle & CLIS_STRIKEOUT) ? TRUE : FALSE;

				hFont = ::CreateFontIndirect( &rcLogfont );
				m_FontCache.Add( CCLFontCacheItem(hFont, dwStyle) );
			}
		}
		item->SetFont( hFont );
	}
	else 
		item->SetFont( NULL );
}


BEGIN_MESSAGE_MAP(CCheckList, CListCtrl)
	//{{AFX_MSG_MAP(CCheckList)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE( nDefferedPosChanging, OnDefferedPosChanging )
	ON_REGISTERED_MESSAGE( nDefferedPosChanged, OnDefferedPosChanged )
//	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
//	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckList message handlers
// ---
int CCheckList::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if ( !::IsDlgClassName(*GetParent()) ) {
		StartupInitiation();
	}

	CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
	if ( pHeader ) {
	  pHeader->ModifyStyle( WS_TABSTOP, 0 );
		
		if ( IsProportionalResizeHeader() ) {
			pHeader->ModifyStyle( HDS_FULLDRAG, 0 );
		}
		
	}

	return 0;
}

// ---
void CCheckList::OnDestroy() {
	if ( m_hOldFont )
		::SelectObject( *GetDC(), m_hOldFont );

	m_FontCache.Flush();

	m_TitleTip.DestroyWindow();

	m_bDestroyed = true;

	CListCtrl::OnDestroy();
}

// ---
void CCheckList::OnItemChanging( NMHDR* pNMHDR, LRESULT* pResult ) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if ( (pNMListView->uChanged & LVIF_STATE) && 
			 (pNMListView->uOldState & LVIS_FOCUSED) &&
			 !pNMListView->uNewState ) {
		if ( !m_bInterfaceLocked ) 
			*pResult = 0;
		else
			*pResult = TRUE;
	}
	else
	  *pResult = 0;
}

// ---
void CCheckList::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if ( (pNMListView->uChanged & LVIF_STATE) && 
			 (pNMListView->uNewState & LVIS_FOCUSED) &&
			 !pNMListView->uOldState ) {
		if ( !m_bInterfaceLocked ) 
			*pResult = 0;
		else
			*pResult = TRUE;
	}
	else
	  *pResult = 0;
}


// --- 
void CCheckList::OnLButtonDown( UINT nFlags, CPoint point ) {
	CListCtrl::OnLButtonDown(nFlags, point);

	UINT flags;
  int iItemIndex = HitTest( point, &flags );
	UINT checkFlag = IsStateImageType() ? LVHT_ONITEMSTATEICON : LVHT_ONITEMICON;

	::SetFocus( *this );
	if ( iItemIndex < 0 )
		iItemIndex = m_nLastSelectedInd;

	SetItemSelected( iItemIndex );
	m_nLastSelectedInd = iItemIndex;

  if ( (flags & checkFlag) ) {
		if ( IsEnableChangeItemsState() )
			CheckItem( iItemIndex );
  }  

	//CListCtrl::OnLButtonDown(nFlags, point);
}


// ---
void CCheckList::NormalizeColumnWidth( int iClientWidth, int iItemIndex, int &iColumnWidth, int &iColumnPercent ) {
	int nRestSize = 0;
	int nRestPercent = 0;
	for ( int j=0; j<iItemIndex; j++ ) {
		nRestSize += GetColumnWidth( j );
		nRestPercent += (*m_Columns)[j]->GetWidthPercent();
	}

	if ( (nRestSize + iColumnWidth) > iClientWidth ) {
		iColumnWidth = iClientWidth - nRestSize;
  	iColumnPercent = iColumnWidth * 100 / iClientWidth;
	}

	if ( (nRestPercent + iColumnPercent) > 100 ) {
		iColumnPercent = 100 - nRestPercent;
		iColumnWidth = iClientWidth * iColumnPercent / 100;
	}

	(*m_Columns)[iItemIndex]->SetWidthPercent( iColumnPercent );
	SetItemWidth( iItemIndex, iColumnWidth );
}


// ---
bool CCheckList::ProportionalResizeHeader( NMHEADER *pNMHeader ) {
	if ( !m_bDynamicResizeHeader ) {
		if ( IsProportionalResizeHeader() && !m_bColumnWidthSetup && !m_bSizeOscillate ) {

			CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
			if ( pHeader && m_Columns && pHeader->GetItemCount() ) {
				SetRedraw( FALSE );

				CSArray<int>	pColumnPercents;
				CSArray<int>	pColumnWidths;
				CSArray<int>	pColumnInds;

				int iHeaderCount = pHeader->GetItemCount();

				CRect rcClientRect;
				GetClientRect( rcClientRect );

				int iItemIndex = pNMHeader->iItem;
				int iClientWidth = rcClientRect.Width();

				int iOldColumnPercent = (*m_Columns)[iItemIndex]->GetWidthPercent();
				int iOldColumnWidth   = iClientWidth * iOldColumnPercent / 100;
/*
				HDITEM stHDItem;
				memset( &stHDItem, 0, sizeof(stHDItem) );
				stHDItem.mask = HDI_WIDTH;

				pHeader->GetItem( iItemIndex, &stHDItem );
*/
				int iColumnWidth = pNMHeader->pitem->cxy;//GetColumnWidth( iItemIndex );//stHDItem.cxy; //
				int iColumnPercent = iColumnWidth * 100 / iClientWidth;
/*
				if ( (pHeader->GetStyle() & HDS_FULLDRAG) && labs(iColumnWidth - iOldColumnWidth) < (iHeaderCount - iItemIndex) )
					return FALSE;
*/
				m_bDynamicResizeHeader = true;
				Lock( E_DINAMICRESIZEHEADER );

				NormalizeColumnWidth( iClientWidth, iItemIndex, iColumnWidth, iColumnPercent );

				//TRACE2("TRACK iOldColumnPercent = %d, iColumnPercent = %d\r\n", iOldColumnPercent, iColumnPercent );

				int iWidthSum = 0;
				if ( pNMHeader->iItem < (iHeaderCount - 1) ) {

					int nRestSize = 0;
					for ( int j=0; j<iItemIndex; j++ ) 
						nRestSize += GetColumnWidth( j );

					iItemIndex += 1;

					iWidthSum = nRestSize + iColumnWidth;

					int iWidthAdd = (iOldColumnWidth - iColumnWidth) / (iHeaderCount - iItemIndex);
					//TRACE1("TRACK iWidthAdd = %d\r\n", iWidthAdd );

					for ( int i=iItemIndex,n=iHeaderCount; i<n; i++ ) {
						HDITEM stHDItem;
						memset( &stHDItem, 0, sizeof(stHDItem) );
						stHDItem.mask = HDI_WIDTH;

						pHeader->GetItem( i, &stHDItem );

						//TRACE2("TRACK i = %d, stHDItem.cxy = %d\r\n", i, stHDItem.cxy );

						int iNewColumnWidth = stHDItem.cxy + iWidthAdd;

						if ( !HDC_WIDTHISTRUE(iNewColumnWidth) )
							iNewColumnWidth = HDC_MINWIDTH;

						int iNewColumnPercent = iNewColumnWidth * 100 / iClientWidth;

						//TRACE3("TRACK i = %d, iNewColumnPercent = %d, iNewColumnWidth = %d\r\n", i, iNewColumnPercent, iNewColumnWidth );

						pColumnPercents.Add( iNewColumnPercent );
						pColumnWidths.Add( iNewColumnWidth );
						pColumnInds.Add( i );
					}
				}
				else {
					int iWidthAdd = (iOldColumnWidth - iColumnWidth) / iItemIndex;
					//TRACE1("TRACK iWidthAdd = %d\r\n", iWidthAdd );

					iWidthSum = iColumnWidth;

					for ( int i=0; i<iItemIndex; i++ ) {
						HDITEM stHDItem;
						memset( &stHDItem, 0, sizeof(stHDItem) );
						stHDItem.mask = HDI_WIDTH;

						pHeader->GetItem( i, &stHDItem );

						//TRACE2("TRACK i = %d, stHDItem.cxy = %d\r\n", i, stHDItem.cxy );

						int iNewColumnWidth = stHDItem.cxy + iWidthAdd;

						if ( !HDC_WIDTHISTRUE(iNewColumnWidth) )
							iNewColumnWidth = HDC_MINWIDTH;

						int iNewColumnPercent = iNewColumnWidth * 100 / iClientWidth;

						//TRACE3("TRACK i = %d, iNewColumnPercent = %d, iNewColumnWidth = %d\r\n", i, iNewColumnPercent, iNewColumnWidth );

						pColumnPercents.Add( iNewColumnPercent );
						pColumnWidths.Add( iNewColumnWidth );
						pColumnInds.Add( i );
					}
				}

					
				int i, n;
				for ( i=0,n=pColumnPercents.Count(); i<n; i++ ) 
					 iWidthSum += pColumnWidths[i];

				if ( pColumnPercents.Count() ) {
					int iCorrectIndex = 0;
					int nExtraWidth = iWidthSum - iClientWidth;
					pColumnWidths[iCorrectIndex] -= nExtraWidth;
					if ( !HDC_WIDTHISTRUE(pColumnWidths[iCorrectIndex]) )
						pColumnWidths[iCorrectIndex] = HDC_MINWIDTH;

					pColumnPercents[iCorrectIndex] = pColumnWidths[iCorrectIndex] * 100 / iClientWidth;
				}

				for ( i=0,n=pColumnPercents.Count(); i<n; i++ ) {
					int iNewColumnPercent = pColumnPercents[i];
					int iNewColumnWidth = pColumnWidths[i];
					int iColumnInd = pColumnInds[i];

					(*m_Columns)[iColumnInd]->SetWidthPercent( iNewColumnPercent );

					SetItemWidth( iColumnInd, iNewColumnWidth );
				}

				m_bDynamicResizeHeader = false;
				Unlock( E_DINAMICRESIZEHEADER );

				SetRedraw( TRUE );
			}

		}
		return true;
	}

	return false;
}




// ---
int CCheckList::CorrectItemSize( int nItemIndex, int nItemSize ) {
	CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
	if ( pHeader && pHeader->GetItemCount() ) {
		CRect rcClientRect;
		GetClientRect( rcClientRect );
		int nRestSize = 0;
		if ( HDC_WIDTHISTRUE(nItemSize) ) {
			for ( int i=0; i<nItemIndex; i++ ) 
				nRestSize += GetColumnWidth( i );
			for ( int j=nItemIndex+1,n=pHeader->GetItemCount(); j<n; j++ ) 
				nRestSize += HDC_MINWIDTH;
			if ( nItemSize > (rcClientRect.Width() - nRestSize) ) 
				nItemSize = rcClientRect.Width() - nRestSize;
		}
		else
			nItemSize = HDC_MINWIDTH;
	}
	return nItemSize;
}

// ---
BOOL CCheckList::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult ) {
//	BOOL bResult = CListCtrl::OnNotify(wParam, lParam, pResult);

	// Нельзя реагировать на это сообщение через ON_NOTIFY_REFLECT, т.к. при его обработке
  // ищется LookupPermanent от Child, а Header не создан как экз. класса 
	NMHDR *pNMHdr = (NMHDR *)lParam;
  if ( (HDN_FIRST > HDN_LAST && pNMHdr->code <= HDN_FIRST && pNMHdr->code >= HDN_LAST) ||
       (HDN_FIRST < HDN_LAST && pNMHdr->code >= HDN_FIRST && pNMHdr->code <= HDN_LAST) ) {
		BOOL bLocked = FALSE;
		BOOL bBreak = FALSE;

    if ( !bBreak && (pNMHdr->code == HDN_BEGINTRACKA || pNMHdr->code == HDN_BEGINTRACKW) ) {
			SetHeaderWasChanged( !IsProportionalResizeHeader() );
			if ( IsProportionalResizeHeader() ) {
				m_bTrackInProgress = true;
				Lock( E_TRACKINPROGRESS );
				bLocked = TRUE;
			}
			
		}
/* FULLDRAG
    if ( pNMHdr->code == HDN_TRACKA || pNMHdr->code == HDN_TRACKW ||
				 pNMHdr->code == HDN_ITEMCHANGINGA || pNMHdr->code == HDN_ITEMCHANGINGW ) {
			NMHEADER *pNMHeader = (NMHEADER *)lParam;
			if ( IsProportionalResizeHeader() && (pNMHeader->pitem->mask & HDI_WIDTH) ) {
				if ( ProportionalResizeHeader( pNMHeader ) )
					return TRUE;
				else {
					*pResult = TRUE;
					return TRUE;
				}
			}
		}
*/
/*	MINWIDTH != 0
    if ( pNMHdr->code == HDN_TRACKA || pNMHdr->code == HDN_TRACKW ||
				 pNMHdr->code == HDN_ITEMCHANGINGA || pNMHdr->code == HDN_ITEMCHANGINGW ) {
			NMHEADER *pNMHeader = (NMHEADER *)lParam;
			if ( !m_bDynamicResizeHeader && !m_bColumnWidthSetup && 
					 IsProportionalResizeHeader() && (pNMHeader->pitem->mask & HDI_WIDTH) ) {

				pNMHeader->pitem->cxy = CorrectItemSize( pNMHeader->iItem, pNMHeader->pitem->cxy );
			}
		}
*/

    if ( !bBreak && (pNMHdr->code == HDN_ITEMCHANGEDA || pNMHdr->code == HDN_ITEMCHANGEDW) ) {
			NMHEADER *pNMHeader = (NMHEADER *)lParam;
			if ( IsProportionalResizeHeader() && (pNMHeader->pitem->mask & HDI_WIDTH) ) {
				bool bResult = ProportionalResizeHeader( pNMHeader );

				m_bTrackInProgress = false;

				if ( !bResult ) {
					*pResult = TRUE;
					bBreak = TRUE;
				}
			}
		}

    if ( !bBreak && (pNMHdr->code == HDN_ENDTRACKA || pNMHdr->code == HDN_ENDTRACKW) ) {
			// Это работает даже не в режиме Report
			CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
			if ( pHeader && m_Columns && pHeader->GetItemCount() ) {
				CRect rcClientRect;
				GetClientRect( rcClientRect );
				if ( IsProportionalResizeHeader() ) {
//					NMHEADER *pNMHeader = (NMHEADER *)lParam;
//					ProportionalResizeHeader( pNMHeader );
				}
				else {
					for ( int i=0,n=pHeader->GetItemCount(); i<n; i++ ) {
						int iColumnWidth = GetColumnWidth( i );
						(*m_Columns)[i]->SetWidthPercent( (iColumnWidth * 100 / rcClientRect.Width()) % 100 );
					}
				}
			}
		}
		if ( bLocked )
			Unlock( E_TRACKINPROGRESS );
		if ( bBreak )
			return TRUE;
	}

	BOOL bResult = CListCtrl::OnNotify(wParam, lParam, pResult);
	return bResult;
}


// ---
void CCheckList::OnSetFocus(CWnd* pOldWnd) {
	CListCtrl::OnSetFocus(pOldWnd);
	int nSelectedIndex = GetSelectedIndex();
	if ( nSelectedIndex < 0 )
		SetItemSelected( 0 );
}


// ---
void CCheckList::OnSize(UINT nType, int cx, int cy) {
	CListCtrl::OnSize(nType, cx, cy);
/*
	if ( m_bFirstChanceSetSize && !m_bColumnWidthSetup && !m_bSizeOscillate && !m_bDynamicResizeHeader ) {
		m_bSizeOscillate = true;
		Lock( E_SIZEOSCILLATE );

		SetColumnsWidth();
		m_bSizeOscillate = false;
		Unlock( E_SIZEOSCILLATE );

		//m_bFirstChanceSetSize = false;
	}
*/
}

// ---
void CCheckList::OnWindowPosChanged( WINDOWPOS FAR* lpwndpos ) {
	if ( !m_bSizeOscillate ) {
		CListCtrl::OnWindowPosChanged(lpwndpos);

		if ( !m_bColumnWidthSetup && !m_bSizeOscillate && !m_bDynamicResizeHeader && m_bDefferedSetColumns ) {
			if ( !(lpwndpos->flags & SWP_NOSIZE) || (lpwndpos->flags & SWP_FRAMECHANGED) ) {
				CRect rcWindowRect;
				GetWindowRect( &rcWindowRect );

				CRect rcClientRect;
				GetClientRect( &rcClientRect );
				int nExtraSize = rcWindowRect.Width() - rcClientRect.Width();

				PostMessage( nDefferedPosChanged, (lpwndpos->cx ? lpwndpos->cx  - nExtraSize : rcClientRect.Width()) - 1, 0 );
				/*
				m_bSizeOscillate = true;
				Lock( E_SIZEOSCILLATE );

				SetColumnsByWidth( (lpwndpos->cx ? lpwndpos->cx  - nExtraSize : rcClientRect.Width()) - 1 );

				m_bSizeOscillate = false;
				Unlock( E_SIZEOSCILLATE );
				*/
				m_bDefferedSetColumns = false;
			}
		}

		CListCtrl::OnWindowPosChanged(lpwndpos);
	}
}

// ---
void CCheckList::OnWindowPosChanging( WINDOWPOS FAR* lpwndpos ) {
	if ( !m_bSizeOscillate ) {
		CListCtrl::OnWindowPosChanging(lpwndpos);

		if ( !m_bColumnWidthSetup && !m_bSizeOscillate && !m_bDynamicResizeHeader ) {
			if ( !(lpwndpos->flags & SWP_NOSIZE) || (lpwndpos->flags & SWP_FRAMECHANGED) ) {
				CRect rcWindowRect;
				GetWindowRect( &rcWindowRect );
				if ( lpwndpos->cx && lpwndpos->cx < rcWindowRect.Width() ) {
					CRect rcClientRect;
					GetClientRect( &rcClientRect );
					int nExtraSize = rcWindowRect.Width() - rcClientRect.Width();

					PostMessage( nDefferedPosChanging, (lpwndpos->cx ? lpwndpos->cx  - nExtraSize : rcClientRect.Width()) - 1, 0 );
					/*
					m_bSizeOscillate = true;
					Lock( E_SIZEOSCILLATE );

					SetColumnsByWidth( (lpwndpos->cx ? lpwndpos->cx  - nExtraSize : rcClientRect.Width()) - 1 );

					m_bSizeOscillate = false;
					Unlock( E_SIZEOSCILLATE );
					*/
				}
				else
					m_bDefferedSetColumns = true;
			}
		}

		CListCtrl::OnWindowPosChanging(lpwndpos);
	}
}

// ---
LRESULT CCheckList::OnDefferedPosChanging( WPARAM wParam, LPARAM lParam ) {
	if ( !m_bDestroyed ) {
		m_bSizeOscillate = true;
		Lock( E_SIZEOSCILLATE );

		SetColumnsByWidth( wParam );

		m_bSizeOscillate = false;
		Unlock( E_SIZEOSCILLATE );
	}

	return 0;
}

// ---
LRESULT CCheckList::OnDefferedPosChanged( WPARAM wParam, LPARAM lParam ) {
	if ( !m_bDestroyed ) {
		m_bSizeOscillate = true;
		Lock( E_SIZEOSCILLATE );

		SetColumnsByWidth( wParam );

		m_bSizeOscillate = false;
		Unlock( E_SIZEOSCILLATE );
	}
	return 0;
}

						/*
						for ( int iCorrectIndex=0,n=pColumnPercents.Count(); iCorrectIndex<n; iCorrectIndex++ ) {
							if ( pColumnWidths[iCorrectIndex] > HDC_MINWIDTH )
								break;
						}
						
						if ( iCorrectIndex >= 0 ) {
							if ( iPercSum < 100 || iWidthSum < iClientWidth ) {
								pColumnPercents[iCorrectIndex] += 100 - iPercSum;
								pColumnWidths[iCorrectIndex] += iClientWidth - iWidthSum;
							}
							if ( iPercSum >= 100 || iWidthSum > iClientWidth ) {
								pColumnPercents[iCorrectIndex] -= iPercSum - 100;
								pColumnWidths[iCorrectIndex] -= iWidthSum - iClientWidth;
							}
						}
						*/
						/*
						//int nExtraPercent = (iPercSum - 100) / int(pColumnPercents.Count());
						int nExtraWidth = (iWidthSum - iClientWidth) / int(pColumnPercents.Count());
						for ( i=0,n=pColumnPercents.Count(); i<n; i++ ) {
							pColumnWidths[i] -= nExtraWidth;
							if ( !HDC_WIDTHISTRUE(pColumnWidths[i]) )
								pColumnWidths[i] = HDC_MINWIDTH;

							pColumnPercents[i] = pColumnWidths[i] * 100 / iClientWidth;
						}
						*/




// ---
BOOL CCheckList::OnCommand( WPARAM wParam, LPARAM lParam ) {
	if ( !CListCtrl::OnCommand(wParam, lParam) ) {
		HWND hWndCtrl = (HWND)lParam;
		if ( !lParam ) {
			// menu or accelerator
			GetMessageReceiver()->SendMessage( CLWM_DOCOMMAND, LOWORD(wParam), 0 );
			return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

/*
// ---
BOOL CCheckList::CheckTextFitItsRect( CPoint point, CRect &rectCell ) {
  LVHITTESTINFO   hi;        
	int             nItem;
	TCHAR           szText[_MAX_PATH];
  LVITEM          lvi;        

	// Find item and subitem under a given point        
	hi.pt = point;
  nItem = SubItemHitTest( &hi );        
	if( nItem < 0 )
    return TRUE;                           

  // Retrieve text, image index and indent of the subitem
  lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;
  lvi.iItem = nItem;        
	lvi.iSubItem = hi.iSubItem;
  lvi.pszText = szText;
  lvi.cchTextMax = sizeof(szText)/sizeof(szText[0]);
  lvi.iIndent = 0;   
	lvi.iImage = -1;
	if( GetItem(&lvi) == FALSE )
    return TRUE;           // This should never happen, but...be paranoid

  // Retrieve bounding rect of the subitem.
  GetSubItemRect( nItem, hi.iSubItem, LVIR_BOUNDS, rectCell );

  // Adjust indent if subitem has icon.        
	if( lvi.iImage != -1 )
    lvi.iIndent += 20; // icon size(16) + some spacing

  // Strange, but GetStringWidth returns incorrect width of the string.
  // In all cases the width of the string is less than actual.
  // If somebody knows the reason please mail me.
  lvi.iIndent += 16;

  // Do not show tooltips if subitem is completely visible
  if( GetStringWidth( szText ) + lvi.iIndent < rectCell.Width() )
    return TRUE;

	return FALSE;
}
*/

// ---
BOOL CCheckList::CheckTextContentsShouldBeTipped( LPCTSTR pText ) { 
	return _tcsstr( pText, _T("__________") ) != pText;
}


// ---
BOOL CCheckList::CheckTextFitItsRect( int nRow, int nCol ) {
	CClientDC dc(this);
	CFont *pFont = GetFont(); 
	CFont *pFontDC = dc.SelectObject( pFont );	

	CRect calcRect(0,0,0,0);

	if( nCol == 0 ) {
		GetItemRect( nRow, &calcRect, LVIR_LABEL );
		calcRect.left += 4;
	}
	else {
		GetSubItemRect( nRow, nCol, LVIR_BOUNDS, calcRect );
		CRect iconRect;
		GetSubItemRect( nRow, nCol, LVIR_ICON, iconRect );
		calcRect.left = iconRect.right;
		calcRect.left += 6;
		calcRect.right -= 6;
	}

	calcRect.right = calcRect.Width();
	calcRect.left = 0;
	calcRect.bottom = calcRect.Height();
	calcRect.top = 0;

	CString textStr = GetTrueItemText( nRow, nCol );
	int nTextWidth = dc.GetTextExtent(textStr).cx;			
	
	dc.SelectObject( pFontDC );	

	if ( calcRect.Width() < nTextWidth && CheckTextContentsShouldBeTipped(textStr) ) 
		return FALSE;
	return TRUE;
}


// ---
void CCheckList::OnMouseMove( UINT nFlags, CPoint point ) {
	if( nFlags == 0 && IsUseTitleTip() && (GetStyle() & LVS_REPORT))	{		
		int nRow, nCol;		
		CRect rcCellRect;

		nRow = GetRowColumnIndex( point, &rcCellRect, &nCol );		
		if( nRow != -1 && !CheckTextFitItsRect(nRow, nCol) ) {
			CClientDC dc(this);
			CFont *pFont = GetFont(); 
			CFont *pFontDC = dc.SelectObject( pFont );	

			int nOffset = dc.GetTextExtent(_T(" "), 1).cx * 2;			

			CRect rcLabel(0,0,0,0);
			if( nCol == 0 ) {
				GetItemRect( nRow, &rcLabel, LVIR_LABEL );
				nOffset = rcLabel.left - rcCellRect.left + nOffset / 2 + 1;	
			}
			else
				nOffset += 2;

			rcCellRect.bottom += !!(GetExtendedStyle() & LVS_EX_GRIDLINES);
			m_TitleTip.Show( rcCellRect, GetTrueItemText(nRow, nCol), nOffset );		

			dc.SelectObject( pFontDC );	
		}	
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

// ---
void CCheckList::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) {
#ifdef ANTI_FLICKERING
  const UINT ENHANCEDKEY = 0x0100;
  const UINT PAGEUPKEY   = 0x21;
  const UINT PAGEDNKEY   = 0x22;
	
  // Handle enhanced keys since OnChar doesn't seem to get
  // them...
	if ( nFlags & ENHANCEDKEY ) {
    // Don't update window if paging up or down. It looks
    // terrible!
    switch ( nChar ) {
			case PAGEUPKEY:
				OnVScroll(SB_PAGEUP, 0, 0);
				return;
				
			case PAGEDNKEY:
				OnVScroll(SB_PAGEDOWN, 0, 0);
				return;
    }
  }
	
  CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
#else	
	if ( nChar == VK_SPACE ) {
		if ( IsEnableChangeItemsState() )
			CheckItem( GetSelectedIndex() );
	}
	else
		CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
#endif
}


// ---
void CCheckList::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	if ( nChar != VK_SPACE )
		CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}


// ---
void CCheckList::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	if ( nChar != VK_SPACE )
		CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

// ---
static CString LoadLocString( CWnd *pLocWnd, UINT uStrID ) {
	CString rcString;
	rcString.LoadString(uStrID);
	TCHAR *pStr = rcString.GetBuffer(_MAX_PATH);
	pLocWnd->SendMessage( nLoadLocalisedStringMessage, uStrID, LPARAM(pStr) );
	rcString.ReleaseBuffer();
	return rcString;
}

// ---
void CCheckList::OnPaint() {
	if ( IsForceAntiFlickering() ) {
		CRect  rc;
		GetParent()->GetClientRect(&rc);
		
		if ( rc.Size() == CSize(0,0) )
			return;
		
		CPaintDC dc(this);
		CDC     MemoryDC;
		CBitmap Bitmap;
		CBrush  Brush;
		
		CRect rcWnd;
		GetWindowRect(&rcWnd);
		ScreenToClient(&rcWnd);
		int rcWndW = rcWnd.Width(), rcWndH = rcWnd.Height();
/*		
		CRgn clip;
		clip.CreateRectRgnIndirect(&rcWnd);
		dc.SelectClipRgn(&clip);
*/		
		// Paint to a compatible DC
		MemoryDC.CreateCompatibleDC( &dc );
		Bitmap.CreateCompatibleBitmap( &dc, rcWndW, rcWndH );
		CBitmap *pOldBitmap = MemoryDC.SelectObject( &Bitmap );
		
		// Fill background (does what WM_ERASEBKGND would do)
		Brush.CreateSolidBrush( GetBkColor() );
		MemoryDC.FillRect( &CRect(0,0, rcWndW,rcWndH), &Brush );
		
		// Hook the header control so that paint gets redirected
		// to the memory dc
		// Это работает даже в режиме не-Report
		CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
		CWMPaintHook  hookHeaderCtrl( pHeader->GetSafeHwnd(), MemoryDC );
		
		// Do client and children areas first otherwise system
		// scrollbars don't draw correctly.
		Print( &MemoryDC, PRF_CLIENT|PRF_CHILDREN );
		Print( &MemoryDC, PRF_NONCLIENT );
		
		if ( (!m_Items || !m_Items->GetSize()) && IsDisplayNoItemsString() ) {
			COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
			COLORREF clrTextBk = GetBkColor();//::GetSysColor(COLOR_WINDOW);
			
			if ( !(GetStyle() & LVS_NOCOLUMNHEADER) ) {
				CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem( 0 ); 
				
				CRect rcH;
				pHeader->GetItemRect( 0, &rcH );
				rcWnd.top += rcH.bottom; 
			}
			
			rcWnd.top += 10; 
			
			CString strText = ::LoadLocString( GetMessageReceiver(), IDS_LST_NOITEMSINVIEW ); 
			
			MemoryDC.SetTextColor(clrText);
			MemoryDC.SetBkColor(clrTextBk);
			MemoryDC.FillRect(rcWnd, &CBrush(clrTextBk));
			//MemoryDC.SelectStockObject( ANSI_VAR_FONT );
			MemoryDC.DrawText( strText, -1, rcWnd, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP );
		}

		// Offset the MemoryDC drawing rectangle by rcBounds 
		CRect rcDraw = rc - rc.TopLeft();
		
		// Dump bits to real device context.
		dc.BitBlt( rc.left,rc.top, rc.Width(),rc.Height(), &MemoryDC, rcDraw.left,rcDraw.top, SRCCOPY );
		
		MemoryDC.SelectObject(pOldBitmap);
	}
	else {

		Default();
		
		if ( (!m_Items || !m_Items->GetSize()) && IsDisplayNoItemsString() ) {
			COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
			COLORREF clrTextBk = ::GetSysColor(COLOR_WINDOW);
			
			CDC* pDC = GetDC();        
			
			// Save dc state
			int nSavedDC = pDC->SaveDC(); 
			
			CRect rc;
			GetWindowRect(&rc);
			ScreenToClient(&rc);
			
			if ( !(GetStyle() & LVS_NOCOLUMNHEADER) ) {
				CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem( 0 ); 
				
				CRect rcH;
				pHeader->GetItemRect( 0, &rcH );
				rc.top += rcH.bottom; 
			}
			
			rc.top += 10; 
			
			CString strText = ::LoadLocString( GetMessageReceiver(), IDS_LST_NOITEMSINVIEW ); 
			
			pDC->SetTextColor(clrText);
			pDC->SetBkColor(clrTextBk);
			pDC->FillRect(rc, &CBrush(clrTextBk));
			//pDC->SelectStockObject( ANSI_VAR_FONT );
			pDC->DrawText( strText, -1, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP );
			
			// Restore dc  
			pDC->RestoreDC( nSavedDC );
			ReleaseDC( pDC );
		}
	}
}

// ---
void CCheckList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if ( IsForceAntiFlickering() ) {
		switch ( nSBCode ) {
			case SB_PAGEDOWN:
			case SB_PAGEUP:
			case SB_LINEDOWN:
			case SB_LINEUP: {
				// Don't update window if paging up or down. It looks
				// terrible!
	//			LockWindowUpdate();
	//			CListCtrl::OnVScroll( nSBCode, nPos, pScrollBar );
	//			UnlockWindowUpdate();
				bool bAntiFlickering = IsForceAntiFlickering();
				SetForceAntiFlickering( false );
				LockWindowUpdate();
				CListCtrl::OnVScroll( nSBCode, nPos, pScrollBar );
				UnlockWindowUpdate();
				UpdateWindow();
				SetForceAntiFlickering( bAntiFlickering );
				break;
			}	
			default:
				CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
				break;
		}
	}
	else
		CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


// ---
BOOL CCheckList::OnEraseBkgnd(CDC* pDC) {
	if ( IsForceAntiFlickering() ) 
		return TRUE;
	else
		return CListCtrl::OnEraseBkgnd(pDC);
}


template <class Type>
class  CPSyncArrayEx : public CPSyncArray<Type> {
	friend void destroy_array( CPSyncArray<Type>& arr );
};


void destroy_array( CPSyncArray<CCheckListItem>& arr ) {
  uint i = 0;
	CPSyncArrayEx<CCheckListItem>& arr2 = (CPSyncArrayEx<CCheckListItem>&)arr;
  for( CPSyncArrayEx<CCheckListItem>::TPtr* parr=arr2.parr; i<arr2.count; i++,parr++ ) {
    delete *parr;
    *parr = 0;
  }  
  arr2.count = 0;
}




void destroy_array( CPSyncArray<CCheckListColumn>& arr ) {
  uint i = 0;
	CPSyncArrayEx<CCheckListColumn>& arr2 = (CPSyncArrayEx<CCheckListColumn>&)arr;
  for( CPSyncArrayEx<CCheckListColumn>::TPtr* parr=arr2.parr; i<arr2.count; i++,parr++ ) {
    delete *parr;
    *parr = 0;
  }  
  arr2.count = 0;
}





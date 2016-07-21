// TreeWnd.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include "WASCRes.rh"
#include <WASCMess.h>
#include <Stuff\SArray.h>
#include <StuffIO\Utils.h>
#include "TreeWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define getKState(p) (::GetKeyState(p) & 0x8000)

/////////////////////////////////////////////////////////////////////////////
// CTreeWnd
// ---
CTreeWnd::CTreeWnd() :
  lastAddNode( NULL ),
  lastAddLevel( 0 ),
	notRedrawCount( 0 ),
	m_bDeleteCollapsedSubTree( false ),
	m_bPossibleRedraw( true ) {
}

// ---
CTreeWnd::~CTreeWnd()	{
}

// ---
void CTreeWnd::SetRedraw( BOOL bRedraw ) {
	if ( !bRedraw ) {
		if ( !notRedrawCount )
			CTreeCtrl::SetRedraw( FALSE );
		notRedrawCount++;
	}
	else {
		if ( notRedrawCount ) 
			notRedrawCount--;
		if ( !notRedrawCount && IsPossibleRedraw() ) 
			CTreeCtrl::SetRedraw( TRUE );
	}
}

// Нумератор для поиска узла по индексу
// ---
HTREEITEM CTreeWnd::RecourseEnumeration( HTREEITEM hNode, int &index ) {
  HTREEITEM result = hNode;
  HTREEITEM child = GetChildItem( hNode );
  while ( index >= 0 && child ) {
    index--;
    result = RecourseEnumeration( child, index );
    child = GetNextSiblingItem( child );
  }  
  return result;
}
    

// Нумератор для поиска индекса заданного узла
// ---
bool CTreeWnd::RecourseEnumeration( HTREEITEM hNode, HTREEITEM find, int &index ) {
  if ( hNode != find ) {
		HTREEITEM child = GetChildItem( hNode );
		bool result = false;
		while ( child && !result ) {
			if ( child != find ) {
				index++;
				result = RecourseEnumeration( child, find, index );
				child = GetNextSiblingItem( child );
			}  
			else 
				result = true;
		}
		return result;
	}
	return true;
}

// Нумератор для поиска уровня заданного узла
// ---
bool CTreeWnd::RecourseEnumeration( HTREEITEM hNode, int findInd, int &index, int &level ) {
  HTREEITEM child = GetChildItem( hNode );
  bool result = false;
  if ( child ) {
    level++;
    while ( child && !result ) {
      if ( index != findInd ) {
        index++;
        result = RecourseEnumeration( child, findInd, index, level );
        child = GetNextSiblingItem( child );
      }  
      else 
        result = true;
    }
    if ( !result )
      level--;
  }
  return result;
}


// Найти узел по индексу    
// ---
HTREEITEM CTreeWnd::NodeByIndex( int ind ) {
  int index = ind;
  return RecourseEnumeration( /*GetRootItem()*/NULL, index );
}


// Раскрыть узел по индексу    
// ---
void CTreeWnd::ExpandNode( int ind ) {    
  HTREEITEM treeNode = NodeByIndex( ind );
  if ( treeNode )
    Expand( treeNode, TVE_EXPAND );
}  


// Закрыть узел по индексу    
// ---
void CTreeWnd::CollapseNode( int ind ) {    
  HTREEITEM treeNode = NodeByIndex( ind );
  if ( treeNode )
    Expand( treeNode, TVE_COLLAPSE /*| TVE_COLLAPSERESET*/ );
}  


// Селектировать узел по индексу
// ---
void CTreeWnd::SelectNode( int ind ) {    
  HTREEITEM treeNode = NodeByIndex( ind );
//  if ( treeNode ) Пусть сам разбирается
    Select( treeNode, TVGN_CARET );
}
    

// Получить индекс селектированного узла    
// ---
int CTreeWnd::GetSelectedIndex() {    
  int index = 0;
  RecourseEnumeration( /*GetRootItem()*/NULL, GetSelectedItem(), index );
  return index;  
}

// Раскрыт ли указанный узел
// ---
bool CTreeWnd::IsExpanded( int ind ) {    
  return IsExpanded( NodeByIndex(ind) );
}


// Раскрыт ли указанный узел
// ---
bool CTreeWnd::IsExpanded( HTREEITEM treeNode ) {    
  if ( treeNode ) {
    if ( GetChildItem(treeNode) ) {
      uint state = TVIS_EXPANDED;
      state = GetItemState( treeNode, state );
      return bool(!!(state & TVIS_EXPANDED));
    }
    return true;
  }
  return false;
}


// Видим ли указанный узел
// ---
bool CTreeWnd::IsNodeVisible( int ind ) {
  bool visible = true;
  for ( --ind; ind >= 0 && visible; ind-- )
    visible = IsExpanded( ind );
  return visible;  
}


// Получить уровень заданного узла по индексу
// ---
int CTreeWnd::GetNodeLevel( int ind ) {
  int index = 0;
  int level = 0;
  RecourseEnumeration( /*GetRootItem()*/NULL, ind, index, level );
  return level;  
}

// Получить уровень заданного узла 
// ---
int CTreeWnd::GetNodeLevel( HTREEITEM hNode ) {
  int level = 1;
  HTREEITEM parent = GetParentItem( hNode );
  while ( parent ) {
    parent = GetParentItem( parent );
    level++;
  }  
  return level;  
}


// Проверить стиль
// ---
bool CTreeWnd::HasStyle( uint32 mask ) {
	return !!(GetStyle() & mask);
}


// ---
HTREEITEM CTreeWnd::AddItemChild( HTREEITEM hNode, CTreeItemInfo &newNode ) {
  newNode.hParent      = hNode;
  newNode.hInsertAfter = TVI_LAST;
/*
	if ( IsStateImageType() ) {
		newNode.item.mask |= TVIF_STATE;
		newNode.item.state = INDEXTOSTATEIMAGEMASK(1);
		newNode.item.stateMask = TVIS_STATEIMAGEMASK;
	}
	else {
		newNode.item.mask |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		newNode.item.iImage = 0;
		newNode.item.iSelectedImage = 0;
	}
*/
  return InsertItem( &newNode );
}


// ---
HTREEITEM CTreeWnd::InsertItemChild( HTREEITEM hParentNode, HTREEITEM hAfterNode, CTreeItemInfo &newNode ) {
  newNode.hParent      = hParentNode;
  newNode.hInsertAfter = hAfterNode == hParentNode ? TVI_FIRST : hAfterNode;
/*
	if ( IsStateImageType() ) {
		newNode.item.mask |= TVIF_STATE;
		newNode.item.state = INDEXTOSTATEIMAGEMASK(1);
		newNode.item.stateMask = TVIS_STATEIMAGEMASK;
	}
	else {
		newNode.item.mask |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		newNode.item.iImage = 0;
		newNode.item.iSelectedImage = 0;
	}
*/
  return InsertItem( &newNode );
}


// Добавить узел в дерево
// ---
HTREEITEM CTreeWnd::AddNode( CTreeItemInfo &newNode, HTREEITEM addAfter ) {
	// Последний уровень
	lastAddLevel = lastAddLevel ? lastAddLevel : GetNodeLevel( addAfter );
	lastAddNode  = lastAddNode ? lastAddNode : addAfter;

  if ( GetCount() >= 1 ) {
    // Если в дереве есть уже узлы
    if ( newNode.level != 1 && !HasStyle(TVS_LINESATROOT) ) {
      // Если заказанный уровень не первый - установить стиль "веточки от корня"
      ModifyStyle( 0, TVS_LINESATROOT );
		  ModifyStyle( 0, TVS_HASLINES | TVS_HASBUTTONS );
			SetRedraw( TRUE );
		}

    if ( lastAddLevel == newNode.level ) {
      // Если уровень тот же, что и последний - 
      // просто добавить узел к папе последнего узла
      lastAddNode = AddItemChild( GetParentItem(lastAddNode), newNode );
      lastAddLevel = newNode.level;
    }  
    else { 
      if ( lastAddLevel <= newNode.level ) {
        // Если уровень больше - появляется новое поддерево
        // добавить узел к последнему добавленному
        lastAddNode = AddItemChild( lastAddNode, newNode );
        lastAddLevel = newNode.level;
      }  
      else {
        // Иначе - подниматься по папам до заданного уровня
        HTREEITEM parent = GetParentItem(lastAddNode);
        while ( parent && GetNodeLevel(parent) > newNode.level ) 
          parent = GetParentItem(parent);
        if ( parent ) {  
          lastAddNode = AddItemChild( GetParentItem(parent), newNode );
          lastAddLevel = newNode.level;
        }
      }   
    }  
  }
  else {
    if ( HasStyle(TVS_LINESATROOT) ) {
      // Если в дереве еще не было узлов - снять стиль "веточки от корня"
      ModifyStyle( TVS_LINESATROOT, 0 );
			SetRedraw( TRUE );
		}

    // Добавить узел в корень дерева  
    lastAddNode  = AddItemChild( /*GetRootItem()*/NULL, newNode );
    lastAddLevel = newNode.level;
  }

	return lastAddNode;
}


// Добавить узел в дерево
// ---
HTREEITEM CTreeWnd::InsertNode( CTreeItemInfo &newNode, HTREEITEM addAfter ) {
	// Последний уровень
	lastAddLevel = lastAddLevel ? lastAddLevel : GetNodeLevel( addAfter );
	lastAddNode  = lastAddNode ? lastAddNode : addAfter;

  if ( GetCount() >= 1 ) {
    // Если в дереве есть уже узлы
    if ( newNode.level != 1 && !HasStyle(TVS_LINESATROOT) ) {
      // Если заказанный уровень не первый - установить стиль "веточки от корня"
      ModifyStyle( 0, TVS_LINESATROOT );
		  ModifyStyle( 0, TVS_HASLINES | TVS_HASBUTTONS );
			SetRedraw( TRUE );
		}

    if ( lastAddLevel == newNode.level ) {
      // Если уровень тот же, что и последний - 
      // просто добавить узел к папе последнего узла
      lastAddNode = InsertItemChild( GetParentItem(lastAddNode), lastAddNode, newNode );
      lastAddLevel = newNode.level;
    }  
    else { 
      if ( lastAddLevel <= newNode.level ) {
        // Если уровень больше - появляется новое поддерево
        // добавить узел к последнему добавленному
        lastAddNode = AddItemChild( lastAddNode, newNode );
        lastAddLevel = newNode.level;
      }  
      else {
        // Иначе - подниматься по папам до заданного уровня
        HTREEITEM parent = GetParentItem(lastAddNode);
        while ( parent && GetNodeLevel(parent) > newNode.level ) 
          parent = GetParentItem(parent);
        if ( parent ) {  
          lastAddNode = AddItemChild( GetParentItem(parent), newNode );
          lastAddLevel = newNode.level;
        }
      }   
    }  
  }
  else {
    if ( HasStyle(TVS_LINESATROOT) ) {
      // Если в дереве еще не было узлов - снять стиль "веточки от корня"
      ModifyStyle( TVS_LINESATROOT, 0 );
			SetRedraw( TRUE );
		}

    // Добавить узел в корень дерева  
    lastAddNode  = AddItemChild( /*GetRootItem()*/NULL, newNode );
    lastAddLevel = newNode.level;
  }

	return lastAddNode;
}


// Удалить поддерево данного узла
// ---
void CTreeWnd::DeleteSubTree( HTREEITEM parentNode, bool bRootIncluded ) {
  HTREEITEM child = GetChildItem( parentNode );
  while ( child ) {
		HTREEITEM nextChild = GetNextSiblingItem( child );
		DeleteSubTree( child );
		DeleteItem( child );
    child = nextChild;
  }  
	if ( bRootIncluded )
		DeleteItem( parentNode );
}


// Установить данные узла по индексу
// ---
void CTreeWnd::SetNodeData( uint32 data, int ind ) {
  HTREEITEM treeNode = NodeByIndex( ind );
  if ( treeNode )
    SetItemData( treeNode, data );
}


// Получить данные узла по индексу
// ---
uint32 CTreeWnd::GetNodeData( int ind ) {
  uint32 data = 0;
  HTREEITEM treeNode = NodeByIndex( ind );
  if ( treeNode )
    return GetItemData( treeNode );
  return data;
}

// Является ли узел последним (есть ли у него дети)
// ---
bool CTreeWnd::IsLeaf( int ind ) {
  return IsLeaf( NodeByIndex(ind) );
}


// Является ли узел последним (есть ли у него дети)
// ---
bool CTreeWnd::IsLeaf( HTREEITEM treeNode ) {
  if ( treeNode )
    return GetChildItem(treeNode) == NULL;
  return true;  
}


// Получить индекс заданного узла
// ---
int CTreeWnd::IndexNode( HTREEITEM hNode ) {
	if ( hNode ) {
		int index = 0;
		RecourseEnumeration( /*GetRootItem()*/NULL, hNode, index );
		return index; 
	}
	return -1;
}


// Нумератор для операции "итерация по всем"
// ---
HTREEITEM CTreeWnd::ForEachEnumeration( HTREEITEM hNode, TvActionFunc action, void *param, int &index ) {
  HTREEITEM result = hNode;
  HTREEITEM child = GetChildItem( hNode );
  while ( child ) {
		index++;
    action( *this, child, index, param );
    result = ForEachEnumeration( child, action, param, index );
    child = GetNextSiblingItem( child );
  }  
  return result;
}
    


// Итератор "по всем"
// ---
void CTreeWnd::ForEach( TvActionFunc action, void *param, HTREEITEM fromNode ) {
  int index = fromNode ? IndexNode( fromNode ) : -1;
  ForEachEnumeration( /*GetRootItem()*/fromNode, action, param, index );
}


// Нумератор для операции "итератор до первого кто"
// ---
HTREEITEM CTreeWnd::FirstThatEnumeration( HTREEITEM hNode, TvCondFunc action, void *param, int &index, bool &result ) {
  HTREEITEM child = GetChildItem( hNode );
  HTREEITEM resNode = child;
  while ( child && !result ) {
    index++;
    if ( !action(*this, child, index, param) ) {
      resNode = FirstThatEnumeration( child, action, param, index, result );
      child = GetNextSiblingItem( child );
    }  
    else {
      result = true;
      resNode = child;
      break;
    }  
  }
  return resNode;
}


// Итератор "до первого кто"
// ---
HTREEITEM CTreeWnd::FirstThat( TvCondFunc action, void *param, HTREEITEM fromNode ) {
  int index = fromNode ? IndexNode( fromNode ) : -1;
  bool result = false;
	HTREEITEM node = FirstThatEnumeration( /*GetRootItem()*/fromNode, action, param, index, result );
  return node && result ? node : NULL;
}


// Получить узел под курсором мыши
// ---
HTREEITEM CTreeWnd::ItemFromMousePos() {
  CPoint point;
  ::GetCursorPos( &point );
  ::MapWindowPoints( NULL, GetSafeHwnd(), &point, 1 );
  return HitTest( point );
}


BEGIN_MESSAGE_MAP(CTreeWnd, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeWnd)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeWnd message handlers

// ---
void CTreeWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if ( nChar == VK_MULTIPLY/*'*'*/ ) {
		SetPossibleRedraw( false );
		SetRedraw(FALSE);
	}

  CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );

	if ( nChar == VK_MULTIPLY/*'*'*/ ) {
		SetPossibleRedraw( true );
		SetRedraw(TRUE);
	}
}

// ---
void CTreeWnd::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  CTreeCtrl::OnChar( nChar, nRepCnt, nFlags );
}


// ---
void CTreeWnd::OnRButtonDown( UINT nFlags, CPoint point ) {
  HTREEITEM selection = HitTest( point );

  if ( selection ) 
    SelectItem( selection );
	
	CTreeCtrl::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// ---
HFONT CCTFontCache::Find( DWORD dwStyle ) {
	for ( int i=0,c=Count(); i<c; i++ ) {
		if ( (*this)[i].m_dwStyle == dwStyle )
			return (*this)[i].m_hFont;
	}
	return NULL;
}

// ---
void CCTFontCache::Flush() {
	for ( int i=0,c=Count(); i<c; i++ ) {
		::DeleteObject( (*this)[i].m_hFont );
	}
}

// ---
CCheckTreeItem::CCheckTreeItem() :
  fChecked( ie_Unchecked ),
	fEnabled( true ),						      // Разрешен
  fNode   ( false ),
	fMarkable( true ),
	fExpanded( false ),
	fSelected( false ),
	fFirstVisible( false ),
	fOwnsData( false ),
	fStrictlyDisabled( false ),
  m_Text( "" ),
	m_TreeItem( NULL ),
	m_Font( NULL ),
	m_Data( NULL ),
  m_NumItems( 0 ),
  m_NumItemsChecked( 0 ),
	m_NumItemsSomeChecked( 0 ),
  m_Level( 1 ),
	m_ImageIndex( ii_LeafUnchecked ),
	m_CurrImageIndex( -1 ) {
}

// ---
CCheckTreeItem::~CCheckTreeItem() {
	if ( IsOwnsData() )
		delete m_Data;
}

// ---
bool CCheckTreeItem::operator == ( const CCheckTreeItem &rItem ) {
	return fChecked								== rItem.fChecked								&&
//				fEnabled									== rcItem.fEnabled								&& не проверять
				 fNode									== rItem.fNode									&&
				 fMarkable							== rItem.fMarkable							&&
//				 fExpanded							== rItem.fExpanded							&& не проверять
				 m_Text									== rItem.m_Text									&&
//				 m_TreeItem							== rItem.m_TreeItem							&& не проверять ! Нужно переинициировать
				 m_NumItems							== rItem.m_NumItems							&&
				 m_NumItemsChecked			== rItem.m_NumItemsChecked			&&
				 m_NumItemsSomeChecked	== rItem.m_NumItemsSomeChecked	&&
				 m_Level								== rItem.m_Level								&&
//				 m_CurrImageIndex				== rItem.m_CurrImageIndex     &&
				 m_ImageIndex						== rItem.m_ImageIndex;
}
      

// --- 
void CCheckTreeItem::Assign( const CCheckTreeItem &rItem ) {
	fChecked							= rItem.fChecked;
//	fEnabled									= rcItem.fEnabled								;
  fNode									= rItem.fNode;
  fMarkable							= rItem.fMarkable;
  fExpanded							= rItem.fExpanded;
  fSelected							= rItem.fSelected;
  fFirstVisible					= rItem.fFirstVisible;
  m_Text								= rItem.m_Text;
  m_TreeItem						= NULL;
  m_NumItems						= rItem.m_NumItems;
  m_NumItemsChecked			= rItem.m_NumItemsChecked;
  m_NumItemsSomeChecked	= rItem.m_NumItemsSomeChecked;
  m_Level								= rItem.m_Level;
  m_CurrImageIndex			= -1;
  m_ImageIndex					= rItem.m_ImageIndex;
}


// ---
CCheckTreeItem *CCheckTreeItem::Duplicate() {
	return (new CCheckTreeItem)->operator = ( *this );
}


// --- 
void CCheckTreeItem::SetText( const CString &setText ) {
  m_Text = setText;
}

// --- 
CString &CCheckTreeItem::GetText() {
  return m_Text;
}


// ---
int CCheckTreeItem::GetImageIndex() {
	return int( IsMarkable()
							? IsNode()
								? IsChecked()
									? GetChecked() == ie_SomeChecked
										? ii_NodeSomeChecked
										:	ii_NodeChecked
									: ii_NodeUnchecked
								:	IsChecked()
									? ii_LeafChecked 
									: ii_LeafUnchecked
							: 0
						); 
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// ---
CCTItemsArray::CCTItemsArray( bool owns ) : 
  CPSyncArray<CCheckTreeItem>( 100, 1, owns ) {
}

// ---
CCTItemsArray::~CCTItemsArray() {
	RemoveAll();
}


// ---
void CCTItemsArray::RemoveAt( int nIndex ) { 
	CPSyncArray<CCheckTreeItem>::RemoveInd( nIndex );
}


// ---
int CCTItemsArray::FindIt( CCheckTreeItem *item ) {
	for ( int i=0, c=GetSize(); i<c; i++ )
		if ( (*this)[i] == item )
			return i;
	return -1;
}

// ---
void CCTItemsArray::InsertAt( int nIndex, CCTItemsArray *pNewArray ) {
	for ( int i=0,c=pNewArray->GetSize(); i<c; i++ )
		Insert( nIndex++, (*pNewArray)[i] );
}

// ---
void CCTItemsArray::InsertAt( int nIndex, CCheckTreeItem *pNewItem ) {
	Insert( nIndex, pNewItem );
}


// ---
bool CCTItemsArray::operator == ( const CCTItemsArray &rItems ) {
	if ( GetSize() == rItems.GetSize() ) {
		for ( int i=0, c=GetSize(); i<c; i++ )
			if ( *(*this)[i] != *rItems[i] )
				return false;
		return true;
	}
	return false;
}

// ---
void CCTItemsArray::operator = ( const CCTItemsArray &rItems ) {
	RemoveAll();

	SetOwns( rItems.GetOwns() );

	for ( int i=0, c=rItems.GetSize(); i<c; i++ ) 
		Add( rItems[i]->Duplicate() );
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

static UINT mAfterLoadExpanding = ::RegisterWindowMessage( _T("AfterLoadExpanding") ); 

// CCheckTree

// ---
CCheckTree::CCheckTree() :
	m_hOldFont( NULL ),
	m_MsgReceiver( NULL ),
	m_hAccelTable( NULL ),
	m_bAccelTableShouldBeDestroyed( false ),
	m_bInterfaceLocked( false ),
	m_nInterfaceLockCount( 0 ),
  m_Items( NULL ), 
  m_ImageList(),
	m_nFirstVisibleInd( -1 ),
	fSpreadMark( true ),				// Распространять маркировку вниз и вверх
	fStateImageType( false ),		// Image типа State
	fOwnsItemsArray( true ),		// Владеет (сам уничтожает) массив элементов
	fSetupImagesAtLoad( true ),	// Устанавливать иконы при загрузке
	fEnableOpOnDisabled( true ), // Разрешены операции на запрещенном узле
	fEnableChangeItemsState( true ), // Разрешено изменение состояния элементов
	fUseCheckSemantic( true ),				// Использовать семантику CheckTree
	fEnablePostLoadProcessing( true ) // Разрешить обработку после загрузки
{
}


// ---
CCheckTree::~CCheckTree() {
	if ( IsOwnsItemsArray() )
		delete m_Items;

	if ( m_bAccelTableShouldBeDestroyed )
		::DestroyAcceleratorTable( m_hAccelTable );
}


// ---
void CCheckTree::StartupInitiation() {
	::SendMessage( *this, WM_SETFONT, ::SendMessage(::GetParent(*this), WM_GETFONT, 0, 0), 0 );

	CreateImageList();

	ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES );	
	
	LoadTreeWindow();
}


// ---
void CCheckTree::CreateImageList() {
	m_ImageList.Create( IsStateImageType() 
											? IDB_WAS_CHECKLISTSTATEIMAGE 
											: IDB_WAS_CHECKLISTIMAGE,
											20, 1, RGB(255, 255, 255) );
	SetImageList( &m_ImageList, IsStateImageType() ? TVSIL_STATE : TVSIL_NORMAL );
}



// ---
void CCheckTree::PreSubclassWindow() {
	CTreeWnd::PreSubclassWindow();

	if ( ::IsDlgClassName(*GetParent()) ) 
		StartupInitiation();
}


// ---
BOOL CCheckTree::PreTranslateMessage( MSG* pMsg ) {
	if ( m_hAccelTable && (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) )
		if ( ::GetFocus() == HWND(*this) && ::TranslateAccelerator(*this, m_hAccelTable, pMsg) )
			return TRUE;

	return CTreeWnd::PreTranslateMessage(pMsg);
}

// ---
void CCheckTree::SetAccelTable( HACCEL	hAccelTable, bool bShouldBeDestroyed ) {
	m_hAccelTable = hAccelTable;
	m_bAccelTableShouldBeDestroyed = bShouldBeDestroyed;
}

// ---
void CCheckTree::LockInterface( bool bLock ) {
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
CWnd *CCheckTree::GetMessageReceiver() {
	if ( m_MsgReceiver )
		return m_MsgReceiver;

	return GetParent();
}

// ---
COLORREF CCheckTree::SetBkColor( COLORREF crColor ) {
	if ( m_ImageList.GetSafeHandle() )
		m_ImageList.SetBkColor( crColor );

	return CTreeWnd::SetBkColor( crColor );
}


// ---
void CCheckTree::SetNodeImage( CCheckTreeItem *item, HTREEITEM hNode ) {

  int imageIndex = item->GetImageIndex();
	if ( item->GetCurrImageIndex() != imageIndex ) {
		if ( !hNode )
			hNode = m_Items ? NodeByIndex( m_Items->FindIt(item) ) : NULL;
#if 0
		HTREEITEM hCurrNode = NodeByIndex( m_Items->FindIt(item) );
		if ( hCurrNode != hNode )
			hNode = hCurrNode;
#endif

		if ( IsStateImageType() )
			// "0"-image must be blank
			SetItemState( hNode, INDEXTOSTATEIMAGEMASK(imageIndex + 1), TVIS_STATEIMAGEMASK );
		else
			SetItemImage( hNode, imageIndex, imageIndex );

		item->SetCurrImageIndex( imageIndex );
		if ( GetCount() == 1 )
			SetRedraw( TRUE );
	}
}


// Подсчитать количество прямых детей узла
// ---
static void SetupNodeChildren( CCheckTree *owner, CCTItemsArray &aItemsArray, int ind ) {
	if ( ind >= 0 ) {
		CCheckTreeItem *item = aItemsArray[ind];

		int firstChildLevel = item->GetLevel() + 1; // Уровень первого ребенка
		int numItems    = 0;
		int numMarkable = 0;

		for ( int i=ind+1,c=aItemsArray.GetSize(); i<c; i++ ) {
			CCheckTreeItem *cItem = aItemsArray[i];
			if ( cItem->GetLevel() == firstChildLevel ) {
				numMarkable += !owner->IsSpreadMark() || cItem->IsMarkable();
				numItems++;
			}
			else  
				if ( cItem->GetLevel() < firstChildLevel ) 
					break;
		}

		item->SetNode( !!numMarkable );  // Это узел
		item->SetNumItems( numItems );
	}
}


// Установить состояние узла в массиве узлов
// ---
static void SetupNodes( CTreeWnd &owner, HTREEITEM hNode, int ind, void *par ) {
  if ( hNode ) {
    CCTItemsArray &aItemsArray = *(CCTItemsArray *)par;
    CCheckTree    &list  = (CCheckTree &)owner; 
		CCheckTreeItem *item = aItemsArray[ind];
    if ( owner.GetChildItem(hNode) ) {
      // Если у узла есть дети

      if ( list.IsSpreadMark() ) {
				// Инача за все отвечает сам Item
				list.InternalSetItemChecked( item, CCheckTreeItem::ie_Unchecked );// Пока не селектирован

				item->SetNumItemsChecked( 0 );  // Селектированных детей нет
			}

      // Подсчитать количество прямых детей узла
			::SetupNodeChildren( &list, aItemsArray, ind );
    }  
  }
}


// Установить состояние узлов в массиве узлов
// ---
void CCheckTree::SetupNodes( HTREEITEM fromNode ) {
	CTreeWnd::ForEach( ::SetupNodes, (void *)m_Items, fromNode );
}

// Установить состояние узла в дереве
// ---
static void SetupImages( CTreeWnd &owner, HTREEITEM hNode, int ind, void *par ) {
  if ( hNode ) {
    CCTItemsArray &m_Items = *(CCTItemsArray *)par;
    CCheckTree    &list  = (CCheckTree &)owner; 
		CCheckTreeItem *item = m_Items[ind];

    if ( !item->IsNode() ) {
      // Если это не узел - установить ему картинки
			list.SetNodeImage( item, hNode );

      // Установить состояние всех родителей узла
      list.CheckParents( hNode );
    }
    else {
//			if ( item->IsChecked() )
				// Если узел включен - включить всех его детей
				list.CheckChildren( hNode, ind, CCheckTreeItem::CieChecked(item->IsChecked()) );

			// Установить состояние всех родителей узла  
			list.CheckParents( hNode );
    }
  }
}


// Установить состояние узлов в дереве
// ---
void CCheckTree::SetupImages( HTREEITEM fromNode ) {
	CTreeWnd::ForEach( ::SetupImages, (void *)m_Items, fromNode );
}


// Добавить узел (автоматически размещается на заданном уровне)
// ---
HTREEITEM CCheckTree::AddItem( CCheckTreeItem *item, HTREEITEM addAfter ) {
	return AddNode( CTreeItemInfo(item->GetText(), item->GetLevel()), addAfter );
}


// ---
HTREEITEM CCheckTree::InsertItem( CCheckTreeItem *item, HTREEITEM addAfter ) {
	return InsertNode( CTreeItemInfo(item->GetText(), item->GetLevel()), addAfter );
}


// ---
static void SetupInternalInfo( CTreeWnd &owner, HTREEITEM hNode, int ind, void *par ) {
  CCheckTree    &list  = (CCheckTree &)owner; 

	CCheckTreeItem *item = (*(CCTItemsArray *)par)[ind];
	
	item->SetTreeItem( hNode );

	list.SetItemData( hNode, (DWORD)item ); 

	// Item создан - должен обязательно перерисоваться
	item->SetCurrImageIndex( -1 );

	list.SetNodeImage( item, hNode );
}


// Загрузить дерево
// ---
void CCheckTree::LoadTreeWindow( HTREEITEM loadAfter, bool bRootInserted ) {
	if ( m_Items && m_Items->GetSize() ) {
		// Будешь комментарить - задумайся. Имею место нарушения перерисовки.
//		SetRedraw(FALSE);
		
		HWND hwndFocused;
		int selectedInd;
		int visibleInd;

		if ( IsPossibleRedraw() ) {
			hwndFocused = ::GetFocus();

			selectedInd = IndexNode( GetSelectedItem() );
			visibleInd = IndexNode( GetFirstVisibleItem() );
		}

		int parentIndex = IndexNode( bRootInserted ? GetParentItem(loadAfter) : loadAfter );
		parentIndex = parentIndex >= 0 ? parentIndex : 0;

		int firstIndex = bRootInserted ? IndexNode(loadAfter) : parentIndex + !!loadAfter;

		HTREEITEM addAfter = bRootInserted ? GetPrevSiblingItem(loadAfter) : loadAfter;
		if ( !addAfter && bRootInserted )
			addAfter = GetParentItem(loadAfter);

		if ( loadAfter )
			DeleteSubTree( loadAfter, bRootInserted );
		else
			DeleteAllItems();

		NewLoadSession();

		if ( bRootInserted && firstIndex < m_Items->GetSize() ) {
			CCheckTreeItem *item = (*m_Items)[firstIndex];
			
			HTREEITEM hAddedItem = InsertItem( item, addAfter );
			
			item->SetTreeItem( hAddedItem );

			SetItemData( hAddedItem, (DWORD)item ); 
	
			// Item создан - должен обязательно перерисоваться
			item->SetCurrImageIndex( -1 );

			addAfter = loadAfter = hAddedItem;

			parentIndex = IndexNode( hAddedItem );
			parentIndex = parentIndex >= 0 ? parentIndex : 0;

			firstIndex = parentIndex + 1;
		}

		if ( firstIndex < m_Items->GetSize() ) {
			CCheckTreeItem *prItem = loadAfter ? (*m_Items)[parentIndex] : NULL;
			CCheckTreeItem::CieChecked fHowToChecked = prItem 
																								 ? prItem->GetChecked()
																								 : CCheckTreeItem::ie_Unchecked;
		
			int lastLevel  = (*m_Items)[parentIndex]->GetLevel() + !!loadAfter;

			for ( int i=firstIndex,c=m_Items->GetSize(); i<c && (*m_Items)[i]->GetLevel() >= lastLevel; i++ ) {
				// Добавить в дерево элементы из массива
				CCheckTreeItem *item = (*m_Items)[i];
				
				HTREEITEM hAddedItem = AddItem( item, addAfter );
				
				::SetupInternalInfo( *this, hAddedItem, i, m_Items );

				if ( item->IsSelected() )
					selectedInd = i;

				if ( item->IsFirstVisible() )
					visibleInd = i;
/*
				item->SetTreeItem( hAddedItem );

				SetItemData( hAddedItem, (DWORD)item ); 
		
				// Item создан - должен обязательно перерисоваться
				item->SetCurrImageIndex( -1 );
*/				
				addAfter = NULL;
			}

			if ( IsUseCheckSemantic() ) {
				// Установить сстояние корневого узла
				::SetupNodeChildren( this, *m_Items, parentIndex );

				if ( prItem )
					SetItemChecked( parentIndex, fHowToChecked, true );

				// Установить состояние узлов в массиве узлов
				SetupNodes( loadAfter );

				if ( prItem )
					SetItemChecked( parentIndex, fHowToChecked, true );
			}
			// Установить состояние узлов в дереве
			if ( IsSetupImagesAtLoad() ) 
				SetupImages( loadAfter );
		}

		PostLoadProcessing();

		if ( IsPossibleRedraw() ) {
			HTREEITEM hNode = CTreeWnd::NodeByIndex( visibleInd );
			if ( hNode ) {
				EnsureVisible( hNode );

				m_nFirstVisibleInd = visibleInd;

				SendMessage( WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, 0), 0l );
				SendMessage( WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0l );
			}

			hNode = CTreeWnd::NodeByIndex( selectedInd );
			if ( hNode ) 
				SelectItem( hNode );

			::SetFocus( hwndFocused );
		}

//		SetRedraw( IsPossibleRedraw() );
	}
}
  

// ---
void CCheckTree::LoadTreeWindow( int fromIndex, int toIndex, HTREEITEM hParent, HTREEITEM addAfter ) {
	if ( m_Items && m_Items->GetSize() ) {
//		SetRedraw(FALSE);
		
		HWND hwndFocused;
		int selectedInd;
		int visibleInd;

		if ( IsPossibleRedraw() ) {
			hwndFocused = ::GetFocus();

			selectedInd = IndexNode( GetSelectedItem() );
			visibleInd = IndexNode( GetFirstVisibleItem() );
		}

		int parentIndex = IndexNode( hParent );
		parentIndex = parentIndex >= 0 ? parentIndex : 0;

		NewLoadSession();

		CCheckTreeItem *prItem = addAfter ? (*m_Items)[parentIndex] : NULL;
		CCheckTreeItem::CieChecked fHowToChecked = prItem 
																							 ? prItem->GetChecked()
																							 : CCheckTreeItem::ie_Unchecked;
	
		CSArray<HTREEITEM> rcParentStack;
		CSArray<HTREEITEM> rcAfterStack;
		int nPIndex = parentIndex;
		HTREEITEM hAddedItem = hParent;

		for ( int i=fromIndex; i<=toIndex; i++ ) {
			// Добавить в дерево элементы из массива
			CCheckTreeItem *item = (*m_Items)[i];
			CCheckTreeItem *parentItem = (*m_Items)[nPIndex];
			
			if ( item->GetLevel() > parentItem->GetLevel() || !hAddedItem ) {
				rcParentStack.Add( hAddedItem );
				rcAfterStack.Add( addAfter );
//				nPIndex = IndexNode( rcParentStack[rcParentStack.MaxIndex()] );
//				nPIndex = nPIndex >= 0 ? nPIndex : 0;
			}
			else {
				if ( item->GetLevel() < parentItem->GetLevel() ) {
					int nDelta = parentItem->GetLevel() - item->GetLevel();
					for ( ; nDelta>0; nDelta-- ) {
						rcParentStack.RemoveInd( rcParentStack.MaxIndex() );
						rcAfterStack.RemoveInd( rcAfterStack.MaxIndex() );
					}
//					nPIndex = IndexNode( rcParentStack[rcParentStack.MaxIndex()] );
//					nPIndex = nPIndex >= 0 ? nPIndex : 0;
				}
			}

			if ( rcParentStack.Count() && rcAfterStack.Count() ) {
				hAddedItem = InsertItemChild( rcParentStack[rcParentStack.MaxIndex()], 
																			rcAfterStack[rcAfterStack.MaxIndex()], 
																			CTreeItemInfo(item->GetText(), item->GetLevel()) );

				rcAfterStack[rcAfterStack.MaxIndex()] = hAddedItem;
			}
			else {
				hAddedItem = InsertItemChild( hAddedItem, addAfter, CTreeItemInfo(item->GetText(), item->GetLevel()) );
			}

			::SetupInternalInfo( *this, hAddedItem, i, m_Items );

			nPIndex = i;
			
			if ( item->IsSelected() )
				selectedInd = i;

			if ( item->IsFirstVisible() )
				visibleInd = i;
		}

		if ( IsUseCheckSemantic() ) {
			// Установить сстояние корневого узла
			::SetupNodeChildren( this, *m_Items, parentIndex );

			if ( prItem )
				SetItemChecked( parentIndex, fHowToChecked, true );

			// Установить состояние узлов в массиве узлов
			SetupNodes( hParent );

			if ( prItem )
				SetItemChecked( parentIndex, fHowToChecked, true );
		}
		// Установить состояние узлов в дереве
		if ( IsSetupImagesAtLoad() )
			SetupImages( hParent );

		PostLoadProcessing();

		if ( IsPossibleRedraw() ) {
			HTREEITEM hNode = CTreeWnd::NodeByIndex( visibleInd );
			if ( hNode ) {
				EnsureVisible( hNode );

				m_nFirstVisibleInd = visibleInd;

				SendMessage( WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, 0), 0l );
				SendMessage( WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0l );
			}

			hNode = CTreeWnd::NodeByIndex( selectedInd );
			if ( hNode ) 
				SelectItem( hNode );

			::SetFocus( hwndFocused );
		}

//	  SetRedraw( IsPossibleRedraw() );
	}
}
  

// Установить внутреннюю информацию
// ---
void CCheckTree::SetupInternalInfo() {
	CTreeWnd::ForEach( ::SetupInternalInfo, (void *)m_Items, NULL );
}


// Выдать индекс данного узла
// ---
int CCheckTree::IndexNode( HTREEITEM hNode ) {
	int foundInd = -1;
	if ( m_Items && m_Items->GetSize() ) {
		if ( hNode ) {
			CCheckTreeItem *clItem = (CCheckTreeItem *)GetItemData( hNode );
			ASSERT( clItem != NULL );

			foundInd = m_Items->FindIt( clItem );
		}
		else
			return foundInd;
	}
	return foundInd == -1 && hNode ? CTreeWnd::IndexNode( hNode ) : foundInd;
}


// Выдать индекс данного узла
// ---
int CCheckTree::IndexNode( CCheckTreeItem *clItem ) {
	int foundInd = -1;
	if ( m_Items->GetSize() ) {
		foundInd = m_Items->FindIt( clItem );
	}
	return foundInd;
}


// Выдать узел по заданному индексу
// ---
HTREEITEM CCheckTree::NodeByIndex( int ind ) {
	if ( ind >=0 && ind < m_Items->GetSize() ) 
		return (*m_Items)[ind]->GetTreeItem();

	return CTreeWnd::NodeByIndex( ind );
}

// ---
static void ExpandNodes( CTreeWnd &owner, HTREEITEM hNode, int ind, void *par ) {
	CCheckTreeItem *item = (*(CCTItemsArray *)par)[ind];
	if ( item->IsExpanded() != owner.IsExpanded(hNode) )
		owner.Expand( hNode, item->IsExpanded() ? TVE_EXPAND : (TVE_COLLAPSE /*| TVE_COLLAPSERESET*/) );   
}
				

// ---
void CCheckTree::PostLoadProcessing() {
	if ( IsEnablePostLoadProcessing() )
		PostMessage( mAfterLoadExpanding, 0, 0 );
//	ForEach( ::ExpandNodes, (void *)m_Items, -1 );
}

// Установить новый массив элементов
// ---
void CCheckTree::SetItemsArray( CCTItemsArray *itemsArray ) {
	if ( itemsArray ) {

		bool bNeedReload = !m_Items || *m_Items != *itemsArray;

		if ( IsOwnsItemsArray() )
			delete m_Items;

		m_Items = itemsArray;

		if ( bNeedReload )
			LoadTreeWindow();
		else
			SetupInternalInfo();
	}
}


// Установить новый массив дескрипторов, начиная с заданного индекса папы
// ---
void CCheckTree::UpdateItemsArray( int fromIndex, CCTItemsArray &itemsArray, bool bRootInserted ) {
	HTREEITEM hNode = NodeByIndex( fromIndex );

	int lastLevel  = fromIndex >= 0 
									 ? (*m_Items)[fromIndex]->GetLevel() + 1
									 : 1;

	if ( bRootInserted )
	  m_Items->RemoveAt( fromIndex );
	else
	  fromIndex++;

	for ( ; fromIndex < m_Items->GetSize() && (*m_Items)[fromIndex]->GetLevel() >= lastLevel; ) 
	  m_Items->RemoveAt( fromIndex );
	
	m_Items->InsertAt( fromIndex, &itemsArray );

	LoadTreeWindow( hNode, bRootInserted );
}


// Установить новый массив дескрипторов, начиная с заданного индекса до конечного индекса
// ---
void CCheckTree::UpdateItemsArrayByRange( int fromIndex, int toIndex, CCTItemsArray &itemsArray ) {
	HTREEITEM hNodeAfter  = NodeByIndex( fromIndex - 1 );
	HTREEITEM hNodeParent = GetParentItem( NodeByIndex(fromIndex) );

	int selectedInd = IndexNode( GetSelectedItem() );

	if ( selectedInd >=	fromIndex && selectedInd <=toIndex )
		SelectItem( NULL );

	for ( int i=toIndex; i>=fromIndex; i-- ) {
		HTREEITEM hNode = NodeByIndex( i );
		if ( hNode )
			DeleteItem( hNode );
	  m_Items->RemoveAt( i );
	}
	
	m_Items->InsertAt( fromIndex, &itemsArray);
	toIndex = fromIndex + itemsArray.GetSize() - 1;

	LoadTreeWindow( fromIndex, toIndex, hNodeParent, hNodeAfter );
}


// Удалить элемент со всем поддеревом, начиная с заданного индекса 
// ---
void CCheckTree::DeleteItemWithSubtree( int nFromIndex ) {
	SetRedraw( FALSE );

	if ( nFromIndex >= 0 ) {
		HTREEITEM hNode = NodeByIndex( nFromIndex );

		int lastLevel  = (*m_Items)[nFromIndex]->GetLevel() + 1;
		int nDelete = nFromIndex + 1;

		for ( ; nDelete < m_Items->GetSize() && (*m_Items)[nDelete]->GetLevel() >= lastLevel; ) 
			m_Items->RemoveAt( nDelete );
		
		m_Items->RemoveAt( nFromIndex );

		DeleteSubTree( hNode );
		DeleteItem( hNode );

		if ( m_Items->GetSize() <= 1 && HasStyle(TVS_LINESATROOT) ) 
			// Если в дереве больше не узлов или остался один - снять стиль "веточки от корня"
			ModifyStyle( TVS_LINESATROOT, 0 );
	}
	else {
		if ( m_Items )
			m_Items->RemoveAll();
		DeleteAllItems();
		//DeleteSubTree( NULL );
		ModifyStyle( TVS_LINESATROOT, 0 );
	}

	SetRedraw( IsPossibleRedraw() );
}


// Установить текст на элемент
// ---
void CCheckTree::SetTreeItemText( int nItemIndex, const CString &rcText ) {
	if ( m_Items && nItemIndex < m_Items->GetSize() ) {
		(*m_Items)[nItemIndex]->SetText( rcText );
		SetItemText( NodeByIndex(nItemIndex), rcText );
	}
}


// Получиь индекс родительского элемента
// ---
int CCheckTree::GetParentIndex( int nItemIndex ) {
	if ( nItemIndex >= 0 ) {
		int lastLevel  = (*m_Items)[nItemIndex]->GetLevel();
		for ( nItemIndex--; nItemIndex >= 0 && (*m_Items)[nItemIndex]->GetLevel() >= lastLevel; nItemIndex-- );
		return nItemIndex;
/*
		HTREEITEM hParentNode = GetParentItem( (*m_Items)[nItemIndex]->GetTreeItem() );
		if ( hParentNode )
			return IndexNode( hParentNode );
*/
	}
	return -1;
}

// Получиь индекс видимого элемента
// ---
int CCheckTree::GetFirstVisibleIndex() {
	return m_nFirstVisibleInd = IndexNode( GetFirstVisibleItem() );
}


// Получиь индекс следующего братского элемента
// ---
int CCheckTree::GetNextSiblingIndex( int nItemIndex ) {
	if ( m_Items && nItemIndex < m_Items->GetSize() ) {
		int lastLevel  = (*m_Items)[nItemIndex]->GetLevel();
		nItemIndex++;
		for ( ; nItemIndex < m_Items->GetSize() && (*m_Items)[nItemIndex]->GetLevel() > lastLevel; nItemIndex++ );
		return (nItemIndex < m_Items->GetSize() && (*m_Items)[nItemIndex]->GetLevel() == lastLevel)
					 ? nItemIndex 
					 : -1;
	}
	return -1;
}

// Получиь индекс первого дочернего элемента
// ---
int CCheckTree::GetFirstChildIndex( int nItemIndex ) {
	if ( m_Items && nItemIndex < m_Items->GetSize() ) {
		if ( nItemIndex >= 0 ) {
			int lastLevel  = (*m_Items)[nItemIndex]->GetLevel() + 1;
			nItemIndex++;
			return ( nItemIndex < m_Items->GetSize() && (*m_Items)[nItemIndex]->GetLevel() == lastLevel )
							? nItemIndex
							: -1;
		}
		return 0;
	}
	return -1;
}


// Добавить новый элемент по дескриптору, начиная с заданного индекса папы
// ---
int CCheckTree::AddItemToParent( CCheckTreeItem *pNewItem, int parentIndex ) {
	// Запретить перерисовку
//	SetRedraw(FALSE);

	int fromIndex = parentIndex + 1;

	HTREEITEM hParentNode = NodeByIndex( parentIndex );

	if ( parentIndex >= 0 ) {
		// Получить родительский элемент
		CCheckTreeItem *parentItem = (*m_Items)[parentIndex];

		// Установить уровень узла
		pNewItem->SetLevel( parentItem->GetLevel() + 1 );

		if ( !IsLeaf(hParentNode) ) {
			// Найти после кого добавлять
			int lastLevel  = (*m_Items)[fromIndex]->GetLevel();

			for ( ; fromIndex < m_Items->GetSize() && (*m_Items)[fromIndex]->GetLevel() >= lastLevel; fromIndex++ ); 
		}
	}
	else {
		// Установить уровень узла
		pNewItem->SetLevel( 1 );
		// Найти после кого добавлять
		fromIndex = m_Items->GetUpperBound() + 1;
	}

	// Вставить в массив
	if ( fromIndex >= 0 )
		m_Items->InsertAt( fromIndex, pNewItem);
	else
		m_Items->Add( pNewItem );

	// Вставить в дерево

//  HTREEITEM hAddedItem = AddItemChild( hParentNode, CTreeItemInfo(pNewItem->GetText(), pNewItem->GetLevel()) );

	
  NewLoadSession();

	HTREEITEM hAddAfter = NodeByIndex( fromIndex - 1 );

	HTREEITEM hAddedItem = AddItem( pNewItem, hAddAfter );
			
	// Установить данные элемента
	pNewItem->SetTreeItem( hAddedItem );

	// Установить данные узла
	SetItemData( hAddedItem, (DWORD)pNewItem ); 

	// Установить картинки узла
	SetNodeImage( pNewItem, hAddedItem );

	// Установить сстояние корневого узла
  ::SetupNodeChildren( this, *m_Items, parentIndex );

	// Установить состояние узлов в массиве узлов
	SetupNodes( hParentNode );

	// Установить состояние узлов в дереве
	SetupImages( hParentNode );

	// Разрешить перерисовку
//	SetRedraw( m_bPossibleRedraw );

	return IndexNode( hAddedItem );
}


// Вставить новый элемент по дескриптору, начиная с заданного индекса папы перед заданным индексом
// ---
int CCheckTree::InsertItemToParent( CCheckTreeItem *pNewItem, int parentIndex, int indexBefore ) {
	// Запретить перерисовку
//	SetRedraw(FALSE);

	int fromIndex = indexBefore;

	HTREEITEM hParentNode = NodeByIndex( parentIndex );

	if ( parentIndex >= 0 ) {
		// Получить родительский элемент
		CCheckTreeItem *parentItem = (*m_Items)[parentIndex];

		// Установить уровень узла
		pNewItem->SetLevel( parentItem->GetLevel() + 1 );
	}
	else {
		// Установить уровень узла
		pNewItem->SetLevel( 1 );
	}

	// Вставить в массив
	if ( fromIndex >= 0 )
		m_Items->InsertAt( fromIndex, pNewItem);
	else
		m_Items->Add( pNewItem );

	// Вставить в дерево

//  HTREEITEM hAddedItem = AddItemChild( hParentNode, CTreeItemInfo(pNewItem->GetText(), pNewItem->GetLevel()) );

	
  NewLoadSession();

	HTREEITEM hAddAfter = NodeByIndex( fromIndex - 1 );

	HTREEITEM hAddedItem = InsertItemChild( hParentNode, hAddAfter, CTreeItemInfo(pNewItem->GetText(), pNewItem->GetLevel()) );
//	HTREEITEM hAddedItem = AddItem( pNewItem, hAddAfter );
			
	// Установить данные элемента
	pNewItem->SetTreeItem( hAddedItem );

	// Установить данные узла
	SetItemData( hAddedItem, (DWORD)pNewItem ); 

	// Установить картинки узла
	SetNodeImage( pNewItem, hAddedItem );

	// Установить сстояние корневого узла
  ::SetupNodeChildren( this, *m_Items, parentIndex );

	// Установить состояние узлов в массиве узлов
	SetupNodes( hParentNode );

	// Установить состояние узлов в дереве
	SetupImages( hParentNode );

	// Разрешить перерисовку
//	SetRedraw( m_bPossibleRedraw );

	return IndexNode( hAddedItem );
}


// Установить признак Checked на элемент
// ---
void CCheckTree::SetItemChecked( int iItemIndex, 
																 CCheckTreeItem::CieChecked fHowToChecked, 
																 bool bSpreadAutomatically ) {

	CCheckTreeItem *item = (*m_Items)[iItemIndex];
	HTREEITEM hNode = NodeByIndex( iItemIndex );

  if ( !item->IsNode() ) {
    // Если это не узел
    // Переключить его состояние
    InternalSetItemChecked( item, fHowToChecked );

    // Установить картинки
		SetNodeImage( item, hNode );
  }
  else {
//		if ( item->IsMarkable() ) убрать пока
			InternalSetItemChecked( item, fHowToChecked );

		// Установить картинки узла
		SetNodeImage( item, hNode );
 
		if ( bSpreadAutomatically )
			// Установить состояние всех детей
			CheckChildren( hNode, iItemIndex, fHowToChecked, false );
	}

	if ( bSpreadAutomatically )
		// Установить состояние всех родителей
		CheckParents( hNode );
}



// ---
void CCheckTree::InternalSetItemChecked( CCheckTreeItem *item, 
																 CCheckTreeItem::CieChecked fHowToChecked ) {
  item->SetCheck( fHowToChecked );
}


// Установить состояние родилей узла
// ---
void CCheckTree::CheckParents( HTREEITEM hNode ) {
	if ( IsSpreadMark() ) {
		HTREEITEM parentNode = GetParentItem( hNode );
		if ( parentNode ) {
			// Если у узла есть папа

			int parentIndex = IndexNode( parentNode );
			CCheckTreeItem *prItem = (*m_Items)[parentIndex];

			if ( prItem->IsMarkable() ) {

			// Если папа вообще включаемый
			// Подсчитать количество включенных прямых детей папы
				int numItemsChecked     = 0;
				int numItemsSomeChecked = 0;
				int firstChildLevel  = prItem->GetLevel() + 1;
  
				for ( int i=parentIndex+1,c=m_Items->GetSize(); i<c; i++ ) {
					CCheckTreeItem *item = (*m_Items)[i];
					if ( item->GetLevel() == firstChildLevel ) {
						bool bChecked = item->GetChecked() == CCheckTreeItem::ie_Checked && item->IsMarkable();
						numItemsChecked += bChecked;

						bool bSomeChecked = item->GetChecked() == CCheckTreeItem::ie_SomeChecked && item->IsMarkable();
						numItemsSomeChecked += bSomeChecked;
					}
					else  
						if ( item->GetLevel() < firstChildLevel )
							break;
				}

				// Установить количество детей селектированных в узле
				prItem->SetNumItemsChecked( numItemsChecked );
				prItem->SetNumItemsSomeChecked( numItemsSomeChecked );

				CCheckTreeItem::CieChecked fHowToCheck = CCheckTreeItem::CieChecked( ItemCheckingAnalize(prItem) );
				InternalSetItemChecked( prItem, fHowToCheck );
			}

			// Установить картинки
			SetNodeImage( prItem, parentNode );

			// Рекурсивно установить состояние папиного папы
			CheckParents( parentNode );
		}
	}
}


// Установить состояние детей узла
// ---
void CCheckTree::CheckChildren( HTREEITEM hNode, int ind, CCheckTreeItem::CieChecked fHowToChecked, bool bSetCheck ) {
	CCheckTreeItem *prItem = (*m_Items)[ind];

	if ( bSetCheck ) {
		if ( prItem->IsMarkable() && prItem->CanChangeChecked() )
			InternalSetItemChecked( prItem, fHowToChecked );

		// Установить картинки узла
		SetNodeImage( prItem, hNode );
	}
 
	if ( IsSpreadMark() ) {
		if ( prItem->GetNumItems() ) {
			// Если у узла есть дети - пройти по всем его прямым детям
			int firstChildLevel = prItem->GetLevel() + 1;
			int numItemsChecked = 0;
			int numItemsSomeChecked = 0;
    
			for ( int i=ind+1,c=m_Items->GetSize(); i<c; i++ ) {
				CCheckTreeItem *crItem = (*m_Items)[i];
				if ( crItem->GetLevel() == firstChildLevel ) {
					CheckChildren( NodeByIndex(i), i, fHowToChecked );
					bool bChecked = crItem->GetChecked() == CCheckTreeItem::ie_Checked && crItem->IsMarkable();
					numItemsChecked += bChecked;

					bool bSomeChecked = crItem->GetChecked() == CCheckTreeItem::ie_SomeChecked && crItem->IsMarkable();
					numItemsSomeChecked += bSomeChecked;
				}
				else  
					if ( crItem->GetLevel() < firstChildLevel )
						break;
			}

			// Установить количество детей селектированных в узле
			prItem->SetNumItemsChecked( numItemsChecked );
			prItem->SetNumItemsSomeChecked( numItemsSomeChecked );

			CCheckTreeItem::CieChecked fHowToCheck = CCheckTreeItem::CieChecked( ItemCheckingAnalize(prItem) );
			InternalSetItemChecked( prItem, fHowToCheck );

			// Установить картинки узла
			SetNodeImage( prItem, hNode );
		}
	}
}


// ---
int CCheckTree::ItemCheckingAnalize( CCheckTreeItem *prItem ) {
	if ( prItem->GetNumItemsChecked() || prItem->GetNumItemsSomeChecked() ) {
/*
		// Если включенные были
		if ( prItem->GetNumItems() == numItemsChecked ) 
			// Если совпадает с количеством детей - он полностью включен
     	return CCheckTreeItem::ie_Checked;
		else 
			// Иначе - он включен чуть-чуть
     	return CCheckTreeItem::ie_SomeChecked;
*/
   	return CCheckTreeItem::ie_Checked;
	}
	else 
		// Иначе он не включен
   	return CCheckTreeItem::ie_Unchecked;
}


// Включить узел
// ---
void CCheckTree::CheckNode( HTREEITEM hNode ) {
  int ind = IndexNode( hNode );
	CCheckTreeItem *item = (*m_Items)[ind];
	SetItemChecked( ind, CCheckTreeItem::CieChecked(!item->IsChecked()), true );
}


// Сканирование
// ---
void CCheckTree::ForEach( TvActionFunc action, void *param, int indFromNode ) {
	if ( m_Items && indFromNode < m_Items->GetSize() ) {
		int firstChildLevel = indFromNode >= 0 
													? (*m_Items)[indFromNode]->GetLevel() + 1
													: 1; // Уровень первого ребенка

		for ( int i=indFromNode + 1,c=m_Items->GetSize(); i<c; i++ ) {
			CCheckTreeItem *item = (*m_Items)[i];
			if ( item->GetLevel() >= firstChildLevel ) 
				action( *this, item->GetTreeItem(), i, param );
			else  
				break;
		}
	}
/*
	HTREEITEM hItem = indFromNode == -1 ? NULL : NodeByIndex( indFromNode );
	CTreeWnd::ForEach( action, param, hItem );
*/
}


// Поиск
// ---
int CCheckTree::FirstThat( TvCondFunc action, void *param, int indFromNode ) {
	if ( m_Items && indFromNode < m_Items->GetSize() ) {
		int firstChildLevel = indFromNode >= 0 
													? (*m_Items)[indFromNode]->GetLevel() + 1
													: 1; // Уровень первого ребенка

		for ( int i=indFromNode + 1,c=m_Items->GetSize(); i<c; i++ ) {
			CCheckTreeItem *item = (*m_Items)[i];
			if ( item->GetLevel() >= firstChildLevel ) {
				if ( action(*this, item->GetTreeItem(), i, param) ) 
					return i;
			}
			else  
				break;
		}
	}
	return -1;
/*
	HTREEITEM hItem = indFromNode == -1 ? NULL : NodeByIndex( indFromNode );
	return IndexNode( CTreeWnd::FirstThat(action, param, hItem) );
*/
}

// ---
void CCheckTree::SetNodeStyle( CCheckTreeItem *item, DWORD dwStyle ) {
	if ( dwStyle ) {
		HFONT hFont = m_FontCache.Find( dwStyle );
		if ( !hFont ) {
			CFont *pFont = GetFont();
			if ( pFont ) {
				LOGFONT rcLogfont;
				pFont->GetLogFont( &rcLogfont );
				rcLogfont.lfWeight = (dwStyle & CTIS_BOLD) ? FW_BOLD : FW_NORMAL;
				rcLogfont.lfItalic = (dwStyle & CTIS_ITALIC) ? TRUE : FALSE;
				rcLogfont.lfUnderline = (dwStyle & CTIS_UNDERLINE) ? TRUE : FALSE;
				rcLogfont.lfStrikeOut = (dwStyle & CTIS_STRIKEOUT) ? TRUE : FALSE;

				hFont = ::CreateFontIndirect( &rcLogfont );
				m_FontCache.Add( CCTFontCacheItem(hFont, dwStyle) );
			}
		}
		item->SetFont( hFont );
	}
	else 
		item->SetFont( NULL );
}

// ---
void CCheckTree::SetEnablePostLoadProcessing( bool set /*= true*/ ) { 
	fEnablePostLoadProcessing = set;
	PostLoadProcessing();
}


// ---
bool CCheckTree::IsEnablePostLoadProcessing() { 
	return !!fEnablePostLoadProcessing;
} 

#define CTreeCtrl CTreeWnd
BEGIN_MESSAGE_MAP(CCheckTree, CTreeCtrl)
#undef CTreeCtrl
	//{{AFX_MSG_MAP(CCheckTree)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEACTIVATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelChanging)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_REGISTERED_MESSAGE(mAfterLoadExpanding, OnAfterLoadExpanding)
	ON_MESSAGE(WM_SETREDRAW, OnSetRedraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCheckTree message handlers

// ---
int CCheckTree::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
	if (CTreeWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	if ( !::IsDlgClassName(*GetParent()) )
		StartupInitiation();

	return 0;
}


// ---
void CCheckTree::OnDestroy() {
	if ( m_hOldFont )
		::SelectObject( *GetDC(), m_hOldFont );

	m_FontCache.Flush();

	CTreeWnd::OnDestroy();
}

// ---
static void ExpandNodeUnconditionally( CTreeWnd &owner, HTREEITEM hNode, int ind, void *par ) {
	if ( !owner.IsExpanded(hNode) )
		owner.Expand( hNode, TVE_EXPAND );   
}
				

// ---
int CCheckTree::OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message ) {
	CToolTipCtrl *pTool = GetToolTips();
	if ( pTool ) 
		pTool->ShowWindow( SW_HIDE );

	return CTreeWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
}


// ---
void CCheckTree::OnNcLButtonDown( UINT nFlags, CPoint point ) {
	CToolTipCtrl *pTool = GetToolTips();
	if ( pTool ) 
		pTool->ShowWindow( SW_HIDE );

	CTreeWnd::OnNcLButtonDown(nFlags, point);
}


// ---
void CCheckTree::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	HTREEITEM hNode = GetSelectedItem();
	if ( nChar == VK_SPACE ) {
		if ( IsEnableChangeItemsState() )
			CheckNode( hNode );
	}
	else {
		if ( GetUpdateRect(NULL, TRUE) )
			UpdateWindow();

		bool bExpanded = IsExpanded( hNode );
		bool bControl  = !!getKState(VK_CONTROL);
		bool bRight		 = !!getKState(VK_RIGHT);
		bool bLeft 		 = !!getKState(VK_LEFT);
		if ( bControl && (bRight || bLeft) ) {
			BYTE pKeyArray[256];
			::GetKeyboardState( pKeyArray );
			pKeyArray[VK_CONTROL]	= 0;
			::SetKeyboardState( pKeyArray );
		}

    CTreeWnd::OnKeyDown( nChar, nRepCnt, nFlags );

		if ( !bExpanded && bRight && bControl ) {
			SetPossibleRedraw( false );
			SetRedraw( FALSE );

			CTreeWnd::ForEach( ::ExpandNodeUnconditionally, (void *)m_Items, hNode );
			EnsureVisible( hNode );

			SetPossibleRedraw( true );
			SetRedraw( TRUE );
		}
	}
}

// ---
void CCheckTree::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	if ( nChar != VK_SPACE ) 
		CTreeWnd::OnChar( nChar, nRepCnt, nFlags );
}


// ---
void CCheckTree::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	if ( nChar != VK_SPACE )
		CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}


// ---
void CCheckTree::OnLButtonDblClk(UINT nFlags, CPoint point) {
	UINT flags;
  HTREEITEM hNode = HitTest( point, &flags );
	UINT fCheckFlag = IsStateImageType() ? TVHT_ONITEMSTATEICON : TVHT_ONITEMICON;

  if ( !hNode || !(flags & fCheckFlag) ) 	
	  CTreeWnd::OnLButtonDblClk(nFlags, point);
}


// ---
void CCheckTree::OnLButtonDown(UINT nFlags, CPoint point) {
	UINT flags;
  HTREEITEM hNode = HitTest( point, &flags );

	if ( hNode ) {
		UINT fCheckFlag = IsStateImageType() ? TVHT_ONITEMSTATEICON : TVHT_ONITEMICON;

		if ( flags & fCheckFlag ) {
			SelectItem( hNode );
			if ( IsEnableChangeItemsState() )
				CheckNode( hNode );
		}  
		else {
			bool bExpanded = IsExpanded( hNode );

			CTreeWnd::OnLButtonDown(nFlags, point);

			if ( !bExpanded && (flags & TVHT_ONITEMBUTTON)	&& getKState(VK_CONTROL) ) {
				SetPossibleRedraw( false );
				SetRedraw( FALSE );

				CTreeWnd::ForEach( ::ExpandNodeUnconditionally, (void *)m_Items, hNode );
				EnsureVisible( hNode );

				SetPossibleRedraw( true );
				SetRedraw( TRUE );
			}
		}
	}
	else
		CTreeWnd::OnLButtonDown(nFlags, point);
}


// ---
void CCheckTree::OnLButtonUp(UINT nFlags, CPoint point) {
	UINT flags;
  HTREEITEM hNode = HitTest( point, &flags );
	UINT fCheckFlag = IsStateImageType() ? TVHT_ONITEMSTATEICON : TVHT_ONITEMICON;

  if ( !hNode || !(flags & fCheckFlag) ) 	
  	CTreeWnd::OnLButtonUp(nFlags, point);
}


// ---
void CCheckTree::OnItemExpanded( NMHDR* pNMHDR, LRESULT* pResult ) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HTREEITEM hNode = pNMTreeView->itemNew.hItem;
	ASSERT( hNode != NULL);

	CCheckTreeItem *item = (CCheckTreeItem *)GetItemData( hNode );
	item->SetExpanded( pNMTreeView->action == TVE_EXPAND/*IsExpanded(hNode)*/ );
	
	SetNodeImage( item, hNode );
	
	*pResult = 0;
}


// ---
LRESULT CCheckTree::OnAfterLoadExpanding( WPARAM wParam, LPARAM lParam ) {
	ForEach( ::ExpandNodes, (void *)m_Items, -1 );

	if ( IsPossibleRedraw() ) {
		int nFirstInd = IndexNode( GetFirstVisibleItem() );
		if ( nFirstInd != m_nFirstVisibleInd ) {
			HTREEITEM hNode = CTreeWnd::NodeByIndex( m_nFirstVisibleInd );
			if ( hNode ) 
				SelectSetFirstVisible( hNode );
		}
	}

	return TRUE;
}

// ---
void CCheckTree::OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult) {
	if ( !m_bInterfaceLocked ) {
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		
		int nItemIndex = IndexNode( pNMTreeView->itemOld.hItem );
		if ( nItemIndex >= 0 && nItemIndex < m_Items->GetSize() )
			(*m_Items)[nItemIndex]->SetSelected( false );

		*pResult = 0;
	}
	else
		*pResult = TRUE;
}

// ---
void CCheckTree::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) {
	if ( !m_bInterfaceLocked ) {
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

		int nItemIndex = IndexNode( pNMTreeView->itemNew.hItem );
		if ( nItemIndex >= 0 && nItemIndex < m_Items->GetSize() )
			(*m_Items)[nItemIndex]->SetSelected( true );

		*pResult = 0;
	}
	else
		*pResult = TRUE;
}

// ---
BOOL CCheckTree::OnCommand( WPARAM wParam, LPARAM lParam ) {
	if ( !CTreeWnd::OnCommand(wParam, lParam) ) {
		HWND hWndCtrl = (HWND)lParam;
		if ( !lParam ) {
			// menu or accelerator
			GetMessageReceiver()->SendMessage( CTWM_DOCOMMAND, LOWORD(wParam), 0 );
			return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}


// ---
LRESULT CCheckTree::OnSetRedraw( WPARAM wParam, LPARAM lParam ) {
	return CWnd::Default();
}
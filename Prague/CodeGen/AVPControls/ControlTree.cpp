// ControlList.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include "WASCRes.rh"
#include <Stuff\PArray.h>
#include <StuffIO\Utils.h>
#include <Stuff\CPointer.h>
#include <Wascmess.h>
#include <ODMenu.h>     
#include "PropertySetGet.h"
#include "ControlTreeSpinEdit.h"
#include "ControlTreeComboBox.h"
#include "ControlTreeDate.h"
#include "ControlTree.h"
#include "ControlTreeIPEdit.h"
#include "ControlTreePIDEdit.h"
#include "ControlTreeEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HHOOK hHook = NULL;
CPArray<CControlTree>	gpWindows(0, 1, false);


#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcsclen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


// ---
LRESULT CALLBACK HookProc( int code, WPARAM wParam, LPARAM lParam ) {
  if ( code >= 0 ) {
		MSG *lpMsg = (MSG *)lParam;
		if( lpMsg->message == WM_KEYDOWN ) {
			for ( int i=0,c=gpWindows.Count(); i<c; i++ ) {
				if ( gpWindows[i]->PreTranslateEditMessage(lpMsg) ) {
					lpMsg->hwnd = 0;
					lpMsg->message = 0;
					return 0;
				}
			}
		}
  }
  return ::CallNextHookEx( hHook, code, wParam ,lParam );
}

// ---
static void SetMSGHook( CControlTree *pWnd ) {
  if ( !hHook ) 
    hHook = ::SetWindowsHookEx( WH_GETMESSAGE, HookProc, NULL, ::GetCurrentThreadId() );
	gpWindows.Add( pWnd );
}

// ---
static void ReleaseMSGHook( CControlTree *const pWnd ) {
	if ( gpWindows.Count() ) 
		gpWindows.RemoveObj( pWnd );

  if ( hHook && !gpWindows.Count() ) { 
		::UnhookWindowsHookEx( hHook );
		hHook = NULL;
  }  
}

// ---
static TCHAR *DuplicateString( const TCHAR *s ) {
  return s ? _tcscpy( new TCHAR[TCSCLEN(s)+1], s ) : 0;
}

// ---
static TCHAR *DuplicateString( const CString &s ) {
  return s.GetLength() 
				 ? _tcscpy(new TCHAR[s.GetLength() + 1], (LPCTSTR)s)
				 : _tcscpy(new TCHAR[1], _T(""));
}

static const UINT mAfterStartEditing = ::RegisterWindowMessage( _T("AfterStartEditing") ); 
static const UINT mAfterEndEditing   = ::RegisterWindowMessage( _T("AfterEndEditing") ); 
static const UINT mGetEncloseRect    = ::RegisterWindowMessage( _T("GetEncloseRect") ); 
static const UINT mAfterChangeState  = ::RegisterWindowMessage( _T("AfterChangeState") ); 
static const UINT mGetIPAddress			 = ::RegisterWindowMessage( _T("GetIPAddress") ); 
static const UINT mGetPIDValue			 = ::RegisterWindowMessage( _T("GetPIDValue") ); 
static const UINT mChildInvalid			 = ::RegisterWindowMessage( _T("ChildInvalid") ); 


/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
struct EditMessageInfo {
	uint			itemIndex;
	HTREEITEM itemNode;
	CWnd		 *editControl;
};

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// ---
CControlTreeItem::CControlTreeItem() :
	CCheckTreeItem(),
	fCanBeRemoved( false ),										// Может быть удален
	fRadioAlias( false ),											// Синоним RadioButton
	fCheckAlias( false ),											// Синоним CheckButton
	fSimpleEditAlias( false ),								// Синоним Edit
	fUpDownEditAlias( false ), 								// Синоним UpDownEdit
	fDropDownComboAlias( false ),							// Синоним DropDownComboBox
	fDropDownListComboAlias( false ),					// Синоним DropDownListComboBox
	fSimpleStringAlias( false ),							// Синоним String
	fExternalEditingAlias( false ),						// Синоним ExternalEditing
	fExclusiveExternalEditingAlias( false ),	// Синоним ExclusiveExternalEditing
	fVScrollUpAlias( false ),									// Синоним VScrollUp
	fVScrollDownAlias( false ),								// Синоним VScrollDown
	fDateAlias( false ),											// Синоним Date
	fTimeAlias( false ),											// Синоним Time
	fDateTimeAlias( false ),									// Синоним DateTime
	fPasswordAlias( false ),									// Синоним Password
	fIPAddressAlias( false ),									// Синоним IPAddress
	fPIDValueAlias( false ),									// Синоним PIDValue
	fEditableCheckAlias( false )							// Синоним EditableCheckButton
{
	SetImageIndex( ii_Empty );
}

// ---
CControlTreeItem::~CControlTreeItem() {
}

// ---
bool CControlTreeItem::operator == ( const CCheckTreeItem &rItem ) {
	CControlTreeItem &rcItem = (CControlTreeItem &)rItem;
	return CCheckTreeItem::operator == ( rItem )											&&
				fCanBeRemoved							== rcItem.fCanBeRemoved						&&	
				fRadioAlias								== rcItem.fRadioAlias							&&
				fCheckAlias								== rcItem.fCheckAlias							&&
				fSimpleEditAlias					== rcItem.fSimpleEditAlias				&&
				fUpDownEditAlias					== rcItem.fUpDownEditAlias				&&
				fDropDownComboAlias				== rcItem.fDropDownComboAlias			&&
				fDropDownListComboAlias		== rcItem.fDropDownListComboAlias &&
				fSimpleStringAlias				== rcItem.fSimpleStringAlias			&&
				fExternalEditingAlias			== rcItem.fExternalEditingAlias		&&
				fVScrollUpAlias						== rcItem.fVScrollUpAlias					&&	
				fVScrollDownAlias					== rcItem.fVScrollDownAlias				&&
				fDateAlias								== rcItem.fDateAlias							&&
				fTimeAlias								== rcItem.fTimeAlias							&&
				fPasswordAlias						== rcItem.fPasswordAlias					&&
				fIPAddressAlias						== rcItem.fIPAddressAlias;
}


// ---
void CControlTreeItem::Assign( const CCheckTreeItem &rItem ) {
	CControlTreeItem &rcItem = (CControlTreeItem &)rItem;
	
	CCheckTreeItem::Assign( rItem );

	fCanBeRemoved							= rcItem.fCanBeRemoved					;	
	fRadioAlias								= rcItem.fRadioAlias						;
	fCheckAlias								= rcItem.fCheckAlias						;
	fSimpleEditAlias					= rcItem.fSimpleEditAlias				;
	fUpDownEditAlias					= rcItem.fUpDownEditAlias				;
	fDropDownComboAlias				= rcItem.fDropDownComboAlias		;
	fDropDownListComboAlias		= rcItem.fDropDownListComboAlias;
	fSimpleStringAlias				= rcItem.fSimpleStringAlias			;
	fExternalEditingAlias			= rcItem.fExternalEditingAlias	;
	fVScrollUpAlias						= rcItem.fVScrollUpAlias				;	
	fVScrollDownAlias					= rcItem.fVScrollDownAlias			;
	fDateAlias								= rcItem.fDateAlias							;
	fTimeAlias								= rcItem.fTimeAlias							;
	fPasswordAlias						= rcItem.fPasswordAlias					;
	fIPAddressAlias						= rcItem.fIPAddressAlias				;
}


// ---
CCheckTreeItem *CControlTreeItem::Duplicate() {
	CControlTreeItem *newItem = new CControlTreeItem;
	*newItem = *this;
	return newItem;
}


// ---
int CControlTreeItem::GetImageIndex() {
	return int( IsMarkable()
							? CCheckTreeItem::GetImageIndex()
							: IsRadioAlias()
								? IsChecked() 
									? ii_RadioChecked 
									: ii_RadioUnchecked
								: IsCheckAlias()
									? IsChecked() 
										? ii_CheckChecked 
										: ii_CheckUnchecked
									:	IsSimpleStringAlias()
										? IsExpanded()
											? ii_SimpleStringExpanded
											: ii_SimpleStringCollapsed
										: m_ImageIndex
						) + (IsEnabled() ? 0 : ii_LastIndex); 
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// ---
static int GetTextSize( LPCTSTR text, CDC &dc ) {

  return LOWORD(::GetTabbedTextExtent(dc, text, TCSCLEN(text), 1, NULL));
}


/////////////////////////////////////////////////////////////////////////////
// CControlTree

CControlTree::CControlTree() :
	m_nItemBeingEditInd( -1 ),
	m_hDeferredStartEditNode( NULL ),
	m_hDeferredStartExtEditNode( NULL ),
	scrollTimerId( 0 ),
	scrollFirstTime( 0 ),
	scrollSecondTime( 0 ),
	fPreventSetFocus( false ),
	fShowSelectionAlways( false ),
	fEnableAutoScanForEnable( true ),
	fEditNodeStartInProgress( false ),
	fAcceptNotChangedText( true )
{
	SetSpreadMark( false );
}

// ---
CControlTree::~CControlTree() {
}


// ---
void CControlTree::CreateImageList() {
	if ( !m_ImageList.m_hImageList )
		m_ImageList.Create( IDB_WAS_CTRLTREEIMAGE, 20, 1, RGB(255, 255, 255) );
	SetImageList( &m_ImageList, TVSIL_NORMAL );
	TreeView_SetItemHeight( *this, 16 );
}



// ---
void CControlTree::PreSubclassWindow() {
	CCheckTree::PreSubclassWindow();

	if ( ::IsDlgClassName(*GetParent()) ) {
		ModifyStyle( 0, TVS_EDITLABELS );	
		SetShowSelectionAlways( fShowSelectionAlways );
	}
}


// ---
void CControlTree::BeginModalState() {
	::ReleaseMSGHook( this );
//	CCheckTree::BeginModalState();
}


// ---
void CControlTree::EndModalState() {
//	CCheckTree::EndModalState();
	::SetMSGHook( this );
}


// ---
void CControlTree::SetShowSelectionAlways( bool bShowAlways ) {
	fShowSelectionAlways = bShowAlways;

	if ( m_hWnd )
		ModifyStyle( !fShowSelectionAlways ? TVS_SHOWSELALWAYS : 0, 
								  fShowSelectionAlways ? TVS_SHOWSELALWAYS : 0 );	
}


#define WILL_BE_ENABLED(mustBeEnabled)	(mustBeEnabled &&							\
																				(clItem->IsChecked() ||				\
																				 (!clItem->IsMarkable() &&		\
																					!clItem->IsRadioAlias() &&	\
																					!clItem->IsCheckAlias())))	\

// --
void CControlTree::ScanForEnableItems( int firstIndex, bool mustBeEnabled ) {

	if ( firstIndex < m_Items->GetSize() ) {
		int sameLevel = (*m_Items)[firstIndex]->GetLevel();

		for ( int i=firstIndex,c=m_Items->GetSize(); i<c; i++ ) {
			CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[i];
			if ( clItem->GetLevel() == sameLevel ) {

				if ( clItem->IsSimpleStringAlias() && sameLevel == 1 )
					mustBeEnabled = clItem->IsEnabled();

				clItem->SetEnabled( mustBeEnabled && !clItem->IsStrictlyDisabled() );

				if ( IsSetupImagesAtLoad() )
					SetNodeImage( clItem, NULL );

				if ( clItem->GetNumItems() ) {
					bool willBeEnabled = WILL_BE_ENABLED(mustBeEnabled); 

					ScanForEnableItems( i + 1, willBeEnabled );

					if ( clItem->IsStrictlyDisabled() )
						SetItemStrictlyDisabled( i, true, true );
				}
			}
			if ( clItem->GetLevel() < sameLevel ) 
				break;
		}
	}
}

// --
void CControlTree::ScanForStrictDisableItems( int firstIndex, bool bDisable ) {

	int sameLevel = (*m_Items)[firstIndex]->GetLevel();

	for ( int i=firstIndex,c=m_Items->GetSize(); i<c; i++ ) {
		CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[i];
		if ( clItem->GetLevel() == sameLevel ) {

			clItem->SetStrictlyDisabled( bDisable );
			SetItemEnable( i, !bDisable && clItem->IsEnabled(), false );

			if ( clItem->GetNumItems() ) 
				ScanForStrictDisableItems( i + 1, bDisable );
		}
		if ( clItem->GetLevel() < sameLevel ) 
			break;
	}
}

// --
void CControlTree::SetItemEnable( int iItemIndex, bool bEnable, bool bScanDown ) {
//	SetRedraw( FALSE );

	if ( iItemIndex >= 0 ) {
		CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[iItemIndex];

		bool willBeEnabled = bEnable && !clItem->IsStrictlyDisabled();//WILL_BE_ENABLED(bEnable); 

		clItem->SetEnabled( willBeEnabled );

		if ( IsSetupImagesAtLoad() )
		  SetNodeImage( clItem, NULL );

		if ( bScanDown && clItem->GetNumItems() ) {
			ScanForEnableItems( iItemIndex + 1, WILL_BE_ENABLED(bEnable) );
			if ( clItem->IsStrictlyDisabled() )
				SetItemStrictlyDisabled( iItemIndex, true, true );
		}
	}
	else {
		for ( int i=0,c=m_Items->GetSize(); i<c; i++ ) {
			CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[i];
			if ( clItem->GetLevel() == 1 ) {

				bool willBeEnabled = bEnable && !clItem->IsStrictlyDisabled();//WILL_BE_ENABLED(bEnable); 

				clItem->SetEnabled( willBeEnabled );
				if ( IsSetupImagesAtLoad() )
					SetNodeImage( clItem, NULL );

				if ( bScanDown && clItem->GetNumItems() ) {
					willBeEnabled = willBeEnabled && (!clItem->IsCanBeChecked() || clItem->IsChecked());
					ScanForEnableItems( i + 1, willBeEnabled );
					if ( clItem->IsStrictlyDisabled() )
						SetItemStrictlyDisabled( i, true, true );
				}
			}
		}
	}

//	SetupImages(); Это страшно долго - нужно ставить Image на каждый обработанный

//	SetRedraw( TRUE );
}

// ---
void CControlTree::SetItemStrictlyDisabled( int iItemIndex, bool bDisable, bool bScanDown ) {
	if ( iItemIndex >= 0 ) {
		if ( m_Items && iItemIndex < m_Items->GetSize() ) {
			CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[iItemIndex];
			clItem->SetStrictlyDisabled( bDisable );
			SetItemEnable( iItemIndex, !bDisable && clItem->IsEnabled(), false );
			if ( bScanDown && clItem->GetNumItems() ) 
				ScanForStrictDisableItems( iItemIndex + 1, bDisable );
		}
	}
	else {
		for ( int i=0,c=m_Items->GetSize(); i<c; i++ ) {
			CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[i];
			clItem->SetStrictlyDisabled( bDisable );
			SetItemEnable( iItemIndex, !bDisable && clItem->IsEnabled(), false );
		}
	}
}

// ---
void CControlTree::PostLoadProcessing() {
	CCheckTree::PostLoadProcessing();

	if ( IsEnableAutoScanForEnable() ) {
		ScanForEnableItems( 0, true );
		SetupImages();
	}
}

// --
void CControlTree::CheckAndScanForEnableItems( CControlTreeItem *clItem, int nIndex ) {
	if ( IsEnableAutoScanForEnable() && clItem->GetNumItems() && clItem->IsCanBeChecked() ) {
	// Если маркируется и есть дети - установить разрешенность узлов
		SetRedraw( FALSE );

		bool bWillBeEnabled = clItem->IsEnabled();

		int nParentIndex = GetParentIndex( nIndex );
		if ( nParentIndex >= 0 ) {
			clItem = (CControlTreeItem *)(*m_Items)[nParentIndex];
			bWillBeEnabled = WILL_BE_ENABLED( bWillBeEnabled );
		}

		ScanForEnableItems( nIndex, bWillBeEnabled );

		SetupImages();

	  SetRedraw( TRUE );
	}
}


// ---
BOOL CControlTree::PreTranslateMessage( MSG* pMsg ) {
	if ( PreTranslateEditMessage(pMsg) )
		return TRUE;

  return CCheckTree::PreTranslateMessage(pMsg);
}

// ---
BOOL CControlTree::PreTranslateEditMessage( MSG* pMsg ) {
  if( GetSafeHwnd() ) {

    if( (GetEditControl() ||
			   m_nItemBeingEditInd >= 0) &&
				!(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
				(pMsg->wParam == VK_RETURN || 
				 pMsg->wParam == VK_DELETE || 
				 pMsg->wParam == VK_ESCAPE || 
				 (::GetKeyState(VK_CONTROL) & 0x8000)) ) {

			::TranslateMessage( pMsg );
			::DispatchMessage( pMsg );
			return TRUE;                            // DO NOT process further
		}
  }
	return FALSE;
}

// Включить/выключить узел
// ---
void CControlTree::CheckNode( HTREEITEM hNode ) {
	if ( hNode ) {
		int ind = IndexNode( hNode );
		if ( m_Items && ind >= 0 && ind <= m_Items->MaxIndex() ) {
			CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[ind];

			if ( clItem->IsCanBeChecked() ) {
				CTTMessageInfo info;

				info.m_nWindowID = GetDlgCtrlID();
				info.m_nItemIndex = ind;
				info.m_Value.m_bSetRemove = !clItem->IsChecked();

				BOOL bResult = TRUE;

				GetMessageReceiver()->SendMessage( CTTM_CHECKEDCHANGING, WPARAM(&bResult), LPARAM(&info) );

				if ( bResult ) {
					SetItemChecked( ind, CCheckTreeItem::CieChecked(!clItem->IsChecked()), true );
					PostMessage( mAfterChangeState, ind, LPARAM(0) );
				}
			}
		}
	}
}


// ---
int CControlTree::GetIndexRadioInGroup( int nParentInd, int nItemInd ) {
	int firstChildLevel = nParentInd >= 0 ? (*m_Items)[nParentInd]->GetLevel() + 1 : 1;
	int nIndex = 0;
	for ( int i=nParentInd+1,c=m_Items->GetSize(); i<c; i++ ) {
		CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[i];
		if ( clItem->GetLevel() == firstChildLevel ) {
			if ( clItem->IsRadioAlias() ) {
				if ( i == nItemInd )
					break; 
				nIndex++;
			}
		}
		else  
			if ( clItem->GetLevel() < firstChildLevel )
				break;
	}
	return nIndex;
}

#define WM_USER7424 WM_USER+7424

#define CTreeCtrl CCheckTree
BEGIN_MESSAGE_MAP(CControlTree, CTreeCtrl)
#undef CTreeCtrl
	//{{AFX_MSG_MAP(CControlTree)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(mAfterStartEditing, OnAfterStartEditing)
	ON_REGISTERED_MESSAGE(mAfterEndEditing,   OnAfterEndEditing)
	ON_REGISTERED_MESSAGE(mAfterChangeState,  OnAfterChangeState)
	ON_REGISTERED_MESSAGE(mChildInvalid,			OnChildInvalid)
	ON_MESSAGE(WM_USER7424, OnWmUser7424)
	ON_MESSAGE( TVM_SETINDENT, OnSetIndent )
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelChanging)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTree message handlers

// ---
int CControlTree::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CCheckTree::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	::SetMSGHook( this );

	if ( !::IsDlgClassName(*GetParent()) ) {
		ModifyStyle( 0, TVS_EDITLABELS );	
		SetShowSelectionAlways( fShowSelectionAlways );
	}

	return 0;
}


// ---
void CControlTree::CheckRadioSiblings( HTREEITEM hNode ) {
	if ( hNode ) {
		int nItemInd = IndexNode( hNode );
		if ( m_Items && nItemInd >= 0 && nItemInd <= m_Items->MaxIndex() ) {
			int nFirstItemLevel = (*m_Items)[nItemInd]->GetLevel();

			int nParentInd = GetParentIndex( nItemInd );

			int nBIndex = -1;
			int nEIndex = -1;

			for ( nBIndex=nItemInd-1; nBIndex>nParentInd; nBIndex-- ) {
				CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[nBIndex];
				if ( (clItem->GetLevel() == nFirstItemLevel && !clItem->IsRadioAlias()) ||
							clItem->GetLevel() < nFirstItemLevel ) {
					break;
				}
			}

			for ( nEIndex=nItemInd+1; nEIndex<m_Items->GetSize(); nEIndex++ ) {
				CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[nEIndex];
				if ( (clItem->GetLevel() == nFirstItemLevel && !clItem->IsRadioAlias()) ||
							clItem->GetLevel() < nFirstItemLevel ) {
					break;
				}
			}

			for ( int i=nBIndex+1; i<nEIndex; i++ ) {
				CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[i];
				if ( i != nItemInd && clItem->GetLevel() == nFirstItemLevel && clItem->IsChecked() ) {
					clItem->Check( false );
					CheckAndScanForEnableItems( clItem, i );

					// Установить картинки узла
					HTREEITEM hSiblingNode = NodeByIndex( i );
					SetNodeImage( clItem, hSiblingNode );
				}
			}

		/*
			HTREEITEM parentNode = GetParentItem( hNode );

			int parentIndex = parentNode ? IndexNode( parentNode ) : -1;
			int firstChildLevel = parentIndex >= 0 ? (*m_Items)[parentIndex]->GetLevel() + 1 : 1;

			for ( int i=parentIndex+1,c=m_Items->GetSize(); i<c; i++ ) {
				CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[i];
				if ( clItem->GetLevel() == firstChildLevel ) {
					HTREEITEM hSiblingNode = NodeByIndex( i );
					if ( hSiblingNode != hNode ) {
						if ( clItem->IsRadioAlias() ) {
							if ( clItem->IsChecked() ) {
								clItem->Check( false );
								CheckAndScanForEnableItems( clItem );

								// Установить картинки узла
								SetNodeImage( clItem, hSiblingNode );
							}
						}
						else
							break;
					}
				}
				else  
					if ( clItem->GetLevel() < firstChildLevel )
						break;
			}
		*/
		}
	}
}

// ---
void CControlTree::ScrollProcessing( CControlTreeItem *clItem, HTREEITEM hNode, int ind ) {
	// Скроллируемый список - Item с кнопкой - вдавить
	clItem->SetVScrollIsDown( true );

	SetNodeImage( clItem, hNode );

	// Вычислить начальный и конечный индексы скроллируемой группы
	bool bScrollUp = clItem->IsVScrollUpAlias() ? true : false;
	int  nFirstItemLevel = clItem->GetLevel();
	int delta = bScrollUp ? 1 : -1;
	int firstInd = bScrollUp ? ind + 1 : ind - 1;
	int limit = bScrollUp ? m_Items->GetSize() : -1;
	int lastInd;

	for ( lastInd=firstInd; lastInd!=limit; lastInd += delta ) {
		CControlTreeItem *clFItem = (CControlTreeItem *)(*m_Items)[lastInd];
		if ( clFItem->GetLevel() == nFirstItemLevel &&
			   (( bScrollUp && clFItem->IsVScrollDownAlias()) ||
				  (!bScrollUp && clFItem->IsVScrollUpAlias())) ) {
			lastInd -= delta;
			break;
		}
	}

	// Запросить у папы скроллирование
	CTTMessageInfo info;
	info.m_nWindowID = GetDlgCtrlID();
	info.m_nItemIndex			  = ind;
	info.m_Value.m_nScrollGroup = MAKEWPARAM(min(firstInd, lastInd), max(firstInd, lastInd));

	BOOL bResult = FALSE;
	GetMessageReceiver()->SendMessage( bScrollUp ? CTTM_VSCROLLUP : CTTM_VSCROLLDOWN, WPARAM(&bResult), LPARAM(&info) );

	if ( bResult ) {

		limit = m_Items->GetSize();
		firstInd = min(firstInd, lastInd) - 1;
		for ( lastInd=firstInd; lastInd<limit; lastInd++ ) {
			CControlTreeItem *clFItem = (CControlTreeItem *)(*m_Items)[lastInd];
			if ( clFItem->GetLevel() == nFirstItemLevel &&
					 clFItem->IsVScrollDownAlias() ) {
				break;
			}
		}
		
		ind = bScrollUp ? firstInd : lastInd;

		// Если выполнено
		for ( int i=firstInd; i<=lastInd; i++ ) 
			SetItemText( NodeByIndex(i), (*m_Items)[i]->GetText() );

		// Внутри обработки сообщения возможна перезакачка массива 
		// Нужно заново установить Image для узла
		clItem = (CControlTreeItem *)(*m_Items)[ind];
		hNode = NodeByIndex( ind );

		// Переустановить Image на кнопку
		clItem->SetVScrollIsDown( true );

		SetNodeImage( clItem, hNode );

    Select( hNode, TVGN_CARET );

		// Запустить таймер
		if ( !scrollTimerId ) {
			if ( !scrollFirstTime ) {
				scrollFirstTime = 2;
				HKEY hKey;
				if ( ::RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Keyboard"), 0, KEY_READ, &hKey) == ERROR_SUCCESS ) {
					BYTE buff[20];
					DWORD size = sizeof( buff );
					if ( ::RegQueryValueEx( hKey, _T("KeyboardDelay"), 0, NULL, buff, &size) == ERROR_SUCCESS ) {
						::RegCloseKey( hKey );
						scrollFirstTime = _ttoi( (LPCTSTR)buff );
						if ( !scrollFirstTime )
							scrollFirstTime = 2;
					}
				}

				scrollFirstTime = 1000/scrollFirstTime;
			}

			scrollTimerId = SetTimer( (uint)this, scrollFirstTime, NULL );
		}
	}
	else
		// Если скроллирование не выполнено - остановить таймер
		if ( scrollTimerId ) {
			KillTimer( scrollTimerId );
			scrollTimerId = 0;
		}
}

// ---
void CControlTree::OnLButtonDown( UINT nFlags, CPoint point ) {
	fEditNodeStartInProgress = false;

  if( GetFocus() != this ) 
	  SetFocus();

	UINT flags;
  HTREEITEM hNode = HitTest( point, &flags );

  if ( hNode ) {
		int ind = IndexNode( hNode );
		CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[ind];

		if ( (flags & /*TVHT_ONITEM*/TVHT_ONITEMICON) ) {
			
			if ( IsEnabled(clItem) ) {
				// Если разрешен

				bool bChecked = false;
				if ( clItem->IsRadioAlias() && (bChecked = clItem->IsChecked()) ) {
					PostMessage( mAfterChangeState, ind, LPARAM(0) );
					return;
				}
				
				if ( IsEnableChangeItemsState() && hNode == GetSelectedItem() && clItem && clItem->IsCanBeEdited() ) 
					fEditNodeStartInProgress = true;
				
				CCheckTree::OnLButtonDown(nFlags, point);

				if ( IsEnableChangeItemsState() ) {
					if ( clItem->IsRadioAlias() && bChecked != clItem->IsChecked() ) 
							// Если это RadioButton - проверить всех братьев
						CheckRadioSiblings( hNode );

					CheckAndScanForEnableItems( clItem, ind );
				}

				if ( clItem->IsExternalEditingAlias() ||
						 clItem->IsExclusiveExternalEditingAlias() ) {
					// Внешнее редактирование - Item с кнопкой - вдавить
					clItem->SetExternalEditingIsDown( true );
					
					SetNodeImage( clItem, hNode );
					SetCapture();
				}

				if ( clItem->IsVScrollUpAlias() ||
						 clItem->IsVScrollDownAlias() ) {
					ScrollProcessing( clItem, hNode, ind );
				}
			}
			else
				// Item запрещен - не дать папе отработать Check - только селектирование
				CTreeWnd::OnLButtonDown(nFlags, point);
		}
		else {
			if ( IsEnableChangeItemsState() && hNode == GetSelectedItem() && clItem && clItem->IsCanBeEdited() ) 
				fEditNodeStartInProgress = true;
			CCheckTree::OnLButtonDown(nFlags, point);
		}
	}
	else
		CCheckTree::OnLButtonDown(nFlags, point);
}


// ---
void CControlTree::PerformExternalEditing( HTREEITEM hNode ) {
	HTREEITEM hSelectedNode = hNode;//GetSelectedItem();
	CTTMessageInfo info;
	BOOL bRepeate = TRUE;

	fEditNodeStartInProgress = false;

	while ( bRepeate ) {
		info.m_nWindowID					= GetDlgCtrlID();
		info.m_nItemIndex					= IndexNode(hSelectedNode);
		info.m_bRepeate						= FALSE;
		info.m_Value.m_pItemText	= ::DuplicateString( GetItemText(hSelectedNode) );

		BOOL bResult = FALSE;
		GetMessageReceiver()->SendMessage( CTTM_DOEXTERNALEDITING, WPARAM(&bResult), LPARAM(&info) );

		bRepeate = info.m_bRepeate;

		if ( bResult && info.m_Value.m_pItemText ) {
			hSelectedNode = GetSelectedItem();
			if ( hSelectedNode ) {
				/*
				UINT flags;
				hNode = HitTest( point, &flags );
				*/
				hNode = NodeByIndex( info.m_nItemIndex );
				if ( hNode ) {
					CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );

					if ( clItem ) {
						// Если редактирование выполнено - заменить текст
						SetItemText( hNode, info.m_Value.m_pItemText );
						clItem->SetText( info.m_Value.m_pItemText );

						PostMessage( mAfterChangeState, info.m_nItemIndex, LPARAM(0) );
					}
				}
			}
		}

		delete [] info.m_Value.m_pItemText;
	}
}

// ---
void CControlTree::OnLButtonUp( UINT nFlags, CPoint point ) {
	HTREEITEM hSelectedNode = GetSelectedItem();
	if ( !hSelectedNode ) {
		CCheckTree::OnLButtonUp( nFlags, point );
		return;
	}
	else {
		UINT flags;
		HTREEITEM hNode = HitTest( point, &flags );

		if ( m_nItemBeingEditInd >= 0 ) {
			if ( hNode && (flags & TVHT_ONITEMICON) ) {
				int ind = IndexNode( hNode );
				if ( ind == m_nItemBeingEditInd )
					return;
			}
		}

		CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hSelectedNode );

		CCheckTree::OnLButtonUp( nFlags, point );

		if ( clItem ) {
			if ( clItem->IsExternalEditingAlias() ||
					 clItem->IsExclusiveExternalEditingAlias()	) {
				// Внешнее редактирование - Item с кнопкой - поднять

				ReleaseCapture();

				clItem->SetExternalEditingIsDown( false );

				SetNodeImage( clItem, hSelectedNode );

				if ( IsEnableChangeItemsState() ) {
					if ( hNode && hNode == hSelectedNode && (flags & TVHT_ONITEMICON) && IsEnabled(clItem) ) {
						// Если отпускание в зоне кнопки селектированного элемента - запросить редактирование
						if ( m_nItemBeingEditInd >= 0 ) 
							m_hDeferredStartExtEditNode = hNode;
						else
							PerformExternalEditing( hNode );
					}
				}
			}
			else
				if ( clItem->IsVScrollUpAlias() ||
						 clItem->IsVScrollDownAlias() ) {
					// Если скроллируемый список - остановить таймер
					if ( scrollTimerId ) {
						KillTimer( scrollTimerId );
						scrollTimerId = 0;
					}
				
					// Item с кнопкой - поднять
					clItem->SetVScrollIsDown( false );

					SetNodeImage( clItem, hSelectedNode );
			
					// Установить селектированного
					HTREEITEM hSiblingNode = clItem->IsVScrollUpAlias() 
																	 ? GetNextSiblingItem( hSelectedNode )
																	 : GetPrevSiblingItem( hSelectedNode );
					if ( hSiblingNode )
						SelectItem( hSiblingNode );

				}
				else 
					if ( hNode && (flags & TVHT_ONITEMICON) ) {
						// Если отпускание в зоне Image - послать сообщение о начале редактирования
						CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );
						if ( IsEnabled(clItem) && !clItem->IsCheckAlias() && !clItem->IsRadioAlias() ) {
							if ( m_nItemBeingEditInd >= 0 ) 
								m_hDeferredStartEditNode = hNode;
							else
								PostMessage( TVM_EDITLABEL, 0, LPARAM(hNode) );
						}
					}
//			 SetRedraw( TRUE );
		}
	}
}

// ---
void CControlTree::OnLButtonDblClk( UINT nFlags, CPoint point ) {
	UINT flags;
  HTREEITEM hNode = HitTest( point, &flags );

  if ( hNode && (flags & TVHT_ONITEMICON) ) {
		int ind = IndexNode( hNode );
		CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[ind];
		
		if ( clItem->IsVScrollUpAlias() ||
				 clItem->IsVScrollDownAlias() ) {
			SelectItem( hNode );
			
			ScrollProcessing( clItem, hNode, ind );

			CTreeCtrl::OnLButtonDblClk(nFlags, point);
			return;
		}
	}


	CCheckTree::OnLButtonDblClk(nFlags, point);
}

// ---
int CControlTree::OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message ) {

	CToolTipCtrl *pTool = GetToolTips();
	if ( pTool ) {
		pTool->ShowWindow( SW_HIDE );
		UpdateWindow();
	}

	if ( message == WM_LBUTTONDOWN && m_nItemBeingEditInd >= 0 ) {
		CPoint rcPoint;
		::GetCursorPos( &rcPoint );
		ScreenToClient( &rcPoint );

		UINT flags;
		HTREEITEM hNode = HitTest( rcPoint, &flags );

		if ( hNode && (flags & TVHT_ONITEMICON) ) {
			int ind = IndexNode( hNode );
			if ( ind == m_nItemBeingEditInd )
				return MA_NOACTIVATEANDEAT;
		}
	}

	return CCheckTree::OnMouseActivate( pDesktopWnd, nHitTest, message );
}



// ---
void CControlTree::OnRButtonDown( UINT nFlags, CPoint point ) {
	 // do your code here....
	if( GetFocus() != this ) 
		SetFocus();

	HTREEITEM selection = HitTest( point );

	if ( selection ) 
		SelectItem( selection );
  
	// but don't call default.... otherwise WM_CONTEXTMENU is NOT sent
	//CTreeCtrl::OnRButtonDown( nFlags, point );
}

// ---
void CControlTree::OnRButtonUp( UINT nFlags, CPoint point ) {
	if( GetFocus() != this ) 
		SetFocus();
	
	CTreeCtrl::OnRButtonUp( nFlags, point );
}	

// ---
void CControlTree::OnContextMenu( CWnd* pWnd, CPoint point ) {
	// this code won't get called unless the default implementation
	// for CTreeCtrl::OnRButtonDown is NOT called
	if ( pWnd == this && !fEditNodeStartInProgress ) {
		CRect rcClient;
		GetClientRect( &rcClient );
		CPoint rcRPoint( point );
		ScreenToClient( &rcRPoint );
		if ( rcClient.PtInRect(rcRPoint) )
			CreateAndTrackPopupMenu( point );
		else
			CCheckTree::OnContextMenu( pWnd, point );
	}
//	else
		// Сюда попадаем, если это окно само не обрабатывает сообщение
		// Попытка передать ему данное сообщение приводит к рекурсии
//		pWnd->SendMessage( WM_CONTEXTMENU, WPARAM(pWnd->GetSafeHwnd()), MAKELPARAM(point.x, point.y) );
}


// ---
void CControlTree::InsideImageClickImitate( HTREEITEM hItemOnWhich ) {
	CImageList *pImg = GetImageList( TVSIL_NORMAL );		
	IMAGEINFO imageinfo;
	pImg->GetImageInfo( 0, &imageinfo );		

	CRect rect;
	GetItemRect( hItemOnWhich, &rect, TRUE );
	rect.right = rect.left - 2;		
	rect.left -= (imageinfo.rcImage.right + 2);					

	CPoint point = rect.TopLeft(); 
	point.Offset( rect.Width() >> 1, rect.Height() >> 1 );    

	PostMessage( WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y) );
	PostMessage( WM_LBUTTONUP  , 0, MAKELPARAM(point.x, point.y) );
}


// ---
void CControlTree::CreateAndTrackPopupMenu( CPoint& point ) {
  HTREEITEM hSelectedItem = GetSelectedItem();
	if ( hSelectedItem ) {
		CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hSelectedItem );

		if (point.x == -1 && point.y == -1) {
		 //keystroke invocation      
			CRect itemRect;
			GetItemRect( hSelectedItem, &itemRect, TRUE );
			ClientToScreen( itemRect );
    
			point = itemRect.TopLeft(); 
			point.Offset( itemRect.Width() >> 1, itemRect.Height() >> 1 );    
		} 

		CODMenu menu(this);
		menu.CreatePopupMenu();

		CString menuItemName;
		if ( clItem->GetNumItems() ) {
			if ( IsExpanded(hSelectedItem)/*clItem->IsExpanded()*/ ) {
				menuItemName.LoadString( IDS_WAS_COLLAPSENODE );
				menu.AppendMenu( MF_STRING, IDS_WAS_COLLAPSENODE, menuItemName );
			}
			else {
				menuItemName.LoadString( IDS_WAS_EXPANDNODE );
				menu.AppendMenu( MF_STRING, IDS_WAS_EXPANDNODE, menuItemName );
			}
		}

		if ( IsEnableChangeItemsState() ) {
			if ( IsEnabled(clItem) && clItem->IsCanBeChecked() ) {
				if ( clItem->IsChecked() ) {
					if ( !clItem->IsRadioAlias() ) {
						menuItemName.LoadString( IDS_WAS_UNCHECKNODE );
						menu.AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_UNCHECKNODE, menuItemName );
					}
				}
				else {
					menuItemName.LoadString( IDS_WAS_CHECKNODE );
					menu.AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_CHECKNODE, menuItemName );
				}
			}

			if ( IsEnabled(clItem) && clItem->IsCanBeEdited() ) {
				menuItemName.LoadString( IDS_WAS_EDITVALUE );
				menu.AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_EDITVALUE, menuItemName );
			}

			if ( IsEnabled(clItem) && clItem->IsCanBeRemoved() ) {
				menuItemName.LoadString( IDS_WAS_REMOVEVALUE );
				menu.AppendMenu( MF_STRING | (!IsEnableChangeItemsState() ? (MF_DISABLED|MF_GRAYED) : 0), IDS_WAS_REMOVEVALUE, menuItemName );
			}
		}

		if ( IsEnabled(clItem) && clItem->IsVScrollUpAlias() ) {
			menuItemName.LoadString( IDS_WAS_SCROLLUP );
			menu.AppendMenu( MF_STRING, IDS_WAS_SCROLLUP, menuItemName );
		}

		if ( IsEnabled(clItem) && clItem->IsVScrollDownAlias() ) {
			menuItemName.LoadString( IDS_WAS_SCROLLDOWN );
			menu.AppendMenu( MF_STRING, IDS_WAS_SCROLLDOWN, menuItemName );
		}

		CTTMessageInfo info;
		info.m_nWindowID				= GetDlgCtrlID();
		info.m_nItemIndex			  = IndexNode( hSelectedItem );
		info.m_Value.m_hMenu    = (HMENU)menu;

		BOOL bResult = TRUE;

		GetMessageReceiver()->SendMessage( CTTM_PREPROCESSCONTEXTMENU, WPARAM(&bResult), LPARAM(&info) );
						
		if ( bResult && menu.GetMenuItemCount() ) {
			menu.SynchronizeMenu();

			menu.LoadToolbar( IDR_WAS_TOOLBAR );

			bResult = FALSE;
			info.m_Value.m_ToolBarStuff[0] = 0;
			info.m_Value.m_ToolBarStuff[1] = 0;

			GetMessageReceiver()->SendMessage( CTTM_GETMENUTOOLBAR, WPARAM(&bResult), LPARAM(&info) );
			if ( bResult && info.m_Value.m_ToolBarStuff[0] ) {
				HINSTANCE hOldResModule = ::AfxGetResourceHandle();
				if ( info.m_Value.m_ToolBarStuff[1] )
					::AfxSetResourceHandle( HINSTANCE(info.m_Value.m_ToolBarStuff[1]) ); 

				menu.LoadToolbar( info.m_Value.m_ToolBarStuff[0] );

				::AfxSetResourceHandle( hOldResModule ); 
			}

//			GetMessageReceiver()->SendMessage( WM_INITMENUPOPUP, (WPARAM)(HMENU)menu, 0 );

			UINT nSelected = menu.TrackPopupMenu( TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTALIGN|TPM_RIGHTBUTTON,
																								point.x, point.y, this ); 
			menu.DestroyMenu();
			UpdateWindow();

			switch ( nSelected ) 	{
				case IDS_WAS_EXPANDNODE :
				case IDS_WAS_COLLAPSENODE : {
						Expand( hSelectedItem, TVE_TOGGLE );
						NM_TREEVIEW rcNMTreeView;
						rcNMTreeView.itemNew.hItem = hSelectedItem;
						rcNMTreeView.action = nSelected == IDS_WAS_EXPANDNODE ? TVE_EXPAND : TVE_COLLAPSE;
						LRESULT rcResult;
						OnItemExpanded( (NMHDR *)&rcNMTreeView, &rcResult );
				}
						break;
				case IDS_WAS_CHECKNODE   :
				case IDS_WAS_UNCHECKNODE :
				case IDS_WAS_SCROLLUP		 : 
				case IDS_WAS_SCROLLDOWN	 : 
						InsideImageClickImitate( hSelectedItem );
					break;
				case IDS_WAS_EDITVALUE :
						PostMessage( TVM_EDITLABEL, 0, LPARAM(hSelectedItem) );
						break;
				case IDS_WAS_REMOVEVALUE :
						PostMessage( WM_KEYDOWN, (WPARAM)VK_DELETE, 0 );
						PostMessage( WM_KEYUP  , (WPARAM)VK_DELETE, 0 );
						break;
				case 0:
						//user selected nothing
						break;
				default: 
						info.m_Value.m_iCommandId = nSelected;

						bResult = FALSE;

						GetMessageReceiver()->SendMessage( CTTM_DOCONTEXTMENUCOMMAND, WPARAM(&bResult), LPARAM(&info) );
						
						if ( bResult )
							PostMessage( mAfterChangeState, info.m_nItemIndex, LPARAM(0) );
						break;
			}
		}
		else
			menu.DestroyMenu();
	}
}

// ---
void CControlTree::OnTimer( UINT nIDEvent ) {
	// TODO: Add your message handler code here and/or call default
	if ( nIDEvent == (uint)this ) {
		// Если это мой таймер
		// остановить таймер
		KillTimer( scrollTimerId );

		// Определить позицию мыши и отработать скроллирование
		CPoint point;
		::GetCursorPos( &point );
		::MapWindowPoints( NULL, GetSafeHwnd(), &point, 1 );
		SendMessage( WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y) );	
		
		// Переустановить таймер на новое время
		if ( !scrollSecondTime ) {
			scrollSecondTime = 15;
			HKEY hKey;
			if ( ::RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Keyboard"), 0, KEY_READ, &hKey) == ERROR_SUCCESS ) {
				BYTE buff[20];
				DWORD size = sizeof( buff );
				if ( ::RegQueryValueEx( hKey, _T("KeyboardSpeed"), 0, NULL, buff, &size) == ERROR_SUCCESS ) {
					::RegCloseKey( hKey );
					scrollSecondTime = _ttoi( (LPCTSTR)buff );
					if ( !scrollSecondTime )
						scrollSecondTime = 15;
				}
			}

			scrollSecondTime = 1000/scrollSecondTime;
		}

		scrollTimerId = SetTimer( (uint)this, scrollSecondTime, NULL );
	}

	CCheckTree::OnTimer( nIDEvent );
}


// ---
void CControlTree::OnBeginLabelEdit( NMHDR* pNMHDR, LRESULT* pResult ) {
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CToolTipCtrl *pTool = GetToolTips();
	if ( pTool ) 
		pTool->ShowWindow( SW_HIDE );

	SendMessage( WM_CANCELMODE );
	
	fEditNodeStartInProgress = false;
	
	HTREEITEM hNode = pTVDispInfo->item.hItem;
	int ind = hNode ? IndexNode( hNode ) : -1;
	if ( !m_Items || !(ind >= 0 && ind <= m_Items->MaxIndex()) ) {
		*pResult = TRUE;
		return;
	}

	CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[ind];

	if ( clItem->IsExclusiveExternalEditingAlias() ) {
		InsideImageClickImitate( hNode );
		*pResult = TRUE;
		return;
	}

  if ( !clItem->IsCanBeEdited() || !IsEnabled(clItem) || !IsEnableChangeItemsState() ) {
		*pResult = TRUE;
		return;
	}

	CTTMessageInfo info;
	info.m_nWindowID				= GetDlgCtrlID();
	info.m_nItemIndex			  = ind;
	info.m_Value.m_pItemText = ::DuplicateString( clItem->GetText() );

	BOOL bResult = TRUE;

	GetMessageReceiver()->SendMessage( CTTM_BEGINLABELEDIT, WPARAM(&bResult), LPARAM(&info) );

	clItem->SetText( info.m_Value.m_pItemText );
	SetItemText( hNode, info.m_Value.m_pItemText );

	m_rcOldItemText = clItem->GetText();
	
	delete [] info.m_Value.m_pItemText;

	if ( !bResult ) {
		*pResult = TRUE;
		return;
	}

	if ( clItem->IsSimpleEditAlias() ||
			 clItem->IsExternalEditingAlias() ||
			 clItem->IsPasswordAlias() ) {
		// Если простой Edit или внешнее редактирование
		HWND hEditWnd = (HWND)SendMessage(TVM_GETEDITCONTROL, 0, 0);

		//::SendMessage( hEditWnd, EM_LIMITTEXT, _MAX_PATH, 0 ); Default value is 32767 symbols

		if ( clItem->IsPasswordAlias() ) {
			clItem->SetText( _T("") );
			SetItemText( hNode, _T("") );
			::SetWindowText( hEditWnd, _T("") );
			//::SetWindowLong( hEditWnd, GWL_STYLE, ES_PASSWORD );
		  ModifyStyle( hEditWnd, 0, ES_PASSWORD, 0 );
			::SendMessage( hEditWnd, EM_SETPASSWORDCHAR, L'*', 0 );
		}
		else
			::SetWindowText( hEditWnd, clItem->GetText() );
		
		CControlTreeEdit *pEdit = new CControlTreeEdit();
		pEdit->SubclassWindow( hEditWnd );

		m_nItemBeingEditInd = ind;

		*pResult = FALSE;
		return;
	}

	if ( clItem->IsIPAddressAlias() ) {
		// Если IPAddress
		CTTMessageInfo info;
		info.m_nWindowID					 = GetDlgCtrlID();
		info.m_nItemIndex					 = ind;
		info.m_Value.m_aIPValue[0] = 0;
		info.m_Value.m_aIPValue[1] = 0;
		info.m_Value.m_aIPValue[2] = 0;
		info.m_Value.m_aIPValue[3] = 0;

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_LOADIPADDRESS, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			CControlTreeIPCtl *pIPEdit = new CControlTreeIPCtl( hNode, GetMessageReceiver() );

			DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | IPAS_NOVALIDATE;

			CRect nodeRect;
			GetItemRect( hNode, &nodeRect, TRUE );
			/*
			nodeRect.right += nodeRect.Width() < nMaxSize ? nMaxSize - nodeRect.Width() : 0;
			nodeRect.right += ::GetSystemMetrics( SM_CXVSCROLL ) << 1;
			
			int nodeHeight = nodeRect.Height();

			nodeRect.bottom += 6 * nodeHeight;
			*/
			pIPEdit->Create( dwStyle, nodeRect, this, 1 );

			pIPEdit->SetFont( GetFont() );


			pIPEdit->SetAddress( (IPA_ADDR *)info.m_Value.m_aIPValue );

			CRect rcEncloseRect;
			pIPEdit->SendMessage( mGetEncloseRect, 0, LPARAM(&rcEncloseRect) );

			nodeRect.right = nodeRect.left + rcEncloseRect.Width();
			nodeRect.bottom = nodeRect.top + rcEncloseRect.Height();

			pIPEdit->SetWindowPos( 0, 0, 0, nodeRect.Width(), nodeRect.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

			pIPEdit->SetFocus();

			EditMessageInfo *emInfo = new EditMessageInfo;
			emInfo->itemIndex = ind;
			emInfo->itemNode = hNode;
			emInfo->editControl = pIPEdit;

			PostMessage( mAfterStartEditing, 0, LPARAM(emInfo) );

			m_nItemBeingEditInd = ind;
		}

		*pResult = TRUE;

		return;
	}


	if ( clItem->IsUpDownEditAlias() ) {
		// Если UpDownEdit
		CTTMessageInfo info;
		info.m_nWindowID							 = GetDlgCtrlID();
		info.m_nItemIndex							 = ind;
		info.m_Value.m_aUpDownStuff[0] = 0;
		info.m_Value.m_aUpDownStuff[1] = 100;
		info.m_Value.m_aUpDownStuff[2] = 1;

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_LOADUPDOWNCONTEXT, WPARAM(&bResult), LPARAM(&info) );

		HWND hEditWnd = (HWND)SendMessage(TVM_GETEDITCONTROL, 0, 0);
		::SetWindowText( hEditWnd, clItem->GetText() );

		CControlTreeSpinEdit *pEditForSpin = new CControlTreeSpinEdit( info.m_Value.m_aUpDownStuff[0],
																																	 info.m_Value.m_aUpDownStuff[1],
																																	 info.m_Value.m_aUpDownStuff[2] );
		pEditForSpin->SubclassWindow( hEditWnd );

		m_nItemBeingEditInd = ind;

		*pResult = FALSE;

		return;
	}

	if ( clItem->IsDropDownComboAlias() || clItem->IsDropDownListComboAlias() ) {
		// Если редактируемый или нередактируемый ComboBox

		CStringList contList;

		CTTMessageInfo info;
		info.m_nWindowID					 = GetDlgCtrlID();
		info.m_nItemIndex			     = ind;
		info.m_Value.m_pStringList = &contList;

		BOOL bResult = TRUE;

		GetMessageReceiver()->SendMessage( CTTM_LOADCOMBOCONTENTS, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult && (contList.GetCount() || clItem->IsDropDownComboAlias()) ) {
			CControlTreeComboBox *comboBox = new CControlTreeComboBox( hNode, &contList, clItem->GetText() );

			DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
											CBS_AUTOHSCROLL | 
											(clItem->IsDropDownListComboAlias() ? CBS_DROPDOWNLIST : CBS_DROPDOWN);

			CDC *dc = GetDC();
			dc->SelectObject( GetFont() );
			int nMaxSize = 0;

			for( POSITION pos = contList.GetHeadPosition(); pos != NULL; ) {
				 int nCurSize = ::GetTextSize( contList.GetNext(pos), *dc );
				 if ( nCurSize > nMaxSize )
					 nMaxSize	= nCurSize;
			}

			ReleaseDC( dc );

			CRect nodeRect;
			GetItemRect( hNode, &nodeRect, TRUE );

			nodeRect.right += nodeRect.Width() < nMaxSize ? nMaxSize - nodeRect.Width() : 0;
			nodeRect.right += ::GetSystemMetrics( SM_CXVSCROLL ) << 1;
			
			int nodeHeight = nodeRect.Height();

			nodeRect.bottom += 6 * nodeHeight;

			comboBox->Create( dwStyle, nodeRect, this, 1 );

			comboBox->SetItemHeight( -1, nodeHeight);

			EditMessageInfo *emInfo = new EditMessageInfo;
			emInfo->itemIndex = ind;
			emInfo->itemNode = hNode;
			emInfo->editControl = comboBox;

			PostMessage( mAfterStartEditing, 0, LPARAM(emInfo) );

			m_nItemBeingEditInd = ind;

		}
		*pResult = TRUE;

		return;
	}

	if ( clItem->IsDateAlias() ) {
		CTTMessageInfo info;
		info.m_nWindowID					 = GetDlgCtrlID();
		info.m_nItemIndex			     = ind;
		::ZeroMemory( &info.m_Value.m_aDateValue, sizeof(info.m_Value.m_aDateValue) );

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_QUERYDATE, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			CControlTreeDate *pDateCtrl = new CControlTreeDate( hNode );

			CRect nodeRect;
			GetItemRect( hNode, &nodeRect, TRUE );

			int nHight = ::GetStandartComboHeight( *GetFont(), AfxGetApp()->m_hInstance ) + (::GetSystemMetrics( SM_CYBORDER ) << 1);
//			int nExtraHight = nHight - nodeRect.Height();

//			nodeRect.top -= (nExtraHight >> 1);
			nodeRect.bottom = nodeRect.top + nHight;

			DWORD dwStyle =  WS_BORDER | WS_CHILD | WS_VISIBLE/* | DTS_APPCANPARSE*/ | DTS_LONGDATEFORMAT;
			pDateCtrl->Create( dwStyle, nodeRect, this, 1 );

			SYSTEMTIME rcSysTime;
			::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			::CopyMemory( &rcSysTime, info.m_Value.m_aDateValue, sizeof(info.m_Value.m_aDateValue) );

			pDateCtrl->SetTime( &rcSysTime );
			pDateCtrl->SetFocus();

			info.m_Value.m_pDateFormat = NULL;

			BOOL bResult = FALSE;

			GetMessageReceiver()->SendMessage( CTTM_QUERYDATEFORMAT, WPARAM(&bResult), LPARAM(&info) );

			if ( bResult && info.m_Value.m_pDateFormat ) {
				pDateCtrl->SetFormat( info.m_Value.m_pDateFormat );
				delete [] info.m_Value.m_pDateFormat;
			}

			CString rcString;
			pDateCtrl->GetWindowText( rcString );
			CSize nTextSize = pDateCtrl->GetDC()->GetTabbedTextExtent( rcString, 1, NULL );
			nodeRect.right = nodeRect.left + nTextSize.cx;
			nodeRect.right += (::GetSystemMetrics( SM_CXVSCROLL ) << 1) + (::GetSystemMetrics( SM_CXBORDER ) << 1);
			
			pDateCtrl->SetWindowPos( NULL, 0, 0, nodeRect.Width(), nodeRect.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
			pDateCtrl->Invalidate();

			EditMessageInfo *emInfo = new EditMessageInfo;
			emInfo->itemIndex = ind;
			emInfo->itemNode = hNode;
			emInfo->editControl = pDateCtrl;

			PostMessage( mAfterStartEditing, 0, LPARAM(emInfo) );

			m_nItemBeingEditInd = ind;
		}
		*pResult = TRUE;

		return;
	}

	if ( clItem->IsTimeAlias() ) {
		CTTMessageInfo info;
		info.m_nWindowID					 = GetDlgCtrlID();
		info.m_nItemIndex			     = ind;
		::ZeroMemory( &info.m_Value.m_aTimeValue, sizeof(info.m_Value.m_aTimeValue) );

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_QUERYTIME, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			CControlTreeDate *pDateCtrl = new CControlTreeDate( hNode );

			CRect nodeRect;
			GetItemRect( hNode, &nodeRect, TRUE );

			int nHight = ::GetStandartComboHeight( *GetFont(), AfxGetApp()->m_hInstance ) + (::GetSystemMetrics( SM_CYBORDER ) << 1);
//			int nExtraHight = nHight - nodeRect.Height();

//			nodeRect.top -= (nExtraHight >> 1);
			nodeRect.bottom = nodeRect.top + nHight;

			DWORD dwStyle =  WS_BORDER | WS_CHILD | WS_VISIBLE/* | DTS_APPCANPARSE*/ | DTS_TIMEFORMAT | DTS_UPDOWN;
			pDateCtrl->Create( dwStyle, nodeRect, this, 1 );

			SYSTEMTIME rcSysTime;
			CTime::GetCurrentTime().GetAsSystemTime( rcSysTime );
			::CopyMemory( &rcSysTime.wHour, info.m_Value.m_aTimeValue, sizeof(info.m_Value.m_aTimeValue) );

			pDateCtrl->SetTime( &rcSysTime );
			pDateCtrl->SetFocus();

			info.m_Value.m_pTimeFormat = NULL;

			BOOL bResult = FALSE;

			GetMessageReceiver()->SendMessage( CTTM_QUERYTIMEFORMAT, WPARAM(&bResult), LPARAM(&info) );

			if ( bResult && info.m_Value.m_pTimeFormat ) {
				pDateCtrl->SetFormat( info.m_Value.m_pTimeFormat );
				delete [] info.m_Value.m_pTimeFormat;
			}

			CString rcString;
			pDateCtrl->GetWindowText( rcString );
			CSize nTextSize = pDateCtrl->GetDC()->GetTabbedTextExtent( rcString, 1, NULL );
			nodeRect.right = nodeRect.left + nTextSize.cx;
			nodeRect.right += (::GetSystemMetrics( SM_CXVSCROLL ) << 1) + (::GetSystemMetrics( SM_CXBORDER ) << 1);

			pDateCtrl->SetWindowPos( NULL, 0, 0, nodeRect.Width(), nodeRect.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
			pDateCtrl->Invalidate();

			EditMessageInfo *emInfo = new EditMessageInfo;
			emInfo->itemIndex = ind;
			emInfo->itemNode = hNode;
			emInfo->editControl = pDateCtrl;

			PostMessage( mAfterStartEditing, 0, LPARAM(emInfo) );

			m_nItemBeingEditInd = ind;
		}
		*pResult = TRUE;

		return;
	}

	if ( clItem->IsDateTimeAlias() ) {
		CTTMessageInfo info;
		info.m_nWindowID					 = GetDlgCtrlID();
		info.m_nItemIndex			     = ind;
		::ZeroMemory( &info.m_Value.m_aDateTimeValue, sizeof(info.m_Value.m_aDateTimeValue) );

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_QUERYDATETIME, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			CControlTreeDate *pDateCtrl = new CControlTreeDate( hNode );

			CRect nodeRect;
			GetItemRect( hNode, &nodeRect, TRUE );

			int nHight = ::GetStandartComboHeight( *GetFont(), AfxGetApp()->m_hInstance ) + (::GetSystemMetrics( SM_CYBORDER ) << 1);
//			int nExtraHight = nHight - nodeRect.Height();

//			nodeRect.top -= (nExtraHight >> 1);
			nodeRect.bottom = nodeRect.top + nHight;

			DWORD dwStyle =  WS_BORDER | WS_CHILD | WS_VISIBLE/* | DTS_APPCANPARSE*/ | DTS_LONGDATEFORMAT | DTS_TIMEFORMAT;
			pDateCtrl->Create( dwStyle, nodeRect, this, 1 );

			SYSTEMTIME rcSysTime;
			::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			::CopyMemory( &rcSysTime, info.m_Value.m_aDateTimeValue, sizeof(info.m_Value.m_aDateTimeValue) );

			pDateCtrl->SetTime( &rcSysTime );
			pDateCtrl->SetFocus();

			info.m_Value.m_pDateTimeFormat = NULL;

			BOOL bResult = FALSE;

			GetMessageReceiver()->SendMessage( CTTM_QUERYDATETIMEFORMAT, WPARAM(&bResult), LPARAM(&info) );

			if ( bResult && info.m_Value.m_pDateFormat ) {
				pDateCtrl->SetFormat( info.m_Value.m_pDateFormat );
				delete [] info.m_Value.m_pDateFormat;
			}

			CString rcString;
			pDateCtrl->GetWindowText( rcString );
			CSize nTextSize = pDateCtrl->GetDC()->GetTabbedTextExtent( rcString, 1, NULL );
			nodeRect.right = nodeRect.left + nTextSize.cx;
			nodeRect.right += (::GetSystemMetrics( SM_CXVSCROLL ) << 1) + (::GetSystemMetrics( SM_CXBORDER ) << 1);
			
			pDateCtrl->SetWindowPos( NULL, 0, 0, nodeRect.Width(), nodeRect.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
			pDateCtrl->Invalidate();

			EditMessageInfo *emInfo = new EditMessageInfo;
			emInfo->itemIndex = ind;
			emInfo->itemNode = hNode;
			emInfo->editControl = pDateCtrl;

			PostMessage( mAfterStartEditing, 0, LPARAM(emInfo) );

			m_nItemBeingEditInd = ind;
		}
		*pResult = TRUE;

		return;
	}

	if ( clItem->IsPIDValueAlias() ) {
		// Если PIDValue
		CTTMessageInfo info;
		info.m_nWindowID					 = GetDlgCtrlID();
		info.m_nItemIndex					 = ind;
		info.m_Value.m_aPIDValue[0] = 0;
		info.m_Value.m_aPIDValue[1] = 0;
		info.m_Value.m_aPIDValue[2] = 0;
		info.m_Value.m_aPIDValue[3] = 0;

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_LOADPIDVALUE, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			CControlTreePIDCtl *pPIDEdit = new CControlTreePIDCtl( hNode, GetMessageReceiver() );

			DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | PIDS_NOVALIDATE;

			CRect nodeRect;
			GetItemRect( hNode, &nodeRect, TRUE );
			/*
			nodeRect.right += nodeRect.Width() < nMaxSize ? nMaxSize - nodeRect.Width() : 0;
			nodeRect.right += ::GetSystemMetrics( SM_CXVSCROLL ) << 1;
			
			int nodeHeight = nodeRect.Height();

			nodeRect.bottom += 6 * nodeHeight;
			*/
			int nHight = ::GetStandartComboHeight( *GetFont(), AfxGetApp()->m_hInstance ) + (::GetSystemMetrics( SM_CYBORDER ) << 1);
			nodeRect.bottom = nodeRect.top + nHight;

			pPIDEdit->Create( dwStyle, nodeRect, this, 1 );

			pPIDEdit->SetFont( GetFont() );


			pPIDEdit->SetPIDValue( (PID_VALUE *)info.m_Value.m_aPIDValue );

			CRect rcEncloseRect;
			pPIDEdit->SendMessage( mGetEncloseRect, 0, LPARAM(&rcEncloseRect) );

			nodeRect.right = nodeRect.left + rcEncloseRect.Width();
			nodeRect.bottom = nodeRect.top + rcEncloseRect.Height();

			pPIDEdit->SetWindowPos( 0, 0, 0, nodeRect.Width(), nodeRect.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

			pPIDEdit->SetFocus();

			EditMessageInfo *emInfo = new EditMessageInfo;
			emInfo->itemIndex = ind;
			emInfo->itemNode = hNode;
			emInfo->editControl = pPIDEdit;

			PostMessage( mAfterStartEditing, 0, LPARAM(emInfo) );

			m_nItemBeingEditInd = ind;
		}

		*pResult = TRUE;

		return;
	}


	*pResult = TRUE;
}


// ---
void CControlTree::ScrollAlongControl( CWnd *control, CRect &controlEncloseRect, HTREEITEM hNode ) {
	CRect clientRect;
	GetClientRect( &clientRect );

	if ( clientRect.right < controlEncloseRect.right ) {
		int shift = controlEncloseRect.right - clientRect.right;
		SCROLLINFO si;
		memset( &si, 0, sizeof(si) );

		GetScrollInfo( SB_HORZ, &si );

		CDC *dc = GetDC();
	  dc->SelectObject( GetFont() );

		CString text = GetItemText( hNode );

		while (	::GetTextSize(text, *dc ) < controlEncloseRect.Width() )
			text += _T(" ");

		ReleaseDC( dc );

		SetItemText( hNode, text );

		fPreventSetFocus = true;

		SendMessage( WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos + shift), 0l );
		
		SendMessage( WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0l );

		si.nMax = max( si.nMax, controlEncloseRect.right );
		si.nPos = max( si.nPos, shift );
		si.nPage = clientRect.Width();

		SetScrollInfo( SB_HORZ, &si );

		fPreventSetFocus = false;

		control->SetWindowPos( NULL, controlEncloseRect.left-shift, controlEncloseRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	}
}			


// ---
static void DecomposeIPAddress( BYTE *nValue, const TCHAR *pStrValue ) {
	DWORD n1, n2, n3, n4;
	_stscanf( pStrValue, _T("%u.%u.%u.%u"), &n1, &n2, &n3, &n4 );
	*(WORD *)(nValue + 0) = (WORD)n1;
	*(WORD *)(nValue + 1) = (WORD)n2;
	*(WORD *)(nValue + 2) = (WORD)n3;
	*(WORD *)(nValue + 3) = (WORD)n4;
}


// ---
static void DecomposePIDValue( WORD *nValue, const TCHAR *pStrValue ) {
	TCHAR *pCopy = _tcsdup( pStrValue );
	TCHAR *pField = pCopy;
	int i = 0;
	while( pField && i < 4 ) {
		TCHAR *pDot = _tcschr( pField, L',' );
		if ( pDot ) 
			*pDot = L'\0';
		DWORD dwValue;
		if ( i == 2 ) {
			TCHAR  pBuff[100];
			_stscanf( pField, _T("%s"), pBuff );
			dwValue = ::TypeName2TypeID(pBuff);
		}
		else
			_stscanf( pField, _T("%u"), &dwValue );

		*(WORD *)(nValue + i) = (WORD)dwValue;
		if ( !pDot )
			break;
		pField = pDot + 1;
		i++;
	}

	free( pCopy );

/*
	DWORD n1, n2, n3, n4;
	TCHAR  pBuff[100];
	sscanf( pStrValue, "%u,%u,%s,%u", &n1, &n2, pBuff, &n4 );
	n3 = ::TypeName2TypeID(pBuff);
	*(WORD *)(nValue + 0) = (WORD)n1;
	*(WORD *)(nValue + 1) = (WORD)n2;
	*(WORD *)(nValue + 2) = (WORD)n3;
	*(WORD *)(nValue + 3) = (WORD)n4;
*/
}


// ---
void CControlTree::OnEndLabelEdit( NMHDR* pNMHDR, LRESULT* pResult ) {
	*pResult = FALSE;

	TV_DISPINFO *pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	TVITEM      *pTVItem = &pTVDispInfo->item;
	
	HTREEITEM hNode = pTVItem->hItem;
	CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );

  int ind = IndexNode( hNode );

	CTTMessageInfo info;
	info.m_nWindowID = GetDlgCtrlID();
	info.m_nItemIndex = ind;

	if ( pTVDispInfo->item.pszText && (IsAcceptNotChangedText() || m_rcOldItemText != pTVDispInfo->item.pszText) ) {
		UpdateWindow();

		if ( m_nItemBeingEditInd >= 0 ) 
			m_nItemBeingEditInd = -1;

		if ( m_rcOldItemText != pTVDispInfo->item.pszText ) {
			if ( clItem->IsCanBeEdited() ) {

				BOOL bResult = TRUE;

				// Запросить подтверждение на замену текста
				if ( clItem->IsDateAlias() ) {
					HWND hWndChlild = ::GetWindow( *this, GW_CHILD );
					if ( hWndChlild ) {
						SYSTEMTIME rcTime;
						::SendMessage( hWndChlild, DTM_GETSYSTEMTIME, 0, (LPARAM)&rcTime );

						::CopyMemory( info.m_Value.m_aDateValue, &rcTime, sizeof(info.m_Value.m_aDateValue) );

						GetMessageReceiver()->SendMessage( CTTM_ENDDATELABELEDIT, WPARAM(&bResult), LPARAM(&info) );
					}
						
					info.m_Value.m_pItemText = ::DuplicateString( pTVDispInfo->item.pszText );
				}
				else {
					if ( clItem->IsTimeAlias() ) {
						HWND hWndChlild = ::GetWindow( *this, GW_CHILD );
						if ( hWndChlild ) {
							SYSTEMTIME rcTime;
							::SendMessage( hWndChlild, DTM_GETSYSTEMTIME, 0, (LPARAM)&rcTime );

							::CopyMemory( info.m_Value.m_aTimeValue, &rcTime.wHour, sizeof(info.m_Value.m_aTimeValue) );

							GetMessageReceiver()->SendMessage( CTTM_ENDTIMELABELEDIT, WPARAM(&bResult), LPARAM(&info) );
						}
							
						info.m_Value.m_pItemText = ::DuplicateString( pTVDispInfo->item.pszText );
					}
					else {
						if ( clItem->IsDateTimeAlias() ) {
							HWND hWndChlild = ::GetWindow( *this, GW_CHILD );
							if ( hWndChlild ) {
								SYSTEMTIME rcTime;
								::SendMessage( hWndChlild, DTM_GETSYSTEMTIME, 0, (LPARAM)&rcTime );

								::CopyMemory( info.m_Value.m_aDateTimeValue, &rcTime, sizeof(info.m_Value.m_aDateTimeValue) );

								GetMessageReceiver()->SendMessage( CTTM_ENDDATETIMELABELEDIT, WPARAM(&bResult), LPARAM(&info) );
							}
								
							info.m_Value.m_pItemText = ::DuplicateString( pTVDispInfo->item.pszText );
						}
						else {
							if ( clItem->IsIPAddressAlias() ) {
								HWND hWndChlild = ::GetWindow( *this, GW_CHILD );
								if ( hWndChlild ) {
									::DecomposeIPAddress( info.m_Value.m_aIPValue, pTVDispInfo->item.pszText );
									//if ( ::SendMessage(hWndChlild, mGetIPAddress, TRUE, (LPARAM)&info.m_Value.m_aIPValue) )
									GetMessageReceiver()->SendMessage( CTTM_ENDADDRESSLABELEDIT, WPARAM(&bResult), LPARAM(&info) );
								}
									
								info.m_Value.m_pItemText = ::DuplicateString( pTVDispInfo->item.pszText );
							}
							else {
								if ( clItem->IsPIDValueAlias() ) {
									HWND hWndChlild = ::GetWindow( *this, GW_CHILD );
									if ( hWndChlild ) {
										::DecomposePIDValue( info.m_Value.m_aPIDValue, pTVDispInfo->item.pszText );
										GetMessageReceiver()->SendMessage( CTTM_ENDPIDLABELEDIT, WPARAM(&bResult), LPARAM(&info) );
									}
										
									info.m_Value.m_pItemText = ::DuplicateString( pTVDispInfo->item.pszText );
								}
								else {
									info.m_Value.m_pItemText = ::DuplicateString( pTVDispInfo->item.pszText );

									GetMessageReceiver()->SendMessage( CTTM_ENDLABELEDIT, WPARAM(&bResult), LPARAM(&info) );
								}
							}
						}
					}
				}

				if ( bResult ) {
					clItem->SetText( (LPSTR)info.m_Value.m_pItemText );

					PostMessage( mAfterEndEditing, ind, LPARAM(hNode) );

					//PostMessage( mAfterChangeState, ind, LPARAM(0) );

					*pResult = TRUE;
				}
				else {
					m_hDeferredStartEditNode = NULL;
					m_hDeferredStartExtEditNode = NULL;
					clItem->SetText( m_rcOldItemText );
					SetItemText( hNode, m_rcOldItemText );
					PostMessage( TVM_EDITLABEL, 0, LPARAM(hNode) );
				}
				
				delete [] info.m_Value.m_pItemText;
			}
		}
		else {
			clItem->SetText( m_rcOldItemText );
			SetItemText( hNode, m_rcOldItemText );
			PostMessage( mAfterEndEditing, ind, LPARAM(hNode) );
		}
	}
	else {
		if ( m_nItemBeingEditInd >= 0 ) 
			m_nItemBeingEditInd = -1;

		info.m_Value.m_pItemText = ::DuplicateString( m_rcOldItemText );
		clItem->SetText( m_rcOldItemText );
		//		info.m_Value.m_pItemText = NULL;

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_CANCELLABELEDIT, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult ) {
			clItem->SetText( (LPSTR)info.m_Value.m_pItemText );
			SetItemText( hNode, info.m_Value.m_pItemText );
		}
		else
			SetItemText( hNode, clItem->GetText() );
		
		delete [] info.m_Value.m_pItemText;
	}

	InvalidateRect( NULL, true );
}


// ---
void CControlTree::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) {
  // Приходит WM_VSCROLL при работе Spin - нужно его запретить
  if ( !(/*nSBCode == SB_THUMBPOSITION	на все коды !!! && */pScrollBar && IsChild(pScrollBar)) )	{
		if( GetFocus() != this ) 
			 SetFocus();

		CCheckTree::OnVScroll( nSBCode, nPos, pScrollBar );

		int nFirstVisibleInd = IndexNode( GetFirstVisibleItem() );
		if ( nFirstVisibleInd != m_nFirstVisibleInd ) {
			CTTMessageInfo info;

			info.m_nWindowID					= GetDlgCtrlID();
			info.m_nItemIndex					= m_nFirstVisibleInd;
			info.m_Value.m_bSetRemove = false;

			GetMessageReceiver()->SendMessage( CTTM_FIRSTVISIBLECHANGING, WPARAM(0), LPARAM(&info) );

			info.m_nItemIndex					= m_nFirstVisibleInd = nFirstVisibleInd;
			info.m_Value.m_bSetRemove = true;
			GetMessageReceiver()->SendMessage( CTTM_FIRSTVISIBLECHANGING, WPARAM(0), LPARAM(&info) );
		}				
	}
}


// ---
void CControlTree::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) {
  if( !fPreventSetFocus && GetFocus() != this ) 
		SetFocus();
	
	CCheckTree::OnHScroll(nSBCode, nPos, pScrollBar);
}


// ---
void CControlTree::ProcessScrollSelection( UINT nChar ) {
	HTREEITEM hNode = GetSelectedItem();
	if ( hNode ) {
		CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );
		if ( clItem->IsVScrollUpAlias() ) {
			if ( nChar == VK_UP ) {
				HTREEITEM hPrevNode = GetNextItem( hNode, TVGN_PREVIOUSVISIBLE );  //GetPrevSiblingItem( hNode );
				if ( hPrevNode ) {
					SelectItem( hPrevNode );
					ProcessScrollSelection( nChar );
					return;
				}
			}
			HTREEITEM hNextNode = GetNextItem( hNode, TVGN_NEXTVISIBLE );  //GetPrevSiblingItem( hNode );
			if ( hNextNode ) 
				SelectItem( hNextNode );
		}
		if ( clItem->IsVScrollDownAlias() ) {
			if ( nChar == VK_DOWN ) {
				HTREEITEM hNextNode = GetNextItem( hNode, TVGN_NEXTVISIBLE );  //GetPrevSiblingItem( hNode );
				if ( hNextNode ) {
					SelectItem( hNextNode );
					ProcessScrollSelection( nChar );
					return;
				}
			}
			HTREEITEM hPrevNode = GetNextItem( hNode, TVGN_PREVIOUSVISIBLE );  //GetPrevSiblingItem( hNode );
			if ( hPrevNode ) 
				SelectItem( hPrevNode );
		}
	}
}

// ---
void CControlTree::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  if ( (nChar == VK_UP || nChar == VK_DOWN ) && (::GetKeyState(VK_CONTROL) & 0x8000) ) {
		HTREEITEM hNode = GetSelectedItem();
		if ( hNode ) {
			if ( nChar == VK_UP ) {
				HTREEITEM hPrevNode = GetNextItem( hNode, TVGN_PREVIOUSVISIBLE );  //GetPrevSiblingItem( hNode );
				if ( hPrevNode ) {
					CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hPrevNode );
					if ( clItem->IsVScrollUpAlias() ) {
						InsideImageClickImitate( hPrevNode );
						return;
					}
					SelectItem( hPrevNode );
				}
			}
			if ( nChar == VK_DOWN ) {
				HTREEITEM hNextNode = GetNextItem( hNode, TVGN_NEXTVISIBLE );  //GetNextSiblingItem( hNode );
				if ( hNextNode ) {
					CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNextNode );
					if ( clItem->IsVScrollDownAlias() ) {
						InsideImageClickImitate( hNextNode );
						return;
					}
					SelectItem( hNextNode );
				}
			}
		}
	}


	if ( nChar == VK_SPACE ) {
		HTREEITEM hNode = GetSelectedItem();
		if ( hNode ) {
			CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );
			if ( !IsEnabled(clItem) || (clItem->IsRadioAlias() && clItem->IsChecked()) ) 
				return;
		}
	}

	CCheckTree::OnKeyDown( nChar, nRepCnt, nFlags );
 
	if ( nChar == VK_SPACE && IsEnableChangeItemsState() ) {
		HTREEITEM hNode = GetSelectedItem();
		if ( hNode ) {
			int ind = IndexNode( hNode );
			CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );
			if ( clItem->IsRadioAlias() ) 
				// Если это RadioButton - проверить всех братьев
				CheckRadioSiblings( hNode );
			CheckAndScanForEnableItems( clItem, ind );
		}
	}

  if ( (nChar == VK_UP || nChar == VK_DOWN ) && !(::GetKeyState(VK_CONTROL) & 0x8000)) 
		ProcessScrollSelection( nChar );
}

// ---
void CControlTree::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if ( nChar == VK_F2 ) 
		PostMessage( TVM_EDITLABEL, 0, LPARAM(GetSelectedItem()) );

  if ( nChar == VK_DELETE && IsEnableChangeItemsState() ) {
		HTREEITEM hNode = GetSelectedItem();
		if ( hNode ) {
			CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );
			if ( clItem && clItem->IsCanBeRemoved() ) {
				CTTMessageInfo info;
				info.m_nWindowID				= GetDlgCtrlID();
				info.m_nItemIndex			  = IndexNode( hNode );
				info.m_Value.m_pItemText = ::DuplicateString( GetItemText(hNode) );

				BOOL bResult = FALSE;

				GetMessageReceiver()->SendMessage( CTTM_REMOVEITEM, (WPARAM)&bResult, (LPARAM)&info );

				if ( bResult )
					PostMessage( mAfterChangeState, info.m_nItemIndex, LPARAM(0) );

				delete [] info.m_Value.m_pItemText;
			}
		}
	}


	CCheckTree::OnKeyUp(nChar, nRepCnt, nFlags);
}


// ---
void CControlTree::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  CCheckTree::OnChar(nChar, nRepCnt, nFlags);
}

// ---
void CControlTree::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ) {
	LPNMTVCUSTOMDRAW lpNMCustomDraw = (LPNMTVCUSTOMDRAW)pNMHDR;
	switch ( lpNMCustomDraw->nmcd.dwDrawStage ) {
		case CDDS_PREPAINT :
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		case CDDS_ITEMPREPAINT : {
			*pResult = CDRF_DODEFAULT;
			HTREEITEM hNode = (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec;
			if ( hNode ) {
				CControlTreeItem *clItem = (CControlTreeItem *)GetItemData( hNode );
				if ( clItem ) {
					if ( !clItem->IsEnabled() )
						lpNMCustomDraw->clrText = ::GetSysColor( COLOR_GRAYTEXT );
					/*
					if ( clItem->IsVScrollUpAlias() || clItem->IsVScrollDownAlias() )
						//lpNMCustomDraw->nmcd.uItemState = CDIS_DISABLED;
						*pResult = CDRF_SKIPDEFAULT;
					*/
				}
			}
		}
			return;
/*
		case CDDS_PREPAINT :
			*pResult = CDRF_NOTIFYPOSTERASE;
			return;
		case CDDS_ITEMPOSTERASE : {
			CRect clientRect;
			GetClientRect( &clientRect );

			lpNMCustomDraw->nmcd.rc.right = clientRect.right;
			CDC *dc = CDC::FromHandle( lpNMCustomDraw->nmcd.hdc );
			dc->FillRect( &lpNMCustomDraw->nmcd.rc, &CBrush(GetSysColor(COLOR_WINDOW)) );
		}
*/
	}
	*pResult = 0;
}


// ---
LRESULT CControlTree::OnAfterStartEditing( WPARAM wParam, LPARAM lParam ) {
	EditMessageInfo *info = (EditMessageInfo *)lParam;

	CRect controlEncloseRect;
	info->editControl->SendMessage( mGetEncloseRect, 0, LPARAM(&controlEncloseRect) );

	ScrollAlongControl( info->editControl, controlEncloseRect, info->itemNode );

	delete info;

	return 0;
}


// ---
LRESULT CControlTree::OnAfterEndEditing( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo info;
	info.m_nWindowID				= GetDlgCtrlID();
	info.m_nItemIndex			  = LOWORD(wParam);

	CControlTreeItem *clItem = (CControlTreeItem *)(*m_Items)[info.m_nItemIndex];
	if ( clItem ) {
		if ( clItem->IsPasswordAlias() ) {
			CString &rcString = clItem->GetText();
			TCHAR *pBuffer = rcString.GetBuffer( rcString.GetLength() + 1 );
			memset( (void *)pBuffer, L'*', rcString.GetLength() );
			rcString.ReleaseBuffer();
		}
		SetItemText( NodeByIndex(info.m_nItemIndex), clItem->GetText() );

		info.m_Value.m_pItemText = ::DuplicateString( GetItemText(HTREEITEM(lParam)) );

		BOOL bResult = FALSE;

		GetMessageReceiver()->SendMessage( CTTM_POSTLABELEDIT, WPARAM(&bResult), LPARAM(&info) );

		if ( bResult )
			PostMessage( mAfterChangeState, info.m_nItemIndex, LPARAM(0) );

		delete [] info.m_Value.m_pItemText;

		if ( m_hDeferredStartEditNode ) {
			if ( CTreeWnd::IndexNode(m_hDeferredStartEditNode) >= 0 )
				PostMessage( TVM_EDITLABEL, 0, LPARAM(m_hDeferredStartEditNode) );
			m_hDeferredStartEditNode = NULL;
		}

		if ( m_hDeferredStartExtEditNode ) {
			if ( CTreeWnd::IndexNode(m_hDeferredStartExtEditNode) >= 0 )
				PerformExternalEditing( m_hDeferredStartExtEditNode );
			m_hDeferredStartExtEditNode = NULL;
		}
	}
	return 0;
}


// ---
LRESULT CControlTree::OnAfterChangeState( WPARAM wParam, LPARAM lParam ) {
  int ind = int( wParam );
	CTTMessageInfo info;

	info.m_nWindowID = GetDlgCtrlID();
	info.m_nItemIndex = ind;

	BOOL bResult = TRUE;

	GetMessageReceiver()->SendMessage( CTTM_STATECHANGED, WPARAM(&bResult), LPARAM(&info) );

	return 0;
}

// ---
void CControlTree::OnSelChanging( NMHDR* pNMHDR, LRESULT* pResult ) {
	CCheckTree::OnSelChanging( pNMHDR, pResult );
	if ( !*pResult ) {
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		// TODO: Add your control notification handler code here
		
		CTTMessageInfo info;

		info.m_nWindowID					= GetDlgCtrlID();
		info.m_nItemIndex					= IndexNode( pNMTreeView->itemOld.hItem );
		info.m_Value.m_bSetRemove = false;

		GetMessageReceiver()->SendMessage( CTTM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );
					
		*pResult = 0;
	}
}

// ---
void CControlTree::OnSelChanged( NMHDR* pNMHDR, LRESULT* pResult ) {
	CCheckTree::OnSelChanged( pNMHDR, pResult );
	if ( !*pResult ) {

		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		
		CTTMessageInfo info;

		info.m_nWindowID					= GetDlgCtrlID();
		info.m_nItemIndex					= IndexNode( pNMTreeView->itemNew.hItem );
		info.m_Value.m_bSetRemove = true;

		GetMessageReceiver()->SendMessage( CTTM_ITEMSELCHANGING, WPARAM(0), LPARAM(&info) );

		*pResult = 0;
	}
}


// ---
LRESULT CControlTree::OnWmUser7424( WPARAM wParam, LPARAM lParam ) {
	if ( GetWindow(GW_CHILD) )
		return 0;
	return Default();
}


// ---
LRESULT CControlTree::OnSetIndent( WPARAM wParam, LPARAM lParam ) {
	// Сообщение блокируется, т.к. есть Images, расчитанные попиксельно
	return 0;
}

// ---
void CControlTree::OnSize( UINT nType, int cx, int cy ) {
	CCheckTree::OnSize( nType, cx, cy );
	
	// TODO: Add your message handler code here
	CWnd *justFocused = GetFocus();
	if ( !fPreventSetFocus && justFocused && IsChild(justFocused) )	
		SetFocus();
}


// ---
void CControlTree::OnDestroy() {
	::ReleaseMSGHook( this );

	CTTMessageInfo info;

	info.m_nWindowID = GetDlgCtrlID();
	
	GetMessageReceiver()->SendMessage( CTTM_WINDOWSHUTDOWN, WPARAM(0), LPARAM(&info) );

	CCheckTree::OnDestroy();
}


// ---
void CControlTree::OnItemExpanding( NMHDR* pNMHDR, LRESULT* pResult ) {
	SetRedraw( FALSE );

	*pResult = 0;
}

// ---
void CControlTree::OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) {
	CCheckTree::OnItemExpanded( pNMHDR, pResult );

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hNode = pNMTreeView->itemNew.hItem;
	ASSERT( hNode != NULL);
	
	CTTMessageInfo info;

	info.m_nWindowID = GetDlgCtrlID();
	info.m_nItemIndex = IndexNode( hNode );
	info.m_Value.m_bSetRemove = pNMTreeView->action == TVE_EXPAND;
	
	GetMessageReceiver()->SendMessage( CTTM_ITEMEXPANDING, WPARAM(0), LPARAM(&info) );

	SetRedraw( TRUE );

	*pResult = 0;
}



// ---
void CControlTree::OnKillFocus( CWnd* pNewWnd ) {
	CCheckTree::OnKillFocus(pNewWnd);

	CTTMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_Value.m_bSetRemove = FALSE;

	GetMessageReceiver()->SendMessage( CTTM_FOCUSCHANGING, WPARAM(HWND(*pNewWnd)), LPARAM(&info) );
}


// ---
void CControlTree::OnSetFocus( CWnd* pOldWnd ) {
	CTreeCtrl::OnSetFocus(pOldWnd);

	CTTMessageInfo info;

	info.m_nItemIndex = GetSelectedIndex();
	info.m_Value.m_bSetRemove = TRUE;

	GetMessageReceiver()->SendMessage( CTTM_FOCUSCHANGING, WPARAM(HWND(*pOldWnd)), LPARAM(&info) );
}

// ---
LPARAM CControlTree::OnChildInvalid( WPARAM wParam, LPARAM lParam) {
	::PostMessage( (HWND)wParam, mChildInvalid, wParam, lParam );
	return 0;
}

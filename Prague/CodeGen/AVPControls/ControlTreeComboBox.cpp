// ControlListComboBox.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include <Stuff\CPointer.h>
#include "ControlTreeComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT mGetEncloseRect = ::RegisterWindowMessage( _T("GetEncloseRect") ); 

/////////////////////////////////////////////////////////////////////////////
// CControlTreeComboBox
// ---
CControlTreeComboBox::CControlTreeComboBox( HTREEITEM hItem, CStringList *plstItems, CString &pText ) {
	
	editCtrl = NULL;
	m_hItem = hItem;
	m_bESC  = false;
	m_additionXSpace = 0;
	m_startupXSpace = 0;

	m_lstItems.AddTail( plstItems );

	m_nSel = -1;

	if ( !pText.IsEmpty() ) {
		int i = 0;
		bool bFound = false;

		for( POSITION pos = m_lstItems.GetHeadPosition(); pos != NULL; i++ ) {
			if ( m_lstItems.GetNext( pos ) == pText && !bFound ) {
				m_nSel = i;
				bFound = true;
			}
		}

		if ( !bFound ) {
			m_nSel = i;
			m_lstItems.AddTail( pText );
		}
	}
}

// ---
CControlTreeComboBox::~CControlTreeComboBox() {
	delete editCtrl;
}


// ---
void CControlTreeComboBox::GetEncloseRect( CRect &encloseRect ) {
	GetWindowRect( &encloseRect );
	GetParent()->ScreenToClient( &encloseRect );
}

// ---
void CControlTreeComboBox::EndWorkingProcessing() {
	CString str;
	GetWindowText( str );

	// Send Notification to parent of ListView ctrl
	TV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->GetSafeHwnd();
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = TVN_ENDLABELEDIT;

	dispinfo.item.mask = TVIF_TEXT;
	dispinfo.item.hItem = m_hItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );

	PostMessage( WM_CLOSE );
}

#define CComboBox CACComboBox
BEGIN_MESSAGE_MAP(CControlTreeComboBox, CComboBox)
#undef CComboBox
	//{{AFX_MSG_MAP(CControlTreeComboBox)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
	ON_REGISTERED_MESSAGE(mGetEncloseRect, OnGetEncloseRect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreeComboBox message handlers

// ---
int CControlTreeComboBox::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
	if (CACComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	for( POSITION pos = m_lstItems.GetHeadPosition(); pos != NULL; ) {
		AddString( (LPCTSTR) (m_lstItems.GetNext( pos )) );
	}
	
//		comboBox->SetHorizontalExtent( GetColumnWidth( nCol ));

	SetCurSel( m_nSel );
	SetFocus();

	if ( (GetStyle() & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST ){
		CWnd *child = GetWindow( GW_CHILD );
    editCtrl = new CControlTreeComboBoxEdit;
		editCtrl->SubclassWindow( *child );

		CRect editRect;
		editCtrl->GetRect( &editRect ) ;

		CRect rect;
		GetWindowRect( &rect );
		GetParent()->ScreenToClient( &rect );

		m_startupXSpace  = rect.Width();
		m_additionXSpace = m_startupXSpace - editRect.Width(); 
}

	return 0;
}


// ---
void CControlTreeComboBox::OnKillFocus( CWnd* pNewWnd ) {
	CACComboBox::OnKillFocus( pNewWnd );
	
	// TODO: Add your message handler code here
	
	if ( !pNewWnd || !IsChild(pNewWnd) ) 
		EndWorkingProcessing();

}


// ---
void CControlTreeComboBox::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_ESCAPE || nChar == VK_RETURN ) {
		if( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}
	
	CACComboBox::OnChar(nChar, nRepCnt, nFlags);
}

// ---
void CControlTreeComboBox::OnEditUpdate() {
	// TODO: Add your control notification handler code here
	if ( editCtrl ) {
		CWnd *parent = GetParent();

		CRect rect;
		GetWindowRect( &rect );
		parent->ScreenToClient( &rect );

		CRect parentClientRect;
		parent->GetClientRect( &parentClientRect );

		CRect editRect;
		editCtrl->GetRect( &editRect ) ;

		int fullWidth = editRect.Width() + m_additionXSpace;
		rect.right = rect.left + fullWidth;

		if ( parentClientRect.right > rect.right && rect.Width() > m_startupXSpace ) {
			SetWindowPos( NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER );
		}	
	}
}

// ---
void CControlTreeComboBox::OnNcDestroy() {
	CACComboBox::OnNcDestroy();
	
	// TODO: Add your message handler code here
	delete this;	
}


// ---
void CControlTreeComboBox::OnSelChange() {
	// TODO: Add your control notification handler code here
}


// ---
LRESULT CControlTreeComboBox::OnGetEncloseRect( WPARAM wParam, LPARAM lParam ) {
	GetEncloseRect( *(CRect *)lParam );
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CControlTreeComboBoxEdit

CControlTreeComboBoxEdit::CControlTreeComboBoxEdit()
{
}

CControlTreeComboBoxEdit::~CControlTreeComboBoxEdit()
{
}


BEGIN_MESSAGE_MAP(CControlTreeComboBoxEdit, CEdit)
	//{{AFX_MSG_MAP(CControlTreeComboBoxEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_MESSAGE(WM_PASTE,OnPaste)
	ON_MESSAGE(EM_GETRECT, OnEmGetRect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreeComboBoxEdit message handlers
// ---
LRESULT CControlTreeComboBoxEdit::OnPaste( WPARAM wParam, LPARAM lParam ) {
	if ( OpenClipboard() )  {
		HANDLE hMem = ::GetClipboardData( CF_UNICODETEXT );
		
		if ( hMem ) {
			void *pMem  = ::GlobalLock( hMem );
			DWORD dwLength = ::GlobalSize( hMem );
			
			if ( ::IsTextUnicode(pMem, dwLength, NULL) ) {
				int nSize = ::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, (wchar_t *)pMem, -1, NULL, 0, NULL, NULL );
				HANDLE hMBMem = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, nSize );
				char *pMStr = (char *)::GlobalLock( hMBMem );
				::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, (wchar_t *)pMem, -1, pMStr, nSize, NULL, NULL );
				::SetClipboardData( CF_TEXT, hMBMem );
				::GlobalUnlock( hMBMem );
			}
			
			::GlobalUnlock( hMem );
		}
		
		CloseClipboard();
	}
	return Default();
}

// ---
void CControlTreeComboBoxEdit::OnKillFocus( CWnd* pNewWnd ) {
	CEdit::OnKillFocus(pNewWnd);
  
	if ( pNewWnd->GetSafeHwnd() != GetParent()->GetSafeHwnd() ) 
	  ((CControlTreeComboBox *)GetParent())->EndWorkingProcessing();
}


// ---
void CControlTreeComboBoxEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	if( nChar == VK_ESCAPE || nChar == VK_RETURN ) {
		if( nChar == VK_ESCAPE )
			((CControlTreeComboBox *)GetParent())->m_bESC = TRUE;
		GetParent()->GetParent()->SetFocus();
		return;
	}
		
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}


// ---
LRESULT CControlTreeComboBoxEdit::OnEmGetRect( WPARAM wParam, LPARAM lParam ) {
	// Перехватываем EM_GETRECT, т.к. EDIT в режиме COMBO_BOX возвращает всегда
	// один и тот же размер

	((RECT *)lParam)->left = ((RECT *)lParam)->top = 0;
/*
	int nTextLength = GetWindowTextLength() + 1;
	CAPointer<char> pText = new char[nTextLength];

	GetWindowText( pText, nTextLength );

	CSize nTextSize = GetDC()->GetTabbedTextExtent( pText, strlen(pText), 1, NULL );
*/
	CString rcString;
	GetWindowText( rcString );
	CSize nTextSize = GetDC()->GetTabbedTextExtent( rcString, 1, NULL );

	((RECT *)lParam)->right  = nTextSize.cx;
	((RECT *)lParam)->bottom = nTextSize.cy;

	return 0;
}


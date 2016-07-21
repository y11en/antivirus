// ControlListSpinEdit.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include "ControlTreeSpinEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CControlTreeSpinButton window

CControlTreeSpinButton::CControlTreeSpinButton( int nStepValue ) :
		m_nStepValue( nStepValue ) {
}

BEGIN_MESSAGE_MAP(CControlTreeSpinButton, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(CControlTreeSpinButton)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltaPos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ---
void CControlTreeSpinButton::OnDeltaPos( NMHDR* pNMHDR, LRESULT* pResult ) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	pNMUpDown->iDelta = m_nStepValue * (pNMUpDown->iDelta >= 0 ? 1 : -1);

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CControlTreeSpinEdit
// ---
CControlTreeSpinEdit::CControlTreeSpinEdit( int nMinValue, int nMaxValue, int nStepValue ) :
	m_pSpinForEdit( NULL ),
	m_bOnSizeProcessing( false ),
	m_nMinValue( max(UD_MINVAL, nMinValue) ),
	m_nMaxValue( min((DWORD)0x7fffffff/*UD_MAXVAL*/, (DWORD)nMaxValue) ),
	m_nStepValue( nStepValue ) {
}


// ---
CControlTreeSpinEdit::~CControlTreeSpinEdit() {
	delete m_pSpinForEdit;
}


// ---
void CControlTreeSpinEdit::GetEncloseRect( CRect &encloseRect ) {
	GetWindowRect( &encloseRect );
	GetParent()->ScreenToClient( &encloseRect );

	ASSERT(m_pSpinForEdit != NULL);

	CRect spinRect;
	m_pSpinForEdit->GetWindowRect( spinRect );
	GetParent()->ScreenToClient( &spinRect );

	encloseRect.right = spinRect.right;
}

// ---
void CControlTreeSpinEdit::ScrollParent() {
	CWnd *parent = GetParent();

	CRect rect;
	GetWindowRect( &rect );
	parent->ScreenToClient( &rect );

	CRect parentClientRect;
	parent->GetClientRect( &parentClientRect );

	if ( parentClientRect.right < rect.right ) {
		int shift = rect.right - parentClientRect.right;

		parent->ScrollWindow( -shift, 0, NULL );

		SCROLLINFO si;
	/*
	typedef struct tagSCROLLINFO {  // si 
		UINT cbSize; 
		UINT fMask; 
		int  nMin; 
		int  nMax; 
		UINT nPage; 
		int  nPos; 
		int  nTrackPos; 
	}   SCROLLINFO; 
	typedef SCROLLINFO FAR *LPSCROLLINFO; 
	*/ 

		parent->GetScrollInfo( SB_HORZ, &si );

		si.nMax = max( si.nMax, rect.right );
		si.nPos = max( si.nPos, shift );
		si.nPage = parentClientRect.Width();

		parent->SetScrollInfo( SB_HORZ, &si );
	}
}			


static UINT mGetEncloseRect = ::RegisterWindowMessage( _T("GetEncloseRect") ); 

BEGIN_MESSAGE_MAP(CControlTreeSpinEdit, CEdit)
	//{{AFX_MSG_MAP(CControlTreeSpinEdit)
	ON_WM_SIZE()
	ON_WM_NCDESTROY()
	ON_WM_WINDOWPOSCHANGING()
	ON_REGISTERED_MESSAGE(mGetEncloseRect, OnGetEncloseRect)
	ON_MESSAGE(EM_GETRECT, OnEmGetRect)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreeSpinEdit message handlers
// ---
void CControlTreeSpinEdit::OnSize( UINT nType, int cx, int cy ) {

  CEdit::OnSize( nType, cx, cy );
	
	if ( !m_bOnSizeProcessing ) {
		m_bOnSizeProcessing = true;

		SetWindowPos( NULL, 0, 0, cx + ::GetSystemMetrics(SM_CXVSCROLL), cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
		
		if ( !m_pSpinForEdit ) {
			CWnd *parent = GetParent();

			m_pSpinForEdit = new CControlTreeSpinButton( m_nStepValue );
			
			CRect rect;
			GetWindowRect( &rect );
			parent->ScreenToClient( &rect );
			
			rect.right = rect.left + ::GetSystemMetrics( SM_CXVSCROLL );
			
			m_pSpinForEdit->Create( WS_VISIBLE | WS_CHILD | WS_BORDER | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_NOTHOUSANDS, rect, parent, 100 );

			m_pSpinForEdit->SetRange32( m_nMinValue, m_nMaxValue );

//	    ScrollParent();
		}

		m_pSpinForEdit->SetBuddy( this );

		m_bOnSizeProcessing = false;
	}
}


// ---
void CControlTreeSpinEdit::OnNcDestroy() {
	CEdit::OnNcDestroy();
	
	delete this;
}



// ---
void CControlTreeSpinEdit::OnWindowPosChanging( WINDOWPOS FAR* lpwndpos ) {
	if ( !m_bOnSizeProcessing && m_pSpinForEdit ) {
		CRect rect;
		GetWindowRect( &rect );
		GetParent()->ScreenToClient( &rect );

		CRect parentClientRect;
		GetParent()->GetClientRect( &parentClientRect );

		lpwndpos->x = rect.left;

		int rightMargin = lpwndpos->x + lpwndpos->cx + ::GetSystemMetrics(SM_CXVSCROLL);// + (::GetSystemMetrics(SM_CXBORDER) << 1);

		if ( parentClientRect.right < rightMargin ) 
			lpwndpos->cx -= rightMargin - parentClientRect.right;
	}


	CEdit::OnWindowPosChanging(lpwndpos);
}


// ---
LRESULT CControlTreeSpinEdit::OnEmGetRect( WPARAM wParam, LPARAM lParam ) {
	LRESULT res = Default();
/*
	ASSERT(m_pSpinForEdit != NULL);

	CRect spinRect;
	m_pSpinForEdit->GetWindowRect( spinRect );
	GetParent()->ScreenToClient( &spinRect );

	((RECT *)lParam)->right += spinRect.right;
*/
	((RECT *)lParam)->right += ::GetSystemMetrics(SM_CXVSCROLL);

	return res;
}


// ---
LRESULT CControlTreeSpinEdit::OnGetEncloseRect( WPARAM wParam, LPARAM lParam ) {
	GetEncloseRect( *(CRect *)lParam );
	return 0;
}

// ---
void CControlTreeSpinEdit::OnShowWindow( BOOL bShow, UINT nStatus ) {
	CEdit::OnShowWindow( bShow, nStatus );

	if ( m_pSpinForEdit && m_pSpinForEdit->GetSafeHwnd() )
		m_pSpinForEdit->ShowWindow( bShow ? SW_SHOW : SW_HIDE );
}

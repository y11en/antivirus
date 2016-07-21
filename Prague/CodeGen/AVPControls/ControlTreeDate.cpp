// ControlTreeDate.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include "ControlTreeDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT mGetEncloseRect = ::RegisterWindowMessage( _T("GetEncloseRect") ); 


/////////////////////////////////////////////////////////////////////////////
// CControlTreeDate

CControlTreeDate::CControlTreeDate(  HTREEITEM hItem ) :
	m_hItem( hItem ),
	m_bESC( false ) {
}

CControlTreeDate::~CControlTreeDate()	{
}

BEGIN_MESSAGE_MAP(CControlTreeDate, CDateTimeCtrl)
	//{{AFX_MSG_MAP(CControlTreeDate)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	ON_REGISTERED_MESSAGE(mGetEncloseRect, OnGetEncloseRect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreeDate message handlers
// ---
void CControlTreeDate::OnKillFocus( CWnd* pNewWnd ) {
	CDateTimeCtrl::OnKillFocus(pNewWnd);
												
	if ( !pNewWnd || !IsChild(pNewWnd) ) {
		CMonthCalCtrl *pMonthCtrl = GetMonthCalCtrl();
		if ( !pMonthCtrl || pMonthCtrl->GetSafeHwnd() != pNewWnd->GetSafeHwnd() ) {
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
	}
}


// ---
void CControlTreeDate::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if( nChar == VK_ESCAPE || nChar == VK_RETURN ) {
		if( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}
	
	
	CDateTimeCtrl::OnChar(nChar, nRepCnt, nFlags);
}


// ---
void CControlTreeDate::OnNcDestroy() {
	CDateTimeCtrl::OnNcDestroy();
	
	delete this;
}


// ---
LRESULT CControlTreeDate::OnGetEncloseRect( WPARAM wParam, LPARAM lParam ) {
	GetWindowRect( (CRect *)lParam );
	GetParent()->ScreenToClient( (CRect *)lParam );
/*
	CString rcString;
	GetWindowText( rcString );
	CSize nTextSize = GetDC()->GetTabbedTextExtent( rcString, 1, NULL );

	((CRect *)lParam)->right  = nTextSize.cx + ::GetSystemMetrics( SM_CXVSCROLL );
*/
	return 0;
}


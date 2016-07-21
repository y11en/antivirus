////////////////////////////////////////////////////////////////////
//
//  TrackToolTip.cpp
//  Track position tooltip
//  AVP general purposes MFC stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Stuff\PArray.h>
#include <StuffMFC\TrackToolTip.h>

//#define TITLETIP_CLASSNAME _T("ZTitleTip")
#define TITLETIP_MOUSEDELAY (1)



/////////////////////////////////////////////////////////////////////////////
// CTitleTip
static HHOOK							g_hKbrdMsgHook = 0;																				
static CPArray<CTitleTip>	g_pKbrdMsgWnd(0, 1, false);																

// ---
LRESULT CALLBACK KeyboardHookProc( int code, WPARAM wParam, LPARAM lParam ) {		
  if ( code >= 0 ) {										
		AFX_MANAGE_STATE(::AfxGetStaticModuleState());																	      
    	
		for ( int i=0,c=g_pKbrdMsgWnd.Count(); i<c; i++ )
			g_pKbrdMsgWnd[i]->Hide();										
  }																																							
  return ::CallNextHookEx( g_hKbrdMsgHook, code, wParam, lParam );								
}																																								




// ---
CTitleTip::CTitleTip() {
	m_bWrapLongText = FALSE;
	m_bOnShow = FALSE;
	m_nMouseMoveDelay = 0;
	m_dwLastLButtonDown = 0;

/* !!!!!!!!!!!!

	// Register the window class if it has not already been registered.
	WNDCLASS wndcls;	
	HINSTANCE hInst = ::AfxGetInstanceHandle();
	if ( !(::GetClassInfo(hInst, TITLETIP_CLASSNAME, &wndcls)) )	{
		// otherwise we need to register a new class		
		wndcls.style = CS_SAVEBITS ;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;		
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;		
		wndcls.hCursor = ::LoadCursor( hInst, IDC_ARROW );
		wndcls.hbrBackground = (HBRUSH)(COLOR_INFOBK + 1); 
		wndcls.lpszMenuName = NULL;		
		wndcls.lpszClassName = TITLETIP_CLASSNAME;
		if ( !::AfxRegisterClass(&wndcls) )			
			::AfxThrowResourceException();
	}
*/
}


// ---
CTitleTip::~CTitleTip() {
	if ( g_pKbrdMsgWnd.Count() )																										
		g_pKbrdMsgWnd.RemoveObj( this );																							

	if ( !g_pKbrdMsgWnd.Count() && g_hKbrdMsgHook ) {
		::UnhookWindowsHookEx( g_hKbrdMsgHook );																			
		g_hKbrdMsgHook = NULL;
	}
}

// ---
BOOL CTitleTip::Create( CWnd * pParentWnd ) {
	g_pKbrdMsgWnd.Add( this );																											
	if ( !g_hKbrdMsgHook )																													
		g_hKbrdMsgHook = ::SetWindowsHookEx( WH_KEYBOARD, KeyboardHookProc, NULL,	::GetCurrentThreadId() );								

	m_pParentWnd = pParentWnd;

		// NEW
	if ( CToolTipCtrl::Create(pParentWnd) ) {
		SetDelayTime( TTDT_AUTOMATIC, -1 );

		CRect rectDisplay;
		GetMargin( &rectDisplay );
		rectDisplay.bottom++;
		SetMargin(	rectDisplay );

		return TRUE;
	}

	return FALSE;

/* !!!!!!!!!!!!
	ASSERT_VALID(pParentWnd);	
	DWORD dwStyle = WS_BORDER | WS_POPUP; 
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;	

	return CWnd::CreateEx( dwExStyle, TITLETIP_CLASSNAME, NULL, dwStyle, 0, 0, 0, 0, NULL, NULL, NULL );
*/
}


// ---
BOOL CTitleTip::ProcessMessage(MSG* pMsg) {	
#if 1

	if ( GetSafeHwnd() ) {
		if ( ::GetFocus() == NULL )	{		
			Hide();
			return TRUE;	
		}

		if ( m_bOnShow ) {
			// Used to qualify WM_LBUTTONDOWN messages as double-clicks    
			DWORD dwTick = 0;
			BOOL  bDoubleClick = FALSE;	
			CWnd *pWnd;	
			int hittest;	
			switch ( pMsg->message ) {	
				case WM_LBUTTONDOWN:        
					// Get tick count since last LButtonDown
					dwTick = ::GetTickCount();
					bDoubleClick = ((dwTick - m_dwLastLButtonDown) <= ::GetDoubleClickTime() );
					m_dwLastLButtonDown = dwTick;
					// NOTE: DO NOT ADD break; STATEMENT HERE! Let code fall through
				case WM_RBUTTONDOWN:	
				case WM_MBUTTONDOWN: {
					POINTS pts = MAKEPOINTS( pMsg->lParam );		
					POINT  point;		
					point.x = pts.x;
					point.y = pts.y;		
					ClientToScreen( &point );		
					pWnd = WindowFromPoint( point );

					if( !pWnd || pWnd == this ) 			
						pWnd = m_pParentWnd;

					hittest = (int)pWnd->SendMessage( WM_NCHITTEST, 0, MAKELONG(point.x,point.y) );		
					if ( hittest == HTCLIENT ) {
						pWnd->ScreenToClient( &point );			
						pMsg->lParam = MAKELONG(point.x,point.y);
					}
					else {			
						switch ( pMsg->message ) {
							case WM_LBUTTONDOWN:
								pMsg->message = WM_NCLBUTTONDOWN;
								break;
							case WM_RBUTTONDOWN: 				
								pMsg->message = WM_NCRBUTTONDOWN;				
								break;
							case WM_MBUTTONDOWN: 				
								pMsg->message = WM_NCMBUTTONDOWN;				
								break;			
						}
						pMsg->wParam = hittest;			
						pMsg->lParam = MAKELONG(point.x,point.y);		
					}
					Hide();
					// If this is the 2nd WM_LBUTTONDOWN in x milliseconds,
					// post a WM_LBUTTONDBLCLK message instead of a single click.
					pWnd->PostMessage( bDoubleClick ? WM_LBUTTONDBLCLK : pMsg->message, pMsg->wParam, pMsg->lParam );		
					return TRUE;        
				}
				case WM_KEYDOWN:	
				case WM_SYSKEYDOWN:		
					Hide();
					// Dont't do it. The keyboard input focus still on the m_pParentWnd
					//m_pParentWnd->PostMessage( pMsg->message, pMsg->wParam, pMsg->lParam );
					return TRUE;	
			}	
		}
	}

#endif

	return FALSE;//CWnd::PreTranslateMessage(pMsg);
}

// ---
BOOL CTitleTip::PreTranslateMessage(MSG* pMsg) {	
	// NEW
	RelayEvent( pMsg );

	return ProcessMessage( pMsg );
}


// Show 		- Show the titletip if needed
// rectTitle		- The rectangle within which the original 
//								title is constrained - in client coordinates
// lpszTitleText	- The text to be displayed
// xoffset		- Number of pixel that the text is offset from
//							left border of the cell
// ---
void CTitleTip::Show( CRect rectTitle, LPCTSTR lpszTitleText, int xoffset /*=0*/ ) {
	if ( GetSafeHwnd() && !rectTitle.IsRectEmpty() ) {

		// If titletip is already displayed, don't do anything.
		if( m_bOnShow )
			return;

		if ( m_nMouseMoveDelay ) {
			m_nMouseMoveDelay--;
			return;
		}

		// Do not display the titletip is app does not have focus
		if( ::GetFocus() == NULL )
			return;

		// Define the rectangle outside which the titletip will be hidden.
		// We add a buffer of one pixel around the rectangle	
		m_rectTitle.top = -1;
		m_rectTitle.left = -(xoffset + 1);	
		m_rectTitle.right = rectTitle.Width() + 1;
		m_rectTitle.bottom = rectTitle.Height() + 1;	

		// Determine the width of the text
		m_pParentWnd->ClientToScreen( rectTitle );	

		CClientDC dc(this);
		CString strTitle( lpszTitleText );

		CFont *pFont = m_pParentWnd->GetFont(); 
		// use same font as ctrl
		CFont *pFontDC = dc.SelectObject( pFont );	
		CRect rectDisplay = rectTitle;

		CSize size = dc.GetTextExtent( strTitle );	
		CSize offset = dc.GetTextExtent( _T(" ") );			

		rectDisplay.left += xoffset;
		rectDisplay.right = rectDisplay.left + size.cx;// + 3;

		// Do not display if the text fits within available space
		if ( rectDisplay.right <= rectTitle.right - xoffset / 2 )
			return;
		

		rectDisplay.left -= offset.cx + 2;// << 1;
		rectDisplay.right += offset.cx;// << 1;

		// Show the titletip	

		// NEW
		m_TitleText = lpszTitleText;
		//AddTool( m_pParentWnd, m_TitleText, rectDisplay, 1 );

		TOOLINFO ti;
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd   = *m_pParentWnd;    
		ti.uId    = (UINT)1;
    ti.hinst  = 0;    
		ti.lpszText  = LPTSTR(LPCTSTR(m_TitleText));
    ti.rect = rectDisplay;     

    SendMessage( TTM_ADDTOOL, 0, (LPARAM)&ti );
    SendMessage( TTM_TRACKPOSITION, 0, (LPARAM)MAKELPARAM(rectDisplay.left,rectDisplay.top) );		
		SendMessage( TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&ti );
		
		if ( m_bWrapLongText ) {
			int iScreenWidth = ::GetSystemMetrics( SM_CXSCREEN );

			SendMessage( TTM_SETMAXTIPWIDTH, 0, (LPARAM)(iScreenWidth - rectDisplay.left) );
		}
		
		SetCapture();

		SetTimer( 100, GetDelayTime(TTDT_AUTOPOP), NULL );

		Activate( TRUE );

/*	!!!!!!!!!!!!
		SetWindowPos( &wndTop, rectDisplay.left, rectDisplay.top, 
									rectDisplay.Width(), rectDisplay.Height(), 
									SWP_SHOWWINDOW|SWP_NOACTIVATE );	

		dc.SetBkMode( TRANSPARENT );
		offset.cy = (rectDisplay.Height() - size.cy);
		offset.cy = (offset.cy / 2) - 1;//!!(offset.cy % 2);
		dc.TextOut( offset.cx - 1, offset.cy, strTitle );

		dc.SelectObject( pFontDC );	
		SetCapture();
*/
		m_bOnShow = TRUE;
	}
}


// ---
void CTitleTip::Hide() {
	if ( GetSafeHwnd() && m_bOnShow ) {
		// NEW
		ReleaseCapture();
		m_bOnShow = FALSE;
		Pop();
		
		TOOLINFO ti;
    ti.cbSize = sizeof(TOOLINFO);
    ti.hwnd   = *m_pParentWnd;    
		ti.uId    = (UINT)1;
		SendMessage( TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&ti );
		
		//Activate( FALSE );
		DelTool( m_pParentWnd, 1 );

		m_nMouseMoveDelay = TITLETIP_MOUSEDELAY;
		
		ShowWindow( SW_HIDE );		  
	}
}

BEGIN_MESSAGE_MAP(CTitleTip, CWnd)
	//{{AFX_MSG_MAP(CTitleTip)	
	ON_WM_MOUSEMOVE()	
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// ---
void CTitleTip::OnMouseMove( UINT nFlags, CPoint rcPoint ) {
	if ( !m_rectTitle.PtInRect(rcPoint) ) {
		Hide();

		ClientToScreen( &rcPoint );
		CWnd *pWnd = WindowFromPoint( rcPoint );

		if ( pWnd == this )
			pWnd = m_pParentWnd;

		int hittest = (int)pWnd->SendMessage( WM_NCHITTEST, 0, MAKELONG(rcPoint.x,rcPoint.y) );

		if ( hittest == HTCLIENT ) {
			pWnd->ScreenToClient( &rcPoint );
			pWnd->PostMessage( WM_MOUSEMOVE, nFlags, MAKELONG(rcPoint.x,rcPoint.y) );	
		} 
		else {
			pWnd->PostMessage( WM_NCMOUSEMOVE, hittest, MAKELONG(rcPoint.x,rcPoint.y) );
		}
	}
	CWnd::OnMouseMove( nFlags, rcPoint );
}

// --- 
LRESULT CTitleTip::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) {
	MSG rcMsg;

	memset( &rcMsg, 0, sizeof(rcMsg) );
	rcMsg.hwnd = *this;
	rcMsg.message = message;
	rcMsg.wParam = wParam;
	rcMsg.lParam = lParam;
	ProcessMessage( &rcMsg );
	
	return CWnd::WindowProc( message, wParam, lParam );
}


// ---
void CTitleTip::OnTimer( UINT nIDEvent ) {
	if ( nIDEvent == 100 ) {
		KillTimer( nIDEvent );
		Hide();
	}
}






////////////////////////////////////////////////////////////////////
//
//  TODialog.cpp
//  Message box with timeout implementation
//  AVP general purposess stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2002
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <Stuff\TODialog.h>

#define TIME_TO_APPEAR		  1000

static BOOL g_bUserIsHere = FALSE;

typedef struct {
	HWND    m_hDialogWnd;
  HANDLE  m_hStopEvent;
	HANDLE  m_hThread;
  UINT	  m_uElapseTime;
  UINT	  m_uBlinkingTime;
	BOOL    m_bStopElapseByUserActivity;
} trMyData;

// ---
static LRESULT CALLBACK MyWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  if ( (WM_KEYFIRST <= uMsg && uMsg <= WM_KEYLAST) || 
			 (WM_MOUSEFIRST <= uMsg && uMsg <= WM_MOUSELAST) ||
			 (WM_NCMOUSEMOVE <= uMsg && uMsg <= WM_NCMBUTTONDBLCLK) )
	  g_bUserIsHere = TRUE;

  return ::CallWindowProc( (WNDPROC)::GetWindowLong(hwnd, GWL_USERDATA), hwnd, uMsg, wParam, lParam );
}

// ---
static BOOL CALLBACK EnumChildProc( HWND hwnd, LPARAM lParam ) {
  if ( lParam )
    ::SetWindowLong( hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (long)MyWindowProc) );
  else
    ::SetWindowLong( hwnd, GWL_WNDPROC, ::GetWindowLong(hwnd, GWL_USERDATA) );
  return TRUE;
}

// ---
static unsigned int _stdcall TimeoutMsgBox( void *pParam ) {
  trMyData *ppMyData = (trMyData *)pParam;
  UINT uTime;
	UINT uBlinkingTime = ppMyData->m_uBlinkingTime;
  UINT uElapseTime;
	UINT uCaretBlinkTime;
  DWORD dWaitRetVal;
  
  if ( ppMyData->m_uElapseTime <= TIME_TO_APPEAR ) 
		return 0;

  ppMyData->m_uElapseTime -= TIME_TO_APPEAR;
  
  //Give time for MessageBox to appear
  //::Sleep( TIME_TO_APPEAR );
  dWaitRetVal = ::WaitForSingleObject( ppMyData->m_hStopEvent, TIME_TO_APPEAR );
	if ( dWaitRetVal != WAIT_TIMEOUT )
		return 0;

	::SetWindowLong( ppMyData->m_hDialogWnd, GWL_USERDATA, ::SetWindowLong(ppMyData->m_hDialogWnd, GWL_WNDPROC, (long)MyWindowProc) );
  ::EnumChildWindows( ppMyData->m_hDialogWnd, EnumChildProc, TRUE );

  if ( uBlinkingTime > ppMyData->m_uElapseTime )
		uBlinkingTime = ppMyData->m_uElapseTime;

_Again: 
  uElapseTime = ppMyData->m_uElapseTime - uBlinkingTime;
  dWaitRetVal = ::WaitForSingleObject( ppMyData->m_hStopEvent, uElapseTime );
  if ( dWaitRetVal == WAIT_TIMEOUT ) {
		if ( g_bUserIsHere ) {
			if ( ppMyData->m_bStopElapseByUserActivity ) {
				dWaitRetVal = WAIT_OBJECT_0;
				goto _Stop;
			}
			g_bUserIsHere = FALSE;
			goto _Again;
		}
		if ( (int)uBlinkingTime > 0 ) {
			::SetForegroundWindow( ppMyData->m_hDialogWnd );
			uCaretBlinkTime = ::GetCaretBlinkTime();
			uTime = uBlinkingTime;
			while ( (int)uTime > 0 ) {
				::FlashWindow( ppMyData->m_hDialogWnd, TRUE );
				dWaitRetVal = ::WaitForSingleObject( ppMyData->m_hStopEvent, uCaretBlinkTime );
				if ( dWaitRetVal != WAIT_TIMEOUT )
					break;
				if ( g_bUserIsHere ) {
					if ( ppMyData->m_bStopElapseByUserActivity ) {
						dWaitRetVal = WAIT_OBJECT_0;
						goto _Stop;
					}
					g_bUserIsHere = FALSE;
					::SendMessage( ppMyData->m_hDialogWnd, WM_NCACTIVATE, (WPARAM)(GetForegroundWindow() == ppMyData->m_hDialogWnd), 0 );
					goto _Again;
				}
				uTime-= uCaretBlinkTime;
			}
		}
  }

_Stop:
	if ( dWaitRetVal == WAIT_TIMEOUT && ::IsWindow(ppMyData->m_hDialogWnd) ) {
		::SetWindowLong( ppMyData->m_hDialogWnd, GWL_WNDPROC, ::GetWindowLong(ppMyData->m_hDialogWnd, GWL_USERDATA) );
		::EnumChildWindows( ppMyData->m_hDialogWnd, EnumChildProc, FALSE );
		::SendMessage( ppMyData->m_hDialogWnd, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), LPARAM(::GetDlgItem(ppMyData->m_hDialogWnd, IDOK)) );
	}

  return 0;
}

// ---
DWORD DialogWithTimeoutStart( HWND hWnd, UINT uElapseTime/*=30000*/, UINT uBlinkingTime/*=5000*/, 
														  BOOL bStopWaitingByUserActivity/*=TRUE*/ ) {

	if ( uElapseTime != INFINITE ) {
		trMyData *pMyData = new trMyData;
		pMyData->m_hDialogWnd				=	hWnd;
		pMyData->m_hStopEvent				= ::CreateEvent( NULL, TRUE, FALSE, NULL );
		pMyData->m_uElapseTime				= uElapseTime;
		pMyData->m_uBlinkingTime			= uBlinkingTime;
		pMyData->m_bStopElapseByUserActivity = bStopWaitingByUserActivity;
  
		unsigned int uThreadID;
		pMyData->m_hThread = (HANDLE)::_beginthreadex( NULL, 0, TimeoutMsgBox,	(void*)pMyData, 0, &uThreadID );

		return (DWORD)pMyData;
	}
	else
		return NULL;
}


// ---
void DialogWithTimeoutStop( DWORD dwCookie ) {
	if ( dwCookie ) {
		trMyData *pMyData = (trMyData *)dwCookie;
		if ( pMyData->m_hThread ) {
			::SetEvent( pMyData->m_hStopEvent );
			::WaitForSingleObject( pMyData->m_hThread, INFINITE );
			::CloseHandle( pMyData->m_hStopEvent );
			::CloseHandle( pMyData->m_hThread );
		}
		delete pMyData;
	}
}



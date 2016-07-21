////////////////////////////////////////////////////////////////////
//
//  TOMsgBox.cpp
//  Message box with timeout implementation
//  AVP general purposess stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2002
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <Stuff\TOMsgBox.h>

static UINT nUnloadJustNow	= ::RegisterWindowMessage( _T("TOMessageBoxUnload") );

#define TIME_TO_APPEAR		  1000

static HWND g_hwndMsgBox = NULL;
static BOOL g_bUserIsHere = FALSE;

typedef struct {
  DWORD	  m_dwCurrentThreadID;
  HANDLE  m_hStopEvent;
  UINT	  m_uElapseTime;
  UINT	  m_uBlinkingTime;
	BOOL    m_bStopElapseByUserActivity;
	int     m_iResByElapsed;
} trMyData;

/*
static CHAR *User32 = _T("User32.dll");
static int (WINAPI *pMessageBoxTimeout)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwTimeout) = 0;

#ifdef UNICODE
#define MessageBoxTimeout  _T("MessageBoxTimeoutW")
#else
#define MessageBoxTimeout  _T("MessageBoxTimeoutA")
#endif // !UNICODE


// ---
static BOOL GetMessageBox() {
	if ( !pMessageBoxTimeout ) {
		HMODULE hModUser32 = ::GetModuleHandle( User32 );
		if ( !hModUser32 )
			return FALSE;
		if (!(pMessageBoxTimeout=(int (WINAPI *)(HWND,LPCTSTR,LPCTSTR,UINT,WORD,DWORD))::GetProcAddress(hModUser32,MessageBoxTimeout)) )
			return FALSE;
	}
	return TRUE;
}
*/

// ---
static BOOL CALLBACK FindMsgBox( HWND hwnd, LPARAM lParam ) {
  TCHAR pClassNameBuf[256];
  BOOL bRetVal = TRUE;

  int iRet = ::GetClassName( hwnd, pClassNameBuf, sizeof(pClassNameBuf) );
  if ( !_tcscmp(pClassNameBuf, _T("#32770")) ) {
		g_hwndMsgBox = hwnd;
		bRetVal = FALSE;
  }

  return bRetVal;
}

// ---
static LRESULT CALLBACK MyWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  if ( (WM_KEYFIRST <= uMsg && uMsg <= WM_KEYLAST) || 
			 (WM_MOUSEFIRST <= uMsg && uMsg <= WM_MOUSELAST) ||
			 (WM_NCMOUSEMOVE <= uMsg && uMsg <= WM_NCMBUTTONDBLCLK) )
		::InterlockedExchange( (PLONG)&g_bUserIsHere, TRUE );

  __try {
		WNDPROC lpPrevWndFunc = (WNDPROC)::GetWindowLong( hwnd, GWL_USERDATA );
		if ( lpPrevWndFunc ) {
			if ( uMsg == nUnloadJustNow )
				::EndDialog( g_hwndMsgBox, wParam );
			else
				return ::CallWindowProc( lpPrevWndFunc, hwnd, uMsg, wParam, lParam );
		}
		else
			return 0;
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
	}
	return 0;
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
  trMyData *prMyData = (trMyData *)pParam;
  UINT uTime;
	UINT uBlinkingTime = prMyData->m_uBlinkingTime;
  UINT uElapseTime;
	UINT uCaretBlinkTime;
  DWORD dWaitRetVal;
  
  if ( prMyData->m_uElapseTime <= TIME_TO_APPEAR ) 
		return 0;
	
  prMyData->m_uElapseTime -= TIME_TO_APPEAR;
  
  //Give time for MessageBox to appear
  //::Sleep( TIME_TO_APPEAR );
  dWaitRetVal = ::WaitForSingleObject( prMyData->m_hStopEvent, TIME_TO_APPEAR );
	if ( dWaitRetVal != WAIT_TIMEOUT )
		return 0;
	
  ::EnumThreadWindows( prMyData->m_dwCurrentThreadID, FindMsgBox, NULL );
  
	if ( !g_hwndMsgBox )
		return 0;
  
	::SetWindowLong( g_hwndMsgBox, GWL_USERDATA, ::SetWindowLong(g_hwndMsgBox, GWL_WNDPROC, (long)MyWindowProc) );
  ::EnumChildWindows( g_hwndMsgBox, EnumChildProc, TRUE );
	
  if ( uBlinkingTime > prMyData->m_uElapseTime )
		uBlinkingTime = prMyData->m_uElapseTime;
	
_Again: 
  uElapseTime = prMyData->m_uElapseTime - uBlinkingTime;
  dWaitRetVal = ::WaitForSingleObject( prMyData->m_hStopEvent, uElapseTime );
  if ( dWaitRetVal == WAIT_TIMEOUT ) {
		if ( g_bUserIsHere ) {
			if ( prMyData->m_bStopElapseByUserActivity ) {
				dWaitRetVal = WAIT_OBJECT_0;
				goto _Stop;
			}
			g_bUserIsHere = FALSE;
			goto _Again;
		}
		if ( (int)uBlinkingTime > 0 ) {
			::SetForegroundWindow( g_hwndMsgBox );
			uCaretBlinkTime = ::GetCaretBlinkTime();
			uTime = uBlinkingTime;
			while ( (int)uTime > 0 ) {
				::FlashWindow( g_hwndMsgBox, TRUE );
				dWaitRetVal = ::WaitForSingleObject( prMyData->m_hStopEvent, uCaretBlinkTime );
				if ( dWaitRetVal != WAIT_TIMEOUT )
					break;
				if ( g_bUserIsHere ) {
					if ( prMyData->m_bStopElapseByUserActivity ) {
						dWaitRetVal = WAIT_OBJECT_0;
						goto _Stop;
					}
					g_bUserIsHere = FALSE;
					::SendMessage( g_hwndMsgBox, WM_NCACTIVATE, (WPARAM)(GetForegroundWindow() == g_hwndMsgBox), 0 );
					goto _Again;
				}
				uTime-= uCaretBlinkTime;
			}
		}
  }
	
_Stop:
	if ( ::IsWindow(g_hwndMsgBox) ) {
		if ( dWaitRetVal == WAIT_TIMEOUT ) {
			//::EndDialog( g_hwndMsgBox, prMyData->m_iResByElapsed );
			::SendMessage( g_hwndMsgBox, nUnloadJustNow, prMyData->m_iResByElapsed, 0 );
		}
		// Don't use it - crash on WinXP
		//::SetWindowLong( g_hwndMsgBox, GWL_WNDPROC, ::GetWindowLong(g_hwndMsgBox, GWL_USERDATA) );
		//::EnumChildWindows( g_hwndMsgBox, EnumChildProc, FALSE );
	}
	
  return 0;
}

// ---
int MsgBoxWithTimeout( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, 
											 UINT uElapseTime/*= 30000*/, UINT uBlinkingTime/*= 5000*/, 
											 BOOL bStopElapseByUserActivity/*= TRUE*/, int iResByElapsed/*= 0*/ ) {

	if ( uElapseTime != INFINITE ) {
		trMyData rMyData;
		rMyData.m_dwCurrentThreadID = ::GetCurrentThreadId();
		rMyData.m_hStopEvent				= ::CreateEvent( NULL, TRUE, FALSE, NULL );
		rMyData.m_uElapseTime				= uElapseTime;
		rMyData.m_uBlinkingTime			= uBlinkingTime;
		rMyData.m_bStopElapseByUserActivity = bStopElapseByUserActivity;

		if ( iResByElapsed == 0 ) {
			iResByElapsed = IDOK;
			
			switch ( uType & MB_TYPEMASK ) {
				case MB_OK:
					iResByElapsed = IDOK;
					break;
				case MB_OKCANCEL:
					iResByElapsed = IDOK;
					switch ( uType & MB_DEFMASK ) {
						case MB_DEFBUTTON2 :
							iResByElapsed = IDCANCEL;
							break;
					}
					break;
				case MB_YESNO:
					iResByElapsed = IDYES;
					switch ( uType & MB_DEFMASK ) {
						case MB_DEFBUTTON2 :
							iResByElapsed = IDNO;
							break;
					}
					break;
				case MB_YESNOCANCEL:
					iResByElapsed = IDYES;
					switch ( uType & MB_DEFMASK ) {
						case MB_DEFBUTTON2 :
							iResByElapsed = IDNO;
							break;
						case MB_DEFBUTTON3 :
							iResByElapsed = IDCANCEL;
							break;
					}
					break;
				case MB_ABORTRETRYIGNORE:
					iResByElapsed = IDABORT;
					switch ( uType & MB_DEFMASK ) {
						case MB_DEFBUTTON2 :
							iResByElapsed = IDRETRY;
							break;
						case MB_DEFBUTTON3 :
							iResByElapsed = IDIGNORE;
							break;
					}
					break;
				case MB_RETRYCANCEL:
					iResByElapsed = IDRETRY;
					switch ( uType & MB_DEFMASK ) {
					case MB_DEFBUTTON2 :
						iResByElapsed = IDCANCEL;
						break;
					}
					break;
			}
		}

		rMyData.m_iResByElapsed     = iResByElapsed;

		unsigned int uThreadID;
		HANDLE hThreadHandle = (HANDLE)::_beginthreadex( NULL, 0, TimeoutMsgBox,	(void*)&rMyData, 0, &uThreadID );

		int iRes = ::MessageBox( hWnd, lpText, lpCaption, uType );

		::SetEvent( rMyData.m_hStopEvent );
		if ( hThreadHandle ) {
			::WaitForSingleObject( hThreadHandle, INFINITE );
			::CloseHandle( hThreadHandle );
		}
		
		::CloseHandle( rMyData.m_hStopEvent );

		return iRes;
	}
	else
		return ::MessageBox( hWnd, lpText, lpCaption, uType );
}



/*
void main()
{
  int nRes = MsgBoxWithTimeout(NULL, "If there is no user interaction,\nthis message box should close in 6 seconds", 
				"Message Box with Timeout", MB_OKCANCEL | MB_DEFBUTTON2, 6000, 3000, TRUE, 0);
}
*/

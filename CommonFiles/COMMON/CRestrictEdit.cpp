// CRestrictEdit.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "CRestrictEdit.h"
#include "WinVerDiff.h"
#include "../KLUtil/UnicodeConv.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool CRestrictionNumberOnly::IsSymbolOK( LPCTSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText)
{
	if (!isdigit  ( szTargetString[nOffset]))
	{
		szErrorTitle.LoadString  (IDS_COMMON_TITLE_INVALID_SYMBOL);
		szErrorText.LoadString  (IDS_COMMON_ONLY_NUMBERS_ALLOWED);
		return false;
	}

	return true;
}

bool CRestrictionNumberOnly::IsSymbolOK( LPCWSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText)
{
	if (!iswdigit  ( szTargetString[nOffset]))
	{
		szErrorTitle.LoadString  (IDS_COMMON_TITLE_INVALID_SYMBOL);
		szErrorText.LoadString  (IDS_COMMON_ONLY_NUMBERS_ALLOWED);
		return false;
	}
	
	return true;
}

bool CRestrictionNumberOnly::Finalize(LPCTSTR szTargetString, CString& szErrorTitle, CString& szErrorText)
{
	int nLength = lstrlen(szTargetString);
	for(int i = 0; i<nLength; ++i)
	{
		if ( !isdigit  (szTargetString[i]) )
		{
			szErrorTitle.LoadString  (IDS_COMMON_TITLE_INVALID_SYMBOL);
			szErrorText.LoadString  (IDS_COMMON_ONLY_NUMBERS_ALLOWED);
			return false;
		}
	}

	return true;
		

}

bool CRestrictionNumberOnly::Finalize(LPCWSTR szTargetString, CString& szErrorTitle, CString& szErrorText)
{
	int nLength = wcslen(szTargetString);
	for(int i = 0; i<nLength; ++i)
	{
		if ( !iswdigit  (szTargetString[i]) )
		{
			szErrorTitle.LoadString  (IDS_COMMON_TITLE_INVALID_SYMBOL);
			szErrorText.LoadString  (IDS_COMMON_ONLY_NUMBERS_ALLOWED);
			return false;
		}
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CRestrictSubstrings::SetIgnoreSequences(std::vector<IgnoreEntry> & ignore )
{
	m_IgnoreSeq = ignore;
	
	for (std::vector<IgnoreEntry>::iterator i = m_IgnoreSeq.begin  (); i!=m_IgnoreSeq.end (); ++i)
	{
		(*i).m_szSeq.MakeUpper  ();
	}
	
}

////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/*
int nStart, nEnd;
pEditWnd->GetSel(nStart, nEnd);
CString szEditText;
pEditWnd->GetWindowText(szEditText);

  CString szLeftSub = szEditText.Left  (nStart);
  CString szRightSub = szEditText.Mid  (nEnd);
  
	szEditText = szLeftSub + (TCHAR)nChar + szRightSub;
	
	  TRACE2 (_T("String new %s, before selection %s\n"), (LPCSTR)szEditText, (LPCSTR)szEditText.Left  (nStart));
	  
*/
bool CRestrictSubstrings::IsSymbolOK( LPCTSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText)
{
	return IsStringOK(szTargetString, szErrorTitle, szErrorText);
}

bool CRestrictSubstrings::IsSymbolOK( LPCWSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText)
{
	return IsStringOK(szTargetString, szErrorTitle, szErrorText);
}


bool CRestrictSubstrings::IsStringOK(LPCTSTR szTargetString, CString& szErrorTitle, CString& szErrorText)
{
	CString szText = szTargetString;
	szText.MakeUpper  ();
	
	for (std::vector<IgnoreEntry>::iterator i = m_IgnoreSeq.begin  (); i!=m_IgnoreSeq.end (); ++i)
	{
		if (_tcsstr(szText, (*i).m_szSeq))
		{
			szErrorTitle = (*i).m_szTitle;
			szErrorText = (*i).m_szText;
			return false;
		}
	}
	
	return true;
}

bool CRestrictSubstrings::IsStringOK(LPCWSTR szTargetString, CString& szErrorTitle, CString& szErrorText)
{
	std::vector<WCHAR> szText(szTargetString, szTargetString + wcslen(szTargetString) + 1);
	_wcsupr(&szText.front());
	
	for (std::vector<IgnoreEntry>::iterator i = m_IgnoreSeq.begin  (); i!=m_IgnoreSeq.end (); ++i)
	{
		if (wcsstr(&szText.front(), __LPCWSTR((LPCSTR)(*i).m_szSeq)))
		{
			szErrorTitle = (*i).m_szTitle;
			szErrorText = (*i).m_szText;
			return false;
		}
	}
	
	return true;
}

bool CRestrictSubstrings::Finalize(LPCTSTR szTargetString, CString& szErrorTitle, CString& szErrorText)
{
	return IsStringOK  ( szTargetString, szErrorTitle, szErrorText );
}

bool CRestrictSubstrings::Finalize(LPCWSTR szTargetString, CString& szErrorTitle, CString& szErrorText)
{
	return IsStringOK  ( szTargetString, szErrorTitle, szErrorText );
}

/////////////////////////////////////////////////////////////////////////////
// CCRestrictEdit

CCRestrictEdit::CCRestrictEdit(CInputRestriction* pRestriction/* = NULL*/ , bool bShowXPBallon /*=true*/) : m_pRestriction(pRestriction), m_bShowBaloon(false)
{
//check windows XP
	if (bShowXPBallon)
	{
		OSVERSIONINFOEX info;
		ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
		info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(GetVersionEx((OSVERSIONINFO*)&info))
		{
			m_bShowBaloon = (info.dwMajorVersion > 5) ||
				(info.dwMajorVersion == 5 && info.dwMinorVersion > 0);
		}
	}
}

CCRestrictEdit::~CCRestrictEdit()
{
}


BEGIN_MESSAGE_MAP(CCRestrictEdit, CEdit)
//{{AFX_MSG_MAP(CCRestrictEdit)
ON_WM_CHAR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCRestrictEdit message handlers

void CCRestrictEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nStart, nEnd;
	GetSel(nStart, nEnd);
	CString szEditText;
	GetWindowText(szEditText);
	
	CString szLeftSub = szEditText.Left  (nStart);
	CString szRightSub = szEditText.Mid  (nEnd);
	
	szEditText = szLeftSub + (TCHAR)nChar + szRightSub;
	
	TRACE2 (_T("String new %s, before selection %s\n"), (LPCSTR)szEditText, (LPCSTR)szEditText.Left  (nStart));
	
	CString strTitle, strText;
	if ( m_pRestriction && GetKeyState(VK_CONTROL) >= 0 &&
		GetKeyState(VK_MENU) >= 0 && !iscntrl(nChar) && !m_pRestriction->IsSymbolOK((LPCTSTR)szEditText, nStart, strTitle, strText))
	{
		
		if ( m_bShowBaloon )
		{
			
			EDITBALLOONTIP ebt;
			
	
			ebt.cbStruct = sizeof(EDITBALLOONTIP);
			ebt.pszTitle = strTitle.AllocSysString();
			ebt.pszText = strText.AllocSysString();
			ebt.ttiIcon = TTI_ERROR;

			SendMessage(EM_SHOWBALLOONTIP, 0, (LPARAM)&ebt); // = EM_SHOWBALLOONTIP

			
			SysFreeString((BSTR)ebt.pszTitle);
			SysFreeString((BSTR)ebt.pszText);
			
		}
		else
			MsgBoxEx(NULL, strText, strTitle, MB_OK | MB_ICONERROR);
		
		return;
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


CCRestrictEditEx::CCRestrictEditEx(HWND hDualWnd, CInputRestriction* pRestriction/* = NULL*/ , bool bShowXPBallon /*=true*/) : 
m_pRestriction(pRestriction), 
m_bShowBaloon(false),
m_hWnd(NULL),
m_fnWndProc(NULL),
m_hTheme(NULL)
{
	//check windows XP
	if (bShowXPBallon)
	{
		OSVERSIONINFOEX info;
		ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
		info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(GetVersionEx((OSVERSIONINFO*)&info))
		{
			m_bShowBaloon = (info.dwMajorVersion > 5) ||
				(info.dwMajorVersion == 5 && info.dwMinorVersion > 0);
		}
	}
}


LRESULT CALLBACK CCRestrictEditEx::WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCRestrictEditEx* pThis = (CCRestrictEditEx*)::GetWindowLong  (hWnd, GWL_USERDATA);
	ASSERT (hWnd == pThis->m_hWnd);
	

	if (uMsg == WM_NCPAINT)
	{
		HDC hDC = ::GetWindowDC(hWnd);
		
		CRect rectWindow;
		::GetWindowRect (hWnd, rectWindow);
		POINT topleft = rectWindow.TopLeft  (), bottomright = rectWindow.BottomRight  ();
		::ScreenToClient (hWnd, &topleft);
		::ScreenToClient (hWnd, &bottomright);
		rectWindow.SetRect  (topleft, bottomright);
		
		CRect rectBorder = rectWindow - rectWindow.TopLeft ();
		CRect rectClip = rectBorder;
		rectClip.InflateRect (-::GetSystemMetrics (SM_CXEDGE), -::GetSystemMetrics (SM_CYEDGE));
		::ExcludeClipRect (hDC, rectClip.left, rectClip.top, rectClip.right, rectClip.bottom);
		
		if (pThis->m_hTheme)
		{
			CCXPThemeHelper::DrawThemeBackground (pThis->m_hTheme, hDC, EP_EDITTEXT, ETS_NORMAL, rectBorder, NULL);
		}
		else
		{
			::DrawEdge (hDC, rectBorder, EDGE_SUNKEN, BF_RECT);
		}
		
		::ReleaseDC(hWnd, hDC);
		
		return FALSE;
	}
	else if (uMsg == WM_CHAR)
	{
		UINT nChar = (UINT)wParam;
		CString strTitle, strText;
		
		int nStart, nEnd;
		::SendMessage(hWnd, EM_GETSEL, (WPARAM)&nStart,(LPARAM)&nEnd);
		
		WCHAR szEditText[MAX_PATH];
		if (IsWin9x())
		{
			TCHAR buff[MAX_PATH];
			::GetWindowText(hWnd, buff, sizeof(buff)/sizeof(buff[0]));
			wcscpy(szEditText, __LPCWSTR(buff));
		}
		else
			::GetWindowTextW(hWnd, szEditText, sizeof(szEditText)/sizeof(szEditText[0]));
		
		
		int nLen = wcslen(szEditText);
		
		ASSERT (nStart>=0 && nStart<MAX_PATH && nEnd>=0 && nEnd<MAX_PATH && nLen<MAX_PATH);
		
		std::wstring szLeftSub(szEditText, szEditText + nStart), szRightSub(szEditText + nStart, szEditText + nLen);
		
		szLeftSub += (WCHAR)nChar + szRightSub;
		
		if ( pThis->m_pRestriction && ::GetKeyState(VK_CONTROL) >= 0 &&
			::GetKeyState(VK_MENU) >= 0 && !iswcntrl(nChar) && !pThis->m_pRestriction->IsSymbolOK(szLeftSub.c_str(), nStart, strTitle, strText))
		{
			DWORD dwPos = (DWORD)::SendMessage  (hWnd, EM_POSFROMCHAR, (WPARAM)nStart, 0);
			RECT rcEdit;
			::GetWindowRect(hWnd, &rcEdit);
			
			
			pThis->m_Balloon.ShowBalloon(CPoint(rcEdit.left + LOWORD(dwPos) + 2, rcEdit.bottom - 4), 3, strTitle, strText);
			
			//			MsgBoxEx(NULL, strText, strTitle, MB_OK | MB_ICONERROR);
			
			return FALSE;
		}
	}
	else if (uMsg == WM_DESTROY)
	{
		if (pThis->m_hTheme)
		{
			CCXPThemeHelper::CloseThemeData (pThis->m_hTheme);
			pThis->m_hTheme = NULL;
		}
		
		::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pThis->m_fnWndProc);
		::SetWindowLong(hWnd, GWL_USERDATA, 0);
	}
	
	return pThis->m_fnWndProc(hWnd, uMsg, wParam, lParam);
}


BOOL CCRestrictEditEx::Create(CWnd* pParent, CRect& rcWnd, DWORD dwStyle/* = 0x54000088*/, DWORD dwExStyle/* = 0x204*/)
{
	ASSERT (pParent!=NULL);
	
	m_hWnd = ::CreateWindowEx (dwExStyle, IsWin9x()?_T("RichEdit20A"):_T("RichEdit20W"), NULL, dwStyle, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), pParent->GetSafeHwnd  (), NULL, AfxGetInstanceHandle  (), NULL);
	if(!::IsWindow(m_hWnd))
		return FALSE;
	
	HMODULE hShlwapi32 = ::LoadLibrary (_T("Shlwapi.dll"));
	if (hShlwapi32)
	{
		typedef HRESULT ( WINAPI *fnSHAutoComplete) ( HWND hwndEdit, DWORD dwFlags );
		
		fnSHAutoComplete SHAutoComplete = (fnSHAutoComplete)::GetProcAddress (hShlwapi32, _T("SHAutoComplete"));
		
		if (SHAutoComplete)
			SHAutoComplete (m_hWnd, SHACF_FILESYSTEM | SHACF_URLALL | SHACF_FILESYS_ONLY);
		
		::FreeLibrary (hShlwapi32);
	}

	VERIFY(m_Balloon.Create  (m_hWnd));
	
	
	if (IsWin9x  ())
	{
		m_fnWndProc = (WNDPROC)::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)WindowProc);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	}
	else
	{
		m_fnWndProc = (WNDPROC)::SetWindowLongW(m_hWnd, GWL_WNDPROC, (LONG)WindowProc);
		::SetWindowLongW(m_hWnd, GWL_USERDATA, (LONG)this);
		
	}
	
	::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)pParent->GetFont()->GetSafeHandle(), FALSE);
	
	if (CCXPThemeHelper::CanApplyXPStyle ())
        m_hTheme = CCXPThemeHelper::OpenThemeData (m_hWnd, L"EDIT");
    else
        m_hTheme = NULL;
	
	return TRUE;
}


CCBalloon::CCBalloon () : m_hWnd(NULL), m_bShow(FALSE), m_lpfnToolTipWndProc(NULL){}

LRESULT CALLBACK CCBalloon::WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCBalloon* pThis = (CCBalloon*)GetWindowLong  (hWnd, GWL_USERDATA);
	ASSERT(pThis->GetSafeHwnd  () == hWnd);
	
	if (uMsg == WM_ACTIVATEAPP && wParam == FALSE)
		pThis->HideBalloon  ();	
	else if (uMsg == WM_TIMER && wParam == 54)
	{
		::KillTimer  (hWnd, 54);
		pThis->HideBalloon  (false);	
	}
	else if (uMsg == WM_DESTROY)
	{
		::SetWindowLong (hWnd, GWL_WNDPROC, (LONG)pThis->m_lpfnToolTipWndProc);
		::SetWindowLong (hWnd, GWL_USERDATA, 0);
	}
	
	
	return pThis->m_lpfnToolTipWndProc(hWnd, uMsg, wParam, lParam);
}

BOOL CCBalloon::Create  (HWND hWndParent)
{
    m_hWnd = CreateWindowEx(0x00080088, //WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | WS_CLIPSIBLINGS | TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_BALLOON,		
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hWndParent,
        NULL,
        NULL,
        NULL
        );
	
	m_lpfnToolTipWndProc = (WNDPROC)::SetWindowLong (GetSafeHwnd  (), GWL_WNDPROC, (LONG)CCBalloon::WndProc);
	::SetWindowLong (GetSafeHwnd  (), GWL_USERDATA, (LONG)this);
	
	
	if ( ::IsWindow(hWndParent) )
	{
		HFONT hFont = (HFONT)::SendMessage(hWndParent, WM_GETFONT, 0, 0);
		if (hFont)
			::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	}
	
	
    ::SetWindowPos(m_hWnd,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	m_pBalloonMsgWnd.push_back ( this );

	if (!m_hKbrdMsgHook)
		m_hKbrdMsgHook = ::SetWindowsHookEx( WH_KEYBOARD, CCBalloon::KeyboardHookProc, NULL,	::GetCurrentThreadId() );

	if (!m_hMouseMsgHook)
		m_hMouseMsgHook = ::SetWindowsHookEx( WH_MOUSE, CCBalloon::MouseHookProc, NULL,	::GetCurrentThreadId() );

	return TRUE;
}

CCBalloon::~CCBalloon()
{
	m_pBalloonMsgWnd.remove ( this );
    
    if  ( m_pBalloonMsgWnd.empty () && m_hKbrdMsgHook )
    {
        ::UnhookWindowsHookEx( m_hKbrdMsgHook );
        m_hKbrdMsgHook = NULL;
    }
	
	if  ( m_pBalloonMsgWnd.empty () && m_hMouseMsgHook )
    {
        ::UnhookWindowsHookEx( m_hMouseMsgHook );
        m_hMouseMsgHook = NULL;
    }
}

BOOL CCBalloon::ShowBalloon(CPoint& pt, int nIconType, LPCTSTR strTitle, LPCTSTR strText, DWORD dwTimeOut /* = 7000*/, DWORD dwMaxWidth /* = 0*/)
{
	ASSERT (::IsWindow(m_hWnd));
	
	::SendMessage(m_hWnd, TTM_SETTITLE, nIconType, (LPARAM)strTitle);
	
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_TRACK;
    ti.hwnd = NULL;
    ti.hinst = NULL;
    ti.uId = 0;
    ti.lpszText = (LPSTR)strText;
	// ToolTip control will cover the whole window
    ti.rect.left = 0;    
    ti.rect.top = 0;
    ti.rect.right = 0;
    ti.rect.bottom = 0;
	
    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
    ::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	
	if (dwMaxWidth)
		::SendMessage ( m_hWnd, TTM_SETMAXTIPWIDTH, 0, ( LPARAM )dwMaxWidth );
	
	
	::SendMessage ( m_hWnd, TTM_TRACKPOSITION, 0, ( LPARAM ) MAKELPARAM ( pt.x, pt.y ) );
	::SendMessage ( m_hWnd, TTM_TRACKACTIVATE, ( WPARAM ) TRUE, ( LPARAM ) &ti );
	

	if (dwTimeOut)
		::SetTimer(m_hWnd, 54, dwTimeOut, NULL);

	m_bShow = true;
	
	return TRUE;
	
}

void CCBalloon::HideBalloon(bool bForceHide /* = true */)
{
	ASSERT(::IsWindow(m_hWnd));

	if (m_bShow)
	{
		TOOLINFO ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.hwnd   = NULL;    
		ti.uId    = 0;
		::SendMessage(m_hWnd, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&ti );
		
		::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)&ti);
		
		if (bForceHide)
			::ShowWindow(m_hWnd, SW_HIDE);
		
		m_bShow = false;
	}
}

LRESULT CALLBACK CCBalloon::KeyboardHookProc( int code, WPARAM wParam, LPARAM lParam )
{
    if  ( code >= 0 )
    {
        for ( std::list < CCBalloon* >::iterator i = m_pBalloonMsgWnd.begin (); i != m_pBalloonMsgWnd.end (); ++i )
        {
			// key was pressed (not released)
			if (!(lParam & 0x80000000))
				( *i ) -> HideBalloon ();
        }
    }
    return ::CallNextHookEx( m_hKbrdMsgHook, code, wParam, lParam );
	
}																																								


LRESULT CALLBACK CCBalloon::MouseHookProc( int code, WPARAM wParam, LPARAM lParam )
{
    if  ( code >= 0 )
    {
		if (wParam != WM_MOUSEMOVE && wParam != WM_NCMOUSEMOVE)
		{
			for ( std::list < CCBalloon* >::iterator i = m_pBalloonMsgWnd.begin (); i != m_pBalloonMsgWnd.end (); ++i )
			{
				( *i ) -> HideBalloon ();
			}
		}
    }
	
    return ::CallNextHookEx( m_hMouseMsgHook, code, wParam, lParam );
}


HHOOK CCBalloon::m_hKbrdMsgHook = 0;
HHOOK CCBalloon::m_hMouseMsgHook = 0;

std::list < CCBalloon * > CCBalloon::m_pBalloonMsgWnd;																																								

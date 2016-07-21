// RichTextImpl.cpp: implementation of the CRichTextImpl class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "richtext.h"
#include "RichTextImpl.h"

#include <StuffIO\utils.h>
#include <commctrl.h>

#include <stuff/parray.h>
#include <stuff/cpointer.h>
#include <sign\sign.h>// kaspersky lab modules sign check
#include "robo_helper_dload.h"
#include "MailWeb.h"
#include "DragWnd.h"
#include "../KLUtil/UnicodeConv.h"
#include "LockedBitmap.h"
#include "WinVerDiff.h"

#pragma comment(lib, "comctl32.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const struct _InitializeRtf20
{
	_InitializeRtf20 ()
	{
		m_RtfDll = LoadLibrary ( _T("riched20.dll") );
		
		INITCOMMONCONTROLSEX iccex; 
		iccex.dwICC = ICC_WIN95_CLASSES;
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		VERIFY(InitCommonControlsEx(&iccex));
		
	}
	
	~_InitializeRtf20 ()
	{
		if ( m_RtfDll ) FreeLibrary ( m_RtfDll );
	}
	
	HMODULE m_RtfDll;
}g_RtfInitialize;

static BOOL CompareWndClassName(HWND hWnd, LPCTSTR lpszClassName)
{
	_ASSERT(::IsWindow(hWnd));
	TCHAR szTemp[32];
	::GetClassName(hWnd, szTemp, 32);
	return lstrcmpi(szTemp, lpszClassName) == 0;
}

static HWND GetAnyParentDlg( HWND hwnd)
{
	HWND hParentDlg = hwnd;
	while ( hParentDlg && !(CompareWndClassName  ( hParentDlg, _T("#32770") ) && (::GetWindowLong  (hParentDlg, GWL_STYLE) & WS_POPUP)) )
		hParentDlg = ::GetParent  (hParentDlg);
	
	return hParentDlg;
}

CRichTextImpl::CRichTextImpl(HWND hWnd) 
:	m_bOverControl(false), 
	m_Hwnd(hWnd), 
	m_bDrag(true), 
	m_dwLBtnDownLinkID(LINK_NOT_PRESSED),
	m_bRedraw(true)
{
	if (!g_RtfInitialize.m_RtfDll)
		throw 1;
	
	::SetWindowLong ( hWnd, GWL_USERDATA, (DWORD)this );
	
	//Only for 2000 or later
	m_hLinkCursor = ::LoadCursor ( NULL, IDC_HAND );
	
	//windows NT
	if ( m_hLinkCursor == NULL )
		m_hLinkCursor = ::LoadCursor  (g_RtfInitialize.m_RtfDll, MAKEINTRESOURCE(105));
	
	SendMessage(hWnd, EM_SETEVENTMASK, 0, ENM_LINK);

	m_oldWndProc = (WNDPROC) ::SetWindowLong ( hWnd, GWL_WNDPROC, (DWORD) CRichTextImpl::WndProc );

	m_ToolTip.SetWrapLongText ( TRUE );
	
}

CRichTextImpl::~CRichTextImpl()
{
	clear ();

//	if ( m_RtfDll ) 
//		FreeLibrary ( m_RtfDll );
}

LRESULT CALLBACK CRichTextImpl::WndProc(
	HWND hwnd,      // handle to window
	UINT uMsg,      // message identifier
	WPARAM wParam,  // first message parameter
	LPARAM lParam   // second message parameter
	)
{
	CRichTextImpl * p = ( CRichTextImpl * ) GetWindowLong ( hwnd, GWL_USERDATA );
	
	WNDPROC proc = p -> m_oldWndProc;

	if  ( ::IsWindow ( p->m_ToolTip.GetSafeHwnd () ) )
    {
		MSG msg;
		msg.hwnd = hwnd;
		msg.message = uMsg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		DWORD dwPos = ::GetMessagePos  ();
		msg.pt.x = RT_GET_X_LPARAM(dwPos);
		msg.pt.y = RT_GET_Y_LPARAM(dwPos);
		msg.time = ::GetMessageTime  ();
		
        p->m_ToolTip.PreTranslateMessage ( &msg );
    }
	

	if ( uMsg == WM_SETREDRAW )
	{
		p->m_bRedraw = (wParam == TRUE);
		return FALSE;
	}
	else if ( uMsg == WM_PAINT )
	{
		if (!p->m_bRedraw)
		{
			PAINTSTRUCT ps;
			::BeginPaint(hwnd, &ps);
			::EndPaint(hwnd, &ps);

			return FALSE;
		}
		else
		{
			LRESULT ret = proc ( hwnd, uMsg, wParam, lParam );
			HDC hDC = ::GetDC(hwnd);
			p->DrawLocks(hwnd, hDC);
			::ReleaseDC(hwnd, hDC);

			return ret;
		}
	}
	else if ( uMsg == WM_ERASEBKGND)
	{
		if (!p->m_bRedraw)
			return TRUE;
	}
    else if  ( uMsg == WM_NCDESTROY )
    {
        SetWindowLong ( hwnd, GWL_WNDPROC, (DWORD) proc );
        SetWindowLong ( hwnd, GWL_USERDATA, 0 );
        
        delete p;
    }
    else if ( uMsg == WM_SETCURSOR )
    {
        SetCtrlCursor(hwnd);
        return TRUE;
    }
    else if ( uMsg == WM_SETFOCUS )
    {
        return FALSE;
    }
    else if ( uMsg == WM_SIZE )
    {
        p -> clear_tooltip();
    }
    else if ( uMsg == WM_LBUTTONDOWN )
    {
        POINT pt;
        
        pt.x = RT_GET_X_LPARAM(lParam);
        pt.y = RT_GET_Y_LPARAM(lParam); 

		CHARRANGE cr;
        DWORD dwLinkID;

		if ( p -> IsOverLink ( hwnd, pt, &cr, &dwLinkID ) )
		{
			p -> m_dwLBtnDownLinkID = dwLinkID;

			HWND hwndGranny = GetParent(hwnd);
            if (hwndGranny)
            {
                LONG idCtrl = GetWindowLong(hwnd, GWL_ID);
                
                ENLINK enLink;
                
                enLink.nmhdr.hwndFrom = hwnd;
                enLink.nmhdr.idFrom = idCtrl;
                enLink.nmhdr.code = EN_LINK;
                
                enLink.chrg = cr;
                enLink.msg = WM_LBUTTONDOWN;
                
                ::SendMessage(hwndGranny, WM_NOTIFY, 0, (LPARAM)&enLink);
            }
		}
		else if ( p -> m_bDrag )
            LButtonDragWnd(hwnd, lParam);

        return FALSE;
    }
    else if ( uMsg == WM_LBUTTONUP )
    {
        POINT pt;
        
        pt.x = RT_GET_X_LPARAM(lParam);
        pt.y = RT_GET_Y_LPARAM(lParam); 
        
        CHARRANGE cr;
        
        DWORD dwLinkID;

        if  ( p -> IsOverLink ( hwnd, pt, &cr, &dwLinkID ) && p -> m_dwLBtnDownLinkID == dwLinkID)
        {
            if (dwLinkID == -2)
            {
                p -> on_spec_word_click(cr);
                return FALSE;
            }
            
            HWND hwndGranny = GetParent(hwnd);
            
            if (hwndGranny)
            {
                LONG idCtrl = GetWindowLong(hwnd, GWL_ID);
                
                ENLINK enLink;
                
                enLink.nmhdr.hwndFrom = hwnd;
                enLink.nmhdr.idFrom = idCtrl;
                enLink.nmhdr.code = EN_LINK;
                
                enLink.chrg = cr;
                enLink.msg = WM_LBUTTONUP;
                
                HCURSOR hCurrent = ::GetCursor();
                HCURSOR hWait = ::LoadCursor(NULL, IDC_APPSTARTING);
                if (hWait)
                    ::SetCursor(hWait);
                
                ::SendMessage(hwndGranny, WM_NOTIFY, 0, (LPARAM)&enLink);
                
                ::SetCursor(hCurrent);
            }
        }
        return FALSE;
    }
    else if ( uMsg == WM_KEYDOWN )
    {
        // fix around for VK_ESCAPE in a multiline Edit that is on a Dialog
        // that doesn't have a cancel or the cancel is disabled.
        if (	( wParam == VK_ESCAPE || wParam == VK_CANCEL ) &&
            (::GetWindowLong(hwnd, GWL_STYLE) & ES_MULTILINE))
        {
            HWND hParentDlg = GetAnyParentDlg(hwnd);
            
            if (hParentDlg)
            {
                HWND hItem = ::GetDlgItem(hParentDlg, IDCANCEL);
                if (hItem == NULL || ::IsWindowEnabled(hItem))
				{
					bool bCancel = true;
					::SendMessage(hParentDlg, UM_CANCANCELNOW, WPARAM(&bCancel), 0);
					
					if (bCancel)
						::SendMessage(hParentDlg, WM_COMMAND, IDCANCEL, 0);
				}
                return TRUE;
            }
			
		}else if ( wParam == VK_TAB )
		{
			HWND hParentDlg = GetAnyParentDlg(hwnd);
			HWND hNextItem = ::GetNextDlgTabItem( hParentDlg, hwnd, FALSE);
			
			if ( ::IsWindow(hNextItem) )
				::SetFocus  (hNextItem);
		}
    }
    else if ( uMsg == WM_SETHWND )
    {
        p -> m_hHideWnd = ( HWND ) wParam;
        return FALSE;
    }
    else if ( uMsg == WM_GETLISTITEMS )
    {
        return p -> GetListItems ( wParam, lParam );
    }
	else if ( uMsg == WM_MOUSEWHEEL )
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			return FALSE;	// don't handle Ctrl+Mouse wheel (to prevent font sizing inside RichEdit)
		}
	}
	else if ( uMsg >= WM_LBUTTONDBLCLK && uMsg <= WM_MBUTTONDBLCLK)
    {
		//prevent set mouse focus
        return FALSE;
    }

    
	return ::IsWindow ( hwnd ) ? proc ( hwnd, uMsg, wParam, lParam ) : FALSE;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CRichTextImpl::DrawLocks(HWND hwnd, HDC hDC, const CPoint& ptOffset)
{
	for ( RTF_TOKEN_LIST::const_iterator i = m_lstTokenText.begin  (); i != m_lstTokenText.end  (); ++i )
	{
		if (i->token.mask & TOKEN_LOCKED)
		{
			LRESULT dwRet = m_oldWndProc(hwnd, EM_POSFROMCHAR, (WPARAM)i->cr.cpMax-1, (LPARAM)0);
			short pos_left = (short)(ptOffset.x + (short)LOWORD(dwRet) - 1);
			short pos_top = (short)(ptOffset.y + (short)HIWORD(dwRet) + 1);
			
			HDC hBitmapDC = ::CreateCompatibleDC(hDC);
			
			
			HGDIOBJ pOld = ::SelectObject(hBitmapDC, g_hLockBitmap);
			
			HDC hBitmapMaskDC = ::CreateCompatibleDC(hDC);
			HGDIOBJ pOldMask = ::SelectObject(hBitmapMaskDC, g_hLockBitmapMask);
			
			::BitBlt(hDC, pos_left, pos_top, g_LockBitmapSize.cx, g_LockBitmapSize.cy, hBitmapMaskDC, 0, 0, SRCAND);
			::BitBlt(hDC, pos_left, pos_top, g_LockBitmapSize.cx, g_LockBitmapSize.cy, hBitmapDC, 0, 0, SRCPAINT);
			
			::SelectObject(hBitmapDC, pOld);
			::DeleteDC(hBitmapDC);
			::SelectObject(hBitmapMaskDC, pOldMask);
			::DeleteDC(hBitmapMaskDC);
		}
	}
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
LRESULT CRichTextImpl::GetListItems ( WPARAM wParam, LPARAM lParam )
{
    if  ( m_hHideWnd == NULL )
    {
        return TRUE;
    }

    if  ( lParam == -1 )
    {
        lParam = INT_MAX;
    }
    
    RTF_TOKEN_LIST Tokens;

    unsigned count = 0;
    
    RTF_TOKEN_LIST::const_iterator i;

    for ( i = m_lstTokenText.begin (); i != m_lstTokenText.end (); ++i )
    {
        if  ( i -> token.type != TOKEN::reg && i -> token.type != TOKEN::comment )
        {
            if  ( count == lParam )
            {
                break;
            }
            if  ( count++ >= wParam )
            {
                Tokens.push_back ( *i );
            }
        }
    }

    CEP_USES_SERIALIZE (Tokens.size()* 
		(SIZEOF_ROWNUMBER + SIZEOF_COLUMNNUMBER + 6*SIZEOF_COLUMNITEMNUMBER + 4*SIZEOF_INT + 2*SIZEOF_STRING));

    count = 0;

    for ( i = Tokens.begin (); i != Tokens.end (); ++i )
    {
        CEP_SER_ROWNUMBER ( count++ ) ;
        CEP_SER_COLUMNNUMBER ( 0 ) ;

        CHARRANGE cr = i -> cr;

        RTF_MAPPING_CR2CRECT_MAP cr_map;

        // Rect
        RECT rect;

        if  ( CalcTextRanges ( cr, cr_map ) )
        {
            SetRect ( &rect, 0, 0, 0, 0 );
            int nMaxArea = 0;

            // find the largest rect
            for ( RTF_MAPPING_CR2CRECT_MAP::const_iterator it = cr_map.begin (); it != cr_map.end (); ++it )
            {
                int nCurrArea = ( it -> second.bottom - it -> second.top ) * ( it -> second.right - it -> second.left );

                if  ( nCurrArea > nMaxArea )
                {
                    SetRect ( &rect, it -> second.left, it -> second.top, it -> second.right, it -> second.bottom );
                    nMaxArea = nCurrArea;
                }
            }

            RECT rcView;
            GetClientRect ( m_Hwnd, &rcView );

            // get only visible part of the link
            IntersectRect ( &rect, &rect, &rcView );
        }

        if  ( IsRectEmpty ( &rect ) )
        {
            SetRect ( &rect, -1, -1, -1, -1 );
        }
        CEP_SER_COLUMNITEMNUMBER ( 3 );
        CEP_SER_INT ( rect.left );
        
        CEP_SER_COLUMNITEMNUMBER ( 4 );
        CEP_SER_INT ( rect.top );
        
        CEP_SER_COLUMNITEMNUMBER ( 5 );
        CEP_SER_INT ( rect.right );
        
        CEP_SER_COLUMNITEMNUMBER ( 6 );
        CEP_SER_INT ( rect.bottom );

        // Text
        GETTEXTLENGTHEX gtl;
        gtl.codepage = CP_ACP;
        gtl.flags = GTL_DEFAULT;
        int nChars = m_oldWndProc ( m_Hwnd, EM_GETTEXTLENGTHEX, ( WPARAM ) &gtl, 0 );

        if  ( nChars != E_INVALIDARG )
        {
            // allocate memory buffer
            TCHAR * pszText = new TCHAR [ nChars + 1 ];

            // get text from RichEdit
            GETTEXTEX gte = { 0 };
            gte.cb = nChars;
            gte.codepage = CP_ACP;
            m_oldWndProc ( m_Hwnd, EM_GETTEXTEX, ( WPARAM ) &gte, ( LPARAM ) pszText );

            // get char range from the buffer
            pszText [ cr.cpMax ] = 0;
            CEP_SER_COLUMNITEMNUMBER ( 1 );
            CEP_SER_STRING ( pszText + cr.cpMin );

            delete [] pszText;
        }

        // Tooltip
        RTF_TOOLTIP_MAP::const_iterator iToolTip = m_CompressPathMap.find ( cr );

        if  ( iToolTip != m_CompressPathMap.end () )
        {
            CEP_SER_COLUMNITEMNUMBER ( 7 );
            CEP_SER_STRING ( iToolTip -> second );
        }
    }

    COPYDATASTRUCT cds;
    cds.dwData = 0;
    cds.cbData = CEP_SER_DATASIZE ();
    cds.lpData = CEP_SER_DATA ();
    ::SendMessage ( m_hHideWnd, WM_COPYDATA, ( WPARAM ) m_Hwnd, ( LPARAM ) &cds );

    return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CRichTextImpl::CreateTooltip (LPCTSTR szText, const CHARRANGE& cr, RECT& tip_rect)
{

	HWND hwndTT;                 // handle to the ToolTip control
	// struct specifying info about tool in ToolTip control
	TOOLINFO ti;
	unsigned int uid = 0;       // for ti initialization

	
	/* CREATE A TOOLTIP WINDOW */
	hwndTT = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_Hwnd,
		NULL,
		NULL,
		NULL
		);
	
	SetWindowPos(hwndTT,
		HWND_TOPMOST,
		0,
		0,
		0,
		0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	
	
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = m_Hwnd;
	ti.hinst = NULL;
	ti.uId = (UINT)hwndTT;
	ti.lpszText = const_cast<LPTSTR>(szText);
	ti.rect = tip_rect;
	
	::SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	m_CreatedTooltipMap[cr] = ti;
	
} 

//return true to halt further processing, false proccess further

bool CRichTextImpl::SetCtrlCursor (HWND hwnd)
{
	CRichTextImpl * p = (CRichTextImpl *) GetWindowLong  (hwnd, GWL_USERDATA);
	
	DWORD dwPos = ::GetMessagePos  ();
	POINT pt;
	pt.x = RT_GET_X_LPARAM(dwPos);
	pt.y = RT_GET_Y_LPARAM(dwPos);
	
	::ScreenToClient(hwnd, &pt);
	
	bool bRet = false;
	
	if ( p -> IsOverLink  (hwnd, pt) )
	{
		if (p -> m_hLinkCursor)
			::SetCursor(p -> m_hLinkCursor);
		
		bRet = true;
	}
	else
	{
		HCURSOR hArrow = ::LoadCursor(NULL, IDC_ARROW);
		if (hArrow)
			::SetCursor  (hArrow);

		p -> m_dwLBtnDownLinkID = LINK_NOT_PRESSED;
		
		bRet = false;
	}
	
	LPCTSTR szToolTipText = NULL;
	CHARRANGE cr;
	if  ( p -> IsOverTooltipArea ( hwnd, pt, szToolTipText, cr ) && ::IsWindow ( p -> m_ToolTip.GetSafeHwnd () ) )
	{
		RTF_MAPPING_CR2CRECT_MAP cr_map;
		if (p->CalcTextRanges(cr, cr_map))
		{
			_ASSERT(cr_map.size  () == 1);
			p -> m_ToolTip.Show ( cr_map.begin () -> second, szToolTipText, true );
		}
	}
	return bRet;
}

void CRichTextImpl::AddToolTip(LPCTSTR szToolTipText, CHARRANGE& cr)
{
	if (!IsToolTipCreated(cr))
	{
		RTF_MAPPING_CR2CRECT_MAP cr_map;
		if (CalcTextRanges(cr, cr_map))
		{
			for (RTF_MAPPING_CR2CRECT_MAP::iterator i = cr_map.begin  (); i!= cr_map.end  (); ++i)
			{
				CreateTooltip ( szToolTipText, i->first, i->second );
			}
		}

	}
		
}

bool CRichTextImpl::CalcTextRanges(const CHARRANGE& cr, RTF_MAPPING_CR2CRECT_MAP& cr2rect)
{
	cr2rect.clear  ();

	LONG c_pos = cr.cpMin;

	do
	{
		CHARRANGE add_cr;
		RECT rect;

		GetLineRangeByPos(c_pos, add_cr);
		add_cr.cpMin = c_pos;
		if ( add_cr.cpMax > cr.cpMax)
			add_cr.cpMax = cr.cpMax;
		
		CalcSingleLineRect(add_cr, rect);

		if ( (rect.right - rect.left)*(rect.bottom - rect.top) )
			cr2rect[add_cr] = rect;
		
		c_pos = add_cr.cpMax + 1;
		
		
	}while ( cr.cpMax >= c_pos );
	
	return true;
	
}

void CRichTextImpl::CalcSingleLineRect(CHARRANGE& cr, RECT& rect)
{
	SIZE dim;
	int nExtra;
	CalcCharacterDim(cr.cpMin, dim, nExtra);

	LRESULT dwRet = m_oldWndProc(m_Hwnd, EM_POSFROMCHAR, (WPARAM)cr.cpMin, (LPARAM)0);
	rect.left = (short)LOWORD(dwRet) - dim.cx/2;
	rect.top = (short)HIWORD(dwRet);

	dwRet = m_oldWndProc(m_Hwnd, EM_POSFROMCHAR, (WPARAM)cr.cpMax, (LPARAM)0);
	rect.right = (short)LOWORD(dwRet) + dim.cx/2;

	_ASSERT((short)HIWORD(dwRet) == rect.top);
	
	rect.bottom = rect.top + dim.cy;

	RECT rcClient;

	::GetClientRect(m_Hwnd, &rcClient);

	if ( rect.left>=rcClient.right || rect.right<=0 || rect.bottom<=0 || rect.top>=rcClient.bottom )
	{
		rect.left = 0;
		rect.right = 0;
		rect.bottom = 0;
		rect.top = 0;
		
		return;
	}

	if (rect.left<0)
		rect.left = 0;

	if (rect.top<0)
		rect.top = 0;

	if (rect.right>rcClient.right)
		rect.right=rcClient.right;

	if (rect.bottom>rcClient.bottom)
		rect.bottom = rcClient.bottom;
	
}

bool CRichTextImpl::GetLineRangeByPos(LONG pos, CHARRANGE& cr)
{
	LRESULT dwLine = m_oldWndProc(m_Hwnd, EM_EXLINEFROMCHAR, (WPARAM)0, (LPARAM)pos);

	cr.cpMin = m_oldWndProc(m_Hwnd, EM_LINEINDEX, (WPARAM)dwLine, (LPARAM)0);
	if (cr.cpMin==-1)
		return false;

	cr.cpMax = m_oldWndProc(m_Hwnd, EM_LINELENGTH, (WPARAM)pos, (LPARAM)0);
	if (cr.cpMax)
	{
		cr.cpMax+=cr.cpMin - 1;
		return true;
	}

	return false;

}


LPTSTR CRichTextImpl::tstring_inc(LPCTSTR str)
{
	return str && (*str != _T('\0'))?_tcsinc(str):NULL;
}

LPTSTR CRichTextImpl::find_single_bracket( LPTSTR szBegin, int sym)
{
	LPTSTR szNextStart = szBegin;

	while (szNextStart)
	{
		LPTSTR szFind = _tcschr(szNextStart, sym);
		LPTSTR szNext = tstring_inc(szFind);
		
		if (!szFind)
			return NULL;
		
		if ( szNext && (*szNext != sym) )
			return szFind;

		szNextStart = tstring_inc(szNext);
	}

	return NULL;
}

bool CRichTextImpl::get_next_token  ( IN OUT LPTSTR & start, OUT TOKEN & token )
{
	ZeroMemory (&token, sizeof (TOKEN));
	
	//находим первую угловую скобку '<'
	LPTSTR szLeftBracket = find_single_bracket ( start, _T('<') );

	//конец разбора
	if  ( !szLeftBracket )
	{
		if ( parse_spec_word ( start, szLeftBracket, token) )
			return true;
		
		token.type = TOKEN::reg;
		token.start_text = start;
		token.end_text	 = NULL;
		token.mask = TOKEN_TEXT | TOKEN_TYPE;

		start = NULL;
		
		
		return true;
	}

	LPTSTR szNextSym = tstring_inc ( szLeftBracket );
	if (!szNextSym)
		return false;
	
	//разбираем <type:[color],[id]; text>
	if  ( szLeftBracket == start )
	{

		//ищем закрывающую скобку '>'
		LPTSTR szRightBracket = find_single_bracket ( szLeftBracket, _T('>') );
		
		switch ( *szNextSym )
		{
		case _T('b'):
			token.type = TOKEN::bold;
			break;

		case _T('l'):
			token.type = TOKEN::link;
			break;

		case _T('r'):
			token.type = TOKEN::reg;
			break;
		
		case _T('c'):
			token.type = TOKEN::comment;
			break;


		default:;
			return false;
		}

		token.mask |= TOKEN_TYPE;
		szNextSym = tstring_inc ( szNextSym );
		if (!szNextSym)
			return false;
		
		LPCTSTR szTextBegin = _tcschr ( szNextSym, _T(';') );
//			_PARSE_ERROR_IF ( ((szTextBegin == NULL) || (szTextBegin >= szRightBracket)), _T("Text not found") );

          if   ( ( szTextBegin == NULL ) || ( szTextBegin >= szRightBracket ) )
		{
			return false;
		}
			
		
		
		switch  ( *szNextSym )
		{
		case _T(':'):
				
				szNextSym = tstring_inc ( szNextSym );
				if (!szNextSym)
					return false;
				
				if (*szNextSym == _T('p'))
				{
					LPTSTR szColor = tstring_inc ( szNextSym );
					if (!szColor)
						return false;
					
					token.color = _tcstoul( szColor, &szNextSym, 16 );
					token.mask |= TOKEN_COMPRESS;
					
				}
				else if (*szNextSym == _T('l'))
				{
					szNextSym = tstring_inc ( szNextSym );
					if (!szNextSym)
						return false;

					token.mask |= TOKEN_LOCKED;
					token.color = _tcstoul( szNextSym, &szNextSym, 16 );
					if (token.color)
						token.mask |= TOKEN_COLOR;
				}
				else
				{
					token.color = _tcstoul( szNextSym, &szNextSym, 16 );
					token.mask |= TOKEN_COLOR;
				}
				
				if  ( *szNextSym == _T(',') )
				{
					LPTSTR szId = tstring_inc ( szNextSym );
					if (!szId)
						return false;
					
					token.id = _tcstoul( szId, &szNextSym, 16 );
					token.mask |= TOKEN_ID;
				}
				
				//					_PARSE_ERROR_IF ( (*szNextSym != _T(';')), _T("Text not found") );
				if (*szNextSym != _T(';'))
				{
					return false;
				}
				
				
				break;
				
		case _T(','):
			{
				LPTSTR szId = tstring_inc ( szNextSym );
				if (!szId)
					return false;
				
				
				token.id = _tcstoul( szId, &szNextSym, 16 );
				token.mask |= TOKEN_ID;
				
				//				_PARSE_ERROR_IF ( *szNextSym != _T(';'), _T("Text not found") );
				if (*szNextSym != _T(';'))
				{
					return false;
				}
			}
			break;
			
		case _T(';'):
			break;
			
			
		default: ;
			return false;
			
		}
		
		token.start_text = tstring_inc ( szTextBegin );
		token.end_text	 = szRightBracket;
		token.mask |= TOKEN_TEXT;
		
		start = tstring_inc (szRightBracket);

		return true;
		
	}else{
		
		if ( parse_spec_word ( start, szLeftBracket, token) )
			return true;
		
		//regular текст
		token.type = TOKEN::reg;
		token.start_text = start;
		token.end_text= szLeftBracket;
		token.mask = TOKEN_TEXT | TOKEN_TYPE;

		start = (LPTSTR)szLeftBracket;
		
	}
	
	return true;
}

void CRichTextImpl::attach_rtf_text ( IN const HWND hWnd, IN const TOKEN& token )
{
	if ( !(token.mask & TOKEN_TEXT) || (token.start_text == NULL) || (token.type == TOKEN::comment) )
		return;

	TOKEN_EX tex;
	
	tex.token = token;
	tex.token.start_text = NULL;
	tex.token.end_text = NULL;

	size_t nLength = token.end_text ? token.end_text - token.start_text : _tcslen(token.start_text);

	if (nLength == 0)
		return;

	std::vector<TCHAR> pStr(nLength + 1);
	_tcsncpy ( &pStr[0], token.start_text, nLength );
	pStr[nLength] = _T('\0');

	for (std::vector<TCHAR>::iterator i = pStr.begin  (); i!=pStr.end  ();)
	{
		std::vector<TCHAR>::iterator next = i+1;

		if (next == pStr.end  ())
			break;

		if ( (*i == _T('<') && *next == _T('<')) || (*i == _T('>') && *next == _T('>')) )
			i = pStr.erase  (next);
		else
			++i;
	}

	if ( (token.mask & TOKEN_COMPRESS) && pStr.size  () > 0 )
	{
		if ( token.color < pStr.size  ()-1 )
		{
			tex.token.start_text = new TCHAR[pStr.size  ()];
			_tcsncpy ( (LPTSTR)tex.token.start_text, &pStr[0], pStr.size  () );
			tex.token.start_text[pStr.size  ()-1] = _T('\0');
			
			::CompressPath( tex.token.start_text, token.color, true, &pStr[0], pStr.size  () );
		}
		else
			tex.token.mask &= ~TOKEN_COMPRESS;
	}

	if (token.mask & TOKEN_LOCKED)
	{
		pStr.pop_back();
		pStr.push_back(_T('_'));
//		pStr.push_back(_T(' '));
		pStr.push_back(_T('\0'));
	}

	attach_selected( hWnd, &pStr[0], tex.cr );

	m_lstTokenText.push_back  ( tex );
}

void CRichTextImpl::colorize ( HWND hWnd )
{
	
	CHARFORMAT2 cf;

	for ( std::list<CRichTextImpl::TOKEN_EX>::const_iterator i = m_lstTokenText.begin  (); i != m_lstTokenText.end  (); ++i )
	{

		SendMessage( hWnd, EM_SETSEL, (*i).cr.cpMin, (*i).cr.cpMax );
		
		ZeroMemory ( &cf, sizeof ( cf ) );
		cf.cbSize = sizeof ( cf );
		

		if  ( (*i).token.mask & TOKEN_COMPRESS )
		{
			m_CompressPathMap [(*i).cr] = (*i).token.start_text;
			if  ( !::IsWindow ( m_ToolTip.GetSafeHwnd () ) && CWnd::FromHandlePermanent  (hWnd))
				m_ToolTip.Create(CWnd::FromHandlePermanent  (hWnd));
				
		}
		

		cf.dwMask = CFM_COLOR | CFM_UNDERLINE | CFM_BOLD;
		if  ( i -> token.mask & TOKEN_COLOR )
			cf.crTextColor = i -> token.color;
		else
			cf.crTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
		
		if  ( (*i).token.mask & TOKEN_TYPE ) 
		{
			switch ( i -> token.type )
			{
			case CRichTextImpl::TOKEN::link:

				cf.crTextColor = (i -> token.mask & TOKEN_COLOR)?i -> token.color:RGB(0, 0, 255);
				cf.dwEffects |= CFE_UNDERLINE;
				
				if   ( (*i).token.mask & TOKEN_ID )
					m_RichEditMap [ i->cr ] = i->token.id;
				
				break;
				
			case CRichTextImpl::TOKEN::bold:
				cf.dwEffects |= CFE_BOLD;
				break;

			}
			
			SendMessage( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		}
	}
}

void CRichTextImpl::attach_selected( HWND hWnd, LPCTSTR text, CHARRANGE & crange )
{
	CHARRANGE cr;
	SendMessage(hWnd, EM_SETSEL, -1, 0);
	SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
	
	SendMessage ( hWnd, EM_REPLACESEL, FALSE, (LPARAM)text );
	SendMessage(hWnd, EM_SETSEL, cr.cpMin, -1);
	SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
	//	SendMessage(hWnd, EM_SETSEL, cr.cpMin, cr.cpMax - 1 );
	
	crange = cr;
	--crange.cpMax;
	
}

DWORD CRichTextImpl::GetClickedLink ( ENLINK * pENLink )
{
	RTF_LNK_MAP::iterator i = m_RichEditMap.find ( pENLink -> chrg );
	
	if   ( i != m_RichEditMap.end () )
		return i -> second;
	
	return DWORD(LINK_NOT_PRESSED);
}

bool CRichTextImpl::IsOverText(HWND hwndRichCtrl, const POINT & pt, DWORD& dwChar)
{
	POINTL ptl;
	ptl.x = pt.x;
	ptl.y = pt.y;
	
	dwChar = (DWORD)m_oldWndProc(hwndRichCtrl, EM_CHARFROMPOS, 0, (LPARAM)&ptl);
	SIZE dim;
	int nExtra;
	CalcCharacterDim  (dwChar, dim, nExtra);

	CHARRANGE crLine;
	GetLineRangeByPos (dwChar, crLine);
	
	DWORD dwPos = (DWORD)m_oldWndProc(hwndRichCtrl, EM_POSFROMCHAR, (WPARAM)min(dwChar, (DWORD)crLine.cpMax), 0);
	short left_pos = (short)((short)LOWORD(dwPos) - dim.cx/2);
	short top_pos = (short)HIWORD(dwPos);
	
	return (pt.y>=top_pos && pt.y<=top_pos + dim.cy && pt.x>=left_pos && pt.x<=left_pos + dim.cx);
}

bool CRichTextImpl::IsOverLink (HWND hwndRichCtrl, const POINT & pt, CHARRANGE * pCr/* = NULL*/, DWORD* pdwLinkID /* = NULL*/)
{
	DWORD dwRet;
	if ( IsOverText(hwndRichCtrl, pt, dwRet) )
	{
		CHARRANGE cr;
		cr.cpMin = dwRet;
		cr.cpMax = dwRet;
		
		
		RTF_LNK_MAP::iterator i = m_RichEditMap.find ( cr );
		
		if   ( i != m_RichEditMap.end () )
		{
			if ( pdwLinkID )
				*pdwLinkID = i -> second;
			
			if (pCr)
				*pCr = i -> first;
			
			return true;
		}		
	}
	
	return false;
	
}

bool CRichTextImpl::IsOverTooltipArea( HWND hwnd, const POINT &pt, LPCTSTR& pszToolTipText, CHARRANGE& char_range )
{
	DWORD dwRet;
	
	if (IsOverText (hwnd, pt, dwRet))
	{
		CHARRANGE cr;
		cr.cpMin = dwRet;
		cr.cpMax = dwRet;
		
		RTF_TOOLTIP_MAP::const_iterator i = m_CompressPathMap.find ( cr );
		
		if   ( i != m_CompressPathMap.end () )
		{
			CHARRANGE crLine;
			if (GetLineRangeByPos(dwRet, crLine))
			{
				char_range.cpMin = max(i->first.cpMin, crLine.cpMin);
				char_range.cpMax = min(i->first.cpMax, crLine.cpMax);
				pszToolTipText = i->second;
				return true;
			}
		}
	}

	return false;
	
}

bool CRichTextImpl::IsToolTipCreated(CHARRANGE& cr)
{
	RTF_CREATED_TOOLTIP_MAP::const_iterator i = m_CreatedTooltipMap.find ( cr );
	
	if   ( i != m_CreatedTooltipMap.end () )
		return true;
	
	return false;	
}

void CRichTextImpl::clear()
{
	
	SendMessage(m_Hwnd, EM_SETSEL, 0, -1);
	SendMessage ( m_Hwnd, EM_REPLACESEL, FALSE, (LPARAM)_T("") );
	
	CHARFORMAT2 cf;
	ZeroMemory ( &cf, sizeof ( cf ) );
	cf.cbSize = sizeof ( cf );
	
	cf.dwMask = CFM_BOLD;
	cf.dwEffects &= ~CFE_BOLD;
	
	SendMessage ( m_Hwnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
	
	
	for (std::list<TOKEN_EX>::iterator i = m_lstTokenText.begin  (); i!= m_lstTokenText.end  (); ++i)
	{
		if (((*i).token.mask & TOKEN_COMPRESS) && (*i).token.start_text)
			delete [] (*i).token.start_text;
	}

	
	clear_tooltip();
	m_lstTokenText.clear  ();
	m_RichEditMap.clear  ();
	m_CompressPathMap.clear  ();
	
	
}

void CRichTextImpl::clear_tooltip()
{
	for (RTF_CREATED_TOOLTIP_MAP::iterator k = m_CreatedTooltipMap.begin  (); k!= m_CreatedTooltipMap.end  (); ++k)
	{
		SendMessage(m_Hwnd, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &(k->second));
	}
	
	m_CreatedTooltipMap.clear  ();
	
}

void CRichTextImpl::CalcCharacterDim ( long nChar, SIZE& dim, int& nExtra )
{
	dim.cx = 12;
	dim.cy = 12;
	
	HDC hDc = ::CreateDC  (_T("DISPLAY"), NULL, NULL, NULL);
	if (hDc)
	{
		SendMessage(m_Hwnd, EM_SETSEL, nChar, nChar+1);
		CHARFORMAT2 cf;
		cf.cbSize = sizeof (cf);
		SendMessage(m_Hwnd, EM_GETCHARFORMAT, (WPARAM)SCF_SELECTION, (LPARAM)&cf);
		SendMessage(m_Hwnd, EM_SETSEL, -1, -1);
		
		LOGFONT lf;
		ConvertCharformat2LogFont(hDc, cf, lf);
		
		HFONT hFont = ::CreateFontIndirect(&lf);
		
		if (hFont)
		{
			HFONT hOldFont = (HFONT)::SelectObject(hDc, hFont);
			RECT rc_dim;
			::DrawText  ( hDc, "Q", 1, &rc_dim, DT_CALCRECT );

			SIZE ext_size;
			::GetTextExtentPoint32(hDc, "Q", 1, &ext_size);

			dim.cx = rc_dim.right - rc_dim.left;
			dim.cy = rc_dim.bottom - rc_dim.top;

			nExtra = ::GetTextCharacterExtra  (hDc);
			if (nExtra==0x8000000)
				nExtra = 0;

			::SelectObject(hDc, hOldFont);

			::DeleteObject(hFont);
		}
		
		::DeleteDC(hDc);
	}
}

void CRichTextImpl::ConvertCharformat2LogFont(HDC hDc, const CHARFORMAT2& cf, LOGFONT& lf)
{
	DWORD dwFlags = 0;
	if (cf.dwMask & CFM_SIZE)
	{
		LONG yPerInch = ::GetDeviceCaps(hDc, LOGPIXELSY);
		lf.lfHeight = -(int) ((cf.yHeight * yPerInch) / 1440);
	}
	else
		lf.lfHeight = 0;

	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	
	if ((cf.dwMask & (CFM_ITALIC|CFM_BOLD)) == (CFM_ITALIC|CFM_BOLD))
	{
		lf.lfWeight = (cf.dwEffects & CFE_BOLD) ? FW_BOLD : FW_NORMAL;
		lf.lfItalic = (BYTE)((cf.dwEffects & CFE_ITALIC) ? TRUE : FALSE);
	}
	else
	{
		dwFlags |= CF_NOSTYLESEL;
		lf.lfWeight = FW_DONTCARE;
		lf.lfItalic = FALSE;
	}
	
	if ((cf.dwMask & (CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR)) ==
		(CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR))
	{
		dwFlags |= CF_EFFECTS;
		lf.lfUnderline = (BYTE)((cf.dwEffects & CFE_UNDERLINE) ? TRUE : FALSE);
		lf.lfStrikeOut = (BYTE)((cf.dwEffects & CFE_STRIKEOUT) ? TRUE : FALSE);
	}
	else
	{
		lf.lfUnderline = (BYTE)FALSE;
		lf.lfStrikeOut = (BYTE)FALSE;
	}
	
	if (cf.dwMask & CFM_CHARSET)
		lf.lfCharSet = cf.bCharSet;
	else
		dwFlags |= CF_NOSCRIPTSEL;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	if (cf.dwMask & CFM_FACE)
	{
		lf.lfPitchAndFamily = cf.bPitchAndFamily;
		lstrcpy(lf.lfFaceName, __LPTSTR((LPSTR)cf.szFaceName));
	}
	else
	{
		lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
		lf.lfFaceName[0] = (TCHAR)0;
	}
}

bool CRichTextImpl::parse_spec_word ( IN OUT LPTSTR & start, LPCTSTR end, OUT TOKEN & token)
{
	if (start == NULL)
		return false;

	TCHAR* szLink = NULL;
	bool bIsURL = false;

	{
		//overhead
		TCHAR* szURL = _tcsstr(start, _T("http://"));
		TCHAR* szMAIL = _tcsstr(start, _T("mailto:"));
		
		if ( szURL )
		{
			if ( szMAIL )
			{
				if (szURL < szMAIL)
				{
					szLink = szURL;
					bIsURL = true;
				}
				else
				{
					szLink = szMAIL;
					bIsURL = false;
				}
			}
			else
			{
				szLink = szURL;
				bIsURL = true;
			}
		}
		else
		{
			if ( szMAIL )
			{
				szLink = szMAIL;
				bIsURL = false;
			}
			else
				szLink = NULL;
		}
	}

	if ( szLink == NULL )
		return false;

	if ( szLink == start )
	{
		TCHAR* szEndURL = find_end_URL(szLink, end);
		
		token.type = TOKEN::link;
		token.start_text = szLink;
		token.end_text	 = szEndURL;
		token.id = -2;
		token.mask = TOKEN_TEXT | TOKEN_TYPE | TOKEN_ID;
		
		start = token.end_text;
		
		return true;
	}
	else	if ( !end || (end && szLink<end) )
	{
		token.type = TOKEN::reg;
		token.start_text = start;
		token.end_text	 = szLink;
		token.mask = TOKEN_TEXT | TOKEN_TYPE;
		
		start = szLink;
		
		return true;
		
	}
	
	return false;
}

void CRichTextImpl::on_spec_word_click(CHARRANGE & cr)
{
	TCHAR szBuffText[1024];
	m_oldWndProc(m_Hwnd, EM_SETSEL, cr.cpMin, cr.cpMax );
	m_oldWndProc(m_Hwnd, EM_GETSELTEXT, 0, (LPARAM)szBuffText );
	
	if ( szBuffText == _tcsstr ( szBuffText, _T("http://") ) )
	{
		OpenURL(szBuffText);
	}
	else if ( szBuffText == _tcsstr ( szBuffText, _T("mailto:") ) )
	{
		CWaitCursor cur;
		
		MapiRecipDesc Recipient;
		MapiMessage note;
		
		// Recipient Structure
		Recipient.ulReserved = 0;
		Recipient.ulRecipClass = MAPI_TO;
		Recipient.lpszName = szBuffText + sizeof(_T("mailto:"))/sizeof(TCHAR)  - 1; //NULL ended
		Recipient.lpszAddress = "";//
		Recipient.ulEIDSize = 0;
		Recipient.lpEntryID = NULL;
		
		// Message structure
		memset(&note,0, sizeof(MapiMessage));
		note.lpszSubject = "";
		note.lpszNoteText = "";
		note.lpRecips = &Recipient;
		note.nRecipCount = 1;
		note.nFileCount = 0;
		
		SendMail( &note, ::GetParent (m_Hwnd) );
		
	}
}

LPTSTR CRichTextImpl::find_end_URL( LPCTSTR szURL, LPCTSTR end)
{
	TCHAR* szEndURLArr[] = {_T(" "), _T("\n"), _T("\r"), _T("<"), _T(">"), _T("\t")};

	ULONG szEndURL = ULONG_MAX;
	for (int i = 0; i < sizeof (szEndURLArr)/sizeof (szEndURLArr[0]); ++i )
	{
		ULONG nCurrMin = min(szEndURL, (ULONG)_tcsstr(szURL, szEndURLArr[i]));
		
		if (nCurrMin)
			szEndURL = nCurrMin;
	}

	if (end)
		return szEndURL<(ULONG)end?(LPTSTR)szEndURL:(LPTSTR)end;
	else
		return szEndURL==ULONG_MAX?NULL:(LPTSTR)szEndURL;
}

void CRichTextImpl::SetDragMode(bool bDrag)
{
	m_bDrag = bDrag;
}
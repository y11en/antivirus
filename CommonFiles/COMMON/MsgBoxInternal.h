//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_MSGBOX_INTERNAL_H
#define __COMMON_MSGBOX_INTERNAL_H

#include "thunks.h"

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

//******************************************************************************
// 
//******************************************************************************
class CCMessageBoxPatcherBase : public CCThunk < CCMessageBoxPatcherBase, HOOKPROC >, CCStdThunk < CCMessageBoxPatcherBase, WNDPROC >
{
public:
    CCMessageBoxPatcherBase ()
        :   CCThunk < CCMessageBoxPatcherBase, HOOKPROC > ( ( CCThunk < CCMessageBoxPatcherBase, HOOKPROC >::TMFP ) &CCMessageBoxPatcherBase::CBTProc, this ),
		CCStdThunk < CCMessageBoxPatcherBase, WNDPROC > ( ( CCStdThunk < CCMessageBoxPatcherBase, WNDPROC >::TMFP ) &CCMessageBoxPatcherBase::WndProc, this ),
        m_hwndMessageBox ( NULL )
    {
        m_hHook = ::SetWindowsHookEx ( WH_CBT, CCThunk < CCMessageBoxPatcherBase, HOOKPROC >::GetThunk (), NULL, ::GetCurrentThreadId () );
        
        m_OldWndProc = NULL;
    }
    
    ~CCMessageBoxPatcherBase ()
    {
        if   ( m_hHook )
        {
            ::UnhookWindowsHookEx ( m_hHook );
        }
    }

    virtual void OnDestroyWindow ()
    {
    }
    
protected:
    virtual void Init ()
    {
    }
    
    LRESULT CBTProc ( int nCode, WPARAM wParam, LPARAM lParam )
    {
        HWND hwnd = ( HWND ) wParam;
        
        if  ( HCBT_CREATEWND == nCode && m_hwndMessageBox == NULL )
        {
            m_hwndMessageBox = hwnd;
        }
        else if   ( m_hwndMessageBox == hwnd )
        {
            if  ( HCBT_ACTIVATE == nCode )
            {
                if  ( m_OldWndProc == NULL )
                {
                    Init ();
                    
                    m_OldWndProc = ( WNDPROC ) GetWindowLong ( hwnd, GWL_WNDPROC );
                    
                    SetWindowLong ( hwnd, GWL_WNDPROC, ( LONG ) CCStdThunk < CCMessageBoxPatcherBase, WNDPROC >::GetThunk () );
                }
            }
            else if  ( HCBT_DESTROYWND == nCode )
            {
                SetWindowLong ( hwnd, GWL_WNDPROC, ( LONG ) m_OldWndProc );

                OnDestroyWindow ();
            }
        }
        return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);
    }
    
    LRESULT CALLBACK WndProc ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
    {
        LRESULT ret = m_OldWndProc ( hwnd, msg, wParam, lParam );
        
        if   ( msg == WM_LBUTTONDOWN )
        {
            ::PostMessage ( hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam );
        }
        else if   ( msg == WM_NCHITTEST )
        {
            if   ( ret == HTCLIENT )
            {
                return HTCAPTION;
            }
        }
        return ret;
    }
    
    HWND FindWnd ( HWND hwndChildAfter, LPCTSTR lpszClass, RECT & rect )
    {
        HWND hwnd = ::FindWindowEx ( m_hwndMessageBox, hwndChildAfter, lpszClass, NULL );
        
        if  ( hwnd )
        {
            ::GetWindowRect ( hwnd, &rect );
            ::MapWindowPoints ( NULL, m_hwndMessageBox, ( LPPOINT ) &rect, 2 );
        }
        return hwnd;
    }
    
    void MoveButtons ( int nButtonOffsetX, int nButtonOffsetY, HWND hWndInsertAfter )
    {
        HWND hwnd = NULL;
        
		int Size;
        for ( Size = 0;; ++Size )
        {
            hwnd = ::FindWindowEx ( m_hwndMessageBox, hwnd, _T("BUTTON"), NULL );
            
            if  ( hwnd == NULL )
            {
                break;
            }
        }
        
        RECT * pRect = new RECT [ Size ];
        HWND * pWnd = new HWND [ Size ];
        
		int i;
        for ( i = 0;; ++i )
        {
            hwnd = FindWnd ( hwnd, _T("BUTTON"), pRect [ i ] );
            
            if  ( hwnd == NULL )
            {
                break;
            }
            pWnd [ i ] = hwnd;
        }
        
        for  ( i = 0; i < Size; ++i )
        {
            ::SetWindowPos ( pWnd [ i ], hWndInsertAfter, 
                pRect [ i ].left + nButtonOffsetX, pRect [ i ].top + nButtonOffsetY,
                0, 0,
                SWP_NOSIZE /*| SWP_NOACTIVATE | SWP_NOREDRAW*/ );

			::SetWindowLong(pWnd[i], GWL_STYLE, ::GetWindowLong(pWnd[i], GWL_STYLE) | BS_FLAT);
        }
        delete [] pRect;
        delete [] pWnd;
    }
    
    HHOOK m_hHook;
    
    HWND m_hwndMessageBox;
    
    WNDPROC m_OldWndProc;
};

//******************************************************************************
// 
//******************************************************************************
class CCMessageBoxWhiteBarPatcher : public CCMessageBoxPatcherBase
{
public:
    CCMessageBoxWhiteBarPatcher ( HICON hIcon, const std::map < UINT, std::string > & Localization )
    {
        m_hIcon = hIcon;
        
        m_Localization = Localization;

        m_hwndWhiteBar = NULL;
    }
    
    virtual void LocalizeButtons ()
    {
        for ( std::map < UINT, std::string >::iterator i = m_Localization.begin (); i != m_Localization.end (); ++i )
        {
            HWND hWnd = ::GetDlgItem ( m_hwndMessageBox, i -> first );
            
            if  ( hWnd )
            {
                ::SetWindowText ( hWnd, i -> second.c_str () );
            }
        }
    }
    
    virtual void Init ()
    {
        if  ( m_hIcon )
        {
            ::SendMessage ( m_hwndMessageBox, WM_SETICON, ( WPARAM ) TRUE, ( LPARAM ) m_hIcon );
            ::SendMessage ( m_hwndMessageBox, WM_SETICON, ( WPARAM ) FALSE, ( LPARAM ) m_hIcon );
        }
        
        LocalizeButtons ();
        
        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = 7;
        rect.bottom = 30;
        ::MapDialogRect ( m_hwndMessageBox, &rect );
        
        int nWhiteBarHeight = rect.bottom - rect.top;
        int nButtonOffset = rect.right - rect.left;
        
        RECT MsgBoxClientRect;
        ::GetClientRect ( m_hwndMessageBox, &MsgBoxClientRect );
        
        // ищем последнюю кнопку
        for ( HWND hwnd = NULL; hwnd = FindWnd ( hwnd, _T("BUTTON"), rect ); )
        {
        }
        int nButtonHeight = rect.bottom - rect.top;
        
        // определяем на сколько надо сдвинуть кнопки
        int nButtonOffsetX = MsgBoxClientRect.right - rect.right - nButtonOffset;
        
        int OldY = MsgBoxClientRect.bottom - rect.bottom;
        
        int nMsgBoxHeightGrow = nWhiteBarHeight - ( 2 * OldY + nButtonHeight ) + OldY;
        
        int nButtonOffsetY = ( nWhiteBarHeight - nButtonHeight ) / 2 - OldY + OldY;
        
        // create WhiteBar
        m_hwndWhiteBar = ::CreateWindow ( 
            _T("STATIC"), _T(""),
            WS_CHILD | WS_VISIBLE | SS_WHITERECT,
            0, MsgBoxClientRect.bottom - nWhiteBarHeight + nMsgBoxHeightGrow,
            MsgBoxClientRect.right - MsgBoxClientRect.left, nWhiteBarHeight,
            m_hwndMessageBox, NULL, NULL, 0 );
        
        if  ( m_hwndWhiteBar )
        {
            SetWindowPos ( m_hwndWhiteBar, m_hwndMessageBox, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );

            MoveButtons ( nButtonOffsetX, nButtonOffsetY, m_hwndWhiteBar );
            
            ::GetWindowRect ( m_hwndMessageBox, &rect );
            
            ::SetWindowPos ( m_hwndMessageBox, NULL, 0, 0,
                rect.right - rect.left, rect.bottom - rect.top + nMsgBoxHeightGrow,
                SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOREDRAW );
        }
    }
    
private:
    HICON m_hIcon;
    std::map < UINT, std::string > m_Localization;

protected:
    HWND m_hwndWhiteBar;
};

//******************************************************************************
// 
//******************************************************************************
class CCMessageBoxWBCBPatcher : public CCMessageBoxPatcherBase
{
public:
    CCMessageBoxWBCBPatcher ( HICON hIcon, const std::map < UINT, std::string > & Localization, LPCTSTR pszCheckBoxString, bool bCheckBoxState = false )
    :   m_bCheckBoxState ( bCheckBoxState ),
        m_pszCheckBoxString ( pszCheckBoxString ),
        m_hwndCheckBox ( NULL )
    {
        m_hIcon = hIcon;
        
        m_Localization = Localization;
        
        m_hwndWhiteBar = NULL;

    }
    
    virtual void LocalizeButtons ()
    {
        for ( std::map < UINT, std::string >::iterator i = m_Localization.begin (); i != m_Localization.end (); ++i )
        {
            HWND hWnd = ::GetDlgItem ( m_hwndMessageBox, i -> first );
            
            if  ( hWnd )
            {
                ::SetWindowText ( hWnd, i -> second.c_str () );
            }
        }
    }
    
    virtual void Init ()
    {
        if  ( m_hIcon )
        {
            ::SendMessage ( m_hwndMessageBox, WM_SETICON, ( WPARAM ) TRUE, ( LPARAM ) m_hIcon );
            ::SendMessage ( m_hwndMessageBox, WM_SETICON, ( WPARAM ) FALSE, ( LPARAM ) m_hIcon );
        }
        
        LocalizeButtons ();
        
        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = 7;
        rect.bottom = 30;
        ::MapDialogRect ( m_hwndMessageBox, &rect );
        
        int nWhiteBarHeight = rect.bottom - rect.top;
        int nButtonOffset = rect.right - rect.left;
        
        RECT MsgBoxClientRect;
        ::GetClientRect ( m_hwndMessageBox, &MsgBoxClientRect );
        
        // ищем последнюю кнопку
        for ( HWND hwnd = NULL; hwnd = FindWnd ( hwnd, _T("BUTTON"), rect ); )
        {
        }
        int nButtonHeight = rect.bottom - rect.top;
        
        // определяем на сколько надо сдвинуть кнопки
        int nButtonOffsetX = MsgBoxClientRect.right - rect.right - nButtonOffset;
        int OldY = MsgBoxClientRect.bottom - rect.bottom;
        
        RECT rectCheckBox;
        CalcCheckBoxRect ( rectCheckBox, OldY * 2 );
        
        int nWhiteBarTop = rectCheckBox.bottom + OldY;

        int nButtonOffsetY = nWhiteBarTop + OldY - rect.top;
        
        // create WhiteBar
        m_hwndWhiteBar = ::CreateWindow ( 
            _T("STATIC"), _T(""),
            WS_CHILD | WS_VISIBLE | SS_WHITERECT,
            0, nWhiteBarTop,
            MsgBoxClientRect.right - MsgBoxClientRect.left, nWhiteBarHeight,
            m_hwndMessageBox, NULL, NULL, 0 );
        
        if  ( m_hwndWhiteBar )
        {
            MoveButtons ( nButtonOffsetX, nButtonOffsetY, m_hwndWhiteBar );
            
            m_hwndCheckBox = ::CreateWindowEx ( WS_EX_NOPARENTNOTIFY,
                _T("BUTTON"), m_pszCheckBoxString,
                BS_LEFT | BS_AUTOCHECKBOX | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
                rectCheckBox.left, rectCheckBox.top,
                rectCheckBox.right - rectCheckBox.left, rectCheckBox.bottom - rectCheckBox.top,
                m_hwndMessageBox, NULL, NULL, 0 );
            
            if   ( m_hwndCheckBox )
            {
                // Set up right font
                HFONT font = ( HFONT ) ::SendMessage ( m_hwndMessageBox, WM_GETFONT, 0, 0 );
                
                if  ( font )
                {
                    ::SendMessage ( m_hwndCheckBox, WM_SETFONT, ( WPARAM ) font, FALSE );
                }

                // Initial state
                if   ( m_bCheckBoxState )
                {
                    ::SendMessage ( m_hwndCheckBox, BM_SETCHECK, BST_CHECKED, 0 );
                }
            }

            ::GetWindowRect ( m_hwndMessageBox, &rect );
            
            ::SetWindowPos ( m_hwndMessageBox, NULL, 0, 0,
                rect.right - rect.left, rect.bottom - rect.top + ( nWhiteBarTop + nWhiteBarHeight - ( MsgBoxClientRect.bottom - MsgBoxClientRect.top ) ),
                SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOREDRAW );
        }
    }
    
    bool GetBoxState () const
    {
        return m_bCheckBoxState;
    }
    
    BOOL CalcCheckBoxRect ( RECT & rectCheckBox, int nGap )
    {
        RECT rectTmp;
        
        RECT WndRect;
        ::GetClientRect ( m_hwndMessageBox, &WndRect );
        
        // Look for icon or text window
        HWND hwndTextOrIcon = FindWnd ( NULL, _T("STATIC"), rectTmp );

        if   ( hwndTextOrIcon == NULL )
        {
            return FALSE;
        }
        
        // Got the .left value, vertical gap, and, possibly, .bottom
        rectCheckBox.left = rectTmp.left;
        rectCheckBox.right = WndRect.right - 2 * rectTmp.left;
        rectCheckBox.top = rectTmp.bottom;
        
        // Look for text (in case we found an icon)
        hwndTextOrIcon = FindWnd ( hwndTextOrIcon, _T("STATIC"), rectTmp );
        
        if   ( hwndTextOrIcon == NULL )
        {
            return FALSE;
        }
        
        rectCheckBox.top = max ( rectCheckBox.top, rectTmp.bottom ) + nGap;
        
        // Now figure out height of the text
        HDC hdc = ::GetWindowDC ( m_hwndMessageBox );
        
        if   ( hdc == NULL )
        {
            return FALSE;
        }
        
        rectTmp.left = ::GetSystemMetrics ( SM_CXMENUCHECK );
        rectTmp.right = rectCheckBox.right - rectCheckBox.left;
        rectTmp.top = 0;
        rectTmp.bottom = 0x4000;
		

		HFONT font = ( HFONT ) ::SendMessage ( m_hwndMessageBox, WM_GETFONT, 0, 0 );
		
        HGDIOBJ OldFont = SelectObject ( hdc, font );
        
        ::DrawText ( hdc, m_pszCheckBoxString, _tcslen ( m_pszCheckBoxString ), &rectTmp, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK );
        
        SelectObject ( hdc, OldFont );

        ::ReleaseDC ( m_hwndMessageBox, hdc );
        
        // Got the .top
        rectCheckBox.right = rectCheckBox.left + ::GetSystemMetrics ( SM_CXMENUCHECK ) + rectTmp.right - rectTmp.left + 10;
        rectCheckBox.bottom = rectCheckBox.top + rectTmp.bottom;

        return TRUE;
    }
    
    virtual void OnDestroyWindow ()
    {
        m_bCheckBoxState = ( BST_CHECKED == ::SendMessage ( m_hwndCheckBox, BM_GETCHECK, 0, 0 ) );
    }
    
private:
    HICON m_hIcon;
    std::map < UINT, std::string > m_Localization;
    
protected:
    HWND m_hwndWhiteBar;

    
private:
    HWND m_hwndCheckBox;
    bool m_bCheckBoxState;
    LPCTSTR m_pszCheckBoxString;
};

#pragma warning(pop)

#endif // __COMMON_MSGBOX_INTERNAL_H

//==============================================================================

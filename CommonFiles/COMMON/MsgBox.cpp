//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "MsgBoxInternal.h"
#include "MsgBox.h"
#include "resource.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxEx ( HWND hWnd, LPCTSTR pszText, LPCSTR pszCaption, UINT nType, HICON hIcon )
{
    CString caption;

    if  ( pszCaption == NULL )
    {
        CWinApp * pApp = AfxGetApp ();

        if  ( pApp )
        {
            caption = pApp -> m_pszAppName;
        }
        else
        {
            TCHAR szAppName [ _MAX_PATH ];

            if  ( GetModuleFileName ( NULL, szAppName, sizeof ( szAppName ) / sizeof ( szAppName [ 0 ] ) ) )
            {
                caption = szAppName;
            }
        }
    }
    else
    {
        caption = pszCaption;
    }
    
    CString str;

    std::map < UINT, std::string > Localization;

    str.LoadString ( IDS_COMMON_BUTTON_OK_STR );
    Localization [ IDOK ] = ( LPCSTR ) str;

    str.LoadString ( IDS_COMMON_BUTTON_CANCEL_STR );
    Localization [ IDCANCEL ] = ( LPCSTR ) str;

    str.LoadString ( IDS_COMMON_BUTTON_YES_STR );
    Localization [ IDYES ] = ( LPCSTR ) str;
    
    str.LoadString ( IDS_COMMON_BUTTON_NO_STR );
    Localization [ IDNO ] = ( LPCSTR ) str;
    
    CCMessageBoxWhiteBarPatcher patcher ( hIcon, Localization );
    
    return ::MessageBox ( hWnd, pszText, caption, nType );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxEx ( HWND hWnd, UINT nTextId, UINT nCaptionId, UINT nType, HICON hIcon )
{
    CString text;
    text.LoadString ( nTextId );

    CString caption;
    caption.LoadString ( nCaptionId );
    
    return MsgBoxEx ( hWnd, text, caption, nType, hIcon );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxCBEx ( LPCTSTR pszCheckBoxString, bool & bCheckBoxState, HWND hWnd, LPCTSTR pszText, LPCSTR pszCaption, UINT nType, HICON hIcon )
{
    CString caption;
    
    if  ( pszCaption == NULL )
    {
        CWinApp * pApp = AfxGetApp ();
        
        if  ( pApp )
        {
            caption = pApp -> m_pszAppName;
        }
        else
        {
            TCHAR szAppName [ _MAX_PATH ];
            
            if  ( GetModuleFileName ( NULL, szAppName, sizeof ( szAppName ) / sizeof ( szAppName [ 0 ] ) ) )
            {
                caption = szAppName;
            }
        }
    }
    else
    {
        caption = pszCaption;
    }
    
    CString str;
    
    std::map < UINT, std::string > Localization;
    
    str.LoadString ( IDS_COMMON_BUTTON_OK_STR );
    Localization [ IDOK ] = ( LPCSTR ) str;
    
    str.LoadString ( IDS_COMMON_BUTTON_CANCEL_STR );
    Localization [ IDCANCEL ] = ( LPCSTR ) str;
    
    str.LoadString ( IDS_COMMON_BUTTON_YES_STR );
    Localization [ IDYES ] = ( LPCSTR ) str;
    
    str.LoadString ( IDS_COMMON_BUTTON_NO_STR );
    Localization [ IDNO ] = ( LPCSTR ) str;
    
    CCMessageBoxWBCBPatcher patcher ( hIcon, Localization, pszCheckBoxString, bCheckBoxState );
    
    int ret = ::MessageBox ( hWnd, pszText, caption, nType );

    bCheckBoxState = patcher.GetBoxState ();

    return ret;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxCBEx ( LPCTSTR pszCheckBoxString, bool & bCheckBoxState, HWND hWnd, UINT nTextId, UINT nCaptionId, UINT nType, HICON hIcon )
{
    CString text;
    text.LoadString ( nTextId );
    
    CString caption;
    caption.LoadString ( nCaptionId );
    
    return MsgBoxCBEx ( pszCheckBoxString, bCheckBoxState, hWnd, text, caption, nType, hIcon );
}

COMMONEXPORT int MsgBoxOkCancel ( HWND hWnd, LPCTSTR pszText, LPCTSTR pszOKText, LPCTSTR pszCancelText)
{
   CString caption;

    CWinApp * pApp = AfxGetApp ();

    if  ( pApp )
    {
        caption = pApp -> m_pszAppName;
    }
    else
    {
        TCHAR szAppName [ _MAX_PATH ];

        if  ( GetModuleFileName ( NULL, szAppName, sizeof ( szAppName ) / sizeof ( szAppName [ 0 ] ) ) )
        {
            caption = szAppName;
        }
    }
    
    CString str;

    std::map < UINT, std::string > Localization;

    str.LoadString ( IDS_COMMON_BUTTON_OK_STR );
    Localization [ IDOK ] = pszOKText?pszOKText:(LPCSTR)str;

    str.LoadString ( IDS_COMMON_BUTTON_CANCEL_STR );
    Localization [ IDCANCEL ] = pszCancelText?pszCancelText:( LPCSTR ) str;
    
    CCMessageBoxWhiteBarPatcher patcher ( NULL, Localization );
    
    return ::MessageBox ( hWnd, pszText, caption, MB_OKCANCEL|MB_ICONINFORMATION );
}


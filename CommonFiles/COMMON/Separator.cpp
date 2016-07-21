//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "Separator.h"
#include "WinVerDiff.h"

//******************************************************************************
// 
//******************************************************************************
BEGIN_MESSAGE_MAP(CCSeparator, CStatic)
    //{{AFX_MSG_MAP(CCSeparator)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_THEMECHANGED, OnWmThemeChanged)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCSeparator::CCSeparator ( int nIndexColor )
:   m_hTheme ( NULL )
{
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCSeparator::~CCSeparator ()
{
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int CCSeparator::OnCreate ( LPCREATESTRUCT lpCreateStruct )
{
    if   ( CStatic::OnCreate ( lpCreateStruct ) == -1 )
    {
        return -1;
    }
    
    if   ( CCXPThemeHelper::CanApplyXPStyle () )
    {
        m_hTheme = CCXPThemeHelper::OpenThemeData ( m_hWnd, L"BUTTON" );
    }
    else
    {
        m_hTheme = NULL;
    }
    
    return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCSeparator::OnDestroy ()
{
    if   ( m_hTheme )
    {
        CCXPThemeHelper::CloseThemeData ( m_hTheme );
        m_hTheme = NULL;
    }
    CStatic::OnDestroy ();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
LRESULT CCSeparator::OnWmThemeChanged ( WPARAM wParam, LPARAM lParam )
{
    if   ( m_hTheme )
    {
        CCXPThemeHelper::CloseThemeData ( m_hTheme );
    }
    if   ( CCXPThemeHelper::CanApplyXPStyle () )
    {
        m_hTheme = CCXPThemeHelper::OpenThemeData ( m_hWnd, L"BUTTON" );
    }
    else
    {
        m_hTheme = NULL;
    }
    return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCSeparator::OnPaint ()
{
    if   ( m_hTheme == NULL )
    {
        if   ( CCXPThemeHelper::CanApplyXPStyle () )
        {
            m_hTheme = CCXPThemeHelper::OpenThemeData ( m_hWnd, L"BUTTON" );
        }
    }
    
    CPaintDC dc ( this );
    
    CString str;
    GetWindowText ( str );
    
    CRect rectClient, rectWnd;
    GetClientRect ( rectClient );
    GetWindowRect ( rectWnd );
    
    DWORD dwStyle = GetStyle ();
    
    DWORD dwTextFlags = DT_VCENTER | DT_SINGLELINE | DT_LEFT;
    
    if   ( dwStyle & SS_NOPREFIX )
    {
        dwTextFlags |= DT_NOPREFIX;
    }
    
    if   ( str.IsEmpty () == FALSE )
    {
        str += _T(" ");
    }
    
    if   ( m_hTheme == NULL )
    {
        CFont * pFont = dc.SelectObject ( GetFont () );
        
        CSize size = dc.GetTextExtent ( str );
        
        dc.Draw3dRect (
            size.cx , rectWnd.Height () / 2,
            rectWnd.Width () + 1, 2,
            ::GetSysColor ( COLOR_3DSHADOW ),
            ::GetSysColor ( COLOR_3DHIGHLIGHT ) );
        
        dc.SetBkMode ( ::GetSysColor ( TRANSPARENT ) );
        dc.DrawText ( str, rectClient, dwTextFlags );
        
        dc.SelectObject ( pFont );
    }
    else
    {
        int iPartId = BP_GROUPBOX;
        int iStateId = GBS_NORMAL;
        
        CRect rcContent;
        
        CCXPThemeHelper::GetThemeBackgroundContentRect ( m_hTheme,
            dc.GetSafeHdc (), iPartId, iStateId, rectClient, &rcContent );
        
        if   ( str.IsEmpty () == FALSE )
        {
            BSTR bstr = str.AllocSysString ();
            
            CFont * pFont = dc.SelectObject ( GetFont () );
            
            CRect TextRect;
            CCXPThemeHelper::GetThemeTextExtent ( m_hTheme, dc.GetSafeHdc (),
                iPartId, iStateId, bstr, -1, dwTextFlags, NULL, &TextRect );
            
            CCXPThemeHelper::DrawThemeText ( m_hTheme, dc.GetSafeHdc (),
                iPartId, iStateId, bstr, -1, dwTextFlags, 0, &rectClient );
            
            dc.SelectObject ( pFont );
            
            rectClient.left += TextRect.Width ();
            
            SysFreeString ( bstr );
        }
        rectClient.top = rectClient.bottom -= rectClient.Height () / 2;
        
        CCXPThemeHelper::DrawThemeEdge ( m_hTheme, dc.GetSafeHdc (),
            iPartId, iStateId, &rectClient, EDGE_SUNKEN, BF_FLAT | BF_TOP, NULL );
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
BOOL CCSeparator::OnEraseBkgnd ( CDC * pDC )
{
    CWnd * pParent = GetParent ();
    
    if   ( pParent )
    {
        CBrush * pBrush = CBrush::FromHandle ( ( HBRUSH ) pParent -> SendMessage ( WM_CTLCOLORSTATIC,
            ( WPARAM ) ( pDC -> GetSafeHdc () ), ( LPARAM ) GetSafeHwnd () ) );
        
        if   ( pBrush )
        {
            CRect rc;
            GetClientRect ( &rc );
            pBrush -> UnrealizeObject ();
            
            pDC -> FillRect ( &rc, pBrush );
            
            return TRUE;
        }
    }
    return CStatic::OnEraseBkgnd ( pDC );
}

//==============================================================================

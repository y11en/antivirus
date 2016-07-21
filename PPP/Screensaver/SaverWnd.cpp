//
// SaverWnd.h
//

#pragma warning(disable : 4786)

#include "Screensaver.h"
#include "SaverWnd.h"

#include "resource.h"

#define MAX_MOUSEMOVECOUNT  5

//--------------------------- CSaverWnd -----------------------------

CSaverWnd::WndMap_t CSaverWnd::s_mapWnd ;

CSaverWnd::CSaverWnd(CScreensaver* a_pSaver /*= NULL*/)
    : m_hWnd(NULL),
      m_pSaver(a_pSaver),
      m_dwMouseMoveCount(0)
{
    memset(m_strWindowTitle, 0, sizeof(m_strWindowTitle)) ;
    ::LoadString(NULL, IDS_DESCRIPTION, m_strWindowTitle, sizeof(m_strWindowTitle)) ;

    m_ptPrev.x = -1 ;
    m_ptPrev.y = -1 ;
}

BOOL CSaverWnd::OnNCCreate(CREATESTRUCT* a_cs)
{
    return TRUE ;
}

BOOL CSaverWnd::OnCreate(CREATESTRUCT* a_cs)
{
    return TRUE ;
}

BOOL CSaverWnd::OnActivateApp(BOOL a_blActivated, DWORD a_dwThreadID)
{
    return TRUE ;
}

void CSaverWnd::OnDestroy()
{
    if (m_pSaver &&
        m_pSaver->GetSaverMode() != CScreensaver::esmPreview || 
        m_pSaver->GetSaverMode() != CScreensaver::esmTest)
    {
        m_pSaver->ShutdownSaver() ;
    }
}

void CSaverWnd::OnNCDestroy()
{
    m_hWnd = NULL ;
}

BOOL CSaverWnd::OnPaint()
{
    PAINTSTRUCT ps ;
    BeginPaint(m_hWnd, &ps) ;
    
    RECT rc ;
    ::GetClientRect(m_hWnd, &rc) ;
    FillRect(ps.hdc, &rc, (HBRUSH)(COLOR_WINDOW+1)) ;
    FrameRect(ps.hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH)) ;
    RECT rc2 ;
    int height ;
    rc2 = rc ;
    height = DrawText(ps.hdc, "Kaspersky", -1, &rc, DT_WORDBREAK | DT_CENTER | DT_CALCRECT) ;
    rc = rc2 ;

    rc2.top = (rc.bottom + rc.top - height) / 2 ;

    DrawText(ps.hdc, "Kaspersky", -1, &rc2, DT_WORDBREAK | DT_CENTER ) ;

    ExcludeClipRect(ps.hdc, rc.left, rc.top, rc.right, rc.bottom) ;
    ScreenToClient(m_hWnd, (POINT*)&rc.left) ;
    ScreenToClient(m_hWnd, (POINT*)&rc.right) ;
    FillRect(ps.hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH)) ;
    
    EndPaint(m_hWnd, &ps) ;

    return TRUE ;
}

void CSaverWnd::OnMouseMove(UINT a_unFlag, POINT a_pt)
{
    if (m_pSaver && m_pSaver->GetSaverMode() != CScreensaver::esmFull)
    {
        return ;
    }

    if (m_ptPrev.x != a_pt.x || m_ptPrev.y != a_pt.y)
    {
        m_ptPrev = a_pt ;
        ++m_dwMouseMoveCount ;
    }

    if (m_dwMouseMoveCount > MAX_MOUSEMOVECOUNT)
    {
        m_pSaver->InterruptSaver() ;
    }
}

BOOL CSaverWnd::OnSysCommand(UINT a_unID, LPARAM a_lParam)
{
    if (m_pSaver && m_pSaver->GetSaverMode() == CScreensaver::esmFull)
    {
        switch (a_unID)
        {
        case SC_NEXTWINDOW:
        case SC_PREVWINDOW:
        case SC_SCREENSAVE:
        case SC_CLOSE:
            return TRUE ;
        }
    }

    return FALSE ;
}

LONG CSaverWnd::WndProc(UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam)
{
    switch (a_uMsg)
    {
    case WM_NCCREATE:
        return OnNCCreate(reinterpret_cast<CREATESTRUCT*>(a_lParam)) ;

    case WM_CREATE:
        return OnCreate(reinterpret_cast<CREATESTRUCT*>(a_lParam)) ? 0L : -1L ;
        
    case WM_ACTIVATEAPP:
        return OnActivateApp(a_wParam, a_lParam) ? 0L : -1L ;

    case WM_DESTROY:
        OnDestroy() ;
        return 0L ;
        
    case WM_NCDESTROY:
        OnNCDestroy() ;
        return 0L ;
        
    case WM_PAINT:
        return OnPaint() ? 0L : -1L ;

    case WM_MOUSEMOVE:
        POINT pt ;
        pt.x = LOWORD(a_lParam) ;
        pt.y = HIWORD(a_lParam) ;
        OnMouseMove(a_wParam, pt) ;
        return 0 ;
        
    case WM_SYSCOMMAND:
        if (OnSysCommand(a_wParam, a_lParam))
        {
            return 0 ;
        }
        break ;
    }

    return ::DefWindowProc(m_hWnd, a_uMsg, a_wParam, a_lParam) ;
}

LONG CSaverWnd::WndProcS(HWND a_hWnd, UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam)
{
    switch (a_uMsg)
    {
    case WM_NCCREATE:
    case WM_CREATE:
        {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(a_lParam) ;
            if (cs && cs->lpCreateParams)
            {
                CSaverWnd* pWnd = reinterpret_cast<CSaverWnd*>(cs->lpCreateParams) ;
                s_mapWnd[a_hWnd] = pWnd ;
                pWnd->m_hWnd = a_hWnd ;
                return pWnd->WndProc(a_uMsg, a_wParam, a_lParam) ;
            }
        }
        break ;    
    case WM_NCDESTROY:
    default:
        {
            std::map<HWND, CSaverWnd*>::iterator It = s_mapWnd.find(a_hWnd) ;
            if (It != s_mapWnd.end())
            {
                INT_PTR nResult = (*It).second->WndProc(a_uMsg, a_wParam, a_lParam) ;
                if (a_uMsg == WM_NCDESTROY)
                {
                    s_mapWnd.erase(It) ;
                }

                return nResult ;
            }
        }
        break ;
    }

    return ::DefWindowProc(a_hWnd, a_uMsg, a_wParam, a_lParam) ;
}


//-------------------------------------------------------------------
//
// SaverWnd.h
//

#ifndef __SAVERWND_H__
#define __SAVERWND_H__

#pragma warning(disable : 4786)

#include <windows.h>
#include <map>

class CScreensaver ;

//--------------------------- CSaverWnd -----------------------------

class CSaverWnd
{
    typedef std::map<HWND, CSaverWnd*> WndMap_t ;

public:
    CSaverWnd(CScreensaver* a_pSaver = NULL) ;
    
private:
    BOOL OnNCCreate(CREATESTRUCT* a_cs) ;

    BOOL OnCreate(CREATESTRUCT* a_cs) ;

    BOOL OnActivateApp(BOOL a_blActivated, DWORD a_dwThreadID) ;

    void OnDestroy() ;
    
    void OnNCDestroy() ;

    BOOL OnPaint() ;
    
    void OnMouseMove(UINT a_unFlag, POINT a_pt) ;

    BOOL OnSysCommand(UINT a_unID, LPARAM a_lParam) ;

    LONG WndProc(UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam) ;

private:
    static LONG CALLBACK WndProcS(HWND a_hWnd, UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam) ;

private:
    HWND          m_hWnd ;
    TCHAR         m_strWindowTitle[256] ;
    CScreensaver* m_pSaver ;
    POINT         m_ptPrev ;
    DWORD         m_dwMouseMoveCount ;
    
private:
    static WndMap_t     s_mapWnd ;

private:
    friend CScreensaver ;
};

//-------------------------------------------------------------------

#endif // __SAVERWND_H__
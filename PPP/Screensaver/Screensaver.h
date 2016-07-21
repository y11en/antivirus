//
// Screensaver.h
//

#ifndef __SCREENSAVER_H__
#define __SCREENSAVER_H__

#pragma warning(disable : 4786)

#include <windows.h>
#include <map>

#include "SaverWnd.h"

#define MAX_DISPLAYS    20

//--------------------------- CScreensaver --------------------------

class CScreensaver
{
    struct CMonitorInfo 
    {
        CMonitorInfo()
            : m_hMonitor(NULL)
        {
            memset(m_strDeviceName, 0, sizeof(m_strDeviceName)) ;
            memset(m_strMonitorName, 0, sizeof(m_strMonitorName)) ;
            memset(&m_rcScreen, 0, sizeof(m_rcScreen)) ;
        }

        TCHAR       m_strDeviceName[128] ;
        TCHAR       m_strMonitorName[128] ;
        HMONITOR    m_hMonitor ;
        RECT        m_rcScreen ;
        CSaverWnd   m_Wnd ;
    };

    typedef BOOL (PASCAL *VERIFYPWDPROC)(HWND) ;

public:

    enum ESaverMode {
        esmRegistration   = 0x0,
        esmConfig         = 0x1,
        esmPreview        = 0x2,
        esmFull           = 0x3,
        esmTest           = 0x4,
        esmPasswordChange = 0x5,
    };

public:
    CScreensaver() ;

    ~CScreensaver() ;

    void Run(HINSTANCE a_hInstance) ;
    
    DWORD GetSaverMode() ;

    void InterruptSaver() ;

    void ShutdownSaver() ;

private:
    DWORD ParseCommandLine(LPSTR a_szCmdLine) ;
    
    void DoRegistration() ;

    void DoConfig() ;

    void DoSaver() ;

    void DoChangePassword() ;

    void EnumMonitors() ;

    BOOL CreateSaverWindow() ;

private:
    DWORD         m_dwSaverMode ;
    HWND          m_hWnd ;
    HWND          m_hParentWnd ;
    HINSTANCE     m_hInstance ;
    VERIFYPWDPROC m_VerifySaverPassword ;
    BOOL          m_blIs9x ;
    HMODULE       m_hPasswordDLL ;
    CMonitorInfo  m_MonitoInfo[MAX_DISPLAYS] ;
    DWORD         m_dwNumMonitors ;
};

//-------------------------------------------------------------------

#endif // __SCREENSAVER_H__
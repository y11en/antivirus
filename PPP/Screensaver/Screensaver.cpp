//
// Screensaver.cpp
//

#pragma warning(disable : 4786)

#include <windows.h>
#include <regstr.h>
#include <tchar.h>
#include <string>

#include "ConfigDlg.h"
#include "Screensaver.h"

#include "resource.h"

#define PASSWORD_DLL            "PASSWORD.CPL"
#define PASSWORD_CHAGHE_DLL     "MPR.DLL"
#define PASSWORD_FUN            "VerifyScreenSavePwd"
#define PASSWORD_CHANGE_FUN     "PwdChangePasswordA"
#define WND_CLASS_NAME          "KasperskySaverWndClass"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

//--------------------------- CScreensaver --------------------------

CScreensaver::CScreensaver()
    : m_dwSaverMode(esmRegistration),
      m_hWnd(NULL),
      m_hParentWnd(NULL),
      m_hInstance(NULL),
      m_VerifySaverPassword(NULL),
      m_blIs9x(FALSE),
      m_hPasswordDLL(NULL),
      m_dwNumMonitors(0)
{
    for (DWORD dwIndex = 0; dwIndex < ARRAY_SIZE(m_MonitoInfo); ++dwIndex)
    {
        m_MonitoInfo[dwIndex].m_Wnd.m_pSaver = this ;
    }
}

CScreensaver::~CScreensaver()
{
    if (m_hPasswordDLL)
    {
        ::FreeLibrary(m_hPasswordDLL) ;
        m_hPasswordDLL = NULL ;
    }
}

void CScreensaver::Run(HINSTANCE a_hInstance)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE) ;
    
    m_hInstance = a_hInstance ;

    TCHAR* pstrCmdLine = GetCommandLine() ;
    m_dwSaverMode = ParseCommandLine( pstrCmdLine ) ;

    switch (m_dwSaverMode)
    {
    case esmRegistration:
        DoRegistration() ;
        return ;
        
    case esmConfig:
        DoConfig() ;
        return ;

    case esmPreview:
    case esmFull:
    case esmTest:
        DoSaver() ;
        return ;
    case esmPasswordChange:
        DoChangePassword() ;
        return ;
    }
}

DWORD CScreensaver::GetSaverMode()
{
    return m_dwSaverMode ;
}

void CScreensaver::InterruptSaver()
{
    if (m_dwSaverMode != esmTest && m_dwSaverMode != esmFull)
    {
        return ;
    }

    if (m_blIs9x && m_dwSaverMode == esmFull && m_VerifySaverPassword != NULL)
    {
        BOOL blPasswordOkay = FALSE ;
        blPasswordOkay = m_VerifySaverPassword(m_hWnd) ;
        if (!blPasswordOkay)
        {
            return ;
        }
    }

    ShutdownSaver() ;
}

void CScreensaver::ShutdownSaver()
{
    if (m_dwSaverMode == esmFull)
    {
        BOOL blUnused ;
        ::SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, &blUnused, 0) ;
    }

    if (m_hPasswordDLL != NULL)
    {
        ::FreeLibrary(m_hPasswordDLL) ;
        m_hPasswordDLL = NULL ;
    }

    ::PostQuitMessage(0) ;
}

DWORD CScreensaver::ParseCommandLine(LPTSTR a_szCmdLine)
{
    if (*a_szCmdLine == _T('\"'))
    {
        ++a_szCmdLine ;
        while (*a_szCmdLine != _T('\0') && *a_szCmdLine != _T('\"'))
        {
            ++a_szCmdLine ; 
        }

        if (*a_szCmdLine == _T('\"'))
        {
            ++a_szCmdLine ;
        }
    }
    else
    {
        while (*a_szCmdLine != _T('\0') && *a_szCmdLine != _T(' '))
        {
            ++a_szCmdLine ;
        }

        if (*a_szCmdLine == _T(' '))
        {
            ++a_szCmdLine ;
        }
    }

    // Skip along to the first option delimiter "/" or "-"
    while (*a_szCmdLine != _T('\0') && *a_szCmdLine != _T('/') && *a_szCmdLine != _T('-'))
    {
        ++a_szCmdLine ;
    }

    // If there wasn't one, then must be config mode
    if (*a_szCmdLine == _T('\0'))
    {
        return esmConfig ;
    }

    DWORD dwSaverMode = esmRegistration ;
    switch (*(++a_szCmdLine))
    {
    case _T('c'):
    case _T('C'):
        dwSaverMode = esmConfig ;
        break ;

    case _T('t'):
    case _T('T'):
        return esmTest ;

    case _T('p'):
    case _T('P'):
        dwSaverMode = esmPreview ;
        break ;

    case _T('a'):
    case _T('A'):
        dwSaverMode = esmPasswordChange ;
        break ;

    case _T('r'):
    case _T('R'):
        return esmRegistration ;

    default:
        return esmFull ;
    }

    a_szCmdLine++ ;
    while (*a_szCmdLine && !isdigit(*a_szCmdLine))
    {
        ++a_szCmdLine ;
    }

    if (isdigit(*a_szCmdLine))
    {
        m_hParentWnd = HWND(_ttol(a_szCmdLine)) ;
    }
    else
    {
        m_hParentWnd = NULL;
    }

    return dwSaverMode ;
}

void CScreensaver::DoRegistration()
{
    CHAR szSystemDir[MAX_PATH + 1] ;
    memset(szSystemDir, 0, sizeof(szSystemDir)) ;
    UINT unResult = ::GetSystemDirectory(szSystemDir, sizeof(szSystemDir)) ;

    TCHAR szFileName[MAX_PATH + 1] ;
    ::GetModuleFileName(NULL, szFileName, sizeof(szFileName)) ;


    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
    _tsplitpath(szFileName, drive, dir, fname, ext) ;

    std::string  strNewFilePath ;
    strNewFilePath  = szSystemDir ;
    strNewFilePath += "\\" ;
    strNewFilePath += fname ;
    strNewFilePath += ext ;
    if (::CopyFile(szFileName, strNewFilePath.c_str(), FALSE))
    {
        printf("Copied file \'%s\' to folder \'%s\'.", szFileName, szSystemDir) ;
    }
    else
    {
        printf("Cannot copy file \'%s\' to folder \'%s\'.", szFileName, szSystemDir) ;
    }    
}

void CScreensaver::DoConfig()
{
    CConfigDlg dlg(m_hParentWnd) ;
    dlg.Run() ;
}

void CScreensaver::DoSaver()
{
    OSVERSIONINFO osvi ;
    osvi.dwOSVersionInfoSize = sizeof(osvi) ;
    ::GetVersionEx(&osvi) ;
    m_blIs9x = (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ;

    if (m_dwSaverMode == esmFull && m_blIs9x)
    {
        HKEY hKey = NULL ;
        if (::RegOpenKey(HKEY_CURRENT_USER , REGSTR_PATH_SCREENSAVE , &hKey) == ERROR_SUCCESS)
        {
            DWORD dwVal = 0 ;
            DWORD dwSize = sizeof(dwVal) ;
            if ((::RegQueryValueEx(hKey, REGSTR_VALUE_USESCRPASSWORD, NULL, NULL, (BYTE*)&dwVal, &dwSize) == ERROR_SUCCESS) && dwVal)
            {
                m_hPasswordDLL = ::LoadLibrary(TEXT(PASSWORD_DLL)) ;
                if (m_hPasswordDLL)
                {
                    m_VerifySaverPassword = (VERIFYPWDPROC)::GetProcAddress(m_hPasswordDLL, PASSWORD_FUN) ;
                }
            }

            ::RegCloseKey(hKey) ;
        }
    }

    if (m_dwSaverMode == esmFull)
    {
        BOOL blUnused ;
        ::SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, &blUnused, 0) ;

        EnumMonitors() ;
    }

    if (!CreateSaverWindow())
    {
        return ;
    }

    MSG msg ;
    BOOL blResult ;
    while ((blResult = ::GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (blResult == -1)
        {
            break ;
        }

        ::TranslateMessage(&msg) ;
        ::DispatchMessage(&msg) ;
    }
}

void CScreensaver::DoChangePassword()
{
    HINSTANCE hDll = ::LoadLibrary(TEXT(PASSWORD_CHAGHE_DLL)) ;
    if (hDll == NULL)
    {
        return ;
    }

    typedef DWORD (PASCAL *PWCHGPROC)(LPCSTR, HWND, DWORD, LPVOID) ;
    PWCHGPROC pwd = (PWCHGPROC)::GetProcAddress(hDll, PASSWORD_CHANGE_FUN) ;
    if (pwd != NULL)
    {
        pwd("SCRSAVE", m_hParentWnd, 0, NULL) ;
    }

    ::FreeLibrary(hDll) ;
}

void CScreensaver::EnumMonitors()
{
    DWORD iDevice = 0 ;
    DISPLAY_DEVICE dispdev ;
    DISPLAY_DEVICE dispdev2 ;
    DEVMODE devmode ;
    dispdev.cb = sizeof(dispdev) ;
    dispdev2.cb = sizeof(dispdev2) ;
    devmode.dmSize = sizeof(devmode) ;
    devmode.dmDriverExtra = 0 ;
    while (::EnumDisplayDevices(NULL, iDevice, (DISPLAY_DEVICE*)&dispdev, 0))
    {
        // Ignore NetMeeting's mirrored displays
        if ((dispdev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) == 0)
        {
            // To get monitor info for a display device, call EnumDisplayDevices
            // a second time, passing dispdev.DeviceName (from the first call) as
            // the first parameter.
            ::EnumDisplayDevices(dispdev.DeviceName, 0, (DISPLAY_DEVICE*)&dispdev2, 0) ;

            lstrcpy(m_MonitoInfo[m_dwNumMonitors].m_strDeviceName, dispdev.DeviceString) ;
            lstrcpy(m_MonitoInfo[m_dwNumMonitors].m_strMonitorName, dispdev2.DeviceString) ;
            
            if (dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
            {
                ::EnumDisplaySettings(dispdev.DeviceName, ENUM_CURRENT_SETTINGS, &devmode) ;
                if (dispdev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                {
                    // For some reason devmode.dmPosition is not always (0, 0)
                    // for the primary display, so force it.
                    m_MonitoInfo[m_dwNumMonitors].m_rcScreen.left = 0 ;
                    m_MonitoInfo[m_dwNumMonitors].m_rcScreen.top = 0 ;
                }
                else
                {
                    m_MonitoInfo[m_dwNumMonitors].m_rcScreen.left = devmode.dmPosition.x ;
                    m_MonitoInfo[m_dwNumMonitors].m_rcScreen.top = devmode.dmPosition.y ;
                }
                m_MonitoInfo[m_dwNumMonitors].m_rcScreen.right = m_MonitoInfo[m_dwNumMonitors].m_rcScreen.left + devmode.dmPelsWidth ;
                m_MonitoInfo[m_dwNumMonitors].m_rcScreen.bottom = m_MonitoInfo[m_dwNumMonitors].m_rcScreen.top + devmode.dmPelsHeight ;
                m_MonitoInfo[m_dwNumMonitors].m_hMonitor = ::MonitorFromRect(&m_MonitoInfo[m_dwNumMonitors].m_rcScreen, MONITOR_DEFAULTTONULL) ;
            }

            m_dwNumMonitors++ ;
            if (m_dwNumMonitors == MAX_DISPLAYS)
                break;
        }

        ++iDevice ;
    }
}

BOOL CScreensaver::CreateSaverWindow()
{
    WNDCLASS cls ;
    cls.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
    cls.hIcon         = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_APP)) ;
    cls.lpszMenuName  = NULL ;
    cls.lpszClassName = TEXT(WND_CLASS_NAME) ;
    cls.hbrBackground = (HBRUSH)::GetStockObject(GRAY_BRUSH) ;
    cls.hInstance     = m_hInstance ;
    cls.style         = CS_VREDRAW|CS_HREDRAW ;
    cls.lpfnWndProc   = CSaverWnd::WndProcS ;
    cls.cbWndExtra    = 0 ;
    cls.cbClsExtra    = 0 ;
    RegisterClass(&cls) ;

    RECT rc = {0} ;
    DWORD dwStyle = 0 ;
    switch (m_dwSaverMode)
    {
    case esmPreview:
        ::GetClientRect(m_hParentWnd, &rc) ;
        dwStyle = WS_VISIBLE | WS_CHILD ;
        ::AdjustWindowRect(&rc, dwStyle, FALSE) ;
        m_hWnd = ::CreateWindow(TEXT(WND_CLASS_NAME), m_MonitoInfo[0].m_Wnd.m_strWindowTitle, dwStyle, 
                                rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 
                                m_hParentWnd, NULL, m_hInstance, &m_MonitoInfo[0].m_Wnd) ;
        m_MonitoInfo[0].m_Wnd.m_hWnd = m_hWnd;
        break ;
    case esmTest:
        rc.left = rc.top = 50 ;
        rc.right = rc.left + 600 ;
        rc.bottom = rc.top + 400 ;
        dwStyle = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU ;
        ::AdjustWindowRect(&rc, dwStyle, FALSE) ;
        m_hWnd = ::CreateWindow(TEXT(WND_CLASS_NAME), m_MonitoInfo[0].m_Wnd.m_strWindowTitle, dwStyle, 
                                rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 
                                NULL, NULL, m_hInstance, &m_MonitoInfo[0].m_Wnd) ;
        m_MonitoInfo[0].m_Wnd.m_hWnd = m_hWnd ;
        break ;
    case esmFull:
        dwStyle = WS_VISIBLE | WS_POPUP ;
        for (DWORD dwMonitor = 0; dwMonitor < m_dwNumMonitors; ++dwMonitor)
        {
            if (m_MonitoInfo[dwMonitor].m_hMonitor == NULL)
            {
                continue ;
            }

            rc = m_MonitoInfo[dwMonitor].m_rcScreen ;
            HWND hWnd = NULL ;
            hWnd = ::CreateWindowEx(WS_EX_TOPMOST, TEXT(WND_CLASS_NAME), m_MonitoInfo[0].m_Wnd.m_strWindowTitle, 
                                    dwStyle, rc.left, rc.top, rc.right - rc.left, 
                                    rc.bottom - rc.top, hWnd, NULL, m_hInstance, &m_MonitoInfo[0].m_Wnd) ;
            if (hWnd == NULL)
            {
                return FALSE ;
            }

            if (m_hWnd == NULL)
            {
                m_hWnd = hWnd ;
            }

            m_MonitoInfo[0].m_Wnd.m_hWnd = hWnd ;
        }
    }

    return m_hWnd != NULL ;
}

//-------------------------------------------------------------------
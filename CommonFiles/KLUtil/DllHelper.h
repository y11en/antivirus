//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_DLL_HELPER_H
#define __COMMON_DLL_HELPER_H

#include "defs.h"
#include <tchar.h>

namespace KLUTIL
{

//******************************************************************************
// 
//******************************************************************************
struct KLUTIL_API CCDllHelper
{
public:
    CCDllHelper ()
    {
        m_hModule = NULL;
    }
    
    bool Load ( LPCTSTR lpLibFileName )
    {
        m_hModule = LoadLibrary ( lpLibFileName );

        return ( m_hModule != NULL );
    }
    
    void Free ()
    {
        if  ( m_hModule )
        {
            FreeLibrary ( m_hModule );
            m_hModule = NULL;
        }
    }
    
    FARPROC GetProcAddress ( LPCSTR lpProcName )
    {
        return m_hModule ? ::GetProcAddress ( m_hModule, lpProcName ) : NULL;
    }
    
protected:
    HMODULE m_hModule;
};

//******************************************************************************
// 
//******************************************************************************
class KLUTIL_API CCKernel32Helper
{
public:
    CCKernel32Helper ()
    {
        m_DLL.Load ( "kernel32.DLL" );
        
        m_SetProcessWorkingSetSize = ( fnSetProcessWorkingSetSize ) m_DLL.GetProcAddress ( _T("SetProcessWorkingSetSize") );
    }
    
    ~CCKernel32Helper ()
    {
	    m_DLL.Free ();
	    m_SetProcessWorkingSetSize = NULL;
    }
    
    static BOOL SetProcessWorkingSetSize ( HANDLE hProcess, SIZE_T dwMinimumWorkingSetSize, SIZE_T dwMaximumWorkingSetSize )
    {
        return ( m_SetProcessWorkingSetSize ) ? m_SetProcessWorkingSetSize ( hProcess, dwMinimumWorkingSetSize, dwMaximumWorkingSetSize ) : FALSE;
    }
    
private:
    static CCDllHelper m_DLL;

    typedef BOOL ( WINAPI * fnSetProcessWorkingSetSize ) ( HANDLE, DWORD, DWORD );
    static fnSetProcessWorkingSetSize m_SetProcessWorkingSetSize;
};

//******************************************************************************
// 
//******************************************************************************
class KLUTIL_API CCShell32Helper
{
public:
    CCShell32Helper ()
    {
        m_DLL.Load ( "shell32.dll" );
        
#ifdef UNICODE
        m_SHGetSpecialFolderPath = ( fnSHGetSpecialFolderPath ) m_DLL.GetProcAddress ( _T("SHGetSpecialFolderPathW") );
#else
        m_SHGetSpecialFolderPath = ( fnSHGetSpecialFolderPath ) m_DLL.GetProcAddress ( _T("SHGetSpecialFolderPathA") );
#endif
    }
    
    ~CCShell32Helper ()
    {
	    m_DLL.Free ();
	    m_SHGetSpecialFolderPath = NULL;
    }
    
    static BOOL SHGetSpecialFolderPath ( HWND hwndOwner, LPTSTR lpszPath, int nFolder, BOOL fCreate )
    {
        return ( m_SHGetSpecialFolderPath ) ? m_SHGetSpecialFolderPath ( hwndOwner, lpszPath, nFolder, fCreate ) : FALSE;
    }
    
private:
    static CCDllHelper m_DLL;

    typedef BOOL ( WINAPI * fnSHGetSpecialFolderPath ) ( HWND, LPTSTR, int, BOOL );
    static fnSHGetSpecialFolderPath m_SHGetSpecialFolderPath;
};

} // namespace KLUTIL

#endif // __COMMON_DLL_HELPER_H

//==============================================================================

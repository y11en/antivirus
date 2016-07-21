/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	profiles.cpp
 * \author	Andrew Kazachkov
 * \date	04.04.2003 18:36:25
 * \brief	
 * 
 */

#include "std/base/klbase.h"
#include "std/conv/klconv.h"
#include "std/err/klerrors.h"
#include "std/trc/trace.h"
#include "kca/prss/prssconst.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/helpersi.h"

#ifdef _WIN32
#include <atlbase.h>
#include <shlobj.h>
#endif

#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPRSS;


#ifdef _WIN32
typedef BOOL (STDAPICALLTYPE *pSHGetSpecialFolderPath_t)(
                           HWND     hwnd,
                           LPTSTR   pszPath,
                           int      csidl,
                           BOOL fCreate);
#ifdef UNICODE
    const char szaSHGetSpecialFolderPath[]="SHGetSpecialFolderPathW";
#else
    const char szaSHGetSpecialFolderPath[]="SHGetSpecialFolderPathA";
#endif

static HMODULE g_hShell32=NULL;
static pSHGetSpecialFolderPath_t g_pSHGetSpecialFolderPath=NULL;

namespace KLPRSS
{
    void Profiles_Initialize()
    {
        KLERR_TRY
            DWORD dwVersion = GetVersion(); 
            DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
            DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

            g_hShell32=::LoadLibrary(
                            (dwWindowsMajorVersion >= 5 || LONG(dwVersion) < 0)
                                ?   _T("Shell32.dll")
                                :   _T("ShFolder.dll"));
            if(!g_hShell32)KLSTD_THROW_LASTERROR();

            g_pSHGetSpecialFolderPath=(pSHGetSpecialFolderPath_t)
                    ::GetProcAddress(g_hShell32, szaSHGetSpecialFolderPath);
            if(!g_pSHGetSpecialFolderPath)KLSTD_THROW_LASTERROR();
            
            TCHAR szValue[MAX_PATH] = _T("");
            g_pSHGetSpecialFolderPath(NULL, szValue, CSIDL_APPDATA, TRUE);

        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
            Profiles_Deinitialize();
        KLERR_ENDTRY
    }

    void Profiles_Deinitialize()
    {
        g_pSHGetSpecialFolderPath=NULL;
        if(g_hShell32)
        {
            FreeLibrary(g_hShell32);
            g_hShell32=NULL;
        };
    }
};

static std::wstring KLPRSS_GetAppDataPath()
{
    KLSTD_USES_CONVERSION;
    std::wstring wstrResult;
    TCHAR szValue[MAX_PATH] = _T("");    
    if(long(GetVersion()) < 0)// if 9x
    {
    /*
        This code works outside of logon interactive sesion or while
        impersonated ONLY if registry hive was loaded. So use it only under 9x.
    */
    KLERR_BEGIN
        CRegKey key;
        long lResult=key.Open(
                HKEY_CURRENT_USER,
                _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
                KEY_READ);
        if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);        
        DWORD dwValue=KLSTD_COUNTOF(szValue);
        lResult=key.QueryValue(szValue, _T("AppData"), &dwValue);
        if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
        wstrResult=KLSTD_T2W(szValue);
    KLERR_END(3);
    };
    // At least Windows 95 with IE 4.0 installed required
    if(wstrResult.empty())
    {
        if( g_pSHGetSpecialFolderPath &&
            g_pSHGetSpecialFolderPath(NULL, szValue, CSIDL_APPDATA, TRUE))
        {
            wstrResult=KLSTD_T2W(szValue);
        }
        else
            KLSTD_TRACE0(1, L"Warning: cannot access user profile directory\n");
        /*
        HRESULT hr=SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szValue);
        if(FAILED(hr))KLSTD_THROW_LASTERROR_CODE(hr);
        wstrResult=KLSTD_T2W(szValue);
        HMODULE hShell32=NULL;
        KLERR_TRY
            DWORD dwVersion = GetVersion(); 
            DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
            DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

            hShell32=::LoadLibrary(
                            (dwWindowsMajorVersion >= 5)
                                ?   _T("Shell32.dll")
                                :   _T("ShFolder.dll"));
            if(!hShell32)KLSTD_THROW_LASTERROR();

            pSHGetSpecialFolderPath_t pSHGetSpecialFolderPath=(pSHGetSpecialFolderPath_t)
                    ::GetProcAddress(hShell32, szaSHGetSpecialFolderPath);
            if(!pSHGetSpecialFolderPath)KLSTD_THROW_LASTERROR();
            BOOL bResult=pSHGetSpecialFolderPath(NULL, szValue, CSIDL_APPDATA, TRUE);
            if(!bResult)KLSTD_NOTIMP();
            wstrResult=KLSTD_T2W(szValue);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(hShell32)
                FreeLibrary(hShell32);
        KLERR_ENDTRY
        */
    };
    if(wstrResult.empty())
    {
        if(!GetTempPath(KLSTD_COUNTOF(szValue), szValue))
            KLSTD_THROW_LASTERROR();
    };
    return wstrResult;
};

KLCSKCA_DECL std::wstring KLPRSS_MakeUserDataPath()
{
    KLSTD_USES_CONVERSION;
    std::wstring wstrPath=KLPRSS_GetAppDataPath();
    std::wstring wstrResult;
    KLSTD_MakePath(wstrPath, KLPRSS::c_szwDirUserData, L"", wstrResult);
    if(!CreateDirectory(W2CT(wstrResult.c_str()), NULL))
    {
        DWORD dwError=GetLastError();
        if(dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
            KLSTD_THROW_LASTERROR_CODE(dwError);
    };
    return wstrResult;
};

#else
//    #error "Not implemented !"
KLCSKCA_DECL std::wstring KLPRSS_MakeUserDataPath()
{
  return std::wstring( L"/tmp" );
}
#endif


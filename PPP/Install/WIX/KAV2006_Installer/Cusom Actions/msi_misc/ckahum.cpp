#include "stdafx.h"
#include "msi_misc.h"
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

typedef int (*tCKAHUM_Initialize) (HKEY hKey, LPCWSTR szSubKey, void* logger);
typedef int (*tCKAHUM_Deinitialize) (void* logger);
typedef int (*tCKAHUM_Reload) (LPCWSTR szManifestFilePath);
typedef int (*tCKAHUM_Uninstall) ();

MSIHANDLE g_hInstall;
void __stdcall CKAHUM_Logger(int Level, LPCSTR str)
{
    installLog(g_hInstall, "CKAHUM: %s", (char*)str);
}

MSI_MISC_API UINT __stdcall CKAHUM_Reload(MSIHANDLE hInstall)
{
    using namespace MsiSupport::Utility;
    UINT res = ERROR_INSTALL_FAILURE;

    if (GetVersion() < 0x80000000)
    {
        //NT
        installLogW(hInstall, L"CKAHUM_Reload: Unicode entry point");

        wstring sData;

        if (CAGetDataW(hInstall, sData))
        {
            CADataParser<wchar_t> parser;
		    if (parser.parse(sData))
		    {
                const wstring& sDir   = parser.value(L"Dir");
                const wstring& sBases = parser.value(L"Bases");
                const wstring& sReg   = parser.value(L"Reg");
                installLogW(hInstall, L"CKAHUM_Reload: Dir   = '%ls'", sDir.c_str());
                installLogW(hInstall, L"CKAHUM_Reload: Bases = '%ls'", sBases.c_str());
                installLogW(hInstall, L"CKAHUM_Reload: Reg   = '%ls'", sReg.c_str());

			    wchar_t szCurDir[MAX_PATH];
			    if(GetCurrentDirectoryW(MAX_PATH, szCurDir))
				    installLogW(hInstall, L"CKAHUM_Reload: CurDir before = '%ls'", szCurDir);

			    SetCurrentDirectoryW(sDir.c_str());

			    if(GetCurrentDirectoryW(MAX_PATH, szCurDir))
				    installLogW(hInstall, L"CKAHUM_Reload: CurDir after = '%ls'", szCurDir);

			    wstring sDLL = sDir + L"ckahum.dll";
			    HMODULE hDLL = LoadLibraryExW(sDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			    if (hDLL)
			    {
				    tCKAHUM_Initialize pInitialize = (tCKAHUM_Initialize)GetProcAddress(hDLL, "CKAHUM_Initialize");
				    tCKAHUM_Deinitialize    pDeinitialize = (tCKAHUM_Deinitialize) GetProcAddress(hDLL, "CKAHUM_Deinitialize");
				    tCKAHUM_Reload     pReload     = (tCKAHUM_Reload)    GetProcAddress(hDLL, "CKAHUM_Reload");
				    if (pInitialize && pReload)
				    {
					    g_hInstall = hInstall;

					    pInitialize(HKEY_LOCAL_MACHINE, sReg.c_str(), CKAHUM_Logger);
					    pReload(sBases.c_str());
					    pDeinitialize(CKAHUM_Logger);

					    res=ERROR_SUCCESS;

					    g_hInstall = 0;
				    }
				    else
                        installLogW(hInstall, L"CKAHUM_Reload: functions not found: 0x%08X", GetLastError());

				    FreeLibrary(hDLL);
			    }
			    else
                    installLogW(hInstall, L"CKAHUM_Reload: failed loading %ls: 0x%08X", sDLL.c_str(), GetLastError());
		    }
		    else
                installLogW(hInstall, L"CKAHUM_Reload: parsing cadata failed: %ls", sData.c_str());
        }
        else
            installLogW(hInstall, L"CKAHUM_Reload: getting cadata failed");
    }
    else
    {
        //9x
        installLog(hInstall,"CKAHUM_Reload: ANSI entry point");

        cstring sData;

        if (CAGetData(hInstall, sData))
        {
		    CADataParser<char> parser;
		    if (parser.parse(sData))
		    {
                const cstring& sDir   = parser.value("Dir");
                const cstring& sBases = parser.value("Bases");
			    const cstring& sReg   = parser.value("Reg");
                installLog(hInstall,"CKAHUM_Reload: Dir   = '%s'", sDir.c_str());
                installLog(hInstall,"CKAHUM_Reload: Bases = '%s'", sBases.c_str());
                installLog(hInstall,"CKAHUM_Reload: Reg   = '%s'", sReg.c_str());

			    char szCurDir[MAX_PATH];
			    if(GetCurrentDirectory(MAX_PATH, szCurDir))
				    installLog(hInstall,"CKAHUM_Reload: CurDir before = '%s'", szCurDir);

			    SetCurrentDirectory(sDir.c_str());

			    if(GetCurrentDirectory(MAX_PATH, szCurDir))
				    installLog(hInstall,"CKAHUM_Reload: CurDir after = '%s'", szCurDir);

			    cstring sDLL = sDir + "ckahum.dll";
			    HMODULE hDLL = LoadLibraryEx(sDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			    if (hDLL)
			    {
				    tCKAHUM_Initialize pInitialize = (tCKAHUM_Initialize)GetProcAddress(hDLL, "CKAHUM_Initialize");
				    tCKAHUM_Deinitialize    pDeinitialize = (tCKAHUM_Deinitialize) GetProcAddress(hDLL, "CKAHUM_Deinitialize");
				    tCKAHUM_Reload     pReload     = (tCKAHUM_Reload)    GetProcAddress(hDLL, "CKAHUM_Reload");
				    if (pInitialize && pReload)
				    {
					    g_hInstall = hInstall;

                        wchar_t wsRegKey[MAX_PATH];
					    MultiByteToWideChar( CP_ACP, 0, sReg.c_str(), strlen(sReg.c_str())+1, wsRegKey,  _countof(wsRegKey));
					    pInitialize(HKEY_LOCAL_MACHINE, wsRegKey, CKAHUM_Logger);

                        wchar_t wsFolder[MAX_PATH];
					    MultiByteToWideChar( CP_ACP, 0, sBases.c_str(), strlen(sBases.c_str())+1, wsFolder,  _countof(wsFolder));
					    pReload(wsFolder);

					    pDeinitialize(CKAHUM_Logger);

					    res=ERROR_SUCCESS;

					    g_hInstall = 0;
				    }
				    else
                        installLog(hInstall,"CKAHUM_Reload: functions not found: 0x%08X", GetLastError());

				    FreeLibrary(hDLL);
			    }
			    else
                    installLog(hInstall, "CKAHUM_Reload: failed loading %s: 0x%08X", sDLL.c_str(), GetLastError());
		    }
		    else
                installLog(hInstall, "CKAHUM_Reload: parsing cadata failed: %s", sData.c_str());
        }
        else
            installLog(hInstall,"CKAHUM_Reload: getting cadata failed");
    }
    return res;
}

MSI_MISC_API UINT __stdcall CKAHUM_Uninstall(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
    UINT res = ERROR_INSTALL_FAILURE;

    if (GetVersion() < 0x80000000)
    {
        //NT
        installLogW(hInstall, L"CKAHUM_Uninstall: Unicode entry point");
        wstring sData;

	    if (CAGetDataW(hInstall, sData))
	    {
		    CADataParser<wchar_t> parser;

		    if (parser.parse(sData))
		    {
			    const wstring& sDir = parser.value(L"Dir");
			    const wstring& sReg = parser.value(L"Reg");
    			
			    wchar_t szCurDir[MAX_PATH];
			    if(GetCurrentDirectoryW(MAX_PATH, szCurDir))
				    installLogW(hInstall, L"CKAHUM_Uninstall: CurDir before = '%ls'", szCurDir);

			    SetCurrentDirectoryW(sDir.c_str());

			    if(GetCurrentDirectoryW(MAX_PATH, szCurDir))
				    installLogW(hInstall, L"CKAHUM_Uninstall: CurDir after = '%ls'", szCurDir);

			    wstring sDLL = sDir + L"ckahum.dll";
			    HMODULE hDLL = LoadLibraryExW(sDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			    if (hDLL)
			    {
				    tCKAHUM_Initialize      pInitialize   = (tCKAHUM_Initialize)   GetProcAddress(hDLL, "CKAHUM_Initialize");
				    tCKAHUM_Deinitialize    pDeinitialize = (tCKAHUM_Deinitialize) GetProcAddress(hDLL, "CKAHUM_Deinitialize");
				    tCKAHUM_Uninstall       pUninstall    = (tCKAHUM_Uninstall)    GetProcAddress(hDLL, "CKAHUM_Uninstall");
				    if (pInitialize && pDeinitialize && pUninstall)
				    {
					    g_hInstall = hInstall;

					    pInitialize(HKEY_LOCAL_MACHINE, sReg.c_str(), CKAHUM_Logger);
					    pUninstall();
					    pDeinitialize(CKAHUM_Logger);

					    res=ERROR_SUCCESS;

					    g_hInstall = 0;
				    }
				    else
                        installLogW(hInstall, L"CKAHUM_Uninstall: functions not found: 0x%08X", GetLastError());

				    FreeLibrary(hDLL);
			    }
                else
                    installLogW(hInstall, L"CKAHUM_Uninstall: failed loading %ls: 0x%08X", sDLL.c_str(), GetLastError());
		    }
            else
                installLogW(hInstall, L"CKAHUM_Uninstall: parsing cadata failed: %ls", sData.c_str());

	    }
	    else
            installLogW(hInstall, L"CKAHUM_Uninstall: getting cadata failed");
    }
    else
    {
        //9x
        installLog(hInstall,"CKAHUM_Uninstall: ANSI entry point");
	    cstring sData;

	    if (CAGetData(hInstall, sData))
	    {
		    CADataParser<char> parser;

		    if (parser.parse(sData))
		    {
			    const cstring& sDir = parser.value("Dir");
			    const cstring& sReg = parser.value("Reg");
    			
			    char szCurDir[MAX_PATH];
			    if(GetCurrentDirectory(MAX_PATH, szCurDir))
				    installLog(hInstall,"CKAHUM_Uninstall: CurDir before = '%s'", szCurDir);

			    SetCurrentDirectory(sDir.c_str());

			    if(GetCurrentDirectory(MAX_PATH, szCurDir))
				    installLog(hInstall,"CKAHUM_Uninstall: CurDir after = '%s'", szCurDir);

			    cstring sDLL = sDir + "ckahum.dll";
			    HMODULE hDLL = LoadLibraryEx(sDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			    if (hDLL)
			    {
				    tCKAHUM_Initialize      pInitialize   = (tCKAHUM_Initialize)   GetProcAddress(hDLL, "CKAHUM_Initialize");
				    tCKAHUM_Deinitialize    pDeinitialize = (tCKAHUM_Deinitialize) GetProcAddress(hDLL, "CKAHUM_Deinitialize");
				    tCKAHUM_Uninstall       pUninstall    = (tCKAHUM_Uninstall)    GetProcAddress(hDLL, "CKAHUM_Uninstall");
				    if (pInitialize && pDeinitialize && pUninstall)
				    {
					    g_hInstall = hInstall;

	                    WCHAR wsRegKey[MAX_PATH];
					    MultiByteToWideChar( CP_ACP, 0, sReg.c_str(), strlen(sReg.c_str())+1, wsRegKey,  _countof(wsRegKey));
					    pInitialize(HKEY_LOCAL_MACHINE, wsRegKey, CKAHUM_Logger);
					    pUninstall();
					    pDeinitialize(CKAHUM_Logger);

					    res=ERROR_SUCCESS;

					    g_hInstall = 0;
				    }
				    else
                        installLog(hInstall,"CKAHUM_Uninstall: functions not found: 0x%08X", GetLastError());

				    FreeLibrary(hDLL);
			    }
                else
                    installLog(hInstall, "CKAHUM_Uninstall: failed loading %s: 0x%08X", sDLL.c_str(), GetLastError());
		    }
            else
                installLog(hInstall, "CKAHUM_Uninstall: parsing cadata failed: %s", sData.c_str());

	    }
	    else
            installLog(hInstall,"CKAHUM_Uninstall: getting cadata failed");
    }
	return res;
}

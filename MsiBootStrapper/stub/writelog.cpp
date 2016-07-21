#include <windows.h>
#include <stdio.h>
#include "unicodeconv.h"
/*#include <inst_libs\klssinstlib\klssinstlib.h>
#include <kca\prss\settingsstorage.h>
#include "../../../release_type.h"*/

#define KLCS_MODULENAME L"msi_ss"

#if defined(__WKS) && !defined(__WKS_OSS)
    #define PRODUCT L"Workstation"
#elif defined(__WKS) && defined(__WKS_OSS)
    #define PRODUCT L"SOS"
#endif

/*void ReadSSInstallXML(const char *szSourceDir, char *props, bool& uninstall)
{
    TCHAR szStorage[_MAX_PATH];
    strcpy(szStorage, szSourceDir);
    if (szStorage[strlen(szSourceDir) - 1] != '\\')
        strcat(szStorage, "\\");

    strcat(szStorage, "SS_INSTALL.XML");

    HANDLE hFile = CreateFile(szStorage, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);

        KLERR_TRY
        {

            KLSSINSTLIB_Initialize();

            {
                KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pStorage;

                std::wstring swStorage(__LPWSTR(szStorage));
            
                KLPRSS_OpenSettingsStorageR(swStorage, &pStorage);

                KLSTD::CAutoPtr<KLPAR::Params> pInfo;

                pStorage->Read(PRODUCT, L"5.0.0.0", L"InstallerSettings", &pInfo);

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_INSTALL_DIR", &pValue) && 
                        pValue->GetType() == KLPAR::Value::STRING_T)
                    {
                        char *instdir = __LPSTR(((KLPAR::StringValue*)(KLPAR::Value*)pValue)->GetValue());

                        if (*instdir)
                        {
                            strcat(props, " INSTALLDIR=\"");
                            strcat(props, instdir);
                            strcat(props, "\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_PWDHASH", &pValue) && 
                        pValue->GetType() == KLPAR::Value::STRING_T)
                    {
                        char *pwdhash = __LPSTR(((KLPAR::StringValue*)(KLPAR::Value*)pValue)->GetValue());

                        if (*pwdhash)
                        {
                            strcat(props, " KLADMPWDHASH=\"");
                            strcat(props, pwdhash);
                            strcat(props, "\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_UNINSTALL_PWDHASH", &pValue) && 
                        pValue->GetType() == KLPAR::Value::STRING_T)
                    {
                        char *pwdhash = __LPSTR(((KLPAR::StringValue*)(KLPAR::Value*)pValue)->GetValue());

                        if (*pwdhash)
                        {
                            strcat(props, " KLUNINSTPWDHASH=\"");
                            strcat(props, pwdhash);
                            strcat(props, "\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_REMOVE", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool remove = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (remove)
                        {
                            uninstall = true;
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_DELAY_REBOOT", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool delayreboot = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (delayreboot)
                        {
                            strcat(props, " KLDELAYREBOOT=\"1\"");
                        }
                    }
                }

#if defined(__WKS) && !defined(__WKS_OSS)
                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_USE_ISTREAMS", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool useistreams = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (useistreams)
                        {
                            strcat(props, " KLUSEISTREAMS=\"1\"");
                        }
                        else
                        {
                            strcat(props, " KLUSEISTREAMS=\"\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_USE_IDS", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool useids = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (useids)
                        {
                            strcat(props, " KLUSEIDS=\"1\"");
                        }
                        else
                        {
                            strcat(props, " KLUSEIDS=\"\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_USE_RTP_FILE", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool usertpfile = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (usertpfile)
                        {
                            strcat(props, " KLUSERTPFILE=\"1\"");
                        }
                        else
                        {
                            strcat(props, " KLUSERTPFILE=\"\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_USE_RTP_MAIL", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool usertpmail = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (usertpmail)
                        {
                            strcat(props, " KLUSERTPMAIL=\"1\"");
                        }
                        else
                        {
                            strcat(props, " KLUSERTPMAIL=\"\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_USE_RTP_SCRIPT", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool usertpscript = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (usertpscript)
                        {
                            strcat(props, " KLUSERTPSCRIPT=\"1\"");
                        }
                        else
                        {
                            strcat(props, " KLUSERTPSCRIPT=\"\"");
                        }
                    }
                }

                {
                    KLSTD::CAutoPtr<KLPAR::Value> pValue;
                    if (pInfo->GetValueNoThrow(L"KLINST_SE_USE_RTP_MACRO", &pValue) && 
                        pValue->GetType() == KLPAR::Value::BOOL_T)
                    {
                        bool usertpmacro = ((KLPAR::BoolValue*)(KLPAR::Value*)pValue)->GetValue();

                        if (usertpmacro)
                        {
                            strcat(props, " KLUSERTPMACRO=\"1\"");
                        }
                        else
                        {
                            strcat(props, " KLUSERTPMACRO=\"\"");
                        }
                    }
                }
#endif
            }

            KLSSINSTLIB_Deinitialize();
	    }
        KLERR_CATCH(pError)
	    {
	    }
        KLERR_ENDTRY
    }
}

void WriteSettingsStorage(const char *szTaskId, int res, const char *szResStr, const char *szZombie)
{
    HKEY hWindows;
    TCHAR szProgFiles[_MAX_PATH];
    ULONG size = sizeof(szProgFiles);

    if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", &hWindows) != ERROR_SUCCESS)
    {
        return;
    }
    if (RegQueryValueEx(hWindows, TEXT("ProgramFilesDir"), 0, NULL, (BYTE*)szProgFiles, &size) != ERROR_SUCCESS)
    {
        RegCloseKey(hWindows);
        return;
    }
    RegCloseKey(hWindows);

    bool isfolder = false;

    HKEY hShared;
    TCHAR szFolder[_MAX_PATH];
    size = sizeof(szFolder);

    if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\KasperskyLab\\SharedFiles", &hShared) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hShared, TEXT("Folder"), 0, NULL, (BYTE*)szFolder, &size) == ERROR_SUCCESS)
        {
            isfolder = true;
        }
        RegCloseKey(hShared);
    }

    if (!isfolder)
    {
        sprintf(szFolder, "%s\\Common Files\\", szProgFiles);
        CreateDirectory(szFolder, 0);

        sprintf(szFolder, "%s\\Common Files\\Kaspersky Lab\\", szProgFiles);
        CreateDirectory(szFolder, 0);

        if (RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\KasperskyLab\\SharedFiles", &hShared) == ERROR_SUCCESS)
        {
            RegSetValueEx(hShared, TEXT("Folder"), 0, REG_SZ, (BYTE*)szFolder, strlen(szFolder) + 1);
            RegCloseKey(hShared);
        }
    }

    TCHAR szStorage[_MAX_PATH];
    strcpy(szStorage, szFolder);
    if (szStorage[strlen(szFolder) - 1] != '\\')
        strcat(szStorage, "\\");

    strcat(szStorage, "Data\\");
    CreateDirectory(szStorage, 0);

    strcat(szStorage, "E2S_SUBSCRIPTION.XML");

    KLERR_TRY
    {

        KLSSINSTLIB_Initialize();

        {
            KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pStorage;

            std::wstring swStorage(__LPWSTR(szStorage));
            std::wstring swTaskId(__LPWSTR(szTaskId));

            std::wstring swResStr(__LPWSTR(szResStr));

            std::wstring swZombie;

            if (szZombie) swZombie = __LPWSTR(szZombie);
            
            KLPRSS_OpenSettingsStorageRW(swStorage, &pStorage);

            KLPRSS_MAYEXIST(pStorage->CreateSection(L".core", L"", L""));
            KLPRSS_MAYEXIST(pStorage->CreateSection(L".core", L".independent", L""));
            KLPRSS_MAYEXIST(pStorage->CreateSection(L".core", L".independent", L"SubscriptionData"));

            KLSTD::CAutoPtr<KLPAR::Params> pTaskId;
            {
                KLPAR::param_entry_t par[]=
                {
			        KLPAR::param_entry_t(swTaskId.c_str(), (bool)false),
                };
                KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pTaskId);
            }
            
            KLSTD::CAutoPtr<KLPAR::Params> pTaskIdRsltCode;
            {
                KLPAR::param_entry_t par[]=
                {
			        KLPAR::param_entry_t(swTaskId.c_str(), (long)res),
                };
                KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pTaskIdRsltCode);
            }
            
            KLSTD::CAutoPtr<KLPAR::Params> pTaskIdRsltDesc;
            {
                KLPAR::param_entry_t par[]=
                {
			        KLPAR::param_entry_t(swTaskId.c_str(), swResStr.c_str()),
                };
                KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pTaskIdRsltDesc);
            }
            
            KLSTD::CAutoPtr<KLPAR::Params> pTaskIdDelOnRestart;
            {
                KLPAR::param_entry_t par[]=
                {
			        KLPAR::param_entry_t(swTaskId.c_str(), swZombie.c_str()),
                };
                KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pTaskIdDelOnRestart);
            }
            
            KLSTD::CAutoPtr<KLPAR::Params> pInfo;
            {
                KLPAR::param_entry_t par[]=
                {
			        KLPAR::param_entry_t(L"TASK_ID",                pTaskId),
			        KLPAR::param_entry_t(L"TASK_ID_RSLT_CODE",      pTaskIdRsltCode),
			        KLPAR::param_entry_t(L"TASK_ID_RSLT_DESCR",     pTaskIdRsltDesc),
			        KLPAR::param_entry_t(L"TASK_ID_DEL_ON_RESTART", pTaskIdDelOnRestart),
                };            
                KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pInfo);
            }

            pStorage->Replace(L".core", L".independent", L"SubscriptionData", pInfo);
        }

        KLSSINSTLIB_Deinitialize();
	}
    KLERR_CATCH(pError)
	{
	}
    KLERR_ENDTRY
}*/

void WriteSetupLog(const char * szResFile, const char *szRepFile, int res1, int res2, const char *resstr, const char *szZombie)
{
    if (*resstr)
    {
        HANDLE hFile = CreateFile(szRepFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            DWORD written;
            WriteFile(hFile, resstr, strlen(resstr), &written, NULL);
            CloseHandle(hFile);
        }
    }

    char szRes1[16], szRes2[16];
    sprintf(szRes1, "%d", res1);
    sprintf(szRes2, "%d", res2);

    WritePrivateProfileString("ResponseResult", "ResultCode",       szRes1,    szResFile);
    WritePrivateProfileString("ResponseResult", "ScriptResultCode", szRes2,    szResFile);

    if (szZombie)
        WritePrivateProfileString("ResponseResult", "RebootZombieFile", szZombie,  szResFile);
}


/*void WriteSetupResult(int   res1,
                      int   res2,
                      const char *resstr,
                      bool  silent,
                      const char *taskid, 
                      const char *resfiledir, 
                      const char *zombie
                     )
{
    if (taskid)
    {
        WriteSettingsStorage(taskid, res2, resstr, zombie);
    }
    else
    {
        char _resfiledir[_MAX_PATH] = "";
        char _resfile[_MAX_PATH] = "";
        char _repfile[_MAX_PATH] = "";

        bool writelog = false;
        if (silent)
        {
            strcpy(_resfiledir, resfiledir);
            if (_resfiledir[strlen(_resfiledir) - 1] != '\\') strcat(_resfiledir, "\\");

            strcpy(_resfile, _resfiledir);
            strcat(_resfile, "setup.log");
            writelog = true;

            strcpy(_repfile, _resfiledir);
            strcat(_repfile, "avpsetup.rep");
        }

        char _zombiename[_MAX_PATH] = "";
        if (zombie)
        {
            char *slash = strrchr(zombie, '\\');
            if (slash)
            {
                strcpy(_zombiename, slash + 1);
            }
        }

        if (writelog)
        {
            WriteSetupLog(_resfile, _repfile, res1, res2, resstr, zombie ? _zombiename : 0);
        }
    }
}*/

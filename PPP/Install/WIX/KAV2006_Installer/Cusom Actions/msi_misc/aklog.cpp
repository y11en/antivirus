#include "stdafx.h"
#include "msi_misc.h"
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"
#include <stuffio\Ioutil.h>

void WriteSetupLog(const char * szResFile, int res1, int res2)
{
    char szRes1[16], szRes2[16];
    sprintf(szRes1, "%d", res1);
    sprintf(szRes2, "%d", res2);

    WritePrivateProfileString("ResponseResult", "ResultCode",       szRes1,    szResFile);
    WritePrivateProfileString("ResponseResult", "ScriptResultCode", szRes2,    szResFile);
}


void ReportDeferredCAError(MSIHANDLE hInstall, const char*caname, int Result... )
{
	using namespace MsiSupport::Utility;
	
	//char szSetupLogFile[MAX_PATH];
	//char szAVPSetupRepFile[MAX_PATH];
	cstring sData;

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sKLSetupLog = parser.value("KLSETUPLOG");
			if (!sKLSetupLog.empty())
			{
                installLog(hInstall, "%s: Reporting Result=%d to %s", caname, Result, (char *)sKLSetupLog.c_str());	
				WriteSetupLog(sKLSetupLog.c_str(), Result, 0);	
			}

			const cstring& sAVPSetupRep = parser.value("AVPSETUPREP");
			if (!sAVPSetupRep.empty())
			{
				HANDLE hFile = CreateFile(sAVPSetupRep.c_str(),     // file to create
											GENERIC_WRITE,          // open for writing
											0,                      // do not share
											NULL,                   // default security
											CREATE_ALWAYS,          // overwrite existing
											0,   
											NULL);                  // no attr. template

				if (hFile != INVALID_HANDLE_VALUE) 
				{ 
					DWORD dwBytesWritten; 
					const cstring& sErrorMsg = parser.value("CAERROR");

                    char buffer[0x1000];
                    memset(buffer, 0, sizeof(buffer));
                    va_list list;
                    va_start(list,Result);
                    _vsnprintf(buffer, _countof(buffer), sErrorMsg.c_str(), list);
                    buffer[_countof(buffer) - 1] = 0;
                    va_end(list);

                    installLog(hInstall,"%s: Reporting '%s' to %s", caname, buffer, sAVPSetupRep.c_str());

					WriteFile(hFile, buffer, strlen(buffer),&dwBytesWritten, NULL); 
					CloseHandle(hFile);
				}
				else
                    installLog(hInstall,"%s: Failed to open report %s: 0x%08X", sAVPSetupRep.c_str(), GetLastError());
			}
		}
        else
            installLog(hInstall,"%s: unable to parse CAData for ReportCAError: %s", caname, sData.c_str());
	}
    else
        installLog(hInstall,"%s: unable to get CAData for ReportCAError", caname);
}

MSI_MISC_API UINT __stdcall ReportCAError(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "ReportCAError entry point", MB_OK);
	using namespace MsiSupport::Utility;
	DWORD dwErr = -1;
	char szSetupLogFile[MAX_PATH];
	char szAVPSetupRepFile[MAX_PATH];

	const cstring& sKLSetupDir = GetStrPropValue(hInstall, "KLSETUPDIR");
	
	// Important!  Description of error  must be created before Setup.log!
	strcpy(szAVPSetupRepFile, sKLSetupDir.c_str());
	strcat(szAVPSetupRepFile, "avpsetup.rep");
	HANDLE hFile = CreateFile(szAVPSetupRepFile,     // file to create
								GENERIC_WRITE,          // open for writing
								0,                      // do not share
								NULL,                   // default security
								CREATE_ALWAYS,          // overwrite existing
								0,   
								NULL);                  // no attr. template

	if (hFile != INVALID_HANDLE_VALUE) 
	{ 
		DWORD dwBytesWritten; 
		const cstring& sCAError = GetStrPropValue(hInstall, "CAERROR");

        installLog(hInstall,"ReportCAError: Reporting '%s' to %s", sCAError.c_str(), szAVPSetupRepFile);

		WriteFile(hFile, sCAError.c_str(), sCAError.size(),&dwBytesWritten, NULL); 
		CloseHandle(hFile);
	}
	else
        installLog(hInstall,"ReportCAError: Unable to open report file %s: 0x%08X", szAVPSetupRepFile, dwErr);

	// Important! Setup.log must be created at the end of installation!
	if (!sKLSetupDir.empty())
	{
		strcpy(szSetupLogFile, sKLSetupDir.c_str());
		strcat(szSetupLogFile, "setup.log");
        installLog(hInstall,"ReportCAError: Reporting Result=100 to %s", szSetupLogFile);	
		WriteSetupLog(szSetupLogFile, 100, 0);	
	}
		
	return ERROR_SUCCESS;

} 

MSI_MISC_API UINT __stdcall SetSetupResultSuccess(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetSetupResult entry point", MB_OK);
	using namespace MsiSupport::Utility;

	const cstring& sKLSetupLog = GetStrPropValue(hInstall, "KLSETUPLOG");
	if (!sKLSetupLog.empty())
	{			
		WriteSetupLog(sKLSetupLog.c_str(), 0, 0);	
	}
	
	return ERROR_SUCCESS;

} 

void GenZombieName(char *dir, char *zombie)
{
    char zombiedir[MAX_PATH];
    if (!dir)
    {
        GetTempPath(_MAX_PATH, zombiedir);
    }
    else
    {
        strcpy(zombiedir, dir);
    }

    GetTempFileName(zombiedir, "ZMB", 0, zombie);

    DeleteFile(zombie);
}

MSI_MISC_API UINT __stdcall WriteZombie(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "WriteZombie entry point", MB_OK);
    using namespace MsiSupport::Utility;

	char sDir[_MAX_PATH];
	char sZombieFile[_MAX_PATH];
	char sShortZombieFile[_MAX_PATH];
	
	char szSetupLogFile[MAX_PATH];
	//char szAVPSetupRepFile[MAX_PATH];
	
	const cstring& sKLSetupDir = GetStrPropValue(hInstall, "KLSETUPDIR");		
	if (!sKLSetupDir.empty())
	{
		strcpy(sDir, sKLSetupDir.c_str());
		GenZombieName(sDir, sZombieFile);

		HANDLE hFile = CreateFile(sZombieFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
		if (hFile != INVALID_HANDLE_VALUE)
		{
			GetShortPathName(sZombieFile, sShortZombieFile, MAX_PATH);
			strcpy(szSetupLogFile, sKLSetupDir.c_str());
			strcat(szSetupLogFile, "setup.log");
			WritePrivateProfileString("ResponseResult", "RebootZombieFile", sShortZombieFile, szSetupLogFile);
			IOSMoveFileOnReboot(sZombieFile, NULL);
			CloseHandle(hFile);
		}
	}

    return ERROR_SUCCESS;
}

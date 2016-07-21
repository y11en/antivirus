#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <shlwapi.h>
#include <StuffIO/ioutil.h>
#include "resource.h"

#define SETUPERR_SUCCESS        0
#define SETUPERR_FAIL          -1

#define SETUPERR_USAGE         -4
#define SETUPERR_MODULENAME    -1
#define SETUPERR_COPYTOTEMP    1151
#define SETUPERR_MSIEXEC       1157


int GetExe(char *name, char *dir)
{
    HMODULE hstub = GetModuleHandle(NULL);

    if (hstub == NULL)
    {
        return 0;
    }

    if (GetModuleFileName(hstub, name, _MAX_PATH) == 0)
    {
        return 0;
    }

    strcpy(dir, name);

    for (int i = strlen(dir) - 1; i >= 0; --i)
    {
        if (dir [i] == '\\') 
        {
            dir [i] = '\0';
            break;
        }
    }
    return 1;
}

bool Exec(char * szcmdline, char * szdir, bool wait, int* pretcode)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    //MessageBox(NULL, szcmdline, "cmdline", MB_OK);

    if (!CreateProcess(NULL, szcmdline, NULL, NULL, FALSE, 0, NULL, szdir, &si, &pi))
    {
        return false;
    }

    if (wait)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);

        if (pretcode)
        {
            GetExitCodeProcess(pi.hProcess, (DWORD*)pretcode);
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

int ParseStr(const char *instr, char *outstr)
{
    int state = 0;
    int size = strlen(instr);
	int i, j;
    for (i = 0, j = 0; i < size; ++i )
    {
        if (state == 0)
        {
            if (instr[i] == ' ')
            {
                state = 0;
            }
            else if (instr[i] == '"')
            {
                state = 1;
            }
            else
            {
                state = 2;

                outstr[j++] = instr[i];
            }
        }
        else if (state == 1)
        {
            if (instr[i] == '\\')
            {
                state = 3;
            }
            else if (instr[i] == '"')
            {
                state = 2;
            }
            else
            {
                state = 1;

                outstr[j++] = instr[i];
            }
        }
        else if (state == 2)
        {
            if (instr[i] == ' ')
            {
                state = 0;

                outstr[j++] = '\0';
            }
            else if (instr[i] == '"')
            {
                state = 1;
            }
            else
            {
                state = 2;

                outstr[j++] = instr[i];
            }
        }
        else if (state == 3)
        {
            if (instr[i] == '\"')
            {
                state = 1;

                outstr[j++] = instr[i];
            }
            else
            {
                state = 1;

                outstr[j++] = '\\';
                outstr[j++] = instr[i];
            }
        }
    }
    outstr[j++] = '\0';

    return j;
}

char * GetStrParam(char *& strptr, int& size)
{
    if (size == 0)
    {
        return 0;
    }

    char * res = strptr;

    while (size > 0 && *strptr != '\0')
    {
        ++strptr;
        --size;
    }

    if (size > 0)
    {
        ++strptr;
        --size;
    }

    return res;
}

char * GetDefLog(char *logpath, bool uninstall)
{
    char temppath[_MAX_PATH];
    if (!GetTempPath(sizeof(temppath), temppath))
    {
        return 0;
    }

	SYSTEMTIME st;
	GetLocalTime (&st);
    sprintf(logpath, "%skl-%s-%04i-%02i-%02i-%02i-%02i-%02i.log", 
        temppath,
        uninstall ? "uninstall" : "install",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    return logpath;
}

int CopyItselfToTemp(const char *exename, char *tempfile)
{
    char temppath[_MAX_PATH];
    if (!GetTempPath(sizeof(temppath), temppath))
    {
        return 0;
    }
    if (!GetTempFileName(temppath, "", 0, tempfile))
    {
        return 0;
    }

    if (!CopyFile(exename, tempfile, FALSE))
    {
        return 0;
    }

    return 1;
}

void DeleteItself(const char *exename)
{
    IOSMoveFileOnReboot(exename, NULL);
    //MoveFileEx(exename, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}


bool CheckMSIVersion()
{
    bool validversion = false;

    char msipath[_MAX_PATH];
    
    GetSystemDirectory(msipath, sizeof(msipath));
    strcat(msipath, "\\msi.dll");
    
    HMODULE hMSI = LoadLibrary(msipath);
    if (hMSI)
    {
        DLLGETVERSIONPROC dllgetversion = (DLLGETVERSIONPROC)GetProcAddress(hMSI, "DllGetVersion");
        if (dllgetversion)
        {
            DLLVERSIONINFO dvi = {sizeof(DLLVERSIONINFO)};
            if (dllgetversion(&dvi) == NOERROR)
            {
                if (dvi.dwMajorVersion >= 2)
                {
                    validversion = true;
                }
            }
        }
        FreeLibrary(hMSI);
    }

    return validversion;
}

BOOL IfFileExists(LPCTSTR szFileName, BOOL& bIsDirectory) { 
	BOOL bResult = FALSE; 
    DWORD dwAttr = GetFileAttributes(szFileName);  
	if(0xFFFFFFFF != dwAttr) 
	{ 
	     bIsDirectory = (dwAttr & FILE_ATTRIBUTE_DIRECTORY) 
                			?   TRUE  
                			:   FALSE; 
		bResult = TRUE;
	}; 
	return bResult;
}; 

bool ReadSetupIni(char *szIniFile, char *szPackageName, int szPackageNameSize, char *szProduct, int szProductSize, char *szProductLang, int szProductLangSize, char *szCmdLine, int szCmdLineSize)
{
	//MessageBox(NULL, NULL, "ReadSetupIni entry point", MB_OK);
	
	const TCHAR	szSectionNameStartup[22] = "Startup";
	const TCHAR	szSectionNameLanguages[22] = "Languages";
	TCHAR	szKeyName[MAX_PATH];
	TCHAR	szValue[MAX_PATH];
	TCHAR   szDefault[1] = "";
	

	BOOL  bDirectory;
	
	
	if (IfFileExists( szIniFile, bDirectory))
	{
		// reading PackageName value
		_tcscpy(szKeyName, "PackageName");			
		GetPrivateProfileString ((LPCTSTR) szSectionNameStartup, (LPCTSTR) szKeyName,  (LPCTSTR) szDefault, szPackageName,
			szPackageNameSize, szIniFile);

		// reading Product value
		_tcscpy(szKeyName, "Product");	
		GetPrivateProfileString ((LPCTSTR) szSectionNameStartup, (LPCTSTR) szKeyName,  (LPCTSTR) szDefault, szProduct,
			szProductSize, szIniFile);

		// reading Product value
		_tcscpy(szKeyName, "CmdLine");	
		GetPrivateProfileString ((LPCTSTR) szSectionNameStartup, (LPCTSTR) szKeyName,  (LPCTSTR) szDefault, szCmdLine,
			szCmdLineSize, szIniFile);

		// reading default language value
		_tcscpy(szKeyName, "default");	
		if (GetPrivateProfileString ((LPCTSTR) szSectionNameLanguages, (LPCTSTR) szKeyName,  (LPCTSTR) szDefault, szValue,
			MAX_PATH, szIniFile) !=0)
		{
			sprintf(szProductLang, "0x0%s", szValue);
		}


	}
	else
	{
		return false;			
	}

	return true;
	
}

bool LoadLocString(HINSTANCE hInstance, char *szIniFile, char *szSectionName, int ResId, char *szValue, int szValueSize)
{
	//MessageBox(NULL, NULL, "LoadLocString entry point", MB_OK);
	
	TCHAR   szDefault[1] = "";
	TCHAR	szKeyName[10];
	

	BOOL  bDirectory;
	
	
	if (IfFileExists( szIniFile, bDirectory))
	{
		
		 _itot(ResId, szKeyName, 10);
		GetPrivateProfileString ((LPCTSTR) szSectionName, (LPCTSTR) szKeyName,  (LPCTSTR) szDefault, szValue,
			szValueSize, szIniFile);

	}
	else
	{
		::LoadString(hInstance, ResId, szValue, szValueSize);
		return false;			
	}

	return ERROR_SUCCESS;
	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdline, int nCmdShow )
{
    char exename[_MAX_PATH], exedir[_MAX_PATH];
	TCHAR szSetupIni[MAX_PATH] = "setup.ini";
	TCHAR szIniFile[MAX_PATH];
	TCHAR	szProduct[1024];
	TCHAR	szPackageName[1024];
	TCHAR	szCmdLine[1024];
	TCHAR	szProductLang[5];
	TCHAR	szProductLangIni[_MAX_PATH];

    
    if (!GetExe(exename, exedir))
    {
        return SETUPERR_MODULENAME;
    }

	strcpy(szIniFile, exedir);
	strcat(szIniFile, "\\");
	strcat(szIniFile, szSetupIni); 

    int cmdsize = strlen(cmdline);

    bool issilent = false;
    bool uninstall = false;
	bool administrative = false;
    bool instuninstall = false;
    bool deletethis = false;
    char * exelog = 0;
    char * uninstallcode = 0;
	char * msiarg = 0;

    char * cmdp = (char *)_alloca(cmdsize + 1);
    int cmdpsize = ParseStr(cmdline, cmdp);

    char * cmdpptr = cmdp;
	
	char title[1024];
	char locstr[1024];
    char message[4096];
	
	strcpy(szProductLangIni, exedir);
	strcat(szProductLangIni, "\\");
	strcat(szProductLangIni, szProductLang);
	strcat(szProductLangIni, ".ini");

	if (!ReadSetupIni(szIniFile, szPackageName, sizeof(szPackageName), szProduct, sizeof(szProduct), szProductLang, sizeof(szProductLang), szCmdLine, sizeof(szCmdLine)))
	{
		::LoadString(hInstance, IDS_ERROR_1153, message, sizeof(message) - 1);	
		::LoadString(hInstance, IDS_WINDOWTITLE, title, sizeof(title) - 1);
		MessageBox(NULL, message, title, MB_OK);
		return SETUPERR_FAIL;
	}

    for(char * param = GetStrParam(cmdpptr, cmdpsize); param; param = GetStrParam(cmdpptr, cmdpsize))
    {
        if (param[0] == '-' || param[0] == '/')
        {
            if (param[1] == 'h' || param[1] == 'H' || param[1] == '?')
            {				
				LoadLocString(hInstance, szProductLangIni, szProductLang, IDS_STRING_1203, locstr, sizeof(message)); 
				strcpy(message, locstr);
				strcat(message, "\n\n");
				LoadLocString(hInstance, szProductLangIni, szProductLang, IDS_STRING_1205, locstr, sizeof(message));
				strcat(message, locstr);
				strcat(message, "\n");
				LoadLocString(hInstance, szProductLangIni, szProductLang, IDS_STRING_1637, locstr, sizeof(message));
				strcat(message, locstr);
				strcat(message, "\n");
				LoadLocString(hInstance, szProductLangIni, szProductLang, IDS_STRING_1639, locstr, sizeof(message));
				strcat(message, locstr);
				strcat(message, "\n");
				LoadLocString(hInstance, szProductLangIni, szProductLang, IDS_STRING_1206, locstr, sizeof(message));
				strcat(message, locstr);


                MessageBox(NULL, message, szProduct, MB_OK);
                
                return SETUPERR_USAGE;
            }
            else if (param[1] == 's' || param[1] == 'S')
            {
                issilent = true;
            }
            else if (param[1] == 'd' || param[1] == 'D')
            {
                deletethis = true;
            }
			else if (param[1] == 'a' || param[1] == 'A')
            {
                administrative = true;
            }
            else if (param[1] == 'x' || param[1] == 'X')
            {
                uninstall = true;
                uninstallcode = param + 2;
            }
            else if (param[1] == 'l' || param[1] == 'L')
            {
                exelog = param + 2;
            }
			else if (param[1] == 'v' || param[1] == 'V')
            {                
                msiarg = param + 2;
            }
        }
    }

    int ret = SETUPERR_FAIL;
    int res1 = 0, 
        res2 = 0;
    char resstr[512] = "";

    if (!CheckMSIVersion())
    {
        char instmsi[_MAX_PATH];
        strcpy(instmsi, exedir);

	    OSVERSIONINFO	rcOS;
	    rcOS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	    ::GetVersionEx( &rcOS );

	    if( rcOS.dwPlatformId == VER_PLATFORM_WIN32_NT )
        {
            strcat(instmsi, "\\instmsiw.exe /q");
        }
        else
        {
            strcat(instmsi, "\\instmsia.exe /q");
        }

        int instmsires = 0;

        if (Exec(instmsi, exedir, true, &instmsires))
        {
            if (instmsires == ERROR_SUCCESS_REBOOT_REQUIRED)
            {
                if (issilent)
                {
                    ::LoadString(hInstance, IDS_LOGERR_INSTMSI_REBOOT, resstr, sizeof(resstr) - 1);
                }
                else
                {
                    ::LoadString(hInstance, IDS_MSGERR_INSTMSI_REBOOT, message, sizeof(message) - 1);

                    MessageBox (NULL, message, szProduct, MB_OK);
                }
                return SETUPERR_SUCCESS;
            }
        }
    }
	
    char defexelog[_MAX_PATH];
    if (!exelog)
    {
        // default logging
        exelog = GetDefLog(defexelog, uninstall || instuninstall);
    }

    bool writelog = false;

    int res = SETUPERR_FAIL;
    int msires = 0;

    bool usemsires = false;

    char run[1024];

    if (uninstall)
    {
        if (deletethis)
        {
            // uninstall, start from temporary location, 

            DeleteItself(exename);

            strcpy(run, "msiexec /x");
            strcat(run, uninstallcode);
            if (issilent) strcat(run, "\" /qn");
			if (msiarg)   strcat(run, msiarg);
            if (exelog)   strcat(run, " /l*v \""),
                          strcat(run, exelog),
                          strcat(run, "\"");
			if (strlen(szCmdLine) > 0) strcat(run, " "),
					   strcat(run, szCmdLine);

            if (!Exec(run, exedir, true, &msires))
			{
				ret = SETUPERR_MSIEXEC;
			}
			else
			{
				ret = msires;
			}
        }
        else
        {
            // uninstall, start from installed location,

            char exetemp[_MAX_PATH];
            if (CopyItselfToTemp(exename, exetemp))
            {
                strcpy(run, "\"");
                strcat(run, exetemp);
                strcat(run, "\" /d /x");
                strcat(run, uninstallcode);
                if (issilent)   strcat(run, " /s");
				if (msiarg)   strcat(run, msiarg);
                if (exelog)     strcat(run, " /l\""),
                                strcat(run, exelog),
                                strcat(run, "\"");
				if (strlen(szCmdLine) > 0) strcat(run, " "),
					   strcat(run, szCmdLine);


                if (!Exec(run, exedir, true, &msires))
				{
					ret = SETUPERR_MSIEXEC;
				}
				else
				{
					ret = msires;
				}
            }
            else
            {
                ret = SETUPERR_COPYTOTEMP;
            }
        }
    }
    else if (instuninstall)
    {
        // uninstall, start from external location
		wsprintf(run,"msiexec /x \"%s\"", szPackageName);
        if (issilent) strcat(run, "\" /qn");
		if (msiarg)  strcat(run, msiarg);
        if (exelog)   strcat(run, " /l*v \""),
                      strcat(run, exelog),
                      strcat(run, "\"");
		if (strlen(szCmdLine) > 0) strcat(run, " "),
					   strcat(run, szCmdLine);

        if (!Exec(run, exedir, true, &msires))
        {
            ret = SETUPERR_MSIEXEC;
        }
        else
        {
            ret = msires;
        }
    }
	else if (administrative)
    {
        // administrative install
		wsprintf(run,"msiexec /a \"%s\"", szPackageName);
        if (issilent) strcat(run, "\" /qn");
		if (msiarg)  strcat(run, msiarg);
        if (exelog)   strcat(run, " /l*v \""),
                      strcat(run, exelog),
                      strcat(run, "\"");
		if (strlen(szCmdLine) > 0) strcat(run, " "),
					   strcat(run, szCmdLine);

        if (!Exec(run, exedir, true, &msires))
        {
            ret = SETUPERR_MSIEXEC;
        }
        else
        {
            ret = msires;
        }
    }
    else
    {
        // install
		wsprintf(run,"msiexec /i \"%s\"", szPackageName);
        if (issilent) strcat(run, " /qn");
		if (msiarg)  strcat(run, msiarg);
        if (exelog)   strcat(run, " /l*v \""),
                      strcat(run, exelog),
                      strcat(run, "\"");
		if (strlen(szCmdLine) > 0) strcat(run, " "),
					   strcat(run, szCmdLine);
    
        if (!Exec(run, exedir, true, &msires))
        {
            ret = SETUPERR_MSIEXEC;
        }
        else
        {
            ret = msires;
        }
    }


    return ret;
}

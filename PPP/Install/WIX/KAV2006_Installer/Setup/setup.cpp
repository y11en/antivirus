#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "resource.h"
#include <msi.h>
#include <Msiquery.h>

#if defined(__PRODTYPE_KAV)
    #define IDS_PARAM_MSIPACKAGE IDS_PARAM_MSIPACKAGE_KAV 
    #define IDS_WINDOWTITLE      IDS_WINDOWTITLE_KAV
#elif defined (__PRODTYPE_KIS)
    #define IDS_PARAM_MSIPACKAGE IDS_PARAM_MSIPACKAGE_KIS
    #define IDS_WINDOWTITLE      IDS_WINDOWTITLE_KIS
#elif defined (__PRODTYPE_WKS)
    #define IDS_PARAM_MSIPACKAGE IDS_PARAM_MSIPACKAGE_WKS
    #define IDS_WINDOWTITLE      IDS_WINDOWTITLE_WKS
#elif defined (__PRODTYPE_FS)
    #define IDS_PARAM_MSIPACKAGE IDS_PARAM_MSIPACKAGE_FS
    #define IDS_WINDOWTITLE      IDS_WINDOWTITLE_FS
#elif defined (__PRODTYPE_SOS)
    #define IDS_PARAM_MSIPACKAGE IDS_PARAM_MSIPACKAGE_SOS
    #define IDS_WINDOWTITLE      IDS_WINDOWTITLE_SOS
#else
    #error __PRODTYPE_XXX not defined
#endif


#define SETUPERR_SUCCESS        0
#define SETUPERR_FAIL          -1

#define SETUPERR_USAGE         -4
#define SETUPERR_INTERNAL      -1
#define SETUPERR_COPYTOTEMP    1151
#define SETUPERR_MSIEXEC       1157

void * setup_alloc(size_t size)
{
    return ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void setup_free(void *ptr)
{
    ::HeapFree(::GetProcessHeap(), 0, ptr);
}


char * __cdecl setup_strchr (
        const char * string,
        int ch
        )
{
        while (*string && *string != (char)ch)
                string++;

        if (*string == (char)ch)
                return((char *)string);
        return(NULL);
}

#pragma optimize( "", off )
void __cdecl setup_memzero (
        void *dst,
        size_t count
        )
{
    while (count--) {
        *(char *)dst = (char)0;
        dst = (char *)dst + 1;
    }
}
#pragma optimize( "", on ) 

int setup_getexe(char *name, char *dir)
{
    HMODULE hsetup = GetModuleHandle(NULL);

    if (hsetup == NULL)
    {
        return 0;
    }

    if (GetModuleFileName(hsetup, name, _MAX_PATH) == 0)
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

void StrReverse(char str[])
{
	if (str[2]!='\0')
		StrReverse(str+1);
	while (str[1]!='\0')
	{
		str[0]+=str[1];
		str[1]=str[0]-str[1];
		str[0]=str[0]-str[1];
		str++;
	}
}

bool CheckForValidGUID(char * szGUID)
{
	int size = strlen(szGUID);
	if (size> 38)
		return false;

	return true;
}

bool ConvertGUIDToPackedGuid(char * szGUID, char * szPackedGUID)
{
	TCHAR str1[9];
	TCHAR str2[5];
	TCHAR str3[5];
	TCHAR str4[3];
	TCHAR str5[3];
	TCHAR str6[3];
	TCHAR str7[3];
	TCHAR str8[3];
	TCHAR str9[3];
	TCHAR str10[3];
	TCHAR str11[3];


	lstrcpyn(str1, szGUID + 1, 9);
	StrReverse(str1);
	lstrcpyn(str2, szGUID + 10, 5);
	StrReverse(str2);
	lstrcpyn(str3, szGUID + 15, 5);
	StrReverse(str3);
	lstrcpyn(str4, szGUID + 20, 3);
	StrReverse(str4);
	lstrcpyn(str5, szGUID + 22, 3);
	StrReverse(str5);
	lstrcpyn(str6, szGUID + 25, 3);
	StrReverse(str6);
	lstrcpyn(str7, szGUID + 27, 3);
	StrReverse(str7);
	lstrcpyn(str8, szGUID + 29, 3);
	StrReverse(str8);
	lstrcpyn(str9, szGUID + 31, 3);
	StrReverse(str9);
	lstrcpyn(str10, szGUID + 33, 3);
	StrReverse(str10);
	lstrcpyn(str11, szGUID + 35, 3);
	StrReverse(str11);

	lstrcpy(szPackedGUID, str1);
	lstrcat(szPackedGUID, str2);
	lstrcat(szPackedGUID, str3);
	lstrcat(szPackedGUID, str4);
	lstrcat(szPackedGUID, str5);
	lstrcat(szPackedGUID, str6);
	lstrcat(szPackedGUID, str7);
	lstrcat(szPackedGUID, str8);
	lstrcat(szPackedGUID, str9);
	lstrcat(szPackedGUID, str10);
	lstrcat(szPackedGUID, str11);

	return true;
}

bool fix_package_name(char *exedir, char * szPackageName)
{
    bool res = false;

    HMODULE hMSI= LoadLibrary("msi.dll");
    if (hMSI)
    {
        typedef UINT (WINAPI *tMsiOpenDatabaseA)(
	        LPCSTR      szDatabasePath,
	        LPCSTR      szPersist,       
	        MSIHANDLE*   phDatabase);

        typedef UINT (WINAPI *tMsiDatabaseOpenViewA)(
            MSIHANDLE hDatabase,
	        LPCSTR     szQuery,          
	        MSIHANDLE*  phView);         

        typedef UINT (WINAPI *tMsiViewExecute)(
            MSIHANDLE hView,
	        MSIHANDLE hRecord);   

        typedef UINT (WINAPI *tMsiViewFetch)(
            MSIHANDLE hView,
	        MSIHANDLE  *phRecord);
            
        typedef UINT (WINAPI *tMsiRecordGetStringA)(
            MSIHANDLE hRecord,
	        UINT iField,
	        LPSTR  szValueBuf,
	        DWORD   *pcchValueBuf);

        typedef UINT (WINAPI *tMsiCloseHandle)(MSIHANDLE hAny);

        typedef UINT (WINAPI *tMsiViewClose)(MSIHANDLE hView);

        tMsiOpenDatabaseA       pMsiOpenDatabase     = (tMsiOpenDatabaseA)    GetProcAddress(hMSI, "MsiOpenDatabaseA");
        tMsiDatabaseOpenViewA   pMsiDatabaseOpenView = (tMsiDatabaseOpenViewA)GetProcAddress(hMSI, "MsiDatabaseOpenViewA");
        tMsiViewExecute         pMsiViewExecute      = (tMsiViewExecute)      GetProcAddress(hMSI, "MsiViewExecute");
        tMsiViewFetch           pMsiViewFetch        = (tMsiViewFetch)        GetProcAddress(hMSI, "MsiViewFetch");
        tMsiRecordGetStringA    pMsiRecordGetString  = (tMsiRecordGetStringA) GetProcAddress(hMSI, "MsiRecordGetStringA");
        tMsiCloseHandle         pMsiCloseHandle      = (tMsiCloseHandle)      GetProcAddress(hMSI, "MsiCloseHandle");
        tMsiViewClose           pMsiViewClose        = (tMsiViewClose)        GetProcAddress(hMSI, "MsiViewClose");

        if (pMsiOpenDatabase && 
            pMsiDatabaseOpenView &&
            pMsiViewExecute && 
            pMsiViewFetch &&
            pMsiRecordGetString &&
            pMsiCloseHandle && 
            pMsiViewClose)
        {
	        HKEY    hKey           =   NULL;
	        HKEY    hSubKey           =   NULL;
	        DWORD   dwSizeInBytes  =   MAX_PATH;
	        TCHAR	szPackage[MAX_PATH];
	        TCHAR	szValue[MAX_PATH];
	        TCHAR	szProductCode[37];
	        TCHAR	szPackedGUID[33];
	        TCHAR	szKey[MAX_PATH];
	        TCHAR szQuery[256];
	        MSIHANDLE hView;
	        MSIHANDLE hRecord;
	        DWORD	dwErr;
	        MSIHANDLE hDatabase;

	        strcpy(szPackage, exedir);
	        strcat(szPackage, "\\");
	        strcat(szPackage, szPackageName);
	        dwErr = pMsiOpenDatabase(szPackage, MSIDBOPEN_READONLY, &hDatabase);
	        if (ERROR_SUCCESS == dwErr)
	        {

		        strcpy(szQuery,"SELECT * FROM  Property WHERE Property.Property=\'ProductCode\'");

		        // Open a view using the SQL query statement.
		        if(pMsiDatabaseOpenView(hDatabase, szQuery, &hView) 
											          == ERROR_SUCCESS)
		        {
			        // Execute the view just opened.
			        if(pMsiViewExecute(hView, 0) == ERROR_SUCCESS)
			        {
				        // Fetch the record from the view just executed.
				        if(pMsiViewFetch(hView, &hRecord) == ERROR_SUCCESS)
				        {
					        if (pMsiRecordGetString(hRecord, 2, szProductCode, &dwSizeInBytes) == ERROR_SUCCESS )
					        {
						        if (CheckForValidGUID(szProductCode))
						        {
							        ConvertGUIDToPackedGuid(szProductCode, szPackedGUID);
							        strcpy(szKey, TEXT("Installer\\Products\\"));
							        strcat(szKey, szPackedGUID);
							        strcat(szKey, TEXT("\\SourceList"));
							        dwErr = RegOpenKeyEx( HKEY_CLASSES_ROOT, szKey, 0, 
											          KEY_ALL_ACCESS, &hKey );
							        if (dwErr == ERROR_SUCCESS) 
							        {
								        dwErr = RegQueryValueEx(hKey, "PackageName", NULL, NULL, (LPBYTE)szValue, &dwSizeInBytes);
								        if (dwErr == ERROR_SUCCESS)
								        {
									        if (strcmp(szValue, szPackageName) != 0)
									        {
										        dwErr = RegSetValueEx(hKey, "PackageName", 0, REG_SZ, (CONST BYTE *)szPackageName, (DWORD)lstrlen(szPackageName)+1);
										        if  (dwErr == ERROR_SUCCESS)
										        {
											        res = true;
										        }
									        }
								        }

								        RegCloseKey(hKey);
							        }
						        }
					        }
				        }
			        }
		        }
        		
		        //Close all handles
		        pMsiCloseHandle(hRecord);
		        pMsiViewClose(hView);
		        pMsiCloseHandle(hView);
		        pMsiCloseHandle(hDatabase);

	        }
        }
        FreeLibrary(hMSI);
    }

    return res;
}

bool setup_exec(char * szcmdline, char * szdir, bool wait, int* pretcode)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    setup_memzero( &si, sizeof(si) );
    si.cb = sizeof(si);
    setup_memzero( &pi, sizeof(pi) );

    //MessageBox(NULL, szcmdline, "cmdline", MB_OK);

    if (!::CreateProcess(NULL, szcmdline, NULL, NULL, FALSE, 0, NULL, szdir, &si, &pi))
    {
        return false;
    }

    if (wait)
    {
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        if (pretcode)
        {
            ::GetExitCodeProcess(pi.hProcess, (DWORD*)pretcode);
        }
    }

    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);

    return true;
}

int setup_parsestr(const char *instr, char *outstr)
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

char * setup_getstrparam(char *& strptr, int& size)
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

char *setup_itoa(int i, int n, char *str)
{
    char * p = str;
    switch (n) // almost Duff's device
    {
    case 4: *p++ = '0' + (i / 1000) % 10;
    case 3: *p++ = '0' + (i / 100)  % 10;
    case 2: *p++ = '0' + (i / 10)   % 10;
    case 1: *p++ = '0' + (i )       % 10;
    }
    *p = '\0';

    return str;
}


char * setup_getdeflog(char *logpath, bool uninstall)
{
    char temppath[_MAX_PATH];
    if (!::GetTempPath(sizeof(temppath), temppath))
    {
        return 0;
    }

	SYSTEMTIME st;
    ::GetLocalTime (&st);

    /*
    sprintf(logpath, "%skl-%s-%04i-%02i-%02i-%02i-%02i-%02i.log", 
        temppath,
        uninstall ? "uninstall" : "install",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        */

    char buffer[8];

    strcpy(logpath, temppath);
    strcat(logpath, uninstall ? "kl-uninstall-" : "kl-install-" );
    strcat(logpath, setup_itoa(st.wYear, 4, buffer));
    strcat(logpath, "-");
    strcat(logpath, setup_itoa(st.wMonth, 2, buffer));
    strcat(logpath, "-");
    strcat(logpath, setup_itoa(st.wDay, 2, buffer));
    strcat(logpath, "-");
    strcat(logpath, setup_itoa(st.wHour, 2, buffer));
    strcat(logpath, "-");
    strcat(logpath, setup_itoa(st.wMinute, 2, buffer));
    strcat(logpath, "-");
    strcat(logpath, setup_itoa(st.wSecond, 2, buffer));
    strcat(logpath, ".log");

    return logpath;
}

/*
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
    //IOSMoveFileOnReboot(exename, NULL);
    //MoveFileEx(exename, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}
*/

bool setup_checkmsiversion()
{
    bool validversion = false;

    char msipath[_MAX_PATH];
    
    ::GetSystemDirectory(msipath, sizeof(msipath));
    strcat(msipath, "\\msi.dll");
    
    HMODULE hMSI = ::LoadLibrary(msipath);
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
        ::FreeLibrary(hMSI);
    }

    return validversion;
}

template <typename T>
class setup_autoptr
{
public:
    setup_autoptr(T* ptr = 0) : ptr_(ptr) {};
    ~setup_autoptr() { setup_free(ptr_); }

    T* get() { return ptr_; }

private:
    T* ptr_;
};

char title[1024];
char message[4096];
char run[1024];

int APIENTRY setup_main()
{
    HINSTANCE hinst = ::GetModuleHandle(NULL);
    LPSTR cmdline = ::GetCommandLine();

    char exename[_MAX_PATH], exedir[_MAX_PATH];

    char msipackage[_MAX_PATH];
    
    if (!setup_getexe(exename, exedir))
    {
        return SETUPERR_INTERNAL;
    }

    int cmdsize = strlen(cmdline);

    bool issilent = false;
    bool uninstall = false;
	bool admin = false;
    char * exelog = 0;
    char * uninstallcode = 0;
	char * msiarg = 0;

    setup_autoptr<char> auto_props = (char *)setup_alloc(cmdsize + 1);
    char * props = auto_props.get();

    setup_autoptr<char> auto_cmdp = (char *)setup_alloc(cmdsize + 1);
    char * cmdp = auto_cmdp.get();
    int cmdpsize = setup_parsestr(cmdline, cmdp);

    char * cmdpptr = cmdp;
	
    ::LoadString(hinst, IDS_PARAM_MSIPACKAGE, msipackage, sizeof(msipackage) - 1);

    for(char * param = setup_getstrparam(cmdpptr, cmdpsize); param; param = setup_getstrparam(cmdpptr, cmdpsize))
    {
        if (param[0] == '-' || param[0] == '/')
        {
            if (param[1] == 'h' || param[1] == 'H' || param[1] == '?')
            {				
                ::LoadString(hinst, IDS_WINDOWTITLE, title, sizeof(title) - 1 );
                ::LoadString(hinst, IDS_MSGERR_USAGE, message, sizeof(message) - 1);

                ::MessageBox(NULL, message, title, MB_OK);

                return SETUPERR_USAGE;
            }
            else if (param[1] == 's' || param[1] == 'S')
            {
                issilent = true;
            }
			else if (param[1] == 'a' || param[1] == 'A')
            {
                admin = true;
            }
            else if (param[1] == 'x' || param[1] == 'X')
            {
                uninstall = true;
                uninstallcode = param + 2;
            }
            else if (param[1] == 'p' || param[1] == 'P')
            {
                char *eq = setup_strchr(param + 2, '=');
                if (eq)
                {
                    *eq = '\0';

                    strcat(props, " ");
                    strcat(props, param + 2);
                    strcat(props, "=\"");
                    strcat(props, eq + 1);
                    strcat(props, "\"");
                }
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

    if (!setup_checkmsiversion())
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

        if (setup_exec(instmsi, exedir, true, &instmsires))
        {
            if (instmsires == ERROR_SUCCESS_REBOOT_REQUIRED)
            {
                if (!issilent)
                {
                    ::LoadString(hinst, IDS_WINDOWTITLE, title, sizeof(title) - 1 );
                    ::LoadString(hinst, IDS_MSGERR_INSTMSI_REBOOT, message, sizeof(message) - 1);

                    ::MessageBox (NULL, message, title, MB_OK);
                }
                return SETUPERR_SUCCESS;
            }
        }
    }
	
    char defexelog[_MAX_PATH];
    if (!exelog)
    {
        // default logging
        exelog = setup_getdeflog(defexelog, uninstall);
    }

    bool writelog = false;

    int res = SETUPERR_FAIL;
    int msires = 0;

    bool usemsires = false;


    if (uninstall)
    {
        // uninstall, start from temporary location, 

        strcpy(run, "msiexec /x");
        strcat(run, uninstallcode);
        if (issilent) strcat(run, "\" /qn");
        if (*props)   strcat(run, props);
		if (msiarg)   strcat(run," "),
                      strcat(run, msiarg);
        if (exelog)   strcat(run, " /l*v \""),
                      strcat(run, exelog),
                      strcat(run, "\"");

        if (!setup_exec(run, exedir, true, &msires))
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
        if (admin)    strcpy(run, "msiexec /a \"");
        else          strcpy(run, "msiexec /i \"");
        strcat(run, msipackage);
        strcat(run, "\" REINSTALL=\"ALL\" REINSTALLMODE=\"voums\"");
        if (issilent) strcat(run, " /qn");
        if (*props)   strcat(run, props);
		if (msiarg)   strcat(run, " "),
                      strcat(run, msiarg);
        if (exelog)   strcat(run, " /l*v \""),
                      strcat(run, exelog),
                      strcat(run, "\"");
		
		// fix the problem of error 1316 in MinorUpgrade 
		fix_package_name(exedir, msipackage);
    
        if (!setup_exec(run, exedir, true, &msires))
        {
            ret = SETUPERR_MSIEXEC;
        }
        else
        {
            ret = msires;
        }
    }


    ExitProcess(ret);
    return ret;
}

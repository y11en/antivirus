#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "resource.h"
#include <msi.h>
#include <Msiquery.h>
#include "cleanapi.h"
#include <commctrl.h>



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
    #define IDS_PARAM_MSIPACKAGE IDS_PARAM_MSIPACKAGE_KAV 
    #define IDS_WINDOWTITLE      IDS_WINDOWTITLE_KAV
#endif

#define SETUPERR_SUCCESS        0
#define SETUPERR_FAIL          -1

#define SETUPERR_USAGE         -4
#define SETUPERR_INTERNAL      -1

void * setup_alloc(size_t size)
{
    return ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void setup_free(void *ptr)
{
    ::HeapFree(::GetProcessHeap(), 0, ptr);
}

/*
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
*/
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
/*
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
*/
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

/*char *setup_itoa(int i, int n, char *str)
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
}*/


/*char * setup_getdeflog(char *logpath, bool uninstall)
{
    char temppath[_MAX_PATH];
    if (!::GetTempPath(sizeof(temppath), temppath))
    {
        return 0;
    }

	SYSTEMTIME st;
    ::GetLocalTime (&st);

    
    sprintf(logpath, "%skl-%s-%04i-%02i-%02i-%02i-%02i-%02i.log", 
        temppath,
        uninstall ? "uninstall" : "install",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        

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
}*/



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


#define REBOOTFLAG "avp6_post_uninstall"
#define REBOOTFLAG_INSTALL "avp6_post_install"

static int __stdcall CallbackFunction(LPSTR szProductName, void* nPrgCtrl)
{
	//MessageBox(NULL, szProductName, "CallbackFunction entry point", MB_OK);
	SendMessage((HWND)nPrgCtrl, PBM_STEPIT, 0, 0);
	return 1;
}

int APIENTRY cleanapi_main()
{
	
    HINSTANCE hinst = ::GetModuleHandle(NULL);
    LPSTR cmdline = ::GetCommandLine();
	//MessageBox(NULL, cmdline, "", MB_OK);

    char exename[_MAX_PATH], exedir[_MAX_PATH];
    
    if (!setup_getexe(exename, exedir))
    {
        return SETUPERR_INTERNAL;
    }

    int cmdsize = strlen(cmdline);

    bool rebootpending = false;
    bool afterreboot = false;
	bool admin = false;
    char * afterrebootcode = 0;

    setup_autoptr<char> auto_props = (char *)setup_alloc(cmdsize + 1);
    char * props = auto_props.get();

    setup_autoptr<char> auto_cmdp = (char *)setup_alloc(cmdsize + 1);
    char * cmdp = auto_cmdp.get();
    int cmdpsize = setup_parsestr(cmdline, cmdp);

    char * cmdpptr = cmdp;
	
    //::LoadString(hinst, IDS_PARAM_MSIPACKAGE, msipackage, sizeof(msipackage) - 1);

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
            else if (param[1] == 'r' || param[1] == 'R')
            {
                rebootpending = true;
            }
            else if (param[1] == 'a' || param[1] == 'A')
            {
                afterreboot = true;
                afterrebootcode = param + 2;
            }
        }
    }
	

	// call the function
	int res=ERROR_SUCCESS;
	HWND hDlg=FindWindow("MsiDialogNoCloseClass", NULL);
	HWND nPrgCtrl=FindWindowEx(hDlg, NULL, "msctls_progress32", NULL);
	char	szProdName[MAX_PATH];
	res = remove_competitor_software(exedir, nPrgCtrl, (fn_callback_enumerator)CallbackFunction(szProdName, nPrgCtrl));
	
	DWORD   dwSizeInBytes  =   0;
	char	szValue[MAX_PATH];
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwErr;

	// Reboot needed flag
	if (rebootpending)
	{
		//MessageBox(NULL, "", "rebootpending", MB_OK);
		if (GetVersion() < 0x80000000)
		{
			// NT
			dwErr = RegCreateKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\" REBOOTFLAG, 0, 0, REG_OPTION_VOLATILE,
				KEY_ALL_ACCESS, 0, &hKey, 0 );
			if (dwErr == ERROR_SUCCESS) 
			{
				RegCloseKey(hKey);
			}
			else
				//installLog(hInstall, "EnableRebootPending: can't open Run: 0x%08X", dwErr);
				res = SETUPERR_FAIL;
		}
        /*
		else
		{
			// 9x
			dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, 
						  KEY_ALL_ACCESS, &hKey );
			if (dwErr == ERROR_SUCCESS) 
			{
				dwErr = RegSetValueEx(hKey, REBOOTFLAG, 0, REG_SZ, (LPBYTE)szValue, (DWORD) lstrlen(szValue)+1 );
				if  (dwErr == ERROR_SUCCESS)  
				{
					res=ERROR_SUCCESS;
				}
				else
					res = SETUPERR_FAIL;
					//installLog(hInstall, "EnableRebootPending: can't set " REBOOTFLAG ": 0x%08X", dwErr);
				RegCloseKey(hKey);
			}
			else
				res = SETUPERR_FAIL;
				//installLog(hInstall, "EnableRebootPending: can't open RunOnce: 0x%08X", dwErr);
		}
        */
	}
	if (afterreboot)
	{
		//MessageBox(NULL, afterrebootcode, "afterreboot", MB_OK);
		strcpy(szValue, "msiexec.exe /i\"");
		strcat(szValue, afterrebootcode);
		strcat(szValue, "\"");
		//sprintf(szValue, "msiexec.exe /i\"%s\"", sPackageName);
		dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, 
						  KEY_ALL_ACCESS, &hKey );
		if (dwErr == ERROR_SUCCESS) 
		{
			dwErr = RegSetValueEx(hKey, REBOOTFLAG_INSTALL, 0, REG_SZ, (LPBYTE)szValue, (DWORD) lstrlen(szValue)+1 );
			if  (dwErr == ERROR_SUCCESS)
			{
				res=ERROR_SUCCESS;
			}
			else
				res = SETUPERR_FAIL;
				//installLog(hInstall,"RegSetValueEx failed. Error %d\n", GetLastError());

			RegCloseKey(hKey);
		}
		else
			res = SETUPERR_FAIL;
	}
	
	ExitProcess(res);
    return res;
}

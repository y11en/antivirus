// AntiDialHook.cpp : Defines the entry point for the DLL application.
//
#define PR_IMPEX_DEF

#define _CRTIMP
#define WINVER 0x500 // Win2000 +
#include <windows.h>
#include <ras.h>

#include "r3hook/sputnik.h"
#include "r3hook/hookbase64.h"
#include "r3hook/debug.h"

#include <hook/idriver.h>

#include <Prague/pr_remote.h>
#include <AntiDialer/plugin/p_antidial.h>
#include <AntiDialer/structs/s_antidial.h>

CRITICAL_SECTION g_cs;
bool g_bcs_inited = false;
HMODULE g_hThisModule = NULL;
HMODULE g_hRasApi32 = NULL;
OSVERSIONINFO sVerInfo = {0};

DWORD (__stdcall *fRasGetEntryPropertiesA)( LPCSTR, LPCSTR, LPRASENTRYA, LPDWORD, LPBYTE, LPDWORD ) = NULL;
DWORD (__stdcall *fRasGetCountryInfoA)( LPRASCTRYINFO lpRasCtryInfo, LPDWORD lpdwSize ) = NULL;

tCHAR* plugins_list[] = {
	"params.ppl",
};

EXTERN_C hROOT g_root = NULL;
HMODULE g_hModuleRemotePrague = NULL;

PRRemoteAPI g_RP_API = {0};
BOOL g_bPragueLoadTry    = FALSE;
tERROR g_PragueLoadResult = errOBJECT_NOT_INITIALIZED;
tCHAR g_szProcessName[MAX_PATH*4];


#if !defined(RASEO_PreviewPhoneNumber)
#define RASEO_PreviewPhoneNumber        0x00200000
#endif


#define _ONLY_DEVICE_TYPE_MODEM
//#define _SHOW_DEVICE_TYPE

#define LCASE(c) (c >= 'A' ? ( c <= 'Z' ? (c | 0x20) : c ) : c)

void * __cdecl __memmove (
        void * dst,
        const void * src,
        size_t count
        )
{
        void * ret = dst;

#if defined (_M_MRX000) || defined (_M_ALPHA) || defined (_M_PPC)
        {
        extern void RtlMoveMemory( void *, const void *, size_t count );

        RtlMoveMemory( dst, src, count );
        }
#else  /* defined (_M_MRX000) || defined (_M_ALPHA) || defined (_M_PPC) */
        if (dst <= src || (char *)dst >= ((char *)src + count)) {
                /*
                 * Non-Overlapping Buffers
                 * copy from lower addresses to higher addresses
                 */
                while (count--) {
                        *(char *)dst = *(char *)src;
                        dst = (char *)dst + 1;
                        src = (char *)src + 1;
                }
        }
        else {
                /*
                 * Overlapping Buffers
                 * copy from higher addresses to lower addresses
                 */
                dst = (char *)dst + count - 1;
                src = (char *)src + count - 1;

                while (count--) {
                        *(char *)dst = *(char *)src;
                        dst = (char *)dst - 1;
                        src = (char *)src - 1;
                }
        }
#endif  /* defined (_M_MRX000) || defined (_M_ALPHA) || defined (_M_PPC) */

        return(ret);
}

wchar_t * __cdecl __wcsistr (
        const wchar_t * wcs1,
        const wchar_t * wcs2
        )
{
        wchar_t *cp = (wchar_t *) wcs1;
        wchar_t *s1, *s2;

        while (*cp)
        {
                s1 = cp;
                s2 = (wchar_t *) wcs2;

                while ( *s1 && *s2 && !(LCASE(*s1)-LCASE(*s2)) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);
}


DWORD __cdecl __wcslen (
        const wchar_t * wcs
        )
{
        const wchar_t *eos = wcs;

        while( *eos++ ) ;

        return( (DWORD)(eos - wcs - 1) );
}

int __cdecl __wcsicmp (
        const wchar_t * src,
        const wchar_t * dst
        )
{
        int ret = 0 ;

        while( ! (ret = (int)(LCASE(*src) - LCASE(*dst))) && *dst)
                ++src, ++dst;

        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;

        return( ret );
}

int __cdecl __stricmp (
        const char * src,
        const char * dst
        )
{
        int ret = 0 ;

        while( ! (ret = LCASE(*(unsigned char *)src) - LCASE(*(unsigned char *)dst)) && *dst)
                ++src, ++dst;

        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;

        return( ret );
}


char* __strlwr( char* aSource )
{
	int i = 0;
	while ( aSource [i] ) 
	{
		char c = aSource [i];
		if (c >= 'A' && c <= 'Z')
		aSource [ i ] = c | 0x20;
		i++;
	}
	return aSource;
}

bool FastUni2Ansi(WCHAR* sUniSrc, CHAR* sAnsiDst, DWORD dwNumOfChars)
{
	DWORD i = dwNumOfChars;
	if (dwNumOfChars == -1)
		dwNumOfChars = __wcslen(sUniSrc) + 1;
	for (i=0; i<dwNumOfChars; i++)
	{
		if (sUniSrc[i] < 0x80)
		{
			sAnsiDst[i] = (CHAR)sUniSrc[i];
			if (sUniSrc[i] == 0)
				break;
		}
		else
		{
			DWORD nChars;
			if ((CHAR*)sUniSrc != sAnsiDst)
			{
				int convert_len = dwNumOfChars - i;
				nChars = WideCharToMultiByte(CP_ACP, 0, sUniSrc+i, convert_len, sAnsiDst+i, convert_len, NULL, NULL);
				if (nChars != convert_len)
					return false;
				return true;
			}
			else
			{
				CHAR c;
				nChars = WideCharToMultiByte(CP_ACP, 0, sUniSrc+i, 1, &c, 1, NULL, NULL);
				if (nChars != 1)
					return false;
				sAnsiDst[i] = c;
			}
		}
	}
	return true;
}


BOOL LoadPrague()
{
	EnterCriticalSection(&g_cs);
	if (!g_bPragueLoadTry)
	{
		tCHAR szLoadPath[MAX_PATH];
		tCHAR* pModuleName;

		g_bPragueLoadTry = TRUE;
		
		if (!GetModuleFileName(g_hThisModule, szLoadPath, sizeof(szLoadPath)))
			*szLoadPath = 0;
		pModuleName = strrchr(szLoadPath, '\\');
		if (!pModuleName)
			pModuleName = szLoadPath;
		else
			pModuleName++;
		
		strcpy(pModuleName, PR_REMOTE_MODULE_NAME);
		g_hModuleRemotePrague = LoadLibraryEx(szLoadPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		
		if (!g_hModuleRemotePrague)
		{
			ODS(("KAV Anti-Dialer: cannot load module '%s%", szLoadPath));
		}
		else
		{
			tERROR ( pr_call *fGetAPI )(PRRemoteAPI *api) = NULL;
			
			*(void**)&fGetAPI = GetProcAddress(g_hModuleRemotePrague, "PRGetAPI");
			if( fGetAPI )
				g_PragueLoadResult = fGetAPI(&g_RP_API);
			else
				g_PragueLoadResult = errOBJECT_CANNOT_BE_INITIALIZED;
			if (PR_SUCC(g_PragueLoadResult))
				g_PragueLoadResult = g_RP_API.Initialize(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,NULL);
			if (PR_SUCC(g_PragueLoadResult))
				g_PragueLoadResult = g_RP_API.GetRoot(&g_root);
			if (PR_SUCC(g_PragueLoadResult))
			{
				for (int i=0; i<countof(plugins_list); i++)
				{
					hPLUGIN ppl;
					g_PragueLoadResult = g_root->LoadModule(&ppl,plugins_list[i],(tDWORD)strlen(plugins_list[i]),cCP_ANSI);
					if (PR_FAIL(g_PragueLoadResult))
					{
						ODS(("KAV Anti-Dialer: cannot load module '%s'", plugins_list[i]));
						break;
					}
				}
			}
		}
	}
	LeaveCriticalSection(&g_cs);
	return (PR_SUCC(g_PragueLoadResult));
}

// This function compares the passed in "suite name" string
// to the product suite information stored in the registry.
// This only works on the Terminal Server 4.0 platform.

BOOL ValidateProductSuite (LPSTR SuiteName)
{
    BOOL rVal = FALSE;
    LONG Rslt;
    HKEY hKey = NULL;
    DWORD Type = 0;
    DWORD Size = 0;
    LPSTR ProductSuite = NULL;
    LPSTR p;

    Rslt = RegOpenKeyA(
        HKEY_LOCAL_MACHINE,
        "System\\CurrentControlSet\\Control\\ProductOptions",
        &hKey
        );

    if (Rslt != ERROR_SUCCESS)
        goto exit;

    Rslt = RegQueryValueExA( hKey, "ProductSuite", NULL, &Type, NULL, &Size );
    if (Rslt != ERROR_SUCCESS || !Size)
        goto exit;

    ProductSuite = (LPSTR) LocalAlloc( LPTR, Size );
    if (!ProductSuite)
        goto exit;

    Rslt = RegQueryValueExA( hKey, "ProductSuite", NULL, &Type,
        (LPBYTE) ProductSuite, &Size );
     if (Rslt != ERROR_SUCCESS || Type != REG_MULTI_SZ)
        goto exit;

    p = ProductSuite;
    while (*p)
    {
        if (lstrcmpA( p, SuiteName ) == 0)
        {
            rVal = TRUE;
            break;
        }
        p += (lstrlenA( p ) + 1);
    }

exit:
    if (ProductSuite)
        LocalFree( ProductSuite );

    if (hKey)
        RegCloseKey( hKey );

    return rVal;
}
// This function performs the basic check to see if
// the platform on which it is running is Terminal
// services enabled.  Note, this code is compatible on
// all Win32 platforms.  For the Windows 2000 platform
// we perform a "lazy" bind to the new product suite
// APIs that were first introduced on that platform.

BOOL IsTerminalServicesEnabled( VOID )
{
    BOOL    bResult = FALSE;    // assume Terminal Services is not enabled

    DWORD   dwVersion;
    OSVERSIONINFOEXA osVersionInfo;
    ULONGLONG dwlConditionMask = 0;
    HMODULE hmodK32 = NULL;
    HMODULE hmodNtDll = NULL;
    typedef ULONGLONG (__stdcall *PFnVerSetConditionMask)(ULONGLONG,DWORD,BYTE);
    typedef BOOL (__stdcall *PFnVerifyVersionInfoA) (POSVERSIONINFOEXA, DWORD, DWORDLONG);
    PFnVerSetConditionMask pfnVerSetConditionMask;
    PFnVerifyVersionInfoA pfnVerifyVersionInfoA;

    dwVersion = GetVersion();

    // are we running NT ?
    if (!(dwVersion & 0x80000000))
    {
        // Is it Windows 2000 (NT 5.0) or greater ?
        if (LOBYTE(LOWORD(dwVersion)) > 4)
        {
            // In Windows 2000 we need to use the Product Suite APIs
            // Don't static link because it won't load on non-Win2000 systems

            hmodNtDll = GetModuleHandleA( "ntdll.dll" );
            if (hmodNtDll != NULL)
            {
                pfnVerSetConditionMask = (PFnVerSetConditionMask )GetProcAddress( hmodNtDll, "VerSetConditionMask");
                if (pfnVerSetConditionMask != NULL)
                {
                    dwlConditionMask = (*pfnVerSetConditionMask)(dwlConditionMask, VER_SUITENAME, VER_OR);
                    hmodK32 = GetModuleHandleA( "KERNEL32.DLL" );
                    if (hmodK32 != NULL)
                    {
                        pfnVerifyVersionInfoA = (PFnVerifyVersionInfoA)GetProcAddress( hmodK32, "VerifyVersionInfoA");
                        if (pfnVerifyVersionInfoA != NULL)
                        {
                            ZeroMemory(&osVersionInfo, sizeof(osVersionInfo));
                            osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
                            osVersionInfo.wSuiteMask = VER_SUITE_TERMINAL | VER_SUITE_SINGLEUSERTS;
                            bResult = (*pfnVerifyVersionInfoA)(&osVersionInfo, VER_SUITENAME, dwlConditionMask);
                        }
                    }
                }
            }
        }
        else
        {
            // This is NT 4.0 or older
            bResult = ValidateProductSuite( "Terminal Server" );
        }
    }

    return bResult;
}


HANDLE OpenGlobalMutex(DWORD dwDesiredAccess,
                   BOOL bInheritHandle,
                   const CHAR* lpName)
{
	CHAR szMutexName[MAX_PATH];
	int len = 0;
	szMutexName[0] = 0;
	if (IsTerminalServicesEnabled())
	{
		strcpy(szMutexName, "Global\\");
		len = 7;
	}
	strncpy(szMutexName+len, lpName, MAX_PATH-len);
	szMutexName[MAX_PATH-1] = 0; // ensure zero-terminated
	return OpenMutex(dwDesiredAccess, bInheritHandle, szMutexName);
}

BOOL __stdcall _IsTaskActive(const TCHAR* pMutexName)
{
	HANDLE hMutex;
	hMutex = OpenGlobalMutex(MUTEX_ALL_ACCESS, FALSE, pMutexName);
	if(NULL == hMutex)
		return FALSE;
	CloseHandle(hMutex);
	return TRUE;
}

class _auto_free {
public:
	_auto_free(void** pptr) { m_pptr = pptr; };
	~_auto_free() { if (m_pptr && *m_pptr) HeapFree(GetProcessHeap(), 0, *m_pptr); *m_pptr = NULL; };
	void** m_pptr;
};

class _auto_release_proxy {
public:
	_auto_release_proxy(hOBJECT* pobj) { m_pobj = pobj; };
	~_auto_release_proxy() { if (m_pobj && *m_pobj) g_RP_API.ReleaseObjectProxy(*m_pobj); *m_pobj = NULL; };
	hOBJECT* m_pobj;
};


BOOL CheckDenyDial(LPRASDIALPARAMSA lpRasDialParams, LPCSTR lpszPhonebook, LPVOID nRetAddr)
{
	tERROR error = errOK;
    CHAR* szPhoneNumber = "<unknown>";
	hOBJECT hAntiDialTask = NULL;
	DWORD dwEntrySize = 0;
	RASENTRYA* pRasEntry = NULL;
	_auto_free _fpRasEntry((void**)&pRasEntry);
	_auto_release_proxy _task(&hAntiDialTask);
	
	if (lpRasDialParams == NULL)
	{
		ODS(("Anti-Dialer: RasDialA(lpRasDialParams = null)???"));
		return FALSE;
	}

	if (!_IsTaskActive("KLObj_mt_KLANTIDIAL_PR_DEF053874"))
	{
		ODS(("AntiDialHook: CheckDenyDial: AntiDialTask inactive"));
		return FALSE;
	}

	// get RASENTRY structure
	if (g_hRasApi32 == NULL)
		g_hRasApi32 = GetModuleHandle("rasapi32.dll");
	if (!fRasGetEntryPropertiesA)
		*(void**)&fRasGetEntryPropertiesA = GetProcAddress(g_hRasApi32, "RasGetEntryPropertiesA");
	if (fRasGetEntryPropertiesA)
	{
		DWORD error;
		dwEntrySize = 0;
		error = fRasGetEntryPropertiesA(lpszPhonebook, lpRasDialParams->szEntryName, NULL, &dwEntrySize, NULL, NULL);
		ODS(("AntiDialHook: fRasGetEntryPropertiesA(1) dwEntrySize=%d", dwEntrySize));
		if (dwEntrySize)
		{
			dwEntrySize += 0x10000;
			pRasEntry = (RASENTRYA*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwEntrySize);
			if (pRasEntry)
			{
				pRasEntry->dwSize = sizeof(RASENTRYA);
				if (error = fRasGetEntryPropertiesA(lpszPhonebook, lpRasDialParams->szEntryName, pRasEntry, &dwEntrySize, NULL, NULL))
				{
					ODS(("Anti-Dialer: RasGetEntryPropertiesA(entry=%s) failed with err=%08X, size=%d", lpRasDialParams->szEntryName ? lpRasDialParams->szEntryName : "<null>", error, dwEntrySize));
					HeapFree(GetProcessHeap(), 0, pRasEntry);
					pRasEntry = NULL;
				}
				if (pRasEntry)
				{
					ODS(("Options: %08x", pRasEntry->dwfOptions));
				}
			}
		}
	}
	else
	{
		ODS(("AntiDialHook: cannot get fRasGetEntryPropertiesA from g_hRasApi32=%08X", g_hRasApi32));
	}

	if (nRetAddr)
	{
		MEMORY_BASIC_INFORMATION mbi;
		CHAR szModule[MAX_PATH];
		ODS(("AntiDialHook: Ret address is %08X", nRetAddr));
		if (VirtualQuery(nRetAddr, &mbi, sizeof(mbi) ))
		{
			ODS(("AntiDialHook: Allocation base is %08X", mbi.AllocationBase));
			if (GetModuleFileName((HMODULE)mbi.AllocationBase, szModule, MAX_PATH))
			{
				ODS(("AntiDialHook: Calling module is '%s'", szModule));
				CHAR* pModName = strrchr(szModule, '\\');
				if (!pModName)
					pModName = szModule;
				else
					pModName++;
				if (__stricmp(pModName, "rasdlg.dll") == 0)
				{
					if (pRasEntry && (pRasEntry->dwfOptions & RASEO_PreviewPhoneNumber))
					{
						ODS(("AntiDialHook: Calling from 'rasdlg' & PreviewPhoneNumber on, means RasDialDlg has been invoked and number has been previewed by user"));
						return FALSE;
					}
				}
			}
		}
	}

	
	if (*lpRasDialParams->szPhoneNumber)
	{
		szPhoneNumber = lpRasDialParams->szPhoneNumber;
	}
	else if (pRasEntry)
	{
		if (pRasEntry->szDeviceType)
		{
			ODS(("Anti-Dialer: Device=%s, %s", pRasEntry->szDeviceType, pRasEntry->szDeviceName));
#ifdef _ONLY_DEVICE_TYPE_MODEM
			if (0 != strcmp(pRasEntry->szDeviceType, RASDT_Modem))
			{
				ODS(("Anti-Dialer: Device is not modem, skipping"));
				return FALSE;
			}
#endif
		}
		szPhoneNumber = pRasEntry->szLocalPhoneNumber;
	}
	ODS(("Anti-Dialer: RasDialA(entry=%s, local phone=%s, override phone=%s)", lpRasDialParams->szEntryName, szPhoneNumber, lpRasDialParams->szPhoneNumber));

	
	if (!LoadPrague())
	{
		ODS(("AntiDialHook:CheckDenyDial: Can't load prague"));
		return FALSE;
	}

	if (PR_FAIL(error = g_RP_API.GetObjectProxy(PR_PROCESS_ANY, "AntiDialTask", &hAntiDialTask)))
	{
		ODS(("AntiDialHook: Cannot get AntiDialTask proxy, dial passed\n"));
		return FALSE;
	}
	
	cAntiDialCheckRequest request;
	cStrObj sDialPrefix;
	if (pRasEntry)
	{
		if (pRasEntry->dwfOptions & RASEO_UseCountryAndAreaCodes)
		{
			DWORD dwCountryCode = 0;
			if (pRasEntry->dwCountryCode)
				dwCountryCode = pRasEntry->dwCountryCode;
			else if(pRasEntry->dwCountryID)
			{
				if (!fRasGetCountryInfoA)
					*(void**)&fRasGetCountryInfoA = GetProcAddress(g_hRasApi32, "RasGetCountryInfoA");
				if (fRasGetCountryInfoA)
				{
					char buf[0x100];
					DWORD dwSize = 256;
					LPRASCTRYINFOA pInfo = (LPRASCTRYINFOA)&buf;
					pInfo->dwSize = sizeof(RASCTRYINFOA);
					pInfo->dwCountryID = pRasEntry->dwCountryID;
					if (!fRasGetCountryInfoA(pInfo, &dwSize))
					{
						dwCountryCode = pInfo->dwCountryCode;
					}
				}
			}
			if (dwCountryCode)
				sDialPrefix.format(cCP_ANSI, "+%d", dwCountryCode);
			sDialPrefix.append("(");
			sDialPrefix.append(pRasEntry->szAreaCode);
			sDialPrefix.append(")");
		}
	}
	request.m_sEntry = lpRasDialParams->szEntryName;
	request.m_sPhoneNumber = sDialPrefix;
	request.m_sPhoneNumber.append(szPhoneNumber);
#ifdef _SHOW_DEVICE_TYPE
	if (pRasEntry)
	{
		request.m_sPhoneNumber.append(" [");
		request.m_sPhoneNumber.append(pRasEntry->szDeviceType);
		request.m_sPhoneNumber.append("]");
	}
#endif
	request.m_sProcessName = g_szProcessName;
	request.m_bOverride = *lpRasDialParams->szPhoneNumber ? cTRUE : cFALSE;
	request.m_nPID = GetCurrentProcessId();

	ODS(("AntiDialHook:CheckDenyDial: Sending check request..."));
	error = hAntiDialTask->sysSendMsg(pmc_ANTIDIAL, msg_ANTIDIAL_CHECK_NUMBER, NULL, &request, SER_SENDMSG_PSIZE);

	if (error == errACCESS_DENIED)
	{
		ODS(("AntiDialHook:CheckDenyDial: Check finished,  error=errACCESS_DENIED, dial should be blocked"));
		return TRUE;
	}
	else if (error == errOK_DECIDED)
	{
		ODS(("AntiDialHook:CheckDenyDial: Check finished error=%08X, dial allowed", error));
		if(pRasEntry)
		{
			while (pRasEntry->dwAlternateOffset && pRasEntry->dwAlternateOffset < dwEntrySize)
			{
				szPhoneNumber = (CHAR*)pRasEntry + pRasEntry->dwAlternateOffset;
				if (*szPhoneNumber == 0)
					break;
				request.m_sEntry = lpRasDialParams->szEntryName;
				request.m_sPhoneNumber = sDialPrefix;
				request.m_sPhoneNumber.append(szPhoneNumber);
				request.m_sProcessName = g_szProcessName;
				request.m_bOverride = cFALSE;
				ODS(("AntiDialHook:CheckDenyDial: Sending check request for alternate number %s...", szPhoneNumber));
				error = hAntiDialTask->sysSendMsg(pmc_ANTIDIAL, msg_ANTIDIAL_CHECK_NUMBER, NULL, &request, SER_SENDMSG_PSIZE);
				if (error == errACCESS_DENIED)
				{
					ODS(("AntiDialHook:CheckDenyDial: Check finished,  error=errACCESS_DENIED, dial should be blocked"));
					return TRUE;
				}
				pRasEntry->dwAlternateOffset += (tDWORD)strlen(szPhoneNumber) + 1;
			}
		}
		return FALSE;
	}
	else 
	{
		ODS(("AntiDialHook:CheckDenyDial: Check finished error=%08X, unexpected reply!!!", error));
		return FALSE;
	}

	return FALSE;
}

DWORD APIENTRY RasDialA_Hook(
							 LPRASDIALEXTENSIONS lpRasDialExtensions,
							 LPCSTR lpszPhonebook,
							 LPRASDIALPARAMSA lpRasDialParams,
							 DWORD dwNotifierType,
							 LPVOID lpvNotifier,
							 LPHRASCONN lphRasConn)
{
	DWORD dwResult;
	void* OrigFuncAddr = HookGetOrigFunc();
	LPVOID nRetAddr = HookGetRetAddr();
	BOOL bDenyDial = CheckDenyDial(lpRasDialParams, lpszPhonebook, nRetAddr);
	if (bDenyDial)
		dwResult = ERROR_ACCESS_DENIED;
	else
		dwResult = ((DWORD (APIENTRY *)( LPRASDIALEXTENSIONS, LPCSTR, LPRASDIALPARAMSA, DWORD, LPVOID, LPHRASCONN ))OrigFuncAddr)(lpRasDialExtensions, lpszPhonebook, lpRasDialParams, dwNotifierType, lpvNotifier, lphRasConn);
	return dwResult;
}

DWORD APIENTRY RasDialW_Hook( 
							 LPRASDIALEXTENSIONS lpRasDialExtensions,
							 LPCWSTR lpszPhonebook,
							 LPRASDIALPARAMSW lpRasDialParams,
							 DWORD dwNotifierType,
							 LPVOID lpvNotifier,
							 LPHRASCONN lphRasConn)
{
	RASDIALPARAMSA RasDialParamsA;
	void* OrigFuncAddr = HookGetOrigFunc();
	LPVOID nRetAddr = HookGetRetAddr();
	CHAR szPhonebook[0x200];
	BOOL bDenyDial = FALSE;
	DWORD dwResult;
	
	if (lpRasDialParams)
	{
#define U2A(field) FastUni2Ansi(lpRasDialParams->field, RasDialParamsA.field, sizeof(RasDialParamsA.field));
		U2A(szEntryName);
		U2A(szPhoneNumber);
		U2A(szCallbackNumber);
		U2A(szUserName);
		U2A(szPassword);
		U2A(szDomain);
		if (lpszPhonebook)
		{
			FastUni2Ansi((WCHAR*)lpszPhonebook, szPhonebook, sizeof(szPhonebook));
			szPhonebook[sizeof(szPhonebook)-1] = 0;
		}
#undef U2A
		bDenyDial = CheckDenyDial(&RasDialParamsA, lpszPhonebook ? szPhonebook : NULL, nRetAddr);
	}
	
	if (bDenyDial)
		dwResult = ERROR_ACCESS_DENIED;
	else
		dwResult = ((DWORD (APIENTRY *)( LPRASDIALEXTENSIONS, LPCWSTR, LPRASDIALPARAMSW, DWORD, LPVOID, LPHRASCONN ))OrigFuncAddr)(lpRasDialExtensions, lpszPhonebook, lpRasDialParams, dwNotifierType, lpvNotifier, lphRasConn);
	return dwResult;
}

extern "C" __declspec(dllexport) VOID __cdecl hmi(HMODULE hModule)
{
	ODS(("hmi(%08X)", hModule));
	if (!HookBaseInit())
		return;
	HookImportInModule(hModule, "RasDialA", RasDialA_Hook, NULL, 0);
	HookImportInModule(hModule, "RasDialW", RasDialW_Hook, NULL, 0);
}

extern "C" __declspec(dllexport) VOID __cdecl hme(HMODULE hModule)
{
	ODS(("hme(%08X)", hModule));
	if (!HookBaseInit())
		return;
	HookExportInModule(hModule, "RasDialA", RasDialA_Hook, NULL, 0);
	HookExportInModule(hModule, "RasDialW", RasDialW_Hook, NULL, 0);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	char* szProcessFileName;
	char* ext;
	static HANDLE g_hMutex_KavInProcessDllLoaded = NULL;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// ƒолжен быть вызван в любом случае, иначе падаем на ¬исте при запуске IE!
		// (приходит DLL_THREAD_ATTACH после DLL_PROCESS_DETACH и падаем в CRT)
		DisableThreadLibraryCalls((HMODULE)hModule);

		sVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (GetVersionEx(&sVerInfo))
		{
			if (sVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && sVerInfo.dwMajorVersion <= 4) // Microsoft Windows NT 
			{
				return TRUE;
			}
		}

		g_hThisModule = (HMODULE)hModule;

#define KAV_INPROCESS_DLL_LOADED "KAV_INPROCESS_DLL_LOADED"
		g_hMutex_KavInProcessDllLoaded = CreateMutex(NULL, cFALSE, KAV_INPROCESS_DLL_LOADED);
		GetModuleFileName(NULL, g_szProcessName, MAX_PATH);
		__strlwr(g_szProcessName);
		szProcessFileName = strrchr(g_szProcessName, '\\');
		if (!szProcessFileName)
			szProcessFileName = g_szProcessName;
		else
			szProcessFileName++;
		ODS(("Anti-Dialer Hook: DllMain: DLL_PROCESS_ATTACH to %s", g_szProcessName));
		InitializeCriticalSection(&g_cs);
		g_bcs_inited = true;
		g_hRasApi32 = GetModuleHandle("rasapi32.dll");
		ODS(("Anti-Dialer Hook: rasapi32.dll module handle %08X", g_hRasApi32));
		ext = strrchr(szProcessFileName, '.');
		if (!ext)
			ext = szProcessFileName;
		if (NULL == g_hRasApi32 
			&& strcmp(szProcessFileName, "iexplore.exe") !=0 
			&& strcmp(szProcessFileName, "msiexec.exe") !=0 
			&& strcmp(szProcessFileName, "regsvr32.exe") !=0 
			&& strncmp(szProcessFileName, "is", 2) !=0 
			&& strcmp(ext, ".tmp") !=0)
		{
			ODS(("Anti-Dialer Hook: rasapi32.dll not found, exiting."));
			return TRUE; // never return FALSE here!
			// ¬иста загружает но не инитит следующий по пор€дку модуль (например Comctl32.dll), если вернуть FALSE!
		}
		if (!HookBaseInit())
		{
			ODS(("Anti-Dialer Hook: HookBaseInit failed, exiting."));
			return TRUE; // never return FALSE here!
		}
		// still need this for KLIF loading method
		HookProcInProcess("RasDialA", RasDialA_Hook, NULL, FLAG_HOOK_IMPORT | FLAG_HOOK_EXPORT);
		HookProcInProcess("RasDialW", RasDialW_Hook, NULL, FLAG_HOOK_IMPORT | FLAG_HOOK_EXPORT );
		break;
	case DLL_PROCESS_DETACH:
		HookBaseDone();
		if (g_hMutex_KavInProcessDllLoaded)
			CloseHandle(g_hMutex_KavInProcessDllLoaded);
		if (g_bcs_inited)
			DeleteCriticalSection(&g_cs);
		ODS(("Anti-Dialer Hook DllMain: DLL_PROCESS_DETACH"));
		break;
	}
    return TRUE;
}


HRESULT RegisterAppInitDllsW(BOOL bRegister)
{
	HRESULT hRes;
	WCHAR sLongFileName[MAX_PATH*2];
	WCHAR sFileName[MAX_PATH*2];
	HKEY hKey;
	DWORD dwLen;
	bool bVista = DWORD((LOBYTE(LOWORD(GetVersion())))) > 5;
	dwLen = GetModuleFileNameW(g_hThisModule, sLongFileName, countof(sLongFileName));
	if (dwLen == 0)
		return HRESULT_FROM_WIN32(GetLastError());
	if (dwLen >= countof(sLongFileName)-1)
		return CO_E_PATHTOOLONG;

	dwLen = GetShortPathNameW(sLongFileName, sFileName, countof(sFileName)-1);
	if (dwLen == 0)
		return HRESULT_FROM_WIN32(GetLastError());
	if (dwLen >= countof(sFileName)-1)
		return CO_E_PATHTOOLONG;
	sFileName[dwLen+1] = 0;

	if (ERROR_SUCCESS == (hRes = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows", &hKey)))
	{
		WCHAR sAppInit[MAX_PATH*2];
		DWORD dwType = REG_SZ;
		DWORD dwSize = sizeof(sAppInit);
		DWORD nAppInitLen = countof(sAppInit);
		hRes = RegQueryValueExW(hKey, L"AppInit_Dlls", NULL, &dwType, (LPBYTE)sAppInit, &dwSize);
		sAppInit[nAppInitLen-1] = 0;
		if (hRes == ERROR_FILE_NOT_FOUND)
		{
			hRes = ERROR_SUCCESS;
			sAppInit[0] = 0;
		}
		if (ERROR_SUCCESS == hRes)
		{
			WCHAR* pExist, *pNext;
			pExist = __wcsistr(sAppInit, sFileName);
			if (pExist)
			{
				if (bRegister)
				{
					hRes = S_OK;
				}
				else
				{
					DWORD size;
					pNext = pExist + __wcslen(sFileName);
					// if was registered as quoted - include quotes
					if (pExist != sAppInit && pExist[-1]=='\"' && pNext[0]=='\"')
					{
						pExist--;
						pNext++;
					}
					// skip delimiters until next
					while (*pNext == ' ' || *pNext == ',')
						pNext++;
					// find prev, if any
					while (pExist != sAppInit)
					{	
						if (pExist[-1] == ' ' || (pExist[-1] == ',' && *pNext == 0))
							pExist--;
						else
							break;
					}
					size = __wcslen(pNext)+1;
					__memmove(pExist, pNext, size*sizeof(wchar_t));
					dwType = REG_SZ;
					dwSize = (__wcslen(sAppInit)+1)*sizeof(wchar_t);
					if (ERROR_SUCCESS == (hRes = RegSetValueExW(hKey, L"AppInit_Dlls", NULL, dwType, (LPBYTE)sAppInit, dwSize)))
					{
						hRes = S_OK;
					}
					if (bVista && __wcslen(sAppInit) == 0)
					{
						DWORD dw0 = 0;
						RegSetValueExW(hKey, L"LoadAppInit_Dlls", 0, REG_DWORD, (BYTE*)&dw0, sizeof(dw0));
					}
				}
			}
			else
			{
				if (bRegister)
				{
					DWORD size = __wcslen(sAppInit);
					if (size && size < nAppInitLen)
						sAppInit[size++] = ',';
					DWORD copy = min(__wcslen(sFileName)+1, nAppInitLen-size);
					memcpy(sAppInit+size, sFileName, copy*sizeof(wchar_t));
					sAppInit[nAppInitLen-1] = 0;
					dwType = REG_SZ;
					dwSize = (__wcslen(sAppInit)+1)*sizeof(wchar_t);
					// Ќа Vista чтобы работал AppInit_Dlls, нужно выставить LoadAppInit_Dlls в 1
					DWORD dw1 = 1;
					if (ERROR_SUCCESS == (hRes = RegSetValueExW(hKey, L"AppInit_Dlls", NULL, dwType, (LPBYTE)sAppInit, dwSize))
						&& (!bVista || ERROR_SUCCESS == (hRes = RegSetValueExW(hKey, L"LoadAppInit_Dlls", 0, REG_DWORD, (BYTE*)&dw1, sizeof(dw1)))))
					{
						hRes = S_OK;
					}
				}
				else
				{
					hRes = S_OK;
				}
			}
		}
		RegCloseKey(hKey);
	}
	if (0 != hRes)
		hRes = HRESULT_FROM_WIN32(hRes);
	return hRes;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	HRESULT hResult = S_OK;
	if (sVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && sVerInfo.dwMajorVersion <= 4) // Microsoft Windows NT 
		return S_OK;
	hResult = RegisterAppInitDllsW(TRUE);
	if (SUCCEEDED(hResult))
		hResult = RegisterSputnikDllByHandle("rasapi32.dll", g_hThisModule);
	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	HRESULT hResult = S_OK;
	if (sVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && sVerInfo.dwMajorVersion <= 4) // Microsoft Windows NT 
		return S_OK;
	hResult = RegisterAppInitDllsW(FALSE);
	if (SUCCEEDED(hResult))
		hResult = UnregisterSputnikDllByHandle("rasapi32.dll", g_hThisModule);
	return hResult;
}



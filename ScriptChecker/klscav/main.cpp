#define PR_IMPEX_DEF

#define  _WIN32_WINNT 0x400
#include <windows.h>
#include <objbase.h>
#include "..\SC_Interceptor2\scripting\activscp.h"

#if !defined(_WIN64)
#include <tlhelp32.h>
#endif

#define _USE_TRY_
#include "debug.h"

#include <Version/ver_product.h>
#include <Prague/prague.h>
#include <Prague/pr_remote.h>
#include <ScriptChecker/plugin/p_sc.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <AV/iface/i_engine.h>
#include <Prague/plugin\p_tempfile.h>
#include <Prague/iface/i_loader.h>
#include <Prague/iface/i_root.h>
#include <PopupChecker/plugin/p_popupchk.h>
#include <PopupChecker/structs/s_popupchk.h>
#include <AntiPhishing/structs/s_antiphishing.h>
#include <Prague/pr_time.h>
#include <PSAPI.h>

#if VER_PRODUCTVERSION_HIGH <= 7
#include <PDM/structs/s_pdm.h>
#endif

#define _CRCPAGESIZE	2040
#define _CRCPAGECOUNT	10
#include "crc.h"

#include "klscav.h"

#include "..\sc_interceptor2\scplugin.h"
#define PLUGIN_NAME     "SC Anti-Virus Plugin (PR)"
#define PLUGIN_PRIORITY 5
#define	PLUGIN_REG_KEY  SC_REG_PLUGINS "\\" PLUGIN_NAME

tCHAR* plugins_list[] = {
	"params.ppl",
	"pxstub.ppl",
	"tempfile.ppl",
};

CRITICAL_SECTION g_cs;
EXTERN_C hROOT g_root = NULL;
HMODULE g_hModuleRemotePrague = NULL;
PRRemoteAPI g_RP_API = {0};
BOOL g_bPragueLoadTry    = FALSE;
tERROR g_PragueLoadResult = errOBJECT_NOT_INITIALIZED;
BOOL g_bEnabled = TRUE;
hOBJECT g_hSCTask = NULL;
hOBJECT g_hPopupCheckTask = NULL;
hOBJECT g_hAntiBannerTask = NULL;
//hOBJECT g_hPDMTask = NULL;
HINSTANCE g_hInstance = NULL;
PCRCPAGE g_pCRCRoot = NULL;
hTASKMANAGER g_hTM = NULL; // --
hMSG_RECEIVER g_hMsgRecv = NULL;
BOOL   g_bResetMsgHandler = TRUE;
CHAR   g_szProcessName[MAX_PATH*2];

#define EICAR_TEXT "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*"
#define EICAR_TEXT_SIZE 0x44
#define EICAR_TEXT2 "X5O!P%@AP[4PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*" // with missed '\' char because JavaScript parsed '\P' as unrecognized character escape sequence resulting 'P'

#define EICAR_TEXT2_SIZE 0x43

BOOL __stdcall IsDispatchEnabled(DWORD dwActionId);

//static bool g_bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	static HANDLE g_hMutex_KavInProcessDllLoaded = NULL;
	BOOL bRes = TRUE;
	
	g_hInstance = hInstance;
    
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		ODS(("KLSCAV: DLL_PROCESS_ATTACH"));

#define KAV_INPROCESS_DLL_LOADED "KAV_INPROCESS_DLL_LOADED"
		g_hMutex_KavInProcessDllLoaded = CreateMutex(NULL, cFALSE, KAV_INPROCESS_DLL_LOADED);

		DisableThreadLibraryCalls(hInstance);
		ODS(("KLSCAV: DLL_PROCESS_ATTACH finished"));

		InitializeCriticalSection(&g_cs);
    }

    if (dwReason == DLL_PROCESS_DETACH)
	{
		ODS(("KLSCAV: DLL_PROCESS_DETACH"));

/*
// commented because lead to crash
		__try
		{
			if (PR_SUCC(g_PragueLoadResult) && g_RP_API.Deinitialize)
				g_RP_API.Deinitialize();
		} __except(1) {};
*/

		if (g_hMutex_KavInProcessDllLoaded)
			CloseHandle(g_hMutex_KavInProcessDllLoaded);

		DeleteCriticalSection(&g_cs);
		ODS(("KLSCAV: DLL_PROCESS_DETACH finished"));
	}

    return bRes;    // ok
}

void Trace( tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel) 
{
	OutputDebugString(str);
	OutputDebugString("\n");
}


static tERROR pr_call MsgReceive(hOBJECT p_this, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len)
{
	ODS(("KLSCAV\tpmc_POPUP_CHECK received"));
	if (p_msg_cls_id == pmc_POPUP_CHECK && p_msg_id == msg_POPUP_TEMPORARY_ALLOW_CB2)
	{
		ODS(("KLSCAV\tmsg_POPUP_TEMPORARY_ALLOW_CB called"));
		if (p_par_buf_len != SER_SENDMSG_PSIZE || p_par_buf==NULL)
			return errPARAMETER_INVALID;
		cPopupCheckRequest* pPopupCheckRequest = (cPopupCheckRequest*)p_par_buf;
		if (pPopupCheckRequest->m_pCallbackFn)
		{
			ODS(("pCallbackFn=%08X, ctx=%08X", pPopupCheckRequest->m_pCallbackFn, pPopupCheckRequest->m_pCallbackCtx));
			try 
			{
				tCheckPopupURLCallbackFn pCallbackFn = (tCheckPopupURLCallbackFn)pPopupCheckRequest->m_pCallbackFn;
				if (pCallbackFn(pPopupCheckRequest->m_pCallbackCtx, SCP_ACTION_POPUP_TEMPORARY_ALLOWED, pPopupCheckRequest->m_sSourceUrl.c_str(cCP_UNICODE)))
					return errOK_DECIDED;
				return errNOT_OK;
			}
			catch (...) 
			{
				ODS(("CheckPopupURLCallbackFn caused exception"));
				return errUNEXPECTED;
			}
		}
	}
	return errOK;
}

BOOL LoadPrague()
{
	EnterCriticalSection(&g_cs);
	if (!g_bPragueLoadTry)
	{
		BOOL bCurDirChanged = FALSE;
		tCHAR szLoadPath[MAX_PATH];
		tCHAR szCurDir[MAX_PATH*2];
		tCHAR* pModuleName;

		DWORD dwCurDirLen = GetCurrentDirectory(countof(szCurDir), szCurDir);
		if (dwCurDirLen >= countof(szCurDir))
			dwCurDirLen = 0;

		g_bPragueLoadTry = TRUE;
		
		if (!GetModuleFileName(NULL, g_szProcessName, countof(g_szProcessName)))
			g_szProcessName[0] = 0;
		
		if (!GetModuleFileName(g_hInstance, szLoadPath, countof(szLoadPath)))
			*szLoadPath = 0;
		pModuleName = strrchr(szLoadPath, '\\');
		if (!pModuleName)
			pModuleName = szLoadPath;
		else
		{
			//if (dwCurDirLen)
			//{
			//	*pModuleName = 0;
			//	bCurDirChanged = SetCurrentDirectory(szLoadPath);
			//	*pModuleName = '\\';
			//}
			pModuleName++;
		}
		
		strcpy(pModuleName, PR_REMOTE_MODULE_NAME);
		g_hModuleRemotePrague = LoadLibraryEx(szLoadPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		
		if (!g_hModuleRemotePrague)
		{
			ODS(("KAV Script Checker: cannot load module '%s%", szLoadPath));
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
				g_PragueLoadResult = g_RP_API.Initialize(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH|PR_FLAG_LOW_INTEGRITY_PROCESS,NULL);
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
						ODS(("KAV Script Checker: cannot load module '%s'", plugins_list[i]));
						break;
					}
				}
			}
			
			if (PR_SUCC(g_PragueLoadResult))
			{
				tERROR recv_error;
#ifdef _PRAGUE_TRACE_
				g_root->propSetDWord( pgOUTPUT_FUNC, (tDWORD)Trace );
				g_root->sysSetTraceLevel( tlsALL_OBJECTS, prtNOT_IMPORTANT, prtMIN_TRACE_LEVEL);
#endif
				if (PR_SUCC(recv_error = g_root->sysCreateObject((hOBJECT*)&g_hMsgRecv, IID_MSG_RECEIVER)))
				{
					recv_error = g_hMsgRecv->set_pgRECEIVE_PROCEDURE((tFUNC_PTR)MsgReceive);
					if (PR_SUCC(recv_error))
						recv_error = g_hMsgRecv->sysCreateObjectDone();
				}
				if (PR_FAIL(recv_error))
				{
					ODS(("KAV Script Checker: create message handler failed"));
					if (g_hMsgRecv)
					{
						g_hMsgRecv->sysCloseObject();
						g_hMsgRecv = NULL;
					}
				}

			}
		}
		if (bCurDirChanged)
			SetCurrentDirectory(szCurDir);
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
	HANDLE hMutex;
	int len = 0;
	szMutexName[0] = 0;
	if (IsTerminalServicesEnabled())
	{
		strcpy(szMutexName, "Global\\");
		len = 7;
	}
	strncpy(szMutexName+len, "KLObj_mt_", MAX_PATH-len);
	len += 9;
	strncpy(szMutexName+len, lpName, MAX_PATH-len);
	szMutexName[MAX_PATH-1] = 0; // ensure zero-terminated
	hMutex = OpenMutex(dwDesiredAccess, bInheritHandle, szMutexName);
	ODS(("Opening %s %s with err=%08X", szMutexName, hMutex ? "succeeded" : "failed", GetLastError()));
	return hMutex;
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

BOOL __stdcall IsEnabled()
{
	if (_IsTaskActive("KLSCRIPTCHECKER_PR_DEF9362421"))
		return TRUE;

	if (_IsTaskActive("KLPOPUPCHECK_PR_DEF884242"))
		return TRUE;

	if (_IsTaskActive("KLPDM_PR_DEF884242"))
		return TRUE;
	 
	return FALSE;
}

BOOL GetTaskObject(CHAR* szTaskName, CHAR* szTaskMutex, hOBJECT* phTaskObject, BOOL* pbNewProxy)
{
	hOBJECT hTaskObject = *phTaskObject;
	tERROR error;
	if (hTaskObject == NULL)
	{
		if (!_IsTaskActive(szTaskMutex))
		{
			ODS(("KLSCAV: GetTaskObject: %s inactive", szTaskName));
			return FALSE;
		}
	}
	
	if (!LoadPrague())
	{
		ODS(("KLSCAV: GetTaskObject: Can't load prague"));
		return FALSE;
	}

	ODS(("KLSCAV: GetTaskObject: prague ok"));

	EnterCriticalSection(&g_cs);
	if (hTaskObject != NULL)
	{
		if (PR_FAIL(error = g_RP_API.IsValidProxy(hTaskObject)))
		{
			error = g_RP_API.ReleaseObjectProxy(hTaskObject);
			hTaskObject = NULL;
		}
	}
	
	if (hTaskObject == NULL)
	{
		if (pbNewProxy)
			*pbNewProxy = TRUE;
		if (PR_FAIL(error = g_RP_API.GetObjectProxy(PR_PROCESS_ANY, szTaskName, &hTaskObject)))
		{
			ODS(("Cannot get proxy to %s\n", szTaskName));
			hTaskObject = NULL;
		}
	}
	*phTaskObject = hTaskObject;
	LeaveCriticalSection(&g_cs);
	return (hTaskObject != NULL);
}

BOOL __stdcall ProcessScript(WCHAR* pwcsScript, WCHAR* pwcsLanguage, WCHAR* pwcsURL, IActiveScript* pActiveScript, DWORD* pdwFlags)
{
	tERROR error = errOK;
	BOOL bResult = TRUE;
	char* pScriptText = NULL;

	DWORD dwCrc;

	if (g_hSCTask == NULL)
	{
		ODS(("KLSCAV:ProcessScript: SC inactive"));
		if (!_IsTaskActive("KLSCRIPTCHECKER_PR_DEF9362421"))
			return FALSE;
	}
	
	if (!LoadPrague())
	{
		ODS(("KLSCAV:ProcessScript: Can't load prague"));
		return FALSE;
	}

	if (pwcsScript == NULL || pwcsScript[0] == 0)
	{
		ODS(("CheckScriptText2 script empty, pass\n"));
		return TRUE;
	}
	
	DWORD dwAllocSize = WideCharToMultiByte(CP_ACP, 0, pwcsScript, -1, NULL, 0, NULL, NULL);
	if (dwAllocSize == 0)
	{
		ODS(("KLSCAV:ProcessScript: WideCharToMultiByte = 0"));
		return FALSE;
	}

	pScriptText = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAllocSize);
	if (pScriptText == NULL)
	{
		ODS(("HeapAlloc failed\n"));
		return FALSE;
	}

	DWORD dwCurrentScriptLen = WideCharToMultiByte(CP_ACP, 0, pwcsScript, -1, pScriptText, dwAllocSize, NULL, NULL);
	if (dwCurrentScriptLen == 0)
	{
		if (pScriptText != NULL)
			HeapFree(GetProcessHeap(), 0, pScriptText);
		ODS(("KLSCAV:ProcessScript: WideCharToMultiByte2 = 0"));
		return FALSE;
	}

	dwCrc = CalcSum((LPBYTE)pScriptText, dwCurrentScriptLen);

	static DWORD dwOldCrcFreeTickCount = 0;
	DWORD dwTickCount = GetTickCount();
	EnterCriticalSection(&g_cs);
	if (dwOldCrcFreeTickCount && (dwTickCount - dwOldCrcFreeTickCount) > 15*60*1000) // every 15 min
	{
		dwOldCrcFreeTickCount = dwTickCount;
		FreeCrcPages(&g_pCRCRoot);
	}
	BOOL bCheckResult = CheckOldCRC(&g_pCRCRoot, dwCrc);
	LeaveCriticalSection(&g_cs);
	if (bCheckResult)
	{
		ODS(("Script - has been executed before - passed!\n"));
	}
	else
	{
		EnterCriticalSection(&g_cs);
		if (g_hSCTask != NULL)
		{
			if (PR_FAIL(error = g_RP_API.IsValidProxy(g_hSCTask)))
			{
				error = g_RP_API.ReleaseObjectProxy(g_hSCTask);
				g_hSCTask = NULL;
			}
		}
		
		if (g_hSCTask == NULL)
		{
			if (PR_FAIL(error = g_RP_API.GetObjectProxy(PR_PROCESS_ANY, "SCTask", &g_hSCTask)))
			{
				if (pScriptText != NULL)
					HeapFree(GetProcessHeap(), 0, pScriptText);
				ODS(("Cannot get SCTask proxy, script execution passed\n"));
				LeaveCriticalSection(&g_cs);
				return FALSE;
			}
			FreeCrcPages(&g_pCRCRoot);
		}
		
		if (PR_SUCC(error) && g_hSCTask)
			error = g_RP_API.AddRefObjectProxy(g_hSCTask);

		LeaveCriticalSection(&g_cs);
		
		if (PR_SUCC(error) && g_hSCTask)
		{
			hIO io;
			ODS(("KLSCAV:ProcessScript: Creating IO..."));
			if (PR_SUCC(error = g_hSCTask->sysCreateObjectQuick((hOBJECT*)&io, IID_IO, PID_TEMPIO)))
			{
				tCHAR Banner[256];
				tQWORD qwIoPos = 0;
				tDWORD dwWritten = 0;
				tDWORD dwInfectCount = 0;
				tDWORD dwLen = sizeof(dwInfectCount);
				tBOOL  bEicar = cFALSE;

				{ // check for EICAR
					char* _pScript = pScriptText;
					DWORD _dwCurrentScriptLen = dwCurrentScriptLen;

					while (_pScript[0] == ' ' || _pScript[0] == 0x0A || _pScript[0] == 0x0D || _pScript[0] == 9)
					{
						_pScript++;
						_dwCurrentScriptLen--;
					}
					if (_pScript[0] == '\'')
					{
						if (   (_dwCurrentScriptLen >= EICAR_TEXT_SIZE + 1  && memcmp(_pScript+1, EICAR_TEXT,  EICAR_TEXT_SIZE)  == 0)
							|| (_dwCurrentScriptLen >= EICAR_TEXT2_SIZE + 1 && memcmp(_pScript+1, EICAR_TEXT2, EICAR_TEXT2_SIZE) == 0))
						{
							ODS(("KLSCAV:ProcessScript: Eicar found..."));
							bEicar = cTRUE;
						}
					}
				}


				error = io->propSetStr(NULL, pgOBJECT_FULL_NAME, pwcsURL, 0, cCP_UNICODE);

//				if(wcslen(pwcsURL)<900)
//				{
//					tCHAR buf[1000];
//					wsprintf(buf, "%S.script_%08x",pwcsURL,dwCrc);
//					error = io->propSetStr(NULL, pgOBJECT_FULL_NAME, buf, 0, cCP_ANSI);
//				}
//				else
//					error = io->propSetStr(NULL, pgOBJECT_FULL_NAME, pwcsURL, 0, cCP_UNICODE);
				
				if (bEicar)
				{
					if (PR_SUCC(error))
						error = io->SeekWrite(&dwWritten, qwIoPos, EICAR_TEXT, EICAR_TEXT_SIZE);
					qwIoPos += dwWritten;
				}
				else
				{
					if (PR_SUCC(error))
					{
						int nBannerPos;
						for (nBannerPos=0; nBannerPos<30; nBannerPos++) // this is for AV script engine in KAV bases
							Banner[nBannerPos] = '~';
						nBannerPos += wsprintf(Banner+nBannerPos, "\r\n<SCRIPT LANGUAGE=\"%S\">\r\n", pwcsLanguage);
						error = io->SeekWrite(&dwWritten, qwIoPos, Banner, nBannerPos);
						qwIoPos += dwWritten;
					}
					
					if (PR_SUCC(error))
						error = io->SeekWrite(&dwWritten, qwIoPos, pScriptText, dwCurrentScriptLen);
						qwIoPos+=dwWritten;
					
					if (PR_SUCC(error))
					{
						wsprintf(Banner, "\r\n</SCRIPT>");
						error = io->SeekWrite(&dwWritten, qwIoPos, Banner, lstrlen(Banner));
						qwIoPos += dwWritten;
					}
				}
				

				if (PR_SUCC(error))
				{
					ODS(("KLSCAV:ProcessScript: Sending scan request..."));
					error = g_hSCTask->sysSendMsg(pmc_SC_SCAN, msg_SC_SCAN, io, &dwInfectCount, &dwLen);
					ODS(("KLSCAV:ProcessScript: Scan finished err=%x, dwInfectCount=%d", error, dwInfectCount));
					if (dwInfectCount && pdwFlags)
					{
						*pdwFlags |= SCP_BLOCK; 
						*pdwFlags |= SCP_BLOCK_FURTHER; 
					}
					if (!dwInfectCount && errOK_DECIDED == error)
					{
						EnterCriticalSection(&g_cs);
						AddNewCRC(&g_pCRCRoot, dwCrc);
						LeaveCriticalSection(&g_cs);
					}
				}

				if(PR_SUCC(g_hSCTask->sysIsValidObject((hOBJECT)io)))
				{
					io->sysCloseObject();
				}
				else
				{
					ODS(("io invalid, closing skipped\n"));
				}
			}
			g_RP_API.ReleaseObjectProxy(g_hSCTask);
		}
	}

	if (pScriptText != NULL)
		HeapFree(GetProcessHeap(), 0, pScriptText);
	
	if (PR_FAIL(error))
	{
		ODS(("KLSCAV:ProcessScript: return with error=%08X", error));
//		bResult = FALSE; 
	}
	
	return bResult;
}


BOOL __stdcall CheckPopupURL(DWORD dwAction, WCHAR* pwcsSourceURL, WCHAR* pwcsDestinationURL, DWORD* pdwFlags, tCheckPopupURLCallbackFn pCallbackFn, LPVOID pCallbackCtx)
{
	tERROR error;
	BOOL bResult = TRUE;

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) // Control key down
		return FALSE;

	if (!GetTaskObject(POPUP_TASK_NAME, "KLPOPUPCHECK_PR_DEF884242", &g_hPopupCheckTask, &g_bResetMsgHandler))
		return FALSE;

	ODS(("KLSCAV:CheckPopupURL: PopupCheckTask proxy ok, g_hPopupCheckTask=%08X", g_hPopupCheckTask));
	
	static HRESULT (__stdcall *fCoInternetCreateSecurityManager)(IServiceProvider *pSP, IInternetSecurityManager **ppSM, DWORD dwReserved) = NULL;
	cPopupCheckRequest request;

	// cInfectedObjectInfo part
	request.m_nNonCuredReason = NCREASON_REPONLY;
	request.m_strDetectObject = pwcsSourceURL;
	request.m_strDetectName = "Anti-Popup";
	request.m_nDetectType = DETYPE_ADWARE;
	request.m_nDetectStatus = DSTATUS_HEURISTIC;
	request.m_nDetectDanger = DETDANGER_MEDIUM;
	request.m_nDescription = 0;
	request.m_strObjectName = "Internet Explorer";
	request.m_nObjectType = OBJTYPE_MEMORYPROCESS;
	request.m_tmTimeStamp = cDateTime();
	// cPopupCheckRequest part
	request.m_sSourceUrl = pwcsSourceURL;
	request.m_sDestinationUrl = pwcsDestinationURL;
	request.m_pCallbackFn = pCallbackFn;
	request.m_pCallbackCtx = pCallbackCtx;
	if (g_hMsgRecv && pCallbackFn && g_bResetMsgHandler)
	{
		ODS(("KLSCAV:sysRegisterMsgHandler"));
		tERROR reg_err = g_hMsgRecv->sysRegisterMsgHandler(pmc_POPUP_CHECK, rmhLISTENER, g_hPopupCheckTask,  IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		if (PR_SUCC(reg_err) || reg_err==errOBJECT_ALREADY_EXISTS)
		{
			ODS(("KLSCAV:sysRegisterMsgHandler succeeded"));
			g_bResetMsgHandler = FALSE;
		}
		else
		{
			ODS(("KLSCAV:sysRegisterMsgHandler failed, err=%08X", reg_err));
		}
	}
		
		
	if (!fCoInternetCreateSecurityManager)
	{
		ODS(("KLSCAV:CheckPopupURL: before getting fCoInternetCreateSecurityManager...", fCoInternetCreateSecurityManager));
		HMODULE urlmon = GetModuleHandle("urlmon.dll");
		ODS(("KLSCAV:CheckPopupURL: urlmon.dll=%08X", urlmon));
		try 
		{
			*(void**)&fCoInternetCreateSecurityManager = GetProcAddress(urlmon, "CoInternetCreateSecurityManager");
		} 
		catch (...) 
		{
			ODS(("KLSCAV:CheckPopupURL: GetProcAddress(urlmon, \"CoInternetCreateSecurityManager\") caused exception!!!"));
		}
	}
	ODS(("KLSCAV:CheckPopupURL: fCoInternetCreateSecurityManager=%08X...", fCoInternetCreateSecurityManager));
	if (fCoInternetCreateSecurityManager)
	{
		IInternetSecurityManager* pSM = NULL;
		if (SUCCEEDED(fCoInternetCreateSecurityManager(NULL, &pSM, NULL)))
		{
			ODS(("KLSCAV:CheckPopupURL: fCoInternetCreateSecurityManager succeeded pSM=%08X...", pSM));
			if (pwcsSourceURL)
			{
				ODS(("KLSCAV:CheckPopupURL: Mapping %S...", pwcsSourceURL));
				if (FAILED(pSM->MapUrlToZone(pwcsSourceURL, (DWORD*)&request.m_nSourceUrlZone, 0)))
					request.m_nSourceUrlZone = -1;
				ODS(("KLSCAV:CheckPopupURL: Mapping %S result %d", pwcsSourceURL, request.m_nSourceUrlZone));
			}
			if (pwcsDestinationURL)
			{
				ODS(("KLSCAV:CheckPopupURL: Mapping %S...", pwcsDestinationURL));
				if (FAILED(pSM->MapUrlToZone(pwcsDestinationURL, (DWORD*)&request.m_nDestinationUrlZone, 0)))
					request.m_nDestinationUrlZone = -1;
				ODS(("KLSCAV:CheckPopupURL: Mapping %S result %d", pwcsDestinationURL, request.m_nDestinationUrlZone));
			}
			pSM->Release();
		}
	}

	switch (dwAction)
	{
	case SCP_ACTION_POPUP_CHECK_DENY:
		ODS(("KLSCAV:CheckPopupURL: Sending msg_POPUP_CHECK_URL request..."));
		error = g_hPopupCheckTask->sysSendMsg(pmc_POPUP_CHECK, msg_POPUP_CHECK_URL, NULL, &request, SER_SENDMSG_PSIZE);
		if (error == errACCESS_DENIED)
		{
			ODS(("KLSCAV:CheckPopupURL: Check finished,  error=errACCESS_DENIED, pop-up should be blocked"));
			if (pdwFlags)
				*pdwFlags |= SCP_BLOCK; 
		}
		else if (error == errOK_DECIDED)
		{
			ODS(("KLSCAV:CheckPopupURL: Check finished error=%08X, pop-up allowed", error));
		}
		else 
		{
			ODS(("KLSCAV:CheckPopupURL: Check finished error=%08X, unexpected reply!!!", error));
			bResult = FALSE;
		}
		break;
	case SCP_ACTION_POPUP_ADD_TRUSTED_SITE:
		ODS(("KLSCAV:CheckPopupURL: Sending msg_POPUP_ADD_TRUSTED_SITE request..."));
		error = g_hPopupCheckTask->sysSendMsg(pmc_POPUP_CHECK, msg_POPUP_ADD_TRUSTED_SITE, NULL, &request, SER_SENDMSG_PSIZE);
		if (error == errOK_DECIDED)
		{
			ODS(("KLSCAV:CheckPopupURL: msg_POPUP_ADD_TRUSTED_SITE finished ok"));
		}
		else 
		{
			ODS(("KLSCAV:CheckPopupURL: msg_POPUP_ADD_TRUSTED_SITE finished error=%08X, unexpected reply!!!", error));
			bResult = FALSE;
		}
		break;
	case SCP_ACTION_POPUP_TEMPORARY_ALLOWED:
		ODS(("KLSCAV:CheckPopupURL: Sending msg_POPUP_ALLOWED request..."));
		error = g_hPopupCheckTask->sysSendMsg(pmc_POPUP_CHECK, msg_POPUP_ALLOWED, NULL, &request, SER_SENDMSG_PSIZE);
		if (error == errOK_DECIDED)
		{
			ODS(("KLSCAV:CheckPopupURL: msg_POPUP_ALLOWED finished ok"));
		}
		else 
		{
			ODS(("KLSCAV:CheckPopupURL: msg_POPUP_ALLOWED finished error=%08X, unexpected reply!!!", error));
			bResult = FALSE;
		}
		break;
	default:
		ODS(("KLSCAV:CheckPopupURL: unknown action request =%d!!!", dwAction));
		break;
	}

	return bResult;
}

HRESULT DispatchPStoreCreateInstance(DWORD dwActionId, void* pActionDataStruct, DWORD dwActionDataSize, DWORD* pdwFlags)
{
#if VER_PRODUCTVERSION_HIGH >= 8
	return S_OK;
#else
	tERROR error;
	ODS(("KLSCAV:DispatchPStoreCreateInstance..."));
	if (!pActionDataStruct)
		return E_INVALIDARG;
	if (dwActionDataSize != sizeof(SPD_PStoreCreateInstanceData))
		return E_INVALIDARG;
	if (!IsDispatchEnabled(dwActionId))
		return S_OK;
	if (!GetTaskObject("TaskManager", "KLPDM_PR_DEF884242", (hOBJECT*)&g_hTM, NULL))
		return E_NOINTERFACE;
	SPD_PStoreCreateInstanceData* pData = (SPD_PStoreCreateInstanceData*)pActionDataStruct;
	cPdmGenericRequest request;
	request.m_nPID = GetCurrentProcessId();
	request.m_nTID = GetCurrentThreadId();
	request.m_sProcessName = g_szProcessName;
	request.m_vDwParams.push_back((tDWORD)pData->bExecutable);
	request.m_vQwParams.push_back((tQWORD)pData->ret_addr);
	request.m_vQwParams.push_back((tQWORD)pData->hModule);
	cStrObj& sModuleName = request.m_vStrParams.push_back();
	if (pData->szModuleName)
		sModuleName = pData->szModuleName;
	ODS(("KLSCAV:DispatchPStoreCreateInstance: Sending request..."));
	error = g_hTM->sysSendMsg(pmc_PStore, msg_PStoreCreateInstance, NULL, &request, SER_SENDMSG_PSIZE);
	if (error == errACCESS_DENIED)
	{
		ODS(("KLSCAV:DispatchPStoreCreateInstance: Check finished,  error=errACCESS_DENIED"));
		if (pdwFlags)
			*pdwFlags |= SCP_BLOCK; 
		return E_ACCESSDENIED;
	}
	
	if (error == errOK_DECIDED)
	{
		ODS(("KLSCAV:DispatchPStoreCreateInstance: Check finished, allowed", error));
		return S_OK;
	}

	if (error == errOK_NO_DECIDERS)
	{
		ODS(("KLSCAV:DispatchPStoreCreateInstance: Check finished, NO DECIDERS, allowed"));
		return S_OK;
	}

	ODS(("KLSCAV:DispatchPStoreCreateInstance: Check finished error=%08X, unexpected reply!!!", error));
	return E_UNEXPECTED;
#endif
}

HRESULT DispatchIsBanner(DWORD dwActionId, void* pActionDataStruct, DWORD dwActionDataSize, DWORD* pdwFlags)
{
	tERROR error;
	tDWORD msgid;
	ODS(("KLSCAV:DispatchIsBanner..."));
	switch(dwActionId) {
	case SPD_IsBanner:
		msgid = msg_URL_FILTER_CHECK_URL;
		break;
	case SPD_IsWLBanner:
		msgid = msg_URL_FILTER_BLOCK_IF_NOT_WHITELISTED;
		break;
	default:
		return E_NOTIMPL;
	}
	if (!pActionDataStruct)
		return E_INVALIDARG;
	if (dwActionDataSize != sizeof(SPD_IsBannerData))
		return E_INVALIDARG;
	SPD_IsBannerData* pIsBannerData = (SPD_IsBannerData*)pActionDataStruct;
	if (!GetTaskObject("KLAntiBannerTask", "KLANTIBANNER_PR_DEF6900421", &g_hAntiBannerTask, NULL))
		return E_NOINTERFACE;
	cAntiPhishingCheckRequest request;
	request.m_bInteractive = cFALSE;
	request.m_nPID = GetCurrentProcessId();
	request.m_sProcessName = g_szProcessName;
	request.m_sURL = pIsBannerData->pwchBannerUrl;
	ODS(("KLSCAV:DispatchIsBanner: Sending pmc_URL_FILTER_ADBLOCK request..."));
	error = g_hAntiBannerTask->sysSendMsg(pmc_URL_FILTER_ADBLOCK, msgid, NULL, &request, SER_SENDMSG_PSIZE);
	if (error == errACCESS_DENIED)
	{
		ODS(("KLSCAV:DispatchIsBanner: Check finished,  error=errACCESS_DENIED, banner should be blocked"));
		if (pdwFlags)
			*pdwFlags |= SCP_BLOCK; 
		return E_ACCESSDENIED;
	}
	
	if (error == errOK_DECIDED)
	{
		ODS(("KLSCAV:DispatchIsBanner: Check finished error=%08X, banner allowed, template(%S)", error, request.m_sTemplate.data()));
		return S_OK;
	}

	ODS(("KLSCAV:DispatchIsBanner: Check finished error=%08X, unexpected reply!!!", error));
	return E_UNEXPECTED;
}

void FillPostData(cStringObj& post_str, VARIANT* post)
{
	HRESULT hr;
	if (!post)
		return;
	if (post->vt == VT_ARRAY)
	{
		SAFEARRAY* psa = post->parray;
		ODS(("KLSCAV:FillPostData vt=ARRAY, arr=%08X, features=%08X", post->vt, post->parray, psa->fFeatures));
		if (!psa)
			return;
		if (psa->fFeatures & FADF_BSTR)
		{
			// Get a pointer to the elements of the array.
			BSTR HUGEP *pbstr;
			hr = SafeArrayAccessData(psa, (void HUGEP**)&pbstr);
			if (FAILED(hr))
				return;
			for (ULONG i = 0; i < psa->rgsabound[0].cElements-1; i++)
			{
				if (post_str.length())
					post_str.append(L"\n");
				post_str.append(pbstr[i]);
			}
			SafeArrayUnaccessData(psa);
			return;
		}

		if (psa->fFeatures & FADF_VARIANT)
		{
			VARIANT HUGEP *pvar;
			hr = SafeArrayAccessData(psa, (void HUGEP**)&pvar);
			if (FAILED(hr))
				return;
			for (ULONG i = 0; i < psa->rgsabound[0].cElements-1; i++)
				FillPostData(post_str, pvar+i);
			SafeArrayUnaccessData(psa);
			return;
		}

		if (psa->fFeatures & FADF_HAVEVARTYPE)
		{
			VARTYPE vt = *(VARTYPE*)((BYTE*)psa-4);
			if (vt == VT_UI1)
			{
				tCHAR HUGEP *pdata;
				hr = SafeArrayAccessData(psa, (void HUGEP**)&pdata);
				if (FAILED(hr))
					return;
				if (post_str.length())
					post_str.append(L"\n");
				for (ULONG i = 0; i < psa->rgsabound[0].cElements-1; i++)
					post_str.append(pdata+i, cCP_ANSI, 1);
				SafeArrayUnaccessData(psa);
			}
			return;
		}
		return;
	}

	if (post->vt == VT_BSTR)
	{
		ODS(("KLSCAV:FillPostData vt=VT_BSTR, bstr='%S'", post->bstrVal));
		if (post_str.length())
			post_str.append(L"\n");
		post_str.append(post->bstrVal);
		return;
	}
	ODS(("KLSCAV:FillPostData vt=%d", post->vt));
}

BOOL ValidateThreadProcessIDs(DWORD pid, DWORD tid)
{
	BOOL bRes = FALSE;
	if (pid == 0 || tid == 0)
		return FALSE;
#if defined(_WIN64)
	HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, tid);
	if (!hThread)
		return FALSE;
	DWORD _pid = GetProcessIdOfThread(hThread);
	if (pid == _pid)
		bRes = TRUE;
	CloseHandle(hThread);
	return bRes;
#else // _WIN32
	HANDLE hSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32; 

	typedef HANDLE (WINAPI *tCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
	typedef BOOL   (WINAPI *tThread32First)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
	typedef BOOL   (WINAPI *tThread32Next)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);

	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	tCreateToolhelp32Snapshot pCreateToolhelp32Snapshot = (tCreateToolhelp32Snapshot)GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
	tThread32First            pThread32First            = (tThread32First)           GetProcAddress(hKernel32, "Thread32First");
	tThread32Next             pThread32Next             = (tThread32Next)            GetProcAddress(hKernel32, "Thread32Next");

	if( !pCreateToolhelp32Snapshot || !pThread32First || !pThread32Next )
		return FALSE;
	
	//  Take a snapshot of all threads 
	hSnap = pCreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hSnap != INVALID_HANDLE_VALUE ) 
	{
		te32.dwSize = sizeof(THREADENTRY32 ); 
		if( pThread32First( hSnap, &te32 ) ) 
		{
			do 
			{ 
				if( te32.th32OwnerProcessID == pid && te32.th32ThreadID == tid)
				{
					bRes = TRUE;		
					break;
				}
			} while( pThread32Next(hSnap, &te32 ) ); 
		}
		CloseHandle(hSnap);
	}
#endif // _WIN32
	return bRes;
}

// we need disable optimization to perform "dummy" read, over wise function will do nothing!
#pragma optimize( "g", off ) 
BOOL MyIsBadReadPtr(CONST VOID *lp, UINT_PTR ucb)
{
	__try
	{
		UINT_PTR i;
		LPBYTE pb = (LPBYTE)lp;
		BYTE b;
		for (i=0; i<ucb; i++)
			b = pb[i];
		return FALSE;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return TRUE;
	}
}
#pragma optimize( "", on )


ptrdiff_t GetByOffset(LPVOID ptr, DWORD offset, DWORD size)
{
	BYTE* bptr = (BYTE*)ptr;
	bptr += offset;
	if (MyIsBadReadPtr(bptr, sizeof(ptrdiff_t)))
		return 0;
	return *(ptrdiff_t*)bptr;
}

BOOL GetRPCClientInt(IServerSecurity* pServerSecurity, DWORD* pPid, DWORD* pTid)
{
	DWORD i;
	LPVOID pLRPC_SCALL = NULL;
	
	for (i=0; i < 0x40; i++)
	{
		ptrdiff_t addr = GetByOffset(pServerSecurity, 0x18 + i * sizeof(ptrdiff_t), sizeof(ptrdiff_t));
		if ((DWORD)addr > 0x77000000 || (DWORD)addr < (DWORD)GetProcessHeap())
			continue;
		if (addr & 3) // alignment
			continue;
		if (0x89abcdef != (DWORD)GetByOffset((LPVOID)addr, sizeof(ptrdiff_t), sizeof(ptrdiff_t)))
			continue;
		pLRPC_SCALL = (LPVOID)addr;
		break;
	}
	ODS(("KLSCAV:GetRPCClientInt: pLRPC_SCALL=%08X", pLRPC_SCALL));
	if (!pLRPC_SCALL)
		return FALSE;

	for (i=0x40; i<0x180; i++)
	{
		ptrdiff_t pid = GetByOffset(pLRPC_SCALL, i, 4);
		if (pid & 3) // alignment
			continue;
		if (pid < 8 || pid > 0x4000)
			continue;
		if (pid == GetCurrentProcessId())
			continue;
		ptrdiff_t tid = GetByOffset(pLRPC_SCALL, i + sizeof(ptrdiff_t), sizeof(ptrdiff_t));
		if (tid & 3) // alignment
			continue;
		if (tid == 0 || tid > 0x4000)
			continue;
		if (tid == GetCurrentThreadId())
			continue;
		if (ValidateThreadProcessIDs((DWORD)pid, (DWORD)tid))
		{
			if (pPid)
				*pPid = (DWORD)pid;
			if (pTid)
				*pTid = (DWORD)tid;
			ODS(("KLSCAV:GetRPCClientInt: LRPC::SCALL+%x pid=%08X tid=%08X", i, pid, tid));
			return TRUE;
		}
	}
	return FALSE;
}

typedef HRESULT (__stdcall* tCoGetCallContext)( REFIID riid, void **ppInterface );


BOOL GetRPCClient(DWORD* pPid, DWORD* pTid)
{
	IServerSecurity* pServerSecurity = NULL;
	static tCoGetCallContext pfCoGetCallContext = NULL;
	if (pPid)
		*pPid = 0;
	if (pTid)
		*pTid = 0;
	if (!pfCoGetCallContext)
	{
		HMODULE hLib = LoadLibrary("ole32.dll");
		if (hLib)
			pfCoGetCallContext = (tCoGetCallContext)GetProcAddress(hLib, "CoGetCallContext");
	}
	if (!pfCoGetCallContext)
		return FALSE;
	HRESULT hr = pfCoGetCallContext(IID_IServerSecurity , (void**)&pServerSecurity);
	if (hr == RPC_E_CALL_COMPLETE)
	{
		if (pPid)
			*pPid = GetCurrentProcessId();
		if (pTid)
			*pTid = GetCurrentThreadId();
		return TRUE;
	}
	if (FAILED(hr))
		pServerSecurity = NULL;
	ODS(("KLSCAV:GetRPCClient: IServerSecurity=%08X", pServerSecurity));
	if (!pServerSecurity)
		return FALSE;
	BOOL bRes = GetRPCClientInt(pServerSecurity, pPid, pTid);
	pServerSecurity->Release();
	return bRes;
}

tBYTE  g_aDecodeArray[256]; // --
tBYTE  g_aEncodeArray[64]; // --

void ReinitDecodeArray()
{
	static bool inited = false;

	if (inited)
		return;
	inited = true;
	size_t nIndex;
	
    for (nIndex = 0; nIndex < 64; ++nIndex)
    {
        int nValue ;
        if (nIndex < 26)
            nValue = (int)(nIndex + 'A') ;
        else if (nIndex < 52)
            nValue = (int)((nIndex - 26) + 'a') ;
        else if (nIndex < 62)
            nValue = (int)((nIndex - 52) + '0') ;
        else if (nIndex == 62)
            nValue = '+' ;
        else
            nValue = '/' ;

        g_aEncodeArray[nIndex] = (tBYTE)nValue ;
    }

	memset(g_aDecodeArray, 0xFF, sizeof(g_aDecodeArray)) ;
    for (nIndex = 0; nIndex < 64; ++nIndex)
        g_aDecodeArray[g_aEncodeArray[nIndex]] = (tBYTE)nIndex ;

//  aDecodeArray['='] = 64 ;
//  aDecodeArray[255] = '\xFF' ;
}

tERROR Base64Decode( tDWORD* p_result, const tBYTE* p_pEncodedData, tDWORD p_nDataSize, tBYTE* p_pDecodedBuffer, tDWORD p_nBufferSize ) {
	tERROR error = errOK;
    tDWORD read = 0, written = 0;
    tDWORD val = 0;
    tDWORD bits = 0;
	tBYTE b;

	if (!p_pDecodedBuffer && !p_nBufferSize)
	{
		// this is buffer size request
		if (!p_result)
			return errPARAMETER_INVALID;
		*p_result = ((p_nDataSize + 3) / 4) * 3;
		return errOK;
	}

	ReinitDecodeArray();

	if (p_result)
		*p_result = 0;

	if (!p_pEncodedData || !p_pDecodedBuffer)
		return errPARAMETER_INVALID;
	
	while (read < p_nDataSize)
    {
        b = p_pEncodedData[read];
		if (b == '=' || b=='\r' || b=='\n')
		{
			// ignore them
			read++;
			continue;
		}
		b = g_aDecodeArray[b];
        if (0xFF == b)
		{
            error =  errOBJECT_DATA_CORRUPTED; // invalid base64 sequence
			break;
		}
        val |= (bits == 0 ? ((b & 0x3F) << 2) : ((b & 0x3F) >> ((bits + 6) % 8))) ;
        bits += 6;
        if (bits >= 8)
        {
            // byte decoded
			if (written == p_nBufferSize)
			{
				error = errBUFFER_TOO_SMALL;
				break; // buffer too small
			}
			p_pDecodedBuffer[written++] = (tBYTE)val;
			bits -= 8;
            val = (b << (8 - bits)) ;
        }
        read++;
    }

	if (p_result)
		*p_result = written;

    return error;
}

HRESULT DispatchIENavigateHidden(DWORD dwActionId, void* pActionDataStruct, DWORD dwActionDataSize, DWORD* pdwFlags)
{
	tERROR error;
	ODS(("KLSCAV:DispatchIENavigateHidden..."));
	if (!pActionDataStruct)
		return E_INVALIDARG;
	if (dwActionDataSize != sizeof(SPD_IENavigateHiddenData))
		return E_INVALIDARG;
	if (!IsDispatchEnabled(dwActionId))
		return S_OK;
	if (!GetTaskObject("TaskManager", "KLPDM_PR_DEF884242", (hOBJECT*)&g_hTM, NULL))
		return E_NOINTERFACE;

	SPD_IENavigateHiddenData* pData = (SPD_IENavigateHiddenData*)pActionDataStruct;
	cAntiPhishingCheckRequest request;
	request.m_bInteractive = cFALSE;
	GetRPCClient((DWORD *)&request.m_nPID, (DWORD *)&request.m_nTID);
	request.m_sProcessName = g_szProcessName;

	if (GetCurrentProcessId() != request.m_nPID)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, request.m_nPID);
		if (hProcess)
		{
			CHAR  szProcessName[MAX_PATH*2];
			if (GetModuleFileNameEx(hProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(CHAR)))
			{
				request.m_sProcessName = szProcessName;
			}
			CloseHandle(hProcess);
		}
	}
	

	if (!pData->pUrl)
		return E_INVALIDARG;
	request.m_sURL = pData->pUrl;
	tDWORD nParams = request.m_sURL.find_first_of(L"?");
	if (nParams != cStrObj::npos)
	{
		request.m_sTemplate.append(request.m_sURL.substr(nParams+1));
		request.m_sURL.erase(nParams);
	}
	
	FillPostData(request.m_sTemplate, pData->pHeader);
	FillPostData(request.m_sTemplate, pData->pPost);

	ODS(("KLSCAV:DispatchIENavigateHidden: URL='%S'\nData='%S'", request.m_sURL.data(), request.m_sTemplate.substr(0, min(request.m_sTemplate.length(), 1000)).data()));

	tDWORD nEncodedLen = request.m_sTemplate.length();
	if (nEncodedLen)
	{
		tDWORD nEncodedData;
		for (nEncodedData=nEncodedLen-1; nEncodedData!=0; nEncodedData--)
		{
			if (request.m_sTemplate[nEncodedData] != '=')
				continue;
			if (nEncodedData == nEncodedLen-1) // ignore end fill
			{
				nEncodedLen--;
				continue;
			}
			break;
		}

		if (nEncodedData)
		{
			char encoded[0x1000];
			char decoded[0x1000];
			cStrObj strEncodedData = request.m_sTemplate.substr(nEncodedData+1);
			strEncodedData.copy(encoded, countof(encoded));
			encoded[countof(encoded)-1] = 0;
			tDWORD nBinLen;
			Base64Decode(&nBinLen, (tBYTE*)encoded, (tDWORD)strlen(encoded), (tBYTE*)decoded, countof(decoded)-1);
			char lc = 0;
			for (unsigned int i = 0; i<nBinLen; i++)
			{
				char c = decoded[i];
				if (c < 0x20 || lc >= 0x80)
				{
					if (lc < 0x20 || lc >= 0x80)
						continue;
				}
				else
					request.m_sReferrer.append(c);
				lc = c;
			}
			ODS(("KLSCAV:DispatchIENavigateHidden: DecodedData='%S'", request.m_sReferrer.substr(0, min(request.m_sReferrer.length(), 1000)).data()));
		}
	}

	ODS(("KLSCAV:DispatchIENavigateHidden: Sending msg_IE_NAVIGATE_HIDDEN request..."));
	error = g_hTM->sysSendMsg(pmc_IE_NAVIGATE_HIDDEN, msg_IE_NAVIGATE_HIDDEN, NULL, &request, SER_SENDMSG_PSIZE);
	if (error == errACCESS_DENIED)
	{
		ODS(("KLSCAV:DispatchIENavigateHidden: Check finished,  error=errACCESS_DENIED, should be blocked"));
		if (pdwFlags)
			*pdwFlags |= SCP_BLOCK; 
		return E_ACCESSDENIED;
	}
	
	if (error == errOK_DECIDED)
	{
		ODS(("KLSCAV:DispatchIENavigateHidden: Check finished error=errOK_DECIDED, allowed", error));
		return S_OK;
	}

	if (error == errOK_NO_DECIDERS)
	{
		ODS(("KLSCAV:DispatchIENavigateHidden: Check finished error=errOK_NO_DECIDERS, allowed", error));
		return S_OK;
	}


	ODS(("KLSCAV:DispatchIENavigateHidden: Check finished error=%08X, unexpected reply!!!", error));
	return E_UNEXPECTED;
}


BOOL __stdcall IsDispatchEnabled(DWORD dwActionId)
{
	switch(dwActionId) 
	{
	case SPD_IsBanner:
		if (g_hAntiBannerTask)
			return TRUE;
		return _IsTaskActive("KLANTIBANNER_PR_DEF6900421");
	
	case msg_PStoreCreateInstance:
	case SPD_IENavigateHidden:
		return _IsTaskActive("KLPDM_PR_DEF884242");
	}
	return FALSE;
}


HRESULT __stdcall Dispatch(DWORD dwActionId, void* pActionDataStruct, DWORD dwActionDataSize, DWORD* pdwFlags)
{
	switch(dwActionId) {
	case SPD_IsBanner:
	case SPD_IsWLBanner:
		return DispatchIsBanner(dwActionId, pActionDataStruct, dwActionDataSize, pdwFlags);
	case SPD_IENavigateHidden:
		return DispatchIENavigateHidden(dwActionId, pActionDataStruct, dwActionDataSize, pdwFlags);
	case msg_PStoreCreateInstance:
		return DispatchPStoreCreateInstance(dwActionId, pActionDataStruct, dwActionDataSize, pdwFlags);
	}
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	HRESULT hRes = E_FAIL;
	LONG lRes = RegDeleteKey(HKEY_LOCAL_MACHINE, PLUGIN_REG_KEY);
	if (lRes==ERROR_SUCCESS || lRes==ERROR_PATH_NOT_FOUND || lRes==ERROR_FILE_NOT_FOUND)
		hRes = S_OK;
	// cleanup registry
	RegDeleteKey(HKEY_LOCAL_MACHINE, SC_REG_PLUGINS);
	RegDeleteKey(HKEY_LOCAL_MACHINE, SC_REG_KEY);
	RegDeleteKey(HKEY_LOCAL_MACHINE, COMPONENTS_REG_KEY);
	RegDeleteKey(HKEY_LOCAL_MACHINE, KL_REG_KEY);
	return hRes;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	LONG nResult; 
	HKEY hPluginKey;

	DllUnregisterServer();

	nResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, PLUGIN_REG_KEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hPluginKey, NULL);
	if (ERROR_SUCCESS == nResult)
	{
		CHAR szModule[_MAX_PATH];
		CHAR* pszModule;
		DWORD dwDword;

		GetModuleFileName(g_hInstance, szModule, _MAX_PATH);

		if ((g_hInstance == NULL) || (g_hInstance == GetModuleHandle(NULL))) // register as EXE
		{
			// Convert to short path to work around bug in NT4's CreateProcess
			CHAR szModuleShort[_MAX_PATH];
			int cbShortName = GetShortPathName(szModule, szModuleShort, _MAX_PATH);

			if (cbShortName == _MAX_PATH)
				return E_OUTOFMEMORY;

			pszModule = (cbShortName == 0 || cbShortName == ERROR_INVALID_PARAMETER) ? szModule : szModuleShort;
		}
		else
			pszModule = szModule;

		nResult = RegSetValue(hPluginKey, NULL, REG_SZ, (LPCSTR)pszModule, (DWORD)strlen(pszModule));
		if (ERROR_SUCCESS == nResult)
		{
			dwDword = PLUGIN_PRIORITY;
			nResult = RegSetValueEx(hPluginKey, "Priority", NULL, REG_DWORD, (LPBYTE)&dwDword, sizeof(dwDword));
		}
		if (ERROR_SUCCESS == nResult)
		{
			dwDword = 1;
			nResult = RegSetValueEx(hPluginKey, "Enabled", NULL, REG_DWORD, (LPBYTE)&dwDword, sizeof(dwDword));
		}

		RegCloseKey(hPluginKey);
	}

	if (nResult != ERROR_SUCCESS)
		return E_FAIL;
	return S_OK;

}

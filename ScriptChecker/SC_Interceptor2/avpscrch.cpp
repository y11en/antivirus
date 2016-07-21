// avpscrch.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f avpscrchps.mk in the project directory.
//#include "stdafx.h"

#include "../../Windows/Hook/Hook/idriver.h"
#include "resource.h"
#include <initguid.h>
#include "debug.h"
#include "signchk.h"
#include "../../Windows/Hook/R3Hook/sputnik.h"
#include <tchar.h>


#include "HookClassFactory.h"
#include "CommonStruct.h"

#include "scr_inst.h"

#include "InstanceQ.h"

#include "../../windows/hook/r3hook/hookbase64.h"
#include "ActiveScriptHooks.h"
#include "appinfo.h"

void UpdateReg(BOOL bSet);

HINSTANCE g_hInstance = NULL;
BOOL g_bDisableAllHooks = FALSE;

extern BOOL bDontDelete;

char* _OriginalDll = "OriginalDll";

//DWORD g_TlsCurEngineInstance = TLS_OUT_OF_INDEXES;
DWORD g_TlsSCThreadData  = TLS_OUT_OF_INDEXES;


/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
void main(void){}

STDMETHODIMP Hook_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
#ifdef _DEBUG
	char szDbgBuff[0x100];
	char szDbgBuff2[0x100];
#endif

	hRes = ((HRESULT (__stdcall *)(REFCLSID, REFIID , void**))OrigFuncAddr)(rclsid, riid, ppv);
	ODS(("DllGetClassObject( %s, %s ) = %08X", DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &rclsid), DbgStringFromGUID(szDbgBuff2, sizeof(szDbgBuff2), &riid), 
		(SUCCEEDED(hRes) && ppv && *ppv) ? *ppv : NULL));
	if (SUCCEEDED(hRes) && ppv && *ppv)
	{
		ODS(("DllGetClassObject->HookNewObjectMethods"));
		HookNewObjectMethods(NULL, *ppv, riid);
	}
	return hRes;
}

BOOL SetDllGetClassObjectHook(char* szModuleName)
{
	HMODULE hModule = GetModuleHandle(szModuleName);
	if (!hModule)
		return FALSE;
	return HookExportInModule(hModule, "DllGetClassObject", Hook_DllGetClassObject, NULL, 0);
}

static const tHOOK_TBL_ENTRY HookTable[] = {
	{ "DllGetClassObject", NULL, Hook_DllGetClassObject, NULL },
	{ "PStoreCreateInstance", "pstorec.dll", Hook_PStoreCreateInstance, NULL },
	LAST_HOOK_TBL_ENTRY
};

extern "C" __declspec(dllexport) VOID __cdecl hmi(HMODULE hModule)
{
	ODS(("hmi(%08X)", hModule));
	if (!hModule)
		return;
	if (CheckAppFlags(APP_FLAG_DONT_HOOK))
		return;
	HookImportInModuleTbl(hModule, (tHOOK_TBL_ENTRY*)HookTable, 0);
}

extern "C" __declspec(dllexport) VOID __cdecl hme(HMODULE hModule)
{
	ODS(("hme(%08X)", hModule));
	if (!hModule)
		return;
	if (CheckAppFlags(APP_FLAG_DONT_HOOK))
		return;
	HookExportInModuleTbl(hModule, (tHOOK_TBL_ENTRY*)HookTable, 0);
}

BOOL IsLoadedIntoAVP(HINSTANCE hInstance)
{
	TCHAR szPath[MAX_PATH];
	TCHAR szPath1[MAX_PATH];
	TCHAR szPath2[MAX_PATH];
	if (!GetModuleFileName(NULL, szPath, _countof(szPath)))
		return FALSE;
	if (!GetShortPathName(szPath, szPath1, _countof(szPath1)))
		return FALSE;
	if (!GetModuleFileName(hInstance, szPath, _countof(szPath)))
		return FALSE;
	if (!GetShortPathName(szPath, szPath2, _countof(szPath2)))
		return FALSE;
	TCHAR* p = _tcsrchr(szPath1, '\\');
	if (!p)
		return FALSE;
	*p = 0;
	p = _tcsrchr(szPath2, '\\');
	if (!p)
		return FALSE;
	*p = 0;
	if (0 != _tcsicmp(szPath1, szPath2))
		return FALSE;
	return TRUE;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	static HANDLE g_hMutex_KavInProcessDllLoaded = NULL;
	BOOL bRes = TRUE;
	g_hInstance = hInstance;
    if (dwReason == DLL_THREAD_DETACH)
    {
        FreeSCThreadData();
    }
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		//char fn[MAX_PATH];
		ODS(("DLL_PROCESS_ATTACH"));
		
		ScriptEngineListInit();

#ifdef _DEBUG
		if (0 != GetModuleHandle("msdev.exe"))
			return FALSE;
#endif
		if (IsLoadedIntoAVP(hInstance))
		{
			SetAppFlags(APP_FLAG_DONT_HOOK);
			return TRUE;
		}

		HookBaseInit();

		if (!GetModuleHandle("r3hook")) // loading with KLIF
		{
			hme(GetModuleHandle("shdocvw"));
			hme(GetModuleHandle("ieframe"));
			{
				HMODULE hModule = GetModuleHandle("pstorec");
				if (hModule)
				{
					hme(hModule);
					LoadLibrary("pstorec");
				}
			}
		}

#define KAV_INPROCESS_DLL_LOADED "KAV_INPROCESS_DLL_LOADED"
		g_hMutex_KavInProcessDllLoaded = CreateMutex(NULL, cFALSE, KAV_INPROCESS_DLL_LOADED);

//		g_TlsCurEngineInstance = TlsAlloc();
		g_TlsSCThreadData = TlsAlloc();
		if (//g_TlsCurEngineInstance == TLS_OUT_OF_INDEXES || 
			g_TlsSCThreadData == TLS_OUT_OF_INDEXES)
		{
			//::MessageBox(NULL, "Error allocating TLS. KAV Script Checker will continue without some functionality.", "KAV Script Checker", 0);
			ODS(("Error allocating TLS. KAV Script Checker will continue without some functionality."));
		}

//		// verify file signature
//		::GetModuleFileName(hInstance, fn, MAX_PATH);
//#if defined(_DEBUG)
//		int sign_chk_res = SIGN_OK;
//#else
//		int sign_chk_res = _sign_check_file(fn);
//#endif
//
//		if (sign_chk_res != SIGN_OK && sign_chk_res != SIGN_NOT_ENOUGH_MEMORY)
//		{
//			char buff[0x400];
//			switch(sign_chk_res)
//			{
//			case SIGN_FILE_NOT_FOUND:
//				wsprintf(buff, "Script Checker cannot verify signature for executable file '%s'. This may be result of virus activity or data corruption. To restore proper functionality you need to reinstall KAV Script Checker.\nDo you wish to continue?", fn);
//				break;
//			default:
//				wsprintf(buff, "Executable file '%s' was changed (code=%d). This may be result of virus activity or data corruption. To restore proper functionality you need to reinstall KAV Script Checker.\nDo you wish to continue?", fn, sign_chk_res);
//				break;
//			}
//			if (::MessageBox(NULL, buff, "KAV Script Checker", MB_ICONSTOP | MB_YESNO | MB_DEFBUTTON2) != IDYES)
//				return FALSE;
//		}

		ODS(("DLL_PROCESS_ATTACH finished"));
    }

    if (dwReason == DLL_PROCESS_DETACH)
	{
		ODS(("DLL_PROCESS_DETACH"));
		
		FreeSCThreadData();
        HookBaseDone();

//		if (g_TlsCurEngineInstance != TLS_OUT_OF_INDEXES)
//			TlsFree(g_TlsCurEngineInstance);
		if (g_TlsSCThreadData != TLS_OUT_OF_INDEXES)
			TlsFree(g_TlsSCThreadData);
//		g_TlsCurEngineInstance = TLS_OUT_OF_INDEXES;
		g_TlsSCThreadData = TLS_OUT_OF_INDEXES;


#ifdef _PRAGUE
//		if (PR_SUCC(PragueLoadResult) && g_RP_API.PRDeinitialize)
//			g_RP_API.PRDeinitialize();
#endif

		ScriptEngineListDone();

		FreeEngineQueue();

		if (g_hMutex_KavInProcessDllLoaded)
			CloseHandle(g_hMutex_KavInProcessDllLoaded);

		ODS(("DLL_PROCESS_DETACH finished"));
	}

    if (dwReason == DLL_THREAD_ATTACH)
	{
//		if (g_TlsCurEngineInstance != TLS_OUT_OF_INDEXES)
//			TlsSetValue(g_TlsCurEngineInstance, 0); // init tls value
		if (g_TlsSCThreadData != TLS_OUT_OF_INDEXES)
		{
			TlsSetValue(g_TlsSCThreadData, NULL); // init tls value
		}
		ODS(("DLL_THREAD_ATTACH"));
	}

	if (dwReason == DLL_THREAD_DETACH)
	{
		FreeSCThreadData();
		ODS(("DLL_THREAD_DETACH"));
	}

    return bRes;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	//LONG LockCount = _Module.GetLockCount();
	//HRESULT hResult = S_OK; // = (LockCount == 0 ? S_OK : S_FALSE);
	//if (EngineCanUnload() == S_FALSE)
	//	hResult = S_FALSE;
	//ODS(("KAVSC: Can unload=%s. LockCount=%d\n", hResult == S_OK ? "S_OK" : "S_FALSE", LockCount));
    //return hResult;
	return S_FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _DEBUG
	char szDbgBuff[0x100];
	char szDbgBuff2[0x100];
	ODS(("DllGetClassObject( %s, %s )",
		DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &rclsid),
		DbgStringFromGUID(szDbgBuff2, sizeof(szDbgBuff2), &riid)
		));
#endif

	HRESULT hResult = CLASS_E_CLASSNOTAVAILABLE;
	CClassFactory* pClassFactory;
	pClassFactory = new CClassFactory(rclsid, riid, ppv);
	if (pClassFactory != NULL)
	{
		if (pClassFactory->m_ppv == NULL)
		{
			delete pClassFactory;
			pClassFactory = NULL;
		}
		else
		{
			//ODS(("Call original ClassFactory %08X", pClassFactory));
			hResult = pClassFactory->m_hResult;
			*ppv = (LPVOID) (IClassFactory*)pClassFactory;
			ODS(("Original ClassFactory returned hResult=%08X", hResult));
		}
	}
	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
	UpdateReg(TRUE);
	RegisterSputnikDllByHandle("shdocvw.dll", g_hInstance);
	RegisterSputnikDllByHandle("ieframe.dll", g_hInstance);
	RegisterSputnikDllByHandle("pstorec.dll", g_hInstance);
    return S_OK;//_Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	UpdateReg(FALSE);
	UnregisterSputnikDllByHandle("shdocvw.dll", g_hInstance);
	UnregisterSputnikDllByHandle("ieframe.dll", g_hInstance);
	UnregisterSputnikDllByHandle("pstorec.dll", g_hInstance);
    return S_OK;//_Module.UnregisterServer(TRUE);
}


/*
[HKEY_CLASSES_ROOT\CLSID\{f414c260-6ac0-11cf-b6d1-00aa00bbbb58}\InprocServer32]
@="C:\\PROGRAM FILES\\KASPERSKY LAB\\AVP6\\SCRCHPG.DLL"
"ThreadingModel"="Both"
"ScrAuthInProcServer32"="c:\\windows\\system\\jscript.dll"
"OriginalDll"="C:\\PROGRAM FILES\\COMMON FILES\\SYMANTEC SHARED\\SCRIPT BLOCKING\\SCRAUTH.DLL"
*/

void UpdateReg(BOOL bSet = TRUE)
{
	BOOL bFail = FALSE;
	HKEY hKey = NULL;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, "CLSID", 0, KEY_READ, &hKey))
	{
		TCHAR CurrentPath[2048];
		GetModuleFileName(g_hInstance, CurrentPath, sizeof(CurrentPath));

		TCHAR SubkeyName[512];
		DWORD dwSubkeyName = sizeof(SubkeyName);
		HKEY hSubKey;
		HKEY hUpdatedKey;
		TCHAR SubName[2048];
		TCHAR Buffer[2048];
		ULONG uKeyLenght;
		TCHAR LangName[128];
 
		DWORD TypeStr;
		DWORD dwIndex = 0;
		char* GuidArr[] = {
			"{F0B7A1A1-9847-11CF-8F20-00805F2CD064}",	// "Active Scripting Engine"
			"{F0B7A1A2-9847-11CF-8F20-00805F2CD064}",	// "Active Scripting Engine with Parsing"
			"{F0B7A1A3-9847-11CF-8F20-00805F2CD064}",	// "Active Scripting Engine with Encoding"
			"{0AEE2A92-BCBB-11D0-8C72-00C04FC2B085}",	// "Active Scripting Engine with Authoring"
		};
		while (RegEnumKey(hKey, dwIndex++, SubkeyName, dwSubkeyName) == ERROR_SUCCESS)
		{
			TypeStr = REG_SZ;
			for (int cou_guid = 0; cou_guid < 4/*sizeof(GuidArr)/sizeof(GuidArr[0])*/; cou_guid++)
			{
				wsprintf(SubName, "CLSID\\%s\\Implemented Categories\\%s", SubkeyName, GuidArr[cou_guid]);
				if(RegOpenKeyEx(HKEY_CLASSES_ROOT, SubName, 0, KEY_READ | KEY_WRITE, &hSubKey) == ERROR_SUCCESS)
				{
					uKeyLenght= sizeof(LangName);
					HKEY hsk;
					
					wsprintf(SubName, "CLSID\\%s", SubkeyName);
					if(RegOpenKeyEx(HKEY_CLASSES_ROOT, SubName, 0, KEY_READ, &hsk) == ERROR_SUCCESS)
					{
						RegQueryValue(hsk, NULL, LangName, (PLONG) &uKeyLenght);
						RegCloseKey(hsk);
					}
					else
					{
						wsprintf(LangName, "<unknown>");
					}
					
	#ifdef _SHOWDEBUG
					ODSN(("%s  (Name is: %s) ... updating", SubkeyName, LangName));
	#endif

					wsprintf(SubName, "CLSID\\%s\\InprocServer32", SubkeyName);
					if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, SubName, 0, KEY_ALL_ACCESS, &hUpdatedKey))
					{
						uKeyLenght = sizeof(Buffer);
						if (RegQueryValueEx(hUpdatedKey, _OriginalDll, 0, &TypeStr, (LPBYTE) Buffer, &uKeyLenght) != ERROR_SUCCESS)
						{
							if (bSet == TRUE)
							{
								TypeStr = REG_SZ;
								uKeyLenght = sizeof(Buffer);
								if (RegQueryValueEx(hUpdatedKey, NULL, 0, &TypeStr, (LPBYTE) Buffer, &uKeyLenght) == ERROR_SUCCESS)
								{
									if (RegSetValueEx(hUpdatedKey, _OriginalDll, 0, REG_SZ, (CONST BYTE*) Buffer, lstrlen(Buffer)) == ERROR_SUCCESS)
									{
										if (RegSetValueEx(hUpdatedKey, NULL, 0, REG_SZ, (CONST BYTE*) CurrentPath, lstrlen(CurrentPath)) != ERROR_SUCCESS)
											bFail = TRUE;
									}
								}
								else
									bFail = TRUE;
	#ifdef _SHOWDEBUG
								ODS(("... successful"));
	#endif
							}
							else
							{
	#ifdef _SHOWDEBUG
								ODS(("... not installed"));
	#endif
							}
						}
						else
						{
							if (bSet == TRUE)
							{
								if (RegSetValueEx(hUpdatedKey, NULL, 0, REG_SZ, (CONST BYTE*) CurrentPath, lstrlen(CurrentPath)) == ERROR_SUCCESS)
								{
	#ifdef _SHOWDEBUG
									ODS(("... replaced"));
	#endif
								}
								else
									bFail = TRUE;
							}
							else
							{
								if (RegSetValueEx(hUpdatedKey, NULL, 0, REG_SZ, (CONST BYTE*) Buffer, lstrlen(Buffer)) == ERROR_SUCCESS)
								{
									RegDeleteValue(hUpdatedKey, _OriginalDll);
	#ifdef _SHOWDEBUG
									ODS(("... restored"));
	#endif
								}
								else
									bFail = TRUE;
							}

						}
						if (bFail == TRUE)
						{
	#ifdef _SHOWDEBUG
							ODS(("... fail!!!"));
	#endif
						}
						RegCloseKey(hUpdatedKey);
					}
					else
					{
	#ifdef _SHOWDEBUG
						ODS(("... updating failed"));
	#endif
					}
					RegCloseKey(hSubKey);
					break;
				}
			}
		}
		RegCloseKey(hKey);
	}
	else
	{
#ifdef _SHOWDEBUG
		ODS(("Can't access to HKCR"));
#endif
	}
}
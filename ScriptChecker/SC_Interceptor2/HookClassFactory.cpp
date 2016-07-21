//#include "stdafx.h"
#define _CRTIMP

#include "scripting/activscp.h"

#include "HookClassFactory.h"

#include "wtypes.h"

#include "InstanceQ.h"
#include "hooksymb.h"
#include "SymbHooks_JS.h"
#include "SymbHooks_VBS.h"
#include "ActiveScriptHooks.h"
#include "ChkScript.h"
#include "scr_inst.h"
#include "../../windows/hook/r3hook/hookbase64.h"
#include "appinfo.h"

#include "debug.h"

// -----------------------------------------------------------------------------------------

BOOL GetOriginalDllName(REFCLSID rclsid, LPOLESTR pDllName, WCHAR** pLangName, BOOL* bEncoded)
{
	HKEY hKey = NULL;
	char KeyString[MAX_PATH];// = "CLSID\\+rclsid";
	DWORD cbData;
	char buf[MAX_PATH];
	DWORD TypeStr = REG_SZ;
	LONG l1;

	OLECHAR* pguid;
	StringFromCLSID(rclsid, &pguid);
	*bEncoded = FALSE;

	wsprintf(KeyString, "CLSID\\%S\\ProgID", pguid);
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, KeyString, 0, KEY_READ, &hKey))
	{
		cbData = MAX_PATH;
		l1 = RegQueryValueEx(hKey, NULL, NULL, &TypeStr,(LPBYTE)&buf,&cbData);
		if (l1 == ERROR_SUCCESS)
		{
			DWORD dwAllocSize = (lstrlen(buf) + 1) * sizeof(WCHAR);
			*pLangName = (WCHAR*) HeapAlloc(GetProcessHeap(), 0, dwAllocSize);
			if (!*pLangName)
			{
				ODS(("ERROR: Can't allocate memory for langname"));
			}
			else
			{
				if (0 == ::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, buf, -1, *pLangName, dwAllocSize))
				{
					ODS(("ERROR: Can't convert langname"));
					*pLangName = 0;
				}
				else
				{
					// lcase name extension
					WCHAR* pLangNameExt = NULL;
					WCHAR* pLangNameTmp = *pLangName;
					while (*pLangNameTmp)
					{
						if (*pLangNameTmp == '.')
							pLangNameExt = pLangNameTmp+1;
						pLangNameTmp++;
					}
					if (pLangNameExt)
					{
						pLangNameTmp = pLangNameExt;
						while (*pLangNameTmp)
						{
							*pLangNameTmp |= 0x20;
							pLangNameTmp++;
						}
							
						if (wcsncmp(pLangNameExt, L"encode", 6) == 0)
						{
							pLangNameExt[-1] = 0; // cut extension
							*bEncoded = TRUE;
						}
					}
					ODS(("LangName = %S", *pLangName));
				}
			}
		}
		RegCloseKey(hKey);
	}
	else
	{
		ODS(("ERROR: Can't open reg for langname"));
	}

	wsprintf(KeyString, "CLSID\\%S\\InprocServer32", pguid);
	CoTaskMemFree(pguid);
	TypeStr = REG_SZ;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, KeyString, 0, KEY_READ, &hKey))
	{
		cbData = MAX_PATH;
		l1 = RegQueryValueEx(hKey, "OriginalDll", NULL,(unsigned long*)&TypeStr,(LPBYTE)&buf,&cbData);
		if (l1 == ERROR_SUCCESS)
		{
			DoEnvironmentSubst(buf, sizeof(buf));
			ODS(("Original dll name is: %s", buf));
			::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, buf, -1, pDllName, lstrlen(buf) + 1);
			RegCloseKey(hKey);
			return TRUE;
		}
		else
		{
			ODS(("ERROR: Query reg value \"OriginalDll\" failed!"));
		}
		RegCloseKey(hKey);
	}
	else
	{
		ODS(("ERROR: Can't open reg for original dll name\n"));
	}

	ODS(("ERROR: GetOriginalDllName failed\n"));
	return FALSE;
}




///////////////////////////////////////////////////////////////////////////
//
// IClassFactory Methods
//

#ifdef _DEBUG
HRESULT __stdcall HookCoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
	char _guid[0x100];
	void* OrigFuncAddr = HookGetOrigFunc();
	ODS(("CoCreateInstance(%s)\n", DbgStringFromGUID(_guid, sizeof(_guid), &rclsid)));
	return ((HRESULT (__stdcall *)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR*))OrigFuncAddr)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}
HRESULT __stdcall HookCoCreateInstanceEx(
    REFCLSID                    Clsid,
    IUnknown    *               punkOuter, // only relevant locally
    DWORD                       dwClsCtx,
    COSERVERINFO *              pServerInfo,
    DWORD                       dwCount,
    MULTI_QI        *           pResults)
{
	char _guid[0x100];
	void* OrigFuncAddr = HookGetOrigFunc();
	ODS(("CoCreateInstanceEx(%s)\n", DbgStringFromGUID(_guid, sizeof(_guid), &Clsid)));
	return ((HRESULT (__stdcall *)(REFCLSID, IUnknown*, DWORD, COSERVERINFO*, DWORD, MULTI_QI*))OrigFuncAddr)(Clsid,punkOuter,dwClsCtx,pServerInfo,dwCount,pResults);
}

HRESULT __stdcall HookCoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv)
{
	HRESULT hres;
	void* OrigFuncAddr = HookGetOrigFunc();
	char _guid[0x100];
	ODS(("CoGetClassObject(%s)\n", DbgStringFromGUID(_guid, sizeof(_guid), &rclsid)));
//	if (0 == strcmp(_guid, "Shockwave Flash Object"))
//		return E_NOINTERFACE;
	hres = ((HRESULT (__stdcall *)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID FAR*))OrigFuncAddr)(rclsid, dwClsContext, pvReserved, riid, ppv);
	return hres;
}

#endif

CClassFactory::CClassFactory(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
	:m_clsidObject(rclsid)
{
	ODS(("CClassFactory::CClassFactory"));

	m_nRefCount = 1;
	m_ppv = NULL;
	WCHAR DllName[MAX_PATH];
	
//#ifdef _DEBUG
//	static bool bCoCreateInstanceHooked = false;
//	if(!bCoCreateInstanceHooked)
//	{
//		//HookProcInProcess("CoCreateInstance", HookCoCreateInstance, NULL, FLAG_HOOK_EXPORT | FLAG_HOOK_IMPORT);
//		//HookProcInProcess("CoCreateInstanceEx", HookCoCreateInstanceEx, NULL, FLAG_HOOK_EXPORT | FLAG_HOOK_IMPORT);
//		HookProcInProcess("CoGetClassObject", HookCoGetClassObject, NULL, FLAG_HOOK_EXPORT | FLAG_HOOK_IMPORT);
//		bCoCreateInstanceHooked = true;
//	}
//#endif
	
	lstrcpy((char*) DllName, "<not resolved>");

	if (GetOriginalDllName(rclsid, DllName, &m_pLanguage, &m_bEncoded) == FALSE)
		return;
	if (m_pLanguage == NULL)
		m_pLanguage = L"<unknown>";
	
	HINSTANCE hInstance = IsEngineLoaded(DllName);
	if (hInstance == NULL)
	{
		hInstance = GetEngineInstance(DllName, m_pLanguage);
		if (hInstance == NULL)
		{
			char errmsg[MAX_PATH*2];
			wsprintf(errmsg, "Cannot load library for language '%S'\nPath: '%S'\nPlease contact Kaspersky Lab for the solution. (err=%08X)", m_pLanguage, DllName, GetLastError());
			::MessageBox(NULL, errmsg, "KAV Script Checker", MB_ICONSTOP);
			return;
		}

		if (CheckAppFlags(APP_FLAG_DONT_HOOK))
		{
			ODS(("SC: silent mode"));
		}
		else
		{

			// {B54F3741-5B07-11CF-A4B0-00AA004A55E8}
			GUID IID_VBScriptLanguage = {0xB54F3741, 0x5B07, 0x11CF, 0xA4, 0xB0, 0x00, 0xAA, 0x00, 0x4A, 0x55, 0xE8};
			// {B54F3742-5B07-11CF-A4B0-00AA004A55E8}
			GUID IID_VBScriptLanguageAuthoring = {0xB54F3742, 0x5B07, 0x11CF, 0xA4, 0xB0, 0x00, 0xAA, 0x00, 0x4A, 0x55, 0xE8};
			// {B54F3743-5B07-11CF-A4B0-00AA004A55E8}
			GUID IID_VBScriptLanguageEncoding = {0xB54F3743, 0x5B07, 0x11CF, 0xA4, 0xB0, 0x00, 0xAA, 0x00, 0x4A, 0x55, 0xE8};
			
			if ((memcmp(&rclsid, &IID_VBScriptLanguage, sizeof(GUID)) == 0) ||
				(memcmp(&rclsid, &IID_VBScriptLanguageAuthoring, sizeof(GUID)) == 0) ||
				(memcmp(&rclsid, &IID_VBScriptLanguageEncoding, sizeof(GUID)) == 0))
			{
				ODS(("Set symbol hooks for VBS"));
				SetSymbolsHooks(hInstance, sizeof(VbsHooks)/sizeof(VbsHooks[0]), VbsHooks);		
			}

			//	{F414C260-6AC0-11CF-B6D1-00AA00BBBB58}
			GUID IID_JScriptLanguage = {0xF414C260, 0x6AC0, 0x11CF, 0xB6, 0xD1, 0x00, 0xAA, 0x00, 0xBB, 0xBB, 0x58};
			//	{F414C261-6AC0-11CF-B6D1-00AA00BBBB58}
			GUID IID_JScriptLanguageAuthoring = {0xF414C261, 0x6AC0, 0x11CF, 0xB6, 0xD1, 0x00, 0xAA, 0x00, 0xBB, 0xBB, 0x58};
			//	{F414C262-6AC0-11CF-B6D1-00AA00BBBB58}
			GUID IID_JScriptLanguageEncoding = {0xF414C262, 0x6AC0, 0x11CF, 0xB6, 0xD1, 0x00, 0xAA, 0x00, 0xBB, 0xBB, 0x58};

			if ((memcmp(&rclsid, &IID_JScriptLanguage, sizeof(GUID)) == 0) ||
				(memcmp(&rclsid, &IID_JScriptLanguageAuthoring, sizeof(GUID)) == 0) ||
				(memcmp(&rclsid, &IID_JScriptLanguageEncoding, sizeof(GUID)) == 0))
			{
				ODS(("Set symbol hooks for JS"));
				SetSymbolsHooks(hInstance, sizeof(JsHooks)/sizeof(JsHooks[0]), JsHooks);		
			}
		}
	}
	else
	{
		ODS(("Engine dll already loaded\n"));
	}

	FARPROC pFunc = GetProcAddress(hInstance, "DllGetClassObject");
	if (pFunc != NULL)
	{
		m_hResult = ((HRESULT(__stdcall *)(REFCLSID, REFIID, LPVOID*))pFunc)(rclsid, riid, ppv);
		switch (m_hResult)
		{
		case  S_OK:
			m_ppv = (IClassFactory*)*ppv;
			break;
		case CLASS_E_CLASSNOTAVAILABLE:
			ODS(("ERROR: DllGetClassObject - CLASS_E_CLASSNOTAVAILABLE!"));
			break;
		default:
			ODS(("ERROR: DllGetClassObject - Unknown result!"));
			break;
		}
	}
	else
	{
		ODS(("FARPROC is NULL!\n"));
	}
}

CClassFactory::~CClassFactory()
{
	if (m_pLanguage != NULL)
		HeapFree(GetProcessHeap(), 0, m_pLanguage);
	m_ppv = NULL;
//	_Module.Unlock();
}

STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, LPVOID* ppvObject)
{
#ifdef _DEBUG
	char szDbgBuff[0x100];
	ODS(("ClassFactory::QueryInterface( %s )", DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid)));
#endif 

	if (memcmp(&riid, &IID_IClassFactory, sizeof(IID))==0)
	{
		AddRef();
		*ppvObject = (LPVOID)this;
		ODS(("CClassFactory::QueryInterface returning self"));
		return S_OK;
	}

	if (m_ppv == NULL)
	{
		*ppvObject = NULL;
		ODS(("CClassFactory::QueryInterface m_ppv==NULL, returning E_NOINTERFACE"));
		return E_NOINTERFACE;
	}

	
	HRESULT hRes;
	hRes = m_ppv->QueryInterface(riid, ppvObject);

	if (SUCCEEDED(hRes) && ppvObject)
	{   
		if (CheckAppFlags(APP_FLAG_DONT_HOOK))
		{
			ODS(("SC: silent mode"));
		}
		else
		{
			// Hook QueryInterface for each queried interface
			HookNewObjectMethods(m_ppv, *ppvObject, riid);
		}
	}

	if (FAILED(hRes))
	{
		ODS(("ERROR: CClassFactory::QueryInterface failed with %08X", hRes));
	}

	return hRes;
}                                             

STDMETHODIMP_(ULONG) CClassFactory::AddRef()
{
	ODS(("CClassFactory::AddRef[%d->%d]", m_nRefCount, m_nRefCount+1));
	if (m_ppv == NULL)
	{
		DBG_STOP;
		m_nRefCount++;
		return m_nRefCount;
	}

	return  m_ppv->AddRef();
}

STDMETHODIMP_(ULONG) CClassFactory::Release()
{
	ULONG result;

	ODS(("CClassFactory::Release [%d->%d]", m_nRefCount, m_nRefCount-1));
	
	if (m_ppv == NULL)
		result = --m_nRefCount;
	else
		result = m_ppv->Release();
	
	if (result == 0)
		delete this;
	return result;
}

STDMETHODIMP CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObject)
{
	HRESULT hRes;
#ifdef _DEBUG
	char szDbgBuff[0x100];
	ODS(("ClassFactory::CreateInstance( %s )", DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid)));
#endif 

	if (m_ppv == NULL)
	{
		ODS(("CClassFactory::CreateInstance m_ppv==NULL, returning E_NOTIMPL"));
		*ppvObject = NULL;
		return E_NOTIMPL;
	}
	
	hRes = m_ppv->CreateInstance(pUnkOuter, riid, ppvObject);

	//__asm int 3;

	if (SUCCEEDED(hRes))
	{
		if (CheckAppFlags(APP_FLAG_DONT_HOOK))
		{
			ODS(("SC: silent mode"));
		}
		else
		{

			// New instance created
			// register it in global list

			ODS(("ClassFactory::CreateInstance, pvObject = %08X",*ppvObject));
			
			HookNewObjectMethods(*ppvObject, *ppvObject, riid);
			if (riid == IID_IUnknown)
			{ 
				IUnknown* pUnk = (IUnknown*)*ppvObject;
				IActiveScript* pActScr;
				if(SUCCEEDED(pUnk->QueryInterface(IID_IActiveScript, (void**)&pActScr))) // force instance data creation for IActiveScript 
					pActScr->Release();
			}


			SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = GetScriptEngineInstanceData(*ppvObject);
			if (pScriptEngineInstanceData != NULL)
			{
				pScriptEngineInstanceData->pwcsLanguageName = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, (wcslen(m_pLanguage)+1) * sizeof(WCHAR));
				wcscpy(pScriptEngineInstanceData->pwcsLanguageName, m_pLanguage);
				pScriptEngineInstanceData->bEncoded = m_bEncoded;
			}
		}
	}
	
	if (FAILED(hRes))
	{
		ODS(("ERROR: CClassFactory::CreateInstance failed with %08X", hRes));
	}

	return hRes;
}

STDMETHODIMP CClassFactory::LockServer(BOOL fLock)
{
	if (m_ppv == NULL)
		return E_NOTIMPL;

	return m_ppv->LockServer(fLock);
}


// -----------------------------------------------------------------------------------------



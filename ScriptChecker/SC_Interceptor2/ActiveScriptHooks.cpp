#define INITGUID
#include <windows.h>
#include "scripting/activscp.h"
#include "debug.h"
#include "../../windows/hook/r3hook/hookbase64.h"
#include "scr_inst.h"
#include "ActiveScriptHooks.h"
#include "ChkScript.h"
#include "siteinfo.h"
#include "scplugins.h"
#include "../klscav/klscav.h"
#include <tchar.h>

#ifndef countof
  #define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

extern void FlushDocumentWrite(SCRIPT_ENGINE_INSTANCE_DATA* pEngineData, bool bFilterTags);

//#define IFACE_METHOD_HOOK_ID 'IMHI'
//typedef struct tag_IFACE_METHOD_HOOK {
//	tHOOKUSERDATA m_Header;
//	char* m_szIFaceName;
//} tIFACE_METHOD_HOOK;
//
//
//void __cdecl CommonHookUserDataDestructor(tHOOKUSERDATA* pHookUserData)
//{
//	HeapFree(GetProcessHeap(), 0, pHookUserData);
//}

void* g_pJSProxyStartAddress = 0;
void* g_pJSProxyEndAddress = 0;

// =======================================================================================================================

STDMETHODIMP Hook_PStoreCreateInstance(
  void** ppProvider, // IPStore** 
  void* pProviderID, // PST_PROVIDERID*
  void* pReserved,
  DWORD dwFlags
  )
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	void* RetAddr = HookGetRetAddr();
	bool bPacked = false;
	bool bExecutable = false;
	TCHAR szModuleName[MAX_PATH*2] = {0};
	SPD_PStoreCreateInstanceData data;
	memset(&data, 0, sizeof(data));
	data.ret_addr = RetAddr;	

	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi, 0, sizeof(mbi));

	if (VirtualQuery(data.ret_addr, &mbi, sizeof(mbi)))
	{
		data.hModule = (HMODULE)mbi.AllocationBase;
		if (GetModuleFileName(data.hModule, szModuleName, countof(szModuleName)-1))
			data.szModuleName = szModuleName;
		szModuleName[countof(szModuleName)-1]=0; // ensure zero-terminate
		data.bExecutable = (data.hModule == GetModuleHandle(0));
		if (data.bExecutable || !data.szModuleName)
		{
			if (ScPluginsIsDispatchEnabled(msg_PStoreCreateInstance))
			{
				DWORD dwFlags = 0;
				ScPluginsDispatch(msg_PStoreCreateInstance, &data, sizeof(data), &dwFlags);
				if (dwFlags & SCP_BLOCK)
				{
					if (!IsBadWritePtr(ppProvider, sizeof(void*)))
						*ppProvider = NULL;
					return E_ACCESSDENIED;
				}
			}
		}
	}

	hRes = ((HRESULT (__stdcall *)(void**, void*, void*, DWORD))OrigFuncAddr)(ppProvider, pProviderID, pReserved, dwFlags);
	return hRes;
}

/*
STDMETHODIMP InitNewHook(LPVOID _this)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	ODS(("InitNewHook"));
	hRes = ((HRESULT (__stdcall *)(LPVOID))OrigFuncAddr)(_this);
	return hRes;
}

// =======================================================================================================================

STDMETHODIMP AddScriptletHook(LPVOID _this, 
								LPCOLESTR pstrDefaultName,   // address of default name of scriptlet
								LPCOLESTR pstrCode,          // address of scriptlet text
								LPCOLESTR pstrItemName,      // address of item name
								LPCOLESTR pstrSubItemName,   // address of subitem name
								LPCOLESTR pstrEventName,     // address of event name
								LPCOLESTR pstrDelimiter,     // address of end-of-scriptlet delimiter
								DWORD dwSourceContextCookie, // application-defined value for debugging
								ULONG ulStartingLineNumber,  // starting line of the script
								DWORD dwFlags,               // scriptlet flags
								BSTR *pbstrName,             // address of actual name of scriptlet
								EXCEPINFO *pexcepinfo        // address of exception information
								)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	ODS(("AddScriptlet: %S", pstrCode));
	hRes = ((HRESULT (__stdcall *)(LPVOID, LPCOLESTR, LPCOLESTR, LPCOLESTR, LPCOLESTR, LPCOLESTR, LPCOLESTR, DWORD, ULONG, DWORD, BSTR*, EXCEPINFO*))OrigFuncAddr)
		(_this, 
		pstrDefaultName,   // address of default name of scriptlet
		pstrCode,          // address of scriptlet text
		pstrItemName,      // address of item name
		pstrSubItemName,   // address of subitem name
		pstrEventName,     // address of event name
		pstrDelimiter,     // address of end-of-scriptlet delimiter
		dwSourceContextCookie, // application-defined value for debugging
		ulStartingLineNumber,  // starting line of the script
		dwFlags,               // scriptlet flags
		pbstrName,             // address of actual name of scriptlet
		pexcepinfo        // address of exception information
		
		);
	return hRes;
}

// =======================================================================================================================

STDMETHODIMP AddNamedItemHook(LPVOID _this,
    LPCOLESTR pstrName,  // address of item name
    DWORD dwFlags        // item flags
	)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	ODS(("AddNamedItem: \npstrName='%S'\ndwFlags=%08X", pstrName, dwFlags));
	hRes = ((HRESULT (__stdcall *)(LPVOID, LPCOLESTR, DWORD))OrigFuncAddr)(
		_this,
		pstrName,  // address of item name
		dwFlags        // item flags
		);
	return hRes;
}

*/

// =======================================================================================================================
//
//#ifdef _DEBUG
//
//STDMETHODIMP_(ULONG) Generic_AddRefHook(LPVOID _this)
//{
//	ULONG nCount;
//	void* OrigFuncAddr = HookGetOrigFunc();
//	tIFACE_METHOD_HOOK* pIFaceMethodHook = (tIFACE_METHOD_HOOK*)pHookUserData;
//	nCount = ((HRESULT (__stdcall *)(LPVOID))OrigFuncAddr)(_this);
//	ODS(("%08X->%s::AddRef [%d->%d]", _this, pIFaceMethodHook->m_szIFaceName, nCount-1, nCount));
//	return nCount;
//}
//
//# endif

// =======================================================================================================================
//
//#ifdef _DEBUG
//
//STDMETHODIMP_(ULONG) Generic_ReleaseHook(LPVOID _this)
//{
//	ULONG nCount;
//	void* OrigFuncAddr = HookGetOrigFunc();
//	tIFACE_METHOD_HOOK* pIFaceMethodHook = (tIFACE_METHOD_HOOK*)pHookUserData;
//	nCount = ((HRESULT (__stdcall *)(LPVOID))OrigFuncAddr)(_this);
//	ODS(("%08X->%s::Release [%d->%d]", _this, pIFaceMethodHook->m_szIFaceName, nCount+1, nCount));
//	return nCount;
//}
//
//#endif

// =======================================================================================================================

char *szScriptState[] = {"SCRIPTSTATE_UNINITIALIZED", "SCRIPTSTATE_STARTED", "SCRIPTSTATE_CONNECTED",
						 "SCRIPTSTATE_DISCONNECTED", "SCRIPTSTATE_CLOSED", "SCRIPTSTATE_INITIALIZED"};

STDMETHODIMP ActiveScript_SetScriptStateHook(LPVOID _this, SCRIPTSTATE ss)  // identifier of new state
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	
//	ODS(("%08X->ActiveScript::SetScriptState: %s", _this, szScriptState[ss]));

	if (ss == SCRIPTSTATE_INITIALIZED) // Drop allocated memory
		ScriptEngineReinit(_this);

	hRes = ((HRESULT (__stdcall *)(LPVOID, SCRIPTSTATE))OrigFuncAddr)(_this, ss);
	return hRes;
}


// =======================================================================================================================

STDMETHODIMP ActiveScript_SetScriptSiteHook(void* _this, 
    IActiveScriptSite *pScriptSite // address of host site interface
)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	bool bHookQueryInterface = true;
	
//	ODS(("%08X->ActiveScript::SetScriptSite: %08X", _this, pScriptSite));

	// check JSProxy address
	if (!g_pJSProxyStartAddress) 
	{
		HANDLE hJSProxy = GetModuleHandle("jsproxy.dll");
		if (hJSProxy)
		{
			g_pJSProxyStartAddress = hJSProxy;
			// JSProxy is about 15Kb of size. To avoid calculating of real image size
			// we just round up its size to 64Kb - to minimum allocation granularity
			g_pJSProxyEndAddress = (char*)g_pJSProxyStartAddress + 0x10000; 
		}
	}

	
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = GetScriptEngineInstanceData(_this);
	if (pScriptEngineInstanceData)
		pScriptEngineInstanceData->pActiveScriptSite = pScriptSite;

	HookNewObjectMethods(_this, _this, IID_IActiveScript);
	if (pScriptSite)
	{
		TCHAR szModuleName[MAX_PATH*2] = {0};
		MEMORY_BASIC_INFORMATION mbi;
		memset(&mbi, 0, sizeof(mbi));
		void* pQueryIface = NULL;
		void** vtbl = *(void***)pScriptSite;
		if (vtbl)
			pQueryIface = *vtbl;
		if (pQueryIface)
		{
			if (VirtualQuery(pQueryIface, &mbi, sizeof(mbi)))
			{
				if (GetModuleFileName((HMODULE)mbi.AllocationBase, szModuleName, countof(szModuleName)-1))
				{
					TCHAR* pname = _tcsrchr(szModuleName, '\\');
					if (!pname)
						pname = szModuleName;
					else
						pname++;
					if (0 == _tcsicmp(pname, "mscorwks.dll")) // .NET runtime
						bHookQueryInterface = false;
				}
			}
		}
		HookNewObjectMethods(_this, pScriptSite, IID_IActiveScriptSite, bHookQueryInterface);
	}

	hRes = ((HRESULT (__stdcall *)(LPVOID, IActiveScriptSite*))OrigFuncAddr)(_this, pScriptSite);
	return hRes;
}

STDMETHODIMP ClassFactory_CreateInstance(void* _this, LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObject)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
#if defined(_DEBUG) || defined(ENABLE_ODS)
	char szDbgBuff[0x100];
#endif

	hRes = ((HRESULT (__stdcall *)(LPVOID, LPUNKNOWN, REFIID, LPVOID*))OrigFuncAddr)(_this, pUnkOuter, riid, ppvObject);
	ODS(("%08X->ClassFactory::CreateInstance( %s ) = %08X", _this, DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid), (ppvObject ? *ppvObject : NULL)));
	if (SUCCEEDED(hRes) && ppvObject && *ppvObject)
	{
		ODS(("ClassFactory::CreateInstance->HookNewObjectMethods"));
		HookNewObjectMethods(_this, *ppvObject, riid);
	}

	return hRes;
}

// =======================================================================================================================

STDMETHODIMP ActiveScript_Clone(void* _this, 
    IActiveScript **ppscript  // receives pointer to IActiveScript
)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData;
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceDataThis;
	
	hRes = ((HRESULT (__stdcall *)(LPVOID, IActiveScript**))OrigFuncAddr)(_this, ppscript);

	if (hRes != S_OK || ppscript==NULL || *ppscript==NULL)
	{
		ODS(("%08X->ActiveScript::Clone failed", _this));
		return hRes;
	}
	ODS(("%08X->ActiveScript::Clone(OUT pActiveScript=%08X)", _this, *ppscript));

	pScriptEngineInstanceData = GetScriptEngineInstanceData(*ppscript);
	pScriptEngineInstanceDataThis = GetScriptEngineInstanceData(_this);

	if (pScriptEngineInstanceData == NULL)
	{
		pScriptEngineInstanceData = AddScriptEngineInstanceData();
		if (pScriptEngineInstanceData != NULL)
		{
			pScriptEngineInstanceData->pActiveScript = *ppscript;
			if (pScriptEngineInstanceDataThis != NULL && pScriptEngineInstanceDataThis->pwcsLanguageName)
			{
				pScriptEngineInstanceData->pwcsLanguageName = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, (wcslen(pScriptEngineInstanceDataThis->pwcsLanguageName)+1) * sizeof(WCHAR));
				wcscpy(pScriptEngineInstanceData->pwcsLanguageName, pScriptEngineInstanceDataThis->pwcsLanguageName);
			}
		}
	}

	HookNewObjectMethods(_this, *ppscript, IID_IActiveScript);

	return hRes;
}

// =======================================================================================================================

BOOL CheckDenyScript(LPVOID _this, 
					  LPCOLESTR pstrCode,          // address of scriptlet text
					  LPCOLESTR pstrItemName,      // address of item name
					  IUnknown *punkContext,       // address of debugging context
					  LPCOLESTR pstrDelimiter,     // address of end-of-scriptlet delimiter
					  DWORDLONG dwSourceContextCookie, // application-defined value for debugging
					  ULONG ulStartingLineNumber,  // starting line of the script
					  DWORD dwFlags,               // scriptlet flags
					  VARIANT *pvarResult,         // address of buffer for results
					  EXCEPINFO *pexcepinfo,       // address of buffer for error data
					  void* ret_addr
					  )
{
	//DWORD tid = GetCurrentThreadId();
	ODS(( "%08X->ActiveScriptParse::ParseScriptTextHook()", _this));
#ifdef	_DEBUG
	OutputDebugStringW(pstrCode ? pstrCode : L"<NULL>");
	OutputDebugString("\n");
	char szParseScriptTextFlags[0x100] = "";
	
	if (dwFlags & SCRIPTTEXT_DELAYEXECUTION)
		strcat(szParseScriptTextFlags, "SCRIPTTEXT_DELAYEXECUTION ");
	if (dwFlags & SCRIPTTEXT_ISEXPRESSION)
		strcat(szParseScriptTextFlags, "SCRIPTTEXT_ISEXPRESSION ");
	if (dwFlags & SCRIPTTEXT_ISPERSISTENT)
		strcat(szParseScriptTextFlags, "SCRIPTTEXT_ISPERSISTENT ");
	if (dwFlags & SCRIPTTEXT_ISVISIBLE)
		strcat(szParseScriptTextFlags, "SCRIPTTEXT_ISVISIBLE ");
	if (dwFlags & SCRIPTTEXT_HOSTMANAGESSOURCE)
		strcat(szParseScriptTextFlags, "SCRIPTTEXT_HOSTMANAGESSOURCE ");
	ODS(("ParseScriptText flags: [%08X] %s", dwFlags, szParseScriptTextFlags));
#endif

	// quick fix:
	// Anti-banner blocks any url in black-list returning empty GIF-image/Flash instead. In case 
	// requested URL contain script, GIF will be received here and caused interpreter error
	if (pstrCode && 0 == wcsncmp(pstrCode, L"GIF", 3))
	{
		bool unprintable = false;
		size_t len = wcslen(pstrCode);
		if (len < 100)
		{
			for (size_t i=0; i<len; i++)
			{
				if (pstrCode[i] < ' ') // unprintable
				{
					unprintable = true;
					break;
				}
			}
		}
		ODS(("CheckScriptText Received GIF??? unprintable=%d len=%d\n", unprintable, len));
		if (unprintable || len < 10)
		{
			ODS(("CheckScriptText Received GIF image instead script - anti-banner was here?\n"));
			return TRUE;
		}
	}
		
	if (ret_addr > g_pJSProxyStartAddress && ret_addr < g_pJSProxyEndAddress) // this is JSProxy call
	{
		ODS(("CheckScriptText - call from JSProxy\n"));
	}
	else
	{
		if (FAILED(CheckScriptText(_this, (WCHAR*)pstrCode)))
		{
			ODS(("ParseScriptText script blocked"));
			return TRUE;
		}
	}
	ODS(("ParseScriptText passed."));
	return FALSE;
}


STDMETHODIMP ActiveScriptParse32_ParseScriptTextHook(LPVOID _this, 
									  LPCOLESTR pstrCode,          // address of scriptlet text
									  LPCOLESTR pstrItemName,      // address of item name
									  IUnknown *punkContext,       // address of debugging context
									  LPCOLESTR pstrDelimiter,     // address of end-of-scriptlet delimiter
									  DWORD dwSourceContextCookie, // application-defined value for debugging
									  ULONG ulStartingLineNumber,  // starting line of the script
									  DWORD dwFlags,               // scriptlet flags
									  VARIANT *pvarResult,         // address of buffer for results
									  EXCEPINFO *pexcepinfo        // address of buffer for error data
									  )
{

	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	void* ret_addr = HookGetRetAddr();
	if (CheckDenyScript	(_this, 
		pstrCode,          // address of scriptlet text
		pstrItemName,      // address of item name
		punkContext,       // address of debugging context
		pstrDelimiter,     // address of end-of-scriptlet delimiter
		dwSourceContextCookie, // application-defined value for debugging
		ulStartingLineNumber,  // starting line of the script
		dwFlags,               // scriptlet flags
		pvarResult,         // address of buffer for results
		pexcepinfo,        // address of buffer for error data
		ret_addr
		))
	{
		return S_OK;
	}
	hRes = ((HRESULT (__stdcall *)(LPVOID, LPCOLESTR, LPCOLESTR, IUnknown*, LPCOLESTR, DWORD, ULONG, DWORD, VARIANT*, EXCEPINFO*))OrigFuncAddr)
		(_this, 
		pstrCode,          // address of scriptlet text
		pstrItemName,      // address of item name
		punkContext,       // address of debugging context
		pstrDelimiter,     // address of end-of-scriptlet delimiter
		dwSourceContextCookie, // application-defined value for debugging
		ulStartingLineNumber,  // starting line of the script
		dwFlags,               // scriptlet flags
		pvarResult,         // address of buffer for results
		pexcepinfo        // address of buffer for error data
		);	
	return hRes;
}

STDMETHODIMP ActiveScriptParse64_ParseScriptTextHook(LPVOID _this, 
									  LPCOLESTR pstrCode,          // address of scriptlet text
									  LPCOLESTR pstrItemName,      // address of item name
									  IUnknown *punkContext,       // address of debugging context
									  LPCOLESTR pstrDelimiter,     // address of end-of-scriptlet delimiter
									  DWORDLONG dwSourceContextCookie, // application-defined value for debugging
									  ULONG ulStartingLineNumber,  // starting line of the script
									  DWORD dwFlags,               // scriptlet flags
									  VARIANT *pvarResult,         // address of buffer for results
									  EXCEPINFO *pexcepinfo        // address of buffer for error data
									  )
{

	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	void* ret_addr = HookGetRetAddr();
	if (CheckDenyScript	(_this, 
		pstrCode,          // address of scriptlet text
		pstrItemName,      // address of item name
		punkContext,       // address of debugging context
		pstrDelimiter,     // address of end-of-scriptlet delimiter
		dwSourceContextCookie, // application-defined value for debugging
		ulStartingLineNumber,  // starting line of the script
		dwFlags,               // scriptlet flags
		pvarResult,         // address of buffer for results
		pexcepinfo,        // address of buffer for error data
		ret_addr
		))
	{
		return S_OK;
	}
	hRes = ((HRESULT (__stdcall *)(LPVOID, LPCOLESTR, LPCOLESTR, IUnknown*, LPCOLESTR, DWORDLONG, ULONG, DWORD, VARIANT*, EXCEPINFO*))OrigFuncAddr)
		(_this, 
		pstrCode,          // address of scriptlet text
		pstrItemName,      // address of item name
		punkContext,       // address of debugging context
		pstrDelimiter,     // address of end-of-scriptlet delimiter
		dwSourceContextCookie, // application-defined value for debugging
		ulStartingLineNumber,  // starting line of the script
		dwFlags,               // scriptlet flags
		pvarResult,         // address of buffer for results
		pexcepinfo        // address of buffer for error data
		);	
	return hRes;
}
// =======================================================================================================================

STDMETHODIMP_(ULONG) DeleteScriptEngineInstanceDataOnReleaseHook(LPVOID _this)
{
	ULONG nCount;
	void* OrigFuncAddr = HookGetOrigFunc();
	nCount = ((HRESULT (__stdcall *)(LPVOID))OrigFuncAddr)(_this);
//	ODS(("%08X->DeleteScriptEngineInstanceDataOnRelease [%d->%d]", _this, nCount+1, nCount));
	if (nCount == 0)
	{
		SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = GetScriptEngineInstanceData((PVOID)_this);
		if (pScriptEngineInstanceData == NULL)
		{
			ODS(("ERROR: _this=%08X is unregistered???"));
		}
		else
		{
			if (!DeleteScriptEngineInstanceData(pScriptEngineInstanceData))
				ODS(("ERROR: DeleteScriptEngineInstanceDataOnRelease %08X FAILED", _this));
			else
				ODS(("DeleteScriptEngineInstanceDataOnRelease %08X successed", _this));
		}
	}
	return nCount;
}

// =======================================================================================================================

STDMETHODIMP ServiceProvider_QueryServiceHook(LPVOID _this,
	REFGUID guidService, REFIID riid, void **ppvObject)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	__try
	{
#ifdef _DEBUG
//		char szDbgBuff[0x100];
//		char szDbgBuff2[0x100];
		//tIFACE_METHOD_HOOK* pHookInfo = (tIFACE_METHOD_HOOK*)pHookUserData;
		//ODS(("%08X->%s::QueryService( %s, %s )", _this, /*(pHookInfo->m_szIFaceName?pHookInfo->m_szIFaceName:"IUnknown")*/ "IUnknown", DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &guidService), DbgStringFromGUID(szDbgBuff2, sizeof(szDbgBuff2), &riid)));
#endif
		
		hRes = ((HRESULT (__stdcall *)(LPVOID, REFGUID, REFIID , void**))OrigFuncAddr)(_this, guidService, riid, ppvObject);

#ifdef _DEBUG
//		ODS(("%08X->%s::~QueryService( %s, %s, pvObject=%08X ) = %08X", _this, (pHookInfo->m_szIFaceName?pHookInfo->m_szIFaceName:"IUnknown"), DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &guidService), DbgStringFromGUID(szDbgBuff2, sizeof(szDbgBuff2), &riid), (ppvObject?*ppvObject:NULL), hRes));
#endif

		if (hRes != S_OK || ppvObject==NULL || *ppvObject==NULL || *(void**)*ppvObject==NULL)
		{
			ODS(("QueryService failed", hRes));
			return hRes;
		}

		// Hook appropriate methods for any known interface 
		//if (0 == memcmp(&riid, &IID_IActiveScript, sizeof(GUID)))
		HookNewObjectMethods(_this, *ppvObject, riid);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hRes = E_UNEXPECTED;
	}

	return hRes;
}

// =======================================================================================================================

STDMETHODIMP IUnknown_QueryInterfaceHook(LPVOID _this, 
	REFIID riid, void ** ppvObject)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	
#ifdef _DEBUG
	char szDbgBuff[0x100];
//	tIFACE_METHOD_HOOK* pHookInfo = (tIFACE_METHOD_HOOK*)pHookUserData;
	ODS(("%08X->%s::IUnknown_QueryInterfaceHook( %s )", _this, (/*pHookInfo->m_szIFaceName?pHookInfo->m_szIFaceName:*/"IUnknown"), DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid)));
#endif
	
	hRes = ((HRESULT (__stdcall *)(LPVOID, REFIID , void**))OrigFuncAddr)(_this, riid, ppvObject);

#ifdef _DEBUG
//	ODS(("%08X->%s::~IUnknown_QueryInterfaceHook( %s, pvObject=%08X ) = %08X", _this, (pHookInfo->m_szIFaceName?pHookInfo->m_szIFaceName:"IUnknown"), DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid), (ppvObject?*ppvObject:NULL), hRes));
#endif

	if (hRes != S_OK || ppvObject==NULL || *ppvObject==NULL || *(void**)*ppvObject==NULL)
	{
		ODS(("QueryInterface failed", hRes));
		return hRes;
	}

		
	// Hook appropriate methods for any known interface 
	//ODS(("IUnknown_QueryInterfaceHook->HookNewObjectMethods(%08X, %08X)", _this, *ppvObject));
	HookNewObjectMethods(_this, *ppvObject, riid);
	//ODS(("~IUnknown_QueryInterfaceHook->HookNewObjectMethods"));

/*
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = GetScriptEngineInstanceData((PVOID)_this);
	if (!pScriptEngineInstanceData)
		return hRes;

	// IID_IActiveScriptSiteDebug32
	if (0 == memcmp(&IID_IActiveScriptSiteDebug32, &riid, sizeof(GUID)))
	{
		if (TRUE == pScriptEngineInstanceData->bDenyExecution)
		{
			((IUnknown*)(*ppvObject))->Release();
			*ppvObject = NULL;
			ODS(("WARNING: IUnknown_QueryInterfaceHook: bDenyExecution -> suppress  interface quering"));
			return E_NOINTERFACE;
		}
	}
*/
	return hRes;
}

// =======================================================================================================================

STDMETHODIMP ActiveScriptSite_OnStateChangeHook(LPVOID _this,
    SCRIPTSTATE ssScriptState  // new state of engine
)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
#ifdef _DEBUG
	char *szScriptState[] = {"SCRIPTSTATE_UNINITIALIZED", "SCRIPTSTATE_STARTED", "SCRIPTSTATE_CONNECTED",
						 "SCRIPTSTATE_DISCONNECTED", "SCRIPTSTATE_CLOSED", "SCRIPTSTATE_INITIALIZED"};
//	ODS(("%08X->ActiveScriptSite::OnStateChange: %s", _this, szScriptState[ssScriptState]));
#endif
    
	if (ssScriptState == SCRIPTSTATE_INITIALIZED) // Drop allocated memory
		ScriptEngineReinit(_this);

	hRes = ((HRESULT (__stdcall *)(LPVOID, SCRIPTSTATE))OrigFuncAddr)(_this, ssScriptState);

	return hRes;
}

// =======================================================================================================================

STDMETHODIMP ActiveScriptSite_OnScriptErrorHook(LPVOID _this,
    IActiveScriptError *pActiveScriptError  // address of error interface
)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();

	ODS(("%08X->ActiveScriptSite::OnScriptError(IActiveScriptError=%08X)", _this, (pActiveScriptError?*(LPVOID*)pActiveScriptError:NULL)));
	
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = GetScriptEngineInstanceData(_this);
	if (pScriptEngineInstanceData == NULL)
	{
		ODS(("ERROR: ActiveScriptSite_OnScriptErrorHook:  instance not registered? (_this=%08X)", _this));
	}
	else
	{
		if (pScriptEngineInstanceData->bSupressErrorCode)
		{
			//BSTR line = NULL;
			//DWORD dwContext;
			//ULONG ulLineNumber;
			//LONG nCharPos;
			EXCEPINFO einfo;
			//DWORD tid = GetCurrentThreadId();
			//pActiveScriptError->GetSourcePosition(&dwContext, &ulLineNumber, &nCharPos);
			//pActiveScriptError->GetSourceLineText(&line);
			//if (line) SysFreeString(line);
			if (SUCCEEDED(pActiveScriptError->GetExceptionInfo(&einfo)))
			{
				if (einfo.bstrDescription) SysFreeString(einfo.bstrDescription);
				if (einfo.bstrHelpFile) SysFreeString(einfo.bstrHelpFile);
				if (einfo.bstrSource) SysFreeString(einfo.bstrSource);
			}
			if (pScriptEngineInstanceData->bSupressErrorCode == einfo.scode)
			{
				ODS(("ActiveScriptSite_OnScriptErrorHook: bSupressErrors=TRUE -> suppress error message"));
				return S_OK;
			}
		}
		if (pScriptEngineInstanceData->bDenyExecution)
		{
			ODS(("ActiveScriptSite_OnScriptErrorHook: bDenyExecution=TRUE -> suppress error message"));
			return S_OK;
		}
	}

	hRes = ((HRESULT (__stdcall *)(LPVOID, IActiveScriptError *))OrigFuncAddr)(_this, pActiveScriptError);

	return hRes;
}

// =======================================================================================================================

STDMETHODIMP ActiveScriptSite_OnEnterScriptHook(LPVOID _this)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();

	SCRIPT_ENGINE_INSTANCE_DATA* pEngineData = GetScriptEngineInstanceData(_this);
	SC_THREAD_DATA* pThreadData = GetSCThreadData();

	if (!pEngineData || !pThreadData)
	{
		ODS(("%08X->ActiveScriptSite::OnEnterScript [?]", _this));
	}
	else
	{
		ODS(("%08X->ActiveScriptSite::OnEnterScript [%d->%d] [Th:%d->%d]", _this, pEngineData->dwEntryCount, pEngineData->dwEntryCount+1, pThreadData->dwEntryCount, pThreadData->dwEntryCount+1));
		if (0 == pEngineData->dwEntryCount)
		{
			pEngineData->bPassDocumentWrite = 0;
			if (0 == pThreadData->dwEntryCount)
			{
				pThreadData->dwCurEngineIndex = 0;
				pThreadData->pCurrentScriptEngineInstance = _this;
				pThreadData->arrCurEngines[0] = _this;
				pThreadData->bstrEventName = GetEventName(pEngineData->pActiveScriptSite);
			}
			else
			{
				if (pThreadData->dwCurEngineIndex < MAX_CUR_ENGINES)
				{
					SCRIPT_ENGINE_INSTANCE_DATA* pEngineData = GetScriptEngineInstanceData(pThreadData->arrCurEngines[pThreadData->dwCurEngineIndex]);
					if (pEngineData && pEngineData->pDelayedWriteDoc)
						pEngineData->pDelayedWriteDoc->write(NULL);
				}
				pThreadData->dwCurEngineIndex++;
				if (pThreadData->dwCurEngineIndex < MAX_CUR_ENGINES)
					pThreadData->arrCurEngines[pThreadData->dwCurEngineIndex] = _this;
			}
		}
		pEngineData->dwEntryCount++;
		pThreadData->dwEntryCount++;
	}

	hRes = ((HRESULT (__stdcall *)(LPVOID))OrigFuncAddr)(_this);
	return hRes;
}

// =======================================================================================================================

STDMETHODIMP ActiveScriptSite_OnLeaveScriptHook(LPVOID _this)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();

	SCRIPT_ENGINE_INSTANCE_DATA* pEngineData = GetScriptEngineInstanceData(_this);
	SC_THREAD_DATA* pThreadData = GetSCThreadData();

	if (!pEngineData || !pThreadData)
	{
		ODS(("%08X->ActiveScriptSite::OnLeaveScript [?]", _this));
	}
	else
	{
		ODS(("%08X->ActiveScriptSite::OnLeaveScript [%d->%d] [Th:%d->%d]", _this, pEngineData->dwEntryCount, pEngineData->dwEntryCount-1, pThreadData->dwEntryCount, pThreadData->dwEntryCount-1));
		if (0 == pEngineData->dwEntryCount)
		{
			DBG_STOP;
		}
		else
		{
			if (1 == pEngineData->dwEntryCount)
			{
				if (pEngineData->pDelayedWriteDoc)
				{
					UINT nTagsCount = 0;
					bool bDelay = false;
					WCHAR* pCheck = pEngineData->wcsScriptText;
					while (pCheck)
					{
						pCheck = wcschr(pCheck, '<');
						if (!pCheck)
							break;
						pCheck++;
						nTagsCount++;
					} while(pCheck);
					pCheck = pEngineData->wcsScriptText;
					ODS(("*** Flush Delayed DocWrite: TagsCount=%d ***", nTagsCount));
					
					FlushDocumentWrite(pEngineData, (nTagsCount < 30));
				}

				if (pThreadData->dwCurEngineIndex < MAX_CUR_ENGINES )
					pThreadData->arrCurEngines[pThreadData->dwCurEngineIndex] = NULL;
				if (pThreadData->dwCurEngineIndex)
					pThreadData->dwCurEngineIndex--;
			}

			if (1 == pThreadData->dwEntryCount) 
			{
				if (pThreadData->bstrEventName)
					SysFreeString(pThreadData->bstrEventName);
				pThreadData->bNavigate2Script = FALSE;
			}

			pThreadData->dwEntryCount--;
			pEngineData->dwEntryCount--;

		}
	}

	hRes = ((HRESULT (__stdcall *)(LPVOID))OrigFuncAddr)(_this);
	return hRes;
}

// =======================================================================================================================
STDMETHODIMP ActiveScriptSiteDebug_OnScriptErrorDebugHook(LPVOID _this,
	IActiveScriptErrorDebug *pErrorDebug, BOOL *pfEnterDebugger, BOOL *pfCallOnScriptErrorWhenContinuing)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	ODS(("%08X->CActiveScriptSiteDebug::OnScriptErrorDebug()", _this));
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = GetScriptEngineInstanceData(_this);
	if (pScriptEngineInstanceData == NULL)
	{
		ODS(("ERROR: GetScriptEngineInstanceData in ActiveScriptSiteDebug::OnScriptErrorDebug - instance not registered? (_this=%08X)", _this));
	}
	else
	{
		if (pScriptEngineInstanceData->bDenyExecution)
		{
			ODS(("WARNING: ActiveScriptSiteDebug::OnScriptErrorDebug bDenyExecution -> suppress error message"));
			*pfEnterDebugger = FALSE;
			*pfCallOnScriptErrorWhenContinuing = FALSE;
			return S_OK;
		}
	}

	hRes = ((HRESULT (__stdcall *)(LPVOID, IActiveScriptErrorDebug*, BOOL*, BOOL*))OrigFuncAddr)(_this, pErrorDebug, pfEnterDebugger, pfCallOnScriptErrorWhenContinuing);

	return hRes;
}

STDMETHODIMP WebBrowser2_Navigate(
            IWebBrowser2 * This,
            /* [in] */ BSTR URL,
            /* [optional][in] */ VARIANT *Flags,
            /* [optional][in] */ VARIANT *TargetFrameName,
            /* [optional][in] */ VARIANT *PostData,
            /* [optional][in] */ VARIANT *Headers)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	VARIANT_BOOL bVisible;
	if (FAILED(This->get_Visible(&bVisible)))
		bVisible = TRUE;
	if (!bVisible)
	{

		ODS(("%08X->WebBrowser2::OnNavigate(%x '%S', 0x%x, %x '%S', %x '%S', %x '%S') Visible = %s", This, 
			URL ? *URL : 0, URL ? URL : L"NULL",
			Flags ? Flags->lVal : 0, 
			TargetFrameName ? TargetFrameName->vt : 0, (TargetFrameName && TargetFrameName->vt == VT_BSTR) ? TargetFrameName->bstrVal : L"NULL",
			PostData ? PostData->vt : 0, (PostData && PostData->vt == VT_BSTR) ? PostData->bstrVal : L"NULL",
			Headers ? Headers->vt : 0, (Headers && Headers->vt == VT_BSTR) ? Headers->bstrVal : L"NULL",
			bVisible ? "TRUE" : "FALSE"));

		if (URL && (wcschr(URL, '?') || (PostData && PostData->vt != VT_EMPTY)))
		{
			if (ScPluginsIsDispatchEnabled(SPD_IENavigateHidden))
			{
				DWORD dwFlags = 0;
				SPD_IENavigateHiddenData data;
				memset(&data, 0, sizeof(data));
				data.pUrl = URL;
				data.pHeader = Headers;
				data.pPost = PostData;
				ScPluginsDispatch(SPD_IENavigateHidden, &data, sizeof(data), &dwFlags);
				if (dwFlags & SCP_BLOCK)
				{
					return E_ACCESSDENIED;
				}
			}
		}
	}

	hRes = ((HRESULT (__stdcall *)(IWebBrowser2*, BSTR, VARIANT*, VARIANT*, VARIANT*, VARIANT*))OrigFuncAddr)(This, URL, Flags, TargetFrameName, PostData, Headers);

	return hRes;
}


// =======================================================================================================================

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

LPVOID GetRealIfaceFromTearOffThunk(LPVOID pObject)
{
	/* MSHTML.DLL Win98
	8B 44 24 04   mov     eax, [esp+arg_0]
	8B 48 0C      mov     ecx, [eax+0Ch]
	89 4C 24 04   mov     [esp+arg_0], ecx
	8B 48 10      mov     ecx, [eax+10h]
	8B 49 40      mov     ecx, [ecx+40h]
	FF E1         jmp     ecx
	*/

	/* MSHTML.DLL Win2000, WinXP
	8B 44 24 04                    mov     eax, [esp+arg_0]
	C7 40 20 10 00+                mov     dword ptr [eax+20h], 10h // <- method number
	8B 48 0C                       mov     ecx, [eax+0Ch]
	89 4C 24 04                    mov     [esp+arg_0], ecx
	8B 48 10                       mov     ecx, [eax+10h]
	8B 49 40                       mov     ecx, [ecx+40h]
	FF E1                          jmp     ecx
	*/


	__try
	{
		static DWORD s_dwVer = 0;
		if (0 == s_dwVer)
			s_dwVer = GetVersion();  
		MEMORY_BASIC_INFORMATION mbi;
		LPBYTE* vtbl = *(LPBYTE**)pObject;
		if (!VirtualQuery(vtbl, &mbi, sizeof(mbi)))
			return NULL;
		if (mbi.State != MEM_COMMIT) // invalid pointer
			return NULL;
		if (MyIsBadReadPtr(vtbl, 0x11*sizeof(void*))) // double check
			return NULL;
		if (vtbl)
		{
			const BYTE thunk_data1_x64[] = {
				0x4C, 0x8B, 0xD9, 0xC7, 0x41, 0x3C, 0x11, 0x00, 
				0x00, 0x00, 0x48, 0x8B, 0x41, 0x20, 0x48, 0x8B, 
				0x49, 0x18, 0xFF, 0xA0, 0x88, 0x00, 0x00, 0x00
			};
			const BYTE thunk_data1[] = {0x8B, 0x44, 0x24, 0x04};
			const BYTE thunk_data2[] = {0xC7, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00};
			const BYTE thunk_data3[] = {0x8B, 0x48, 0x0C, 0x89, 0x4C, 0x24, 0x04, 0x8B, 0x48, 0x10, 0x8B, 0x49, 0x40, 0xFF, 0xE1};
			LPBYTE tearoffthunk16 = vtbl[16];
			if (!VirtualQuery(tearoffthunk16, &mbi, sizeof(mbi)))
				return NULL;
			if (mbi.State != MEM_COMMIT) // invalid pointer
				return NULL;
			if (!(s_dwVer & 0x80000000)) //	WinNT+
			{
				if (mbi.Type != MEM_IMAGE)
					return NULL;
			}
			if (MyIsBadReadPtr(tearoffthunk16, 0x10))
				return NULL;
			//ODS(("GetRealIfaceFromTearOffThunk: addr %08X->%08X[16]->%08X", pObject, vtbl, tearoffthunk16));
			DWORD offset = 0;
			if (0 == memcmp(tearoffthunk16, thunk_data1_x64, sizeof(thunk_data1_x64)))
			{
				ODS(("Found x64 TearOffThunk %16X->%16X[16]->%16X real vtbl=%16X", pObject, vtbl, tearoffthunk16, ((LPBYTE)pObject) + 0x20));
				return ((LPBYTE)pObject) + 0x20;
			}
			if (0 != memcmp(tearoffthunk16, thunk_data1, sizeof(thunk_data1)))
			{
				//ODS(("GetRealIfaceFromTearOffThunk: step 1 FAILED"));
				return NULL;
			}
			ODS(("GetRealIfaceFromTearOffThunk: step 1 ok"));
			offset += sizeof(thunk_data1);
			if (0 == memcmp(tearoffthunk16+offset, thunk_data2, sizeof(thunk_data2)))
			{
				//ODS(("GetRealIfaceFromTearOffThunk: step 2 ok"));
				offset += sizeof(thunk_data2);
			}
			if (0 != memcmp(tearoffthunk16+offset, thunk_data3, sizeof(thunk_data3)))
			{
				//ODS(("GetRealIfaceFromTearOffThunk: step 3 FAILED"));
				return NULL;
			}
			//ODS(("GetRealIfaceFromTearOffThunk: step 3 ok"));
			return ((LPBYTE)pObject) + 0x10;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		//ODS(("!!! GetRealIfaceFromTearOffThunk: exception occured"));
		return NULL;
	}
	return  NULL;
}

BOOL HookIFaceMethod(void* pObject, DWORD nMethod, LPVOID pHookFunc, char* szIFaceName)
{
	//if (IsMethodHookedBy(pObject,nMethod,pHookFunc))
	//	return TRUE;
	//tIFACE_METHOD_HOOK* pHookUserData = (tIFACE_METHOD_HOOK*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tIFACE_METHOD_HOOK));
	//if (!pHookUserData)
	//	return FALSE;
	//pHookUserData->m_Header.destructor = CommonHookUserDataDestructor;
	//pHookUserData->m_Header.ID = IFACE_METHOD_HOOK_ID;
	//pHookUserData->m_szIFaceName = szIFaceName;
	//return HookObjectMethodIfNotHookedBy(pObject, nMethod, pHookFunc, &pHookUserData->m_Header);
	void* pRealIface = GetRealIfaceFromTearOffThunk(pObject);
	if (!pRealIface)
		pRealIface = pObject;
	return HookObjectMethodIfNotHookedBy(pRealIface, nMethod, pHookFunc, NULL);
}


BOOL SetIUnknownHooks(void* pObject, char* szIFaceName)
{
	BOOL bRes = TRUE;
	if (!szIFaceName)
		szIFaceName = "IUnknown";
	bRes = bRes && HookIFaceMethod(pObject, 0, IUnknown_QueryInterfaceHook, szIFaceName);
/*
#ifdef _DEBUG
	bRes = bRes && HookIFaceMethod(pObject, 1, Generic_AddRefHook, szIFaceName);
	bRes = bRes && HookIFaceMethod(pObject, 2, Generic_ReleaseHook, szIFaceName);
#endif
*/
	return bRes;
}

//#ifdef _DEBUG
//void SetIFaceHooks(void* pObject, REFIID riid)
//{
//	char szIFaceName[0x100];
//	char* pMem;
//	DbgStringFromGUID(szIFaceName, sizeof(szIFaceName), &riid);
//	pMem = (char*)HeapAlloc(GetProcessHeap(), 0, strlen(szIFaceName)+1);
//	if (!pMem)
//		return;
//	strcpy(pMem, szIFaceName);
//	if (!SetIUnknownHooks(pObject, pMem))
//		HeapFree(GetProcessHeap(), 0, pMem);
//	return;
//}
//#endif

static const GUID* arrActiveScriptGUIDs[] = { 
	// ActivScp.h
	&IID_IUnknown,
	&IID_IDispatch,
	&IID_IActiveScriptSite,
	&IID_IActiveScriptError,
	&IID_IActiveScriptError64,
	&IID_IActiveScriptSiteWindow,
	&IID_IActiveScriptSiteInterruptPoll,
	&IID_IActiveScript,
	&IID_IActiveScriptParse32,
	&IID_IActiveScriptParse64,
	&IID_IActiveScriptParseProcedureOld32,
	&IID_IActiveScriptParseProcedureOld64,
	&IID_IActiveScriptParseProcedure32,
	&IID_IActiveScriptParseProcedure64,
	&IID_IActiveScriptParseProcedure2_32,
	&IID_IActiveScriptParseProcedure2_64,
	&IID_IActiveScriptEncode,
	&IID_IActiveScriptHostEncode,
	&IID_IBindEventHandler,
	&IID_IActiveScriptStats,
	&IID_IActiveScriptProperty,
	&IID_ITridentEventSink,
	&IID_IActiveScriptGarbageCollector,
	&IID_IActiveScriptSIPInfo,
	// activdbg.h
	//&IID_IActiveScriptDebug,
	&IID_IActiveScriptSiteDebug32,
	&IID_IActiveScriptErrorDebug,
	//&IID_IDebugCodeContext,
	//&IID_IDebugExpression,
	//&IID_IDebugExpressionContext,
	//&IID_IDebugExpressionCallBack,
	//&IID_IDebugStackFrame,
	//&IID_IDebugStackFrameSniffer,
	//&IID_IDebugStackFrameSnifferEx,
	//&IID_IDebugSyncOperation,
	//&IID_IDebugAsyncOperation,
	//&IID_IDebugAsyncOperationCallBack,
	//&IID_IEnumDebugCodeContexts,
	//&IID_IEnumDebugStackFrames,
	//&IID_IDebugDocumentInfo,
	//&IID_IDebugDocumentProvider,
	//&IID_IDebugDocument,
	//&IID_IDebugDocumentText,
	//&IID_IDebugDocumentTextEvents,
	//&IID_IDebugDocumentTextAuthor,
	//&IID_IDebugDocumentTextExternalAuthor,
	//&IID_IDebugDocumentHelper,
	//&IID_IDebugDocumentHost,
	//&IID_IDebugDocumentContext,
	//&IID_IDebugSessionProvider,
	&IID_IApplicationDebugger,
	&IID_IApplicationDebuggerUI,
	//&IID_IMachineDebugManager,
	//&IID_IMachineDebugManagerCookie,
	//&IID_IMachineDebugManagerEvents,
	//&IID_IProcessDebugManager,
	&IID_IRemoteDebugApplication,
	//&IID_IDebugApplication,
	//&IID_IRemoteDebugApplicationEvents,
	//&IID_IDebugApplicationNode,
	//&IID_IDebugApplicationNodeEvents,
	//&IID_IDebugThreadCall,
	//&IID_IRemoteDebugApplicationThread,
	//&IID_IDebugApplicationThread,
	//&IID_IDebugCookie,
	//&IID_IEnumDebugApplicationNodes,
	//&IID_IEnumRemoteDebugApplications,
	//&IID_IEnumRemoteDebugApplicationThreads,
	//&IID_IDebugFormatter,
	//&IID_ISimpleConnectionPoint,
	//&IID_IDebugHelper,
	//&IID_IEnumDebugExpressionContexts,
	//&IID_IProvideExpressionContexts,
};

void HookNewObjectMethods(void* _this, void* pNewObject, REFIID riid, bool bHookQueryInterface /*= true*/)
{
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = NULL;
#ifdef _DEBUG
	//char szDbgBuff[0x100];
	//ODS(("New object %08X %s", pNewObject, DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid)));
#endif
	if (bHookQueryInterface)
	{
		for (int i=0; i<countof(arrActiveScriptGUIDs); i++)
		{
			if (0 == memcmp(arrActiveScriptGUIDs[i], &riid, sizeof(GUID)))
			{
				SetIUnknownHooks(pNewObject, NULL);
				break;
			}
		}
	}

	// IID_IActiveScript hooks
	if (0 == memcmp(&IID_IActiveScript, &riid, sizeof(GUID)))
	{
		// DumpEnginesList();
		pScriptEngineInstanceData = GetScriptEngineInstanceData(pNewObject);
		if (pScriptEngineInstanceData == NULL) // some IActiveScript instances born not thowgh CreateInstance
			pScriptEngineInstanceData = AddScriptEngineInstanceData();
		if (pScriptEngineInstanceData)
		{
			if (pScriptEngineInstanceData->pActiveScript == NULL)
				pScriptEngineInstanceData->pActiveScript = (IActiveScript*)pNewObject;
			else
			if (pScriptEngineInstanceData->pActiveScript != pNewObject)
				DBG_STOP;
		}
		HookIFaceMethod(pNewObject, 2, DeleteScriptEngineInstanceDataOnReleaseHook, "IActiveScript"); // this hook automatically delete ScriptEngineInstanceData on last instance release
		HookIFaceMethod(pNewObject, 3, ActiveScript_SetScriptSiteHook, "IActiveScript");
		HookIFaceMethod(pNewObject, 5, ActiveScript_SetScriptStateHook, "IActiveScript");
		HookIFaceMethod(pNewObject, 15, ActiveScript_Clone, "IActiveScript");
		
		return;
	}
	
	// IID_IActiveScriptParse hooks
	if (0 == memcmp(&IID_IActiveScriptParse32, &riid, sizeof(GUID)))
	{
		pScriptEngineInstanceData = GetScriptEngineInstanceData(_this);
		if (pScriptEngineInstanceData)
			pScriptEngineInstanceData->pActiveScriptParse = (IActiveScriptParse*)pNewObject;
		HookIFaceMethod(pNewObject, 5, ActiveScriptParse32_ParseScriptTextHook, "IActiveScriptParse");
		return;
	}

	// IID_IActiveScriptParse hooks
	if (0 == memcmp(&IID_IActiveScriptParse64, &riid, sizeof(GUID)))
	{
		pScriptEngineInstanceData = GetScriptEngineInstanceData(_this);
		if (pScriptEngineInstanceData)
			pScriptEngineInstanceData->pActiveScriptParse = (IActiveScriptParse*)pNewObject;
		HookIFaceMethod(pNewObject, 5, ActiveScriptParse64_ParseScriptTextHook, "IActiveScriptParse");
		return;
	}


	// IID_IActiveScriptSiteDebug
	if (0 == memcmp(&IID_IActiveScriptSiteDebug32, &riid, sizeof(GUID)))
	{
		pScriptEngineInstanceData = GetScriptEngineInstanceData(_this);
		if (pScriptEngineInstanceData)
			pScriptEngineInstanceData->pActiveScriptSiteDebug = (IActiveScriptSiteDebug*)pNewObject;
		HookIFaceMethod(pNewObject, 6, ActiveScriptSiteDebug_OnScriptErrorDebugHook, "IActiveScriptSiteDebug32");
		return;
	}

	// IID_IActiveScriptSite
	if (0 == memcmp(&IID_IActiveScriptSite, &riid, sizeof(GUID)))
	{
		pScriptEngineInstanceData = GetScriptEngineInstanceData(_this);
		if (pScriptEngineInstanceData)
			pScriptEngineInstanceData->pActiveScriptSite = (IActiveScriptSite*)pNewObject;
		HookIFaceMethod(pNewObject, 7,  ActiveScriptSite_OnStateChangeHook, "IActiveScriptSite");
		HookIFaceMethod(pNewObject, 8,  ActiveScriptSite_OnScriptErrorHook, "IActiveScriptSite");
		HookIFaceMethod(pNewObject, 9,  ActiveScriptSite_OnEnterScriptHook, "IActiveScriptSite");
		HookIFaceMethod(pNewObject, 10, ActiveScriptSite_OnLeaveScriptHook, "IActiveScriptSite");
		return;
	}

	// IID_IWebBrowser2
	if (0 == memcmp(&IID_IWebBrowser2, &riid, sizeof(GUID)))
	{
		HookIFaceMethod(pNewObject, 11,  WebBrowser2_Navigate, "IWebBrowser2");
		return;
	}
	
	// IID_IServiceProvider
//	if (0 == memcmp(&IID_IServiceProvider, &riid, sizeof(GUID)))
//	{
//		HookIFaceMethod(pNewObject, 3,  ServiceProvider_QueryServiceHook, "IServiceProvider");
//		return;
//	}

	// IID_IClassFactory
	if (0 == memcmp(&IID_IClassFactory, &riid, sizeof(GUID)))
	{
		HookIFaceMethod(pNewObject, 3,  ClassFactory_CreateInstance, "IClassFactory");
		return;
	}

	// default
	return;
}


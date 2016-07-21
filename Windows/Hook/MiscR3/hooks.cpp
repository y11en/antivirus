#include <windows.h>
#include <shlobj.h>
#include <bits.h>

#pragma warning(disable:4996)		// deprecated string functions
#pragma warning(disable:4390)		// empty control statement

#include "..\hook\fssync.h"
#include "..\hook\hookspec.h"
#include "..\hook\funcs.h"
#include "..\kldefs.h"
#include "debug.h"
#include "..\R3Hook\hookbase64.h"
#include "common.h"

extern PFSSYNC_CONTEXT g_pFSSyncContext;
extern volatile BOOL g_bHookBaseInitFailed;
extern volatile BOOL g_bHookBaseInited;

BOOL HookIFaceMethod(void* pObject, DWORD nMethod, LPVOID pHookFunc, char* szIFaceName);
void HookNewObjectMethods(void* _this, void* pNewObject, REFIID riid);
BOOL SetIUnknownHooks(void* pObject, char* szIFaceName);
STDMETHODIMP ClassFactory_CreateInstance(void* _this, LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObject);
STDMETHODIMP PSFactoryBuffer_CreateProxy(PVOID _this, IUnknown *pUnkOuter, REFIID riid, IRpcProxyBuffer **ppProxy, void **ppv);
STDMETHODIMP IUnknown_QueryInterfaceHook(LPVOID _this, 
	REFIID riid, void ** ppvObject);


BOOL HookIFaceMethod(void* pObject, DWORD nMethod, LPVOID pHookFunc, char* szIFaceName)
{
	return HookObjectMethodIfNotHookedBy(pObject, nMethod, pHookFunc, NULL);
}

typedef HRESULT (WINAPI *t_fActiveDesktop_SetWallpaper)(PVOID _this, LPCWSTR pwszWallpaper, DWORD dwReserved);

#define CLIENT_REGISTER_TIMEOUT		5000

BOOL WaitClientInited( DWORD Timeout /*ms*/ )
{
	DWORD SleepCount = 0;
	while ( !g_bHookBaseInitFailed && !g_bHookBaseInited && SleepCount < Timeout / 10 )
	{
		Sleep( 10 );
		SleepCount++;
	}

	return g_bHookBaseInited;
}

STDMETHODIMP ActiveDesktop_SetWallpaper(PVOID _this, LPCWSTR pwszWallpaper, DWORD dwReserved)
{
	t_fActiveDesktop_SetWallpaper fOrigFunc = (t_fActiveDesktop_SetWallpaper)HookGetOrigFunc();

	ODS(("IActiveDesktop::SetWallpaper to %S", pwszWallpaper));

	VERDICT Verdict = Verdict_NotFiltered;

	if ( WaitClientInited( CLIENT_REGISTER_TIMEOUT ) )
	{
		PFILTER_EVENT_PARAM pParam = (PFILTER_EVENT_PARAM)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x1000);
		if (pParam)
		{
			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_R3, R3_ACTIVEDESKTOP_SETWALLPAPER, 0,  PreProcessing, 2);

			PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM)pParam->Params;

			SINGLE_PARAM_INIT_SID(pSingleParam);
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
			if (pwszWallpaper)
			{
				__try
				{
					pSingleParam->ParamSize = (DWORD)sizeof(WCHAR)*(wcslen(pwszWallpaper)+1);
					memcpy(pSingleParam->ParamValue, pwszWallpaper, pSingleParam->ParamSize);
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					pSingleParam->ParamSize = sizeof(WCHAR);
					*(PWCHAR)pSingleParam->ParamValue = 0;
				}
			}
			else
			{
				pSingleParam->ParamSize = sizeof(WCHAR);
				*(PWCHAR)pSingleParam->ParamValue = 0;
			}

			HRESULT hResult;
			ULONG VerdictFlags;
			hResult = g_pFSSyncContext->m_DRV_FilterEvent(
				g_pFSSyncContext->m_pClientContext,
				pParam,
				FALSE,
				&VerdictFlags
				);
			if ( IS_ERROR( hResult ) )
				ODS(("DRV_FilterEvent failed"));
			else
			{
				Verdict = g_pFSSyncContext->m_DRV_VerdictFlags2Verdict( VerdictFlags );
			}

			HeapFree(GetProcessHeap(), 0, pParam);
		}
	}

	if (_PASS_VERDICT(Verdict))
	{
		if (fOrigFunc)
			return fOrigFunc(_this, pwszWallpaper, dwReserved);
	}

	return E_FAIL;
}

typedef HRESULT (WINAPI *t_fBackgroundCopyManager_CreateJob)(PVOID _this, LPCWSTR pDisplayName, BG_JOB_TYPE Type, GUID* pJobID, IBackgroundCopyJob** ppJob);

STDMETHODIMP BackgroundCopyManager_CreateJob(PVOID _this, LPCWSTR pDisplayName, BG_JOB_TYPE Type, GUID* pJobID, IBackgroundCopyJob** ppJob)
{
	HRESULT hResult = E_FAIL;

	t_fBackgroundCopyManager_CreateJob fOrigFunc = (t_fBackgroundCopyManager_CreateJob)HookGetOrigFunc();

	if (fOrigFunc)
	{
		hResult = fOrigFunc(_this, pDisplayName, Type, pJobID, ppJob); 
		if (SUCCEEDED(hResult))
		{
#ifdef _DEBUG
			CHAR szDbgBuff[0x100];
#endif
			ODS(("IBackgroundCopyManager::CreateJob '%S type %d' GUID %s", pDisplayName, Type, DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), pJobID)));

			if (ppJob && *ppJob)
				HookNewObjectMethods(_this, *ppJob, IID_IBackgroundCopyJob);
		}
	}

	return hResult;
}

typedef HRESULT (WINAPI *t_fBackgroundCopyJob_Resume)(PVOID _this);

STDMETHODIMP BackgroundCopyJob_Resume(PVOID _this)
{
	t_fBackgroundCopyJob_Resume fOrigFunc = (t_fBackgroundCopyJob_Resume)HookGetOrigFunc();

	ODS(("IBackgroundCopyJob::Resume"));

	VERDICT Verdict = Verdict_NotFiltered;

	HRESULT hResult;
	PWCHAR LocalFileNames = NULL;
	PWCHAR RemoteFileNames = NULL;
	PWCHAR DisplayName = NULL;
	BG_JOB_TYPE Type = BG_JOB_TYPE_UPLOAD;

// get job params here
	if (_this)
	{
		IEnumBackgroundCopyFiles *i_CopyFiles = NULL;

		__try
		{
			IBackgroundCopyJob *pJob = (IBackgroundCopyJob *)_this;
			pJob->GetDisplayName(&DisplayName);
			pJob->GetType(&Type);

			LocalFileNames = (PWCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x4000);
			if (LocalFileNames)
				LocalFileNames[0] = 0;
			RemoteFileNames = (PWCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x4000);
			if (RemoteFileNames)
				RemoteFileNames[0] = 0;


			hResult = pJob->EnumFiles(&i_CopyFiles);
			if (SUCCEEDED(hResult) && i_CopyFiles)
			{
				DWORD FileCount = 0;
				hResult = i_CopyFiles->GetCount(&FileCount);
				if (SUCCEEDED(hResult))
				{
					for (ULONG i = 0; i < FileCount; i++)
					{
						IBackgroundCopyFile *i_CopyFile = NULL;
						hResult = i_CopyFiles->Next(1, &i_CopyFile, NULL);
						if (SUCCEEDED(hResult) && i_CopyFile)
						{
							if (LocalFileNames)
							{
								PWCHAR _LocalName = NULL;
								hResult = i_CopyFile->GetLocalName(&_LocalName);
								if (SUCCEEDED(hResult) && _LocalName)
								{
									wcscat(LocalFileNames, _LocalName);
									wcscat(LocalFileNames, L" ");
									CoTaskMemFree(_LocalName);
								}
							}

							if (RemoteFileNames)
							{
								PWCHAR _RemoteName = NULL;
								hResult = i_CopyFile->GetRemoteName(&_RemoteName);
								if (SUCCEEDED(hResult) && _RemoteName)
								{
									wcscat(RemoteFileNames, _RemoteName);
									wcscat(RemoteFileNames, L" ");
									CoTaskMemFree(_RemoteName);
								}
							}

							i_CopyFile->Release();
						}
					}
				}

				i_CopyFiles->Release();
				i_CopyFiles = NULL;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			if (LocalFileNames)
			{
				HeapFree(GetProcessHeap(), 0, LocalFileNames);
				LocalFileNames = NULL;
			}
			if (RemoteFileNames)
			{
				HeapFree(GetProcessHeap(), 0, RemoteFileNames);
				RemoteFileNames = NULL;
			}
		}
	}
	
	if (LocalFileNames)
	{
		DWORD Len = (DWORD)wcslen(LocalFileNames);
		if (Len > 0 && LocalFileNames[Len-1] == L' ')
			LocalFileNames[Len-1] = 0;
	}
	if (RemoteFileNames)
	{
		DWORD Len = (DWORD)wcslen(RemoteFileNames);
		if (Len > 0 && RemoteFileNames && RemoteFileNames[Len-1] == L' ')
			RemoteFileNames[Len-1] = 0;
	}

	ODS(("\tparams - display name '%S' local file name '%S' remote file name '%S'", DisplayName, LocalFileNames, RemoteFileNames));

	if ( WaitClientInited( CLIENT_REGISTER_TIMEOUT ) )
	{
		PFILTER_EVENT_PARAM pParam = (PFILTER_EVENT_PARAM)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x4000);
		if (pParam)
		{
			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_R3, R3_BACKGROUNDCOPYJOB_RESUME, 0,  PreProcessing, 5);

			PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM)pParam->Params;

			SINGLE_PARAM_INIT_SID(pSingleParam);
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_TYPE, Type);
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
			pSingleParam->ParamSize = sizeof(WCHAR);
			*(PWCHAR)pSingleParam->ParamValue = 0;
			if (LocalFileNames)
			{
				pSingleParam->ParamSize = (DWORD)sizeof(WCHAR)*(wcslen(LocalFileNames)+1);
				memcpy(pSingleParam->ParamValue, LocalFileNames, pSingleParam->ParamSize);
			}
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_DEST_W);
			pSingleParam->ParamSize = sizeof(WCHAR);
			*(PWCHAR)pSingleParam->ParamValue = 0;
			if (RemoteFileNames)
			{
				pSingleParam->ParamSize = (DWORD)sizeof(WCHAR)*(wcslen(RemoteFileNames)+1);
				memcpy(pSingleParam->ParamValue, RemoteFileNames, pSingleParam->ParamSize);
			}
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_PARAM_W);
			pSingleParam->ParamSize = sizeof(WCHAR);
			*(PWCHAR)pSingleParam->ParamValue = 0;
			if (DisplayName)
			{
				pSingleParam->ParamSize = (DWORD)sizeof(WCHAR)*(wcslen(DisplayName)+1);
				memcpy(pSingleParam->ParamValue, DisplayName, pSingleParam->ParamSize);
			}
			SINGLE_PARAM_SHIFT(pSingleParam);

			HRESULT hResult;
			ULONG VerdictFlags;
			hResult = g_pFSSyncContext->m_DRV_FilterEvent(
				g_pFSSyncContext->m_pClientContext,
				pParam,
				FALSE,
				&VerdictFlags
				);
			if ( IS_ERROR( hResult ) )
				ODS(("DRV_FilterEvent failed"));
			else
			{
				Verdict = g_pFSSyncContext->m_DRV_VerdictFlags2Verdict( VerdictFlags );
			}

			HeapFree(GetProcessHeap(), 0, pParam);
		}
	}

	if (DisplayName)
		CoTaskMemFree(DisplayName);
	if (LocalFileNames)
		HeapFree(GetProcessHeap(), 0, LocalFileNames);
	if (RemoteFileNames)
		HeapFree(GetProcessHeap(), 0, RemoteFileNames);

	if (_PASS_VERDICT(Verdict))
	{
		if (fOrigFunc)
			return fOrigFunc(_this);
	}

	return E_FAIL;
}

VERDICT Pre_ShowHTMLDialog(IMoniker *pMoniker)
{
	VERDICT Verdict = Verdict_NotFiltered;
	HRESULT hResult;
	PWCHAR pDisplayName = NULL;

	if (pMoniker)
	{
		__try
		{
			IBindCtx *pBindCtx = NULL;

			hResult = CreateBindCtx(0, &pBindCtx);
			if (SUCCEEDED(hResult))
			{
				PWCHAR _pDisplayName = NULL;

				hResult = pMoniker->GetDisplayName(pBindCtx, NULL, &pDisplayName);
				if (IS_ERROR(hResult))
					pDisplayName = NULL;

				pBindCtx->Release();
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			pDisplayName = NULL;
		}
	}

	ODS(("ShowHTMLDialog - %S", pDisplayName));

	if (pDisplayName)
	{
		if ( WaitClientInited( CLIENT_REGISTER_TIMEOUT ) )
		{
			PFILTER_EVENT_PARAM pParam = (PFILTER_EVENT_PARAM)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x1000);
			if (pParam)
			{
				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_R3, R3_SHOWHTMLDIALOG, 0,  PreProcessing, 2);

				PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM)pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);
				SINGLE_PARAM_SHIFT(pSingleParam);

				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
				pSingleParam->ParamSize = (DWORD)sizeof(WCHAR)*(wcslen(pDisplayName)+1);
				memcpy(pSingleParam->ParamValue, pDisplayName, pSingleParam->ParamSize);
				SINGLE_PARAM_SHIFT(pSingleParam);

				HRESULT hResult;
				ULONG VerdictFlags;
				hResult = g_pFSSyncContext->m_DRV_FilterEvent(
					g_pFSSyncContext->m_pClientContext,
					pParam,
					FALSE,
					&VerdictFlags
					);
				if ( IS_ERROR( hResult ) )
					ODS(("DRV_FilterEvent failed"));
				else
				{
					Verdict = g_pFSSyncContext->m_DRV_VerdictFlags2Verdict( VerdictFlags );
				}

				HeapFree(GetProcessHeap(), 0, pParam);
			}
		}

// free ole str
		IMalloc *pMalloc = NULL;

		hResult = CoGetMalloc(1, &pMalloc);
		if (SUCCEEDED(hResult) && pMalloc)
			pMalloc->Free(pDisplayName);
	}

	return Verdict;
}

typedef HRESULT (WINAPI *t_fShowHTMLDialog)(	HWND hwndParent,
												IMoniker *pMk,
												VARIANT *pvarArgIn,
												WCHAR *pchOptions,
												VARIANT *pvarArgOut
												);

STDMETHODIMP Hook_ShowHTMLDialog(	HWND hwndParent,
									IMoniker *pMk,
									VARIANT *pvarArgIn,
									WCHAR *pchOptions,
									VARIANT *pvarArgOut
								)
{
	t_fShowHTMLDialog fOrigFunc = (t_fShowHTMLDialog)HookGetOrigFunc();

	ODS(("ShowHTMLDialog"));

	VERDICT Verdict = Pre_ShowHTMLDialog(pMk);

	if (_PASS_VERDICT(Verdict))
	{
		if (fOrigFunc)
			return fOrigFunc(hwndParent, pMk, pvarArgIn, pchOptions, pvarArgOut);
	}
		
	return E_FAIL;
}

typedef HRESULT (WINAPI *t_fShowHTMLDialogEx)(	HWND hwndParent,
												IMoniker *pMk,
												DWORD dwDialogFlags,
												VARIANT *pvarArgIn,
												WCHAR *pchOptions,
												VARIANT *pvarArgOut
												);
	
STDMETHODIMP Hook_ShowHTMLDialogEx(	HWND hwndParent,
									IMoniker *pMk,
									DWORD dwDialogFlags,
									VARIANT *pvarArgIn,
									WCHAR *pchOptions,
									VARIANT *pvarArgOut
									)
{
	t_fShowHTMLDialogEx fOrigFunc = (t_fShowHTMLDialogEx)HookGetOrigFunc();

	ODS(("ShowHTMLDialogEx"));

	VERDICT Verdict = Pre_ShowHTMLDialog(pMk);

	if (_PASS_VERDICT(Verdict))
	{
		if (fOrigFunc)
			return fOrigFunc(hwndParent, pMk, dwDialogFlags, pvarArgIn, pchOptions, pvarArgOut);
	}

	return E_FAIL;
}

/*
 * Raw Input request APIs
 */
typedef struct tagRAWINPUTDEVICE {
    USHORT usUsagePage; // Toplevel collection UsagePage
    USHORT usUsage;     // Toplevel collection Usage
    DWORD dwFlags;
    HWND hwndTarget;    // Target hwnd. NULL = follows keyboard focus
} RAWINPUTDEVICE, *PRAWINPUTDEVICE, *LPRAWINPUTDEVICE;

typedef CONST RAWINPUTDEVICE* PCRAWINPUTDEVICE;

typedef BOOL (WINAPI *t_fRegisterRawInputDevices)(	PCRAWINPUTDEVICE pRawInputDevices,
													UINT uiNumDevices,
													UINT cbSize);

BOOL WINAPI Hook_RegisterRawInputDevices(	PCRAWINPUTDEVICE pRawInputDevices,
											UINT uiNumDevices,
											UINT cbSize)
{
	t_fRegisterRawInputDevices fOrigFunc = (t_fRegisterRawInputDevices)HookGetOrigFunc();

	ODS(("RegisterRawInputDevices"));

	VERDICT Verdict = Verdict_NotFiltered;

	if ( WaitClientInited( CLIENT_REGISTER_TIMEOUT ) )
	{
		CHAR Buf[sizeof(FILTER_EVENT_PARAM)+0x100];
		ZeroMemory(Buf, sizeof(Buf));
		PFILTER_EVENT_PARAM pParam = (PFILTER_EVENT_PARAM)Buf;
		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_R3, R3_REGISTERINPUTDEVICES, 0,  PreProcessing, 1);
		PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM)pParam->Params;

		SINGLE_PARAM_INIT_SID(pSingleParam);
		SINGLE_PARAM_SHIFT(pSingleParam);

		HRESULT hResult;
		ULONG VerdictFlags;
		hResult = g_pFSSyncContext->m_DRV_FilterEvent(
			g_pFSSyncContext->m_pClientContext,
			pParam,
			FALSE,
			&VerdictFlags
			);
		if ( IS_ERROR( hResult ) )
			ODS(("DRV_FilterEvent failed"));
		else
		{
			Verdict = g_pFSSyncContext->m_DRV_VerdictFlags2Verdict( VerdictFlags );
		}
	}

	if (_PASS_VERDICT(Verdict))
	{
		if (fOrigFunc)
			return fOrigFunc(pRawInputDevices, uiNumDevices, cbSize);
	}

	return E_FAIL;
}

void HookNewObjectMethods(void* _this, void* pNewObject, REFIID riid)
{
	SetIUnknownHooks(pNewObject, NULL);

// IID_IClassFactory
	if (0 == memcmp(&IID_IClassFactory, &riid, sizeof(GUID)))
	{
		HookIFaceMethod(pNewObject, 3,  ClassFactory_CreateInstance, "IClassFactory");
		return;
	}

	if (0 == memcmp(&IID_IPSFactoryBuffer, &riid, sizeof(GUID)))
	{
		HookIFaceMethod(pNewObject, 3,  PSFactoryBuffer_CreateProxy, "IPSFactoryBuffer");
		return;
	}

	if (0 == memcmp(&IID_IActiveDesktop, &riid, sizeof(GUID)))
	{
		HookIFaceMethod(pNewObject, 5,  ActiveDesktop_SetWallpaper, "IActiveDesktop");
		return;
	}

	if (0 == memcmp(&IID_IBackgroundCopyManager, &riid, sizeof(GUID)))
	{
		HookIFaceMethod(pNewObject, 3,  BackgroundCopyManager_CreateJob, "IBackgroundCopyManager");
		return;
	}

	if (0 == memcmp(&IID_IBackgroundCopyJob, &riid, sizeof(GUID)))
	{
		HookIFaceMethod(pNewObject, 7,  BackgroundCopyJob_Resume, "IBackgroundCopyJob");
		return;
	}

// default
	return;
}

BOOL SetIUnknownHooks(void* pObject, char* szIFaceName)
{
	BOOL bRes = TRUE;
	if (!szIFaceName)
		szIFaceName = "IUnknown";
	bRes = bRes && HookIFaceMethod(pObject, 0, IUnknown_QueryInterfaceHook, szIFaceName);

	return bRes;
}

STDMETHODIMP IUnknown_QueryInterfaceHook(LPVOID _this, 
	REFIID riid, void ** ppvObject)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	
#ifdef _DEBUG
//	char szDbgBuff[0x100];
//	ODS(("%08X->%s::IUnknown_QueryInterfaceHook( %s )", _this, (/*pHookInfo->m_szIFaceName?pHookInfo->m_szIFaceName:*/"IUnknown"), DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid)));
#endif
	
	hRes = ((HRESULT (__stdcall *)(LPVOID, REFIID , void**))OrigFuncAddr)(_this, riid, ppvObject);

	if (hRes != S_OK || ppvObject==NULL || *ppvObject==NULL || *(void**)*ppvObject==NULL)
	{
		ODS(("QueryInterface failed", hRes));
		return hRes;
	}

		
	// Hook appropriate methods for any known interface 
	//ODS(("IUnknown_QueryInterfaceHook->HookNewObjectMethods(%08X, %08X)", _this, *ppvObject));
	HookNewObjectMethods(_this, *ppvObject, riid);
	//ODS(("~IUnknown_QueryInterfaceHook->HookNewObjectMethods"));

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

typedef HRESULT (WINAPI *t_fPSFactoryBuffer_CreateProxy)(PVOID _this, IUnknown *pUnkOuter, REFIID riid, IRpcProxyBuffer **ppProxy, void **ppv);

STDMETHODIMP PSFactoryBuffer_CreateProxy(PVOID _this, IUnknown *pUnkOuter, REFIID riid, IRpcProxyBuffer **ppProxy, void **ppv)
{
	HRESULT hRes;
	t_fPSFactoryBuffer_CreateProxy OrigFuncAddr = (t_fPSFactoryBuffer_CreateProxy)HookGetOrigFunc();

#if defined(_DEBUG) || defined(ENABLE_ODS)
	char szDbgBuff[0x100];
#endif

	hRes = OrigFuncAddr(_this, pUnkOuter, riid, ppProxy, ppv);
	ODS(("%08X->PSFactoryBuffer::CreateProxy( %s ) = %08X", _this, DbgStringFromGUID(szDbgBuff, sizeof(szDbgBuff), &riid), (ppv ? *ppv : NULL) ));
	if (SUCCEEDED(hRes) && ppv && *ppv)
	{
		ODS(("PSFactoryBuffer::CreateProxy->HookNewObjectMethods"));
		HookNewObjectMethods(_this, *ppv, riid);
	}

	return hRes;
}

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

BOOL HookExports()
{
	BOOL bResult = FALSE;

	HMODULE hMsHtml = GetModuleHandleA("mshtml.dll");
	if (hMsHtml)
	{
		bResult = HookExportInModule(hMsHtml, "ShowHTMLDialog", Hook_ShowHTMLDialog, NULL, 0);
		bResult &= HookExportInModule(hMsHtml, "ShowHTMLDialogEx", Hook_ShowHTMLDialogEx, NULL, 0);
	}

	HMODULE hUser32 = GetModuleHandleA("user32.dll");
	if (hUser32)
	{
		bResult &= HookExportInModule(hUser32, "RegisterRawInputDevices", Hook_RegisterRawInputDevices, NULL, 0);
	}

	return bResult;
}

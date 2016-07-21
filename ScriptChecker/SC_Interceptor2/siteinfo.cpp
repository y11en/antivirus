#include "siteinfo.h"

#include "scripting/activscp.h"
#include "exdisp.h"
#include "mshtml.h"
#include <SHLGUID.h>
#include "resource.h"
#include "comutil.h"
#include "comdef.h"
#include "commctrl.h"
#include "IMimeMessageW.h"
#include "debug.h"
#include "../../windows/hook/r3hook/hookbase64.h"
#include "scplugins.h"
//#include <hlink.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <EXDISPID.H>
#include "IeStatusBar.h"
#include "gmutex.h"
#include "../klscav/klscav.h"
#include "appinfo.h"

#define PLACE_POPUP_ICON 3

#define LCASE(c) ( c >= 'A' ? ( c <= 'Z' ? c | 0x20 : c ) : c )

extern BOOL HookIFaceMethod(void* pObject, DWORD nMethod, LPVOID pHookFunc, char* szIFaceName);

int __cdecl __wcsnicmp (
						const wchar_t * first,
						const wchar_t * last,
						size_t count
						)
{
	if (!first || !last)
		return(-1);
	if (!count)
		return(0);
	
	while (--count && *first && LCASE(*first) == LCASE(*last))
	{
		first++;
		last++;
	}
	
	return((int)(*first - *last));
}



static BSTR GetNamedPropertyBStr(IDispatch* pDisp, WCHAR* pwcsProp)
{
	BSTR bstr = NULL;
	wchar_t	*prop_names[1] = {0};
	DISPID	dispIds[1] = {0};
	prop_names[0] = pwcsProp;
	if(SUCCEEDED(pDisp->GetIDsOfNames(IID_NULL, prop_names, 1, LANG_SYSTEM_DEFAULT,dispIds)))
		_com_dispatch_propget(pDisp, dispIds[0], VT_BSTR, (void*)&bstr);
	return bstr;
}

HRESULT _SP_QueryService(IServiceProvider* sp, 
				/* [in] */ REFGUID guidService,
                /* [in] */ REFIID riid,
                /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject
				)
{
	// MS has bug - they forgot call AddRef for interface returned from 
	// IServiceProvider::QueryService of IE ActiveScriptSite, as a 
	// result unbalanced AddRef/Release lead to unexpected crashes

	ULONG counter1, counter2;
	IUnknown* pvObject1 = NULL;
	IUnknown* pvObject2 = NULL;
	HRESULT hRes, hRes2;
	
	hRes = sp->QueryService(guidService, riid, ppvObject);
	if (FAILED(hRes))
		return hRes;

	pvObject1 = (IUnknown*)*ppvObject;
	pvObject1->AddRef();
	counter1 = pvObject1->Release();
		
	hRes2 = sp->QueryService(guidService, IID_IUnknown, (void**)&pvObject2);
	if (FAILED(hRes2))
		return hRes;

	pvObject2->AddRef();
	counter2 = pvObject2->Release();

	if (pvObject1 == pvObject2 && counter2 == counter1 && counter2 != 0)
	{
		// MS bug - counter2 must be greater then counter1
		ODS(("_SP_QueryService: unbalanced refcount found"));
		pvObject1->AddRef();
	}
	else
	{
		pvObject2->Release();
	}
	return hRes;
}

HWND GetExplorerStatusBarHWnd(IUnknown* pActiveScriptSite, IWebBrowser2** ppWebBrowser2)
{
	HRESULT hRes;
	IServiceProvider* sp;
	IServiceProvider* sp2;
	IWebBrowser2* pWebBrowser2;

	HWND hWndStatusBar = NULL;
	if (ppWebBrowser2)
		*ppWebBrowser2 = NULL;
	if (pActiveScriptSite == NULL)
		return NULL;
	hRes = pActiveScriptSite->QueryInterface(IID_IServiceProvider, (void**)&sp);
	if (SUCCEEDED(hRes))
	{
		hRes = _SP_QueryService(sp, SID_STopLevelBrowser, IID_IServiceProvider, (void **)(&sp2));
		if (SUCCEEDED(hRes))
		{
			hRes = _SP_QueryService(sp2, SID_SWebBrowserApp, IID_IWebBrowser2, (void **)(&pWebBrowser2));
			if (SUCCEEDED(hRes))
			{
				HWND hWndBrowser;
				if (ppWebBrowser2)
					*ppWebBrowser2 = pWebBrowser2;
				hRes = pWebBrowser2->get_HWND((SHANDLE_PTR*)&hWndBrowser);
				if (SUCCEEDED(hRes) && hWndBrowser!=NULL)
				{
					hWndStatusBar = FindWindowEx(hWndBrowser, NULL, "msctls_statusbar32", NULL);
					if (!hWndStatusBar)
					{
						// IE7
						hWndStatusBar = FindWindowEx(hWndBrowser, NULL, "TabWindowClass", NULL);
						if (hWndStatusBar)
							hWndStatusBar = FindWindowEx(hWndStatusBar, NULL, "msctls_statusbar32", NULL);
					}
				}
				pWebBrowser2->Release();
			}
			sp2->Release();
		}
		sp->Release();
	}
	return hWndStatusBar;
}

struct IScriptletError;
interface IScriptletSite {
	public: virtual long __stdcall QueryInterface(struct _GUID const &,void * *);
	public: virtual unsigned long __stdcall AddRef(void);
	public: virtual unsigned long __stdcall Release(void);
	public: virtual long __stdcall OnError(unsigned long,struct IScriptletError *);
	public: virtual long __stdcall EnableModeless(int);
	public: virtual long __stdcall GetInfo(unsigned long,struct IServiceProvider *,struct tagVARIANT *,struct IServiceProvider *);
};
interface IScriptletContext {
	public: virtual long __stdcall QueryInterface (struct _GUID const &,void * *);
	public: virtual unsigned long __stdcall AddRef (void);
	public: virtual unsigned long __stdcall Release (void);
	public: virtual long __stdcall Initialize(struct IScriptletSite *);
	public: virtual long __stdcall Abort(struct tagEXCEPINFO *);
	public: virtual long __stdcall Reset(void);
	public: virtual long __stdcall GetSite(struct _GUID const &,void * *);
	public: virtual long __stdcall SetProperty(unsigned long,struct tagVARIANT *,unsigned long);
	public: virtual long __stdcall GetProperty(unsigned long,struct tagVARIANT *,unsigned long);
};
struct ITextStream;
interface IHost2 {
	public: virtual long __stdcall QueryInterface(struct _GUID const &,void * *);
	public: virtual unsigned long __stdcall AddRef(void);
	public: virtual unsigned long __stdcall Release(void);
	public: virtual long __stdcall GetTypeInfoCount(unsigned int *);
	public: virtual long __stdcall GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo * *);
	public: virtual long __stdcall GetIDsOfNames(struct _GUID const &,unsigned short * *,unsigned int,unsigned long,long *);
	public: virtual long __stdcall Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *);
	public: virtual long __stdcall get_Name(unsigned short * *);
	public: virtual long __stdcall get_Application(struct IDispatch * *);
	public: virtual long __stdcall get_FullName(unsigned short * *);
	public: virtual long __stdcall get_Path(unsigned short * *);
	public: virtual long __stdcall get_Interactive(short *);
	public: virtual long __stdcall put_Interactive(short);
	public: virtual long __stdcall Quit(int);
	public: virtual long __stdcall get_ScriptName(unsigned short * *);
	public: virtual long __stdcall get_ScriptFullName(unsigned short * *);
	public: virtual long __stdcall get_Arguments(struct IArguments * *);
	public: virtual long __stdcall get_Version(unsigned short * *);
	public: virtual long __stdcall get_BuildVersion(int *);
	public: virtual long __stdcall get_Timeout(long *);
	public: virtual long __stdcall put_Timeout(long);
	public: virtual long __stdcall CreateObject(unsigned short *,unsigned short *,struct IDispatch * *);
	public: virtual long __stdcall Echo(struct tagSAFEARRAY *);
	public: virtual long __stdcall GetObjectA(unsigned short *,unsigned short *,unsigned short *,struct IDispatch * *);
	public: virtual long __stdcall DisconnectObject(struct IDispatch *);
	public: virtual long __stdcall Sleep(long);
	public: virtual long __stdcall ConnectObject(struct IDispatch *,unsigned short *);
	public: virtual long __stdcall get_StdIn(struct ITextStream * *);
	public: virtual long __stdcall get_StdOut(struct ITextStream * *);
	public: virtual long __stdcall get_StdErr(struct ITextStream * *);
};

interface IPersistMime {
	public: virtual long __stdcall QueryInterface (struct _GUID const &,void * *);
	public: virtual unsigned long __stdcall AddRef (void);
	public: virtual unsigned long __stdcall Release (void);
	public: virtual long __stdcall ContextSensitiveHelp(int);
	public: virtual long __stdcall Load(struct IMimeMessageW *);
	public: virtual long __stdcall Save(struct IMimeMessageW *,unsigned long);
};

BOOL FindFrame(IHTMLWindow2* pHTMLWindow2, WCHAR* wcsTarget )
{

	if (wcsTarget == NULL)
		return FALSE;
	if (0 == wcscmp(wcsTarget, L"_blank"))
		return FALSE;
	
	IHTMLDocument2Ptr pHTMLDoc;
	if (SUCCEEDED(pHTMLWindow2->get_document(&pHTMLDoc)) && NULL != pHTMLDoc)
	{
		BSTR pDocUrl = NULL; 
		DWORD dwFlags = 0;
		
		IHTMLFramesCollection2Ptr pFrames;
		if (SUCCEEDED(pHTMLDoc->get_frames(&pFrames)) && NULL != pFrames)
		{
			VARIANT item;
			VARIANT frame;
			item.vt = VT_BSTR;
			item.bstrVal = wcsTarget;
			if (SUCCEEDED(pFrames->item(&item, &frame)) && NULL != frame.pdispVal)
			{
				ODS(("CheckDenyPopupURL: Frame %S found", wcsTarget));
				frame.pdispVal->Release();
				return TRUE;
			}
			else
			{
				long lenght;
				if (SUCCEEDED(pFrames->get_length(&lenght)))
				{
					ODS(("HTMLWindow2_OpenHook: sFrames count=%d", lenght));
					for (long i=0; i<lenght; i++)
					{
						IHTMLWindow2* pWindow;
						item.vt = VT_I4;
						item.llVal = i;
						pFrames->item(&item, &frame);
						if (frame.pdispVal)
						{
							frame.pdispVal->QueryInterface(IID_IHTMLWindow2, (void**)&pWindow);
#ifdef _DEBUG
							IHTMLDocument2Ptr pDoc;
							BSTR pName = NULL;
							pWindow->get_document(&pDoc);
							pWindow->get_name(&pName);
							pDoc->get_URL(&pDocUrl);
							ODS(("HTMLWindow2_OpenHook: Frame(%d) name is '%S' url='%S'", i, pName, pDocUrl));
							if (pDocUrl)
								SysFreeString(pDocUrl);
							if (pName)
								SysFreeString(pName);
							pDocUrl = NULL;
#endif
							if (FindFrame(pWindow, wcsTarget))
								return TRUE;
							pWindow->Release();
							frame.pdispVal->Release();
						}

					}
				}
			}
		}
	}
	return FALSE;
}

BOOL __stdcall CheckPopupURLCallbackFn(LPVOID pCallbackCtx, DWORD dwAction, WCHAR* pwcsSourceURL)
{
	HWND hWnd = (HWND)pCallbackCtx;
	if (dwAction == SCP_ACTION_POPUP_TEMPORARY_ALLOWED)
	{
		ODS(("calling IeStatusBarTemoraryAllow(hWnd=%X, \"%S\")...", hWnd, pwcsSourceURL ? pwcsSourceURL : L"NULL"));
		bool bRes = IeStatusBarTemoraryAllow(hWnd, pwcsSourceURL, true);
		ODS(("IeStatusBarTemoraryAllow done", hWnd, pwcsSourceURL ? pwcsSourceURL : L"NULL"));
	}
	return TRUE;
}

BOOL CheckDenyPopupURL(IUnknown* _this, WCHAR* wcsSourceUrl, WCHAR* wcsDestinationUrl, WCHAR* wcsTarget)
{
	BOOL bDenyOpen = FALSE;
	IWebBrowser2* pWebBrowser2 = NULL;
	HWND hWnd = GetExplorerStatusBarHWnd(_this, &pWebBrowser2);

	if (WCHAR* ptailtoremove = wcschr(wcsSourceUrl, '#'))
		*ptailtoremove = 0; // truncate # and all after it - it is a link within the page.

	if (IsPopupsTemoraryAllowed(hWnd, wcsSourceUrl))
	{
		ScPluginsCheckPopupURL(SCP_ACTION_POPUP_TEMPORARY_ALLOWED, wcsSourceUrl, wcsDestinationUrl, NULL, NULL, NULL);
		ODS(("Popups temporary allowed for hwnd=%08X: source=%S, target=%S", hWnd, wcsSourceUrl ? wcsSourceUrl : L"", wcsTarget ? wcsTarget : L"<empty>"));
		IeStatusBarSetState(hWnd, PLACE_POPUP_ICON, PBSTATE_ALLOWED, pWebBrowser2, wcsSourceUrl);
		return FALSE;
	}
	
	try
	{

		SC_THREAD_DATA* pThreadData = GetSCThreadData();
		if (pThreadData)
		{
			if (pThreadData->bNavigate2Script)
			{
				// navigate to script by user action
				ODS(("CheckDenyPopupURL: navigate2script"));
				return FALSE;
			}
			if (pThreadData->bstrEventName)
			{
				ODS(("CheckDenyPopupURL: event=%S", pThreadData->bstrEventName));
				if (0 == wcscmp(pThreadData->bstrEventName, L"click"))
					return FALSE;
				if (0 == wcscmp(pThreadData->bstrEventName, L"change"))
					return FALSE;
			}
		}

		if (NULL == wcsTarget
			|| 0 == wcscmp(wcsTarget, L"_media")
			|| 0 == wcscmp(wcsTarget, L"_parent")
			|| 0 == wcscmp(wcsTarget, L"_search")
			|| 0 == wcscmp(wcsTarget, L"_self")
			|| 0 == wcscmp(wcsTarget, L"_top"))
		{
			ODS(("CheckDenyPopupURL: target=%S", wcsTarget ? wcsTarget : L"<empty>"));
			return FALSE; // valid target
		}

		IHTMLWindow2Ptr pHTMLWindow2;
		if (SUCCEEDED(_this->QueryInterface(IID_IHTMLWindow2, (void**)&pHTMLWindow2)) && NULL != pHTMLWindow2)
		{
			IHTMLWindow2Ptr pHTMLWindow2Top;
			pHTMLWindow2->get_top(&pHTMLWindow2Top);
			if (FindFrame(NULL != pHTMLWindow2Top ? pHTMLWindow2Top : pHTMLWindow2, wcsTarget))
				return FALSE;
			IHTMLDocument2Ptr pHTMLDoc;
			if (SUCCEEDED(pHTMLWindow2->get_document(&pHTMLDoc)) && NULL != pHTMLDoc)
			{
				BSTR pDocUrl = NULL; 
				DWORD dwFlags = 0;
				
				if (!wcsSourceUrl)
				{
					pDocUrl = GetNamedPropertyBStr(pHTMLDoc, L"URLUnencoded");
					if (!pDocUrl)
						pHTMLDoc->get_URL(&pDocUrl);
					wcsSourceUrl = pDocUrl;
				}

				if (ScPluginsCheckPopupURL(SCP_ACTION_POPUP_CHECK_DENY, wcsSourceUrl, wcsDestinationUrl, &dwFlags, CheckPopupURLCallbackFn, hWnd))
				{
					if (dwFlags & SCP_BLOCK)
					{
						ODS(("CheckDenyPopupURL: ScPluginsCheckPopupURL say BLOCK"));
						if (pDocUrl)
							SysFreeString(pDocUrl);
						IeStatusBarSetState(hWnd, PLACE_POPUP_ICON, PBSTATE_BLOCKED, pWebBrowser2, wcsSourceUrl);
						return TRUE; // block pop-up window
					}
					else
					{
						IeStatusBarSetState(hWnd, PLACE_POPUP_ICON, PBSTATE_ALLOWED, pWebBrowser2, wcsSourceUrl);
						ODS(("CheckDenyPopupURL: ScPluginsCheckPopupURL say ALLOW"));
					}
				}
				if (pDocUrl)
					SysFreeString(pDocUrl);
			}
		}
	}
	catch(...)
	{
		ODS(("ERROR: Exception catched in CheckDenyPopupURL"));
	}
	
	return FALSE;
}

STDMETHODIMP HTMLWindow2_OpenHook(IHTMLWindow2* _this, 
            /* [in][defaultvalue] */ BSTR url,
            /* [in][defaultvalue] */ BSTR name,
            /* [in][defaultvalue] */ BSTR features,
            /* [in][defaultvalue] */ VARIANT_BOOL replace,
            /* [out][retval] */ IHTMLWindow2 __RPC_FAR *__RPC_FAR *pomWindowResult)
{
    HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	BOOL bDenyOpen = FALSE;
	BSTR pDocUrl = NULL; 
	IHTMLDocument2* pHTMLDoc = NULL;
	
	if (SUCCEEDED(_this->get_document(&pHTMLDoc)) && NULL != pHTMLDoc)
	{
		pDocUrl = GetNamedPropertyBStr(pHTMLDoc, L"URLUnencoded");
		if (!pDocUrl)
			pHTMLDoc->get_URL(&pDocUrl);
		pHTMLDoc->Release();
	}

    ODS(("%08X->pHTMLWindow2::Open(\"%S\", \"%S\", \"%S\")", _this, url ? url : L"<url=empty>", name ? name : L"name=<empty>", features ? features : L"features=<empty>"));
	
	bDenyOpen = CheckDenyPopupURL(_this, pDocUrl, url, name);
	if (pDocUrl)
		SysFreeString(pDocUrl);

	if (bDenyOpen) 
	{
		IWebBrowser2* pWebBrowser2 = NULL; 
		ODS(("* BLOCKED * %08X->pHTMLWindow2::Open(\"%S\", \"%S\", \"%S\")", _this, url ? url : L"<url=empty>", name ? name : L"name=<empty>", features ? features : L"features=<empty>"));
		if (pomWindowResult)
			*pomWindowResult = NULL;

		SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineData = GetScriptEngineInstanceData( GetCurEngineInstanceTls() );
		if (pScriptEngineData)
			pScriptEngineData->bSupressErrorCode = 0x800A138F;
		return S_OK;
	}


    hRes = ((HRESULT (__stdcall *)(IHTMLWindow2*,BSTR,BSTR,BSTR,VARIANT_BOOL,IHTMLWindow2**))OrigFuncAddr)(_this, url, name, features, replace,pomWindowResult);
	ODS(("HTMLWindow2_OpenHook: Original func returned hres = %08X", hRes));
    return hRes;
}

STDMETHODIMP HTMLWindow3_showModelessDialog(IHTMLWindow3* _this, 
	BSTR url,
    VARIANT *varArgIn,
    VARIANT *options,
    IHTMLWindow2 **pDialog
)
{
	HRESULT hRes;
	void* OrigFuncAddr = HookGetOrigFunc();
	BOOL bDenyOpen = FALSE;
	ODS(("%08X->pHTMLWindow2::showModelessDialog(\"%S\", ...)", _this, url ? url : L"<url=empty>"));

	BSTR pDocUrl = NULL; 
	IHTMLDocument2* pHTMLDoc = NULL;
	IHTMLWindow2* pHTMLWindow2 = NULL;

	
	if (SUCCEEDED(_this->QueryInterface(IID_IHTMLWindow2, (void**)&pHTMLWindow2)) && NULL != pHTMLWindow2)
	{
		if (SUCCEEDED(pHTMLWindow2->get_document(&pHTMLDoc)) && NULL != pHTMLDoc)
		{
			pDocUrl = GetNamedPropertyBStr(pHTMLDoc, L"URLUnencoded");
			if (!pDocUrl)
				pHTMLDoc->get_URL(&pDocUrl);
			pHTMLDoc->Release();
		}
		pHTMLWindow2->Release();
	}

	bDenyOpen = CheckDenyPopupURL(_this, pDocUrl, url, L"_blank");
	if (pDocUrl)
		SysFreeString(pDocUrl);
	
	if (bDenyOpen) 
	{
		ODS(("* BLOCKED * %08X->pHTMLWindow2::showModelessDialog(\"%S\", ...)", _this, url ? url : L"<url=empty>"));
		if (pDialog)
			*pDialog = NULL;
		SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineData = GetScriptEngineInstanceData( GetCurEngineInstanceTls() );
		if (pScriptEngineData)
			pScriptEngineData->bSupressErrorCode = 0x800A138F;
		return S_OK;
	}

    hRes = ((HRESULT (__stdcall *)(IHTMLWindow3*,BSTR,VARIANT*,VARIANT*,IHTMLWindow2**))OrigFuncAddr)(_this, url, varArgIn, options, pDialog);
	ODS(("HTMLWindow2_OpenHook: Original func returned hres = %08X", hRes));
    return hRes;
}

class MyHTMLDocumentEvents : public HTMLDocumentEvents
{
public:
	MyHTMLDocumentEvents(IServiceProvider* _sp)
	{
		sp = _sp; 
		sp->AddRef();
	};
	~MyHTMLDocumentEvents() 
	{ 
		if (sp) 
		{
			sp->Release(); 
			sp = NULL;
		};
	}
	HRESULT STDMETHODCALLTYPE MyHTMLDocumentEvents::QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
		
	ULONG STDMETHODCALLTYPE AddRef( void);
	
	ULONG STDMETHODCALLTYPE Release( void);
	
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ UINT *pctinfo);
        
	HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo **ppTInfo);
        
    HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ REFIID riid,
		/* [size_is][in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr);
public:
	IServiceProvider* sp;
};

HRESULT STDMETHODCALLTYPE MyHTMLDocumentEvents::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (riid == IID_IDispatch)
	{
		*ppvObject = this;
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE MyHTMLDocumentEvents::AddRef( void)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE MyHTMLDocumentEvents::Release( void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyHTMLDocumentEvents::GetTypeInfoCount( 
	/* [out] */ UINT *pctinfo)
{
	return E_NOTIMPL;
}
    
HRESULT STDMETHODCALLTYPE MyHTMLDocumentEvents::GetTypeInfo( 
	/* [in] */ UINT iTInfo,
	/* [in] */ LCID lcid,
	/* [out] */ ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}
    
HRESULT STDMETHODCALLTYPE MyHTMLDocumentEvents::GetIDsOfNames( 
	/* [in] */ REFIID riid,
	/* [size_is][in] */ LPOLESTR *rgszNames,
	/* [in] */ UINT cNames,
	/* [in] */ LCID lcid,
	/* [size_is][out] */ DISPID *rgDispId)
{
	return E_NOTIMPL;
}
    
    /* [local] */ HRESULT STDMETHODCALLTYPE MyHTMLDocumentEvents::Invoke( 
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID riid,
	/* [in] */ LCID lcid,
	/* [in] */ WORD wFlags,
	/* [out][in] */ DISPPARAMS *pDispParams,
	/* [out] */ VARIANT *pVarResult,
	/* [out] */ EXCEPINFO *pExcepInfo,
	/* [out] */ UINT *puArgErr)
{

//	HRESULT hRes;
	DWORD tid = GetCurrentThreadId();
	ODS(("MyHTMLDocumentEvents::Invoke: dispIdMember=%d", dispIdMember));
//	if (dispIdMember == 1048 || dispIdMember == -606 || dispIdMember == 105)
//		return E_NOTIMPL;
//	IUnknown* Unkn = NULL;
//	hRes = _SP_QueryService(sp, CLSID_HTMLWindow2, IID_IUnknown, (void **)(&Unkn));
//	if (SUCCEEDED(hRes) && Unkn)
//	{
//		IHTMLWindow2* pHTMLWindow2 = NULL;
//		hRes = Unkn->QueryInterface(IID_IHTMLWindow2, (void **)(&pHTMLWindow2));
//		if (SUCCEEDED(hRes) && pHTMLWindow2)
//		{
//			IHTMLEventObj* event = NULL;
//			if (SUCCEEDED(pHTMLWindow2->get_event(&event)) && event)
//			{
//				BSTR bstrEventType = NULL;
//				if (SUCCEEDED(event->get_type(&bstrEventType)) && bstrEventType)
//				{
//					ODS(("MyHTMLDocumentEvents::Invoke: event=%S", bstrEventType ? bstrEventType : L"<empty>"));
//					SysFreeString(bstrEventType);
//				}
//				event->Release();
//			}
////			IHTMLDocument2* doc = NULL;
////			hRes = pHTMLWindow2->get_document(&doc);
////			if (SUCCEEDED(hRes) && NULL != doc)
////			{
////				IConnectionPointContainer* cpcontainer = NULL;
////				if (SUCCEEDED(doc->QueryInterface(IID_IConnectionPointContainer, (void**)&cpcontainer)) && NULL != cpcontainer)
////				{
////					IConnectionPoint* cp = NULL;
////					if (SUCCEEDED(cpcontainer->FindConnectionPoint(DIID_HTMLDocumentEvents, &cp)) && NULL!=cp)
////					{
////						DWORD dwCookie;
////						cp->Advise(&_events, &dwCookie);
////						cp->Release();
////					}
////					cpcontainer->Release();
////				}
////				doc->Release();
////			}
//			pHTMLWindow2->Release();
//		}	
//		Unkn->Release();
//	}
	if (dispIdMember == DISPID_BEFORENAVIGATE2)
	{
		VARIANT* pVar = NULL;
		BSTR pNavigateUrl = NULL;
		IDispatch* pDispatch = NULL;

		if (pDispParams->cArgs >= 5)
		{
			pVar = &pDispParams->rgvarg[5];
			while (pVar->vt == (VT_BYREF | VT_VARIANT))
				pVar = pVar->pvarVal;
			if (pVar->vt == VT_BSTR)
			{
				pNavigateUrl = pVar->bstrVal;
			}
		}
		if (pDispParams->cArgs >= 6)
		{
			pVar = &pDispParams->rgvarg[6];
			while (pVar->vt == (VT_BYREF | VT_VARIANT))
				pVar = pVar->pvarVal;
			if (pVar->vt == VT_DISPATCH)
			{
				pDispatch = pVar->pdispVal;
			}
		}

		if (pDispatch)
		{
			IWebBrowser2* pWebBrowser2 = NULL;
			HWND hWnd = GetExplorerStatusBarHWnd(pDispatch, &pWebBrowser2);
			IeStatusBarSetState(hWnd, PLACE_POPUP_ICON, PBSTATE_IDLE, pWebBrowser2, pNavigateUrl);
		}

		ODS(("MyHTMLDocumentEvents::Invoke: dispIdMember=DISPID_BEFORENAVIGATE2 pDispatch=%08X pNavigateUrl=%08X:%S", pDispatch, pNavigateUrl, pNavigateUrl?pNavigateUrl:L"NULL"));

		if (pNavigateUrl)
		{
			if (0 == __wcsnicmp(pNavigateUrl, L"javascript:", 11))
			{
				SC_THREAD_DATA* pThreadData = GetSCThreadData();
				ODS(("MyHTMLDocumentEvents::Invoke: dispIdMember=DISPID_BEFORENAVIGATE2, URL=%S, pThreadData=%08X, dwEntryCount=%d", pNavigateUrl,  pThreadData, pThreadData ? pThreadData->dwEntryCount : -1));
				if (pThreadData)
				{
					if (pThreadData->dwEntryCount == 0)
					{
						pThreadData->bNavigate2Script = TRUE;
					}
				}
			}
		}

	}
	return E_NOTIMPL;
}

MyHTMLDocumentEvents* g_events = NULL;

BSTR GetEventName(IActiveScriptSite* pActiveScriptSite)
{
	BSTR bstrEventType = NULL;
	IServiceProvider* sp = NULL;
	HRESULT hRes;
	
	if (!pActiveScriptSite)
		return NULL;
	hRes = pActiveScriptSite->QueryInterface(IID_IServiceProvider, (void**)&sp);
	if (SUCCEEDED(hRes) && sp)
	{
		
		{
			// check for IE
			IServiceProvider* sp2 = NULL;
			hRes = _SP_QueryService(sp, SID_STopLevelBrowser, IID_IServiceProvider, (void **)(&sp2));
			if (SUCCEEDED(hRes) && sp2)
			{
				// yes, it's IE
				sp2->Release();
			}
			else
			{
				// wow, not IE?
				sp->Release();
				return NULL;
			}
		}
		IUnknown* pUnkn = NULL;
		hRes = pActiveScriptSite->GetItemInfo(L"window", SCRIPTINFO_IUNKNOWN, &pUnkn, NULL);
		//hRes = _SP_QueryService(sp, CLSID_HTMLWindow2, IID_IHTMLWindow3, (void **)(&pUnkn));
		if (SUCCEEDED(hRes) && pUnkn)
		{
			IHTMLWindow2* pHTMLWindow2 = NULL;
			hRes = pUnkn->QueryInterface(IID_IHTMLWindow2, (void **)(&pHTMLWindow2));
			if (SUCCEEDED(hRes) && pHTMLWindow2)
			{
				IHTMLEventObj* event = NULL;
				static bool g_bWindowOpenHookSet = FALSE;

				if (!g_bWindowOpenHookSet)
				{
					HookIFaceMethod(pHTMLWindow2, 29, HTMLWindow2_OpenHook, NULL);
					g_bWindowOpenHookSet = TRUE;
				}
				if (SUCCEEDED(hRes = pHTMLWindow2->get_event(&event)) && event)
				{
					//ODS(("GetEventName: 2, event=%08X, hRes = %08X", event, hRes));
					if (SUCCEEDED(event->get_type(&bstrEventType)) && bstrEventType)
					{
						ODS(("GetEventName: event=%S", bstrEventType ? bstrEventType : L"<empty>"));
						//SysFreeString(bstrEventType);
					}
					event->Release();
					event= NULL;
				}
				pHTMLWindow2->Release();
			}
			IHTMLWindow3* pHTMLWindow3 = NULL;
			hRes = pUnkn->QueryInterface(IID_IHTMLWindow3, (void **)(&pHTMLWindow3));
			if (SUCCEEDED(hRes) && pHTMLWindow2)
			{
				static bool g_bWindowShowModelessDialog = FALSE;
				if (!g_bWindowShowModelessDialog)
				{
					HookIFaceMethod(pHTMLWindow3, 19, HTMLWindow3_showModelessDialog, NULL);
					g_bWindowShowModelessDialog = TRUE;
				}
				pHTMLWindow3->Release();
			}
			pUnkn->Release();
		}
		sp->Release();
	}
	return bstrEventType;
}

PWCHAR wcsistr(PWCHAR s, PWCHAR ss) // no codepage support here!!!
{
	WCHAR c, cs;
	if (!s || !ss)
		return NULL;
	cs = LCASE(*ss);
	while (*s)
	{
		c = LCASE(*s);
		if (c == cs)
		{
			int i = 1;
			while (true) 
			{
				WCHAR  cs = LCASE(ss[i]);
				c = LCASE(s[i]);
				if (0 == cs) // found
					return s;
				if (0 == c)  // end of string
					return NULL;
				if (c != cs) // not matched
					break;
				i++;
			}
		}
		s++;
	}
	return NULL;
}


bool FindAttribute(WCHAR* pData, WCHAR* pAttr, WCHAR** pAttrValue, DWORD* pdwAttrValueLen)
{
	if (!pAttrValue || !pdwAttrValueLen)
		return false;
	if (!pData)
		return false;
	if (!*pData)
		return false;
	if (!pAttr)
		return false;
	*pAttrValue = L"";
	*pdwAttrValueLen = 0;
	WCHAR quote = '>';
	do {
		pData = wcsistr(pData+1, pAttr);
		if (!pData)
			break;
		if (pData[-1] > ' ')
			continue;
		pData += wcslen(pAttr);
		while (*pData > 0 && *pData <= ' ')
			pData++;
		if (*pData != '=')
			continue;
		pData++;
		while (*pData > 0 && *pData <= ' ')
			pData++;
		if (!*pData)
			continue;
		if (*pData == '\'' || *pData == '\"')
		{
			quote = *pData;
			pData++;
		}
		*pAttrValue = pData;
		while (*pData > 0x20 && *pData != quote)
			pData++;
		*pdwAttrValueLen = (DWORD)(ULONG_PTR)(pData - *pAttrValue);
		return true;
	} while(1);
	return false;
}

bool ExtractDomain(WCHAR* pUrl, DWORD dwLen, WCHAR** ppDomain, DWORD* pdwDomainLen)
{
	WCHAR* pDomain = NULL;
	WCHAR* pDomainEnd = pUrl;

	while (dwLen)
	{
		WCHAR c = *pDomainEnd;
		if (0==c)
			return false;
		if ('/'==c || '?'==c)
			break;
		if (':'==c && '/'==pDomainEnd[1] && '/'==pDomainEnd[2])
		{
			pDomainEnd+=3;
			pDomain = pDomainEnd;
		}
		pDomainEnd++;
		dwLen--;
	}
	if (pDomain && pDomainEnd!=pDomain)
	{
		*pdwDomainLen = (ULONG)(ULONG_PTR)(pDomainEnd - pDomain);
		*ppDomain = pDomain;
		return true;
	}

	return false;
}

int CompareDomains(WCHAR* pUrl1, DWORD dwUrl1Len, WCHAR* pUrl2, DWORD dwUrl2Len)
{
	int level = 0;
	WCHAR *pDomain1, *pDomain2;
	DWORD dwLen1, dwLen2;
//	DWORD dwLen11, dwLen12;
	if (!ExtractDomain(pUrl1, dwUrl1Len, &pDomain1, &dwLen1))
		return 0;
	if (!ExtractDomain(pUrl2, dwUrl2Len, &pDomain2, &dwLen2))
		return -1;
//	dwLen11 = dwLen1; dwLen12 = dwLen2;
//	do {
//		WCHAR c1, c2;
//		dwLen1--; dwLen2--;
//		c1 = pDomain1[dwLen1]; c1 = LCASE(c1);
//		c2 = pDomain2[dwLen2]; c2 = LCASE(c2);
//		if (c1 != c2)
//			break;
//		if ('.' == c1)
//			level++;
//	} while(dwLen1 && dwLen2);
//	if (!dwLen1 && !dwLen2)
//		level++;
//	if (0 == level)
//	{
//		dwLen1 = dwLen11; dwLen2 = dwLen12;
		while (dwLen1 && pDomain1[dwLen1] != '.') 
			dwLen1--;
		while (dwLen2 && pDomain2[dwLen2] != '.') 
			dwLen2--;
		if (dwLen1 && dwLen2)
		{
			do {
				WCHAR c1, c2;
				dwLen1--; dwLen2--;
				c1 = pDomain1[dwLen1]; c1 = LCASE(c1);
				c2 = pDomain2[dwLen2]; c2 = LCASE(c2);
				if (c1 != c2)
					break;
				if ('.' == c1)
					level++;
			} while(dwLen1 && dwLen2);
			if (!dwLen1 && !dwLen2)
				level++;
		}
		if (level)
			level = -(level+1);
//	}
	ODS(("*** CompareDomains: %d", level));
	return level;
}

bool RemoveTag(WCHAR* pData, WCHAR* pDocUrl, WCHAR* pTag, WCHAR* pSrcAttr, bool bSrcAttrInTagOnly, bool* pbRemoved, bool bReqClosingTag = true, WCHAR* pParam = NULL, WCHAR* pExclude = NULL)
{
	WCHAR *pStart, *pEnd;
	WCHAR *pAttrValue = L"";
	DWORD dwAttrValueLen = 0;
	if (!pData)
		return false;
	size_t nTagLen = wcslen(pTag);
	while (true) {
		bool bSkip = false;
		do {
			pStart = wcschr(pData, '<');
			if (!pStart)
				return false;
			pData = pStart+1;
			if (0==_wcsnicmp(pData, pTag, nTagLen) && (pData[nTagLen] < 'A'))
				break;
		} while (pStart);
		if (bReqClosingTag)
		{
			while ((pEnd = wcschr(pData, '<')) && (pEnd[1] != '/' || 0!=_wcsnicmp(pEnd+2, pTag, nTagLen) || pEnd[2+nTagLen] != '>'))
				pData = pEnd+1;
			if (!pEnd)
				return true;
			pEnd += nTagLen+3;
		}
		else
		{
			pEnd = wcschr(pData, '>');
			if (!pEnd)
				return true;
			pEnd += 1;
		}
		pData = pEnd;
		WCHAR tmpch = *pEnd;
		if (!bSrcAttrInTagOnly)
		{
			pAttrValue = NULL;
			if (pParam)
			{
				pAttrValue = pStart;
				do {
					FindAttribute(pAttrValue, L"name", &pAttrValue, &dwAttrValueLen);
					if ( !pAttrValue)
						break;
					if ( 0 == _wcsnicmp(pAttrValue, pParam, dwAttrValueLen) )
					{
						FindAttribute(pAttrValue + dwAttrValueLen, L"value", &pAttrValue, &dwAttrValueLen);
						break;
					}
				} while( pAttrValue );
			}
			if (!pAttrValue)
				FindAttribute(pStart, pSrcAttr, &pAttrValue, &dwAttrValueLen);
		}
		*pEnd = 0;
		if (bSrcAttrInTagOnly)
			FindAttribute(pStart, pSrcAttr, &pAttrValue, &dwAttrValueLen);
		if (!dwAttrValueLen)
			bSkip = true;
		if (pExclude)
		{
			if (wcsistr(pStart, pExclude))
				bSkip=true;
		}
		if (!bSkip && dwAttrValueLen && pDocUrl)
		{
			int nCmpRes = CompareDomains(pDocUrl, (DWORD)wcslen(pDocUrl), pAttrValue, dwAttrValueLen);
			if (nCmpRes >= 2 || nCmpRes < 0)
			{
				bSkip = true;
				ODS(("*** SkipTag *** :: %S=%S", pSrcAttr, pAttrValue));
			}
		}
		if (!bSkip && dwAttrValueLen)
		{
			DWORD dwFlags = 0;
			WCHAR tmpch2;
			SPD_IsBannerData sIsBannerData;
			tmpch2 = pAttrValue[dwAttrValueLen];
			pAttrValue[dwAttrValueLen] = 0;
			ODS(("*** Req *** :: %S=%S", pSrcAttr, pAttrValue));
			memset(&sIsBannerData, 0, sizeof(sIsBannerData));
			sIsBannerData.bDocumentWrite = TRUE;
			sIsBannerData.pwchBannerUrl = pAttrValue;
			ScPluginsDispatch(SPD_IsWLBanner, &sIsBannerData, sizeof(sIsBannerData),  &dwFlags);
			bSkip = !(dwFlags & SCP_BLOCK);
#ifdef _DEBUG
//			bSkip = false;
#endif
			pAttrValue[dwAttrValueLen] = tmpch2;
		}
		if (!bSkip)
		{
#ifdef _DEBUG
			if (dwAttrValueLen)
			{
				WCHAR tmpch2 = pAttrValue[dwAttrValueLen];
				pAttrValue[dwAttrValueLen] = 0;
				ODS(("*** RemoveTag *** :: %S=%S", pSrcAttr, pAttrValue));
				pAttrValue[dwAttrValueLen] = tmpch2;
			}
			ODS(("*** RemoveTag *** :: %S", pStart));
			Sleep(1);
#endif
			while (pStart != pEnd)
				*pStart++ = ' ';
			if (pbRemoved)
				*pbRemoved = true;
		}
		*pEnd = tmpch;
	}
	return false;
}


void FlushDocumentWrite(SCRIPT_ENGINE_INSTANCE_DATA* pEngineData, bool bFilterTags)
{
	bool bRemoved = false;

	if (!pEngineData)
		return;
	if (!pEngineData->wcsScriptText)
		return;
	if (!pEngineData->pDelayedWriteDoc)
		return;

	if (bFilterTags)
	{
		//RemoveTag(pCheck, pEngineData->wcsURL, L"SCRIPT", L"src", true,  &bRemoved);
		RemoveTag(pEngineData->wcsScriptText, pEngineData->wcsURL, L"IMG",    L"src", true,  &bRemoved, false);
		RemoveTag(pEngineData->wcsScriptText, pEngineData->wcsURL, L"OBJECT", L"src", false, &bRemoved, true, L"movie");
		RemoveTag(pEngineData->wcsScriptText, pEngineData->wcsURL, L"IFRAME", L"src", true,  &bRemoved, true);
		//RemoveTag(pCheck, L"A", &bRemoved);
		//RemoveTag(pCheck, L"DIV", &bRemoved);
		if (bRemoved)
			pEngineData->bSupressErrorCode = 0x800A138F;
	}

	SAFEARRAYBOUND bound[1];
	bound[0].lLbound = 0;
	bound[0].cElements = 1;
	SAFEARRAY* pSafeArray = SafeArrayCreate(VT_VARIANT, 1, &bound[0]);
	if (pSafeArray)
	{
		HRESULT hRes = 0;
		long index = 0;
		VARIANT v;
		v.vt = VT_BSTR;
		v.bstrVal = SysAllocString(pEngineData->wcsScriptText);
		FreeEngineScriptStr(pEngineData);
		if (v.bstrVal)
		{
			if (SUCCEEDED(hRes = SafeArrayPutElement(pSafeArray, &index, &v)))
			{
				InterlockedIncrement(&pEngineData->bPassDocumentWrite);
				hRes = pEngineData->pDelayedWriteDoc->write(pSafeArray);
				InterlockedDecrement(&pEngineData->bPassDocumentWrite);
			}
			SysFreeString(v.bstrVal);
		}
		SafeArrayDestroy(pSafeArray);
	}
	pEngineData->pDelayedWriteDoc->Release();
	pEngineData->pDelayedWriteDoc = NULL;
	return;
}

BOOL HTMLDocument2_Write_CheckDelay(IHTMLDocument2* _this, SAFEARRAY* psarray, bool bLine, BSTR* ppBstr, SCRIPT_ENGINE_INSTANCE_DATA** ppEngineData)
{
	HRESULT hRes = S_OK;
	BOOL bBlockScriptedInjects = FALSE;
	UINT i;

	ODS(("*** CheckDelayWrite ***"));

	if (ppBstr)
		*ppBstr = NULL;

	if (ppEngineData)
		*ppEngineData = NULL;

	LPVOID pCurEngine = GetCurEngineInstanceTls();
	if (!pCurEngine)
		return FALSE;

	SCRIPT_ENGINE_INSTANCE_DATA* pEngineData = GetScriptEngineInstanceData(pCurEngine);
	
	if (!pEngineData)
		return FALSE;

	if (ppEngineData)
		*ppEngineData = pEngineData;

	if (!psarray)
		return FALSE;

//#ifndef _DEBUG
	bBlockScriptedInjects = IsGlobalMutexExist("KLObj_mt_KLBANNERCHECK_PR_DEF1689312");
//#else
//	bBlockScriptedInjects = TRUE;
//#endif

	if (!bBlockScriptedInjects)
		return FALSE;

	if (0 == psarray->cDims)
		return FALSE;

	if (0 == psarray->rgsabound[0].cElements)
		return FALSE;

	if (!(psarray->fFeatures & FADF_VARIANT))
		return FALSE;

	VARIANT* pvar = (VARIANT*)psarray->pvData;
	if (!pvar)
		return FALSE;
		
	// validate array
	for (i=0; i< psarray->rgsabound[0].cElements; i++)
	{
		if (pvar[i].vt != VT_BSTR)
			return FALSE;
		if (pvar[i].bstrVal == NULL)
			return FALSE;
		if (i == 0 && ppBstr)
			*ppBstr = pvar[i].bstrVal;
		if (wcsistr(pvar[i].bstrVal, L"<table") || 0==_wcsnicmp(pvar[i].bstrVal, L"<html>", 6))
		{
			InterlockedIncrement(&pEngineData->bPassDocumentWrite);
			return FALSE;
		}
	}

	bool bSkip = false;

	if (pEngineData->bPassDocumentWrite)
		return FALSE;

	if (pEngineData->wcsURL)
	{
		if (0 != _wcsnicmp(pEngineData->wcsURL, L"http", 4))
			return FALSE;

		BSTR pUrl = NULL;
		IHTMLDocument2Ptr doc2;
		if (SUCCEEDED(_this->QueryInterface(&doc2)) && (doc2 != NULL))
		{
			doc2->get_URL(&pUrl);
			if (pUrl)
			{
				if (0 != _wcsnicmp(pUrl, L"http", 4))
					bSkip = true;
				SysFreeString(pUrl);
			}
		}
		
	}
	
	if (bSkip)
		return FALSE;

	for (i=0; i< psarray->rgsabound[0].cElements; i++)
	{
		ODS(("*** DelayWrite *** : %S", pvar[i].bstrVal));
		AddEngineScriptStr(pCurEngine, pvar[i].bstrVal);
	}
	if (bLine)
		AddEngineScriptStr(pCurEngine, L"\r\n");
	
	if (NULL == pEngineData->pDelayedWriteDoc)
		_this->QueryInterface( IID_IHTMLDocument2, (void**) &pEngineData->pDelayedWriteDoc);
	
	WCHAR* wcsPtr = NULL;
	WCHAR* wcsLastPtr = pEngineData->wcsScriptText;
	do
	{
		if (wcsistr(wcsLastPtr, L"<script"))
		{
			wcsPtr = wcsistr(wcsLastPtr, L"</script>");
			if (wcsPtr)
				wcsPtr += 9; // wstrlen(L"</script>");
		}
		else
		{
			wcsPtr = wcsistr(wcsLastPtr, L"</");
			if (wcsPtr)
			{
				wcsPtr+=2;
				WCHAR c;
				do
				{
					wcsPtr++;
					c = *wcsPtr;
					c = LCASE(c);
				} while(c>='a' && c<='z');
				if (c != '>')
					wcsPtr = NULL;
				else
					wcsPtr++;
			}
			if (!wcsPtr)
			{
				wcsPtr = wcsistr(wcsLastPtr, L"/>");
				if (wcsPtr)
					wcsPtr+=2;
			}
		}
		if (wcsPtr)
			wcsLastPtr = wcsPtr;
	} while (wcsPtr && *wcsLastPtr);

	while (*wcsLastPtr == ' ')
		wcsLastPtr++;

	if (wcsLastPtr != pEngineData->wcsScriptText)
	{
		WCHAR* pLeft = NULL;
		if (*wcsLastPtr)
		{
			pLeft = SysAllocString(wcsLastPtr);
			*wcsLastPtr = 0;
		}
		FlushDocumentWrite(pEngineData, true);
		if (pLeft)
		{
			AddEngineScriptStr(pCurEngine, pLeft);
			SysFreeString(pLeft);
		}
	}

	if (pEngineData->wcsScriptText &&
		NULL == pEngineData->pDelayedWriteDoc)
		_this->QueryInterface( IID_IHTMLDocument2, (void**) &pEngineData->pDelayedWriteDoc);
	
	return TRUE;
}

HRESULT HTMLDocument2_WriteCommonHook(IHTMLDocument2* _this, SAFEARRAY* psarray, bool bLine)
{
	HRESULT hResult = S_OK;
	void* OrigFuncAddr = HookGetOrigFunc();
	SCRIPT_ENGINE_INSTANCE_DATA* pEngineData = NULL;
	BSTR bstr = NULL;
	if (HTMLDocument2_Write_CheckDelay(_this, psarray, bLine, &bstr, &pEngineData))
		return S_OK;
	
	FlushDocumentWrite(pEngineData, false);
			
	if (psarray)
		hResult = ((HRESULT (__stdcall *)(IHTMLDocument2*,SAFEARRAY*))OrigFuncAddr)(_this, psarray);

	ODS(("Res:%08X HTMLDocument2_write%s(%S)", hResult, bLine?"ln":"", bstr ? bstr : L"<NULL>"));
	//Sleep(5000);
	return hResult;
}

STDMETHODIMP HTMLDocument2_WriteHook(IHTMLDocument2* _this, SAFEARRAY* psarray)
{
	void* OrigFuncAddr = HookGetOrigFunc();
	return HTMLDocument2_WriteCommonHook(_this, psarray, false);
}

STDMETHODIMP HTMLDocument2_WriteLnHook(IHTMLDocument2* _this, SAFEARRAY* psarray)
{
	void* OrigFuncAddr = HookGetOrigFunc();
	return HTMLDocument2_WriteCommonHook(_this, psarray, true);
}

BSTR GetSourceURL(IActiveScriptSite* pActiveScriptSite)
{
	HRESULT hRes;
	BSTR pURL = NULL;
	
	if (!pActiveScriptSite)
		return NULL;

	try
	{
		IServiceProvider* sp = NULL;
		hRes = pActiveScriptSite->QueryInterface(IID_IServiceProvider, (void**)&sp);
		if (SUCCEEDED(hRes) && sp)
		{
			IServiceProvider* sp2 = NULL;
			hRes = _SP_QueryService(sp, SID_STopLevelBrowser, IID_IServiceProvider, (void **)(&sp2));
			if (SUCCEEDED(hRes) && sp2)
			{
				IWebBrowser2* pWebBrowser2 = NULL;
				hRes = _SP_QueryService(sp2, SID_SWebBrowserApp, IID_IWebBrowser2, (void **)(&pWebBrowser2));
				if (SUCCEEDED(hRes) && pWebBrowser2)
				{
					IDispatchPtr pDisp;
					{
						IConnectionPointContainer* cpcontainer = NULL;
						if (SUCCEEDED(pWebBrowser2->QueryInterface(IID_IConnectionPointContainer, (void**)&cpcontainer)) && NULL!=cpcontainer)
						{
							IConnectionPoint* cp = NULL;
							if (SUCCEEDED(cpcontainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &cp)) && NULL!=cp)
							{
								HRESULT hRes;
								DWORD dwCookie;
								if (!g_events) 
									g_events = new MyHTMLDocumentEvents(sp);
								hRes = cp->Advise(g_events, &dwCookie);
								// we re-advise multiple times on the same pWebBrowser2, so memory leak possible here
								ODS(("WebBrowser2 events Advise returned %08X, cookie=%08X", hRes, dwCookie));
								cp->Release();
								cp = NULL;
							}
							cpcontainer->Release();
							cpcontainer = NULL;
						}
					}
					if(SUCCEEDED(pWebBrowser2->get_Document(&pDisp)) && pDisp != NULL)
					{
						static bool bDocumentWriteHooked = false;
						pURL = GetNamedPropertyBStr(pDisp, L"URLUnencoded");
						if (!pURL || !bDocumentWriteHooked)
						{
							IHTMLDocument2Ptr pDoc2;
							if(SUCCEEDED(pDisp->QueryInterface(&pDoc2)) && pDoc2 != NULL)
							{
								if (!pURL)
									pDoc2->get_URL(&pURL);
								if (!bDocumentWriteHooked)
								{
									bDocumentWriteHooked = true;
									if (CheckAppFlags(APP_FLAG_IEXPLORER/*to be tested: APP_FLAG_BROWSER*/)) // don't hook if not IE
									{
										HookIFaceMethod(pDoc2, 59, HTMLDocument2_WriteHook,  NULL); // IHTMLDocument::write
										HookIFaceMethod(pDoc2, 60, HTMLDocument2_WriteLnHook,  NULL); // IHTMLDocument::writeln
									}
								}
							}
						}
					}
					pWebBrowser2->Release();
					pWebBrowser2=NULL;
				}

//		IHTMLDocument2* doc = NULL;
//								hRes = pHTMLWindow2->get_document(&doc);
//								if (SUCCEEDED(hRes) && NULL != doc)
//								{
//									IConnectionPointContainer* cpcontainer = NULL;
//									if (SUCCEEDED(doc->QueryInterface(IID_IConnectionPointContainer, (void**)&cpcontainer)) && NULL != cpcontainer)
//									{
//										IConnectionPoint* cp = NULL;
//										if (SUCCEEDED(cpcontainer->FindConnectionPoint(DIID_HTMLDocumentEvents, &cp)) && NULL!=cp)
//										{
//											DWORD dwCookie;
//											g_events = new MyHTMLDocumentEvents(sp);
//											cp->Advise(g_events, &dwCookie);
//											cp->Release();
//										}
//										cpcontainer->Release();
//									}
//									doc->Release();
//								}

				sp2->Release();
				sp2 = NULL;
			}
			else // not WebBrowser
			{
				IUnknown* Unkn = NULL;
				hRes = _SP_QueryService(sp, CLSID_HTMLWindow2, IID_IUnknown, (void **)(&Unkn));
				if (SUCCEEDED(hRes))
				{
					IHTMLWindow2Ptr pHTMLWindow2;
					hRes = Unkn->QueryInterface(IID_IHTMLWindow2, (void **)(&pHTMLWindow2));
					if (SUCCEEDED(hRes))
					{
						IHTMLDocument2Ptr HTMLDocument2;
						hRes = pHTMLWindow2->get_document(&HTMLDocument2);
						if (SUCCEEDED(hRes))
							HTMLDocument2->get_URL(&pURL);
						IDispatchPtr Disp;
						hRes = pHTMLWindow2->get_external(&Disp);
						if (SUCCEEDED(hRes))
						{
							IPersistMime* PersistMime;
							GUID IID_IPersistMime = {0xDE4AD8DA, 0x555F, 0x11D1, 0x8D, 0xD0, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xF9}; //{DE4AD8DA-555F-11D1-8DD0-00C04FB951F9}
							hRes = Disp->QueryInterface(IID_IPersistMime, (void **)(&PersistMime));
							if (SUCCEEDED(hRes))
							{
								void* IMimeMessageVtbl = NULL;
								IMimeMessageW* pIMimeMessage = NULL;
								pIMimeMessage = *(IMimeMessageW**)(((BYTE*)PersistMime) + 0x44);
								if (pIMimeMessage)
								{
									IMimeMessageW* pIMimeMessage2 = NULL;
									hRes = CoCreateInstance(CLSID_IMimeMessage, NULL, CLSCTX_SERVER, IID_IMimeMessageW, (void**)&pIMimeMessage2);
									if (pIMimeMessage2 && SUCCEEDED(hRes))
									{
										IMimeMessageVtbl = *(void**)pIMimeMessage2;
										pIMimeMessage2->Release();
										pIMimeMessage2 = NULL;
									}
									else
									{
										try 
										{
											if (*(void**)pIMimeMessage == IMimeMessageVtbl)
											{
												WCHAR _buff[0x1000];
												WCHAR* buff;
												HRESULT hResult;
												PROPVARIANT varPropValue;
												
												wcscpy(_buff, L"Outlook Express Message ");
												buff = _buff+wcslen(_buff);
												
												varPropValue.vt = VT_LPWSTR;
												varPropValue.bstrVal = NULL;
												hResult = pIMimeMessage->GetPropA((char*)"From", 0, (tagPROPVARIANT*)&varPropValue);
												if (SUCCEEDED(hResult))
												{
													if (*varPropValue.bstrVal)
													{
														wcscat(buff, L" From: ");
														wcscat(buff, varPropValue.bstrVal);
													}
													PropVariantClear(&varPropValue);
												}
												
												varPropValue.vt = VT_LPWSTR;
												varPropValue.bstrVal = NULL;
												hResult = pIMimeMessage->GetPropA((char*)"Date", 0, (tagPROPVARIANT*)&varPropValue);
												if (SUCCEEDED(hResult))
												{
													if (*varPropValue.bstrVal)
													{
														wcscat(buff, L" Date: ");
														wcscat(buff, varPropValue.bstrVal);
													}
													PropVariantClear(&varPropValue);
												}
												
												varPropValue.vt = VT_LPWSTR;
												varPropValue.bstrVal = NULL;
												hResult = pIMimeMessage->GetProp((char*)"Subject", 0, (tagPROPVARIANT*)&varPropValue);
												if (SUCCEEDED(hResult))
												{
													if (*varPropValue.bstrVal)
													{
														wcscat(buff, L" Subject: ");
														wcscat(buff, varPropValue.bstrVal);
													}
													PropVariantClear(&varPropValue);
												}
												
												if (buff[0])
												{
													SysFreeString(pURL);
													pURL = SysAllocString(_buff);
												}
											}
										}
										catch (...)
										{
											pIMimeMessage = NULL;
										}
									}
								}
								PersistMime->Release();
								PersistMime = NULL;
							}
						}

					}
					Unkn->Release();
					Unkn = NULL;
				}
			}
			sp->Release();	
			sp = NULL;
		}
		
		// try WScript.ScriptFullName
		if (!pURL)
		{
			IActiveScriptSite* pASS;
			GUID IID_IHost               = {0x91AFBD1B, 0x5FEB, 0x43F5,  0xB0, 0x28, 0xE2, 0xCA, 0x96, 0x06, 0x17, 0xEC};
			GUID SID_ScriptletContext    = {0x06290BE1, 0x48AA, 0x11D2, 0x84, 0x32, 0x00, 0x60, 0x08, 0xC3, 0xFB, 0xFC};
			GUID IID_IScriptletContext   = {0x06290BEA, 0x48AA, 0x11D2, 0x84, 0x32, 0x00, 0x60, 0x08, 0xC3, 0xFB, 0xFC};
			GUID IID_ComScriptletContext = {0x06290BF8, 0x48AA, 0x11D2, 0x84, 0x32, 0x00, 0x60, 0x08, 0xC3, 0xFB, 0xFC};
			GUID IID_IScriptletSiteWSH   = {0x06290C08, 0x48AA, 0x11D2, 0x84, 0x32, 0x00, 0x60, 0x08, 0xC3, 0xFB, 0xFC};
			GUID IID_IScriptletSite      = {0x06290BE8, 0x48AA, 0x11D2, 0x84, 0x32, 0x00, 0x60, 0x08, 0xC3, 0xFB, 0xFC};
			if (SUCCEEDED(pActiveScriptSite->QueryInterface(IID_IActiveScriptSite, (void**)&pASS)))
			{
				IServiceProvider* pSP;
				if (SUCCEEDED(pASS->QueryInterface(IID_IServiceProvider, (void**)&pSP)))
				{
					IScriptletContext* pSLC; 
					if(SUCCEEDED(_SP_QueryService(pSP, SID_ScriptletContext, IID_IScriptletContext, (void**)&pSLC)))
					{
						IScriptletSite* pScriptletSite;
						if(SUCCEEDED(pSLC->GetSite(IID_IScriptletSite, (void**)&pScriptletSite)))
						{
							VARIANT v;
							v.vt = VT_BSTR;
							v.bstrVal = L"WSH";
							VARIANT o;
							o.vt = VT_EMPTY;
							if(SUCCEEDED(pScriptletSite->GetInfo(2, NULL, &v, (IServiceProvider*)&o)) && o.vt == VT_DISPATCH)
							{
								IHost2* pHost = (IHost2*)o.bstrVal;
								// petrovitch 2006.07.10 - it's better to realloc BSTR by SysAllocString
								if(SUCCEEDED(pHost->get_ScriptFullName((unsigned short**)&pURL)))
								{
								
								}
								pHost->Release();
							}
							pScriptletSite->Release();
						}
						pSLC->Release();
					}
					pSP->Release();
				}
				pASS->Release();
			}
		}
		
		if (!pURL)
		{
			IUnknownPtr pUnk;
			if (SUCCEEDED(pActiveScriptSite->QueryInterface(IID_IUnknown, (void**)&pUnk)))
			{
				IDispatchPtr pDisp;
				if(SUCCEEDED(pUnk->QueryInterface(IID_IDispatch, (void**)&pDisp)))
					pURL = GetNamedPropertyBStr(pDisp, L"ScriptFullName");
			}
		}

// 		// TODO: maybe check for script engine state (init/execute)?
// 		SCRIPT_ENGINE_INSTANCE_DATA* pData = GetScriptEngineInstanceData(pActiveScriptSite);
// 		if (pData && pData->pActiveScript)
// 		{
// 			SCRIPTSTATE state=SCRIPTSTATE_UNINITIALIZED;
// 			// CSCRIPT is still UNINITIALIZED here
// 			ODS(("GetUrl: GetScriptState=>%08X state=%d", pData->pActiveScript->GetScriptState(&state), state));
// 			state=state;
// 		}
		
		// GetItemInfo call is unsafe, because ScriptSite initialization may not be completed, this leads to crash in ScriptSite.
		// IIS fail to load ASP if GetItemInfo called (and crashed)
		if (CheckAppFlags(APP_FLAG_USE_GETITEMINFO)) 
		{
			if (!pURL)
			{
				IUnknownPtr pUnk;
				if (SUCCEEDED(pActiveScriptSite->GetItemInfo(L"WScript", SCRIPTINFO_IUNKNOWN, &pUnk, NULL)))
				{
					IDispatchPtr pDisp;
					if(SUCCEEDED(pUnk->QueryInterface(IID_IDispatch, (void**)&pDisp)))
						pURL = GetNamedPropertyBStr(pDisp, L"ScriptFullName");
				}
			}
			
			// We ignore JSProxy calls now - see CheckDenyScript
// 			if (!pURL)
// 			{
// 				IUnknownPtr pUnk;
// 				if (SUCCEEDED(pActiveScriptSite->GetItemInfo(L"JSProxy", SCRIPTINFO_IUNKNOWN, &pUnk, NULL)))
// 				{
// 					pURL = SysAllocString(L"JSProxy");
// 				}
// 			}
		}

		/*
		// if document not found get file name from command line
		if ( !pURL )
		{
			int cargs = 0;
			wchar_t** cmd_line = CommandLineToArgvW(GetCommandLineW(), &cargs);
			if(cmd_line && cargs > 1)
				script_url = cmd_line[1];//! ָלÿ פאיכא vbs || js
			else
				script_url = L"<unknown document>";
		}
		*/
		
		/*
		hRes = ((IActiveScriptSite*)pScriptEngineInstanceData->pActiveScriptSite)->GetItemInfo(L"window", SCRIPTINFO_IUNKNOWN | SCRIPTINFO_ITYPEINFO, &pIUnknown, &pTypeInfo);
		if (SUCCEEDED(hRes))
		{
			WCHAR* wcsStatus = L"status";

			hRes = pIUnknown->QueryInterface(IID_IDispatch, (void**)&pWindowDispatch);
			if (SUCCEEDED(hRes))
			{
				hRes = pWindowDispatch->GetIDsOfNames(IID_NULL, &wcsStatus, 1, 0, &dispidStatus);
				if (SUCCEEDED(hRes))
				{
					dispParams.cArgs = 0;
					//hRes = pWindowDispatch->Invoke(dispidStatus, IID_NULL, 0, DISPATCH_PROPERTYGET, &dispParams, &varOldStatus, NULL, NULL);
					if (SUCCEEDED(hRes))
						bRestoreStatusBar = TRUE;
					dispParams.cArgs = 1;
					varStatus.vt = VT_BSTR;
					varStatus.bstrVal = L"Kaspersky Anti-Virus Script Checker analysing script...";
					dispParams.rgvarg = &varStatus;
					//hRes = pWindowDispatch->Invoke(dispidStatus, IID_NULL, 0, DISPATCH_PROPERTYPUT, &dispParams, &varResult, NULL, NULL);
					if (FAILED(hRes))
					{
						ODS(("SetStatusBar failed!!!"));
					}
				}
				else
				{
					pWindowDispatch = NULL;
				}
			}

			pIUnknown->Release();
			pTypeInfo->Release();
		}
		*/

		// replace" %20"->space
		if (pURL)
		{
			WCHAR* ptr;
			while (ptr = wcsstr(pURL, L"%20"))
			{
				size_t len = wcslen(ptr+3) + 1;
				ptr[0] = ' ';
				memmove(ptr+1, ptr+3, len*sizeof(WCHAR));
			}
		}

	}
	catch (...)
	{
		ODS(("Exception in GetSourceURL\n"));
	}
	return pURL;
}
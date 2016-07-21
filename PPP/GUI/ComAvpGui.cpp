// ComAvpGui.cpp: implementation of the CGUIControler class.
//
//////////////////////////////////////////////////////////////////////

#if defined (_WIN32)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0500
#endif
#define WINVER        0x0500
#define _WIN32_IE     0x0500

#define _ATL_STATIC_REGISTRY
#include <atlbase.h>

#include "StdAfx.h"
#include "comgui.h"

CComModule _Module;	// needed for ATL headers

#include <atlcom.h>
#include <commctrl.h>
#include "ComAvpGui.h"

#include "avpgui_i.c"

//////////////////////////////////////////////////////////////////////

static void GuiGetProductStatus(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	tDWORD nStatus = 0;
	
	{
		CProfile * pProfile = g_pProduct;
		if( !pProfile || !(pProfile->m_nState & STATE_FLAG_OPERATIONAL) )
			nStatus |= COMGUI_PRODUCTSTATUS_PROTECTIONNOTRUNNING;
	}
	
	if( g_hGui->m_statistics && !!g_hGui->m_statistics->m_State.m_KeyState )
		nStatus |= COMGUI_PRODUCTSTATUS_ACTIVATENEED;

	pEvt->m_nRes = (tPTR)nStatus;
}

//////////////////////////////////////////////////////////////////////
// CGUIItem

#define LIBID_GUICONTROLLERLib	LIBID_GUIATLLib

#include "ComBaseGui.cpp"

//////////////////////////////////////////////////////////////////////
// CGUIController

class ATL_NO_VTABLE CGUIController :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGUIController, &CLSID_CGUIController>,
	public IDispatchImpl<IGUIController, &IID_IGUIController, &LIBID_GUICONTROLLERLib>,
	public CGUIItemId
{
public:
	CGUIController()
	{
		_CCGUIItemPtr pRoot = GUIControllerGetRoot(L"");
		Attach(pRoot, pRoot);
	}
	
	~CGUIController(){}

	DECLARE_REGISTRY_RESOURCEID(IDR_GUICONTROLLER)
	DECLARE_NOT_AGGREGATABLE(CGUIController)
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CGUIController)
		COM_INTERFACE_ENTRY(IGUIController)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public: // IGUIController
	STDMETHOD(GetItem)(BSTR strName, VARIANT_BOOL bExact, IDispatch **pItem);
	STDMETHOD(GetLastBalloonText)(BSTR *pVal);
	STDMETHOD(DoCommand)(BSTR pVal);
	STDMETHOD(GetProductStatus)(unsigned long *pStatus);
};

//////////////////////////////////////////////////////////////////////

STDMETHODIMP CGUIController::GetItem(BSTR strName, VARIANT_BOOL bExact, IDispatch **pItem)
{
	USES_CONVERSION;
	_CCGUIItemPtr pItemBase = ItemGet(strName, !!bExact);
	if( !pItemBase )
		return E_FAIL;

	CComGUIItem * pGuiItem = NULL;
	HRESULT hResult = CComGUIItem::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;

	pGuiItem->Attach(m_pRoot, pItemBase, pItemBase->m_strItemId.c_str(), cCP_ANSI);
	*pItem = pGuiItem;
	return S_OK;
}

STDMETHODIMP CGUIController::GetLastBalloonText(BSTR *pVal)
{
	USES_CONVERSION;
	*pVal = SysAllocString(L"");
	return S_OK;
}

STDMETHODIMP CGUIController::DoCommand(BSTR pVal)
{
	PostItemOpWaitGui(GuiRootDoCommand, 0, 0, 0, pVal);
	return S_OK;
}

STDMETHODIMP CGUIController::GetProductStatus(unsigned long *pStatus)
{
	*pStatus = (unsigned long)CallItemOp(GuiGetProductStatus);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// COM export routines

STDAPI DllCanUnloadNow(void)                                        { return (_Module.GetLockCount()==0) ? S_OK : S_FALSE; }
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) { return _Module.GetClassObject(rclsid, riid, ppv); }
STDAPI DllRegisterServer(void)                                      { return _Module.RegisterServer(TRUE); }
STDAPI DllUnregisterServer(void)                                    { return _Module.UnregisterServer(TRUE); }

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_CGUIController, CGUIController)
END_OBJECT_MAP()

bool GUIControllerInit(tPTR hInstance)
{
	if( !_GUIController_RootsListInit(hInstance) )
		return false;
	return SUCCEEDED(_Module.Init(ObjectMap, (HINSTANCE)hInstance, &LIBID_GUICONTROLLERLib));
}

void GUIControllerDone()
{
	_Module.Term();
	_GUIController_RootsListDone();
}

/////////////////////////////////////////////////////////////////////////////

bool GUIControllerRegister()
{
	if( !_GUIController_Register() )
		return false;
	::CoInitialize(NULL);
	return SUCCEEDED(_Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE));
}

void GUIControllerUnregister()
{
	__try
	{
		_Module.RevokeClassObjects();
		::CoUninitialize();
		_GUIController_Unegister();
	}
	__except(1){}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#endif // _WIN32

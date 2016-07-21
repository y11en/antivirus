// ComAvpGui.cpp: implementation of the CGUIControler class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0500
#endif
#define WINVER        0x0500
#define _WIN32_IE     0x0500

#define _ATL_STATIC_REGISTRY
#include <atlbase.h>

#include "stdafx.h"
#include "comgui.h"

CComModule _Module;	// needed for ATL headers

#include <atlcom.h>
#include <commctrl.h>
#include "ComAdminGui.h"

#include "admingui_i.c"

//////////////////////////////////////////////////////////////////////
// CGUIItem

#define LIBID_GUICONTROLLERLib	LIBID_AdminGUIATLLib

#include "ComBaseGui.cpp"

//////////////////////////////////////////////////////////////////////
// CAdminGUIController

class ATL_NO_VTABLE CAdminGUIController :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CAdminGUIController, &CLSID_CAdminGUIController>,
	public IDispatchImpl<IAdminGUIController, &IID_IAdminGUIController, &LIBID_GUICONTROLLERLib>,
	public CGUIItemId
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_GUICONTROLLER)
	DECLARE_NOT_AGGREGATABLE(CAdminGUIController)
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAdminGUIController)
		COM_INTERFACE_ENTRY(IAdminGUIController)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public: // IAdminGUIController
	STDMETHOD(GetObject)(BSTR strName, IDispatch ** pPage);
};

//////////////////////////////////////////////////////////////////////

STDMETHODIMP CAdminGUIController::GetObject(BSTR strName, IDispatch **pPage)
{
	PR_TRACE((g_root, prtIMPORTANT, "AdminGUIController::GetObject(%S) called", strName));

	_CCGUIItemPtr pRoot = GUIControllerGetRoot(strName);
	if( !pRoot )
		return E_FAIL;
	
	CGUIItem * pGuiItem = NULL;
	HRESULT hResult = CGUIItem::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;

	pGuiItem->Attach(pRoot, pRoot);
	*pPage = pGuiItem;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// COM export routines

STDAPI DllCanUnloadNow(void)                                        { return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE; }
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) { return _Module.GetClassObject(rclsid, riid, ppv); }
STDAPI DllRegisterServer(void)                                      { return _Module.RegisterServer(TRUE); }
STDAPI DllUnregisterServer(void)                                    { return _Module.UnregisterServer(TRUE); }

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_CAdminGUIController, CAdminGUIController)
END_OBJECT_MAP()

bool GUIControllerInit(tPTR hInstance)
{
	if( FAILED(_Module.Init(ObjectMap, (HINSTANCE)hInstance, &LIBID_GUICONTROLLERLib)) )
		return false;
	return _GUIController_RootsListInit(hInstance);
}

void GUIControllerDone()
{
	_GUIController_RootsListDone();
	_Module.Term();
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
	_Module.RevokeClassObjects();
	::CoUninitialize();
	_GUIController_Unegister();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

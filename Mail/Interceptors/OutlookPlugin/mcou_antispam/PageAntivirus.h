
////////////////////////////////////////////////////////////////////////////
//	Copyright : Amit Dey 2002
//
//	Email :amitdey@joymail.com
// PageAntivirus.h : Declaration of the CPageAntivirus

#pragma once

#include "resource.h"       // main symbols
//#import "dll\MSOUTL9.OLB" raw_interfaces_only
#include "GuiLoader.h"

/////////////////////////////////////////////////////////////////////////////
// CPageAntivirus

class ATL_NO_VTABLE CPageAntivirus : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IPageAntivirus, &IID_IPageAntivirus, &LIBID_OUTLOOKADDINLib>,
	public CComControlBase,
	public CWindow,
	public IPersistStreamInitImpl<CPageAntivirus>,
	public IOleControlImpl<CPageAntivirus>,
	public IOleObjectImpl<CPageAntivirus>,
	public IOleInPlaceActiveObjectImpl<CPageAntivirus>,
	public IViewObjectExImpl<CPageAntivirus>,
	public IOleInPlaceObjectWindowlessImpl<CPageAntivirus>,
	public IPersistStorageImpl<CPageAntivirus>,
	public ISpecifyPropertyPagesImpl<CPageAntivirus>,
	public IQuickActivateImpl<CPageAntivirus>,
	public IDataObjectImpl<CPageAntivirus>,
	public IProvideClassInfo2Impl<&CLSID_PageAntivirus, NULL, &LIBID_OUTLOOKADDINLib>,
	public CComCoClass<CPageAntivirus, &CLSID_PageAntivirus>,
	public IPersistPropertyBagImpl<CPageAntivirus>,
    public IDispatchImpl<PropertyPage,&__uuidof(PropertyPage),&LIBID_OUTLOOKADDINLib>,
	public CDialogBindingT<cAntispamPluginStatus>
{
public:

	typedef CDialogBindingT<cAntispamPluginStatus> TBaseDlg;

	CPageAntivirus() : CComControlBase(m_hWnd), TBaseDlg(&m_Status) {}

protected:
	virtual HRESULT ControlQueryInterface(const IID& iid, void** ppv);
	virtual HWND CreateControlWindow(HWND hWndParent, RECT& rcPos);
	virtual HWND OnGetParent();
	virtual void OnInit();
	virtual void OnClicked(CItemBase* pItem);
//	virtual void OnChangedData(CItemBase* pItem);
	virtual void OnTimerRefresh(tDWORD nTimerSpin);
	void         GetStatus(cAntispamPluginStatus &Status);
	void         SetDirty(bool bDirty = true);

	HWND                      m_hWndParent;
	cAntispamPluginStatus     m_Status;

public:
DECLARE_REGISTRY_RESOURCEID(IDR_PAGE_ANTIVIRUS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPageAntivirus)
	COM_INTERFACE_ENTRY(IPageAntivirus)
	COM_INTERFACE_ENTRY2(IDispatch,IPageAntivirus)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IPersistPropertyBag) 
	COM_INTERFACE_ENTRY(Outlook::PropertyPage) 
END_COM_MAP()

BEGIN_PROP_MAP(CPageAntivirus)
END_PROP_MAP()

BEGIN_MSG_MAP(CPageAntivirus)
END_MSG_MAP()

BEGIN_SINK_MAP(CPageAntivirus)
END_SINK_MAP()

	STDMETHOD(GetControlInfo)(LPCONTROLINFO pCI);
	STDMETHOD(SetClientSite)(IOleClientSite *pSite);
	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid);

// IViewObjectEx
DECLARE_VIEW_STATUS(0)

// IPageAntivirus
public:
	// PropertyPage
	STDMETHOD(GetPageInfo)(BSTR * HelpFile, LONG * HelpContext);
	STDMETHOD(get_Dirty)(VARIANT_BOOL * Dirty);
	STDMETHOD(Apply)();

private:
	CComVariant                        m_fDirty;
	CComPtr<Outlook::PropertyPageSite> m_pPropPageSite; 
};
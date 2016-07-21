
////////////////////////////////////////////////////////////////////////////
//	Copyright : Amit Dey 2002
//
//	Email :amitdey@joymail.com
// PageAntispam.h : Declaration of the CPageAntispam

#pragma once

#include "resource.h"       // main symbols
#import "dll\MSOUTL9.OLB" raw_interfaces_only

#include <ProductCore/GuiLoader.h>

/////////////////////////////////////////////////////////////////////////////
// CPageAntispamSink

class CPageAntispamSink : public CDialogBindingViewT<>
{
public:
	typedef CDialogBindingViewT<> TBaseDlg;

	CPageAntispamSink(HWND hWndParent, bool bAskOnCancel = false, bool bAdvView = true) : 
	  m_hWndParent(hWndParent), 
	  m_bAskOnCancel(bAskOnCancel),
	  m_bAdvView(bAdvView),
	  TBaseDlg(&m_Settings)
	  {}

protected:
	void OnCreate();
	virtual void OnInit();
	virtual bool OnOk();
	virtual bool OnCanClose(bool bUser);
	virtual bool OnClicked(CItemBase* pItem);
	virtual void OnChangedData(CItemBase* pItem);
	virtual void OnTimerRefresh(tDWORD nTimerSpin);
	virtual void SetDirty(bool bDirty = true) {}
	virtual HWND OnGetParent();
	void         GetStatus(cAntispamPluginStatus &Status);
	bool         ValidateRule(AntispamPluginRule_t &Rule);
	bool         SaveSettings();

	HWND                      m_hWndParent;
	CAntispamOUSupportClass * m_pAS;
	AntispamPluginSettings_t  m_Settings;
	cAntispamPluginStatus     m_Status;
	bool                      m_bAskOnCancel;
	bool                      m_bAdvView;
};

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

class CFolderTree : public CTreeItemSink
{
public:
	CFolderTree(CAntispamOUSupportClass * pAS) : m_pAS(pAS) {}

protected:
	struct TVIDATA : public cTreeItem
	{
		TVIDATA(cStrObj& strName, tUINT nFolder, tUINT nChildren)
		{
			m_flags = UPDATE_FLAG_TEXT|UPDATE_FLAG_SUBCOUNT;
			m_strName = strName;
			m_nFolder = nFolder;
			m_nChildren = nChildren ? 1 : 0;
		}
		tUINT m_nFolder;
	};

public:
	bool GetFolder(FolderInfo_t& FolderInfo);

protected:
	void OnInited();
	bool OnItemExpanding(cTreeItem * pItem, tUINT& nChildren);

	bool ExpandItem(cTreeItem * pItem);

	CAntispamOUSupportClass * m_pAS;
	FolderInfoList_t          m_aFolders;
};

/////////////////////////////////////////////////////////////////////////////
// CBrowseForFolder

class CBrowseForFolder : public CDialogBindingT<>
{
public:
	typedef CDialogSink TBase;
	CBrowseForFolder(CAntispamOUSupportClass *pAS, FolderInfo_t *pFldInfo) : m_Tree(pAS), m_pFldInfo(pFldInfo) {}

	SINK_MAP_BEGIN()
		SINK_STATIC("FolderTree", m_Tree)
	SINK_MAP_END(CDialogSink)

protected:
	virtual bool OnOk();
	
	FolderInfo_t *            m_pFldInfo;
	CFolderTree               m_Tree;
};


/////////////////////////////////////////////////////////////////////////////
// CPageAntispam

class ATL_NO_VTABLE CPageAntispam : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IPageAntispam, &IID_IPageAntispam, &LIBID_OUTLOOKADDINLib>,
	public CComControlBase,
	public CWindowImpl<CPageAntispam>,
	public IPersistStreamInitImpl<CPageAntispam>,
	public IOleControlImpl<CPageAntispam>,
	public IOleObjectImpl<CPageAntispam>,
	public IOleInPlaceActiveObjectImpl<CPageAntispam>,
	public IViewObjectExImpl<CPageAntispam>,
	public IOleInPlaceObjectWindowlessImpl<CPageAntispam>,
	public IPersistStorageImpl<CPageAntispam>,
	public ISpecifyPropertyPagesImpl<CPageAntispam>,
	public IQuickActivateImpl<CPageAntispam>,
	public IDataObjectImpl<CPageAntispam>,
	public IProvideClassInfo2Impl<&CLSID_PageAntispam, NULL, &LIBID_OUTLOOKADDINLib>,
	public CComCoClass<CPageAntispam, &CLSID_PageAntispam>,
	public IPersistPropertyBagImpl<CPageAntispam>,
    public IDispatchImpl<PropertyPage,&__uuidof(PropertyPage),&LIBID_OUTLOOKADDINLib>,
	public CPageAntispamSink
{
public:
	CPageAntispam() : CPageAntispamSink(NULL), CComControlBase(m_hWnd) {}

	HRESULT IOleInPlaceObject_SetObjectRects(LPCRECT prcPos,LPCRECT prcClip)
	{
		if( !prcPos )
			return E_POINTER;

		RECT rcNew = *prcPos;
		if( m_hWnd )
		{
			RECT rc; ::GetClientRect(m_hWnd, &rc);
			rcNew.bottom = rcNew.top + RECT_CY(rc);
			rcNew.right = rcNew.top + RECT_CX(rc);
		}
		
		return CComControlBase::IOleInPlaceObject_SetObjectRects(&rcNew, &rcNew);
	}
	
protected:
	virtual HRESULT ControlQueryInterface(const IID& iid, void** ppv);
	virtual HWND CreateControlWindow(HWND hWndParent, RECT& rcPos);
	virtual void SetDirty(bool bDirty = true);

public:
DECLARE_REGISTRY_RESOURCEID(IDR_PAGE_ANTISPAM)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPageAntispam)
	COM_INTERFACE_ENTRY(IPageAntispam)
	COM_INTERFACE_ENTRY2(IDispatch,IPageAntispam)
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

BEGIN_PROP_MAP(CPageAntispam)
END_PROP_MAP()

BEGIN_MSG_MAP(CPageAntispam)
END_MSG_MAP()

BEGIN_SINK_MAP(CPageAntispam)
END_SINK_MAP()

	STDMETHOD(GetControlInfo)(LPCONTROLINFO pCI);
	STDMETHOD(SetClientSite)(IOleClientSite *pSite);
	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid);

// IViewObjectEx
DECLARE_VIEW_STATUS(0)

// IPageAntispam
public:
	// PropertyPage
	STDMETHOD(GetPageInfo)(BSTR * HelpFile, LONG * HelpContext);
	STDMETHOD(get_Dirty)(VARIANT_BOOL * Dirty);
	STDMETHOD(Apply)();

private:
	CComVariant                        m_fDirty;
	CComPtr<Outlook::PropertyPageSite> m_pPropPageSite; 
};
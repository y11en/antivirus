// CombaseGui.cpp: implementation of the CGUIItem class.
//
//////////////////////////////////////////////////////////////////////
#if defined (_WIN32)
#include <Guiloader.h>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

enum eGUIItemType {eStd, eListIter, eListCell, eListCol};

MIDL_INTERFACE("8BD2F7A6-842B-48E6-8BDC-B30F0A582660")
IGUIItemType : public IUnknown
{
	virtual eGUIItemType STDMETHODCALLTYPE GetInternalType() { return eStd; }
};

bool _Check_Obj(IDispatch *pItem, eGUIItemType eType)
{
	if( !pItem )
		return false;
	
	IGUIItemType * pType = NULL;
	if( FAILED(pItem->QueryInterface(__uuidof(IGUIItemType), (void **)&pType)) )
		return false;

	bool ok = pType->GetInternalType() == eType;
	pType->Release();
	return ok;
}

void _Simulate_MouseClick(HWND hWnd, int x, int y, int nMode = 0, bool bSync = true)
{
	LPARAM lParam = MAKELONG(x, y);
	
	if( bSync )
	{
		::SendMessage(hWnd, nMode ? WM_RBUTTONDOWN : WM_LBUTTONDOWN, 0, lParam);
		::SendMessage(hWnd, nMode ? WM_RBUTTONUP : WM_LBUTTONUP, 0, lParam);
	}
	else
	{
		::PostMessage(hWnd, nMode ? WM_RBUTTONDOWN : WM_LBUTTONDOWN, 0, lParam);
		::PostMessage(hWnd, nMode ? WM_RBUTTONUP : WM_LBUTTONUP, 0, lParam);
	}
}

//////////////////////////////////////////////////////////////////////
// Funcs

#define CHK_CTL_ACTIVE(_pItem)	!(_pItem->m_nState & (ISTATE_DISABLED/*|ISTATE_HIDE*/))

static void GuiItemGet(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	if( pEvt->m_strVal1.size() )
		pItem = pItem->GetItemByPath(pEvt->m_strVal1.c_str(cCP_ANSI), false, !!pEvt->m_nVal1);
	
	if( pItem )
	{
		pItem->AddRef();
		pEvt->m_nRes = pItem;
	}
}

static void GuiItemGetByPos(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	if( pItem = pItem->GetByPos((tSIZE_T)pEvt->m_nVal1) )
	{
		pItem->AddRef();
		pEvt->m_nRes = pItem;
	}
}

static void GuiItemGetParent(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	pItem = pItem->m_pParent;
	if( pItem )
	{
		pItem->AddRef();
		pEvt->m_nRes = pItem;
	}
}

static void _GuiItem_GetImage(bool bBacknd, CRootItem * pRoot, CItemBase * pItem, cStrObj& strId)
{
	if( !pItem )
		return;

	CImageList_ * pIList = NULL;
	if( bBacknd )
	{
		if( pItem->m_pBackground )
		{
			strId = pRoot->GetBackgroundKey((CBackground *)(CImageBase *)pItem->m_pBackground);
			pIList = &pItem->m_pBackground;
		}
	}
	else if( pItem->m_pIcon )
	{
		strId = pRoot->GetIconKey((CIcon *)(CImageBase *)pItem->m_pIcon);
		pIList = &pItem->m_pBackground;
	}

	if( pIList && pIList->IsIndexed() )
	{
		tCHAR c[50];
		for(tDWORD i = 0; i < IMAGELIST_SIZE; i++)
		{
			_snprintf(c, countof(c), ",%u", pIList->GetImage(i));
			strId.append(c, cCP_ANSI);
		}
	}
}

static void GuiItemGetImage(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	_GuiItem_GetImage(!!pEvt->m_nVal1, pRoot, pItem, pEvt->m_strVal1);
}

static void GuiItemSetStr(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;
	
	if( CHK_CTL_ACTIVE(pItem) && pItem->GetType() == tid_STRING_OBJ )
	{
		pItem->SetValue(cVariant(pEvt->m_strVal1.data()));
		pItem->SetChangedData();
	}
}

static void GuiItemGetCount(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	if( pItem->m_strItemType == GUI_ITEMT_LISTCTRL )
		pEvt->m_nRes = 0;
	else
		pEvt->m_nRes = (tPTR)pItem->GetChildrenCount();
}

static void GuiItemGetInt(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cVariant data;
	pItem->GetValue(data);
	pEvt->m_nVal1 = (tPTR)data.ToQWORD();
}

static void GuiItemClick(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !CHK_CTL_ACTIVE(pItem) )
		return;

	CPopupMenuItem * pMenu = NULL;
	{
		for(CItemBase * p = pItem; p; p = p->m_pParent)
			if( p->m_strItemType == GUI_ITEMT_MENU )
			{
				pMenu = (CPopupMenuItem *)p;
				break;
			}
	}

	switch( (long)pEvt->m_nVal1 )
	{
	case 0:
	{
		if( pMenu )
			pMenu->TrackEnd(pItem);
		else
			pItem->Click();
	} break;

	case 1:
	{
		if( !pMenu )
		{
			RECT rc; pItem->GetParentRect(rc, pItem->m_pRoot);
			_Simulate_MouseClick(pItem->GetOwner()->GetWindow(), rc.left, rc.top, 1);
		}
	} break;
	}
}

static void GuiItemDestroy(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)          { if( pItem ) pItem->Destroy(); }
static void GuiItemSelect(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)           { if( CHK_CTL_ACTIVE(pItem) ) pItem->Select(); }
static void GuiItemSetInt(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)           { if( CHK_CTL_ACTIVE(pItem) ) pItem->SetInt((tDWORD)pEvt->m_nVal1, true); }
static void GuiItemGetText(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)          { pRoot->LocalizeStr(pEvt->m_strVal1, pItem->GetDisplayText()); }
static void GuiItemGetId(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)            { pRoot->LocalizeStr(pEvt->m_strVal1, pItem->m_strItemId.c_str()); }
static void GuiItemGetType(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)          { pRoot->LocalizeStr(pEvt->m_strVal1, pItem->m_strItemType.c_str()); }
static void GuiItemGetPos(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)           { pEvt->m_nRes = (tPTR)pItem->m_nPos; }
static void GuiItemIsChecked(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)        { pEvt->m_nRes = (tPTR)pItem->IsChecked(); }
static void GuiItemGetRect(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)          { pItem->GetParentRect(*(RECT *)pEvt->m_nVal1); }
static void GuiRootDoCommand(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)        { pRoot->DoCommand(pItem == pRoot ? NULL : pItem, pEvt->m_strVal1.c_str(cCP_ANSI), true); }

static void GuiListIter_Count(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt);
static void GuiList_GetCols(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt);
static void GuiListIter_Next(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt);

//////////////////////////////////////////////////////////////////////
// CGUIItemId

#define _ITEM_ADDREF(_pItem)  if( _pItem ) (_pItem)->AddRef();
#define _ITEM_RELEASE(_pItem) if( _pItem ) (_pItem)->Release();

class _CCGUIItemPtr
{
public:
	_CCGUIItemPtr(CItemBase * p = NULL) : m_p(p) {}
	~_CCGUIItemPtr() { _ITEM_RELEASE( m_p ); }
	
	CItemBase * operator->() { return m_p; }
	operator CItemBase * ()  { return m_p; }
	_CCGUIItemPtr& operator = (CItemBase * p)  { m_p = p; return *this; }

	CItemBase * m_p;
};

class CGUIItemId
{
public:
	CGUIItemId(CItemBase * pRoot = NULL, CItemBase * pItem = NULL, const tVOID * pName = NULL, tCODEPAGE cp = cCP_UNICODE)
	{
		Attach(pRoot, pItem, pName, cp);
	}

	void Attach(CItemBase * pRoot, CItemBase * pItem, const tVOID * pName = NULL, tCODEPAGE cp = cCP_UNICODE)
	{
		m_pRoot = pRoot; _ITEM_ADDREF(m_pRoot);
		m_pItem = pItem; _ITEM_ADDREF(m_pItem);
		if( pName ) m_strId.assign(pName, cp);
	}
	
	void CallItemOpEx(cGuiEvent * pEvt, tBOOL bSync = cTRUE)
	{
		if( !m_pRoot )
			return;
		
		pEvt->m_pItem     = m_pItem;
		pEvt->m_strItemId = m_strId;
		pEvt->m_bSync     = bSync;

		if( !bSync )
			_ITEM_ADDREF(m_pItem);
		
		if( ((CRootItem *)(CItemBase *)m_pRoot)->SendEvent(cGuiEvent::eIID, pEvt, EVENT_SEND_OWN_QUEUE|(bSync ? EVENT_SEND_SYNC : 0)) == errOBJECT_BAD_INTERNAL_STATE )
			if( !bSync )
				_ITEM_RELEASE(m_pItem);
	}

	void PostItemOpEx(cGuiEvent::OpFunc * pFunc, tPTR nVal1 = 0, tPTR nVal2 = 0, tPTR nVal3 = 0, LPCWSTR strVal1 = NULL, bool bWaitForGuiLoop = false)
	{
		cSerObj<cGuiEvent> pEvt; pEvt.init(cGuiEvent::eIID);
		pEvt->m_pFunc = pFunc;
		pEvt->m_nVal1 = nVal1;
		pEvt->m_nVal2 = nVal2;
		pEvt->m_nVal3 = nVal3;
		pEvt->m_strVal1 = strVal1;
		PostItemOpEx2(pEvt, bWaitForGuiLoop);
	}

	void PostItemOpEx2(cGuiEvent * pEvt, bool bWaitForGuiLoop = false)
	{
		CallItemOpEx(pEvt, cFALSE);
		
		if( bWaitForGuiLoop )
		{
			cSerObj<cGuiEvent> pEvtSync; pEvtSync.init(cGuiEvent::eIID);
			CallItemOpEx(pEvtSync, cTRUE);
		}
	}

	void PostItemOp(cGuiEvent::OpFunc * pFunc, tPTR nVal1 = 0, tPTR nVal2 = 0, tPTR nVal3 = 0, LPCWSTR strVal1 = NULL)
	{
		PostItemOpEx(pFunc, nVal1, nVal2, nVal3, strVal1);
	}

	void PostItemOpWaitGui(cGuiEvent::OpFunc * pFunc, tPTR nVal1 = 0, tPTR nVal2 = 0, tPTR nVal3 = 0, LPCWSTR strVal1 = NULL)
	{
		PostItemOpEx(pFunc, nVal1, nVal2, nVal3, strVal1, true);
	}

	tPTR CallItemOp(cGuiEvent::OpFunc * pFunc, tPTR nVal1 = 0, tPTR nVal2 = 0, tPTR nVal3 = 0, LPCWSTR strVal1 = NULL)
	{
		cGuiEvent pEvt;
		pEvt.m_pFunc = pFunc;
		pEvt.m_nVal1 = nVal1;
		pEvt.m_nVal2 = nVal2;
		pEvt.m_nVal3 = nVal3;
		pEvt.m_strVal1 = strVal1;
		CallItemOpEx(&pEvt, cTRUE);
		return pEvt.m_nRes;
	}

	CItemBase * ItemGet(LPCWSTR strItemId = NULL, bool bExact = false) { return (CItemBase *)CallItemOp(GuiItemGet, (tPTR)bExact, 0, 0, strItemId); }
	CItemBase * ItemGetByPos(long nPos)           { return (CItemBase *)CallItemOp(GuiItemGetByPos, (tPTR)nPos); }

	_CCGUIItemPtr   m_pRoot;
	_CCGUIItemPtr   m_pItem;
	cStrObj         m_strId;
};

//////////////////////////////////////////////////////////////////////
// CGUIItem

class ATL_NO_VTABLE CGUIItem : 
	public CGUIItemId,
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<ICGUIItem, &IID_ICGUIItem, &LIBID_GUICONTROLLERLib>,
	public IGUIItemType
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CGUIItem)
		COM_INTERFACE_ENTRY(ICGUIItem)
		COM_INTERFACE_ENTRY(IGUIItemType)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public: // ICGUIItem
	STDMETHOD(get_X)(long *pVal);
	STDMETHOD(get_Y)(long *pVal);
	STDMETHOD(get_Width)(long *pVal);
	STDMETHOD(get_Height)(long *pVal);
	STDMETHOD(get_Text)(BSTR *pVal);
	STDMETHOD(get_Icon)(BSTR *pVal);
	STDMETHOD(get_Background)(BSTR *pVal);
	STDMETHOD(get_Enable)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Check)(long *pVal);
	STDMETHOD(get_Hide)(VARIANT_BOOL *pVal);
	STDMETHOD(get_ValueInteger)(long *pVal);
	STDMETHOD(get_Id)(BSTR * pVal);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(get_Pos)(long * pVal);
	STDMETHOD(get_Selected)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Children)(IDispatch ** pChildren);

	STDMETHOD(GetItem)(BSTR vId, VARIANT_BOOL bExact, IDispatch **pItem);
	STDMETHOD(GetItemByIdx)(long vId, IDispatch **pItem);
	STDMETHOD(GetParent)(IDispatch **pItem);
	STDMETHOD(GetCount)(long *pCount);
	STDMETHOD(Click)(long nMode);
	STDMETHOD(Select)(VARIANT_BOOL bSelect);
	STDMETHOD(SetValueInteger)(long newVal);
	STDMETHOD(SetValueString)(BSTR newVal);

	STDMETHOD(get_ToolTip)(BSTR *pVal)						{ *pVal = ::SysAllocString(L""); return S_OK; }
	STDMETHOD(SetCheck)(long newVal)						{ return SetValueInteger(newVal != FALSE); }
	STDMETHOD(DoCommand)(BSTR pVal)							{ PostItemOpWaitGui(GuiRootDoCommand, 0, 0, 0, pVal); return S_OK; }

	STDMETHOD(get_HasChildren)(VARIANT_BOOL *pVal)			{ return E_NOTIMPL; }
	STDMETHOD(Next)(VARIANT_BOOL *pVal)						{ return E_NOTIMPL; }
	STDMETHOD(Cell)(ICGUIItem *pCol, ICGUIItem **pCell)		{ return E_NOTIMPL; }
	STDMETHOD(get_Expanded)(VARIANT_BOOL *pVal)				{ return E_NOTIMPL; }
    STDMETHOD(Expand)(VARIANT_BOOL bExpand = TRUE)			{ return E_NOTIMPL; }
};

class CComGUIItem : public CGUIItem, public CComCoClass<CComGUIItem, &CLSID_CGUIItem> {};

//////////////////////////////////////////////////////////////////////
// CGUIListIterator

class ATL_NO_VTABLE CGUIListIterator :
	public CGUIItemId,
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<ICGUIItem, &IID_ICGUIItem, &LIBID_GUICONTROLLERLib>,
	public IGUIItemType
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CGUIListIterator)
		COM_INTERFACE_ENTRY(ICGUIItem)
		COM_INTERFACE_ENTRY(IGUIItemType)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	CGUIListIterator() : m_pNode(NULL), m_nIdx(0) {}

	eGUIItemType STDMETHODCALLTYPE GetInternalType() { return eListIter; }

public: // ICGUIItem
	STDMETHOD(get_X)(long *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Y)(long *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Width)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Height)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Text)(BSTR *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Icon)(BSTR *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Background)(BSTR *pVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_Enable)(VARIANT_BOOL *pVal)								{ return E_NOTIMPL; }
	STDMETHOD(get_Check)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Hide)(VARIANT_BOOL *pVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_ValueInteger)(long *pVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_Id)(BSTR * pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Type)(BSTR * pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Pos)(long * pVal)											{ return E_NOTIMPL; }
	STDMETHOD(GetItem)(BSTR vId, VARIANT_BOOL bExact, IDispatch **pItem)	{ return E_NOTIMPL; }
	STDMETHOD(Click)(long nMode)											{ return E_NOTIMPL; }
	STDMETHOD(SetValueInteger)(long newVal)									{ return E_NOTIMPL; }
	STDMETHOD(SetValueString)(BSTR newVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_ToolTip)(BSTR *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(SetCheck)(long newVal)										{ return E_NOTIMPL; }
	STDMETHOD(DoCommand)(BSTR pVal)											{ return E_NOTIMPL; }

	STDMETHOD(get_Selected)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Children)(IDispatch ** pChildren);
	STDMETHOD(GetItemByIdx)(long vId, IDispatch **pItem);
	STDMETHOD(get_HasChildren)(VARIANT_BOOL *pVal);
	STDMETHOD(GetCount)(long *pCount);
	STDMETHOD(Next)(VARIANT_BOOL *pVal);
	STDMETHOD(Select)(VARIANT_BOOL bSelect);
	STDMETHOD(Cell)(ICGUIItem *pCol, ICGUIItem **pCell);
	STDMETHOD(GetParent)(IDispatch **pItem);
	STDMETHOD(get_Expanded)(VARIANT_BOOL *pVal);
    STDMETHOD(Expand)(VARIANT_BOOL bExpand);

public:
	tPTR    m_pNode;
	tDWORD  m_nIdx;
};

class CComGUIListIterator : public CGUIListIterator, public CComCoClass<CComGUIListIterator, &CLSID_CGUIItem> {};

//////////////////////////////////////////////////////////////////////
// CGUIListCell

class ATL_NO_VTABLE CGUIListCell :
	public CGUIItemId,
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<ICGUIItem, &IID_ICGUIItem, &LIBID_GUICONTROLLERLib>,
	public IGUIItemType
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CGUIListCell)
		COM_INTERFACE_ENTRY(ICGUIItem)
		COM_INTERFACE_ENTRY(IGUIItemType)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	CGUIListCell() : m_pNode(NULL), m_nIdx(0), m_pCol(NULL) {}

	eGUIItemType STDMETHODCALLTYPE GetInternalType() { return eListCell; }

public: // ICGUIItem
	STDMETHOD(get_X)(long *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Y)(long *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Width)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Height)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Hide)(VARIANT_BOOL *pVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_Id)(BSTR * pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Type)(BSTR * pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Pos)(long * pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Selected)(VARIANT_BOOL *pVal)								{ return E_NOTIMPL; }
	STDMETHOD(get_Children)(IDispatch ** pChildren)							{ return E_NOTIMPL; }
	STDMETHOD(GetItem)(BSTR vId, VARIANT_BOOL bExact, IDispatch **pItem)	{ return E_NOTIMPL; }
	STDMETHOD(GetItemByIdx)(long vId, IDispatch **pItem)					{ return E_NOTIMPL; }
	STDMETHOD(GetParent)(IDispatch **pItem)									{ return E_NOTIMPL; }
	STDMETHOD(GetCount)(long *pCount)										{ return E_NOTIMPL; }
	STDMETHOD(Select)(VARIANT_BOOL bSelect)									{ return E_NOTIMPL; }
	STDMETHOD(SetValueString)(BSTR newVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_ToolTip)(BSTR *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(DoCommand)(BSTR pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_HasChildren)(VARIANT_BOOL *pVal)							{ return E_NOTIMPL; }
	STDMETHOD(Next)(VARIANT_BOOL *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(Cell)(ICGUIItem *pCol, ICGUIItem **pCell)						{ return E_NOTIMPL; }
	STDMETHOD(get_Expanded)(VARIANT_BOOL *pVal)								{ return E_NOTIMPL; }
	STDMETHOD(Expand)(VARIANT_BOOL bExpand = TRUE)							{ return E_NOTIMPL; }

	STDMETHOD(get_Check)(long *pVal)										{ return get_ValueInteger(pVal); }
	STDMETHOD(SetCheck)(long newVal)										{ return SetValueInteger(newVal); }
	STDMETHOD(get_Enable)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Text)(BSTR *pVal);
	STDMETHOD(get_Icon)(BSTR *pVal);
	STDMETHOD(get_Background)(BSTR *pVal);
	STDMETHOD(get_ValueInteger)(long *pVal);
	STDMETHOD(SetValueInteger)(long newVal);
	STDMETHOD(Click)(long nMode);

public:
	tPTR    m_pNode;
	tDWORD  m_nIdx;
	tPTR    m_pCol;
};

class CComGUIListCell : public CGUIListCell, public CComCoClass<CComGUIListCell, &CLSID_CGUIItem> {};

//////////////////////////////////////////////////////////////////////
// CGUIListCol

class ATL_NO_VTABLE CGUIListCol :
	public CGUIItemId,
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<ICGUIItem, &IID_ICGUIItem, &LIBID_GUICONTROLLERLib>,
	public IGUIItemType
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CGUIListCol)
		COM_INTERFACE_ENTRY(ICGUIItem)
		COM_INTERFACE_ENTRY(IGUIItemType)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	CGUIListCol() : m_pCol(NULL) {}

	eGUIItemType STDMETHODCALLTYPE GetInternalType() { return eListCol; }

public: // ICGUIItem
	STDMETHOD(get_X)(long *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Y)(long *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Width)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Height)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Enable)(VARIANT_BOOL *pVal)								{ return E_NOTIMPL; }
	STDMETHOD(get_Background)(BSTR *pVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_Type)(BSTR * pVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Pos)(long * pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_Selected)(VARIANT_BOOL *pVal)								{ return E_NOTIMPL; }
	STDMETHOD(get_Children)(IDispatch ** pChildren)							{ return E_NOTIMPL; }
	STDMETHOD(Select)(VARIANT_BOOL bSelect)									{ return E_NOTIMPL; }
	STDMETHOD(SetValueString)(BSTR newVal)									{ return E_NOTIMPL; }
	STDMETHOD(get_ToolTip)(BSTR *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(DoCommand)(BSTR pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_HasChildren)(VARIANT_BOOL *pVal)							{ return E_NOTIMPL; }
	STDMETHOD(Next)(VARIANT_BOOL *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(Cell)(ICGUIItem *pCol, ICGUIItem **pCell)						{ return E_NOTIMPL; }
	STDMETHOD(get_Check)(long *pVal)										{ return E_NOTIMPL; }
	STDMETHOD(SetCheck)(long newVal)										{ return E_NOTIMPL; }
	STDMETHOD(get_Icon)(BSTR *pVal)											{ return E_NOTIMPL; }
	STDMETHOD(get_ValueInteger)(long *pVal)									{ return E_NOTIMPL; }
	STDMETHOD(SetValueInteger)(long newVal)									{ return E_NOTIMPL; }
	
	STDMETHOD(get_Id)(BSTR * pVal);
	STDMETHOD(get_Hide)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Text)(BSTR *pVal);
	STDMETHOD(get_Expanded)(VARIANT_BOOL *pVal);
	STDMETHOD(Expand)(VARIANT_BOOL bExpand);
	STDMETHOD(GetParent)(IDispatch **pItem);
	STDMETHOD(GetCount)(long *pCount);
	STDMETHOD(Click)(long nMode);
	STDMETHOD(GetItem)(BSTR vId, VARIANT_BOOL bExact, IDispatch **pItem);
	STDMETHOD(GetItemByIdx)(long vId, IDispatch **pItem);

public:
	tPTR    m_pCol;
};

class CComGUIListCol : public CGUIListCol, public CComCoClass<CComGUIListCol, &CLSID_CGUIItem> {};

//////////////////////////////////////////////////////////////////////
// CGUIItem

STDMETHODIMP CGUIItem::get_X(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiItemGetRect, (tPTR)&rcRect);
	*pVal = rcRect.left;
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Y(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiItemGetRect, (tPTR)&rcRect);
	*pVal = rcRect.top;
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Width(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiItemGetRect, (tPTR)&rcRect);
	*pVal = RECT_CX(rcRect);
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Height(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiItemGetRect, (tPTR)&rcRect);
	*pVal = RECT_CY(rcRect);
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Text(BSTR *pVal)
{
	cGuiEvent pEvt(GuiItemGetText); CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Icon(BSTR *pVal)
{
	cGuiEvent pEvt(GuiItemGetImage); CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Background(BSTR *pVal)
{
	cGuiEvent pEvt(GuiItemGetImage); pEvt.m_nVal1 = (tPTR)1; CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Enable(VARIANT_BOOL *pVal)
{
	*pVal = (m_pItem->m_nState & ISTATE_DISABLED) ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Check(long *pVal)
{
	*pVal = (long)CallItemOp(GuiItemIsChecked);
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Hide(VARIANT_BOOL *pVal)
{
	*pVal = (m_pItem->m_nState & ISTATE_HIDE) ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CGUIItem::get_ValueInteger(long *pVal)
{
	cGuiEvent pEvt(GuiItemGetInt); CallItemOpEx(&pEvt);
	*pVal = (long)pEvt.m_nVal1;
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Id(BSTR * pVal)
{
	cGuiEvent pEvt(GuiItemGetId); CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Type(BSTR * pVal)
{
	cGuiEvent pEvt(GuiItemGetType); CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Pos(long * pVal)
{
	*pVal = (long)CallItemOp(GuiItemGetPos);
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Selected(VARIANT_BOOL *pVal)
{
	*pVal = (m_pItem->m_nState & ISTATE_SELECTED) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Children(IDispatch ** pChildren)
{
	if( m_pItem->m_strItemType != GUI_ITEMT_LISTCTRL )
		return *pChildren = NULL, S_OK;
	
	CComGUIListIterator * pListIter = NULL;
	HRESULT hResult = CComGUIListIterator::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pListIter);
	if( FAILED(hResult) )
		return hResult;

	pListIter->Attach(m_pRoot, m_pItem);
	pListIter->m_pNode = (tPTR)(cRowNode *)(CListItem *)(CItemBase *)m_pItem;
	pListIter->m_nIdx = 0;
	*pChildren = pListIter;
	return S_OK;
}

STDMETHODIMP CGUIItem::GetItem(BSTR vId, VARIANT_BOOL bExact, IDispatch ** pItem)
{
	if( (m_pItem->m_strItemType == GUI_ITEMT_LISTCTRL) && !wcscmp(vId, L"cols") )
	{
		CComGUIListCol * pGuiItem = NULL;
		HRESULT hResult = CComGUIListCol::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void**)&pGuiItem);
		if( FAILED(hResult) )
			return hResult;
		
		pGuiItem->Attach(m_pRoot, m_pItem);
		pGuiItem->m_pCol = CallItemOp(GuiList_GetCols);
		*pItem = pGuiItem;
		return S_OK;
	}
	
	_CCGUIItemPtr pItemBase = ItemGet(vId, !!bExact);
	if( !pItemBase )
		return E_FAIL;

	CComGUIItem * pGuiItem = NULL;
	HRESULT hResult = CComGUIItem::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void**)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;
	
	pGuiItem->Attach(m_pRoot, pItemBase, pItemBase->m_strItemId.c_str(), cCP_ANSI);
	*pItem = pGuiItem;
	return S_OK;
}

STDMETHODIMP CGUIItem::GetItemByIdx(long vId, IDispatch ** pItem)
{
	if( m_pItem->m_strItemType == GUI_ITEMT_LISTCTRL )
		return *pItem = NULL, S_OK;

	_CCGUIItemPtr pItemBase = ItemGetByPos(vId);
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

STDMETHODIMP CGUIItem::GetParent(IDispatch ** pItem)
{
	_CCGUIItemPtr pItemBase = (CItemBase *)CallItemOp(GuiItemGetParent);
	if( !pItemBase )
		return E_FAIL;

	CComGUIItem * pGuiItem = NULL;
	HRESULT hResult = CComGUIItem::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void**)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;

	pGuiItem->Attach(m_pRoot, pItemBase);
	*pItem = pGuiItem;
	return S_OK;
}

STDMETHODIMP CGUIItem::GetCount(long *pCount)
{
	if( m_pItem->m_strItemType == GUI_ITEMT_LISTCTRL )
		return *pCount = 0, S_OK;
	
	*pCount = (long)CallItemOp(GuiItemGetCount);
	return S_OK;
}

STDMETHODIMP CGUIItem::Click(long nMode)            { PostItemOpWaitGui(GuiItemClick, (tPTR)nMode); return S_OK; }
STDMETHODIMP CGUIItem::Select(VARIANT_BOOL bSelect) { PostItemOpWaitGui(GuiItemSelect, (tPTR)bSelect); return S_OK; }
STDMETHODIMP CGUIItem::SetValueInteger(long newVal) { PostItemOpWaitGui(GuiItemSetInt, (tPTR)newVal); return S_OK; }
STDMETHODIMP CGUIItem::SetValueString(BSTR newVal)  { PostItemOpWaitGui(GuiItemSetStr, 0, 0, 0, newVal); return S_OK; }

//////////////////////////////////////////////////////////////////////
// CGUIListIterator

static bool _List_IsValidNode(cRowNode * pNode, cRowNode * pCheck)
{
	if( pCheck == pNode )
		return true;

	for(tDWORD i = 0, n = pNode->Nodes().size(); i < n; i++)
		if( _List_IsValidNode(pNode->Nodes()[i], pCheck) )
			return true;

	return false;
}

static bool _List_IsValidColumn(const cColumn * pColumn, cColumn * pCheck)
{
	if( pCheck == pColumn )
		return true;

	for(tDWORD i = 0, n = pColumn->m_vChilds.size(); i < n; i++)
		if( _List_IsValidColumn(pColumn->m_vChilds[i], pCheck) )
			return true;

	return false;
}

static bool _ListIter_GetRowId(cRowNode::cRowId& rowId, CItemBase * pItem, cGuiEvent * pEvt, bool bSelf = false)
{
	cRowNode * pNode = (cRowNode *)pEvt->m_nVal1;
	tDWORD nIdx = (tDWORD)pEvt->m_nVal2;
	
	if( !pNode )
		return false;
	if( !bSelf && !nIdx )
		return false;
	
	if( pItem->m_strItemType != GUI_ITEMT_LISTCTRL )
		return false;
	if( !_List_IsValidNode((CListItem *)pItem, pNode) )
		return false;

	tDWORD nNodes = pNode->Nodes().size();
	if( nIdx > nNodes )
	{
		rowId.m_node = pNode;
		rowId.m_pos = nIdx - nNodes;
	}
	else
	{
		rowId.m_node = nIdx ? pNode->Nodes()[nIdx - 1] : pNode;
		rowId.m_pos = 0;
	}

	return true;
}

static bool _ListIter_GetSelfRowId(cRowNode::cRowId& rowId, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode * pNode = (cRowNode *)pEvt->m_nVal1;
	
	if( pItem->m_strItemType != GUI_ITEMT_LISTCTRL )
		return false;
	if( !_List_IsValidNode((CListItem *)pItem, pNode) )
		return false;

	rowId.m_node = pNode;
	return true;
}

//////////////////////////////////////////////////////////////////////

static void GuiListIter_Count(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( _ListIter_GetSelfRowId(rowId, pItem, pEvt) )
		pEvt->m_nRes = (tPTR)(rowId.m_node->Nodes().size() + rowId.RowCount());
}

static void GuiListIter_Next(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( !_ListIter_GetSelfRowId(rowId, pItem, pEvt) )
		return;

	tDWORD nIdx = (tDWORD)pEvt->m_nVal2;
	tDWORD nSubItems = rowId.m_node->Nodes().size() + rowId.RowCount();
	pEvt->m_nRes = (tPTR)(nIdx < nSubItems);
}

static void GuiListIter_GetChildren(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( !_ListIter_GetRowId(rowId, pItem, pEvt) )
		return;

	if( (cRowNode *)pEvt->m_nVal1 != rowId.m_node )
		pEvt->m_nRes = (tPTR)rowId.m_node;
}

static void GuiListIter_HasChildren(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( _ListIter_GetRowId(rowId, pItem, pEvt) )
		pEvt->m_nRes = (tPTR)((cRowNode *)pEvt->m_nVal1 != rowId.m_node);
}

static void GuiListIter_Selected(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( _ListIter_GetRowId(rowId, pItem, pEvt) )
		pEvt->m_nRes = (tPTR)!!rowId.Idx().m_sel;
}

static void GuiListIter_Select(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( !_ListIter_GetRowId(rowId, pItem, pEvt) )
		return;
	
	CListItem * pList = (CListItem *)pItem;
	
	pList->EnsureVisible(rowId);
	RECT rc; pList->GetElemViewRect(&rowId, NULL, rc);
	_Simulate_MouseClick(pList->GetWindow(), (rc.left + rc.right)/2, (rc.top + rc.bottom)/2);
}

static void GuiListIter_IsExpanded(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( _ListIter_GetRowId(rowId, pItem, pEvt) && !rowId.m_pos )
		pEvt->m_nRes = (tPTR)rowId.m_node->IsExpanded();
}

static void GuiListIter_Expand(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( _ListIter_GetRowId(rowId, pItem, pEvt) && !rowId.m_pos )
		rowId.m_node->Expand(!!pEvt->m_nVal3);
}

//////////////////////////////////////////////////////////////////////

STDMETHODIMP CGUIListIterator::get_Selected(VARIANT_BOOL *pVal)
{
	*pVal = CallItemOp(GuiListIter_Selected, m_pNode, (tPTR)m_nIdx) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGUIListIterator::Select(VARIANT_BOOL bSelect)
{
	CallItemOp(GuiListIter_Select, m_pNode, (tPTR)m_nIdx, (tPTR)bSelect);
	return S_OK;
}

STDMETHODIMP CGUIListIterator::get_Children(IDispatch ** pChildren)
{
	CComGUIListIterator * pListIter = NULL;
	HRESULT hResult = CComGUIListIterator::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pListIter);
	if( FAILED(hResult) )
		return hResult;

	pListIter->Attach(m_pRoot, m_pItem);
	pListIter->m_pNode = CallItemOp(GuiListIter_GetChildren, m_pNode, (tPTR)m_nIdx);
	pListIter->m_nIdx = 0;
	*pChildren = pListIter;
	return S_OK;
}

STDMETHODIMP CGUIListIterator::GetItemByIdx(long vId, IDispatch **pItem)
{
	if( !CallItemOp(GuiListIter_Next, m_pNode, (tPTR)vId) )
		return *pItem = NULL, S_OK;
	
	CComGUIListIterator * pListIter = NULL;
	HRESULT hResult = CComGUIListIterator::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pListIter);
	if( FAILED(hResult) )
		return hResult;

	pListIter->Attach(m_pRoot, m_pItem);
	pListIter->m_pNode = m_pNode;
	pListIter->m_nIdx = vId + 1;
	*pItem = pListIter;
	return S_OK;
}

STDMETHODIMP CGUIListIterator::get_HasChildren(VARIANT_BOOL *pVal)
{
	*pVal = CallItemOp(GuiListIter_HasChildren, m_pNode, (tPTR)m_nIdx) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGUIListIterator::GetCount(long *pCount)
{
	*pCount = (ULONG)CallItemOp(GuiListIter_Count, m_pNode);
	return S_OK;
}

STDMETHODIMP CGUIListIterator::Next(VARIANT_BOOL *pVal)
{
	*pVal = CallItemOp(GuiListIter_Next, m_pNode, (tPTR)m_nIdx) ? (m_nIdx++, VARIANT_TRUE) : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGUIListIterator::Cell(ICGUIItem *pCol, ICGUIItem **pCell)
{
	if( !_Check_Obj(pCol, eListCol) )
		return E_INVALIDARG;
	
	CComGUIListCell * pListCell = NULL;
	HRESULT hResult = CComGUIListCell::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pListCell);
	if( FAILED(hResult) )
		return hResult;

	pListCell->Attach(m_pRoot, m_pItem);
	pListCell->m_pNode = m_pNode;
	pListCell->m_nIdx = m_nIdx;
	pListCell->m_pCol = ((CGUIListCol *)pCol)->m_pCol;
	*pCell = pListCell;
	return S_OK;
}

STDMETHODIMP CGUIListIterator::GetParent(IDispatch **pItem)
{
	return E_NOTIMPL;
}

STDMETHODIMP CGUIListIterator::get_Expanded(VARIANT_BOOL *pVal)
{
	*pVal = CallItemOp(GuiListIter_IsExpanded, m_pNode, (tPTR)m_nIdx) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGUIListIterator::Expand(VARIANT_BOOL bExpand)
{
	CallItemOp(GuiListIter_Expand, m_pNode, (tPTR)m_nIdx, (tPTR)bExpand);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CGUIListCell

CGridItem * _GuiListCell_GetId(CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	if( !_ListIter_GetRowId(rowId, pItem, pEvt) )
		return NULL;
	
	CListItem * pList = (CListItem *)pItem;
	cColumn * pColumn = (cColumn *)pEvt->m_nVal3;
	if( !_List_IsValidColumn(&pList->GetTopColumn(), pColumn) )
		return NULL;

	return pList->GetCellItem(rowId, *pColumn);
}

bool _GuiListCell_GetId(cRowNode::cRowId& rowId, cColumn *& pCol, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !_ListIter_GetRowId(rowId, pItem, pEvt) )
		return false;
	
	CListItem * pList = (CListItem *)pItem;
	cColumn * pColumn = (cColumn *)pEvt->m_nVal3;
	if( !_List_IsValidColumn(&pList->GetTopColumn(), pColumn) )
		return false;

	pCol = pColumn;
	return true;
}

//////////////////////////////////////////////////////////////////////

static void GuiListCell_GetText(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	CGridItem * pCell = _GuiListCell_GetId(pItem, pEvt);
	if( pCell )
		pRoot->LocalizeStr(pEvt->m_strVal1, pCell->GetDisplayText());
}

static void GuiListCell_GetImage(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	CGridItem * pCell = _GuiListCell_GetId(pItem, pEvt);
	if( pCell )
		_GuiItem_GetImage(!!pEvt->m_nVal4, pRoot, pCell, pEvt->m_strVal1);
}

static void GuiListCell_IsEnabled(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	CGridItem * pCell = _GuiListCell_GetId(pItem, pEvt);
	if( pCell )
		pEvt->m_nRes = (tPTR)!(pCell->m_nState & ISTATE_DISABLED);
}

static void GuiListCell_GetCheck(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	CGridItem * pCell = _GuiListCell_GetId(pItem, pEvt);
	if( pCell )
		pEvt->m_nRes = (tPTR)pCell->m_value.ToInt();
}

static void GuiListCell_SetCheck(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	cColumn * pColumn;
	if( !_GuiListCell_GetId(rowId, pColumn, pItem, pEvt) )
		return;
	
	CListItem * pList = (CListItem *)pItem;
	pList->CheckItem(rowId, *pColumn, (long)pEvt->m_nVal4, true);
	pList->Update();
}

static void GuiListCell_Click(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cRowNode::cRowId rowId;
	cColumn * pColumn;
	if( !_GuiListCell_GetId(rowId, pColumn, pItem, pEvt) )
		return;

	CListItem * pList = (CListItem *)pItem;

	pList->EnsureVisible(rowId);
	RECT rc; pList->GetElemViewRect(&rowId, pColumn, rc);
	_Simulate_MouseClick(pList->GetWindow(), (rc.left + rc.right)/2, (rc.top + rc.bottom)/2, !!pEvt->m_nVal4);
}

//////////////////////////////////////////////////////////////////////

STDMETHODIMP CGUIListCell::get_Text(BSTR *pVal)
{
	cGuiEvent pEvt(GuiListCell_GetText);
	pEvt.m_nVal1 = m_pNode;
	pEvt.m_nVal2 = (tPTR)m_nIdx;
	pEvt.m_nVal3 = m_pCol;
	CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIListCell::get_Icon(BSTR *pVal)
{
	cGuiEvent pEvt(GuiListCell_GetImage);
	pEvt.m_nVal1 = m_pNode;
	pEvt.m_nVal2 = (tPTR)m_nIdx;
	pEvt.m_nVal3 = m_pCol;
	pEvt.m_nVal4 = (tPTR)false;
	CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIListCell::get_Background(BSTR *pVal)
{
	cGuiEvent pEvt(GuiListCell_GetImage);
	pEvt.m_nVal1 = m_pNode;
	pEvt.m_nVal2 = (tPTR)m_nIdx;
	pEvt.m_nVal3 = m_pCol;
	pEvt.m_nVal4 = (tPTR)true;
	CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIListCell::get_Enable(VARIANT_BOOL *pVal)
{
	*pVal = CallItemOp(GuiListCell_IsEnabled, m_pNode, (tPTR)m_nIdx, m_pCol) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGUIListCell::get_ValueInteger(long *pVal)
{
	*pVal = (long)CallItemOp(GuiListCell_GetCheck, m_pNode, (tPTR)m_nIdx, m_pCol);
	return S_OK;
}

STDMETHODIMP CGUIListCell::SetValueInteger(long newVal)
{
	cGuiEvent pEvt(GuiListCell_SetCheck);
	pEvt.m_nVal1 = m_pNode;
	pEvt.m_nVal2 = (tPTR)m_nIdx;
	pEvt.m_nVal3 = m_pCol;
	pEvt.m_nVal4 = (tPTR)newVal;
	CallItemOpEx(&pEvt);
	return S_OK;
}

STDMETHODIMP CGUIListCell::Click(long nMode)
{
	cGuiEvent pEvt(GuiListCell_Click);
	pEvt.m_nVal1 = m_pNode;
	pEvt.m_nVal2 = (tPTR)m_nIdx;
	pEvt.m_nVal3 = m_pCol;
	pEvt.m_nVal4 = (tPTR)nMode;
	PostItemOpEx2(&pEvt, true);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CGUIListCol

static void GuiList_GetCols(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( pItem->m_strItemType == GUI_ITEMT_LISTCTRL )
		pEvt->m_nRes = (tPTR)&((CListItem *)pItem)->GetTopColumn();
}

static bool _ListCol_GetColId(cColumn*& pCol, CItemBase * pItem, cGuiEvent * pEvt)
{
	cColumn * pColumn = (cColumn *)pEvt->m_nVal1;
	
	if( pItem->m_strItemType != GUI_ITEMT_LISTCTRL )
		return false;
	if( !_List_IsValidColumn(&((CListItem *)pItem)->GetTopColumn(), pColumn) )
		return false;

	pCol = pColumn;
	return true;
}

static void GuiListCol_GetCol(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cColumn * pColumn;
	if( !_ListCol_GetColId(pColumn, pItem, pEvt) )
		return;
	
	if( pEvt->m_nVal2 )
	{
		bool bExact = !!pEvt->m_nVal3;
		tString strColId; pRoot->LocalizeStr(strColId, pEvt->m_strVal1.data(), pEvt->m_strVal1.size());
		
		pEvt->m_nRes = (tPTR)pColumn->Find(strColId.c_str());
	}
	else
	{
		tDWORD nIdx = (tDWORD)pEvt->m_nVal3;
		if( nIdx < pColumn->m_vChilds.size() )
			pEvt->m_nRes = (tPTR)pColumn->m_vChilds[nIdx];
	}
}

static void GuiListCol_Expand(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cColumn * pColumn;
	if( !_ListCol_GetColId(pColumn, pItem, pEvt) )
		return;
	
	if( pEvt->m_nVal2 )
		pColumn->Expand(!!pEvt->m_nVal3);
	else
		pEvt->m_nRes = (tPTR)!!(pColumn->m_nFlags & cColumn::fExpanded);
}

static void GuiListCol_GetText(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cColumn * pColumn;
	if( _ListCol_GetColId(pColumn, pItem, pEvt) )
		pRoot->LocalizeStr(pEvt->m_strVal1, pColumn->m_sText.c_str(), pColumn->m_sText.size());
}

static void GuiListCol_GetCount(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cColumn * pColumn;
	if( _ListCol_GetColId(pColumn, pItem, pEvt) )
		pEvt->m_nRes = (tPTR)pColumn->m_vChilds.size();
}

static void GuiListCol_Click(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cColumn * pColumn;
	if( !_ListCol_GetColId(pColumn, pItem, pEvt) )
		return;

	CListItem * pList = (CListItem *)pItem;
	RECT rc;
	switch( (tDWORD)pEvt->m_nVal2 )
	{
	case 0:
		pList->GetElemViewRect(NULL, pColumn, rc);
		_Simulate_MouseClick(pList->GetWindow(), (rc.left + rc.right)/2, (rc.top + rc.bottom)/2, 0, false);
		break;
	case 1:
		pList->GetElemViewRect(NULL, pColumn, rc);
		_Simulate_MouseClick(pList->GetWindow(), (rc.left + rc.right)/2, (rc.top + rc.bottom)/2, 1);
		break;
	case 2:
		pList->GetElemViewRect(NULL, pColumn, rc, CListItem::etColFilter);
		_Simulate_MouseClick(pList->GetWindow(), (rc.left + rc.right)/2, (rc.top + rc.bottom)/2);
		break;
	}
}

//////////////////////////////////////////////////////////////////////

STDMETHODIMP CGUIListCol::get_Id(BSTR * pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CGUIListCol::get_Hide(VARIANT_BOOL *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CGUIListCol::get_Text(BSTR *pVal)
{
	cGuiEvent pEvt(GuiListCol_GetText); CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIListCol::get_Expanded(VARIANT_BOOL *pVal)
{
	*pVal = CallItemOp(GuiListCol_Expand, m_pCol, (tPTR)false) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGUIListCol::Expand(VARIANT_BOOL bExpand)
{
	CallItemOp(GuiListCol_Expand, m_pCol, (tPTR)true, (tPTR)bExpand);
	return S_OK;
}

STDMETHODIMP CGUIListCol::GetParent(IDispatch **pItem)
{
	return E_NOTIMPL;
}

STDMETHODIMP CGUIListCol::GetCount(long *pCount)
{
	*pCount = (long)CallItemOp(GuiListCol_GetCount, m_pCol);
	return S_OK;
}

STDMETHODIMP CGUIListCol::Click(long nMode)
{
	PostItemOpWaitGui(GuiListCol_Click, m_pCol, (tPTR)nMode);
	return S_OK;
}

STDMETHODIMP CGUIListCol::GetItem(BSTR vId, VARIANT_BOOL bExact, IDispatch **pItem)
{
	tPTR pCol = CallItemOp(GuiListCol_GetCol, m_pCol, (tPTR)true, (tPTR)bExact, vId);
	if( !pCol )
		return *pItem = NULL, S_FALSE;

	CComGUIListCol * pGuiItem = NULL;
	HRESULT hResult = CComGUIListCol::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void**)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;
	
	pGuiItem->Attach(m_pRoot, m_pItem);
	pGuiItem->m_pCol = pCol;
	*pItem = pGuiItem;
	return S_OK;
}

STDMETHODIMP CGUIListCol::GetItemByIdx(long vId, IDispatch **pItem)
{
	tPTR pCol = CallItemOp(GuiListCol_GetCol, m_pCol, (tPTR)false, (tPTR)vId);
	if( !pCol )
		return *pItem = NULL, S_FALSE;

	CComGUIListCol * pGuiItem = NULL;
	HRESULT hResult = CComGUIListCol::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void**)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;
	
	pGuiItem->Attach(m_pRoot, m_pItem);
	pGuiItem->m_pCol = pCol;
	*pItem = pGuiItem;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class AdminGUI_RootItem
{
public:
	_CCGUIItemPtr m_root;
	cStrObj       m_object;
};

typedef cVector<AdminGUI_RootItem> AdminGUI_RootsList;

//////////////////////////////////////////////////////////////////////

static CRITICAL_SECTION     g_csRootsList;
static AdminGUI_RootsList * g_pRootsList;
static CGuiLoader           g_GuiLib;
static HINSTANCE            g_hComMdlInstance;

//////////////////////////////////////////////////////////////////////

class AdminGUI_RootsList_AutoLocker
{
public:
	AdminGUI_RootsList_AutoLocker()  { EnterCriticalSection(&g_csRootsList); }
	~AdminGUI_RootsList_AutoLocker() { LeaveCriticalSection(&g_csRootsList); }
};

//////////////////////////////////////////////////////////////////////

static bool _GUIController_RootsListInit(tPTR hInstance)
{
	g_hComMdlInstance = (HINSTANCE)hInstance;
	g_pRootsList = new AdminGUI_RootsList();
	if( !g_pRootsList )
		return false;
	::InitializeCriticalSection(&g_csRootsList);
	return true;
}

static void _GUIController_RootsListDone()
{
	::DeleteCriticalSection(&g_csRootsList);
	delete g_pRootsList;
}

static bool _GUIController_Register()
{
	return g_GuiLib.Load(g_root);
}

static void _GUIController_Unegister()
{
	{
		AdminGUI_RootsList_AutoLocker _Locker;
		g_pRootsList->clear();
	}
	
	g_GuiLib.Unload();
}

//////////////////////////////////////////////////////////////////////

void GUIControllerAddRoot(CRootItem * pRoot, LPCWSTR strObject)
{
	if( !g_pRootsList || !pRoot )
		return;

	AdminGUI_RootsList_AutoLocker _Locker;

	AdminGUI_RootItem& _item = g_pRootsList->push_back();
	_item.m_root = pRoot; _ITEM_ADDREF(_item.m_root);
	_item.m_object = strObject;
}

void GUIControllerRemoveRoot(CRootItem * pRoot)
{
	if( !g_pRootsList || !pRoot )
		return;

	AdminGUI_RootsList_AutoLocker _Locker;

	for(int i = 0, n = g_pRootsList->size(); i < n; i++)
		if( g_pRootsList->at(i).m_root == pRoot )
		{
			g_pRootsList->remove(i, 1);
			break;
		}

	if( !g_pRootsList->size() )
		g_pRootsList->clear();
}

CRootItem * GUIControllerGetRoot(LPCWSTR strObject)
{
	if( !g_pRootsList )
		return NULL;

	AdminGUI_RootsList_AutoLocker _Locker;

	for(int i = 0, n = g_pRootsList->size(); i < n; i++)
		if( g_pRootsList->at(i).m_object == strObject )
		{
			CItemBase * pRoot = g_pRootsList->at(i).m_root;
			_ITEM_ADDREF(pRoot);
			return (CRootItem *)pRoot;
		}
	
	return NULL;
}

static LPCSTR _GetFuncName(cGuiEvent::OpFunc * pFunc)
{
	#define _GETFUNCNAME_ITEM(_name) if( pFunc == Gui##_name ) return #_name;
	
	_GETFUNCNAME_ITEM(ItemGet);
	_GETFUNCNAME_ITEM(ItemGetByPos);
	_GETFUNCNAME_ITEM(ItemClick);
	_GETFUNCNAME_ITEM(ItemSelect);
	_GETFUNCNAME_ITEM(ItemSetInt);
	_GETFUNCNAME_ITEM(ItemGetInt);
	_GETFUNCNAME_ITEM(ItemGetText);
	_GETFUNCNAME_ITEM(ItemGetId);
	_GETFUNCNAME_ITEM(ItemGetType);
	_GETFUNCNAME_ITEM(ItemGetPos);
	_GETFUNCNAME_ITEM(ItemGetImage);
	_GETFUNCNAME_ITEM(ItemIsChecked);
	_GETFUNCNAME_ITEM(ItemGetRect);
	_GETFUNCNAME_ITEM(ItemSetStr);
	_GETFUNCNAME_ITEM(ItemGetCount);
	_GETFUNCNAME_ITEM(ItemDestroy);
	_GETFUNCNAME_ITEM(RootDoCommand);

	static char strId[100]; _snprintf(strId, countof(strId), "0x%p", pFunc);
	strId[countof(strId) - 1] = 0;
	return strId;
}

static LPCSTR _GetParentName(CItemBase * pItem)
{
	if( !pItem || !pItem->m_pParent )
		return "";
	return pItem->m_pParent->m_strItemId.c_str();
}

void GUIControllerEvent(hOBJECT hObj, CRootItem * pRoot, cGuiEvent * pParams)
{
	if( pParams->m_pFunc )
	{
		PR_TRACE((hObj, prtIMPORTANT, "gui\tGUIController: root(0x%p) func(%s) item(0x%p id:%S) parent(0x%p id:%s) vals_ptr(0x%p,0x%p,0x%p) vals_str(%S,%S,%S)",
			pRoot,
			_GetFuncName(pParams->m_pFunc),
			
			pParams->m_pItem,
			pParams->m_strItemId.data(),
			
			pParams->m_pItem ? pParams->m_pItem->m_pParent : NULL,
			_GetParentName(pParams->m_pItem),
			
			pParams->m_nVal1,
			pParams->m_nVal2,
			pParams->m_nVal3,
			pParams->m_strVal1.data(),
			pParams->m_strVal2.data(),
			pParams->m_strVal3.data()));
	}

	bool bExecFunc = false;
	CItemBase * pItem = pParams->m_pItem;
	if( (!pItem || pItem->m_pRoot) && pParams->m_pFunc )
	{
		bExecFunc = true;

		if( pItem && !pParams->m_strItemId.empty() && pItem->m_pParent )
			if( !(pItem = pItem->m_pParent->GetItem(pParams->m_strItemId.c_str(cCP_ANSI), NULL, false)) )
				bExecFunc = false;
	}
	
	if( bExecFunc && (!GUI_COM_RO(pRoot) || pParams->m_pFunc == GuiRootDoCommand) )
		pParams->m_pFunc(pRoot, pItem, pParams);
	
	if( !pParams->m_bSync && pParams->m_pItem )
		pParams->m_pItem->Release();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#endif // _WIN32

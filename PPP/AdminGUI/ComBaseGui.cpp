// CombaseGui.cpp: implementation of the CGUIItem class.
//
//////////////////////////////////////////////////////////////////////

#include <Guiloader.h>

//////////////////////////////////////////////////////////////////////
// Funcs

static void GuiItemGet(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	if( !pEvt->m_strVal1.empty() )
		pItem = pItem->GetItem(pEvt->m_strVal1.c_str(cCP_ANSI));
	
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

	pItem = pItem->GetByPos((tSIZE_T)pEvt->m_nVal1);
	if( pItem )
	{
		pItem->AddRef();
		pEvt->m_nRes = pItem;
	}
}

static void GuiItemSetStr(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	if( pItem->GetType() == tid_STRING_OBJ )
	{
		pItem->SetValue(cVariant(pEvt->m_strVal1.data()));
		pItem->SetChangedData();
	}
}

static void GuiItemGetCount(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	if( pItem->m_nFlags & STYLE_LIST_ITEM )
		pEvt->m_nRes = (tPTR)((CListItem *)(CItemBase *)pItem)->GetItemsCount();
	else
		pEvt->m_nRes = (tPTR)pItem->GetChildrenCount();
}

static void GuiListItemGetRect(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( !pItem )
		return;

	RECT * pRect = (RECT *)pEvt->m_nVal3;
	if( (int)pEvt->m_nVal2 != -1 )
		ListView_GetSubItemRect(pItem->GetWindow(), (int)pEvt->m_nVal1, (int)pEvt->m_nVal2, LVIR_BOUNDS, pRect);
	else
		ListView_GetItemRect(pItem->GetWindow(), (int)pEvt->m_nVal1, pRect, LVIR_BOUNDS);
}

static void GuiItemClick(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)            { pItem->Click(); }
static void GuiItemSelect(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)           { pItem->Select(); }
static void GuiItemGetText(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)          { pRoot->LocalizeStr(pItem->GetDisplayText(), pEvt->m_strVal1); }
static void GuiItemGetId(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)            { pRoot->LocalizeStr(pItem->m_strItemId.c_str(), pEvt->m_strVal1); }
static void GuiItemGetPos(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)           { pEvt->m_nRes = (tPTR)pItem->m_nPos; }
static void GuiItemIsChecked(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)        { pEvt->m_nRes = (tPTR)pItem->IsChecked(); }
static void GuiItemGetRect(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)          { pItem->GetParentRect(*(RECT *)pEvt->m_nVal1); }
static void GuiListItemClick(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)        { ListView_SetItemState(((CListItem *)pItem)->GetWindow(), pEvt->m_nVal1, LVIS_SELECTED|LVIS_FOCUSED, LVIF_STATE); }
static void GuiListItemSelect(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)       { ListView_SetItemState(((CListItem *)pItem)->GetWindow(), pEvt->m_nVal1, LVIS_SELECTED, LVIF_STATE); }
static void GuiListItemSetCheck(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)     { ((CListItem *)pItem)->SetCheck((int)pEvt->m_nVal1, (int)pEvt->m_nVal2, !!pEvt->m_nVal3); }
static void GuiListItemGetText(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)      { ((CListItem *)pItem)->GetItemTextW((int)pEvt->m_nVal1, (int)pEvt->m_nVal2, pEvt->m_strVal1); }
static void GuiListItemGetIcon(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)      { pEvt->m_nRes = (tPTR)((CListItem *)pItem)->GetItemIcon((int)pEvt->m_nVal1); }
static void GuiListItemGetCheck(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)     { pEvt->m_nRes = (tPTR)((CListItem *)pItem)->GetCheck((int)pEvt->m_nVal1, (int)pEvt->m_nVal2); }
static void GuiListItemGetColsCount(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt) { pEvt->m_nRes = (tPTR)((CListItem *)pItem)->GetColsCount(); }
static void GuiRootShowMainWnd(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)      { pRoot->SendEvent(cAskGuiAction::SHOW_MAIN_WND); }
static void GuiRootDoCommand(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)        { pRoot->DoCommand(NULL, pEvt->m_strVal1.c_str(cCP_ANSI), true); }
static void GuiItemSetInt(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	if( pItem->GetType() == tid_STRING_OBJ )
	{
		cVariant val = (tDWORD)pEvt->m_nVal1;
		pItem->SetValue(val);
		pItem->SetChangedData();
	}
	else
		pItem->SetInt((tDWORD)pEvt->m_nVal1, true);
}

static void GuiItemGetInt(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt)
{
	cVariant val;
	pItem->GetValue(val);
	*(tQWORD *)pEvt->m_nVal1 = val.ToQWORD();
}

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
		
		if( ((CRootItem *)(CItemBase *)m_pRoot)->SendEvent(cGuiEvent::eIID, pEvt, bSync ? EVENT_SEND_SYNC : 0) == errOBJECT_BAD_INTERNAL_STATE )
			if( !bSync )
				_ITEM_RELEASE(m_pItem);
	}

	void PostItemOp(cGuiEvent::OpFunc * pFunc, tPTR nVal1 = 0, tPTR nVal2 = 0, tPTR nVal3 = 0, LPCWSTR strVal1 = NULL)
	{
		cSerObj<cGuiEvent> pEvt; pEvt.init(cGuiEvent::eIID);
		pEvt->m_pFunc = pFunc;
		pEvt->m_nVal1 = nVal1;
		pEvt->m_nVal2 = nVal2;
		pEvt->m_nVal3 = nVal3;
		pEvt->m_strVal1 = strVal1;
		CallItemOpEx(pEvt, cFALSE);
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

	CItemBase * ItemGet(LPCWSTR strItemId = NULL) { return (CItemBase *)CallItemOp(GuiItemGet, 0, 0, 0, strItemId); }
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
	public CComCoClass<CGUIItem, &CLSID_CGUIItem>,
	public IDispatchImpl<ICGUIItem, &IID_ICGUIItem, &LIBID_GUICONTROLLERLib>
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CGUIItem)
		COM_INTERFACE_ENTRY(ICGUIItem)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public: // ICGUIItem
	STDMETHOD(get_X)(long *pVal);
	STDMETHOD(get_Y)(long *pVal);
	STDMETHOD(get_Width)(long *pVal);
	STDMETHOD(get_Height)(long *pVal);
	STDMETHOD(get_Text)(BSTR *pVal);
	STDMETHOD(get_Enable)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Check)(long *pVal);
	STDMETHOD(get_Hide)(VARIANT_BOOL *pVal);
	STDMETHOD(get_ValueInteger)(long *pVal);
	STDMETHOD(get_Id)(BSTR * pVal);
	STDMETHOD(get_Pos)(long * pVal);

	STDMETHOD(GetItem)(BSTR vId, IDispatch **pItem);
	STDMETHOD(GetItemByIdx)(long vId, IDispatch **pItem);
	STDMETHOD(GetCount)(long *pCount);
	STDMETHOD(Click)();
	STDMETHOD(Select)();
	STDMETHOD(SetValueInteger)(long newVal);
	STDMETHOD(SetValueString)(BSTR newVal);
	
	STDMETHOD(get_Icon)(BSTR *pVal)                       { *pVal = ::SysAllocString(L""); return S_OK; }
	STDMETHOD(get_ToolTip)(BSTR *pVal)                    { *pVal = ::SysAllocString(L""); return S_OK; }
	STDMETHOD(SetCheck)(long newVal)                      { return SetValueInteger(newVal != FALSE); }
};

//////////////////////////////////////////////////////////////////////
// CGUIListItem

class ATL_NO_VTABLE CGUIListItem :
	public CGUIItemId,
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGUIListItem, &CLSID_CGUIItem>,
	public IDispatchImpl<ICGUIItem, &IID_ICGUIItem, &LIBID_GUICONTROLLERLib>
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CGUIItem)
		COM_INTERFACE_ENTRY(ICGUIItem)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	CGUIListItem() : m_nIdx(-1), m_nSubIdx(-1) {}

public: // ICGUIItem
	STDMETHOD(get_X)(long *pVal);
	STDMETHOD(get_Y)(long *pVal);
	STDMETHOD(get_Width)(long *pVal);
	STDMETHOD(get_Height)(long *pVal);
	STDMETHOD(get_Text)(BSTR *pVal);
	STDMETHOD(get_Icon)(BSTR *pVal);
	STDMETHOD(get_Check)(long *pVal);
	STDMETHOD(get_Pos)(long * pVal);

	STDMETHOD(GetItemByIdx)(long vId, IDispatch **pItem);
	STDMETHOD(GetCount)(long *pCount);
	STDMETHOD(Click)();
	STDMETHOD(Select)();
	STDMETHOD(SetValueInteger)(long newVal);

	STDMETHOD(get_ToolTip)(BSTR *pVal)                    { *pVal = ::SysAllocString(L""); return S_OK; }
	STDMETHOD(get_Enable)(VARIANT_BOOL *pVal)             { *pVal = VARIANT_FALSE; return S_OK; }
	STDMETHOD(get_Hide)(VARIANT_BOOL *pVal)               { *pVal = VARIANT_FALSE; return S_OK; }
	STDMETHOD(get_ValueInteger)(long *pVal)               { return get_Check(pVal); }
	STDMETHOD(get_Id)(BSTR * pVal)                        { *pVal = ::SysAllocString(L""); return S_OK; }
	STDMETHOD(GetItem)(BSTR vId, IDispatch **pItem)       { return E_NOTIMPL; }
	STDMETHOD(SetCheck)(long newVal)                      { return SetValueInteger(newVal != FALSE); }
	STDMETHOD(SetValueString)(BSTR newVal)                { return E_NOTIMPL; }

public:
	long            m_nIdx;
	long            m_nSubIdx;
};

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
	tQWORD val = 0; CallItemOp(GuiItemGetInt, (tPTR)&val);
	*pVal = (long)val;
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Id(BSTR * pVal)
{
	cGuiEvent pEvt(GuiItemGetId); CallItemOpEx(&pEvt);
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIItem::get_Pos(long * pVal)
{
	*pVal = (long)CallItemOp(GuiItemGetPos);
	return S_OK;
}

STDMETHODIMP CGUIItem::GetItem(BSTR vId, IDispatch ** pItem)
{
	_CCGUIItemPtr pItemBase = ItemGet(vId);
	if( !pItemBase )
		return E_FAIL;

	CGUIItem * pGuiItem = NULL;
	HRESULT hResult = _CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void**)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;
	
	pGuiItem->Attach(m_pRoot, pItemBase, vId);
	*pItem = pGuiItem;
	return S_OK;
}

STDMETHODIMP CGUIItem::GetItemByIdx(long vId, IDispatch ** pItem)
{
	if( m_pItem->m_nFlags & STYLE_LIST_ITEM )
	{
		_CCGUIItemPtr pList = ItemGet();
		if( !pList )
			return E_FAIL;
		
		CGUIListItem * pLstItem = NULL;
		HRESULT hResult = CGUIListItem::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pLstItem);
		if( FAILED(hResult) )
			return hResult;

		pLstItem->Attach(m_pRoot, pList);
		pLstItem->m_nIdx = vId;
		*pItem = pLstItem;
		return S_OK;
	}

	_CCGUIItemPtr pItemBase = ItemGetByPos(vId);
	if( !pItemBase )
		return E_FAIL;

	CGUIItem * pGuiItem = NULL;
	HRESULT hResult = _CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pGuiItem);
	if( FAILED(hResult) )
		return hResult;

	pGuiItem->Attach(m_pRoot, pItemBase, pItemBase->m_strItemId.c_str(), cCP_ANSI);
	*pItem = pGuiItem;
	return S_OK;
}

STDMETHODIMP CGUIItem::GetCount(long *pCount)       { *pCount = (long)CallItemOp(GuiItemGetCount); return S_OK; }
STDMETHODIMP CGUIItem::Click()                      { PostItemOp(GuiItemClick); return S_OK; }
STDMETHODIMP CGUIItem::Select()                     { PostItemOp(GuiItemSelect); return S_OK; }
STDMETHODIMP CGUIItem::SetValueInteger(long newVal) { PostItemOp(GuiItemSetInt, (tPTR)newVal); return S_OK; }
STDMETHODIMP CGUIItem::SetValueString(BSTR newVal)  { PostItemOp(GuiItemSetStr, 0, 0, 0, newVal); return S_OK; }

//////////////////////////////////////////////////////////////////////
// CGUIListItem

STDMETHODIMP CGUIListItem::get_X(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiListItemGetRect, (tPTR)m_nIdx, (tPTR)m_nSubIdx, (tPTR)&rcRect);
	*pVal = rcRect.left;
	return S_OK;
}

STDMETHODIMP CGUIListItem::get_Y(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiListItemGetRect, (tPTR)m_nIdx, (tPTR)m_nSubIdx, (tPTR)&rcRect);
	*pVal = rcRect.top;
	return S_OK;
}

STDMETHODIMP CGUIListItem::get_Width(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiListItemGetRect, (tPTR)m_nIdx, (tPTR)m_nSubIdx, (tPTR)&rcRect);
	*pVal = RECT_CX(rcRect);
	return S_OK;
}

STDMETHODIMP CGUIListItem::get_Height(long *pVal)
{
	RECT rcRect = {0, }; CallItemOp(GuiListItemGetRect, (tPTR)m_nIdx, (tPTR)m_nSubIdx, (tPTR)&rcRect);
	*pVal = RECT_CY(rcRect);
	return S_OK;
}

STDMETHODIMP CGUIListItem::get_Text(BSTR * pVal)
{
	cGuiEvent pEvt(GuiListItemGetText);
	pEvt.m_nVal1 = (tPTR)m_nIdx;
	pEvt.m_nVal2 = (tPTR)(m_nSubIdx != -1 ? m_nSubIdx : 0);
	CallItemOpEx(&pEvt);
	
	*pVal = ::SysAllocString(pEvt.m_strVal1.data());
	return S_OK;
}

STDMETHODIMP CGUIListItem::get_Icon(BSTR *pVal)
{
	WCHAR strNum[MAX_4B_DIGIT_STRSIZE] = { L"" };
	if( m_nSubIdx == -1 )
		_ltow((int)CallItemOp(GuiListItemGetIcon, (tPTR)m_nIdx), strNum, 10);
	*pVal = ::SysAllocString(strNum);
	return S_OK;
}

STDMETHODIMP CGUIListItem::get_Check(long *pVal)
{
	*pVal = (long)CallItemOp(GuiListItemGetCheck, (tPTR)m_nIdx, m_nSubIdx != -1 ? (tPTR)m_nSubIdx : 0);
	return S_OK;
}

STDMETHODIMP CGUIListItem::get_Pos(long * pVal)
{
	*pVal = (m_nSubIdx != -1) ? m_nSubIdx : m_nIdx;
	return S_OK;
}

STDMETHODIMP CGUIListItem::GetItemByIdx(long vId, IDispatch **pItem)
{
	if( m_nSubIdx != -1 )
		return E_NOTIMPL;

	_CCGUIItemPtr pList = ItemGet();
	if( !pList )
		return E_FAIL;

	CGUIListItem * pLstSubItem = NULL;
	HRESULT hResult = CGUIListItem::_CreatorClass::CreateInstance(NULL, __uuidof(ICGUIItem), (void **)&pLstSubItem);
	if( FAILED(hResult) )
		return hResult;

	pLstSubItem->Attach(m_pRoot, pList);
	pLstSubItem->m_nIdx = m_nIdx;
	pLstSubItem->m_nSubIdx = vId;
	*pItem = pLstSubItem;
	return S_OK;
}

STDMETHODIMP CGUIListItem::GetCount(long *pCount)
{
	if( m_nSubIdx != -1 )
		return E_NOTIMPL;
	*pCount = (long)CallItemOp(GuiListItemGetColsCount);
	return S_OK;
}

STDMETHODIMP CGUIListItem::Click()
{
	if( m_nSubIdx != -1 )
		return E_NOTIMPL;
	PostItemOp(GuiListItemClick, (tPTR)m_nIdx);
	return S_OK;
}

STDMETHODIMP CGUIListItem::Select()
{
	if( m_nSubIdx != -1 )
		return E_NOTIMPL;
	PostItemOp(GuiListItemSelect, (tPTR)m_nIdx);
	return S_OK;
}

STDMETHODIMP CGUIListItem::SetValueInteger(long newVal)
{
	PostItemOp(GuiListItemSetCheck, (tPTR)m_nIdx, m_nSubIdx != -1 ? (tPTR)m_nSubIdx : 0, (tPTR)newVal);
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
	_GETFUNCNAME_ITEM(ItemGetPos);
	_GETFUNCNAME_ITEM(ItemIsChecked);
	_GETFUNCNAME_ITEM(ItemGetRect);
	_GETFUNCNAME_ITEM(ItemSetStr);
	_GETFUNCNAME_ITEM(ItemGetCount);
	_GETFUNCNAME_ITEM(ListItemClick);
	_GETFUNCNAME_ITEM(ListItemSelect);
	_GETFUNCNAME_ITEM(ListItemSetCheck);
	_GETFUNCNAME_ITEM(ListItemGetRect);
	_GETFUNCNAME_ITEM(ListItemGetText);
	_GETFUNCNAME_ITEM(ListItemGetIcon);
	_GETFUNCNAME_ITEM(ListItemGetCheck);
	_GETFUNCNAME_ITEM(ListItemGetColsCount);
	_GETFUNCNAME_ITEM(RootShowMainWnd);
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
	PR_TRACE((hObj, prtIMPORTANT, "GUIController: root(0x%p) func(%s) item(0x%p id:%S) parent(0x%p id:%s) vals_ptr(0x%p,0x%p,0x%p) vals_str(%S,%S,%S)",
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

	bool bExecFunc = false;
	CItemBase * pItem = pParams->m_pItem;
	if( !pItem || pItem->m_pRoot )
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

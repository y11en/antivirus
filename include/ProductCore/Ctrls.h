// ItemBase.h: interface for the CItemBased class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CTRLS_H_)
#define _CTRLS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum OSDCN_TYPE {OSDCN_NAME = 1, OSDCN_PATH = 2};
extern void   osd_CompressName(CRootItem * pRoot, OSDCN_TYPE eType, const tCHAR * pStr, HDC hDc, HFONT hFont, int nCX, tString& strDst);

#include "gridview.h"

//////////////////////////////////////////////////////////////////////
// CGroupItem

class CGroupItem : public CItemBase
{
public:
	CGroupItem();

	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void DrawBorder(HDC dc, RECT& rcClient);
	void DrawContent(HDC dc, RECT& rcClient);
	void OnAdjustClientRect(RECT& rcClient);

	void OnFocus(bool bFocusted)
	{
		CItemBase::OnFocus(bFocusted);

		for(size_t i = 0; i < m_aItems.size(); i++)
		{
			CItemBase * pItem = m_aItems[i];
			if( bFocusted )
				pItem->m_nState |= ISTATE_FOCUSED;
			else
				pItem->m_nState &= ~ISTATE_FOCUSED;
			pItem->Update();
		}
	}
	
public:
	int   m_nHeaderSize;
};

//////////////////////////////////////////////////////////////////////
// CRefsItem

class CRefsItem : public CItemBase
{
public:
	CRefsItem();
	~CRefsItem();
	
	void         Cleanup();
	bool         SetText(LPCSTR strText);
	CItemBase *  GetItem(LPCSTR strItemId, LPCSTR strItemType = NULL, bool bRecurse = true);
	tSIZE_T      GetChildrenCount();
	CItemBase *  GetByPos(tSIZE_T nIdx);
	void         SendEvent(tDWORD nEventId, cSerializable* pData, bool bBroadcast);
	virtual void UpdateRefsText(LPCSTR strText, bool bFull = true);

public:
	struct RefData
	{
		RefData() { memset(&m_chrRange, 0, sizeof(m_chrRange)); memset(&m_chrRangeEx, 0, sizeof(m_chrRangeEx)); }
		
		virtual CItemBase * _GetItem() = 0;
		virtual void        _SetProps(CItemProps& strProps) = 0;
		virtual void        _Init() = 0;
		virtual void        _UpdateProps(int flags, tDWORD nStateMask) = 0;

		POINT                m_chrRange;
		POINT                m_chrRangeEx;
	}; friend struct RefData;

protected:
	void    OnUpdateTextProp();
	LPCSTR  OnGetDisplayText();
	void    OnChangedData(CItemBase* pItem);

	virtual void OnApplyBlockStyles() = 0;

protected:
	void _RefsDeleteAll(std::vector<RefData *>& a);
	void _ApplyBlockStyles();
	void _UpdateTextAndLinks(tString &strText, int pos, int num);
	
	static LPCSTR _FindTagStr(LPCSTR str, LPCSTR strFind);

protected:
	virtual RefData *   _RefCreate() = 0;

	std::vector<RefData *> m_aRefs;
	std::vector<RefData *> m_aRefsDel;
	std::vector<POINT>     m_aBolds;
	tString                m_strDisplayText;
};

template <class RefBaseT = CItemBase>
class CRefsItemLinkT : public RefBaseT, public CRefsItem::RefData
{
public:
	using RefBaseT::GetOwner;
	using RefBaseT::AddRef;
	using RefBaseT::Release;
	
	~CRefsItemLinkT() { RefBaseT::m_pParent = NULL; }
	
	CItemBase * _GetItem() { return this; }
	void        _SetProps(CItemProps& aProps) { RefBaseT::InitProps(aProps, &CSinkCreateCtx(this)); }
	void        _Init() { RefBaseT::InitItem(NULL); }
	void        _UpdateProps(int flags, tDWORD nStateMask) { OnUpdateProps(flags, nStateMask, NULL); }

	void        OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc) { RefBaseT::OnUpdateProps(flags & ~(UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE), nStateMask, NULL); }
	void        Click(bool bForce) { AddRef(); RefBaseT::Click(bForce); Release(); }
};

//////////////////////////////////////////////////////////////////////
// CTreeItem

typedef tPTR hTreeItem;
typedef tPTR hImageList;

struct cTreeItem
{
	cTreeItem() : m_hItem(NULL), m_flags(0), m_nImage(0),
		m_nSelectedImage(0), m_nChildren(1), m_state(0), m_stateMask(0) {}
	virtual ~cTreeItem() {}

	enum
	{
		fIconStateAbsent            =  0x0000,
		fIconStateUnchecked         =  0x1000,
		fIconStateChecked           =  0x2000,
		fIconStateDisabledUnchecked =  0x3000,
		fIconStateDisabledChecked   =  0x4000,
		fIconStateUser0             =  0x5000,
		fIconStateMask              =  0xf000,
		fFullStateMask              =  0xffff,

		fStateDisabled              = 0x10000,
	};

	hTreeItem       m_hItem;
	
	tDWORD          m_flags;
	
	cStrObj         m_strName;
	tUINT           m_nImage;
	tUINT           m_nSelectedImage;
	tDWORD          m_state;
	tDWORD          m_stateMask;
	tUINT           m_nChildren;
};

class CTreeItem : public CItemBase
{
public:
	virtual cTreeItem * AppendItem(cTreeItem * pParent, cTreeItem * pItem, cTreeItem * pAfter = NULL) { return NULL; }
	virtual void        DeleteItem(cTreeItem * pItem) {}
	virtual cTreeItem * GetParentItem(cTreeItem * pItem) { return NULL; }
	virtual cTreeItem * GetChildItem(cTreeItem * pItem) { return NULL; }
	virtual cTreeItem * GetNextItem(cTreeItem * pItem) { return NULL; }
	virtual cTreeItem * GetSelectionItem() { return NULL; }
	virtual cTreeItem * TreeHitTest(POINT &pt, tUINT * pnFlags) { *pnFlags = 0; return NULL; }
	virtual void        SelectItem(cTreeItem * pItem) {}
	virtual void        ExpandItem(cTreeItem * pItem, bool bExpand = true) {}
	virtual void        EnsureVisibleItem(cTreeItem * pItem) {}
	virtual void        SetItemCheckState(cTreeItem * pItem, bool bChecked = true) {}
	virtual void        SortChildren(cTreeItem * pItem, bool bRecurse = false) {}
	virtual void        SetImageList(hImageList hImages) {}
	virtual bool        AddStateIcon(CIcon * pIcon) { return false; }
	
	virtual cTreeItem * Idx2Item(cTreeItem * pParent, int nIdx);
	virtual int         Item2Idx(cTreeItem * pParent, cTreeItem * pItem);
	
	virtual void		DeleteChildItems(cTreeItem * pParent);

protected:
	bool      IsAdjustable(bool bHorz){ return false; }
	bool      IsResizeble(bool bHorz) { return true; }
};

class CTreeItemSink : public CItemSinkT<CTreeItem>
{
public:
	virtual void OnUpdateItemProps(cTreeItem * pItem, tDWORD flags = UPDATE_FLAG_ALL) {}
	virtual bool OnItemExpanding(cTreeItem * pItem, tUINT& nChildren) { return true; }
	virtual bool OnItemChecking(cTreeItem * pItem) { return true; }
	virtual int  OnSortCompare(cTreeItem * pItem1, cTreeItem * pItem2) { return 0; }
	virtual void OnItemDestroy(cTreeItem * pItem) { delete pItem; }

public:
	struct CSuccessSel
	{
		CSuccessSel(CObjectInfo& pObject) : m_nSrcSuccSize(0), m_Object(pObject) {}
		
		tDWORD			m_nSrcSuccSize;
		CObjectInfo&	m_Object;
	};
	
	virtual bool InitBrowseInfo(CBrowseObjectInfo& pInfo) { return false; }
	virtual bool SetSelObject(CObjectInfo& pObject, CSuccessSel * pSuccessSel = NULL ) { return false; }
	virtual bool GetSelObject(CObjectInfo& pObject) { return false; }
};

//////////////////////////////////////////////////////////////////////
// CTabItem

class CTabItem : public CItemBase
{
public:
	typedef CItemBase TBase;
	
	CTabItem();

	void    ShowPageItem(CItemBase * pItem, bool bShow);
	bool    SetNextPage(bool bBackward = false);
	bool    SetCurPage(CItemBase * pPage);

protected:
	bool    IsAdjustable(bool bHorz) { return false; }
	bool    IsResizeble(bool bHorz) { return true; }
		    
	void    OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);
	void    OnSelect(CItemBase* pItem);
	void    OnSelectItem(CItemBase * pItem, bool bSelect);
	void    OnAdjustClientRect(RECT& rcClient);
	void    OnInited();

	void    SaveSettings(cAutoRegKey& pKey);
	void    LoadSettings(cAutoRegKey& pKey);

protected:
	tSIZE_T     _Page2Idx(CItemBase* pItem);
	CItemBase * _Idx2Page(tSIZE_T nIdx);
	void        _SetPage(tSIZE_T nPos, CItemBase* pItem);

protected:
	virtual tSIZE_T ctl_GetCurSel() = 0;
	virtual void   ctl_SetCurSel(tSIZE_T nIdx) = 0;
	virtual void   ctl_SetItem(tSIZE_T nIdx, LPCSTR strText, CItemBase * pItem) = 0;
	virtual tSIZE_T ctl_GetItemCount() = 0;
	virtual void   ctl_DeleteItem(tSIZE_T nIdx) = 0;
	virtual void   ctl_DeleteAllItems() = 0;
	virtual void   ctl_AdjustRect(RECT& rcClient) = 0;

protected:
	tSIZE_T   m_nCurItem;
};

//////////////////////////////////////////////////////////////////////
// CSlider

class CSliderItem : public CItemBase
{
public:
	typedef CItemBase TBase;
	
	CSliderItem();

	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	bool IsAdjustable(bool bHorz) { return false; }
	bool IsResizeble(bool bHorz) { return ctl_IsVert() ? !bHorz : bHorz; }

	bool IsOut() { return true; }
	tTYPE_ID GetType() { return tid_DWORD; }
	bool SetValue(const cVariant &pValue);
	bool GetValue(cVariant &pValue);

protected:
	virtual void ctl_SetRange(tSIZE_T nMin, tSIZE_T nMax) = 0;
	virtual void ctl_SetPos(tSIZE_T nPos) = 0;
	virtual tSIZE_T ctl_GetPos() = 0;
	virtual bool ctl_IsVert() = 0;

protected:
	void _DataChanged();
	
	tSIZE_T             m_nCurPos;
	POINT               m_ptLim;
};

//////////////////////////////////////////////////////////////////////
// CDialogItem

#define DLG_ACTION_OK          0x00001
#define DLG_ACTION_CANCEL      0x00002
#define DLG_ACTION_OKCANCEL    (DLG_ACTION_OK|DLG_ACTION_CANCEL)
#define DLG_ACTION_YES         0x00004
#define DLG_ACTION_NO          0x00008
#define DLG_ACTION_YESNO       (DLG_ACTION_YES|DLG_ACTION_NO)
#define DLG_ACTION_APPLY       0x00010
#define DLG_ACTION_NEXT        0x00020
#define DLG_ACTION_BACK        0x00040
#define DLG_ACTION_CLOSE       0x00080
#define DLG_ACTION_WIZARD      (DLG_ACTION_NEXT|DLG_ACTION_BACK|DLG_ACTION_CANCEL)

#define DLG_MODE_DLGBUTTONS    0x00100
#define DLG_MODE_BUTTONS       0x00FFF
#define DLG_MODE_NEWDATA       0x01000
#define DLG_MODE_ALWAYSMODAL   0x02000
#define DLG_MODE_ALWAYSNEXT    0x04000
#define DLG_MODE_NOHELP        0x08000
#define DLG_MODE_CENTERBUTTONS 0x10000
#define DLG_MODE_NOSAVEPOS     0x20000

#define DLG_ACTION_EXPAND      0x40000
#define DLG_ACTION_COLLAPSE    0x80000
#define DLG_MODE_EXPANDABLE    (DLG_ACTION_EXPAND|DLG_ACTION_COLLAPSE)
#define DLG_MODE_CUSTOMFOOTER 0x100000

#define DLG_STYLE_DRAGABLE     0x0001
#define DLG_STYLE_LAYERED      0x0002
#define DLG_STYLE_RESIZABLE    0x0004
#define DLG_STYLE_BOTTOM       0x0008 // place on the bottom
#define DLG_STYLE_DEFAULT      0x0010 // default size
#define DLG_STYLE_NOAUTOFOCUS  0x0020
#define DLG_STYLE_ROUNDED      0x0040

class CDialogItem : public CItemBase
{
public:
	typedef CItemBase TBase;
	
	enum eTrans {TRANS_NULL, TRANS_MED, TRANS_FULL};
	enum eShow {showNormal = 1, showMaximazed = 3, showMinimized = 6};

	CDialogItem();
	
	void Clear();
	bool SetNextPage(bool bBackward = false);
	void UpdateView(int nFlags = 0);

	unsigned     m_fModal : 1;
	unsigned     m_bCloseDirect : 1;

public:
	virtual void        Close(tDWORD nAction = DLG_ACTION_CANCEL) {}
	virtual bool        CloseConfirm();
	virtual void        SetShowMode(eShow eShowMode);
	virtual bool        IsActive() { return true; }
	virtual void        Activate() {}
	virtual tDWORD      DoModal();
	virtual void        SetDirty(bool bDirty = true);
	virtual void        EnableAction(tDWORD nAction, bool bEnable);
	virtual bool        SetNextDlgItem(bool bGroup, bool bBackward) { return false; }
	virtual void        SetTimeout(tDWORD nTimeout);
	virtual void        SetErrResult(tERROR err);
	virtual bool        IsActual() { return m_bActive || (m_nState & ISTATE_MOUSEOVER); }
	virtual void        ModalEnableParent(bool bEnable) {}
	virtual CItemBase * GetCurPage();
	virtual bool        SetCurPage(CItemBase * pPage);
	virtual bool        SetCurPage(const char * pPageId);
	virtual void        DoAutomate();
	virtual CFieldsBinder* GetBinder();
	virtual cNode *     CreateOperand(const char *name, cNode *container, cAlloc *al);
	virtual cNode *     CreateOperator(const char *name, cNode **args, cAlloc *al);

	virtual void		ModifyDlgFlags(tDWORD dwAdd, tDWORD dwRemove);

	virtual void        Expand(bool bExpand = true, bool bResize = true);

	bool IsExpandable() { return !!(m_nDlgFlags & DLG_MODE_EXPANDABLE); }
	bool IsExpanded() { return !!m_bExpanded; }
	void SetCtxItem(CItemBase *pCtxItem) { m_pCtxItem = pCtxItem; }

protected:
	void ApplyStyle(LPCSTR strStyle);
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void InitItem(LPCSTR strType);
	bool IsResizeble(bool bHorz);
	void ShowPageItem(CItemBase * pItem, bool bShow);

	void OnInit();
	void OnInited();
	bool OnClicked(CItemBase * pItem);
	void OnChangedData(CItemBase* pItem) { SetDirty(true); UpdateView(); TBase::OnChangedData(pItem); }
	void OnAdjustClientRect(RECT& rcClient) {}
	void TimerRefresh(tDWORD nTimerSpin);
	void LoadSettings(cAutoRegKey& pKey);
	void SaveSettings(cAutoRegKey& pKey);

	virtual void OnChangeShowMode(eShow eNewMode);

	bool _Apply();
	bool _Ok();
	bool _Next();
	void _ResetTimeout() { m_nAutoCloseTimeout = m_nTimeout; }
	void _InitButton(LPCSTR pBtnName, tDWORD nActions);
	void _InitDefaultRect(RECT& rcScreen, RECT& rcFrame);
	
	void _EnableSysClose(HWND hWnd, bool bEnable);
	bool _GetWindowPlacement(HWND hWnd, RECT& rcNormal, eShow& showCmd);
	bool _GetGetWorkArea(RECT& rc);
	tDWORD _GetSameWindowOffset();
	void _SetTransparent(eTrans eOp);
	
	virtual tUINT _GetDlgPosOffset() { return 0; }
	virtual bool _CheckDlgPosOffset() { return true; }
	virtual bool _CloseDlg() { return false; }
	virtual void _InitDlgRect(RECT& rcFrame) {}

protected:
	virtual void   ctl_SetTransparentValue(tBYTE nAlpha) = 0;
	virtual tDWORD ctl_SetTransparentTimer(tDWORD nInterval) = 0;
	virtual void   ctl_KillTransparentTimer(tDWORD nId) = 0;
	
	void   ctl_OnTransparentTimer();
	void   ctl_OnActivate(bool bActive);
	void   ctl_OnMouseMove();

protected:
	tDWORD       m_nDlgStyles;
	tDWORD       m_nDlgFlags;
	SIZE         m_szSizeDef;
	SIZE         m_szSizeExp;
	SIZE         m_szSizeMinTmp;
	RECT         m_rcRectLoad;
	tUINT        m_nRelativeLoad;
	CIcon *      m_pDlgIcon;
	cNode *      m_pNodeApply;
	
	tDWORD       m_nTimeout;
	tDWORD       m_nAutoCloseTimeout;

	CItemBase *  m_pCtxItem;
	CItemBase *  m_pFooter;
	CItemBase *  m_pOk;
	CItemBase *  m_pCancel;
	CItemBase *  m_pNo;
	CItemBase *  m_pApply;
	CItemBase *  m_pNext;
	CItemBase *  m_pBack;
	CItemBase *  m_pSheet;
	CItemBase *  m_pExpand;
	CItemBase *  m_pCollapse;
	CItemBase *  m_pWndResizer;

	unsigned     m_fDirty : 1;
	unsigned     m_bActive : 1;
	unsigned     m_bSmartTransparency : 1;
	unsigned     m_bUserClose : 1;
	unsigned     m_bMaximize : 1;
	unsigned     m_bExpanded : 1;
	tDWORD       m_nResult;

	tDWORD       m_nTransparencyTimerId;
	tDWORD       m_nTransparencyFrom;
	tDWORD       m_nTransparencyTo;
	tDWORD       m_nTransparency;

protected: // Wizard specific
	void _SelectItem(CItemBase * pItem, bool bSelect);
	tSIZE_T _GetNextIdx(tSIZE_T nFrom, bool bBackward = false);
	
	tSIZE_T m_nCurIdx;

protected:
	void   AddValidators(CItemBase * pItem);
	void   AddValidator(CItemProps& btnsProps, CItemBase * pItem, tDWORD nAction);
	tDWORD ProcessValidator(CItemBase * pItem, tDWORD nAction);
	void   _GetValidatorIds(CItemBase * pItem, tDWORD nAction, LPCSTR& strMacro, CItemBinding **& ppBinding);

	friend class cNodeWizardMask;
	friend class cNodeWizardPagesMask;
	friend class cNodeWizardGotoPage;
	
	MEMBER_NODE(cNodeIsTabbed)       m_nodeIsTabbed;

	struct eb
	{
		eb() : m_ok(NULL), m_cancel(NULL), m_next(NULL), m_prev(NULL) {}
		~eb() { clear(); }
		
		void clear();
		
		CItemBinding * m_ok;
		CItemBinding * m_cancel;
		CItemBinding * m_next;
		CItemBinding * m_prev;
	};

	typedef cVector<eb, cSimpleMover<eb> > ebs;

	eb	m_eb;
	ebs m_ebs;
};

class CDialogSink : public CItemSinkT<CDialogItem>
{
public:
	CDialogSink(tDWORD nActionsMask = 0) : m_nActionsMask(nActionsMask), m_pValidator(NULL) {}
	
	tDWORD DoModal(CItemBase* pParent, LPCSTR strSection = NULL, tDWORD nActionsMask = DLG_ACTION_OKCANCEL, bool bNew = false, bool bSingleton = false)
	{
		if( !m_nActionsMask )
			m_nActionsMask = nActionsMask;

		if( bNew )
			m_nActionsMask |= DLG_MODE_NEWDATA;

		if( !m_strSection.empty() )
			strSection = m_strSection.c_str();
		
		if( !pParent )
			return 0;
		CItemBase * pTopOwner = pParent->GetOwner(true);
		if( !pTopOwner || !pTopOwner->m_pRoot )
			return 0;
		
		CItemBase * pItem = NULL;
		{
			if( bSingleton )
			{
				while( pItem = ((CItemBase *)pTopOwner->m_pRoot)->GetByFlagsAndState(STYLE_TOP_ITEM, ISTATE_ALL, pItem) )
				{
					if( pItem->m_strItemId == strSection )
						break;
				}
			}
			else
			{
				int i, n;
				
				for(i = 0, n = (int)pTopOwner->m_aItems.size(); i < n; i++)
				{
					pItem = pTopOwner->m_aItems[i];
					if( (pItem->m_nFlags & STYLE_TOP_ITEM) &&
						pItem->m_strItemId == strSection )
						break;
				}
			
				if( i == n )
					pItem = NULL;
			}
			
			if( pItem )
			{
				((CDialogItem *)pItem)->Activate();
				return 0;
			}
		}

		pTopOwner->LoadItem(this, pItem, strSection, NULL, 0);
		if( !pItem )
			return 0;

		if( pItem->m_strItemType != GUI_ITEMT_DIALOG )
		{
			pItem->Destroy();
			return 0;
		}
		
		((CDialogItem*)pItem)->Activate();
		return Item()->DoModal();
	}

	void   Close(tDWORD nAction=DLG_ACTION_CANCEL){ Item()->Close(nAction); }

	virtual bool ReInit(CDialogSink* pFrom){ return true; }
	virtual void OnInitDialog(tDWORD& nFlags){ nFlags |= m_nActionsMask; }
	virtual bool OnOk(){ return true; }
	virtual bool OnClose(tDWORD& nResult){ return true; }
	virtual bool OnApply(){ return true; }
	virtual bool OnNext(){ return true; }
	virtual bool OnPrev(){ return true; }
	virtual tDWORD OnGetId(){ return 0; }
	
public:
	tString         m_strSection;
	tString         m_strItemId;
	tDWORD          m_nActionsMask;
	CDataValidator* m_pValidator;
};

//////////////////////////////////////////////////////////////////////
// CDialogControlItem

class CDialogControlItem : public CItemBase
{
public:
	CDialogControlItem()
	{
		m_nFlags |= STYLE_DRAW_BACKGROUND|STYLE_DRAW_CHILDS|STYLE_TRANSPARENT;
	}

	bool IsAdjustable(bool bHorz) { return false; }
	bool IsResizeble(bool bHorz)  { return true; }
};

//////////////////////////////////////////////////////////////////////
// CComboItem

class CComboItem : public CItemBase
{
public:
	typedef CItemBase TBase;
	
	CComboItem();

	virtual tDWORD  AddItem(LPCWSTR strName, cVar& v);
	virtual tDWORD  FindItem(LPCWSTR strName, const cVar& v);
	virtual void    SetDef(tDWORD nIdx);
	virtual tDWORD  GetItemsCount() { return m_aItems.size(); }
	virtual void    DeleteAllItems();
	virtual tDWORD  GetCurSel() { return ctl_GetCurSel(); }
	virtual void    SetCurSel(tDWORD nIdx) { ctl_SetCurSel(nIdx); _SelChanged(nIdx); }
	virtual cVar&   GetItemVal(tDWORD nIdx);
			   
protected:
	void        OnInit();
	void        ApplyStyle(LPCSTR strStyle);
	void        OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);
	bool        IsResizeble(bool bHorz) { return( bHorz ? TBase::IsResizeble(bHorz) : false ); }
	tTYPE_ID    GetType();
	bool        SetType(tTYPE_ID nType);
	bool        IsOut();
	bool        SetValue(const cVariant &pValue);
	bool        GetValue(cVariant &pValue);
	CItemBase * CreateChild(LPCSTR strItemId, LPCSTR strItemType);
	cNode *     CreateOperator(const char *name, cNode **args, cAlloc *al);

protected:
	virtual tSIZE_T ctl_GetCurSel() = 0;
	virtual void    ctl_SetCurSel(tSIZE_T nIdx) = 0;
	virtual tSIZE_T ctl_AddItem(LPCWSTR strName) = 0;
	virtual tSIZE_T ctl_GetItemsCount() = 0;
	virtual void    ctl_DeleteAllItems() = 0;
	virtual void    ctl_DeleteItem(tSIZE_T nIdx) = 0;
	virtual tSIZE_T ctl_FindByName(LPCWSTR strName) = 0;
	virtual void    ctl_GetTextByPos(tSIZE_T nIdx, cStrObj& strText) = 0;
	virtual void    ctl_GetText(cStrObj& strText) = 0;
	virtual void    ctl_SetText(LPCWSTR strText) = 0;
	virtual tPTR    ctl_GetItemData(tSIZE_T nIdx) = 0;
	virtual void    ctl_SetItemData(tSIZE_T nIdx, tPTR pData) = 0;

protected:
	class Item : public CItemBase
	{
	public:
		Item();
		
		void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);

		void Select(bool bSelect);
		bool SetValue(const cVariant &pValue);
		bool GetValue(cVariant &pValue);
		
		cVar     m_v;
		unsigned m_used : 1;
	}; friend class Item;

	void		_SelChanged(tSIZE_T nIdx);
	CItemBase *	_SelIdx2Item(tDWORD nSel);
	tSIZE_T		_Item2SelIdx(CItemBase * p);
	void		_SetItem(Item * pItem);
	void        _InitFromVector();
	
	tTYPE_ID          m_type;
	unsigned          m_edit : 1;
	unsigned          m_sorted : 1;
	static cVar       m_void;
};

//////////////////////////////////////////////////////////////////////
// CPopupMenuItem

#define POPUPMENU_SEP	((CItemBase *)-1)

class CPopupMenuItem : public CItemBase
{
public:
	CPopupMenuItem();
	
	void Cleanup();
	
	virtual void        SetFlags(tDWORD nFlags) { m_nLoadFlags = nFlags; }
	virtual void        SetPt(POINT& pt) { m_pt = pt; }
	virtual CItemBase * Track(POINT * pt = NULL, bool *bDataChanged = NULL);
	virtual void        TrackEnd(CItemBase * pItem = NULL);
	virtual bool        AddFromSection(LPCSTR strLoadInfo, CSinkCreateCtx *pCtx);
	virtual bool        SetText(LPCSTR strItem, LPCSTR strTextId);

protected:
	virtual HMENU  ctl_GetRootItem() = 0;
	virtual HMENU  ctl_AddItem(HMENU hMenu, CItemBase * pItem, bool bSub = false) = 0;
	virtual tDWORD ctl_Track(POINT& pt) = 0;
	virtual void   ctl_TrackEnd() = 0;
	virtual void   ctl_CleanSeps(HMENU hMenu) = 0;

protected:
	void OnInit();
	
	void _InitItems(CItemBase * pParent);
	void _InitPopup(HMENU hMenu, CItemBase * pParent);

	tDWORD      m_nLoadFlags;
	POINT       m_pt;
	CItemBase * m_pSelItem;
};

//////////////////////////////////////////////////////////////////////
// CTrayItem

class CTrayItem : public CItemBase
{
public:
	CTrayItem() { m_nFlags |= STYLE_CUSTOMCHILDREN; }

	virtual void        ShowBalloon(LPCSTR szText, LPCSTR szTitle, LPCSTR szValue = NULL, tDWORD uTimeout=1000, tDWORD dwIconType=0) = 0;
	virtual LPCSTR      GetBalloonText() = 0;
};

//////////////////////////////////////////////////////////////////////
// CEditItem

class CEditItem : public CItemBase
{
public:
	typedef CItemBase TBase;
	CEditItem();
	
	bool SetText(LPCSTR strText);
	
	virtual void   AddText(LPCSTR strText, bool bAppend = false);

protected:
	void     InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void     ApplyStyle(LPCSTR strStyle);
	bool     IsAdjustable(bool bHorz);
	bool     IsOut();
	bool     SetValue(const cVariant &pValue);
	bool     GetValue(cVariant &pValue);
	tTYPE_ID GetType();

	void     OnFocus(bool bFocusted);
	void     OnUpdateTextProp();

protected:
	virtual void ctl_ReplaceText(LPCWSTR strText, tSIZE_T nFrom, tSIZE_T nTo = -1) = 0;
	virtual void ctl_ReplaceSelText(LPCWSTR strText) = 0;
	virtual void ctl_SetSelText(tSIZE_T nFrom, tSIZE_T nTo, bool bAsync = false) = 0;
	virtual bool ctl_IsMultiline() = 0;
	virtual bool ctl_IsNumber() = 0;
	virtual bool ctl_IsReadonly() = 0;

protected:
	void _DataChanged();

protected:
	unsigned  m_nCompress : 2;
	unsigned  m_bDummyPasswdShow : 1;
	unsigned  m_bDummyPasswdSimpleShow : 1;
	unsigned  m_bChangeNotify : 1;
	unsigned  m_bTrimSpaces : 1;
	POINT     m_ptLim;
	tString	  m_strCompressed;
};

//////////////////////////////////////////////////////////////////////
// CIPAddrEditItem

class CIPAddrEditItem : public CItemBase
{
public:

protected:
	void     OnUpdateTextProp() {}

	tTYPE_ID GetType()                  { return tid_DWORD; }
	bool     IsOut()                    { return true; }
	bool     IsAdjustable(bool bHorz)   { return true; }

protected:
	void _DataChanged()
	{
		if( m_nState & ISTATE_INITED )
			OnChangedData(this);
	}
};

//////////////////////////////////////////////////////////////////////
// CRichEditItem

class CRichEditItem : public CRefsItem
{
public:
	typedef CRefsItemLinkT<CItemBase> TRef;

protected:
	RefData * _RefCreate() { return new TRef(); }

	bool IsAdjustable(bool bHorz){ return true; }
};

//////////////////////////////////////////////////////////////////////
// CStdinRedirectedSinkBase

class CStdinRedirectedSinkBase : public CItemSinkT<>
{
public:
	CStdinRedirectedSinkBase() :
		m_err(errOK),
		m_nProcessExitCode(0)
	{}

	virtual tERROR CreateProcess(LPCWSTR strParameters, LPCWSTR strBaseFolder, tDWORD nEvent) = 0;
	virtual tERROR ShellExec(HWND hWnd, LPCWSTR strExe, LPCWSTR strParameters, LPCWSTR strBaseFolder, tDWORD nEvent) = 0;
	virtual tERROR DoneProcess() = 0;
	virtual tERROR CheckProcessOutput() = 0;
	virtual void   GetResultText(tString& str) = 0;
	virtual void   DoneProxyProcess() = 0;
	virtual HANDLE GetChildHandle() = 0;

	tERROR      m_err;
	tDWORD      m_nProcessExitCode;
};

/////////////////////////////////////////////////////////////////////////////
// CShellTreeSinkBase

class CShellTreeSinkBase : public CTreeItemSink
{
public:
	CShellTreeSinkBase();

	bool InitBrowseInfo(CBrowseObjectInfo& pInfo);

	tDWORD              m_nMask;
	CBrowseObjectInfo * m_pInfo;
	cTreeItem *         m_pRoot;
};

/////////////////////////////////////////////////////////////////////////////
// CHTMLItem

class CHTMLItem : public CItemBase
{
public:
	virtual bool Navigate(LPCSTR szUrl) { return false; }
	virtual bool Submit(LPCSTR szFormName = NULL) { return false; }
};

/////////////////////////////////////////////////////////////////////////////
// CSplitterItem

class CSplitterItem : public CItemBase
{
public:
	CSplitterItem();
	// CItemBase
	void	 OnInited();
	void	 Resize(tDWORD cx, tDWORD cy, bool bDirect = true);
	bool	 SetValue(const cVariant &pValue);
	bool	 GetValue(cVariant &pValue);
	tTYPE_ID GetType() { return tid_DWORD; }
	//
	int		SetSplitBarSize(int iValue)	{ return m_cxySplitBar = iValue; }
	int		SetMinLength(int iValue)	{ return m_cxyMinLength = iValue; }
	void	SetRatio(float fRatio);
	
protected:
	tBOOL	m_bVertical;
	RECT	m_rcSplitter;
	RECT	m_rcBorder;
	float	m_ratio;
	int		m_cxySplitBar;	// splitter bar width/height
	int		m_cxyMinLength; // minimal width/height for control

private:
	void	SetSplitterPanes(bool bUpdate);
	void	UpdateSplitterLayout();
	void	GetSplitterPaneRect(tBOOL LeftTopPane, LPRECT lpRect);

};

/////////////////////////////////////////////////////////////////////////////
// CChartCtrlBase

class CChartCtrlBase : public CItemBase
{
public:
	CChartCtrlBase() {}

	virtual int DoCommand(LPCSTR szCmd, LPCSTR args) = 0;

protected:
	cNode * CreateOperator(const char *name, cNode **args, cAlloc *al);
};

class CReportSink : public CItemSink
{
public:
	virtual tERROR GetRecordCount( tDWORD* pdwRecords ) = 0;
	virtual tERROR GetRecord( tDWORD* result, tDWORD dwIndex, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CHexViewItem

class CHexViewItem : public CItemBase
{
public:
	CHexViewItem();

	// CItemBase
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void DrawContent(HDC dc, RECT& rcClient);
	bool SetValue(const cVariant &pValue);

	// CHexViewItem
	int  DataSize() { return (int)m_Data.size(); }
	void RecalcLayouts();
	tINT GetDataPos(POINT pt);
	void SetTopIndex(tINT topindex);
	
	void EnsureSelVisible(tINT selStart, tINT selEnd)
	{
		if( selStart == 0xffffffff )
			return;

		int BytesPerPage = m_LinesPerPage * m_BytesPerRow;

		while( !(selStart >= m_topindex && selStart <= m_topindex + BytesPerPage))
		{
			if( selStart < m_topindex)	ResetTopIndexTo(m_topindex - BytesPerPage);
			else						ResetTopIndexTo(m_topindex + BytesPerPage);
		}
		
		UpdateScrollBarPos();
	}

	virtual void UpdateScrollBars()		{}
	virtual void UpdateScrollBarPos()	{}

protected:
	cVector<tBYTE>	m_Data;		// binary data

	tINT	m_topindex;			// offset of first visible byte on screen
	tINT	m_BytesPerRow;		// byte per row 
	tINT	m_LinesPerPage;		// lines per page
	tINT	m_LastViewIndex;
	tINT	m_lineHeight;
	tINT	m_signWidth;

	tINT	m_offAddress,
			m_offHex,
			m_offAscii;
private:
	void DrawStr(HDC dc, char* sBuff, int left, int top);

	void ResetTopIndexTo(tINT topindex)
	{
		if(m_Data.size() == 0 )
			m_topindex = 0;
		else
		{
			m_topindex = topindex;
			if(m_topindex < 0 )
				m_topindex = 0; 
			if( m_topindex > m_LastViewIndex )
				m_topindex = m_LastViewIndex;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// CHexViewSink

class CHexViewSink : public CItemSink
{
public:
	CHexViewSink() : m_selStart(0xffffffff),  m_selEnd(0xffffffff) {}

	void SetCurSel(tINT start, tINT count, bool bEnsureVisible = false, bool bUpdate = true)
	{ 
		if(count > 0 )	{ 
			m_selStart = start; 
			m_selEnd = start + count - 1;	
			if(bEnsureVisible && m_pItem)
				((CHexViewItem*)m_pItem)->EnsureSelVisible(m_selStart, m_selEnd);
		}
		else	{ 
			m_selStart = m_selEnd = 0xffffffff; 
		}
		
		if(bUpdate)
			m_pItem->Update();
	}

	bool IsInRange(tINT pos)  { return ( pos >= m_selStart && pos <= m_selEnd) ? true : false; }

	// virtual
	virtual void OnPosSelected(int pos) {}
	virtual void OnNextPrev(bool bNext) {}
	
protected:
	tINT m_selStart,
		 m_selEnd;
};


//////////////////////////////////////////////////////////////////////

#endif // !defined(_CTRLS_H_)

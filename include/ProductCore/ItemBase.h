// ItemBase.h: interface for the CItemBased class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMBASE_H__92386C65_7130_41F0_A037_C7F9211E6DD9__INCLUDED_)
#define AFX_ITEMBASE_H__92386C65_7130_41F0_A037_C7F9211E6DD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ItemProps.h"
#include "Interpret.h"

//////////////////////////////////////////////////////////////////////

#define GUI_ITEMT_BASE              ""
#define GUI_ITEMT_BUTTON            "button"
#define GUI_ITEMT_BITMAPBUTTON      "bmpbutton"
#define GUI_ITEMT_LOCKBUTTON        "lockbutton"
#define GUI_ITEMT_CHECK             "check"
#define GUI_ITEMT_RADIO             "radio"
#define GUI_ITEMT_GROUP             "group"
#define GUI_ITEMT_BOX               "box"
#define GUI_ITEMT_HOTLINK           "hotlink"
#define GUI_ITEMT_PROGRESS          "progress"
#define GUI_ITEMT_COMBO             "combo"
#define GUI_ITEMT_TAB               "tab"
#define GUI_ITEMT_LISTCTRL          "list"
#define GUI_ITEMT_TREE              "tree"
#define GUI_ITEMT_PROPSHEETPAGE     "propsheetpage"
#define GUI_ITEMT_EDIT              "edit"
#define GUI_ITEMT_TIMEEDIT          "timeedit"
#define GUI_ITEMT_RICHEDIT          "richedit"
#define GUI_ITEMT_SLIDER            "slider"
#define GUI_ITEMT_IPADDRESS         "ipaddress"
#define GUI_ITEMT_DIALOG            "dialog"
#define GUI_ITEMT_TRAY              "tray"
#define GUI_ITEMT_MENU              "menu"
#define GUI_ITEMT_MENUITEM          "menuitem"
#define GUI_ITEMT_CREDITS           "credits"
#define GUI_ITEMT_HTML              "html"
#define GUI_ITEMT_SLIDESHOW         "slideshow"
#define GUI_ITEMT_CHART             "chart"
#define GUI_ITEMT_SPLITTER          "splitter"
#define GUI_ITEMT_HEXVIEWER         "hexviewer"

#define GUI_STR_AUTOSECT_SEP        '.'
#define GUI_STR_AUTOSECT_SEP_STR    "."
#define GUI_STR_ITEMIDSUFF_SEP_STR  "#"

//////////////////////////////////////////////////////////////////////

#define ISTATE_SELECTED             0x00000001
#define ISTATE_FOCUSED              0x00000002
#define ISTATE_HIDE                 0x00000008
#define ISTATE_CHANGED              0x00000010
#define ISTATE_DISABLEDEX           0x00000020
#define ISTATE_INITED               0x00000040
#define ISTATE_DISABLED             0x00000080
#define ISTATE_DEFAULT              0x00000100
#define ISTATE_FROZEN               0x00000200
#define ISTATE_HIDDENEX             0x00000400
#define ISTATE_SHOWTIP              0x00000800
#define ISTATE_HOTLIGHT             0x00001000
#define ISTATE_CLOSING              0x00002000
#define ISTATE_WRAPTIP              0x00004000
#define ISTATE_RESIZING             0x00008000
#define ISTATE_SHEETPAGE_HIDE       0x00010000
#define ISTATE_DRAW_CONTENT         0x00020000
#define ISTATE_PREINITED            0x00040000
#define ISTATE_MOUSEOVER            0x00080000
#define ISTATE_DISABLEDDEP          0x00100000
#define ISTATE_ANIMATE              0x00200000
#define ISTATE_DATACHANGING         0x00400000
#define ISTATE_NEED_UPDATE          0x00800000
#define ISTATE_CUSTOM               0x01000000
#define ISTATE_ALL                  0xFFFFFFFF

#define ALIGN_LEFT                  0x00000000
#define ALIGN_TOP                   0x00000000
#define ALIGN_RIGHT                 0x00000001
#define ALIGN_BOTTOM                0x00000002
#define ALIGN_HCENTER               0x00000004
#define ALIGN_VCENTER               0x00000008
#define ALIGN_HBEFORE               0x00000010
#define ALIGN_HAFTER                0x00000020
#define ALIGN_VBEFORE               0x00000040
#define ALIGN_VAFTER                0x00000080
#define ALIGN_HLINKED               (ALIGN_HBEFORE|ALIGN_HAFTER|ALIGN_HSAME)
#define ALIGN_VLINKED               (ALIGN_VBEFORE|ALIGN_VAFTER|ALIGN_VSAME)
#define ALIGN_LINKED                (ALIGN_HLINKED|ALIGN_VLINKED)
#define ALIGN_PARENT                (ALIGN_RIGHT|ALIGN_BOTTOM|ALIGN_VCENTER|ALIGN_HCENTER)
#define ALIGN_HSAME                 0x00000100
#define ALIGN_VSAME                 0x00000200
#define ALIGN_NOTTOP                (ALIGN_VLINKED|ALIGN_BOTTOM|ALIGN_VCENTER)
#define ALIGN_NOTLEFT               (ALIGN_HLINKED|ALIGN_RIGHT|ALIGN_HCENTER)
#define ALIGN_NOTTOPLEFT            (ALIGN_NOTTOP|ALIGN_NOTLEFT)
#define ALIGN_REVERSE_X             (ALIGN_HBEFORE|ALIGN_RIGHT)
#define ALIGN_REVERSE_Y             (ALIGN_VBEFORE|ALIGN_BOTTOM)
#define ALIGN_REVERSE               (ALIGN_REVERSE_X|ALIGN_REVERSE_Y)
#define ALIGN_MASK                  (ALIGN_LEFT|ALIGN_TOP|ALIGN_RIGHT|ALIGN_BOTTOM|ALIGN_HCENTER|ALIGN_HBEFORE|ALIGN_HAFTER|ALIGN_VBEFORE|ALIGN_VAFTER|ALIGN_HLINKED|ALIGN_VLINKED|ALIGN_LINKED|ALIGN_PARENT|ALIGN_HSAME|ALIGN_VSAME|ALIGN_NOTTOP|ALIGN_NOTLEFT|ALIGN_NOTTOPLEFT|ALIGN_REVERSE_X|ALIGN_REVERSE_Y)
#define ALIGN_TEXT_MASK             (ALIGN_VCENTER|ALIGN_RIGHT|ALIGN_BOTTOM|ALIGN_HCENTER|TEXT_SINGLELINE)

#define AUTOSIZE_X                  0x00000400
#define AUTOSIZE_Y                  0x00000800
#define AUTOSIZE                    (AUTOSIZE_X|AUTOSIZE_Y)
#define AUTOSIZE_PARENT_X           0x00001000
#define AUTOSIZE_PARENT_Y           0x00002000
#define AUTOSIZE_PARENT             (AUTOSIZE_PARENT_X|AUTOSIZE_PARENT_Y)

#define TEXT_SINGLELINE             0x00004000
#define TEXT_IS_VALUE               LONG_LONG_CONST(0x0000008000000000)

#define STYLE_WINDOW                0x00040000
#define STYLE_NON_OWNER             0x00080000
#define STYLE_DRAW_CONTENT          0x00100000
#define STYLE_DRAW_BACKGROUND       0x00200000
#define STYLE_DRAW_CHILDS           0x00400000
#define STYLE_NO_CLIPING            0x40000000
#define STYLE_EXT                   0x00010000
#define STYLE_EXTPRM                LONG_LONG_CONST(0x0000200000000000)
#define STYLE_SKIP_UPDATEDATA       LONG_LONG_CONST(0x0000000100000000)
#define STYLE_SHEET_ITEM            LONG_LONG_CONST(0x0000000200000000)
#define STYLE_FIX                   LONG_LONG_CONST(0x0000000400000000)
#define STYLE_BODY                  LONG_LONG_CONST(0x0000000800000000)
#define STYLE_NOPARENTDISABLE       LONG_LONG_CONST(0x0000001000000000)
#define STYLE_IFLASTNOEXTEND        LONG_LONG_CONST(0x0000002000000000)
#define STYLE_DESTROY_ICON          LONG_LONG_CONST(0x0000004000000000)
#define STYLE_REMEMBER_DATA         LONG_LONG_CONST(0x0000010000000000)
#define STYLE_UNDERLINEOFF          LONG_LONG_CONST(0x0000020000000000)
#define STYLE_BUTTON_LIKE           LONG_LONG_CONST(0x0000040000000000)
#define STYLE_CHECKBOX_LIKE         LONG_LONG_CONST(0x0000080000000000)
#define STYLE_NOACTIVATE            LONG_LONG_CONST(0x0000100000000000)
#define STYLE_DYNAMIC_SHOW          LONG_LONG_CONST(0x0000400000000000)
#define STYLE_EXT_ITEM              (STYLE_TOP_ITEM|STYLE_EXT)
#define STYLE_OWN_PLACEMENT         LONG_LONG_CONST(0x0000800000000000)
#define STYLE_SHEET_PAGES           LONG_LONG_CONST(0x0001000000000000)
#define STYLE_CUSTOMCHILDREN        LONG_LONG_CONST(0x0002000000000000)

#define STYLE_DEBUG                 0x00008000
#define STYLE_ENUM                  0x00800000
#define STYLE_ENUM_VALUE            0x01000000
#define STYLE_NOBINDHIDE            0x02000000
#define STYLE_DESTROYSINK           0x04000000
#define STYLE_TOP_ITEM              0x08000000
#define STYLE_POLICY_LB             0x10000000
#define STYLE_TRANSPARENT           0x20000000
#define STYLE_CLICKABLE             0x80000000

// Unused
#define STYLE_UNUSED_LIST_ITEM      0x00020000
#define STYLE_UNUSED_51             LONG_LONG_CONST(0x0004000000000000)
#define STYLE_UNUSED_52             LONG_LONG_CONST(0x0008000000000000)
#define STYLE_UNUSED_53             LONG_LONG_CONST(0x0010000000000000)
#define STYLE_UNUSED_54             LONG_LONG_CONST(0x0020000000000000)
#define STYLE_UNUSED_55             LONG_LONG_CONST(0x0040000000000000)
#define STYLE_UNUSED_56             LONG_LONG_CONST(0x0080000000000000)
#define STYLE_UNUSED_57             LONG_LONG_CONST(0x0100000000000000)
#define STYLE_UNUSED_58             LONG_LONG_CONST(0x0200000000000000)
#define STYLE_UNUSED_59             LONG_LONG_CONST(0x0400000000000000)
#define STYLE_UNUSED_60             LONG_LONG_CONST(0x0800000000000000)
#define STYLE_UNUSED_61             LONG_LONG_CONST(0x1000000000000000)
#define STYLE_UNUSED_62             LONG_LONG_CONST(0x2000000000000000)
#define STYLE_UNUSED_63             LONG_LONG_CONST(0x4000000000000000)
#define STYLE_UNUSED_64             LONG_LONG_CONST(0x8000000000000000)

// Text only flags (used with ALIGN_... flags to draw text)
#define TEXT_MODIFY                 0x02000000
#define TEXT_CALCSIZE               0x04000000
#define TEXT_OPAQUE                 0x08000000
#define TEXT_GRAYED                 0x10000000
#define TEXT_HOTLIGHT               0x20000000

#define STYLE_ALL                   LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF)

#define SINGLELINE_MARGSIZE	        1

#define UPDATE_FLAG_TEXT            0x00001
#define UPDATE_FLAG_FONT            0x00002
#define UPDATE_FLAG_ICON            0x00004
#define UPDATE_FLAG_ICONSELECTED    0x00008
#define UPDATE_FLAG_BACKGROUND      0x00010
#define UPDATE_FLAG_POSITION        0x00020
#define UPDATE_FLAG_SIZE            0x00040
#define UPDATE_FLAG_SUBCOUNT        0x00080
#define UPDATE_FLAG_STATE           0x00100
#define UPDATE_FLAG_DATA            0x00200
#define UPDATE_FLAG_NO_UPDATE       0x10000
#define UPDATE_FLAG_ALL             0xFFFF

#define LOAD_FLAG_PRELOAD           0x0001
#define LOAD_FLAG_REPLACE           0x0002
#define LOAD_FLAG_DESTROY_SINK      0x0004
#define LOAD_FLAG_NOSHOW            0x0008
#define LOAD_FLAG_INFO              0x0010
#define LOAD_FLAG_EDIT              0x0020

#define UNDEF_VALUE                 (-2)
#define MAX_CTL_SIZE                0x7FFF

#define LOAD_MENU_FLAG_SHOW         0x0001
#define LOAD_MENU_FLAG_DESTROY      0x0002
#define LOAD_MENU_FLAG_DEFITEM      0x0004
#define LOAD_MENU_FLAG_BOTTOMOFITEM 0x0008

#define ENUMITEMS_BYORDER           0x01
#define ENUMITEMS_BACKWARD          0x02
#define ENUMITEMS_SKIPCHILDREN      0x04
#define ENUMITEMS_LOOPED            0x08
#define ENUMITEMS_TAB_ACTIVE        0x10
#define ENUMITEMS_TAB_GROUP         0x20

#define TIMER_REFRESH_INT           100 // ms

#define SHOWITEM_NORECALC           0x01
#define SHOWITEM_DIRECT             0x02

#define pmTEST_GUI                  0x164eeb39

#define GUI_TIMERSPIN_INTERVAL( _ms ) (!(nTimerSpin % ((_ms)/TIMER_REFRESH_INT)))

#define _ITEM_COMPARE(_ItemId)                     (pItem->m_strItemId == _ItemId || pItem->m_strItemAlias == _ItemId || pItem->m_strItemType == _ItemId)
#define _ITEM_COMPARE_EX(_ItemId, _ItemType)       (_ITEM_COMPARE(_ItemId) && pItem->m_strItemType == _ItemType)

#define SINK_MAP_DECLARE()                         virtual void OnInitBinding(CItemBase* pItem, CSinkCreateCtx * pCtx);
#define SINK_MAP_BEGIN()                           virtual void OnInitBinding(CItemBase* pItem, CSinkCreateCtx * pCtx){
#define SINK_MAP_BEGINEX(_ThisClass)               void _ThisClass::OnInitBinding(CItemBase* pItem, CSinkCreateCtx * pCtx){
#define SINK_STATIC(_ItemId, _Sink)                if( _ITEM_COMPARE(_ItemId) ) { pItem->AttachSink(&_Sink, false); }
#define SINK_STATIC_EX(_ItemId, _ItemType, _Sink)  if( _ITEM_COMPARE_EX(_ItemId, _ItemType) ) { pItem->AttachSink(&_Sink, false); }
#define SINK_CUSTOM(_ItemId, _Func)                if( _ITEM_COMPARE(_ItemId) ) { _Func; }
#define SINK_CUSTOM_EX(_ItemId)                    if( _ITEM_COMPARE(_ItemId) )
#define SINK_DYNAMIC(_ItemId, _Sink)               if( (!pItem->m_pSink /*|| !(pItem->m_nFlags & STYLE_DESTROYSINK)*/) && _ITEM_COMPARE(_ItemId) ) { pItem->AttachSink(new _Sink, true); }
#define SINK_DYNAMIC_EX(_ItemId, _ItemType, _Sink) if( (!pItem->m_pSink /*|| !(pItem->m_nFlags & STYLE_DESTROYSINK)*/) && _ITEM_COMPARE_EX(_ItemId, _ItemType) ) { pItem->AttachSink(new _Sink, true); }
#define SINK_DYNAMIC_TOPITEM(_Sink)                if( (!pItem->m_pSink /*|| !(pItem->m_nFlags & STYLE_DESTROYSINK)*/) && (pItem->m_nFlags & STYLE_TOP_ITEM) ) { pItem->AttachSink(new _Sink, true); }
#define SINK_ITEM(_ItemId, _pMember)               if( _ITEM_COMPARE(_ItemId) ) _pMember = pItem;
#define SINK_MAP_END(_BaseClass)                   _BaseClass::OnInitBinding(pItem, pCtx); }

#define SINK_STATIC_LIST(_ItemId, _Sink)           SINK_STATIC_EX(_ItemId, GUI_ITEMT_LISTCTRL, _Sink)

#define tid_UNK		((tTYPE_ID)0)

#define MEMBER_NODE(_Class) class _Class : public cNode { public: void Destroy() {} bool IsConst() const { return false; } void Exec(cVariant &value, cNodeExecCtx &ctx, bool get); }; friend class _Class; _Class

inline void debugBreak() {}

//////////////////////////////////////////////////////////////////////

class  CItemProps;
class  CItemPropVals;
class  CItemSink;
struct CSinkCreateCtx;
struct CItemBinding;
class  CPopupMenuItem;
struct CObjectInfo;
struct CBrowseObjectInfo;
struct CFieldsBinder;
struct CStructData;
struct CStructId;
class  cNode;
class  cNodeExecCtx;
class  cSerFieldAcc;

struct cStrData
{
	cStrData(LPCSTR str = NULL, tDWORD len = -1) : p(str), n(len) {}
	cStrData(const tString& str) : p(str.c_str()), n(str.size()) {}
	cStrData(const cStrBuff& str) : p(str), n(str.used() ? str.used() - 1 : 0) {}
	int cmp(LPCSTR str, tDWORD len = -1) const;
	int cmp(const tString& str) const { return cmp(str.c_str(), str.size()); }
	
	LPCSTR p; tDWORD n;
};

//////////////////////////////////////////////////////////////////////
// CItemBase

class CValueBase
{
public:
	virtual tTYPE_ID GetType() { return tid_STRING_OBJ; }
	virtual bool SetType(tTYPE_ID nType) { return false; }
	virtual bool IsOut() { return false; }
	virtual bool IsVector() { return false; }
	virtual bool SetValue(const cVariant &pValue) { return false; }
	virtual bool GetValue(cVariant &pValue) { return false; }
	virtual void SetBinder(CFieldsBinder* pBinder) {}

	bool SetInt(tQWORD val)
	{
		bool bOk = false;
		switch( GetType() )
		{
		case tid_BOOL:
		case tid_DWORD:
		case tid_QWORD: { bOk = SetValue(cVariant(val)); } break;
		}
		return bOk;
	}

	bool GetInt(tQWORD& val)
	{
		bool bOk = false;
		switch( GetType() )
		{
		case tid_BOOL:
		case tid_DWORD:
		case tid_QWORD: { cVariant res; bOk = GetValue(res); val = (tQWORD)res.ToInt(); } break;
		}
		return bOk;
	}
	
	tQWORD GetInt()
	{
		tQWORD nRes = 0; GetInt(nRes);
		return nRes;
	}
};

//////////////////////////////////////////////////////////////////////

class CMacroResolver
{
public:
	virtual bool Resolve(tDWORD nFmtId, LPCSTR strMacro, tString& strResult)=0;
};

//////////////////////////////////////////////////////////////////////

class CDataValidator
{
public:
	virtual bool Validate(CItemBase *pItem, cSerializable *pData) = 0;
};

//////////////////////////////////////////////////////////////////////

class CItemEditData
{
public:
	struct Cd
	{
		bool	IsValid() { return !!m_strId.size(); }
		
		tString		m_strId;
		tString		m_strProps;
		tString		m_strText;
		tString		m_strTipText;

		cVector<Cd, cSimpleMover<Cd> >
					m_aItems;
	};

public:
	static void Init(CItemBase * pItem, CItemProps& _Props, const tString& strItemOrigSection);

	void	Destroy();
	
	virtual CItemProps&	GetOverrideOriginalInfo(CItemProps& _Props, tString& strText);
	virtual CItemProps&	GetBaseInfo(CItemProps& _Props);
	virtual void	    GetOverridedRawInfo(tString& strProps, tString& strText);
	virtual CItemBase * DeleteItem(CItemBase * pItem);
	virtual void        CopyData(CItemBase* pItem, Cd& _data);
	
public:
	bool IsLinked();		// Ёлмемент €вл€етс€ ссылкой на внешнюий элемен
	bool IsOverridable();	// Ёлмемент €вл€етс€ переопредел€емым в ссылочном внешнем элементе

public:
	// View information
	CItemBase * m_pItem;
	cStrObj		m_strOldGroup;
	tString		m_strOverridableOrigSection;
	unsigned	m_bLinked : 1;
	unsigned	m_bOverridable : 1;
	CItemBase * m_pAttrOwner;
	CItemBase * m_pAttrOwnerParent;
	CItemBase * m_pTop;
	CItemBase * m_pOverridableOwner;
	tString     m_strOverrideId;

	// Changed by editor
	cStrObj		m_strGroup;
	tString		m_strSection;
	tString		m_strItemId;
	tString		m_strProps;
	tString		m_strText;
	tString		m_strTipText;
	CItemProps	m_Props;
	unsigned	m_bExtProp : 1;
	unsigned	m_bTextSimple : 1;

	std::vector<tString> m_aDelSects;
};

inline bool CItemEditData::IsLinked() { return m_bLinked; }
inline bool CItemEditData::IsOverridable() { return m_bOverridable; }

//////////////////////////////////////////////////////////////////////

class CItemBase : public CValueBase, public CMacroResolver, public cNode::cNodeFactory
{
public:
	friend class CRootItem;
	friend class CItemEditData;
	friend struct cNodeUpdateView;

	enum eGbfasMode{gbfasNoRecurse = 0, gbfasRecurse = 1, gbfasUpToParent = 2, gbfasBackward = 0x10000};
	
	typedef std::vector<CItemBase*> Items;

	struct Override
	{
		void clear() { m_strProps.erase(); m_strText.erase(); m_aItems.clear(); }
		
		tString		m_strId;
		tString		m_strProps;
		tString		m_strText;
		tString		m_strTipText;

		cVector<Override, cSimpleMover<Override> > m_aItems;
	};

	typedef cVector<Override, cSimpleMover<Override> > Overrides;

public:
	CItemBase();
	virtual ~CItemBase() {}

	virtual void AddRef();
	virtual void Release();
	virtual void Destroy();

	virtual bool LoadItem(CItemSink* pSink, CItemBase*& pItem, LPCSTR strSection = NULL, LPCSTR strItemId = NULL, tDWORD nFlags = 0, CSinkCreateCtx * pCtx = NULL);
	virtual bool MoveItem(CItemBase* pRelativeItem, CItemBase* pItem, bool bAfter = false);
	virtual bool AttachSink(CItemSink* pSink, bool bDestroy);
	virtual bool AddItem(CItemBase* pItem);
	virtual CItemBase* CloneItem(CItemBase* pClone, LPCSTR strItemId, bool bUseIni = false);
	virtual void RemoveItem(CItemBase* pItem);
	virtual void Cleanup();
	virtual void Clear();
	virtual void ReLoad();
	virtual bool SetText(LPCSTR strText);
	virtual void SetFont(CFontStyle* pFont);
	virtual void SetIcon(CIcon* pIcon);
	virtual void SetIcon(LPCSTR strIcon);
	virtual void SetBackground(CBackground* pBackground);
	virtual void SetBackground(LPCSTR strBackground);
	virtual bool SetTipText(LPCSTR strText);

	virtual LPCSTR     GetText(){ return m_strText.c_str(); }
	virtual LPCSTR     GetDisplayText(){ return OnGetDisplayText(); }
	virtual bool       GetTipInfo(POINT& pt, tString* pText, RECT* pRect) { return false; }
	virtual LPCSTR     LoadLocString(tString& str, LPCSTR strId, bool bRaw = false, bool * bFromGlobal = NULL);

	virtual tSIZE_T    GetChildrenCount();
	virtual CItemBase* GetOwner(bool bTop = false, LPCSTR strItemId = NULL);
	virtual CItemBase* GetByPos(tSIZE_T nIdx);
	virtual CItemBase* GetBottomItem();
	virtual CItemBase* GetNext(bool bVisible = true);
	virtual CItemBase* GetPrev(bool bVisible = true);
	virtual CItemBase* GetChildFirst(bool bVisible = true);
	virtual CItemBase* GetChildLast(bool bVisible = true);
	virtual CItemBase* GetItem(const cStrData &strItemId, LPCSTR strItemType = NULL, bool bRecurse = true);
	virtual CItemSink* GetSink(LPCSTR strItemId, LPCSTR strItemType = NULL, bool bRecurse = true);
	virtual CItemBase* GetNextItem(CItemBase * pPrev, const cStrData &strItemId, LPCSTR strItemType = NULL, bool bRecurse = true, bool bBackward = false);
	virtual CItemBase* EnumNextItem(CItemBase * pPrev, tDWORD nFlags = 0);
	virtual CItemBase* GetDefItem();
	virtual tDWORD     GetItemId();
	virtual CItemBase* GetById(tDWORD nId);
	virtual CItemBase* GetByFlagsAndState(tQWORD nFlags = STYLE_ALL, tDWORD nState = ISTATE_ALL, CItemBase * pPrev = NULL, tDWORD nMode = gbfasRecurse);
	virtual CItemBase* GetItemByPath(LPCSTR strItemPath, bool bGoUp = true, bool bExact = false);
	virtual CItemBase* GetItem(HWND hWnd, CItemBase * pPrev = NULL, bool bBackward = false);
	virtual CPopupMenuItem * LoadItemMenu(POINT * ptMenu, tDWORD nFlags = LOAD_MENU_FLAG_SHOW|LOAD_MENU_FLAG_DESTROY, CItemBase * pPlaceItem = NULL, CSinkCreateCtx *pCtx = NULL);

	virtual bool IsParent(CItemBase* pParent);
	virtual long IsChecked(LPCSTR strItem = NULL);
	virtual bool IsEnabled(LPCSTR strItem = NULL);
	virtual bool IsVisible(LPCSTR strItem = NULL);
	virtual bool IsInited(){ return m_nState & ISTATE_INITED ? true : false; }
	virtual bool IsOwner(){ return (m_nFlags & STYLE_WINDOW) && !(m_nFlags & STYLE_NON_OWNER); }

	virtual void SendEvent(tDWORD nEventId, cSerializable* pData, bool bBroadcast);
	virtual void SetChangedData() { OnChangedData(this); }

	virtual HWND GetWindow(){ return 0; }
	virtual void GetClientRectEx(RECT& rcClient, bool bMargined, bool bCorrect = false);
	virtual void GetParentRect(RECT& rcParent, CItemBase* pOwner=NULL);
	virtual bool GetUpdateRectEx(RECT& rcFrame, RECT* rcUpdate, CItemBase* pOwner=NULL);
	virtual CItemBase* GetItemWithBackground();

	virtual bool Clip(HDC dc, RECT* rcUpdate, CItemBase* pOwner=NULL);
	virtual bool Draw(HDC dc, RECT* rcUpdate);
	virtual void DrawItem(HDC dc, RECT& rcFrame);
	virtual void DrawChildren(HDC dc, RECT* rcUpdate);
	virtual bool DrawParentBackground(HDC dc, RECT * rcDraw = NULL);
	virtual void DrawBackground(HDC dc, RECT& rcClient, RECT * prcClip = NULL);
	virtual void DrawBorder(HDC dc, RECT& rcClient);
	virtual void DrawContent(HDC dc, RECT& rcClient);
	virtual void DrawIconText(HDC dc, RECT& rc);

	virtual void Show(bool bShow, tDWORD nFlags = 0);
	virtual void Show(LPCSTR strItem, bool bShow);
	virtual void Enable(bool bEnable);
	virtual void Enable(LPCSTR strItem, bool bOn, bool bDisableOnly = false);
	virtual void EnableDep(bool bOn);
	virtual void Freeze(bool bFreeze);
	virtual void Focus(bool bFocus);
	virtual void Hotlight(bool bOn);
	virtual void Select(bool bSelect);
	virtual void SetDefault(bool bOn);
	virtual void InvalidateEx(RECT *rc){}
	virtual void Update(RECT *rc=NULL);
	virtual void UpdateView(int nFlags = 0){}
	virtual void UpdateProps(int nFlags, tPTR pData);
	virtual void Resize(tDWORD cx, tDWORD cy, bool bDirect = true);
	virtual void SetRect(RECT rc);
	virtual void Reoffset();
	virtual void Click(bool bForce = false, bool rClick = false);
	virtual void DblClick();
	virtual void Select();
	virtual void TimerRefresh(tDWORD nTimerSpin);
	virtual void LockUpdate(bool bLock, bool bUpdate = true);
	virtual void SetInt(tQWORD val, bool bNotify = false);
	virtual void RecalcItemsLayout(bool bSize);
	virtual CItemBase * GetNextChildTabstop(CItemBase * pPrev, tDWORD nFlags = 0);
	virtual bool IsSameTabGroup(CItemBase * pItem);
	
	virtual void ShowPageItem(CItemBase * pItem, bool bShow) {}
	virtual bool SetNextPage(bool bBackward = false);
	virtual CItemBase * GetCurPage();
	virtual bool SetCurPage(CItemBase * pPage);
	
	virtual LPCSTR GetOwnerRelativePath(tString& strPath, CItemBase * pOwner, bool bFullPath = true, LPCSTR strSuffix = NULL, CItemBase * p = NULL);
	virtual CItemBase * HitTest(POINT& pt, CItemBase * pInParent = NULL, bool bClickable = false);
	virtual void RecalcLayout(bool bSize = true);

public:
	virtual void InitBinding(LPCSTR sFmtText, const cSerDescriptor* pDesc = NULL);
	virtual void UpdateData(bool bIn, CStructData *pData = NULL);
	virtual void AddDataSource(cSerializable* pData, LPCSTR strSect = NULL, bool bAddFirst = false);
	virtual CFieldsBinder* GetBinder();
	virtual cSerFieldAcc * GetValueHolder();

protected:
	void         OnChangedProp(tDWORD nPropMask);

public:
	tTYPE_ID     GetType();
	bool         SetType(tTYPE_ID nType);
	bool         IsOut();
	bool         SetValue(const cVariant &pValue);
	bool         GetValue(cVariant &pValue);
	bool         IsVector();

	void         GetMinSize(SIZE& szSizeMin);
	bool         Resolve(tDWORD nFmtId, LPCSTR strMacro, tString& strResult);

protected:
	virtual CItemBase* CreateChild(LPCSTR strItemId, LPCSTR strItemType){ return NULL; }
	virtual void InitItem(LPCSTR strType);
	virtual void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	virtual void InitSize();
	virtual void ApplyStyle(LPCSTR strStyle);
	virtual void GetAddSize(SIZE& szAdd);
	virtual bool IsResizeble(bool bHorz);
	virtual bool IsDrawInParent();
	virtual tDWORD GetFontFlags();
	virtual void SetTabOrder(CItemBase * pItem, bool bAfter) {}
	virtual bool IsTabstop() { return false; }

	virtual void SaveSettings(cAutoRegKey& pKey);
	virtual void LoadSettings(cAutoRegKey& pKey);

	virtual void OnAdjustClientRect(RECT& rcClient){}
	virtual void OnAdjustUpdateRect(RECT& rcUpdate);
	virtual void OnInit(){}
	virtual void OnInited();
	virtual void OnFocus(bool bFocusted);
	virtual void OnSyncProps() {}
	virtual void OnUpdateProps(int flags = UPDATE_FLAG_ALL, tDWORD nStateMask = 0, RECT * rc = NULL);
	virtual LPCSTR OnGetDisplayText();
	virtual void OnCalcTextSize(HDC dc, SIZE& sz, tDWORD nFlags);
	virtual void OnInitBinding(CItemBase* pItem, CSinkCreateCtx * pCtx){}
	virtual void OnUpdateTextProp() {}
	virtual void OnAdjustBordSize(RECT& rc) {}
	virtual void OnDeInit() {}
	
public:
	virtual void OnActivate(CItemBase * pItem, bool bActivate);
	virtual bool OnCanRememberData(CItemBase * pItem, bool bSave);
	virtual void OnSelect(CItemBase * pItem);
	virtual bool OnClicked(CItemBase * pItem);
	virtual void OnDblClicked(CItemBase * pItem);
	virtual bool OnChangingData(CItemBase * pItem);
	virtual void OnChangedData(CItemBase * pItem);
	virtual void OnChangedState(CItemBase * pItem, tDWORD nStateMask = ISTATE_ALL);
	virtual void OnMouseHover(CItemBase* pItem);
	virtual void OnMouseLeave(CItemBase* pItem);
	virtual bool OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem) { return true; }
	virtual void OnNavigate(CItemBase * pItem, const tString &strURL, int Flags, tCPTR pPostData, tSIZE_T nPostDataLen, const tString &strHeaders, bool &bCancel);

	virtual cNode *CreateOperand(const char *name, cNode *container, cAlloc *al);
	virtual cNode *CreateOperator(const char *name, cNode **args, cAlloc *al);

protected:
	virtual bool ctl_IsFocused() { return false; }
	virtual void ctl_GetText(cStrObj& strText) {}
	virtual void ctl_SetText(LPCWSTR strText) {}

protected:
	static CItemBase* CreateItem(LPCSTR strItemId, LPCSTR strItemType);

	bool         LoadInternal(CItemSink* pSink, LPCSTR strItemId, LPCSTR strItemInfo, CItemBase*& pItem, tDWORD nFlags = 0, CItemBase * pTop = NULL, CSinkCreateCtx * pCtx = NULL);
	void         LoadChilds(tDWORD nFlags, CItemBase * pTop, CSinkCreateCtx * pCtx);
	void         LoadMenuInternal(CPopupMenuItem * pMenu, CItemBase * pItem, LPCSTR strLoadInfo, CSinkCreateCtx *pCtx);
	void         PreBind(CItemBase* pItem, CSinkCreateCtx * pCtx);
	void         PreInit(CItemBase * pTop = NULL, CSinkCreateCtx * pCtx = NULL);
	void         PreInit1(CSinkCreateCtx * pCtx);
	void         PreInit2();
	void         CleanProps();
	void         PostInit(CItemBase * pTop);
	void         PostShow();
	void         DeInit();
	void         RecalcPlacement(bool bSize = true);
	void         RecalcParentSize();
	CItemBase *  EnumNextItemInternal(CItemBase * pPrev, tDWORD nFlags);
	tSIZE_T      GetTotalChildrenCount();
	bool         IsFit(CItemBase *pChild);
	
	CItemBase *  GetNextOrderChild(CItemBase * pPrev, bool bBackward = false);
	void         CorrectTabOrder();

	int          GetIconMargSize();
	int          GetStateImage();

	void         _CheckEnabled();
	void         _CheckShow(bool bRecacl = false);
	void         _ApplyAttrs(CItemProps& strProps);
	
	void         _InitOverrides(CItemPropVals& prp);
	void         __InitOverrides(Overrides& _ovrds, CItemPropVals& prp, tString& strOverId = TOR_tString);
	void         _GetOverridesPropString(Overrides& _ovrds, tString& str, bool bText = false);
	void         _GetOverride(CItemBase * pExtItem, tString& strId, CItemProps& _Props, tString& strSection);
	Override *   _GetOverrideItem(tString& strOverId, bool bCreateIfNotextst = false);
	Override *   __GetOverrideItem(Overrides& _ovrds, LPSTR strOverId, bool bCreateIfNotextst);

	static bool  _ExtractSection(tString& strProps, tString& strSection, bool bCheckOnly = false);

public:
	CRootItem *       m_pRoot;
	tString           m_strSection;
	tString           m_strItemId;
	tString           m_strItemType;
	tString           m_strItemAlias;
	CItemBase *       m_pOwner;
	CItemBase *       m_pParent;
	CItemSink *       m_pSink;
	POINT             m_ptOffset;
	RECT              m_rcShift;
	RECT              m_rcMargin;
	SIZE              m_szSize;
	SIZE              m_szSizeMin;
	SIZE              m_szSizeMax;
	POINT             m_ptResizeScale;
	SIZE              m_szSizeWoScale;
	CFontStyle *      m_pFont;
	CImageList_       m_pBackground;
	CImageList_       m_pIcon;
	CBgContext *      m_pBgCtx;
	CBorder *         m_pBorder;
	tStrText          m_strText;
	tString           m_strTipText;
	tQWORD            m_nFlags;
	tDWORD            m_nState;
	tDWORD            m_nIconAlign;
	tDWORD            m_nTextFlags;
	void *            m_pUserData;
	CItemProps *      m_pProps;
	tSIZE_T           m_nOrder;
	tWORD             m_nTabGroup;
	tSIZE_T           m_nPos;
	tWORD             m_nHelpId;
	Overrides *       m_pOverrides;
	CItemBinding *    m_pBinding;
	CItemEditData *   m_pEditData;
	
	Items             m_aItems;

	tDWORD            m_ref;
	tDWORD            m_lockUpdate;

	unsigned          m_bHideExpanded : 1;
	unsigned          m_bHideCollapsed : 1;
};

//////////////////////////////////////////////////////////////////////
// Item Sink

#define ITEMBIND_F_OWN				0x01
#define ITEMBIND_F_OWNEXCEPTTHIS	0x02
#define ITEMBIND_F_VALUE			0x04

struct CSinkCreateCtx
{
	CSinkCreateCtx(CItemBase *item, LPCSTR funcName = NULL, cNode **args = NULL, size_t argc = 0, cNodeExecCtx * ctx = NULL) :
		m_item(item), m_funcName(funcName), m_args(args), m_argc(argc), m_ctx(ctx) {}

	CItemBase *    m_item;
	LPCSTR         m_funcName;
	cNode **       m_args;
	size_t         m_argc;
	cNodeExecCtx * m_ctx;
};

class CItemSink : public cNode::cNodeFactory
{
public:
	CItemSink() : m_pItem(NULL) {}
	virtual ~CItemSink() { if( m_pItem ) m_pItem->m_pSink = NULL; }
	virtual void Destroy() { delete this; }

	virtual void OnInitBinding(CItemBase * pItem, CSinkCreateCtx * pCtx) {}
	virtual void OnInitProps(CItemProps& pProps) {}
	virtual tDWORD OnBindingFlags() { return 0; }
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual bool OnCanClose(bool bUser) { return true; }
	virtual void OnInitFrom(CItemBase * pItem) {}
	virtual void OnInit() {}
	virtual void OnInited() {}
	virtual void OnEnable(bool bOn) {}
	virtual void OnActivate(CItemBase * pItem, bool bActivate) {}
	virtual bool OnCanRememberData(CItemBase * pItem, bool bSave) { return true; }
	virtual bool OnSelect(CItemBase * pItem) { return false; }
	virtual bool OnClicked(CItemBase * pItem) { return false; }
	virtual bool OnDblClicked(CItemBase * pItem) { return false; }
	virtual void OnChangedData(CItemBase * pItem) {}
	virtual void OnChangedState(CItemBase * pItem, tDWORD nStateMask = ISTATE_ALL) {}
	virtual void OnMouseHover(CItemBase * pItem) {}
	virtual void OnMouseLeave(CItemBase * pItem) {}
	virtual bool OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem) { return true; }
	virtual void OnEvent(tDWORD nEventId, cSerializable * pData) {}
	virtual void OnTimerRefresh(tDWORD nTimerSpin) {}
	virtual HWND OnGetParent() { return NULL; }
	virtual bool OnGetHelpId(tWORD &nHelpId) { return false; }
	virtual bool OnNavigate(CItemBase * pItem, const tString &strURL, int Flags, tCPTR pPostData, tSIZE_T nPostDataLen, const tString &strHeaders, bool &bCancel) { return false; }
	virtual bool OnAddDataSection(LPCSTR strSect) { return false; }
	
	virtual tTYPE_ID OnGetType(){ return tid_STRING_OBJ; }
	virtual bool OnSetType(tTYPE_ID nType){ return false; }
	virtual bool OnSetValue(const cVariant &pValue){ return false; }
	virtual bool OnGetValue(cVariant &pValue){ return false; }
	virtual bool OnIsOut() { return false; }
	virtual bool OnIsVector(){ return false; }
	
public:
	CItemBase * m_pItem;
};

//////////////////////////////////////////////////////////////////////

template <class TItem = CItemBase, class TItemSink = CItemSink>
class CItemSinkT : public TItemSink
{
public:
	using TItemSink::m_pItem;

	operator TItem*() const{ return Item(); }
	TItem* operator->(){ return Item(); }

	bool       IsInited() { return m_pItem->IsInited(); }
	CItemBase* GetItem(LPCSTR strItemId, LPCSTR strItemType = NULL, bool bRecurse=true){ return m_pItem->GetItem(strItemId, strItemType, bRecurse); }
	LPCSTR     LoadLocString(tString& str, LPCSTR strId){ return m_pItem->LoadLocString(str, strId); }
	tString&   ItemId() const{ return m_pItem->m_strItemId; }
	TItem*     Item() const{ return (TItem*)m_pItem; }
	CRootItem* Root() const{ return m_pItem->m_pRoot; }
	
	void       Show(LPCSTR strItem, bool bShow){ m_pItem->Show(strItem, bShow); }
	void       Enable(LPCSTR strItem, bool bOn){ m_pItem->Enable(strItem, bOn); }
	
	inline void Show(bool bShow){ Show(NULL, bShow); }
	inline void Enable(bool bOn){ Enable(NULL, bOn); }
};

//////////////////////////////////////////////////////////////////////

class CDescriptor : public CItemSinkT<>
{
public:
	virtual bool SetItemData(cSer *pData) { return false; }
	virtual bool GetItemData(cSer *pData) { return false; }
};

class CDescriptorDataOwner : public CItemSinkT<>
{
public:
	virtual void SetDescriptor(CDescriptor *pDesc) {}
};

//////////////////////////////////////////////////////////////////////

#include <ProductCore/Ctrls.h>

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_ITEMBASE_H__92386C65_7130_41F0_A037_C7F9211E6DD9__INCLUDED_)

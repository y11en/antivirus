// Ctrls.cpp
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <ProductCore/ItemBase.h>
#include <ProductCore/binding.h>
#if defined (__unix__)
#include <unix/compatutils/compatutils.h>
#define INFINITE (-1)
#endif

//////////////////////////////////////////////////////////////////////

#define OWNEROFMEMBER_PTR( _owner_class, _member )	\
	((_owner_class *)((unsigned char *)this - (unsigned char *)&((reinterpret_cast<_owner_class *>(0))->_member)))

extern tBOOL osd_OffsetRect(RECT *, int, int);
extern tBOOL osd_PtInRect(RECT * rc, POINT& pt);

static int _abs(int x) { return x < 0 ? -x : x; }

//////////////////////////////////////////////////////////////////////
// CGroupItem

CGroupItem::CGroupItem() : m_nHeaderSize(0)
{
	m_nFlags |= STYLE_DRAW_CONTENT;
}

void CGroupItem::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	CItemBase::InitProps(strProps, pCtx);

	RECT rcBound = {0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE};
	m_pFont->Draw(m_pRoot->GetDesktopDC(), "W", rcBound, TEXT_CALCSIZE);

	m_nHeaderSize = RECT_CY(rcBound);
}

void CGroupItem::DrawBorder(HDC dc, RECT& rcClient)
{
	int cx = rcClient.left + 10 + (m_pIcon ? m_pIcon->Size().cx : 0);
	int cy = rcClient.top - m_nHeaderSize / (m_strText.empty() ? 2 : 1);
	RECT rcText = {cx, cy, rcClient.right, rcClient.bottom};

	tDWORD nTextFlags = GetFontFlags();
	m_pFont->Draw(dc, OnGetDisplayText(), rcText, nTextFlags|TEXT_CALCSIZE);
	m_pFont->Draw(dc, OnGetDisplayText(), rcText, nTextFlags);

	rcText.left -= 3, rcText.right += 2;
	m_pRoot->ClipRect(dc, &rcText);

	rcClient.top -= m_nHeaderSize / 2 + m_nHeaderSize % 2;
	CItemBase::DrawBorder(dc, rcClient);
}

void CGroupItem::DrawContent(HDC dc, RECT& rcClient)
{
	rcClient.left += m_nHeaderSize;
	rcClient.top -= m_nHeaderSize / (m_strText.empty() ? 2 : 1);

	if( m_pIcon )
		m_pIcon->Draw(dc, rcClient, NULL, m_pIcon.GetImage(GetStateImage()));
}

void CGroupItem::OnAdjustClientRect(RECT& rcClient)
{
	rcClient.top += m_nHeaderSize / (m_strText.empty() ? 2 : 1);
	rcClient.left += 2;
	rcClient.right -= 2;
	rcClient.bottom -= 2;
}

//////////////////////////////////////////////////////////////////////
// CPopupMenuItem

CPopupMenuItem::CPopupMenuItem() : m_nLoadFlags(LOAD_MENU_FLAG_SHOW), m_pSelItem(NULL)
{
	m_nFlags |= STYLE_WINDOW|STYLE_CUSTOMCHILDREN; m_pt.x = m_pt.y = 0;
}

void CPopupMenuItem::Cleanup()
{
	if( m_pSelItem )
		m_pSelItem->Release(), m_pSelItem = NULL;
	CItemBase::Cleanup();
}

void CPopupMenuItem::OnInit()
{
	CItemBase::OnInit();
	_InitItems(this);
}

bool CPopupMenuItem::SetText(LPCSTR strItem, LPCSTR strTextId)
{
	if( CItemBase * pItem = GetItem(strItem) )
		return pItem->SetText(LoadLocString(TOR_tString, strTextId));
	return false;
}

static CBackground * _get_backgnd(CItemBase * pTop)
{
	for(tDWORD i = 0, n = pTop->m_aItems.size(); i < n; i++)
	{
		CItemBase * pItem = pTop->m_aItems[i];
		CBackground * pB = pItem->m_pBackground;
		if( pB )
			return pB;
		
		pB = _get_backgnd(pItem);
		if( pB )
			return pB;
	}

	return NULL;
}

CItemBase * CPopupMenuItem::Track(POINT * pt, bool *bDataChanged)
{
	if( bDataChanged )
		*bDataChanged = false;

	if( pt )
		m_pt = *pt;

	CItemBase * pSelItem = NULL;
	if( m_nLoadFlags & LOAD_MENU_FLAG_DEFITEM )
		pSelItem = GetDefItem();
	else if( m_nLoadFlags & LOAD_MENU_FLAG_SHOW )
	{
		if( CBackground * pBackground = _get_backgnd(this) )
			SetBackground(pBackground);
		
		HMENU hMenu = ctl_GetRootItem();
		_InitPopup(hMenu, this);
		
		tDWORD nResult = ctl_Track(m_pt);
		pSelItem = m_pSelItem ? m_pSelItem : GetById(nResult);
	}

	bool dataChanged = false;
	if( pSelItem )
	{
		if( pSelItem->m_nFlags & STYLE_CHECKBOX_LIKE )
		{
			pSelItem->Select(!(pSelItem->m_nState & ISTATE_SELECTED));
			pSelItem->SetChangedData();
			dataChanged = true;
		}
		else if( pSelItem->m_nFlags & STYLE_ENUM_VALUE )
		{
			pSelItem->Select();
			dataChanged = true;
		}

		pSelItem->Click();
	}

	if( bDataChanged )
		*bDataChanged = dataChanged;
	
	return pSelItem;
}

void CPopupMenuItem::TrackEnd(CItemBase * pItem)
{
	if( pItem && pItem->IsParent(this) )
	{
		m_pSelItem = pItem;
		m_pSelItem->AddRef();
	}
	
	ctl_TrackEnd();
}

bool CPopupMenuItem::AddFromSection(LPCSTR strLoadInfo, CSinkCreateCtx *pCtx)
{
	CItemBase * pMenuItems = m_pRoot->CreateItem(NULL, "");
	if( !pMenuItems )
		return false;

	if( !strchr(strLoadInfo, '[') )
	{
		pMenuItems->m_strSection = strLoadInfo;
		strLoadInfo = NULL;
	}
	
	pMenuItems->m_nFlags |= STYLE_TOP_ITEM;
	LoadInternal(NULL, NULL, strLoadInfo, pMenuItems, 0, NULL, pCtx);
	pMenuItems->m_nFlags &= ~STYLE_TOP_ITEM;

	if( pMenuItems->GetChildrenCount() )
		return true;
	
	pMenuItems->Destroy();
	return false;
}

//////////////////////////////////////////////////////////////////////

void CPopupMenuItem::_InitItems(CItemBase * pParent)
{
	pParent->m_nFlags |= STYLE_CLICKABLE;
	pParent->m_pFont = NULL;
	
	for(size_t i = 0; i < pParent->m_aItems.size(); i++)
		_InitItems(pParent->m_aItems[i]);
}

void CPopupMenuItem::_InitPopup(HMENU hMenu, CItemBase * pParent)
{
	for(CItemBase * pItem = pParent->GetChildFirst(); pItem; pItem = pItem->GetNext())
	{
		if( pItem->m_nState & ISTATE_HIDE )
			continue;

		if( pItem->m_strText.empty() && pItem->m_aItems.size() )
		{
			_InitPopup(hMenu, pItem);
			continue;
		}
		
		HMENU hSubMenu = ctl_AddItem(hMenu, pItem, !!pItem->m_aItems.size());
		if( hSubMenu )
			_InitPopup(hSubMenu, pItem);
	}

	ctl_CleanSeps(hMenu);
}

//////////////////////////////////////////////////////////////////////
// CRefsItem

#define EVENT_ID_REFSITEM_CLEANUP     0x8bb2cf71

CRefsItem::CRefsItem()
{
	m_nFlags |= STYLE_CUSTOMCHILDREN;
}

CRefsItem::~CRefsItem()
{
	_RefsDeleteAll(m_aRefs);
	_RefsDeleteAll(m_aRefsDel);
}

void CRefsItem::Cleanup()
{
	m_aItems.clear();
	CItemBase::Cleanup();
}

bool CRefsItem::SetText(LPCSTR strText)
{
	if( m_strText == strText )
		return false;
	
	UpdateRefsText(strText, true);
	return CItemBase::SetText(strText);
}

void CRefsItem::SendEvent(tDWORD nEventId, cSerializable* pData, bool bBroadcast)
{
	if( nEventId != EVENT_ID_REFSITEM_CLEANUP )
	{
		CItemBase::SendEvent(nEventId, pData, bBroadcast);
		return;
	}

	_RefsDeleteAll(m_aRefsDel);
}

void CRefsItem::UpdateRefsText(LPCSTR strText, bool bFull)
{
	m_strDisplayText.erase();
	m_strDisplayText.reserve(strlen(strText));
	
	// "<a idLaunchHelp onclick(help(nHelpId))><b>Show help with id " + nHelpId + "</b></a>"

	{
		for(size_t i = 0; i < m_aRefs.size(); i++)
			m_aRefsDel.insert(m_aRefsDel.end(), m_aRefs[i]);
		m_aRefs.clear();
		m_aItems.clear();
		m_pRoot->SendEvent(EVENT_ID_REFSITEM_CLEANUP, NULL, 0, this);
	}
	m_aBolds.clear();

	LPCSTR strTextTail = strText;
	bool l_tag = false;
	for(;;)
	{
		if( l_tag )
		{
			LPCSTR strTagClose = _FindTagStr(strTextTail, "</a>");
			if( strTagClose )
			{
				m_strDisplayText.append(strTextTail, strTagClose - strTextTail);
				strTextTail = strTagClose + 4;
			}
			else
			{
				m_strDisplayText += strTextTail;
				strTextTail += strlen(strTextTail);
			}

			if( bFull )
			{
				RefData * pRef = m_aRefs[m_aRefs.size() - 1];
				pRef->m_chrRange.y = (LONG)m_strDisplayText.size();
				pRef->_GetItem()->m_strText.append(m_strDisplayText.c_str() + pRef->m_chrRange.x, pRef->m_chrRange.y - pRef->m_chrRange.x);
			}

			l_tag = false;
			continue;
		}
		
		LPCSTR strTagEnd = NULL;
		LPCSTR strTag = _FindTagStr(strTextTail, "<a");
		if( strTag )
			strTagEnd = _FindTagStr(strTag + 2, ">");
		
		if( !strTag || !strTagEnd )
		{
			m_strDisplayText += strTextTail;
			break;
		}

		l_tag = true;
		
		m_strDisplayText.append(strTextTail, strTag - strTextTail);
		strTextTail = strTagEnd + 1;
		
		if( bFull )
		{
			m_aRefs.push_back(_RefCreate());
			
			RefData * pRef = m_aRefs[m_aRefs.size() - 1];
			CItemBase * pItem = pRef->_GetItem();

			m_aItems.push_back(pItem);

			pItem->m_pRoot   = m_pRoot;
			pItem->m_pParent = this;
			pItem->m_pOwner  = this;
			pItem->m_nPos    = m_aRefs.size() - 1;
			pItem->m_nFlags |= STYLE_CLICKABLE;
			
			CItemProps _Props;
			{
				tString strParams; strParams.append(strTag + 2, strTagEnd - (strTag + 2));
				_Props.Parse(strParams.c_str(), pItem->m_strItemId);
				m_pRoot->TrimString(pItem->m_strItemId);
			}
			
			pRef->m_chrRange.x = (LONG)m_strDisplayText.size();
			
			pRef->_SetProps(_Props);
			pRef->_Init();
			pRef->_UpdateProps(UPDATE_FLAG_ALL, ISTATE_ALL);
		}
	}
	
	for(int posTag = 0, posTagClose = 0; posTag != -1 && posTagClose != -1; )
	{
		if( (posTag      = (int)m_strDisplayText.find("<b>"))  != -1 )  _UpdateTextAndLinks(m_strDisplayText, posTag,      countof("<b>") - 1);
		if( (posTagClose = (int)m_strDisplayText.find("</b>")) != -1 )  _UpdateTextAndLinks(m_strDisplayText, posTagClose, countof("</b>") - 1);

		if( bFull )
			if( posTag != -1 && posTagClose != -1 && posTag < posTagClose )
			{
				POINT _bold;
				_bold.x = posTag;
				_bold.y = posTagClose;
				m_aBolds.push_back(_bold);
			}
	}

	if( bFull )
		if( !m_aRefs.size() && !m_aBolds.size() )
			m_strDisplayText.erase();
}

CItemBase * CRefsItem::GetItem(LPCSTR strItemId, LPCSTR strItemType, bool bRecurse)
{
	if (!strItemId)
		return NULL;
	
	for(size_t i = 0; i < m_aRefs.size(); i++)
	{
		CItemBase * pItem = m_aRefs[i]->_GetItem();
		if( pItem->m_strItemId == strItemId )
			return pItem;
	}
	
	return NULL;
}

CItemBase * CRefsItem::GetByPos(tSIZE_T nIdx)
{
	if( nIdx >= m_aRefs.size() )
		return NULL;
	return m_aRefs[nIdx]->_GetItem();
}

tSIZE_T CRefsItem::GetChildrenCount()
{
	return m_aRefs.size();
}

void CRefsItem::OnUpdateTextProp()
{
	//UpdateRefsText(m_strText.c_str(), true);
	CItemBase::OnUpdateTextProp();
	
	_ApplyBlockStyles();
}

LPCSTR CRefsItem::OnGetDisplayText()
{
	if( m_strDisplayText.empty() )
		return CItemBase::OnGetDisplayText();
	return m_strDisplayText.c_str();
}

void CRefsItem::OnChangedData(CItemBase* pItem)
{
	if( pItem != this )
		pItem = this;
	else if( m_nFlags & STYLE_ENUM_VALUE )
		pItem = m_pParent;
	CItemBase::OnChangedData(pItem);
}

void CRefsItem::_RefsDeleteAll(std::vector<RefData *>& a)
{
	for(size_t i = 0; i < a.size(); i++)
	{
		CItemBase * pItem = a[i]->_GetItem();
		pItem->m_pParent = NULL;
		pItem->Destroy();
	}
	
	a.clear();
}

void CRefsItem::_ApplyBlockStyles()
{
	for(size_t i = 0; i < m_aRefs.size(); i++)
	{
		RefData * pRef = m_aRefs[i];
		CItemBase * pItem = pRef->_GetItem();

		pItem->m_nState |= ISTATE_INITED;
	}
	
	OnApplyBlockStyles();
	
	_CheckEnabled();
}

void CRefsItem::_UpdateTextAndLinks(tString &strText, int pos, int num)
{
	strText.replace(pos, num, "");
	for(size_t i = 0; i < m_aRefs.size(); i++)
	{
		RefData * pRef = m_aRefs[i];
		if( pRef->m_chrRange.x > pos )
			pRef->m_chrRange.x -= num;
		if( pRef->m_chrRange.y > pos )
			pRef->m_chrRange.y -= num;
	}
}

LPCSTR CRefsItem::_FindTagStr(LPCSTR str, LPCSTR strFind)
{
	if( !str || !strFind )
		return NULL;
	
	size_t nStr = strlen(str), nStrFind = strlen(strFind);

	while( nStr >= nStrFind )
	{
		if( !memcmp(str, strFind, nStrFind) )
			return str;

		nStr--;
		str++;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CComboItem

CComboItem::Item::Item() :
	m_used(0)
{
	m_nFlags |= STYLE_ENUM_VALUE;
}

void CComboItem::Item::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	TBase::OnUpdateProps(flags, nStateMask, rc);

	CComboItem * p = (CComboItem *)m_pParent;

	if( (flags & UPDATE_FLAG_TEXT) || (nStateMask & ISTATE_HIDE) )
		p->_SetItem(this);
}

void CComboItem::Item::Select(bool bSelect)
{
	CItemBase::Select(bSelect);
	
	CComboItem * p = (CComboItem *)m_pParent;
	if( bSelect )
	{
		tSIZE_T nSel = p->_Item2SelIdx(this);
		if( p->ctl_GetCurSel() != nSel )
			p->ctl_SetCurSel(nSel);
	}
}

bool CComboItem::Item::SetValue(const cVariant &pValue)
{
	m_v = pValue;
	return true;
}

bool CComboItem::Item::GetValue(cVariant &pValue)
{
	if( m_v.Type() == cVar::vtVoid )
		return CItemBase::GetValue(pValue);
	
	pValue = m_v;
	return true;
}

//////////////////////////////////////////////////////////////////////

cVar CComboItem::m_void;

CComboItem::CComboItem() :
	m_type(tid_UNK), m_edit(0), m_sorted(0)
{
	m_nFlags |= TEXT_SINGLELINE|STYLE_ENUM|STYLE_CUSTOMCHILDREN;
}

void CComboItem::OnInit()
{
	TBase::OnInit();
	_InitFromVector();
}

void CComboItem::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_EDIT)    m_edit = 1; sbreak;
	scase(STR_PID_SORTED)  m_sorted = 1; sbreak;
	send;
}

void CComboItem::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	TBase::OnUpdateProps(flags, nStateMask, rc);

	if( flags & UPDATE_FLAG_TEXT )
		for(size_t i = 0, n = m_aItems.size(); i < n; i++)
			((Item *)m_aItems[i])->OnUpdateProps(UPDATE_FLAG_TEXT, 0, NULL);
}

CItemBase * CComboItem::CreateChild(LPCSTR strItemId, LPCSTR strItemType)
{
	return new Item();
}

tDWORD CComboItem::AddItem(LPCWSTR strName, cVar& v)
{
	Item * pItem = (Item *)CreateChild(NULL, NULL);
	swap_objmem(pItem->m_v, v);
	pItem->m_v.ToString(pItem->m_strItemId);
	m_pRoot->LocalizeStr(pItem->m_strText, strName);
	
	CItemBase::AddItem(pItem);
	return pItem->m_nPos;
}

tDWORD CComboItem::FindItem(LPCWSTR strName, const cVar& v)
{
	if( strName )
	{
		if( CItemBase * p = _SelIdx2Item(ctl_FindByName(strName)) )
			return p->m_nPos;
		return -1;
	}
	
	for(size_t i = 0, n = m_aItems.size(); i < n; i++)
	{
		Item * p = (Item *)m_aItems[i];
		if( p->m_v == v )
			return p->m_nPos;
	}

	return -1;
}

void CComboItem::SetDef(tDWORD nIdx)
{
}

void CComboItem::DeleteAllItems()
{
	ctl_DeleteAllItems();
	Clear();
}

cVar& CComboItem::GetItemVal(tDWORD nIdx)
{
	return (nIdx < m_aItems.size()) ? ((Item *)m_aItems[nIdx])->m_v : m_void;
}

tTYPE_ID CComboItem::GetType()
{
	if( IsVector() && m_pSink )
		return m_pSink->OnGetType();
	return m_type;
}

bool CComboItem::SetType(tTYPE_ID nType)
{
	m_type = nType;
	if( m_pSink )
		m_pSink->OnSetType(m_type);
	return true;
}

bool CComboItem::IsOut()
{
	return true;
}

bool CComboItem::SetValue(const cVariant &pValue)
{
	if( CItemBase::SetValue(pValue) )
		return true;

	if( m_edit )
		ctl_SetText(pValue.ToStringObj().data());
	else
		ctl_SetCurSel(-1);
	
	return true;
}

bool CComboItem::GetValue(cVariant &pValue)
{
	tDWORD nCur = ctl_GetCurSel();
	if( m_edit && (nCur == -1) )
	{
		cStrObj strVal; ctl_GetText(strVal);
		m_pRoot->TrimString(strVal);
		pValue = strVal.data();
		return true;
	}
	
	return CItemBase::GetValue(pValue);
}

void CComboItem::_SelChanged(tSIZE_T nIdx)
{
	if( CItemBase * p = _SelIdx2Item(ctl_GetCurSel()) )
		p->Select();
}

CItemBase * CComboItem::_SelIdx2Item(tDWORD nSel)
{
	tSIZE_T i = m_sorted ? (tSIZE_T)ctl_GetItemData(nSel) : nSel;
	return i < m_aItems.size() ? m_aItems[i] : NULL;
}

tSIZE_T CComboItem::_Item2SelIdx(CItemBase * p)
{
	if( !m_sorted )
		return p->m_nPos < ctl_GetItemsCount() ? p->m_nPos : -1;
	
	for(tSIZE_T i = 0, n = ctl_GetItemsCount(); i < n; i++)
	{
		tSIZE_T nPos = (tSIZE_T)ctl_GetItemData(i);
		if( p->m_nPos == nPos )
			return i;
	}

	return -1;
}

void CComboItem::_SetItem(Item * pItem)
{
	if( pItem->m_used )
	{
		ctl_DeleteItem(_Item2SelIdx(pItem));
		pItem->m_used = 0;
	}
	
	if( !(pItem->m_nState & ISTATE_HIDDENEX) )
	{
		cStrObj strW; m_pRoot->LocalizeStr(strW, pItem->m_strText.c_str(), pItem->m_strText.size());
		tDWORD nIdx = ctl_AddItem(strW.data());
		if( nIdx != -1 )
			ctl_SetItemData(nIdx, (tPTR)pItem->m_nPos);

		if( pItem->m_nState & ISTATE_SELECTED )
			ctl_SetCurSel(nIdx);
	}
}

void CComboItem::_InitFromVector()
{
	CItemPropVals &Vals = m_pProps->Get("items");
	if( !Vals.IsValid() )
		return;
	
	cVariant var;
	CItemProps Props(Vals.GetStr());
	CFieldsBinder *pBinder = GetBinder();
	
	cVectorHnd vector;
	{
		CGuiPtr<cNode> pVectNode = m_pRoot->CreateNode(Props.Get("vector").Get().c_str(), pBinder, this);
		if( !pVectNode )
			return;
		
		pVectNode->Exec(var, TOR(cNodeExecCtx, (m_pRoot)));
		const cVectorData *vd = var.c_vector();
		if( !vd )
		{
			PR_ASSERT_MSG(0, "operand is not a vector");
			return;
		}
	
		vector.attach(vd->m_v, vd->m_type, vd->m_flags);
	}

	CFieldsBinder itemBinder(m_pRoot, pBinder, NULL, cSerializableObj::getDescriptor(vector.m_type));

	CGuiPtr<cNode> pItemText = m_pRoot->CreateNode(Props.Get("text").Get().c_str(), &itemBinder, this);
	if( !pItemText )
		return;
	
	CGuiPtr<cNode> pItemValue = m_pRoot->CreateNode(Props.Get("v").Get().c_str(), &itemBinder, this);
	if( !pItemValue )
		return;

	cStrObj strTextW;
	CStructData sd(NULL);
	cNodeExecCtx ec(m_pRoot, this, &sd);
	for(size_t i = 0, n = vector.size(); i < n; i++)
	{
		sd.m_ser = (cSer *)vector.at(i);

		pItemText->Exec(var, ec);
		tString &strText = var.MakeString();
		if( strText.empty() )
			continue;

		m_pRoot->LocalizeStr(strTextW, strText.c_str(), strText.size());
		pItemValue->Exec(var, ec);
		AddItem(strTextW.data(), var);
	}
}

//////////////////////////////////////////////////////////////////////

struct cComboNode : public cNodeArgs<3>
{
	cComboNode(CComboItem* item, cNode **args) : m_item(item), cNodeArgs<3>(args){}

	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		cVariant name, v;
		m_args[1]->Exec(name, ctx, get);
		m_args[2]->Exec(v, ctx, get);
		tDWORD idx = m_item->AddItem(name.ToStringObj().data(), v);
		if( idx != -1 )
			m_item->SetCurSel(idx);
	}

	CComboItem* m_item;
};

cNode * CComboItem::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name) scase("add")  return new cComboNode(this, args); sbreak; send;
	return CItemBase::CreateOperator(name, args, al);
}

//////////////////////////////////////////////////////////////////////
// CTreeItem

cTreeItem * CTreeItem::Idx2Item(cTreeItem * pParent, int nIdx)
{
	if( nIdx == -1 )
		return NULL;
	cTreeItem * pChild = GetChildItem(pParent);
	for(; pChild; pChild = GetNextItem(pChild))
	{
		if( !nIdx )
			break;
		nIdx--;
	}

	return pChild;
}

int CTreeItem::Item2Idx(cTreeItem * pParent, cTreeItem * pItem)
{
	if( !pItem )
		return -1;

	int nIdx = 0;
	for(cTreeItem * pChild = GetChildItem(pParent); pChild; pChild = GetNextItem(pChild))
	{
		if( pChild == pItem )
			return nIdx;
		nIdx++;
	}

	return -1;
}

void CTreeItem::DeleteChildItems(cTreeItem * pParent)
{
	cTreeItem* pDelete;
	cTreeItem* pItem = GetChildItem(pParent);
	while( pItem )
	{
		pDelete = pItem;
		pItem = GetNextItem(pItem);
		DeleteItem(pDelete);
	}
}

//////////////////////////////////////////////////////////////////////
// CTabItem

CTabItem::CTabItem()
{
	m_nFlags |= STYLE_SHEET_ITEM|STYLE_SHEET_PAGES|STYLE_TRANSPARENT|STYLE_NON_OWNER;
	m_nCurItem = -1;
}

void CTabItem::ShowPageItem(CItemBase * pItem, bool bShow)
{
	if( bShow == !(pItem->m_nState & ISTATE_SHEETPAGE_HIDE) )
		return;
	
	tSIZE_T nPos;
	if( bShow )
	{
		pItem->m_nState &= ~ISTATE_SHEETPAGE_HIDE;
		nPos = _Page2Idx(pItem);
	}
	else
	{
		nPos = _Page2Idx(pItem);
		pItem->m_nState |= ISTATE_SHEETPAGE_HIDE;
	}

	if( !(m_nState & ISTATE_INITED) )
		return;

	if( bShow )
		_SetPage(nPos, pItem);
	else
		ctl_DeleteItem(nPos);
}

bool CTabItem::SetNextPage(bool bBackward)
{
	if( !bBackward )
	{
		size_t i = (m_nCurItem == m_aItems.size() - 1) ? 0 : (m_nCurItem + 1);
		for(; i < m_aItems.size(); i++)
			if( !(m_aItems[i]->m_nState & ISTATE_SHEETPAGE_HIDE) )
			{
				m_aItems[i]->Select();
				return true;
			}
		
		return false;
	}

	tSIZE_T i = (m_nCurItem == -1) ? m_aItems.size() : m_nCurItem;
	for(; i > 0; i--)
		if( !(m_aItems[i - 1]->m_nState & ISTATE_SHEETPAGE_HIDE) )
		{
			m_aItems[i - 1]->Select();
			return true;
		}

	return false;
}

bool CTabItem::SetCurPage(CItemBase * pPage)
{
	if( !pPage || pPage->m_pParent != this )
		return false;

	pPage->Select();
	return true;
}

void CTabItem::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	TBase::OnUpdateProps(flags, nStateMask, rc);

	if( (flags & UPDATE_FLAG_TEXT) )
	{
		size_t k = 0;
		for(size_t i = 0; i < m_aItems.size(); i++)
		{
			CItemBase * l_item = m_aItems[i];
			if( l_item->m_nState & ISTATE_SHEETPAGE_HIDE )
				continue;
			
			_SetPage(k++, l_item);

			if( !(l_item->m_nFlags & AUTOSIZE_PARENT) )
			{
				l_item->m_nFlags |= AUTOSIZE_PARENT;
				if( m_nState & ISTATE_INITED )
					l_item->RecalcLayout();
			}
			
			l_item->Show(l_item->m_nState & ISTATE_SELECTED, SHOWITEM_DIRECT);
		}

		tSIZE_T nTabs = ctl_GetItemCount();
		while( nTabs > k )
			ctl_DeleteItem(k), nTabs--;
	}

	if( nStateMask & ISTATE_HIDE )
	{
		if( m_nCurItem >= ctl_GetItemCount() )
			m_nCurItem = 0;
		
		if( !(m_nState & ISTATE_HIDE) )
			OnSelect(_Idx2Page(m_nCurItem));
	}
}

void CTabItem::OnInited()
{
	TBase::OnInited();

	Resize(0, 0);

	for(size_t i = 0, k = 0; i < m_aItems.size(); i++)
	{
		CItemBase * l_item = m_aItems[i];

		l_item->m_nFlags &= ~(ALIGN_HAFTER|ALIGN_HBEFORE);

		if( l_item->m_nState & ISTATE_SHEETPAGE_HIDE )
			continue;

		if( m_nCurItem == -1 && (l_item->m_nState & ISTATE_DEFAULT) )
		{
			m_nCurItem = k;
			break;
		}
		
		k++;
	}
}

void CTabItem::OnAdjustClientRect(RECT& rcClient)
{
	ctl_AdjustRect(rcClient);
}

void CTabItem::OnSelect(CItemBase * pItem)
{
	if( !pItem )
		return;
	
	if( pItem->m_pParent != this ||
		(pItem->m_nState & ISTATE_SHEETPAGE_HIDE) )
		return;

	tSIZE_T nCurSel = ctl_GetCurSel();
	if( nCurSel != -1 )
		OnSelectItem(_Idx2Page(nCurSel), false);
	
	m_nCurItem = _Page2Idx(pItem);
	ctl_SetCurSel(m_nCurItem);
	OnSelectItem(pItem, true);
}

void CTabItem::OnSelectItem(CItemBase * pItem, bool bSelect)
{
	if( !pItem )
		return;
	
	if( !bSelect )
	{
		pItem->m_nState &= ~ISTATE_SELECTED;
		pItem->Show(false, SHOWITEM_DIRECT);
		return;
	}

	pItem->m_nState |= ISTATE_SELECTED;
	pItem->Show(true, SHOWITEM_DIRECT);
	TBase::OnSelect(pItem);
}

tSIZE_T CTabItem::_Page2Idx(CItemBase* pItem)
{
	tSIZE_T nIdx = -1;
	for(; pItem; pItem = pItem->GetPrev(false))
		if( !(pItem->m_nState & ISTATE_SHEETPAGE_HIDE) )
			nIdx++;

	return nIdx;
}

CItemBase* CTabItem::_Idx2Page(tSIZE_T nIdx)
{
	CItemBase* pItem = GetChildFirst(false);
	for(; pItem; pItem = pItem->GetNext(false))
	{
		if( pItem->m_nState & ISTATE_SHEETPAGE_HIDE )
			continue;
		
		if( (tNATIVE_INT)--nIdx < 0 )
			break;
	}

	return pItem;
}

void CTabItem::_SetPage(tSIZE_T nPos, CItemBase* pItem)
{
	const tString& strText = GetPlainText(pItem->GetDisplayText());
	ctl_SetItem(nPos, strText.c_str(), pItem);
}

void CTabItem::SaveSettings(cAutoRegKey& pKey)
{
	TBase::SaveSettings(pKey);

	tSIZE_T nCurSel = ctl_GetCurSel();
	if( nCurSel == -1 )
		return;

	tString strKey; GetOwnerRelativePath(strKey, GetOwner(true), true, "Tab");
	pKey.set_val(strKey.c_str(), tid_DWORD, (tPTR)&nCurSel);
}

void CTabItem::LoadSettings(cAutoRegKey& pKey)
{
	TBase::LoadSettings(pKey);

	if( m_nCurItem != -1 )
		return;

	tString strKey; GetOwnerRelativePath(strKey, GetOwner(true), true, "Tab");

	tDWORD nDefItem = -1;
	pKey.get_val(strKey.c_str(), tid_DWORD, (tPTR)&nDefItem);
	if( (tINT)nDefItem >= 0 && nDefItem < m_aItems.size() )
		m_nCurItem = nDefItem;
}

//////////////////////////////////////////////////////////////////////
// CEditItem

#define EDIT_DUMMY_PASSWD L"pppppppppp"

CEditItem::CEditItem() :
	m_nCompress(0), m_bDummyPasswdShow(0), m_bDummyPasswdSimpleShow(0),
	m_bChangeNotify(1), m_bTrimSpaces(1)
{
	m_nFlags |= TEXT_SINGLELINE|TEXT_IS_VALUE;
	m_ptLim.x = 0; m_ptLim.y = 0x7FFFFFFF;
}

void CEditItem::AddText(LPCSTR strText, bool bAppend)
{
	if( !bAppend )
	{
		ctl_ReplaceSelText(m_pRoot->LocalizeStr(TOR_cStrObj, strText));
		return;
	}
	
	size_t nModifyPos = m_strText.size();
	for(size_t i = 0, n = strlen(strText); i < n; i++)
	{
		CHAR c = strText[i];

		size_t nErase = 0;
		switch( c )
		{
		case 0x08:	// Backspace
			nErase = 1;
			break;
		case 0x0D:	// Back to line begin (CR)
			if( (i + 1 >= n) || (strText[i + 1] != 0x0A) )
			{
				LPCSTR strRet = strrchr(m_strText.c_str(), '\n');
				if( strRet )
					nErase = m_strText.size() - (strRet + 1 - m_strText.c_str());
				else
					nErase = m_strText.size();
			}
			break;
		}

		if( nErase )
		{
			m_strText.erase(m_strText.size() - nErase);
			if( m_strText.size() < nModifyPos )
				nModifyPos = m_strText.size();
		}
		else
			m_strText += c;
	}
	
	ctl_ReplaceText(m_pRoot->LocalizeStr(TOR_cStrObj, m_strText.c_str() + nModifyPos), nModifyPos, -1);
}

void CEditItem::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);

	// Get limits
	CItemPropVals& l_prpNumLim = strProps.Get(STR_PID_NUMLIM);
	if( l_prpNumLim.IsValid() )
	{
		m_ptLim.x = (LONG)l_prpNumLim.GetLong(0);
		if( !l_prpNumLim.Get(1).empty() )
			m_ptLim.y = (LONG)l_prpNumLim.GetLong(1);
	}
}

void CEditItem::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_COMPRESS)     m_nCompress = 1; sbreak;
	scase(STR_PID_COMPRESSPATH) m_nCompress = 2; sbreak;
	scase(STR_PID_NOTRIM)       m_bTrimSpaces = 0; sbreak;
	scase(STR_PID_PASSWORD)     m_bTrimSpaces = 0; sbreak;
	scase(STR_PID_PASSWORDSET)  m_bTrimSpaces = 0; sbreak;
	send;
}

bool CEditItem::IsAdjustable(bool bHorz)
{
	return !bHorz;
}

bool CEditItem::IsOut()
{
	return !ctl_IsReadonly();
}

void CEditItem::OnFocus(bool bFocusted)
{
	if( m_bDummyPasswdShow && !m_bDummyPasswdSimpleShow )
	{
		m_bChangeNotify = 0;
		if( bFocusted )
			ctl_SetText(L"");
		else
			ctl_SetText(EDIT_DUMMY_PASSWD);
		m_bChangeNotify = 1;
	}
	
	if( ctl_IsNumber() && !bFocusted )
	{
		cVariant nVal;
		GetValue(nVal);
		SetValue(nVal);
		
		cVariant nValNew;
		GetValue(nValNew);

		if( nVal != nValNew )
			OnChangedData(this);
	}

	if( m_nCompress )
	{
		ctl_SetText(m_pRoot->LocalizeStr(TOR_cStrObj, bFocusted ? m_strText.c_str() : m_strCompressed.c_str()));
		ctl_SetSelText(0, -1);
	}

	TBase::OnFocus(bFocusted);
}

void CEditItem::OnUpdateTextProp()
{
	if( !m_nCompress )
	{
		if( m_bDummyPasswdShow )
			ctl_SetText(EDIT_DUMMY_PASSWD);
		else
			ctl_SetText(m_pRoot->LocalizeStr(TOR_cStrObj, m_strText.c_str(), m_strText.size()));
		
		if( ctl_IsMultiline() )
			ctl_SetSelText(0, 0, true);
		return;
	}
	
	osd_CompressName(m_pRoot, m_nCompress == 1 ? OSDCN_NAME : OSDCN_PATH, m_strText.c_str(), m_pRoot->GetDesktopDC(), m_pFont ? m_pFont->Handle() : NULL, m_szSize.cx, m_strCompressed);
	
	if( !ctl_IsFocused() )
		ctl_SetText(m_pRoot->LocalizeStr(TOR_cStrObj, m_strCompressed.c_str(), m_strCompressed.size()));

	TBase::OnUpdateTextProp();
}

bool CEditItem::SetText(LPCSTR strText)
{
	m_bChangeNotify = 0;
	bool l_res = TBase::SetText(strText);
	m_bChangeNotify = 1;

	return l_res;
}

bool CEditItem::SetValue(const cVariant &pValue)
{
	if( !ctl_IsNumber() )
		return TBase::SetValue(pValue);

	tDWORD nVal = pValue.ToDWORD();
	if( nVal < (tDWORD)m_ptLim.x )
		nVal = m_ptLim.x;
	else if( nVal > (tDWORD)m_ptLim.y )
		nVal = m_ptLim.y;

	tCHAR strNum[100]; _ltoa(nVal, strNum, 10);
	return SetText(strNum), true;
}

bool CEditItem::GetValue(cVariant &pValue)
{
	if( !ctl_IsNumber() )
	{
		if( m_bDummyPasswdShow )
			return false;
		return TBase::GetValue(pValue);
	}

	if( m_strText.empty() )
		return false;
		
	tDWORD nVal = atol(m_strText.c_str());
	if( nVal < (tDWORD)m_ptLim.x )
		nVal = m_ptLim.x;
	else if( nVal > (tDWORD)m_ptLim.y )
		nVal = m_ptLim.y;
	
	pValue = nVal;
	return true;
}

tTYPE_ID CEditItem::GetType()
{
	if( m_pSink && (m_pSink->OnBindingFlags() & ITEMBIND_F_VALUE) )
		return m_pSink->OnGetType();

	return ctl_IsNumber() ? tid_DWORD : tid_STRING_OBJ;
}

void CEditItem::_DataChanged()
{
	if( !ctl_IsReadonly() && m_bChangeNotify &&
		(m_nState & ISTATE_INITED) )
	{
		m_bDummyPasswdShow = 0;
		
		cStrObj strText; ctl_GetText(strText);
		if( m_bTrimSpaces )
			m_pRoot->TrimString(strText);
		m_pRoot->LocalizeStr(m_strText, strText.data(), strText.size());
		
		OnChangedData(this);
	}
}

//////////////////////////////////////////////////////////////////////
// CSlider

CSliderItem::CSliderItem() :
	m_nCurPos(0)
{
	m_ptLim.x = 0; m_ptLim.y = 0x7FFFFFFF;
}

void CSliderItem::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);

	// Get limits
	CItemPropVals& l_prpNumLim = strProps.Get(STR_PID_NUMLIM);
	if( l_prpNumLim.IsValid() )
	{
		m_ptLim.x = (LONG)l_prpNumLim.GetLong(0);
		if( !l_prpNumLim.Get(1).empty() )
			m_ptLim.y = (LONG)l_prpNumLim.GetLong(1);
	}

	ctl_SetRange(m_ptLim.x, m_ptLim.y);
}

bool CSliderItem::SetValue(const cVariant &pValue)
{
	tSIZE_T nVal = (tSIZE_T)pValue.ToDWORD();
	m_nCurPos = nVal;
	
	if( ctl_IsVert() )
		nVal = m_ptLim.y - nVal;
	ctl_SetPos(nVal);
	return true;
}

bool CSliderItem::GetValue(cVariant &pValue)
{
	tSIZE_T nVal = ctl_GetPos();
	if( ctl_IsVert() )
		nVal = m_ptLim.y - nVal;
	
	pValue = (tDWORD)nVal;
	return true;
}

void CSliderItem::_DataChanged()
{
	tSIZE_T nPos = ctl_GetPos();
	if( m_nCurPos != nPos )
		OnChangedData(this), m_nCurPos = nPos;
}

//////////////////////////////////////////////////////////////////////
// CDialogItem

#define DLG_FIXITEM_PANEL             "DialogPanel"

#define PLACEMENT_VAL                 "Placement"
#define PLACEMENT_FMT                 "x=%d, y=%d, cx=%d, cy=%d, show=%u, relative=%u, expanded=%d, exp_cy=%d"

#define TRANSPARENT_UP_SPEED          1000  // count per second
#define TRANSPARENT_DOWN_SPEED        128   // count per second
#define TRANSPARENT_STEP_TIME         25    // milliseconds

#define _IS_VALIDATOR_ACTIVE(_n)		!!(_n)
#define _VALIDATOR_ENABLE(_n)			!(_n&1)

CDialogItem::CDialogItem() :
	m_nDlgStyles(0), m_nDlgFlags(0), m_pDlgIcon(NULL),
	
	m_nTimeout(-1), m_nAutoCloseTimeout(-1), m_fModal(0),
	
	m_pCtxItem(NULL), m_pFooter(NULL), m_pOk(NULL), m_pCancel(NULL), m_pNo(NULL),
	m_pApply(NULL), m_pNext(NULL), m_pBack(NULL), m_pSheet(NULL), m_pExpand(NULL), m_pCollapse(NULL), m_pWndResizer(NULL),
	m_pNodeApply(NULL),
	
	m_fDirty(0), m_bActive(0), m_bSmartTransparency(1),
	m_bCloseDirect(0), m_bUserClose(0), m_bMaximize(0),
	m_bExpanded(0),
	m_nResult(DLG_ACTION_CANCEL),
	m_nRelativeLoad(0),

	m_nCurIdx(-1),
	
	m_nTransparencyTimerId(0), m_nTransparencyFrom(0),
	m_nTransparencyTo(0), m_nTransparency(0)
{
	m_nFlags |= STYLE_DRAW_BACKGROUND|STYLE_TOP_ITEM|STYLE_TRANSPARENT;
	memset(&m_szSizeExp, 0, sizeof(m_szSizeExp));
	memset(&m_rcRectLoad, 0, sizeof(m_rcRectLoad));
}

void CDialogItem::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	sswitch(strStyle)
	scase("resizable")          m_nDlgStyles |= DLG_STYLE_RESIZABLE; sbreak;
	scase("dragable")           m_nDlgStyles |= DLG_STYLE_DRAGABLE; sbreak;
	scase("bottom")             m_nDlgStyles |= DLG_STYLE_BOTTOM; sbreak;
	scase("defsize")            m_nDlgStyles |= DLG_STYLE_DEFAULT; sbreak;
	scase("noautofocus")        m_nDlgStyles |= DLG_STYLE_NOAUTOFOCUS; sbreak;
	scase("layered")            if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) ) m_nDlgStyles |= DLG_STYLE_LAYERED; sbreak;
	scase("rounded")            m_nDlgStyles |= DLG_STYLE_ROUNDED; sbreak;

	scase("wizard")      m_nFlags |= STYLE_SHEET_ITEM; m_nDlgFlags |= DLG_ACTION_WIZARD; sbreak;
	scase("nohelp")      m_nDlgFlags |= DLG_MODE_NOHELP; sbreak;
	scase("a_ok")        m_nDlgFlags |= DLG_ACTION_OK; sbreak;
	scase("a_cancel")    m_nDlgFlags |= DLG_ACTION_CANCEL; sbreak;
	scase("a_close")     m_nDlgFlags |= DLG_ACTION_CLOSE; sbreak;
	scase("a_yes")       m_nDlgFlags |= DLG_ACTION_YES; sbreak;
	scase("a_no")        m_nDlgFlags |= DLG_ACTION_NO; sbreak;
	scase("a_apply")     m_nDlgFlags |= DLG_ACTION_APPLY; sbreak;
	scase("a_next")      m_nDlgFlags |= DLG_ACTION_NEXT; sbreak;
	scase("a_back")      m_nDlgFlags |= DLG_ACTION_BACK; sbreak;
	scase("expandable")  m_nDlgFlags |= DLG_MODE_EXPANDABLE; sbreak;
    scase("customfooter")m_nDlgFlags |= DLG_MODE_CUSTOMFOOTER; sbreak;
	send
}

CFieldsBinder* CDialogItem::GetBinder()
{
	if( m_pBinding )
		return m_pBinding->m_pBinder;

	// поскольку parent диалога должен быть top item, нужен элемент, с которого
	// можно было бы вз€ть родительский биндер.
	if( m_pCtxItem )
		return m_pCtxItem->GetBinder();

	if( m_pParent )
		return m_pParent->GetBinder();

	return NULL;
}

void CDialogItem::ModifyDlgFlags(tDWORD dwAdd, tDWORD dwRemove)
{
	m_nDlgFlags |= dwAdd;
	m_nDlgFlags &= ~dwRemove;
}

void CDialogItem::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);

	m_pNodeApply = CItemBinding::InitHolder(this, strProps, STR_PID_ONAPPLY);
	
	if( !m_pIcon )
	{
		if( CIcon * pMain = m_pRoot->GetIcon(ICONID_DEFAULT) )
			m_pDlgIcon = m_pRoot->CreateIcon(pMain->IconHnd());
		
		m_pIcon = NULL;
	}
	
	{
		CItemPropVals& l_prpSize = strProps.Get(STR_PID_SIZE);
		m_szSizeDef.cx = (LONG)l_prpSize.GetLong(2);
		m_szSizeDef.cy = (LONG)l_prpSize.GetLong(3);
	}

	if( m_nFlags & STYLE_SHEET_ITEM )
	{
		m_pSheet = GetByFlagsAndState(STYLE_SHEET_PAGES, ISTATE_ALL);
		if( !m_pSheet )
		{
			m_nFlags |= STYLE_SHEET_PAGES;
			m_pSheet = this;
		}
	}
	else
	{
		m_pSheet = GetByFlagsAndState(STYLE_SHEET_ITEM, ISTATE_ALL);
		if( m_pSheet && m_pSheet->m_strItemType != GUI_ITEMT_TAB )
			m_pSheet = NULL;
	}
}

void CDialogItem::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);
	m_pRoot->AddDialog(this);
}

bool CDialogItem::IsResizeble(bool bHorz)
{
	if( bHorz || !IsExpandable() )
		return true;

	return IsExpanded();
}

void CDialogItem::ShowPageItem(CItemBase * pItem, bool bShow)
{
	if( bShow == !(pItem->m_nState & ISTATE_SHEETPAGE_HIDE) )
		return;
	
	if( bShow )
		pItem->m_nState &= ~ISTATE_SHEETPAGE_HIDE;
	else
		pItem->m_nState |= ISTATE_SHEETPAGE_HIDE;
}

void CDialogItem::Clear()
{
	if( m_pNodeApply )
		m_pNodeApply->Destroy(), m_pNodeApply = NULL;
	
	TBase::Clear();
	if( m_pDlgIcon )
		m_pDlgIcon->Destroy(), m_pDlgIcon = NULL;

	m_pFooter = NULL;
	m_pOk = NULL;
	m_pCancel = NULL;
	m_pNo = NULL;
	m_pApply = NULL;
	m_pNext = NULL;
	m_pBack = NULL;
	m_pSheet = NULL;
	m_pExpand = NULL;
	m_pCollapse = NULL;
	m_pWndResizer = NULL;

	m_eb.clear();
	m_ebs.clear();
}

void CDialogItem::UpdateView(int nFlags)
{
	tDWORD bValidateCancel = 0;
	tDWORD bValidateOk = 0;
	tDWORD bValidateNext = 0;
	tDWORD bValidatePrev = 0;
	
	if( m_pSheet )
	{
		if( m_nFlags & STYLE_SHEET_ITEM )
		{
			CItemBase * pCurPage = GetCurPage();

			EnableAction(DLG_ACTION_NEXT, _VALIDATOR_ENABLE(ProcessValidator(pCurPage, DLG_ACTION_NEXT)) );

			{
				bool bBack = _GetNextIdx(m_nCurIdx, true) != -1;
				if( bBack )
					bBack = _VALIDATOR_ENABLE(ProcessValidator(pCurPage, DLG_ACTION_BACK));

				EnableAction(DLG_ACTION_BACK, bBack);
			}

			bValidateOk |= ProcessValidator(pCurPage, DLG_ACTION_OK);
			bValidateCancel |= ProcessValidator(pCurPage, DLG_ACTION_CANCEL);

			bool bFinish = (_GetNextIdx(m_nCurIdx) == -1);
			m_pOk = bFinish ? m_pNext : NULL;
			if( m_pNext )
			{
				m_pNext->m_pUserData = (void *)(bFinish ? DLG_ACTION_OK : DLG_ACTION_NEXT);
				if( !(m_nDlgFlags & DLG_MODE_ALWAYSNEXT) )
					m_pNext->SetText(m_pRoot->LoadLocString(TOR_tString, bFinish ? "Finish" : "Next"));
			}
		}
		else
		{
			for(tSIZE_T i = 0, n = m_pSheet->m_aItems.size(); i < n; i++)
			{
				CItemBase * pPage = m_pSheet->m_aItems[i];
				if( pPage != m_pFooter )
				{
					bValidateOk |= ProcessValidator(pPage, DLG_ACTION_OK);
					bValidateCancel |= ProcessValidator(pPage, DLG_ACTION_CANCEL);
				}
			}
		}
	}

	bValidateOk |= ProcessValidator(this, DLG_ACTION_OK);
	bValidateCancel |= ProcessValidator(this, DLG_ACTION_CANCEL);
	
//	if( _IS_VALIDATOR_ACTIVE(bValidateOk) )
		EnableAction(DLG_ACTION_OK, _VALIDATOR_ENABLE(bValidateOk));
//	if( _IS_VALIDATOR_ACTIVE(bValidateCancel) )
		EnableAction(DLG_ACTION_CANCEL, _VALIDATOR_ENABLE(bValidateCancel));
}

CItemBase * CDialogItem::GetCurPage()
{
	if( !(m_nFlags & STYLE_SHEET_ITEM) || !m_pSheet )
		return NULL;
	
	if( (tNATIVE_INT)m_nCurIdx < 0 || m_nCurIdx >= m_pSheet->m_aItems.size() )
		return NULL;
	
	return m_pSheet->m_aItems[m_nCurIdx];
}

bool CDialogItem::SetCurPage(CItemBase * pPage)
{
	if( !(m_nFlags & STYLE_SHEET_ITEM) || !m_pSheet )
		return false;

	if( !pPage )
		return false;

	m_pSheet->LockUpdate(true);
	
	bool bSelChanging = m_nCurIdx != pPage->m_nPos;
	if( bSelChanging )
	{
		if( m_nCurIdx != -1 )
			_SelectItem(m_pSheet->m_aItems[m_nCurIdx], false);
		m_nCurIdx = pPage->m_nPos;
	}
	
	UpdateView();
	
	_SelectItem(pPage, true);

	m_pSheet->LockUpdate(false);

	if( bSelChanging )
	{
		if( pPage->m_pBinding )
			pPage->m_pBinding->ProcessReflection(CItemBinding::hOnSelect);
		OnSelect(pPage);
	}
	return true;
}

bool CDialogItem::SetCurPage(const char * pPageId)
{
	if( CItemBase *pPage = m_pSheet->GetItem(pPageId, NULL, false) )
		return SetCurPage(pPage);
	return false;
}

bool CDialogItem::SetNextPage(bool bBackward)
{
	if( !(m_nFlags & STYLE_SHEET_ITEM) )
		return false;

	if( bBackward )
	{
		if( m_pSink && !((CDialogSink *)m_pSink)->OnPrev() )
			return false;
	}
	else if( !_Next() )
		return false;

	tSIZE_T nNextPage = _GetNextIdx(m_nCurIdx, bBackward);
	if( nNextPage == -1 )
	{
		if( !bBackward )
			_Ok();
		return false;
	}
	
	SetCurPage(m_pSheet->m_aItems[nNextPage]);
	return true;
}

void CDialogItem::OnInit()
{
	if( m_pSink )
	{
		tDWORD nDlgFlags = m_nDlgFlags;
		((CDialogSink *)m_pSink)->OnInitDialog(m_nDlgFlags);
		
		if( nDlgFlags & DLG_MODE_BUTTONS )
		{
			m_nDlgFlags &= ~DLG_MODE_BUTTONS;
			m_nDlgFlags |= nDlgFlags & DLG_MODE_BUTTONS;
		}
	}
	
	if( m_nDlgFlags & DLG_MODE_NEWDATA )
		SetDirty();
	
	if( m_nDlgFlags & DLG_MODE_BUTTONS )
	{
		if( !(m_nDlgFlags & (DLG_ACTION_CLOSE|DLG_ACTION_CANCEL)) )
			_EnableSysClose(GetWindow(), false);

		m_pFooter = NULL;
        if( m_nDlgFlags & DLG_MODE_CUSTOMFOOTER )
            m_pFooter = GetItem("Footer");
        else if( m_nDlgFlags & DLG_MODE_DLGBUTTONS )
		    m_pFooter = this;
	    else
		{
		    LoadItem(NULL, m_pFooter, DLG_FIXITEM_PANEL, NULL, LOAD_FLAG_PRELOAD);
			if( m_pFooter )
			{
				m_pFooter->m_rcShift.left   = - m_rcMargin.left;
				//m_pFooter->m_rcShift.top    = - m_rcMargin.top;
				m_pFooter->m_rcShift.right  = - m_rcMargin.right;
				m_pFooter->m_rcShift.bottom = - m_rcMargin.bottom;
			}
		}

		_InitButton("Confirm",  DLG_ACTION_OK|DLG_ACTION_YES);
		_InitButton("Cancel",   DLG_ACTION_CANCEL|DLG_ACTION_CLOSE);
		_InitButton("No",       DLG_ACTION_NO);
		_InitButton("Apply",    DLG_ACTION_APPLY);
		_InitButton("Next",     DLG_ACTION_NEXT);
		_InitButton("Back",     DLG_ACTION_BACK);

		if( m_pApply )
			SetDirty(false);

		if( !m_pNext && m_pFooter )
			m_pFooter->Show("Next", false);
		if( !m_pBack && m_pFooter )
			m_pFooter->Show("Back", false);

		if( (m_nDlgFlags & DLG_MODE_CENTERBUTTONS) && m_pFooter )
			if( CItemBase * pButtonsGroup = m_pFooter->GetItem("BtnsGroup") )
			{
				pButtonsGroup->m_nFlags &= ~ALIGN_RIGHT;
				pButtonsGroup->m_nFlags |= ALIGN_HCENTER;
			}
	}
	
	if( m_nDlgFlags & DLG_MODE_EXPANDABLE )
	{
		//if( !m_pFooter )
		//	m_pFooter = this;

		_InitButton("Expand",   DLG_ACTION_EXPAND);
		_InitButton("Collapse", DLG_ACTION_COLLAPSE);
	}

	if( m_pFooter && ((m_pRoot->m_nGuiFlags & GUIFLAG_NOHELP) || (m_nDlgFlags & DLG_MODE_NOHELP)) )
		m_pFooter->Show("Help", false);

	m_pWndResizer = (m_pFooter ? m_pFooter : this)->GetItem("WndResizer");

	TBase::OnInit();
	
	if( m_pSheet )
	{
		m_ebs.resize(m_pSheet->m_aItems.size());
		
		for(CItemBase * pPage = m_pSheet->GetChildFirst(false); pPage; pPage = pPage->GetNext(false))
		{
			if( pPage == m_pFooter )
				continue;
			
			if( m_nFlags & STYLE_SHEET_ITEM )
				pPage->Show(false, SHOWITEM_DIRECT);

			AddValidators(pPage);
		}
	}

	AddValidators(this);

	if( (m_nDlgStyles & DLG_STYLE_LAYERED) && m_bSmartTransparency )
		ctl_SetTransparentValue(0);
}

void CDialogItem::OnInited()
{
	TBase::OnInited();

	tUINT nDlgPos = _GetDlgPosOffset();

	if( m_szSizeDef.cx < m_szSize.cx )
		m_szSizeDef.cx = m_szSize.cx;

	if( m_szSizeDef.cy < m_szSize.cy )
		m_szSizeDef.cy = m_szSize.cy;

	if( m_szSizeExp.cy == 0 )
		m_szSizeExp.cy = m_szSizeDef.cy - m_szSizeMin.cy;
	m_szSizeMinTmp.cy = m_szSizeMin.cy;

	RECT rcFrame, rcScreen;
	_GetGetWorkArea(rcScreen);

	RECT rcMinFrame = {0, 0, m_szSizeMin.cx, m_szSizeMin.cy};
	OnAdjustBordSize(rcMinFrame);

	{
		int x = m_rcRectLoad.left, y = m_rcRectLoad.top, cx = RECT_CX(m_rcRectLoad), cy = RECT_CY(m_rcRectLoad);
		POINT absOffsets[] =
		{
			{x,                                y},
			{RECT_CX(rcScreen) - (x + cx),     y},
			{x,                                RECT_CY(rcScreen) - (y + cy)},
			{RECT_CX(rcScreen) - (x + cx),     RECT_CY(rcScreen) - (y + cy)},
			{RECT_CX(rcScreen)/2 - (x + cx/2), RECT_CY(rcScreen)/2 - (y + cy/2)},
		};

		if( (!x && !y && !cx && !cy) || m_nRelativeLoad >= countof(absOffsets) )
			_InitDefaultRect(rcScreen, rcFrame);
		else
		{
			rcFrame.left   = rcScreen.left + absOffsets[m_nRelativeLoad].x;
			rcFrame.top    = rcScreen.top  + absOffsets[m_nRelativeLoad].y;
			rcFrame.right  = rcFrame.left + cx;
			rcFrame.bottom = rcFrame.top + cy;
		}
	}

	if( RECT_CX(rcFrame) < RECT_CX(rcMinFrame) ) rcFrame.right  = rcFrame.left + RECT_CX(rcMinFrame);
	if( RECT_CY(rcFrame) < RECT_CY(rcMinFrame) ) rcFrame.bottom = rcFrame.top + RECT_CY(rcMinFrame);

	int	nMargin = _GetSameWindowOffset() * nDlgPos;
	if( nMargin )
		osd_OffsetRect(&rcFrame, nMargin, nMargin);

	if( !(m_nDlgStyles & DLG_STYLE_RESIZABLE) || (m_nDlgStyles & DLG_STYLE_LAYERED) )
	{
		if( m_nFlags & AUTOSIZE )
		{
			if( m_nFlags & AUTOSIZE_X )
			{
				if( m_nDlgStyles & DLG_STYLE_BOTTOM )
					rcFrame.left   = rcFrame.right  - (m_szSize.cx + RECT_CX(rcMinFrame) - m_szSizeMin.cx);
				else
					rcFrame.right  = rcFrame.left   + (m_szSize.cx + RECT_CX(rcMinFrame) - m_szSizeMin.cx);
			}
			
			if( m_nFlags & AUTOSIZE_Y )
			{
				if( m_nDlgStyles & DLG_STYLE_BOTTOM )
					rcFrame.top    = rcFrame.bottom - (m_szSize.cy + RECT_CY(rcMinFrame) - m_szSizeMin.cy);
				else
					rcFrame.bottom = rcFrame.top    + (m_szSize.cy + RECT_CY(rcMinFrame) - m_szSizeMin.cy);
			}
		}
	}

	// Check all visible
	if( rcFrame.right > rcScreen.right || rcFrame.bottom > rcScreen.bottom )
	{
		if( rcFrame.right > rcScreen.right || rcFrame.bottom > rcScreen.bottom )
			osd_OffsetRect(&rcFrame,
				(rcFrame.right > rcScreen.right)   ? (rcScreen.right - rcFrame.right) : 0,
				(rcFrame.bottom > rcScreen.bottom) ? (rcScreen.bottom - rcFrame.bottom) : 0);

		if( rcFrame.left < 0 || rcFrame.top < 0 )
			osd_OffsetRect(&rcFrame,
				(rcFrame.left < 0) ? -rcFrame.left : 0,
				(rcFrame.top < 0)  ? -rcFrame.top : 0);
	}

	_InitDlgRect(rcFrame);

	if( m_nFlags & STYLE_SHEET_ITEM )
	{
		tSIZE_T nPage = _GetNextIdx((m_nCurIdx == -1) ? -1 : (m_nCurIdx - 1));
		if( nPage != -1 )
			SetCurPage(m_pSheet->m_aItems[nPage]);
	}

	UpdateView();

	if( IsExpandable() )
		Expand(m_bExpanded, false);

	{
		CItemBase * pBtnDef = NULL;
		if( !pBtnDef ) { CItemBase * pBtn = m_pOk;              if( pBtn && !(pBtn->m_nState & ISTATE_HIDE) ) pBtnDef = pBtn; }
		if( !pBtnDef ) { CItemBase * pBtn = GetItem("Confirm"); if( pBtn && !(pBtn->m_nState & ISTATE_HIDE) ) pBtnDef = pBtn; }
		if( !pBtnDef ) { CItemBase * pBtn = m_pNext;            if( pBtn && !(pBtn->m_nState & ISTATE_HIDE) ) pBtnDef = pBtn; }
		if( !pBtnDef ) { CItemBase * pBtn = m_pCancel;          if( pBtn && !(pBtn->m_nState & ISTATE_HIDE) ) pBtnDef = pBtn; }
		if( !pBtnDef ) { CItemBase * pBtn = GetItem("Close");   if( pBtn && !(pBtn->m_nState & ISTATE_HIDE) ) pBtnDef = pBtn; }
		
		if( pBtnDef && pBtnDef->m_strItemType == GUI_ITEMT_BUTTON )
			pBtnDef->SetDefault(true);
	}
}

void CDialogItem::SetShowMode(eShow eShowMode)
{
	OnChangeShowMode(eShowMode);
};

void CDialogItem::OnChangeShowMode(eShow eNewMode)
{
	if( IsExpandable() )
	{
		if( eNewMode == showMaximazed && !IsExpanded() )
		{
			Expand(true);
		}
	}

	if( m_pWndResizer )
	{
		if( eNewMode == showMaximazed )
			m_pWndResizer->Show(false);
		else if( eNewMode == showNormal )
			m_pWndResizer->Show(!!(m_nDlgStyles & DLG_STYLE_RESIZABLE));
	}
}

void CDialogItem::Expand(bool bExpand, bool bResize)
{
	m_bExpanded = bExpand ? 1 : 0;

	for(CItemBase * pChild = GetChildFirst(false); pChild; pChild = GetNextItem(pChild, NULL))
	{
		if( pChild->m_bHideExpanded || pChild->m_bHideCollapsed )
		{
			if( m_bExpanded && pChild->m_bHideExpanded ||
				!m_bExpanded && pChild->m_bHideCollapsed )
			{
				// ƒелаем элемент невидимым
				pChild->Show(false);
			}
			else
			{
				// ƒелаем элемент видимым, но надо учесть и биндинг, 
				// который также может повли€ть на видимость элемента
				pChild->m_nState &= ~ISTATE_HIDDENEX;

				// ƒергаем биндинг, чтобы он пересчитал видимость
				UpdateData(true, &CStructData(NULL,NULL));

				// ≈сли биндинг не помен€л флаг на невидимость, то окончательно делаем
				// элемент видимым
				if( !(pChild->m_nState & ISTATE_HIDDENEX) )
				{
					pChild->m_nState |= ISTATE_HIDDENEX;
					pChild->Show(true);
				}
			}
		}
	}

	if( bResize )
	{
		if( bExpand )
		{
			m_szSizeMinTmp.cy = m_szSizeMin.cy;
			m_szSizeMin.cy = m_szSizeDef.cy;
			Resize(0, m_szSizeExp.cy);
		}
		else
		{
			eShow showCmd;
			RECT rc;
			if( _GetWindowPlacement(GetWindow(), rc, showCmd) && showCmd == showMaximazed )
				SetShowMode(showNormal);

			m_szSizeMin.cy = m_szSizeMinTmp.cy;
			if( m_szSize.cy != m_szSizeMin.cy )
			{
				m_szSizeExp.cy = m_szSize.cy - m_szSizeMin.cy;
				Resize(0, -m_szSizeExp.cy);
			}
		}
	}
}

bool CDialogItem::_Next()
{
	if( !(m_nFlags & STYLE_SHEET_ITEM) )
		return true;

	CItemBase * pPage = GetCurPage();
	for(CItemBase * p = pPage; p; p = pPage->EnumNextItem(p))
	{
		if( !p->m_pBinding )
			continue;

		cVar vRes; p->m_pBinding->ProcessReflection(CItemBinding::hOnOk, vRes);
		if( vRes.Type() == cVariant::vtInt && !vRes.ToDWORD() )
			return false;
	}

	if( m_pSink && !((CDialogSink *)m_pSink)->OnNext() )
		return false;

	return true;
}

bool CDialogItem::_Apply()
{
	if( !(m_nFlags & STYLE_SHEET_ITEM) )
		for(CItemBase * p = this; p = EnumNextItem(p); )
		{
			if( !p->m_pBinding )
				continue;

			cVar vRes; p->m_pBinding->ProcessReflection(CItemBinding::hOnOk, vRes);
			if( vRes.Type() == cVariant::vtInt && !vRes.ToDWORD() )
				return false;
		}

	if( m_pBinding )
	{
		cVar vRes; m_pBinding->ProcessReflection(CItemBinding::hOnOk, vRes);
		if( vRes.Type() == cVariant::vtInt && !vRes.ToDWORD() )
			return false;
	}

	if( m_fDirty )
	{
		if( m_pSink && !((CDialogSink *)m_pSink)->OnApply() )
			return false;

		if( m_pNodeApply )
		{
			cVar vRes; m_pNodeApply->Exec(vRes, TOR(cNodeExecCtx, (m_pRoot, this)));
			if( vRes.Type() == cVariant::vtInt && !vRes.ToDWORD() )
				return false;
		}
	}

	return true;
}

bool CDialogItem::_Ok()
{
	if( !_Next() )
		return false;
	if( !_Apply() )
		return false;

	if( m_pSink && !((CDialogSink *)m_pSink)->OnOk() )
		return false;

	Close(m_pOk ? (tDWORD)m_pOk->m_pUserData : DLG_ACTION_OK);
	return true;
}

bool CDialogItem::OnClicked(CItemBase* pItem)
{
	if( pItem == m_pCancel )
		return CloseConfirm();

	if( pItem == m_pOk )
		return _Ok();
	
	if( pItem == m_pApply )
	{
		if( _Apply() )
			SetDirty(false);
		return false;
	}
	
	if( pItem == m_pNext )
	{
		SetNextPage(false);
		return false;
	}
	else if( pItem == m_pBack )
	{
		SetNextPage(true);
		return false;
	}
	else if( pItem == m_pNo )
	{
	}
	else if( pItem == m_pExpand )
	{
		Expand(true);
		return false;
	}
	else if( pItem == m_pCollapse )
	{
		Expand(false);
		return false;
	}
	else
		return TBase::OnClicked(pItem);

	Close((tDWORD)pItem->m_pUserData);
	return false;
}

void CDialogItem::TimerRefresh(tDWORD nTimerSpin)
{
	TBase::TimerRefresh(nTimerSpin);
	
	if( m_nTimeout == -1 || IsActual() )
		return;
	
	if( m_nAutoCloseTimeout > TIMER_REFRESH_INT )
	{
		m_nAutoCloseTimeout -= TIMER_REFRESH_INT;
		return;
	}
	
	if( !m_bSmartTransparency || !(m_nDlgStyles & DLG_STYLE_LAYERED) )
		m_bCloseDirect = 1, _CloseDlg();

	_SetTransparent(TRANS_NULL);
}

tDWORD CDialogItem::DoModal()
{
	AddRef();
	m_pRoot->DoModalDialog(this, (m_nDlgFlags & DLG_MODE_ALWAYSMODAL) ? true : false);
	
	tDWORD nRes = m_nResult;
	Release();
	return nRes;
}

void CDialogItem::SetTimeout(tDWORD nTimeout)
{
	m_nTimeout = m_nAutoCloseTimeout = nTimeout;
	if( (m_nState & ISTATE_INITED) && m_bSmartTransparency )
		_SetTransparent(IsActual() ? TRANS_FULL : TRANS_MED);
}

void CDialogItem::SetErrResult(tERROR err)
{
	m_pRoot->SetDialogErrResult(this, err);
}

void CDialogItem::_InitButton(LPCSTR pBtnName, tDWORD nActions)
{
	if( !m_pFooter )
		return;

	CItemBase * pBtn = m_pFooter->GetItem(pBtnName);
	if( !pBtn )
		return;

	if( m_nDlgFlags & nActions & DLG_ACTION_OK )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Ok")),     m_pOk = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_YES )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Yes")),    m_pOk = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_NEXT )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Next")),   m_pNext = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_BACK )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Back")),   m_pBack = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_CANCEL )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Cancel")), m_pCancel = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_CLOSE )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Close")),  m_pCancel = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_APPLY )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Apply")),  m_pApply = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_NO )
	{
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "No"));
		if( m_nDlgFlags & (DLG_ACTION_CANCEL|DLG_ACTION_CLOSE) )
			m_pNo = pBtn;
		else
			m_pCancel = pBtn;
	}
	else if( m_nDlgFlags & nActions & DLG_ACTION_EXPAND )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Expand")),   m_pExpand = pBtn;
	else if( m_nDlgFlags & nActions & DLG_ACTION_COLLAPSE )
		pBtn->SetText(m_pRoot->LoadLocString(TOR_tString, "Collapse")),   m_pCollapse = pBtn;

	pBtn->Show(!!(m_nDlgFlags & nActions));
	pBtn->m_pUserData = (void*)(m_nDlgFlags & nActions);
}

void CDialogItem::SetDirty(bool bDirty)
{
	m_fDirty = bDirty;

	if( m_pOk || m_pApply )
	{
		if( m_pApply )
			m_pApply->Enable(m_fDirty);
		if( m_pCancel )
			m_pCancel->SetText(m_pRoot->LoadLocString(TOR_tString, m_fDirty ? "Cancel" : "Close"));
	}
}

void CDialogItem::EnableAction(tDWORD nAction, bool bEnable)
{
	CItemBase* pButton = NULL;
	if( (nAction & (DLG_ACTION_OK|DLG_ACTION_YES)) && m_pOk)
		m_pOk->Enable(bEnable);
	
	if( (nAction & DLG_ACTION_BACK) && m_pApply )
		m_pApply->Enable(bEnable);
	
	if( (nAction & (DLG_ACTION_CANCEL|DLG_ACTION_NO)) && m_pCancel )
	{
		m_pCancel->Enable(bEnable);
		_EnableSysClose(GetWindow(), bEnable);
	}
	
	if( (nAction & DLG_ACTION_NEXT) && m_pNext )
		m_pNext->Enable(bEnable);
	
	if( (nAction & DLG_ACTION_BACK) && m_pBack )
		m_pBack->Enable(bEnable);
}

bool CDialogItem::CloseConfirm()
{
	m_bUserClose = 1;
	return _CloseDlg();
}

void CDialogItem::DoAutomate()
{
	for(;;)
	{
		while(m_pNext && m_pNext->m_nState & ISTATE_DISABLED)
			m_pRoot->MessageLoop(&CItemBase());
		
		if( !SetNextPage() )
			break;
	}
}

//////////////////////////////////////////////////////////////////////

void CDialogItem::_SetTransparent(eTrans eOp)
{
	if( !(m_nDlgStyles & DLG_STYLE_LAYERED) || !m_bSmartTransparency )
		return;
	
	m_nTransparencyFrom = m_nTransparency;
	switch( eOp )
	{
	case TRANS_FULL: m_nTransparencyTo = 255; break;
	case TRANS_MED:  m_nTransparencyTo = m_pRoot->m_nDlgTransparency; break;
	case TRANS_NULL: m_nTransparencyTo = 0;   break;
	}
	
	if( m_nTransparencyFrom == m_nTransparencyTo )
		return;

	ctl_SetTransparentValue(m_nTransparencyFrom);

	if( !m_nTransparencyTimerId )
		m_nTransparencyTimerId = ctl_SetTransparentTimer(TRANSPARENT_STEP_TIME);
}

void CDialogItem::ctl_OnTransparentTimer()
{
	bool bUp = m_nTransparencyTo >= m_nTransparencyFrom;
	if( bUp )
	{
		int nStep = TRANSPARENT_UP_SPEED/(1000/TRANSPARENT_STEP_TIME);
		
		m_nTransparency = m_nTransparencyFrom + (m_nTransparency - m_nTransparencyFrom) + nStep;
		if( m_nTransparency == m_nTransparencyFrom )
			m_nTransparency++;
		
		if( m_nTransparency > m_nTransparencyTo )
			m_nTransparency = m_nTransparencyTo;
	}
	else
	{
		int nStep = TRANSPARENT_DOWN_SPEED/(1000/TRANSPARENT_STEP_TIME);

		m_nTransparency = m_nTransparencyTo + (m_nTransparency - m_nTransparencyTo) - nStep;
		if( m_nTransparency < m_nTransparencyTo || m_nTransparency > m_nTransparencyFrom )
			m_nTransparency = m_nTransparencyTo;
	}
	
	ctl_SetTransparentValue(m_nTransparency);

	if( m_nTransparency == m_nTransparencyTo )
		ctl_KillTransparentTimer(m_nTransparencyTimerId), m_nTransparencyTimerId = 0;
	
	if( !m_nTransparency && (m_nTimeout != INFINITE) )
		m_bCloseDirect = 1, _CloseDlg();
}

void CDialogItem::ctl_OnActivate(bool bActive)
{
	m_bActive = bActive;
	
	if( m_bActive )
		_ResetTimeout();
	
	_SetTransparent(m_bActive ? TRANS_FULL : TRANS_MED);
	
	CItemBase::OnActivate(this, m_bActive);
}

void CDialogItem::ctl_OnMouseMove()
{
	_ResetTimeout();
	_SetTransparent(TRANS_FULL);
}

void CDialogItem::_InitDefaultRect(RECT& rcScreen, RECT& rcFrame)
{
	rcFrame = rcScreen;

	SIZE szDef;
	{
		RECT rcDefTmp = {0, 0, m_szSizeDef.cx, m_szSizeDef.cy};
		OnAdjustBordSize(rcDefTmp);
		
		szDef.cx = RECT_CX(rcDefTmp);
		szDef.cy = RECT_CY(rcDefTmp);
	}
	
	int cx, cy;
	if( m_nDlgStyles & (DLG_STYLE_BOTTOM|DLG_STYLE_DEFAULT) )
	{
		if( m_nDlgStyles & DLG_STYLE_BOTTOM )
		{
			rcFrame.bottom -= m_rcShift.bottom;
			rcFrame.right -= m_rcShift.right;
			rcFrame.left = rcFrame.right  - szDef.cx;
			rcFrame.top  = rcFrame.bottom - szDef.cy;
		}
		else
		{
			cx = RECT_CX(rcFrame) / 8;
			cy = RECT_CY(rcFrame) / 6;
		}
	}
	else
	{
		CItemBase * pParent = m_pParent ? m_pParent->GetOwner(true) : NULL;
		if( pParent )
		{
			eShow nShow;
			_GetWindowPlacement(pParent->GetWindow(), rcFrame, nShow);
		}
		
		cx = (RECT_CX(rcFrame) - szDef.cx) / 2;
		cy = (RECT_CY(rcFrame) - szDef.cy) / 2;
	}

	if( !(m_nDlgStyles & DLG_STYLE_BOTTOM) )
	{
		rcFrame.left   += cx;
		rcFrame.right  = rcFrame.left + szDef.cx;
		rcFrame.top    += cy;
		rcFrame.bottom = rcFrame.top + szDef.cy;
	}

	if( rcFrame.right > rcScreen.right || rcFrame.bottom > rcScreen.bottom )
		osd_OffsetRect(&rcFrame,
			(rcFrame.right > rcScreen.right)   ? (rcScreen.right - rcFrame.right) : 0,
			(rcFrame.bottom > rcScreen.bottom) ? (rcScreen.bottom - rcFrame.bottom) : 0);

	if( rcFrame.left < 0 || rcFrame.top < 0 )
		osd_OffsetRect(&rcFrame,
			(rcFrame.left < 0) ? -rcFrame.left : 0,
			(rcFrame.top < 0)  ? -rcFrame.top : 0);
}

void CDialogItem::LoadSettings(cAutoRegKey& pKey)
{
	TBase::LoadSettings(pKey);

	if( m_nDlgFlags & DLG_MODE_NOSAVEPOS )
		return;

	cStrObj data;
	if( PR_FAIL(pKey.get_strobj(PLACEMENT_VAL, data)) )
		return;

	int  x, y, cx, cy, expanded, exp_cy;
	expanded = exp_cy = 0;
	eShow showCmd = showNormal;
	tUINT nRelative = 0;
	
	if( sscanf(data.c_str(cCP_ANSI), PLACEMENT_FMT, &x, &y, &cx, &cy, &showCmd, &nRelative, &expanded, &exp_cy) < 5 )
		return;

	if( exp_cy > 0 )
	{
		m_szSizeExp.cy = exp_cy;
		m_bExpanded = expanded ? 1 : 0;
	}

	m_rcRectLoad.left   = x;
	m_rcRectLoad.top    = y;
	m_rcRectLoad.right  = m_rcRectLoad.left + cx;
	m_rcRectLoad.bottom = m_rcRectLoad.top  + cy;

	m_nRelativeLoad = nRelative;
	
	m_bMaximize = showCmd == showMaximazed;
}

void CDialogItem::SaveSettings(cAutoRegKey& pKey)
{
	TBase::SaveSettings(pKey);

	if( (m_nDlgFlags & DLG_MODE_NOSAVEPOS) || !_CheckDlgPosOffset() || !pKey.opened() )
		return;

	int  x, y, cx, cy, expanded, exp_cy;
	eShow showCmd = showNormal;
	tUINT nRelative = 0;

	expanded = m_bExpanded;
	exp_cy = m_szSizeExp.cy;

	{
		RECT rc;
		if( !_GetWindowPlacement(GetWindow(), rc, showCmd) )
			return;
		
		RECT rcScreen; _GetGetWorkArea(rcScreen);

		cx = RECT_CX(rc);
		cy = RECT_CY(rc);
		
		POINT relOffsets[] =
		{
			/* Relative LeftTop */    {rc.left,                                rc.top},
			/* Relative RightTop */	  {RECT_CX(rcScreen) - (rc.left + cx),     rc.top},
			/* Relative LeftBottom */ {rc.left,                                RECT_CY(rcScreen) - (rc.top + cy)},
			/* Relative LeftBottom */ {RECT_CX(rcScreen) - (rc.left + cx),     RECT_CY(rcScreen) - (rc.top + cy)},
			/* Relative Center */     {RECT_CX(rcScreen)/2 - (rc.left + cx/2), RECT_CY(rcScreen)/2 - (rc.top + cy/2)},
		};
		
		for(int i = 1; i < countof(relOffsets); i++)
		{
			if( (_abs(relOffsets[i].x) + _abs(relOffsets[i].y)) <
				(_abs(relOffsets[nRelative].x) + _abs(relOffsets[nRelative].y)) )
				nRelative = i;
		}

		x = relOffsets[nRelative].x;
		y = relOffsets[nRelative].y;
	}
	
	cStrObj data; data.format(cCP_ANSI, PLACEMENT_FMT, x, y, cx, cy, showCmd, nRelative, expanded, exp_cy);
	pKey.set_strobj(PLACEMENT_VAL, data);
}

void CDialogItem::eb::clear()
{
	if( m_ok )		m_ok->m_pItem->Release(),     delete m_ok,     m_ok = NULL;
	if( m_cancel )	m_cancel->m_pItem->Release(), delete m_cancel, m_cancel = NULL;
	if( m_next )	m_next->m_pItem->Release(),   delete m_next,   m_next = NULL;
	if( m_prev )	m_prev->m_pItem->Release(),   delete m_prev,   m_prev = NULL;
}

void CDialogItem::_GetValidatorIds(CItemBase * pItem, tDWORD nAction, LPCSTR& strMacro, CItemBinding **& ppBinding)
{
	strMacro = NULL;
	ppBinding = NULL;
	
	eb& _eb = pItem == this ? m_eb : m_ebs[pItem->m_nPos];
	
	switch( nAction )
	{
	case DLG_ACTION_OK:
		strMacro = "Ok";
		ppBinding = &_eb.m_ok;
		break;
	case DLG_ACTION_CANCEL:
		strMacro = "Cancel";
		ppBinding = &_eb.m_cancel;
		break;
	case DLG_ACTION_NEXT:
		strMacro = "Next";
		ppBinding = &_eb.m_next;
		break;
	case DLG_ACTION_BACK:
		strMacro = "Back";
		ppBinding = &_eb.m_prev;
		break;
	}
}

void CDialogItem::AddValidators(CItemBase * pItem)
{
	if( !pItem->m_pProps )
		return;
	
	CItemProps btnsProps(pItem->m_pProps->Get("btns").GetStr());

	if( m_nFlags & STYLE_SHEET_ITEM )
	{
		AddValidator(btnsProps, pItem, DLG_ACTION_NEXT);
		AddValidator(btnsProps, pItem, DLG_ACTION_BACK);
	}

	AddValidator(btnsProps, pItem, DLG_ACTION_OK);
	AddValidator(btnsProps, pItem, DLG_ACTION_CANCEL);
}

void CDialogItem::AddValidator(CItemProps& btnsProps, CItemBase * pItem, tDWORD nAction)
{
	LPCSTR strMacro; CItemBinding ** ppBinding; _GetValidatorIds(pItem, nAction, strMacro, ppBinding);
	if( !strMacro || !ppBinding || !pItem->m_pProps )
		return;
	
	const tString& strValidator = btnsProps.Get(strMacro).Get();
	if( strValidator.empty() )
		return;
	
	CItemBase * pValidItem = new CItemBase();
	pItem->AddItem(pValidItem);
	pValidItem->AddRef();
	
	CItemBinding * pBinding = new CItemBinding(pValidItem);
	pBinding->Init(TOR(CItemProps, (strValidator.c_str())));
	*ppBinding = pBinding;
}

tDWORD CDialogItem::ProcessValidator(CItemBase * pItem, tDWORD nAction)
{
	if( !pItem )
		return 0;
	
	if( pItem != this && !m_ebs.size() )
		return 0;
	
	LPCSTR strMacro; CItemBinding ** ppBinding; _GetValidatorIds(pItem, nAction, strMacro, ppBinding);
	if( !strMacro || !ppBinding )
		return 0;

	CItemBinding * pValidator = *ppBinding;
	if( !pValidator )
		return 0;
	
	pValidator->UpdateData(true, CStructData::Default());
	return (pValidator->m_pItem->m_nState & ISTATE_DISABLED) ? 1 : 0;
}

void CDialogItem::_SelectItem(CItemBase * pItem, bool bSelect)
{
	if( !bSelect )
	{
		pItem->m_nState &= ~ISTATE_SELECTED;
		pItem->Show(false, SHOWITEM_DIRECT);
		return;
	}

	pItem->m_nState |= ISTATE_SELECTED;
	pItem->Show(true, SHOWITEM_DIRECT);
}

tSIZE_T CDialogItem::_GetNextIdx(tSIZE_T nFrom, bool bBackward)
{
	CItemBase * pPage = NULL;
	if( nFrom != -1 )
	{
		if( nFrom >= m_pSheet->m_aItems.size() )
			return -1;
		pPage = m_pSheet->m_aItems[nFrom];
		pPage = bBackward ? pPage->GetPrev(false) : pPage->GetNext(false);
	}
	else
		pPage = bBackward ? m_pSheet->GetChildLast(false) : m_pSheet->GetChildFirst(false);

	for( ; pPage; pPage = bBackward ? pPage->GetPrev(false) : pPage->GetNext(false) )
		if( !(pPage->m_nState & ISTATE_SHEETPAGE_HIDE) && pPage != m_pFooter )
			return pPage->m_nPos;

	return -1;
}

/////////////////////////////////////////////////////////////////////////////

class cNodeWizardMask : public cNode
{
public:
	cNodeWizardMask(CItemBase * owner) : m_owner(owner) {}
	bool IsConst() const { return false; }
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		CDialogItem * pDialog = (CDialogItem *)m_owner;
		
		if( get )
		{
			tQWORD mask = 0;
			for(CItemBase * pPage = pDialog->m_pSheet->GetChildFirst(false); pPage; pPage = pPage->GetNext(false))
				if( !(pPage->m_nState & ISTATE_SHEETPAGE_HIDE) && pPage != pDialog->m_pFooter )
					mask |= ((tQWORD)1 << pPage->m_nPos);

			value = mask;
		}
		else
		{
			tQWORD mask = value.ToQWORD();
			for(CItemBase * pPage = pDialog->m_pSheet->GetChildFirst(false); pPage; pPage = pPage->GetNext(false))
				if( pPage != pDialog->m_pFooter )
					pPage->Show(!!(mask & ((tQWORD)1 << pPage->m_nPos)));
		}
	}

	CItemBase * m_owner;
};

class cNodeWizardPagesMask : public cNodeArgs<1>
{
public:
	cNodeWizardPagesMask(cNode **args, CDialogItem * wizard) : cNodeArgs<1>(args), m_mask(0)
	{
		if( m_args[0] && m_args[0]->Type() == ntString && wizard->m_pSheet )
		{
			CItemPropVals _vals; _vals.Parse(((cNodeStr *)m_args[0])->m_str.c_str(), 0, true);
			for(tDWORD i = 0, n = _vals.Count(); i < n; i++)
				if( CItemBase * pPage = wizard->m_pSheet->GetItem(_vals.GetStr(i), NULL, false) )
					m_mask |= (tQWORD)1 << pPage->m_nPos;
		}
	}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) value = m_mask; }
	tQWORD        m_mask;
};

class cNodeWizardGotoPage : public cNodeArgs<1>
{
public:
	cNodeWizardGotoPage(cNode **args, CDialogItem * wizard) : cNodeArgs<1>(args), m_wizard(wizard) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		if( !get ) return;

		if( !m_args[0] )
		{
			m_wizard->SetNextPage();
			return;
		}
		
		m_args[0]->Exec(value, ctx);
		switch(value.Type())
		{
		case cVariant::vtInt:    m_wizard->SetNextPage(!value); break;
		case cVariant::vtString: m_wizard->SetCurPage(value.c_str()); break;
		}
	}
	
	CDialogItem * m_wizard;
};

void CDialogItem::cNodeIsTabbed::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	CDialogItem * p = OWNEROFMEMBER_PTR(CDialogItem, m_nodeIsTabbed);
	if( get )
		value = p->m_pSheet && p->m_pSheet->m_strItemType == GUI_ITEMT_TAB;
}

/////////////////////////////////////////////////////////////////////////////

cNode * CDialogItem::CreateOperand(const char *name, cNode *container, cAlloc *al)
{
	sswitch(name)
	scase("wizMask")	  return new cNodeWizardMask(this); sbreak;
	scase("isTabbed")	  return &m_nodeIsTabbed; sbreak;
	send;

	return TBase::CreateOperand(name, container, al);
}

cNode * CDialogItem::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	scase("wizPagesMask") return new cNodeWizardPagesMask(args, this); sbreak;
	scase("wizNext")	  return new cNodeWizardGotoPage(args, this); sbreak;
	scase("gotoPage")	  return new cNodeWizardGotoPage(args, this); sbreak;
	send;

	return TBase::CreateOperator(name, args, al);
}

/////////////////////////////////////////////////////////////////////////////
// CShellTreeSinkBase

CShellTreeSinkBase::CShellTreeSinkBase() :
	m_pInfo(NULL), m_nMask(0), m_pRoot(NULL)
{}

bool CShellTreeSinkBase::InitBrowseInfo(CBrowseObjectInfo& pInfo)
{
	m_pInfo = &pInfo;
	m_nMask = pInfo.m_nTypeMask;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CSplitterItem

CSplitterItem::CSplitterItem() : 
	m_bVertical(cFALSE), m_ratio(0.5F), m_cxySplitBar(0), m_cxyMinLength(0)
{
	m_rcSplitter.left = m_rcSplitter.top = m_rcSplitter.right = m_rcSplitter.bottom = 0;
	m_rcBorder.left = m_rcBorder.top = m_rcBorder.right = m_rcBorder.bottom = 0;
}

void CSplitterItem::OnInited()
{
	CItemBase::OnInited();
	SetSplitterPanes(true);
}

void CSplitterItem::Resize(tDWORD cx, tDWORD cy, bool bDirect)
{
	Items aItemsVoid;

	swap_objmem(aItemsVoid, m_aItems);
	CItemBase::Resize(cx, cy, bDirect);
	swap_objmem(m_aItems, aItemsVoid);

	SetSplitterPanes(true);
}

#define RATIO_KOEFF 100000.0F

bool CSplitterItem::SetValue(const cVariant &pValue)
{
	tDWORD dwRatio = pValue.ToDWORD();
	if( dwRatio > 0 )
		SetRatio((float)dwRatio/RATIO_KOEFF);
	return true;
}

bool CSplitterItem::GetValue(cVariant &pValue)
{
	pValue = (tDWORD)(RATIO_KOEFF*m_ratio);
	return true;
}

void CSplitterItem::SetRatio(float fRatio)
{
	m_ratio = fRatio;
	SetSplitterPanes(true);
}

void CSplitterItem::SetSplitterPanes(bool bUpdate)
{
	// вычисл€ем размер пр€моугольников границы и SplitterBar
	GetClientRectEx(m_rcBorder, true);
	if(m_bVertical)
	{
		float fLeft = (float)(m_rcBorder.left + m_rcBorder.right - m_cxySplitBar) * m_ratio;
		RECT rc = { (int)fLeft, m_rcBorder.top, (int)fLeft + m_cxySplitBar, m_rcBorder.bottom };
		m_rcSplitter = rc;
	}
	else
	{
		float fTop = (float)(m_rcBorder.top + m_rcBorder.bottom - m_cxySplitBar) * m_ratio;
		RECT rc = { m_rcBorder.left, (int)fTop, m_rcBorder.right, (int)fTop + m_cxySplitBar };
		m_rcSplitter = rc;
	}

	if(bUpdate)
		UpdateSplitterLayout();
}

void CSplitterItem::UpdateSplitterLayout()
{
	if(m_aItems.size() != 2)
		return;

	RECT rect = { 0, 0, 0, 0 };

	LockUpdate(true);
	GetSplitterPaneRect(cTRUE, &rect);
	if( m_aItems[0] )
		m_aItems[0]->SetRect(rect);

	GetSplitterPaneRect(cFALSE, &rect);
	if( m_aItems[1] )
		m_aItems[1]->SetRect(rect);
	LockUpdate(false);

	CItemBase::Update(&m_rcBorder);
}

void CSplitterItem::GetSplitterPaneRect(tBOOL LeftTopPane, LPRECT lpRect)
{
	*lpRect = m_rcBorder;
	
	if(LeftTopPane)	{
		if(m_bVertical)	{ lpRect->right	 = m_rcSplitter.left;	}
		else			{ lpRect->bottom = m_rcSplitter.top;	}
	}
	else {
		if(m_bVertical)	{ lpRect->left = m_rcSplitter.right; }
		else			{ lpRect->top  = m_rcSplitter.bottom; }
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChartCtrlBase

FUNC_NODE(cNodeChartZoom, 2)
{ 
	CChartCtrlBase * pCtrl = m_args[0] ? (CChartCtrlBase *)FUNC_NODE_PRM(0).c_ctl() : NULL;
	if( !pCtrl )
		return;

	tLONG nVal = m_args[1] ? (tLONG)FUNC_NODE_PRM(1).ToDWORD() : 0;
	value = (tDWORD)pCtrl->DoCommand(nVal < 0 ? "-" : "+", "0");
}

FUNC_NODE(cNodeChartIsCanZoom, 2)
{ 
	CChartCtrlBase * pCtrl = m_args[0] ? (CChartCtrlBase *)FUNC_NODE_PRM(0).c_ctl() : NULL;
	if( !pCtrl )
		return;

	tLONG nVal = m_args[1] ? (tLONG)FUNC_NODE_PRM(1).ToDWORD() : 0;
	value = (tDWORD)pCtrl->DoCommand(nVal < 0 ? "IsDecrease" : "IsIncrease", "0");
}

FUNC_NODE(cNodeChartProcess, 2)
{ 
	CChartCtrlBase * pCtrl = m_args[0] ? (CChartCtrlBase *)FUNC_NODE_PRM(0).c_ctl() : NULL;
	if( !pCtrl )
		return;

	tDWORD nVal = m_args[1] ? FUNC_NODE_PRM(1).ToDWORD() : 1;
	value = (tDWORD)pCtrl->DoCommand(nVal ? "on" : "off", "0");
}

cNode * CChartCtrlBase::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	scase("zoom")      return new cNodeChartZoom(args); sbreak;
	scase("isCanZoom") return new cNodeChartIsCanZoom(args); sbreak;
	scase("process")   return new cNodeChartProcess(args); sbreak;
	send;

	return CItemBase::CreateOperator(name, args, al);
}

/////////////////////////////////////////////////////////////////////////////
// CHexViewItem

char hextable[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
#define TOHEX(a, b)	{*b++ = hextable[a >> 4];*b++ = hextable[a&0xf];}

CHexViewItem::CHexViewItem()
{
	m_topindex		= 0;
	m_LastViewIndex = 0;
	m_BytesPerRow	= 16;
	m_LinesPerPage	= 1;

	m_offAddress = m_offHex	= m_offAscii = 0;
	m_signWidth  = 10;
	m_lineHeight = 13;
}

void CHexViewItem::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	CItemBase::InitProps(strProps, pCtx);

	RECT rcBound = {0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE};
	m_pFont->Draw(m_pRoot->GetDesktopDC(), "W", rcBound, TEXT_CALCSIZE);

	m_signWidth  = RECT_CX(rcBound);
	m_lineHeight = RECT_CY(rcBound);
	
	PR_ASSERT(m_signWidth);
	PR_ASSERT(m_lineHeight);
	if( 0 == m_signWidth )
		m_signWidth = 10;
	if( 0 == m_lineHeight )
		m_lineHeight = 10;
}

void CHexViewItem::DrawContent(HDC dc, RECT& rcClient)
{
	CItemBase::DrawContent(dc, rcClient);
	
	int len = (int)m_Data.size();
	if(len == 0 )
		return;

	char buf[10];
	
	COLORREF SelColor = m_pRoot->GetSysColor("SYS_WINDOW");
	COLORREF& color = m_pFont->GetColor();
	COLORREF oldColor = color;

	CHexViewSink* pHVSink = m_pSink ? (CHexViewSink*)m_pSink : NULL;

	int top = rcClient.top;
	for(int	index = m_topindex; (index < len) && (top < RECT_CY(rcClient)); index += m_BytesPerRow)
	{
		color = oldColor;
		// show Adress
		sprintf(buf, "%04lX", index);
		DrawStr(dc, buf, m_offAddress, top);

		int curHexOff = m_offHex;
		for(int n = 0; n < m_BytesPerRow; n++)
		{
			int pos = index + n;

			bool bSelected = pHVSink ? pHVSink->IsInRange(pos) : false;

			// show Hex
			char* p = &buf[0];
			if( pos < len ){ TOHEX(m_Data[pos], p); }
			else		   { sprintf(buf, "  "); }
			buf[2] = 0;

			if(bSelected)
			{
				color = (m_nState & ISTATE_FOCUSED ) ? SelColor : oldColor;
				RECT rcSel = {curHexOff, top, curHexOff + 3 * m_signWidth, top + m_lineHeight};
				m_pRoot->DrawRect(dc, rcSel, drSelected, m_nState);
			}
			else
				color = oldColor;

			DrawStr(dc, buf, curHexOff, top);
			curHexOff += 3 * m_signWidth;

			// show ASCII
			RECT rcAscii = {m_offAscii + n * m_signWidth, top, m_offAscii + (n + 1)*m_signWidth, top + m_lineHeight};
			if( rcAscii.left < RECT_CX(rcClient) )
			{
				if(bSelected)
					m_pRoot->DrawRect(dc, rcAscii, drSelected, m_nState);

				buf[0] = (pos < len) ? m_Data[pos] : ' ';
				if( buf[0] < 0x20 || buf[0] > 0x7f ) buf[0] = '.';
				buf[1] = 0;
				m_pFont->Draw(dc, buf, rcAscii, ALIGN_HCENTER|ALIGN_TOP|TEXT_SINGLELINE/*|TEXT_NOPREFIX*/);
			}
		}

		top += m_lineHeight;
	}
	
	color = oldColor;
}

bool CHexViewItem::SetValue(const cVariant &pValue)
{
	if( const cVectorData * vd = pValue.c_vector() )
	{
		if( vd->m_type == tid_BYTE ) 
		{
			cVector<tBYTE> &vect = *(cVector<tBYTE> *)vd->m_v;
			m_Data = vect;

			m_topindex = 0;
			RecalcLayouts();

			if( m_pSink )
				((CHexViewSink*)m_pSink)->SetCurSel(0xffffffff, 0);

			return true;
		}
	}
	return false;
}

void CHexViewItem::RecalcLayouts()
{
	// отступ между колонками
	int offDummy = m_signWidth + m_signWidth/2;
	int len = (int)m_Data.size();

	RECT rc;
	GetClientRectEx(rc, true);

	m_offAddress = m_signWidth/2;
	
	// адреса вывод€тс€ как 4 байта + 1 m_signWidth
	m_offHex = m_offAddress + m_signWidth * 4 + offDummy;
	
	// вычисление числа байт на строку 
	if( m_signWidth > 0 )
	{
		m_BytesPerRow = (rc.right - rc.left - m_offHex - offDummy) / (4*m_signWidth);
		if( m_BytesPerRow < 1 )
			m_BytesPerRow = 1;
	}
	else
		m_BytesPerRow = 1;

	// отступ дл€ отрисовки ASCII 
	m_offAscii	= m_offHex + (m_BytesPerRow * 3 * m_signWidth) - m_signWidth + offDummy;

	// число строк на странице
	m_LinesPerPage = (rc.bottom - rc.top) / m_lineHeight;

	// вычисл€ем последний видимый индекс
	m_LastViewIndex = 0;
	while( m_LastViewIndex <= len)
	{
		if( len >= m_LastViewIndex && len <= m_LastViewIndex + ((m_LinesPerPage - 1) * m_BytesPerRow))
			break;
		m_LastViewIndex += m_BytesPerRow;
	}

	UpdateScrollBars();
}

tINT CHexViewItem::GetDataPos(POINT pt)
{
	int len = (int)m_Data.size();
	if(len == 0 )
		return -1;

	RECT rcClient;
	GetClientRectEx(rcClient, true);

	int top = rcClient.top;
	for(int	index = m_topindex; (index < len) && (top < RECT_CY(rcClient)); index += m_BytesPerRow)
	{
		int curHexOff = m_offHex;
		for(int n = 0; n < m_BytesPerRow; n++)
		{
			int pos = index + n;
			
			if( pos >= len )
				break;

			RECT rcHex = {curHexOff, top, curHexOff + 3 * m_signWidth, top + m_lineHeight};
			curHexOff += 3 * m_signWidth;

			if( osd_PtInRect(&rcHex, pt) )
				return pos;

			// show ASCII
			RECT rcAscii = {m_offAscii + n * m_signWidth, top, m_offAscii + (n + 1)*m_signWidth, top + m_lineHeight};
			if( osd_PtInRect(&rcAscii, pt) )
				return pos;
		}

		top += m_lineHeight;
	}

	return -1;
}

void CHexViewItem::SetTopIndex(tINT topindex)
{
	int oldTop = m_topindex;
	
	ResetTopIndexTo(topindex);

	if( oldTop != m_topindex )
	{
		UpdateScrollBarPos();
		Update();
	}
}

void CHexViewItem::DrawStr(HDC dc, char* sBuff, int left, int top)
{
	char buf[2]; buf[1] = 0;
	for(int	i = 0; i < (int)strlen(sBuff); i++)
	{
		buf[0] = sBuff[i];
		RECT rc = {left + i * m_signWidth, top, left + (i + 1)*m_signWidth, top + m_lineHeight};
		m_pFont->Draw(dc, buf, rc, ALIGN_HCENTER|ALIGN_TOP|TEXT_SINGLELINE/*|TEXT_NOPREFIX*/);
	}
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

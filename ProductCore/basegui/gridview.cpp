// gridview.cpp: implementation of the cDataView class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <algorithm>
//#include <ProductCore/gridview.h>

int DRAW_ICON_MARGIN = 18;
int LIST_ICON_SIZE   = 16;
int LIST_CELL_MARGIN = 3;
int LIST_COL_DEFSIZE = 20;
int LIST_MIN_COL_SIZE = 30;

#define LIST_INDENT_SIZE   DRAW_ICON_MARGIN
#define LIST_ROW_DEFSIZE   DRAW_ICON_MARGIN
#define LIST_CHECK_SIZE    LIST_ICON_SIZE

#define BLOCK_SIZE              0x200
#define ADDROWS_NO_IDX          ((tIdx*)(-1))
#define SORT_BY_INDEX           ((cNode*)(-1))

#define VERSION_TAG             "13"

#define GRID_EVENT_UPDATELAYOUT 0xabba0001
#define GRID_EVENT_ONFOCUS      0xabba0002
#define GRID_EVENT_ONSELECT     0xabba0003

#define GRID_COLUMN_TYPE        "column"

#define GRID_GROUP              "Grid.Groups"
#define ITEMID_HDRPOPUPMENU     "Grid.HdrPopup"
#define ITEMID_FLTPOPUPMENU     "Grid.FltPopup"
#define ITEMID_POPUPMENU        "Grid.Popup"
#define ITEMID_FIND_DIALOG      "Grid.Find"

#define ITEMID_SORTUP		    "SortUp"
#define ITEMID_SORTDOWN		    "SortDown"
#define ITEMID_GROUP            "Group"
#define ITEMID_FIND             "Find"
#define ITEMID_FILTER           "Filter"
#define ITEMID_SELECTALL        "SelectAll"
#define ITEMID_COPY             "Copy"

#define FILTER_NONE             "None"
#define FILTER_EQUALS           "Equals"
#define FILTER_DONOTEQUAL		"DoNotEqual"
#define FILTER_GREATERTHAN		"GreaterThan"
#define FILTER_GREATERTHANOREQUAL "GreaterThanOrEqual"
#define FILTER_LESSTHAN			"LessThan"
#define FILTER_LESSTHANOEEQUAL	"LessThanOeEqual"
#define FILTER_BLANKS			"Blanks"
#define FILTER_NONBLANKS		"NonBlanks"
#define FILTER_LIKE				"Like"
#define FILTER_NOTLIKE			"NotLike"

//////////////////////////////////////////////////////////////////////

struct cFilterCtx : public CDialogSink
{
	enum eColType
	{
		ctMask  = 1,
		ctEnum  = 2,
		ctDate  = 3,
	};

	struct cCondition
	{
		cCondition() : m_dDate(0), m_qEnum(0){}

		cStrObj  m_sCond;
		cStrObj  m_sMask;
		tQWORD   m_qEnum;
		tDWORD   m_dDate;
	};

	struct cEnumSink : public CItemSinkT<CComboItem>
	{
		cEnumSink(CListItem* pList, cColumn* col) : m_pList(pList), m_col(col){}
		void OnInit(){ m_pList->FillSwitchItem(Item(), *m_col, true); }
		CListItem * m_pList;
		cColumn   * m_col;
	};

	CListItem *   m_pList;
	cColumn   *   m_col;
	eColType      m_nColType;
	tDWORD        m_nOpType;
	cCondition    m_c[2];
	cNode *       m_node;

	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_nColType, tid_DWORD, "colType")
		BIND_VALUE(m_nOpType, tid_DWORD, "opType")

		BIND_VALUE(m_c[0].m_sCond, tid_STRING_OBJ, "cond1")
		BIND_VALUE(m_c[0].m_sMask, tid_STRING_OBJ, "mask1")
		BIND_VALUE(m_c[0].m_qEnum, tid_QWORD, "enum1")
		BIND_VALUE(m_c[0].m_dDate, tid_DWORD, "date1")

		BIND_VALUE(m_c[1].m_sCond, tid_STRING_OBJ, "cond2")
		BIND_VALUE(m_c[1].m_sMask, tid_STRING_OBJ, "mask2")
		BIND_VALUE(m_c[1].m_qEnum, tid_QWORD, "enum2")
		BIND_VALUE(m_c[1].m_dDate, tid_DWORD, "date2")
	BIND_MEMBER_MAP_END(CItemSink)

	SINK_MAP_BEGIN()
		SINK_DYNAMIC_EX("enum", GUI_ITEMT_COMBO, cEnumSink(m_pList, m_col))
	SINK_MAP_END(CDialogSink)

	cFilterCtx(CListItem* pList, cColumn* col) :
		m_pList(pList), m_col(col), m_nColType(ctMask), m_nOpType(1)
	{ m_c[0].m_sCond = FILTER_EQUALS; }

	bool InitNode()
	{
		m_node = m_col->m_pItem->m_filter ? m_col->m_pItem->m_filter :
			m_col->m_pItem->m_pBinding->GetHandler(CItemBinding::hText);

		while(m_node)
		{
			if( m_node->Type() == ntSwitch )
			{
				m_node = ((cNodeSwitch*)m_node)->m_arg1.m_arg;
				m_nColType = ctEnum;
				continue;
			}

			if( m_node->Type() == ntDateGroup )
			{
				m_node = ((cNodeDateGroup*)m_node)->m_args[0];
				m_nColType = ctDate;
				continue;
			}
			break;
		}
		return m_node && m_node->Type() == ntSerField;
	}

	tString GetValStr(cCondition& c)
	{
		char val[0x20] = "";
		switch(m_nColType)
		{
		case ctMask: sprintf_s(val, sizeof(val), "%S", c.m_sMask.data()); break;
		case ctEnum: sprintf_s(val, sizeof(val), "0x%.16I64x", c.m_qEnum); break;
		case ctDate: sprintf_s(val, sizeof(val), "0x%.8x", c.m_dDate); break;
		}
		return val;
	}

	void SaveData(cCondition& c, tString& sData)
	{
		sData += c.m_sCond.c_str(cCP_TCHAR);
		sData += ':';
		sData += GetValStr(c);
		sData += ':';
	}

	void SaveData(tString& sData)
	{
		SaveData(m_c[0], sData);
		SaveData(m_c[1], sData);
	}

	void LoadData(cCondition& c, LPCSTR& sData)
	{
		const char* s = strchr(sData, ':');
		if( s )
		{
			c.m_sCond.assign(sData, 0, s-sData);
			if( !(s = strchr(sData = s+1, ':')) )
				return;
		}
		switch(m_nColType)
		{
		case ctMask: c.m_sMask.assign(sData, 0, s-sData); break;
		case ctEnum: sscanf(sData, "0x%16I64x", &c.m_qEnum); break;
		case ctDate: sscanf(sData, "0x%8x", &c.m_dDate); break;
		}
		sData = s+1;
	}

	void LoadData(LPCSTR& sData)
	{
		LoadData(m_c[0], sData);
		LoadData(m_c[1], sData);
	}

	bool MakeConditionNode(int pos, tString& sCondition)
	{
		cCondition& c = m_c[pos];

		const char* fmt = NULL;
		sswitch(c.m_sCond.c_str(cCP_TCHAR))
		scase(FILTER_EQUALS)          fmt = "node==%s"; sbreak;
		scase(FILTER_DONOTEQUAL)      fmt = "node!=%s"; sbreak;
		scase(FILTER_GREATERTHAN)     fmt = "node>%s"; sbreak;
		scase(FILTER_GREATERTHANOREQUAL) fmt = "node>=%s"; sbreak;
		scase(FILTER_LESSTHAN)        fmt = "node<%s"; sbreak;
		scase(FILTER_LESSTHANOEEQUAL) fmt = "node<=%s"; sbreak;
		scase(FILTER_BLANKS)          fmt = "!node"; sbreak;
		scase(FILTER_NONBLANKS)       fmt = "node"; sbreak;
		scase(FILTER_LIKE)            fmt = "like(node,%s)"; sbreak;
		scase(FILTER_NOTLIKE)         fmt = "!like(node,%s)"; sbreak;
		send;

		if( !fmt )
			return false;

		char buff[0x40];
		sprintf_s(buff, sizeof(buff), fmt, GetValStr(c).c_str());
		sCondition += buff;
		return true;
	}

	cNode* MakeNode()
	{
		tString condition;
		if( MakeConditionNode(1, condition) )
			condition += (m_nOpType == 1) ? "&&" : "||";

		MakeConditionNode(0, condition);

		CNodeFactory factory(m_col->m_pItem);
		return cNode::Parse(condition.c_str(), &factory, factory.m_pAlloc);
	}

	cNode* MakeEqNode(cNode* node, tQWORD val)
	{
		m_node = node;
		m_nColType = ctEnum;

		m_c[0].m_sCond = FILTER_EQUALS;
		m_c[0].m_qEnum = val;

		m_c[1].m_sCond = FILTER_NONE;
		return MakeNode();
	}
};

struct cFindCtx : public CDialogSink
{
	cFindCtx(CListItem* pList) : m_pList(pList)
	{
		m_nActionsMask = DLG_ACTION_CANCEL|DLG_MODE_DLGBUTTONS;
		m_bMatchCase = 0;
		m_bWholeWord = 0;
		m_nFlags = 0;
	}

	bool OnClicked(CItemBase* pItem)
	{
		int flags = (m_bMatchCase ? CListItem::eMatchCase : 0) | 
			(m_bWholeWord ? CListItem::eWholeWord : 0);
		if( pItem->m_strItemId == "Find" )
			m_pList->FindRows(flags);
		else if( pItem->m_strItemId == "Mark" )
			if( m_pList->FindRows(flags|CListItem::eMarkAll) )
				Close();
		return false;
	}

	struct cFillCols : public CItemSinkT<CComboItem>
	{
		cFillCols(CListItem* pList) : m_pList(pList){}
		void OnInit(){ m_pList->FillColumnsCombo(Item()); }

		CListItem * m_pList;
	};

	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_sText,   tid_STRING_OBJ, "Mask")
		BIND_VALUE(m_sColumn, tid_STRING_OBJ, "Column")
		BIND_VALUE(m_bMatchCase, tid_BOOL, "MatchCase")
		BIND_VALUE(m_bWholeWord, tid_BOOL, "WholeWord")
	BIND_MEMBER_MAP_END(CItemSink)

	SINK_MAP_BEGIN()
		SINK_DYNAMIC_EX("columns", GUI_ITEMT_COMBO, cFillCols(m_pList))
	SINK_MAP_END(CDialogSink)

	CListItem * m_pList;
	cStrObj     m_sText;
	cStrObj     m_sColumn;
	tBOOL       m_bMatchCase;
	tBOOL       m_bWholeWord;
	tDWORD      m_nFlags;
};

//////////////////////////////////////////////////////////////////////

CGridItem::CGridItem() :
	m_type(tUndef), m_nodeflags(0), m_colflags(0),
	m_condition(0), m_template(0), m_node(0), m_sort(0), m_expand(0), m_filter(0)
{
	m_nWrappedPos = NULL;
}

CGridItem::~CGridItem()
{
	if( m_condition )
		m_condition->Destroy();
	if( m_sort && m_sort != SORT_BY_INDEX )
		m_sort->Destroy();
	if( m_expand )
		m_expand->Destroy();
	if( m_filter )
		m_filter->Destroy();
}

cNode * CGridItem::CreateOperand(const char *name, cNode *container, cAlloc *al)
{
	if( !strcmp(name, "val") )
		return new CGroupNode(this, CGridItem::CGroupNode::eValue);

	if( !strcmp(name, "count") )
		return new CGroupNode(this, CGridItem::CGroupNode::eCount);

	if( !strcmp(name, "node") )
	{
		cColumn* col = (cColumn*)m_pUserData;
		cFilterCtx* ctx = (cFilterCtx*)col->m_pFltCtx;
		return new cNodeAgregate(ctx->m_node);
	}

	return CItemBase::CreateOperand(name, container, al);
}

CItemBase* CGridItem::CreateChild(LPCSTR strItemId, LPCSTR strItemType)
{
	bool bFooter = !strcmp(strItemId,"footer");
	bool bRowItem = !strcmp(strItemId,"rowedit");

	if( !strcmp(strItemType,"menu") || bFooter || bRowItem )
		return CItemBase::CreateChild(strItemId, strItemType);

	return new CGridItem();
}

void CGridItem::InitProps(CItemProps& strProps, CSinkCreateCtx *pCtx)
{
	CItemBase::InitProps(strProps, pCtx);

	CNodeFactory factory(this);

	tString groupid;
	CItemPropVals& prp = strProps.Get("group");
	if( prp.IsValid() )
	{
		groupid = prp.Get(0);
		GetGridItem(m_template, groupid.c_str());

		tString condition = prp.Get(1);
		if( condition.empty() && groupid != "default" )
			condition = groupid;

		if( !condition.empty() )
		{
			m_condition = cNode::Parse(condition.c_str(), &factory, factory.m_pAlloc);

			tString attr = prp.Get(2);
			if( attr == "filter" )
				m_type |= tFilter;
			else if( attr == "default" )
				m_type |= fDefGroup;
		}
	}

	CItemPropVals& fprp = strProps.Get("filter");
	if( fprp.IsValid() )
	{
		tString type = fprp.Get(0);

		tString condition = fprp.Get(1);
		if( !condition.empty() )
			m_filter = cNode::Parse(condition.c_str(), &factory, factory.m_pAlloc);

		if( type == "auto" )
			m_colflags |= cColumn::fAutoFlt;
		else if( type == "custom" )
			m_colflags |= cColumn::fCustomFlt;
	}

	CItemPropVals& sprp = strProps.Get("sort");
	if( sprp.IsValid() )
	{
		tString condition = sprp.Get(0);

		if( condition == "index" )
			m_sort = SORT_BY_INDEX;
		else if( !condition.empty() )
			m_sort = cNode::Parse(condition.c_str(), &factory, factory.m_pAlloc);

		tString defsort = sprp.Get(1);
		if( defsort == "sortup" )
			m_colflags |= cColumn::fSortUp;
		else if( defsort == "sortdown" )
			m_colflags |= cColumn::fSortDown;
	}

	CItemPropVals& eprp = strProps.Get("expand");

	if( eprp.IsValid() )
	{
		tString condition = eprp.Get(0);
		m_expand = cNode::Parse(condition.c_str(), &factory, factory.m_pAlloc);
	}
}

void CGridItem::ApplyStyle(LPCSTR strStyle)
{
	CItemBase::ApplyStyle(strStyle);

	sswitch(strStyle)
	scase("noplus")        m_nodeflags |= cRowNode::fNoPlus; sbreak;
	scase("checked")       m_nodeflags |= cRowNode::fChecked; sbreak;
	scase("checkedgroup")  m_nodeflags |= (cRowNode::fChecked|cRowNode::fCheckGroup); sbreak;
	scase("fixedpos")      m_colflags |= cColumn::fFixedPos; sbreak;
	scase("fixedsize")     m_colflags |= cColumn::fFixedSize; sbreak;
	scase("dragable")      m_colflags |= cColumn::fDragable; sbreak;
	send;
}

bool CGridItem::GetGridItem(CGridItem*& item, LPCSTR sTemplate)
{
	tString items, text;
	m_pRoot->GetString(items, PROFILE_LAYOUT, GRID_GROUP, sTemplate);
	if( items.empty() )
		m_pRoot->GetString(items, PROFILE_LAYOUT, GRID_GROUP, "default");

	m_pRoot->GetString(text, PROFILE_LOCALIZE, GRID_GROUP, sTemplate);		

	if( !text.empty() )
		items += " text(" + text + ")";

	CItemBase * pCtxItem = this;
	if( &item == &m_template )
		while( pCtxItem )
		{
			bool bBreak = pCtxItem->m_strItemType == GUI_ITEMT_LISTCTRL;
			pCtxItem = pCtxItem->m_pParent;
			if( bBreak ) break;
		}

	LockUpdate(true);
	LoadItem(NULL, (CItemBase*&)item,
		items.c_str(), sTemplate, LOAD_FLAG_INFO, &CSinkCreateCtx(pCtxItem));
	LockUpdate(false, false);

	if( !item )
		return false;

	item->m_type = tGroup;
	item->m_nFlags |= STYLE_OWN_PLACEMENT;
	return true;
}

tDWORD CGridItem::GetFontFlags()
{
	tDWORD flags = (m_nTextFlags & ALIGN_TEXT_MASK) | TEXT_SINGLELINE|ALIGN_VCENTER;
	if( m_nState & ISTATE_DISABLED )
		flags |= TEXT_GRAYED;
	if( m_nWrappedPos )
		flags |= TEXT_MODIFY;
	return flags;
}

bool CGridItem::GetUpdateRectEx(RECT& rcFrame, RECT* rcUpdate, CItemBase* pOwner)
{
	if( rcUpdate )
		rcFrame = *rcUpdate;

	rcFrame.left += m_ptOffset.x;
	rcFrame.top += m_ptOffset.y;
	return true;
}

void CGridItem::OnChangedData(CItemBase * pItem)
{
	CItemBase::OnChangedData(pItem);

	// колонка листа сигналит об изменении данных в элементе списка
	// наверное, в этом случае лист тоже должен сигналить об изменении списка вцелом.
	CItemBase *owner = pItem->GetOwner();
	if( owner != this  )
	{
		if( owner->m_strItemType == GUI_ITEMT_LISTCTRL ) // work around. почему-то CMenuItem не имеет флага STYLE_TOP_ITEM
		{
			PR_ASSERT_MSG(owner->m_strItemType == GUI_ITEMT_LISTCTRL, "it should be a list control");
			owner->SetChangedData();
		}
	}
}

void CGridItem::CGroupNode::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( m_type == eIsNode )
		value = m_list->m_focused.IsNode();
	else if( m_owner->m_node )
	{
		if( m_type == eValue )
			value = m_owner->m_node->m_val;
		else if( m_type == eCount )
			value = m_owner->m_node->Count();
	}
	else if( m_type == eValue && m_list )
		value = m_list->m_focused.m_node->m_val;
}

//////////////////////////////////////////////////////////////////////

CListItem::CListItem() :
	cThreadPoolBase("GridThread"),
	m_flags(0), m_focused(0), m_nUnique(0), m_pDesc(NULL), m_pSelBg(NULL)
{
	m_pList = this;
	m_bExpanded = true;
	m_szPage.cx = m_szPage.cy = 0;

	m_groupitem = m_rowprops = NULL;
	m_footer = m_rowitem = NULL;
	m_hitcol = m_sortcol = m_hotcol = m_dragcol = NULL;
	m_dragimg = NULL;
	m_curview = m_drpos = -1;
	m_nCurState = m_nLoopState = m_nTimer = 0;
	m_sorting = NULL;
	m_findctx = NULL;
	m_bWasLoad = m_bWasScroll = false;

	memset(m_dctx, 0, sizeof(m_dctx));

	ClearData();
}

CListItem::~CListItem()
{
	if( m_findctx )
		delete m_findctx;

	ClearFilters(false);

	if( m_pBorderBg )
		m_pBorderBg->Destroy();
	if( m_pSelBg )
		m_pSelBg->Destroy();
}

void CListItem::Cleanup()
{
	cThreadPoolBase::de_init();
	CItemBase::Cleanup();
}

void CListItem::ClearData(bool bNodes)
{
	cRowNode::ClearData(bNodes);

	m_ptView.x = m_ptView.y = 0;
	m_focused.m_node = this;
	m_focused.m_pos = 0;
	m_toprow.m_node = this;
	m_toprow.m_pos = 0;
}

void CListItem::ClearFilters(bool bColumns)
{
	for(size_t i = 0; i < m_filters.size(); i++)
	{
		if( !bColumns || m_filters[i].m_id > (tPTR)20 )
		{
			m_filters[i].m_filter->Destroy();
			m_filters.erase(m_filters.begin()+i--);
		}
	}

	if( bColumns )
		UpdateViewState();
}

bool CListItem::Bind(tDWORD nSerId, bool bUseSink)
{
	if( bUseSink )
		m_flags |= fUseSink;

	m_nUnique = nSerId;
	m_pDesc = cSerializableObj::getDescriptor(m_nUnique);
	if( m_pDesc )
	{
		InitBinding(NULL, m_pDesc);
		m_flags |= fBinded;
		return true;
	}
	return false;
}

void CListItem::SendListEvent(tDWORD eventId, tDWORD event)
{
	if( !m_pSink )
		return;

	if( m_nTimer )
		m_pRoot->SendEvent(eventId, NULL, 0, this);
	else
		((CListItemSink *)m_pSink)->OnListEvent((CListItemSink::eListEvents)event);
}

void CListItem::FocusRow(cRowId& row, bool bClearSel, bool bUpdate)
{
	if( !row.m_node )
		return;

	if( bClearSel )
		ClearSelection();

	m_focused = row;
	m_focused.Select(true);

	if( bUpdate )
		EnsureVisibleEx(m_focused);

	if( m_pSink )
		SendListEvent(GRID_EVENT_ONFOCUS, CListItemSink::leFocusChanged);
}

void CListItem::FocusFirstRow(cRowNode* node)
{
	if( !m_focused.IsNode() )
		return;

	bool bOk = (m_focused.m_node == this);
	if( node && node->m_parent )
	{
		if( node->m_parent->m_nodes.size() > 1 )
		{
			cRowNode* next = node->m_parent->m_nodes[1];
			bOk = (next == m_focused.m_node);
		}
	}

	if( bOk )
		FocusRow(TOR(cRowId,(node,0)), true, true);
}

void CListItem::OnInit()
{
	m_pIcoCols = m_pRoot->GetIcon("GridCol");

	m_pSelColor = m_pRoot->GetSysColor("SYS_WINDOW");
	m_pBorderBg = m_pRoot->CreateBackground(m_pRoot->GetSysColor("SYS_3DFACE"));

//	COLORREF cSelColor = m_pRoot->GetSysColor("SYS_HIGHLIGHT");
//	m_pSelBg = m_pRoot->CreateBackground(cSelColor, -1, 0, CImageBase::Right);

	if( m_pIcoCols )
		m_szPlus = m_pIcoCols->Size();

	InitItems(this);
}

void CListItem::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	CItemBase::OnUpdateProps(flags, nStateMask, rc);

	if( m_nTimer && (nStateMask & ISTATE_HIDE) && !(m_nState & ISTATE_HIDE) )
	{
		cThreadTaskBase::start(*this);
		m_pRoot->EnableToolTip(this, true);
	}
}

bool CListItem::GetTipInfo(POINT& pt, tString* pText, RECT* pRect)
{
	if( !m_hitcol )
		return false;

	PRAutoLocker lock(m_Locker);

	long*& pnWrappedPos = m_hitcol->m_pItem->m_nWrappedPos;
	long nWrappedPos = 0;
	pnWrappedPos = &nWrappedPos;

	RECT rcFrame = {0,0,0,0};
	HDC dc = m_pRoot->GetTempDC(rcFrame, NULL);

	if( m_hotflags & cColumn::hfInner )
	{
		DrawCol(dc, rcFrame, *m_hitcol); 
	}
	else if( m_hotrow.m_node )
	{
		rcFrame.bottom = m_hotrow.Size();
		rcFrame.right = m_szPage.cx;
		DrawRow(dc, rcFrame, m_hotrow); 
	}

	m_pRoot->ClipRect(dc, NULL);

	pnWrappedPos = NULL;
	if( !nWrappedPos )
		return false;

	tString * ppText = NULL;
	if( m_hotflags & cColumn::hfInner )
	{
		ppText = &m_hitcol->m_sText;

		if( pRect )
		{
			pRect->top = m_hitcol->m_Offset.y;
			pRect->left = nWrappedPos - m_ptView.x;
		}
	}
	else
	{																						
		ppText = &m_hitcol->m_pItem->m_strText;

		if( pRect )
		{
			pRect->top = m_hotrow.Offset() - m_ptView.y + m_cols.m_Size.cy + 2;
			pRect->left = nWrappedPos - m_ptView.x;
		}
	}

	if( pText )
		*pText = *ppText;

	if( pRect )
	{
		pRect->bottom = MAX_CTL_SIZE;
		pRect->right = MAX_CTL_SIZE;

		m_pFont->Draw(dc, ppText->c_str(), *pRect, TEXT_CALCSIZE);
		pRect->bottom += 2;
	}

	return true;
}

void CListItem::InitItems(CGridItem* pTop)
{
	if( pTop == this )
	{
		m_type |= CGridItem::tColumn;
		m_pUserData = &m_cols;

		m_cols.m_Offset.y = -LIST_COL_DEFSIZE;
		m_cols.m_Offset.x = 0;
		m_cols.m_nLevel = 0;
		m_cols.m_pList = this;
		m_cols.m_pItem = this;
		m_cols.m_nFlags |= cColumn::fExpanded;
	}										

	cColumn* parent = NULL;
	cRowNode* node = NULL;;

	if( pTop->m_type & CGridItem::tColumn )
	{
		parent = (cColumn*)pTop->m_pUserData;
	}
	else if( pTop->m_type & CGridItem::tRow )
		node = (cRowNode*)pTop->m_pUserData;

	size_t i, n = pTop->m_aItems.size();
	for(i = 0; i < n; i++)
	{
		CGridItem* pItem = (CGridItem*)pTop->m_aItems[i];
		pItem->m_nFlags |= STYLE_OWN_PLACEMENT;

		if( pItem->m_strItemId == "views" )
		{
			continue;
		}
		else if( pItem->m_strItemId == "footer" )
		{
			m_footer = pItem;
			continue;
		}
		else if( pItem->m_strItemId == "rowedit" )
		{
			m_rowitem = pItem;
			continue;
		}
		else
		{
			if( (pItem->m_type & CGridItem::tGroup) )
				continue;

			if( pItem->m_strItemId == "row" )
			{
				m_rowprops = pItem;

				pItem->m_pUserData = (cRowNode*)this;
				pItem->m_type |= CGridItem::tRow;
			}
			else if( node )
			{
				cRowNode* _node = node->AddNode(0, pItem->m_nodeflags, pItem);

				pItem->m_pUserData = _node;
				pItem->m_type |= CGridItem::tRow;
			}
			else if( parent )
			{
				tString sText;
				m_pRoot->GetString(sText, PROFILE_LOCALIZE,
					pTop->m_strSection.c_str(), pItem->m_strItemId.c_str());

				size_t size = sText.size();
				if( size )
					if( sText[0] == '\"' ) sText.erase(0,1), size--;

				if( size )
					if( sText[size-1] == '\"' ) sText.erase(size-1);

				cColumn* col = parent->AddColumn(pItem, sText.c_str());
				col->m_nFlags |= pItem->m_colflags & ~(cColumn::fSortUp|cColumn::fSortDown);

				if( pItem->m_colflags & cColumn::fSortUp )
					SortByColumn(*col, 1);
				else if( pItem->m_colflags & cColumn::fSortDown )
					SortByColumn(*col, 2);

				if( !m_condition && (pItem->m_type & fDefGroup) )
					MakeGroup(pItem, 0);
			}
		}
		InitItems(pItem);
	}
}

void CListItem::InitGroupItem()
{
	if( !m_groupitem )
		GetGridItem(m_groupitem, "default");
}

void CListItem::OnInited()
{
	if( !m_bWasLoad )
	{
		UpdateColumnsLayout();
		if( m_curview != -1 )
			SetView(m_views[m_curview].m_sViewData.c_str(cCP_ANSI));
	}
	UpdateView();
	UpdateLayout();
}

bool CListItem::OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem)
{
/*	cRowId nItem;
	CStructData sd(NULL);
	
	while( GetSelectedItem(nItem) != CListItem::invalidIndex )
	{
		if( cSer * pSer = GetRowData(nItem) )
		{
			sd.m_ser = pSer;
			pMenu->UpdateData(true, &sd);
		}
	}*/

	return false;
}

void CListItem::InitProps(CItemProps& strProps, CSinkCreateCtx *pCtx)
{
	CGridItem::InitProps(strProps, pCtx);

	m_nTimer = (tDWORD)strProps.Get("refresh").GetLong();

	CItemPropVals& fprp = strProps.Get("filter");
	if( fprp.IsValid() )
	{
		tString condition = fprp.Get(0);
		SetFilter(condition.c_str());
	}

	if( m_condition )
		MakeGroup(this, 0);

	if( m_nTimer )
		cThreadPoolBase::init(g_root);
}

void CListItem::TimerRefresh(tDWORD nTimerSpin)
{
	CGridItem::TimerRefresh(nTimerSpin);
//	if( m_nTimer && !(nTimerSpin % m_nTimer) )
//		UpdateView(0);
}

void CListItem::ApplyStyle(LPCSTR strStyle)
{
	CItemBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_SORTED)         m_flags |= fSorted; sbreak;
	scase(STR_PID_READONLY)       m_flags |= fReadOnly; sbreak;
	scase(STR_PID_AUTOSCROLL)     m_flags |= fAutoScroll; sbreak;
	scase(STR_PID_SINGLESEL)      m_flags |= fSingleSel; sbreak;
	scase(STR_PID_NOHEADER)       m_flags |= fNoHeader; sbreak;
	send;
}

cNode * CListItem::CreateOperand(const char *name, cNode *container, cAlloc *al)
{
	if( !strcmp(name, "isNode") )
		return new CGroupNode(this, CGridItem::CGroupNode::eIsNode, this);
	if( !strcmp(name, "val") )
		return new CGroupNode(this, CGridItem::CGroupNode::eValue, this);
	return CGridItem::CreateOperand(name, container, al);
}

void CListItem::CNode::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	switch(m_type)
	{
	case tSetFilter:
		{
			tDWORD nId = m_args[2] ? FUNC_NODE_PRM(2).ToDWORD() : 0;
			m_list->SetFilter(FUNC_NODE_PRM(1).c_str(), (tPTR)nId);
		}
		break;
	case tFindData:
		{
			tDWORD nFlags = m_args[2] ? FUNC_NODE_PRM(2).ToDWORD() : 0;
			m_list->FindRows(nFlags, NULL, FUNC_NODE_PRM(1).c_str());
		}
		break;
	case tSetView:
		m_list->SetView(FUNC_NODE_PRM(1).c_str());
		break;
	case tGetView:
		{
			m_list->InitViews();
			cVectorData data;
			data.m_v = (tMemChunk*)&m_list->m_views;
			data.m_type = (tTYPE_ID)cGridViewTemplate::eIID;
			value.Init(data);
		}
		break;
	case tCurView:
		if( get )
		{
			if( m_list->m_curview != -1 )
				value.Init(m_list->m_views[m_list->m_curview].m_sViewName.data());
		}
		else
		{
			m_list->SetCurView(value.ToString().c_str());
		}
		break;
	}
}

cNode * CListItem::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	scase("setfilter")  return new CNode(this, CNode::tSetFilter, args); sbreak;
	scase("finddata")   return new CNode(this, CNode::tFindData, args); sbreak;
	scase("setview")    return new CNode(this, CNode::tSetView, args); sbreak;
	scase("getview")    return new CNode(this, CNode::tGetView, args); sbreak;
	scase("curview")    return new CNode(this, CNode::tCurView, args); sbreak;
	send;
	return CItemBase::CreateOperator(name, args, al);
}

void CListItem::UpdateData(bool bIn, CStructData *pData)
{
	if( m_pBinding )
		m_pBinding->UpdateData(bIn, pData);
}

void CListItem::UpdateColumnsLayout(bool bInitSizeX)
{
	if( bInitSizeX )
		m_cols.InitSizeX(m_szPage.cx);

	if( m_flags & fNoHeader )
	{
		m_cols.m_Size.cy = 0;
		return;
	}

	m_cols.InitSizeY(m_cols.MaxLevel());
	m_cols.RecalcOffset();
	UpdateLayout(true);
}

void CListItem::UpdateView(int nFlags)
{
	if( !m_nTimer )
		UpdateViewEx(true);
}

bool CListItem::UpdateAsTree(cRowNode* node)
{
	tDWORD i, k, nChilds = 0;
	tIdxArr id, rows;

	node->GetIdxArr(0, id);
	if( !((CListItemSink*)m_pSink)->GetTreeInfo(id, &nChilds, rows) )
		return false;

	m_flags |= fIsTree;

	CStructData sd(NULL);
	cNodeExecCtx ctx(m_pRoot, this, &sd, NULL);

	for(i = 0, k = 0; i < nChilds; i++)
	{
		cRowNode* _node;
		if( !GetRowData(node, i, false, &ctx) )
			continue;

		if( k++ < node->m_nodes.size() )
		{
			_node = node->m_nodes[k - 1];
			_node->m_nIdx.m_idx = i;
		}
		else 
			_node = node->AddNode(i);

		UpdateAsTree(_node);
	}

	while( k < node->m_nodes.size() )
		node->m_nodes[k]->Destroy();

	for(i = 0; i < rows.size(); i++)
	{
		if( i < node->m_nCount )
			node->GetIdx(i+1).m_idx = rows[i];
		else
			node->AddRows(1, &rows[i]);
	}

	tLONG diff = rows.size() - m_nCount;
	if( diff < 0 )
		node->DelRows(-diff, NULL);

	if( node->m_parent == this )
		node->Expand(true);

	return true;
}

void CListItem::UpdateViewEx(bool bSync)
{
	if( !m_pSink || !(m_nState & ISTATE_INITED) )
		return;

	CListItemSink* pSink = (CListItemSink*)m_pSink;
	cDataInfo info = m_info;
	if( !pSink->GetInfo(m_info) )
	{
		UpdateAsTree(this);
		UpdateLayout(true);
		return;
	}

	if( info.m_nTimeStamp == m_info.m_nTimeStamp &&
		info.m_nCount == m_info.m_nCount )
	{
		if( m_info.m_nLastModified != info.m_nLastModified )
			UpdateLayout(true);
		return;
	}

	tDWORD nFilters = (tDWORD)m_filters.size();
	bool bSimple = !m_group && !m_sorting && !nFilters && !(m_flags & fUseSink);

	if( info.m_nTimeStamp != m_info.m_nTimeStamp ||
		m_info.m_nCount < info.m_nCount && m_info.m_nLastModified != info.m_nLastModified )
	{
		if( !bSimple )
		{
			tString sData;
			SaveColumnInfo(m_cols, sData);

			ClearData();
			info.m_nCount = 0;
		}
		else
			info.m_nCount = m_nCount;
	}

	tLONG diff = m_info.m_nCount-info.m_nCount;

	if( !m_info.m_nCount )
	{
		DelRows(m_nCount, NULL);
	}
	else if( bSimple )
	{
		if( diff >= 0 )
			AddRows(diff, NULL);
		else
			DelRows(-diff, NULL);
	}
	else
	{
		CStructData sd(NULL);
		cNodeExecCtx ctx(m_pRoot, this, &sd, NULL);

		bool bForward = !(GetSortState() & cColumn::fSortUp);
		tDWORD nCount = m_info.m_nCount - info.m_nCount;

		for(tIdx i = 0; i < (tIdx)nCount; i++)
		{
			cRowIdx idx;
			idx.m_idx = bForward ? i + info.m_nCount : m_info.m_nCount - i - 1;

			if( !(i % BLOCK_SIZE) )
			{
				tIdx from = bForward ? idx.m_idx : (idx.m_idx+1 > BLOCK_SIZE ? idx.m_idx+1 - BLOCK_SIZE : 0);
				pSink->PrepareRows(from, BLOCK_SIZE);
			}

			if( !GetRowData(this, idx.m_idx, false, &ctx) )
				continue;

			idx.m_sel = 0;
			AddRow(idx, ctx);

			if( !bSync && i && !(i % (5*BLOCK_SIZE)) )
				if( !UpdateLayout(false) )
					break;
		}
	}

	UpdateLayout(true);
}

void CListItem::UpdateViewState(bool bComplete, bool bBySort)
{
	PRAutoLocker lock(m_Locker);

	m_cols.UpdateState();

	m_nCurState++;
	m_nLoopState = 0;

	if( !bComplete && (!(m_flags & fUseSink) || bBySort) )
	{
		if( !m_nTimer )
			RefreshView(true);
		else
			Update();
	}
	else
	{
		m_info.m_nTimeStamp--;
		if( !m_nTimer )
			UpdateViewEx(false);
	}
}

void CListItem::SendEvent(tDWORD nEventId, cSerializable* pData, bool bBroadcast)
{
	CItemBase::SendEvent(nEventId, pData, bBroadcast);

	if( nEventId == GRID_EVENT_UPDATELAYOUT )
	{
		PRAutoLocker lock(m_Locker);
		UpdateLayout(true);
	}
	else if( nEventId == GRID_EVENT_ONFOCUS )
		((CListItemSink *)m_pSink)->OnListEvent(CListItemSink::leFocusChanged);
	else if( nEventId == GRID_EVENT_ONSELECT )
		((CListItemSink *)m_pSink)->OnListEvent(CListItemSink::leSelectionChanged);
}

void CListItem::RefreshLoop()
{
	while( is_inited() && !(m_nState & ISTATE_HIDE) )
	{
		{
			PRAutoLocker lock(m_Locker);

			if( m_nLoopState != m_nCurState )
			{
				m_nLoopState = m_nCurState;
				RefreshView(false);
			}

			UpdateViewEx(false);
		}

		PrSleep(m_nTimer);
	}
}

void CListItem::SortByColumn(cColumn& col, long how)
{
	if( !col.m_pItem || !col.m_pItem->m_pBinding )
		return;

	cNode* node = col.m_pItem->m_sort ? col.m_pItem->m_sort :
		col.m_pItem->m_pBinding->GetHandler(CItemBinding::hText);

	if( !node )
		return;

	tDWORD oldFlags = m_sortcol ? m_sortcol->m_nFlags : 0;

	if( m_sortcol )
		m_sortcol->m_nFlags &= ~(cColumn::fSortDown|cColumn::fSortUp);

	if( how == 1 || !how && (oldFlags & cColumn::fSortDown) )
		col.m_nFlags |= cColumn::fSortUp;
	else
		col.m_nFlags |= cColumn::fSortDown;

	m_sortcol = &col;
	m_sorting = node;

	UpdateViewState(false, true);
}

void CListItem::MakeGroup(CGridItem* group, long how)
{
	if( !group || !group->m_condition || (m_group == group && !how) )
		return;

	size_t i, n = m_groups.size();
	if( how ) // delete
	{
		for(i = 0; i < n; i++)
			if( m_groups[i] == group )
			{
				m_groups.erase(m_groups.begin()+i);
				break;
			}
	}
	else
	{
		for(i = 0; i < n; i++)
			if( m_groups[i] == group )
				return;

		m_groups.push_back(group);
	}

	UpdateViewState();
}

void CListItem::SetFilter(cNode* node, tPTR pId)
{
	PRAutoLocker lock(m_Locker);

	for(size_t i = 0; i < m_filters.size(); i++)
		if( m_filters[i].m_id == pId )
		{
			m_filters[i].m_filter->Destroy();
			m_filters.erase(m_filters.begin()+i);
			break;
		}

	if( node )
	{
		cFilter flt;
		flt.m_id = pId;
		flt.m_filter = node;
		m_filters.push_back(flt);
	}

	UpdateViewState(true);
}

void CListItem::SetFilter(LPCSTR sFilter, tPTR pId)
{
	cNode* filter = NULL;
	if( sFilter && *sFilter )
	{
		CNodeFactory factory(this);
		filter = cNode::Parse(sFilter, &factory, factory.m_pAlloc);
	}
	SetFilter(filter, pId);
}

cSer* CListItem::GetRowData(cRowNode* node, tIdx idx, bool bIsNode, cNodeExecCtx* pctx)
{
	if( !(m_flags & fBinded) )
		return NULL;

	cSer* data;
	if( bIsNode && node->m_sink )
	{
		data = node->m_sink->GetData();
	}
	else if( m_flags & fIsTree )
	{
		tIdxArr id;
		cRowNode* _node = bIsNode ? node->m_parent : node;
		if( _node )
			_node->GetIdxArr(0, id);
		id.push_back(idx);
		data = ((CListItemSink*)m_pSink)->GetTreeData(id, !pctx ? bIsNode : true);
	}
	else
		data = ((CListItemSink*)m_pSink)->GetRowData(idx);

	if( !pctx || !data )
		return data;

	cVariant val;
	pctx->m_data->m_ser = data;

	size_t i, n = m_filters.size();
	for(i = 0; i < n; i++)
	{
		m_filters[i].m_filter->Exec(val, *pctx);
		if( !val )
			return NULL;
	}
	return data;
}

bool CListItem::GetPlusRect(cRowId& row, RECT& rcPlus, bool& bDraw)
{
	if( !m_pIcoCols || !row.IsNode() )
		return false;

	if( row.m_node->m_flags & cRowNode::fNoPlus )
		return false;

	bDraw = true;
	if( !row.m_node->m_nCount && !row.m_node->m_nodes.size() )
	{
		bDraw = false;
		if( row.m_node->m_parent == this /*|| !(row.m_node->m_flags & cRowNode::fChecked)*/ )
			return false;
	}

	rcPlus.left = ((2*row.m_node->m_nIndent-1)*LIST_INDENT_SIZE - m_szPlus.cx)/2 - m_ptView.x;
	rcPlus.top = (2*row.Offset() + row.Size() - m_szPlus.cy)/2 - m_ptView.y + m_cols.m_Size.cy;
	rcPlus.right = rcPlus.left + m_szPlus.cx;
	rcPlus.bottom = rcPlus.top + m_szPlus.cy;
	return true;
}

bool CListItem::GetCheckRect(cRowId& row, cColumn& col, RECT& rcCheck)
{
	if( !((col.m_nFlags & cColumn::fChecked) && !row.IsCustom()) &&
			!(row.IsNode() && (row.m_node->m_flags & fChecked)) )
		return false;

	if( col.IsFirst() )
	{
		bool bDraw;
		if( GetPlusRect(row, rcCheck, bDraw) )
			rcCheck.left += (DRAW_ICON_MARGIN+m_szPlus.cx)/2+1;
		else
			rcCheck.left = ((2*row.Indent()+1)*LIST_INDENT_SIZE - LIST_CHECK_SIZE)/2 - m_ptView.x;
	}
	else
		rcCheck.left = (2*col.m_Offset.x + col.m_Size.cx - LIST_CHECK_SIZE)/2 - m_ptView.x;

	rcCheck.top = (2*row.Offset() + row.Size() - LIST_CHECK_SIZE)/2 - m_ptView.y + m_cols.m_Size.cy;
	rcCheck.right = rcCheck.left + LIST_CHECK_SIZE;
	rcCheck.bottom = rcCheck.top + LIST_CHECK_SIZE;
	return true;
}

bool CListItem::GetColFilterRect(cColumn& col, RECT& rcFilter)
{
	if( !m_pIcoCols )
		return false;

	if( !(col.m_nFlags & (cColumn::fAutoFlt|cColumn::fCustomFlt)) )
		return false;

	if( !(&col == m_hotcol) && !(col.m_nFlags & cColumn::fFiltered) )
		return false;

	rcFilter.left = col.m_Offset.x - m_ptView.x;
	rcFilter.top = col.m_Offset.y;
	rcFilter.right = rcFilter.left + m_szPlus.cx;
	rcFilter.bottom = rcFilter.top + m_szPlus.cx;
	return true;
}

bool CListItem::GetColPlusRect(cColumn& col, RECT& rcPlus)
{
	if( !m_pIcoCols || !col.m_vChilds.size() )
		return false;

	rcPlus.left = col.m_Offset.x + col.m_Size.cx - DRAW_ICON_MARGIN - m_ptView.x;
	rcPlus.top = (2*col.m_Offset.y + col.m_Size.cy - m_szPlus.cy)/2;
	rcPlus.right = rcPlus.left + m_szPlus.cx;
	rcPlus.bottom = rcPlus.top + m_szPlus.cy;
	return true;
}

void CListItem::DrawContent(HDC dc, RECT& rcClient)
{
	RECT rcDraw;
	rcDraw.top = 0;
	rcDraw.left = m_ptView.x;
	rcDraw.right = m_ptView.x + m_szPage.cx;

	if( !(m_flags & fNoHeader) )
		DrawCol(dc, rcDraw, m_cols);

	if( m_cols.m_Size.cx < m_szPage.cx )
	{
		RECT rcNCCol = {m_cols.m_Size.cx, 0, rcDraw.right, m_cols.m_Size.cy};
		m_pRoot->DrawRect(dc, rcNCCol, drColHeader, m_nState);
	}

	if( m_footer )
		m_footer->Draw(dc, NULL);

	PRAutoLocker lock(m_Locker);

	long maxsize = m_ptView.y + m_szPage.cy;
	bool next = true;
	for(cRowId row = m_toprow; next; next = NextRow(row))
	{
		rcDraw.top = row.Offset();
		if( rcDraw.top >= maxsize )
			break;

		rcDraw.top -= m_ptView.y - m_cols.m_Size.cy;
		rcDraw.bottom = rcDraw.top + row.Size();
		DrawRow(dc, rcDraw, row);
	}
}

void CListItem::DrawRow(HDC dc, RECT& rc, cRowId& row)
{
	if( row.m_node == this && !row.m_pos )
		return;

	RECT rcFrame = rc;
	rcFrame.left = 0;
	rcFrame.right = m_szPage.cx;

	bool bGroup = (!row.m_pos && row.m_node->m_pHeader);

	cRowIdx& idx = row.Idx();

	cCellData cell;
	cell.m_col = &m_cols;
	cell.m_Idx = idx.m_idx;
	cell.m_indent = 0;
	cell.m_data = NULL;
	cell.m_item = bGroup ? (CGridItem*)row.m_node->m_pHeader : this;
	cell.m_frame = &rcFrame;
	cell.m_footer = NULL;
	cell.m_selected = idx.m_sel;
	cell.m_template = bGroup;
	cell.m_group = !!(row.m_node->m_flags & cRowNode::fSkinGroup);
	cell.m_data = (!bGroup || cell.m_group) ? GetRowData(row.m_node, cell.m_Idx, row.IsNode()) : NULL;

	RECT rcFooter;
	if( m_footer && m_footer->m_ptOffset.y > rc.top )
	{
		cell.m_footer = &rcFooter;
		m_footer->GetParentRect(rcFooter);
		m_pRoot->ClipRect(dc, &rcFooter, true);
	}

	if( m_rowprops && !(cell.m_template && !cell.m_group) )
	{
		if( cell.m_data )
			m_rowprops->UpdateData(true, &CStructData(cell.m_data));

		if( m_rowprops->m_pBackground )
			m_rowprops->m_pBackground->Draw(dc, rcFrame, NULL, 0);
	}

	if( !bGroup && !(m_flags & fNoHeader) && m_pBorderBg )
	{
		RECT rcBg = {0, rc.bottom-1, m_szPage.cx, rc.bottom};
		m_pBorderBg->Draw(dc, rcBg);
	}

	if( cell.m_selected )
	{
//		if( m_rowitem )
//			m_pSelBg->Draw(dc, *cell.m_frame);
//		else
			m_pRoot->DrawRect(dc, *cell.m_frame, drSelected, m_nState);
	}

	if( bGroup && cell.m_item->m_template )
	{
		cCellData cgroup = cell;
		cgroup.m_item = cell.m_item->m_template;
		DrawCell(dc, rc, row, cgroup);
	}

	DrawCell(dc, rc, row, cell);

	if( (m_focused == row && (m_nState & ISTATE_FOCUSED)) )
	{
		m_pRoot->DrawRect(dc, rcFrame, drFocused, m_nState);
		
		if( m_rowitem )
		{
			if( cell.m_data )
				m_rowitem->UpdateData(true, &CStructData(cell.m_data));

			m_rowitem->Draw(dc, NULL);
		}
	}
}

void CListItem::DrawCell(HDC dc, RECT& rc, cRowId& row, cCellData& cell)
{
	cColumn& col = *cell.m_col;

	if( col.m_nFlags & cColumn::fHided )
		return;

	bool bFullRow = (cell.m_template && !cell.m_group);
	bool bFullCell = !col.IsGroup() || bFullRow;
	bool bDrawChilds = (cell.m_item == this || !bFullCell);

	if( !bDrawChilds || cell.m_template )
	{
		RECT rcCell = *cell.m_frame;
		rcCell.left = col.m_Offset.x - rc.left;
		rcCell.right = cell.m_template ? m_szPage.cx : rcCell.left + col.m_Size.cx;

		bool bDrawContent = true;
		bool bFirstColumn = !bDrawChilds & col.IsFirst();

		if( !bDrawChilds )
			m_pRoot->ClipRect(dc, &rcCell, false);

		cell.m_indent = 0;

		if( bFirstColumn )
		{
			cell.m_indent = row.Indent() * LIST_INDENT_SIZE;

			RECT rcPlus; bool bDraw;
			if( GetPlusRect(row, rcPlus, bDraw) )
			{
				if( bDraw )
					m_pIcoCols->Draw(dc, rcPlus, NULL, row.m_node->m_bExpanded ? 9 : 8);
				cell.m_indent += DRAW_ICON_MARGIN;
			}
		}

		if( m_pSink )
			((CListItemSink*)m_pSink)->GetCellData(cell);

		CItemBinding* binding = cell.m_item->m_pBinding;

		if( binding )
		{
			cell.m_item->m_node = row.m_node;
			binding->UpdateData(true, cell.m_data && !cell.m_template ?
				&CStructData(cell.m_data) : NULL);

			cell.m_item->m_node = NULL;
		}

		RECT rcCheck;
		if( GetCheckRect(row, col, rcCheck) )
		{
			tDWORD flags = drCheckBox|drFlat;
			if( cell.m_item->m_value )
				flags |= drPressed;

			if( cell.m_item->m_nState & ISTATE_DISABLED )
				flags |= drDisabled;

			m_pRoot->DrawRect(dc, rcCheck, flags, m_nState);
			cell.m_indent += LIST_CHECK_SIZE;

			if( !bFirstColumn )
				bDrawContent = false;
		}

		if( bFirstColumn && m_rowprops && !bFullRow )
		{
			if( m_rowprops->m_pIcon )
			{
				const SIZE& szIcon = m_rowprops->m_pIcon->Size();

				RECT rcIcon = rc;
				rcIcon.left = cell.m_indent + LIST_CELL_MARGIN - rc.left;
				rcIcon.right = rcIcon.left + szIcon.cx;

				m_rowprops->m_pIcon->Draw(dc, rcIcon, NULL, 0);
				cell.m_indent += DRAW_ICON_MARGIN;
			}
		}

		if( cell.m_template )
		{
			tLONG diff = (m_szPage.cx - cell.m_item->m_szSize.cx);
			if( diff )
				cell.m_item->Resize(diff, 0);
		}
		else
			rcCell.right -= LIST_CELL_MARGIN;

		CItemBase* contentItem = cell.m_item;
		if( cell.m_group )
		{
			CGridItem* group = row.m_node->GetGroupItem();
			if( group && group == cell.m_item )
				contentItem = group->m_template;
		}

		CFontStyle *&pFont = contentItem->m_pFont, *pOldFont = pFont;
		if( m_rowprops && !bFullRow )
			pFont = m_rowprops->m_pFont;

		COLORREF& color = pFont->GetColor();
		COLORREF oldColor = color;

		cell.m_item->DrawBackground(dc, rcCell);
		cell.m_item->DrawBorder(dc, rcCell);

		if( m_nState & ISTATE_FOCUSED )
		{
			if( cell.m_selected )
				color = m_pSelColor;

			if( m_rowitem && m_focused == row && 
					rcCell.right >= cell.m_frame->right - LIST_CELL_MARGIN )
				rcCell.right -= 35;
		}

		rcCell.left += cell.m_indent + LIST_CELL_MARGIN;

		tString sTemp;
		if( cell.m_item->m_nWrappedPos )
		{
			sTemp = cell.m_item->m_strText;
			cell.m_item->m_strText.resize(sTemp.size()+4);
		}

		if( bDrawContent )
		{
			cell.m_item->m_szSize.cy = RECT_CY(rcCell);

			if( !(cell.m_template && cell.m_group) )
				contentItem->DrawContent(dc, rcCell);
		}

		if( cell.m_template && !(cell.m_item->m_type & CGridItem::tRow) )
		{
			rcCell.left = 0;
			cell.m_item->DrawChildren(dc, &rcCell);
		}

		if( cell.m_item->m_nWrappedPos )
			if( strcmp(sTemp.c_str(), cell.m_item->m_strText.c_str()) )
			{
				cell.m_item->m_strText = sTemp;
				*cell.m_item->m_nWrappedPos = rcCell.left;
				if( contentItem->m_pIcon )
					*cell.m_item->m_nWrappedPos += DRAW_ICON_MARGIN + 1;
			}

		color = oldColor;
		pFont = pOldFont;

		m_pRoot->ClipRect(dc, NULL);
		if( cell.m_footer )
			m_pRoot->ClipRect(dc, cell.m_footer, true);
	}
	else if( cell.m_item && cell.m_item->m_pBinding )
	{
		cNode* props = cell.m_item->m_pBinding->GetHandler(CItemBinding::hProps);
		if( props )
		{
			CStructData sd(cell.m_data);
			cNodeExecCtx ctx(m_pRoot, this, &sd, NULL);

			cVariant val;
			props->Exec(val, ctx);
		}
	}

	if( bDrawChilds )
	{
//		PrSleep(100);
		cell.m_template = false;

		size_t i, n = col.m_vChilds.size();
		for(i = 0; i < n; i++)
		{
			cell.m_col = col.m_vChilds[i];
			cell.m_item = cell.m_col->m_pItem;
			DrawCell(dc, rc, row, cell);
		}
	}
}

void CListItem::DrawCol(HDC dc, RECT& rc, cColumn& col)
{
	size_t i, n = col.m_vChilds.size();
	if( n && (col.m_nFlags & cColumn::fExpanded) )
	{
		for(i = 0; i < n; i++)
			DrawCol(dc, rc, *col.m_vChilds[i]);
	}

	if( &col == &m_cols || (col.m_nFlags & cColumn::fHided) )
		return;

	tDWORD flags = drColHeader;
	if( &col == m_hotcol )
		flags |= drHotlight;

	RECT rcCol = rc;
	rcCol.top = col.m_Offset.y - rc.top;
	rcCol.bottom = rcCol.top + col.m_Size.cy;
	rcCol.left = col.m_Offset.x - rc.left;
	rcCol.right = rcCol.left + col.m_Size.cx;

	m_pRoot->ClipRect(dc, &rcCol, false);
	m_pRoot->DrawRect(dc, rcCol, flags, 0);

	tDWORD dflags = TEXT_SINGLELINE|ALIGN_VCENTER|ALIGN_HCENTER;
	if( m_nState & ISTATE_DISABLED )
		dflags |= TEXT_GRAYED;

	RECT rcText = rcCol;
	rcText.left += 7;

	RECT rcPlus;
	if( GetColPlusRect(col, rcPlus) )
	{
		m_pIcoCols->Draw(dc, rcPlus, NULL, (col.m_nFlags & cColumn::fExpanded) ? 5 : 4);
		rcText.right -= m_pIcoCols->Size().cx;
	}

	if( &col == m_sortcol && m_pIcoCols )
	{
		RECT rcSort = rcText;
		rcSort.top = (rcCol.top + rcCol.bottom - m_pIcoCols->Size().cy) / 2;
		rcSort.left = rcSort.right - DRAW_ICON_MARGIN;
		m_pIcoCols->Draw(dc, rcSort, NULL, (col.m_nFlags & cColumn::fSortDown) ? 1 : 0);

		rcText.right -= DRAW_ICON_MARGIN;
	}

	RECT rcFilter;
	if( GetColFilterRect(col, rcFilter) )
		m_pIcoCols->Draw(dc, rcFilter, NULL, &col == m_hotcol ? 3 : 2);

	LPCSTR sText = col.m_sText.c_str();
	tString sTemp;

	if( col.m_pItem->m_nWrappedPos )
	{
		dflags |= TEXT_MODIFY;
		sTemp.resize(col.m_sText.size()+4);
		sTemp = sText;
		sText = sTemp.c_str();
	}

	m_pFont->Draw(dc, sText, rcText, dflags);

	if( col.m_pItem->m_nWrappedPos && col.m_sText != sTemp )
		*col.m_pItem->m_nWrappedPos = rcText.left;

	if( &col == m_dragcol )
		m_pRoot->DrawRect(dc, rcCol, drInvert, 0);

	m_pRoot->ClipRect(dc, NULL);
}

void CListItem::Scroll(enScrollType type, enUserAction dir, tDWORD pos)
{
	long& pt = (type == stVScroll) ? m_ptView.y : m_ptView.x;
	long maxp = (type == stVScroll) ? m_nAllSize : m_cols.m_Size.cx;
	long page = (type == stVScroll) ? m_szPage.cy : m_szPage.cx;

	switch(dir)
	{
	case sdNext:     pt += 4; break;
	case sdPrev:     PrevRow(m_toprow); pt -= m_toprow.Size(); break;
	case sdPageNext: pt += page; break;
	case sdPagePrev: pt -= page; break;
	case sdHome:     pt = 0; break;
	case sdEnd:      pt = maxp; break;
	case sdExactPos: pt = pos; break;
	}

	m_bWasScroll = true;
	UpdateLayout(true);
}

void CListItem::UpdateItem(cRowId* prow, cColumn* pcol)
{
	Update(); // !!!!!!!
}

void CListItem::UpdateRowItem()
{
	if( !m_rowitem )
		return;

	if( m_focused.m_node == this && !m_focused.m_pos )
	{
		m_rowitem->Show(false);
		return;
	}

	long diff = m_focused.Offset() - m_ptView.y;
	if( diff < 0 || diff > m_szPage.cy )
	{
		m_rowitem->Show(false);
		return;
	}

	m_rowitem->LockUpdate(true);
	m_rowitem->Show(true);

	bool bChanged = false;
	long foffs = diff + m_cols.m_Size.cy;
	long rsize = m_focused.Size();

	if( m_rowitem->m_szSize.cx != m_szPage.cx ||
		m_rowitem->m_szSize.cy != rsize )
	{
		m_rowitem->Resize(m_szPage.cx-m_rowitem->m_szSize.cx,
			rsize-m_rowitem->m_szSize.cy, true);
		bChanged = true;
	}

	if( m_rowitem->m_ptOffset.y != foffs )
		m_rowitem->m_ptOffset.y = foffs, bChanged = true;

	if( bChanged )
		m_rowitem->Reoffset();
	m_rowitem->LockUpdate(false);
}

bool CListItem::UpdateLayout(bool bUpdate)
{
	if( !(m_nState & ISTATE_INITED) )
		return false;

	if( !m_pRoot->IsOwnThread() )
	{
		if( !is_inited() )
			return false;

		tDWORD nTickCount = PrGetTickCount();
		if( !bUpdate && nTickCount - m_nLastTick < 10*m_nTimer )
			return true;

		m_nLastTick = nTickCount;
		m_Locker.unlock();
		m_pRoot->SendEvent(GRID_EVENT_UPDATELAYOUT, NULL, 0, this);
		PrSleep(m_nTimer);
		m_Locker.lock();
		return true;
	}

	if( !GetWindow() )
		return false;

	SIZE szView;
	OnGetViewSize(szView);

	m_szPage.cx = szView.cx;
	m_szPage.cy = szView.cy - m_cols.m_Size.cy;

	if( m_footer )
	{
		m_szPage.cy -= m_footer->m_szSize.cy;

		bool bChanged = false;
		if( m_footer->m_ptOffset.y + m_footer->m_szSize.cy != szView.cy )
			m_footer->m_ptOffset.y = szView.cy - m_footer->m_szSize.cy, bChanged = true;

		if( m_footer->m_szSize.cx != m_szPage.cx )
			m_footer->m_szSize.cx = m_szPage.cx, bChanged = true;

		if( bChanged )
			m_footer->Reoffset();
	}

	if( m_ptView.y > (long)m_nAllSize - m_szPage.cy + 1 )
		m_ptView.y = m_nAllSize - m_szPage.cy + 1;

	if( m_ptView.x > (long)m_cols.m_Size.cx - m_szPage.cx + 1 )
		m_ptView.x = m_cols.m_Size.cx - m_szPage.cx + 1;

	if( m_ptView.y < 0 )
		m_ptView.y = 0;

	if( m_ptView.x < 0 )
		m_ptView.x = 0;

	if( m_toprow.m_node == this || m_toprow.Offset() != m_ptView.y )
	{
		GetRow(m_ptView.y, m_toprow);
		if( m_ptView.y != m_toprow.Offset() )
			NextRow(m_toprow);
		m_ptView.y = m_toprow.Offset();
	}

	UpdateRowItem();

	if( bUpdate )
	{
		cScrollInfo info;
		info.nType = stVScroll;
		info.nMax = m_nAllSize;
		info.nPage = m_szPage.cy;
		info.nPos = m_ptView.y;
		OnUpdateScrollInfo(info);

		info.nType = stHScroll;
		info.nMax = m_cols.m_Size.cx;
		info.nPage = m_szPage.cx+1;
		info.nPos = m_ptView.x;
		OnUpdateScrollInfo(info);

		UpdateItem();
	}

	return true;
}

CGridItem* CListItem::GetCellItem(cRowId& row, cColumn& col, CStructData* data)
{
	CStructData sd(NULL);
	if( !data )
		data = &sd;

	CGridItem * pItem = col.m_pItem;
	if( row.IsStaticItem() )
	{
		pItem = (CGridItem *)row.m_node->m_pHeader;
		data->m_sect = STRUCTDATA_ALLSECTIONS;
	}
	else
		data->m_ser = GetRowData(row);

	pItem->UpdateData(true, data);

	return pItem;
}

long CListItem::CheckItem(cRowId& row, cColumn& col, long how, bool recourse)
{
	CStructData sd(NULL);
	CGridItem * pItem = GetCellItem(row, col, &sd);

	if( how == -2 )
		return !!pItem->m_value;

	if( how == -1 )
	{
		if( pItem->m_nState & ISTATE_DISABLED )
			return -1;

		pItem->m_value = !pItem->m_value;
	}
	else
		pItem->m_value = (tDWORD)how;

	pItem->UpdateData(false, &sd);

	long res = pItem->m_value.ToDWORD();

	cRowNode* toCheck = NULL;

	tDWORD i;
	if( recourse && (row.m_node->m_flags & cRowNode::fCheckGroup) )
	{
		if( row.IsNode() )
		{
			for(i = 0; i < row.m_node->m_nodes.size(); i++)
				CheckItem(TOR(cRowId, (row.m_node->m_nodes[i], 0)), col, res, true);

			for(i = 1; i <= row.m_node->m_nCount; i++)
				CheckItem(TOR(cRowId, (row.m_node, i)), col, res);
		}
		else if( res )
			CheckItem(TOR(cRowId, (row.m_node, 0)), col, res);
		else 
			toCheck = row.m_node;
	}
	else if( row.m_node->m_parent && row.m_node->m_parent->m_flags & cRowNode::fCheckGroup )
	{
		if( res )
			CheckItem(TOR(cRowId, (row.m_node->m_parent, 0)), col, res);
		else
			toCheck = row.m_node->m_parent;
	}

	if( toCheck )
	{
		bool bAllUnchecked = true;
		for(cRowId rowc(toCheck, 1); bAllUnchecked && rowc.m_pos <= toCheck->m_nCount; rowc.m_pos++)
			bAllUnchecked = !CheckItem(rowc, col, -2);

		for(i = 0; bAllUnchecked && i < toCheck->m_nodes.size(); i++)
			bAllUnchecked = !CheckItem(TOR(cRowId, (toCheck->m_nodes[i], 0)), col, -2);

		if( bAllUnchecked )
			CheckItem(TOR(cRowId, (toCheck, 0)), col, 0);
	}

	return res;
}

void CListItem::FillColumnsCombo(CItemBase* pCombo)
{
	CComboItem* combo = (CComboItem*)pCombo;

	size_t i, n = m_cols.m_vChilds.size();
	for(i = 0; i < n; i++)
	{
		cColumn* col = m_cols.m_vChilds[i];
		CGridItem* pItem = m_cols.m_vChilds[i]->m_pItem;

		cStrObj sColName;
		utf8_to_utf16(col->m_sText.c_str(), -1, sColName);

		cVariant v(col->m_pItem->m_strItemId);
		combo->AddItem(sColName.data(), v);
	}
}

bool CListItem::FindRows(int flags, cColumn* col, LPCSTR strText)
{
	if( !m_findctx )
		m_findctx = new cFindCtx(this);

	cFindCtx* fctx = (cFindCtx*)m_findctx;

	if( strText )
		fctx->m_sText = strText;

	if( fctx->m_sText.empty() )
		flags |= eShowDlg;

	if( flags & eShowDlg )
	{
		CDialogItem * pItem = fctx->Item();
		if( !pItem )
			GetOwner(true)->LoadItem(fctx, (CItemBase*&)pItem, ITEMID_FIND_DIALOG, "", 0);
		if( !pItem )
			return false;

		pItem->Activate();
		return true;
	}

	cVariant val;
	vector<cNode*> vNodes;
	size_t i, n, len = fctx->m_sText.size();

	bool bAll = fctx->m_sColumn == L"All";
	for(i = 0; i < m_cols.m_vChilds.size(); i++)
	{
		CGridItem* pItem = m_cols.m_vChilds[i]->m_pItem;
		if( !bAll && fctx->m_sColumn != pItem->m_strItemId.c_str() )
			continue;

		cNode* node = pItem->m_pBinding->GetHandler(CItemBinding::hText);
		if( node )
			vNodes.push_back(node);

	}

	if( !(n = vNodes.size()) )
		return false;

	CStructData sd(NULL);
	cNodeExecCtx ctx(m_pRoot, this, &sd, NULL);
	cRowId row = m_focused;
	bool bFound = false;

	do
	{
		bool res = (flags & eFindUp) ? PrevRow(row) : NextRow(row);
		if( !res )
			(flags & eFindUp) ? LastRow(row) : FirstRow(row);

		sd.m_ser = GetRowData(row);

		for(i = 0; i < n; i++)
		{
			cNode* node = vNodes[i];
			node->Exec(val, ctx);

			const cStrObj& sText = val.ToStringObj();
			tDWORD pos = 0;
			while( 1 )
			{
				pos = sText.find(fctx->m_sText, pos, cStrObj::whole,
					fSTRING_FIND_FORWARD|(!(flags & eMatchCase) ? fSTRING_COMPARE_CASE_INSENSITIVE:0));

				if( pos == cStrObj::npos )
					break;

				if( !(flags & eWholeWord) )
					break;

				char p = pos ? sText[pos-1] : 0;
				if( p && (p != ' ' && p != '\t') )
					continue;

				p = sText[pos+len];
				if( !isalpha(p) && !isdigit(p) )
					break;
				pos++;
			}

			if( pos == cStrObj::npos )
				continue;

			bFound = true;
			if( flags & eMarkAll )
			{
				row.Select(true);
				break;
			}

			FocusRow(row, true, true);
		}
	}
	while( !(row == m_focused) );

	if( !bFound )
		m_pRoot->ShowMsgBox(*fctx, "NotFound", "dlg_caption", MB_OK|MB_ICONEXCLAMATION);

	return bFound;
}

void CListItem::ShowMenu(cColumn& col, POINT& pt)
{
	CPopupMenuItem * pMenu = NULL;
	LoadItem(NULL, (CItemBase *&)pMenu, ITEMID_HDRPOPUPMENU);
	if( !pMenu )
		return;

//	pMenu->Enable(ITEMID_FIND, !!GetItemsCount());

	if( m_sortcol == &col )
	{
		CItemBase * pItem = pMenu->GetItem(m_sortcol->m_nFlags & cColumn::fSortDown ?
				ITEMID_SORTDOWN : ITEMID_SORTUP);

		if( pItem )
			pItem->m_nState = ISTATE_SELECTED;
	}

	CItemBase* pGroup = pMenu->GetItem(ITEMID_GROUP);
	if( pGroup )
	{
		if( !col.m_pItem->m_condition )
			pGroup->Enable(false);
		else if( col.m_nFlags & cColumn::fGrouped )
			pGroup->m_nState = ISTATE_SELECTED;
	}

	CItemBase* pFilter = pMenu->GetItem(ITEMID_FILTER);
	if( pFilter )
	{
		if( !(col.m_nFlags & (cColumn::fAutoFlt|cColumn::fCustomFlt)) )
			pFilter->Enable(false);
		else if( col.m_nFlags & cColumn::fFiltered )
			pFilter->m_nState = ISTATE_SELECTED;
	}

	size_t nCount = m_cols.m_vChilds.size();
	for(size_t i = 0; i < nCount; i++)
	{
		cColumn& col = *m_cols.m_vChilds[i];

		CItemBase * pItem = pMenu->CloneItem(NULL, "");
		pItem->m_strText = col.m_sText;
		pItem->m_strItemType = GRID_COLUMN_TYPE;

		if( !(col.m_nFlags & cColumn::fHided) )
			pItem->m_nState = ISTATE_SELECTED;

		if( col.m_nFlags & cColumn::fFixedPos )
			pItem->m_nState = ISTATE_DISABLED;
	}

	POINT ptMenu = pt;
	{
		RECT rcGlobal; GetParentRect(rcGlobal, m_pRoot);
		ptMenu.x += rcGlobal.left + 2;
		ptMenu.y += rcGlobal.top + 2;
	}
	pMenu->Track(&ptMenu);
	pMenu->Destroy();
}

cNode* CListItem::FillSwitchItem(CItemBase* pTop, cColumn& col, bool bCombo)
{
	cNodeExecCtx ctx(m_pRoot);

	cNode* pText = col.m_pItem->m_filter ? col.m_pItem->m_filter :
		col.m_pItem->m_pBinding->GetHandler(CItemBinding::hText);

	cNodeSwitch* pIcon = (cNodeSwitch*)col.m_pItem->m_pBinding->GetHandler(CItemBinding::hIcon);
	if( pIcon && pIcon->Type() != ntSwitch )
		pIcon = NULL;
	else if( pIcon && !pIcon->m_inited )
		pIcon->Init(ctx);

	if( pText->Type() != ntSwitch )
		return NULL;

	cNodeInt * curnode = NULL;
	if( col.m_filter && col.m_filter->Type() == ntCmpEQ )
	{
		curnode = (cNodeInt*)((cOperatorBinary*)col.m_filter)->m_args[1];
		if( curnode->Type() != ntNumber )
			curnode = NULL;
	}

	cVariant value;

	cNodeSwitch* pSwitch = (cNodeSwitch*)pText;
	if( !pSwitch->m_inited )
		pSwitch->Init(ctx);

	for(size_t i = 0; i < pSwitch->m_cases.size(); i++)
	{
		cNodeSwitch::cCaseItem& item = pSwitch->m_cases[i];
		if( item.m_node == notInitialized )
			pSwitch->PrepareNode(item.m_node, i, ctx);

		if( !item.m_node || item.m_node == useNext )
			continue;

		item.m_node->Exec(value, ctx, true);
		if( bCombo )
		{
			((CComboItem*)pTop)->AddItem((LPCWSTR)value.ToStringObj().data(), TOR(cVar, (item.m_id)));
			continue;
		}

		CItemBase * pItem = pTop->CloneItem(NULL, "");
		pItem->m_strText = value.ToString();
		pItem->m_strItemType = GRID_COLUMN_TYPE;
		pItem->m_pUserData = &item;

		if( curnode && curnode->m_val == item.m_id )
			pItem->m_nState = ISTATE_SELECTED;

		if( !pIcon )
			continue;

		bool bNext = false;
		for(size_t j = 0; j < pIcon->m_cases.size(); j++)
		{
			cNodeSwitch::cCaseItem& ico = pIcon->m_cases[j];
			if( ico.m_id != item.m_id || bNext )
				continue;

			if( ico.m_node == notInitialized )
				pIcon->PrepareNode(ico.m_node, j, ctx);

			bNext = (ico.m_node == useNext);

			if( ico.m_node && !bNext )
			{
				ico.m_node->Exec(value, ctx, true);
				pItem->SetIcon(m_pRoot->GetIcon(value.ToString().c_str()));
			}
		}
	}

	return pSwitch->m_arg1.m_arg;
}

void CListItem::ShowFilterMenu(cColumn& col, POINT& pt)
{
	if( col.m_nFlags & cColumn::fCustomFlt )
	{
		ShowFilterDialog(col);
		return;
	}

	if( !(col.m_nFlags & cColumn::fAutoFlt) )
		return;

	CPopupMenuItem * pMenu = NULL;
 	LoadItem(NULL, (CItemBase *&)pMenu, ITEMID_FLTPOPUPMENU);
	if( !pMenu )
		return;

	cNode* caseNode = FillSwitchItem(pMenu, col, false);

	POINT ptMenu = pt;
	{
		RECT rcGlobal; GetParentRect(rcGlobal, m_pRoot);
		OnAdjustClientRect(rcGlobal);
		ptMenu.x += rcGlobal.left + 2;
		ptMenu.y += rcGlobal.top + 2;
	}
	CItemBase* pItem = pMenu->Track(&ptMenu);

	if( pItem )
	{
		switch(pItem->m_nPos)
		{
		case 0: ShowFilterDialog(col); break;
		case 1: SetFilter((cNode*)NULL, (tPTR)&col); break;
		case 2: break;
		default:
			if( cNodeSwitch::cCaseItem* item = (cNodeSwitch::cCaseItem*)pItem->m_pUserData )
			{
				cFilterCtx *& pCtx = (cFilterCtx*&)col.m_pFltCtx;
				if( !pCtx )
					pCtx = new cFilterCtx(this, &col);

				SetFilter(pCtx->MakeEqNode(caseNode, item->m_id), (tPTR)&col);
			}
		}
	}

	pMenu->Destroy();
}

void CListItem::ShowFilterDialog(cColumn& col)
{
	cFilterCtx *& pCtx = (cFilterCtx*&)col.m_pFltCtx;
	if( !pCtx )
		pCtx = new cFilterCtx(this, &col);

	if( pCtx->InitNode() )
		if( pCtx->DoModal(this, "Grid.Filter") == DLG_ACTION_OK )
			SetFilter(pCtx->MakeNode(), (tPTR)&col);
}

void CListItem::GetElemViewRect(cRowId * row, cColumn * col, RECT& rc, enElemType nType)
{
	rc.left = 0;
	rc.right = m_szSize.cx;
	
	if( row )
	{
		rc.top = m_cols.m_Size.cy + row->Offset() - m_ptView.y;
		rc.bottom = rc.top + row->Size();
	}

	RECT rcCol;
	if( col )
	{
		rcCol.left = col->m_Offset.x - m_ptView.x;
		rcCol.right = rcCol.left + col->m_Size.cx;
		rcCol.top = col->m_Offset.y - m_ptView.y;
		rcCol.bottom = rcCol.top + col->m_Size.cy;

		if( row )
		{
			rc.left = rcCol.left;
			rc.right = rcCol.right;
		}
		else
		{
			if( nType == etNothing )
				rc = rcCol;
			else if( nType == etColFilter )
				GetColFilterRect(*col, rc);
		}
	}
}

bool CListItem::OnClicked(CItemBase* pItem)
{
	bool res = CItemBase::OnClicked(pItem);

	CItemBase * pHdrMenu = pItem->GetOwner(false, ITEMID_HDRPOPUPMENU);
	if( pHdrMenu )
	{
		if( !m_hotcol )
			return res;

		if( pItem->m_strItemType == GRID_COLUMN_TYPE )
		{
			tDWORD diff = pItem->m_pParent->m_aItems.size() - pItem->m_nPos;
			cColumn* col = m_cols.m_vChilds[m_cols.m_vChilds.size() - diff];
			col->Hide(!(col->m_nFlags & cColumn::fHided) );
		}
		else
		{
			sswitch(pItem->m_strItemId.c_str())
			scase(ITEMID_SORTUP)   SortByColumn(*m_hotcol, 1); sbreak;
			scase(ITEMID_SORTDOWN) SortByColumn(*m_hotcol, 2); sbreak;
			scase(ITEMID_GROUP)    MakeGroup(m_hotcol->m_pItem, (m_hotcol->m_nFlags & cColumn::fGrouped) ? 1 : 0); sbreak;
			scase(ITEMID_FIND)     FindRows(eShowDlg, m_hotcol); sbreak;
			scase(ITEMID_FILTER)   ShowFilterDialog(*m_hotcol); sbreak;
			send;
		}
	}
	return res;
}

void CListItem::OnItemCliked(cRowId& row, cColumn& col, bool rClick)
{
	CItemBase * pItem = col.m_pItem;
	if( !pItem || !(pItem->m_nFlags & STYLE_CLICKABLE) )
		if( !(row.IsNode() && row.m_node->m_pHeader) )
			pItem = m_rowprops;

	if( !pItem || !(pItem->m_nFlags & STYLE_CLICKABLE) )
		return;

	CFieldsBinder * pBinder = pItem->m_pBinding ? pItem->m_pBinding->m_pBinder : NULL;
	if( !pBinder )
		return;

	cCellData cell;
	cell.m_col = &col;
	cell.m_data = GetRowData(row);

	if( m_pSink )
		((CListItemSink*)m_pSink)->GetCellData(cell);

//	pItem->m_ptOffset.x = col.m_Offset.x - m_ptView.x;
//	pItem->m_ptOffset.y = row.Offset() - m_ptView.y + m_cols.m_Size.cy;
//	pItem->Reoffset();

	pBinder->AddDataSource(cell.m_data);
	pItem->Click(false, rClick);
	pBinder->ClearSources();
}

void CListItem::OnCliked(POINT& pt, tDWORD state)
{
	RECT rcPlus;
	long posY = m_ptView.y + pt.y - m_cols.m_Size.cy;
	long posX = m_ptView.x + pt.x;

	long flags = 0;
	cColumn* col = m_cols.HitTest(posX, pt.y, flags, true);

	PRAutoLocker lock(m_Locker);

	if( flags & cColumn::hfInner )
	{
		OnOver(pt);
		if( state & fDouble )
		{
			if( col && (flags & cColumn::hfBorder) )
				AutoFitColumn(*col);
		}
		else if( col && (flags & cColumn::hfBorder) )
		{
			OnSetCursor(ctResize);
			if( OnColumnTrack(*col, pt.x) )
				UpdateLayout(true);
		}
		else if( col && (flags & cColumn::hfFilter) )
		{
			ShowFilterMenu(*col, pt);
		}
		else if( col )
		{
			if( GetColPlusRect(*col, rcPlus) )
			{
				if( rcPlus.left <= pt.x && pt.x <= rcPlus.right &&
					rcPlus.top <= pt.y && pt.y <= rcPlus.bottom )
				{
					col->Expand(!(col->m_nFlags & cColumn::fExpanded));
					return;
				}
			}

			if( state & fRButton )
				ShowMenu(*col, pt);
			else if( (col->m_nFlags & cColumn::fFixedPos) || !OnDragItem(NULL, col, pt) )
				SortByColumn(*col);
		}
		return;
	}

	cRowId row;
	if( posY - m_ptView.y > m_szPage.cy || !GetRow(posY, row) )
		return;

	RECT rcCheck;
	if( col && GetCheckRect(row, *col, rcCheck) )
	{
		if( rcCheck.left <= pt.x && pt.x <= rcCheck.right &&
			rcCheck.top <= pt.y && pt.y <= rcCheck.bottom )
		{
			if( CheckItem(row, *col, -1, true) != -1 )
			{
				CItemBase *item = row.IsStaticItem() ? row.m_node->m_pHeader : this;
				item->SetChangedData();
			}
			Update();
			return;
		}
	}

	if( state & fDouble )
	{
		if( row.IsNode() && (row.m_node->m_nodes.size() || row.m_node->m_nCount) )
		{
			row.m_node->Expand(!row.m_node->m_bExpanded);
			return;
		}

		CItemBase::DblClick();
		return;
	}

	if( (state & fShift) && !(m_flags & fSingleSel) )
	{
		ClearSelection();
		if( m_lastSel > posY )
			SelectRange(posY, m_lastSel);
		else
			SelectRange(m_lastSel, posY);
	}
	else if( !(state & fRButton) || !row.Idx().m_sel )
	{
		bool bSelect = true;
		if( state & fControl )
			bSelect = !row.Idx().m_sel;
		else
			ClearSelection();

		row.Select(bSelect);
		m_lastSel = posY;
	}

	FocusRow(row, false, true);
	m_bWasScroll = false;

	bool bDraw;
	if( GetPlusRect(row, rcPlus, bDraw) && bDraw )
	{
		if( rcPlus.left <= pt.x && pt.x <= rcPlus.right &&
			rcPlus.top <= pt.y && pt.y <= rcPlus.bottom )
		{
			row.m_node->Expand(!row.m_node->m_bExpanded);
			return;
		}
	}

	if( col )
		if( (state & fRButton) || !(col->m_nFlags & cColumn::fDragable) || !OnDragItem(&row, col, pt) )
			OnItemCliked(row, *col, !!(state & fRButton));
}

bool CListItem::OnOver(POINT& pt)
{
	cColumn * oldcol = m_hitcol;

	m_hotflags = 0;
	m_hitcol = m_cols.HitTest(pt.x + m_ptView.x, pt.y, m_hotflags, true);

	bool bChangedTip = false;
	if( !(m_hotflags & cColumn::hfInner) )
	{
		PRAutoLocker lock(m_Locker);

		cRowId row = m_hotrow;

		long posY = m_ptView.y + pt.y - m_cols.m_Size.cy;
		if( posY - m_ptView.y <= m_szPage.cy )
			if( !GetRow(posY, m_hotrow) )
				m_hotrow.m_node = this, m_hotrow.m_pos = 0;

		bChangedTip = (m_hotcol || !(row == m_hotrow) || (oldcol != m_hitcol));
		m_hotcol = NULL;
	}
	else
	{
		bChangedTip = (m_hitcol != m_hotcol);
		m_hotcol = m_hitcol;
	}

	if( bChangedTip )
	{
		m_pRoot->EnableToolTip(this, true);
		UpdateItem(NULL, m_hotcol);
	}

	if( !m_hotcol && m_hitcol && m_hotrow.m_node != this &&
		(m_hitcol->m_pItem->m_nFlags & STYLE_CLICKABLE) )
	{
		CItemBinding* binding = m_hitcol->m_pItem->m_pBinding;
		if( binding && binding->GetHandler(CItemBinding::hOnClick) )
		{
			OnSetCursor(ctHand);
			return true;
		}
	}

	if( m_hotcol && (m_hotflags & cColumn::hfBorder) )
	{
		OnSetCursor(ctResize);
		return true;
	}
	return true;
}

void CListItem::OnKeyDown(enUserAction action, tDWORD state)
{
	PRAutoLocker lock(m_Locker);

	cRowId id = m_focused;
	switch(action)
	{
	case sdLeft: 
		if( !(state & fControl) )
		{
			if( m_focused.IsNode() )
				m_focused.m_node->Expand(false);
			else if( m_focused.m_node != this )
			{
				id.m_pos = 0;
				break;
			}
		}		
		else
			Scroll(stHScroll, (state & fControl) ? sdHome : sdPagePrev, 0);
		return;
	case sdRight: 
		if( !(state & fControl) && m_focused.IsNode() )
			m_focused.m_node->Expand(true);
		else
			Scroll(stHScroll, (state & fControl) ? sdEnd : sdPageNext, 0);
		return;
	case sdPrev: 
		PrevRow(id);
		break;
	case sdNext: 
		NextRow(id);
		break;
	case sdHome: 
		FirstRow(id);
		break;
	case sdEnd: 
		LastRow(id);
		break;
	case sdPagePrev: 
		if( !GetRow(m_focused.Offset()-m_szPage.cy, id) )
			FirstRow(id);
		break;
	case sdPageNext: 
		if( !GetRow(m_focused.Offset()+m_szPage.cy, id) )
			LastRow(id);
		break;
	case sdSpace:
		if( m_cols.m_vChilds.size() )
		{
			cColumn& col = *m_cols.m_vChilds[0];
			if( col.m_nFlags & cColumn::fChecked )
			{
				bool dataChanged = false;
				long how = -1;
				cRowId row;
				while( GetSelectedItem(row) != -1 )
				{
					long res = CheckItem(row, col, how, how == -1);
					if( how == -1 )
						how = res;
					
					if( res != -1 )
						if( row.IsStaticItem() )
							row.m_node->m_pHeader->SetChangedData();
						else
							dataChanged = true;
				}
				
				if( dataChanged )
					SetChangedData();
				Update();
			}
		}
		return;
	default:
		return;
	}

	if( id == m_focused )
		return;

	FocusRow(id, true, true);
	m_bWasScroll = false;
}

void CListItem::OnSize(SIZE& sz)
{
	if( m_bWasLoad )
		return;

	long oldPageCX = m_szPage.cx;

	UpdateLayout(false);

	if( oldPageCX != m_szPage.cx )
	{
		long diff = m_szPage.cx - oldPageCX;
		if( !m_nTimer )
			m_cols.ChangeSize(diff);
	}

	UpdateLayout(true);
//	EnsureVisible(m_focused);
}

void CListItem::OnExpanded(cRowNode* node, bool bExpand)
{
	if( m_pSink )
		((CListItemSink*)m_pSink)->OnNodeExpanded(node, bExpand);

	if( !bExpand )
	{
		cRowId row(node,0);
		if( node->m_nSelItems || m_focused.m_node == node )
			FocusRow(row, false, false);

		if( node->m_nSelItems )
		{
			ClearSelection();
			row.Select(true);
		}
	}
	else
	{
		if( !m_nTimer )
			RefreshView(true);
		else
			m_nLoopState = 0;
	}

	if( node->m_nListState == m_nCurState )
		EnsureVisible(m_focused);
}

void CListItem::OnRowInserted(cRowNode* node, tDWORD pos)
{
	if( m_focused.m_node == node && m_focused.m_pos >= pos )
		m_focused.m_pos++;

	if( m_toprow.m_node == node && m_toprow.m_pos >= pos )
		m_toprow.m_pos++;

	m_ptView.y = m_toprow.Offset();
}

void CListItem::EnsureVisibleEx(cRowId& row, bool bThis)
{
	if( row.m_node != this )
	{
		if( !row.m_node->m_bExpanded && !(bThis && row.IsNode()) )
			row.m_node->Expand(true);

		cRowId parent(row.m_node->m_parent, 0);
		EnsureVisibleEx(parent, false);
	}

	if( bThis )
		EnsureVisible(row);
}

void CListItem::EnsureVisible(cRowId& row)
{
	if( !(m_nState & ISTATE_INITED) )
		return;

	long offset = row.Offset();
	long size = row.Size();

	if( m_ptView.y > offset )
	{
		m_ptView.y = offset;
	}
	else if( m_ptView.y + m_szPage.cy < offset + size )
	{
		m_ptView.y = offset + size - m_szPage.cy;
	}

	UpdateLayout(true);
}

bool CListItem::DragItem(cRowId* row, cColumn* col, POINT& pt, bool bEnd)
{
	if( !m_dragimg && bEnd )
		return false;

	if( !row )
		return DragColumn(*col, pt, bEnd);

	if( !m_dragimg )
	{
		tDWORD indent = row->Indent() * LIST_INDENT_SIZE;
		m_dragimg = m_pRoot->CreateImage(CRootItem::eImageImage,
				NULL, col->m_Size.cx - indent, row->Size());

		RECT rc = {col->m_Offset.x + indent, 0, 0, row->Size()};
		DrawRow(m_dragimg->DcHnd(), rc, *row);
	}

	cRowId dragrow;

	long posY = pt.y - m_cols.m_Size.cy, flags = 0;
	cColumn* to = m_cols.HitTest(pt.x + m_ptView.x, pt.y, flags);

	enCursorType cursor = ctNo;
	if( to == col && posY < m_szPage.cy && GetRow(m_ptView.y + posY, dragrow) )
	{
		if( m_pSink && ((CListItemSink*)m_pSink)->OnDragRow(*row, dragrow, bEnd) )
			cursor = ctDefault;
	}

	m_pRoot->DragImage(this, m_dragimg, m_dctx[0], pt, bEnd ? dtEnd : dtMove);

	if( !bEnd )
	{
		OnSetCursor(cursor);
		return true;
	}

	if( m_dragimg )
		m_dragimg->Destroy(), m_dragimg = NULL;

	OnSetCursor(ctDefault);
	return false;
}

bool CListItem::DragColumn(cColumn& col, POINT& pt, bool bEnd)
{
	bool ret = false;
	if( !m_dragimg )
	{
		m_dragimg = m_pRoot->CreateImage(CRootItem::eImageImage,
				NULL, col.m_Size.cx, col.m_Size.cy);

		if( m_dragimg )
		{
			RECT rc = {col.m_Offset.x, col.m_Offset.y};
			DrawCol(m_dragimg->DcHnd(), rc, col);
		}

		m_dragcol = &col;
		UpdateItem(NULL, m_dragcol);
	}

	enCursorType cursor = bEnd ? ctDefault : ctNo;
	enDragType   drtype = bEnd ? dtEnd : dtHide;

	long posx = -1, flags = 0;
	cColumn* to = m_cols.HitTest(pt.x + m_ptView.x, pt.y, flags);
	if( to && (flags & cColumn::hfInner) )
	{
		posx = to->m_Offset.x;
		if( flags & cColumn::hfRight )
			posx += to->m_Size.cx;
		else if( to->m_nFlags & cColumn::fFixedPos )
			to = NULL;

		if( to && to != &col && col.m_parent == to->m_parent && 
				posx != col.m_Offset.x && posx != col.m_Offset.x + col.m_Size.cx )
		{
			if( !bEnd )
			{
				drtype = dtMove;
				cursor = ctDefault;
			}
		}
		else
			posx = -1;
	}

	POINT ptt = pt;

	bool bHideImage = (m_drpos != posx);
	m_pRoot->DragImage(this, m_dragimg, m_dctx[0], ptt, bHideImage ? dtHide : bEnd ? dtEnd : dtMove);

	if( m_pIcoCols )
	{
		if( to ) pt.x = posx - m_ptView.x - 3;
		if( to ) pt.y = to->m_Offset.y - 6;
		m_pRoot->DragImage(this, m_pIcoCols, m_dctx[1], pt, drtype, 7);

		if( to ) pt.y = to->m_Offset.y + col.m_Size.cy + 4;
		m_pRoot->DragImage(this, m_pIcoCols, m_dctx[2], pt, drtype, 6);
	}

	if( bHideImage )
		m_pRoot->DragImage(this, m_dragimg, m_dctx[0], ptt, bEnd ? dtEnd : dtMove);

	m_drpos = posx;
	if( drtype == dtEnd )
	{
		if( m_dragimg )
			m_dragimg->Destroy(), m_dragimg = NULL;
		m_dragcol = NULL;

		if( to && posx != -1 )
		{
			posx = -1;
			std::vector<cColumn*>& v = to->m_parent->m_vChilds;
			for(size_t i = 0; i < v.size(); i++)
			{
				if( v[i] == &col )
				{
					v.erase(v.begin()+i--);
					continue;
				}

				if( v[i] == to )
				{
					if( flags & cColumn::hfRight )
						v.insert(v.begin() + ++i, &col);
					else
						v.insert(v.begin() + i, &col);
					i++;
				}
			}
			m_cols.RecalcOffset();
			ret = true;
		}
		UpdateItem();
	}

	OnSetCursor(cursor);
	return true;
}

void CListItem::AutoFitColumn(cColumn& col, bool bRecourse)
{
	if( bRecourse )
	{
		size_t i, n = col.m_vChilds.size();
		for(i = 0; i < n; i++)
			AutoFitColumn(*col.m_vChilds[i], bRecourse);

		if( &col == &m_cols )
		{
			if( col.m_Size.cx < m_szPage.cx )
				col.ChangeSize(m_szPage.cx-col.m_Size.cx);
			UpdateLayout(true);
		}
	}

	if( col.IsGroup() || col.IsHiden() )
		return;

	HDC dc = m_pRoot->GetDesktopDC();
	RECT rcCalc = {0,0,0,MAX_CTL_SIZE};
	long colSize = 0;

	CStructData sd(NULL);
	CGridItem* pItem = col.m_pItem;

	long maxsize = m_ptView.y + m_szPage.cy;
	bool next = true, bColFirst = col.IsFirst();
	for(cRowId row = m_toprow; next; next = NextRow(row))
	{
		if( (long)row.Offset() >= maxsize )
			break;

		if( row.IsCustom() )
			continue;

		tLONG shift = 0;
		if( bColFirst )
			shift += row.Indent() * LIST_INDENT_SIZE;

		sd.m_ser = GetRowData(row);
		pItem->UpdateData(true, &sd);

		rcCalc.left = shift + 3*LIST_CELL_MARGIN;
		rcCalc.right = MAX_CTL_SIZE;
		if( pItem->m_pIcon || m_rowprops )
			rcCalc.left += DRAW_ICON_MARGIN;

		pItem->m_pFont->Draw(dc, pItem->m_strText.c_str(),
			rcCalc, TEXT_CALCSIZE|TEXT_SINGLELINE);

		if( rcCalc.right > colSize )
			colSize = rcCalc.right;
	}

	col.ChangeSize(colSize-col.m_Size.cx);
	if( !bRecourse )
		UpdateLayout(true);
}

void CListItem::MakeCopyString(cRowId& row, cColumn& col, cStrObj& sText)
{
	if( col.IsHiden() )
		return;

	size_t i, n = col.m_vChilds.size();
	if( !col.IsGroup() )
	{
		CGridItem* pItem = GetCellItem(row, col);
		if( pItem )
		{
			cStrObj sData; m_pRoot->LocalizeStr(sData, pItem->m_strText.c_str());
			sText += sData;
		}
		sText += L"\t";
		if( n )
			return;
	}

	for(i = 0; i < n; i++)
		MakeCopyString(row, *col.m_vChilds[i], sText);

	if( &col == &m_cols )
		sText += L"\r\n";
}

bool CListItem::GetColumnInfo(cColumn& col, const char* in, tString* out)
{
	char buff[0x40] = VERSION_TAG;
	if( &m_cols != &col )
		strncpy_s(buff, sizeof(buff), col.m_pItem->m_strItemId.c_str(), 2);

	if( in )
	{
		if( strncmp(in, buff, 2) )
			return false;

		long oldsize = col.m_Size.cx, size = col.m_Size.cx;
		long flags = 0;
		sscanf(in+2, "%4x%2x%4x", &flags, &col.m_nGrpPos, &size);

		col.m_nFlags &= 0xFFFFF000;
		col.m_nFlags |= (flags & 0x00000FFF);

		if( size < 0xFFFF )
			col.ChangeSize(size-oldsize);
	}
	else
	{
		sprintf_s(buff+2, sizeof(buff)-2, "%.4x%.2x%.4x",
			col.m_nFlags & 0xFFFF, col.m_nGrpPos, col.m_Size.cx & 0xFFFF);

		*out += buff;
	}
	return true;
}

void CListItem::SaveColumnInfo(cColumn& col, tString& sData)
{
	GetColumnInfo(col, NULL, &sData);

	if( (col.m_nFlags & cColumn::fFiltered) && col.m_pFltCtx )
	{
		sData += ':';
		((cFilterCtx*)col.m_pFltCtx)->SaveData(sData);
	}

	SaveExpanded(col.m_pItem);

	size_t i, n = col.m_pItem->m_vExpanded.size();
	for(i = 0; i < n; i++)
	{
		char buff[0x40];
		sprintf_s(buff, sizeof(buff), "%.16I64x", col.m_pItem->m_vExpanded[i]);
		sData += buff;
	}

	sData += ';';

	n = col.m_vChilds.size();
	for(i = 0; i < n; i++)
	{
		cColumn& child = *col.m_vChilds[i];
		SaveColumnInfo(child, sData);
	}
}

bool CListItem::LoadColumnInfo(cColumn& col, LPCSTR& sData)
{
	const char* sEnd = strchr(sData, ';');
	if( !sEnd )
		return false;

	if( (sEnd-sData < 12) || !GetColumnInfo(col, sData, NULL) )
		return false;

	if( (sData+=12) != sEnd && *sData == ':' )
	{
		cFilterCtx *& pCtx = (cFilterCtx*&)col.m_pFltCtx;
		if( !pCtx )
			pCtx = new cFilterCtx(this, &col);
		if( pCtx->InitNode() )
		{
			pCtx->LoadData(sData);
			SetFilter(pCtx->MakeNode(), (tPTR)&col);
		}
	}

	col.m_pItem->m_vExpanded.clear();
	for( ;*sData != ';'; sData += 16 )
	{
		tQWORD val;
		if( !sscanf(sData, "%16I64x", &val) )
			break;

		col.m_pItem->m_vExpanded.push_back(val);
	}

	sData = sEnd+1;

	if( (col.m_nFlags & cColumn::fGrouped) && col.m_pItem->m_condition )
	{
		if( (size_t)col.m_nGrpPos >= m_groups.size() )
			m_groups.resize(col.m_nGrpPos+1);

		m_groups[col.m_nGrpPos] = col.m_pItem;
	}

	if( col.m_nFlags & cColumn::fSortUp )
		SortByColumn(col, 1);
	else if( col.m_nFlags & cColumn::fSortDown )
		SortByColumn(col, 2);

	size_t i, j, n = col.m_vChilds.size();
	for(i = 0; i < n; i++)
		for(j = i; j < n; j++)
		{
			cColumn& child = *col.m_vChilds[j];
			if( !LoadColumnInfo(child, sData) )
				continue;

			if( i != j )
			{
				col.m_vChilds[j] = col.m_vChilds[i];
				col.m_vChilds[i] = &child;
			}
			break;
		}

	return true;
}

void CListItem::SaveSettings(cAutoRegKey& pKey)
{
	CItemBase::SaveSettings(pKey);

	if( !(m_nFlags & STYLE_REMEMBER_DATA) )
		return;

	tString strKey, sData;
	SaveColumnInfo(m_cols, sData);

	GetOwnerRelativePath(strKey, GetOwner(true));
	if( m_curview == -1 )
	{
		pKey.set_strobj(strKey.c_str(), cStrObj(sData.c_str()));
	}
	else
	{
		cAutoRegKey pSubKey(pKey, strKey.c_str(), cTRUE);
		cGridViewTemplate& data = m_views[m_curview];

		pSubKey.set_strobj(data.m_sViewName.c_str(cCP_ANSI), cStrObj(sData.c_str()));
		pSubKey.set_strobj("current", data.m_sViewName);
	}
}

void CListItem::LoadSettings(cAutoRegKey& pKey)
{
	CItemBase::LoadSettings(pKey);

	if( !(m_nFlags & STYLE_REMEMBER_DATA) )
		return;

	tString strKey;
	GetOwnerRelativePath(strKey, GetOwner(true));

	cStrObj strData;
	if( m_curview == -1 )
	{
		pKey.get_strobj(strKey.c_str(), strData);
	}
	else
	{
		strData = m_views[m_curview].m_sViewData;
//		cAutoRegKey pSubKey;
//		if( PR_SUCC(pSubKey.open(pKey, pKey, strKey.c_str())) )
//			pSubKey.get_strobj(m_views[m_curview].m_sViewName.c_str(cCP_ANSI), strData);
	}

	if( !strData.empty() )
	{
		m_bWasLoad = true;
		UpdateColumnsLayout();
		SetView(strData.c_str(cCP_ANSI));
	}
}

void CListItem::InitViews()
{
	CItemBase* pViewItems = GetItem("views");
	if( pViewItems )
	{
		size_t i, n = pViewItems->m_aItems.size();
		for(i = 0; i < n; i++)
		{
			CItemBase* pItem = pViewItems->m_aItems[i];

			if( pItem->m_nState & ISTATE_DEFAULT ) 
				m_curview = i;

			cGridViewTemplate data;
			m_pRoot->LocalizeStr(data.m_sViewName, pItem->m_strText.c_str());

			cVariant res; pItem->GetValue(res);
			cStrObj vdata; res.ToStringObj(vdata);

			data.m_sViewData = VERSION_TAG;
			data.m_sViewData.append(vdata);
			data.m_sViewData.erase(2,2);

			m_views.push_back(data);
		}
	}

	tString strKey = "Windows\\";
	strKey += m_pOwner->m_strSection;
	strKey += "\\";

	tString strSubKey;
	GetOwnerRelativePath(strSubKey, GetOwner(true));
	strKey += strSubKey;

	cAutoRegKey pKey;
	if( PR_FAIL(pKey.open(m_pRoot->m_cfg, cRegRoot, strKey.c_str(), cFALSE)) )
		return;

	cStrObj sCurrent;
	pKey.get_strobj("current", sCurrent);

	tCHAR buff[0x1000];
	for(tDWORD idx = 0; ; idx++)
	{
		cGridViewTemplate data;
		if( PR_FAIL(((cRegistry*)pKey)->GetValueNameByIndex(NULL, pKey, idx, buff, sizeof(buff))) )
			break;

		data.m_sViewName = buff;

		tDWORD size = sizeof(buff); tTYPE_ID type = tid_STRING;
		if( PR_FAIL(((cRegistry*)pKey)->GetValueByIndex(NULL, pKey, idx, &type, buff, sizeof(buff))) )
			continue;

		if( !strchr(buff, ';') || strncmp(buff, VERSION_TAG, 2) )
			continue;

		size_t i, n = m_views.size();
		for(i = 0; i < n; i++)
			if( m_views[i].m_sViewName == data.m_sViewName )
				break;

		if( i == n )
			m_views.push_back(data);

		if( !strncmp(buff, VERSION_TAG, 2) )
			m_views[i].m_sViewData = buff;

		if( sCurrent == data.m_sViewName )
			m_curview = i;
	}
}

void CListItem::SetView(LPCSTR sView)
{
	if( !(m_nState & ISTATE_INITED) )
		return;

	if( strncmp(sView, VERSION_TAG, 2) )
		return;

	PRAutoLocker lock(m_Locker);

	if( !sView )
		return;

	m_groups.clear();
	m_group = NULL;
	ClearFilters(true);

	if( m_condition )
		MakeGroup(this, 0);

	LoadColumnInfo(m_cols, sView);
	
	size_t i, n = m_groups.size();
	for(i = 0; i < n; i++)
		if( !m_groups[i] )
		{
			m_groups.resize(i);
			break;
		}

	UpdateColumnsLayout(false);
}

void CListItem::SetCurView(LPCSTR curview)
{
	size_t i, n = m_views.size();
	for(i = 0; i < n; i++)
	{
		tString sView; m_pRoot->LocalizeStr(sView, m_views[i].m_sViewName.data());
		if( sView == curview )
			break;
	}

	if( (m_curview != i) && (m_curview != -1 || i == n) )
	{
		tString sData; SaveColumnInfo(m_cols, sData);
		if( i != n )
			m_views[m_curview].m_sViewData = sData.c_str();
		else
		{
			cGridViewTemplate& data = m_views.push_back();
			data.m_sViewName = curview;
			data.m_sViewData = sData.c_str();
		}
	}

	m_curview = i;
	SetView(m_views[m_curview].m_sViewData.c_str(cCP_ANSI));
}

//////////////////////////////////////////////////////////////////////

bool CListItem::GetItemFocus(cRowId& row)
{
	row = m_focused;
	return true;
}

void CListItem::SetItemFocus(cRowId& row, bool fEnsureVisible, bool fUnselectAll)
{
	FocusRow(row, fUnselectAll, fEnsureVisible);
}

//////////////////////////////////////////////////////////////////////

cColumn::cColumn()
{
	m_nID = 0;
	m_pList = NULL;
	m_parent = NULL;
	m_filter = NULL;
	m_pItem = NULL;
	m_pFltCtx = NULL;
	m_nExpSize = m_nGrpPos = m_nCurCol = m_nFlags = m_nScale = m_nLevel = 0;
	m_Size.cx = m_Size.cy = m_Offset.x = m_Offset.y = 0;
}

cColumn::~cColumn()
{
	size_t i, n = m_vChilds.size();
	for(i = 0; i < n; i++)
		delete m_vChilds[i];

	if( m_pFltCtx )
		delete (cFilterCtx*)m_pFltCtx;
}

cColumn* cColumn::AddColumn(CGridItem* pItem, LPCSTR caption)
{
	cColumn* col = new cColumn;

	if( caption )
		col->m_sText = caption;

	if( pItem->m_nFlags & AUTOSIZE_X )
		col->m_nScale = -1;
	else if( pItem->m_nFlags & AUTOSIZE_PARENT_X )
	{
		col->m_nScale = pItem->m_ptResizeScale.x;
		if( col->m_nScale > 100 )
			col->m_nScale = 100;
	}
	else
	{
		col->m_Size.cx = pItem->m_szSizeMin.cx;
	}

	if( !col->m_Size.cx )
		col->m_Size.cx = LIST_MIN_COL_SIZE;

	if( pItem->m_nState & ISTATE_HIDDENEX ) 
		col->m_nFlags |= cColumn::fHided;

	if( pItem->m_nState & ISTATE_SELECTED ) 
		col->m_nFlags |= cColumn::fChecked;

	if( pItem->m_nState & ISTATE_DEFAULT ) 
		col->m_nFlags |= cColumn::fIndex;

	col->m_Offset.y = m_Offset.y + LIST_COL_DEFSIZE;

	col->m_pItem = (CGridItem*)pItem;
	col->m_parent = this;
	col->m_pList = m_pList;
	col->m_nLevel = m_nLevel + 1;

	pItem->m_pUserData = col;
	pItem->m_type |= CGridItem::tColumn;

	m_vChilds.push_back(col);
	return col;
}

cColumn* cColumn::AddColumn(LPCSTR section, LPCSTR caption, tDWORD nID)
{
	CGridItem* pItem = NULL;
	if( !m_pItem->GetGridItem(pItem, section) )
		return NULL;

	pItem->m_strItemId = __itoa10(m_vChilds.size());

	cColumn* col = AddColumn(pItem, caption);
	col->m_nID = nID;
	m_pList->UpdateColumnsLayout(false);
	return col;
}

cColumn* cColumn::Find(LPCSTR sId)
{
	if( m_pItem->m_strItemId == sId )
		return this;

	size_t i, n = m_vChilds.size();
	for(i = 0; i < n; i++)
		if( cColumn* col = m_vChilds[i]->Find(sId) )
			return col;

	return NULL;
}

void cColumn::Expand(bool expanded)
{
	if( expanded )
	{
		m_nFlags |= fExpanded;
		m_nExpSize = m_Size.cx;
	}
	else
	{
		if( m_nExpSize )
			m_Size.cx = m_nExpSize;
		m_nFlags &= ~fExpanded;
	}

	m_pList->UpdateColumnsLayout(false);
}

void cColumn::Hide(bool hide)
{
	if( hide )
		m_nFlags |= fHided;
	else
		m_nFlags &= ~fHided;

	m_pList->UpdateColumnsLayout(true);
}

void cColumn::Destroy()
{
	if( !m_parent )
		return;

	vector<cColumn*>& v = m_parent->m_vChilds;
	for(size_t i = 0; v.size(); i++)
		if( v[i] == this )
		{
			v.erase(v.begin()+i);
			break;
		}

	m_pList->UpdateColumnsLayout(false);
	delete this;
}

long cColumn::MaxLevel()
{
	if( !(m_nFlags & fExpanded) || (m_nFlags & fHided) )
		return 0;

	long nSubLev = 0;
	for(size_t i = 0; i < m_vChilds.size(); i++)
		nSubLev = max(m_vChilds[i]->MaxLevel()+1, nSubLev);

	return nSubLev;
}

void cColumn::InitSizeX(tDWORD sz)
{
	if( !(m_nFlags & fExpanded) )
		return;

	long nFixedSize = 0, nScaleSize = 0;

	size_t i, n = m_vChilds.size();
	for(i = 0; i < n; i++)
	{																			
		cColumn& col = *m_vChilds[i];
		if( !col.m_nScale && !(col.m_nFlags & fHided) )
			nFixedSize += col.m_Size.cx;
	}

	if( nFixedSize > (long)sz )
		sz = nFixedSize + sz/2;

	sz -= nFixedSize;
	for(i = 0; i < n; i++)
	{
		cColumn& col = *m_vChilds[i];
		if( col.m_nFlags & fHided )
			continue;

		if( col.m_nScale )
		{
			if( col.m_nScale == -1 )
				col.m_Size.cx = (sz) / n;
			else
				col.m_Size.cx = (sz * col.m_nScale) / 100;

			if( col.m_Size.cx < LIST_MIN_COL_SIZE )
				col.m_Size.cx = LIST_MIN_COL_SIZE;

			nScaleSize += col.m_Size.cx;
		}

		col.InitSizeX(col.m_Size.cx);
	}

	if( n )
		m_Size.cx = nScaleSize + nFixedSize;
	else  if( !m_parent )
		m_Size.cx = sz;
}

void cColumn::InitSizeY(tDWORD maxLevel)
{
	size_t i, n = m_vChilds.size();

	m_Size.cy = (maxLevel - m_nLevel+1)*LIST_COL_DEFSIZE;
	if( !m_nLevel )
		m_Size.cy -= LIST_COL_DEFSIZE;
	else if( n && (m_nFlags & fExpanded) )
		m_Size.cy = LIST_COL_DEFSIZE;

	for(i = 0; i < n; i++)
		m_vChilds[i]->InitSizeY(maxLevel);
}

void cColumn::Resize(long& diff)
{
	size_t i, n = m_vChilds.size();
	long tdiff = diff;

	for(i = 0; i < n; i++)
	{
		size_t pos = (i + m_nCurCol) % n;

		cColumn& col = *m_vChilds[pos];
		if( col.m_nFlags & fHided ) 
			continue;

		if( col.m_nScale )
		{
			long part;
			if( col.m_nScale == -1 )
				part = (long)tdiff / (long)n;
			else
				part = (tdiff * (long)col.m_nScale) / 100;

			if( part == 0 )
				part = diff;

			if( part + col.m_Size.cx < 0 )
				part = -col.m_Size.cx;

			col.ChangeSize(part);
			if( !(diff -= part) )
				break;
		}

//		col.Resize(diff);
	}
	m_nCurCol++;
}

void cColumn::ChangeSize(long diff)
{
	size_t n = m_vChilds.size();

	if( n && (m_nFlags & fExpanded) )
	{
		while( diff )
		{
			long tmp = diff;
			Resize(diff);
			if( tmp == diff )
			{
				if( n )
				{
					if( m_parent )
						m_vChilds[0]->ChangeSize(diff);
					return;
				}
				break;
			}
		}
	}

	if( m_Size.cx + diff < LIST_MIN_COL_SIZE )
		diff = LIST_MIN_COL_SIZE-m_Size.cx;

	m_Size.cx += diff;

	for(cColumn* parent = m_parent; parent; parent = parent->m_parent)
	{
		parent->m_Size.cx += diff;
		if( !parent->m_parent )
			parent->RecalcOffset();
	}
}

void cColumn::RecalcOffset()
{
	if( !(m_nFlags & fExpanded) )
		return;

	long nOffset = m_Offset.x;
	size_t i, n = m_vChilds.size();
	for(i = 0; i < n; i++)
	{
		cColumn& col = *m_vChilds[i];
		if( col.m_nFlags & fHided ) 
			continue;

		col.m_Offset.x = nOffset;
		col.RecalcOffset();
		nOffset += col.m_Size.cx;
	}

	if( n )
		m_Size.cx = nOffset - m_Offset.x;
}

void cColumn::UpdateState()
{
	if( !m_pList )
		return;

	size_t i, n = m_vChilds.size();
	for(i = 0; i < n; i++)
		m_vChilds[i]->UpdateState();

	m_filter = NULL;
	m_nFlags &= ~(fGrouped|fFiltered);

	n = m_pList->m_groups.size();
	for(i = 0; i < n; i++)
		if( m_pList->m_groups[i] == m_pItem )
		{
			m_nFlags |= fGrouped;
			m_nGrpPos = i;
			break;
		}

	n = m_pList->m_filters.size();
	for(i = 0; i < n; i++)
		if( m_pList->m_filters[i].m_id == (tPTR)this )
		{
			m_nFlags |= fFiltered;
			m_filter = m_pList->m_filters[i].m_filter;
			break;
		}
}

cColumn* cColumn::HitTest(long x, long y, long& flags, bool bBorder)
{
	if( m_nFlags & fHided )
		return  NULL;

	if( m_nLevel && y >= m_Offset.y && y < m_Offset.y + m_Size.cy )
	{
		flags |= hfInner;
		if( m_nFlags & (fAutoFlt|fCustomFlt) )
			if( x > m_Offset.x && x - m_Offset.x < 11 && y - m_Offset.y < 11 )
				flags |= hfFilter;
	}
	else if( m_nFlags & fExpanded )
	{
		size_t i, n = m_vChilds.size();
		for(i = n; i > 0; i--)
			if( cColumn* col = m_vChilds[i-1]->HitTest(x, y, flags, bBorder) )
			{
				if( !m_Size.cy )
					flags &= ~hfInner;
				return col;
			}
	}

	bool bThis = false;
	long offs = m_Offset.x + m_Size.cx;

	if( m_Offset.x < x && x < offs )
	{
		if( x >= m_Offset.x + m_Size.cx/2 )
			flags |= hfRight;
		else
			flags |= hfLeft;
		bThis = true;
	}

	if( bBorder && x - 5 < offs && offs < x + 5 )
	{
		flags |= hfBorder;
		return this;
	}

	return bThis ? this : NULL;
}

//////////////////////////////////////////////////////////////////////

cRowNode::cRowNode(cRowNode* parent, CItemBase* pHeader, tDWORD nIdx, tDWORD nFlags, cVariant* val) :
	m_parent(parent), m_ppos(0), m_flags(nFlags), m_bExpanded(false), m_group(0),
	m_nIndent(0), m_nCount(0), m_nSelItems(0), m_stree(0), m_nListState(0), m_nSortState(0),
	m_nOffset(0), m_nAllSize(0), m_pHeader(pHeader), m_pList(NULL), m_sink(NULL)
{
	m_nIdx.m_idx = nIdx;
	m_nIdx.m_sel = 0;

	m_nRowSize = LIST_ROW_DEFSIZE;
	m_nNodeSize = m_parent ? LIST_ROW_DEFSIZE : 0;
	m_level = m_parent ? m_parent->m_level+1 : 0;

	if( val )
		m_val = *val;

	if( !m_parent )
		return;

	m_pList = m_parent->m_pList;
	m_nListState = m_pList->m_nListState;
	m_nSortState = m_pList->m_nSortState;

	if( m_level < m_pList->m_groups.size() )
		m_group = m_pList->m_groups[m_level];

	if( m_flags & fSkinGroup )
	{
		m_pList->InitGroupItem();
		m_pHeader = m_pList->m_groupitem;
	}

	if( m_pHeader && (m_pList->m_flags & CListItem::fUseSink) )
		m_flags |= fSkinGroup;

	CGridItem* templ = (CGridItem*)m_pHeader;
	if( templ && templ->m_template )
		templ = templ->m_template;

	if( templ && templ->m_szSizeMin.cy )
		m_nNodeSize = templ->m_szSizeMin.cy;

	m_nIndent = m_parent->m_nIndent;
	if( !(m_parent->m_flags & fNoPlus) )
		m_nIndent++;

	m_parent->InsertNode(this, false);

	if( m_flags & fNoPlus )
		Expand(true);
}

cRowNode::~cRowNode()
{
	ClearData();

	cRowId& fr = m_pList->m_focused;
	if( fr.m_node == this )
		fr.m_node = m_pList;

	cRowId& hr = m_pList->m_hotrow;
	if( hr.m_node == this )
		hr.m_node = m_parent;

	cRowId& tr = m_pList->m_toprow;
	if( tr.m_node == this )
		tr.m_node = m_parent;
}

void cRowNode::Destroy()
{
	cRowId& fr = m_pList->m_focused;
	if( fr.m_node == this )
	{
		cRowId newfr = fr;
		PrevRow(newfr);

		if( m_nIdx.m_sel )
			m_pList->FocusRow(newfr, true, false);
	}

	ClearData(true);

	if( m_parent )
	{
		ReSize(-(tLONG)m_nNodeSize, true);

		for(size_t i = 0; i < m_parent->m_nodes.size(); i++)
		{
			cRowNode* _node = m_parent->m_nodes[i];
			if( _node->m_nIdx.m_idx > m_nIdx.m_idx )
				_node->m_nIdx.m_idx--;

			if( i > m_ppos )
				_node->m_ppos--;
		}

		m_parent->m_nodes.erase(m_parent->m_nodes.begin()+m_ppos);
	}

	CListItem* pList = m_pList;
	delete this;
}

const cVariant& cRowNode::GetSortValue(cVariant& defval)
{
	cRowId row(this, 0);
	if( row.IsCustom() || GetGroupItem() )
		return m_val;

	cNode* sort = m_pList->m_sorting;
	if( !sort || sort == SORT_BY_INDEX )
		return m_val;

	CStructData sd(m_pList->GetRowData(row));
	cNodeExecCtx ctx(m_pList->m_pRoot, m_pList, &sd, NULL);

	m_pList->m_sorting->Exec(defval, ctx);
	return defval;
}

void cRowNode::InsertNode(cRowNode* node, bool bRefresh)
{
	tLONG sortState = m_pList->GetSortState();

	node->m_ppos = m_nodes.size();
	if( sortState )
	{
		cVariant val1, val2;
		const cVariant& val = node->GetSortValue(val1);

		bool bUp = !(sortState & cColumn::fSortDown);
		for(size_t i = 0; i < m_nodes.size(); i++)
			if( (val > m_nodes[i]->GetSortValue(val2)) == bUp )
			{
				node->m_ppos = i;
				break;
			}
	}

	m_nodes.insert(m_nodes.begin()+node->m_ppos, node);

	for(size_t i = node->m_ppos+1; i < m_nodes.size(); i++)
		m_nodes[i]->m_ppos++;

	tDWORD nOffset;
	if( node->m_ppos )
	{
		cRowNode* prev = m_nodes[node->m_ppos-1];
		nOffset = prev->m_nOffset + prev->Size();
	}
	else
		nOffset = m_nOffset + m_nNodeSize;

	if( bRefresh )
	{
		node->Shift(nOffset-node->m_nOffset);

		tLONG size = node->Size();
		for(size_t i = node->m_ppos+1; i < m_nodes.size(); i++)
			m_nodes[i]->Shift(size);
	}
	else
	{
		node->m_nOffset = nOffset;
		node->ReSize(node->m_nNodeSize, true);
	}
}

void cRowNode::ClearData(bool bNodes)
{
	size_t i, n;
	if( bNodes )
	{
		i, n = m_nodes.size();
		for(i = 0; i < n; i++)
			delete m_nodes[i];

		m_nodes.clear();
		if( m_nAllSize )
			ReSize(m_nNodeSize-m_nAllSize, false);

		if( m_sink )
		{
			m_sink->Destroy();
			m_sink = NULL;
		}
	}
	else
	{
		ReSize(-(tLONG)m_nRowSize*m_nCount, false);
	}

	n = m_rows.size();
	for(i = 0; i < n; i++)
		delete[] m_rows[i];

	m_rows.clear();
	m_nCount = 0;

	tDWORD selNum = GetSelectedCount();
	if( m_nIdx.m_sel )
		selNum--;

	RemoveSelection(selNum);

	m_cache.clear();
	delete m_stree;
	m_stree = NULL;
}

inline cRowNode::cRowIdx* cRowNode::GetBlock(tDWORD nBlock)
{
	if( nBlock >= m_rows.size() )
		m_rows.resize(nBlock+1);

	cRowIdx*& block = m_rows[nBlock];
	if( !block )
	{
		block = new cRowIdx[BLOCK_SIZE];
		memset(block, 0, BLOCK_SIZE*sizeof(cRowIdx));
	}
	return block;
}

inline cRowNode::cRowIdx& cRowNode::GetIdx(tDWORD nPos)
{
	if( !nPos )
		return m_nIdx;

	tDWORD i = 0, csize = m_cache.size();
	for(tSortCache::iterator it = m_cache.begin(); i < csize; ++i, ++it)
	{
		tDWORD pos = it->m_pos + i;
		if( nPos == pos )
			return it->m_idx;

		if( nPos < pos )
		{
			if( nPos - i > m_nCount - csize )
				return it->m_idx;

			break;
		}
	}

	nPos -= (i + 1);

	return GetBlock(nPos / BLOCK_SIZE)[nPos % BLOCK_SIZE];
}

bool cRowNode::DelRows(tLONG nCount, tIdx* nIdx)
{
	tDWORD diff = 0;
	for(tDWORD i = 1; i <= m_nCount; i++)
	{
		cRowIdx& idx = GetIdx(i);

		int idxDel = 0;
		for(tLONG j = 0; j < nCount; j++)
		{
			tIdx pos = nIdx ? nIdx[j] : (m_nCount-j);
			if( idx.m_idx == pos )
			{
				diff++;
				idxDel = -1;
				if( idx.m_sel )
					Select(i, false);
				break;
			}

			if( (tIdx)idx.m_idx > pos )
				idxDel++;
		}

		if( idxDel > 0 )
			idx.m_idx -= idxDel;

		if( idxDel != -1 && diff )
			GetIdx(i-diff) = idx;
	}

	m_nCount -= nCount;
	ReSize(-(tLONG)m_nRowSize*nCount, false);

	cRowId& fr = m_pList->m_focused;
	if( fr.m_node == this && fr.m_pos > m_nCount )
		m_pList->FocusRow(TOR(cRowId,(this,m_nCount)), false, false);

	fr.Select(true);
	return true;
}

cRowNode* cRowNode::AddNode(tIdx nIdx, tDWORD flags, CItemBase* pHeader, cVariant* val)
{
	return new cRowNode(this, pHeader, nIdx, flags, val);
}

bool cRowNode::AddRows(tLONG nCount, tIdx* nIdx, cRowId *row)
{
	tDWORD newCount = m_nCount + nCount;

	if( nIdx != ADDROWS_NO_IDX )
	{
		if( !m_cache.empty() )
			FlushSortedCache();

		tDWORD i, c = m_nCount;
		for(i = 0; c < newCount; c++, i++)
		{
			cRowIdx& idx = GetBlock(c / BLOCK_SIZE)[c % BLOCK_SIZE];
			idx.m_idx = nIdx ? nIdx[i] : c;
			idx.m_sel = 0;
		}
	}

	m_nCount = newCount;
	ReSize(m_nRowSize*nCount, false);

	if( row )
	{
		row->m_node = this;
		row->m_pos = m_nCount;
	}

	return true;
}

bool cRowNode::AddRow(cRowIdx& idx, cNodeExecCtx& ctx, bool focus)
{
	if( !m_group )
	{
		if( m_pList->m_pSink )
			if( !((CListItemSink*)m_pList->m_pSink)->OnAddRow(this, idx.m_idx) )
				return false;

		if( !m_pList->m_sorting || !m_bExpanded )
		{
			AddRows(1, (tIdx*)&idx);
			if( idx.m_sel )
				Select(m_nCount, true);
			if( !m_bExpanded )
				m_nListState = 0;
		}
		else
			AddSortedRow(idx, ctx);

		if( focus )
		{
			cRowId& fr = m_pList->m_focused;
			fr.m_node = this; fr.m_pos = 1;
		}
		return true;
	}

	cVariant val;
	m_group->m_condition->Exec(val, ctx);

	cRowNode* node = NULL;
	size_t i, n = m_nodes.size();
	for(i = 0; i < n; i++)
		if( m_nodes[i]->m_val == val )
		{
			node = m_nodes[i];
			break;
		}

	if( !node && !(m_group->m_type & CGridItem::tFilter) )
	{
		PRAutoLocker lock(m_pList->m_Locker);
		node = AddNode(0, 0, m_group->m_template, &val);

		bool bToExpand = false;

		n = m_group->m_vExpanded.size();
		if( n )
		{
			tValArr vValue;
			node->GetGroupValue(vValue);

			for(i = 0; i < n; i += vValue.size())
				if( !memcmp(&*vValue.begin(),
						&*m_group->m_vExpanded.begin() + i,
						vValue.size()*sizeof(tQWORD)) )
				{
					bToExpand = true;
					break;
				}
		}

		if( !bToExpand && !n )
			if( cNode* expand = m_group->m_template ? m_group->m_template->m_expand : NULL )
			{
				m_group->m_template->m_node = node;
				expand->Exec(val, ctx);
				bToExpand = !!val;
			}

		if( bToExpand )
			node->Expand(true);
	}

	return node ? node->AddRow(idx, ctx, focus) : false;
}

void cRowNode::AddSortedRow(cRowIdx& idx, cNodeExecCtx& ctx)
{
	cNode* sort = m_pList->m_sorting;

	cVariant val;
	if( sort == SORT_BY_INDEX )
		val = (tDWORD)idx.m_idx;
	else
		sort->Exec(val, ctx);

	cSortTree **ptree = &m_stree;
	tDWORD csize = (tDWORD)m_cache.size();
	tDWORD i, f = 1, t = m_nCount-csize+1, pos;

	bool bUp = !(m_pList->m_sortcol->m_nFlags & cColumn::fSortDown);
	while( 1 )
	{
		if( !*ptree )
		{
			pos = (t+f)/2;
			*ptree = new cSortTree(pos);

			tIdx idx = GetBlock((pos-1) / BLOCK_SIZE)[(pos-1) % BLOCK_SIZE].m_idx;

			ctx.m_data->m_ser = m_pList->GetRowData(this, idx, false);
			if( sort == SORT_BY_INDEX )
				(*ptree)->m_val = (tDWORD)idx;
			else
				sort->Exec((*ptree)->m_val, ctx);
		}
		else
			pos = (*ptree)->m_pos;

		cSortTree* tree = *ptree;
		int res = val.Compare(tree->m_val);
		if( bUp )
			res = -res;

		if( !res || pos == t && res > 0 || pos == f && res < 0 )
			break;

		if( res < 0 )
			t = pos, ptree = &tree->m_right;
		else
		{
			if( f == pos ) pos++;
			f = pos, ptree = &tree->m_left;
		}
	}

	tSortCache::iterator it = m_cache.begin();
	for(i = 0; i < csize; ++i, ++it)
	{
		cSortCache& e = *it;
		if( e.m_pos < pos )
			continue;

		if( e.m_pos == pos && (bUp ? val < e.m_val : val > e.m_val) )
			continue;

		break;
	}

	cSortCache elem;
	elem.m_pos = pos;
	elem.m_val = val;
	elem.m_idx.m_idx = idx.m_idx;
	elem.m_idx.m_sel = 0;
	m_cache.insert(it, elem);

	AddRows(1, ADDROWS_NO_IDX);

	tLONG real_pos = (tLONG)(pos + i);

	m_pList->OnRowInserted(this, real_pos);

	if( m_nSelItems > real_pos )
		m_nSelItems++;

	if( idx.m_sel )
		Select(real_pos, true);

	if( m_cache.size() == BLOCK_SIZE )
		FlushSortedCache();
}

void cRowNode::FlushSortedCache()
{
	tDWORD i = 0, k = 0, c = 0, b, n, num = m_cache.size();
	if( !num )
		return;

	cRowIdx* block = new cRowIdx[BLOCK_SIZE];
	tSortCache::iterator cpos = m_cache.begin();
	b = n = (cpos->m_pos-1) / BLOCK_SIZE;
	cRowIdx* tocopy = GetBlock(b);

	while(1)
	{
		tDWORD pos = n * BLOCK_SIZE + i + 1;

		if( c < num && pos == (cpos->m_pos + c) )
		{
			block[i++] = cpos->m_idx;
			++cpos; ++c;
		}
		else
		{
			block[i++] = tocopy[k++];
			if( k == BLOCK_SIZE )
			{
				tocopy = m_rows[++b];
				k = 0;
			}
		}

		if( i == BLOCK_SIZE )
		{
			cRowIdx* tmp = block;
			block = m_rows[n];
			m_rows[n] = tmp;

			GetBlock(++n);
			i = 0;

			if( c == num && num == BLOCK_SIZE )
			{
				m_rows.insert(m_rows.begin()+n, block);
				break;
			}
		}

		if( pos == m_nCount )
		{
			m_rows.insert(m_rows.begin()+n, block);
			break;
		}
	}

	m_cache.clear();

	delete m_stree;
	m_stree = NULL;
}

bool cRowNode::RefreshView(bool bSync, cRefreshCtx& ctx)
{
	CStructData sd(NULL);
	cNodeExecCtx ectx(m_pList->m_pRoot, m_pList, &sd, NULL);

	tIdx focusedIdx = -1;
	if( ctx.m_focused && m_pList->m_sorting )
	{
		cRowId& fr = m_pList->m_focused;

		focusedIdx = ctx.m_focused->m_idx;
		sd.m_ser = m_pList->GetRowData(fr.m_node, focusedIdx, fr.IsNode());
		AddRow(*ctx.m_focused, ectx, true);
	}

	CListItemSink* pSink = (CListItemSink*)m_pList->m_pSink;

	tDWORD npos = 0; bool bBreak = false;
	for(size_t i = 0; i < ctx.m_rows.size() && !bBreak && (m_nListState == m_pList->m_nCurState); i++)
	{
		cRowIdx* v = ctx.m_rows[i];
		tDWORD k, num = ctx.m_nums[i];
		if( !num )
			continue;

		std::sort(v, v+num);

		tIdx minIdx = v[0].m_idx;
		pSink->PrepareRows(minIdx, BLOCK_SIZE);

		for(k = 0; k < num && npos != ctx.m_count; k++, npos++)
		{
			cRowIdx& idx = v[k];
			if( (tIdx)idx.m_idx > minIdx + BLOCK_SIZE )
			{
				minIdx = idx.m_idx;
				pSink->PrepareRows(minIdx, BLOCK_SIZE);
			}

			if( focusedIdx == idx.m_idx )
				continue;

			sd.m_ser = m_pList->GetRowData(this, idx.m_idx, false);
			AddRow(idx, ectx);
		}

		if( !bSync && !(i % 10) )
			bBreak = !m_pList->UpdateLayout(!!i);
	}

	FlushSortedCache();
	return !bBreak;
}

void cRowNode::CollectRows(cRefreshCtx& ctx, bool bRecursive)
{
	FlushSortedCache();

	cRowId& fr = m_pList->m_focused;
	if( fr.m_node == this && fr.m_pos )
		ctx.m_focused = &fr.Idx();

	tDWORD size = m_nCount;
	for(size_t i = 0; i < m_rows.size(); i++)
	{
		ctx.m_rows.push_back(m_rows[i]);
		ctx.m_nums.push_back(size < BLOCK_SIZE ? size : BLOCK_SIZE);

		size = size > BLOCK_SIZE ? size - BLOCK_SIZE : 0;
	}

	ctx.m_count += m_nCount;

	m_rows.clear();

	if( bRecursive )
		for(size_t i = 0; i < m_nodes.size(); i++)
			m_nodes[i]->CollectRows(ctx, true);
}

void cRowNode::RefreshView(bool bSync)
{
	if( !m_bExpanded )
		return;

	bool bGroupChanged = false;
	size_t groups = m_pList->m_groups.size();
	if( m_level < groups )
	{
		if( bGroupChanged = (m_group != m_pList->m_groups[m_level]) )
			m_group = m_pList->m_groups[m_level];
	}
	else if( m_level == groups && (groups || m_pList->m_nTimer) )
	{
		if( bGroupChanged = !!m_nodes.size() )
			m_group = NULL;
	}

	if( m_sink )
		bGroupChanged = false;

	size_t i, n;
	if( !bGroupChanged )
	{
		tLONG sortState = m_pList->GetSortState();
		if( m_nSortState != sortState )
		{
			m_nSortState = sortState;

			tNodes nodes(m_nodes);
			m_nodes.clear();

			for(size_t i = 0; i < nodes.size(); i++)
				InsertNode(nodes[i], true);

			m_pList->UpdateLayout(true);
		}

		n = m_nodes.size();
		for(i = 0; i < n; i++)
			m_nodes[i]->RefreshView(bSync);

		if( m_nListState == m_pList->m_nCurState )
			return;

		if( !m_nCount )
		{
			m_nListState = m_pList->m_nCurState;
			return;
		}
	}

	cRefreshCtx ctx;
	CollectRows(ctx, bGroupChanged);

	do
	{
		m_nListState = m_pList->m_nCurState;
		ClearData(bGroupChanged);

		if( !RefreshView(bSync, ctx) )
			break;
	}
	while( m_nListState != m_pList->m_nCurState );

	m_nSortState = m_pList->GetSortState();

	n = ctx.m_rows.size();
	for(i = 0; i < n; i++)
		delete[] ctx.m_rows[i];

	m_pList->UpdateLayout(true);
}

void cRowNode::GetGroupValue(tValArr& vValue)
{
	if( !m_parent )
		return;

	vValue.push_back(m_val.ToQWORD());
	m_parent->GetGroupValue(vValue);
}

void cRowNode::SaveExpanded(CGridItem* pItem)
{
//	if( m_level > 2 )
//		return;

	if( this == m_pList )
		pItem->m_vExpanded.clear();

	if( m_parent && m_bExpanded && pItem == m_parent->m_group )
		GetGroupValue(pItem->m_vExpanded);

	size_t i, n = m_nodes.size();
	for(i = 0; i < n; i++)
	{
		m_nodes[i]->SaveExpanded(pItem);
	}
}

bool cRowNode::Expand(bool bExpand)
{
	if( !m_parent )
		return false;

	if( m_bExpanded == bExpand )
		return false;

	m_bExpanded = bExpand;

	int nSize = m_nAllSize - m_nNodeSize;
	if( !bExpand ) nSize = -nSize;

	m_parent->ReSize(nSize, false);

	tNodes& v = m_parent->m_nodes;
	for(size_t i = m_ppos+1; i < v.size(); i++)
		v[i]->Shift(nSize);

	m_pList->OnExpanded(this, bExpand);
	return true;
}

void cRowNode::ReSize(int nSize, bool bNode)
{
	ChSize(nSize, bNode);
	if( m_parent && (bNode || m_bExpanded) )
		m_parent->ReSize(nSize, false);
}

void cRowNode::ChSize(int nSize, bool bNode)
{
	m_nAllSize += nSize;

	if( !(m_parent && (bNode || m_bExpanded)) )
		return;

	tNodes& v = m_parent->m_nodes;
	for(size_t i = m_ppos+1; i < v.size(); i++)
		v[i]->Shift(nSize);
}

void cRowNode::Shift(int nShift)
{
	m_nOffset += nShift;

	for(size_t i = 0; i < m_nodes.size(); i++)
		m_nodes[i]->Shift(nShift);
}

inline tDWORD cRowNode::GetRowPos(tDWORD offset)
{
	if( !m_bExpanded || !m_nRowSize || offset <= m_nOffset + m_nNodeSize )
		return 0;

	long nPos = ((long)m_nOffset + (long)m_nAllSize - (long)offset) / (long)m_nRowSize;
	if( nPos < 0 )
		return m_nCount;

	if( nPos >= (long)m_nCount )
		return -1;

	return m_nCount - nPos;
}

bool cRowNode::GetRow(tDWORD offset, cRowId& id)
{
	if( m_nOffset > offset || offset > m_nOffset + Size() )
		return false;

	if( m_bExpanded )
	{
		size_t i, n = m_nodes.size();
		for(i = 0; i < n; i++)
			if( m_nodes[i]->GetRow(offset, id) )
				return true;
	}

	id.m_node = this;
	id.m_pos = GetRowPos(offset);
	return true;
}

bool cRowNode::NextRow(cRowId& id)
{
	cRowNode* node = id.m_node;
	if( !node )
		id.m_node = node = this;

	if( node->m_bExpanded )
	{
		if( !id.m_pos && node->m_nodes.size() )
		{
			id.m_node = node->m_nodes[0]; id.m_pos = 0;
			return true;
		}

		if( id.m_pos < node->m_nCount )
		{
			id.m_pos++;
			return true;
		}
	}
	return NextParentRow(id);
}

bool cRowNode::NextParentRow(cRowId& id)
{
	cRowNode* node = id.m_node;
	if( !node->m_parent )
		return false;

	if( node->m_ppos == node->m_parent->m_nodes.size()-1 )
	{
		if( !node->m_parent->m_bExpanded )
			return false;

		if( !node->m_parent->m_nCount )
		{
			id.m_node = node->m_parent;
			if( NextParentRow(id) )
				return true;

			id.m_node = node;
			return false;
		}

		id.m_node = node->m_parent;
		id.m_pos = 1;
		return true;
	}

	cRowNode* next = node->m_parent->m_nodes[node->m_ppos+1];
	return next->FirstRow(id);
}

bool cRowNode::PrevRow(cRowId& id)
{
	cRowNode* node = id.m_node;

	if( id.m_pos > 0 )
	{
		if( id.m_pos == 1 && node->m_nodes.size() )
			return node->m_nodes[node->m_nodes.size()-1]->LastRow(id);

		if( !--id.m_pos && !node->m_parent )
			id.m_pos = 1;

		return true;
	}

	if( !node->m_parent )
		return false;

	if( node->m_ppos == 0 )
	{
		if( !node->m_parent->m_parent )
			return false;

		id.m_node = node->m_parent; id.m_pos = 0;
		return true;
	}

	cRowNode* prev = node->m_parent->m_nodes[node->m_ppos-1];
	return prev->LastRow(id);
}

bool cRowNode::FirstRow(cRowId& id)
{
	if( !m_parent && m_nodes.size() )
		return m_nodes[0]->FirstRow(id);

	id.m_node = this;
	id.m_pos = (m_parent || !m_nCount) ? 0 : 1;
	return true;
}

bool cRowNode::LastRow(cRowId& id)
{
	if( !m_bExpanded || m_nCount || !m_nodes.size() )
	{
		id.m_node = this;
		id.m_pos = m_bExpanded ? m_nCount : 0;
		return true;
	}

	return m_nodes[m_nodes.size()-1]->LastRow(id);
}

bool cRowNode::FindRow(tIdx nIdx, cRowId& row)
{
	tDWORD i, n;
	for(i = 0; i < m_nCount+1; i++)
		if( nIdx == GetIdx(i).m_idx )
		{
			row.m_node = this;
			row.m_pos = i;
			return true;
		}

	n = m_nodes.size();
	for(i = 0; i < n; i++)
		if( m_nodes[i]->FindRow(nIdx, row) )
			return true;

	return false;
}

size_t cRowNode::GetSelectedCount()
{
	return m_nSelItems > 0 ? 1 : -m_nSelItems;
}

tIdx cRowNode::GetSelectedItem(cRowId &nCookie)
{
	while( NextRow(nCookie) )
	{
		cRowIdx& idx = nCookie.Idx();
		if( idx.m_sel )
			return idx.m_idx;
	}
	return -1;
}

bool cRowNode::IsItemSelected(tIdx nItem)
{
	return false;
}

void cRowNode::Select(tDWORD nPos, bool bSelect)
{
	cRowIdx* idx = nPos != -1 ? &GetIdx(nPos) : NULL;

	if( nPos != -1 )
	{
		if( idx->m_sel == bSelect )
			return;
		idx->m_sel = bSelect;
	}

	if( bSelect )
	{
		if( !m_nSelItems )
			m_nSelItems = (nPos != -1 ? nPos+1 : nPos);
		else if( m_nSelItems > 0 )
			m_nSelItems = -2;
		else
			m_nSelItems--;

		if( nPos != -1 && m_pList->m_pSink )
			m_pList->SendListEvent(GRID_EVENT_ONSELECT, CListItemSink::leSelectionChanged);
	}
	else
	{
		if( m_nSelItems > 0 )
			m_nSelItems = 0;
		else if( m_nSelItems != 0 )
			m_nSelItems++;
	}

	if( m_parent )
		m_parent->Select(-1, bSelect);
}

void cRowNode::ClearSelection()
{
	if( !m_nSelItems )
		return;

	size_t i, n = m_nodes.size();
	for(i = 0; i < n; i++)
		m_nodes[i]->ClearSelection();

	if( !m_nSelItems )
		return;

	if( m_nSelItems > 0 )
	{
		Select(m_nSelItems-1, false);
		return;
	}

	for(tDWORD i = 0; i < m_nCount+1; i++)
		Select(i, false);
}

void cRowNode::RemoveSelection(tDWORD num)
{
	if( !num )
		return;

	if( m_nSelItems > 0 )
		m_nSelItems = 0;
	else if( m_nSelItems != 0 )
		m_nSelItems += num;

	if( m_parent )
		m_parent->RemoveSelection(num);
}

void cRowNode::SelectRange(tDWORD nFrom, tDWORD nTo)
{
	if( m_nOffset >= nTo || (m_nOffset + Size()) < nFrom )
		return;

	size_t i, n = m_nodes.size();
	for(i = 0; i < n; i++)
		m_nodes[i]->SelectRange(nFrom, nTo);

	tDWORD r1 = GetRowPos(nFrom);
	tDWORD r2 = GetRowPos(nTo);

	if( r1 == -1 ) r1 = 1;
	if( r2 == -1 ) r2 = 0;

	for(i = r1; i <= r2; i++)
		Select(i, true);
}

tDWORD cRowNode::GetOffset(tDWORD nRow)
{
	if( !nRow )
		return m_nOffset;

	return m_nOffset + (m_nAllSize - m_nRowSize*(m_nCount-nRow+1));
}

tDWORD cRowNode::GetSize(tDWORD nRow)
{
	if( !nRow )
		return m_nNodeSize;

	return m_nRowSize;
}

bool cRowNode::GetIdxArr(tDWORD nPos, tIdxArr& pId)
{
	if( m_pList == this )
		return true;

	if( m_parent )
		m_parent->GetIdxArr(0, pId);

	pId.push_back(m_nIdx.m_idx);
	if( nPos )
	{
		cRowId row(this, nPos);
		pId.push_back(row.Idx().m_idx);
	}
	return true;
}

tDWORD cRowNode::Count()
{
	tDWORD ret = m_nCount;

	size_t i, n = m_nodes.size();
	for(i = 0; i < n; i++)
		ret += m_nodes[i]->Count();

	return ret;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

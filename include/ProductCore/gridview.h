// gridview.h: interface for the cDataView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GRIDVIEW_H_)
#define _GRIDVIEW_H_

class  CListItem;
class  CDialogItem;
class  cNode;
class  cNodeExecCtx;
struct cRowNode;

typedef int                 tIdx;
typedef std::vector<tIdx>   tIdxArr;
typedef std::vector<tQWORD> tValArr;

#include "Interpret.h"
#include <Prague/pr_wrappers.h>
#include <Prague/iface/i_threadpool.h>
#include <list>

//////////////////////////////////////////////////////////////////////

struct CGridItem : public CItemBase
{
	CGridItem();
	~CGridItem();

	enum enType
	{
		tUndef     = 0x000,
		tColumn    = 0x001,
		tRow       = 0x002,
		tGroup     = 0x004,
		tFilter    = 0x100,
		fDefGroup  = 0x200,
	};

	CItemBase* CreateChild(LPCSTR strItemId, LPCSTR strItemType);
	cNode * CreateOperand(const char *name, cNode *container, cAlloc *al);

	void OnInited(){ m_nState &= ~ISTATE_INITED; }
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void ApplyStyle(LPCSTR strStyle);

	tDWORD GetFontFlags();
	bool GetUpdateRectEx(RECT& rcFrame, RECT* rcUpdate, CItemBase* pOwner);
	bool GetGridItem(CGridItem*& item, LPCSTR sTemplate);

	bool IsOut(){ return true; }
	bool SetValue(const cVariant &pValue){ m_value = pValue; return true; }
	bool GetValue(cVariant &pValue){ pValue = m_value; return true; }

	void OnChangedData(CItemBase * pItem);

public:
	struct CGroupNode : public cNode
	{
		enum enNode
		{
			eValue = 0x1,
			eCount = 0x2,
			eIsNode = 0x3,
		};

		CGroupNode(CGridItem* owner, enNode type, CListItem* list = NULL)
			: m_type(type), m_owner(owner), m_list(list) {}

		void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
		bool IsConst() const { return false; }

		CListItem*  m_list;
		CGridItem*  m_owner;
		enNode      m_type;
	};

	cNode *      m_condition;
	cNode *      m_sort;
	cNode *      m_expand;
	cNode *      m_filter;
	tDWORD       m_type;
	cVariant     m_value;
	CGridItem*   m_template;
	cRowNode*    m_node;
	tDWORD       m_nodeflags;
	tDWORD       m_colflags;
	long*        m_nWrappedPos;
	tValArr      m_vExpanded;
};

struct cColumn
{
public:
	virtual cColumn* AddColumn(LPCSTR section, LPCSTR caption, tDWORD nID);
	virtual cColumn* Find(LPCSTR sId);
	virtual void Expand(bool expanded);
	virtual void Destroy();

public:
	cColumn();
	~cColumn();

	bool IsFirst(){ return m_Offset.x == 0 && (!m_parent || m_parent->m_vChilds[0] == this); }
	bool IsGroup(){ return m_vChilds.size() && (m_nFlags & fExpanded); }
	bool IsHiden(){ return !!(m_nFlags & fHided); }
	void Resize(long& diff);
	void Hide(bool hide);
	void ChangeSize(long diff);
	long MaxLevel();
	void InitSizeX(tDWORD sz);
	void InitSizeY(tDWORD maxLevel);
	void RecalcOffset();
	void UpdateState();
	cColumn* HitTest(long x, long y, long& flags, bool bBorder=false);
	cColumn* AddColumn(CGridItem* pItem, LPCSTR caption);

	enum enFlags
	{
		fExpanded  = 0x0001,
		fHided     = 0x0002,
		fSortUp    = 0x0010,
		fSortDown  = 0x0020,
		fIndex     = 0x0100,
		fChecked   = 0x0200,
		fGrouped   = 0x0400,
		fFiltered  = 0x0800,
		fAutoFlt   = 0x1000,
		fCustomFlt = 0x2000,
		fFixedPos  = 0x4000,
		fFixedSize = 0x8000,
		fDragable  = 0x10000,
	};

	enum enHitFlags
	{
		hfBorder   = 0x0001,
		hfInner    = 0x0002,
		hfLeft     = 0x0004,
		hfRight    = 0x0008,
		hfFilter   = 0x0010,
	};

	tDWORD       m_nID;
	CListItem*   m_pList;
	cColumn*     m_parent;
	cNode*       m_filter;
	CGridItem*   m_pItem;
	tPTR         m_pFltCtx;
	tDWORD		 m_nFlags;
	tDWORD		 m_nScale;
	tLONG		 m_nLevel;
	tLONG		 m_nGrpPos;
	tDWORD		 m_nCurCol;
	tDWORD		 m_nExpSize;
	POINT		 m_Offset;
	SIZE         m_Size;
	tString      m_sText;
	std::vector<cColumn*> m_vChilds;
};

struct cCellData
{
	tIdx           m_Idx;
	tLONG          m_indent;
	cColumn*       m_col;
	cSerializable* m_data;
	CGridItem*	   m_item;
	RECT*          m_frame;
	RECT*          m_footer;
	bool           m_selected;
	bool           m_template;
	bool           m_group;
};

struct cDataInfo
{
	cDataInfo() : m_nCount(0), m_nTimeStamp(0), m_nLastModified(0){}

	tDWORD m_nCount;
	tQWORD m_nTimeStamp;
	tQWORD m_nLastModified;
};

struct cGridViewTemplate : public cSerializable
{
	DECLARE_IID(cGridViewTemplate, cSerializable, PID_SYSTEM, bguist_cGridViewTemplate);
	
	cStrObj m_sViewName;
	cStrObj m_sViewData;
};

//////////////////////////////////////////////////////////////////////

struct cRowNode
{
public:
	enum enFlags
	{
		fSkinGroup  = 0x01,
		fChecked    = 0x02,
		fCheckGroup = 0x04,
		fNoPlus     = 0x08,
	};

	struct cRowIdx
	{
		unsigned  m_idx : 31;
		unsigned  m_sel : 1;

		bool operator < (const cRowIdx& id) const { return m_idx < id.m_idx; }
	};

	struct cRowId
	{
		cRowId(cRowNode* node=NULL, tDWORD pos=0) :
			m_node(node), m_pos(pos){}

		bool operator == (const cRowId& id) const { return m_node == id.m_node && m_pos == id.m_pos; }

		void     Select(bool bSelect){ m_node->Select(m_pos, bSelect); }
		bool     Next(){ return m_node->NextRow(*this); }
		bool     IsNode() const { return m_pos == 0; }
		bool     IsStaticItem() const { return IsNode() && m_node->m_flags & fChecked; }
		bool     IsCustom() const { return (IsNode() && m_node->m_pHeader && !(m_node->m_flags & cRowNode::fSkinGroup)); }
		bool     IdxArr(tIdxArr& pId){ return m_node->GetIdxArr(m_pos, pId); }
		cRowIdx& Idx(){ return m_node->GetIdx(m_pos); }
		tDWORD   Offset(){ return m_node->GetOffset(m_pos); }
		tDWORD   Size(){ return m_node->GetSize(m_pos); }
		tDWORD   RowCount(){ return m_node->m_nCount; }
		tDWORD   Indent(){ long ind = m_node->m_nIndent; if( IsNode() ) ind--; else if( m_node->m_parent && !m_node->m_pHeader ) ind++; return ind; }
		bool     DeleteItem(){ if( m_pos ){ tIdx idx = Idx().m_idx; return m_node->DelRows(1, &idx); } m_node->Destroy(); return true; }

		cRowNode* m_node;
		tDWORD    m_pos;
	};

	struct cSink
	{
		virtual void  Destroy(){};
		virtual cSer* GetData(){ return NULL; }
	};

	cSink* m_sink;

	typedef std::vector<cRowNode*>  tNodes;
	typedef std::vector<cRowIdx*>   tIndex;

public:
	virtual void Destroy();

	virtual cRowNode* AddNode(tIdx nIdx, tDWORD flags = 0, CItemBase* pHeader = NULL, cVariant* val = NULL);
	virtual bool AddRows(tLONG nCount, tIdx* nIdx = NULL, cRowId *row = NULL);
	virtual bool DelRows(tLONG nCount, tIdx* nIdx);
	virtual bool Expand(bool bExpand);
	virtual void ClearData(bool bNodes = true);
	virtual bool GetIdxArr(tDWORD nPos, tIdxArr& pId);
	virtual cRowIdx& GetIdx(tDWORD nRow);

	virtual size_t GetSelectedCount();
	virtual tIdx GetSelectedItem(cRowId &nCookie);
	virtual bool IsItemSelected(tIdx nItem);
	virtual void Select(tDWORD nPos, bool bSelect);
	virtual bool FindRow(tIdx nIdx, cRowId& row);

	const tNodes& Nodes() const { return m_nodes; }
	bool          IsExpanded() const { return m_bExpanded; }
	tDWORD        RowCount() const { return m_nCount; }

protected:
	cRowNode(cRowNode* parent=NULL, CItemBase* pHeader=NULL, tDWORD nIdx=-1, tDWORD nFlags = 0, cVariant* val = NULL);
	~cRowNode();

	friend struct cRowId;

	bool        AddRow(cRowIdx& idx, cNodeExecCtx& ctx, bool focus = false);
	void        AddSortedRow(cRowIdx& idx, cNodeExecCtx& ctx);
	void        InsertNode(cRowNode* node, bool bRefresh);

	tDWORD      Size(){ return m_bExpanded ? m_nAllSize : m_nNodeSize; }
	void        ReSize(int nSize, bool bNode);
	void        ChSize(int nSize, bool bNode);
	void        Shift(int nShift);
	tDWORD      Count();

	cRowIdx*    GetBlock(tDWORD nBlock);
	tDWORD      GetOffset(tDWORD nRow);
	tDWORD      GetSize(tDWORD nRow);
	tDWORD      GetRowPos(tDWORD offset);
	bool        GetRow(tDWORD offset, cRowId& id);
	bool        NextRow(cRowId& id);
	bool        NextParentRow(cRowId& id);
	bool        PrevRow(cRowId& id);
	bool        FirstRow(cRowId& id);
	bool        LastRow(cRowId& id);

	void        SelectRange(tDWORD nFrom, tDWORD nTo);
	void        ClearSelection();
	void        RemoveSelection(tDWORD num);

	struct cRefreshCtx
	{
		cRefreshCtx() : m_count(0), m_focused(0){}

		tIndex   m_rows;
		std::vector<tDWORD> m_nums;
		tDWORD   m_count;
		cRowIdx* m_focused;
	};

	bool        RefreshView(bool bSync, cRefreshCtx& ctx);
	void        RefreshView(bool bSync);
	void        FlushSortedCache();
	void        CollectRows(cRefreshCtx& ctx, bool bRecursive);

	void        SaveExpanded(CGridItem* pItem);
	void        GetGroupValue(tValArr& vValue);
	CGridItem*  GetGroupItem(){ return m_parent ? m_parent->m_group : NULL; }
	const cVariant& GetSortValue(cVariant& defval);

protected:
	friend class  CListItem;
	friend struct CGridItem::CGroupNode;

	struct cSortTree
	{
		cSortTree(tDWORD pos) : m_left(0), m_right(0), m_pos(pos){}
		~cSortTree(){ delete m_left; delete m_right; }

		cSortTree * m_left;
		cSortTree * m_right;
		tDWORD      m_pos;
		cVariant    m_val;
	};

	struct cSortCache
	{
		tDWORD      m_pos;
		cRowIdx     m_idx;
		cVariant    m_val;
	};

	typedef std::list<cSortCache> tSortCache;

protected:
	CListItem*   m_pList;
	cRowNode*    m_parent;
	tDWORD       m_ppos;
	tDWORD       m_level;
	tDWORD       m_flags;
	tNodes       m_nodes;
	tIndex       m_rows;

	CGridItem*   m_group;
	tSortCache   m_cache;
	cSortTree *  m_stree;

	cRowIdx      m_nIdx;
	tDWORD       m_nIndent;
	tDWORD       m_nCount;
	tDWORD       m_nOffset;
	tDWORD       m_nNodeSize;
	tDWORD       m_nRowSize;
	tDWORD       m_nAllSize;
	tLONG        m_nSelItems;
	tLONG        m_nListState;
	tLONG        m_nSortState;
	CItemBase*   m_pHeader;
	bool         m_bExpanded;
	cVariant     m_val;
};

//////////////////////////////////////////////////////////////////////

class CListItem :
	public CGridItem,
	public cRowNode,
	private cThreadPoolBase,
	private cThreadTaskBase
{
public:
	CListItem();
	~CListItem();

	enum enFlags
	{
		fSorted    = 0x0001,
		fReadOnly  = 0x0002,
		fAutoScroll= 0x0004,
		fSingleSel = 0x0008,
		fNoHeader  = 0x0010,
		fBinded    = 0x0020,
		fIsTree    = 0x0040,
		fUseSink   = 0x0080,
	};

	enum enFindFlags
	{
		eShowDlg   = 0x0001,
		eMatchCase = 0x0002,
		eWholeWord = 0x0004,
		eFindUp    = 0x0008,
		eMarkAll   = 0x0010,
	};
	
	enum enElemType
	{
		etNothing   = 0x0000,
		etColFilter = 0x0002,
	};

	static const tIdx invalidIndex = -1;

	virtual bool Bind(tDWORD nSerId, bool bUseSink = false);
	virtual void UpdateView(int nFlags = 0);

	virtual void SetView(LPCSTR sView);

	virtual void SetFilter(LPCSTR sFilter, tPTR pId = 0);
	virtual bool FindRows(int flags, cColumn* col = NULL, LPCSTR strText = NULL);

	virtual bool GetColData(cColumn** pData){ *pData = &m_cols; return true; };
	virtual cSer* GetRowData(cRowId& row){ return GetRowData(row.m_node, row.Idx().m_idx, row.IsNode()); }

	virtual bool GetItemFocus(cRowId& row);
	virtual void SetItemFocus(cRowId& row, bool fEnsureVisible = false, bool fUnselectAll = true);

	virtual void ReSize(int nSize, bool bNode){ cRowNode::ReSize(nSize, bNode); UpdateLayout(); }
	virtual void Scroll(enScrollType type, enUserAction dir, tDWORD pos=0);
	
	virtual void UpdateItem(cRowId& row) {}
	virtual bool UpdateLayout(bool bUpdate = false);

	virtual bool IsTree() const { return !!(m_flags & fIsTree); }

	virtual void EnsureVisible(cRowId& row);

	virtual CGridItem* GetCellItem(cRowId& row, cColumn& col, CStructData* data = NULL);
	virtual void GetTopRow(cRowId& row){ row = m_toprow; }
	virtual long CheckItem(cRowId& row, cColumn& col, long how = -1, bool recourse = false);
	const cColumn& GetTopColumn() const { return m_cols; }
	virtual void GetElemViewRect(cRowId * row, cColumn * pCol, RECT& rc, enElemType nType = etNothing);

protected:
	virtual void OnUpdateScrollInfo(cScrollInfo& info){}
	virtual void OnGetViewSize(SIZE& szView){}
	virtual bool OnColumnTrack(cColumn& col, tDWORD posX){ return false; }
	virtual bool OnDragItem(cRowId* row, cColumn* col, POINT& pt){ return false; }
	virtual void OnSetCursor(enCursorType type){}

	virtual void OnCliked(POINT& pt, tDWORD state);
	virtual bool OnOver(POINT& pt);
	virtual void OnKeyDown(enUserAction action, tDWORD state);
	virtual void OnExpanded(cRowNode* node, bool bExpand);
	virtual void OnSize(SIZE& sz);
	virtual void OnRowInserted(cRowNode* node, tDWORD pos);
	virtual void OnShowToolTip(){}

protected:
	virtual void do_work(){ RefreshLoop(); }

	virtual void OnInit();
	virtual void OnInited();
	virtual bool OnClicked(CItemBase * pItem);
	virtual bool OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem);
	virtual void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);
	virtual void OnItemCliked(cRowId& row, cColumn& col, bool rClick);

	virtual void Cleanup();
	virtual void ApplyStyle(LPCSTR strStyle);
	virtual void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	virtual void TimerRefresh(tDWORD nTimerSpin);
	virtual void SendEvent(tDWORD nEventId, cSerializable* pData, bool bBroadcast);

	virtual void DrawChildren(HDC dc, RECT* rcUpdate){}
	virtual void DrawContent(HDC dc, RECT& rcClient);

	virtual bool IsAdjustable(bool bHorz){ return( false ); }
	virtual bool IsResizeble(bool bHorz) { return( true ); }

	virtual bool SetValue(const cVariant &pValue){ return CItemBase::SetValue(pValue); }
	virtual bool GetValue(cVariant &pValue){ return CItemBase::GetValue(pValue); }
	virtual void UpdateData(bool bIn, CStructData *pData);

	virtual bool GetTipInfo(POINT& pt, tString* pText, RECT* pRect);

	virtual bool GetUpdateRectEx(RECT& rcFrame, RECT* rcUpdate, CItemBase* pOwner){ return CItemBase::GetUpdateRectEx(rcFrame, rcUpdate, pOwner); }
	virtual cNode * CreateOperand(const char *name, cNode *container, cAlloc *al);
	virtual cNode * CreateOperator(const char *name, cNode **args, cAlloc *al);

	virtual void ClearData(bool bNodes = true);

	virtual void SaveSettings(cAutoRegKey& pKey);
	virtual void LoadSettings(cAutoRegKey& pKey);

protected:
	friend struct cRowNode;
	friend struct cColumn;
	friend struct CGroupNode;

	void         InitItems(CGridItem* pTop);
	void         InitViews();
	void         DrawRow(HDC dc, RECT& rc, cRowId& row);
	void         DrawCol(HDC dc, RECT& rc, cColumn& col);
	void         DrawCell(HDC dc, RECT& rc, cRowId& row, cCellData& cell);
	bool         GetPlusRect(cRowId& row, RECT& rcPlus, bool& bDraw);
	bool         GetCheckRect(cRowId& row, cColumn& col, RECT& rcCheck);
	bool         GetColPlusRect(cColumn& col, RECT& rcPlus);
	bool         GetColFilterRect(cColumn& col, RECT& rcFilter);
	bool         DragItem(cRowId* row, cColumn* col, POINT& pt, bool bEnd);
	bool         DragColumn(cColumn& col, POINT& pt, bool bEnd);
	void         MakeGroup(CGridItem* group, long how = 0);
	void         ClearFilters(bool bColumns);
	void         InitGroupItem();
	void         UpdateViewEx(bool bSync);
	void         RefreshLoop();
	void         SetFilter(cNode* node, tPTR pId);
	void         ShowFilterDialog(cColumn& col);
	void         UpdateItem(cRowId* prow = NULL, cColumn* pcol = NULL);
	void         UpdateRowItem();
	long         GetSortState(){ return m_sortcol ? (m_sortcol->m_nFlags & (cColumn::fSortDown|cColumn::fSortUp)) : 0; }
	void         FocusRow(cRowId& row, bool bClearSel, bool bUpdate);
	void         FocusFirstRow(cRowNode* node);
	void         AutoFitColumn(cColumn& col, bool bRecourse = false);
	bool         UpdateAsTree(cRowNode* node);
	cSer*        GetRowData(cRowNode* node, tIdx idx, bool bIsNode, cNodeExecCtx* pctx = NULL);
	void         UpdateColumnsLayout(bool bInitSizeX = true);
	void         UpdateViewState(bool bComplete = false, bool bBySort = false);
	void         MakeCopyString(cRowId& row, cColumn& col, cStrObj& sText);
	bool         GetColumnInfo(cColumn& col, const char* in, tString* out);
	void         SaveColumnInfo(cColumn& col, tString& sData);
	bool         LoadColumnInfo(cColumn& col, LPCSTR& sData);
	void         SetCurView(LPCSTR curview);
	void         EnsureVisibleEx(cRowId& row, bool bThis = true);
	void         ShowFilterMenu(cColumn& col, POINT& pt);
	void         SortByColumn(cColumn& col, long how = 0);
	void         ShowMenu(cColumn& col, POINT& pt);
	void         SendListEvent(tDWORD eventId, tDWORD event);

public:
	cNode*       FillSwitchItem(CItemBase* pTop, cColumn& col, bool bCombo);
	void         FillColumnsCombo(CItemBase* pCombo);

protected:
	struct CNode : public cNodeArgs<3>
	{
		enum enNode
		{
			tSetFilter = 0x1,
			tFindData  = 0x2,
			tSetView   = 0x3,
			tGetView   = 0x4,
			tCurView   = 0x5,
		};

		CNode(CListItem* list, enNode type, cNode **args)
			: m_type(type), m_list(list), cNodeArgs<3>(args) {}

		void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

		CListItem*  m_list;
		enNode      m_type;
	};

	struct cFilter
	{
		tPTR     m_id;
		cNode*   m_filter;
	};

	friend struct CNode;

	typedef std::vector<cFilter>       tFilters;
	typedef std::vector<CGridItem*>    tGroups;
	typedef cVector<cGridViewTemplate> tViews;

	cDataInfo    m_info;
	cColumn      m_cols;
	tDWORD       m_flags;
    tDWORD       m_nUnique;
	cSerDescriptor* m_pDesc;
	CIcon*       m_pIcoCols;
	COLORREF     m_pSelColor;
	CBackground* m_pBorderBg;
	CBackground* m_pSelBg;

	CGridItem*   m_footer;
	CGridItem*   m_rowitem;
	CGridItem*   m_rowprops;
	CGridItem*   m_groupitem;

	tGroups      m_groups;
	tFilters     m_filters;
	CItemSink*   m_findctx;
	tViews       m_views;
	long         m_curview;

	cRowId       m_toprow;
	cRowId       m_focused;
	cRowId       m_hotrow;
	long         m_lastSel;
	long         m_hotflags;
	cColumn*     m_hotcol;
	cColumn*     m_hitcol;
	bool         m_bWasScroll;
	bool         m_bWasLoad;

	cColumn*     m_sortcol;
	cNode*       m_sorting;

	tLONG        m_nCurState;
	tLONG        m_nLoopState;
    tDWORD       m_nTimer;
    tDWORD       m_nLastTick;
	PRLocker     m_Locker;

	cColumn*     m_dragcol;
	CImageBase*  m_dragimg;
	void*        m_dctx[3];
	tLONG        m_drpos;

	POINT        m_ptView;
	SIZE         m_szPage;
	SIZE         m_szPlus;
};

//////////////////////////////////////////////////////////////////////

class CListItemSink : public CDescriptorDataOwner
{
public:
	typedef cRowNode::cRowId cRowId;

	enum eListEvents
	{
		leSelectionChanged,
		leFocusChanged,
	};

	virtual bool     GetInfo(cDataInfo& info){ return false; }
	virtual cSer *   GetRowData(tIdx nItem){ return NULL; }
	virtual bool     GetCellData(cCellData& cell){ return false; }
	virtual void     PrepareRows(tIdx nFrom, tDWORD nCount){}
	virtual bool     GetTreeInfo(tIdxArr& pId, tDWORD* pNodes, tIdxArr& pRows){ return false; }
	virtual cSer *   GetTreeData(tIdxArr& pId, bool bIsNode){ return NULL; }

	virtual cSer *   GetEditRowData(){ return NULL; }
	virtual bool     IsEditNewItem(){ return false; }

	virtual bool     OnListEvent(eListEvents nEvent) { return true; }
	virtual void     OnNodeExpanded(cRowNode* node, bool bExpanded) {}
	virtual bool     OnAddRow(cRowNode* node, tIdx row) { return true; }
	virtual bool     OnDragRow(cRowId& row, cRowId& rowto, bool bEnd) { return false; }
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(_GRIDVIEW_H_)

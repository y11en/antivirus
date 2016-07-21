// UniCtrls.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNICTRLS_H__5401AE0F_38DD_4399_883A_F072CC926781__INCLUDED_)
#define AFX_UNICTRLS_H__5401AE0F_38DD_4399_883A_F072CC926781__INCLUDED_

QWidget * GetParentQW(CItemBase * pItem, bool bTop = false);

//////////////////////////////////////////////////////////////////////
// CItemWindowT

template <class TQtImpl, class TItemBase = CItemBase>
class CItemWindowT : public TQtImpl, public TItemBase
{
public:
	using TItemBase::m_pRoot;
	using TItemBase::m_nFlags;
	using TItemBase::m_pOwner;
	using TItemBase::OnFocus;
	using TItemBase::OnAdjustBordSize;
	using TItemBase::GetByFlagsAndState;
	using TQtImpl::mousePressEvent;
	
	CItemWindowT() : m_bBordered(1) { m_nFlags |= STYLE_WINDOW; }

	HWND GetWindow() { return (HWND)static_cast<QWidget *>(this); }

	void InitItem(LPCSTR strType)
	{
		TItemBase::InitItem(strType);
		
		QWidget * parent = (QWidget *)m_pOwner->GetWindow();
		TQtImpl::setParent(parent);
	}

	void ApplyStyle(LPCSTR strStyle)
	{
		TItemBase::ApplyStyle(strStyle);
		
		sswitch(strStyle)
		scase(STR_PID_NOBORDER) m_bBordered = 0; sbreak;
		send
	}

	void OnUpdateTextProp()
	{
		ctl_SetText(m_pRoot->LocalizeStr(TOR_cStrObj, TItemBase::OnGetDisplayText()));
		TItemBase::OnUpdateTextProp();
	}

	void OnUpdateProps(int flags = UPDATE_FLAG_ALL, tDWORD nStateMask = 0, RECT * rc = NULL)
	{
		CItemBase * pItem = this;
		
		if( (flags & UPDATE_FLAG_TEXT) && (pItem->m_nState & ISTATE_CHANGED) )
		{
			OnUpdateTextProp();
			pItem->m_nState &= ~ISTATE_CHANGED;
		}
		
		if( (flags & UPDATE_FLAG_FONT) && pItem->m_pFont )
		{
			QFont * p = (QFont *)pItem->m_pFont->Handle();
			if( p )
				TQtImpl::setFont(*p);
		}

		if( (flags & UPDATE_FLAG_ICON) && pItem->m_pIcon )
		{
			QIcon * p = (QIcon *)pItem->m_pIcon->IconHnd();
			if( p )
				TQtImpl::setWindowIcon(*p);
		}

		if( flags & (UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE) )
		{
			RECT rcFrame; pItem->GetParentRect(rcFrame);
			OnAdjustBordSize(rcFrame);

			if( (flags & (UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE)) == (UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE) )
				TQtImpl::setGeometry(QRect(rcFrame.left, rcFrame.top, RECT_CX(rcFrame), RECT_CY(rcFrame)));
			else if( flags & UPDATE_FLAG_POSITION )
				TQtImpl::move(QPoint(rcFrame.left, rcFrame.top));
			else if( flags & UPDATE_FLAG_SIZE )
				TQtImpl::resize(QSize(RECT_CX(rcFrame), RECT_CY(rcFrame)));
		}

		if( nStateMask & ISTATE_DISABLED )
			TQtImpl::setEnabled(!(pItem->m_nState & ISTATE_DISABLED));

		if( nStateMask & ISTATE_HIDE )
			TQtImpl::setVisible(!(pItem->m_nState & ISTATE_HIDE));

		TItemBase::OnUpdateProps(flags, nStateMask, rc);
	}

	void Focus(bool bFocus)
	{
		if( bFocus )
			TQtImpl::setFocus();
	}

	void Update(RECT * rc)
	{
		TItemBase::Update(rc);
		
		if( rc )
			TQtImpl::update(rc->left, rc->top, RECT_CX(*rc), RECT_CY(*rc));
		else
			TQtImpl::update();
	}

protected:
	bool ctl_IsFocused() { return TQtImpl::hasFocus(); }

protected: // Event handlers
	void focusInEvent(QFocusEvent * e)
	{
		OnFocus(true);
		TQtImpl::focusInEvent(e);
	}
	
	void focusOutEvent(QFocusEvent * e)
	{
		OnFocus(false);
		TQtImpl::focusOutEvent(e);
	}
	
	void mouseMoveEvent(QMouseEvent * e)
	{
		POINT pt = {e->x(), e->y()};
		
		CItemBase * pHitItem = HitTest(pt, this, true);
		m_pRoot->SetTrackItem(pHitItem);
		
		TQtImpl::mouseMoveEvent(e);
	}
	
	void leaveEvent(QEvent * e)
	{
		m_pRoot->SetTrackItem(NULL);
		TQtImpl::leaveEvent(e);
	}

	void _MsgPaint(QPaintEvent * e)
	{
		QPainter painter(this);
		RECT rcUpdate;
		HDC  dc = (HDC)&painter;
		
		{
			const QRect& redrawRect = e->rect();
			rcUpdate.left   = redrawRect.left();
			rcUpdate.top    = redrawRect.top();
			rcUpdate.right  = redrawRect.right();
			rcUpdate.bottom = redrawRect.bottom();
		}
		
		bool bClip = TItemBase::Clip(dc, &rcUpdate);
		TItemBase::Draw(dc, &rcUpdate);
		m_pRoot->ClipRect(dc, NULL);
	}
	
	bool _MsgMenu(POINT& ptMenu)
	{
		if( ptMenu.x == -1 && ptMenu.y == -1 )
		{
			if( CItemBase * pFocus = GetByFlagsAndState(STYLE_ALL, ISTATE_FOCUSED) )
				pFocus->LoadItemMenu(NULL, LOAD_MENU_FLAG_SHOW|LOAD_MENU_FLAG_DESTROY|LOAD_MENU_FLAG_BOTTOMOFITEM);
			return true;
		}

		CItemBase * pItem = m_pRoot->m_pTrackItem;
		if( !pItem )
			pItem = this;
		
		if( pItem->m_nState & ISTATE_DISABLED )
			return false;

		CPopupMenuItem * pMenu = pItem->LoadItemMenu(&ptMenu, LOAD_MENU_FLAG_SHOW);
		if( !pMenu )
			return false;
		
		bool bLoaded = !!pMenu->GetChildrenCount();
		pMenu->Destroy();
		return bLoaded;
	}
	
	void contextMenuEvent(QContextMenuEvent * e)
	{
		POINT ptMenu = {e->x(), e->y()};
		_MsgMenu(ptMenu);

		TQtImpl::contextMenuEvent(e);
	}

	void mouseReleaseEvent(QMouseEvent * e)
	{
		if( m_pRoot->m_pTrackItem && (m_pRoot->m_pTrackItem->m_nFlags & STYLE_CLICKABLE) )
			m_pRoot->m_pTrackItem->Click();

		TQtImpl::mouseReleaseEvent(e);
	}

	void mouseDoubleClickEvent(QMouseEvent * e)
	{
		if( m_pRoot->m_pTrackItem && (m_pRoot->m_pTrackItem->m_nFlags & STYLE_CLICKABLE) )
			mousePressEvent(e);

		CItemBase * pItem = m_pRoot->m_pTrackItem ? m_pRoot->m_pTrackItem : this;
		pItem->DblClick();
		
		TQtImpl::mouseDoubleClickEvent(e);
	}

	unsigned m_bBordered : 1;
};

//////////////////////////////////////////////////////////////////////
// CImageList

class CImageList : public cVector<QPixmap>
{
public:
	typedef QPixmap t;
	
	t *     GetImage(tDWORD nIdx) const;
	tDWORD  AddImage(const t * pImage);
	
	tDWORD  AddIcon(QIcon * qIcon, const QSize& sz, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
	QIcon * GetIcon(tDWORD nIdx, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
	bool    GetToIcon(tDWORD nIdx, QIcon& qIcon, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
};

//////////////////////////////////////////////////////////////////////
// CGroupBox

class CGroupBox : public CItemWindowT<QGroupBox, CGroupItem>
{
public:
	typedef CItemWindowT<QGroupBox, CGroupItem> TBase;
	CGroupBox() { m_nFlags |= STYLE_NON_OWNER; setMouseTracking(true); }

protected:
	void   ctl_SetText(LPCWSTR strText)  { setTitle(_S2Q(strText)); }
};

//////////////////////////////////////////////////////////////////////
// CBitmapButton

class CBitmapButton : public CItemWindowT<QPushButton>
{
Q_OBJECT

public:
	typedef CItemWindowT<QPushButton> TBase;

	CBitmapButton()
	{
		m_nFlags |= STYLE_CLICKABLE|TEXT_SINGLELINE|STYLE_TRANSPARENT|STYLE_DRAW_BACKGROUND;
		connect(this, SIGNAL(clicked()), this, SLOT(_on_clicked()));

		setMouseTracking(true);
	}

	bool IsResizeble(bool bHorz){ return false; }

	void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
	{
		TBase::OnUpdateProps(flags, nStateMask, rc);

		if( nStateMask & ISTATE_DEFAULT )
			QPushButton::setDefault(!!(m_nState & ISTATE_DEFAULT));
	}

protected:
	void paintEvent(QPaintEvent * e) { _MsgPaint(e); }

protected slots:
	void _on_clicked()
	{
		if( isEnabled() && isVisible() )
			Click();
	}
};

//////////////////////////////////////////////////////////////////////
// CHotlink

class CHotlink : public CItemWindowT<QWidget>
{
public:
	typedef CItemWindowT<QWidget> TBase;
	
	CHotlink();

	void OnInit();
	void OnFocus(bool bFocusted);

	void paintEvent(QPaintEvent * e) { _MsgPaint(e); }
};

//////////////////////////////////////////////////////////////////////
// CChkRadBase

class CChkRadBase : public CRefsItem
{
public:
	typedef CRefsItemLinkT<CHotlink> TRef;

	CChkRadBase();
	~CChkRadBase();

	RefData * _RefCreate() { return new TRef(); }
	
	void OnAdjustClientRect(RECT& rcClient);
	void OnApplyBlockStyles();

	bool IsResizeble(bool bHorz){ return false; }
	void GetAddSize(SIZE& szAdd);
	bool IsOut(){ return true; }

protected:
	CFontStyle * m_pLinkFont;
};

//////////////////////////////////////////////////////////////////////
// CCheck

class CCheck : public CItemWindowT<QCheckBox, CChkRadBase>
{
Q_OBJECT

public:
	typedef CItemWindowT<QCheckBox, CChkRadBase> TBase;
	
	CCheck() : m_bInvert(0)
	{
		connect(this, SIGNAL(stateChanged(int)), this, SLOT(ctl_OnValueChanged(int)));
	}

	void ApplyStyle(LPCSTR strStyle)
	{
		TBase::ApplyStyle(strStyle);
		
		sswitch(strStyle)
		stcase(invert) m_bInvert = 1; sbreak;
		send;
	}

	tTYPE_ID GetType(){ return tid_DWORD; }

	bool SetValue(tPTR pValue)
	{
		Qt::CheckState nVal = *(tDWORD *)pValue ?
			(m_bInvert ? Qt::Unchecked : Qt::Checked) :
			(m_bInvert ? Qt::Checked : Qt::Unchecked);
		
		setCheckState(nVal);
		return true;
	}

	bool GetValue(tPTR pValue)
	{
		Qt::CheckState nVal = checkState();

		*(tDWORD *)pValue = (nVal == Qt::Checked) ?
			(m_bInvert ? 0 : 1) :
			(m_bInvert ? 1 : 0);
		return true;
	}

protected slots:
	void ctl_OnValueChanged(int) { SetChangedData(); }

protected:
	void   ctl_SetText(LPCWSTR strText)  { setText(_S2Q(strText)); }
	
	unsigned m_bInvert : 1;
};

//////////////////////////////////////////////////////////////////////
// CRadio

class CRadio : public CItemWindowT<QRadioButton, CChkRadBase>
{
Q_OBJECT

public:
	typedef CItemWindowT<QRadioButton, CChkRadBase> TBase;

	CRadio()
	{
		m_nFlags |= STYLE_ENUM_VALUE;
		connect(this, SIGNAL(clicked(bool)), this, SLOT(ctl_OnValueChanged(bool)));
	}
	
	tTYPE_ID GetType(){ return tid_BOOL; }
	bool     SetValue(tPTR pValue){ setChecked(*(tBOOL *)pValue ? true : false); return true; }
	bool     GetValue(tPTR pValue){ *(tBOOL *)pValue = isChecked(); return true; }

protected slots:
	void ctl_OnValueChanged(bool) { SetChangedData(); }

protected:
	void   ctl_SetText(LPCWSTR strText)  { setText(_S2Q(strText)); }
};

//////////////////////////////////////////////////////////////////////
// CButton

class CButton : public CItemWindowT<QPushButton>
{
Q_OBJECT

public:
	typedef CItemWindowT<QPushButton> TBase;

	CButton()
	{
		m_nFlags |= STYLE_CLICKABLE|TEXT_SINGLELINE;
		connect(this, SIGNAL(clicked()), this, SLOT(_on_clicked()));
	}

	void GetAddSize(SIZE& szAdd){ szAdd.cx += 16; }
	bool IsResizeble(bool bHorz){ return false; }

	void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
	{
		TBase::OnUpdateProps(flags, nStateMask, rc);

		if( nStateMask & ISTATE_DEFAULT )
			QPushButton::setDefault(!!(m_nState & ISTATE_DEFAULT));
	}

protected:
	void   ctl_SetText(LPCWSTR strText)  { setText(_S2Q(strText)); }

protected slots:
	void _on_clicked()
	{
		if( isEnabled() && isVisible() )
			Click();
	}
};

//////////////////////////////////////////////////////////////////////
// CProgress

class CProgress : public CItemWindowT<QProgressBar>
{
public:
	CProgress()
	{
		setTextVisible(false);
	}
	
	tTYPE_ID GetType() { return tid_DWORD; }
	bool     IsResizeble(bool bHorz) { return bHorz; }
	bool     IsAdjustable(bool bHorz) { return false; }

	bool SetValue(tPTR pValue)
	{
		tDWORD nValue = *(tDWORD *)pValue;
		setValue(nValue == -1 ? 0 : nValue);
		return true;
	}
	
	bool GetValue(tPTR pValue)
	{
		*(tDWORD *)pValue = value();
		return true;
	}
};

//////////////////////////////////////////////////////////////////////
// CCombo

class CCombo : public CItemWindowT<QComboBox, CComboItem>
{
Q_OBJECT

public:
	typedef CItemWindowT<QComboBox, CComboItem> TBase;
	
	void       InitItem(LPCSTR strType);
	void       GetAddSize(SIZE& szAdd) { szAdd.cy += 8; }

public:
	tSIZE_T ctl_GetCurSel();
	void    ctl_SetCurSel(tSIZE_T nIdx);
	tSIZE_T ctl_GetItemsCount();
	void    ctl_DeleteItem(tSIZE_T nIdx);
	void    ctl_DeleteAllItems();
	void    ctl_GetTextByPos(tSIZE_T nIdx, cStrObj& strText);
	void    ctl_SetText(LPCWSTR strText);
	void    ctl_GetText(cStrObj& strText);
	tSIZE_T ctl_FindByName(LPCWSTR strName);
	tSIZE_T ctl_AddItem(LPCWSTR strName);
	tPTR    ctl_GetItemData(tSIZE_T nIdx);
	void    ctl_SetItemData(tSIZE_T nIdx, tPTR pData);

protected slots:
	void OnSelChanged(int index) { _SelChanged(index); }
};

//////////////////////////////////////////////////////////////////////
// CTab

class CTab : public CItemWindowT<QTabWidget, CTabItem>
{
Q_OBJECT

public:
	~CTab();

protected:
	tSIZE_T ctl_GetCurSel();
	void    ctl_SetCurSel(tSIZE_T nIdx);
	void    ctl_SetItem(tSIZE_T nIdx, LPCSTR strText, CItemBase * pItem);
	tSIZE_T ctl_GetItemCount();
	void    ctl_DeleteItem(tSIZE_T nIdx);
	void    ctl_DeleteAllItems();
	void    ctl_AdjustRect(RECT& rcClient);

protected:
	typedef CItemWindowT<QTabWidget, CTabItem> TBase;
	
	void InitItem(LPCSTR strType);
	CItemBase * CreateChild(LPCSTR strItemId, LPCSTR strItemType);

protected:
	void tabRemoved(int index);

protected slots:
	void OnSelChanged(int index);
};

/*
//////////////////////////////////////////////////////////////////////
// CVListImpl

class CVListImpl : public CItemWindowT<QTreeView, CListItem>, public QAbstractListModel
{
Q_OBJECT

public:
	typedef CItemWindowT<QTreeView, CListItem> TBase;
	
	CVListImpl();
	~CVListImpl();
	
	void    UpdateView(int nFlags);
	int     AddIcon(CIcon * pIcon);
	CIcon * GetIcon(int nIndex);

protected:
	void   ApplyStyle(LPCTSTR strStyle);
	void   InitItem(LPCSTR strType);

protected:
	void   ctl_DeleteAllItems() { m_nItems = 0; }
	void   ctl_RedrawItem(int nItem);
	void   ctl_Redraw();
	void   ctl_SetItemCount(tDWORD nCount) { m_nItems = nCount; }
	tSIZE_T ctl_GetItemCount() { return m_nItems; }
	tDWORD ctl_GetItemState(tSIZE_T nItem, tDWORD nMask) { return 0; }
	void   ctl_SetItemState(tSIZE_T nItem, tDWORD nState, tDWORD nMask) {}
	void   ctl_ScrollDown() {}
	int    ctl_GetNextSelectedItem(int nPrev = -1);
	tDWORD ctl_GetSelectedCount() { return m_nSelCount; }

	void   ctl_ColsUpdate();
	void   ctl_ColsResize();
	tDWORD ctl_ColGetIdx(int nDisplayPos) { return nDisplayPos; }

protected: // QAbstractListModel
	int      columnCount(const QModelIndex &parent) const;
	int      rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	void     selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	bool     setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

protected slots:
	void _on_selected(const QModelIndex &index) {}
	void _on_activated(const QModelIndex &index) { DblClick(); }
	
	void _on_hdr_sectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void _on_hdr_sectionResized(int logicalIndex, int oldSize, int newSize) {}
    void _on_hdr_sectionPressed(int logicalIndex) {}
    void _on_hdr_sectionClicked(int logicalIndex) {}

protected:
	QHeaderView * m_header;
	CImageList    m_ImageList;
	unsigned      m_hdrinit : 1;
	unsigned      m_bChecked : 1;

	tDWORD        m_nItems;
	tDWORD        m_nSelCount;
};
*/

//////////////////////////////////////////////////////////////////////
// CTreeImpl

class CTreeImpl : public CItemWindowT<QTreeWidget, CTreeItem>
{
Q_OBJECT

public:
	typedef CItemWindowT<QTreeWidget, CTreeItem> TBase;
	
	CTreeImpl();

	cTreeItem * AppendItem(cTreeItem * pParent, cTreeItem * pItem, cTreeItem * pAfter = NULL);
	void        DeleteItem(cTreeItem * pItem);
	cTreeItem * GetParentItem(cTreeItem * pItem);
	cTreeItem * GetChildItem(cTreeItem * pItem);
	cTreeItem * GetNextItem(cTreeItem * pItem);
	cTreeItem * GetSelectionItem();
	cTreeItem * TreeHitTest(POINT &pt, tUINT * pnFlags);
	void        SelectItem(cTreeItem * pItem);
	void        ExpandItem(cTreeItem * pItem, bool bExpand);
	void        SetItemCheckState(cTreeItem * pItem, bool bChecked = true);
	void        SortChildren(cTreeItem * pItem, bool bRecurse);
	void        SetImageList(hImageList hImages);
	void        UpdateView(int nFlags);

protected:
	class TVItem : public QTreeWidgetItem
	{
	public:
		TVItem() : m_data(NULL) {}
		~TVItem()
		{
			if( !m_data )
				return;
			
			CTreeImpl * pTree = static_cast<CTreeImpl *>(treeWidget());
			if( pTree && pTree->m_pSink )
				((CTreeItemSink *)pTree->m_pSink)->OnItemDestroy(m_data);
		}
		
		cTreeItem * m_data;
	};
	
protected:
	void      ApplyStyle(LPCSTR strStyle);
	void      InitItem(LPCSTR strType);

	cTreeItem * _GetItemData(QTreeWidgetItem * tvi);

protected slots:
	void _on_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous) { OnSelect(this); }
	void _on_itemExpanded(QTreeWidgetItem * tvi);

protected:
	CImageList * m_pImageList;
};

//////////////////////////////////////////////////////////////////////
// CEdit

class CEdit : public CItemWindowT<QWidget, CEditItem>
{
Q_OBJECT

public:
	typedef CItemWindowT<QWidget, CEditItem> TBase;
	CEdit();

protected:
	void ctl_GetText(cStrObj& strText);
	void ctl_SetText(LPCWSTR strText);
	void ctl_ReplaceText(LPCWSTR strText, tSIZE_T nFrom, tSIZE_T nTo = -1);
	void ctl_ReplaceSelText(LPCWSTR strText);
	void ctl_SetSelText(tSIZE_T nFrom, tSIZE_T nTo, bool bAsync = false);
	bool ctl_IsMultiline();
	bool ctl_IsNumber();
	bool ctl_IsReadonly();
	void ctl_CompressName(LPSTR strText);
	LPCSTR ctl_CompressPath(LPCSTR strText);
	
protected:
	void resizeEvent(QResizeEvent * e);

protected slots:
	void ctl_MLE_OnDataChanged() { _DataChanged(); }
	void ctl_SLE_OnDataChanged(const QString &) { _DataChanged(); }

protected:
	void Cleanup();
	void ApplyStyle(LPCSTR strStyle);
	void InitItem(LPCSTR strType);
	void GetAddSize(SIZE& szAdd);

	union
	{
		QWidget   * m_pE;
		QLineEdit * m_pSLE;
		QTextEdit * m_pMLE;
	};

	unsigned m_bReadonly : 1;
	unsigned m_bPassword : 1;
	unsigned m_bNumeric : 1;
	unsigned m_bMultiline : 1;
	unsigned m_bScroll : 1;
};

//////////////////////////////////////////////////////////////////////
// CTimeEditBox

class CTimeEditBox : public CItemWindowT<QDateTimeEdit> {};

//////////////////////////////////////////////////////////////////////
// CIPAddressCtrl

class CIPAddressCtrl : public CItemWindowT<QLineEdit, CIPAddrEditItem>
{
Q_OBJECT

public:
	CIPAddressCtrl();

protected:
	void     GetAddSize(SIZE& szAdd);
	bool     SetValue(tPTR pValue);
	bool     GetValue(tPTR pValue);

protected slots:
	void ctl_OnDataChanged(const QString &) { _DataChanged(); }
};

//////////////////////////////////////////////////////////////////////
// CRichEditImpl

class CRichEditImpl : public CItemWindowT<QTextEdit, CRichEditItem>
{
Q_OBJECT

public:
	typedef CItemWindowT<QTextEdit, CRichEditItem> TBase;
	CRichEditImpl();

protected:
	void   ApplyStyle(LPCSTR strStyle);
	void   InitItem(LPCSTR strType);

	void   OnApplyBlockStyles();
	void   OnCalcTextSize(HDC dc, SIZE& sz, tDWORD nFlags);
//	void   OnUpdateTextProp() { CRichEditItem::OnUpdateTextProp(); }

protected:
	void   ctl_SetText(LPCWSTR strText)  { setPlainText(_S2Q(strText)); }

protected:
/*
	void focusInEvent(QFocusEvent * e);
	void focusOutEvent(QFocusEvent * e);
	void enterEvent(QEvent * e);
	void leaveEvent(QEvent * e);
*/
	void mouseDoubleClickEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);

protected slots:

protected:
	unsigned m_bNoFocus : 1;
};

//////////////////////////////////////////////////////////////////////
// CSlider

class CSlider : public CItemWindowT<QSlider, CSliderItem>
{
Q_OBJECT

public:
	typedef CItemWindowT<QSlider, CSliderItem> TBase;
	
	CSlider();
	
	void ApplyStyle(LPCSTR strStyle);
	void InitItem(LPCSTR strType);

protected:
	void ctl_SetRange(tSIZE_T nMin, tSIZE_T nMax);
	void ctl_SetPos(tSIZE_T nPos);
	tSIZE_T  ctl_GetPos();
	bool ctl_IsVert();

protected slots:
	void ctl_OnValueChanged(int) { _DataChanged(); }
};

//////////////////////////////////////////////////////////////////////
// CPopupMenu

class CPopupMenu : public CPopupMenuItem
{
public:
	CPopupMenu() : m_pMenu(NULL) {}
	~CPopupMenu() { if( m_pMenu ) delete m_pMenu; }

protected:
	HMENU  ctl_GetRootItem();
	HMENU  ctl_AddItem(HMENU hMenu, CItemBase * pItem, bool bSub = false);
	tDWORD ctl_Track(POINT& pt);
	void   ctl_TrackEnd();
	void   ctl_CleanSeps(HMENU hMenu) {}

	QMenu * m_pMenu;
};

//////////////////////////////////////////////////////////////////////
// CDialogItemImpl

class CDialogItemImpl : public CItemWindowT<QDialog, CDialogItem>
{
public:
	typedef CItemWindowT<QDialog, CDialogItem> TBase;
	friend class CDialogItem;
	
	CDialogItemImpl();
	
	bool IsActive();
	void Activate();
	void GetParentRect(RECT& rcParent, CItemBase * pOwner = NULL);
	void ModalEnableParent(bool bEnable);
	void Close(tDWORD nAction);

protected:
	void ApplyStyle(LPCSTR strStyle);
	void InitItem(LPCSTR strType);
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);

	void OnInited();
	void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);
	void OnAdjustBordSize(RECT& rc);

protected:
	void ctl_SetText(LPCWSTR strText) { setWindowTitle(_S2Q(strText)); }

protected:
	void _InitDlgRect(RECT& rcFrame);
	bool _CloseDlg();
	void _AdjustFrameRect(RECT& rc, bool bInclude);

protected:
	void   ctl_SetTransparentValue(tBYTE nAlpha);
	tDWORD ctl_SetTransparentTimer(tDWORD nInterval);
	void   ctl_KillTransparentTimer(tDWORD nId);

protected: // Event handlers
	bool event(QEvent * e);
	void reject();
	void done(int);
	void resizeEvent(QResizeEvent * e);
	void timerEvent(QTimerEvent * e);
	void paintEvent(QPaintEvent * e) { _MsgPaint(e); }

protected:
	Qt::WindowFlags m_nStyles;
	unsigned        m_bEnableParent : 1;
	unsigned        m_bCloseCalled : 1;
	unsigned        m_bResizing : 1;
};

//////////////////////////////////////////////////////////////////////
// CDialogControl

class CDialogControl : public CItemWindowT<QWidget, CDialogControlItem>
{
public:
	typedef CItemWindowT<QWidget, CDialogControlItem> TBase;

	CDialogControl() { setMouseTracking(true); }
	void paintEvent(QPaintEvent * e) { _MsgPaint(e); }
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_UNICTRLS_H__5401AE0F_38DD_4399_883A_F072CC926781__INCLUDED_)

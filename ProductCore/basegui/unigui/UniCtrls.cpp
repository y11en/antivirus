// WinCtrls.cpp: implementation of the CWinCtrls class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UniRoot.h"
#include "moc_ctrls.cpp"

//////////////////////////////////////////////////////////////////////

QWidget * GetParentQW(CItemBase * pItem, bool bTop)
{
	if( !pItem )
		return NULL;

	HWND hWnd = NULL;
	for(CItemBase * pOwner; pOwner = pItem->GetOwner(bTop); )
	{
		if( hWnd = pOwner->GetWindow() )
			break;

		if( !(pItem = pOwner->m_pParent) )
			break;
	}
	
	return (QWidget *)hWnd;
}

//////////////////////////////////////////////////////////////////////

extern tBOOL osd_OffsetRect(RECT * rc, int x, int y);

void osd_CompressName(CRootItem * pRoot, OSDCN_TYPE eType, const tCHAR * pStr, HDC hDc, HFONT hFont, int nCX, tString& strDst)
{
	strDst = pStr ? pStr : "";
	if( strDst.empty() )
		return;

	// .............
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

QString& CUniRoot::_2QS(LPCSTR str)
{
	return _2QS(str, m_qsTemp);
}

QString& CUniRoot::_2QS(LPCSTR str, QString& q)
{
	return q = QString::fromUtf8(str);
}

QString& CUniRoot::_2QS(cStrObj& str, QString& q)
{
	return q.setUtf16(reinterpret_cast<const unsigned short*>(str.data()), str.size());
}

QString& CUniRoot::_2QS(LPCWSTR str)
{
	return m_qsTemp.setUtf16(reinterpret_cast<const unsigned short*>(str), wcslen(str));
}

void CUniRoot::_2PS(const QString& q, cStrObj& s)
{
	s = reinterpret_cast<const tWCHAR*>(q.utf16());
}

void CUniRoot::_2PS(const QString& q, tString& str)
{
	str = (const char *)q.toUtf8();
}

//////////////////////////////////////////////////////////////////////
// CImageList

CImageList::t * CImageList::GetImage(tDWORD nIdx) const
{
	return nIdx < size() ? &at(nIdx) : NULL;
}

tDWORD CImageList::AddImage(const t * pImage)
{
	if( !pImage )
		return -1;
	insert(size()) = *pImage;
	return size() - 1;
}

tDWORD CImageList::AddIcon(QIcon * qIcon, const QSize& sz, QIcon::Mode mode, QIcon::State state)
{
	if( !qIcon )
		return -1;

	const QPixmap& pixmap = qIcon->pixmap(sz, mode, state);
	if( pixmap.isNull() )
		return -1;
	return AddImage(&pixmap);
}

QIcon * CImageList::GetIcon(tDWORD nIdx, QIcon::Mode mode, QIcon::State state) const
{
	QIcon * qi = new QIcon();
	if( qi && !GetToIcon(nIdx, *qi, mode, state) )
		delete qi, qi = NULL;

	return qi;
}

bool CImageList::GetToIcon(tDWORD nIdx, QIcon& qIcon, QIcon::Mode mode, QIcon::State state) const
{
	t * pImage = GetImage(nIdx);
	if( !pImage )
		return false;
	
	qIcon.addPixmap(*pImage, mode, state);
	return true;
}

//////////////////////////////////////////////////////////////////////
// CHotlink

CHotlink::CHotlink()
{
	m_nFlags |= TEXT_SINGLELINE|STYLE_CLICKABLE|STYLE_TRANSPARENT|STYLE_DRAW_CONTENT|STYLE_DRAW_BACKGROUND;
	setMouseTracking(true);
}

void CHotlink::OnInit()
{
	TBase::OnInit();

	if( GUI_ISSTYLE_SIMPLE(m_pRoot) )
	{
		if( m_pFont->GetColor() == RGB(255,255,255) )
			m_pFont = m_pRoot->GetFont("Hotlink");
	}
}

void CHotlink::OnFocus(bool bFocusted)
{
	TBase::OnFocus(bFocusted);

	for(int i = 0; i < m_aItems.size(); i++)
		m_aItems[i]->Focus(bFocusted);
}

//////////////////////////////////////////////////////////////////////
// CChkRadBase

#define DRAW_CHKRAD_SIZE 13

CChkRadBase::CChkRadBase() :
	m_pLinkFont(NULL)
{
	m_nFlags |= STYLE_CLICKABLE|STYLE_DRAW_CONTENT|STYLE_TRANSPARENT;
}

CChkRadBase::~CChkRadBase()
{
	if( m_pLinkFont )
		m_pLinkFont->Destroy(), m_pLinkFont = NULL;
}

void CChkRadBase::GetAddSize(SIZE& szAdd)
{
	szAdd.cx += DRAW_CHKRAD_SIZE + 4 + 4;
	if( szAdd.cy < DRAW_CHKRAD_SIZE )
		szAdd.cy = DRAW_CHKRAD_SIZE;
}

void CChkRadBase::OnAdjustClientRect(RECT& rcClient)
{
	rcClient.left += DRAW_CHKRAD_SIZE + 4 + 3;
}

void CChkRadBase::OnApplyBlockStyles()
{
	if( !m_pLinkFont )
	{
		CFontStyle::INFO pFontInfo; m_pFont->GetInfo(pFontInfo);
		m_pLinkFont = m_pRoot->CreateFont();
		m_pLinkFont->Init(m_pRoot, m_pFont->GetName().c_str(), pFontInfo.m_nHeight, RGB(0, 0, 0xFF), pFontInfo.m_nBold, pFontInfo.m_bItalic, true, pFontInfo.m_nCharExtra);
	}
	
	if( !m_aRefs.size() )
		return;

	RECT rcCli; GetClientRectEx(rcCli, false);
	
	QPainter& _painter = *(QPainter *)m_pRoot->GetDesktopDC();
	QFont& _font = *(QFont *)m_pFont->Handle();
	if( !&_painter || !&_font )
		return;

	_painter.save();

	_painter.setFont(_font);
	
	QString _qsDispText; ((CUniRoot *)m_pRoot)->_2QS(GetDisplayText(), _qsDispText);
	for(int i = 0; i < m_aRefs.size(); i++)
	{
		CRefsItem::RefData * pRef = m_aRefs[i];
		CItemBase * pItem = pRef->_GetItem();
		QWidget * pWidget = (QWidget *)pItem->GetWindow();
		
		if( !pWidget )
			continue;
		
		pItem->m_pFont = m_pLinkFont;
		
		RECT rcRef;
		
		{
			QRect rcText(0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE);
			if( pRef->m_chrRange.x )
				_painter.drawText(rcText, Qt::TextSingleLine, _qsDispText.mid(0, pRef->m_chrRange.x), &rcText);
			else
			{
				_painter.drawText(rcText, Qt::TextSingleLine, QObject::tr("W"), &rcText);
				rcText.setWidth(0);
			}
			
			rcRef.left   = rcCli.left + rcText.width();
			rcRef.top    = rcCli.top;
			rcRef.bottom = rcCli.top + rcText.height();
			rcRef.right  = rcRef.left;
		}
		
		if( pRef->m_chrRange.y > pRef->m_chrRange.x )
		{
			QRect rcText(0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE);
			_painter.drawText(rcText, Qt::TextSingleLine, _qsDispText.mid(pRef->m_chrRange.x, pRef->m_chrRange.y - pRef->m_chrRange.x), &rcText);
			rcRef.right += rcText.width();
		}
		
		pItem->m_szSize.cx = RECT_CX(rcRef);
		pItem->m_szSize.cy = RECT_CY(rcRef);
		pWidget->setGeometry(rcRef.left, rcRef.top, pItem->m_szSize.cx, pItem->m_szSize.cy);
	}
	
	_painter.restore();
}

//////////////////////////////////////////////////////////////////////
// CCombo

void CCombo::InitItem(LPCSTR strType)
{
	if( m_edit )
		setEditable(true);
	
//	if( m_sorted )
//		m_nStyles |= CBS_SORT;

	TBase::InitItem(strType);
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSelChanged(int)));
}

tSIZE_T CCombo::ctl_GetCurSel() { return currentIndex(); }
void    CCombo::ctl_SetCurSel(tSIZE_T nIdx) { setCurrentIndex(nIdx); }
tSIZE_T CCombo::ctl_GetItemsCount() { return count(); }
void    CCombo::ctl_DeleteItem(tSIZE_T nIdx) { removeItem(nIdx); }
void    CCombo::ctl_DeleteAllItems() { for(int i = 0, n = count(); i < n; i++) removeItem(0); }

tSIZE_T CCombo::ctl_AddItem(LPCWSTR strName)
{
	addItem(_S2Q(strName));
	return count() - 1;
}

tPTR CCombo::ctl_GetItemData(tSIZE_T nIdx)
{
	return (tPTR)itemData(nIdx).toULongLong();
}

void CCombo::ctl_SetItemData(tSIZE_T nIdx, tPTR pData)
{
	setItemData(nIdx, (qulonglong)pData);
}

tSIZE_T CCombo::ctl_FindByName(LPCWSTR strName)
{
	return findText(_S2Q(strName));
}

void CCombo::ctl_GetText(cStrObj& strText)
{
	QLineEdit * pEdit = lineEdit();
	if( !pEdit )
	{
		strText = L"";
		return;
	}
	
	const QString& qStr = pEdit->text();
	_Q2S(qStr, strText);
}

void CCombo::ctl_SetText(LPCWSTR strText)
{
	QLineEdit * pEdit = lineEdit();
	if( pEdit )
		pEdit->setText(_S2Q(strText));
}

void CCombo::ctl_GetTextByPos(tSIZE_T nIdx, cStrObj& strText)
{
	const QString& qStr = itemText(nIdx);
	_Q2S(qStr, strText);
}

//////////////////////////////////////////////////////////////////////
// CTab

CTab::~CTab()
{
	ctl_DeleteAllItems();
}

void CTab::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(OnSelChanged(int)));
}

CItemBase * CTab::CreateChild(LPCTSTR strItemId, LPCTSTR strItemType)
{
	if( strItemType == NULL || !*strItemType )
		return new CDialogControl();

	return NULL;
}

tSIZE_T CTab::ctl_GetCurSel() { return currentIndex(); }
void   CTab::ctl_SetCurSel(tSIZE_T nIdx) { setCurrentIndex(nIdx); }
tSIZE_T CTab::ctl_GetItemCount() { return count(); }
void   CTab::ctl_DeleteItem(tSIZE_T nIdx) { removeTab(nIdx); }
void   CTab::ctl_DeleteAllItems() { for(int i = 0, n = count(); i < n; i++) removeTab(0); }

void CTab::ctl_AdjustRect(RECT& rcClient)
{
	const QRect& gt = geometry();
    
	QStyleOptionTabWidgetFrame option;
	option.init(this);
	option.tabBarSize = sizeHint();
	option.rightCornerWidgetSize = QSize(0, 0);
	option.leftCornerWidgetSize = QSize(0, 0);
	option.shape = QTabBar::RoundedNorth;
	
	const QRect& panelRect = style()->subElementRect(QStyle::SE_TabWidgetTabContents, &option, this);

	rcClient.left   += panelRect.left();
	rcClient.right  -= gt.width() - panelRect.right();
	rcClient.top    += panelRect.top();
	rcClient.bottom -= gt.height() - panelRect.bottom();
}

void CTab::ctl_SetItem(tSIZE_T nIdx, LPCSTR strText, CItemBase * pItem)
{
	tSIZE_T n = count();
	if( nIdx >= n )
	{
		nIdx = n;
		insertTab(nIdx, new QWidget(), _S2Q(strText));
	}
	else
		widget(nIdx)->setWindowTitle(_S2Q(strText));
}

void CTab::tabRemoved(int index)
{
	QWidget * pPageArea = widget(index);
	delete pPageArea;
}

void CTab::OnSelChanged(int index)
{
	OnSelectItem(_Idx2Page(m_nCurItem), false);
	OnSelectItem(_Idx2Page(index), true);
	m_nCurItem = index;
}

//////////////////////////////////////////////////////////////////////
// CTreeImpl

CTreeImpl::CTreeImpl() :
	m_pImageList(NULL)
{
	setItemsExpandable(true);
	setDragEnabled(false);
	setColumnCount(1);
	
	QTreeWidgetItem * pHeader = headerItem();
	setItemHidden(pHeader, true);

	connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(_on_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(_on_itemExpanded(QTreeWidgetItem *)));
}

void CTreeImpl::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);

	sswitch(strStyle)
//	scase(STR_PID_NOSCROLL) m_nStyles |= TVS_NOSCROLL; sbreak;
//	stcase(checked)         m_nStyles |= TVS_CHECKBOXES;  sbreak;
//	stcase(nobuttons)       m_nStyles &= ~TVS_HASBUTTONS;  sbreak;
//	stcase(nolines)         m_nStyles &= ~TVS_HASLINES;    sbreak;
	stcase(nolinesatroot)   setRootIsDecorated(false); sbreak;
	send;
}

void CTreeImpl::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);
	
	if( !m_bBordered )
		setFrameShape(QFrame::NoFrame);
}

void CTreeImpl::SetImageList(hImageList hImages)
{
	m_pImageList = (CImageList *)hImages;
}

cTreeItem * CTreeImpl::AppendItem(cTreeItem * pParent, cTreeItem * pItem, cTreeItem * pAfter)
{
	if( !pItem )
		return NULL;

	TVItem * tvi = new TVItem();
	if( m_pSink )
		((CTreeItemSink *)m_pSink)->OnUpdateItemProps(pItem, UPDATE_FLAG_ALL);

	tvi->setText(0, _S2Q(pItem->m_strName.data()));
	if( m_pImageList )
	{
		QIcon qIcon;
		m_pImageList->GetToIcon(pItem->m_nImage,         qIcon, QIcon::Normal, QIcon::Off);
		m_pImageList->GetToIcon(pItem->m_nSelectedImage, qIcon, QIcon::Normal, QIcon::On);
		tvi->setIcon(0, qIcon);
	}
	tvi->m_data = pItem;

	if( pParent )
	{
		QTreeWidgetItem * tviP = (QTreeWidgetItem *)pParent->m_hItem;
		if( tviP )
		{
			TVItem * tviC = (TVItem *)tviP->child(0);
			if( tviC && !tviC->m_data )
				delete tviC;
			
			if( pAfter )
			{
				// MBI !!!!!!!!!
			}
			else
				tviP->addChild(tvi);
		}
	}
	else
	{
		if( pAfter )
		{
			// MBI !!!!!!!!!
		}
		else
			addTopLevelItem(tvi);
	}
	
	if( pItem->m_nChildren )
	{
		TVItem * tviC = new TVItem();
		tvi->addChild(tviC);
	}

	pItem->m_hItem = (hTreeItem)tvi;
	return pItem;
}

cTreeItem * CTreeImpl::GetSelectionItem()
{
	return _GetItemData(currentItem());
}

void CTreeImpl::SelectItem(cTreeItem * pItem)
{
	if( pItem ) setCurrentItem((QTreeWidgetItem *)pItem->m_hItem);
}

void CTreeImpl::ExpandItem(cTreeItem * pItem, bool bExpand)
{
	if( !pItem )
		return;
	
	if( bExpand )
		expandItem((QTreeWidgetItem *)pItem->m_hItem);
	else
		collapseItem((QTreeWidgetItem *)pItem->m_hItem);
}

void CTreeImpl::DeleteItem(cTreeItem * pItem)
{
	if( pItem ) delete (QTreeWidgetItem *)pItem->m_hItem;
}

void CTreeImpl::SetItemCheckState(cTreeItem * pItem, bool bChecked)
{
	//if( pItem ) TreeView_SetCheckState(m_hWnd, (HTREEITEM)pItem->m_hItem, bChecked);
}

cTreeItem * CTreeImpl::GetParentItem(cTreeItem * pItem)
{
	if( !pItem )
		return NULL;
	return _GetItemData(((QTreeWidgetItem *)pItem->m_hItem)->parent());
}

cTreeItem * CTreeImpl::GetChildItem(cTreeItem * pItem)
{
	if( !pItem )
		return _GetItemData(topLevelItem(0));
	
	return _GetItemData(((QTreeWidgetItem *)pItem->m_hItem)->child(0));
}

cTreeItem * CTreeImpl::GetNextItem(cTreeItem * pItem)
{
	if( !pItem )
		return NULL;
	
	QTreeWidgetItem * tvi = (QTreeWidgetItem *)pItem->m_hItem;
	QTreeWidgetItem * tviP = tvi->parent();
	if( tviP )
		return _GetItemData(tviP->child(tviP->indexOfChild(tvi) + 1));
	
	return _GetItemData(topLevelItem(indexOfTopLevelItem(tvi) + 1));
}

cTreeItem * CTreeImpl::TreeHitTest(POINT& pt, UINT *pnFlags)
{
	return _GetItemData(itemAt(QPoint(pt.x, pt.y)));
}

void CTreeImpl::SortChildren(cTreeItem * pItem, bool bRecurse)
{
}

void CTreeImpl::UpdateView(int nFlags)
{
	update();
}

//////////////////////////////////////////////////////////////////////

cTreeItem * CTreeImpl::_GetItemData(QTreeWidgetItem * tvi)
{
	if( !tvi )
		return NULL;
	return ((TVItem *)tvi)->m_data;
}

void CTreeImpl::_on_itemExpanded(QTreeWidgetItem * tvi)
{
	cTreeItem * pItem = _GetItemData(tvi);
	if( !pItem )
		return;
	
	TVItem * tviC = (TVItem *)tvi->child(0);
	if( !tviC || tviC->m_data )
		return;
	
	delete tviC;

	tUINT nSubCount = 0;
	if( m_pSink )
		((CTreeItemSink *)m_pSink)->OnItemExpanding(pItem, nSubCount);
}

/*
//////////////////////////////////////////////////////////////////////
// CVListImpl

CVListImpl::CVListImpl() :
	m_hdrinit(0), m_bChecked(0), m_nItems(0), m_nSelCount(0)
{
	setModel(this);
	setRootIsDecorated(false);
	setItemsExpandable(false);
	setDragEnabled(false);
	setSelectionMode(ExtendedSelection);
	setSelectionBehavior(SelectRows);

	m_header = header();

	m_header->connect((QTreeView *)this, SIGNAL(clicked(const QModelIndex&)), (QTreeView *)this, SLOT(_on_selected(const QModelIndex&)));
	m_header->connect((QTreeView *)this, SIGNAL(activated(const QModelIndex&)), (QTreeView *)this, SLOT(_on_activated(const QModelIndex&)));
	
	m_header->connect(m_header, SIGNAL(sectionMoved(int,int,int)), (QTreeView *)this, SLOT(_on_hdr_sectionMoved(int,int,int)));
	m_header->connect(m_header, SIGNAL(sectionResized(int,int,int)), (QTreeView *)this, SLOT(_on_hdr_sectionResized(int,int,int)));
	m_header->connect(m_header, SIGNAL(sectionPressed(int)), (QTreeView *)this, SLOT(_on_hdr_sectionPressed(int)));
	m_header->connect(m_header, SIGNAL(sectionClicked(int)), (QTreeView *)this, SLOT(_on_hdr_sectionClicked(int)));
}

CVListImpl::~CVListImpl()
{
	setModel(NULL);
}

int CVListImpl::AddIcon(CIcon * pIcon)
{
	if( !pIcon )
		return -1;

	const SIZE& sz = pIcon->Size();
	return m_ImageList.AddIcon((QIcon *)pIcon->IconHnd(), QSize(sz.cx, sz.cy));
}

CIcon * CVListImpl::GetIcon(int nIndex)
{
	QIcon * qIcon = m_ImageList.GetIcon(nIndex);
	if( !qIcon )
		return NULL;
	return m_pRoot->CreateIcon((HICON)qIcon, true);
}

void CVListImpl::UpdateView(int nFlags)
{
	TBase::UpdateView(nFlags);
	if( m_nState & ISTATE_INITED )
		m_nSelCount = 0, doItemsLayout();
}

void CVListImpl::ApplyStyle(LPCTSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_CHECKED)        m_bChecked = 1; sbreak;
	scase(STR_PID_FIXEDCOLUMNS)   m_header->setDragEnabled(false); sbreak;
	scase(STR_PID_SINGLESEL)      setSelectionMode(SingleSelection); sbreak;
	send;
}

void CVListImpl::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);

	if( m_fNoHeader )
		m_header->setHidden(true);
}

//////////////////////////////////////////////////////////////////////

void CVListImpl::ctl_Redraw()
{
	if( m_nState & ISTATE_INITED )
		m_nSelCount = 0, doItemsLayout();
}

void CVListImpl::ctl_ColsUpdate()
{
	m_nSelCount = 0, doItemsLayout();
	
	m_hdrinit = 1;
	
	m_nColumnCount = 0;
	
	for(int i = 0, n = m_colsInfo.size(); i < n; i++)
	{
		CColInfo& _col = m_colsInfo[i];
		
		if( _col.m_nScale )
			_col.m_nWidth = ((m_szSize.cx - 4) * _col.m_nScale) / 100;
		if( _col.m_bIndex )
			m_nIndexCol = i;
		if( !_col.m_bHide )
			m_nColumnCount++;
		
		if( isColumnHidden(i) != _col.m_bHide )
			setColumnHidden(i, _col.m_bHide);

		int nDisplayPos = m_header->visualIndex(i);
		if( (nDisplayPos != -1) && (_col.m_nOrder != nDisplayPos) )
			m_header->moveSection(nDisplayPos, _col.m_nOrder);

		if( (m_header->sectionSize(i) != _col.m_nWidth) && _col.m_nWidth )
			m_header->resizeSection(i, _col.m_nWidth);
	}

	m_hdrinit = 0;
}

void CVListImpl::ctl_ColsResize()
{
	ctl_ColsUpdate();
}

int CVListImpl::ctl_GetNextSelectedItem(int nPrev)
{
	if( !m_pViewData )
		return -1;

	for(int i = ((nPrev == -1) ? 0 : (nPrev + 1)), n = m_pViewData->GetCount(); i < n; i++)
	{
		CRecord * pRecord = m_pViewData->GetRecord(i);
		if( pRecord->m_fSelected )
			return i;
	}

	return -1;
}

void CVListImpl::ctl_RedrawItem(int nItem)
{
	update();
}

//////////////////////////////////////////////////////////////////////

int CVListImpl::columnCount(const QModelIndex &parent) const
{
	return m_colsInfo.size();
}

QVariant CVListImpl::headerData(int section, Qt::Orientation orientation, int role) const
{
	if( m_fNoHeader )
		return QVariant();
	if( orientation != Qt::Horizontal )
		return QVariant();
	if( section >= m_colsInfo.size() )
		return QVariant();
	if( role != Qt::DisplayRole )
		return QVariant();

	const CColInfo& _col = m_colsInfo[section];
	return _S2Q(_col.m_sName.c_str());
}

int CVListImpl::rowCount(const QModelIndex &parent) const
{
	if( parent.isValid() )
		return 0;
	
	return m_nItems;
}

QVariant CVListImpl::data(const QModelIndex &index, int role) const
{
	if( !index.isValid() )
		return QVariant();

	if( !m_pViewData )
		return QVariant();

	int nCol = index.column();
	int nRow = index.row();
	
	if( role == Qt::CheckStateRole )
	{
		const CColInfo& _col = m_colsInfo[nCol];
		if( _col.m_bChecked || (m_bChecked && !nCol) )
			return ((CVListImpl *)this)->GetCheck(nRow, nCol);
		
		return QVariant();
	}

	if( role == Qt::DisplayRole )
	{
		CRecord * pRecord = m_pViewData->GetRecord(nRow, true);
		if( pRecord )
		{
			tCHAR * strText = pRecord->GetData(nCol);
			if( !strText )
				strText = "";

			return _S2Q(strText);
		}
	}

	if( role == Qt::DecorationRole && nCol == 0 )
	{
		CRecord * pRecord = m_pViewData->GetRecord(nRow, true);
		if( pRecord )
		{
			QIcon qIcon; m_ImageList.GetToIcon(pRecord->GetStatus(), qIcon);
			return qIcon;
		}
	}
	
	return QVariant();
}

bool CVListImpl::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if( role == Qt::CheckStateRole && m_pViewData )
	{
		int nCol = index.column();
		int nRow = index.row();
		
		SetCheck(nRow, nCol, value.toBool());
	}
	
	return QAbstractListModel::setData(index, value, role);
}

Qt::ItemFlags CVListImpl::flags(const QModelIndex &index) const
{
    if( !index.isValid() )
		return QAbstractListModel::flags(index);

	int nCol = index.column();

	const CColInfo& _col = m_colsInfo[nCol];

	Qt::ItemFlags _flags = Qt::ItemIsSelectable|Qt::ItemIsEnabled;
	if( _col.m_bChecked || (m_bChecked && !nCol) )
		_flags |= Qt::ItemIsUserCheckable;
	
	return _flags;
}

void CVListImpl::selectionChanged(const QItemSelection& selected, const QItemSelection &deselected)
{
	TBase::selectionChanged(selected, deselected);

	const QModelIndexList& _sels = selected.indexes();
	const QModelIndexList& _desels = deselected.indexes();

	if( m_pViewData )
	{
		int i;

		for(i = 0; i < _sels.size(); i++)
		{
			CRecord * pRecord = m_pViewData->GetRecord(_sels[i].row());
			if( !pRecord->m_fSelected )
			{
				pRecord->m_fSelected = 1;
				m_nSelCount++;
			}
		}
		
		for(i = 0; i < _desels.size(); i++)
		{
			CRecord * pRecord = m_pViewData->GetRecord(_desels[i].row());
			if( pRecord->m_fSelected )
			{
				pRecord->m_fSelected = 0;
				m_nSelCount--;
			}
		}
	}
	
	OnChangedState(this);
}

//////////////////////////////////////////////////////////////////////

void CVListImpl::_on_hdr_sectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
	if( m_hdrinit )
		return;
	
	for(int i = 0; i < m_colsInfo.size(); i++)
		m_colsInfo[i].m_nOrder = m_header->visualIndex(i);
}
*/

//////////////////////////////////////////////////////////////////////
// CEdit

CEdit::CEdit() :
	m_pE(NULL),
	m_bReadonly(0),
	m_bPassword(0),
	m_bNumeric(0),
	m_bMultiline(0),
	m_bScroll(1)
{
}

void CEdit::Cleanup()
{
	TBase::Cleanup();
	delete m_pE, m_pE = NULL;
}

void CEdit::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_READONLY)     m_bReadonly = 1; sbreak;
	scase(STR_PID_PASSWORD)     m_bPassword = 1; sbreak;
	scase(STR_PID_PASSWORDSET)  m_bPassword = 1; m_bDummyPasswdShow = 1; sbreak;
	scase(STR_PID_NUM)          m_bNumeric = 1; sbreak;
	scase(STR_PID_MULTILINE)    m_bMultiline = 1; sbreak;
	scase(STR_PID_NOSCROLL)     m_bScroll = 0; sbreak;
	send;
}

void CEdit::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);

	if( m_bMultiline )
	{
		m_pMLE = new QTextEdit();
		m_pMLE->setReadOnly(m_bReadonly);
		m_pMLE->setHorizontalScrollBarPolicy(m_bScroll ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
		m_pMLE->setVerticalScrollBarPolicy(m_bScroll ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
		if( !m_bBordered )
			m_pMLE->setFrameShape(QFrame::NoFrame);

		m_pMLE->connect(m_pMLE, SIGNAL(textChanged()), this, SLOT(ctl_MLE_OnDataChanged()));
	}
	else
	{
		m_pSLE = new QLineEdit();
		m_pSLE->setReadOnly(m_bReadonly);
		m_pSLE->setFrame(m_bBordered);
		if( m_bPassword )
			m_pSLE->setEchoMode(QLineEdit::Password);
		if( !m_bBordered )
			m_pMLE->setBackgroundRole(QPalette::Background);

		m_pSLE->connect(m_pSLE, SIGNAL(textChanged(const QString &)), this, SLOT(ctl_SLE_OnDataChanged(const QString &)));
	}

	m_pE->setParent(this);
}

void CEdit::GetAddSize(SIZE& szAdd)
{
	if( m_bBordered )
		szAdd.cy += 8;
	else
		TBase::GetAddSize(szAdd);
}

void CEdit::ctl_GetText(cStrObj& strText)
{
	const QString& qStr = m_bMultiline ? m_pMLE->toPlainText() : m_pSLE->text();
	_Q2S(qStr, strText);
}

void CEdit::ctl_SetText(LPCWSTR strText)
{
	if( m_bMultiline )
		m_pMLE->setPlainText(_S2Q(strText));
	else
		m_pSLE->setText(_S2Q(strText));
}

void CEdit::ctl_ReplaceText(LPCWSTR strText, tSIZE_T nFrom, tSIZE_T nTo)
{
}

void CEdit::ctl_ReplaceSelText(LPCWSTR strText)
{
}

void CEdit::ctl_SetSelText(tSIZE_T nFrom, tSIZE_T nTo, bool bAsync)
{
}

bool CEdit::ctl_IsMultiline() { return m_bMultiline; }
bool CEdit::ctl_IsNumber() { return m_bNumeric; }
bool CEdit::ctl_IsReadonly() { return m_bReadonly; }

void CEdit::resizeEvent(QResizeEvent * e)
{
	TBase::resizeEvent(e);
	if( m_pE )
		m_pE->resize(e->size());
}

//////////////////////////////////////////////////////////////////////
// CIPAddressCtrl

CIPAddressCtrl::CIPAddressCtrl()
{
	connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(ctl_OnDataChanged(const QString &)));
	setInputMask("000.000.000.000");
}

void CIPAddressCtrl::GetAddSize(SIZE& szAdd)
{
	szAdd.cx += 125; szAdd.cy += 8;
}

bool CIPAddressCtrl::SetValue(tPTR pValue)
{
	tDWORD dwIP[4];

	tBYTE * pIP = (tBYTE *)pValue;
	dwIP[0] = pIP[3];
	dwIP[1] = pIP[2];
	dwIP[2] = pIP[1];
	dwIP[3] = pIP[0];

	setText(QString().sprintf("%u.%u.%u.%u", dwIP[0], dwIP[1], dwIP[2], dwIP[3]));
	return false;
}

bool CIPAddressCtrl::GetValue(tPTR pValue)
{
	tDWORD dwIP[4];
	sscanf(text().toLocal8Bit(), "%u.%u.%u.%u", &dwIP[0], &dwIP[1], &dwIP[2], &dwIP[3]);

	tBYTE * pIP = (tBYTE *)pValue;
	pIP[3] = (tBYTE)dwIP[0];
	pIP[2] = (tBYTE)dwIP[1];
	pIP[1] = (tBYTE)dwIP[2];
	pIP[0] = (tBYTE)dwIP[3];
	return true;
}

//////////////////////////////////////////////////////////////////////
// CRichEditImpl

CRichEditImpl::CRichEditImpl() :
	m_bNoFocus(0)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAcceptRichText(true);
}

void CRichEditImpl::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_VSCROLL)     setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); sbreak;
	scase(STR_PID_HSCROLL)     setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); sbreak;
	scase(STR_PID_READONLY)    setReadOnly(true); sbreak;
	//scase("ralign")            m_nStyles |= ES_RIGHT; sbreak;
	//scase(STR_PID_TRANSPARENT) m_nStylesEx |= WS_EX_TRANSPARENT; sbreak;
	scase(STR_PID_NOFOCUS)     m_bNoFocus = 1; sbreak;
	send;
}

void CRichEditImpl::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);
	
	if( !m_bBordered )
		setFrameShape(QFrame::NoFrame);
}

int __GetCharsLen(CRootItem * pRoot, LPSTR strText, int n)
{
	tCHAR c = strText[n];
	strText[n] = 0;
	
	LPCWSTR strUni = pRoot->LocalizeStr(TOR_cStrObj, strText);
	int nLen = wcslen(strUni);
	
	strText[n] = c;
	return nLen;
}

void CRichEditImpl::OnApplyBlockStyles()
{
	QTextCursor cursor(textCursor());
	
	QBrush linkClr(QColor(0, 0, 255));
	
	QTextCharFormat normalFormat;

	QTextCharFormat linkFormat;
	linkFormat.setFontUnderline(true);
	linkFormat.setForeground(linkClr);
	linkFormat.setAnchor(true);
	linkFormat.setAnchorHref(tr("anch_ref"));
	linkFormat.setAnchorName(tr("anch_name"));
	
	QTextCharFormat boldFormat;
	boldFormat.setFontWeight(QFont::Bold);

	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
	cursor.setCharFormat(normalFormat);

	LPSTR strDisplayText = &m_strDisplayText[0];
	int i;
	
	for(i = 0; i < m_aRefs.size(); i++)
	{
		POINT& _chrrRef = m_aRefs[i]->m_chrRange;
		POINT& _chrrRefEx = m_aRefs[i]->m_chrRangeEx;

		_chrrRefEx.x = __GetCharsLen(m_pRoot, strDisplayText, _chrrRef.x);
		_chrrRefEx.y = __GetCharsLen(m_pRoot, strDisplayText, _chrrRef.y);
		
		cursor.setPosition(_chrrRefEx.x);
		cursor.setPosition(_chrrRefEx.y, QTextCursor::KeepAnchor);
		cursor.mergeCharFormat(linkFormat);
	}

	for(i = 0; i < m_aBolds.size(); i++)
	{
		POINT& _chrrBold = m_aBolds[i];

		POINT _chrr;
		_chrr.x = __GetCharsLen(m_pRoot, strDisplayText, _chrrBold.x);
		_chrr.y = __GetCharsLen(m_pRoot, strDisplayText, _chrrBold.y);
		
		cursor.setPosition(_chrr.x);
		cursor.setPosition(_chrr.y, QTextCursor::KeepAnchor);
		cursor.mergeCharFormat(boldFormat);
	}

	cursor.clearSelection();
}

void CRichEditImpl::OnCalcTextSize(HDC dc, SIZE& sz, tDWORD nFlags)
{
	sz.cy = 100;
}

//////////////////////////////////////////////////////////////////////

/*
void CRichEditImpl::focusInEvent(QFocusEvent * e)
{
	if( !m_bNoFocus )
		TBase::focusInEvent(e);
}

void CRichEditImpl::focusOutEvent(QFocusEvent * e)
{
	if( !m_bNoFocus )
		TBase::focusOutEvent(e);
}

void CRichEditImpl::enterEvent(QEvent * e)
{
	if( !m_bNoFocus )
		TBase::enterEvent(e);
}

void CRichEditImpl::leaveEvent(QEvent * e)
{
	if( !m_bNoFocus )
		TBase::leaveEvent(e);
}
*/

void CRichEditImpl::mouseDoubleClickEvent(QMouseEvent * e)
{
	if( !m_bNoFocus )
		TBase::mouseDoubleClickEvent(e);
}

void CRichEditImpl::mouseMoveEvent(QMouseEvent * e)
{
	if( !m_bNoFocus )
		TBase::mouseMoveEvent(e);
}

void CRichEditImpl::mouseReleaseEvent(QMouseEvent * e)
{
	int nCharPos = cursorForPosition(e->pos()).position();
	for(int i = 0; i < m_aRefs.size(); i++)
	{
		RefData * pRef = m_aRefs[i];
		if( (nCharPos >= pRef->m_chrRangeEx.x) && (nCharPos <= pRef->m_chrRangeEx.y) )
		{
			pRef->_GetItem()->Click();
			break;
		}
	}

	TBase::mouseReleaseEvent(e);
}

//////////////////////////////////////////////////////////////////////
// CSlider

CSlider::CSlider()
{
	setOrientation(Qt::Horizontal);
	setTickPosition(QSlider::TicksBelow);
}

void CSlider::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_VERTICAL) setOrientation(Qt::Vertical); setTickPosition(QSlider::TicksBothSides); sbreak;
	send;
}

void CSlider::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(ctl_OnValueChanged(int)));
}

void CSlider::ctl_SetRange(tSIZE_T nMin, tSIZE_T nMax) { setRange(nMin, nMax); }
void CSlider::ctl_SetPos(tSIZE_T nPos) { setValue(nPos); }
tSIZE_T  CSlider::ctl_GetPos() { return value(); }
bool CSlider::ctl_IsVert() { return orientation() == Qt::Vertical; }

//////////////////////////////////////////////////////////////////////
// CPopupMenu

HMENU CPopupMenu::ctl_GetRootItem()
{
	if( !m_pMenu )
		m_pMenu = new QMenu();
	return (HMENU)m_pMenu;
}

HMENU CPopupMenu::ctl_AddItem(HMENU hMenu, CItemBase * pItem, bool bSub)
{
	if( !hMenu )
		return NULL;
	
	QMenu * pMenu = (QMenu *)hMenu;
	
	if( pItem == POPUPMENU_SEP )
	{
		pMenu->addSeparator();
		return NULL;
	}

	QIcon * pIcon = NULL;
	if( pItem->m_pIcon && pItem->m_pIcon->IconHnd() )
		pIcon = (QIcon *)pItem->m_pIcon->IconHnd();

	const QString& qsText = _S2Q(pItem->m_strText.c_str());

	QMenu * pSubMenu = NULL;
	if( bSub )
	{
		if( pIcon )
			pSubMenu = pMenu->addMenu(*pIcon, qsText);
		else
			pSubMenu = pMenu->addMenu(qsText);
	}
	else
	{
		QAction * pAction = NULL;
		if( pIcon )
			pAction = pMenu->addAction(*pIcon, qsText);
		else
			pAction = pMenu->addAction(qsText);
		
		if( pAction )
		{
			pAction->setData(pItem->GetItemId());
			
			if( pItem->m_nState & ISTATE_DISABLED )
				pAction->setEnabled(false);
			
			if( pItem->m_nState & ISTATE_SELECTED )
			{
				pAction->setCheckable(true);
				pAction->setChecked(true);
			}

			if( pItem->m_nState & ISTATE_DEFAULT )
				pMenu->setDefaultAction(pAction);
		}
	}
	
	return (HMENU)pSubMenu;
}

tDWORD CPopupMenu::ctl_Track(POINT& pt)
{
//	if( !m_pMenu )
		return -1;
	
	QWidget * pParent = GetParentQW(m_pParent);
	if( !pParent )
		pParent = QApplication::desktop();
	
	m_pMenu->setParent(pParent);
	
	QAction * pAction = m_pMenu->exec(QPoint(pt.x, pt.y));
	if( !pAction )
		return -1;
	
	return pAction->data().toInt();
}

void CPopupMenu::ctl_TrackEnd()
{
}

//////////////////////////////////////////////////////////////////////
// CDialogItemImpl

CDialogItemImpl::CDialogItemImpl() :
	m_bEnableParent(0), m_bCloseCalled(0), m_bResizing(0)
{
	m_nStyles = Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::FramelessWindowHint;
	setMouseTracking(true);
}

void CDialogItemImpl::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);
	setWindowFlags(m_nStyles);

	if( m_nStyles & Qt::WindowMaximizeButtonHint )
		setSizeGripEnabled(true);
}

void CDialogItemImpl::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase("nocaption")   m_nStyles &= ~(Qt::WindowTitleHint|Qt::WindowSystemMenuHint); sbreak;
	scase("resizable")   m_nStyles &= ~(Qt::FramelessWindowHint); m_nStyles |= Qt::WindowMaximizeButtonHint; sbreak;
	scase("minimizable") m_nStyles |= Qt::WindowMinimizeButtonHint; sbreak;
	scase("ontop")       m_nStyles |= Qt::WindowStaysOnTopHint; sbreak;
	scase("layered")     m_nStyles |= Qt::WindowStaysOnTopHint|Qt::Popup; sbreak;
	send
}

void CDialogItemImpl::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);

	if( m_pDlgIcon )
	{
		QIcon qIcon;

		CIcon * pDlgIcons[] = {m_pDlgIcon, m_pRoot->GetIcon(ICONID_DEFAULT".big")};
		for(int i = 0; i < countof(pDlgIcons); i++)
		{
			CIcon * pIcon = pDlgIcons[i];
			if( !pIcon )
				continue;
			
			QIcon * pqIcon = (QIcon *)pIcon->IconHnd();
			if( !pqIcon )
				continue;

			const SIZE szIcon = pIcon->Size();
			qIcon.addPixmap(pqIcon->pixmap(szIcon.cx, szIcon.cy));
		}
		
		setWindowIcon(qIcon);
	}
}

bool CDialogItemImpl::IsActive()
{
	return isActiveWindow();
}

void CDialogItemImpl::Activate()
{
	if( !isVisible() )
		m_bMaximize ? QDialog::showMaximized() : QDialog::showNormal();
	m_bMaximize = 0;

	_SetTransparent(IsActual() ? TRANS_FULL : TRANS_MED);

	if( !(m_nDlgStyles & DLG_STYLE_NOAUTOFOCUS) )
		activateWindow();

	if( !(m_nDlgStyles & DLG_STYLE_NOAUTOFOCUS) )
		setFocus();
}

void CDialogItemImpl::GetParentRect(RECT& rc, CItemBase * pOwner)
{
	if( pOwner != m_pRoot )
	{
		TBase::GetParentRect(rc, pOwner);
		return;
	}

	QPoint pt = mapToGlobal(QPoint(0, 0));

	rc.left = pt.x();
	rc.top = pt.y();
	rc.right = rc.left + m_szSize.cx;
	rc.bottom = rc.top + m_szSize.cy;
}

void CDialogItemImpl::ModalEnableParent(bool bEnable)
{
	QWidget * hParent = parentWidget();
	if( !hParent )
		return;

	if( bEnable )
	{
		if( isActiveWindow() )
			hParent->activateWindow();
	}
	else
	{
		setModal(true);
		setAttribute(Qt::WA_ShowModal, true);
		setAttribute(Qt::WA_WState_Visible, false);
		setAttribute(Qt::WA_WState_Hidden, true);
		setVisible(true);
	}
}

void CDialogItemImpl::Close(tDWORD nAction)
{
	m_nResult = nAction;
	m_bCloseDirect = 1;

	if( ((CUniRoot *)m_pRoot)->m_bActive )
		m_bUserClose=1;
	
	_CloseDlg();
}

void CDialogItemImpl::OnInited()
{
	TBase::OnInited();

	SIZE szWndMin; GetMinSize(szWndMin);
	
	QSize qszMin(szWndMin.cx, szWndMin.cy);
	QSize qszMax(MAX_CTL_SIZE, MAX_CTL_SIZE);
	
	if( !IsResizeble(true) )
		qszMax.setWidth(qszMin.width());
	
	if( !IsResizeble(false) )
		qszMax.setHeight(qszMin.height());

	setMinimumSize(qszMin);
	setMaximumSize(qszMax);
}

void CDialogItemImpl::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	if( m_bResizing )
		flags &= ~UPDATE_FLAG_SIZE;
	
	TBase::OnUpdateProps(flags & ~UPDATE_FLAG_ICON, nStateMask & ~ISTATE_HIDE, rc);

	if( (nStateMask & ISTATE_HIDE) && (m_nState & ISTATE_HIDE) )
		setVisible(false);
}

//////////////////////////////////////////////////////////////////////

void   CDialogItemImpl::ctl_SetTransparentValue(tBYTE nAlpha) { /*setWindowOpacity(((qreal)nAlpha)/0xFF);*/ }
tDWORD CDialogItemImpl::ctl_SetTransparentTimer(tDWORD nInterval) { return startTimer(nInterval); }
void   CDialogItemImpl::ctl_KillTransparentTimer(tDWORD nId) { killTimer(nId); }

//////////////////////////////////////////////////////////////////////

void CDialogItemImpl::reject()
{
	m_bUserClose = 1;
	TBase::reject();
}

void CDialogItemImpl::done(int r)
{
	if( !m_bCloseCalled && !_CloseDlg() )
		return;
	
	if( m_pParent )
	{
		m_pParent->RemoveItem(this);
		m_nState |= ISTATE_CLOSING;
		m_pRoot->PostAction(ITEM_ACTION_DESTROY, this);
	}
	
	TBase::done(r);
}

bool CDialogItemImpl::event(QEvent * e)
{
	return TBase::event(e);
}

void CDialogItemImpl::resizeEvent(QResizeEvent * e)
{
	{
		SIZE l_szDiff = {e->size().width() - m_szSize.cx, e->size().height() - m_szSize.cy};
		
		if( l_szDiff.cx || l_szDiff.cy )
		{
			LockUpdate(true);
			
			m_bResizing = 1;
			Resize(l_szDiff.cx, l_szDiff.cy, false);
			m_bResizing = 0;
			
			LockUpdate(false);
		}
	}

	{
		SIZE l_szWidgetDiff = {e->size().width()  - e->oldSize().width(), e->size().height() - e->oldSize().height()};
	
		if( (l_szWidgetDiff.cx || l_szWidgetDiff.cy) && (m_nState & ISTATE_INITED) &&
			(m_nDlgStyles & DLG_STYLE_BOTTOM) )
		{
			pos().setX(pos().x() - l_szWidgetDiff.cx);
			pos().setY(pos().y() - l_szWidgetDiff.cy);
			
			move(pos());
		}
	}

	if( m_nDlgStyles & DLG_STYLE_LAYERED )
	{
		if( !GUI_ISSTYLE_SIMPLE(m_pRoot) || m_pRoot->IsThemed() )
		{
			QRect qrFrame = frameGeometry();
			qrFrame.translate(-qrFrame.left(), -qrFrame.top());
			
			QRegion rgn(qrFrame);
			{
				#define DLG_REGION_CIRCLE   6
				#define DLG_REGION_C        (2*DLG_REGION_CIRCLE)
				#define DLG_REGION_S        DLG_REGION_CIRCLE
				
				QRegion qrSquare(0, 0, DLG_REGION_S, DLG_REGION_S, QRegion::Rectangle);
				QRegion qrCircle(0, 0, DLG_REGION_C, DLG_REGION_C, QRegion::Ellipse);

				rgn -= qrSquare; rgn += qrCircle;

				qrSquare.translate(qrFrame.width() - DLG_REGION_S, 0);
				qrCircle.translate(qrFrame.width() - DLG_REGION_C, 0);

				rgn -= qrSquare; rgn += qrCircle;

				qrSquare.translate(0, qrFrame.height() - DLG_REGION_S);
				qrCircle.translate(0, qrFrame.height() - DLG_REGION_C);

				rgn -= qrSquare; rgn += qrCircle;

				qrSquare.translate(DLG_REGION_S - qrFrame.width(), 0);
				qrCircle.translate(DLG_REGION_C - qrFrame.width(), 0);

				rgn -= qrSquare; rgn += qrCircle;
			}
			
			setMask(rgn);
		}
		else
			clearMask();
	}

//	if( m_nDlgStyles & DLG_STYLE_DRAGABLE )
//		setbuddy()
	
	//setWindowIcon
	//setWindowOpacity(0.5);
	TBase::resizeEvent(e);
}

void CDialogItemImpl::timerEvent(QTimerEvent * e)
{
	ctl_OnTransparentTimer();
	QObject::timerEvent(e);
}

//////////////////////////////////////////////////////////////////////

void CDialogItemImpl::_AdjustFrameRect(RECT& rc, bool bInclude)
{
	const QRect& qrcNoBorder = geometry();
	const QRect& qrcWithBorder = frameGeometry();

	RECT rcDiff =
	{
		qrcNoBorder.left() - qrcWithBorder.left(),
		qrcNoBorder.top() - qrcWithBorder.top(),
		
		qrcWithBorder.right() - qrcNoBorder.right(),
		qrcWithBorder.bottom() - qrcNoBorder.bottom()
	};
	
	if( bInclude )
	{
		rc.left   -= rcDiff.left;
		rc.right  += rcDiff.right;
		rc.top    -= rcDiff.top;
		rc.bottom += rcDiff.bottom;
	}
	else
	{
		rc.left   += rcDiff.left;
		rc.right  -= rcDiff.right;
		rc.top    += rcDiff.top;
		rc.bottom -= rcDiff.bottom;
	}
}

void CDialogItemImpl::OnAdjustBordSize(RECT& rc)
{
	_AdjustFrameRect(rc, true);
}

bool CDialogItemImpl::_CloseDlg()
{
	bool bUserClose = m_bUserClose;
	bool bAppClose = !m_pRoot->m_bActive;
	bool bAbandoned = false;
	
	if( (!m_bUserClose) && (m_nResult == DLG_ACTION_CANCEL) )
		bAbandoned = true;
	
	m_bUserClose = 0;
	
	if( m_pSink )
	{
		if( !m_bCloseDirect && !m_pSink->OnCanClose(bUserClose) )
			return false;
		if( !bAppClose && !((CDialogSink *)m_pSink)->OnClose(m_nResult) )
			return false;
	}
	
	if( bAbandoned )
		SetErrResult(errABANDONED);

	ModalEnableParent(true);
	
	if( !m_bCloseCalled )
	{
		m_bCloseCalled = 1;
		close();
	}
	
	return true;
}

void CDialogItemImpl::_InitDlgRect(RECT& rcFrame)
{
	bool bResize = true;
	if( !(m_nDlgStyles & DLG_STYLE_RESIZABLE) || (m_nDlgStyles & DLG_STYLE_LAYERED) )
		bResize = false;

	RECT rcGeom = rcFrame; _AdjustFrameRect(rcGeom, false);
	setGeometry(QRect(rcGeom.left, rcGeom.top, RECT_CX(rcGeom), RECT_CY(rcGeom)));
}

//////////////////////////////////////////////////////////////////////

bool CDialogItem::_GetWindowPlacement(HWND hWnd, RECT& rcNormal, eShow& showCmd)
{
	if( !hWnd )
		return false;
	
	QWidget * pW = (QWidget *)hWnd;
	
	const QRect& rcW = pW->normalGeometry();
	rcNormal.left = rcW.left();
	rcNormal.top = rcW.top();
	rcNormal.right = rcW.right();
	rcNormal.bottom = rcW.bottom();
	((CDialogItemImpl *)this)->_AdjustFrameRect(rcNormal, true);
	
	showCmd = pW->isMaximized() ? showMaximazed : showNormal;
	return true;
}

bool CDialogItem::_GetGetWorkArea(RECT& rc)
{
	QWidget * pScreen = QApplication::desktop();
	if( !pScreen )
		return false;
	
	const QRect& qrcScreen = pScreen->frameGeometry();
	rc.left = qrcScreen.left();
	rc.top = qrcScreen.top();
	rc.right = qrcScreen.right();
	rc.bottom = qrcScreen.bottom();
	return true;
}

void CDialogItem::_EnableSysClose(HWND hWnd, bool bEnable)
{
}

tDWORD CDialogItem::_GetSameWindowOffset()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////

CItemBase * CUniRoot::CreateItem(LPCSTR strId, LPCSTR strType)
{
	sswitch(strType)
	scase(GUI_ITEMT_BUTTON)                return new CButton();                sbreak;
	scase(GUI_ITEMT_GROUP)                 return new CGroupBox();              sbreak;
	scase(GUI_ITEMT_BOX)                   return new CGroupBox();              sbreak;
	scase(GUI_ITEMT_BITMAPBUTTON)          return new CBitmapButton();          sbreak;
	scase(GUI_ITEMT_CHECK)                 return new CCheck();                 sbreak;
	scase(GUI_ITEMT_RADIO)                 return new CRadio();                 sbreak;
	scase(GUI_ITEMT_HOTLINK)               return new CHotlink();               sbreak;
	scase(GUI_ITEMT_PROGRESS)              return new CProgress();              sbreak;
	scase(GUI_ITEMT_COMBO)                 return new CCombo();                 sbreak;
	scase(GUI_ITEMT_TAB)                   return new CTab();                   sbreak;
//	scase(GUI_ITEMT_LISTCTRL)              return new CVListImpl();             sbreak;
	scase(GUI_ITEMT_TREE)                  return new CTreeImpl();              sbreak;
	scase(GUI_ITEMT_EDIT)                  return new CEdit();                  sbreak;
	scase(GUI_ITEMT_TIMEEDIT)              return new CTimeEditBox();           sbreak;
	scase(GUI_ITEMT_RICHEDIT)              return new CRichEditImpl();          sbreak;
	scase(GUI_ITEMT_SLIDER)                return new CSlider();                sbreak;
	scase(GUI_ITEMT_IPADDRESS)             return new CIPAddressCtrl();         sbreak;
	scase(GUI_ITEMT_MENU)                  return new CPopupMenu();             sbreak;
	scase(GUI_ITEMT_DIALOG)                return new CDialogItemImpl();        sbreak;
//	scase(GUI_ITEMT_TRAY)                  return new CTrayIcon();              sbreak;
	scase(GUI_ITEMT_CREDITS)               return NULL;                         sbreak;
	
	scase(GUI_ITEMT_LOCKBUTTON)            return NULL;                         sbreak;
	scase(GUI_ITEMT_PROPSHEETPAGE)         return NULL;                         sbreak;
	send;

	return CRootItem::CreateItem(strId, strType);
}

//////////////////////////////////////////////////////////////////////

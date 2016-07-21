//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <ProductCore/ItemBase.h>
#include <ProductCore/binding.h>
#include <ProductCore/DataSource.h>

#include <Prague/iface/i_csect.h>

#include <algorithm>
#include <math.h>
#include <wctype.h>
#include "Formats.h"
//////////////////////////////////////////////////////////////////////

#define RECORD_CACHE_MAXCOUNT	    5000
#define RECORD_ASYNC_SORT_COUNT	    3000
#define RECORD_CACHE_SORT_MAXCOUNT	100000

#define PROFILE_COLUMNS		    "Columns"

#define ITEMID_SORTUP		    "ListCtrl_SortUp"
#define ITEMID_SORTDOWN		    "ListCtrl_SortDown"
#define ITEMID_FIND             "ListCtrl_Find"
#define ITEMID_SELECTALL        "ListCtrl_SelectAll"
#define ITEMID_COPY             "ListCtrl_Copy"
#define ITEMID_AUTOSCROLL       "ListCtrl_AutoScroll"

#define ITEMID_HDRPOPUPMENU     "ListCtrl_HdrPopup"
#define ITEMID_POPUPMENU        "ListCtrl_Popup"
#define ITEMID_FIND_DIALOG      "ListCtrl_Find"

#define STRID_NOTFOUND          "NotFound"

#define EVENT_ID_SORTDONE       0x7a31cb0e
#define EVENT_ID_SORTPROGRESS   0x11298adf

/*
#ifndef WIN32
#include <ctype.h>
char * _strupr(char *x){
	char *y=x;
	while(*y){
		*y=toupper(*y);
		y++;
	}
	return x;
} 
#endif 	
*/

//////////////////////////////////////////////////////////////////////
// CFindDataCtx

class CFindDataCtx : public CDialogSink
{
public:
	CFindDataCtx(CListItem * pList);

	void OnInit();
	bool OnClicked(CItemBase * pItem);
	void OnChangedData(CItemBase * pItem);
	void UpdateData(bool bIn);

public:
	CListItem *      m_pList;
	cStrObj          m_strText;
	tBOOL            m_bFindDown;
	tBOOL            m_bMatchCase;
	tBOOL            m_bWholeWord;
	int              m_nColumn;
	bool             m_bEnable;
	CComboItem *     m_pColumns;
	CItemBase *      m_pFindEdit;
	CItemBase *      m_pMatchCase;
	CItemBase *      m_pWholeWord;
	CItemBase *      m_pFindBtn;
	CItemBase *      m_pMarkBtn;
};

//////////////////////////////////////////////////////////////////////
// CSortProgress

class CSortProgress :
	public CDataSource,
	public CRootTaskBase
{
public:
	CSortProgress(CListItem * pList, CDataSource& c);
	~CSortProgress();

	void Start();
	void Stop();
	void Progress() {}

	int               m_nSortId;
	int               m_nSortRows;
	int               m_nSortCol;
	eSortType_t       m_eSortOrder;
	int               m_nSortOperations;
	int               m_nSortCompletion;
	bool              m_fSortCanceled;
				      
	int               m_nPrevSortCol;
	eSortType_t       m_ePrevSortOrder;

	hCRITICAL_SECTION m_hCS;

protected:
	void   do_work();
	void   on_exit();

private:
	CListItem *       m_pList;
	int               m_nHeight;
	int               m_nSortWork;
};

//////////////////////////////////////////////////////////////////////
// CListItem

CListItem::CListItem() :
	m_nIndexCol(-1),
	m_pReader(NULL),
	m_pViewData(NULL),
	m_pSortData(NULL),
	m_pFindData(NULL),
	m_fSorted(0),
	m_fReadOnly(0),
	m_fUpdateCols(0),
	m_fAutoScroll(0),
	m_nColumnCount(0),
	m_nCurSortCol(0),
	m_fSingleSel(0),
	m_fNoHeader(0),
	m_fBlockAutoScroll(0),
	m_fAutoScrollActive(0),
	m_eCurSortOrder(eNotSorted),
	m_fOpSelItemsUpdateNeed(0)
{
	m_nFlags |= STYLE_TRANSPARENT|STYLE_LIST_ITEM;
}

void CListItem::Cleanup()
{
	ClearSource();

	if( m_pFindData )
		delete m_pFindData;

	TBase::Cleanup();
}

void CListItem::ClearCache() { m_pViewData->ClearCache(); }

void CListItem::ClearList()
{
	ctl_DeleteAllItems();
	InitData();
}

//!Функция сохраняет индексы выбранных записей в массив
void CListItem::SaveState(std::vector<int>& sels)
{
	if ( !m_pViewData)
		return;
	int count = m_pViewData ->GetCount();
	for( int i = 0; i<count; ++i )
		if ( m_pViewData ->GetRecord( i ) ->m_fSelected )
			sels.push_back(i);
}
//!Функция восстанавливает индексы выбранных записей из массива
void CListItem::RestoreState(const std::vector<int>& sels)
{
	if ( !m_pViewData || sels.empty())
		return;
	for(size_t i = 0; i < sels.size(); ++i)
		m_pViewData->GetRecord(sels[i])->m_fSelected = 1;
}

bool CListItem::InitData()
{
	ClearSource();

	m_pReader = (CListItemSink*)m_pSink;

//	m_pViewData = new CDataSource(this, RECORD_CACHE_MAXCOUNT);
	UpdateView(UPDATE_FLAG_REDRAW|UPDATE_FLAG_SORT|UPDATE_FLAG_SOURCE|UPDATE_FLAG_FORCE);
	return true;
}

void CListItem::OnInit()
{
	TBase::OnInit();
	
	if( m_aItems.size() )
	{
		for(size_t i = 0; i < m_aItems.size(); i++)
		{
			CItemBase* pItem = m_aItems[i];

			int nWidth = 0, nScale = 0;
			if( pItem->m_nFlags & AUTOSIZE_PARENT_X )
				nScale = pItem->m_szSizeMin.cy;
			else
				nWidth = pItem->m_szSizeMin.cx;

			bool bHide = !!(pItem->m_nState & ISTATE_HIDDENEX);
			bool bChecked = !!(pItem->m_nState & ISTATE_SELECTED);
			bool bIndex = !!(pItem->m_nState & ISTATE_DEFAULT);
			AddColumn(GetPlainText(pItem->m_strText.c_str()).c_str(), nWidth, nScale, bIndex, bHide, bChecked);
		}
	}
	else if( m_fNoHeader )
		AddColumn(NULL, 0);
}

void CListItem::OnClicked(CItemBase * pItem)
{
	CItemBase * pHdrMenu = pItem->GetOwner(false, ITEMID_HDRPOPUPMENU);
	if( pHdrMenu )
	{
		sswitch(pItem->m_strItemId.c_str())
		scase(ITEMID_SORTUP)   SortItems(m_nClickCol, eSortUp); sbreak;
		scase(ITEMID_SORTDOWN) SortItems(m_nClickCol, eSortDown); sbreak;
		scase(ITEMID_FIND)     FindItem(eShowDlg, m_nClickCol); sbreak;
		sdefault()             ShowColumn((int)pItem->m_pUserData, !!(pItem->m_nState & ISTATE_SELECTED) ); sbreak;
		send;
		return;
	}

	sswitch(pItem->m_strItemId.c_str())
	scase(ITEMID_FIND)      FindItem(eShowDlg); return; sbreak;
	scase(ITEMID_SELECTALL) SelectAll(); return; sbreak;
	scase(ITEMID_COPY)      CopyToClipboard(); return; sbreak;
	
	scase(ITEMID_AUTOSCROLL)
		if( tSIZE_T nItems = ctl_GetItemCount() )
			ctl_SetItemState(nItems - 1, ISTATE_FOCUSED, ISTATE_FOCUSED);
		ctl_ScrollDown();
		sbreak;
	send;

	TBase::OnClicked(pItem);
}

bool CListItem::OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem)
{
	pMenu->AddFromSection(ITEMID_POPUPMENU);

	bool bSomeItems = !!GetItemsCount();
	
	pMenu->Enable(ITEMID_FIND, bSomeItems);
	pMenu->Enable(ITEMID_COPY, GetNextSelectedItem() != -1);
	pMenu->Enable(ITEMID_SELECTALL, bSomeItems);
	pMenu->Show(ITEMID_SELECTALL, !m_fSingleSel);
	pMenu->Show(ITEMID_AUTOSCROLL, !!m_fAutoScroll);
	return true;
}

void CListItem::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_SORTED)         m_fSorted = 1; sbreak;
	scase(STR_PID_READONLY)       m_fReadOnly = 1; sbreak;
	scase(STR_PID_AUTOSCROLL)     m_fAutoScroll = 1; sbreak;
	scase(STR_PID_SINGLESEL)      m_fSingleSel = 1; sbreak;
	scase(STR_PID_NOHEADER)       m_fNoHeader = 1; sbreak;
	send;
}

void CListItem::Resize(tDWORD cx, tDWORD cy, bool bDirect)
{
	TBase::Resize(cx, cy, bDirect);

	if( cx )
		ctl_ColsResize();
}

void CListItem::SaveSettings(cAutoRegKey& pKey)
{
	size_t nCount = m_colsInfo.size();
	if( !nCount )
		return;

	int nColData[RECORD_MAX_FIELDS][4];
	for(size_t i = 0; i < nCount; i++)
	{
		CColInfo& pInfo = m_colsInfo[i];
		nColData[i][0] = pInfo.m_nOrder;
		nColData[i][1] = pInfo.m_nScale;
		nColData[i][2] = pInfo.m_nWidth;
		nColData[i][3] = pInfo.m_bHide;
	}

	tString strKey; GetOwnerRelativePath(strKey, PROFILE_COLUMNS);
	pKey.set_val(strKey.c_str(), tid_BINARY, (tPTR)nColData, (tDWORD)(sizeof(int)*nCount*4));
}

void CListItem::LoadSettings(cAutoRegKey& pKey)
{
	int nColData[RECORD_MAX_FIELDS][4];
	tDWORD nSize = sizeof(nColData);

	tString strKey; GetOwnerRelativePath(strKey, PROFILE_COLUMNS);
	if( PR_FAIL(pKey.get_val(strKey.c_str(), tid_BINARY, (tPTR)nColData, &nSize)) )
		return;

	nSize /= sizeof(int)*4;

	size_t nCount = min(m_colsInfo.size(), nSize);
	for(size_t i = 0; i < nCount; i++)
	{
		if( (size_t)nColData[i][0] >= nCount )
			continue;

		CColInfo& pInfo = m_colsInfo[i];
		pInfo.m_nOrder = nColData[i][0];
		pInfo.m_nScale = nColData[i][1];
		pInfo.m_nWidth = nColData[i][2];
		pInfo.m_bHide = !!nColData[i][3];
	}

	ctl_ColsUpdate();
}

void CListItem::SendEvent(tDWORD nEventId, cSerializable * pData, bool bBroadcast)
{
	if( nEventId == EVENT_ID_SORTDONE && pData && pData->isBasedOn(cGuiParams::eIID) )
	{
		cGuiParams * _prm = (cGuiParams *)pData;
		
		if( !(!m_pSortData || m_pSortData->m_nSortId != (int)_prm->m_nVal1 || m_pSortData->m_fSortCanceled) )
		{
			UpdateSortStatus();
			UpdateState();
			ctl_Redraw();
			delete m_pSortData, m_pSortData = NULL;
		}
	}

	if( nEventId == EVENT_ID_ICONEXTRUCTED )
		UpdateView(UPDATE_FLAG_SOURCE);

	TBase::SendEvent(nEventId, pData, bBroadcast);
}

void CListItem::TimerRefresh(tDWORD nTimerSpin)
{
	TBase::TimerRefresh(nTimerSpin);

	if( GUI_TIMERSPIN_INTERVAL(400) && m_pSortData )
		UpdateSortStatus(GUI_TIMERSPIN_INTERVAL(800));
}

void CListItem::UpdateView(int nFlags)
{
/*	if( !m_pReader || !m_pViewData )
		return;

	if( m_fOpSelItemsUpdateNeed )
	{
		m_fOpSelItemsUpdateNeed = 2;
		m_nOpSelItemsUpdateFlags = nFlags;
		return;
	}
	
	if( nFlags & UPDATE_FLAG_REINIT )
	{
		int nFocusItem = GetItemFocus();
		std::vector<int> sels;
		SaveState(sels);//запоминаем селекцию
		InitData();

		int nCount = m_pViewData->GetCount();
		if( nFocusItem >= nCount )
			nFocusItem = nCount-1;

		if( m_fAutoScrollActive )
		{
			ctl_SetItemState(nCount - 1, 0, ISTATE_FOCUSED);
			ctl_ScrollDown();
		}
		else if( nFocusItem != -1 )
			SetItemFocus(nFocusItem);
		RestoreState(sels);//восстанавливаем селекцию
		return;
	}

	if( !(nFlags & UPDATE_FLAG_FORCE) && !m_pReader->IsDataChanged() )
		return;

	if( nFlags & UPDATE_FLAG_SOURCE )
		ClearCache();

	long nCount = m_pReader->GetRecordsCount();
	long nViewCount = m_pViewData->GetCount();

	if( m_pReader->IsFiltered() )
	{
		m_pReader->OnBOF();

		m_pViewData->Clear();
		
		CRecordEx pRecord;
		for(int i = 0; i < nCount; i++)
		{
			pRecord.m_nPos = i;
			if( m_pReader->GetRecord(pRecord) )
			{
				if( !pRecord.m_fHidden && m_pReader->OnAddRecord(pRecord) )
				{
					m_pViewData->AddRecords(1);
					m_pViewData->GetRecord(m_pViewData->GetCount()-1)->m_nPos = pRecord.m_nPos;
				}
			}
		}
		m_pReader->OnEOF();
	}
	else if( nCount != nViewCount )
		m_pViewData->AddRecords(nCount - nViewCount);

	if( nCount != nViewCount )
		m_pReader->OnChangeCount(nViewCount = m_pViewData->GetCount());

	nCount = (long)GetItemsCount();

	if( nCount != nViewCount || !!(nFlags & (UPDATE_FLAG_REDRAW|UPDATE_FLAG_SOURCE)) )
		ctl_SetItemCount(nViewCount);

	bool bAutoScroll = nCount != nViewCount && nCount &&
		((ctl_GetItemState(nCount - 1, ISTATE_SELECTED|ISTATE_FOCUSED) == ISTATE_FOCUSED) ||
		(m_fAutoScroll && !ctl_IsFocused() && (GetNextSelectedItem() == -1)));

	bool bAutoScroll2 = true;
	if( bAutoScroll && ((m_pFindData && m_pFindData->Item()) || m_fBlockAutoScroll) )
		bAutoScroll2 = false;
	
	m_fAutoScrollActive = 0;
	if( bAutoScroll )
	{
		ctl_SetItemState(nCount - 1,     0,              ISTATE_FOCUSED);
		ctl_SetItemState(nViewCount - 1, ISTATE_FOCUSED, ISTATE_FOCUSED);
		
		if( bAutoScroll2 )
		{
			ctl_ScrollDown();
			m_fAutoScrollActive = 1;
		}
	}

	if( nFlags & UPDATE_FLAG_SORT )
		SortCancel();
*/
}

bool CListItem::IsItemSelected(int nItem)
{
	return !!(ctl_GetItemState(nItem, ISTATE_SELECTED) & ISTATE_SELECTED);
}

void CListItem::AddColumn(LPCSTR strName, tDWORD nWidth, int nScale, bool bIndex, bool bHide, bool bChecked)
{
	m_colsInfo.push_back(CColInfo());
	CColInfo& pInfo = *m_colsInfo.rbegin();
	pInfo.m_nOrder = (int)m_colsInfo.size() - 1;
	pInfo.m_nWidth = nWidth;
	pInfo.m_nScale = nScale;
	pInfo.m_bHide = bHide;
	pInfo.m_bIndex = bIndex;
	pInfo.m_bChecked = bChecked;
	pInfo.m_sName = strName ? strName : "";

	ctl_ColsUpdate();
}

void CListItem::ShowColumn(int nColPos, bool bHide)
{
	CColInfo& pInfo = m_colsInfo[nColPos];
	if( pInfo.m_bHide == bHide )
		return;

	pInfo.m_bHide = bHide;
	ctl_ColsUpdate();
}

void CListItem::DeleteAllColumns()
{
	m_colsInfo.clear();
	ctl_ColsUpdate();
}

int CListItem::GetItemIcon(int nItem)
{
	if( !m_pViewData )
		return -1;

	if( CRecord * pRecord = m_pViewData->GetRecord(nItem, true) )
		return pRecord->GetStatus();
	return -1;
}

void CListItem::UpdateState()
{
	if( !m_pViewData )
		return;

	int nCount = m_pSortData ? m_pSortData->GetCount() : m_pViewData->GetCount();

	for(int i = 0; i < nCount; i++)
	{
		CRecord * pRecord = m_pViewData->GetRecord(i);
		if( !pRecord )
			continue;

		if( m_pSortData )
			m_pViewData->SwapRecordState(pRecord, i, *m_pSortData);

		if( pRecord->m_fFocused )
			SetItemFocus(i, true, false);
	}
}

tCHAR * CListItem::GetItemText(int nItem, int nColumn)
{
	if( !m_pViewData )
		return NULL;

	int nColIdx = ctl_ColGetIdx(nColumn);
	if( nColIdx == -1 )
		return NULL;
	
	CRecord * pRecord = m_pViewData->GetRecord(nItem, true);
	return pRecord ? pRecord->GetData(nColIdx) : NULL;
}

void CListItem::GetColumnText(int nColumn, tString& strText)
{
	int nColIdx = ctl_ColGetIdx(nColumn);
	if( nColIdx != -1 )
	{
		CColInfo& _col = m_colsInfo[nColIdx];
		strText = _col.m_sName;
	}
}

int CListItem::GetImage(int nItem)
{
	if( !m_pViewData )
		return -1;

	CRecord * pRecord = m_pViewData->GetRecord(nItem, true);
	return pRecord ? pRecord->GetStatus() : -1;
}

int CListItem::GetSourcePos(int nItem)
{
	if( !m_pViewData )
		return -1;

	const CRecord * pRecord = m_pViewData->GetRecord(nItem);
	return pRecord ? pRecord->m_nPos : -1;
}

int CListItem::FindItemByPos(int nPos)
{
	if( !m_pViewData )
		return -1;

    for (int nItem = 0; nItem < GetRecordsCount(); ++nItem)
    {
        const CRecord * pRecord = m_pViewData->GetRecord(nItem);
        if (pRecord && pRecord->m_nPos == nPos)
            return nItem;
    }
    return -1;
}

void CListItem::UpdateItem(int nItem)
{
	CRecord * pRecord = m_pViewData->GetRecord(nItem);
	if( pRecord )
		m_pViewData->ClearRecordData(pRecord);

	ctl_RedrawItem(nItem);
}

bool CListItem::OperateItem(int nPos, int nOp)
{
	if( nOp == VLISTCTRL_OP_DELETE )
		return DeleteItem(nPos, false, false);
	return false;
}

void CListItem::OperateSelectedItems(int nOp)
{
/*	if( !m_pViewData )
		return;
	
	if( m_pReader && !m_pReader->OnCanOperate(nOp) )
		return;
	
	m_fOpSelItemsUpdateNeed = 1;
	
	bool   bSomeError = false;
	tDWORD nFirst = -1;

	CDialogItem * pTopDlg = m_pOwner ? (CDialogItem *)m_pOwner->GetOwner(true) : NULL;

	int nCount = m_pViewData->GetCount();
	for(int i = nCount-1; i >= 0; i--)
	{
		CRecord * pRecord = m_pViewData->GetRecord(i);
		if( !pRecord->m_fSelected )
			continue;

		bool bResult = false;
		if( m_pReader )
			bResult = m_pReader->OnOperateItem(i, nOp);

		if( !bResult )
			bResult = OperateItem(i, nOp);

		if( !bResult )
			bSomeError = true;
		else
			nFirst = i;

		// RETURN IF TOP DIALOG IS CLOSED !!!
		if( pTopDlg && pTopDlg->m_bCloseDirect )
			return;
	}

	if( !bSomeError && nFirst != -1 )
		SetItemFocus(nFirst);

	OnChangedData(this);
	ctl_Redraw();

	if( m_fOpSelItemsUpdateNeed != 1 )
	{
		m_fOpSelItemsUpdateNeed = 0;
		UpdateView(m_nOpSelItemsUpdateFlags);
	}
	m_fOpSelItemsUpdateNeed = 0;
*/
}

void CListItem::DeleteSelectedItems()
{
	if( !m_fReadOnly )
		OperateSelectedItems(VLISTCTRL_OP_DELETE);
}

bool CListItem::GetCheck(int nItem, int nColumn)
{
	const CRecord * pRecord = m_pViewData->GetRecord(nItem);
	return pRecord ? pRecord->GetCheck(nColumn) : false;
}

bool CListItem::SetCheck(int nItem, int nColumn, bool bChecked, bool fUpdate)
{
/*	if( m_fReadOnly )
		return false;
	
	if( !m_pReader->OnListEvent(nItem, nColumn, VLISTCTRL_EVENT_CHECKING) )
		return false;
	
	CRecord * pRecord = m_pViewData->GetRecord(nItem, true);
	if( !pRecord )
		return false;
	pRecord->SetCheck(nColumn, bChecked ? 1 : 0);
	m_pReader->UpdateRecord(*pRecord);
	
	if( fUpdate )
	{
		m_pReader->OnListEvent(nItem, nColumn, VLISTCTRL_EVENT_CHECKED);
		OnChangedData(this);
		ctl_RedrawItem(nItem);
	}
	
*/	return true;
}

bool CListItem::InsertItem(int nItem, bool fUpdate, bool fSelect)
{
/*	if( m_fReadOnly )
		return false;
	
	if( !m_pViewData )
		return false;

	ctl_SetItemState(-1, 0, ISTATE_SELECTED|ISTATE_FOCUSED);

	if( !m_pViewData->InsertRecord(nItem) )
		return false;

	ctl_SetItemCount(m_pViewData->GetCount());

	if( fSelect )
		SetItemFocus(nItem, true);

	if( fUpdate )
	{
		OnChangedData(this);
		ctl_Redraw();

		if (m_fSorted)
			SortItems(m_nCurSortCol, m_eCurSortOrder);
	}
	
	m_pReader->OnChangeCount(m_pViewData->GetCount());
*/	return true;
}

bool CListItem::DeleteItem(int nItem, bool fUpdate, bool fReselect)
{
/*	if( m_fReadOnly )
		return false;
	
	CRecord * pRecord = m_pViewData->GetRecord(nItem);
	if( !pRecord )
		return false;
	
	bool bFocused = pRecord->m_fFocused;
	
	if( !m_pReader->OnDeleteRecord(*pRecord) ||
	    !m_pViewData->DeleteRecord(nItem) )
		return false;

	ctl_SetItemCount(m_pViewData->GetCount());

	if( fReselect && bFocused )
		SetItemFocus(nItem);

	if( fUpdate )
	{
		OnChangedData(this);
		ctl_Redraw();
	}

	m_pReader->OnChangeCount(m_pViewData->GetCount());
*/	return true;
}

bool CListItem::IsAtLeastOneChecked()
{
	for( int nItem = 0; nItem < m_pViewData->GetCount(); nItem++ )
		if( GetCheck(nItem, 0) )
			return( true );
	return( false );
}

//////////////////////////////////////////////////////////////////////

/*
bool CListItem::GetRecord(CRecord &pRecord)
{
	cAutoCS autoCS(m_pSortData ? m_pSortData->m_hCS : NULL, true);
	return m_pReader->GetRecord(pRecord);
}
*/
long CListItem::GetRecordsCount()
{
	return 0;//m_pReader->GetRecordsCount();
}

//////////////////////////////////////////////////////////////////////

void CListItem::ClearSource()
{
	if( m_pSortData )
		delete m_pSortData, m_pSortData = NULL;
	if( m_pViewData )
		delete m_pViewData, m_pViewData = NULL;
}

bool CListItem::SortCheck()
{
	if( !m_pSortData )
		return true;

	if( m_pSortData->m_fSortCanceled )
		return false;

	m_pSortData->Progress();
	return true;
}

void CListItem::SetSelection(int nFrom, int nTo, bool fSel)
{
	bool fall = (nFrom == -1);
	if( fall )
		nFrom = 0, nTo = m_pViewData->GetCount();

	for(int i = nFrom; i <= nTo; i++)
	{
		CRecord * pRecord = m_pViewData->GetRecord(i);
		if( pRecord )
			pRecord->m_fSelected = fSel ? 1 : 0;
	}
}

void CListItem::PrepareCache(int iFrom, int iTo)
{
	if( !m_pViewData || m_pSortData )
		return;

	for(int i = iFrom; i <= iTo; i++)
		m_pViewData->GetRecord(i, true);
}

static bool _cStrObj_findWholeWord(const cStrObj& strText, const cStrObj& strFind, tDWORD nCmpFlags)
{
	tDWORD nText = strText.size();
	tDWORD nFind = strFind.size();
	tDWORD findPos;
	
	for(int i = 0; (findPos = strText.find(strFind, i, cStrObj::whole, fSTRING_FIND_FORWARD|nCmpFlags)) != cStrObj::npos; i += nFind)
	{
		if( (findPos > 0) && iswalpha(strText[findPos - 1]) )
			continue;
		
		tWORD nAfterWordPos = findPos + nFind;
		if( (nAfterWordPos < nText) && iswalpha(strText[nAfterWordPos]) )
			continue;

		return true;
	}
	
	return false;
}

bool CListItem::FindItem(int nFlags, int nColumn)
{
	if( !m_pFindData )
		m_pFindData = new CFindDataCtx(this);

	if( nFlags & (eFindDown|eFindUp) )
		m_pFindData->m_bFindDown = (nFlags & eFindDown);

	if( nColumn != -1 || (nFlags & eShowDlg) )
		m_pFindData->m_nColumn = nColumn;

	if( m_pFindData->m_strText.empty() )
		nFlags |= eShowDlg;

	if( nFlags & eShowDlg )
	{
		CDialogItem * pItem = m_pFindData->Item();
		if( !pItem )
			GetOwner(true)->LoadItem(m_pFindData, (CItemBase*&)pItem, ITEMID_FIND_DIALOG, "", 0);
		if( pItem )
			pItem->Activate();
		return true;
	}

	int nCount = m_pViewData->GetCount();
	if( !nCount )
		return false;

	cStrObj strBuff;
	cStrObj strFind = m_pFindData->m_strText;

	bool bDown = m_pFindData->m_bFindDown || (nFlags & eMarkAll);
	bool bFound = false;

	int nCurrent = GetItemFocus();
	if( nCurrent == -1 ) nCurrent = bDown ? 0 : nCount - 1;

	int nItem = nCurrent;

	if( nFlags & eMarkAll )
		ctl_SetItemState(-1, 0, ISTATE_SELECTED|ISTATE_FOCUSED);

	int nColTo = m_pFindData->m_nColumn == -1 ? m_nColumnCount : m_pFindData->m_nColumn + 1;
	int nColFrom = m_pFindData->m_nColumn == -1 ? 0 : m_pFindData->m_nColumn;
	int nSelFlags = ISTATE_SELECTED|ISTATE_FOCUSED;

	while( 1 )
	{
		nItem += bDown ? 1 : -1;

		if( nItem > nCount - 1 )
			nItem = 0;

		if( nItem < 0 )
			nItem = nCount - 1;

		CRecord * pRecord = m_pViewData->GetRecord(nItem, true);
		if( pRecord && pRecord->m_pData )
		{
			for(int j = nColFrom; j < nColTo; j++)
			{
				int nColIdx = ctl_ColGetIdx(j);
				if( nColIdx == -1 )
					continue;
				
				LPSTR strText = pRecord->GetData(nColIdx);
				if( !strText )
					continue;

				m_pRoot->LocalizeStr(strText, strBuff);
				
				tDWORD nCmpFlags = m_pFindData->m_bMatchCase ? fSTRING_COMPARE_CASE_SENSITIVE : fSTRING_COMPARE_CASE_INSENSITIVE;
				if( m_pFindData->m_bWholeWord )
				{
					if( !_cStrObj_findWholeWord(strBuff, strFind, nCmpFlags) )
						continue;
				}
				else
				{
					if( strBuff.find(strFind, 0, cStrObj::whole, fSTRING_FIND_FORWARD|nCmpFlags) == cStrObj::npos )
						continue;
				}
				
				if( nFlags & eMarkAll )
				{
					ctl_SetItemState( nItem, nSelFlags, nSelFlags);
					nSelFlags = ISTATE_SELECTED;
					bFound = true;
					break;
				}
				else
				{
					SetItemFocus(nItem, true);
					bFound = true;
					goto find_end;
				}
			}
		}

		if( nItem == nCurrent )
			break;
	}

find_end:
	if( !bFound )
		m_pRoot->ShowMsgBox(*m_pFindData, STRID_NOTFOUND, "dlg_caption", MB_OK|MB_ICONEXCLAMATION);

	return bFound;
}

void CListItem::SortItems(int nColumn, eSortType_t eSortOrder)
{	
	if( m_pSortData )
	{
		bool bCancel = nColumn == m_nCurSortCol;

		m_nCurSortCol = m_pSortData->m_nPrevSortCol;
		m_eCurSortOrder = m_pSortData->m_ePrevSortOrder;
		UpdateSortStatus();
		delete m_pSortData, m_pSortData = NULL;

		if( bCancel )
			return;
	}

	int nPrevSortCol = m_nCurSortCol;
	eSortType_t ePrevSortOrder = m_eCurSortOrder;

	m_nCurSortCol = nColumn;
	m_eCurSortOrder = eSortOrder;

	UpdateSortStatus();

	if( m_eCurSortOrder == eNotSorted || !GetWindow() )
		return;

	tDWORD nRows = (tDWORD)GetItemsCount();

	if( m_pViewData && nRows > RECORD_ASYNC_SORT_COUNT && nColumn != m_nIndexCol )
	{
		m_pSortData = new CSortProgress(this, *m_pViewData);
		m_pSortData->m_nSortRows = nRows;
		m_pSortData->m_nSortCol = m_nCurSortCol;
		m_pSortData->m_eSortOrder = m_eCurSortOrder;
		m_pSortData->m_nPrevSortCol = nPrevSortCol;
		m_pSortData->m_ePrevSortOrder = ePrevSortOrder;
		m_pSortData->Start();
	}
	else
	{
		QuickSort(nRows, nColumn, eSortOrder);
		UpdateState();
		ctl_Redraw();
	}
}

void CListItem::QuickSort(int nRows, int nColumn, eSortType_t eSortOrder)
{
#define COMPARE(n1, n2) ( eSortOrder == eSortUp ? \
	SortComapare(nColumn, n1, n2) : -SortComapare(nColumn, n1, n2) )
#define MULTI_CMP(n1,n2) ( eSortOrder == eSortUp ? \
	MultiSortCompare(nColumn, n1, n2) : -MultiSortCompare(nColumn, n1, n2) )

	int lo = 0, hi = nRows-1, mid, loguy, higuy;
    int lostk[30], histk[30];
    unsigned size;
    int stkptr = 0, max;

    if( hi < 1 )
        return;

recurse:

    size = (hi - lo) + 1;
    if (size <= 8) {
		for(int hh = hi; hh > lo; hh--) {
			max = lo;
			for(int p = lo+1; p <= hh; p ++)
				if (MULTI_CMP(p, max) > 0) max = p;
			SwapItems(max, hh);
		}
    }
    else
	{
        mid = lo + (size / 2);
        loguy = lo;
        higuy = hi + 1;

		if( nRows > RECORD_ASYNC_SORT_COUNT )
		{
			SwapItems(mid, lo);

			while(1) {
				do  {
					if( !SortCheck() )
						return;
					loguy ++;
				} while (loguy <= hi && MULTI_CMP(loguy, lo) < 0);

				do  {
					if( !SortCheck() )
						return;
					higuy --;
				} while (higuy > lo && MULTI_CMP(higuy, lo) > 0);

				if (higuy < loguy)
					break;

				SwapItems(loguy, higuy);
			}
		}
		else
		{
			while(1) {
				do  {
					if( !SortCheck() )
						return;
					loguy ++;
				} while (loguy <= hi && MULTI_CMP(loguy, lo) <= 0);

				do  {
					if( !SortCheck() )
						return;
					higuy --;
				} while (higuy > lo && MULTI_CMP(higuy, lo) >= 0);

				if (higuy < loguy)
					break;

				SwapItems(loguy, higuy);
			}
		}

        SwapItems(lo, higuy);

        if ( higuy - 1 - lo >= hi - loguy ) {
            if (lo + 1 < higuy) {
                lostk[stkptr] = lo;
                histk[stkptr] = higuy - 1;
                ++stkptr;
            }

            if (loguy < hi) {
                lo = loguy;
                goto recurse;
            }
        }
        else {
            if (loguy < hi) {
                lostk[stkptr] = loguy;
                histk[stkptr] = hi;
                ++stkptr;
            }

            if (lo + 1 < higuy) {
                hi = higuy - 1;
                goto recurse;
            }
        }
    }

    --stkptr;
    if (stkptr >= 0) {
        lo = lostk[stkptr];
        hi = histk[stkptr];
        goto recurse;
    }
    else
        return;
}

//!Функция сначала сравнивает записи в указанном столбце и, 
//в случае если результат сравнения равен 0 (элементы равны),
//сравнивает все столбцы слева направо, пока результат сравнения 
//не станет отличен от нуля или пока не кончатся столбцы.
//Т.е. происходит упорядочивание сначала по главному столбцу, 
//а затем по всем остальным столбцам.
int	CListItem::MultiSortCompare(int nColumn, int nPos1, int nPos2)
{
	int res = SortComapare(nColumn, nPos1, nPos2);
	for(int col = 0;!res && col < GetColsCount();++col){
		if( nColumn == col ) continue;
		res = SortComapare(col, nPos1, nPos2);
	}
	return res;
}

int CListItem::SortComapare(int nColumn, int nPos1, int nPos2)
{
	if( m_pSortData )
		m_pSortData->m_nSortOperations++;

	CDataSource * pSource = m_pSortData ? m_pSortData : m_pViewData;

	CRecord *pRecord1 = pSource->GetRecord(nPos1, nColumn != m_nIndexCol);
	CRecord *pRecord2 = pSource->GetRecord(nPos2, nColumn != m_nIndexCol);

	if( nColumn == m_nIndexCol )
		return pRecord1->m_nPos - pRecord2->m_nPos;

	nColumn = ctl_ColGetIdx(nColumn);
	if( nColumn == -1 )
		return 0;

	CRecordData *pData1 = pRecord1 ? pRecord1->m_pData : NULL;
	CRecordData *pData2 = pRecord2 ? pRecord2->m_pData : NULL;

/*	if( nColumn == 0 )
	{
		int nImage1 = pData1 ? pData1->m_Status : 0;
		int nImage2 = pData2 ? pData2->m_Status : 0;

		if( nImage1 != nImage2 )
			return nImage1 - nImage2;
	}
*/
/*	if( m_pReader && pRecord1 && pRecord2 )
	{
		int nRes = m_pReader->SortComapare(nColumn, *pRecord1, *pRecord2);
		if( nRes != UNDEF_VALUE )
			return nRes;
	}
*/	
	tCHAR * pText1 = pData1 ? (tCHAR *)pData1->m_FieldVals[nColumn] : NULL;
	tCHAR * pText2 = pData2 ? (tCHAR *)pData2->m_FieldVals[nColumn] : NULL;

	if( !pText1 )
		return pText2 ? 1 : 0;

	if( !pText2 )
		return pText1 ? -1 : 0;

	return _stricmp(pText1, pText2);
}

void CListItem::SwapItems(int nPos1, int nPos2)
{
	CDataSource * pSource = m_pSortData ? m_pSortData : m_pViewData;

	pSource->SwapRecords(nPos1, nPos2);
	if( m_pSortData )
		m_pSortData->m_nSortOperations++;
}

//////////////////////////////////////////////////////////////////////

void CListItem::_ColumnClick(int nIdx)
{
	if( !m_fSorted )
		return;

	eSortType_t eSortOrder = eSortDown;
	if( m_nCurSortCol == nIdx )
	{
		if( m_eCurSortOrder == eSortDown )
			eSortOrder = eSortUp;
	}
	
	SortItems(nIdx, eSortOrder);
}

void CListItem::_ColumnRClick(int nCol, POINT& pt)
{
	CPopupMenuItem * pMenu = NULL;
	LoadItem(NULL, (CItemBase *&)pMenu, ITEMID_HDRPOPUPMENU);
	if( !pMenu )
		return;

	m_nClickCol = nCol;

	size_t nCount = m_colsInfo.size();
	for(size_t i = 0; i < nCount; i++)
	{
		CColInfo& pInfo = m_colsInfo[i];

		CItemBase * pItem = pMenu->CloneItem(NULL, "");
		pItem->m_strText = pInfo.m_sName;
		pItem->m_pUserData = (void *)i;

		if( !pInfo.m_bHide )
			pItem->m_nState = ISTATE_SELECTED;
	}

	if( !m_fSorted )
	{
		pMenu->Show(ITEMID_SORTUP, false);
		pMenu->Show(ITEMID_SORTDOWN, false);
	}

	pMenu->Enable(ITEMID_SORTUP, m_nClickCol != -1);
	pMenu->Enable(ITEMID_SORTDOWN, m_nClickCol != -1);
	pMenu->Enable(ITEMID_FIND, !!GetItemsCount());
	if( m_nClickCol != -1 && m_nCurSortCol == m_nClickCol && m_eCurSortOrder != eNotSorted )
	{
		CItemBase * pItem = pMenu->GetItem(m_eCurSortOrder == eSortDown ? ITEMID_SORTDOWN : ITEMID_SORTUP);
		if( pItem )
			pItem->m_nState = ISTATE_SELECTED;
	}

	pMenu->Track(&pt);
	pMenu->Destroy();
}

void CListItem::_RightMouseClick(POINT& pt)
{
}

//////////////////////////////////////////////////////////////////////
// CFindDataCtx

CFindDataCtx::CFindDataCtx(CListItem* pList) :
	m_pList(pList), m_bFindDown(cTRUE), m_bMatchCase(cFALSE),
	m_bWholeWord(cFALSE), m_nColumn(-1), m_bEnable(true)
{
	m_nActionsMask = DLG_ACTION_CANCEL|DLG_MODE_DLGBUTTONS;
}

void CFindDataCtx::OnInit()
{
	CDialogSink::OnInit();

	m_pColumns = (CComboItem *)GetItem("ColumnCmb");
	m_pFindEdit = GetItem("FindEdit");
	m_pWholeWord = GetItem("WholeWord");
	m_pMatchCase = GetItem("MatchCase");
	m_pFindBtn = GetItem("Find");
	m_pMarkBtn = GetItem("Mark");

	if( m_pList->m_fSingleSel )
		m_pMarkBtn->Show(false);

	if( m_pColumns )
	{
		m_pColumns->SetType(tid_QWORD);
		
		tString strDefault; m_pList->m_pRoot->GetString(strDefault, PROFILE_LOCALIZE, NULL, "AllColumns");
		if( !strDefault.empty() )
			m_pColumns->AddItem(Root()->LocalizeStr(strDefault.c_str()), -1);

		for(size_t i = 0, n = m_pList->m_colsInfo.size(); i < n; i++)
			m_pColumns->AddItem(Root()->LocalizeStr(m_pList->m_colsInfo[i].m_sName.c_str()), i);

		if( m_pList->m_fNoHeader )
			m_pColumns->Enable(false);
	}
	
	UpdateData(true);

	if( m_pFindEdit )
	{
		OnChangedData(m_pFindEdit);
		m_pFindEdit->Focus(true);
	}
}

void CFindDataCtx::OnChangedData(CItemBase* pItem)
{
	if( pItem != m_pFindEdit )
		return;

	bool bEnable = !m_pFindEdit->m_strText.empty();
	if( m_bEnable == bEnable )
		return;


	m_bEnable = bEnable;
	if( m_pFindBtn ) m_pFindBtn->Enable(m_bEnable);
	if( m_pMarkBtn ) m_pMarkBtn->Enable(m_bEnable);
}

bool CFindDataCtx::OnClicked(CItemBase* pItem)
{
	UpdateData(false);

	if( !m_bEnable )
		return false;

	if( pItem == m_pFindBtn )
	{
		m_pList->FindItem(0);
		return true;
	}
	else if( pItem == m_pMarkBtn )
	{
		if( m_pList->FindItem(CListItem::eMarkAll) )
			Close();
		return true;
	}

	return false;
}

void CFindDataCtx::UpdateData(bool bIn)
{
	if( m_pColumns )
	{
		cVariant nVal = (tQWORD)-1;
		if( bIn )
		{
			nVal = (tQWORD)m_nColumn;
			m_pColumns->SetValue(nVal);
		}
		else
		{
			m_pColumns->GetValue(nVal);
			m_nColumn = nVal.ToInt32();
		}
	}
	
	cVariant val;
	if( m_pFindEdit )  bIn ? m_pFindEdit->SetValue(cVariant(m_strText.data())) : m_pFindEdit->GetValue(val), val.ToStringObj(m_strText);
	if( m_pMatchCase ) bIn ? m_pMatchCase->SetValue(cVariant(m_bMatchCase)) : m_pMatchCase->GetValue(val), m_bMatchCase = (tBOOL)val.ToQWORD();
	if( m_pWholeWord ) bIn ? m_pWholeWord->SetValue(cVariant(m_bWholeWord)) : m_pWholeWord->GetValue(val), m_bWholeWord = (tBOOL)val.ToQWORD();
}

//////////////////////////////////////////////////////////////////////
// CSortProgress

CSortProgress::CSortProgress(CListItem * pList, CDataSource& c) :
	CRootTaskBase(pList->m_pRoot, 0x665cd9e0), // is a crc32 for "CSortProgress"
	CDataSource(c, RECORD_CACHE_SORT_MAXCOUNT),
	m_pList(pList),
	m_nSortCompletion(0),
	m_nSortOperations(0),
	m_fSortCanceled(false),
	m_hCS(NULL)
{
	static int g_nSortIndex = 0;
	m_nSortId = g_nSortIndex++;

	g_root->sysCreateObjectQuick((hOBJECT*)&m_hCS, IID_CRITICAL_SECTION);
}

CSortProgress::~CSortProgress()
{
	Stop();

	if( m_hCS )
		m_hCS->sysCloseObject();
}

void CSortProgress::Start()
{
	wait_for_stop();

	m_nSortWork = 2*(int)(m_nSortRows * log((double)m_nSortRows));
	start();
}

void CSortProgress::Stop()
{
	m_fSortCanceled = true;
	wait_for_stop();
}

void CSortProgress::do_work()
{
	m_pList->QuickSort(m_nSortRows, m_nSortCol, m_eSortOrder);
}

void CSortProgress::on_exit()
{
	cGuiParams _prm;
	_prm.m_nVal1 = (tPTR)m_nSortId;
	m_pList->m_pRoot->SendEvent(EVENT_ID_SORTDONE, &_prm, 0, m_pList);
}

//////////////////////////////////////////////////////////////////////

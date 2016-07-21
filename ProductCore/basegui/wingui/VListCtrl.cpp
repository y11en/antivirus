//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winctrls.h"
#include <algorithm>
#include <math.h>
#include <ProductCore/DataSource.h>

#define ListView_SetColumnWidthAsyn(hwnd, iCol, cx) \
	(BOOL)(::PostMessage((hwnd), LVM_SETCOLUMNWIDTH, (WPARAM)(int)(iCol), MAKELPARAM((cx), 0)))

tDWORD LVM_DRAG_DONE = RegisterWindowMessage("LVM_DRAG_DONE");
tDWORD LVM_CHECK_NOSELSTATE = RegisterWindowMessage("LVM_CHECK_NOSELSTATE");

//////////////////////////////////////////////////////////////////////
// CVListCtrlImpl

HBITMAP   CVListCtrlImpl::m_bmSortUp;
HBITMAP   CVListCtrlImpl::m_bmSortDown;
HBITMAP   CVListCtrlImpl::m_bmSortUpProgress;
HBITMAP   CVListCtrlImpl::m_bmSortDownProgress;

//////////////////////////////////////////////////////////////////////

CVListCtrlImpl::CVListCtrlImpl() :
	m_hImageList(NULL),
	m_fSelectTransaction(0),
	m_fSubItemTip(0)
{
	m_nStyles = WS_CHILD|WS_TABSTOP|/*WS_CLIPCHILDREN|*/LVS_REPORT|LVS_SHOWSELALWAYS|LVS_OWNERDATA|LVS_OWNERDRAWFIXED;
	m_nStylesEx = WS_EX_CLIENTEDGE|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_HEADERDRAGDROP;

	m_scrollInfo.nMax = 0;
}

void CVListCtrlImpl::ApplyStyle(LPCTSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_CHECKED)        m_nStylesEx |= LVS_EX_CHECKBOXES; sbreak;
	scase(STR_PID_FIXEDCOLUMNS)   m_nStylesEx &= ~LVS_EX_HEADERDRAGDROP; sbreak;
	scase(STR_PID_NOHEADER)       m_nStyles |= LVS_NOCOLUMNHEADER; sbreak;
	scase(STR_PID_SINGLESEL)      m_nStyles |= LVS_SINGLESEL; sbreak;
	send;
}

void CVListCtrlImpl::OnInit()
{
	if( !CWindowL::m_bAnsi )
		ListView_SetUnicodeFormat(m_hWnd, TRUE);

	ListView_SetExtendedListViewStyle(m_hWnd, m_nStylesEx);

	TBase::OnInit();

	int nStateMask = LVIS_FOCUSED|LVIS_SELECTED;
	if( m_nStylesEx & LVS_EX_CHECKBOXES )
		nStateMask |= LVIS_STATEIMAGEMASK;
	
	ListView_SetCallbackMask(m_hWnd, nStateMask);

	if( m_fSorted && !(m_nStyles & LVS_NOCOLUMNHEADER) )
	{
		COLORMAP clMap;
		clMap.from = RGB(192,192,192);
		clMap.to = GetSysColor(COLOR_3DFACE);

		if( !m_bmSortDown )
		{
			m_bmSortDown         = ::CreateMappedBitmap(g_hInstance, IDB_SORT_UP, 0, &clMap, 1);
			m_bmSortUp           = ::CreateMappedBitmap(g_hInstance, IDB_SORT_DOWN, 0, &clMap, 1);
			m_bmSortDownProgress = ::CreateMappedBitmap(g_hInstance, IDB_SORT_UP_PROGRESS, 0, &clMap, 1);
			m_bmSortUpProgress   = ::CreateMappedBitmap(g_hInstance, IDB_SORT_DOWN_PROGRESS, 0, &clMap, 1);
		}
	}

	if( HWND hHeader = ListView_GetHeader(m_hWnd) )
	{
		::InvalidateRect(hHeader, NULL, TRUE);
		::UpdateWindow(hHeader);
	}
}

void CVListCtrlImpl::OnAdjustClientRect(RECT& rcClient)
{
	RECT cr; CWindow::GetClientRect(&cr);
	RECT wr; CWindow::GetWindowRect(&wr);

	POINT d = {(RECT_CX(wr) - RECT_CX(cr)), (RECT_CY(wr) - RECT_CY(cr))};
	rcClient.right  -= d.x;
	rcClient.bottom -= d.y;
}

void CVListCtrlImpl::UpdateColumnsData(bool bFromHeader, bool bUpdateIdx)
{
	if( m_fUpdateCols )
		return;

	m_fUpdateCols = 1;

	unsigned i, nCount, nCols, nOffset = 0;

	if( bUpdateIdx )
	{
		nCount = (unsigned)m_colsInfo.size();
		std::vector<unsigned> vecOrder(nCount);

		for(i = 0; i < nCount; i++)
			vecOrder[m_colsInfo[i].m_nOrder] = i;

		m_colsIdx.clear();
		m_nColumnCount = 0;

		for(i = 0; i < nCount; i++)
			if( !m_colsInfo[vecOrder[i]].m_bHide )
				m_colsIdx.push_back(vecOrder[i]), m_nColumnCount++;
	}

	{
		CWindowLocker(m_hWnd, bUpdateIdx);

		HWND hHeader = ListView_GetHeader(m_hWnd);

		nCount = m_nColumnCount;

		nCols = Header_GetItemCount(hHeader);
		for(i = 0; i < nCount; i++)
		{
			size_t nColPos = m_colsIdx[i];
			CColInfo& pInfo = m_colsInfo[nColPos];

			bool bNew = i >= nCols;

			HDITEM hdi;
			hdi.mask = HDI_LPARAM|HDI_WIDTH;
			if( !bNew )
				Header_GetItem(hHeader, i, &hdi);

			if( bFromHeader )
			{
				if( hdi.cxy != pInfo.m_nWidth )
				{
					if( pInfo.m_nScale )
						pInfo.m_nScale = 100 * hdi.cxy / (m_szSize.cx-4);

					pInfo.m_nWidth = hdi.cxy;
				}
			}
			else
			{
				if( pInfo.m_nScale )
					pInfo.m_nWidth = ((m_szSize.cx-4) * pInfo.m_nScale) / 100;
			}

			pInfo.m_nOffset = nOffset;
			nOffset += pInfo.m_nWidth;

			if( bFromHeader )
				continue;

			if( pInfo.m_bIndex )
				m_nIndexCol = i;

			LVCOLUMN lvc; memset(&lvc, 0, sizeof(lvc));
			
			if( pInfo.m_bChecked )
			{
				lvc.mask |= LVCF_FMT;
				lvc.fmt = LVCFMT_CENTER;
			}

			if( bNew || hdi.lParam != nColPos )
			{
				lvc.mask |= LVCF_TEXT;
				lvc.pszText = (LPSTR)pInfo.m_sName.c_str();
				lvc.cchTextMax = (int)pInfo.m_sName.size();
			}

			if( (bNew || hdi.cxy != pInfo.m_nWidth) && pInfo.m_nWidth )
			{
				lvc.mask |= LVCF_WIDTH;
				lvc.cx = pInfo.m_nWidth;
			}
							
			if( !lvc.mask )
				continue;

			if( bNew )
				ListView_InsertColumnL(m_pRoot, i, &lvc);
			else
				ListView_SetColumnL(m_pRoot, i, &lvc);

			if( !pInfo.m_nWidth )
				ListView_SetColumnWidth(m_hWnd, i, LVSCW_AUTOSIZE_USEHEADER);

			if( lvc.mask & LVCF_TEXT )
			{
				hdi.mask = HDI_LPARAM;
				hdi.lParam = nColPos;
				Header_SetItem(hHeader, i, &hdi);
			}
		}

		nCols = Header_GetItemCount(hHeader);
		for(i = nCount; i < nCols; i++)
			ListView_DeleteColumn(m_hWnd, nCount);
	}

	if( bFromHeader )
		Invalidate();

	m_fUpdateCols = 0;
}

//////////////////////////////////////////////////////////////////////

int CVListCtrlImpl::AddIcon(CIcon* pIcon)
{
	if( !pIcon )
		return -1;

	if( !m_hImageList )
	{
        CWinRoot* const pWinRoot = static_cast<CWinRoot*>(m_pRoot);
		m_hImageList =
            ImageList_Create(pWinRoot->LIST_ICON_SIZE, pWinRoot->LIST_ICON_SIZE, ILC_COLORDDB|ILC_MASK, 16, 16);

		if( m_pRoot->IsOwnThread() )
		{
			ListView_SetImageList(m_hWnd, m_hImageList, LVSIL_SMALL);
			ListView_SetItemCountEx(m_hWnd, m_pViewData->GetCount(), 0);
		}
		else
		{
			::PostMessage(m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)m_hImageList);
			::PostMessage(m_hWnd, LVM_SETITEMCOUNT, (WPARAM)m_pViewData->GetCount(), (LPARAM)0);
		}
	}

	return ImageList_AddIcon(m_hImageList, pIcon->IconHnd());
}

CIcon * CVListCtrlImpl::GetIcon(int nIndex)
{
	HICON hIcon = ImageList_GetIcon(m_hImageList,nIndex, ILD_NORMAL);
	if( !hIcon )
		return NULL;
	return m_pRoot->CreateIcon(hIcon, true);
}

tSIZE_T CVListCtrlImpl::ctl_GetItemCount()
{
	return ListView_GetItemCount(m_hWnd);
}

bool CVListCtrlImpl::GetItemTextW(int nItem, int nColumn, cStrObj& strText)
{
	CWindowL::ListView_GetItemTextL(m_pRoot, m_hWnd, nItem, nColumn, strText);
	return true;
}

void CVListCtrlImpl::SetItemFocus(int nItem, bool fEnsureVisible, bool fUnselectAll )
{
	if( nItem == m_pViewData->GetCount() )
		nItem--;

	if( nItem < 0 )
		return;

	if ( fUnselectAll )
		ListView_SetItemState(m_hWnd, -1, 0, LVIS_SELECTED|LVIS_FOCUSED);
	ListView_SetItemState(m_hWnd, nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	if( fEnsureVisible )
		ListView_EnsureVisible(m_hWnd, nItem, FALSE);
}

int CVListCtrlImpl::GetItemFocus()
{
	return ListView_GetNextItem(m_hWnd, -1, LVIS_FOCUSED);
}

int CVListCtrlImpl::ctl_GetNextSelectedItem(int nPrev)
{
	return ListView_GetNextItem(m_hWnd, nPrev, LVIS_SELECTED);
}

tDWORD CVListCtrlImpl::ctl_GetSelectedCount()
{
	return ListView_GetSelectedCount(m_hWnd);
}

void CVListCtrlImpl::UpdateSortStatus(bool bProgress)
{
	HWND hHeader = ListView_GetHeader(m_hWnd);

	for(int i = 0; i < m_nColumnCount; i++)
	{	
		HDITEM hdi;
		hdi.mask = HDI_FORMAT;
		Header_GetItem(hHeader, i, &hdi);

		bool bUseXPArrows = m_pRoot && ((CWinRoot*)m_pRoot)->m_nComCtlVer >= COMCTL_VER(6, 0);

		if( i != m_nCurSortCol || m_eCurSortOrder == eNotSorted )
		{
			if(bUseXPArrows)
				hdi.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
			else
				hdi.fmt &= ~(HDF_BITMAP|HDF_BITMAP_ON_RIGHT);
		}
		else
		{
			if(bUseXPArrows)
			{
				hdi.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
				hdi.fmt |= m_eCurSortOrder == eSortDown ? HDF_SORTDOWN : HDF_SORTUP;
			}
			else
			{
				hdi.mask |= HDI_BITMAP;
				hdi.fmt |= HDF_BITMAP|HDF_BITMAP_ON_RIGHT;
				if( m_eCurSortOrder == eSortDown )
					hdi.hbm = bProgress ? m_bmSortDownProgress : m_bmSortDown;
				else
					hdi.hbm = bProgress ? m_bmSortUpProgress : m_bmSortUp;
			}
		}

		Header_SetItem(hHeader, i, &hdi);
	}
}

void CVListCtrlImpl::UpdateView(int nFlags)
{
	if( nFlags & (UPDATE_FLAG_REINIT) ){
		CWindowLocker wndLocker(m_hWnd); //лочим окно чтобы не мерцало
		TBase::UpdateView(nFlags);
	}
	else{
		TBase::UpdateView(nFlags);
	}
}

void CVListCtrlImpl::CopyToClipboard()
{
	tString strCopyText;

	int nItem = -1;
	for( nItem = 0; nItem< m_pViewData ->GetCount(); ++nItem )
	{
		CRecord *pRecord = m_pViewData->GetRecord(nItem, true);
		if( !pRecord || !pRecord ->m_fSelected )
			continue;

		for(int i = 0; i < m_nColumnCount; i++)
		{
			LPSTR strText = pRecord->GetData(m_colsIdx[i]);
			strCopyText += strText;
			if( i != m_nColumnCount-1 )
				strCopyText += '\t';
		}

		if( !(m_nStyles & LVS_SINGLESEL) )
			strCopyText += "\r\n";
	}

	if( strCopyText.empty() )
		return;

    HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, strCopyText.size() + 1);
	HANDLE  hglbLocale = GlobalAlloc(GMEM_MOVEABLE, sizeof(LCID));
    if( !hglbCopy || !hglbLocale )
	{
		if( hglbCopy )   GlobalFree(hglbCopy);
		if( hglbLocale ) GlobalFree(hglbLocale);
		return;
	}

	void* lptstrCopy = GlobalLock(hglbCopy);
	strcpy((char*)lptstrCopy, strCopyText.c_str());
	GlobalUnlock(hglbCopy);

	*((LCID *)GlobalLock(hglbLocale)) = m_pRoot->m_nLocaleId;
	GlobalUnlock(hglbLocale);

	if( OpenClipboard() )
	{
		EmptyClipboard();
		
		SetClipboardData(CF_TEXT, hglbCopy);
		SetClipboardData(CF_LOCALE, hglbLocale);

		CloseClipboard();
	}
}

void CVListCtrlImpl::SelectAll()
{
	tSIZE_T n = GetItemsCount();
	if( !n )
		return;
	
	ListView_SetSelectionMark(m_hWnd, 0);
	m_fSelectTransaction = 1;
	for(tSIZE_T i = 0; i < n; i++)
		ListView_SetItemState(m_hWnd, i, LVIS_SELECTED, LVIS_SELECTED);
	m_fSelectTransaction = 0;

	NMLVODSTATECHANGE nmlv; memset(&nmlv, 0, sizeof(nmlv));
	nmlv.hdr.hwndFrom = m_hWnd;
	nmlv.hdr.code = LVN_ODSTATECHANGED;
	nmlv.iTo = (int)n - 1;
	nmlv.uNewState = LVIS_SELECTED;
	::SendMessage(m_hWnd, OCM_NOTIFY, 0, (LPARAM)&nmlv);
}

void CVListCtrlImpl::_MoveColumn(int nColPos, int nColPosTo)
{
	if( nColPos == nColPosTo )
		return;

	size_t nCount = m_colsInfo.size();
	std::vector<unsigned> vecOrder(nCount);
	unsigned i;
	for(i = 0; i < nCount; i++)
		vecOrder[m_colsInfo[i].m_nOrder] = i;

	vecOrder.erase(vecOrder.begin() + m_colsInfo[nColPos].m_nOrder);
	vecOrder.insert(vecOrder.begin() + m_colsInfo[nColPosTo].m_nOrder, nColPos);

	for(i = 0; i < nCount; i++)
		m_colsInfo[vecOrder[i]].m_nOrder = i;

	UpdateColumnsData(false, true);
}

void CVListCtrlImpl::_GetDispInfo(LVITEMA * pItem, bool bAnsi)
{
	if( !m_pViewData )
		return;

	CRecord * pRecord = m_pViewData->GetRecord(pItem->iItem, true);
	if( !pRecord )
		return;

	if( (pItem->mask & LVIF_TEXT) && pRecord->m_pData )
	{
		if( bAnsi )
		{
			if( (unsigned)pItem->iSubItem >= m_colsIdx.size() )
				pItem->pszText = "unexpected";
			else
				pItem->pszText = pRecord->GetData(m_colsIdx[pItem->iSubItem]);
		}
		else
		{
			if( (size_t)pItem->iSubItem >= m_colsIdx.size() )
				pItem->pszText = (LPSTR)L"unexpected";
			else
				pItem->pszText = (LPSTR)m_pRoot->LocalizeStr(pRecord->GetData(m_colsIdx[pItem->iSubItem]));
		}
	}

	if( pItem->mask & LVIF_IMAGE )
		pItem->iImage = pRecord->GetStatus();

	if( pItem->mask & LVIF_STATE )
	{
		if( pItem->stateMask & LVIS_STATEIMAGEMASK )
			pItem->state |= INDEXTOSTATEIMAGEMASK(pRecord->GetCheck(0/*must be col num*/) ? 2 : 1);
		if( (pItem->stateMask & LVIS_FOCUSED) && pRecord->m_fFocused )
			pItem->state |= LVIS_FOCUSED;
		if( (pItem->stateMask & LVIS_SELECTED) && pRecord->m_fSelected )
			pItem->state |= LVIS_SELECTED;
	}
}

void CVListCtrlImpl::_GetTipInfo(NMLVGETINFOTIPA * pInfo, bool bAnsi)
{
	tString l_tip;

	if( m_pSink )
		m_pSink->OnGetTipInfo(pInfo->iItem, l_tip);

	if( !l_tip.empty() )
	{
		if( bAnsi )
			strncpy(pInfo->pszText, l_tip.c_str(), pInfo->cchTextMax);
		else
			wcsncpy((LPWSTR)pInfo->pszText, m_pRoot->LocalizeStr(l_tip.c_str()), pInfo->cchTextMax);
	}
}

UINT CVListCtrlImpl::_CnvState(tDWORD nState)
{
	UINT nCtlState = 0;
	if( nState & ISTATE_FOCUSED )
		nCtlState |= LVIS_FOCUSED;
	if( nState & ISTATE_SELECTED )
		nCtlState |= LVIS_SELECTED;

	return nCtlState;
}

tDWORD CVListCtrlImpl::_CnvStateBack(UINT nState)
{
	UINT nRecordState = 0;
	if( nState & LVIS_FOCUSED )
		nRecordState |= ISTATE_FOCUSED;
	if( nState & LVIS_SELECTED )
		nRecordState |= ISTATE_SELECTED;

	return nRecordState;
}

RECT CVListCtrlImpl::GetCheckRect(RECT rcCell, bool bCentered)
{
	RECT rcChecked = rcCell;
	rcChecked.right = rcChecked.left + static_cast<CWinRoot*>(m_pRoot)->DRAW_ICON_MARGIN;
	InflateRect(&rcChecked, -2, -1);
	if( bCentered )
	{
		int xoffset = (rcCell.right - rcCell.left - rcChecked.right + rcChecked.left)/2;
		OffsetRect(&rcChecked, xoffset, 0);
	}
	return rcChecked;
}

LRESULT CVListCtrlImpl::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT pDrawInfo = (LPDRAWITEMSTRUCT)lParam;

    CWinRoot* const pWinRoot = static_cast<CWinRoot*>(m_pRoot);

	CRecord *pRecord = m_pViewData->GetRecord(pDrawInfo->itemID, true);
	if( !pRecord )
		return 0;

	bool bDisabled = !IsWindowEnabled();
	tDWORD nItemState = 0;
	if( bDisabled || (pDrawInfo->itemState & ODS_DISABLED) )
		nItemState |= ISTATE_DISABLED;
	
	HDC hDrawDc = pDrawInfo->hDC;
	RECT *rcFrame = &pDrawInfo->rcItem;
	RECT rcBack = *rcFrame, rcItem = *rcFrame;
	RECT rcTemtDC = *rcFrame;
	rcTemtDC.left = 0; rcTemtDC.right = m_szSize.cx;

	HDC dc = m_pRoot->GetTempDC(rcTemtDC, &rcTemtDC);
	::SelectObject(dc, m_pFont->Handle());

	if( pRecord->m_nIndent )
		rcItem.left += pRecord->m_nIndent*pWinRoot->DRAW_ICON_MARGIN;
	if( m_nStylesEx & LVS_EX_CHECKBOXES )
		rcItem.left += pWinRoot->DRAW_ICON_MARGIN;
	if( m_hImageList )
		rcItem.left += pWinRoot->DRAW_ICON_MARGIN;

	COLORREF clTextColor = COLOR_WINDOWTEXT;
	if( pRecord && pRecord->m_fSelected )
	{
		COLORREF clBackColor = COLOR_HIGHLIGHT;
		if( m_nState & ISTATE_FOCUSED )
			clTextColor = COLOR_WINDOW;
		else
			clBackColor = COLOR_BTNFACE;

		::FillRect(dc, &rcItem, GetSysColorBrush(clBackColor));
		rcBack.right = rcItem.left;
	}
	
	if( nItemState & ISTATE_DISABLED )
		clTextColor = COLOR_BTNSHADOW;
	
	::FillRect(dc, &rcBack, GetSysColorBrush(bDisabled ? COLOR_BTNFACE : COLOR_WINDOW));

	::SetBkMode(dc, TRANSPARENT);
	::SetTextColor(dc, GetSysColor(clTextColor));

	for(int i = 0; i < m_nColumnCount && pRecord; i++)
	{
		int nIdx = m_colsIdx[i];

		CColInfo& pInfo = m_colsInfo[nIdx];

		RECT rcCell = *rcFrame;
		rcCell.left = rcFrame->left + pInfo.m_nOffset;
		rcCell.right = rcCell.left + pInfo.m_nWidth;

		if( rcCell.right < 0 || rcCell.left > m_szSize.cx )
			continue;

		::IntersectClipRect(dc, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom);

		if( !pInfo.m_nOffset )
		{
			if( pRecord->m_nIndent )
				rcCell.left += pRecord->m_nIndent*pWinRoot->DRAW_ICON_MARGIN;

			if( m_nStylesEx & LVS_EX_CHECKBOXES )
			{
				RECT rcChecked = GetCheckRect(rcCell, false);

				static_cast<CWinRoot*>(m_pRoot)->DrawCheckBox(dc, rcChecked, pRecord->GetCheck(0), nItemState, true);

				rcCell.left += pWinRoot->DRAW_ICON_MARGIN;
			}

			if( m_hImageList )
			{
				RECT rcIcon = rcCell;
				rcIcon.right = rcIcon.left + pWinRoot->DRAW_ICON_MARGIN;
				InflateRect(&rcIcon, -2, -((rcCell.bottom - rcCell.top - pWinRoot->LIST_ICON_SIZE)/2 + 1));

				DWORD nFlags = ILD_NORMAL|ILD_TRANSPARENT;
//				if( pRecord->m_fSelected )
//					nFlags |= ILD_SELECTED;
				ImageList_Draw(m_hImageList, pRecord->GetStatus(), dc, rcIcon.left, rcIcon.top, nFlags);

				rcCell.left += pWinRoot->DRAW_ICON_MARGIN;
			}
		}

		InflateRect(&rcCell, -2, 0);
		
		tCHAR * strText = pRecord->GetData(nIdx);
		if( !strText )
			strText = "";

		if( pInfo.m_bChecked )
		{
			bool bhasText = !*strText;
			RECT rcChecked = GetCheckRect(rcCell, bhasText);
			static_cast<CWinRoot*>(m_pRoot)->DrawCheckBox(dc, rcChecked, pRecord->GetCheck(nIdx),
				(pRecord->IsCheckDisabled(nIdx) ? ISTATE_DISABLED : 0)|nItemState, true);

			if( bhasText )
				rcCell.left += pWinRoot->DRAW_ICON_MARGIN;
		}
		
		CWindowL::DrawText(m_pRoot, dc, strText, rcCell, DT_END_ELLIPSIS|DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);
		::SelectClipRgn(dc, NULL);
	}

	if( (pRecord && pRecord->m_fFocused) && (m_nState & ISTATE_FOCUSED) )
		::DrawFocusRect(dc, &rcItem);

	m_pRoot->CopyTempDC(dc, hDrawDc, *rcFrame);
	return S_OK;
}

LRESULT CVListCtrlImpl::OnNotifyCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	NMHDR * phdr = (NMHDR *)lParam;

	switch( phdr->code )
	{
	case LVN_GETDISPINFOA:
	case LVN_GETDISPINFOW:
		_GetDispInfo(&((NMLVDISPINFOA *)phdr)->item, phdr->code == LVN_GETDISPINFOA);
		break;
	case LVN_GETINFOTIPA:
	case LVN_GETINFOTIPW:
		_GetTipInfo((NMLVGETINFOTIPA *)lParam, phdr->code == LVN_GETINFOTIPA);
		break;
	
	case LVN_ODCACHEHINT:
	{
		NMLVCACHEHINT* pHint = (NMLVCACHEHINT*)lParam;
		PrepareCache(pHint->iFrom, pHint->iTo);
	}
	break;
	
	case LVN_ODSTATECHANGED:
	{
		LPNMLVODSTATECHANGE nmlv = (LPNMLVODSTATECHANGE)lParam;
		SetSelection(nmlv->iFrom, nmlv->iTo, (nmlv->uNewState & LVIS_SELECTED)!=0);
		OnChangedState(this);
		ctl_Redraw(); //после изменения состояния записей, необходимо перерисовать
	}
	break;
	
	case LVN_ITEMCHANGED:
		if( !m_fSelectTransaction )
		{
			LPNMLISTVIEW nmlv = (LPNMLISTVIEW)lParam;

			if( (nmlv->uNewState & LVIS_FOCUSED) != (nmlv->uOldState & LVIS_FOCUSED) )
			{
				CRecord * pRecord = m_pViewData->GetRecord(nmlv->iItem);
				if( pRecord )
					pRecord->m_fFocused = (nmlv->uNewState & LVIS_FOCUSED) ? 1 : 0;
			}

			if( (nmlv->uNewState & LVIS_SELECTED) != (nmlv->uOldState & LVIS_SELECTED) )
			{
				SetSelection(nmlv->iItem, nmlv->iItem, (nmlv->uNewState & LVIS_SELECTED)!=0);

				if( (nmlv->uNewState & LVIS_SELECTED) /*|| (m_SelTo = -1)*/ )
					OnChangedState(this);
				else
					PostMessage(LVM_CHECK_NOSELSTATE);
			}
			if( (nmlv->uNewState & (LVIS_FOCUSED | LVIS_SELECTED) ) != 
				(nmlv->uOldState & (LVIS_FOCUSED|LVIS_SELECTED) ) )
				ctl_RedrawItem(nmlv->iItem); //после изменения состояния, запись необходимо перерисовать
		}
	break;

	case LVN_COLUMNCLICK:
		_ColumnClick(((NM_LISTVIEW *)lParam)->iSubItem);
		break;
	
//	case NM_DBLCLK:
	case NM_RETURN:
		DblClick();
		break;

	case NM_RCLICK:
	{
		POINT ptAction = ((NMITEMACTIVATE *)lParam)->ptAction;
		ClientToScreen(&ptAction);
		_RightMouseClick(ptAction);
	}
	break;
	}
	
	return S_OK;
}

LRESULT CVListCtrlImpl::OnTTNShow(WPARAM wParam, LPARAM lParam, BOOL& bHandled, bool bAnsi)
{
	NMHDR * pHdr = (NMHDR *)lParam;
	
	bHandled = TRUE;

	HWND hTip = ListView_GetToolTips(m_hWnd);
	if( pHdr->hwndFrom != hTip )
		return bHandled = FALSE, 0;

	if( !m_fSubItemTip )
		return bHandled = FALSE, 0;

	::ClientToScreen(m_hWnd, &m_ptSubItemTip);
	::SetWindowPos(hTip, HWND_TOPMOST,
		m_ptSubItemTip.x, m_ptSubItemTip.y, 0, 0,
		SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
	return TRUE;
}

LRESULT CVListCtrlImpl::OnTTNGetDispInfo(WPARAM wParam, LPARAM lParam, BOOL& bHandled, bool bAnsi)
{
	NMHDR * pHdr = (NMHDR *)lParam;
	
	bHandled = TRUE;

	HWND hTip = ListView_GetToolTips(m_hWnd);
	if( pHdr->hwndFrom != hTip )
		return bHandled = FALSE, 0;

	m_fSubItemTip = 0;

	LVHITTESTINFO _hitTest; memset(&_hitTest, 0, sizeof(_hitTest));
	::GetCursorPos(&_hitTest.pt);
	::ScreenToClient(m_hWnd, &_hitTest.pt);
	ListView_SubItemHitTest(m_hWnd, &_hitTest);

	if( !_hitTest.iSubItem )
		return bHandled = FALSE, 0;

	m_fSubItemTip = 1;
	
	RECT  rcItem;
	PVOID pszText;
	
	ListView_GetSubItemRect(m_hWnd, _hitTest.iItem, _hitTest.iSubItem, LVIR_BOUNDS, &rcItem);
	{
		LVITEMA _Item; memset(&_Item, 0, sizeof(_Item));
		_Item.mask = LVIF_TEXT;
		_Item.iItem = _hitTest.iItem;
		_Item.iSubItem = _hitTest.iSubItem;
		_GetDispInfo(&_Item, bAnsi);
		pszText = _Item.pszText;
	}

	HDC dc = ::GetWindowDC(m_hWnd);
	if( !pszText || !dc || !m_pFont )
		return 0;

	if( bAnsi )
	{
		if( !*(LPCSTR)pszText )
			return 0;
	}
	else
	{
		if( !*(LPCWSTR)pszText )
			return 0;
	}

	RECT rcText = {rcItem.left, rcItem.top, MAX_CTL_SIZE, MAX_CTL_SIZE};
	{
		HGDIOBJ hOldFont = ::SelectObject(dc, m_pFont->Handle());
		if( bAnsi )
		{
			LPCSTR strTextA = (LPCSTR)pszText;
			::DrawTextA(dc, strTextA, (int)strlen(strTextA), &rcText, DT_CALCRECT);
		}
		else
		{
			LPCWSTR strTextW = (LPCWSTR)pszText;
			::DrawTextW(dc, strTextW, (int)wcslen(strTextW), &rcText, DT_CALCRECT);
		}
		
		::SelectObject(dc, hOldFont);
		::ReleaseDC(m_hWnd, dc);
	}

	bool bTipNeed = false;
	{
		RECT rcCli; ::GetClientRect(m_hWnd, &rcCli);
		if( RECT_CX(rcText) > RECT_CX(rcItem) )
			bTipNeed = true;
		if( rcItem.left < 0 )
			bTipNeed = true;
		if( rcItem.left + RECT_CX(rcText) > RECT_CX(rcCli) )
			bTipNeed = true;
	}
	
	if( !bTipNeed )
		return 0;

	m_ptSubItemTip.x = rcItem.left;
	m_ptSubItemTip.y = rcItem.top;
	
	tString& bufItemTip = ((CWinRoot *)m_pRoot)->m_bufItemTip;
	
	if( bAnsi )
	{
		size_t n = (strlen((LPSTR)pszText) + 1)*sizeof(CHAR);
		bufItemTip.resize(n);
		if( bufItemTip.size() == n )
			memcpy(&bufItemTip[0], pszText, n);

		NMTTDISPINFOA * pStruct = (NMTTDISPINFOA *)pHdr;
		pStruct->lpszText = (LPSTR)bufItemTip.c_str();
	}
	else
	{
		size_t n = (wcslen((LPWSTR)pszText) + 1)*sizeof(WCHAR);
		bufItemTip.resize(n);
		if( bufItemTip.size() == n )
			memcpy(&bufItemTip[0], pszText, n);
	
		NMTTDISPINFOW * pStruct = (NMTTDISPINFOW *)pHdr;
		pStruct->lpszText = (LPWSTR)bufItemTip.c_str();
	}

	return 0;
}

LRESULT CVListCtrlImpl::OnNotifyChild(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
//	if( m_fUpdateCols )
//		return S_OK;

	NMHEADER* phdr = (NMHEADER*)lParam;
	ULONG nCode = phdr->hdr.code;

	switch( nCode )
	{
	case TTN_GETDISPINFOA:
	case TTN_GETDISPINFOW:
		return OnTTNGetDispInfo(wParam, lParam, bHandled, nCode == TTN_GETDISPINFOA);
	
	case TTN_SHOW:
		return OnTTNShow(wParam, lParam, bHandled, CWindowL::m_bAnsi);
	
	case HDN_ITEMCHANGINGW:
	case HDN_ITEMCHANGINGA:
	case HDN_ITEMCHANGEDW:
	case HDN_ITEMCHANGEDA:
		PostMessage(LVM_DRAG_DONE);
		break;

	case HDN_ENDDRAG:
		if( (phdr->pitem->iOrder >= 0) && ((unsigned)phdr->pitem->iOrder < m_colsIdx.size()) )
		{
			_MoveColumn(m_colsIdx[phdr->iItem], m_colsIdx[phdr->pitem->iOrder]);
			PostMessage(LVM_DRAG_DONE);
		}
		bHandled = TRUE;
		return TRUE;

	case NM_RCLICK:
		if( m_nStylesEx & LVS_EX_HEADERDRAGDROP )
		{
			POINT ptMenu; GetCursorPos(&ptMenu);
			
			HWND hHeader = ListView_GetHeader(m_hWnd);
			
			HDHITTESTINFO hi = {{ptMenu.x, ptMenu.y}, 0, -1};
			::ScreenToClient(hHeader, &hi.pt);
			::SendMessage(hHeader, HDM_HITTEST, 0, (LPARAM)&hi);
			
			_ColumnRClick(hi.iItem, ptMenu);
		}
		break;
	}
	
	return S_OK;
}

LRESULT CVListCtrlImpl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_nColumnCount == 1 )
	{
		RECT rcList; GetClientRectEx(rcList, false);
		ListView_SetColumnWidthAsyn(m_hWnd, 0, RECT_CX( rcList ));
	}
	
	bHandled = FALSE;
	return S_OK;
}

LRESULT CVListCtrlImpl::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT CVListCtrlImpl::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT CVListCtrlImpl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !wParam )
	{
		bHandled = FALSE;
		return S_OK;
	}

	HDC hDC = (HDC)wParam;

	RECT rcExlude, rcClient, rcLastItem;
	GetClientRectEx(rcClient, false);

	tSIZE_T nCount = GetItemsCount();
	if( nCount )
	{
		ListView_GetItemRect(m_hWnd, ListView_GetTopIndex(m_hWnd), &rcExlude, LVIR_BOUNDS);
		ListView_GetItemRect(m_hWnd, nCount-1, &rcLastItem, LVIR_BOUNDS);

		rcExlude.left = max(2, rcExlude.left);
		rcExlude.bottom = min(rcLastItem.bottom, rcClient.bottom);

		::ExcludeClipRect(hDC, rcExlude.left, rcExlude.top, rcExlude.right, rcExlude.bottom);
	}

	::SetBkColor(hDC, IsWindowEnabled() ? ListView_GetBkColor(m_hWnd) : GetSysColor(COLOR_BTNFACE));
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcClient, NULL, 0, NULL);

	::SelectClipRgn(hDC, NULL);
	return S_OK;
}

LRESULT CVListCtrlImpl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(wParam)
	{
	case VK_SPACE:
		if( m_nStylesEx & LVS_EX_CHECKBOXES )
		{
			tDWORD nItem = ListView_GetNextItem(m_hWnd, -1, LVIS_FOCUSED);
			if( nItem == -1 )
				return S_OK;

			bool bNewCheck = !GetCheck(nItem, 0);

			nItem = -1;
			while( (nItem = ListView_GetNextItem(m_hWnd, nItem, LVIS_SELECTED)) != -1 )
			{
				SetCheck(nItem, 0, bNewCheck, false);
				m_pReader->OnListEvent(nItem, 0, VLISTCTRL_EVENT_CHECKED);
			}

			OnChangedData(this);
			Invalidate();
			return S_OK;
		}
		break;
	
	case VK_DELETE:
		DeleteSelectedItems();
		return S_OK;
		
	case 'A':
		if( GetKeyState(VK_CONTROL) & 0x80000000 )
		{
			if( m_nStyles & LVS_SINGLESEL )
				return S_OK;
			
			SelectAll();
			return S_OK;
		}
		break;

	case 'F':
		if( GetKeyState(VK_CONTROL) & 0x80000000 )
		{
			FindItem(eShowDlg);
			return S_OK;
		}
		break;

	case 'C':
	case VK_INSERT:
		if( GetKeyState(VK_CONTROL) & 0x80000000 )
			CopyToClipboard();
		break;

	case VK_F3:
		FindItem(!(GetKeyState(VK_SHIFT) & 0x80000000) ? eFindDown : eFindUp);
		break;
	}

	return bHandled = FALSE, S_OK;
}

LRESULT CVListCtrlImpl::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	LVHITTESTINFO hi;
	hi.pt.x = GET_X_LPARAM(lParam);
	hi.pt.y = GET_Y_LPARAM(lParam);
	if( ListView_SubItemHitTest(m_hWnd, &hi) == -1 || hi.iItem == -1 || hi.iSubItem == -1 )
		return S_OK;

	// проверка чека в колоках
	CRecord * pRecord = m_pViewData->GetRecord(hi.iItem, true);

	if( hi.iSubItem > 0 )
	{
		int nIdx = m_colsIdx[hi.iSubItem];
		CColInfo& pInfo = m_colsInfo[nIdx];
		if( !pInfo.m_bChecked || (pRecord && pRecord->IsCheckDisabled(nIdx)) )
			return S_OK;

		RECT rcCell;
		ListView_GetItemRect(m_hWnd, hi.iItem, &rcCell, LVIR_BOUNDS);
		rcCell.left = rcCell.left + pInfo.m_nOffset;
		rcCell.right = rcCell.left + pInfo.m_nWidth;

		if( ::PtInRect(&rcCell, hi.pt) )
			SetCheck(hi.iItem, nIdx, !GetCheck(hi.iItem, nIdx));

		bHandled = TRUE;
		return S_OK;
	}

    CWinRoot* const pWinRoot = static_cast<CWinRoot*>(m_pRoot);
	tDWORD nIndent = pRecord ? pRecord->m_nIndent : 0;

	if( (m_nStylesEx & LVS_EX_CHECKBOXES) && (hi.flags & LVHT_ONITEMSTATEICON) &&
		((tDWORD)hi.pt.x >= (nIndent + 0)*pWinRoot->DRAW_ICON_MARGIN) &&
		((tDWORD)hi.pt.x <  (nIndent + 1)*pWinRoot->DRAW_ICON_MARGIN) )
	{
		SetCheck(hi.iItem, 0, !GetCheck(hi.iItem, 0));
		bHandled = TRUE;
		return S_OK;
	}

	if( ListView_GetImageList(m_hWnd, LVSIL_SMALL) && (hi.flags & LVHT_ONITEMICON) &&
		((tDWORD)hi.pt.x >= (nIndent + 0 + ((m_nStylesEx & LVS_EX_CHECKBOXES) ? 1 : 0))*pWinRoot->DRAW_ICON_MARGIN) &&
		((tDWORD)hi.pt.x <= (nIndent + 1 + ((m_nStylesEx & LVS_EX_CHECKBOXES) ? 1 : 0))*pWinRoot->DRAW_ICON_MARGIN) )
	{
//		if( m_pSink )
//			((CListItemSink*)m_pSink)->OnClickIcon(hi.iItem);
		bHandled = TRUE;
		return S_OK;
	}
	return S_OK;
}

LRESULT CVListCtrlImpl::OnLButtonDblClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return OnLButtonDown(WM_LBUTTONDOWN, wParam, lParam, bHandled);
}

LRESULT CVListCtrlImpl::OnDragDone(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdateColumnsData(true);
	return S_OK;
}

LRESULT CVListCtrlImpl::OnCheckNoSelState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !GetSelectedCount() )
		OnChangedState(this);
	return S_OK;
}

LRESULT CVListCtrlImpl::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	switch( LOWORD(wParam) )
	{
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		if( !m_scrollInfo.nMax )
		{
			m_scrollInfo.cbSize = sizeof(m_scrollInfo);
			m_scrollInfo.fMask = SIF_RANGE|SIF_PAGE;
			::GetScrollInfo(m_hWnd, SB_VERT, &m_scrollInfo);
		}
		
		m_scrollInfo.nPos = HIWORD(wParam);
		
		m_fBlockAutoScroll = 1;
		break;
	
	case SB_ENDSCROLL:
		bHandled = TRUE;
		if( m_fBlockAutoScroll )
		{
			//PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::CVListCtrlImpl::OnVScroll(SB_ENDSCROLL) nPage(%u) nMax(%u) nPos(%u)", m_scrollInfo.nPage, m_scrollInfo.nMax, m_scrollInfo.nPos));
			
			if( (UINT)m_scrollInfo.nPos >= m_scrollInfo.nMax - m_scrollInfo.nPage )
				m_fBlockAutoScroll = 0;
			
			m_scrollInfo.nMax = 0;
		}
		break;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void CVListCtrlImpl::ctl_DeleteAllItems()
{
	ListView_DeleteAllItems(m_hWnd);
}

void CVListCtrlImpl::ctl_RedrawItem(int nItem)
{
	ListView_Update(m_hWnd, nItem);
}

void CVListCtrlImpl::ctl_Redraw()
{
	Invalidate();
}

void CVListCtrlImpl::ctl_SetItemCount(tDWORD nCount)
{
	ListView_SetItemCountEx(m_hWnd, nCount, LVSICF_NOSCROLL);
}

tDWORD CVListCtrlImpl::ctl_GetItemState(tSIZE_T nItem, tDWORD nMask)
{
	return _CnvStateBack(ListView_GetItemState(m_hWnd, nItem, _CnvState(nMask)));
}

void CVListCtrlImpl::ctl_SetItemState(tSIZE_T nItem, tDWORD nState, tDWORD nMask)
{
	ListView_SetItemState(m_hWnd, nItem, _CnvState(nState), _CnvState(nMask));
}

int WINAPI Hooked_ScrollWindowEx(HWND hWnd, int dx, int dy, const RECT *prcScroll, const RECT *prcClip, HRGN hrgnUpdate, LPRECT prcUpdate, UINT flags)
{
	ValidateRect(hWnd, NULL);
	return NULLREGION;
}

void CVListCtrlImpl::ctl_ScrollDown()
{
	static PVOID pfnScrollWindowEx = NULL;
	static int (WINAPI *fnScrollWindowEx)(HWND hWnd, int dx, int dy, const RECT *prcScroll, const RECT *prcClip, HRGN hrgnUpdate, LPRECT prcUpdate, UINT flags) = NULL;
	static bool bNt = !(GetVersion() & 0x80000000);

	if( bNt )
	{
		if( !pfnScrollWindowEx && pfnScrollWindowEx != (PVOID)(-1) )
		{
			HMODULE hUser32 = GetModuleHandle("comctl32.dll");
			if( hUser32 )
				pfnScrollWindowEx = GetProcImportPtr(hUser32, "ScrollWindowEx");

			if( !pfnScrollWindowEx )
				pfnScrollWindowEx = (PVOID)(-1);
			else
				*(void**)&fnScrollWindowEx = *(void**)pfnScrollWindowEx;
		}

		if( fnScrollWindowEx )
			*(void**)pfnScrollWindowEx = Hooked_ScrollWindowEx;
	}

	SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
	
	if( bNt )
	{
		if( fnScrollWindowEx )
		{
			*(void**)pfnScrollWindowEx = fnScrollWindowEx;
			InvalidateRect(NULL);
			UpdateWindow();
		}
	}
}

tDWORD CVListCtrlImpl::ctl_ColGetIdx(int nDisplayPos)
{
	return (size_t)nDisplayPos < m_colsIdx.size() ? m_colsIdx[nDisplayPos] : -1;
}

//////////////////////////////////////////////////////////////////////

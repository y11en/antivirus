//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegEdit.h"

#include "RegEditDoc.h"
#include "RegEditView.h"

#include "PersistentData.h"
#include "RegMetadata.h"

#include "DlgEditorInt.h"
#include "DlgEditorString.h"
#include "DlgEditorBin.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
IMPLEMENT_DYNCREATE(CRegEditView, CListView)

BEGIN_MESSAGE_MAP(CRegEditView, CListView)
	//{{AFX_MSG_MAP(CRegEditView)
	ON_WM_LBUTTONDBLCLK()
	ON_UPDATE_COMMAND_UI(ID_MODIFY_VALUE, OnUpdateModifyValue)
	ON_COMMAND(ID_MODIFY_VALUE, OnModifyValue)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_DELETE_ITEM, OnUpdateDeleteItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_UPDATE_COMMAND_UI(ID_RENAME_ITEM, OnUpdateRenameItem)
	ON_COMMAND(ID_RENAME_ITEM, OnRenameItem)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_UPDATE_COMMAND_UI(ID_NEW_STRING_ITEM, OnUpdateNewStringItem)
	ON_COMMAND(ID_NEW_STRING_ITEM, OnNewStringItem)
	ON_UPDATE_COMMAND_UI(ID_NEW_DWORD_ITEM, OnUpdateNewDwordItem)
	ON_COMMAND(ID_NEW_DWORD_ITEM, OnNewDwordItem)
	ON_UPDATE_COMMAND_UI(ID_NEW_BINARY_ITEM, OnUpdateNewBinaryItem)
	ON_COMMAND(ID_NEW_BINARY_ITEM, OnNewBinaryItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CRegEditView::CRegEditView()
{
	m_dwCurKey = 0;
}

CRegEditView::~CRegEditView()
{
}

BOOL CRegEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |=	LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS | LVS_NOSORTHEADER;
	return CListView::PreCreateWindow(cs);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditView::OnDraw(CDC* pDC)
{
	CRegEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditView::OnInitialUpdate()
{
	//-----------------------------------------------------//
	CListView::OnInitialUpdate();

	//-----------------------------------------------------//

	if(m_lstImage)
		return;

	m_lstImage.Create(IDB_TREE_IMAGES,16,0,RGB(255,0,255));
	m_lstImage.SetBkColor(GetSysColor(COLOR_WINDOW));
	GetListCtrl().SetImageList(&m_lstImage, LVSIL_SMALL);

	//-----------------------------------------------------//
	int nLen1 = 100, nLen2 = 100, nLen3 = 100;
	CString strColInfo =
		GlobalGetApp()->GetPersistentData(DATA_CODE_COLUMNS);
	
	if(!strColInfo.IsEmpty())
		sscanf(strColInfo, _T("%d, %d, %d"), &nLen1, &nLen2, &nLen3);

	//-----------------------------------------------------//
	LVCOLUMN lvc; 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
    lvc.iSubItem = 0;
    lvc.pszText = _T("Name");	
    lvc.cx = nLen1;			// width of column in pixels
    lvc.fmt = LVCFMT_LEFT;  // left-aligned column
	GetListCtrl().InsertColumn(0, &lvc);

    lvc.iSubItem = 1;
    lvc.pszText = _T("Type");	
    lvc.cx = nLen2;			// width of column in pixels
    lvc.fmt = LVCFMT_LEFT;  // left-aligned column
	GetListCtrl().InsertColumn(1, &lvc);

    lvc.iSubItem = 2;
    lvc.pszText = _T("Data");	
    lvc.cx = nLen3;			// width of column in pixels
    lvc.fmt = LVCFMT_LEFT;  // left-aligned column
	GetListCtrl().InsertColumn(2, &lvc);

	//-----------------------------------------------------//

	//if(m_dwCurKey)
		UpdateKeyValues(m_dwCurKey);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CString CRegEditView::GetColumnsInfo()
{
	int nLen1, nLen2, nLen3;
	CRect rc;

	GetListCtrl().GetHeaderCtrl()->GetItemRect(0, rc);
	nLen1 = rc.Width();

	GetListCtrl().GetHeaderCtrl()->GetItemRect(1, rc);
	nLen2 = rc.Width();

	GetListCtrl().GetHeaderCtrl()->GetItemRect(2, rc);
	nLen3 = rc.Width();

	CString strColInfo;
	strColInfo.Format(_T("%d, %d, %d"), nLen1, nLen2, nLen3);
	return strColInfo;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditView::GetValueCount(tDWORD& o_dwCount)
{
	cRegistry* pReg = GetDocument()->m_pReg;
	tRegKey oCurKey = (tRegKey) m_dwCurKey;

	tERROR nErr = pReg->GetValueCount(&o_dwCount, oCurKey);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot get values count."), nErr);
		return FALSE;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditView::GetValueTypeAndVal(tDWORD i_dwIdx, tTYPE_ID& o_nType, BYTE* o_pBuf, tDWORD& io_BufSize)
{
	cRegistry* pReg = GetDocument()->m_pReg;
	tRegKey oCurKey = (tRegKey) m_dwCurKey;

	tDWORD dwDataLen;
	tERROR nErr = pReg->GetValueByIndex(&dwDataLen, oCurKey, i_dwIdx, &o_nType, o_pBuf, io_BufSize);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot get value by index."), nErr);
		return FALSE;
	}

	if(dwDataLen >= (io_BufSize-1))
		dwDataLen = io_BufSize - 1;

	o_pBuf[io_BufSize-1] = 0;
	io_BufSize = dwDataLen;

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditView::SetValueTypeAndVal(tDWORD i_dwIdx, tTYPE_ID i_nType, BYTE* i_pBuf, tDWORD i_nBufSize)
{
	cRegistry* pReg = GetDocument()->m_pReg;
	tRegKey oCurKey = (tRegKey) m_dwCurKey;

	tERROR nErr = pReg->SetValueByIndex(oCurKey, i_dwIdx, i_nType, i_pBuf, i_nBufSize, FALSE);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot set value by index."), nErr);
		return FALSE;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditView::AppendValue(LPCTSTR i_strValName, tTYPE_ID i_nType, BYTE* i_pBuf, tDWORD i_nBufSize)
{
	cRegistry* pReg = GetDocument()->m_pReg;
	tRegKey oCurKey = (tRegKey) m_dwCurKey;

	tERROR nErr = pReg->SetValue(oCurKey, i_strValName, i_nType, i_pBuf, i_nBufSize, TRUE);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot set value by index."), nErr);
		return FALSE;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditView::GetValueName(tDWORD i_dwIdx, CString& o_strName)
{
	cRegistry* pReg = GetDocument()->m_pReg;
	tRegKey oCurKey = (tRegKey) m_dwCurKey;
							
	tDWORD dwRet;
	tERROR nErr = pReg->GetValueNameByIndex(&dwRet, oCurKey, i_dwIdx, o_strName.GetBufferSetLength(1024), 1024);
	o_strName.ReleaseBuffer();

	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot get value name by index."), nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditView::DeleteValue(tDWORD i_dwIdx)
{
	cRegistry* pReg = GetDocument()->m_pReg;
	tRegKey oCurKey = (tRegKey) m_dwCurKey;
							
	tERROR nErr = pReg->DeleteValueByInd(oCurKey, i_dwIdx);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot delete value."), nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditView::AppendValue(tTYPE_ID i_nType)
{
	tDWORD dwValCount;
	if(!GetValueCount(dwValCount))
		return FALSE;

	LPCTSTR strNewValueName = _T("New value name");
	BYTE	arrBuf[2]=  { 0 , 0 };
	if(!AppendValue(strNewValueName, i_nType, arrBuf, 0))
		return FALSE;

	UINT uiImg;
	CString strTypeTitle;
	MD::GetTypeImgAndTitle(i_nType, uiImg, strTypeTitle);

	int nItems = GetListCtrl().GetItemCount();
	int nItemIdx = GetListCtrl().InsertItem(nItems, strNewValueName, uiImg);
	GetListCtrl().SetItemText(nItemIdx, 1, strTypeTitle);
	GetListCtrl().SetItemData(nItemIdx, dwValCount);

	GetListCtrl().SetItemState(nItemIdx, LVIS_SELECTED, LVIS_SELECTED); 

	OnRenameItem();

	GetDocument()->SetModifiedFlag();

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditView::AdjustItemDataOnDeletion(DWORD i_dwItemDeleted)
{
	int nItems = GetListCtrl().GetItemCount();
	for(int i=0; i<nItems; i++)
	{
		DWORD dwCurData = GetListCtrl().GetItemData(i);
		if(dwCurData > i_dwItemDeleted)
			GetListCtrl().SetItemData(i, dwCurData-1);
	}
}


// ---
void CRegEditView::insert( CListCtrl& list, CString& name, DWORD ins, DWORD idx ) {
	tTYPE_ID nType;
	BYTE	   arrBuf[4096];
	tDWORD	 dwDataLen = 4096;

	if( !GetValueTypeAndVal(idx,nType,arrBuf,dwDataLen) )
		return;

	DWORD err;
	UINT uiImg;
	CString strTypeTitle;
	MD::GetTypeImgAndTitle( nType, uiImg, strTypeTitle );

	int nItemIdx = list.InsertItem( ins, name, uiImg );
	if ( !list.SetItemText(nItemIdx,1,strTypeTitle) )
		err = ::GetLastError();

	MD::GetDataStrRep( nType, arrBuf, dwDataLen, strTypeTitle );
	if ( !list.SetItemText(nItemIdx,2,strTypeTitle) )
		err = ::GetLastError();

	list.SetItemData( nItemIdx, idx );
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditView::UpdateKeyValues(DWORD i_dwKey) {
	CListCtrl& list = GetListCtrl();
	list.DeleteAllItems();

	m_dwCurKey = i_dwKey;
	//if(0 == m_dwCurKey)
	//	return;

	tDWORD dwValCount;
	if( !GetValueCount(dwValCount) )
		return;

	tDWORD i, ins = 0;
	CString strValueName;
	for( i=0; i<dwValCount; i++ ) {
		if( !GetValueName(i,strValueName) )
			return;
		if ( strValueName.IsEmpty() ) {
			strValueName = "(default)";
			insert( list, strValueName, ins++, i );
			break;
		}
	}

	for( i=0; i<dwValCount; i++ ) {
		if(!GetValueName(i, strValueName))
			return;
		if ( strValueName.IsEmpty() )
			continue;
		insert( list, strValueName, ins++, i );
	}

	if( list.GetItemCount() )
		list.SetItemState( 0, LVIS_FOCUSED, LVIS_FOCUSED ); 
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
int  CRegEditView::GetSelectedItem()
{
	return GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditView::EditCurrentValue()
{
	//---------------------------------------------------------//
	int nSelItem = GetSelectedItem();
	ASSERT(-1 != nSelItem);

	tDWORD dwItemIdx = GetListCtrl().GetItemData(nSelItem);

	tTYPE_ID nType;
	BYTE	arrBuf[4096];
	tDWORD	dwDataLen = 4096;

	if(!GetValueTypeAndVal(dwItemIdx, nType, arrBuf, dwDataLen))
		return;

	CString strValueName;
	if(!GetValueName(dwItemIdx, strValueName))
		return;

	BOOL bModified = FALSE;

	//---------------------------------------------------------//
	if(MD::IsTypeEditDecimal(nType))
	{
		CDlgEditorInt oDlg;
		oDlg.m_strValueName = strValueName;
		oDlg.m_nType		= nType;
		oDlg.m_pBuf			= arrBuf;

		if(IDOK == oDlg.DoModal())
			bModified = oDlg.m_bModified;
	}
	else
	//---------------------------------------------------------//
	if(MD::IsTypeEditString(nType))
	{
		DlgEditorString oDlg;
		oDlg.m_strValueName = strValueName;
		oDlg.m_nType		= nType;
		oDlg.m_pBuf			= arrBuf;
		oDlg.m_dwSize		= dwDataLen;

		if(IDOK == oDlg.DoModal())
			bModified = oDlg.m_bModified;

		if(bModified)
			dwDataLen = oDlg.m_dwSize + 1;

	}
	else
	//---------------------------------------------------------//
	if(MD::IsTypeEditBinary(nType))
	{
		AfxMessageBox("Not implemented!");
	}
	else
		AfxMessageBox("Not implemented!");

	if(bModified)
	{
		//---------------------------------------------------------//
		// update value in the registry
		if(SetValueTypeAndVal(dwItemIdx, nType, arrBuf, dwDataLen))
		{
			//---------------------------------------------------------//
			// update value on the screen
			CString strValue;
			MD::GetDataStrRep(nType, arrBuf, dwDataLen, strValue);
			GetListCtrl().SetItemText(nSelItem, 2, strValue);

			GetDocument()->SetModifiedFlag();
		}
	}

	SetFocus();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditView::PerformRename(int i_nItem, LPCTSTR i_strNewName)
{
	tDWORD dwValCount;
	if(!GetValueCount(dwValCount))
		return;

	DWORD dwValIndex = GetListCtrl().GetItemData(i_nItem);

	tTYPE_ID nType;
	BYTE	arrBuf[4096];
	tDWORD	dwDataLen = 4096;

	if(!GetValueTypeAndVal(dwValIndex, nType, arrBuf, dwDataLen))
		return;

	if(!AppendValue(i_strNewName, nType, arrBuf, dwDataLen))
		return;

	if(!DeleteValue(dwValIndex))
		return;

	AdjustItemDataOnDeletion(dwValIndex);
	
	GetListCtrl().SetItemText(i_nItem, 0, i_strNewName);
	GetListCtrl().SetItemData(i_nItem, dwValCount-1);

	GetDocument()->SetModifiedFlag();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(UPD_HINT_SHOW_THE_KEY == lHint && pSender != this)
		UpdateKeyValues(DWORD(pHint));
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CRegEditView message handlers
void CRegEditView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
}

// ---
void CRegEditView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListView::OnLButtonDblClk(nFlags, point);

	UINT uFlags;
	GetListCtrl().HitTest(point, &uFlags);
	if (uFlags & LVHT_ONITEMLABEL || uFlags & LVHT_ONITEMICON)
		SendMessage(WM_COMMAND, ID_MODIFY_VALUE);
}
			
// ---
void CRegEditView::OnUpdateModifyValue(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE == GetDocument()->m_bReadOnly
		&& -1 != GetSelectedItem());
}

// ---
void CRegEditView::OnModifyValue() 
{
	EditCurrentValue();
}
		
// ---
void CRegEditView::OnUpdateDeleteItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE == GetDocument()->m_bReadOnly &&
		-1 != GetSelectedItem());
}

// ---
void CRegEditView::OnDeleteItem() 
{
	int nItemSel = GetSelectedItem();
	DWORD dwValIndex = GetListCtrl().GetItemData(nItemSel);
	if(DeleteValue(dwValIndex))
	{
		AdjustItemDataOnDeletion(dwValIndex);
		GetListCtrl().DeleteItem(nItemSel);
		GetDocument()->SetModifiedFlag();
	}
}

// ---
void CRegEditView::OnUpdateRenameItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE == GetDocument()->m_bReadOnly &&
		-1 != GetSelectedItem());
}

void CRegEditView::OnRenameItem() 
{
	int nItemSel = GetSelectedItem();
	GetListCtrl().EditLabel(nItemSel);
}

// ---
void CRegEditView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if(-1 == point.x || -1 == point.y)
	{
		CRect rc;
		GetWindowRect(rc);
		point = rc.TopLeft();
		point += CPoint(2, 2);
	}

	pWnd;
	CMenu oMenu;
	if(oMenu.LoadMenu(IDR_MENU_VALUES_PANE))
	{
		CMenu* pSubMenu = oMenu.GetSubMenu(0);
		if (pSubMenu)
			pSubMenu->TrackPopupMenu(0, point.x, point.y, this);
		oMenu.DestroyMenu();
	}
}

// ---
void CRegEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
	if(nChar == VK_RETURN)
		SendMessage(WM_COMMAND, ID_MODIFY_VALUE);
	else
	if(nChar == VK_F2)
		SendMessage(WM_COMMAND, ID_RENAME_ITEM);
	else
	if(nChar == VK_DELETE)
		SendMessage(WM_COMMAND, ID_DELETE_ITEM);
}

// ---
void CRegEditView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(!pDispInfo->item.pszText)
		return;

	int nItem = pDispInfo->item.iItem;
	CString strNewName = pDispInfo->item.pszText;

	// check if new value is empty
	if(strNewName.IsEmpty())
	{
		AfxMessageBox(_T("You could not set empty value name."));
		return;
	}

	// check if this value wasn't changed
	DWORD dwValIndex = GetListCtrl().GetItemData(nItem);
	CString strValueName;
	if(!GetValueName(dwValIndex, strValueName))
			return;

	if(strNewName != strValueName)
		PerformRename(nItem, strNewName);
}

// ---
void CRegEditView::OnUpdateNewStringItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE == GetDocument()->m_bReadOnly);
}

void CRegEditView::OnNewStringItem() 
{
	AppendValue(tid_STRING);	
}

// ---
void CRegEditView::OnUpdateNewDwordItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE == GetDocument()->m_bReadOnly);
}

void CRegEditView::OnNewDwordItem() 
{
	AppendValue(tid_DWORD);	
}

// ---
void CRegEditView::OnUpdateNewBinaryItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE == GetDocument()->m_bReadOnly);
}

void CRegEditView::OnNewBinaryItem() 
{
	AppendValue(tid_BINARY);	
}
		  
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CRegEditView diagnostics

#ifdef _DEBUG
void CRegEditView::AssertValid() const
{
	CListView::AssertValid();
}

void CRegEditView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CRegEditDoc* CRegEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRegEditDoc)));
	return (CRegEditDoc*)m_pDocument;
}
#endif //_DEBUG
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//





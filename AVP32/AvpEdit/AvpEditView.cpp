// AvpEditView.cpp : implementation of the CAvpEditView class
//

#include "stdafx.h"
#include "AvpEdit.h"
#include "MainFrm.h"

#include "AvpEditDoc.h"
#include "AvpEditView.h"

#include "dialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* MainFramePtr;

/////////////////////////////////////////////////////////////////////////////
// CAvpEditView

IMPLEMENT_DYNCREATE(CAvpEditView, CListView)

BEGIN_MESSAGE_MAP(CAvpEditView, CListView)
	//{{AFX_MSG_MAP(CAvpEditView)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_COMMAND(ID_EDIT_INSERTRECORD, OnEditInsertrecord)
	ON_COMMAND(ID_EDIT_DELETERECORD, OnEditDeleterecord)
	ON_COMMAND(ID_EDIT_EDITRECORD, OnEditEditrecord)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_FILE_PACK, OnFilePack)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_COMMAND(ID_WINDOW_NEXT, OnWindowNext)
	ON_COMMAND(ID_FILE_SAVERELOAD, OnFileSavereload)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemchanging)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FINDAGAIN, OnEditFindagain)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvpEditView construction/destruction

CAvpEditView::CAvpEditView()
{
	// TODO: add construction code here
	LastInsertType=0;
}

CAvpEditView::~CAvpEditView()
{
}

BOOL CAvpEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~(LVS_LIST | LVS_ICON | LVS_SMALLICON);
	cs.style |= LVS_REPORT;
//	cs.style |= LVS_SINGLESEL;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAvpEditView drawing

void CAvpEditView::OnDraw(CDC* pDC)
{
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


/*
UINT ThreadAddItems( LPVOID pParam )
{
	return	((CAvpEditView*)pParam)->AddItems();
}


UINT CAvpEditView::AddItems() 
{
	LV_ITEM			lvitem;
	CAvpEditDoc* pDoc = GetDocument();

	CListCtrl& ctlList = (CListCtrl&) GetListCtrl();

	CString title=pDoc->GetTitle();
//	SendMessage(WM_ADD_ITEMS,(UINT)&(title + "  Opening..."));

//	pDoc->SetTitle(title+"  Opening...");
//	MainFramePtr->SetMessageText("Base Opening...");

	int k=pDoc->EditArray.GetSize();
	ctlList.SetItemCount(k+200);

	
	for(int i=0;i<k;i++){
//		CRecordEdit* rep=(CRecordEdit*)(pDoc->EditArray[i]);

		int j=ctlList.GetItemCount();

		lvitem.iItem = j;
		lvitem.iSubItem = 0;
		lvitem.mask = LVIF_TEXT;
		if(i == 0){
			lvitem.state = LVIS_FOCUSED;
			lvitem.mask |= LVIF_STATE;
		}

		lvitem.pszText = LPSTR_TEXTCALLBACK;
		ctlList.InsertItem(&lvitem);
		ctlList.SetItemText(j,1,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(j,2,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(j,3,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(j,4,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(j,5,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(j,6,LPSTR_TEXTCALLBACK);
	}

///	SendMessage(WM_ADD_ITEMS,(UINT)&title);
//	pDoc->SetTitle(title);
	MainFramePtr->SetMessageText(title+" is loaded.");
	return i;
}
*/	
	



#include "OptDlg.h"
extern COptDlg OptDlg;
CFont cf;

void CAvpEditView::OnInitialUpdate()
{

	CAvpEditDoc* pDoc = GetDocument(); 
	ASSERT_VALID(pDoc);

	if(OptDlg.m_FontName.GetLength()){
		cf.CreatePointFont(OptDlg.m_FontSize,OptDlg.m_FontName);
		GetListCtrl().SetFont(&cf);
	}

	CListView::OnInitialUpdate();
	CListCtrl& ctlList = (CListCtrl&) GetListCtrl();

	ListView_SetExtendedListViewStyle(ctlList.m_hWnd, ListView_GetExtendedListViewStyle(ctlList.m_hWnd)|LVS_EX_FULLROWSELECT);

	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for(int i=0;i<pDoc->ColumnCount;i++)
	{
		lvcolumn.pszText = pDoc->ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = pDoc->ColumnCx[i];
		ctlList.InsertColumn(i, &lvcolumn);
	}

	int k=pDoc->EditArray.GetSize();

	CString title=pDoc->GetTitle();
	MainFramePtr->SetMessageText(title+" Loading...");
	MainFramePtr->UpdateWindow();
	BeginWaitCursor();

	ctlList.SetItemCount(k+200);
	LV_ITEM lvitem;
	lvitem.mask = LVIF_TEXT;
	lvitem.pszText = LPSTR_TEXTCALLBACK;
	for(i=0;i<k;i++){
		lvitem.iItem = i;
		lvitem.iSubItem = 0;	ctlList.InsertItem(&lvitem);
		lvitem.iSubItem = 1;	ctlList.SetItem(&lvitem);
		lvitem.iSubItem = 2;	ctlList.SetItem(&lvitem);
		lvitem.iSubItem = 3;	ctlList.SetItem(&lvitem);
		lvitem.iSubItem = 4;	ctlList.SetItem(&lvitem);
		lvitem.iSubItem = 5;	ctlList.SetItem(&lvitem);
		lvitem.iSubItem = 6;	ctlList.SetItem(&lvitem);
		lvitem.iSubItem = 7;	ctlList.SetItem(&lvitem);
		if(i%100==0){
			CString s;
			s.Format("  Loading...%d%%",(i*100)/k);
			MainFramePtr->SetMessageText(title+s);
			MainFramePtr->UpdateWindow();
		}
	}



	BOOL max;
	CMDIChildWnd* w=MainFramePtr->MDIGetActive(&max);
	if(w==NULL)((CMDIChildWnd*)	GetParent())->MDIMaximize();

	MainFramePtr->SetMessageText(title+" is loaded.");
	MainFramePtr->UpdateWindow();
	EndWaitCursor();

//	AfxBeginThread(ThreadAddItems,this);
//	pDoc->InitialUpdateView(this);

}

/////////////////////////////////////////////////////////////////////////////
// CAvpEditView printing

BOOL CAvpEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAvpEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAvpEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAvpEditView diagnostics

#ifdef _DEBUG
void CAvpEditView::AssertValid() const
{
	CListView::AssertValid();
}

void CAvpEditView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CAvpEditDoc* CAvpEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAvpEditDoc)));
	return (CAvpEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAvpEditView message handlers



void CAvpEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	LV_ITEM			lvitem;
	int i;
	CRecordEdit* re;

	switch(lHint){
	case UV_ADD:
		if(((UVStruct*)pHint)->Sender != this) break;

	case UV_INSERT:
	case UV_CHANGE:
		{
		CListCtrl& ctlList=GetListCtrl();
		re=((UVStruct*)pHint)->REptr;
		i =((UVStruct*)pHint)->Index;

		
		if(lHint!=UV_CHANGE) 
		{
			int j=ctlList.GetItemCount();
			if((j%200)==0)ctlList.SetItemCount(j+200);

			if(lHint==UV_ADD)i=j;
			
			lvitem.mask = LVIF_TEXT;
			lvitem.iSubItem = 0;
			lvitem.pszText = NULL;
			lvitem.iItem = i;
			ctlList.InsertItem(&lvitem);
		}
		ctlList.SetItemText(i,0,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(i,1,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(i,2,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(i,3,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(i,4,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(i,5,LPSTR_TEXTCALLBACK);
		ctlList.SetItemText(i,6,LPSTR_TEXTCALLBACK);

		UpdateWindow();
		}
		break;
	case UV_DELETE:
		GetListCtrl().DeleteItem(((UVStruct*)pHint)->Index);
		break;
	
	default:
		CListView::OnUpdate( pSender, lHint, pHint);
		return;
		break;

	}

	while(-1 != (i=GetListCtrl().GetNextItem(-1,LVNI_FOCUSED)))
		GetListCtrl().SetItemState(i,0,LVIS_FOCUSED);
	
	i= (lHint==UV_DELETE ||lHint==UV_CHANGE)?0:1;
	i+=((UVStruct*)pHint)->Index;
	int j=GetListCtrl().GetItemCount()-1;
	if(i>j)i=j;
	GetListCtrl().SetItemState(i,LVIS_FOCUSED,LVIS_FOCUSED);

}

void CAvpEditView::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch(pLVKeyDow->wVKey){
	case VK_RETURN:
		OnEditEditrecord();
		break;
	case VK_INSERT:
		OnEditInsertrecord();
		break;
	case VK_DELETE:
		OnEditDeleterecord();
	default:
		break;
	}
	
	*pResult = 0;
}

void CAvpEditView::OnEditInsertrecord() 
{
	int ret=IDOK;
	CDInsert di;
	di.m_iType=LastInsertType;
	if(IDOK!=di.DoModal()) return;

	CRecordEdit* re=new CRecordEdit(di.m_iType);

	int i=GetListCtrl().GetNextItem(-1,LVNI_FOCUSED);
	int j=GetListCtrl().GetItemCount();
	if(i==-1 || (i==(j-1) && -1==GetListCtrl().GetNextItem(-1,LVNI_SELECTED))) i=j;

	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(i>0)	
	{
		CString s=((CRecordEdit*)(pDoc->EditArray[i-1]))->GetName();
		if(s.GetLength() && di.m_iType<=RT_EXTRACT)
			re->SetName(CString((s[0]=='#')?"":"# ") + s);
	}
		

	ret = re->EditRecord();
	if(ret != IDOK){	delete re;	return;	}

	pDoc->InsertRecord(i,re);

	LastInsertType=di.m_iType;
}

void CAvpEditView::OnEditDeleterecord() 
{

	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int i=GetListCtrl().GetNextItem(-1,LVNI_SELECTED);
	if(i == -1) return;
	if(IDYES != AfxMessageBox(_T("Delete selected records?"),MB_YESNO)) return;

	while(i != -1){
		pDoc->DeleteRecord(i);
		i=GetListCtrl().GetNextItem(-1,LVNI_SELECTED);
	}
}


void CAvpEditView::OnEditEditrecord() 
{
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int i=GetListCtrl().GetNextItem(-1,LVNI_FOCUSED);
	if(i == -1)return;

	if(IDOK!=pDoc->EditRecord(i))return;
}

void CAvpEditView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnEditEditrecord();
	*pResult = 0;
}

void CAvpEditView::OnEditCopy() 
{
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int i=GetListCtrl().GetNextItem(-1,LVNI_SELECTED);
	if(i == -1) return;

	pDoc->ClipboardFree();

	while(i != -1){
		pDoc->ClipboardCopy(i);
		i=GetListCtrl().GetNextItem(i,LVNI_SELECTED);
	}

}

void CAvpEditView::OnEditCut() 
{
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int i=GetListCtrl().GetNextItem(-1,LVNI_SELECTED);
	if(i == -1) return;

	pDoc->ClipboardFree();

	while(i != -1){
		pDoc->ClipboardCopy(i);
		pDoc->DeleteRecord(i);
		i=GetListCtrl().GetNextItem(-1,LVNI_SELECTED);
	}
}

void CAvpEditView::OnEditPaste() 
{
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int i=GetListCtrl().GetNextItem(-1,LVNI_FOCUSED);
	int j=GetListCtrl().GetItemCount();
	if(i==-1 || (i==(j-1) && -1==GetListCtrl().GetNextItem(-1,LVNI_SELECTED))) i=j;

	pDoc->ClipboardPaste(i);
}

void CAvpEditView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->ClipboardCount());
}

void CAvpEditView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((-1 != GetListCtrl().GetNextItem(-1,LVNI_SELECTED)));
}

void CAvpEditView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((-1 != GetListCtrl().GetNextItem(-1,LVNI_SELECTED)));
}


extern BOOL SignFile(const char* lpszPathName);

void CAvpEditView::OnFilePack() 
{
	CAvpEditDoc* doc=(CAvpEditDoc*)GetDocument();
	CString s=doc->GetPathName();
	doc->Compression=1;

	s=s.Left(s.GetLength()-3)+"avc";

	CFile f(s,CFile::modeWrite|CFile::modeCreate);
	CArchive a(&f,CArchive::store);
	GetDocument()->Serialize(a);
	a.Flush();
	a.Close();
	f.Close();

	SignFile(s);
}


void CAvpEditView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
    if (pDispInfo->item.mask & TVIF_TEXT)
    {
		int i;
		CAvpEditDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CListCtrl& ctlList = (CListCtrl&) GetListCtrl();
		CRecordEdit* rep=(CRecordEdit*)(pDoc->EditArray[pDispInfo->item.iItem]);
		const char* sptr;
		switch(pDispInfo->item.iSubItem)
		{
		case 0: 
			if(rep->Type==RT_SEPARATOR)sptr=rep->GetComment();
			else sptr=rep->GetName();

			for(i=0;i<pDoc->ColumnCount;i++)
				pDoc->ColumnCx[i]=ctlList.GetColumnWidth(i);
			break;

		case 1:		sptr=rep->IsModified()?"*":"";		break;
		case 2:		sptr=rep->GetTypeString();			break;
		case 3:		sptr=rep->GetSubTypeString();		break;
		case 4:		sptr=rep->GetMethodString();		break;
		case 5:		sptr=rep->GetLink16String();		break;
		case 6:		sptr=rep->GetLink32String();		break;
		case 7:		sptr=rep->GetComment();			break;

		}
		pDispInfo->item.pszText = (char*)sptr;
	//	lstrcpy (pDispInfo->item.pszText,sptr);
    }

	*pResult = 0;
}


void CAvpEditView::OnWindowNext() 
{
	MainFramePtr->MDINext();
}

void CAvpEditView::OnFileSavereload() 
{
	// TODO: Add your command handler code here
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	SendMessage(WM_COMMAND,ID_FILE_SAVE);
//	pDoc->SaveModified();
	MainFramePtr->ScanDialog.m_List.DeleteAllItems();
	MainFramePtr->PostMessage(WM_COMMAND,ID_FILE_RELOADBASES);
	
}


void CAvpEditView::OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMListView->uNewState & LVIS_FOCUSED)
	{
		CAvpEditDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		MainFramePtr->SetMessageText(pDoc->MakeFullName(pNMListView->iItem));
//		MainFramePtr->UpdateWindow();
	}
	
	*pResult = 0;
}


void CAvpEditView::OnEditFind() 
{
	while(IDOK==FindDlg.DoModal()){
		if(FindDlg.m_String.GetLength()){
			OnEditFindagain();
			return;
		}
	}
}

void CAvpEditView::OnEditFindagain() 
{
	if(!FindDlg.m_String.GetLength()) {
		OnEditFind();
		return;
	}
	BeginWaitCursor();

	CListCtrl& ctlList = (CListCtrl&) GetListCtrl();
	int i=ctlList.GetNextItem(-1,LVNI_FOCUSED);
	if(i == -1) i=0;
	CString s;
	CString f=FindDlg.m_String;
	if(!FindDlg.m_Case)f.MakeLower();
/*
	CAvpEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CRecordEdit* rep;
*/
	while(-1 != (i=ctlList.GetNextItem(i,LVNI_ALL)))
	{
//		rep=(CRecordEdit*)(pDoc->EditArray[i]);
		for(int j=0;j<=7;j++){
			ctlList.GetItemText(i,j,s.GetBuffer(0x200),0x200);
			s.ReleaseBuffer();
/*
			switch(j)
			{
			case 0: 	s=rep->GetName();				break;
			case 1:		s=rep->IsModified()?"*":"";		break;
			case 2:		s=rep->GetTypeString();			break;
			case 3:		s=rep->GetSubTypeString();		break;
			case 4:		s=rep->GetMethodString();		break;
			case 5:		s=rep->GetLink16String();		break;
			case 6:		s=rep->GetLink32String();		break;
			case 7:		s=rep->GetComment();			break;
			}
*/
			if(s.GetLength()){
				if(!FindDlg.m_Case)s.MakeLower();
				if(-1!=s.Find(f)){
					while(-1 != (j=ctlList.GetNextItem(-1,LVNI_FOCUSED)))
							ctlList.SetItemState(j,0,LVIS_FOCUSED);
					while(-1 != (j=ctlList.GetNextItem(-1,LVNI_SELECTED)))
							ctlList.SetItemState(j,0,LVIS_SELECTED);
							
					ctlList.SetItemState(i,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
					ctlList.EnsureVisible(i,FALSE);
					goto ret;
				}
			}
		}
	}
ret:
	EndWaitCursor();
	return;
}
/*
void CAvpEditView::OnFileSave() 
{
	// TODO: Add your command handler code here
	
}
*/

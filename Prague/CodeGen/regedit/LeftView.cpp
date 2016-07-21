//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegEdit.h"

#include "RegEditDoc.h"
#include "LeftView.h"
#include "TreeImages.h"
#include "MainFrm.h"
#include <pr_registry.h>

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)
													      
BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnExpand )
	ON_UPDATE_COMMAND_UI(ID_RENAME_ITEM, OnUpdateRenameItem)
	ON_COMMAND(ID_RENAME_ITEM, OnRenameItem)
	ON_UPDATE_COMMAND_UI(ID_DELETE_ITEM, OnUpdateDeleteItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_UPDATE_COMMAND_UI(ID_NEW_KEY_ITEM, OnUpdateNewKey)
	ON_COMMAND(ID_NEW_KEY_ITEM, OnNewKey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CLeftView construction/destruction

CLeftView::CLeftView()
{
	m_nCreateCount = 0;
}

CLeftView::~CLeftView()
{
}

// ---
cRegistry* CLeftView::reg() { return GetDocument()->m_pReg; }

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_EDITLABELS | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	return CTreeView::PreCreateWindow(cs);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CLeftView drawing
void CLeftView::OnDraw(CDC* pDC)
{
	CRegEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CLeftView::OnInitialUpdate()
{
	//-----------------------------------------------------//
	CTreeView::OnInitialUpdate();

	GetTreeCtrl().DeleteAllItems();

	CRegEditDoc* pDoc = GetDocument();
	CTreeCtrlEx& ctlTree = (CTreeCtrlEx&) GetTreeCtrl();

	if(!m_lstImage)
	{
		m_lstImage.Create(IDB_TREE_IMAGES,16,0,RGB(255,0,255));
		m_lstImage.SetBkColor(GetSysColor(COLOR_WINDOW));
		ctlTree.SetImageList(&m_lstImage);
	}

	m_tRoot	= ctlTree.GetRootItem();
	m_tCurrent = m_tPrevious = m_tRoot;

	//-----------------------------------------------------//
	cRegistry* pReg = reg(); //GetDocument()->m_pReg;
	if(NULL == pReg)
	{
		GetDocument()->UpdateAllViews(this, UPD_HINT_SHOW_THE_KEY, NULL);
		UpdateStatus();	
		return;
	}


	UINT nImgIdx = GetDocument()->m_bWinReg ? TREE_REG_WIN : TREE_REG_DTREE;
	//CString strAbbreavatedRoot = GetDocument()->m_strRegKey;
	//GlobalGetApp()->AppAbbreviateName(strAbbreavatedRoot.GetBufferSetLength(1024), 1024, TRUE);
	//strAbbreavatedRoot.ReleaseBuffer();

	m_tRoot = m_tRoot.AddTail("\\"/*strAbbreavatedRoot*/, nImgIdx);

	//tRegKey oRootKey = pReg->get_pgROOT_KEY();
	//m_tRoot.SetData((DWORD)oRootKey);

	m_tCurrent = m_tPrevious = m_tRoot;

	PopulateSubTree( m_tRoot );
	m_tRoot.Expand();
	m_tRoot.Select();
}
				
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::RegGetKeyCount(tDWORD& o_dwKeyCount, tRegKey i_oKey)
{
	cRegistry* pReg = GetDocument()->m_pReg;
							
	tERROR nErr = pReg->GetKeyCount(&o_dwKeyCount, i_oKey);
	if( PR_FAIL(nErr) ) {
		GlobalGetApp()->ReportPragueError(_T("Cannot get key count."), nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::RegOpenKeyByIndex	(tRegKey i_oParentKey, tRegKey& o_oKey, tDWORD i_dwIndex)
{
	cRegistry* pReg = GetDocument()->m_pReg;
							
	tERROR nErr = pReg->OpenKeyByIndex(&o_oKey, i_oParentKey, i_dwIndex, 0);
	if(PR_FAIL(nErr))
	{
		char buff[20];
		CString msg;
		_itoa( i_dwIndex, buff, 10 );
		pReg->GetKeyName( 0, i_oParentKey, msg.GetBufferSetLength(1024), 1024, cTRUE );
		msg.ReleaseBuffer();
		msg += _T(" - cannot open key by index(");
		msg += buff;
		msg += _T(")");
		GlobalGetApp()->ReportPragueError(msg, nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::RegGetKeyNameByIndex(tRegKey i_oKey, tDWORD i_dwIndex, CString& o_strKeyName)
{
	cRegistry* pReg = GetDocument()->m_pReg;
					
	tDWORD dwRet;
	tERROR nErr = pReg->GetKeyNameByIndex(&dwRet, i_oKey, i_dwIndex, o_strKeyName.GetBufferSetLength(1024), 1024, 0);
	o_strKeyName.ReleaseBuffer();

	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot get key name by index."), nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::RegCloseKey(tRegKey i_oKey)
{
	cRegistry* pReg = GetDocument()->m_pReg;
							
	tERROR nErr = pReg->CloseKey(i_oKey);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot close the key."), nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::RegDeleteKey(tRegKey i_oKey, LPCTSTR i_strSubKey)
{
	cRegistry* pReg = GetDocument()->m_pReg;
							
	tERROR nErr = pReg->DeleteKey(i_oKey, i_strSubKey);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot delete the key."), nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::createSubKey( tRegKey i_oParentKey, tRegKey* sub_key, LPCTSTR i_strNewKeyName, bool create_def_val )
{
	cRegistry* pReg = GetDocument()->m_pReg;
							
	tRegKey oKey = 0;
	tERROR nErr = pReg->OpenKey(&oKey, i_oParentKey, i_strNewKeyName, cTRUE);
	if ( PR_SUCC(nErr) ) {
		if ( create_def_val )
			pReg->SetValue( oKey, "", tid_STRING, "", 1, cTRUE );
		if ( sub_key )
			*sub_key = oKey;
		else
			pReg->CloseKey( oKey );
	}
	else {
		CString msg( _T("Cannot create new key.\n\t") );
		msg += i_strNewKeyName;
		GlobalGetApp()->ReportPragueError( msg, nErr );
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::RegCopyKey(tRegKey i_oFrom, tRegKey o_oTo)
{
	cRegistry* pReg = GetDocument()->m_pReg;
							
	tERROR nErr = pReg->CopyKey(i_oFrom, (hREGISTRY)*pReg, o_oTo);
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot copy key."), nErr);
		return FALSE;;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CTreeCursor CLeftView::FindFirst( pfnFindFirst i_pfn, void* i_pData, CTreeCursor i_tStartFrom, BOOL i_bProcessParent ) {
	if( i_bProcessParent && i_pfn(i_tStartFrom,i_pData) )
		return i_tStartFrom;

	CTreeCursor oCur = i_tStartFrom.GetChild();
	while( oCur ) {
		if ( i_pfn(oCur,i_pData) )
			return oCur;
		oCur = oCur.GetNextSibling();
	}

	return CTreeCursor();
}


// ---
bool CLeftView::GetPath( CTreeCursor& node, CString& path ) {
	CTreeCursor dad = node.GetParent();
	if ( dad ) {
		if ( !GetPath(dad,path) )
			return false;
		CString sect = node.GetText();
		if ( sect.GetLength() ) {
			if ( path.GetLength() )
				path += "\\";
			path += node.GetText();
		}
	}
	else
		path.Empty();
	return true;
}


// ---
tRegKey CLeftView::openKey( CTreeCursor& node ) {
	CString path;
	tRegKey key = cRegNothing;
	if ( GetPath(node,path) ) {
		if ( path.IsEmpty() )
			key = cRegRoot;
		else {
			cRegistry* pReg = GetDocument()->m_pReg;
			const char* key_path = path;
			pReg->OpenKey( &key, cRegRoot, key_path, cFALSE );
		}
	}
	return key;
}



//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::PopulateSubTree( CTreeCursor& i_oTreeCur ) {
	//CTreeCursor oCurSave = i_oTreeCur;

	//CTreeCursor dad = i_oTreeCur.GetParent();
	//if ( !dad )
	//	return TRUE;

	CTreeCursor child = i_oTreeCur.GetChild();
	if ( child ) {
		CString text = child.GetText();
		if ( !text.IsEmpty() )
			return TRUE;
		child.Delete();
	}

	cRegistry*  pReg( reg() );
	cAutoRegKey oCurKey( pReg, openKey(i_oTreeCur) ); //(tRegKey)i_oTreeCur.GetData();
	tDWORD      dwKeyCount( oCurKey.get_key_count() );

	//if( !RegGetKeyCount(dwKeyCount,oCurKey) )
	//	return FALSE;

	for( tDWORD i=0; i<dwKeyCount; i++ ) {
		//cAutoRegKey oKey( oCurKey, i );
		//if( !oKey.opened() )
		//	break;
		CString strKeyName;
		cERROR err = oCurKey.get_key_name_by_index( i, strKeyName.GetBufferSetLength(1024), 1024 );
		strKeyName.ReleaseBuffer();

		if ( PR_FAIL(err) )
			return FALSE;
		//if( !RegGetKeyNameByIndex(oCurKey,i,strKeyName) )
		//	return FALSE;
		CTreeCursor oCurAdded = i_oTreeCur.AddTail( strKeyName, TREE_FOLDER_CLOSED );
		//oCurAdded.SetData( (DWORD)oKey );

		cAutoRegKey sub;
		oCurKey.open_by_index( sub, i );
		if ( sub.get_key_count() )
			oCurAdded.AddTail( "", TREE_FOLDER_CLOSED );
	}

	//i_oTreeCur = oCurSave;
	//i_oTreeCur = i_oTreeCur.GetChild();
	//while( i_oTreeCur ) {
	//	if(!PopulateSubTree(i_oTreeCur))
	//		return FALSE;
	//	i_oTreeCur = i_oTreeCur.GetNextSibling();
	//}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CLeftView::UpdateStatus()
{
	LPCTSTR cstrDiv = _T("\\");

	CString strRegKey = m_tCurrent.GetText();
	CTreeCursor tParent = m_tCurrent.GetParent();
	while (tParent)
	{
		strRegKey.Insert(0, cstrDiv);
		strRegKey.Insert(0, tParent.GetText());
		tParent = tParent.GetParent();
	}

	GlobalGetMainWnd()->SetStatusIdleMsg(strRegKey);
}
		 
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::IsCurrentRoot()
{
	return !m_tCurrent.GetParent();
}

BOOL CLeftView::IsCurrentEditable()
{
	return (FALSE == GetDocument()->m_bReadOnly) && !IsCurrentRoot();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CLeftView::AppendNewItem() {
	//tRegKey oNewKey;
	cAutoRegKey oParentKey( reg(), openKey(m_tCurrent) ); // = (tRegKey)m_tCurrent.GetData();

	CString strNewKeyName;
	strNewKeyName.Format( _T("Key%d"), m_nCreateCount );

	if( !createSubKey(oParentKey,0,strNewKeyName,true) )
		return;

	++m_nCreateCount;

	CTreeCursor oCurAdded = m_tCurrent.AddTail( strNewKeyName, TREE_FOLDER_CLOSED );
	//oCurAdded.SetData( (DWORD)oNewKey );

	m_tCurrent.Expand();
	oCurAdded.Select();

	GetDocument()->SetModifiedFlag();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//BOOL pKeyCleaner(CTreeCursor i_tCur, void* i_pData)
//{
//	CLeftView* pLeftView = (CLeftView*)i_pData;
//	cAutoRegKey oKey( pLeftView->reg(), pLeftView->openKey(i_tCur) ); // = (tRegKey)i_tCur.GetData();
//	if(!pLeftView->RegCloseKey(oKey))
//		return TRUE;
//	//i_tCur.SetData(0);
//	return FALSE;
//}	


// ---
inline tDWORD GetItemIndex( CTreeCursor i_tCur ) {
	CTreeCursor tCur = i_tCur.GetParent();
	tCur = tCur.GetChild();
	tDWORD dwRetVal = 0;
	while(tCur != i_tCur)
	{
		++dwRetVal;
		tCur = tCur.GetNextSibling();
	}
	return dwRetVal;
}



//BOOL pKeyIndexRecoverer( CTreeCursor i_tCur, void* i_pData ) {
//	CLeftView*  pLeftView = (CLeftView*)i_pData;
//	cAutoRegKey oKeyParent( pLeftView->reg(), pLeftView->openKey(i_tCur.GetParent()) ); // = (tRegKey)i_tCur.GetParent().GetData();
//	tDWORD      dwItemIndex = GetItemIndex(i_tCur);
//
//	tRegKey oKey;
//	if( !pLeftView->RegOpenKeyByIndex(oKeyParent, oKey, dwItemIndex))
//		return TRUE;
//
//	i_tCur.SetData( (DWORD)oKey );
//
//	return FALSE;
//}	



//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CLeftView::RenameItem(LPCTSTR i_strNewName)
{
	cAutoRegKey oParentKey( reg(), openKey(m_tCurrent.GetParent()) ); // = (tRegKey)m_tCurrent.GetParent().GetData();
	cAutoRegKey oKey      ( reg(), openKey(m_tCurrent) ); // = (tRegKey)m_tCurrent.GetData();
	CString strKeyName    ( m_tCurrent.GetText() );
	tRegKey oNewKey;

	if( !createSubKey(oParentKey,&oNewKey,i_strNewName,false) )
		return FALSE;
	if( !RegCopyKey(oKey, oNewKey) )
		return FALSE;
	if( !RegDeleteKey(oParentKey,strKeyName) )
		return FALSE;
	RegCloseKey( oNewKey );
	GetDocument()->SetModifiedFlag();

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CLeftView::DeleteItem()
{
	if(!m_tCurrent)
		return;

	cAutoRegKey oParentKey( reg(), openKey(m_tCurrent.GetParent()) ); // = (tRegKey)m_tCurrent.GetParent().GetData();
	CString strKeyName = m_tCurrent.GetText();

	//if(FindFirst(pKeyCleaner, (void*)this, m_tCurrent))
	//	return;

	if( !RegDeleteKey(oParentKey,strKeyName) )
		return;

	m_tCurrent.Delete();
	GetDocument()->SetModifiedFlag();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CLeftView message handlers
void CLeftView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;

	CTreeCursor tPrevCur = m_tPrevious;
	m_tPrevious = m_tCurrent; 
	CTreeCtrlEx& ctlTree = (CTreeCtrlEx&) GetTreeCtrl();
	m_tCurrent = ctlTree.GetSelectedItem();

	if(m_tPrevious && m_tPrevious != m_tRoot)
		m_tPrevious.SetImage(TREE_FOLDER_CLOSED, TREE_FOLDER_CLOSED);

	if(m_tCurrent && m_tCurrent != m_tRoot)
		m_tCurrent.SetImage(TREE_FOLDER_OPENED, TREE_FOLDER_OPENED);

	//DWORD dwData = m_tCurrent.GetData();
	cAutoRegKey curr( reg(), openKey(m_tCurrent) );
	tRegKey key = curr;
	GetDocument()->UpdateAllViews(this, UPD_HINT_SHOW_THE_KEY, (CObject*)key/*dwData*/);

	UpdateStatus();	
}

// --- 
void CLeftView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);

	if(nChar == VK_F2)
		SendMessage(WM_COMMAND, ID_RENAME_ITEM);
	else
	if(nChar == VK_DELETE)
		SendMessage(WM_COMMAND, ID_DELETE_ITEM);
	else
	if(nChar == VK_INSERT)
		SendMessage(WM_COMMAND, ID_NEW_KEY_ITEM);
}

// --- 
void CLeftView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(!IsCurrentEditable())
		*pResult = 1;
}

// --- 
void CLeftView::OnExpand( NMHDR* pNMHDR, LRESULT* pResult ) {
	*pResult = 0;
	NMTREEVIEW* info = (NMTREEVIEW*)pNMHDR;
	if ( info->action == 2 )
		PopulateSubTree( CTreeCursor(info->itemNew.hItem,&(CTreeCtrlEx&)GetTreeCtrl()) );
}

// --- 
void CLeftView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pDispInfo = (TV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(!pDispInfo->item.pszText)
		return;

	CString strNewName = pDispInfo->item.pszText;

	// check if new value is empty
	if(strNewName.IsEmpty())
	{
		AfxMessageBox(_T("You could not set empty key name."));
		return;
	}

	// check if this value wasn't changed
	CString strValueName = m_tCurrent.GetText();

	if(strNewName != strValueName)
		if(!RenameItem(strNewName))
			m_tCurrent.SetText(strValueName);
}
      
// ---
void CLeftView::OnUpdateRenameItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsCurrentEditable());
}

void CLeftView::OnRenameItem() 
{
	GetTreeCtrl().EditLabel(m_tCurrent);
}

// ---
void CLeftView::OnUpdateDeleteItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsCurrentEditable());
}

void CLeftView::OnDeleteItem() 
{
	DeleteItem();
}

// ---
void CLeftView::OnUpdateNewKey(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE == GetDocument()->m_bReadOnly);
}

void CLeftView::OnNewKey() 
{
	AppendNewItem();
	OnRenameItem();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CRegEditDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRegEditDoc)));
	return (CRegEditDoc*)m_pDocument;
}
#endif //_DEBUG
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//




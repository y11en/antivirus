// VEControlTree.cpp : implementation file
//

#include "stdafx.h"
#include "../avpcontrols/WASCRes.rh"

#include "Resource.h"
#include "VEControlTree.h"
#include "VisualEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// ---
CVEControlTreeItem::CVEControlTreeItem() :
	CControlTreeItem()
{
}

// ---
CVEControlTreeItem::~CVEControlTreeItem() {
}

// ---
int CVEControlTreeItem::GetImageIndex() 
{
	return int(IsEnabled() ? ii_EnabledImageIndex : ii_DisabledImageIndex ) + (CControlTreeItem::ii_LastIndex); 
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CVEControlTreeBase

CVEControlTreeBase::CVEControlTreeBase() :
CControlTree()
{
	m_bTechnicalEditingMode = FALSE;
}

// ---
CVEControlTreeBase::~CVEControlTreeBase() {
}

// intercept setting array and in technical editing mode allow edit only comments nodes
void	CVEControlTreeBase::SetItemsArray( CCTItemsArray *itemsArray ){

	if(!itemsArray)
		return;

	if(!m_bTechnicalEditingMode){
		CControlTree::SetItemsArray(itemsArray);
		ShowRoot();
		return;			
	}

	for (int nIndex = 0; nIndex < itemsArray->GetSize(); nIndex++ ) {

		CControlTreeItem *item = (CControlTreeItem*) (*itemsArray)[nIndex];

		if(!item)
			continue;

		// remove new items nodes
		if(IsNewNodeItem(item)){
			if(CPPD(item))
				delete 	CPPD(item);
			itemsArray->RemoveAt(nIndex);
			--nIndex;
			continue;
		}

		// leave comments nodes as is
		if(IsCommentItem(item))
			continue;

		if(!item->IsCanBeChecked())
			item->SetSimpleStringAlias();

		item->SetCanBeRemoved( false );
	}

	CControlTree::SetItemsArray(itemsArray);
	ShowRoot();
}					

void CVEControlTreeBase::ShowRoot()
{
	HTREEITEM hNode = NodeByIndex( 0 );
	if ( hNode ) 
		PostMessage(TVM_ENSUREVISIBLE, 0, (LPARAM)hNode); 
}

// determine weahter the item is comment item
BOOL	CVEControlTreeBase::IsCommentItem(CControlTreeItem *i_pItem){

	void* pData = CPPD(i_pItem);
	if (!i_pItem || !CPPD(i_pItem))
		return FALSE;

	HDATA hData = CPPD(i_pItem)->m_hData;
	HPROP hProp = CPPD(i_pItem)->m_hProp;
	
	if(NULL == hData || NULL == hProp)
		return FALSE;

	BOOL bRet = FALSE;
	
	AVP_dword nPID = ::PROP_Get_Id(hProp);

	bRet = 
		nPID == VE_PID_SHORTCOMMENT ||
		nPID == VE_PID_LARGECOMMENT ||
		nPID == VE_PID_BEHAVIORCOMMENT ||
		nPID == VE_PID_VALUECOMMENT;

	return bRet;
}

// return true if node label is matched with predefined labels
BOOL	CVEControlTreeBase::IsNewNodeItem(CControlTreeItem *i_Item){

	BOOL bRet = FALSE;

	CString& strText = i_Item->GetText();

	int arrResIds[] = {
		IDS_NEWINTERFACE,
		IDS_NEWPROPERTY,
		IDS_NEWMETHOD,
		IDS_NEWMETHODPARAMETER,
		IDS_NEWDATASRUCTURE,
		IDS_NEWDATASRUCTUREMEMBER,
		IDS_NEWTYPE,
		IDS_NEWCONSTANT,
		IDS_NEWERRORCODE,
		IDS_NEW_MESSAGE_CLASS,
		IDS_NEW_MESSAGE,
		IDS_SM_NEWOBJECT,
		IDS_WAS_PP_NEWVALUE
	};

	int arrSize = sizeof(arrResIds) / sizeof(arrResIds[0]);

	for(int i=0; i<arrSize; i++){
		CString strLabel;
		strLabel.LoadString(arrResIds[i]);
		if(strLabel == strText){
			bRet = TRUE;
			break;
		}

	}

	return bRet;
}


#define CTreeCtrl CControlTree
BEGIN_MESSAGE_MAP(CVEControlTreeBase, CTreeCtrl)
#undef CTreeCtrl
//{{AFX_MSG_MAP(CVEControlTree)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CVEControlTree

CVEControlTree::CVEControlTree() :
	CVEControlTreeBase()
{
}

// ---
CVEControlTree::~CVEControlTree() {
}

// ---
void CVEControlTree::CreateImageList() {
	CControlTree::CreateImageList();
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_INTERFACEENABLEDICON) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_INTERFACEDISABLEDICON) );
}

BEGIN_MESSAGE_MAP(CVEControlTree, CVEControlTreeBase)
	//{{AFX_MSG_MAP(CVEControlTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// DAddNewFilter.cpp : implementation file
//

#include "stdafx.h"
#include "avpgcfg.h"
#include "DAddNewFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "FilterTreeInfo.h"
/////////////////////////////////////////////////////////////////////////////
// CDAddNewFilter dialog


CDAddNewFilter::CDAddNewFilter(CWnd* pParent /*=NULL*/)
	: CDTreeBase(&m_TreeCtrl, CDAddNewFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDAddNewFilter)
	m_FilterPosition = -1;
	//}}AFX_DATA_INIT
	m_pFilter = (PFILTER_TRANSMIT) m_pFilterBuffer;
	ZeroMemory(m_pFilter, FILTER_BUFFER_SIZE);
	m_FilterPosition = 0;
}


void CDAddNewFilter::DoDataExchange(CDataExchange* pDX)
{
	CDTreeBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDAddNewFilter)
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
	DDX_Radio(pDX, IDC_RADIO_POSITION, m_FilterPosition);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDAddNewFilter, CDTreeBase)
	//{{AFX_MSG_MAP(CDAddNewFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDAddNewFilter message handlers

BOOL CDAddNewFilter::OnInitDialog() 
{
	CDTreeBase::OnInitDialog();
	
	// TODO: Add extra initialization here
	CCTItemsArray &newItems = *new CCTItemsArray(true);
	CControlTreeItem *item;

	item = new CControlTreeItem;
	item->SetText(g_pLocalize->GetLocalizeString(IDS_TREEROOT));
	item->SetLevel(1);
	item->SetSimpleStringAlias();
	item->SetCanBeRemoved(false);
	item->SetExpanded(true);
	item->SetData(0, false);
	newItems.Add(item);

	ZeroMemory(m_pFilter, FILTER_BUFFER_SIZE);

	strcpy(m_pFilter->m_ProcName, "*");

	char pBuff[FILTER_BUFFER_SIZE];
	PFILTER_PARAM pNewParam = (PFILTER_PARAM) pBuff;
	pNewParam->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pNewParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	pNewParam->m_ParamID = _PARAM_OBJECT_URL;
	pNewParam->m_ParamSize = lstrlen("*") + 1;
	lstrcpy((char *) pNewParam->m_ParamValue, "*");

	ChangeFilterParamValue(m_pFilter, pNewParam);


	CString strtmp;
	CHAR* pch;
	DWORD Idx = 0;
	CStringList StringList;
	FillFuncList(&StringList, m_pFilter->m_HookID, m_pFilter->m_FunctionMj, m_pFilter->m_FunctionMi, NULL, NULL);
	POSITION pos = StringList.GetHeadPosition();
	if (pos != NULL)
	{
		strtmp = StringList.GetAt(pos);
		pch = (LPSTR)(LPCSTR)strtmp;
		FillFuncList(NULL, m_pFilter->m_HookID, m_pFilter->m_FunctionMj, m_pFilter->m_FunctionMi, pch, &Idx);
		m_pFilter->m_HookID = Idx;
	}
	
	AddFilterItem(&newItems, m_pFilter);

	int Count = newItems.GetSize();
	CCheckTreeItem* pItem;
	for (int cou = 0; cou < Count; cou++)
	{
		pItem =  newItems[cou];
		if (pItem != NULL)
			pItem->SetExpanded();
	}

	m_TreeCtrl.SetItemsArray(&newItems);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

PFILTER_TRANSMIT CDAddNewFilter::GetFilter()
{
	switch (m_FilterPosition)
	{
	case 0:
		m_pFilter->m_FltCtl = FLTCTL_ADD2TOP;
		break;
	case 1:
		m_pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
		break;
	case 2:
		m_pFilter->m_FltCtl = FLTCTL_ADD;
		break;
	}
	return m_pFilter;
}

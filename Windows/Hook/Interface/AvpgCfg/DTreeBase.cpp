// DTreeBase.cpp : implementation file
//

#include "stdafx.h"
#include "avpgcfg.h"
#include "DTreeBase.h"

#include <Hook\IDriver.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// -----------------------------------------------------------------------------------------
void SetItemText(char** ptch, CString* pstr)
{
	delete []*ptch;
	*ptch = new char[pstr->GetLength() + 1];
	lstrcpyn(*ptch, *pstr, pstr->GetLength() + 1);
}

void DeleteNodesData( CTreeWnd &owner, HTREEITEM cur, int ind, void *par)
{
	CCTItemsArray &items = *((CCheckTree &)owner).GetItemsArray();
	CCheckTreeItem* pItem =  items[ind];
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		if (pFilterTreeInfo != NULL)
			delete pFilterTreeInfo;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDTreeBase dialog

CDTreeBase::CDTreeBase(CControlTree* pTreeCtrl, DWORD IDD, CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CDTreeBase)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pTreeCtrl = pTreeCtrl;
	m_pFilterTransmit = NULL;
	m_pFilterTransmitSave = (PFILTER_TRANSMIT) m_pFilterBuffer;
	ZeroMemory(m_pFilterTransmitSave, FILTER_BUFFER_SIZE);
}


void CDTreeBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDTreeBase)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDTreeBase, CDialog)
	//{{AFX_MSG_MAP(CDTreeBase)
	ON_MESSAGE(CTTM_WINDOWSHUTDOWN, OnWindowShutdown)
	ON_MESSAGE(CTTM_BEGINLABELEDIT, OnCTTBeginLabelEdit)
	ON_MESSAGE(CTTM_ENDLABELEDIT, OnCTTEndLabelEdit)
	ON_MESSAGE(CTTM_POSTLABELEDIT, OnCTTPostLabelEdit)
	ON_MESSAGE(CTTM_CANCELLABELEDIT, OnCTTCancelLabelEdit)
	ON_MESSAGE(CTTM_LOADCOMBOCONTENTS, OnCTTLoadComboContents)
	ON_MESSAGE(CTTM_LOADUPDOWNCONTEXT, OnCTTLoadUpDownContext)
	ON_MESSAGE(CTTM_DOEXTERNALEDITING, OnCTTDoExternalEditing)
	ON_MESSAGE(CTTM_CHECKEDCHANGING, OnCTTCheckChanging)
	ON_MESSAGE(CTTM_LOADIPADDRESS, OnCTTLoadIpAddress)
	ON_MESSAGE(CTTM_ENDADDRESSLABELEDIT, OnCTTEndIpEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDTreeBase message handlers

void CDTreeBase::AddFilterItem(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter)
{
	CString strDescr;
	CString strSubDescr;
	CControlTreeItem *item;
	bool bChecked;
	CString strtmp;
//	PCHAR pObjName;

	SYSTEMTIME ExpirST;
	FILETIME LocalT;
	CHAR Time[64];
	CHAR Date[64];

	item = new CControlTreeItem;

// -----------------------------------------------------------------------------------------
	strDescr = GetFilterDesciptions(pFilter);
// -----------------------------------------------------------------------------------------

	/*pObjName = GetObjNameFromFilter(pFilter);
	if (pObjName == NULL)
		pObjName = "";*/

	item->SetText(strDescr);
	item->SetLevel(2);
	item->SetSimpleStringAlias();
	item->SetCanBeRemoved(true);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_DESCRIPTION), false);
	pItems->Add(item);

	item = new CControlTreeItem;
	strSubDescr.Format(g_pLocalize->GetLocalizeString(IDS_PROCESSFMT), pFilter->m_ProcName);
	item->SetText(strSubDescr);
	item->SetLevel(3);
	item->SetDropDownComboAlias();
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_PROCESSNAME), false);
	pItems->Add(item);

	if(pFilter->m_Expiration)
	{
		FileTimeToLocalFileTime((FILETIME*)&(pFilter->m_Expiration), &LocalT);
		FileTimeToSystemTime(&LocalT, &ExpirST);
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,&ExpirST,NULL,Date,sizeof(Date));
		GetTimeFormat(LOCALE_USER_DEFAULT,LOCALE_NOUSEROVERRIDE,&ExpirST,NULL,Time,sizeof(Time));
		strtmp.Format(g_pLocalize->GetLocalizeString(IDS_FILTEREXPIREDFMT),Date,Time);

		item = new CControlTreeItem;
		item->SetText(strtmp);
		item->SetLevel(3);
		item->SetSimpleStringAlias();
		item->SetCanBeRemoved(false);
		item->SetExpanded(false);
		item->SetData(new CFilterTreeInfo(pFilter, _FILTER_EXPIRATION), false);
		pItems->Add(item);
	}
	
	bool bPopup = pFilter->m_Flags & FLT_A_POPUP;

	item = new CControlTreeItem;
	item->SetText(g_pLocalize->GetLocalizeString(IDS_PERMIT));
	item->SetLevel(3);
	item->SetRadioAlias();
	if (bPopup == false)
		bChecked = (pFilter->m_Flags & FLT_A_DENY) == 0 ? true : false;
	else
		bChecked = false;
	item->Check(bChecked);
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_PERMIT), false);
	pItems->Add(item);

	item = new CControlTreeItem;
	item->SetText(g_pLocalize->GetLocalizeString(IDS_DENY));
	item->SetLevel(3);
	item->SetRadioAlias();
	if (bPopup == false)
		bChecked = (pFilter->m_Flags & FLT_A_DENY) != 0 ? true : false;
	else
		bChecked = false;
	item->Check(bChecked);
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_DENY), false);
	pItems->Add(item);
	
	item = new CControlTreeItem;
	item->SetText(g_pLocalize->GetLocalizeString(IDS_PROMPT));
	item->SetLevel(3);
	item->SetRadioAlias();
	item->Check(bPopup);
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_PROMPT), false);
	pItems->Add(item);

	item = new CControlTreeItem;
	item->SetText(g_pLocalize->GetLocalizeString(IDS_DEFAULTPERMIT));
	item->SetLevel(4);
	item->SetRadioAlias();
	bChecked = (pFilter->m_Flags & FLT_A_DENY) == 0 ? true : false;
	item->Check(bChecked);
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_PROMPT_DEFALLOW), false);
	pItems->Add(item);

	item = new CControlTreeItem;
	item->SetText(g_pLocalize->GetLocalizeString(IDS_DEFAULTDENY));
	item->SetLevel(4);
	item->SetRadioAlias();
	bChecked = (pFilter->m_Flags & FLT_A_DENY) != 0 ? true : false;
	item->Check(bChecked);
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_PROMPT_DEFDENY), false);
	pItems->Add(item);


	item = new CControlTreeItem;
	item->SetText(GetRequestTimeoutString(pFilter));
	item->SetLevel(4);
	item->SetUpDownEditAlias();
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_TIMEOUT), false);
	pItems->Add(item);
	
	item = new CControlTreeItem;
	item->SetText(g_pLocalize->GetLocalizeString(IDS_LOG));
	item->SetLevel(3);
	item->SetCheckAlias();
	bChecked = (pFilter->m_Flags & FLT_A_LOG) != 0 ? true : false ;
	item->Check(bChecked);
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_LOG), false);
	pItems->Add(item);
	
	item = new CControlTreeItem;
	item->SetText(FillFuncList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL));
	item->SetLevel(3);
	item->SetDropDownListComboAlias();
	item->SetCanBeRemoved(false);
	item->SetExpanded(false);
	item->SetData(new CFilterTreeInfo(pFilter, _FILTER_FUNCTION), false);
	pItems->Add(item);
	
	AddMajFunc(pItems, pFilter);
}

void CDTreeBase::AddMajFunc(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter, bool bExpand)
{
	CHAR* MajFunc = FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
	if (MajFunc != NULL)
	{
		CControlTreeItem *item;

		item = new CControlTreeItem;
		item->SetText(FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL));
		item->SetLevel(4);
		item->SetDropDownListComboAlias();
		item->SetCanBeRemoved(false);
		item->SetExpanded(bExpand);
		item->SetData(new CFilterTreeInfo(pFilter, _FILTER_MAJ), false);
		pItems->Add(item);

		AddMinFunc(pItems, pFilter, bExpand);
	}
	AddParam(pItems, pFilter, bExpand);
}

void CDTreeBase::AddMinFunc(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter, bool bExpand)
{
	CHAR* MajFunc = FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
	CControlTreeItem *item;
	int level = 5;
	if (MajFunc != NULL)
	{
		item = new CControlTreeItem;
		item->SetText(FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL));
		item->SetLevel(level++);
		item->SetDropDownListComboAlias();
		item->SetCanBeRemoved(false);
		item->SetExpanded(bExpand);
		item->SetData(new CFilterTreeInfo(pFilter, _FILTER_MIN), false);
		pItems->Add(item);
	}
}

void CDTreeBase::AddParam(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter, bool bExpand)
{
	CControlTreeItem *item;
	CString strSubDescr;
	
	switch (pFilter->m_HookID)
	{
	case FLTTYPE_NFIOR:
		//	case FLTTYPE_IFS:
	case FLTTYPE_FIOR:
		//	case FLTTYPE_I21:
	case FLTTYPE_R3:
	case FLTTYPE_REGS:
		{
			char pBuff[FILTER_BUFFER_SIZE];
			PFILTER_PARAM pNewParam = (PFILTER_PARAM) pBuff;
			pNewParam->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			pNewParam->m_ParamFlt = FLT_PARAM_WILDCARD;
			pNewParam->m_ParamID = _PARAM_OBJECT_URL;
			pNewParam->m_ParamSize = lstrlen("*") + 1;
			lstrcpy((char*)pNewParam->m_ParamValue, "*");
			
			ChangeFilterParamValue(pFilter, pNewParam);

			item = new CControlTreeItem;
			strSubDescr.Format(g_pLocalize->GetLocalizeString(IDS_OBJECTFMT), pNewParam->m_ParamValue);
			item->SetText(strSubDescr);
			item->SetLevel(4);
			item->SetSimpleEditAlias();
			item->SetCanBeRemoved(false);
			item->SetExpanded(false);
			item->SetData(new CFilterTreeInfo(pFilter, _FILTER_FILENAME), false);
			pItems->Add(item);
		}
		break;
	case FLTTYPE_PID:
		{
			PIPV4 pIp4;
			char pBuff[FILTER_BUFFER_SIZE];
			PFILTER_PARAM pNewParam = (PFILTER_PARAM) pBuff;
			pNewParam->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			pNewParam->m_ParamFlt = FLT_PARAM_EUA;
			pNewParam->m_ParamID = ID_IP_SRC_ADDRESS;
			pNewParam->m_ParamSize = sizeof(IPV4);
			pIp4 = (PIPV4) pNewParam->m_ParamValue;
			ZeroMemory(pIp4, sizeof(IPV4));
			
			ChangeFilterParamValue(pFilter, pNewParam);
			
			item = new CControlTreeItem;
			strSubDescr.Format(g_pLocalize->GetLocalizeString(IDS_IPFMT_SRC), pIp4->ip[0], pIp4->ip[1], pIp4->ip[2], pIp4->ip[3]);
			item->SetText(strSubDescr);
			item->SetLevel(4);
			item->SetIPAddressAlias();
			item->SetCanBeRemoved(false);
			item->SetExpanded(false);
			item->SetData(new CFilterTreeInfo(pFilter, _FILTER_IPSRC), false);
			pItems->Add(item);

			/*item = new CControlTreeItem;
			unsigned short Port = pIp4->port;
			strSubDescr.Format(g_pLocalize->GetLocalizeString(IDS_IPPORTFMT), Port);
			item->SetText(strSubDescr);
			item->SetLevel(4);
			item->SetSimpleEditAlias();
			item->SetCanBeRemoved(false);
			item->SetExpanded(false);
			item->SetData(new CFilterTreeInfo(pFilter, _FILTER_IPPORTSRC), false);
			pItems->Add(item);*/

			pNewParam->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			pNewParam->m_ParamFlt = FLT_PARAM_EUA;
			pNewParam->m_ParamID = ID_IP_DST_ADDRESS;
			pNewParam->m_ParamSize = sizeof(IPV4);
			pIp4 = (PIPV4) pNewParam->m_ParamValue;
			ZeroMemory(pIp4, sizeof(IPV4));
			
			ChangeFilterParamValue(pFilter, pNewParam);
			
			item = new CControlTreeItem;
			strSubDescr.Format(g_pLocalize->GetLocalizeString(IDS_IPFMT_DST), pIp4->ip[0], pIp4->ip[1], pIp4->ip[2], pIp4->ip[3]);
			item->SetText(strSubDescr);
			item->SetLevel(4);
			item->SetIPAddressAlias();
			item->SetCanBeRemoved(false);
			item->SetExpanded(false);
			item->SetData(new CFilterTreeInfo(pFilter, _FILTER_IPDST), false);
			pItems->Add(item);

			/*item = new CControlTreeItem;
			Port = pIp4->port;
			strSubDescr.Format(g_pLocalize->GetLocalizeString(IDS_IPPORTFMT), Port);
			item->SetText(strSubDescr);
			item->SetLevel(4);
			item->SetSimpleEditAlias();
			item->SetCanBeRemoved(false);
			item->SetExpanded(false);
			item->SetData(new CFilterTreeInfo(pFilter, _FILTER_IPPORTDST), false);
			pItems->Add(item);*/
	
		}
		break;
	}
}

LRESULT	CDTreeBase::OnWindowShutdown( WPARAM wParam, LPARAM lParam)
{
	RemoveAllTreeInfoItems();
	return 0;
}

void CDTreeBase::RemoveAllTreeInfoItems()
{
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	if (pItems == NULL)
		return;
	int Count = pItems->GetSize();
	CCheckTreeItem* pItem;
	CFilterTreeInfo* pFilterTreeInfo;
	for (int cou = 0; cou < Count; cou++)
	{
		pItem =  (*pItems)[cou];
		if (pItem != NULL)
		{
			pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
			if (pFilterTreeInfo != NULL)
				delete pFilterTreeInfo;
		}
	}
}

// -----------------------------------------------------------------------------------------

LRESULT CDTreeBase::OnCTTLoadComboContents( WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;

	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		PFILTER_TRANSMIT pFilter = pFilterTreeInfo->m_pFilter;
		*pbLoad = TRUE;
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_PROCESSNAME:
			{
				CEnumProcess Enum((CStringList*) info->m_Value.m_pStringList);
				Enum.EnumProcesses();
			}
			break;
		case _FILTER_FUNCTION:
			FillFuncList((CStringList*) info->m_Value.m_pStringList, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			break;
		case _FILTER_MAJ:
			FillMajList((CStringList*) info->m_Value.m_pStringList, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			break;
		case _FILTER_MIN:
			FillMinList((CStringList*) info->m_Value.m_pStringList, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			break;
		default:
			*pbLoad = FALSE;
			break;
		}
	}
	return 0;
}

LRESULT	CDTreeBase::OnCTTLoadUpDownContext( WPARAM wParam, LPARAM lParam)
{
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	
	// timeout
	info->m_Value.m_aUpDownStuff[0] = 0;	// Min
	info->m_Value.m_aUpDownStuff[1] = 9999;	// Max
	info->m_Value.m_aUpDownStuff[2] = 1;	// Step

	return 0;
}

LRESULT	CDTreeBase::OnCTTCheckChanging( WPARAM wParam, LPARAM lParam )
{
	ItemChanged();
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_PERMIT:
			pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags & ~FLT_A_DENY;
			pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags & ~FLT_A_POPUP;
			break;
		case _FILTER_DENY:
			pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_DENY;
			pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags & ~FLT_A_POPUP;
			break;
		case _FILTER_PROMPT:
			{
				pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_POPUP | FLT_A_DENY;
				CCheckTreeItem* pNextItem =  (*pItems)[info->m_nItemIndex + 1];
				if (pNextItem != NULL)
				{
					CFilterTreeInfo* pFilterTreeInfoNext = (CFilterTreeInfo*)pNextItem->GetData();
					if (pFilterTreeInfoNext != NULL)
					{
						pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags & ~FLT_A_DENY;
						if (pFilterTreeInfoNext->m_nProperty == _FILTER_PROMPT_DEFALLOW)
						{
							if (pNextItem->IsChecked())
								pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_POPUP;
							else
								pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_POPUP | FLT_A_DENY;
						}
						if (pFilterTreeInfoNext->m_nProperty == _FILTER_PROMPT_DEFDENY)
						{						
							if (!pNextItem->IsChecked())
								pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_POPUP;
							else
								pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_POPUP | FLT_A_DENY;
						}
					}
				}
					
			}
			break;
		case _FILTER_PROMPT_DEFDENY:
			pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_POPUP | FLT_A_DENY;
			break;
		case _FILTER_PROMPT_DEFALLOW:
			pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_POPUP;
			pFilterTreeInfo->m_pFilter->m_Flags = pFilterTreeInfo->m_pFilter->m_Flags & ~FLT_A_DENY;
			break;
		case _FILTER_LOG:
			pFilterTreeInfo->m_pFilter->m_Flags = info->m_Value.m_bSetRemove == TRUE ? pFilterTreeInfo->m_pFilter->m_Flags | FLT_A_LOG : pFilterTreeInfo->m_pFilter->m_Flags & ~FLT_A_LOG;
			break;
		default:
			break;
		}
		UpdateFilterDescribe(info->m_nItemIndex, pFilterTreeInfo);
	}
	return 0;
}

LRESULT CDTreeBase::OnCTTBeginLabelEdit( WPARAM wParam, LPARAM lParam)
{
	CString strtmp;
	PCHAR pObjName;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	//PCHAR pObjName;
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_FILENAME:
			pObjName = GetObjNameFromFilter(pFilterTreeInfo->m_pFilter);
			if (pObjName == NULL)
				pObjName = "";
			strtmp = pObjName;
			SetItemText(&info->m_Value.m_pItemText, &strtmp);
			break;
		case _FILTER_PROCESSNAME:
			strtmp = pFilterTreeInfo->m_pFilter->m_ProcName;
			SetItemText(&info->m_Value.m_pItemText, &strtmp);
			break;
		case _FILTER_TIMEOUT:
			strtmp.Format("%u", pFilterTreeInfo->m_pFilter->m_Timeout);
			SetItemText(&info->m_Value.m_pItemText, &strtmp);
			break;
		/*case _FILTER_IPPORTSRC:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, _PARAM_OBJECT_TR_IPV4_SRC);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					unsigned short Port = pIp4->port;
					strtmp.Format("%u", Port);
					SetItemText(&info->m_Value.m_pItemText, &strtmp);
				}
			}
			break;
		case _FILTER_IPPORTDST:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, _PARAM_OBJECT_TR_IPV4_DEST);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					unsigned short Port = pIp4->port;
					strtmp.Format("%u", Port);
					SetItemText(&info->m_Value.m_pItemText, &strtmp);
				}
			}
			break;*/
		default:
			break;
		}
	}
	return 0;
}

LRESULT	CDTreeBase::OnCTTDoExternalEditing( WPARAM wParam, LPARAM lParam)
{
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	
	return 0;
}

LRESULT CDTreeBase::OnCTTEndLabelEdit( WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbResult = (LPBOOL) wParam;
	CString strtmp;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_FILENAME:
			if (lstrlen(info->m_Value.m_pItemText) > MAXPATHLEN)
			{
				strtmp.Format(g_pLocalize->GetLocalizeString(IDS_STRINGLENGTHFMT), MAXPATHLEN);
				MessageBox(strtmp, g_pLocalize->GetLocalizeString(IDS_ERRORMSG), MB_ICONINFORMATION);
				*pbResult = FALSE;
			}
			break;
		case _FILTER_PROCESSNAME:
			if (lstrlen(info->m_Value.m_pItemText) > PROCNAMELEN)
			{
				strtmp.Format(g_pLocalize->GetLocalizeString(IDS_STRINGLENGTHFMT), PROCNAMELEN);
				MessageBox(strtmp, g_pLocalize->GetLocalizeString(IDS_ERRORMSG), MB_ICONINFORMATION);
				*pbResult = FALSE;
			}
			break;
		case _FILTER_EXPIRATION:
			break;
		case _FILTER_DENY:
			break;
		case _FILTER_PROMPT:
			break;
		case _FILTER_TIMEOUT:
			{
				long TimeOut;
				strtmp = info->m_Value.m_pItemText; // Checked falue
				if (String2Long(&strtmp, &TimeOut) == false)
				{
					MessageBox(g_pLocalize->GetLocalizeString(IDS_EXPECTEDNUMBER), g_pLocalize->GetLocalizeString(IDS_ERRORMSG), MB_ICONINFORMATION);
					*pbResult = FALSE;
				}
				else if ((TimeOut < 0) || (TimeOut > 9999))
				{
					MessageBox(g_pLocalize->GetLocalizeString(IDS_TIMEOUTRANGE), g_pLocalize->GetLocalizeString(IDS_ERRORMSG), MB_ICONINFORMATION);
					*pbResult = FALSE;
				}
			}
			break;
		case _FILTER_IPPORTSRC:
		case _FILTER_IPPORTDST:
			{
				long Port;
				strtmp = info->m_Value.m_pItemText; // Checked falue
				if (String2Long(&strtmp, &Port) == false)
				{
					MessageBox(g_pLocalize->GetLocalizeString(IDS_EXPECTEDNUMBER), g_pLocalize->GetLocalizeString(IDS_ERRORMSG), MB_ICONINFORMATION);
					*pbResult = FALSE;
				}
				else if (Port > 0xffff)
				{
					MessageBox(g_pLocalize->GetLocalizeString(IDS_PORTRANGE), g_pLocalize->GetLocalizeString(IDS_ERRORMSG), MB_ICONINFORMATION);
					*pbResult = FALSE;
				}
			}
		default:
			break;
		}
	}
	return 0;
}

LRESULT CDTreeBase::OnCTTPostLabelEdit( WPARAM wParam, LPARAM lParam)
{
	ItemChanged();

	LPBOOL pbResult = (LPBOOL) wParam;
	CString strtmp;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	DWORD Idx;
	PCHAR pObjName;

	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		PFILTER_TRANSMIT pFilter = pFilterTreeInfo->m_pFilter;
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_FILENAME:
			{
				char pBuff[FILTER_BUFFER_SIZE];
				PFILTER_PARAM	pNewParam = (PFILTER_PARAM) pBuff;
				pNewParam->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
				pNewParam->m_ParamFlt = FLT_PARAM_WILDCARD;
				pNewParam->m_ParamID = _PARAM_OBJECT_URL;
				pNewParam->m_ParamSize = lstrlen(info->m_Value.m_pItemText) + 1;
				memcpy(pNewParam->m_ParamValue, info->m_Value.m_pItemText, pNewParam->m_ParamSize);

				ChangeFilterParamValue(pFilterTreeInfo->m_pFilter, pNewParam);

				pObjName = GetObjNameFromFilter(pFilterTreeInfo->m_pFilter);
				if (pObjName == NULL)
					pObjName = "";

				strtmp.Format(g_pLocalize->GetLocalizeString(IDS_OBJECTFMT), pObjName);
				m_pTreeCtrl->SetTreeItemText(info->m_nItemIndex, strtmp);
			}
			break;
		case _FILTER_PROCESSNAME:
			lstrcpyn(pFilterTreeInfo->m_pFilter->m_ProcName, info->m_Value.m_pItemText, PROCNAMELEN);
			strtmp.Format(g_pLocalize->GetLocalizeString(IDS_PROCESSFMT), pFilterTreeInfo->m_pFilter->m_ProcName);
			m_pTreeCtrl->SetTreeItemText(info->m_nItemIndex, strtmp);
			break;
		case _FILTER_EXPIRATION:
			break;
		case _FILTER_DENY:
			break;
		case _FILTER_PROMPT:
			break;
		case _FILTER_TIMEOUT:
			{
				long TimeOut;
				strtmp = info->m_Value.m_pItemText;
				if (String2Long(&strtmp, &TimeOut) == false)
					*pbResult = FALSE;
				else
				{
					pFilterTreeInfo->m_pFilter->m_Timeout = TimeOut;
					strtmp = GetRequestTimeoutString(pFilterTreeInfo->m_pFilter);
					m_pTreeCtrl->SetTreeItemText(info->m_nItemIndex, strtmp);
				}
			}
			break;
		case _FILTER_LOG:
			break;
		case _FILTER_FUNCTION:
			FillFuncList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, info->m_Value.m_pItemText, &Idx);
			pFilter->m_HookID = Idx;
			UpdateSubTree(pFilterTreeInfo, info->m_nItemIndex);
			break;
		case _FILTER_MAJ:
			FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, info->m_Value.m_pItemText, &Idx);
			pFilter->m_FunctionMj = Idx;
			UpdateSubTree(pFilterTreeInfo, info->m_nItemIndex);
			break;
		case _FILTER_MIN:
			FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, info->m_Value.m_pItemText, &Idx);
			pFilter->m_FunctionMi = Idx;
			UpdateSubTree(pFilterTreeInfo, info->m_nItemIndex);
			break;
		case _FILTER_CHECKPARAM:
			break;
		case _FILTER_PARAM:
			break;
		case _FILTER_IPSRC:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilter, ID_IP_SRC_ADDRESS);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					strtmp.Format(g_pLocalize->GetLocalizeString(IDS_IPFMT_SRC), pIp4->ip[0], pIp4->ip[1], pIp4->ip[2], pIp4->ip[3]);
					m_pTreeCtrl->SetTreeItemText(info->m_nItemIndex, strtmp);
				}
			}
			break;
		case _FILTER_IPDST:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilter, ID_IP_DST_ADDRESS);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					strtmp.Format(g_pLocalize->GetLocalizeString(IDS_IPFMT_DST), pIp4->ip[0], pIp4->ip[1], pIp4->ip[2], pIp4->ip[3]);
					m_pTreeCtrl->SetTreeItemText(info->m_nItemIndex, strtmp);
				}
			}
			break;
		/*case _FILTER_IPPORTSRC:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, _PARAM_OBJECT_TR_IPV4_SRC);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					unsigned short Port;
					strtmp = info->m_Value.m_pItemText;
					if (String2US(&strtmp, &Port) == false)
						*pbResult = FALSE;
					else
					{
						pIp4->port = (SHORT) Port;
						strtmp.Format(g_pLocalize->GetLocalizeString(IDS_IPPORTFMT), Port);
						m_pTreeCtrl->SetTreeItemText(info->m_nItemIndex, strtmp);
					}
				}
			}
			break;
		case _FILTER_IPPORTDST:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, _PARAM_OBJECT_TR_IPV4_DEST);
				if (pNewParam != NULL)
				{
					unsigned short Port;
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					strtmp = info->m_Value.m_pItemText;
					if (String2US(&strtmp, &Port) == false)
						*pbResult = FALSE;
					else
					{
						pIp4->port = (SHORT) Port;
						strtmp.Format(g_pLocalize->GetLocalizeString(IDS_IPPORTFMT), Port);
						m_pTreeCtrl->SetTreeItemText(info->m_nItemIndex, strtmp);
					}
				}
			}
			break;*/
		default:
			break;
		}
		UpdateFilterDescribe(info->m_nItemIndex, pFilterTreeInfo);
	}
	return 0;
}

LRESULT CDTreeBase::OnCTTCancelLabelEdit( WPARAM wParam, LPARAM lParam)
{
	// Восстановить текст
	LPBOOL pbChangeText = (LPBOOL) wParam;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	CString strtmp;
	PCHAR pObjName;
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_FILENAME:
			pObjName = GetObjNameFromFilter(pFilterTreeInfo->m_pFilter);
			if (pObjName == NULL)
				pObjName = "";

			strtmp.Format(g_pLocalize->GetLocalizeString(IDS_OBJECTFMT), pObjName);
			SetItemText(&info->m_Value.m_pItemText, &strtmp);
			*pbChangeText = TRUE;
			break;
		case _FILTER_PROCESSNAME:
			strtmp.Format(g_pLocalize->GetLocalizeString(IDS_PROCESSFMT), pFilterTreeInfo->m_pFilter->m_ProcName);
			SetItemText(&info->m_Value.m_pItemText, &strtmp);
			*pbChangeText = TRUE;
			break;
		case _FILTER_EXPIRATION:
			break;
		case _FILTER_DENY:
			break;
		case _FILTER_PROMPT:
			break;
		case _FILTER_TIMEOUT:
			strtmp = GetRequestTimeoutString(pFilterTreeInfo->m_pFilter);
			SetItemText(&info->m_Value.m_pItemText, &strtmp);
			*pbChangeText = TRUE;
			break;
		case _FILTER_LOG:
			break;
		case _FILTER_FUNCTION:
			break;
		case _FILTER_MAJ:
			break;
		case _FILTER_MIN:
			break;
		case _FILTER_CHECKPARAM:
			break;
		case _FILTER_PARAM:
			strtmp = GetParamString(pFilterTreeInfo->m_pFilter);
			SetItemText(&info->m_Value.m_pItemText, &strtmp);
			*pbChangeText = TRUE;
			break;
		case _FILTER_IPSRC:
			break;
		case _FILTER_IPDST:
			break;
		/*case _FILTER_IPPORTSRC:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, _PARAM_OBJECT_TR_IPV4_SRC);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					unsigned short Port = pIp4->port;
				
					strtmp.Format(g_pLocalize->GetLocalizeString(IDS_IPPORTFMT), Port);
					SetItemText(&info->m_Value.m_pItemText, &strtmp);
					*pbChangeText = TRUE;
				}
			}
			break;
		case _FILTER_IPPORTDST:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, _PARAM_OBJECT_TR_IPV4_DEST);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					unsigned short Port = pIp4->port;
					
					strtmp.Format(g_pLocalize->GetLocalizeString(IDS_IPPORTFMT), Port);
					SetItemText(&info->m_Value.m_pItemText, &strtmp);
					*pbChangeText = TRUE;
				}
			}
			break;*/
		default:
			break;
		}
	}
	return 0;
}

void CDTreeBase::ItemChanged()
{
}

void CDTreeBase::UpdateFilterDescribe(int Index, CFilterTreeInfo* pFilterTreeInfo)
{
	CString strtmp = GetFilterDesciptions(pFilterTreeInfo->m_pFilter);

	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	if (pItems == NULL)
		return;

	int Idx = Index - 1;
	bool bExit = false;
	CCheckTreeItem* pItem;
	pFilterTreeInfo = NULL;
	while (!bExit)
	{
		if (Idx >= 0)
		{
			pItem =  (*pItems)[Idx];
			if (pItem != NULL)
			{
				pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
				if (pFilterTreeInfo != NULL)
				{
					if (pFilterTreeInfo->m_nProperty == _FILTER_DESCRIPTION)
						bExit = true;
				}
				else
					bExit = true;
			}
		}
		else
			bExit = true;
		if (bExit == false)
			Idx--;
	}
	if (pFilterTreeInfo != NULL)
	{
		if (pFilterTreeInfo->m_nProperty == _FILTER_DESCRIPTION)
		{
			m_pTreeCtrl->SetTreeItemText(Idx, strtmp);
		}
	}
}

void CDTreeBase::UpdateSubTree(CFilterTreeInfo* pFilterTreeInfo, int nItemIndex)
{
	PFILTER_TRANSMIT pFilter  = pFilterTreeInfo->m_pFilter;
	DeleteSubTreeItemInfo(nItemIndex);
	CCTItemsArray &Items = *new CCTItemsArray(false);
	
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem;
	pItem = (*pItems)[nItemIndex];
	int Level = pItem->GetLevel();
	
	switch(pFilterTreeInfo->m_nProperty)
	{
	case _FILTER_FUNCTION:
		AddMajFunc(&Items, pFilter, true);
		break;
	case _FILTER_MAJ:
		AddMinFunc(&Items, pFilter, true);
		break;
	case _FILTER_MIN:
		break;
	default:
		break;
	}
	DbgPrint(3, "Update items tree (from idx %d)\n", nItemIndex);
	m_pTreeCtrl->UpdateItemsArray(nItemIndex, Items, false);
	
	delete &Items;
}

// -----------------------------------------------------------------------------------------
void CDTreeBase::DeleteSubTreeItemInfo(int nItemIndex)
{
	// Все вложенные убить
    m_pTreeCtrl->ForEach( ::DeleteNodesData, NULL, nItemIndex);
}

LRESULT	CDTreeBase::OnCTTLoadIpAddress(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;
	
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		PFILTER_TRANSMIT pFilter = pFilterTreeInfo->m_pFilter;
		*pbLoad = TRUE;
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_IPSRC:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, ID_IP_SRC_ADDRESS);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					info->m_Value.m_aIPValue[0] = pIp4->ip[0];
					info->m_Value.m_aIPValue[1] = pIp4->ip[1];
					info->m_Value.m_aIPValue[2] = pIp4->ip[2];
					info->m_Value.m_aIPValue[3] = pIp4->ip[3];
				}
				else
					*pbLoad = FALSE;
			}
			break;
		case _FILTER_IPDST:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, ID_IP_DST_ADDRESS);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					info->m_Value.m_aIPValue[0] = pIp4->ip[0];
					info->m_Value.m_aIPValue[1] = pIp4->ip[1];
					info->m_Value.m_aIPValue[2] = pIp4->ip[2];
					info->m_Value.m_aIPValue[3] = pIp4->ip[3];
				}
				else
					*pbLoad = FALSE;
			}
			break;
			
		default:
			*pbLoad = FALSE;
			break;
		}
	}
	return 0;
}

LRESULT	CDTreeBase::OnCTTEndIpEdit(WPARAM wParam, LPARAM lParam)
{
	ItemChanged();
	
	LPBOOL pbResult = (LPBOOL) wParam;
	CString strtmp;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_pTreeCtrl->GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];

	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		PFILTER_TRANSMIT pFilter = pFilterTreeInfo->m_pFilter;
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_IPSRC:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, ID_IP_SRC_ADDRESS);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					pIp4->ip[0] = info->m_Value.m_aIPValue[0];
					pIp4->ip[1] = info->m_Value.m_aIPValue[1];
					pIp4->ip[2] = info->m_Value.m_aIPValue[2];
					pIp4->ip[3] = info->m_Value.m_aIPValue[3];
				}
			}
			break;
		case _FILTER_IPDST:
			{
				PIPV4 pIp4;
				PFILTER_PARAM pNewParam = GetFilterParam(pFilterTreeInfo->m_pFilter, ID_IP_DST_ADDRESS);
				if (pNewParam != NULL)
				{
					pIp4 = (PIPV4) pNewParam->m_ParamValue;
					pIp4->ip[0] = info->m_Value.m_aIPValue[0];
					pIp4->ip[1] = info->m_Value.m_aIPValue[1];
					pIp4->ip[2] = info->m_Value.m_aIPValue[2];
					pIp4->ip[3] = info->m_Value.m_aIPValue[3];
				}
			}
			break;
			
		default:
			*pbResult = FALSE;
		}
	}
	return 0;
}
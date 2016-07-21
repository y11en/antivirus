// DSettings.cpp : implementation file
//

#include "stdafx.h"
#include "AvpgCfg.h"
#include "DTreeMode.h"
#include "DAddNewFilter.h"

#include <hook\IDriver.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//typedef void (*TvActionFunc)( CTreeWnd &owner, HTREEITEM cur, int ind, void *par );
void TreeIterator(CTreeWnd &owner, HTREEITEM cur, int ind, void *par)
{
	CCTItemsArray* pItems =  (CCTItemsArray*) par;
	if (pItems != NULL)
	{
		CCheckTreeItem* pItem =  (*pItems)[ind];
		if (pItem != NULL)
		{
			CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
			if (pFilterTreeInfo != NULL)
				delete pFilterTreeInfo;
		}
	}
}
// -----------------------------------------------------------------------------------------
#define SIZEOF(a) sizeof(a)/sizeof(CHAR*)

CHAR NullStr[]="\0";
CHAR UnknownStr[]="-?-";

#include "..\..\ntcommon.cpp"
#include "..\..\vxdcommn.cpp"

CHAR *szAll="*";

CHAR *ParamOperStr[]={
	"== ",
	" & ",
	" > ",
	" < "
};

BOOL AddFilter(HANDLE hDevice,LPSTR szFileName,LPSTR szProcessName,DWORD dwTimeout,DWORD dwFlags,
			   DWORD HookID, DWORD FuncMj, DWORD FuncMi, LONGLONG ExpireTime) ///*DWORD Param,DWORD Value,*/
{
	BOOL bRet;
	DWORD dwFilterSite = 0;	//new filters position
	bRet = AddFSFilter(hDevice, AppReg.m_AppID, szFileName, szProcessName, dwTimeout, dwFlags, HookID, FuncMj, FuncMi, ExpireTime, PreProcessing, &dwFilterSite);
	return bRet; 
}

bool String2Long(CString* str, long* lng)
{
	bool result = true;
	COleVariant var;
	var = *str;
	try
	{
		var.ChangeType(VT_I4);
		*lng = var.lVal; 
	}
	catch(COleException* pexc)
	{
		pexc->Delete();
		result = false;
	}

	return result;
}

bool String2US(CString* str, unsigned short* lng)
{
	bool result = true;
	COleVariant var;
	var = *str;
	try
	{
		var.ChangeType(VT_UI2);
		*lng = var.uiVal; 
	}
	catch(COleException* pexc)
	{
		pexc->Delete();
		result = false;
	}
	
	return result;
}

CHAR* FillParamList(CStringList *pStringList, PFILTER_TRANSMIT pFilter)
{
	CHAR* pResult = NULL;
/*	DWORD Func = pFilter->m_Function;
	DWORD Param =  pFilter->m_Param & ~FLT_PARAM;

	DWORD FuncNameArrSize;
	CHAR  **FuncNameArr=NULL;
	DWORD MjFn=(Func & ~FLTTYPE)>>8;
	if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		switch(Func & FLTTYPE)
		{
		case FLTTYPE_DISK:
			switch(MjFn)
			{
			case IRP_MJ_READ:
			case IRP_MJ_WRITE:
				FuncNameArrSize=SIZEOF(NTDiskDeviceRWParamName);
				FuncNameArr=NTDiskDeviceRWParamName;
				break;
				//			default:
				//				FuncNameArr=NULL;
			}
			break;
			//		case FLTTYPE_NFIOR:
			//		case FLTTYPE_FIOR:
			//		case FLTTYPE_REGS:
			//		default:
			//			FuncNameArr=NULL;
		}
	}
	else
	{
		switch(Func & FLTTYPE)
		{
		case FLTTYPE_IOS:
			switch(MjFn)
			{
			case IOR_READ:
			case IOR_WRITE:
			case IOR_WRITEV:
				FuncNameArrSize=SIZEOF(VxDIOSRWParamName);
				FuncNameArr=VxDIOSRWParamName;
				break;
			case IOR_GEN_IOCTL:
				FuncNameArrSize=SIZEOF(VxDIOSIOCTLParamName);
				FuncNameArr=VxDIOSIOCTLParamName;
				break;
			}
			break;
			//		case FLTTYPE_IFS:
			//		case FLTTYPE_REGS:
			//		default:
			//			FuncNameArr=NULL;
		}
	}

	if(FuncNameArr != NULL)
		pResult = FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, Param, NULL, NULL);
*/
	return pResult;
}

CString GetParamString(PFILTER_TRANSMIT pFilter)
{
	CString result;
	result.Empty();

/*	CHAR chr[64];
	_itoa(pFilter->m_Value, chr, 10);

	CHAR* pOperType = FillParamList(NULL, pFilter);
	CHAR* pOperator = FillFunctionsList(NULL, ParamOperStr, SIZEOF(ParamOperStr), ((pFilter->m_Param & FLT_PARAM)>>24), NULL, NULL);
	if (pOperType != NULL)
		result.Format("%s %s %s", pOperType, pOperator, chr);

*/
	return result;
}

CString GetFilterDesciptions(PFILTER_TRANSMIT pFilter)
{
	CString strDescr, strSubDescr;
	CString strParam;

	CHAR* pchFunc = FillFuncList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
	CHAR* pchMajFunc = FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
	CHAR* pchMinFunc = FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
	PCHAR pObjName = GetObjNameFromFilter(pFilter);
	if (pObjName == NULL)
		pObjName = "";

	strSubDescr.Empty();
	if (pchFunc != NULL)
	{
		strSubDescr = pchFunc;
	}

	if (pchMajFunc != NULL)
	{
		strSubDescr += ".";
		strSubDescr += pchMajFunc;
	}

	if (pchMinFunc != NULL)
	{
		strSubDescr += ".";
		strSubDescr += pchMinFunc;
	}

	strParam = GetParamString(pFilter);
	if (!strParam.IsEmpty())
		strSubDescr += "(" + strParam + ")";
	
	
	CString Decision = " - allow";
	if (pFilter->m_Flags & FLT_A_DENY)
		Decision = " - deny";
	if (pFilter->m_Flags & FLT_A_POPUP)
		Decision = " - prompt";

	strDescr.Format("%s for object '%s' by process '%s'%s", strSubDescr, 
		(lstrcmp(pObjName, "*") == 0) ? "<all>": pObjName, 
		(lstrcmp(pFilter->m_ProcName, "*") == 0) ? "<all>": pFilter->m_ProcName, Decision);

	if (pFilter->m_Flags & FLT_A_INFO)
		strDescr += " -- (info)";
	if (pFilter->m_Flags & FLT_A_NOTIFY)
		strDescr += " -- (notify)";


	return strDescr;
}
// -----------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CDTreeMode dialog
CString GetRequestTimeoutString(PFILTER_TRANSMIT pFilter)
{
	CString result, strtmp;
	strtmp = g_pLocalize->GetLocalizeString(IDS_REQUESTTIMEOUTFMT);
	result.Format(strtmp, pFilter->m_Timeout);
	
	return result;
}

CHAR* FillFunctionsList(CStringList *pStringList, CHAR  **FuncNameArr,DWORD FuncNameArrSize, DWORD CurrentPos, CHAR* pstrFun, DWORD* Idx)
{
	if (FuncNameArr == NULL)
		return NULL;
	if (FuncNameArrSize == 0)
		return NULL;

	DWORD i = 0;
	DWORD Pos = 0;
	CString strtmp;
	if(FuncNameArr)
	{
		if (CurrentPos == 0)
			while (lstrcmp(FuncNameArr[i++], NullStr) == 0)
				CurrentPos++;
		for(i = 0;i<FuncNameArrSize;i++)
		{
			if (lstrcmp(FuncNameArr[i], NullStr) != 0) // hide empty string
			{
				strtmp = FuncNameArr[i];
				if (pStringList != NULL)
				{
//					if (!strtmp.IsEmpty())
						pStringList->AddTail(strtmp);
				}
				else
				{
					if (CurrentPos != -1)
					{
						if (CurrentPos == Pos)
							return FuncNameArr[i];
					}
					else if (lstrcmp(FuncNameArr[i], pstrFun) == 0)
					{
						*Idx = i;
						TRACE("Generated index for function %s is %d\n", pstrFun, *Idx);
					}
				}
			}
			Pos++;
		}
	}
	return NULL;
}

CHAR* FillFuncList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx)
{
	DWORD FuncNameArrSize;
	CHAR  **FuncNameArr;
	if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		FuncNameArrSize=SIZEOF(NTFunctionTypeName);
		FuncNameArr=NTFunctionTypeName;
	} else {
		FuncNameArrSize=SIZEOF(VxDFunctionTypeName);
		FuncNameArr=VxDFunctionTypeName;
	}
	if (pstrFun != NULL)
		return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, -1, pstrFun, Idx);
	return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, Func, NULL, NULL);
}

CHAR* FillMajList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx)
{
	DWORD FuncNameArrSize;
	CHAR  **FuncNameArr=NULL;

	if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(Func) {
		case FLTTYPE_PID:
			FuncNameArrSize=SIZEOF(NTFunctionPid);
			FuncNameArr=NTFunctionPid;
			break;
		case FLTTYPE_FIOR:
			FuncNameArrSize=SIZEOF(NTFastIOFunctionName);
			FuncNameArr=NTFastIOFunctionName;
			break;
		case FLTTYPE_NFIOR:
		case FLTTYPE_DISK:
			FuncNameArrSize=SIZEOF(NTMajorFunctionName);
			FuncNameArr=NTMajorFunctionName;
			break;
		case FLTTYPE_REGS:
			FuncNameArrSize=SIZEOF(NTRegistryFunctionName);
			FuncNameArr=NTRegistryFunctionName;
			break;
		case FLTTYPE_R3:
			FuncNameArrSize=SIZEOF(NTR3FuncName);
			FuncNameArr=NTR3FuncName;
			break;
			//		default:
//			FuncNameArr=NULL;
		}
	} else {
		switch(Func) {
		case FLTTYPE_IOS:
			FuncNameArrSize=SIZEOF(VxDIOSCommandName);
			FuncNameArr=VxDIOSCommandName;
			break;
		case FLTTYPE_IFS:
			FuncNameArrSize=SIZEOF(VxDIFSFuncName);
			FuncNameArr=VxDIFSFuncName;
			break;
		case FLTTYPE_REGS:
			FuncNameArrSize=SIZEOF(VxDRegistryFunctionName);
			FuncNameArr=VxDRegistryFunctionName;
			break;
		case FLTTYPE_I21:
			FuncNameArrSize=SIZEOF(VxDI21FuncName);
			FuncNameArr=VxDI21FuncName;
			break;
		case FLTTYPE_R3:
			FuncNameArrSize=SIZEOF(VxDR3FuncName);
			FuncNameArr=VxDR3FuncName;
			break;
//		default:
//			FuncNameArr=NULL;
		}
	}
	if (pstrFun != NULL)
		return FillFunctionsList(pStringList, FuncNameArr,FuncNameArrSize,-1, pstrFun, Idx);
	return FillFunctionsList(pStringList, FuncNameArr,FuncNameArrSize, FuncMj, pstrFun, Idx);
}

CHAR* FillMinList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx)
{
	DWORD FuncNameArrSize;
	CHAR  **FuncNameArr=NULL;;
	DWORD MjFn= FuncMj;
	if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(Func) {
		case FLTTYPE_NFIOR:
			switch(MjFn) {
			case IRP_MJ_READ:
			case IRP_MJ_WRITE:
				FuncNameArrSize=SIZEOF(NTMinorFunction4RW);
				FuncNameArr=NTMinorFunction4RW;
				break;
			case IRP_MJ_QUERY_INFORMATION:
			case IRP_MJ_SET_INFORMATION:
				FuncNameArrSize=SIZEOF(NTFileInformation);
				FuncNameArr=NTFileInformation;
				break;
			case IRP_MJ_QUERY_VOLUME_INFORMATION:
			case IRP_MJ_SET_VOLUME_INFORMATION:
				FuncNameArrSize=SIZEOF(NTVolumeInformation);
				FuncNameArr=NTVolumeInformation;
				break;
			case IRP_MJ_DIRECTORY_CONTROL:
				FuncNameArrSize=SIZEOF(NTMinorFunction4DirectoryControl);
				FuncNameArr=NTMinorFunction4DirectoryControl;
				break;
			case IRP_MJ_FILE_SYSTEM_CONTROL:
				FuncNameArrSize=SIZEOF(NTMinorFunction4FSControl);
				FuncNameArr=NTMinorFunction4FSControl;
				break;
			case IRP_MJ_LOCK_CONTROL:
				FuncNameArrSize=SIZEOF(NTMinorFunction4LockControl);
				FuncNameArr=NTMinorFunction4LockControl;
				break;
			case IRP_MJ_PNP_POWER:
				FuncNameArrSize=SIZEOF(NTMinorFunction4PNP_Power);
				FuncNameArr=NTMinorFunction4PNP_Power;
				break;
//			default:
//				FuncNameArr=NULL;
			}
			break;
		case FLTTYPE_DISK:
			switch(MjFn) {
			case IRP_MJ_DEVICE_CONTROL:
				FuncNameArrSize=SIZEOF(NTDiskDeviceIOCTLName);
				FuncNameArr=NTDiskDeviceIOCTLName;
				break;
//			default:
//				FuncNameArr=NULL;
			}
			break;
//		case FLTTYPE_FIOR:
//		case FLTTYPE_REGS:
//		default:
//			FuncNameArr=NULL;
		}
	} else {
		switch(Func) {
		case FLTTYPE_IFS:
			switch(MjFn) {
			case IFSFN_CLOSE:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Close);
				FuncNameArr=VxDIFSFlags4Close;
				break;
			case IFSFN_DIR:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Dir);
				FuncNameArr=VxDIFSFlags4Dir;
				break;
			case IFSFN_DASDIO:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4DASDIO);
				FuncNameArr=VxDIFSFlags4DASDIO;
				break;
			case IFSFN_FILEATTRIB:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4FileAttr);
				FuncNameArr=VxDIFSFlags4FileAttr;
				break;
			case IFSFN_FILETIMES:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4FileTimes);
				FuncNameArr=VxDIFSFlags4FileTimes;
				break;
			case IFSFN_FILELOCKS:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4FileLock);
				FuncNameArr=VxDIFSFlags4FileLock;
				break;
			case IFSFN_OPEN:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4OpenAccessMode);
				FuncNameArr=VxDIFSFlags4OpenAccessMode;
				break;
			case IFSFN_SEARCH:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Search);
				FuncNameArr=VxDIFSFlags4Search;
				break;
			case IFSFN_DISCONNECT:
				FuncNameArrSize=SIZEOF(VxDIFSFlags4Disconnect);
				FuncNameArr=VxDIFSFlags4Disconnect;
				break;
			default:
				FuncNameArr=NULL;
			}
			break;
		case FLTTYPE_I21:
			switch(MjFn) {
			case 0x43:
			case 0x57:
				FuncNameArrSize=SIZEOF(VxDI21FAttrAndDateSubFunc);
				FuncNameArr=VxDI21FAttrAndDateSubFunc;
				break;
			case 0x44:
				FuncNameArrSize=SIZEOF(VxDI21IOCTLSubFunc);
				FuncNameArr=VxDI21IOCTLSubFunc;
				break;
			case 0x71:
				FuncNameArrSize=SIZEOF(VxDI21LFNSubFunc);
				FuncNameArr=VxDI21LFNSubFunc;
				break;
			case 0x73:
				FuncNameArrSize=SIZEOF(VxDI2173SubFunc);
				FuncNameArr=VxDI2173SubFunc;
				break;
			default:
				FuncNameArr=NULL;
			}
			break;
//		case FLTTYPE_IOS:
//		case FLTTYPE_REGS:
//		default:
//			FuncNameArr=NULL;
		}
	}
	if (pstrFun != NULL)
		return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, -1, pstrFun, Idx);
	return FillFunctionsList(pStringList, FuncNameArr, FuncNameArrSize, FuncMi, NULL, NULL);
}

CDTreeMode::CDTreeMode(CFiltersStorage* pFiltersStorage, CWnd* pParent /*=NULL*/)
	: CDTreeBase(&m_TreeCtrl, CDTreeMode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDTreeMode)
	//}}AFX_DATA_INIT
	m_pFiltersStorage = pFiltersStorage;
	m_bSettingChanged = false;
	m_hWndApply = NULL;
	m_hWndCancel = NULL;
}


void CDTreeMode::DoDataExchange(CDataExchange* pDX)
{
	CDTreeBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDTreeMode)
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDTreeMode, CDTreeBase)
	//{{AFX_MSG_MAP(CDTreeMode)
	ON_WM_SIZE()
	ON_MESSAGE(WU_REFRESHTREE, OnRefreshTree)
	ON_MESSAGE(CTTM_PREPROCESSCONTEXTMENU, OnCTTPreProcessContextMenu)
	ON_MESSAGE(CTTM_DOCONTEXTMENUCOMMAND, OnCTTDoContextMenuCommand)
	ON_MESSAGE(CTTM_ITEMSELCHANGING, OnCTTItemSelChanging)
	ON_MESSAGE(CTTM_REMOVEITEM, OnCTTRemoveItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDTreeMode message handlers

BOOL CDTreeMode::OnInitDialog() 
{
	CDTreeBase::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ImageList.Create( IDB_WAS_CTRLTREEIMAGE, 20, 1, RGB(255, 255, 255));
	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDTreeMode::OnSize(UINT nType, int cx, int cy) 
{
	CDTreeBase::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_TreeCtrl.m_hWnd))
	{
		static RULE rules[] = {    // Action    Act-on                   Relative-to           Offset
		// ------    ------                   -----------           ------
		{lSTRETCH, lRIGHT(IDC_TREE),			lRIGHT(lPARENT)				,-1},
		{lSTRETCH, lBOTTOM(IDC_TREE),			lBOTTOM(lPARENT)			,-1},

		{lEND}};

		Layout_ComputeLayout( GetSafeHwnd(), rules);
	}	
}

BOOL CDTreeMode::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDTreeBase::DestroyWindow();
}

void CDTreeMode::RefreshTree(void)
{
	m_pFilterTransmit = NULL;
	RemoveAllTreeInfoItems();

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

	PFILTER_TRANSMIT pFilter;
	int Count = m_pFiltersStorage->m_FiltersArray.GetSize();
//	for (int cou = Count - 1; cou >= 0; cou--)	// последний фильтр в списке проверяется первым
	for (int cou = 0; cou < Count; cou++)
	{
		pFilter = m_pFiltersStorage->m_FiltersArray.ElementAt(cou);
		if (pFilter != NULL)
			AddFilterItem(&newItems, pFilter);
	}
	
	m_TreeCtrl.SetItemsArray(&newItems);
}

// -----------------------------------------------------------------------------------------

LRESULT	CDTreeMode::OnCTTPreProcessContextMenu(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	HMENU hMenu = info->m_Value.m_hMenu;

	CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		if (pFilterTreeInfo != NULL)
		{
			::AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		}
	}
	::AppendMenu(hMenu, MF_STRING, IDMI_ADDNEWFILTER, g_pLocalize->GetLocalizeString(IDS_MENU_ADDNEWFILTER));
	::AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(hMenu, MF_STRING, IDMI_REFRESHFILTERTREE, g_pLocalize->GetLocalizeString(IDS_MENU_REFRESHFILTERTREE));

	*pbLoad = TRUE;
	return 0;
}

LRESULT	CDTreeMode::OnCTTDoContextMenuCommand(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	switch (info->m_Value.m_iCommandId)
	{
	case IDMI_REFRESHFILTERTREE:
		if (m_bSettingChanged == true)
			if (MessageBox("Are you sure? Current changes will be lost!", "Warning", MB_ICONQUESTION | MB_YESNO) == IDYES)
				StateChanged(true);
		break;
	case IDMI_ADDNEWFILTER:
		{
			UpdateFilter(_REQUEST_APLLY);
			CDAddNewFilter DAddNewFilter;
			if (DAddNewFilter.DoModal() == IDOK)
			{
				m_pFiltersStorage->AddNewFilter(DAddNewFilter.GetFilter());
				// найти позицию нового фильра в списке драйвера
				// и вставить его на необходимую позицию...
				PostMessage(WU_REFRESHTREE); //!!
			}
		}
	}
	*pbLoad = TRUE;
	return 0;
}

// ------------------------------------------------------------------------------------------

LRESULT	CDTreeMode::OnCTTItemSelChanging( WPARAM wParam, LPARAM lParam)
{
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	if (info->m_nItemIndex == -1)
		return 0;

	CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
	CCheckTreeItem* pItem;
	pItem = (*pItems)[info->m_nItemIndex];
	CFilterTreeInfo* pFTI = (CFilterTreeInfo*) pItem->GetData();

	if (info->m_Value.m_bSetRemove == TRUE)
	{
		if ((pFTI == NULL) || (m_pFilterTransmit != pFTI->m_pFilter))
			UpdateFilter(_REQUEST_APLLY);
		if (pFTI != NULL)
		{
			if (pFTI->m_pFilter != m_pFilterTransmit)
			{
				m_pFilterTransmit = pFTI->m_pFilter;
				CopyMemory(m_pFilterTransmitSave, m_pFilterTransmit, FILTER_BUFFER_SIZE);
			}
		}
		else
			m_pFilterTransmit = NULL;
	}

	return 0;
}

void CDTreeMode::UpdateFilter(int TypeUpdate)
{
	if (m_bSettingChanged == false)
		return;

	int result = IDYES;
	if (TypeUpdate == _REQUEST_APLLY)
		result = MessageBox("Apply changes?", "Warning", MB_ICONQUESTION | MB_YESNO);
	if (TypeUpdate == _REQUEST_CANCEL)
		result = IDCANCEL;

	if (result == IDYES)
	{
		if (m_pFilterTransmit != NULL)
		{
			char pFilterBuffer[FILTER_BUFFER_SIZE];
			PFILTER_TRANSMIT _pNewFilter = (PFILTER_TRANSMIT) pFilterBuffer;

			CopyMemory(_pNewFilter, m_pFilterTransmit, FILTER_BUFFER_SIZE);
			if (m_pFiltersStorage->DeleteFilter(m_pFilterTransmitSave) == true)
			{
				if (m_pFiltersStorage->AddNewFilter(_pNewFilter))
				{
					//!! удалить из дерева старый и добавить новый в нужную позицию...
				}
				else
					MessageBox("Old filter delete but new filter not created", "Message", MB_ICONINFORMATION);
			}
			else
				MessageBox("Can't change filter value", "Message", MB_ICONINFORMATION);
		}
	}
	else
	{
		//!! Восстановить значение в дереве
	}
	PostMessage(WU_REFRESHTREE); //!!

	m_pFilterTransmit = NULL;

	::EnableWindow(m_hWndApply, FALSE);
	::EnableWindow(m_hWndCancel, FALSE);

	m_bSettingChanged = false;
}

void CDTreeMode::StateChanged(bool bReloadStorage)
{
	if (bReloadStorage == true)
		m_pFiltersStorage->ReloadFiltersArray();
	RefreshTree();
	::EnableWindow(m_hWndApply, FALSE);
	::EnableWindow(m_hWndCancel, FALSE);
}

// -----------------------------------------------------------------------------------------

LRESULT CDTreeMode::OnCTTRemoveItem( WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	CString strtmp;
	CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
		strtmp = "This filter will be deleted!\nAre you sure?\n(";
		strtmp += info->m_Value.m_pItemText;
		strtmp += ")";
		if (MessageBox(strtmp, "Warning", MB_ICONINFORMATION | MB_YESNO) == IDNO)
			return 0;

		CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*)pItem->GetData();
		if (pFilterTreeInfo != NULL)
		{
			if (m_pFilterTransmit != NULL)
			{
				CopyMemory(pFilterTreeInfo->m_pFilter, m_pFilterTransmitSave, FILTER_BUFFER_SIZE);
			}
			if (m_pFiltersStorage->DeleteFilter(pFilterTreeInfo->m_pFilter) == true) // удаляем в драйвере фильтр
			{
				if (pFilterTreeInfo != NULL)
				{
					delete pFilterTreeInfo;
					m_TreeCtrl.ForEach(TreeIterator, pItems, info->m_nItemIndex);	//освобождаем память в subtree
					m_TreeCtrl.DeleteItemWithSubtree(info->m_nItemIndex);			// удаляем ветку
					*pbLoad = TRUE;
				}
			}
		}
	}
	if (*pbLoad == FALSE)
		MessageBox("Can't delete filter", "Warning", MB_ICONINFORMATION);
	PostMessage(WU_REFRESHTREE); //!! кривизна
	return 0;
}

void CDTreeMode::ItemChanged()
{
	m_bSettingChanged = true;
	::EnableWindow(m_hWndApply, TRUE);
	::EnableWindow(m_hWndCancel, TRUE);
}

long CDTreeMode::OnRefreshTree(WPARAM wParam, LPARAM lParam)
{
	StateChanged(true);

	return 0;
}
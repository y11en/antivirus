// DDefaultFilters.cpp : implementation file
// Сделано криво но работает

#include "stdafx.h"
#include "avpgcfg.h"
#include "DDefaultFilters.h"
//#include "FiltersArray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHAR *RegCreateServNT="\\registry\\machine\\system\\currentcontrolset\\services\\*";
CHAR *RegCreateVxD="HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Services\\VxD\\*";

CHAR *RegWriteRunNT="\\registry\\machine\\software\\microsoft\\windows\\currentversion\\run*";
CHAR *RegWriteRunVxD="HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run*";
CHAR *RegWriteRunVxDU="HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run*";


#define SIZEOF(a) sizeof(a)/sizeof(CHAR*)

CHAR *szExecFileName[]=
{
	"*.exe",
	"*.com",
	"*.sys",
	"*.bin",
	"*.dll",
	"*.vxd",
	"*.386",
	"*.ocx",
	"*.cpl",
	"*.bat",
	"*.cmd"
};

#define SIMPLESTRING			1
#define RADIO					2
#define CHECK					3

typedef struct _DEF_FILTER
{
	_DEF_FILTER()
	{
		m_type = -1;
		m_Ids = -1;
		m_bModified = false;
	}
	DWORD m_type;
	DWORD m_Ids;
	bool m_bModified;
	CFiltersArray m_FiltersArray;
}DEF_FILTER, *PDEF_FILTER;

typedef struct _ARREL
{
	DWORD ids;
	DWORD level;
	DWORD type;
}ARREL, *PARREL;

ARREL FileOp_TreeArr[] = {
	{IDS_FILE_OP, 1, SIMPLESTRING},
	{IDS_FILEOP_DENY_W, 3, CHECK},
	{IDS_PERMIT, 4, RADIO},
	{IDS_DENY, 4, RADIO},
	{IDS_PROMPT, 4, RADIO},
	{IDS_DEFAULTPERMIT, 5, RADIO},
	{IDS_DEFAULTDENY, 5, RADIO},
//	{IDS_REQUESTTIMEOUT, 5, CHECK},
	{IDS_LOGTHISEVENT, 4, CHECK},

	{IDS_FILEOP_DENY_D, 3, CHECK},
	{IDS_PERMIT, 4, RADIO},
	{IDS_DENY, 4, RADIO},
	{IDS_PROMPT, 4, RADIO},
	{IDS_DEFAULTPERMIT, 5, RADIO},
	{IDS_DEFAULTDENY, 5, RADIO},
//	{IDS_REQUESTTIMEOUT, 5, CHECK},
	{IDS_LOGTHISEVENT, 4, CHECK},

	{IDS_FILEOP_DENY_R, 3, CHECK},
	{IDS_PERMIT, 4, RADIO},
	{IDS_DENY, 4, RADIO},
	{IDS_PROMPT, 4, RADIO},
	{IDS_DEFAULTPERMIT, 5, RADIO},
	{IDS_DEFAULTDENY, 5, RADIO},
//	{IDS_REQUESTTIMEOUT, 5, CHECK},
	{IDS_LOGTHISEVENT, 4, CHECK},
};

ARREL OtherOp_TreeArr[] = {
	{IDS_DISK_OP, 1, SIMPLESTRING},
	{IDS_DISK_DENY_W_MB, 2, CHECK},
	
	{IDS_PERMIT, 3, RADIO},
	{IDS_DENY, 3, RADIO},
	{IDS_PROMPT, 3, RADIO},
	{IDS_DEFAULTPERMIT, 4, RADIO},
	{IDS_DEFAULTDENY, 4, RADIO},
//	{IDS_REQUESTTIMEOUT, 4, CHECK},

	{IDS_LOGTHISEVENT, 3, CHECK},
	{IDS_DISK_DENY_W_B, 2, CHECK},
	
	{IDS_PERMIT, 3, RADIO},
	{IDS_DENY, 3, RADIO},
	{IDS_PROMPT, 3, RADIO},
	{IDS_DEFAULTPERMIT, 4, RADIO},
	{IDS_DEFAULTDENY, 4, RADIO},
//	{IDS_REQUESTTIMEOUT, 4, CHECK},
	{IDS_LOGTHISEVENT, 3, CHECK},

	{IDS_DISK_DENY_F, 2, CHECK},
	{IDS_PERMIT, 3, RADIO},
	{IDS_DENY, 3, RADIO},
	{IDS_PROMPT, 3, RADIO},
	{IDS_DEFAULTPERMIT, 4, RADIO},
	{IDS_DEFAULTDENY, 4, RADIO},
//	{IDS_REQUESTTIMEOUT, 4, CHECK},
	{IDS_LOGTHISEVENT, 3, CHECK},
/*};

ARREL RegOp_TreeArr[] = {*/
	{IDS_REG_OP, 1, SIMPLESTRING},
	{IDS_DENY_CREATE_AUTOSTART, 2, CHECK},
	{IDS_PERMIT, 3, RADIO},
	{IDS_DENY, 3, RADIO},
	{IDS_PROMPT, 3, RADIO},
	{IDS_DEFAULTPERMIT, 4, RADIO},
	{IDS_DEFAULTDENY, 4, RADIO},
//	{IDS_REQUESTTIMEOUT, 4, CHECK},
	{IDS_LOGTHISEVENT, 3, CHECK},

	{IDS_DENY_CREATE_NEW_S_D, 2, CHECK},
	{IDS_PERMIT, 3, RADIO},
	{IDS_DENY, 3, RADIO},
	{IDS_PROMPT, 3, RADIO},
	{IDS_DEFAULTPERMIT, 4, RADIO},
	{IDS_DEFAULTDENY, 4, RADIO},
//	{IDS_REQUESTTIMEOUT, 4, CHECK},
	{IDS_LOGTHISEVENT, 3, CHECK},
};
/////////////////////////////////////////////////////////////////////////////
// CDDefaultFilters dialog
extern BOOL AddFilter(HANDLE hDevice, LPSTR szFileName, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags,
					  DWORD HookID, DWORD FuncMj, DWORD FuncMi, LONGLONG ExpireTime);

bool SetLogDeny(DWORD ids, 	CFiltersArray* pFilterArray, CControlTreeItem* pItem)
{
	// кривовато :(
	bool bCheck = false;
	PFILTER_TRANSMIT pFilter;
	DWORD Flags;
	if (ids == IDS_LOGTHISEVENT || ids == IDS_DENY || ids == IDS_PERMIT || ids == IDS_PROMPT || ids == IDS_DEFAULTDENY || ids == IDS_DEFAULTPERMIT)
	{
		Flags = 0;
		if (pFilterArray->GetSize() != 0)
		{
			Flags = FLT_A_LOG | FLT_A_POPUP | FLT_A_DENY;
			for (int cou2 = 0; cou2 < pFilterArray->GetSize(); cou2++)
			{
				pFilter = (PFILTER_TRANSMIT) pFilterArray->GetAt(cou2);
				if (!(pFilter->m_Flags & FLT_A_POPUP))
					Flags &= ~FLT_A_POPUP;
				if (!(pFilter->m_Flags & FLT_A_LOG))
					Flags &= ~FLT_A_LOG;
				if (!(pFilter->m_Flags & FLT_A_DENY))
					Flags &= ~FLT_A_DENY;
			}
		}
		if ((ids == IDS_LOGTHISEVENT) && (Flags & FLT_A_LOG))
			bCheck = true;
		if ((ids == IDS_DENY) && (Flags & FLT_A_DENY) && !(Flags & FLT_A_POPUP))
			bCheck = true;
		if ((ids == IDS_PERMIT) && !(Flags & FLT_A_DENY) && !(Flags & FLT_A_POPUP))
			bCheck = true;
		if ((ids == IDS_PROMPT) && (Flags & FLT_A_POPUP))
			bCheck = true;
		if ((ids == IDS_DEFAULTDENY) && (Flags & FLT_A_DENY) && (Flags & FLT_A_POPUP))
			bCheck = true;
		if ((ids == IDS_DEFAULTPERMIT) && !(Flags & FLT_A_DENY) && (Flags & FLT_A_POPUP))
			bCheck = true;
		if (pItem != NULL)
			pItem->Check(bCheck);
	}
	
	return bCheck;
}

bool IsElementChecked(CCTItemsArray* pItems, int idx, DWORD* pFlags)
{
	// this elemet (c) - object
	// +1 (r) Permit
	// +2 (r) Deny
	// +3 (r) Show prompt
	// +4 (r) Allow by default
	// +5 (r) Deny by default
	// +6 (c) Log this event

	CCheckTreeItem* pItem;
	pItem = (*pItems)[idx++];
	if (!pItem->IsChecked())
		return false;
	
	pItem = (*pItems)[idx++]; *pFlags = 0;
	pItem = (*pItems)[idx++]; if (pItem->IsChecked()) *pFlags = FLT_A_DENY;
	pItem = (*pItems)[idx++]; if (pItem->IsChecked()) *pFlags = FLT_A_POPUP;
	if (*pFlags == FLT_A_POPUP)
	{
		pItem = (*pItems)[idx++]; if (pItem->IsChecked()) *pFlags = FLT_A_POPUP;
		pItem = (*pItems)[idx++]; if (pItem->IsChecked()) *pFlags = FLT_A_POPUP | FLT_A_DENY;
	}
	else
		idx += 2;
	pItem = (*pItems)[idx++]; if (pItem->IsChecked()) *pFlags = *pFlags | FLT_A_LOG;

	return true;
}

CControlTreeItem* AddElement(CCTItemsArray* pnewItems, PARREL pArr, char* pch = NULL)
{
	CControlTreeItem *item;

	item = new CControlTreeItem;
	item->SetData(NULL, false);
	if (pch != NULL)
		item->SetText(pch);
	else
		item->SetText(g_pLocalize->GetLocalizeString(pArr->ids));
	item->SetLevel(pArr->level);
	switch (pArr->type)
	{	
	case CHECK:
		item->SetCheckAlias();
		break;
	case RADIO:
		item->SetRadioAlias();
		break;
	case SIMPLESTRING:
		item->SetSimpleStringAlias();
		break;
	default:
		item->SetSimpleStringAlias();
	}
	item->SetCanBeRemoved(false);
	if (pArr->ids == IDS_FILE_OP || pArr->ids == IDS_DISK_OP || pArr->ids == IDS_REG_OP)
		item->SetExpanded(true);
	else
		item->SetExpanded(false);
//	item->SetData((PVOID)pArr->ids, false);
	return item;
}

bool CDDefaultFilters::IsFiltered(char* pchProcName, char* pchObjName, DWORD HookID, DWORD FuncMj, DWORD FuncMi, PFILTER_TRANSMIT pFilter, CFiltersArray* pFilterArray)
{
	/*ZeroMemory(pFilter, sizeof(FILTER_BUFFER_SIZE));
	pFilter->m_AppID = AppReg.m_AppID;
	strcpy(pFilter->m_ProcName, pchProcName);

	char pBuff[FILTER_BUFFER_SIZE];
	PFILTER_PARAM	pNewParam = (PFILTER_PARAM) pBuff;
	pNewParam->m_IsCachable = FALSE;
	pNewParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	pNewParam->m_ParamID = _PARAM_OBJECT_URL;
	pNewParam->m_ParamSize = lstrlen(pchObjName) + 1;
	memcpy(pNewParam->m_ParamValue, pchObjName, pNewParam->m_ParamSize);

	ChangeFilterParamValue(pFilter, pNewParam);

	pFilter->m_HookID = HookID;
	pFilter->m_FunctionMj = FuncMj;
	pFilter->m_FunctionMi = FuncMi;
	
	if (m_pFiltersStorage->IsFiltered(pFilter))
	{
		PFILTER_TRANSMIT pNewFilter = (PFILTER_TRANSMIT) new char[FILTER_BUFFER_SIZE];
		CopyMemory(pNewFilter, pFilter, FILTER_BUFFER_SIZE);
		pFilterArray->Add(pNewFilter);
		return true;
	}*/
	
	return false;
}

CDDefaultFilters::CDDefaultFilters(CFiltersStorage* pFiltersStorage, CWnd* pParent /*=NULL*/)
	: CDialog(CDDefaultFilters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDDefaultFilters)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pFiltersStorage = pFiltersStorage;
	m_MinSize.x = 0;
	m_bSettingChanged = false;
}


void CDDefaultFilters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDDefaultFilters)
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDDefaultFilters, CDialog)
	//{{AFX_MSG_MAP(CDDefaultFilters)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_MESSAGE(CTTM_WINDOWSHUTDOWN, OnCTTWindowShutdown)
	ON_MESSAGE(CTTM_BEGINLABELEDIT, OnCTTBeginLabelEdit)
	ON_MESSAGE(CTTM_POSTLABELEDIT, OnCTTPostLabelEdit)
	ON_MESSAGE(CTTM_CHECKEDCHANGING, OnCTTCheckChanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDDefaultFilters message handlers
// -----------------------------------------------------------------------------------------
BOOL CDDefaultFilters::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rect;
	GetWindowRect(&rect);
	rect.bottom++;
	m_MinSize.x = rect.Width();
	m_MinSize.y = rect.Height();

	m_TreeCtrl.SetEnableOpOnDisabled(false);
	ReloadTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDDefaultFilters::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_FiltersArray.ResetContent();
	return CDialog::DestroyWindow();
}

// -----------------------------------------------------------------------------------------

void CDDefaultFilters::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI)
{
	CWnd::OnGetMinMaxInfo(lpMMI);
	if (m_MinSize.x != 0)
	{
		lpMMI->ptMinTrackSize.x = m_MinSize.x;
		lpMMI->ptMinTrackSize.y = m_MinSize.y;
		lpMMI->ptMaxTrackSize.x = m_MinSize.x;
	}
}

void CDDefaultFilters::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
/*	if (IsWindow(m_TreeCtrl.m_hWnd))
	{
		static RULE rules[] = {    // Action    Act-on                   Relative-to           Offset
		// ------    ------                   -----------           ------
		{lSTRETCH, lRIGHT(IDC_TREE),			lRIGHT(lPARENT)			,-1},
		{lSTRETCH, lBOTTOM(IDC_TREE),			lBOTTOM(lPARENT)		,-1},

		{lEND}};

		Layout_ComputeLayout( GetSafeHwnd(), rules);
	}*/
}

void CDDefaultFilters::UpdateFilters()
{
	if (m_bSettingChanged == false)
		return;

	int result = IDYES;
		result = ::MessageBox(NULL, "Apply changes?", "Warning", MB_ICONQUESTION | MB_YESNO);

	if (result == IDYES)
	{
		CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
		CCheckTreeItem* pItem;
		PDEF_FILTER pDefFilter;
		DWORD dwFlags;
		if (pItems != NULL)
		{
			for (int cou = 0; cou < pItems->GetSize(); cou++)
			{
				pItem = (*pItems)[cou];
				if (pItem != NULL)
				{
					pDefFilter = (PDEF_FILTER) pItem->GetData();
					if (pDefFilter != NULL && pDefFilter->m_bModified == true)
					{
						// remove filters
						for (int subcou =0; subcou < pDefFilter->m_FiltersArray.GetSize(); subcou++)
							m_pFiltersStorage->DeleteFilter(pDefFilter->m_FiltersArray.ElementAt(subcou), true);
						pDefFilter->m_FiltersArray.ResetContent();

						if (IsElementChecked(pItems, cou, &dwFlags))
						{
							if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT)
							{
								if (pDefFilter->m_type != -1)
								{
									switch (pDefFilter->m_type)
									{
									case IDS_FILEOP_DENY_W:
										//AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[subcou], "*", DEFAULT_TIMEOUT, dwFlags, , 0);
										AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[pDefFilter->m_Ids], "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_NORMAL, 0);
										AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[pDefFilter->m_Ids], "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_FIOR, FastIoWrite, 0, 0);
										break;
									case IDS_FILEOP_DENY_D:
										AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[pDefFilter->m_Ids], "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileDispositionInformation, 0);
										break;
									case IDS_FILEOP_DENY_R:
										AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[pDefFilter->m_Ids], "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 0);
										break;
									}
								}
								else
								{
									switch (pDefFilter->m_Ids)
									{
									case IDS_DENY_CREATE_NEW_S_D:
										AddFilter(m_pFiltersStorage->m_hDevice, RegCreateServNT, "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_REGS, CreateKey, 0, 0);
										break;
									case IDS_DENY_CREATE_AUTOSTART:
										AddFilter(m_pFiltersStorage->m_hDevice, RegWriteRunNT, "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_REGS, SetValueKey, 0, 0);
										break;
									}
								}
							}
							else // win9x
							{
								if (pDefFilter->m_type != -1)
								{
									switch (pDefFilter->m_type)
									{
									case IDS_FILEOP_DENY_W:
										AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[pDefFilter->m_Ids], "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_IFS, IFSFN_WRITE, 0, 0);
										break;
									case IDS_FILEOP_DENY_D:
										AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[pDefFilter->m_Ids], "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_IFS, IFSFN_DELETE, 0, 0);
										break;
									case IDS_FILEOP_DENY_R:
										AddFilter(m_pFiltersStorage->m_hDevice, szExecFileName[pDefFilter->m_Ids], "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_IFS, IFSFN_RENAME, 0, 0);
										break;
									}
								}
								else
								{
									if (pDefFilter->m_type != -1)
									{
										switch (pDefFilter->m_type)
										{
											
										case IDS_DENY_CREATE_NEW_S_D:
											AddFilter(m_pFiltersStorage->m_hDevice, RegCreateVxD, "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_REGS, CreateKeyVxD, 0, 0);
											break;
										case IDS_DENY_CREATE_AUTOSTART:
											AddFilter(m_pFiltersStorage->m_hDevice, RegWriteRunVxD, "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_REGS, SetValueExVxD, 0, 0);
											AddFilter(m_pFiltersStorage->m_hDevice, RegWriteRunVxD, "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_REGS, SetValueVxD, 0, 0);
											AddFilter(m_pFiltersStorage->m_hDevice, RegWriteRunVxDU, "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_REGS, SetValueExVxD, 0, 0);
											AddFilter(m_pFiltersStorage->m_hDevice, RegWriteRunVxDU, "*", DEFAULT_TIMEOUT, dwFlags, FLTTYPE_REGS, SetValueVxD, 0, 0);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		m_pFiltersStorage->ReloadFiltersArray();
	}

	m_bSettingChanged = false;
}

void CDDefaultFilters::RemoveAllTreeInfoItems(void)
{
	CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
	CCheckTreeItem* pItem;
	PDEF_FILTER pDefFilter;
	if (pItems != NULL)
	{
		for (int cou = 0; cou < pItems->GetSize(); cou++)
		{
			pItem = (*pItems)[cou];
			if (pItem != NULL)
			{
				pDefFilter = (PDEF_FILTER) pItem->GetData();
				if (pDefFilter != NULL)
				{
					pDefFilter->m_FiltersArray.ResetContent();
					delete pDefFilter;
				}
			}
		}
	}
}

long CDDefaultFilters::OnCTTWindowShutdown(WPARAM wParam, LPARAM lParam)
{
	RemoveAllTreeInfoItems();
	return 0;
}


void CDDefaultFilters::ReloadTree(void)
{
	CCTItemsArray &newItems = *new CCTItemsArray(true);
	
	// с этой кучей надо что-то делать :(
	CControlTreeItem *item;
	int sizeTreeArr;
	sizeTreeArr = sizeof(FileOp_TreeArr) / (3 * sizeof(DWORD));
	
	int cou;
	ARREL arrel;
	arrel.level = 2;
	arrel.type = SIMPLESTRING;
	
	PDEF_FILTER pDefFilter;
	CFiltersArray* pFilterArray;

	char pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;
	bool bFiltered;
	
	item = AddElement(&newItems, &FileOp_TreeArr[0]);
	newItems.Add(item);
	for (int subcou = 0; subcou < SIZEOF(szExecFileName); subcou++)
	{
		item = AddElement(&newItems, &arrel, szExecFileName[subcou]);
		newItems.Add(item);
		pDefFilter = NULL;
		
		for (cou = 1; cou < sizeTreeArr; cou++)
		{
			item = AddElement(&newItems, &FileOp_TreeArr[cou]);
			
			if (FileOp_TreeArr[cou].level == 3)
			{
				pDefFilter = new DEF_FILTER;
				pDefFilter->m_type = FileOp_TreeArr[cou].ids;
				pDefFilter->m_Ids = subcou;
				item->SetData(pDefFilter, false);
				bFiltered = FALSE;
				if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT)
				{
					switch (FileOp_TreeArr[cou].ids)
					{
					case IDS_FILEOP_DENY_W:
						{
							bFiltered = IsFiltered("*", szExecFileName[subcou], FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_NORMAL, _pFilter, &pDefFilter->m_FiltersArray);
							bFiltered = bFiltered && IsFiltered("*", szExecFileName[subcou], FLTTYPE_FIOR, FastIoWrite, 0, _pFilter, &pDefFilter->m_FiltersArray);
						}
						break;
					case IDS_FILEOP_DENY_D:
						bFiltered = IsFiltered("*", szExecFileName[subcou], FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileDispositionInformation, _pFilter, &pDefFilter->m_FiltersArray);
						break;
					case IDS_FILEOP_DENY_R:
						bFiltered = IsFiltered("*", szExecFileName[subcou], FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, _pFilter, &pDefFilter->m_FiltersArray);
						break;
					}
				}
				else
				{//Win 9x
					switch (FileOp_TreeArr[cou].ids)
					{
					case IDS_FILEOP_DENY_W:
						bFiltered = IsFiltered("*", szExecFileName[subcou], FLTTYPE_IFS, IFSFN_WRITE, 0, _pFilter, &pDefFilter->m_FiltersArray);
						if (bFiltered)
							OutputDebugString("IDS_FILEOP_DENY_W\n");
						//bFiltered = bFiltered && IsFiltered("*", szExecFileName[subcou], FLTTYPE_IFS, IFSFN_OPEN, 1, &Filter, &pDefFilter->m_FiltersArray);
						//bFiltered = bFiltered && IsFiltered("*", szExecFileName[subcou], FLTTYPE_IFS, IFSFN_OPEN, 2, &Filter, &pDefFilter->m_FiltersArray);
						break;
					case IDS_FILEOP_DENY_D:
						bFiltered = IsFiltered("*", szExecFileName[subcou], FLTTYPE_IFS, IFSFN_DELETE, 0, _pFilter, &pDefFilter->m_FiltersArray);
						if (bFiltered)
							OutputDebugString("IDS_FILEOP_DENY_D\n");
						break;
					case IDS_FILEOP_DENY_R:
						bFiltered = IsFiltered("*", szExecFileName[subcou], FLTTYPE_IFS, IFSFN_RENAME, 0, _pFilter, &pDefFilter->m_FiltersArray);
						if (bFiltered)
							OutputDebugString("IDS_FILEOP_DENY_R\n");
						break;
					}
				}
				item->Check(bFiltered);
//				TRACE("Item checked: %d\n", bFiltered);
			}
			SetLogDeny(FileOp_TreeArr[cou].ids, &pDefFilter->m_FiltersArray, item);
			newItems.Add(item);
		}
	}	

	DWORD Type = -1;
	pDefFilter = NULL;
	sizeTreeArr = sizeof(OtherOp_TreeArr) / (3 * sizeof(DWORD));
	pFilterArray = NULL;
	bool bEnabled = false;
//	DWORD type = -1;
	for (cou = 0; cou < sizeTreeArr; cou++)
	{
		item = AddElement(&newItems, &OtherOp_TreeArr[cou]);
		
		// -----------------------------------------------------------------------------------------
		if (OtherOp_TreeArr[cou].level == 1)
		{
//			type = ;
			if (OtherOp_TreeArr[cou].ids == IDS_REG_OP)
				bEnabled = true;
		}
		item->SetEnabled(bEnabled);
		// -----------------------------------------------------------------------------------------
		
		bFiltered = false;
		
		if (OtherOp_TreeArr[cou].level == 2)
		{
			pDefFilter = new DEF_FILTER;
//			pDefFilter->m_type = type;
			pDefFilter->m_Ids = OtherOp_TreeArr[cou].ids;
			item->SetData(pDefFilter, false);
		}
		if(g_OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT)
		{
			switch (OtherOp_TreeArr[cou].ids)
			{
			case IDS_DENY_CREATE_NEW_S_D:
				{
					bFiltered = IsFiltered("*", RegCreateServNT, FLTTYPE_REGS, CreateKey, 0, _pFilter, &pDefFilter->m_FiltersArray);
				}
				break;
			case IDS_DENY_CREATE_AUTOSTART:
				{
					bFiltered = IsFiltered("*", RegWriteRunNT, FLTTYPE_REGS, SetValueKey, 0, _pFilter, &pDefFilter->m_FiltersArray);
				}
			}
		}
		else
		{
			switch (OtherOp_TreeArr[cou].ids)
			{
			case IDS_DENY_CREATE_NEW_S_D:
				{
					bFiltered = IsFiltered("*", RegCreateVxD, FLTTYPE_REGS, CreateKeyVxD, 0, _pFilter, &pDefFilter->m_FiltersArray);
				}
				break;
			case IDS_DENY_CREATE_AUTOSTART:
				{
					bFiltered = bFiltered && IsFiltered("*", RegWriteRunVxD, FLTTYPE_REGS, SetValueExVxD, 0, _pFilter, &pDefFilter->m_FiltersArray);
					bFiltered = bFiltered && IsFiltered("*", RegWriteRunVxD, FLTTYPE_REGS, SetValueVxD, 0, _pFilter, &pDefFilter->m_FiltersArray);
					bFiltered = bFiltered && IsFiltered("*", RegWriteRunVxDU, FLTTYPE_REGS, SetValueExVxD, 0, _pFilter, &pDefFilter->m_FiltersArray);
					bFiltered = bFiltered && IsFiltered("*", RegWriteRunVxDU, FLTTYPE_REGS, SetValueVxD, 0,  _pFilter, &pDefFilter->m_FiltersArray);
				}
			}
		}
		if (bFiltered)
		{
			item->Check();
			bFiltered = false;
		}
		SetLogDeny(OtherOp_TreeArr[cou].ids, &pDefFilter->m_FiltersArray, item);
		newItems.Add(item);
	}
	
	m_TreeCtrl.SetItemsArray(&newItems);
}

LRESULT CDDefaultFilters::OnCTTBeginLabelEdit( WPARAM wParam, LPARAM lParam)
{
	CString strtmp;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
	}
	return 0;
}

LRESULT CDDefaultFilters::OnCTTPostLabelEdit( WPARAM wParam, LPARAM lParam)
{
	ItemChanged();

	LPBOOL pbResult = (LPBOOL) wParam;
	CString strtmp;
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
	CCheckTreeItem* pItem =  (*pItems)[info->m_nItemIndex];
	if (pItem != NULL)
	{
	}

	return 0;
}

LRESULT	CDDefaultFilters::OnCTTCheckChanging( WPARAM wParam, LPARAM lParam )
{
	ItemChanged();

	PDEF_FILTER pDefFilter;	
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;
	CCTItemsArray* pItems =  m_TreeCtrl.GetItemsArray();
	int idx = info->m_nItemIndex;
	CCheckTreeItem* pItem = (*pItems)[idx];
	while (pItem != NULL)
	{
		pDefFilter = (PDEF_FILTER) pItem->GetData();
		pItem = NULL;
		if (pDefFilter != NULL)
			pDefFilter->m_bModified = true;
		else
		{
			if ((--idx) >= 0)
				pItem = (*pItems)[idx];
		}
	}
	
	return 0;
}

void CDDefaultFilters::ItemChanged()
{
	m_bSettingChanged = true;
}


void CDDefaultFilters::OnOK() 
{
	// TODO: Add extra validation here
	UpdateFilters();
	CDialog::OnOK();
}

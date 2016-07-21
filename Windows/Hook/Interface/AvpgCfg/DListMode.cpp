// DListMode.cpp : implementation file
//
//CELC.lib

#include "stdafx.h"
#include "avpgcfg.h"
#include "DListMode.h"
#include "FilterTreeInfo.h"
#include "EnumProcess.h"

#include "DAddNewFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDListMode dialog


CDListMode::CDListMode(CFiltersStorage* pFiltersStorage, CWnd* pParent /*=NULL*/)
	: CDialog(CDListMode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDListMode)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pFiltersStorage = pFiltersStorage;
	m_bSettingChanged = false;

	m_pFilterTransmit = NULL;
	m_pFilterTransmitSave = (PFILTER_TRANSMIT) m_pFilterBuffer;
}


void CDListMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDListMode)
	DDX_Control(pDX, IDC_LIST, m_lList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDListMode, CDialog)
	//{{AFX_MSG_MAP(CDListMode)
	ON_WM_SIZE()
	ON_MESSAGE(WU_REFRESHTREE, OnRefreshList)
	ON_MESSAGE(CELCM_LOADCOMBOCONTENTS, OnCELCLoadComboContents)
	ON_MESSAGE(CELCM_ENDLABELEDIT, OnCELCEndLabelEdit)
	ON_MESSAGE(CELCM_POSTLABELEDIT, OnCELCPostLabelEdit)
	ON_MESSAGE(CELCM_CHECKEDCHANGING, OnCELCCheckChanging)
	ON_MESSAGE(CELCM_PREPROCESSCONTEXTMENU, OnCELCPreProcessContextMenu)
	ON_MESSAGE(CELCM_DOCONTEXTMENUCOMMAND, OnCELCDoContextMenuCommand)
	ON_MESSAGE(CELCM_ITEMSELCHANGING, OnCELCSelChanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDListMode message handlers

void CDListMode::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_lList.m_hWnd))
	{
		static RULE rules[] = {    // Action    Act-on                   Relative-to           Offset
		// ------    ------                   -----------           ------
		{lSTRETCH, lRIGHT(IDC_LIST),			lRIGHT(lPARENT)				,-1},
		{lSTRETCH, lBOTTOM(IDC_LIST),			lBOTTOM(lPARENT)			,-1},

		{lEND}};

		Layout_ComputeLayout( GetSafeHwnd(), rules);
	}	
}

BOOL CDListMode::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_lList.PrepareControl(0);

	LV_COLUMN lvcolumn;
	CRect rect;
	CString TitleArr[10] = {_T("Object"), _T("Process"), _T("Function type "), _T("Major"), _T("Minor"), _T("Check parameters"), _T("Parameters value"), _T("Flags"), _T("Flags param"), _T("Log")};
	TCHAR tbuf[32];
	int sizeTitleArr = sizeof(TitleArr) / sizeof(*TitleArr);
	m_lList,GetClientRect(&rect);
	for (int cou = 0; cou < sizeTitleArr; cou++)  // add the columns to the list control
	{
		_tcscpy(tbuf, _T(TitleArr[cou]));
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = tbuf;
		lvcolumn.iSubItem = cou;
		lvcolumn.cx = 90;
		m_lList.InsertColumn(cou, &lvcolumn);  // assumes return value is OK.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDListMode::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	FreeUserData();
	return CDialog::DestroyWindow();
}

void CDListMode::FreeUserData()
{
	CFilterTreeInfo* pFilterTreeInfo;
	int ColumnCount = m_lList.GetHeaderCtrl()->GetItemCount();
	int RowCount = m_lList.GetItemCount();
	for (int row = 0; row < RowCount; row++)
	{
		for (int col = 0; col < ColumnCount; col++)
		{
			pFilterTreeInfo = (CFilterTreeInfo*) m_lList.GetItemData(row, col);
			if (pFilterTreeInfo != NULL)
				delete pFilterTreeInfo;
		}
	}
}

void CDListMode::UpdateFilter(int TypeUpdate)
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
	PostMessage(WU_REFRESHTREE); //!!

	::EnableWindow(m_hWndApply, FALSE);
	::EnableWindow(m_hWndCancel, FALSE);

	m_bSettingChanged = false;
}

void CDListMode::StateChanged(bool bReloadStorage)
{
	CString strtmp;
	CListCtrlItem* pListCtrlItem;
	CFilterTreeInfo* pFilterTreeInfo;
	PFILTER_TRANSMIT pFilter;
	TCHAR buffer[MAX_PATH];

	PCHAR pObjName;

	FreeUserData();
	m_lList.DeleteAllItems();

	m_bSettingChanged = false;

	if (bReloadStorage == true)
		m_pFiltersStorage->ReloadFiltersArray();

	LV_ITEM lvitem;
	int Count = m_pFiltersStorage->m_FiltersArray.GetSize();
	int idx = 0;
	int iSubItem = 0;
	for (int cou = 0; cou < Count; cou++)
	{
		pFilter = m_pFiltersStorage->m_FiltersArray.ElementAt(cou);
		if (pFilter != NULL)
		{
			iSubItem = 0;
			pObjName = GetObjNameFromFilter(pFilter);
			if (pObjName == NULL)
				pObjName = "";
			// -----------------------------------------------------------------------------------------
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			lvitem.iItem = idx;
			lvitem.iSubItem = iSubItem++;
			sprintf( buffer, "%s", pObjName);
			lvitem.pszText = buffer;
			pListCtrlItem = new CListCtrlItem;
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_FILENAME);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetSimpleEditAlias();
			lvitem.lParam = (LPARAM) pListCtrlItem;

			m_lList.InsertItem(&lvitem);
			// -----------------------------------------------------------------------------------------
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			lvitem.iSubItem = iSubItem++;
			sprintf( buffer, "%s", pFilter->m_ProcName);
			lvitem.pszText = buffer;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_PROCESSNAME);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetDropDownComboAlias();
			m_lList.InsertItem(&lvitem);
			// -----------------------------------------------------------------------------------------
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			CHAR* pchFunc = FillFuncList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			if (pchFunc != NULL)
			{
				sprintf( buffer, "%s", pchFunc);
				lvitem.pszText = buffer;
			}
			else
			{
				pListCtrlItem->SetEnabled(false);
				lvitem.pszText = NULL;
			}
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_FUNCTION);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetDropDownListComboAlias();
			m_lList.InsertItem(&lvitem);
			// -----------------------------------------------------------------------------------------
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			CHAR* pchMajFunc = FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			if (pchMajFunc != NULL)
			{
				sprintf( buffer, "%s", pchMajFunc);
				lvitem.pszText = buffer;
			}
			else
			{
				pListCtrlItem->SetEnabled(false);
				lvitem.pszText = NULL;
			}
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_MAJ);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetDropDownListComboAlias();
			m_lList.InsertItem(&lvitem);
			// -----------------------------------------------------------------------------------------
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			CHAR* pchMinFunc = FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			if (pchMinFunc != NULL)
			{
				sprintf( buffer, "%s", pchMinFunc);
				lvitem.pszText = buffer;
			}
			else
			{
				pListCtrlItem->SetEnabled(false);
				lvitem.pszText = NULL;
			}
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_MIN);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetDropDownListComboAlias();
			m_lList.InsertItem(&lvitem);
			// -----------------------------------------------------------------------------------------
			strtmp = GetParamString(pFilter);

			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			sprintf( buffer, "Check parametrs");
			lvitem.pszText = buffer;
			//if (strtmp.IsEmpty())
				pListCtrlItem->SetEnabled(false);
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_CHECKPARAM);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetCheckAlias();
			m_lList.InsertItem(&lvitem);

			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			sprintf( buffer, "%s", strtmp);
			lvitem.pszText = buffer;
			if (strtmp.IsEmpty())
				pListCtrlItem->SetEnabled(false);
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_PARAM);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetExclusiveExternalEditingAlias();
			m_lList.InsertItem(&lvitem);
			// -----------------------------------------------------------------------------------------
			int bFlag = pFilter->m_Flags & FLT_A_POPUP;					// 1 - if popup
			if (bFlag == 0)
				bFlag = (pFilter->m_Flags & FLT_A_DENY) == 0 ? -1 : 0;	// allow
			if (bFlag == 0)
				bFlag = (pFilter->m_Flags & FLT_A_DENY) != 0 ? -2 : 0;	// deny
			
			switch (bFlag)
			{
			case 1:
				strtmp = "show query";
				break;
			case -1:
				strtmp = "allow action";
				break;
			case -2:
				strtmp = "deny action";
				break;
			default:
				strtmp = "bug!";
				break;
			}
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			sprintf( buffer, "%s", strtmp);
			lvitem.pszText = buffer;
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_FLAGS);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetDropDownListComboAlias();
			m_lList.InsertItem(&lvitem);

			// -----------------------------------------------------------------------------------------
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			strtmp.Format("Timeout: %d.", pFilter->m_Timeout);
			if (pFilter->m_Flags & FLT_A_DENY)
				strtmp += " Default: deny action.";
			else
				strtmp += " Default: allow action.";

			sprintf(buffer, "%s", strtmp);
			lvitem.pszText = buffer;
			if (bFlag <= 0)
				pListCtrlItem->SetEnabled(false);
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_PARAM);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetExclusiveExternalEditingAlias();
			m_lList.InsertItem(&lvitem);

			// -----------------------------------------------------------------------------------------
			lvitem.mask = LVIF_TEXT | LVIF_PARAM;
			lvitem.iSubItem = iSubItem++;
			pListCtrlItem = new CListCtrlItem;
			lvitem.lParam = (LPARAM) pListCtrlItem;
			sprintf( buffer, "Log this event");
			lvitem.pszText = buffer;
			pFilterTreeInfo = new CFilterTreeInfo(pFilter, _FILTER_LOG);
			pListCtrlItem->SetUserData((DWORD) pFilterTreeInfo);
			pListCtrlItem->SetCheckAlias();
			if (pFilter->m_Flags & FLT_A_LOG)
				pListCtrlItem->SetChecked();
			m_lList.InsertItem(&lvitem);
			// -----------------------------------------------------------------------------------------
			idx++;
		}
	}
}

CListCtrlItem* CDListMode::GetItemWith(int Row, int nProperty, int *pnColumnIndex)
{
	CListCtrlItem* pItem;
	CFilterTreeInfo* pFilterTreeInfo;
	for (int cou = 0; cou < m_lList.GetColumnCount();cou++)
	{
		pItem = m_lList.GetListCtrlItem(Row, cou);
		if (pItem != NULL)
		{
			pFilterTreeInfo = (CFilterTreeInfo*) pItem->GetUserData();
			if (pFilterTreeInfo != NULL)
				if (pFilterTreeInfo->m_nProperty == nProperty)
				{
					*pnColumnIndex = cou;
					return pItem;
				}
		}
	}
	return NULL;
}
// -----------------------------------------------------------------------------------------
LRESULT CDListMode::OnCELCLoadComboContents(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbResult = (LPBOOL) wParam;
	CELCMessageInfo *info = (CELCMessageInfo *)lParam;
	CStringList* pStringList = info->m_Value.m_pStringList;

	PFILTER_TRANSMIT pFilter;

	*pbResult = TRUE;
	CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*) info->UserValue;
	if (pFilterTreeInfo != NULL)
	{
		pFilter = pFilterTreeInfo->m_pFilter;
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_PROCESSNAME:
			{
				CEnumProcess Enum((CStringList*) info->m_Value.m_pStringList);
				Enum.EnumProcesses();
			}
			break;
		case _FILTER_FUNCTION:
			FillFuncList(pStringList, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			break;
		case _FILTER_MAJ:
			FillMajList(pStringList, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			break;
		case _FILTER_MIN:
			FillMinList(pStringList, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			break;
		case _FILTER_FLAGS:
			pStringList->AddTail("show query");
			pStringList->AddTail("allow action");
			pStringList->AddTail("deny action");
			break;
		default:
			*pbResult = FALSE;
		}
	}
	else
		*pbResult = FALSE;
	
	return 0;
}

LRESULT CDListMode::OnCELCEndLabelEdit(WPARAM wParam, LPARAM lParam)
{
	CString strtmp;

	LPBOOL pbResult = (LPBOOL) wParam;
	CELCMessageInfo *info = (CELCMessageInfo *)lParam;

	*pbResult = TRUE;
	CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*) info->UserValue;
	if (pFilterTreeInfo != NULL)
	{
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_FILENAME:
			if (lstrlen(info->m_Value.m_pItemText) > MAXPATHLEN)
			{
				MessageBox("Длина строки должна быть меньше MAXPATHLEN", "Ошибка", MB_ICONINFORMATION);
				*pbResult = FALSE;
			}
			break;
		case _FILTER_PROCESSNAME:
			if (lstrlen(info->m_Value.m_pItemText) > PROCNAMELEN)
			{
				MessageBox("Длина строки должна быть меньше PROCNAMELEN", "Ошибка", MB_ICONINFORMATION);
				*pbResult = FALSE;
			}
			break;
		default:
			break;
		}
	}
	return 0;
}

LRESULT CDListMode::OnCELCPostLabelEdit(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbResult = (LPBOOL) wParam;
	CELCMessageInfo *info = (CELCMessageInfo *)lParam;
	CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*) info->UserValue;
	DWORD Idx;
	PFILTER_TRANSMIT pFilter;
	m_bSettingChanged = true;
	if (pFilterTreeInfo != NULL)
	{
		pFilter = pFilterTreeInfo->m_pFilter;
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_FILENAME:
			{
				char pBuff[FILTER_BUFFER_SIZE];
				PFILTER_PARAM	pNewParam = (PFILTER_PARAM) pBuff;
				pNewParam->m_IsCachable = FALSE;
				pNewParam->m_ParamFlt = FLT_PARAM_WILDCARD;
				pNewParam->m_ParamID = _PARAM_OBJECT_URL;
				pNewParam->m_ParamSize = lstrlen(info->m_Value.m_pItemText) + 1;
				memcpy(pNewParam->m_ParamValue, info->m_Value.m_pItemText, pNewParam->m_ParamSize);

				ChangeFilterParamValue(pFilterTreeInfo->m_pFilter, pNewParam);
			}
			break;
		case _FILTER_PROCESSNAME:
			lstrcpyn(pFilter->m_ProcName, info->m_Value.m_pItemText, PROCNAMELEN);
			break;
		case _FILTER_FUNCTION:
			FillFuncList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, info->m_Value.m_pItemText, &Idx);
			pFilter->m_HookID = Idx;
			UpdateList(info);
			break;
		case _FILTER_MAJ:
			FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, info->m_Value.m_pItemText, &Idx);
			pFilter->m_FunctionMj = Idx;
			UpdateList(info);
			break;
		case _FILTER_MIN:
			FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, info->m_Value.m_pItemText, &Idx);
			pFilter->m_FunctionMi = Idx;
			UpdateList(info);
			break;
		case _FILTER_PARAM:
			break;
		default:
			break;
		}
	}
	::EnableWindow(m_hWndApply, TRUE);
	::EnableWindow(m_hWndCancel, TRUE);

	return 0;
}

LRESULT CDListMode::OnCELCCheckChanging(WPARAM wParam, LPARAM lParam)
{
	CELCMessageInfo *info = (CELCMessageInfo *)lParam;
	CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*) info->UserValue;
	CListCtrlItem* pListItem;
	if (pFilterTreeInfo != NULL)
	{
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_CHECKPARAM:
			pListItem = GetItemWith(info->m_nRowIndex, _FILTER_PARAM, &info->m_nColumnIndex);
			pListItem->SetEnabled(info->m_Value.m_bSetRemove == TRUE ? true : false);
			m_lList.Update(info->m_nRowIndex, info->m_nColumnIndex);
			break;
		}
	}
	return 0;
}

void CDListMode::UpdateList(CELCMessageInfo *info)
{
	CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*) info->UserValue;
	PFILTER_TRANSMIT pFilter;
	CListCtrlItem* pListItem;
	CHAR* pch;
	char empty[] = "";
	if (pFilterTreeInfo != NULL)
	{
		pFilter = pFilterTreeInfo->m_pFilter;
		switch (pFilterTreeInfo->m_nProperty)
		{
		case _FILTER_FUNCTION:
			pListItem = GetItemWith(info->m_nRowIndex, _FILTER_MAJ, &info->m_nColumnIndex);
			info->UserValue = (DWORD) pListItem->GetUserData();
			ASSERT(info->UserValue != NULL);
			UpdateList(info);
			break;
		case _FILTER_MAJ:
			pch = FillMajList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			pListItem = GetItemWith(info->m_nRowIndex, _FILTER_MAJ, &info->m_nColumnIndex);
			if (pch == NULL)
			{
				pListItem->SetEnabled(false);
				pch = empty;
			}
			else
				pListItem->SetEnabled(true);
			m_lList.SetItemText(info->m_nRowIndex, info->m_nColumnIndex, pch);
			
			pListItem = GetItemWith(info->m_nRowIndex, _FILTER_MIN, &info->m_nColumnIndex);
			info->UserValue = (DWORD) pListItem->GetUserData();
			ASSERT(info->UserValue != NULL);
			UpdateList(info);
			break;
		case _FILTER_MIN:
			pch = FillMinList(NULL, pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi, NULL, NULL);
			pListItem = GetItemWith(info->m_nRowIndex, _FILTER_MIN, &info->m_nColumnIndex);
			if (pch == NULL)
			{
				pListItem->SetEnabled(false);
				pch = empty;
			}
			else
				pListItem->SetEnabled(true);
			m_lList.SetItemText(info->m_nRowIndex, info->m_nColumnIndex, pch);

			pListItem = GetItemWith(info->m_nRowIndex, _FILTER_CHECKPARAM, &info->m_nColumnIndex);
			info->UserValue = (DWORD) pListItem->GetUserData();
			ASSERT(info->UserValue != NULL);
			UpdateList(info);
			break;
		case _FILTER_CHECKPARAM:
			{
				bool bEnable = false;
				CString strtmp;
				strtmp = GetParamString(pFilterTreeInfo->m_pFilter);
				pListItem = GetItemWith(info->m_nRowIndex, _FILTER_CHECKPARAM, &info->m_nColumnIndex);
				if (strtmp.IsEmpty())
					pch = empty;
				else
				{
					//bEnable = true; 				//!! disbaled!
					pch = (LPSTR)(LPCSTR) strtmp;
				}
				pListItem->SetEnabled(bEnable);
				bEnable = pListItem->IsChecked();

				pListItem = GetItemWith(info->m_nRowIndex, _FILTER_PARAM, &info->m_nColumnIndex);
				m_lList.SetItemText(info->m_nRowIndex, info->m_nColumnIndex, pch);
				pListItem->SetEnabled(bEnable);
			}
			break;
		default:
			break;
		}
	}
}

LRESULT CDListMode::OnCELCPreProcessContextMenu(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;
	CELCMessageInfo *info = (CELCMessageInfo *)lParam;
	HMENU hMenu = info->m_Value.m_hMenu;

	if (info->m_nRowIndex != -1)
		::AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(hMenu, MF_STRING, IDMI_ADDNEWFILTER, g_pLocalize->GetLocalizeString(IDS_MENU_ADDNEWFILTER));
	::AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(hMenu, MF_STRING, IDMI_REFRESHFILTERTREE, g_pLocalize->GetLocalizeString(IDS_MENU_REFRESHFILTERTREE));

	*pbLoad = TRUE;
	return 0;
}

LRESULT CDListMode::OnCELCDoContextMenuCommand(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;
	CELCMessageInfo *info = (CELCMessageInfo *)lParam;

	switch (info->m_Value.m_iCommandId)
	{
	case IDMI_REFRESHFILTERTREE:
		if (m_bSettingChanged == true)
			if (MessageBox("Are you sure? Current changing will be lost!", "Warning", MB_ICONQUESTION | MB_YESNO) == IDYES)
				StateChanged();
		break;
	case IDMI_ADDNEWFILTER:
		{
			UpdateFilter(_REQUEST_APLLY);
			CDAddNewFilter DAddNewFilter;
			if (DAddNewFilter.DoModal() == IDOK)
			{
				m_pFiltersStorage->AddNewFilter(DAddNewFilter.GetFilter());
				StateChanged(true);
			}
		}
		break;
	}
	*pbLoad = TRUE;
	return 0;
}

long CDListMode::OnRefreshList(WPARAM wParam, LPARAM lParam)
{
	StateChanged(true);

	return 0;
}

LRESULT CDListMode::OnCELCSelChanging(WPARAM wParam, LPARAM lParam)
{
	LPBOOL pbLoad = (LPBOOL) wParam;
	CELCMessageInfo *info = (CELCMessageInfo *)lParam;

	CFilterTreeInfo* pFilterTreeInfo = (CFilterTreeInfo*) info->UserValue;
	if (info->m_Value.m_bSetRemove == TRUE && pFilterTreeInfo != NULL)
	{
		if (pFilterTreeInfo->m_pFilter != m_pFilterTransmit)
		{
			if (m_pFilterTransmit != NULL)
				UpdateFilter(_REQUEST_APLLY);

			m_pFilterTransmit = pFilterTreeInfo->m_pFilter;
			CopyMemory(m_pFilterTransmitSave, m_pFilterTransmit, FILTER_BUFFER_SIZE);
		}
	}

	return 0;
}

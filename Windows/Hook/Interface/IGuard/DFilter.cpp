// DFilter.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DFilter.h"

#include "EnumProcess.h"

#include "DParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFilter dialog
CDFilter::CDFilter(PFILTER_TRANSMIT pFilter, CString* pCaption, CWnd* pParent /*=NULL*/)
	: CDialog(CDFilter::IDD, pParent)
{

	m_AddPos_HookID = 0;
	m_AddPos_FuncMj = 0;
	m_AddPos_FuncMi = 0;

	m_pFilter = pFilter;
	m_FilterWork = (PFILTER_TRANSMIT) m_pFilterBuffer;

	if (m_pFilter != NULL)
		CopyMemory(m_FilterWork, m_pFilter, FILTER_BUFFER_SIZE);
	else
	{
		ZeroMemory(m_pFilterBuffer, FILTER_BUFFER_SIZE);
		m_FilterWork->m_AppID = AppReg.m_AppID;
		lstrcpy(m_FilterWork->m_ProcName, "*");
		m_FilterWork->m_Timeout = 30;
	}

	m_pCaption = pCaption;
	
	//{{AFX_DATA_INIT(CDFilter)
	m_eExpTime = 0;
	m_chProcessnameCacheable = FALSE;
	m_eFilterName = _T("");
	m_chDbgBreak = FALSE;
	//}}AFX_DATA_INIT

	m_chMiAll = m_FilterWork->m_FunctionMj == -1 ? TRUE : FALSE;
	m_chMjAll = m_FilterWork->m_FunctionMi == -1 ? TRUE : FALSE;;
	

	m_rPos = 1;

	m_chTouchFilter = (m_FilterWork->m_Flags & FLT_A_TOUCH) != 0 ? TRUE : FALSE;
	m_chDontPass = (m_FilterWork->m_Flags & FLT_A_DENYPASS2LOWPR) != 0 ? TRUE : FALSE;
	m_chLogThis = (m_FilterWork->m_Flags & FLT_A_LOG) != 0 ? TRUE : FALSE;
	m_chTryNextFilter = (m_FilterWork->m_Flags & FLT_A_CHECKNEXTFILTER) != 0 ? TRUE : FALSE;
	m_chDbgBreak = (m_FilterWork->m_Flags & FLT_A_DBG_BREAK) != 0 ? TRUE : FALSE;

	switch (m_FilterWork->m_ProcessingType)
	{
	case PreProcessing:
		m_chProcessingType = 0;
		break;
	case PostProcessing:
		m_chProcessingType = 1;
		break;
	case AnyProcessing:
		m_chProcessingType = 2;
		break;
	}
	
	m_chResetCache = (m_FilterWork->m_Flags & FLT_A_RESETCACHE) != 0 ? TRUE : FALSE;
	m_chUseCache = (m_FilterWork->m_Flags & FLT_A_USECACHE) != 0 ? TRUE : FALSE;
	m_chSave2Cache = (m_FilterWork->m_Flags & FLT_A_SAVE2CACHE) != 0 ? TRUE : FALSE;
	m_chDropCache = (m_FilterWork->m_Flags & FLT_A_DROPCACHE) != 0 ? TRUE : FALSE;

	m_chProcessnameCacheable = (m_FilterWork->m_Flags & FLT_A_PROCESSCACHEABLE) != 0 ? TRUE : FALSE;

	m_chUserLog = m_FilterWork->m_Flags & FLT_A_USERLOG;
	m_Timeout = m_FilterWork->m_Timeout;
	
	if (m_FilterWork->m_Flags & FLT_A_POPUP) m_rFilterType = 1;
	else if (m_FilterWork->m_Flags & FLT_A_INFO) m_rFilterType = 2;
	else if (m_FilterWork->m_Flags & FLT_A_NOTIFY) m_rFilterType = 3;
	else m_rFilterType = 0;

	m_rVerdict = 0;
	if (m_FilterWork->m_Flags & FLT_A_PASS) m_rVerdict = 1;
	else if (m_FilterWork->m_Flags & FLT_A_DENY) m_rVerdict = 2;
}


void CDFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFilter)
	DDX_Control(pDX, IDC_COMBO_FUNC_MI, m_cbFuncMi);
	DDX_Control(pDX, IDC_COMBO_FUNC_MJ, m_cbFuncMj);
	DDX_Control(pDX, IDC_COMBO_HOOKID, m_cbHook);
	DDX_Control(pDX, IDC_COMBO_PROCESS, m_cbProcess);
	DDX_Control(pDX, IDC_LIST_PARAMETERS, m_lParameters);
	DDX_Check(pDX, IDC_CHECK_DONTPASS, m_chDontPass);
	DDX_Check(pDX, IDC_CHECK_DROP_CACHE, m_chDropCache);
	DDX_Check(pDX, IDC_CHECK_LOGTHIS, m_chLogThis);
	DDX_Check(pDX, IDC_CHECK_PROCESSING_TYPE, m_chProcessingType);
	DDX_Check(pDX, IDC_CHECK_RESET_CACHE, m_chResetCache);
	DDX_Check(pDX, IDC_CHECK_USE_CACHE, m_chUseCache);
	DDX_Check(pDX, IDC_CHECK_SAVE2CACHE, m_chSave2Cache);
	DDX_Check(pDX, IDC_CHECK_USERGLOG, m_chUserLog);
	DDX_Text(pDX, IDC_EDIT_REQUEST_TIMELIFE, m_Timeout);
	DDV_MinMaxInt(pDX, m_Timeout, 0, 99999);
	DDX_Radio(pDX, IDC_RADIO_FILTER_TYPE, m_rFilterType);
	DDX_Radio(pDX, IDC_RADIO_VERDICT, m_rVerdict);
	DDX_Radio(pDX, IDC_RADIO_POS_TOP, m_rPos);
	DDX_Check(pDX, IDC_CHECK_ALL_MI, m_chMiAll);
	DDX_Check(pDX, IDC_CHECK_ALL_MJ, m_chMjAll);
	DDX_Check(pDX, IDC_CHECK_TOUCH_FILTER, m_chTouchFilter);
	DDX_Text(pDX, IDC_EDIT_EXPTIME, m_eExpTime);
	DDV_MinMaxInt(pDX, m_eExpTime, 0, 9999);
	DDX_Check(pDX, IDC_CHECK_PROCESSNAME_CACHEABLE, m_chProcessnameCacheable);
	DDX_Check(pDX, IDC_CHECK_TRYTOLOCATENEXTFILTER, m_chTryNextFilter);
	DDX_Text(pDX, IDC_EDIT_FILTER_NAME, m_eFilterName);
	DDV_MaxChars(pDX, m_eFilterName, 255);
	DDX_Check(pDX, IDC_CHECK_DBG_BREAK, m_chDbgBreak);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDFilter, CDialog)
//{{AFX_MSG_MAP(CDFilter)
ON_CBN_SELCHANGE(IDC_COMBO_HOOKID, OnSelchangeComboHookid)
ON_CBN_SELCHANGE(IDC_COMBO_FUNC_MJ, OnSelchangeComboFuncMj)
ON_CBN_SELCHANGE(IDC_COMBO_FUNC_MI, OnSelchangeComboFuncMi)
ON_WM_CONTEXTMENU()
ON_BN_CLICKED(IDC_CHECK_PROCESSING_TYPE, OnCheckProcessingType)
ON_BN_CLICKED(IDC_CHECK_ALL_MJ, OnCheckAllMj)
ON_BN_CLICKED(IDC_CHECK_ALL_MI, OnCheckAllMi)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFilter message handlers

void CDFilter::FillMjList()
{
	m_cbFuncMj.ResetContent();
	
	CStringList FuncList;
	DWORD dwIdx;
	int iPos = -1;
	int CuritemPos = 0;
	
	dwIdx = m_FilterWork->m_FunctionMj;
	FillMajList(&FuncList, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, NULL, &dwIdx);
	CHAR* Func = FillMajList(NULL, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, NULL, &dwIdx);
	if (Func != NULL)
	{
		if (dwIdx != m_FilterWork->m_FunctionMj)
			m_FilterWork->m_FunctionMj = dwIdx;
	}

	FillMiList();

	CString szFunc;
	POSITION pos;
	pos = FuncList.GetHeadPosition();
	while (pos != NULL)
	{
		szFunc = FuncList.GetAt(pos);
		CuritemPos = m_cbFuncMj.AddString(szFunc);
		if (lstrcmp(szFunc, Func) == 0)
			iPos = CuritemPos;
		
		FuncList.GetNext(pos);
	}
	if (iPos == -1)
	{
		m_cbFuncMj.EnableWindow(FALSE);
	}
	else
	{
		m_cbFuncMj.SetCurSel(iPos);
		m_cbFuncMj.EnableWindow();
	}
	
	OnCheckAllMj();
}

void CDFilter::FillMiList()
{
	m_cbFuncMi.ResetContent();
	
	CStringList FuncList;
	DWORD dwIdx;
	int iPos = -1;
	int CuritemPos = 0;
	
	dwIdx = m_FilterWork->m_FunctionMi;
	FillMinList(&FuncList, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, NULL, &dwIdx);
	CHAR* Func = FillMinList(NULL, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, NULL, &dwIdx);
	if (Func != NULL)
	{
		if (dwIdx != m_FilterWork->m_FunctionMi)
			m_FilterWork->m_FunctionMi = dwIdx;
		
	}
	
	CString szFunc;
	POSITION pos;
	pos = FuncList.GetHeadPosition();
	while (pos != NULL)
	{
		szFunc = FuncList.GetAt(pos);
		CuritemPos = m_cbFuncMi.AddString(szFunc);
		if (lstrcmp(szFunc, Func) == 0)
			iPos = CuritemPos;
		
		FuncList.GetNext(pos);
	}
	if (iPos == -1)
	{
		m_cbFuncMi.EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_ALL_MI)->EnableWindow(FALSE);
	}
	else
	{
		m_cbFuncMi.SetCurSel(iPos);
		m_cbFuncMi.EnableWindow();
		GetDlgItem(IDC_CHECK_ALL_MI)->EnableWindow(TRUE);
	}
}

BOOL CDFilter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ADDITIONALSTATE AddState;
	DWORD BytesRet;

	GetDlgItem(IDC_CHECK_DBG_BREAK)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_DBG_BREAK)->ShowWindow(SW_HIDE);
	if (DeviceIoControl(g_hDrvDevice, IOCTLHOOK_GetDriverInfo, NULL, 0, &AddState, sizeof(AddState), &BytesRet, NULL))
	{
		if (AddState.DrvFlags & _DRV_FLAG_DEBUG)
		{
			GetDlgItem(IDC_CHECK_DBG_BREAK)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_DBG_BREAK)->ShowWindow(SW_SHOW);
		}
	}
		
	SetWindowText(*m_pCaption);

	if (m_pFilter != NULL)
	{
		GetDlgItem(IDC_STATIC_ADD_POS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_POS_TOP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_POS_BOTTOM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_POS_AFTER)->ShowWindow(SW_HIDE);
	}

	POSITION pos;
	int iPos = -1;
	int CuritemPos = 0;
	DWORD dwIdx;

	CStringList FuncList;
	dwIdx = m_FilterWork->m_HookID;
	FillFuncList(&FuncList, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, NULL, &dwIdx);
	CHAR *pHookID = FillFuncList(NULL, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, NULL, &dwIdx);
	if (dwIdx != m_FilterWork->m_HookID)
		m_FilterWork->m_HookID = dwIdx;

	CString szFunc;
	pos = FuncList.GetHeadPosition();
	while (pos != NULL)
	{
		szFunc = FuncList.GetAt(pos);
		CuritemPos = m_cbHook.AddString(szFunc);
		if (lstrcmp(szFunc, pHookID) == 0)
			iPos = CuritemPos;
		
		FuncList.GetNext(pos);
	}
	m_cbHook.SetCurSel(iPos);

	FillMjList();

	CString szProcess;
	CStringList ProcesList;
	CEnumProcess Enum(&ProcesList);
	Enum.EnumProcesses();

	iPos = -1;

	pos = ProcesList.GetHeadPosition();
	while (pos != NULL)
	{
		szProcess = ProcesList.GetAt(pos);
		CuritemPos = m_cbProcess.AddString(szProcess);
		if (lstrcmp(szProcess, m_FilterWork->m_ProcName) == 0)
			iPos = CuritemPos;
		
		ProcesList.GetNext(pos);
	}

	if (iPos == -1)
		iPos =m_cbProcess.AddString(m_FilterWork->m_ProcName);
	
	m_cbProcess.SetCurSel(iPos);

	RefreshParameters();

	TogglePos();
	
	GetDlgItem(IDC_CHECK_DONTPASS)->ShowWindow(SW_HIDE);
	if (g_FuncLevel > 20)
	{
		GetDlgItem(IDC_CHECK_USERGLOG)->EnableWindow(TRUE);
//		GetDlgItem(IDC_CHECK_DONTPASS)->ShowWindow(SW_SHOW);
	//	GetDlgItem(IDC_CHECK_PROCESSING_TYPE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_ALL_MJ)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_ALL_MI)->ShowWindow(SW_HIDE);
	}

	if (g_FuncLevel > 10)
	{
		GetDlgItem(IDC_CHECK_USE_CACHE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SAVE2CACHE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_DROP_CACHE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_RESET_CACHE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_EXPTIME)->EnableWindow(TRUE);
	}

	OnCheckProcessingType();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDFilter::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	m_FilterWork->m_Flags = FLT_A_DEFAULT;

	if (m_chDontPass) m_FilterWork->m_Flags |= FLT_A_DENYPASS2LOWPR;
	if (m_chLogThis) m_FilterWork->m_Flags |= FLT_A_LOG;

	switch (m_chProcessingType)
	{
	case 0:
		m_FilterWork->m_ProcessingType = PreProcessing;
		break;
	case 1:
		m_FilterWork->m_ProcessingType = PostProcessing;
		break;
	case 2:
		m_FilterWork->m_ProcessingType = AnyProcessing;
		break;
	}
	
	if (m_chResetCache) m_FilterWork->m_Flags |= FLT_A_RESETCACHE;
	if (m_chUseCache) m_FilterWork->m_Flags |= FLT_A_USECACHE;
	if (m_chSave2Cache) m_FilterWork->m_Flags |= FLT_A_SAVE2CACHE;
	if (m_chDropCache) m_FilterWork->m_Flags |= FLT_A_DROPCACHE;
	if (m_chUserLog) m_FilterWork->m_Flags |= FLT_A_USERLOG;
	if (m_chTouchFilter) m_FilterWork->m_Flags |= FLT_A_TOUCH;
	if (m_chTryNextFilter) m_FilterWork->m_Flags |= FLT_A_CHECKNEXTFILTER;
	if (m_chDbgBreak) m_FilterWork->m_Flags |= FLT_A_DBG_BREAK;

	if (m_chProcessnameCacheable) m_FilterWork->m_Flags |= FLT_A_PROCESSCACHEABLE;

	m_FilterWork->m_Timeout = m_Timeout;

	switch (m_rFilterType)
	{
	case 0:
		break;
	case 1:
		m_FilterWork->m_Flags |= FLT_A_POPUP;
		break;
	case 2:
		m_FilterWork->m_Flags |= FLT_A_INFO;
		break;
	case 3:
		m_FilterWork->m_Flags |= FLT_A_NOTIFY;
		break;
	}
	
	switch (m_rVerdict)
	{
	case 0:
		m_FilterWork->m_Flags |= FLT_A_DEFAULT;
		break;
	case 1:
		m_FilterWork->m_Flags |= FLT_A_PASS;
		break;
	case 2:
		m_FilterWork->m_Flags |= FLT_A_DENY;
		break;
	}
	
	CString Process = "*";
	m_cbProcess.GetWindowText(Process);
	lstrcpy(m_FilterWork->m_ProcName, Process);
	
	ULARGE_INTEGER		ExpTime;
	FILETIME				STasFT;
	
	if (m_eExpTime != NULL)
	{
		GetSystemTimeAsFileTime(&STasFT);
		ExpTime.LowPart=STasFT.dwLowDateTime;
		ExpTime.HighPart=STasFT.dwHighDateTime;
		STasFT.dwLowDateTime = m_eExpTime;
		ExpTime.QuadPart = ExpTime.QuadPart + (LONGLONG)STasFT.dwLowDateTime*600000000;
		m_FilterWork->m_Expiration = ExpTime.QuadPart;
	}
	else
		m_FilterWork->m_Expiration = 0;
	
	if (m_chMjAll == TRUE)
	{
		m_FilterWork->m_FunctionMj = -1;
		m_FilterWork->m_FunctionMi = -1;
	}
	if (m_chMiAll == TRUE)
		m_FilterWork->m_FunctionMi = -1;
	
	if (!m_eFilterName.IsEmpty())
	{
		BYTE buffer[sizeof(FILTER_PARAM) + MAX_PATH + 1];
		PFILTER_PARAM pFilterName = (PFILTER_PARAM) buffer;
		LPTSTR name = m_eFilterName.GetBuffer(MAX_PATH);

		pFilterName->m_ParamFlags = _FILTER_PARAM_FLAG_MAP_TO_EVENT;
		pFilterName->m_ParamFlt = FLT_PARAM_NOP;
		pFilterName->m_ParamID = _PARAM_USER_FILTER_NAME;
		pFilterName->m_ParamSize = lstrlen(name) + 1;
		lstrcpy((LPSTR) pFilterName->m_ParamValue, name);

		AddFilterParamValue(m_FilterWork, pFilterName);

		m_eFilterName.ReleaseBuffer();
	}
	
	CDialog::OnOK();
}

void CDFilter::OnSelchangeComboHookid() 
{
	// TODO: Add your control notification handler code here
	int iPos = m_cbHook.GetCurSel();
	if (iPos != CB_ERR)
	{
		CHAR* pstr;
		DWORD dwIdx;
		CString szFunc;
		m_cbHook.GetLBText(iPos, szFunc);
		pstr = (LPSTR)(LPCSTR) szFunc;
		FillFuncList(NULL, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, pstr, &dwIdx);
		m_FilterWork->m_HookID = dwIdx;
		m_FilterWork->m_FunctionMj = 0;
		m_FilterWork->m_FunctionMi = 0;
		
		FillMjList();
		
		TogglePos();
	}
}

void CDFilter::OnSelchangeComboFuncMj() 
{
	// TODO: Add your control notification handler code here
	int iPos = m_cbFuncMj.GetCurSel();
	if (iPos != CB_ERR)
	{
		CHAR* pstr;
		DWORD dwIdx;
		CString szFunc;
		m_cbFuncMj.GetLBText(iPos, szFunc);
		pstr = (LPSTR)(LPCSTR) szFunc;
		FillMajList(NULL, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, pstr, &dwIdx);
		m_FilterWork->m_FunctionMj = dwIdx;
		m_FilterWork->m_FunctionMi = 0;

		TogglePos();

		FillMiList();
	}	
}

void CDFilter::OnSelchangeComboFuncMi() 
{
	// TODO: Add your control notification handler code here
	int iPos = m_cbFuncMi.GetCurSel();
	if (iPos != CB_ERR)
	{
		CHAR* pstr;
		DWORD dwIdx;
		CString szFunc;
		m_cbFuncMi.GetLBText(iPos, szFunc);
		pstr = (LPSTR)(LPCSTR) szFunc;
		FillMinList(NULL, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, pstr, &dwIdx);
		m_FilterWork->m_FunctionMi = dwIdx;

		TogglePos();
	}
}

void CDFilter::RefreshParameters()
{
	int iPos = 0;
	CString description;
	m_lParameters.DeleteAllItems();

	PFILTER_PARAM pSingleParam;
	pSingleParam = (PFILTER_PARAM) m_FilterWork->m_Params;
	for (ULONG cou = 0; cou < m_FilterWork->m_ParamsCount; cou++)
	{
		GetParameterDescription(pSingleParam, &description);
		if (pSingleParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT)
			description = "Mapped: " + description;
		m_lParameters.InsertItem(iPos, description);

		pSingleParam = (PFILTER_PARAM)((BYTE*)pSingleParam + sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);
	}
}

void CDFilter::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	if (pWnd->m_hWnd != m_lParameters.m_hWnd)
		return;

	HMENU hSysMenu;
	
	hSysMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_PARAMETERS));
	if (hSysMenu != NULL)
	{
		POINT pt;
		HMENU hSubMenu;
		CWnd* pForeground = GetForegroundWindow();

		GetCursorPos(&pt);
		SetForegroundWindow();
		hSubMenu = GetSubMenu(hSysMenu,0);
		if (hSubMenu != NULL)
		{
			int SelCount = m_lParameters.GetSelectedCount();
			if (SelCount != 1)
				EnableMenuItem(hSubMenu, IDMI_DEL_CURRENT_PARAMETER, MF_BYCOMMAND | MF_GRAYED);
				
			int nSelection = TrackPopupMenu(hSubMenu,TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x,pt.y, 0, this->m_hWnd,NULL);
			switch (nSelection)
			{
			case IDMI_ADDNEW_PARAMETER:
				{
					CDParameter DParameter(NULL, this);
					if (DParameter.DoModal() == IDOK)
						AddFilterParamValue(m_FilterWork, DParameter.m_pSingleParamWork);
				}
				break;
			case IDMI_DEL_CURRENT_PARAMETER:
				{
					POSITION pos = m_lParameters.GetFirstSelectedItemPosition();
					if (pos != NULL)
					{
						int nItem = m_lParameters.GetNextSelectedItem(pos);
						DelFilterParamValue(m_FilterWork, nItem);
					}
				}
				break;			
			}
			RefreshParameters();
		}
		
		DestroyMenu(hSysMenu);
		
		if (pForeground != NULL)
			pForeground->SetForegroundWindow();
	}
}

void CDFilter::SetAddPos(ULONG HookID, ULONG FuncMj, ULONG FuncMi)
{
	m_AddPos_HookID = HookID;
	m_AddPos_FuncMj = FuncMj;
	m_AddPos_FuncMi = FuncMi;
}

void CDFilter::TogglePos(void)
{
	if ((m_FilterWork->m_HookID == m_AddPos_HookID) && (m_FilterWork->m_FunctionMj == m_AddPos_FuncMj) && (m_FilterWork->m_FunctionMi == m_AddPos_FuncMi))
		GetDlgItem(IDC_RADIO_POS_AFTER)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_RADIO_POS_AFTER)->EnableWindow(FALSE);

	DWORD dwIdx = m_FilterWork->m_HookID;
	CString strtmp;
	strtmp.Format("%d (%#x)", dwIdx, dwIdx);
	SetDlgItemText(IDC_STATIC_HOOKID, strtmp);

	dwIdx = m_FilterWork->m_FunctionMj;
	if (m_chMjAll)
		dwIdx = -1;
	strtmp.Format("%d (%#x)", dwIdx, dwIdx);
	SetDlgItemText(IDC_STATIC_MJ, strtmp);
	
	dwIdx = m_FilterWork->m_FunctionMi;
	if (m_chMiAll)
		dwIdx = -1;
	strtmp.Format("%d (%#x)", dwIdx, dwIdx);
	SetDlgItemText(IDC_STATIC_MI, strtmp);
}

void CDFilter::OnCheckProcessingType()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	switch (m_chProcessingType)
	{
	case 0:
		SetDlgItemText(IDC_CHECK_PROCESSING_TYPE, "Pre");
		break;
	case 1:
		SetDlgItemText(IDC_CHECK_PROCESSING_TYPE, "Post");
		break;
	case 2:
		SetDlgItemText(IDC_CHECK_PROCESSING_TYPE, "Both");
		break;
	}
}

void CDFilter::OnCheckAllMj() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_chMjAll == TRUE)
	{
		m_cbFuncMj.EnableWindow(FALSE);
		m_cbFuncMi.EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_ALL_MI)->EnableWindow(FALSE);
		m_chMiAll = TRUE;
		UpdateData(FALSE);
	}
	else
	{
		m_cbFuncMj.EnableWindow(TRUE);
		
		DWORD dwIdx;
		CHAR* Func = FillMinList(NULL, m_FilterWork->m_HookID, m_FilterWork->m_FunctionMj, m_FilterWork->m_FunctionMi, NULL, &dwIdx);
		if (Func != NULL)
		{
			GetDlgItem(IDC_CHECK_ALL_MI)->EnableWindow(TRUE);
			m_cbFuncMi.EnableWindow(TRUE);
		}

		m_chMiAll = FALSE;
		UpdateData(FALSE);
	}	

	TogglePos();
}

void CDFilter::OnCheckAllMi() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_chMiAll == TRUE)
		m_cbFuncMi.EnableWindow(FALSE);
	else
		m_cbFuncMi.EnableWindow(TRUE);

	TogglePos();
}

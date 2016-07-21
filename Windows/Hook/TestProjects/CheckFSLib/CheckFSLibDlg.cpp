// CheckFSLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CheckFSLib.h"
#include "CheckFSLibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\..\..\hook\hook\avpgcom.h"

#include "..\..\..\hook\hook\FSDrvLib.h"
#include "..\..\..\hook\hook\IDriver.h"
#include "..\..\..\hook\hook\HookSpec.h"
#include "..\..\..\hook\hook\Funcs.h"

DWORD EvExit = 0;
DWORD gAppID = 0;
long g_Mark = 0;

#ifdef _UNICODE
#define _ASNI_STR	_T("%S"
#define _UNI_STR	_T("%s"
#else
#define _ASNI_STR	_T("%s"
#define _UNI_STR	_T("%S"
#endif

#define _MAXPATHLEN__	2048
#define _SAME_PARAM_ID	1001

/////////////////////////////////////////////////////////////////////////////
// CCheckFSLibDlg dialog

CCheckFSLibDlg::CCheckFSLibDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCheckFSLibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckFSLibDlg)
	m_chStatus = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCheckFSLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckFSLibDlg)
	DDX_Control(pDX, IDC_BUTTON_ADDPOPUPFILTER, m_bPopup);
	DDX_Control(pDX, IDC_BUTTON_ADDFILTER, m_bAddFilter);
	DDX_Control(pDX, IDC_BUTTON_ADDINFOFILTER, m_bInfo);
	DDX_Check(pDX, IDC_CHECK_STATUS, m_chStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCheckFSLibDlg, CDialog)
	//{{AFX_MSG_MAP(CCheckFSLibDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_CHECK_STATUS, OnCheckStatus)
	ON_BN_CLICKED(IDC_BUTTON_ADDFILTER, OnButtonAddfilter)
	ON_BN_CLICKED(IDC_BUTTON_ADDINFOFILTER, OnButtonAddinfofilter)
	ON_BN_CLICKED(IDC_BUTTON_ADDPOPUPFILTER, OnButtonAddpopupfilter)
	ON_BN_CLICKED(IDC_BUTTON_R3, OnButtonR3)
	ON_BN_CLICKED(IDC_BUTTON_SETWRITEDETECTFILTER, OnButtonSetwritedetectfilter)
	ON_BN_CLICKED(IDC_BUTTON_DEL_THE_SAME, OnButtonDelTheSame)
	ON_BN_CLICKED(IDC_BUTTON_MASK_LIST, OnButtonMaskList)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_PARAM, OnButtonChangeParam)
	ON_BN_CLICKED(IDC_BUTTON_RESET_TRACE, OnButtonResetTrace)
	ON_BN_CLICKED(IDC_BUTTON_GETFILESTAT, OnButtonGetfilestat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckFSLibDlg message handlers

BOOL CCheckFSLibDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	m_PopupFilterId = _INVALID_FILTER_ID;

	if (FSDrvInterceptorInitEx(_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_USE_DRIVER_CACHE | _CLIENT_FLAG_WITHOUTCASCADE, 
		AVPG_Driver_Specific, AVPG_STANDARTRIORITY, 100) == FALSE)
		SetWindowText(_T("CheckFSLib...Failed to start"));
	else
	{
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);	
	/*		DWORD BytesRet;
	COMMON_MEMORY InMem;
	COMMON_MEMORY OutMem;
	InMem.m_AppID = FSDrvGetAppID();
	InMem.m_BaseAdress = 0;
		InMem.m_Size = 4096;*/
		
		gAppID = FSDrvGetAppID();
		
		FSDrvInterceptorSetActive(TRUE);
		FSDrvInterceptorSetActive(FALSE);
		
		//DeviceIoControl(FSDrvGetDeviceHandle(), IOCTLHOOK_COMMONMEMORY, &InMem, sizeof(InMem), &OutMem, sizeof(OutMem), &BytesRet, NULL);
	}

	GetDlgItem(IDC_BUTTON_CHANGE_PARAM)->EnableWindow(FALSE);

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCheckFSLibDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCheckFSLibDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CCheckFSLibDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	FSDrvInterceptorDone();
	
	return CDialog::DestroyWindow();
}

void CCheckFSLibDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	if (FSDrvInterceptorRegisterInvisibleThread() == FALSE)
	{
		SetWindowText(_T("CheckFSLib... Add failed"));
	}
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(TRUE);
}

void CCheckFSLibDlg::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(TRUE);

	FSDrvInterceptorUnregisterInvisibleThread();
	SetWindowText(_T("CheckFSLib...ready"));
}

void CCheckFSLibDlg::OnCheckStatus() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	FSDrvInterceptorSetActive(m_chStatus);
}

void CCheckFSLibDlg::OnButtonAddfilter() 
{
	// TODO: Add your control notification handler code here
	static bool bOnButtonAddfilter = false;
	if (bOnButtonAddfilter == false)
	{
		bOnButtonAddfilter = true;
		OutputDebugString(_T("OnButton add filter\n"));
		
		m_bAddFilter.EnableWindow(FALSE);
#ifdef _DEBUG
		AddFSFilter(FSDrvGetDeviceHandle(), gAppID, "*.exe", "*", 0, FLT_A_NOTIFY, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL); 
#endif
	}
}

typedef struct _THREAD_PARAM
{
	GET_EVENT GetEvent;
	SET_VERDICT SetVerdict;
	BYTE bEventData[sizeof(EVENT_TRANSMIT) + (MAX_PATH + 1) * 2];
}THREAD_PARAM, *PTHREAD_PARAM;

// Function name	: GetEventData
// Description	    : get event data  from driver
// Return type		: BOOL  - TRUE if retrieving complete
// Argument         : PTHREAD_PARAM pThreadParam
PSINGLE_PARAM GetEventParam(PEVENT_TRANSMIT pEvent, DWORD ParamID)
{
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEvent->m_Params;
	for (DWORD cou =  0; cou < pEvent->m_ParamsCount; cou++)
	{
	/*wsprintf(dbgmsg, "Parametr %d ID: %x\n", cou, pSingleParam->ParamID);
		OutputDebugString(dbgmsg);*/
		
		if (pSingleParam->ParamID == ParamID)
			return pSingleParam;
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}
	return NULL;
}

BOOL GetEventData(PTHREAD_PARAM pThreadParam)
{
#ifdef _DEBUG
	DWORD BytesRet;
	DWORD BufferSize;
	
	pThreadParam->GetEvent.m_AppID = gAppID;
	
	// -----------------------------------------------------------------------------------------
	pThreadParam->GetEvent.Mark = InterlockedIncrement(&g_Mark); 
	BufferSize = sizeof(pThreadParam->bEventData);
	// -----------------------------------------------------------------------------------------
	
	if(!DeviceIoControl(FSDrvGetDeviceHandle(), IOCTLHOOK_GetEvent, &(pThreadParam->GetEvent), sizeof(GET_EVENT), &(pThreadParam->bEventData), BufferSize, &BytesRet, NULL))
	{
		OutputDebugString(_T("GetEventData: GetEvent failed!\n"));
	}
	else
	{
		if(BytesRet == 0)
		{
			TCHAR mark[MAX_PATH];
			wsprintf(mark, _T("i_avpg: Data has very big size! (buffer size %d)\n"), BufferSize);
			OutputDebugString(mark);
		}
		return TRUE;
	}
#endif
	return FALSE;
}

DWORD WINAPI EventWaiterThread(void* pParam)
{
	unsigned long EventCount = 0;
	HANDLE hWhistle = FSDrvGetWhistlerUpHandle();
	THREAD_PARAM ThreadParam;
	
	DWORD BytesRet;
	HDSTATE Activity;
	SET_VERDICT SetVerdict;
	
	TCHAR dbgmsg[_MAXPATHLEN__];
	TCHAR dbgmsg2[_MAXPATHLEN__];
	wsprintf(dbgmsg, _T("EventWaiterThreadID: %x\n"), GetCurrentThreadId());
	OutputDebugString(dbgmsg);
	
	OutputDebugString(_T("CheckFSLib: Set thread priority to crititcal\n"));
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	
	OutputDebugString(_T("CheckFSLib: Wait events\n"));
	while (WaitForSingleObject(hWhistle, INFINITE) == WAIT_OBJECT_0) 
	{
		if (EvExit != 0)
		{
			OutputDebugString(_T("CheckFSLib: exit\n"));
			break;
		}
		//OutputDebugString("i_avpg: Event signaled\n");
		Activity.Activity = _AS_DontChange;
		Activity.AppID = gAppID;
		Activity.QLen = 0;
		Activity.QUnmarkedLen = 0;
		if (!DeviceIoControl (FSDrvGetDeviceHandle(), IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
			Activity.QUnmarkedLen = 1;
		
		while (Activity.QUnmarkedLen > 0) 
		{
			Activity.QUnmarkedLen--;
			if (GetEventData(&ThreadParam) == FALSE)
			{
				//Activity.QUnmarkedLen = 0;	//!!
			}
			else
			{
				PSINGLE_PARAM pSingleParam;
				PSINGLE_PARAM pSingleParamDest;
				PEVENT_TRANSMIT pEvT = (PEVENT_TRANSMIT) ThreadParam.bEventData;
				EventCount++;
				
				if (!(pEvT->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
				{
					if (GetEventParam(pEvT, _SAME_PARAM_ID) != NULL)
					{
						EVENT_OBJECT_REQUEST Request;
						ULONG RetSize = 0;
						
						ZeroMemory(&Request, sizeof(Request));
						
						Request.m_AppID = gAppID;
						Request.m_Mark = ThreadParam.GetEvent.Mark;
						Request.m_RequestType = _event_delsame_in_queue_by_cacheid;
						
						IDriverEventObjectRequest(FSDrvGetDeviceHandle(), &Request, NULL, &RetSize);
					}
					
					SetVerdict.m_Verdict = Verdict_Pass;
					SetVerdict.m_AppID = gAppID;
					SetVerdict.m_Mark = ThreadParam.GetEvent.Mark;
					SetVerdict.m_ExpTime = 0;
					
					IDriverSetVerdict2(FSDrvGetDeviceHandle(), &SetVerdict);
				}
				
				pSingleParam = GetEventParam(pEvT, _PARAM_OBJECT_URL_W);
				pSingleParamDest = GetEventParam(pEvT, _PARAM_OBJECT_URL_DEST_W);
				
				wsprintf(dbgmsg, _T("Client %#8x Event number %u%s. Object:Url ")_ASNI_STR":_T(")_UNI_STR"). Number of parameters: %d\n"), gAppID, EventCount, 
					(pEvT->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK) == 0 ? _T("") : _T("(deleted on mark)"),
					pEvT->m_ProcName, pSingleParam->ParamValue, pEvT->m_ParamsCount);
				if (pSingleParamDest != NULL)
				{
					wsprintf(dbgmsg2, _T("Destination")_UNI_STR)"", pSingleParamDest->ParamValue);
					lstrcat(dbgmsg, dbgmsg2);
				}
				
				OutputDebugString(dbgmsg);
				
				pSingleParam = GetEventParam(pEvT, _PARAM_OBJECT_SID);
				if (!pSingleParam)
					OutputDebugStringA("\n");
				else
				{
					PSID   pUserSid;
					char Domain[MAX_PATH];
					char User[MAX_PATH];
					DWORD cbName, cbRefDomainName;
					SID_NAME_USE _SidNameUse;
					
					cbRefDomainName = MAX_PATH;
					cbName = MAX_PATH;
					_SidNameUse = (SID_NAME_USE)(SidTypeUser - 1);
					
					ZeroMemory(Domain, MAX_PATH);
					ZeroMemory(User, MAX_PATH);
					
					pUserSid = pSingleParam->ParamValue;
					if(LookupAccountSidA(NULL, pUserSid, User, &cbName, Domain, &cbRefDomainName, &_SidNameUse) != 0)
					{
						wsprintfA((PCHAR) dbgmsg, "Domain: '%s'. User '%s'\n", Domain, User);
						OutputDebugStringA((PCHAR) dbgmsg);
					}
				}
			}
		}
	}
	
	return 0;
}

void CCheckFSLibDlg::OnButtonAddinfofilter() 
{
	// TODO: Add your control notification handler code here
	static bool bOnButtonAddinfofilter = false;
	if (bOnButtonAddinfofilter == false)
	{
		bOnButtonAddinfofilter = true;
		DWORD threadid;
		m_bInfo.EnableWindow(FALSE);
		
		OutputDebugString(_T("OnButton add info filter\n"));
		
		CreateThread(NULL, 0, EventWaiterThread, NULL, 0, &threadid);
		
		AddFSFilter(FSDrvGetDeviceHandle(), gAppID, "*.ttt", "*", 0, FLT_A_INFO | FLT_A_DELETE_ON_MARK, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL); 
		AddFSFilter(FSDrvGetDeviceHandle(), gAppID, "*.ttt", "*", 0, FLT_A_INFO | FLT_A_DELETE_ON_MARK, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL); 
	}
}

void CCheckFSLibDlg::OnButtonAddpopupfilter() 
{
	// TODO: Add your control notification handler code here
	static bool bOnButtonAddpopupfilter = false;
	if (bOnButtonAddpopupfilter == false)
	{
		HANDLE hDevice = FSDrvGetDeviceHandle();
		bOnButtonAddpopupfilter = true;
		DWORD threadid;
		m_bPopup.EnableWindow(FALSE);
		gAppID = FSDrvGetAppID();
		
		OutputDebugString(_T("OnButton add info filter\n"));
		
		CreateThread(NULL, 0, EventWaiterThread, NULL, 0, &threadid);
		
		m_PopupFilterId = AddFSFilter(FSDrvGetDeviceHandle(), gAppID, "*.TTT", "*", 0, FLT_A_POPUP /*| FLT_A_CHECKNEXTFILTER*/, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL);

//		AddFSFilter(FSDrvGetDeviceHandle(), gAppID, "*.TTT", "*", 0, FLT_A_POPUP /*| FLT_A_CHECKNEXTFILTER*/, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL); 

		if (_INVALID_FILTER_ID != m_PopupFilterId)	
			GetDlgItem(IDC_BUTTON_CHANGE_PARAM)->EnableWindow(TRUE);
	}	
}

void CCheckFSLibDlg::OnButtonR3() 
{
	// TODO: Add your control notification handler code here
	ULONG index;
	CHAR FName[MAX_PATH];
	CHAR SecondName[MAX_PATH];
	
	CHAR ProcName[PROCNAMELEN];
	VERDICT Verdict;
	DWORD BytesRet;
	PSINGLE_PARAM pSingleParam;
	
	HANDLE hDevice = FSDrvGetDeviceHandle();
	
	PBYTE			pByte;
	DWORD			tmp;
	
	BYTE RequestData[sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + MAXPATHLEN * 2 + 1];
	PFILTER_EVENT_PARAM pParam = (PFILTER_EVENT_PARAM)RequestData;
	
	pParam->ThreadID = 0;
	// -----------------------------------------------------------------------------------------
	lstrcpyA(ProcName, "FSDrvLib.exe");
	lstrcpyA(FName, "c:\\command.com");
	index = 22;
	SecondName[0] = 0;
	// -----------------------------------------------------------------------------------------
	
	//DbgPrint(8,T("HOOK-3: %s %s %s %s\n", ProcName, T("R3HTable[index].FuncName", FName, SecondName);
	pParam->HookID = FLTTYPE_R3;
	pParam->FunctionMj = index;
	pParam->FunctionMi = 0;
	pParam->ProcessingType = PreProcessing;
	pParam->ProcName[0] = 0;
	pParam->ThreadID = 0;
	pParam->UnsafeValue = 0;
	
	pSingleParam = (PSINGLE_PARAM) pParam->Params;
	pSingleParam->ParamID = _PARAM_OBJECT_URL;
	pSingleParam->ParamSize = strlen(FName) + 1;
	memcpy(pSingleParam->ParamValue, FName, pSingleParam->ParamSize);
	if(SecondName!=NULL) {
		tmp = sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;
		pByte = (BYTE*)pSingleParam + tmp;
		pSingleParam = (PSINGLE_PARAM) pByte;
		
		pSingleParam->ParamID = _PARAM_OBJECT_URL_DEST;
		pSingleParam->ParamSize = strlen(SecondName) + 1;
		memcpy(pSingleParam->ParamValue, FName, pSingleParam->ParamSize);
		
		pParam->ParamsCount = 2;
	} else {
		pParam->ParamsCount = 1;
	}
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		if(DeviceIoControl(hDevice, IOCTLHOOK_FilterEvent, RequestData, sizeof(RequestData), &Verdict, sizeof(Verdict), &BytesRet, NULL)) {
			if(Verdict != Verdict_Pass) {
				if(Verdict == Verdict_Kill) {
					//DbgPrint(4, T("HOOK: Kill %s\n", pParam->ProcName?pParam->ProcName:"");
				} else if(Verdict == Verdict_Discard) {
					//DbgPrint(4, T("HOOK: Discard %s - %s %s\n", pParam->ProcName?pParam->ProcName:"", FName, SecondName?SecondName:"");
				}
			}
		} else {
			//DbgPrint(2, T("HOOK: deviceIoControl failed in R3 hook\n");
		}
	} else {
		//DbgPrint(2, T("HOOK: Can't open driver in R3 hook\n");
	}
}


void CCheckFSLibDlg::OnButtonSetwritedetectfilter() 
{
	// TODO: Add your control notification handler code here
	
	DWORD threadid;
	WCHAR root[MAX_PATH];
	
	lstrcpyW(root, L"*");
	
	OutputDebugString(_T("OnButton add write-detect filter\n"));
	
	CreateThread(NULL, 0, EventWaiterThread, NULL, 0, &threadid);
	
	DWORD DeadlockTimeout = DEADLOCKWDOG_TIMEOUT;
	HANDLE hDevice = FSDrvGetDeviceHandle();
	ULONG AppID = gAppID;
	
	DWORD dwFlags;
	dwFlags = FLT_A_INFO | FLT_A_DELETE_ON_MARK;
	// close
	{
		BYTE Buf1[sizeof(FILTER_PARAM) + _MAXPATHLEN__ + 1];
		BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(DWORD)];
		PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
		PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
		
		Param1->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param1->m_ParamFlt = FLT_PARAM_WILDCARD;
		Param1->m_ParamID = _PARAM_OBJECT_URL_W;
		lstrcpynW((PWCHAR) Param1->m_ParamValue, root, _MAXPATHLEN__);
		Param1->m_ParamSize = (lstrlenW((PWCHAR) Param1->m_ParamValue) + 1) * 2;
		
		Param2->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param2->m_ParamFlt = FLT_PARAM_AND;
		Param2->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
		*(DWORD*)Param2->m_ParamValue = 1;
		Param2->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, 
			dwFlags, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PreProcessing, NULL, Param1, Param2, NULL);
	}	
	
	// rename - info, drop dest cache
	{
		BYTE Buf1[sizeof(FILTER_PARAM) + _MAXPATHLEN__ + 1];
		BYTE Buf2[sizeof(FILTER_PARAM) + _MAXPATHLEN__ + 1];
		PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
		PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
		
		Param1->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param1->m_ParamFlt = FLT_PARAM_NOP;
		Param1->m_ParamID = _PARAM_OBJECT_URL_W;
		lstrcpynW((PWCHAR) Param1->m_ParamValue, root, _MAXPATHLEN__);
		Param1->m_ParamSize = (lstrlenW((PWCHAR) Param1->m_ParamValue) + 1) * 2;
		
		Param2->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param2->m_ParamFlt = FLT_PARAM_WILDCARD;
		Param2->m_ParamID = _PARAM_OBJECT_URL_DEST_W;
		lstrcpynW((PWCHAR) Param2->m_ParamValue, root, _MAXPATHLEN__);
		Param2->m_ParamSize = (lstrlenW((PWCHAR) Param1->m_ParamValue) + 1) * 2;
		
		AddFSFilter2(hDevice, AppID, "*", DeadlockTimeout, dwFlags, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing, NULL, Param1, Param2, NULL);
	}
	
	// rename - info
	{
		BYTE Buf1[sizeof(FILTER_PARAM) + _MAXPATHLEN__ + 1];
		BYTE Buf2[sizeof(FILTER_PARAM) + _MAXPATHLEN__ + 1];
		PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
		PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
		
		Param1->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param1->m_ParamFlt = FLT_PARAM_WILDCARD;
		Param1->m_ParamID = _PARAM_OBJECT_URL_W;
		lstrcpynW((PWCHAR) Param1->m_ParamValue, root, _MAXPATHLEN__);
		Param1->m_ParamSize = (lstrlenW((PWCHAR) Param1->m_ParamValue) + 1) * 2;
		
		Param2->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param2->m_ParamFlt = FLT_PARAM_NOP;
		Param2->m_ParamID = _PARAM_OBJECT_URL_DEST_W;
		lstrcpynW((PWCHAR) Param2->m_ParamValue, root, _MAXPATHLEN__);
		Param2->m_ParamSize = (lstrlenW((PWCHAR) Param1->m_ParamValue) + 1) * 2;
		
		AddFSFilter2(hDevice, AppID, "*", DeadlockTimeout, dwFlags, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing, NULL, Param1, Param2, NULL);
	}
	
	//delete
	{
		BYTE Buf1[sizeof(FILTER_PARAM) + _MAXPATHLEN__ + 1];
		PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
		Param1->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param1->m_ParamFlt = FLT_PARAM_WILDCARD;
		Param1->m_ParamID = _PARAM_OBJECT_URL_W;
		lstrcpynW((PWCHAR) Param1->m_ParamValue, root, _MAXPATHLEN__);
		Param1->m_ParamSize = (lstrlenW((PWCHAR) Param1->m_ParamValue) + 1) * 2;
		
		AddFSFilter2(hDevice, AppID, "*", DeadlockTimeout, dwFlags, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileDispositionInformation, 0, PreProcessing, NULL, Param1, NULL);
	}
	
	//create
	{
		AddFSFilter2(hDevice, AppID, "*", DeadlockTimeout, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL, NULL);
	}
	// write
	{
		AddFSFilter2(hDevice, AppID, "*", DeadlockTimeout, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_WRITE, 0, 0, PreProcessing, NULL, NULL);
	}
}


void CCheckFSLibDlg::OnButtonDelTheSame() 
{
	// TODO: Add your control notification handler code here
	DWORD threadid;
	CreateThread(NULL, 0, EventWaiterThread, NULL, 0, &threadid);
	
	DWORD DeadlockTimeout = DEADLOCKWDOG_TIMEOUT;
	HANDLE hDevice = FSDrvGetDeviceHandle();
	ULONG AppID = gAppID;
	
	{
#define _BIN_PARAM_SIZE sizeof(DWORD)
		BYTE Buf1[sizeof(FILTER_PARAM) + _BIN_PARAM_SIZE];
		PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
		
		Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MAP_TO_EVENT | _FILTER_PARAM_FLAG_CACHABLE;
		Param1->m_ParamFlt = FLT_PARAM_NOP;
		Param1->m_ParamID = _SAME_PARAM_ID;
		Param1->m_ParamSize = _BIN_PARAM_SIZE;
		*(DWORD*)Param1->m_ParamValue = 1;
		
		AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_NFIOR, IRP_MJ_CREATE, -1, 0, PreProcessing, NULL, Param1, NULL);
	}	
	
}

void CCheckFSLibDlg::OnButtonMaskList() 
{
	// TODO: Add your control notification handler code here
	DWORD threadid;
	CreateThread(NULL, 0, EventWaiterThread, NULL, 0, &threadid);
	
	DWORD DeadlockTimeout = DEADLOCKWDOG_TIMEOUT;
	HANDLE hDevice = FSDrvGetDeviceHandle();
	{
		BYTE Buf1[sizeof(FILTER_PARAM) + MAX_PATH];
		PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
		ULONG TotalSize = 0;
		ULONG tmplen;
		WCHAR* ptr;
		
		Param1->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
		Param1->m_ParamFlt = /*FLT_PARAM_MASKUNSENS_LIST*/FLT_PARAM_MASK_LIST;
		Param1->m_ParamID = _PARAM_OBJECT_URL_W;
		
		ptr = (PWCHAR) Param1->m_ParamValue;
		
		lstrcpyW(ptr, L"*.txt");
		tmplen = (lstrlenW(ptr) + 1);
		ptr += tmplen;
		TotalSize += tmplen * sizeof(WCHAR);
		
		lstrcpyW(ptr, L"*.lng");
		tmplen = (lstrlenW(ptr) + 1);
		ptr += tmplen;
		TotalSize += tmplen * sizeof(WCHAR);
		
		lstrcpyW(ptr, L"\0");
		tmplen = (lstrlenW(ptr) + 1);
		TotalSize += tmplen * sizeof(WCHAR);
		
		Param1->m_ParamSize = TotalSize;
		
		AddFSFilter2(hDevice, gAppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_NFIOR, IRP_MJ_CREATE, -1, 0, PreProcessing, NULL, Param1, NULL);
	}	
}

void CCheckFSLibDlg::OnButtonChangeParam() 
{
	// TODO: Add your control notification handler code here
	BYTE Buf[0x1000];
	memset(Buf, 0, sizeof(Buf));

	PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) Buf;

	pFlt->m_FltCtl = FLTCTL_CHANGE_FILTER_PARAM;
	pFlt->m_AppID = gAppID;
	pFlt->m_ParamsCount = 1;
	pFlt->m_FilterID = m_PopupFilterId;

	PFILTER_PARAM pParam = (PFILTER_PARAM) pFlt->m_Params;

	pParam->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG;
	pParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	pParam->m_ParamID = _PARAM_OBJECT_URL;
	pParam->m_ParamSize = lstrlen("*.TT1") + 1;
	lstrcpyn((PCHAR) pParam->m_ParamValue, "*.TT1", MAXPATHLEN);

	HANDLE hDevice = FSDrvGetDeviceHandle();
	IDriverFilterTransmit(hDevice, pFlt, NULL, sizeof(Buf), 0);
}

void CCheckFSLibDlg::OnButtonResetTrace() 
{
	// TODO: Add your control notification handler code here
	ULONG debcat = (ULONG) -1;
	ULONG deblev = 3;

	HANDLE hDevice = FSDrvGetDeviceHandle();
	IDriverChangeDebugInfoLevel(hDevice, deblev);
	IDriverChangeDebugInfoMask(hDevice, debcat);
}

void CCheckFSLibDlg::OnButtonGetfilestat() 
{
	// TODO: Add your control notification handler code here
	//FILESYSTEM_STATISTICS	
}

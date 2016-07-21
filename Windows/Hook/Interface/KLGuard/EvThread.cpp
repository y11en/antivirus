#include "stdafx.h"

#define __ONLY_PARAM_DESCR

extern void* _MM_Alloc(unsigned int nSize);
extern void _MM_Free(void* p);

#include "..\IGuard\Params.h"
#include "..\IGuard\Params.cpp"


#include "EvThread.h"
#include "resource.h"

// -----------------------------------------------------------------------------------------
// internal data
LONG EvExit = 0;
LONG g_Mark = 1;

int CreateEventInfoDlg(HWND hWnd, PEVENT_TRANSMIT pEvt)
{
	int Result = 0;
	HMODULE hInfoModule = LoadLibrary("IGuard.dll");
	if (hInfoModule != NULL)
	{
		FARPROC infoproc = GetProcAddress(hInfoModule, "EventInfoProc");
		if (infoproc != NULL)
		{
			OutputDebugString("Show info interface\n");
			Result = ((int(*)(HWND, PEVENT_TRANSMIT))infoproc)(hWnd, pEvt);
			OutputDebugString("Hide info interface\n");
		}
		FreeLibrary(hInfoModule);
	}
	return Result;
}

// -----------------------------------------------------------------------------------------
CHAR				NullStr[]="\0";
CHAR				UnknownStr[]="-?-";
CHAR				*szAll="*";

#define sprintf wsprintf

#include "..\..\ntcommon.cpp"
#include "..\..\vxdcommn.cpp"

DWORD			ReqestTlsIndex = -1;

VOID ApplEnableWin(HWND hWnd,BOOL Enable)
{
	EnableWindow(GetDlgItem(hWnd,IDC_APPL_TIME),Enable);
	EnableWindow(GetDlgItem(hWnd,IDC_APPL_TIME+30000),Enable);
	EnableWindow(GetDlgItem(hWnd,IDC_APPL_PRC_THIS),Enable);
	EnableWindow(GetDlgItem(hWnd,IDC_APPL_PRC_ALL),Enable);
	EnableWindow(GetDlgItem(hWnd,IDC_APPL_OBJ_THIS),Enable);
	EnableWindow(GetDlgItem(hWnd,IDC_APPL_OBJ_ALL),Enable);
	EnableWindow(GetDlgItem(hWnd,IDC_APPL_LOG),Enable);
//	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		EnableWindow(GetDlgItem(hWnd,IDC_KILL),!Enable);
//	}
}

HWND InitUDCtl(HWND hdlg,int IDEdit,int IDUD,int nUpper,int nLower,int nPos,HINSTANCE hInst)
{
HWND hEdit;
HWND hUDCtl;
RECT rc;
int  cx, cy, x, y;
	hEdit = GetDlgItem(hdlg, IDEdit);
	GetClientRect(hEdit, &rc);
	MapWindowPoints(hEdit, hdlg, (LPPOINT)&rc.left, 2);
	cx = 10;
	cy = (rc.bottom - rc.top) + 2;
	x  = rc.right;
	y  = rc.top - 1;
	hUDCtl = CreateWindow(UPDOWN_CLASS, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE |
		UDS_WRAP | UDS_ARROWKEYS |
		UDS_ALIGNRIGHT | UDS_SETBUDDYINT,
		x, y, cx, cy, hdlg,(HMENU)IDUD,       // ID of the UD control
		hInst,NULL);
	if (hUDCtl) {
		SendMessage(hUDCtl, UDM_SETBUDDY, (WPARAM)hEdit, 0L);
		SendMessage(hUDCtl, UDM_SETRANGE, 0, (LPARAM)MAKELONG(nUpper, nLower));
		SendMessage(hUDCtl, UDM_SETPOS, 0, (LPARAM)MAKELONG(nPos, 0));
	}
	return hUDCtl;
}

void RectScreenToClient(HWND hwnd,LPRECT rect)
{
	POINT p;

	p.y=rect->top;
	p.x=rect->left;
	ScreenToClient(hwnd,&p);
	rect->top=p.y;
	rect->left=p.x;
	p.y=rect->bottom;
	p.x=rect->right;
	ScreenToClient(hwnd,&p);
	rect->bottom=p.y;
	rect->right=p.x;
}

POINT g_ObjectNameSize = {0, 0};

LRESULT CALLBACK RequestDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	PTHREAD_PARAM	pThreadParam;
	PEVENT_TRANSMIT pEvt;

	HDC				hDC;
	SIZE			Size;
	RECT			RectWnd;
	RECT			rPos;
	CHAR			FuncName[MAXPATHLEN];
	HWND			MsgHWnd;
	HWND			hWndPos;
	CHAR			Timeout[10];
	CHAR			TimeoutCache[10];
	DWORD			H;
	PCHAR			AplPrc;
	PCHAR			AplObj;
	DWORD			AplFlg;
	FILETIME		STasFT;
	ULARGE_INTEGER	ExpTime;
	PCHAR			pObjName = "<empty>";
	HWND			hWndApply;

	int Item = 0;
	CHAR EventParamDesc[4096];
	CHAR ParamDesc[4096];

	CHAR pName[4096];
	CHAR pUnicodeName[2048];
	DWORD dwType;

	PSINGLE_PARAM pSingleParam;

	DWORD cou;
	
	HGDIOBJ hGdi, hGdiOld;

	pThreadParam = (PTHREAD_PARAM) TlsGetValue(ReqestTlsIndex);
	pEvt = (PEVENT_TRANSMIT) pThreadParam->bEventData;

	DWORD dwFilterSite = 0;	//new filters position 
	
	switch (wMsg)
	{ 
	case WM_INITDIALOG:
		GetWindowRect(hWnd, &RectWnd);
		
		hWndPos = GetDlgItem(hWnd, IDC_STATIC_POS);
		GetWindowRect(hWndPos, &rPos);
		
		if (pEvt->m_EventFlags & _EVENT_FLAG_DEADLOCK_DETECTED)
			SetWindowText(hWnd,TEXT("Warning! Deadlock detected..."));
		else
			SetWindowText(hWnd,TEXT("Warning!"));
		
		if(pThreadParam->pOSVer->dwPlatformId==VER_PLATFORM_WIN32_NT)
			NTGetFunctionStr(FuncName, pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi);
		else
			VxDGetFunctionStr(FuncName,pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi);

		MsgHWnd = GetDlgItem(hWnd, IDC_OBJ_MSG);
		hDC = GetDC(hWnd);
		hGdi = (HGDIOBJ) ::SendMessage(hWnd, WM_GETFONT, 0, 0);
		if (hGdi != NULL)
			hGdiOld = SelectObject(hDC, hGdi);
		
		g_ObjectNameSize.x = 0;

		Item = 0;
		lstrcpy(EventParamDesc, "");
		pSingleParam = (PSINGLE_PARAM) pEvt->m_Params;
		for (cou =  0; (cou < pEvt->m_ParamsCount); cou++)
		{
			GetSingleParameterInfo(pSingleParam, pName, ParamDesc, &dwType);
		
			if (pSingleParam->ParamID == _PARAM_OBJECT_URL)
				pObjName = (CHAR*) pSingleParam->ParamValue;
			if (pSingleParam->ParamID == _PARAM_OBJECT_URL_W)
			{
				wsprintf(pUnicodeName, "%S", (CHAR*) pSingleParam->ParamValue);
				pObjName = pUnicodeName;
			}

			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
			if (lstrcmp(ParamDesc, "") == 0)
			{
			}
			else
			{
				if (lstrlen(EventParamDesc) != 0)
					lstrcat(EventParamDesc, "\r\n");

				lstrcat(EventParamDesc, pName);
				lstrcat(EventParamDesc, ": ");
				lstrcat(EventParamDesc, ParamDesc);
			}
		}
		
		if(GetTextExtentPoint32(hDC, pObjName, strlen(pObjName), &Size))
		{
			RECT Rect;
			Size.cx += 14;	//scroll?
			GetWindowRect(MsgHWnd, &Rect);
			if(Rect.right - Rect.left < Size.cx)
			{
				RectScreenToClient(hWnd,  &Rect);
				g_ObjectNameSize.x = Size.cx - (Rect.right - Rect.left);
				MoveWindow(MsgHWnd, Rect.left, Rect.top, Size.cx, Rect.bottom - Rect.top, FALSE);
			}
		}
		g_ObjectNameSize.y = Size.cy;
	
		MoveWindow(hWnd, RectWnd.left - g_ObjectNameSize.x / 2, RectWnd.top, RectWnd.right - RectWnd.left + g_ObjectNameSize.x, RectWnd.bottom - RectWnd.top - (rPos.bottom - rPos.top ) - g_ObjectNameSize.y, FALSE);

		SelectObject(hDC, hGdiOld);
		ReleaseDC(hWnd,hDC);
		char procname[64];
		wsprintf(procname, "%s (%u)", pEvt->m_ProcName, pEvt->m_ProcessID);
		SetDlgItemText(hWnd,IDC_PROCESS_MSG, procname);
		SetDlgItemText(hWnd,IDC_FUNC_MSG,FuncName);
		SetDlgItemText(hWnd,IDC_OBJ_MSG, EventParamDesc);
		
		if (pEvt->m_Timeout) {
			H = pEvt->m_Timeout%3600;
			wsprintf(Timeout,"%02d:%02d:%02d",pEvt->m_Timeout/3600,H/60,H%60);
			SetDlgItemText(hWnd,IDC_TIMEOUT, Timeout);
			SetTimer(hWnd,2,1000,NULL);
		} else {
			SetDlgItemText(hWnd,IDC_TIMEOUT,"");
		}
		
		InitUDCtl(hWnd, IDC_APPL_TIME, IDC_APPL_TIME+30000, 999, 0, 5, g_hInst);
		CheckDlgButton(hWnd,IDC_APPL_THISTIME,BST_CHECKED);
		CheckDlgButton(hWnd,IDC_APPL_PRC_THIS,BST_CHECKED);
		CheckDlgButton(hWnd,IDC_APPL_OBJ_THIS,BST_CHECKED);
		CheckDlgButton(hWnd,IDC_APPL_LOG,BST_UNCHECKED);

		if (!(pEvt->m_EventFlags & _EVENT_FLAG_SAVECACHE_AVAILABLE))
		{
			EnableWindow(GetDlgItem(hWnd, IDC_STATIC_CACHE_TIMEOUT), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_EDIT_REQ_TIMEOUT), FALSE);
		}
		SetDlgItemText(hWnd, IDC_EDIT_REQ_TIMEOUT, "0");
		
		ApplEnableWin(hWnd, FALSE);
		break;
	case WM_TIMER:
		//DbgPrint(1, "HOOK-FACE: RequestDlgProc: WM_TIMER\n");
		if(--pEvt->m_Timeout == 0)
			PostQuitMessage(3);
		H = pEvt->m_Timeout%3600;
		wsprintf(Timeout,"%02d:%02d:%02d",pEvt->m_Timeout/3600,H/60,H%60);
		SetDlgItemText(hWnd,IDC_TIMEOUT,Timeout);
		TlsSetValue(ReqestTlsIndex, pThreadParam); 
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		TimeoutCache[0] = 0;
		if (GetDlgItemText(hWnd, IDC_EDIT_REQ_TIMEOUT, TimeoutCache, sizeof(TimeoutCache)))
		{
			long timeout = atol(TimeoutCache);
			pThreadParam->SetVerdict.m_ExpTime = timeout;
		}
		//DbgPrint(1, "HOOK-FACE: RequestDlgProc: exit\n");
		KillTimer(hWnd, 2);
		break;
	case WM_CONTEXTMENU:
		{
			ADDITIONALSTATE AddState;
			DWORD BytesRet;

			if (DeviceIoControl(pThreadParam->hDevice, IOCTLHOOK_GetDriverInfo, NULL, 0, &AddState, sizeof(AddState), &BytesRet, NULL))
			{
				if (AddState.DrvFlags & _DRV_FLAG_DEBUG)
				{
					HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU_REQUEST_MENU));
					if (hMenu != NULL)
					{
						HMENU hSubMenu = GetSubMenu(hMenu, 0);
						if (hSubMenu != NULL)
						{
							POINT pt;
							GetCursorPos(&pt);
							HWND hFore = GetForegroundWindow();

							SetForegroundWindow(hWnd);
							DWORD MenuId = TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
							switch (MenuId)
							{
							case IDMI_DBG_BREAK:
								pThreadParam->SetVerdict.m_Verdict=Verdict_Debug;
								PostQuitMessage(0);
								break;
							default:
								break;
							}

							SetForegroundWindow(hFore);
						}
					}
				}
			}
		}
		break;
	case WM_COMMAND:
		{
			switch (LOWORD (wParam)) 
			{
			case IDC_APPL_THISTIME:
				if(GET_WM_COMMAND_CMD(wParam, lParam)==BN_CLICKED)
					ApplEnableWin(hWnd,FALSE);
				return TRUE;
			case IDC_APPL_ALWAYS:
				if(GET_WM_COMMAND_CMD(wParam, lParam)==BN_CLICKED) {
					ApplEnableWin(hWnd,TRUE);
					EnableWindow(GetDlgItem(hWnd,IDC_APPL_TIME),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_APPL_TIME+30000),FALSE);
				}
				return TRUE;
			case IDC_APPL_TIMED:
				if(GET_WM_COMMAND_CMD(wParam, lParam)==BN_CLICKED)
					ApplEnableWin(hWnd, TRUE);
				return TRUE;
			case IDC_PASS:
				pThreadParam->SetVerdict.m_Verdict=Verdict_Pass;
				break;
			case IDC_DISCARD:
				pThreadParam->SetVerdict.m_Verdict=Verdict_Discard;
				break;
			case IDC_KILL:
				pThreadParam->SetVerdict.m_Verdict=Verdict_Kill;
				break;
			case IDC_APPLY_FOR:
				hWndPos = GetDlgItem(hWnd, IDC_STATIC_POS);
				GetWindowRect(hWndPos, &rPos);
				
				GetWindowRect(hWnd, &RectWnd);

				hWndApply = GetDlgItem(hWnd,IDC_APPLY_FOR);
				if (hWndApply != NULL)
				{
					LRESULT PushState = SendMessage(hWndApply, BM_GETSTATE, 0, 0);
					//DbgPrint(1, "Push state %d\n", PushState);
					if (PushState & BST_CHECKED)
						MoveWindow(hWnd, RectWnd.left, RectWnd.top, RectWnd.right - RectWnd.left, RectWnd.bottom - RectWnd.top + (rPos.bottom - rPos.top ) + g_ObjectNameSize.y, TRUE);
					else 
						MoveWindow(hWnd, RectWnd.left, RectWnd.top, RectWnd.right - RectWnd.left, RectWnd.bottom - RectWnd.top - (rPos.bottom - rPos.top ) - g_ObjectNameSize.y, TRUE);
					return TRUE;
				}
				return FALSE;
			default:
				return FALSE;
			}

			if(IsDlgButtonChecked(hWnd,IDC_APPL_THISTIME)==BST_UNCHECKED) 
			{
				if(IsDlgButtonChecked(hWnd,IDC_APPL_TIMED)==BST_CHECKED)
				{
					GetSystemTimeAsFileTime(&STasFT);
					ExpTime.LowPart=STasFT.dwLowDateTime;
					ExpTime.HighPart=STasFT.dwHighDateTime;
					GetDlgItemText(hWnd,IDC_APPL_TIME,(LPSTR)Timeout,10);
					STasFT.dwLowDateTime=atol(Timeout);
					ExpTime.QuadPart=ExpTime.QuadPart+(LONGLONG)STasFT.dwLowDateTime*600000000;
				}
				else
				{
					ExpTime.QuadPart=0;
				}
				if(IsDlgButtonChecked(hWnd,IDC_APPL_PRC_THIS)==BST_CHECKED)
					AplPrc=pEvt->m_ProcName;
				else
					AplPrc = szAll;
				if(IsDlgButtonChecked(hWnd,IDC_APPL_OBJ_THIS)==BST_CHECKED)
					AplObj=pObjName;
				else
					AplObj=szAll;
				
				AplFlg=0;
				if(IsDlgButtonChecked(hWnd,IDC_APPL_LOG)==BST_CHECKED)
					AplFlg|=FLT_A_LOG;
				if(pThreadParam->SetVerdict.m_Verdict == Verdict_Discard)
					AplFlg|=FLT_A_DENY;
				
				AddFSFilter(pThreadParam->hDevice, pThreadParam->AppID, AplObj, AplPrc, DEADLOCKWDOG_TIMEOUT, AplFlg,	//!! timeout
					pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi, ExpTime.QuadPart, PreProcessing, &dwFilterSite);
			}
			PostQuitMessage(0);
			break;
			default:
				return FALSE;
		}
		break;
	}
	return FALSE;
}

// -----------------------------------------------------------------------------------------

int ExceptionLockReached(char* pMessage, char* pUrl, PTHREAD_PARAM pThreadParam)
{
	DbgPrint(1, " ----------------- Exception reached ----------------- \n");
	DbgPrint(1, "Handle: %x, Message: '%s' Url: %s\n", pThreadParam->hDevice, pMessage, pUrl);
	return 1;
}
// -----------------------------------------------------------------------------------------

BOOL _cdecl _go(DT_CALLTYPE eCallType, PDGS_THREAD pDogsThread)
{
	BOOL bResult = TRUE;
	PTHREAD_PARAM pThreadParam = (PTHREAD_PARAM) pDogsThread->pData;

	if (pThreadParam == NULL)
		return FALSE;

	switch (eCallType)
	{
	case DT_INIT:
		{
			if (IDriverRegisterInvisibleThread(pThreadParam->hDevice, pThreadParam->AppID) == 0)
			{
				DbgPrint(1, "RequestThread: Can't register invisible thread\n");
				bResult = FALSE;
			}
		}
		break;
	case DT_PROCEED:
		{
			DWORD BytesRet;

			char* pObjName = NULL;

			TlsSetValue(ReqestTlsIndex, pThreadParam);

			__try
			{
				PEVENT_TRANSMIT pEvt;
				pEvt = (PEVENT_TRANSMIT) pThreadParam->bEventData;
				if (pEvt->m_EventFlags & _EVENT_FLAG_TOUCH)
				{
					CreateEventInfoDlg(NULL, pEvt);
				}
				else
				{
					MSG msg;
					HWND hRDlg;

					hRDlg = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_REQUEST_DLG), NULL,(DLGPROC) RequestDlgProc, 0);
					if(!hRDlg)
					{
						DbgPrint(1, "HOOK-FACE: CreateRequestDlg failed\n");
					}
					else
					{
						while(GetMessage(&msg,NULL, 0, 0)) 
						{
							if (!IsDialogMessage (hRDlg, &msg))
							{
								TranslateMessage(&msg);
								DispatchMessage(&msg);
							}
						}
						DestroyWindow(hRDlg);
					}
				}
			}
			__except(ExceptionLockReached("WaiterThread: internal crashe", pObjName, pThreadParam), 1)
			{

			}
			TlsSetValue(ReqestTlsIndex, NULL); 
			
			if (!DeviceIoControl(pThreadParam->hDevice, IOCTLHOOK_SetVerdict, &pThreadParam->SetVerdict, sizeof(SET_VERDICT), NULL, 0, &BytesRet, NULL))
			{
				DbgPrint(1, "RequestThread: Set verdict failed! Error is %x\n", GetLastError());
			}
			GlobalFree(pThreadParam);
		}
		break;
	case DT_CLOSE:
		{
			IDriverUnregisterInvisibleThread(pThreadParam->hDevice, pThreadParam->AppID);
		}
		break;
	}
	return bResult;
}
// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------
// Function name	: CopyThreadParam
// Description	    : coping data
// Return type		: void 
// Argument         : PTHREAD_PARAM pThreadParamTo
// Argument         : PTHREAD_PARAM pThreadParam
void CopyThreadParam(PTHREAD_PARAM pThreadParamTo, PTHREAD_PARAM pThreadParam)
{
	pThreadParamTo->hDevice = pThreadParam->hDevice;
	pThreadParamTo->hWhistleup = pThreadParam->hWhistleup;
	pThreadParamTo->AppID = pThreadParam->AppID;
	pThreadParamTo->SetVerdict = pThreadParam->SetVerdict;
	pThreadParamTo->pOSVer = pThreadParam->pOSVer;
}

/*// -----------------------------------------------------------------------------------------
// delayed operation block
CRITICAL_SECTION Delayed_CrSec;

#include <stuff\parray.h>
CPArray <THREAD_PARAM> DelayedList(0, 1, false);

PDGS_THREAD pDeleaydThread = NULL;

DWORD WINAPI DelayedJobThread(void* pParam)
{
	PDGS_THREAD pThread = (PDGS_THREAD) pParam;
	PTHREAD_PARAM pThreadParam;

	DbgPrint(1, "WaiterThread: : Delayed thread started\n");

	if (pThread != NULL)
	{
		if (_go(DT_INIT, pThread) == TRUE)
		{
			if (pThread->dwStatus == _THRD_STATUS_BISY)
				SetEvent(pThread->hEvent);

			while (WaitForSingleObject(pThread->hEvent, INFINITE) == WAIT_OBJECT_0)
			{
				if (pThread->dwStatus == _THRD_STATUS_EXIT)
					break;
				do
				{
					pThreadParam = NULL;
					if (pThreadParam != NULL)
					{
						// processing event ------------------------------------------------------------
						pThread->pData = pThreadParam;
						//DbgPrint(1, "Delayed event processing: \n");
						_go(DT_PROCEED, pThread);
						// end processing --------------------------------------------------------------
					}

					//DbgPrint(1, "-------------- Count is %d\n", DelayedList.Count());
					EnterCriticalSection(&Delayed_CrSec);
					if (DelayedList.Count() > 0)
					{
						__try
						{
							pThreadParam = DelayedList[0];
							if (pThreadParam != NULL)
								DelayedList.RemoveInd(0);
						}
						__except(1)
						{
						}
					}
					LeaveCriticalSection(&Delayed_CrSec);
				} while (pThreadParam != NULL);

				pThread->dwStatus = _THRD_STATUS_FREE;
			}
		}
		else
		{
			DbgPrint(1, "WaiterThread: : WT. init failed\n");
		}
		_go(DT_CLOSE, pThread);

		CloseHandle(pThread->hEvent);
		_MM_Free(pThread);
	}
	pDeleaydThread = NULL;
	DbgPrint(1, "WaiterThread: : Delayed thread finished.\n");
	return 0;
}

void InitDelayed(PTHREAD_PARAM pThreadParam)
{
	InitializeCriticalSection(&Delayed_CrSec);

	pDeleaydThread = (PDGS_THREAD) _MM_Alloc(sizeof(DGS_THREAD));
	if (pDeleaydThread != NULL)
	{
		pDeleaydThread->dwStatus = _THRD_STATUS_FREE;
		pDeleaydThread->pData = pThreadParam;
		
		pDeleaydThread->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (pDeleaydThread->hEvent != NULL)
		{
			pDeleaydThread->hThread = CreateThread(NULL, 0, DelayedJobThread, (void*) pDeleaydThread, 0, &pDeleaydThread->dwThreadID);
			if(pDeleaydThread->hThread == NULL)
			{
				CloseHandle(pDeleaydThread->hEvent);
				_MM_Free(pDeleaydThread);
				pDeleaydThread = NULL;
			}
		}
	}
}

BOOL IsEventDalayed(PTHREAD_PARAM pThreadParam)
{
	BOOL bRet = FALSE;
	PTHREAD_PARAM pThreadParamOld;
	DWORD count;
	char* pUrlOld;
	char* pUrl;

	PEVENT_TRANSMIT pEvt;
	PSINGLE_PARAM pSingleParam;

	EnterCriticalSection(&Delayed_CrSec);

	count = DelayedList.Count();
	if (count > 0)
	{
		for (DWORD cou = 0; cou < count && bRet == FALSE; cou++)
		{
			pThreadParamOld = DelayedList[cou];
			if (pThreadParamOld != NULL)
			{
				pUrl = NULL;
				pUrlOld = NULL;

				pEvt = (PEVENT_TRANSMIT) pThreadParam->bEventData;
				pSingleParam = (PSINGLE_PARAM) pEvt->m_Params;

				while (pSingleParam != NULL && pSingleParam->ParamID != _PARAM_OBJECT_URL)
					pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
				if (pSingleParam != NULL)
					pUrl = (char*) pSingleParam->ParamValue;
					
				pEvt = (PEVENT_TRANSMIT) pThreadParamOld->bEventData;
				pSingleParam = (PSINGLE_PARAM) pEvt->m_Params;

				while (pSingleParam != NULL && pSingleParam->ParamID != _PARAM_OBJECT_URL)
					pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
				if (pSingleParam != NULL)
					pUrlOld = (char*) pSingleParam->ParamValue;

				if (pUrl != NULL && pUrlOld !=NULL)
					if (lstrcmp(pUrl, pUrlOld) == 0)
						bRet = TRUE;
			}
		}
	}


	LeaveCriticalSection(&Delayed_CrSec);
	
	return bRet;
}

BOOL AddToDelayedList(PTHREAD_PARAM pThreadParam)
{
	BOOL bRet = FALSE;

	if (pDeleaydThread == NULL)
		return bRet;

	if (IsEventDalayed(pThreadParam) == FALSE)
	{
		bRet = TRUE;
		EnterCriticalSection(&Delayed_CrSec);
		DelayedList.Add(pThreadParam);
		LeaveCriticalSection(&Delayed_CrSec);
		
		if (pDeleaydThread != NULL)
			SetEvent(pDeleaydThread->hEvent);
	}
	
	return bRet;
}

void ResetDelayedList()
{
	PTHREAD_PARAM pThreadParam;
	DWORD BytesRet;
	
	DbgPrint(1, "WaiterThread: : ResetDelayedList...\n");
	EnterCriticalSection(&Delayed_CrSec);

	DbgPrint(1, "DelayedThread: reseting. Event count: %d...\n", DelayedList.Count());
	
	for (DWORD dwCou = 0; dwCou < (DWORD) DelayedList.Count(); dwCou++)
	{
		pThreadParam = DelayedList[dwCou];
		if (pThreadParam != NULL)
		{
			__try
			{
				if (!DeviceIoControl(pThreadParam->hDevice, IOCTLHOOK_SetVerdict, &pThreadParam->SetVerdict, sizeof(SET_VERDICT), NULL, 0, &BytesRet, NULL))
				{
					DbgPrint(1, "DelayedThread: pass failed!\n");
				}
				GlobalFree(pThreadParam);
			}
			__except(1)
			{
			}
		}
	}
	DelayedList.RemoveAll();

	LeaveCriticalSection(&Delayed_CrSec);

	if (pDeleaydThread != NULL)
	{
		HANDLE hThread = pDeleaydThread->hThread;

		DbgPrint(1, "WaiterThread: : Closing delayed thread...\n");

		pDeleaydThread->pData = NULL;
		pDeleaydThread->dwStatus = _THRD_STATUS_EXIT;
	
		SetEvent(pDeleaydThread->hEvent);
		if (WaitForSingleObject(hThread, 1000 * 15) == WAIT_TIMEOUT)
		{
			DbgPrint(1, "WaiterThread: : delayedthread timeout elapsed - kill\n");
			__try
			{
				_go(DT_CLOSE, pDeleaydThread);
				TerminateThread(hThread, 0);
			}
			__except(TRUE)
			{
			}
		}
	}
	pDeleaydThread = NULL;
	DeleteCriticalSection(&Delayed_CrSec);
	
	DbgPrint(1, "WaiterThread: : ResetDelayedList...done\n");
}
*/
// -----------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------
// Function name	: GetEventData
// Description	    : get event data  from driver
// Return type		: BOOL  - TRUE if retrieving complete
// Argument         : PTHREAD_PARAM pThreadParam
BOOL GetEventData(PTHREAD_PARAM pThreadParam)
{
	DWORD BytesRet;
	DWORD BufferSize;

	pThreadParam->Event.m_AppID = pThreadParam->AppID;

// -----------------------------------------------------------------------------------------
	pThreadParam->Event.Mark = InterlockedIncrement(&g_Mark); 
	BufferSize = sizeof(pThreadParam->bEventData);
// -----------------------------------------------------------------------------------------

	if (!IDriverGetEvent(pThreadParam->hDevice, pThreadParam->AppID, pThreadParam->Event.Mark, (BYTE*) &(pThreadParam->bEventData), BufferSize, &BytesRet))
	{
		DbgPrint(1, "GetEventData: GetEvent failed! (buffer size was %d)\n", BufferSize);
		IDriverSkipEvent(pThreadParam->hDevice, pThreadParam->AppID);
	}
	else
	{
		if(BytesRet)
		{
			PEVENT_TRANSMIT pEvt = (PEVENT_TRANSMIT) pThreadParam->bEventData;
			DWORD HookID = pEvt->m_HookID;
			DWORD FunctionMj = pEvt->m_FunctionMj;
			DWORD FunctionMi = pEvt->m_FunctionMi;

			return TRUE;
		}
		else
		{
			DbgPrint(1, "GetEventData: GetEvent failed! (buffer size was %d) skipping\n", BufferSize);
			IDriverSkipEvent(pThreadParam->hDevice, pThreadParam->AppID);
		}
	}
	
	return FALSE;
}

// -----------------------------------------------------------------------------------------
// Function name	: WatchDog
// Description	    : thread informing about client activity
// Return type		: DWORD WINAPI 
// Argument         : void* pParam - PTHREAD_PARAM
DWORD WINAPI WatchDog(void* pParam)
{
	PTHREAD_PARAM pThreadParam = (PTHREAD_PARAM) pParam;
	HDSTATE Activity;
	DWORD BytesRet;
	DWORD FailCount = 0;

	DbgPrint(1, "WatchDogThreadID: %x\n", GetCurrentThreadId());

	while (pThreadParam->hWhistleup != NULL && WaitForSingleObject(pThreadParam->hWhistleup, 1000) != WAIT_FAILED) 
	{
		Activity.Activity = _AS_DontChange;
		Activity.AppID = pThreadParam->AppID;
		if (!DeviceIoControl (pThreadParam->hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
		{
			DbgPrint(1, "WatchDog: watch dog failed!\n");
			FailCount++;
		}
		if (FailCount > 10)
		{
			DbgPrint(1, "WatchDog: too many errors!\n");
			CloseHandle(pThreadParam->hWhistleup);
			pThreadParam->hWhistleup = NULL;
		}
	}
	
	DbgPrint(1, "WatchDog: done\n");

	_MM_Free(pThreadParam);
	
	return 0;
}

// -----------------------------------------------------------------------------------------
// Function name	: EventWaiterThread
// Description	    : thread function for waiting event from driver
// Return type		: DWORD WINAPI 
// Argument         : void* pParam - PTHREAD_PARAM
DWORD WINAPI EventWaiterThread(void* pParam)
{
	PTHREAD_PARAM pThreadParam = (PTHREAD_PARAM) pParam;
	DWORD	wathdog_id;
	
	PTHREAD_PARAM pWatchDogParam;
	PTHREAD_PARAM pNewParam;

	BOOL bNeedFree;

	DbgPrint(1, "EventWaiterThreadID: %x\n", GetCurrentThreadId());

	ReqestTlsIndex = TlsAlloc();
	if (ReqestTlsIndex == -1)
	{
		DbgPrint(1, "EventThread: Tls failed!\n");
		return 0;
	}

	if (pThreadParam == NULL)
	{
		DbgPrint(1, "EventThread: Param is NULL!\n");
		TlsFree(ReqestTlsIndex);

		return 0;
	}

	if (DTInit(_go, 5, pThreadParam) == FALSE)
	{
		DbgPrint(1, "EventThread: DogsThred failed!\n");
		TlsFree(ReqestTlsIndex);

		return 0;
	}

//	InitDelayed(pThreadParam);

//	EvCacheInit(pThreadParam->hTree, pThreadParam->hHeap, 1000);

	pWatchDogParam = (PTHREAD_PARAM) _MM_Alloc(sizeof(_THREAD_PARAM));
	if (pWatchDogParam == NULL)
	{
		DbgPrint(1, "EventThread: pWatchDogParam is NULL!\n");
		TlsFree(ReqestTlsIndex);

		return 0;
	}
	
	CopyThreadParam(pWatchDogParam, pThreadParam);
#ifdef _DEBUG
	pWatchDogParam->hWhistleup = CreateEvent(NULL, FALSE, FALSE, "MonCon_WatchDog");
#else
	pWatchDogParam->hWhistleup = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif

	if (pThreadParam->hDevice != NULL && pThreadParam->hWhistleup != NULL && pWatchDogParam->hWhistleup != NULL)
	{
		DbgPrint(1, "WaiterThread: : Set thread priority to crititcal\n");
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		DbgPrint(1, "WaiterThread: : Create WatchDog thread\n");
		if(CreateThread(NULL, 0, WatchDog, (void*) pWatchDogParam, 0, &wathdog_id))
		{
			DWORD dwWaitStatus;

			dwWaitStatus = WaitForSingleObject(pThreadParam->hWhistleup, INFINITE);
			while (dwWaitStatus != WAIT_ABANDONED) 
			{
				//DbgPrint(1, "WaiterThread: : event...\n");
				if (EvExit != 0)
				{
					DbgPrint(1, "WaiterThread: : exit\n");
					break;
				}
				/*if (dwWaitStatus == WAIT_TIMEOUT)
				{
					// System free, we can use time to check close files
//					ProcessDelayedList();
				}
				else*/
				{
					//dwWaitStatus == WAIT_OBJECT_0
					DWORD BytesRet;
					HDSTATE Activity;
					SET_VERDICT SetVerdict;

					Activity.Activity = _AS_DontChange;
					Activity.AppID = pThreadParam->AppID;
					Activity.QLen = 0;
					Activity.QUnmarkedLen = 0;
					if (!DeviceIoControl (pThreadParam->hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
						Activity.QUnmarkedLen = 1;

					while (Activity.QUnmarkedLen > 0) 
					{
						Activity.QUnmarkedLen--;
						pNewParam = (PTHREAD_PARAM) _MM_Alloc(sizeof(_THREAD_PARAM));
						bNeedFree = TRUE;
						if (pNewParam != NULL)
						{
							CopyThreadParam(pNewParam, pThreadParam);
							if (GetEventData(pNewParam) == FALSE)
							{
								//Activity.QUnmarkedLen = 0;	//!!
							}
							else
							{
								SetVerdict.m_Verdict = Verdict_Default;	//answer will not be cached in drive - if you want this - change to Verdict_Pass
								SetVerdict.m_AppID = pNewParam->AppID;
								SetVerdict.m_Mark = pNewParam->Event.Mark;
								SetVerdict.m_ExpTime = 0;
								
								pNewParam->SetVerdict = SetVerdict;

								if (DTProceedData(pNewParam) == TRUE)
									bNeedFree = FALSE;
								else
								{
									DbgPrint(1, "EventWaiterThread: CreateThread failed. Event will be passed.\n");
								}
								if (bNeedFree == TRUE)
								{
									IDriverSetVerdict2(pThreadParam->hDevice, &pNewParam->SetVerdict);
								}
							}
							if (bNeedFree == TRUE)
								GlobalFree(pNewParam);
						}
						else
						{
							DbgPrint(1, "EventWaiterThread: Error during allocating memory for NewParam\n");
						}
					}
				}
				dwWaitStatus = WaitForSingleObject(pThreadParam->hWhistleup, 500);
			}
			DbgPrint(1, "EventWaiterThread: WaitForSingleObject failed\n");
		}
	}
	else
	{
		DbgPrint(1, "EventWaiterThread: some handles are null!\n");
	}
	__try
	{
		if (pWatchDogParam->hWhistleup != NULL)
		{
			CloseHandle(pWatchDogParam->hWhistleup);
			pWatchDogParam->hWhistleup = NULL;
		}
	}
	__except(TRUE)
	{
	}

//	EvCacheDone();

	DbgPrint(1, "DTDone...\n");
	DTDone(pThreadParam);
	DbgPrint(1, "DTDone completed\n");

//	ResetDelayedList();

	_MM_Free(pThreadParam);
	
	DbgPrint(1, "WaiterThread: : EvThread closed\n");

	TlsFree(ReqestTlsIndex);

	return 0;
}


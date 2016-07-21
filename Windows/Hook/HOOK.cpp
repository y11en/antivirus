#define _WIN32_IE 0x0300

#ifdef AlternateInterface
	#pragma message("AlternateInterface enabled!")
	#define Interface_ID AVPG_StandAlone
	#define _LOG_FILE_NAME "AltAvpg.log"
#else
	#pragma message("AlternateInterface disabled!")
	#define Interface_ID OldFaseAppID
	#define _LOG_FILE_NAME "Avpg.log"
#endif

#include <windows.h>
#include <windowsx.h>
#include <process.h>
#include <winioctl.h>
#include <commctrl.h>
#include <prsht.h>


#include "resource.h"
#include "../hook\funcs.h>
#include "../hook\avpgcom.h>
#include "../hook\HookSpec.h>

APP_REGISTRATION AppReg;

#define SIZEOF(a) sizeof(a)/sizeof(CHAR*)
#define wID LOWORD(wParam)
#define TOT_PAGES 2
#define wCode HIWORD(wParam)

CHAR *NullStr="\0";
CHAR *UnknownStr="-?-";

#define sprintf	wsprintf
#include "ntcommon.cpp"
#include "vxdcommn.cpp"

#define __BUFFER	4096
//#define UNREGISTER_DRIVER   // For deleting driver from SCManager database

#define DEFAULT_TIMEOUT 0

#define START_FILE_OPERATION IDC_WRITE_EXE
#define END_FILE_OPERATION IDC_RENAME_EXE_PROMPT

#define START_DIRECTIO_OPERATION IDC_MBR_WRITE
#define END_DIRECTIO_OPERATION IDC_WRITE_BOOT_PROMPT

#define START_REGISTRY_OPERATION IDC_REG_WRITE
#define END_REGISTRY_OPERATION IDC_REG_PROMPT


CHAR AppName[9];
CHAR DrvStr[]="\\\\.\\%s";
CHAR *szAVPGKeyName="SOFTWARE\\KasperskyLab\\"AVPGNAME;
CHAR *szConfigKeyName="Config";
CHAR *FiltersKeyName="Filters";
CHAR *FiltersKeyNameNT="SYSTEM\\CurrentControlSet\\Services\\"AVPGNAME"\\Parameters\\Filters";
CHAR *szLogValName="Log";

CHAR *RegCreateServNT="\\registry\\machine\\system\\currentcontrolset\\services\\*";
CHAR *RegCreateVxD="HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Services\\VxD\\*";
//HKEY_LOCAL_MACHINE\System\CurrentControlSet\control\SessionManager\KnownVxDs
CHAR *RegWriteRunVxD="HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run*";
CHAR *RegWriteRunVxDU="HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run*";
CHAR *RegWriteRunNT="\\registry\\machine\\software\\microsoft\\windows\\currentversion\\run*";

//CHAR *DevFloppy="\\Device\\Floppy*";
//CHAR *DevHardDisk="\\Device\\Harddisk*";

CHAR *szExecFileName[]={
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
	"*.cmd"};
CHAR *szAll="*";

UINT uiBackUpFileOper[END_FILE_OPERATION-START_FILE_OPERATION];
UINT uiBackUpDirectIOOper[END_DIRECTIO_OPERATION-START_DIRECTIO_OPERATION];
UINT uiBackUpRegOper[END_REGISTRY_OPERATION-START_REGISTRY_OPERATION];
BOOL IsIntoConfigDialog=FALSE;

CHAR *ParamOperStr[]={
	"== ",
	" & ",
	" > ",
	" < "
};

#define IDC_ICONNOT	100
#define WM_NOTIFYICON		(WM_APP+100)
#define SysExt TEXT(".SYS")
#define VxdExt TEXT(".VXD")
HANDLE			hDevice=INVALID_HANDLE_VALUE;
HANDLE			hWhistleup=NULL;
HANDLE			hWFChanged=NULL;

#ifdef AlternateInterface
CHAR *WhistleupName="AVPGHWUp";
CHAR *WFChangedName="AVPGHWFCh";
#else
CHAR *WhistleupName="AVPGHWUpAlternate";
CHAR *WFChangedName="AVPGHWFChAlternate";
#endif

HINSTANCE		hInst;
LONG				OldQLen=0;
OSVERSIONINFO	OSVer;
PCHAR				DriverExt;
const DWORD		ApplySize=65;
DWORD				ReqestTlsIndex=-1;

PCHAR GetObjNameFromEvent(PEVENT_TRANSMIT pEvt)
{
	PCHAR pObjName = NULL;
	PSINGLE_PARAM	pSingleParam;
	pSingleParam = (PSINGLE_PARAM) pEvt->m_Params;
	while (pSingleParam != NULL && pSingleParam->ParamID != _PARAM_OBJECT_URL)
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);

	if (pSingleParam != NULL)
		pObjName = (char*) pSingleParam->ParamValue;

	return pObjName;
}

DWORD LoadDeviceDriver(CHAR *Name,CHAR *Path)
{
SC_HANDLE	SchSCManager=0;
SC_HANDLE	schService;
DWORD	Err;
	if(SchSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS)) {
		if((schService=CreateService(SchSCManager,Name,Name,SERVICE_ALL_ACCESS,SERVICE_KERNEL_DRIVER,SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL,Path,NULL,NULL,NULL,NULL,NULL))
			|| ((Err=GetLastError())==ERROR_SERVICE_EXISTS)) {
			CloseServiceHandle(schService);
			if(schService=OpenService(SchSCManager,Name,SERVICE_ALL_ACCESS)) {
				if((StartService(schService,0,NULL)) || ((Err=GetLastError())==ERROR_SERVICE_ALREADY_RUNNING)) {
					Err=0;
				}
				CloseServiceHandle(schService);
			}
		}
	} else {
		Err=GetLastError();
	}
	if(SchSCManager)
		CloseServiceHandle(SchSCManager);
	return Err;
}


DWORD UnloadDeviceDriver(const CHAR * Name)
{
SC_HANDLE	SchSCManager;
SC_HANDLE	schService;
SERVICE_STATUS  serviceStatus;
DWORD	Err;
	if(SchSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS)) {
		if(schService=OpenService(SchSCManager,Name,SERVICE_ALL_ACCESS)) {
			Err=ControlService(schService,SERVICE_CONTROL_STOP,&serviceStatus);
			CloseServiceHandle(schService);
			if(Err) {
#ifdef UNREGISTER_DRIVER
				if(schService=OpenService(SchSCManager,Name,SERVICE_ALL_ACCESS)) {
					Err=DeleteService(schService);
					CloseServiceHandle(schService);
					if(Err) {
#endif //UNREGISTER_DRIVER
						CloseServiceHandle(SchSCManager);
						return 0;
#ifdef UNREGISTER_DRIVER
					}
				}
#endif //UNREGISTER_DRIVER
			}
		}
	 	CloseServiceHandle(SchSCManager);
	} else 	
		if(GetLastError()==ERROR_ACCESS_DENIED)
			return 0;
	Err=GetLastError();
	return Err;
}

BOOL TrayMessage(HWND hDlg, UINT uFlags, DWORD dwMessage, UINT uID, HICON hIcon, CHAR *pszTip)
{
BOOL res;
NOTIFYICONDATA tnd;
	tnd.cbSize=sizeof(NOTIFYICONDATA);
	tnd.hWnd=hDlg;
	tnd.uID=uID;
	tnd.uFlags=uFlags;
	tnd.uCallbackMessage=WM_NOTIFYICON;
	tnd.hIcon=hIcon;
	if(pszTip)
		lstrcpyn(tnd.szTip,pszTip,sizeof(tnd.szTip));
	else
		tnd.szTip[0] = '\0';
	res=Shell_NotifyIcon(dwMessage, &tnd);
	if(hIcon)
		DestroyIcon(hIcon);
	return res;
}

void SetSmile(HWND hWnd,DWORD idi)
{
	TrayMessage(hWnd,NIF_ICON,NIM_MODIFY,IDC_ICONNOT,(HICON)LoadImage(hInst,MAKEINTRESOURCE(idi),IMAGE_ICON,16,16,0),AppName);
	SetClassLong(hWnd,GCL_HICON,(LONG)LoadIcon(hInst,MAKEINTRESOURCE(idi)));
}

void Error(HWND hWnd,LPCSTR MyMsg, DWORD LastError)
{
LPVOID lpMsgBuf;
CHAR Tmp[1024];
	SetSmile(hWnd,IDI_ICON3);
 	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,LastError,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL);
	wsprintf(Tmp,"%s\n%s",MyMsg,LastError?lpMsgBuf:"");
	LocalFree(lpMsgBuf);
//	CloseHandle(hDevice);
//	hDevice=INVALID_HANDLE_VALUE;
	MessageBox(hWnd,Tmp,AppName,MB_OK|MB_ICONERROR);
	PostQuitMessage(1);
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
	hUDCtl = CreateWindow(UPDOWN_CLASS,NULL,
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

typedef struct _STATE {
	ULONG Time;
	BOOL  bAppl;
//	EVENT_TRANSMIT Event;
} STATE,*PSTATE;

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

BOOL SetFilter(DWORD FltCtl,LPSTR szFileName,LPSTR szProcessName,DWORD dwTimeout,DWORD dwFlags,DWORD Func,DWORD Param,DWORD Value,LONGLONG ExpireTime)
{
/* !!!!!!!!!!!!!!!!
FILTER_TRANSMIT Filter;
DWORD BytesRet;
	ZeroMemory(&Filter,sizeof(Filter));
	lstrcpyn(Filter.m_ObjName,szFileName,MAXPATHLEN);
	lstrcpyn(Filter.m_ProcName,szProcessName,PROCNAMELEN);
	Filter.Timeout=dwTimeout;
	Filter.Expiration=ExpireTime;
	Filter.Flags=dwFlags;
	Filter.Function=Func;
	Filter.Param=Param;
	Filter.Value=Value;
	Filter.FltCtl=FltCtl;
	return DeviceIoControl(hDevice,IOCTLHOOK_FiltersCtl,&Filter,sizeof(FILTER_TRANSMIT),NULL,0,&BytesRet,NULL);
*/
	return TRUE;
}

LRESULT CALLBACK ReqestDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
DWORD			BytesRet;
SET_VERDICT	SetVerdict;
HDC			hDC;
SIZE			Size;
RECT			Rect;
CHAR			FuncName[MAXPATHLEN];
HWND			MsgHWnd;
CHAR			Timeout[10];
DWORD			H;
PSTATE		State;
PCHAR			AplPrc;
PCHAR			AplObj;
DWORD			AplFlg;
DWORD			OnBaseUnits;
FILETIME		STasFT;
ULARGE_INTEGER ExpTime;
PCHAR pObjName = "<empty>";

HGDIOBJ hGdi, hGdiOld;

char EventBuffer[__BUFFER];

PEVENT_TRANSMIT pEvt = (PEVENT_TRANSMIT) EventBuffer;
//FILTER_TRANSMIT FilterTransmit;

GET_EVENT GetEvent;
long cy;
	switch (wMsg) { 
	case WM_INITDIALOG:
		#ifdef _DEBUG
		OutputDebugString("HOOK-FACE: ReqestDlgProc WM_INITDIALOG\n");
		#endif //_DEBUG
		if(!(State=(PSTATE)LocalAlloc(LMEM_ZEROINIT,sizeof(*State)))) {
			PostQuitMessage(2);
			break;
		}
		GetWindowRect(hWnd,&Rect);
		MoveWindow(hWnd,Rect.left,Rect.top,Rect.right-Rect.left,Rect.bottom-Rect.top-(ApplySize*HIWORD(GetDialogBaseUnits()))/8,FALSE);
		SetWindowText(hWnd,TEXT("Warning!"));
		GetEvent.m_AppID = Interface_ID;
		GetEvent.Mark = (unsigned)hWnd;
		if(!DeviceIoControl(hDevice,IOCTLHOOK_GetEvent,&GetEvent,sizeof(GET_EVENT),pEvt,__BUFFER,&BytesRet,NULL)) {
			//Error(hWnd,"GetEvent failed",0);//!!
			OutputDebugString("GetEvent failed!\n");
			break;
		}
		if(!BytesRet) {
			PostQuitMessage(0);
			break;
		}
		State->bAppl=FALSE;
		State->Time = pEvt->m_Timeout;
		TlsSetValue(ReqestTlsIndex,State);

		if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT)
			NTGetFunctionStr(FuncName, pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi);
		else
			VxDGetFunctionStr(FuncName,pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi);

		MsgHWnd=GetDlgItem(hWnd,IDC_OBJ_MSG);
		hDC=GetDC(hWnd);
		hGdi = (HGDIOBJ) ::SendMessage(hWnd, WM_GETFONT, 0, 0);
		if (hGdi != NULL)
			hGdiOld = SelectObject(hDC, hGdi);

		pObjName = GetObjNameFromEvent(pEvt);
		if (pObjName == NULL)
			pObjName = "<not defined>";
		if(GetTextExtentPoint32(hDC,pObjName,strlen(pObjName),&Size)) {
			GetWindowRect(MsgHWnd,&Rect);
			if(Rect.right-Rect.left<Size.cx) {
				RectScreenToClient(hWnd,&Rect);
				cy = Size.cx - (Rect.right - Rect.left);
				MoveWindow(MsgHWnd,Rect.left,Rect.top,Size.cx,Rect.bottom-Rect.top,FALSE);
				GetWindowRect(hWnd,&Rect);
				MoveWindow(hWnd,Rect.left - cy / 2, Rect.top, Rect.right - Rect.left + cy, Rect.bottom-Rect.top,FALSE);
				}
			}
		SelectObject(hDC, hGdiOld);
		ReleaseDC(hWnd,hDC);
		SetDlgItemText(hWnd,IDC_PROCESS_MSG,pEvt->m_ProcName);
		SetDlgItemText(hWnd,IDC_FUNC_MSG,FuncName);
		SetDlgItemText(hWnd,IDC_OBJ_MSG,pObjName);

		if(State->Time) {
			H=State->Time%3600;
			wsprintf(Timeout,"%02d:%02d:%02d",State->Time/3600,H/60,H%60);
			SetDlgItemText(hWnd,IDC_TIMEOUT,Timeout);
			SetTimer(hWnd,2,1000/*ms*/,NULL);
		} else {
			SetDlgItemText(hWnd,IDC_TIMEOUT,"");
		}

		InitUDCtl(hWnd,IDC_APPL_TIME,IDC_APPL_TIME+30000,999,0,5,hInst);
		CheckDlgButton(hWnd,IDC_APPL_THISTIME,BST_CHECKED);
		CheckDlgButton(hWnd,IDC_APPL_PRC_THIS,BST_CHECKED);
		CheckDlgButton(hWnd,IDC_APPL_OBJ_THIS,BST_CHECKED);
		CheckDlgButton(hWnd,IDC_APPL_LOG,BST_UNCHECKED);
		ApplEnableWin(hWnd,FALSE);

//		if(OSVer.dwPlatformId!=VER_PLATFORM_WIN32_NT) {
//			EnableWindow(GetDlgItem(hWnd,IDC_KILL),FALSE);
//		}
		ShowWindow(hWnd,SW_SHOW);
		break;
	case WM_TIMER:
		State=(PSTATE)TlsGetValue(ReqestTlsIndex);
		if(--State->Time==0)
			PostQuitMessage(3);
		H=State->Time%3600;
		wsprintf(Timeout,"%02d:%02d:%02d",State->Time/3600,H/60,H%60);
		SetDlgItemText(hWnd,IDC_TIMEOUT,Timeout);
		TlsSetValue(ReqestTlsIndex,State); 
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		State=(PSTATE)TlsGetValue(ReqestTlsIndex);
		LocalFree(State);
		TlsSetValue(ReqestTlsIndex,NULL); 
		KillTimer(hWnd,2);
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		State=(PSTATE)TlsGetValue(ReqestTlsIndex);
		switch (LOWORD (wParam)) {
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
				ApplEnableWin(hWnd,TRUE);
			return TRUE;
		case IDC_PASS:
			SetVerdict.m_Verdict=Verdict_Pass;
			break;
		case IDC_DISCARD:
			SetVerdict.m_Verdict=Verdict_Discard;
			break;
		case IDC_KILL:
			SetVerdict.m_Verdict=Verdict_Kill;
			break;
		case IDC_APPLY_FOR:
			GetWindowRect(hWnd,&Rect);
			OnBaseUnits=(ApplySize*HIWORD(GetDialogBaseUnits()))/8;
			if(State->bAppl) {
				MoveWindow(hWnd,Rect.left,Rect.top,Rect.right-Rect.left,Rect.bottom-Rect.top-OnBaseUnits,TRUE);
				SetWindowText(GetDlgItem(hWnd,IDC_APPLY_FOR),"Advanced &>>");
				State->bAppl=FALSE;
			} else {
				MoveWindow(hWnd,Rect.left,Rect.top,Rect.right-Rect.left,Rect.bottom-Rect.top+OnBaseUnits,TRUE);
				SetWindowText(GetDlgItem(hWnd,IDC_APPLY_FOR),"Advanced &<<");
				State->bAppl=TRUE;
			}
			TlsSetValue(ReqestTlsIndex,State); 
			return TRUE;
		default:
			return FALSE;
		}
		if(IsDlgButtonChecked(hWnd,IDC_APPL_THISTIME)==BST_UNCHECKED) {
			if(IsDlgButtonChecked(hWnd,IDC_APPL_TIMED)==BST_CHECKED) {
				GetSystemTimeAsFileTime(&STasFT);
				ExpTime.LowPart=STasFT.dwLowDateTime;
				ExpTime.HighPart=STasFT.dwHighDateTime;
				GetDlgItemText(hWnd,IDC_APPL_TIME,(LPSTR)Timeout,10);
				STasFT.dwLowDateTime=atol(Timeout);
				ExpTime.QuadPart=ExpTime.QuadPart+(LONGLONG)STasFT.dwLowDateTime*600000000;
			} else {
				ExpTime.QuadPart=0;
			}
			if(IsDlgButtonChecked(hWnd,IDC_APPL_PRC_THIS)==BST_CHECKED)
				AplPrc=pEvt->m_ProcName;
			else
				AplPrc=szAll;
			if(IsDlgButtonChecked(hWnd,IDC_APPL_OBJ_THIS)==BST_CHECKED)
				AplObj=pObjName;
			else
				AplObj=szAll;
			AplFlg=0;
			if(IsDlgButtonChecked(hWnd,IDC_APPL_LOG)==BST_CHECKED)
				AplFlg|=FLT_A_LOG;
			if(SetVerdict.m_Verdict==Verdict_Discard)
				AplFlg|=FLT_A_DENY;

			/*FilterTransmit.m_AppID = Interface_ID;
			FilterTransmit.m_FltCtl = FLTCTL_ADD;
			FilterTransmit.m_Expiration = ExpTime.QuadPart;
			FilterTransmit.m_Flags = AplFlg;
			FilterTransmit.m_Function = pEvt->m_Function;
			strncpy(FilterTransmit.m_ObjName, AplObj, MAXPATHLEN);
			strncpy(FilterTransmit.m_ProcName, AplPrc, PROCNAMELEN);
			FilterTransmit.m_ParamsCount = 0;
			FilterTransmit.m_Timeout = DEFAULT_TIMEOUT;
			DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, &FilterTransmit, sizeof(FILTER_TRANSMIT), &FilterTransmit, sizeof(FILTER_TRANSMIT), &BytesRet,NULL);
//			SetFilter(FLTCTL_ADD,AplObj,AplPrc,DEFAULT_TIMEOUT,AplFlg,pEvt->m_Function,pEvt->m_Param,pEvt->m_Value,ExpTime.QuadPart);
*/
		}
		SetVerdict.m_Mark=(unsigned)hWnd;
		SetVerdict.m_AppID = Interface_ID;
		DeviceIoControl(hDevice,IOCTLHOOK_SetVerdict,&SetVerdict,sizeof(SET_VERDICT),NULL,0,&BytesRet,NULL);
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

unsigned __stdcall RequestDlg(void* ahDlg)
{
MSG msg;
HWND hRDlg;
	hRDlg=CreateDialogParam(hInst,MAKEINTRESOURCE(IDD_REQUEST_DLG),NULL,(DLGPROC) ReqestDlgProc,(LONG)0);
	if(!hRDlg) {
		#ifdef _DEBUG
		OutputDebugString("HOOK-FACE: CreateRequestDlg failed\n");
		#endif //_DEBUG
		return 0;
	}
	while(GetMessage(&msg,NULL,0,0)) 
		if (!IsDialogMessage (hRDlg,&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	DestroyWindow(hRDlg);
	return 0;
}

unsigned __stdcall EventWaiterThread(void* ahWnd)
{
HWND			hWnd=(HWND)ahWnd;
unsigned	thrdaddr;
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	while(WaitForSingleObject(hWhistleup,INFINITE)!=WAIT_FAILED) {
		#ifdef _DEBUG
			OutputDebugString("HOOK-FACE: hWhistleup waited\n");
		#endif //_DEBUG
		SetSmile(hWnd,IDI_ICON3);
		InterlockedIncrement(&OldQLen);
		if(!_beginthreadex(NULL,0,RequestDlg,(void*)0,0,&thrdaddr)) {
			#ifdef _DEBUG
			OutputDebugString("HOOK-FACE: CreateThread failed\n");
			#endif //_DEBUG
		}

	}
	#ifdef _DEBUG
		OutputDebugString("HOOK-FACE: WaitForSingleObject failed\n");
	#endif //_DEBUG
	return 0;
}

DWORD GetListCurrentData(HWND hWList)
{
	DWORD result = -1;
	DWORD idx = SendMessage(hWList, CB_GETCURSEL, 0, 0);
	if (idx != CB_ERR)
		result = SendMessage(hWList, CB_GETITEMDATA , idx, 0);
	return result;
}

VOID FillFunctionsList(HWND hWList,CHAR  **FuncNameArr,DWORD FuncNameArrSize,DWORD CurrentPos)
{
DWORD i;
DWORD pos = 0;
DWORD idx = 0;
	SendMessage(hWList,CB_RESETCONTENT,0,0);
	if(FuncNameArr)
	{
		for(i=0;i<FuncNameArrSize;i++)
		{
			if (strcmp(FuncNameArr[i], NullStr) != 0) // hide empty string
			{
				idx = SendMessage(hWList,CB_ADDSTRING,0,(LPARAM)FuncNameArr[i]);
				SendMessage(hWList,CB_SETITEMDATA, idx, i);
				if (CurrentPos == i)
					pos = idx;
			}
		}
		SendMessage(hWList,CB_SETCURSEL,pos,0);
	}
}

VOID FillFuncList(HWND hFuncList,DWORD Func, DWORD FuncMj, DWORD FuncMi)
{
DWORD FuncNameArrSize;
CHAR  **FuncNameArr;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		FuncNameArrSize=SIZEOF(NTFunctionTypeName);
		FuncNameArr=NTFunctionTypeName;
	} else {
		FuncNameArrSize=SIZEOF(VxDFunctionTypeName);
		FuncNameArr=VxDFunctionTypeName;
	}
	FillFunctionsList(hFuncList,FuncNameArr,FuncNameArrSize,Func);
}

VOID FillMajList(HWND hMajList,DWORD Func, DWORD FuncMj, DWORD FuncMi)
{
DWORD FuncNameArrSize;
CHAR  **FuncNameArr=NULL;;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(Func) {
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
//		default:
//			FuncNameArr=NULL;
		}
	}
	FillFunctionsList(hMajList,FuncNameArr,FuncNameArrSize,FuncMj);
	EnableWindow(hMajList,(BOOL)FuncNameArr);
}

VOID FillMinList(HWND hMinList,DWORD Func, DWORD FuncMj, DWORD FuncMi)
{
DWORD FuncNameArrSize;
CHAR  **FuncNameArr=NULL;;
DWORD MjFn=FuncMj;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
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
//		case FLTTYPE_IOS:
//		case FLTTYPE_REGS:
//		default:
//			FuncNameArr=NULL;
		}
	}
	FillFunctionsList(hMinList,FuncNameArr,FuncNameArrSize,FuncMi);
	EnableWindow(hMinList,(BOOL)FuncNameArr);
}

BOOL FillParamList(HWND hdlg,DWORD Func, DWORD FuncMj, DWORD FuncMi, DWORD Param)
{
DWORD FuncNameArrSize;
CHAR  **FuncNameArr=NULL;
BOOL ChckPar;
HWND hParamList=GetDlgItem(hdlg,IDC_PARAM_NAME);
DWORD MjFn=FuncMj;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(Func) {
		case FLTTYPE_DISK:
			switch(MjFn) {
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
	} else {
		switch(Func) {
		case FLTTYPE_IOS:
			switch(MjFn) {
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
	FillFunctionsList(hParamList,FuncNameArr,FuncNameArrSize,Param);
	EnableWindow(GetDlgItem(hdlg,IDC_CHECKPARAM),(BOOL)FuncNameArr);
	ChckPar=FALSE;
	if(FuncNameArr) {
		if(IsDlgButtonChecked(hdlg,IDC_CHECKPARAM)==BST_CHECKED)
			ChckPar=TRUE;
	} else {
		CheckDlgButton(hdlg,IDC_CHECKPARAM,BST_UNCHECKED);
		SetDlgItemText(hdlg,IDC_PARAM_VALUE,"0");
	}
	EnableWindow(hParamList,ChckPar);
	EnableWindow(GetDlgItem(hdlg,IDC_PARAM_OPER),ChckPar);
	EnableWindow(GetDlgItem(hdlg,IDC_PARAM_VALUE),ChckPar);
	return (BOOL)FuncNameArr;
}


BOOL GetEventData(DWORD FltCtl,HWND hdlg)
{
/*
FILTER_TRANSMIT	Filter;
SYSTEMTIME			ExpirST;
FILETIME				LocalT;
CHAR	Timeout[128];
CHAR	Time[64];
CHAR	Date[64];
DWORD BytesRet;
BOOL IsSystem;
DWORD ChckPar;
	DeviceIoControl(hDevice,IOCTLHOOK_FiltersCtl,&FltCtl,sizeof(FltCtl),&Filter,sizeof(FILTER_TRANSMIT),&BytesRet,NULL);
	SetDlgItemText(hdlg,IDC_FILEMASKEDIT,(LPSTR)Filter.FileName);
	SetDlgItemText(hdlg,IDC_PROCESSMASKEDIT,(LPSTR)Filter.Process);
	SetDlgItemText(hdlg,IDC_TIMEOUT,_itoa(Filter.Timeout,Time,10));
	CheckDlgButton(hdlg,IDC_SKIPBOX,Filter.Flags & FLT_A_DENY);
	CheckDlgButton(hdlg,IDC_SHOWPROMPT,Filter.Flags & FLT_A_POPUP);
	CheckDlgButton(hdlg,IDC_WRITETOLOG,Filter.Flags & FLT_A_LOG);
	FillFuncList(GetDlgItem(hdlg,IDC_FNLIST),Filter.Function);
	FillMajList(GetDlgItem(hdlg,IDC_MAJFNLIST),Filter.Function);
	FillMinList(GetDlgItem(hdlg,IDC_MINFNLIST),Filter.Function);
	if(Filter.Param!=FLT_PARAM_DONT_CHECK) {
		ChckPar=BST_CHECKED;
		SendMessage(GetDlgItem(hdlg,IDC_PARAM_OPER),CB_SETCURSEL,((Filter.Param & FLT_PARAM)>>24)-1,0);
		SetDlgItemText(hdlg,IDC_PARAM_VALUE,_itoa(Filter.Value,Time,10));
	} else {
		ChckPar=BST_UNCHECKED;
	}
	CheckDlgButton(hdlg,IDC_CHECKPARAM,ChckPar);
	FillParamList(hdlg,Filter.Function,Filter.Param & ~FLT_PARAM);

	EnableWindow(GetDlgItem(hdlg,IDC_NEXT),Filter.FltCtl&FLTCTL_NEXT);
	EnableWindow(GetDlgItem(hdlg,IDC_PREV),Filter.FltCtl&FLTCTL_PREV);
	IsSystem=!(Filter.Flags&FLT_SYSTEM);
	EnableWindow(GetDlgItem(hdlg,IDC_DELFLT),IsSystem);
	if(Filter.Expiration) {
		FileTimeToLocalFileTime((FILETIME*)&Filter.Expiration,&LocalT);
		FileTimeToSystemTime(&LocalT,&ExpirST);
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,&ExpirST,NULL,Date,sizeof(Date));
		GetTimeFormat(LOCALE_USER_DEFAULT,LOCALE_NOUSEROVERRIDE,&ExpirST,NULL,Time,sizeof(Time));
		wsprintf(Timeout,"This filter will be expired %s at %s",Date,Time);
		SetDlgItemText(hdlg,IDC_EXPIRATION_DATE,Timeout);
		SendMessage(GetDlgItem(hdlg,IDC_FILEMASKEDIT),EM_SETREADONLY,TRUE,0);
		SendMessage(GetDlgItem(hdlg,IDC_PROCESSMASKEDIT),EM_SETREADONLY,TRUE,0);
		EnableWindow(GetDlgItem(hdlg,IDC_FNLIST),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_MAJFNLIST),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_MINFNLIST),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_CHECKPARAM),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_PARAM_NAME),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_PARAM_OPER),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_PARAM_VALUE),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_SKIPBOX),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_SHOWPROMPT),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_WRITETOLOG),FALSE);
		EnableWindow(GetDlgItem(hdlg,IDC_TIMEOUT),FALSE);
	} else {
		SetDlgItemText(hdlg,IDC_EXPIRATION_DATE,NullStr);
		SendMessage(GetDlgItem(hdlg,IDC_FILEMASKEDIT),EM_SETREADONLY,!IsSystem,0);
		SendMessage(GetDlgItem(hdlg,IDC_PROCESSMASKEDIT),EM_SETREADONLY,!IsSystem,0);
		EnableWindow(GetDlgItem(hdlg,IDC_FNLIST),IsSystem);
		EnableWindow(GetDlgItem(hdlg,IDC_PARAM_NAME),ChckPar=IsSystem && (ChckPar==BST_CHECKED));
		EnableWindow(GetDlgItem(hdlg,IDC_PARAM_OPER),ChckPar);
		EnableWindow(GetDlgItem(hdlg,IDC_PARAM_VALUE),ChckPar);
		if(!IsSystem) {
			EnableWindow(GetDlgItem(hdlg,IDC_CHECKPARAM),IsSystem);
			EnableWindow(GetDlgItem(hdlg,IDC_MAJFNLIST),IsSystem);
			EnableWindow(GetDlgItem(hdlg,IDC_MINFNLIST),IsSystem);
		}
		EnableWindow(GetDlgItem(hdlg,IDC_SKIPBOX),TRUE);
		EnableWindow(GetDlgItem(hdlg,IDC_SHOWPROMPT),TRUE);
		EnableWindow(GetDlgItem(hdlg,IDC_WRITETOLOG),TRUE);
		EnableWindow(GetDlgItem(hdlg,IDC_TIMEOUT),TRUE);
	}
	SendMessage(GetParent(hdlg),PSM_UNCHANGED,(WPARAM)hdlg,0l);
*/
	return TRUE;
}

BOOL SetEventData(HWND hdlg)
{
/*
FILTER_TRANSMIT Filter;
DWORD Fn,MajF,MinF,BytesRet;
CHAR	Timeout[128];
	Timeout[0]=0;
	GetDlgItemText(hdlg,IDC_EXPIRATION_DATE,Timeout,sizeof(Timeout));
	if(strlen(Timeout))
		return FALSE;
	ZeroMemory(&Filter,sizeof(Filter));
	GetDlgItemText(hdlg,IDC_FILEMASKEDIT,(LPSTR)Filter.FileName,MAXPATHLEN);
	GetDlgItemText(hdlg,IDC_PROCESSMASKEDIT,(LPSTR)Filter.Process,PROCNAMELEN);
	GetDlgItemText(hdlg,IDC_TIMEOUT,(LPSTR)Timeout,4);
	Filter.Timeout=atol(Timeout);
	Filter.Flags=0;
	if(IsDlgButtonChecked(hdlg,IDC_SKIPBOX)==BST_CHECKED)
		Filter.Flags|=FLT_A_DENY;
	if(IsDlgButtonChecked(hdlg,IDC_SHOWPROMPT)==BST_CHECKED)
		Filter.Flags|=FLT_A_POPUP;
	if(IsDlgButtonChecked(hdlg,IDC_WRITETOLOG)==BST_CHECKED)
		Filter.Flags|=FLT_A_LOG;
//	Fn=SendMessage(GetDlgItem(hdlg,IDC_FNLIST),CB_GETCURSEL,0,0);
	Fn = GetListCurrentData(GetDlgItem(hdlg,IDC_FNLIST));
//	if((MajF=SendMessage(GetDlgItem(hdlg,IDC_MAJFNLIST),CB_GETCURSEL,0,0))==(DWORD)-1)
	if((MajF=GetListCurrentData(GetDlgItem(hdlg,IDC_MAJFNLIST)))==(DWORD)-1)
		MajF=0;
//	if((MinF=SendMessage(GetDlgItem(hdlg,IDC_MINFNLIST),CB_GETCURSEL,0,0))==(DWORD)-1)
	if((MinF=GetListCurrentData(GetDlgItem(hdlg,IDC_MINFNLIST)))==(DWORD)-1)
		MinF=0;
	Filter.Function=(Fn<<24) | (MajF<<8) | MinF;
	if(IsDlgButtonChecked(hdlg,IDC_CHECKPARAM)==BST_CHECKED) {
//		MajF=SendMessage(GetDlgItem(hdlg,IDC_PARAM_OPER),CB_GETCURSEL,0,0)+1;
		MajF=GetListCurrentData(GetDlgItem(hdlg,IDC_PARAM_OPER)) + 1;
//		if((MinF=SendMessage(GetDlgItem(hdlg,IDC_PARAM_NAME),CB_GETCURSEL,0,0))==(DWORD)-1)
		if((MinF=GetListCurrentData(GetDlgItem(hdlg,IDC_PARAM_NAME)))==(DWORD)-1)
			MinF=0;
		Filter.Param=(MajF<<24) | MinF;
		GetDlgItemText(hdlg,IDC_PARAM_VALUE,(LPSTR)Timeout,10);
		Filter.Value=atol(Timeout);
	} else {
		Filter.Param=FLT_PARAM_DONT_CHECK;
	}
	Filter.FltCtl=FLTCTL_CHNG;
	DeviceIoControl(hDevice,IOCTLHOOK_FiltersCtl,&Filter,sizeof(FILTER_TRANSMIT),NULL,0,&BytesRet,NULL);
*/
	return TRUE;
}


int GetConfigValue(HWND hdlg,HKEY hKey,DWORD dwID)
{
DWORD cbData,Type,dwValueFlag=0;
CHAR szKeyName[128];
	cbData=sizeof(dwValueFlag);
	Type=REG_DWORD;
	_itoa(dwID,szKeyName,10);
	RegQueryValueEx(hKey,szKeyName,NULL,&Type,(LPBYTE)&dwValueFlag,&cbData);
	CheckDlgButton(hdlg,dwID,dwValueFlag & 1);
	if(dwValueFlag )
		EnableWindow(GetDlgItem(hdlg,dwID+1),TRUE);
	if((dwValueFlag & 2) == 2)
		CheckDlgButton(hdlg,dwID+1,1);
	return dwValueFlag ;
}


int GetConfiguration(HWND hdlg)
{
HKEY hAVPGKey=NULL;
HKEY hKey=NULL;
DWORD dwID;
int ret=0;
	__try {
		ZeroMemory(uiBackUpFileOper,sizeof(uiBackUpFileOper));
		ZeroMemory(uiBackUpDirectIOOper,sizeof(uiBackUpDirectIOOper));
		ZeroMemory(uiBackUpRegOper,sizeof(uiBackUpRegOper));

		if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_LOCAL_MACHINE,szAVPGKeyName,0,KEY_READ,&hAVPGKey))
			return ret;
		if(ERROR_SUCCESS!=RegOpenKeyEx(hAVPGKey,szConfigKeyName,0,KEY_READ,&hKey))
			return ret;

		for(dwID=START_FILE_OPERATION;dwID<END_FILE_OPERATION;dwID+=2){
			uiBackUpFileOper[dwID-START_FILE_OPERATION]=GetConfigValue(hdlg,hKey,dwID);
		}
		for(dwID=START_DIRECTIO_OPERATION;dwID<END_DIRECTIO_OPERATION;dwID+=2){
			uiBackUpDirectIOOper[dwID-START_DIRECTIO_OPERATION ]=GetConfigValue(hdlg,hKey,dwID);
		}
		for(dwID=START_REGISTRY_OPERATION;dwID<END_REGISTRY_OPERATION;dwID+=2){
			uiBackUpRegOper[dwID-START_REGISTRY_OPERATION]=GetConfigValue(hdlg,hKey,dwID);
		}
		GetConfigValue(hdlg,hKey,IDC_ADV_MODE);
		return ret;
	}
	__finally {
		if(hKey)
			RegCloseKey(hKey);
		if(hAVPGKey)
			RegCloseKey(hAVPGKey);
	}
	return ret;
}

void SetConfigValue(HWND hdlg,HKEY hKey,DWORD dwID)
{
DWORD cbData,Type,dwValueFlag=0;
CHAR szKeyName[128];
	cbData=sizeof(dwValueFlag);
	Type=REG_DWORD;
	dwValueFlag=IsDlgButtonChecked(hdlg,dwID);
	if(dwValueFlag)
		if(IsDlgButtonChecked(hdlg,dwID+1))
			dwValueFlag+=2;
	_itoa(dwID,szKeyName,10);
	RegSetValueEx(hKey,szKeyName,NULL,Type,(LPBYTE)&dwValueFlag,cbData);
}

int SetConfiguration(HWND hdlg)
{
HKEY hAVPGKey=NULL;
HKEY hKey=NULL;
int ret=0;
DWORD dwID;
	__try {
		if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_LOCAL_MACHINE,szAVPGKeyName,0,KEY_ALL_ACCESS,&hAVPGKey))
			return ret;
		if(ERROR_FILE_NOT_FOUND==RegOpenKeyEx(hAVPGKey,szConfigKeyName,0,KEY_ALL_ACCESS,&hKey)) {
			if(ERROR_SUCCESS!=RegCreateKey(hAVPGKey,szConfigKeyName,&hKey))
				return ret;
		}
		ret=1;
		for(dwID=START_FILE_OPERATION;dwID<END_FILE_OPERATION;dwID+=2)
			SetConfigValue(hdlg,hKey,dwID);
		for(dwID=START_DIRECTIO_OPERATION;dwID<END_DIRECTIO_OPERATION;dwID+=2)
			SetConfigValue(hdlg,hKey,dwID);
		for(dwID=START_REGISTRY_OPERATION;dwID<END_REGISTRY_OPERATION;dwID+=2)
			SetConfigValue(hdlg,hKey,dwID);
		SetConfigValue(hdlg,hKey,IDC_ADV_MODE);
		return ret;
	}
	__finally {
		if(hKey)
			RegCloseKey(hKey);
		if(hAVPGKey)
			RegCloseKey(hAVPGKey);
	}
	return ret;
}

void FileOperFilters(DWORD dwID,DWORD dwOldSettings,DWORD dwNewSettings,DWORD dwOperation)
{
DWORD Fn;
//DWORD Param;
DWORD Value;
DWORD dwFlags=FLT_A_LOG | FLT_A_DENY;
DWORD Fn1 = 0;
BOOL isSecondFnPresent=FALSE;
DWORD Fn2 = 0;
BOOL is3rdFnPresent=FALSE;
//CHAR TmpStr[MAXPATHLEN];
//int i;
//	Param=FLT_PARAM_DONT_CHECK;
	Value=0;
	if(dwNewSettings==3)	//Ask User
		dwFlags|=FLT_A_POPUP;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(dwID) {
		case IDC_WRITE_EXE:
			Fn=FLTTYPE_NFIOR | (IRP_MJ_WRITE<<8) | IRP_MN_NORMAL;
			isSecondFnPresent=TRUE;
			Fn1=FLTTYPE_FIOR | (FastIoWrite<<8);
			break;
		case IDC_DEL_EXE:
			Fn=FLTTYPE_NFIOR | (IRP_MJ_SET_INFORMATION<<8) | FileDispositionInformation;
			break;
		case IDC_RENAME_EXE:
			Fn=FLTTYPE_NFIOR | (IRP_MJ_SET_INFORMATION<<8) | FileRenameInformation;
			break;
		}
	} else {
		switch(dwID) {
		case IDC_WRITE_EXE:
			Fn=FLTTYPE_IFS | (IFSFN_WRITE<<8);
			//!!
/*			Fn1=FLTTYPE_IFS | (IFSFN_OPEN<<8) | 1;
			Fn2=FLTTYPE_IFS | (IFSFN_OPEN<<8) | 2;
			isSecondFnPresent=is3rdFnPresent=TRUE;*/
			break;
		case IDC_DEL_EXE:
			Fn=FLTTYPE_IFS | (IFSFN_DELETE<<8);
			break;
		case IDC_RENAME_EXE:
			/*Fn=FLTTYPE_IFS | (IFSFN_RENAME<<8);
			for(i=0;i<SIZEOF(szExecFileName);i++) {
				lstrcpy(TmpStr,szExecFileName[i]);
				lstrcat(TmpStr,szAll);
				SetFilter(dwOperation,TmpStr,szAll,DEFAULT_TIMEOUT,dwFlags,Fn,Param,Value,0);
			}*/
			return;
		}
	}
/*	for(i=0;i<SIZEOF(szExecFileName);i++) {
		SetFilter(dwOperation,szExecFileName[i],szAll,DEFAULT_TIMEOUT,dwFlags,Fn,Param,Value,0);
		if(isSecondFnPresent)
			SetFilter(dwOperation,szExecFileName[i],szAll,DEFAULT_TIMEOUT,dwFlags,Fn1,Param,Value,0);
		if(is3rdFnPresent)
			SetFilter(dwOperation,szExecFileName[i],szAll,DEFAULT_TIMEOUT,dwFlags,Fn2,Param,Value,0);
	}*/
}
//!!
/*
void CheckPartTable(DWORD dwOperation,DWORD dwFlags,PCHAR DriveStr,UCHAR Drive,UCHAR Head,UCHAR Sector,ULONG Cylinder,ULONG RelSec)
{
DWORD i;
PREAD_INT13 pI13Req;
MBOOT	*PartTable;
	if(PartTable=(MBOOT*)LocalAlloc(0,sizeof(MBOOT))) {
		if(pI13Req=(PREAD_INT13)LocalAlloc(0,sizeof(PREAD_INT13))) {
			pI13Req->Drive=Drive;
			pI13Req->Len=1;
			pI13Req->Cylinder=Cylinder;
			pI13Req->Head=Head;
			pI13Req->Sector=Sector;
			if(DeviceIoControl(hDevice,IOCTLHOOK_ReadInt13,pI13Req,sizeof(READ_INT13),PartTable,sizeof(MBOOT),&i,NULL)) {
// А сигнатуру Пушкин проверять будет???
				for(i=0;i<4;i++) {
					if(PartTable->part_table[i].rel_sec) {
						SetFilter(dwOperation,DriveStr,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_IOS | (IOR_WRITE<<8),FLT_PARAM_DW_EUA,PartTable->part_table[i].rel_sec+RelSec,0);
						SetFilter(dwOperation,DriveStr,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_IOS | (IOR_WRITEV<<8),FLT_PARAM_DW_EUA,PartTable->part_table[i].rel_sec+RelSec,0);
					}
					if(PartTable->part_table[i].sys==5 || PartTable->part_table[i].sys==0x0f)
						CheckPartTable(dwOperation,dwFlags,DriveStr,Drive,PartTable->part_table[i].beg_head,PartTable->part_table[i].beg_sec & 0x3f,
							(((ULONG)(PartTable->part_table[i].beg_sec & 0xc0))<<2) + PartTable->part_table[i].beg_cyl,RelSec+PartTable->part_table[i].rel_sec);
				}
			}
			LocalFree(pI13Req);
		}
		LocalFree(PartTable);
	}
}

void DirectIoOperFilters(DWORD dwID,DWORD dwOldSettings,DWORD dwNewSettings,DWORD dwOperation)
{
DWORD Fn;
DWORD Param,Value;
DWORD dwFlags=FLT_A_LOG | FLT_A_DENY;
DWORD Fn1;
DWORD Param1,Value1;
BOOL isSecondFnPresent=FALSE;
LPSTR lpFileName;
HANDLE hDrive;
DRIVE_LAYOUT_INFORMATION *dl=NULL;
DWORD dwRet;
CHAR	TmpStr[MAXPATHLEN];
DWORD	i,j;
	if(dwNewSettings==3)	//Ask User
		dwFlags|=FLT_A_POPUP;
	Param=Param1=FLT_PARAM_DONT_CHECK;
	Value=Value1=0;
	lpFileName=szAll;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(dwID) {
		case IDC_MBR_WRITE:
//			SetFilter(dwOperation,szAll,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_DISK | (IRP_MJ_DEVICE_CONTROL<<8) | ((IOCTL_DISK_DELETE_DRIVE_LAYOUT & 0x1ffc)>>2),FLT_PARAM_DONT_CHECK,0,0);
			SetFilter(dwOperation,szAll,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_DISK | (IRP_MJ_DEVICE_CONTROL<<8) | ((IOCTL_DISK_SET_DRIVE_LAYOUT & 0x1ffc)>>2),FLT_PARAM_DONT_CHECK,0,0);
			SetFilter(dwOperation,szAll,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_DISK | (IRP_MJ_DEVICE_CONTROL<<8) | ((IOCTL_DISK_SET_PARTITION_INFO & 0x1ffc)>>2),FLT_PARAM_DONT_CHECK,0,0);
			SetFilter(dwOperation,szAll,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_DISK | (IRP_MJ_WRITE<<8),FLT_PARAM_DW_EUA,0,0);
			return;
		case IDC_WRITE_BOOT: //здесь только вычисляемые сектора
			for(i=0;i<16;i++) {
				wsprintf(TmpStr,"\\\\.\\PhysicalDrive%d",i);
				if((hDrive=CreateFile(TmpStr,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
					continue;
				if(QueryDosDevice(TmpStr+4,TmpStr,sizeof(TmpStr))) {
					if((dl=(DRIVE_LAYOUT_INFORMATION*)LocalAlloc(0,4096))) {
						if(DeviceIoControl(hDrive,IOCTL_DISK_GET_DRIVE_LAYOUT,NULL,0,dl,4096,&dwRet,NULL)) {
							for(j=0;j<dl->PartitionCount;j++) {
								if(dwRet=(DWORD)(dl->PartitionEntry[j].StartingOffset.QuadPart/512))
									SetFilter(dwOperation,TmpStr,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_DISK | (IRP_MJ_WRITE<<8),FLT_PARAM_DW_EUA,dwRet,0);
							}
						}
					}
				}
				if(dl)
					LocalFree(dl);
				CloseHandle(hDrive);
			}
			return;
		case IDC_FORMAT_HDD:
			Fn=FLTTYPE_DISK | (IRP_MJ_DEVICE_CONTROL<<8) | ((IOCTL_DISK_FORMAT_TRACKS & 0x1ffc)>>2);
			isSecondFnPresent=TRUE;
			Fn1=FLTTYPE_DISK | (IRP_MJ_DEVICE_CONTROL<<8) | ((IOCTL_DISK_FORMAT_TRACKS_EX & 0x1ffc)>>2);
			break;
		}
	} else {
		switch(dwID) {
		case IDC_MBR_WRITE:
			SetFilter(dwOperation,szAll,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_IOS | (IOR_WRITE<<8),FLT_PARAM_DW_EUA,0,0);
			SetFilter(dwOperation,szAll,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_IOS | (IOR_WRITEV<<8),FLT_PARAM_DW_EUA,0,0);
			return;
		case IDC_WRITE_BOOT:
//			SetFilter(dwOperation,szAll,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_IFS | (IFSFN_DASDIO<<8) | DIO_ABS_WRITE_SECTORS,FLT_PARAM_DONT_CHECK,0,0);
			for(i=0;i<16;i++) {
				wsprintf(TmpStr,"HD%d",i);
				CheckPartTable(dwOperation,dwFlags,TmpStr,(UCHAR)(i+0x80),0,1,0,0);
			}
			return;
		case IDC_FORMAT_HDD:
			Fn=FLTTYPE_IOS | (IOR_FORMAT<<8);
			isSecondFnPresent=TRUE;
			Fn1=FLTTYPE_IOS | (IOR_GEN_IOCTL<<8);
			Param1=FLT_PARAM_DW_EUA;
			Value1=0x42;
			break;
		}
	}
	SetFilter(dwOperation,lpFileName,szAll,DEFAULT_TIMEOUT,dwFlags,Fn,Param,Value,0);
	if(isSecondFnPresent)
		SetFilter(dwOperation,lpFileName,szAll,DEFAULT_TIMEOUT,dwFlags,Fn1,Param1,Value1,0);
}

void RegOperFilters(DWORD dwID,DWORD dwOldSettings,DWORD dwNewSettings,DWORD dwOperation)
{
DWORD Fn;
DWORD Param,Value;
DWORD dwFlags=FLT_A_LOG | FLT_A_DENY;
LPSTR lpFileName;
	Param=FLT_PARAM_DONT_CHECK;
	Value=0;
	if(dwNewSettings==3)	//Ask User
		dwFlags|=FLT_A_POPUP;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		switch(dwID) {
		case IDC_REG_WRITE:
			Fn=FLTTYPE_REGS | (SetValueKey<<8);
			lpFileName=RegWriteRunNT;
			break;
		case IDC_REG_CREATE:
			Fn=FLTTYPE_REGS | (CreateKey<<8);
			lpFileName=RegCreateServNT;
			break;
		}
	} else {
		switch(dwID) {
		case IDC_REG_WRITE:
			SetFilter(dwOperation,RegWriteRunVxD,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_REGS | (SetValueExVxD<<8),Param,Value,0);
			SetFilter(dwOperation,RegWriteRunVxD,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_REGS | (SetValueVxD<<8),Param,Value,0);
			SetFilter(dwOperation,RegWriteRunVxDU,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_REGS | (SetValueExVxD<<8),Param,Value,0);
			SetFilter(dwOperation,RegWriteRunVxDU,szAll,DEFAULT_TIMEOUT,dwFlags,FLTTYPE_REGS | (SetValueVxD<<8),Param,Value,0);
			return;
		case IDC_REG_CREATE:
			Fn=FLTTYPE_REGS | (CreateKeyVxD<<8);
			lpFileName=RegCreateVxD;
			break;
		}
	}
	SetFilter(dwOperation,lpFileName,szAll,DEFAULT_TIMEOUT,dwFlags,Fn,Param,Value,0);
}
*/
void CompareConfigurations(HWND hdlg)
{
/*
int dwID;
DWORD dwValueFlag;
DWORD dwOperation;
	for(dwID=START_FILE_OPERATION;dwID<END_FILE_OPERATION;dwID+=2) {
		dwValueFlag=IsDlgButtonChecked(hdlg,dwID);
		if(dwValueFlag)
			if(IsDlgButtonChecked(hdlg,dwID+1))
				dwValueFlag+=2;
		if(dwValueFlag!=uiBackUpFileOper[dwID-START_FILE_OPERATION]) {
			if(dwValueFlag==0)	//Remove filter
				dwOperation=FLTCTL_FDEL;
			else 
				dwOperation=FLTCTL_ADD;
			FileOperFilters(dwID,uiBackUpFileOper[dwID-START_FILE_OPERATION],dwValueFlag,dwOperation);
		}
	}
	for(dwID=START_DIRECTIO_OPERATION;dwID<END_DIRECTIO_OPERATION;dwID+=2) {
		dwValueFlag=IsDlgButtonChecked(hdlg,dwID);
		if(dwValueFlag)
			if(IsDlgButtonChecked(hdlg,dwID+1))
				dwValueFlag+=2;
		if(dwValueFlag!=uiBackUpDirectIOOper[dwID-START_DIRECTIO_OPERATION]) {
			if(dwValueFlag==0)	//Remove filter
				dwOperation=FLTCTL_FDEL;
			else 
				dwOperation=FLTCTL_ADD;
			DirectIoOperFilters(dwID,uiBackUpDirectIOOper[dwID-START_DIRECTIO_OPERATION],dwValueFlag,dwOperation);
		}
	}
	for(dwID=START_REGISTRY_OPERATION;dwID<END_REGISTRY_OPERATION;dwID+=2) {
		dwValueFlag=IsDlgButtonChecked(hdlg,dwID);
		if(dwValueFlag)
			if(IsDlgButtonChecked(hdlg,dwID+1))
				dwValueFlag+=2;
		if(dwValueFlag!=uiBackUpRegOper[dwID-START_REGISTRY_OPERATION]) {
			if(dwValueFlag==0)	//Remove filter
				dwOperation=FLTCTL_FDEL;
			else 
				dwOperation=FLTCTL_ADD;
			RegOperFilters(dwID,uiBackUpRegOper[dwID-START_REGISTRY_OPERATION],dwValueFlag,dwOperation);
		}
	}
*/
}

BOOL IsAdvancedModeOn(void)
{
HKEY hAVPGKey=NULL;
HKEY hKey=NULL;
int ret=0;
	__try {
		if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_LOCAL_MACHINE,szAVPGKeyName,0,KEY_READ,&hAVPGKey))
			return FALSE;
		if(ERROR_SUCCESS!=RegOpenKeyEx(hAVPGKey,szConfigKeyName,0,KEY_READ,&hKey))
			return FALSE;
		return GetConfigValue(NULL,hKey,IDC_ADV_MODE);
	}
	__finally{
		if(hKey)
			RegCloseKey(hKey);
		if(hAVPGKey)
			RegCloseKey(hAVPGKey);
	}
	return TRUE;
}

void FillInPropertyPage( PROPSHEETPAGE* psp, LPCSTR idDlg, LPCSTR pszProc, DLGPROC pfnDlgProc,HINSTANCE hInsts)
{
	psp->dwSize = sizeof(PROPSHEETPAGE);
	psp->dwFlags = PSP_USETITLE|PSP_HASHELP;
	psp->hInstance = hInsts;
	psp->pszTemplate = idDlg;
	psp->pszIcon = NULL;
	psp->pfnDlgProc = pfnDlgProc;
	psp->pszTitle = pszProc;
	psp->lParam = 0;
}

__declspec(dllexport)BOOL CALLBACK MainDlgProc(HWND hWnd, UINT iMsg, UINT wParam, LONG lParam)
{
//DWORD MajF,Fn;
//BOOL ChckPar;
	switch (iMsg) {
//		case WM_HELP:
//			WinHelp(hDlg,szHelpFileName,HELP_CONTEXT,CONFIGDIALOGAVP_TOPIC);
//			return TRUE;
		case WM_NOTIFY: {
			LPNMHDR lpnmhdr;
			lpnmhdr = (NMHDR FAR *)lParam;
			switch (lpnmhdr->code) {
//				case PSN_HELP:
//					WinHelp(hDlg,szHelpFileName,HELP_CONTEXT,CONFIGDIALOGAVP_TOPIC);
//					return TRUE;
				case PSN_APPLY:
					if(PropSheet_GetCurrentPageHwnd(lpnmhdr->hwndFrom)==hWnd)
						SetEventData(hWnd);
					return TRUE;
				case PSN_SETACTIVE:
//					GetEventData(FLTCTL_CURR,hWnd); // !!!!!!!!!!!!!!!!
					return TRUE;
 
			}
			break;
		}
		case WM_INITDIALOG:
			InitUDCtl(hWnd,IDC_TIMEOUT,IDC_TIMEOUT+30000,999,0,0,hInst);
			FillFunctionsList(GetDlgItem(hWnd,IDC_PARAM_OPER),ParamOperStr,SIZEOF(ParamOperStr),0);
//			GetEventData(FLTCTL_CURR,hWnd);//!!!!!!!!!!!!!!!!!!!
			return TRUE;
		case WM_COMMAND:
			/*switch (wID) {
				case IDC_TIMEOUT:
				case IDC_FILEMASKEDIT:
				case IDC_PROCESSMASKEDIT:
				case IDC_PARAM_VALUE:
					if(GET_WM_COMMAND_CMD(wParam, lParam)==EN_CHANGE)
						goto ChangeState;
					break;
				case IDC_SKIPBOX:
				case IDC_SHOWPROMPT:
				case IDC_WRITETOLOG:
					if(GET_WM_COMMAND_CMD(wParam, lParam)==BN_CLICKED)
						goto ChangeState;
					break;
				case IDC_CHECKPARAM:
					if(GET_WM_COMMAND_CMD(wParam, lParam)==BN_CLICKED) {
						if(IsDlgButtonChecked(hWnd,IDC_CHECKPARAM)==BST_CHECKED)
							ChckPar=TRUE;
						else
							ChckPar=FALSE;
						EnableWindow(GetDlgItem(hWnd,IDC_PARAM_NAME),ChckPar);
						EnableWindow(GetDlgItem(hWnd,IDC_PARAM_OPER),ChckPar);
						EnableWindow(GetDlgItem(hWnd,IDC_PARAM_VALUE),ChckPar);
						goto ChangeState;
					}
					break;
				case IDC_FNLIST:
					if(GET_WM_COMMAND_CMD(wParam, lParam)==CBN_SELCHANGE) {
//						Fn=SendMessage((HWND) lParam,CB_GETCURSEL,0,0)<<24;
						Fn=GetListCurrentData((HWND) lParam)<<24;
						FillMajList(GetDlgItem(hWnd,IDC_MAJFNLIST),Fn);
						FillMinList(GetDlgItem(hWnd,IDC_MINFNLIST),Fn);
						FillParamList(hWnd,Fn,0);
						goto ChangeState;
					}
					break;
				case IDC_MAJFNLIST:
					if(GET_WM_COMMAND_CMD(wParam, lParam)==CBN_SELCHANGE) {
//						Fn=SendMessage(GetDlgItem(hWnd,IDC_FNLIST),CB_GETCURSEL,0,0);
						Fn=GetListCurrentData(GetDlgItem(hWnd,IDC_FNLIST));
//						if((MajF=SendMessage((HWND) lParam,CB_GETCURSEL,0,0))==(DWORD)-1)
						if((MajF=GetListCurrentData((HWND) lParam))==(DWORD)-1)
							MajF=0;
						Fn=(Fn<<24) | (MajF<<8);
						FillMinList(GetDlgItem(hWnd,IDC_MINFNLIST),Fn);
						FillParamList(hWnd,Fn,0);
						goto ChangeState;
					}
					break;
				case IDC_MINFNLIST:
					if(GET_WM_COMMAND_CMD(wParam, lParam)==CBN_SELCHANGE) {
//						Fn=SendMessage(GetDlgItem(hWnd,IDC_FNLIST),CB_GETCURSEL,0,0);
						Fn=GetListCurrentData(GetDlgItem(hWnd,IDC_FNLIST));
//						if((MajF=SendMessage(GetDlgItem(hWnd,IDC_MAJFNLIST),CB_GETCURSEL,0,0))==(DWORD)-1)
						if((MajF=GetListCurrentData(GetDlgItem(hWnd,IDC_MAJFNLIST)))==(DWORD)-1)
							MajF=0;
						Fn=(Fn<<24) | (MajF<<8);
//						if((MajF=SendMessage((HWND) lParam,CB_GETCURSEL,0,0))==(DWORD)-1)
						if((MajF=GetListCurrentData((HWND) lParam))==(DWORD)-1)
							MajF=0;
						Fn=Fn | MajF;
						FillParamList(hWnd,Fn,0);
						goto ChangeState;
					}
					break;
				case IDC_PARAM_NAME:
				case IDC_PARAM_OPER:
					if(GET_WM_COMMAND_CMD(wParam, lParam)==CBN_SELCHANGE)
						goto ChangeState;
					break;
				case IDC_NEXT:
					GetEventData(FLTCTL_NEXT,hWnd);
					break;
				case IDC_PREV:
//					GetEventData(FLTCTL_PREV,hWnd); //!!!!!!!!!!!!!!!!!!!!!!
					break;
				case IDC_ADDFLT:
					SetEventData(hWnd);
//					GetEventData(FLTCTL_NEW,hWnd);//!!!!!!!!!!!!!!!!!!!!!!!!
					SetFocus(GetDlgItem(hWnd,IDC_FILEMASKEDIT));
					goto ChangeState;
				case IDC_DELFLT:
					GetEventData(FLTCTL_DEL,hWnd);
					break;
			}*/
			break;
		default:
			break;
	}
	return FALSE;
/*ChangeState:
	SendMessage(GetParent(hWnd),PSM_CHANGED,(WPARAM)hWnd,0l);
	return TRUE;*/
}
__declspec(dllexport)BOOL CALLBACK DumpDlgProc(HWND hdlg, UINT iMsg, UINT wParam, LONG lParam)
{
BOOL Flag;
HPROPSHEETPAGE hpp;
static HWND hPropDlg;
PROPSHEETPAGE psp;
CHAR Caption2[64];

	switch (iMsg) {
//		case WM_HELP:
//			WinHelp(hDlg,szHelpFileName,HELP_CONTEXT,CONFIGDIALOGAVP_TOPIC);
//			return TRUE;
		case WM_NOTIFY: {
			LPNMHDR lpnmhdr;
			lpnmhdr = (NMHDR FAR *)lParam;
			switch (lpnmhdr->code) {
//				case PSN_HELP:
//					WinHelp(hDlg,szHelpFileName,HELP_CONTEXT,CONFIGDIALOGAVP_TOPIC);
//					return TRUE;
				case PSN_APPLY:
					SetConfiguration(hdlg);
					CompareConfigurations(hdlg);
					GetConfiguration(hdlg);
					return TRUE;
				case PSN_SETACTIVE:
					hPropDlg=lpnmhdr->hwndFrom;
					return TRUE;
 
			}
			break;
		}
		case WM_INITDIALOG:
			GetConfiguration(hdlg);
			return TRUE;
		case WM_COMMAND:
			switch (wID) {
				case IDC_ADV_MODE:
					if(IsDlgButtonChecked(hdlg,wID)) {
						LoadString(hInst,IDS_MAIN_DLG_CAPT,Caption2,sizeof(Caption2));
						FillInPropertyPage(&psp,MAKEINTRESOURCE(IDD_MAINDLG),Caption2,(DLGPROC)MainDlgProc,hInst);
						hpp=CreatePropertySheetPage(&psp);
						if(hpp)
							PropSheet_AddPage(hPropDlg,hpp);
					}
					else
						PropSheet_RemovePage(hPropDlg,1,NULL);
					goto ChangeState;
			}
			if((wID & 1)!=1) {
				Flag=IsDlgButtonChecked(hdlg,wID);
				EnableWindow(GetDlgItem(hdlg,wID+1),Flag);
				if(!Flag)
					CheckDlgButton(hdlg,wID+1,(UINT)0);
			}
			goto ChangeState;
			break;
		default:
			break;
	}
	return FALSE;
ChangeState:
	SendMessage(GetParent(hdlg),PSM_CHANGED,(WPARAM)hdlg,0l);
	return TRUE;
}

#ifdef AlternateInterface
int CreateConfigDlg(HINSTANCE hInst, HWND hWnd)
{
	int Result = 0;
	static BOOL bShowed = FALSE;
	if (bShowed == FALSE)
	{
		Result = -1;
		bShowed = TRUE;
		HMODULE hCfgModule = LoadLibrary("avpgcfg.dll");
		if (hCfgModule != NULL)
		{
			FARPROC cfgproc = GetProcAddress(hCfgModule, "CfgProc");
			if (cfgproc != NULL)
			{
				OutputDebugString("Show alternate interface\n");
				Result = ((int(*)(HINSTANCE, HWND, HANDLE, PAPP_REGISTRATION))cfgproc)(hInst, hWnd, hDevice, &AppReg);
				OutputDebugString("Hide alternate interface\n");
			}
			FreeLibrary(hCfgModule);
		}

		bShowed	 = FALSE;
	}
	return Result;
}

#else
int CreateConfigDlg(HINSTANCE hInst,HWND hwndOwner)
{
PROPSHEETPAGE psp[TOT_PAGES];
PROPSHEETHEADER psh;
BOOL ret;
UINT TotPages=0;
CHAR Caption1[64];
CHAR Caption2[64];
CHAR TextForOut[128];

    //Fill out the PROPSHEETPAGE data structure

	if(IsIntoConfigDialog)
		return TRUE;
	else IsIntoConfigDialog=TRUE;
	LoadString(hInst,IDS_DUMP_DLG_CAPT,Caption1,sizeof(Caption1));
	FillInPropertyPage(&psp[TotPages],MAKEINTRESOURCE(IDD_DUMPDLG),Caption1,(DLGPROC)DumpDlgProc,hInst);
	TotPages++;

	if(IsAdvancedModeOn())
	{
		LoadString(hInst,IDS_MAIN_DLG_CAPT,Caption2,sizeof(Caption2));
		FillInPropertyPage(&psp[TotPages],MAKEINTRESOURCE(IDD_MAINDLG),Caption2,(DLGPROC)MainDlgProc,hInst);
		TotPages++;
	}

	//Fill out the PROPSHEETHEADER
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_USEICONID | PSH_PROPSHEETPAGE | PSH_HASHELP;
	psh.hwndParent = hwndOwner;
	psh.hInstance = hInst;
	psh.pszIcon = NULL;
	psh.ppsp = (LPCPROPSHEETPAGE) &psp;
	LoadString(hInst,IDS_CFG_DLG_CAPT,TextForOut,sizeof(TextForOut));
	psh.pszCaption = TextForOut;
	psh.nPages = TotPages;
	psh.nStartPage=0;//StartPage;

	ret=PropertySheet(&psh);
	IsIntoConfigDialog=FALSE;
	return ret;
}
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
CHAR	Tmp[256];
CHAR  Tmp1[1024];
CHAR	*pTmp;
DWORD i,Err=0;
DWORD BuildNum,BytesRet;//,MajF,Fn;
HDSTATE Activity;
	switch (wMsg) { 
		case WM_INITDIALOG:
			SetClassLong(hWnd,GCL_HICON,(LONG)LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1)));
			TrayMessage(hWnd,NIF_MESSAGE|NIF_ICON|NIF_TIP,NIM_ADD,IDC_ICONNOT,(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON2),IMAGE_ICON,16,16,0),AppName);
			AppReg.m_AppID=Interface_ID;
			AppReg.m_Priority=AVPG_STANDARTRIORITY + 1;
			AppReg.m_CurProcId=GetCurrentProcessId();
			AppReg.m_LogFileName[0]=0;
			AppReg.m_ClientFlags = _CLIENT_FLAG_NONE;
			AppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
//			strcpy(AppReg.LogFileName, _LOG_FILE_NAME);
// Device must be open only in share mode
			if(SearchPath(NULL,AppName,DriverExt,sizeof(Tmp1),Tmp1,&pTmp)) {
				GetShortPathName(Tmp1,Tmp,sizeof(Tmp));
				CharToOem(Tmp,Tmp);
				strcpy(AppReg.m_LogFileName,Tmp);
				for(i=strlen(AppReg.m_LogFileName);i>=0;i--) 
					if(AppReg.m_LogFileName[i]=='\\') {
						strcpy(AppReg.m_LogFileName+i+1,_LOG_FILE_NAME);
						break;
					}
				if(i==0)
					AppReg.m_LogFileName[0]=0;
				if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
					Err=LoadDeviceDriver(AppName,Tmp);
					if(!Err || Err==ERROR_ACCESS_DENIED) {
						Err=0;
						wsprintf(Tmp1,DrvStr,AppName);
						if((hDevice=CreateFile(Tmp1,0/*GENERIC_READ | GENERIC_WRITE*/,0/*FILE_SHARE_READ | FILE_SHARE_WRITE*/,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
							Err=GetLastError();
					}
				} else {
					wsprintf(Tmp1,DrvStr,Tmp);
					if((hDevice=CreateFile(Tmp1,0,0,NULL,0,FILE_FLAG_DELETE_ON_CLOSE,NULL))==INVALID_HANDLE_VALUE)
						Err=GetLastError();
				}
			} else {
				Err=GetLastError();
			}
			if(Err) {
				wsprintf(Tmp1,"Unable to load %s%s", AppName,DriverExt);
				Error(hWnd,Tmp1,Err);//!! For Vik - disable this line :)
				break;
			}
			if(!DeviceIoControl(hDevice,IOCTLHOOK_GetVersion,NULL,0,&BuildNum,sizeof(BuildNum),&BytesRet,NULL) || 
				BuildNum!=HOOK_BUILD_NUM){
				Error(hWnd,TEXT("The driver has wrong version"),0);
				break;
			}
			if(!(hWhistleup=CreateEvent(NULL,FALSE,FALSE,WhistleupName)) ||
				(!(hWFChanged=CreateEvent(NULL,FALSE,FALSE,WFChangedName)))) {
				Error((HWND)hWnd,"Unable to communicate with driver",GetLastError());
				break;
			}
			if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
				lstrcpy(AppReg.m_WhistleUp,WhistleupName);
				lstrcpy(AppReg.m_WFChanged,WFChangedName);
			} else {
				HINSTANCE	hK32;
				static HANDLE (WINAPI *pOpenVxDHandle)(HANDLE)=0;
				if(!(hK32=LoadLibrary("kernel32.dll")) ||
					!(pOpenVxDHandle=(HANDLE(WINAPI *)(HANDLE))GetProcAddress(hK32,"OpenVxDHandle")) ||
					!(*(VOID**)AppReg.m_WhistleUp=pOpenVxDHandle(hWhistleup)) ||
					!(*(VOID**)AppReg.m_WFChanged=pOpenVxDHandle(hWFChanged))) {
					Error((HWND)hWnd,"Unable to communicate with driver",GetLastError());
					break;
				}
			}
			if(!DeviceIoControl(hDevice,IOCTLHOOK_RegistrApp,&AppReg,sizeof(AppReg),NULL,0,&BytesRet,NULL)) {
				AppReg.m_AppID = _SYSTEM_APPLICATION;
				Error((HWND)hWnd,"Unable to register.",GetLastError());
				break;
			}
			if((ReqestTlsIndex=TlsAlloc())==-1) {
				Error(hWnd,"Unable to allocate TLS",GetLastError());
				break;
			}
			if (!_beginthreadex(NULL,0,EventWaiterThread,(void*)hWnd,0,(unsigned*)&BytesRet)) {
				Error(hWnd,TEXT("Unable to start thread"),0);
				break;
			}
			Activity.Activity=_AS_Active;
			Activity.AppID=Interface_ID;
			if(!DeviceIoControl(hDevice,IOCTLHOOK_Activity,&Activity,sizeof(Activity),&Activity,sizeof(Activity),&BytesRet,NULL)) {
				Error(hWnd,TEXT("IOCTL Error"),0);
				break;
			}
			SetTimer(hWnd,1,1000/*ms*/,NULL);
			SetSmile(hWnd,IDI_ICON1);
//			GetEventData(FLTCTL_CURR,hWnd);
			break;
		case WM_TIMER:
			Activity.Activity=_AS_DontChange;
			Activity.AppID=Interface_ID;
			DeviceIoControl(hDevice,IOCTLHOOK_Activity,&Activity,sizeof(Activity),&Activity,sizeof(Activity),&BytesRet,NULL);
			if(!Activity.QLen && Activity.Activity==_AS_Active && OldQLen)
				SetSmile(hWnd,IDI_ICON1);
			InterlockedExchange(&OldQLen,Activity.QLen);
			break;
		case WM_ENDSESSION:
		case WM_CLOSE:
		case WM_DESTROY:
			TrayMessage(hWnd,NIF_MESSAGE, NIM_DELETE, IDC_ICONNOT, NULL, NULL);
			KillTimer(hWnd,1);
			if(ReqestTlsIndex!=-1)
				TlsFree(ReqestTlsIndex);
			PostQuitMessage(0);
			break;
		case WM_NOTIFYICON:
			switch (lParam){
				case WM_LBUTTONDOWN:
					/*if(CreateConfigDlg(hInst,hWnd)==-1)
						Error(hWnd,TEXT("Unable to create main window"),GetLastError());
						
					else */
						SetForegroundWindow(hWnd);
					PostMessage(hWnd, WM_COMMAND, TRAY_SHOW, 0);
					return 0;
				case WM_RBUTTONDOWN: {
				POINT pt;
				HMENU hSysMenu,hSubMenu;
					hSysMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDR_MENU1));
					GetCursorPos(&pt);
					SetForegroundWindow(hWnd);
					hSubMenu=GetSubMenu(hSysMenu,0);
					Activity.Activity=_AS_DontChange;
					Activity.AppID=Interface_ID;
					if(!DeviceIoControl(hDevice,IOCTLHOOK_Activity,&Activity,sizeof(Activity),&Activity,sizeof(Activity),&BytesRet,NULL))
						Error(hWnd,TEXT("Can't get activity status"),0);
					if(Activity.Activity==_AS_Active)
						InsertMenu(hSubMenu,TRAY_EXIT,MF_BYCOMMAND|MF_STRING,IDC_PAUSE, TEXT("Pause"));
					else 
						InsertMenu(hSubMenu,TRAY_EXIT,MF_BYCOMMAND|MF_STRING,IDC_START, TEXT("Start"));
					SetMenuDefaultItem(hSubMenu,TRAY_SHOW,MF_BYCOMMAND);
					TrackPopupMenu(hSubMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x,pt.y, 0, hWnd,NULL);
					DestroyMenu(hSysMenu);
				}
				break;
			}
		case WM_COMMAND:
			switch (LOWORD (wParam)) {
				case TRAY_EXIT:
				case IDEXIT:
					PostQuitMessage(0);
					break;
				case TRAY_SHOW:
					if(CreateConfigDlg(hInst,hWnd)==-1)
						Error(hWnd,TEXT("Unable to create main window"),GetLastError());
					else 
						SetForegroundWindow(hWnd);
					break;
				case IDC_PAUSE:
					Activity.Activity=_AS_Sleep;
					Activity.AppID=Interface_ID;
					if(!DeviceIoControl(hDevice,IOCTLHOOK_Activity,&Activity,sizeof(Activity),&Activity,sizeof(Activity),&BytesRet,NULL))
						Error(hWnd,TEXT("Can't pause"),0);
					SetSmile(hWnd,IDI_ICON2);
					break;
				case IDC_START:
					Activity.Activity=_AS_Active;
					Activity.AppID=Interface_ID;
					if(!DeviceIoControl(hDevice,IOCTLHOOK_Activity,&Activity,sizeof(Activity),&Activity,sizeof(Activity),&BytesRet,NULL))
						Error(hWnd,TEXT("Can't start"),0);
					SetSmile(hWnd,IDI_ICON1);
					break;
			}
		default:
			return FALSE;
	}
	return TRUE;
}

CHAR LogDisk[]="\\\\.\\%C:";
void Sync(void)
{
DWORD Disk;
CHAR  DriveString[16];
HANDLE hTmpDrive;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		for(Disk='C';Disk<='Z';Disk++) {
			wsprintf(DriveString,LogDisk,Disk);
			hTmpDrive=CreateFile(DriveString,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING,NULL);
			if(hTmpDrive!=INVALID_HANDLE_VALUE) {
				FlushFileBuffers(hTmpDrive);
				CloseHandle(hTmpDrive);
			}
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
MSG msg;
HWND hWnd,hFocusWnd;
//HANDLE hMutex;
//DWORD i,DotPos,
//LPTSTR CmdLine;
DWORD Err=0;
CHAR  Tmp1[256];
HDSTATE Activity;
DWORD BytesRet;
	OSVer.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&OSVer))
		return FALSE;
	switch (OSVer.dwPlatformId) {
		case VER_PLATFORM_WIN32s:
			return FALSE;
		case VER_PLATFORM_WIN32_WINDOWS:
			DriverExt=VxdExt;
			break;
		case VER_PLATFORM_WIN32_NT:
			DriverExt=SysExt;
			break;
	}
/*
	CmdLine=GetCommandLine();
	for(i=strlen(CmdLine);i!=0;i--) {
		if(CmdLine[i]=='.')
			DotPos=i;
		if(CmdLine[i]=='\\') {
			i++;
			break;
		}
	}
	strncpy(AppName,CmdLine+i,DotPos-i>8?8:DotPos-i);
	hMutex=CreateMutex(NULL,TRUE,AppName);
	if(GetLastError()==ERROR_ALREADY_EXISTS)
		return FALSE;
*/
	strcpy(AppName,"avpg");
	Sync();
	hFocusWnd=GetForegroundWindow();
	hInst=hInstance;
	hWnd=CreateDialogParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,(DLGPROC) WndProc,(LONG)0);
	if(!hWnd)
		return FALSE;
	ShowWindow(hWnd,SW_HIDE);
	SetForegroundWindow(hFocusWnd);
	while(GetMessage(&msg,NULL,0,0))
		if (!IsDialogMessage (hWnd,&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	if (hDevice !=INVALID_HANDLE_VALUE && AppReg.m_AppID != _SYSTEM_APPLICATION) {
		Activity.Activity = _AS_Unload;
		if (!(AppReg.m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE)) {
			if (::MessageBox(NULL, "Do you want to stay driver resident?", "Attention!", MB_ICONQUESTION | MB_YESNO) == IDYES)
				Activity.Activity = _AS_UnRegisterStayResident;
		}
	}

	Activity.AppID=Interface_ID;

	DestroyWindow(hWnd);
	if(hWhistleup)
		CloseHandle(hWhistleup);
	if(hWFChanged)
		CloseHandle(hWFChanged);

	DeviceIoControl(hDevice,IOCTLHOOK_Activity,&Activity,sizeof(Activity),&Activity,sizeof(Activity),&BytesRet,NULL);

	if(hDevice!=INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
	hDevice=INVALID_HANDLE_VALUE;
	if(OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT) {
		if (Err=UnloadDeviceDriver(AppName)) {
		 wsprintf(Tmp1, TEXT("Error unloading \"%s\""),AppName);
		 Error(HWND_DESKTOP,Tmp1,Err);
		}
	}
//	CloseHandle(hMutex);
	return msg.wParam;
}

/*
Known bugz and incompleteness:

To Do List
- add protect:
	system.ini
	HKEY_Оба\SOFTWARE\Microsoft\Command Processor\AutoRun
*/

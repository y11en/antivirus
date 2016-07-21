// KLGuard.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
// -----------------------------------------------------------------------------------------
HINSTANCE			g_hInst = NULL;
HANDLE				g_hKLGDevice = NULL;

APP_REGISTRATION	g_AppReg;

HANDLE				g_hEvWaiterThread = NULL;

HANDLE				g_hKLGWhistleup = NULL;
HANDLE				g_hKLGWFChanged = NULL;

char				g_KLGWhistleupName[MAX_PATH]; 
char				g_KLGWFChangedName[MAX_PATH]; 


OSVERSIONINFO		g_OSVer;
CHAR				g_AppName[PROCNAMELEN];

CHAR				g_LogDisk[]="\\\\.\\%C:";

static BOOL bShowedInterface = FALSE;

_tMultiByteToWideChar _pMultiByteToWideChar = NULL;
_tWideCharToMultiByte _pWideCharToMultiByte = NULL;

HANDLE g_hHeap;

long g_HeapUsage4 = 0;

void* _MM_Alloc(unsigned int nSize)
{
	InterlockedIncrement(&g_HeapUsage4);
	return HeapAlloc(g_hHeap, 0, nSize);
}

void _MM_Free(void* p)
{
	if (p == NULL)
	{
#ifdef _DEBUG
		OutputDebugString("Try to free memory with NULL pointer\n");
#endif
		return;
	}
	InterlockedDecrement(&g_HeapUsage4);
	HeapFree(g_hHeap, 0, p);
}

BOOL TryToGetUnicodeFunctions(void)
{
	BOOL bRet = FALSE;
	HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");
	if (hKernel != NULL)
	{
		if (_pWideCharToMultiByte == NULL)
			_pWideCharToMultiByte = (_tWideCharToMultiByte) GetProcAddress(hKernel, "WideCharToMultiByte");
		if (_pMultiByteToWideChar == NULL)
			_pMultiByteToWideChar = (_tMultiByteToWideChar) GetProcAddress(hKernel, "MultiByteToWideChar");

		if (_pWideCharToMultiByte != NULL && _pMultiByteToWideChar != NULL)
			bRet = TRUE;
	}

	return bRet;
}


DWORD GetSettedDword(PCHAR pRegBoolValue)
{
	HKEY hSubKey = NULL;
	DWORD TypeStr;
	ULONG uKeyLenght;
	
	DWORD Value = 0;
	
	DWORD dwDisposition;
	if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\KasperskyLab\\KLGuard", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, &dwDisposition) == ERROR_SUCCESS)
	{
		uKeyLenght = sizeof(Value);
		TypeStr = REG_DWORD;
		if (RegQueryValueEx(hSubKey, pRegBoolValue, 0, &TypeStr, (LPBYTE) &Value, &uKeyLenght) != ERROR_SUCCESS)
		{
			DbgPrint(1, "KLGuard: Can't query %s\n", pRegBoolValue);
		}
	}
	
	DbgPrint(1, "KLGuard: %s = %d'\n", pRegBoolValue, Value);

	return Value;
}

void GetLogFileName(char* pBuffer, DWORD BufferLength)
{
	HKEY hSubKey = NULL;
	DWORD TypeStr;
	ULONG uKeyLenght;
	
	pBuffer[0] = 0;
	
	DWORD dwDisposition;
	if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\KasperskyLab\\KLGuard", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, &dwDisposition) == ERROR_SUCCESS)
	{
		uKeyLenght = BufferLength;
		TypeStr = REG_SZ;
		if (RegQueryValueEx(hSubKey, "LogFileName", 0, &TypeStr, (LPBYTE) pBuffer, &uKeyLenght) != ERROR_SUCCESS)
		{
			DbgPrint(1, "KLGuard: Can't query LogFileName\n");
		}
	}
	if (lstrlen(pBuffer) == 0)
	{
		GetCurrentDirectory(BufferLength, pBuffer);
		lstrcat(pBuffer, "KLGuard.log");	// default value
	}
	
	DbgPrint(1, "KLGuard: LogFileName = '%s'\n", pBuffer);
}

void Sync(void)
{
	DWORD Disk;
	CHAR  DriveString[16];
	HANDLE hTmpDrive;
	
	if(g_OSVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		for(Disk = 'C'; Disk <= 'Z'; Disk++) {
			wsprintf(DriveString, g_LogDisk, Disk);
			hTmpDrive = CreateFile(DriveString, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);
			if(hTmpDrive != INVALID_HANDLE_VALUE)
			{
				FlushFileBuffers(hTmpDrive);
				CloseHandle(hTmpDrive);
			}
		}
	}
}

BOOL TrayMessage(HWND hDlg, UINT uFlags, DWORD dwMessage, UINT uID, HICON hIcon, CHAR *pszTip)
{
	BOOL res;
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = hDlg;
	tnd.uID = uID;
	tnd.uFlags = uFlags;
	tnd.uCallbackMessage = WM_NOTIFYICON;
	tnd.hIcon = hIcon;
	if(pszTip)
		lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip));
	else
		tnd.szTip[0] = '\0';
	res = Shell_NotifyIcon(dwMessage, &tnd);
	if(hIcon)
		DestroyIcon(hIcon);
	
	return res;
}

void SetSmile(HWND hWnd,DWORD idi)
{
	TrayMessage(hWnd, NIF_ICON, NIM_MODIFY, IDC_ICONNOT, (HICON)LoadImage(g_hInst,  MAKEINTRESOURCE(idi), IMAGE_ICON, 16, 16, 0), g_AppName);
	SetClassLong(hWnd, GCL_HICON, (LONG)LoadIcon(g_hInst,  MAKEINTRESOURCE(idi)));
}

// -----------------------------------------------------------------------------------------
int CreateConfigDlg(HINSTANCE hInst, HWND hWnd)
{
	int Result = 0;
	if (bShowedInterface == FALSE)
	{
		Result = -1;
		bShowedInterface = TRUE;
		HMODULE hCfgModule = LoadLibrary("avpgcfg.dll");
		if (hCfgModule == NULL)
			hCfgModule = LoadLibrary("IGuard.dll");
		if (hCfgModule != NULL)
		{
			FARPROC cfgproc = GetProcAddress(hCfgModule, "CfgProc");
			if (cfgproc != NULL)
			{
				OutputDebugString("Show alternate interface\n");
				Result = ((int(*)(HINSTANCE, HWND, HANDLE, PAPP_REGISTRATION))cfgproc)(hInst, hWnd, g_hKLGDevice, &g_AppReg);
				OutputDebugString("Hide alternate interface\n");
			}
			FreeLibrary(hCfgModule);
		}
		
		bShowedInterface	 = FALSE;
	}
	return Result;
}

// -----------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskbarRestart;
	
	APPSTATE_REQUEST AppRequest;
	APPSTATE AppState;
	
	s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
	
	switch (wMsg)
	{ 
	case WM_INITDIALOG:
		SetClassLong(hWnd, GCL_HICON,(LONG)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1)));
		TrayMessage(hWnd, NIF_MESSAGE | NIF_ICON | NIF_TIP, NIM_ADD, IDC_ICONNOT, 
			(HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, 0), g_AppName);
		PostMessage(hWnd, WM_COMMAND, IDC_START, 0);
		break;
	case WM_TIMER:
		break;
	case WM_ENDSESSION:
	case WM_CLOSE:
	case WM_DESTROY:
		TrayMessage(hWnd,NIF_MESSAGE, NIM_DELETE, IDC_ICONNOT, NULL, NULL);
		PostQuitMessage(0);
		break;
	case WM_NOTIFYICON:
		{
			switch (lParam)
			{
			case WM_LBUTTONDOWN:
				SetForegroundWindow(hWnd);
				PostMessage(hWnd, WM_COMMAND, TRAY_SHOW, 0);
				return 0;
			case WM_RBUTTONDOWN: 
				{
					POINT pt;
					HMENU hSysMenu,hSubMenu;
					
					hSysMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU_MAIN));
					GetCursorPos(&pt);
					SetForegroundWindow(hWnd);
					hSubMenu = GetSubMenu(hSysMenu,0);
					
					if (bShowedInterface)
					{
						EnableMenuItem(hSubMenu, TRAY_EXIT, MF_BYCOMMAND | MF_GRAYED);
						EnableMenuItem(hSubMenu, TRAY_SHOW, MF_BYCOMMAND | MF_GRAYED);
					}
					
					AppRequest = _AS_DontChange;
					if (IDriverState(g_hKLGDevice, g_AppReg.m_AppID, AppRequest, &AppState) == TRUE)
					{
						if(AppState == _AS_Active)
							InsertMenu(hSubMenu,TRAY_EXIT,MF_BYCOMMAND|MF_STRING,IDC_PAUSE, TEXT("Pause"));
						else 
							InsertMenu(hSubMenu,TRAY_EXIT,MF_BYCOMMAND|MF_STRING,IDC_START, TEXT("Start"));
					}
					if (g_AppReg.m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
						EnableMenuItem(hSubMenu, TRAY_RESET_CACHE, MF_BYCOMMAND | MF_ENABLED);
					else
						EnableMenuItem(hSubMenu, TRAY_RESET_CACHE, MF_BYCOMMAND | MF_GRAYED);
						
					SetMenuDefaultItem(hSubMenu,TRAY_SHOW,MF_BYCOMMAND);
					TrackPopupMenu(hSubMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x,pt.y, 0, hWnd,NULL);
					DestroyMenu(hSysMenu);
				}
				break;
			}
		}
		break;
	case WM_COMMAND:
		{
			switch (LOWORD (wParam))
			{
			case TRAY_RESET_CACHE:
				if (::MessageBox(NULL, "Do you want to clear cache?", "Attention!", MB_ICONQUESTION | MB_YESNO) == IDYES)
					IDriverCacheClean(g_hKLGDevice, g_AppReg.m_AppID);
				break;
			case TRAY_EXIT:
				PostQuitMessage(0);
				break;
			case TRAY_SHOW:
				if(CreateConfigDlg(g_hInst, hWnd) == -1)
					MessageBox(hWnd, "Unable to create configuration window!", "KL Guard: Error:", MB_ICONINFORMATION);
				else 
					SetForegroundWindow(hWnd);
				break;
			case IDC_PAUSE:
				AppRequest = _AS_GoSleep;
				IDriverState(g_hKLGDevice, g_AppReg.m_AppID, AppRequest, &AppState);
				switch (AppState)
				{
				case _AS_Active:
					SetSmile(hWnd, IDI_ICON1);
					break;
				case _AS_Sleep:
					SetSmile(hWnd, IDI_ICON2);
					break;
				default:
					SetSmile(hWnd, IDI_ICON3);
					break;
				}
				break;
			case IDC_START:
				AppRequest = _AS_DontChange;
				AppState = _AS_NotRegistered;
				IDriverState(g_hKLGDevice, g_AppReg.m_AppID, AppRequest, &AppState);
				switch (AppState)
				{
				case _AS_Active:
					SetSmile(hWnd, IDI_ICON1);
					break;
				default:
					SetSmile(hWnd, IDI_ICON2);
					{
						AppRequest = _AS_GoActive;
						if (IDriverState(g_hKLGDevice, g_AppReg.m_AppID, AppRequest, &AppState) == TRUE)
							SetSmile(hWnd, IDI_ICON1);
					}

				}
			}
		}
		break;
	default:
		if(wMsg == s_uTaskbarRestart)
		{
			// Код восстановления иконки
			TrayMessage(hWnd, NIF_MESSAGE | NIF_ICON | NIF_TIP, NIM_ADD, IDC_ICONNOT, 
				(HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, 0), g_AppName);
		}
		return FALSE;
	}
	return TRUE;
}

// -----------------------------------------------------------------------------------------
#include <Sign\Sign.h>
CHAR ThisFileName[MAX_PATH * 2];

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// TODO: Place code here.
	HWND hWnd;
	HWND hFocusWnd;
	MSG msg;
	DWORD threadid;
	
	BOOL bStayResident = FALSE;
	
	g_hInst = hInstance;

	g_hHeap = GetProcessHeap();
	
	g_OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&g_OSVer))
		return 0;
	
	//Sync();

	GetModuleFileName(hInstance, ThisFileName, sizeof(ThisFileName));	
	int _sign = sign_check_file(ThisFileName, 1, NULL, 0, 0);
	if (_sign != 0)
		return _sign;

	if(g_OSVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (TryToGetUnicodeFunctions() == FALSE)
			return 0;
	}
	
	PTHREAD_PARAM pThreadParam = NULL;
	
	wsprintf(g_KLGWhistleupName, "KLGWhistleup%d", g_AppReg.m_AppID);
	wsprintf(g_KLGWFChangedName, "KLGWFChanged%d", g_AppReg.m_AppID);
	
	g_AppReg.m_AppID = AVPG_KLGuard;
	g_AppReg.m_Priority = AVPG_STANDARTRIORITY + 100;
	g_AppReg.m_CurProcId = GetCurrentProcessId();
	
	GetLogFileName(g_AppReg.m_LogFileName, MAX_PATH);

	g_AppReg.m_ClientFlags = _CLIENT_FLAG_NONE | _CLIENT_FLAG_WITHOUTWATCHDOG;
	
	if (GetSettedDword("AllowCacheDeadlock"))
		g_AppReg.m_ClientFlags |= _CLIENT_FLAG_CACHDEADLOCK;
	
	if (!GetSettedDword("ClientType"))
		g_AppReg.m_ClientFlags |= _CLIENT_FLAG_POPUP_TYPE;
	
	if (GetSettedDword("SaveFilters"))
		g_AppReg.m_ClientFlags |= _CLIENT_FLAG_SAVE_FILTERS;
	
	if (GetSettedDword("UseBlueScreen"))
		g_AppReg.m_ClientFlags |= _CLIENT_FLAG_USE_BLUE_SCREEN_REQUEST;
	
	if (GetSettedDword("UseDriverCache"))
		g_AppReg.m_ClientFlags |= _CLIENT_FLAG_USE_DRIVER_CACHE;
	
	if (GetSettedDword("WithoutCascade"))
		g_AppReg.m_ClientFlags |= _CLIENT_FLAG_WITHOUTCASCADE;
	
	if (GetSettedDword("WithoutWatchDog"))
		g_AppReg.m_ClientFlags |= _CLIENT_FLAG_WITHOUTWATCHDOG;
	
	g_AppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
	g_AppReg.m_CacheSize = 4096;
	
	g_hKLGDevice = RegisterApp(&g_AppReg, &g_OSVer, &g_hKLGWhistleup, &g_hKLGWFChanged, g_KLGWhistleupName, g_KLGWFChangedName);
	if (g_hKLGDevice == INVALID_HANDLE_VALUE)
	{
		DbgPrint(1, "Register KL Guard failed!\n");
	}
	else
	{
		IDriverRegisterInvisibleThread(g_hKLGDevice, g_AppReg.m_AppID);
		pThreadParam = (PTHREAD_PARAM) _MM_Alloc(sizeof(THREAD_PARAM));
		if (pThreadParam == NULL)
		{
			DbgPrint(1, "Allocating memory for ThreadParam failed!\n");
		}
		else
		{
			pThreadParam->hDevice = g_hKLGDevice;
			pThreadParam->hWhistleup = g_hKLGWhistleup;
			pThreadParam->AppID = g_AppReg.m_AppID;
			pThreadParam->pOSVer = &g_OSVer;
			
			g_hEvWaiterThread = CreateThread(NULL, 0, EventWaiterThread, (void*) pThreadParam, 0, &threadid);
			if (g_hEvWaiterThread == NULL)
			{
				DbgPrint(1, "Starting EventWaiterThread failed!\n");
			}
			else
			{
				lstrcpy(g_AppName, "KL Guard");
				hFocusWnd = GetForegroundWindow();
				hWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC) WndProc,(LONG) 0);
				if(!hWnd)
					return FALSE;
				
				ShowWindow(hWnd, SW_HIDE);
				SetForegroundWindow(hFocusWnd);
				while(GetMessage(&msg, NULL, 0, 0))
				{
					if (!IsDialogMessage (hWnd, &msg))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					
				}
				
				if (!(g_AppReg.m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE))
				{
					if (::MessageBox(NULL, "Would you like to leave filters active?", "Attention!", MB_ICONQUESTION | MB_YESNO) == IDYES)
						bStayResident = TRUE;
				}
				
				DestroyWindow(hWnd);
				
				if(g_hKLGWhistleup)
				{
					InterlockedIncrement(&EvExit);
					PulseEvent(g_hKLGWhistleup);
					CloseHandle(g_hKLGWhistleup);
				}
				if(g_hKLGWFChanged)
					CloseHandle(g_hKLGWFChanged);
				
				WaitForSingleObject(g_hEvWaiterThread, INFINITE);
			}
		}
		IDriverUnregisterInvisibleThread(g_hKLGDevice, g_AppReg.m_AppID);
		UnRegisterApp(g_hKLGDevice, &g_AppReg, bStayResident, &g_OSVer);
	}
	
	return 0;
}

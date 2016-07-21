#include <windows.h>
#include "..\hook\FSDrvLib.h"

#include <winioctl.h>
#include "..\hook\avpgcom.h"
#include "..\hook\IDriver.h"
//#include <sign/sign.h>
#include "service/sa.h"

#define WhistleupName	"Whistleup"
#define WFChangedName	"WFChanged"

#define PAGE_SIZE 0x1000

PAPP_REGISTRATION g_pAppReg = NULL;
HANDLE g_hDevice = INVALID_HANDLE_VALUE;
OSVERSIONINFO g_FSDrvLibOSVer;
char g_WhistleupName[MAX_PATH];
char g_WFChangedName[MAX_PATH];

HANDLE g_hWhistleup = NULL;
HANDLE g_hWFChanged = NULL;

extern char _APPNAME[MAX_PATH];
extern WCHAR* pDrvPath;
extern WCHAR _drvpath_tsp[];


typedef DWORD (WINAPI *_tSetProcessAffinityMask)(HANDLE, DWORD);	
static _tSetProcessAffinityMask _pSetProcessAffinityMask = NULL;

typedef BOOL (WINAPI *_tGetProcessAffinityMask)(HANDLE, DWORD*, DWORD*);	
static _tGetProcessAffinityMask _pGetProcessAffinityMask = NULL;

#define _MOD_FILE_NAME_SIZE	MAX_PATH * 4

class FSDrvLib_Sync
{
public:
	LONG m_RefCount;
	HANDLE m_hFSEvent;
	ULONG m_AppID;
	HSA	m_pHSA;
	
	FSDrvLib_Sync()
	{
		DbgPrint(1, "FSDrvLib: gFSSync constructor!\n");
		m_RefCount = 0;
		LARGE_INTEGER qPerfomance;
		QueryPerformanceCounter((LARGE_INTEGER*) &qPerfomance);

		m_pHSA = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);

#ifdef _DEBUG
		{
			char ev_name[32];
			wsprintf(ev_name, "FSDRV_%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);				
			m_hFSEvent = CreateEvent(SA_Get(m_pHSA), FALSE, TRUE, ev_name);
		}
#else
		m_hFSEvent = CreateEvent(SA_Get(m_pHSA), FALSE, TRUE, NULL);
#endif

		m_AppID = _SYSTEM_APPLICATION;

		g_FSDrvLibOSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		GetVersionEx(&(g_FSDrvLibOSVer));
	}
	
	~FSDrvLib_Sync()
	{
		DbgPrint(1, "FSDrvLib: gFSSync destructor!\n");

		if (g_pAppReg != NULL)
		{
			DbgPrint(1, "\n!!! FSDrvLib: unbalanced exit!!! RefCount %d\n", m_RefCount);
			while (m_RefCount > 0)
				FSDrvInterceptorDone();
		}

		if (m_hFSEvent != NULL)
		{
			CloseHandle(m_hFSEvent);
			m_hFSEvent = NULL;
		}

		if (m_pHSA)
		{
			SA_Destroy(m_pHSA);
			m_pHSA = NULL;
		}
	}
	
	BOOL IsValid()
	{
		if (m_hFSEvent == NULL)
		{
			DbgPrint(1, "FSDrvLib: gFSSync bad event!\n");
			return FALSE;
		}
		
		return TRUE;
	}
};

FSDrvLib_Sync gFSSync;

BOOL FSDrvInterceptorInit(void)
{
	return FSDrvInterceptorInitEx(_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG, AVPG_Driver_Specific, AVPG_STANDARTRIORITY, 0);
}

BOOL FSDrvInterceptorInitEx(DWORD AppFlags, ULONG AppID, ULONG Priority, ULONG CacheSize)
{
	BOOL bResult = FALSE;
	DbgPrint(1, "FSDrvLib: Init %s\n",_APPNAME);
	DWORD SignCheck = 0;

	if (!gFSSync.IsValid())
	{
		DbgPrint(1, "FSDrvLib: Init failed - not valid gFSSync!\n");
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}
	
	InterlockedIncrement(&gFSSync.m_RefCount);

	if (WaitForSingleObject(gFSSync.m_hFSEvent, 10 * 1000) != WAIT_OBJECT_0)
	{
		SetLastError(ERROR_INVALID_EVENT_COUNT);
		return bResult;
	}

	if (g_pAppReg != NULL)
		bResult = TRUE;
	else
	{
		DWORD ModuleNameSize;
		PTCHAR CurrentModule = (PTCHAR) HeapAlloc(GetProcessHeap(), 0, _MOD_FILE_NAME_SIZE);
		if (CurrentModule != NULL)
		{
			ModuleNameSize = GetModuleFileName(NULL, CurrentModule, _MOD_FILE_NAME_SIZE);
			//! SOBKO !!!!!!!!!!!!!!!!!!!
			SignCheck = 0; //sign_check_file(CurrentModule, 1, NULL, 0, 0);

			HeapFree(GetProcessHeap(), 0, CurrentModule);
		}

		if(g_FSDrvLibOSVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");
			_pSetProcessAffinityMask = (_tSetProcessAffinityMask) GetProcAddress(hKernel, "SetProcessAffinityMask");
			_pGetProcessAffinityMask = (_tGetProcessAffinityMask) GetProcAddress(hKernel, "GetProcessAffinityMask");

			if ((_pSetProcessAffinityMask == NULL) || (_pGetProcessAffinityMask == NULL))
			{
				_pSetProcessAffinityMask = NULL;
				_pGetProcessAffinityMask = NULL;
			}
		}
		g_pAppReg = (PAPP_REGISTRATION) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(APP_REGISTRATION));
		if (g_pAppReg != NULL)
		{
			g_pAppReg->m_CurProcId = GetCurrentProcessId();
			g_pAppReg->m_AppID = AppID;
			g_pAppReg->m_CacheSize = CacheSize;
			g_pAppReg->m_Priority = Priority;
			//lstrcpy(g_pAppReg->LogFileName, _this->data->sCLIENT_DEFINITION.m_InternalLogFileName);
			g_pAppReg->m_ClientFlags = AppFlags;
			g_pAppReg->m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;


			LARGE_INTEGER qPerfomance;
			QueryPerformanceCounter((LARGE_INTEGER*) &qPerfomance);
			
			wsprintf(g_WhistleupName, "DWU%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);
			wsprintf(g_WFChangedName, "DWC%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);
			
			CLIENT_INFO ClientInfo;
			g_hDevice = RegisterAppEx(g_pAppReg, &g_FSDrvLibOSVer, &(g_hWhistleup), &(g_hWFChanged), g_WhistleupName, g_WFChangedName, &ClientInfo);
			if (g_hDevice != INVALID_HANDLE_VALUE)
			{
				bResult = TRUE;
				gFSSync.m_AppID = ClientInfo.m_AppID;
			}
			else
			{
				DbgPrint(1, "FSDrvLib - init failed!\n");
				HeapFree(GetProcessHeap(), 0, g_pAppReg);
				g_pAppReg = NULL;
				SetLastError(ERROR_CONNECTION_UNAVAIL);
			}
		}
		else
		{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		}
	}

	SetEvent(gFSSync.m_hFSEvent);
	return bResult;
}

BOOL FSDrvInterceptorInitTsp(void)
{
	if (FSDrvInterceptorInit())
	{
		if (IDriverIsTspOk())
			return TRUE;
		FSDrvInterceptorDone();
		return FALSE;
	}
	lstrcpy(_APPNAME, "TSP");
	pDrvPath = _drvpath_tsp;
	
	if (FSDrvInterceptorInit())
	{
		if (IDriverIsTspOk())
			return TRUE;
		FSDrvInterceptorDone();
	}

	return FALSE;
}

void FSDrvInterceptorDone(void)
{
	if (!gFSSync.IsValid())
		return;

	if (WaitForSingleObject(gFSSync.m_hFSEvent, 10 * 1000) != WAIT_OBJECT_0)
		return;
	
	InterlockedDecrement(&gFSSync.m_RefCount);
	if (gFSSync.m_RefCount == 0)
	{
		if (g_pAppReg != NULL)
		{
			DbgPrint(1, "Unregister app\n");
			UnRegisterApp(g_hDevice, g_pAppReg, FALSE, &g_FSDrvLibOSVer);

			HeapFree(GetProcessHeap(), 0, g_pAppReg);
			g_pAppReg = NULL;
			gFSSync.m_AppID = _SYSTEM_APPLICATION;
			
			if (g_hWhistleup != NULL)
			{
				CloseHandle(g_hWhistleup);
				g_hWhistleup = NULL;
			}
			if (g_hWFChanged != NULL)
			{
				CloseHandle(g_hWFChanged);
				g_hWFChanged = NULL;
			}
		}
		g_hDevice = INVALID_HANDLE_VALUE;
		// VM
		//DbgPrint(1, "FSDrv -  Done completed\n");
	}

	SetEvent(gFSSync.m_hFSEvent);
}

BOOL FSDrvInterceptorRegisterInvisibleThread(void)
{
	DWORD dwThreadID;
	if (!gFSSync.IsValid())
		return FALSE;

	if (gFSSync.m_AppID == _SYSTEM_APPLICATION)
	{
		DbgPrint(1, "Invisible (register) operation failed - client not registered!\n");
		return FALSE;
	}
	
	dwThreadID = IDriverRegisterInvisibleThread(g_hDevice, gFSSync.m_AppID);
	if (dwThreadID != 0)
		return TRUE;

	return FALSE;
}

void FSDrvInterceptorUnregisterInvisibleThread(void)
{
	DWORD dwThreadID;

	if (!gFSSync.IsValid())
		return;
	
	if (gFSSync.m_AppID == _SYSTEM_APPLICATION)
	{
		DbgPrint(1, "Invisible (unregister) operation failed - client not registered\n");
		return;
	}
	
	dwThreadID = IDriverUnregisterInvisibleThread(g_hDevice, gFSSync.m_AppID);
}

BOOL FSDrvInterceptorSetActive(BOOL bActive)
{
	BOOL bRet = FALSE;
	APPSTATE CurrentState;

	CurrentState = _AS_NotRegistered;
	if (bActive == TRUE)
		bRet = IDriverState(g_hDevice, gFSSync.m_AppID, _AS_GoActive, &CurrentState);
	else
		bRet = IDriverState(g_hDevice, gFSSync.m_AppID, _AS_GoSleep, &CurrentState);
	if (bRet == TRUE && (CurrentState != _AS_NotRegistered))
		bRet = TRUE;
	
	return bRet;
	
}

// -----------------------------------------------------------------------------------------
BOOL FSDrvTSPCommonReq(ORIG_HANDLE hOrig,TSP_REQUEST_CLASS Rc)
{
	TSPRS_LOCK Req;
	Req.ReqClass=Rc;
	Req.hOrig=hOrig;
	return IDriverTSPRequest(g_hDevice, &Req,NULL);
}

ORIG_HANDLE FSDrvTSPRegister(SIZE_T max_size)
{
	ORIG_HANDLE hOrig;
	TSPRS_REGISTER Req;
	Req.ReqClass=TSPRC_Register;
	Req.MaxSize=max_size;
	
	if(IDriverTSPRequest(g_hDevice,(PTSPRS_LOCK) &Req,&hOrig))
		return hOrig;
	else 
	{
		Sleep(3000);	// система не успевает забрать вирутальные адреса?
		if(IDriverTSPRequest(g_hDevice,(PTSPRS_LOCK) &Req,&hOrig))
			return hOrig;

		return NULL;
	}
}

BOOL FSDrvTSPLock(ORIG_HANDLE hOrig,SIZE_T real_size,DWORD min_number_of_shadows)
{
	TSPRS_LOCK Req;
	Req.ReqClass=TSPRC_Lock;
	Req.hOrig=hOrig;
	Req.real_size=real_size;
	Req.min_number_of_shadows=min_number_of_shadows;
	return IDriverTSPRequest(g_hDevice, &Req, NULL);
}

BOOL FSDrvTSPUnregister(ORIG_HANDLE hOrig)
{
	TSPRS_LOCK Req;
	Req.ReqClass=TSPRC_Unregister;
	Req.hOrig=hOrig;
	return IDriverTSPRequest(g_hDevice, &Req, NULL);
}

BOOL FSDrvTSPThreadEnter(ORIG_HANDLE hOrig)
{
	TSPRS_LOCK Req;
	Req.ReqClass=TSPRC_ThreadEnter;
	Req.hOrig=hOrig;
	return IDriverTSPRequest(g_hDevice, &Req, NULL);
}	

BOOL FSDrvTSPThreadLeave(ORIG_HANDLE hOrig)
{
	TSPRS_LOCK Req;
	Req.ReqClass=TSPRC_ThreadLeave;
	Req.hOrig=hOrig;
	return IDriverTSPRequest(g_hDevice, &Req, NULL);
}	

BOOL FSDrvTSPSetMinShadows(ULONG MinShadows)
{
	TSPRS_REGISTER Req;
	Req.ReqClass = TSPRC_SetMinShadow;
	Req.MaxSize = MinShadows;
	
	return IDriverTSPRequest(g_hDevice, (PTSPRS_LOCK) &Req, NULL);
}

// -----------------------------------------------------------------------------------------
HANDLE FSDrvGetDeviceHandle()
{
	return g_hDevice;
}

HANDLE FSDrvGetWhistlerUpHandle()
{
	return g_hWhistleup;
}

ULONG FSDrvGetAppID()
{
	if (g_pAppReg != NULL)
		return (gFSSync.m_AppID);
	
	return AVPG_Driver_Specific;
}

BOOL FSDrv_GetDiskStat(unsigned __int64* pTimeCurrent, unsigned __int64* pTotalWaitTime)
{
	if (!gFSSync.IsValid())
		return FALSE;

	if ((!pTimeCurrent) || (!pTotalWaitTime))
		return FALSE;

	return IDriver_GetDiskStat(FSDrvGetDeviceHandle(), FSDrvGetAppID(), pTimeCurrent, pTotalWaitTime);
}
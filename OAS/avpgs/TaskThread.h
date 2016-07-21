#ifndef __HOOK_TASK_THREAD
#define __HOOK_TASK_THREAD

#include <windows.h>

#include "../../windows/hook/hook/avpgcom.h"
#include "../../windows/hook/hook/IDriver.h"
#include "../../windows/hook/hook/HookSpec.h"
#include "../../windows/hook/hook/Funcs.h"

#include "DrvEventList.h"

bool GetEventData(DrvEventList* pDrvEventList, HANDLE hDevice, ULONG AppID, BYTE** pData, ULONG *pMark);

//+ ----------------------------------------------------------------------------------------

class HookRegisteredApp
{
public:
	HANDLE			m_hDevice;
	ULONG			m_AppID;
	OSVERSIONINFO	m_OsInfo;
	ULONG			m_DrvFlags;

	HookRegisteredApp() : m_hDevice(INVALID_HANDLE_VALUE), m_AppID(_SYSTEM_APPLICATION)
	{
		m_OsInfo.dwOSVersionInfoSize = sizeof(m_OsInfo);
		GetVersionEx(&m_OsInfo);
		m_DrvFlags = _DRV_FLAG_NONE;
	};

	HookRegisteredApp(HANDLE hDevice, ULONG AppID) : 
		m_hDevice(hDevice),
		m_AppID(AppID),
		m_DrvFlags(_DRV_FLAG_NONE)
		{
		};

	BOOL IsConnected()
	{
		return ((m_hDevice != INVALID_HANDLE_VALUE) && (m_AppID != _SYSTEM_APPLICATION));
	}
	
	void NewInternalData(HANDLE	hDevice, ULONG	AppID, ULONG DrvFlags)
	{
		m_hDevice = hDevice;
		m_AppID = AppID;
		m_DrvFlags = DrvFlags;
	}

	bool ChangeActiveStatus(bool bActive);

	void ResetFilters()
	{
		if (!IsConnected())
			return;

		FILTER_TRANSMIT FilterTransmit;
		FilterTransmit.m_AppID = m_AppID;
		FilterTransmit.m_FltCtl = FLTCTL_RESET_FILTERS;
		
		IDriverFilterTransmit(m_hDevice, &FilterTransmit, &FilterTransmit, sizeof(FilterTransmit), sizeof(FilterTransmit));
	}
};

//+ ----------------------------------------------------------------------------------------

class HookAppReg
{
public:
	APP_REGISTRATION	m_AppReg;
	char				m_WhistleupName[MAX_PATH];
	char				m_WFChangedName[MAX_PATH];

	HANDLE				m_hWhistleup;
	HANDLE				m_hWFChanged;
public:
	HookAppReg()
	{
		m_hWhistleup = 0;
		m_hWFChanged = 0;

		m_AppReg.m_LogFileName[0] = 0;
		m_AppReg.m_CurProcId = GetCurrentProcessId();
		wsprintfA(m_WhistleupName, "KLG2U%d%d%d", GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount());
		wsprintfA(m_WFChangedName, "KLG2C%d%d%d", GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount());
	}

	HANDLE Register(DWORD Flags = _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WITHOUTCASCADE | _CLIENT_FLAG_WANTIMPERSONATE, 
		ULONG AppID = AVPG_Driver_Specific, ULONG Priority = AVPG_INFOPRIORITY)
	{
		m_AppReg.m_AppID = AppID;
		m_AppReg.m_Priority = Priority;
		m_AppReg.m_ClientFlags = Flags;
		m_AppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

		OSVERSIONINFO OsInfo;
		OsInfo.dwOSVersionInfoSize = sizeof(OsInfo);
		GetVersionEx(&OsInfo);

		if ((OsInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (OsInfo.dwMinorVersion == 0))
			Flags |= _CLIENT_FLAG_CACHDEADLOCK;
		else
			Flags |= _CLIENT_FLAG_WATCHDOG_USEPAUSE;

		return RegisterApp(&m_AppReg, &OsInfo, &(m_hWhistleup), &(m_hWFChanged), m_WhistleupName, m_WFChangedName);
	}

	inline ULONG GetAppID() {return m_AppReg.m_AppID;};

	static void UnregisterApp(HANDLE hDevice, ULONG AppID)
	{
		if (hDevice != INVALID_HANDLE_VALUE && AppID != _SYSTEM_APPLICATION)
			IDriverUnregisterApp(hDevice, AppID, FALSE);
	}

	void UnregisterApp(HANDLE hDevice)
	{
		UnregisterApp(hDevice, m_AppReg.m_AppID);
		m_AppReg.m_AppID = _SYSTEM_APPLICATION;
	}

	virtual ~HookAppReg()
	{
		if (m_hWhistleup != 0)
		{
			CloseHandle(m_hWhistleup);
			m_hWhistleup = 0;
		}
		if (m_hWFChanged != 0)
		{
			CloseHandle(m_hWFChanged);
			m_hWFChanged = 0;
		}
	}
};

//+ ----------------------------------------------------------------------------------------

class ThreadParam : public HookRegisteredApp
{
public:
	ThreadParam(){};
	virtual ~ThreadParam(){};

	virtual bool AddFilters(){ return true;};
	
	virtual void BeforeExit(){};
	virtual void SingleEvent(){};
	virtual void FiltersChanged(){};
	virtual ULONG GetCacheSize(){return 0;}
};

//+ ----------------------------------------------------------------------------------------

class ThreadSync
{
public:
	// in
	HANDLE			m_hStopEvent;
	PVOID			m_pThreadParam;
	// out
private:
	HANDLE			m_hStartedEvent;
	bool			m_bStartSucceed;

public:
	ThreadSync(PVOID pThreadParam) : m_hStartedEvent(NULL), m_bStartSucceed(false), 
		m_hStopEvent(NULL), m_pThreadParam(pThreadParam) {};

	~ThreadSync()
	{
		if (m_hStartedEvent)
		{
			CloseHandle(m_hStartedEvent);
			m_hStartedEvent = NULL;
		}
	}
	

	HANDLE StartThread(LPTHREAD_START_ROUTINE lpThreadFunc, HANDLE hStopEvent);

	void SetStartSucceed()
	{
		m_bStartSucceed = true;
		SetEvent(m_hStartedEvent);
	};

	bool IsStartSucceed()
	{
		return m_bStartSucceed;
	}
};

//+ ----------------------------------------------------------------------------------------

class HookTask
{
public:
	HANDLE	m_hStopEvent;
	HANDLE	m_hThread;

public:
	HookTask()
	{
		m_hStopEvent = CreateEvent(0, TRUE, FALSE, 0);
		m_hThread = INVALID_HANDLE_VALUE;
	};
	
	void Done()
	{
		if (m_hThread != INVALID_HANDLE_VALUE)
		{
			SetEvent(m_hStopEvent);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = INVALID_HANDLE_VALUE;
		}

		CloseHandle(m_hStopEvent);
	};

	bool Start(ThreadParam* pThreadParam);
};

//+ ----------------------------------------------------------------------------------------


#endif //__HOOK_TASK_THREAD
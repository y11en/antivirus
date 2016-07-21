#include "TaskThread.h"

bool GetEventData(DrvEventList* pDrvEventList, HANDLE hDevice, ULONG AppID, BYTE** pData, ULONG *pMark)
{
	DWORD BytesRet;
	BOOL bReq;
	
	static LONG Mark = 0;
	
	InterlockedIncrement(&Mark);
	if (Mark == 0)
		InterlockedIncrement(&Mark);
	
	ULONG EventDataSize = 2048;
	BYTE* pEventData = pDrvEventList->Alloc(EventDataSize);
	if (pEventData != NULL)
	{
		*pMark = Mark;
		bReq = IDriverGetEvent(hDevice, AppID, Mark, pEventData, EventDataSize, &BytesRet);
		if (bReq == FALSE || BytesRet == 0)
		{
			DWORD BytesRet = 0;
			EventDataSize = 0;
			if (DeviceIoControl(hDevice, IOCTLHOOK_GetEventSize, &AppID, sizeof(AppID), &EventDataSize, sizeof(EventDataSize), &BytesRet, NULL))
			{
				HeapFree(GetProcessHeap(), 0, pEventData);
				pEventData = NULL;
				if (EventDataSize != 0)
				{
					pEventData = pDrvEventList->Alloc(EventDataSize);
					if (pEventData != NULL)
					{
						bReq = IDriverGetEvent(hDevice, AppID, Mark, pEventData, EventDataSize, &BytesRet);
						if (bReq == FALSE || BytesRet == 0)
						{
							pDrvEventList->Free(pEventData);
							pEventData = NULL;
						}
					}
				}
			}
		}
	}
	
	if (pEventData != NULL)
	{
		*pData = pEventData;
		return true;
	}
	
	IDriverSkipEvent(hDevice, AppID);
	
	return false;
}


bool HookRegisteredApp::ChangeActiveStatus(bool bActive)
{
	APPSTATE CurrentState;
	APPSTATE_REQUEST Req = _AS_GoSleep;

	if (bActive)
		Req = _AS_GoActive;

	if (IDriverState(m_hDevice, m_AppID, Req, &CurrentState))
		return true;
	
	Req = _AS_DontChange;
	if (IDriverState(m_hDevice, m_AppID, Req, &CurrentState))
	{
		if ((bActive) && (CurrentState == _AS_Active))
			return true;
		if ((!bActive) && (CurrentState == _AS_Sleep))
			return true;
	}

	return false;
}

HANDLE ThreadSync::StartThread(LPTHREAD_START_ROUTINE lpThreadFunc, HANDLE hStopEvent)
{
	DWORD dwThreadID = 0;

	if (m_hStartedEvent != 0 || hStopEvent == 0)
		return INVALID_HANDLE_VALUE;

	m_hStopEvent = hStopEvent;
	
	m_hStartedEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (!m_hStartedEvent)
		return INVALID_HANDLE_VALUE;

	HANDLE hThread = CreateThread(0, 0, lpThreadFunc, (LPVOID) this, 0, &dwThreadID);
	if (hThread == NULL)
		return INVALID_HANDLE_VALUE;
	
	HANDLE handles[2];
	handles[0] = m_hStartedEvent;
	handles[1] = hThread;

	DWORD dwResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
	switch (dwResult)
	{
	case WAIT_OBJECT_0:
		if (m_bStartSucceed)
			return hThread;
		break;
	}

	return INVALID_HANDLE_VALUE;
}

//+ ----------------------------------------------------------------------------------------
DWORD WINAPI ThreadFunc_Processes(LPVOID lpThreadParameter)
{
	ThreadSync* pThreadSync = (ThreadSync*) lpThreadParameter;
	if (pThreadSync == NULL)
		return -1;

	HookAppReg		AppReg;
	ThreadParam*	pThreadParam = (ThreadParam*) pThreadSync->m_pThreadParam;

	AppReg.m_AppReg.m_CacheSize = pThreadParam->GetCacheSize();

	HANDLE hDevice = AppReg.Register(_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_USE_DRIVER_CACHE | _CLIENT_FLAG_WANTIMPERSONATE 
		| _CLIENT_FLAG_WITHOUTCASCADE, AVPG_MPraguePlugin, AVPG_STANDARTRIORITY);
	
	if (hDevice == INVALID_HANDLE_VALUE)
		return -1;
	
	ADDITIONALSTATE AddState;
	if (!IDriverGetAdditionalState(hDevice, &AddState))
		AddState.DrvFlags = _DRV_FLAG_NONE;

	pThreadParam->NewInternalData(hDevice, AppReg.GetAppID(), AddState.DrvFlags);

	HANDLE hStopEvent = pThreadSync->m_hStopEvent;

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	IDriverRegisterInvisibleThread(hDevice, AppReg.GetAppID());

	if (!pThreadParam->AddFilters())
		return -1;

	pThreadSync->SetStartSucceed();

	HANDLE handles[3];
	handles[0] = hStopEvent;
	handles[1] = AppReg.m_hWhistleup;
	handles[2] = AppReg.m_hWFChanged;

	DWORD dwResult = WAIT_TIMEOUT;

	HDSTATE Activity;

	bool bExit = false;
	while (!bExit)
	{
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			pThreadParam->BeforeExit();
			bExit = true;
			break;

		case WAIT_TIMEOUT:
		case WAIT_OBJECT_0 + 1:
			{
				IDriverGetState(hDevice, AppReg.GetAppID(), &Activity);
				while (Activity.QUnmarkedLen)
				{
					pThreadParam->SingleEvent();
					Activity.QUnmarkedLen--;
				}
			}
			break;
		case WAIT_OBJECT_0 + 2:
			// filters changed
			pThreadParam->FiltersChanged();
			break;
		}

		if (!bExit)
			dwResult = WaitForMultipleObjects(sizeof(handles) / sizeof(handles[0]), handles, FALSE, 1000);
	}

	IDriverUnregisterInvisibleThread(pThreadParam->m_hDevice, pThreadParam->m_AppID);

	HookAppReg::UnregisterApp(pThreadParam->m_hDevice, pThreadParam->m_AppID);
	return 0;
}

bool HookTask::Start(ThreadParam* pThreadParam)
{
	if (pThreadParam == 0)
		return false;

	ThreadSync ThSync(pThreadParam);
	m_hThread = ThSync.StartThread(ThreadFunc_Processes, m_hStopEvent);
	if (m_hThread == INVALID_HANDLE_VALUE)
		return false;

	return ThSync.IsStartSucceed();
}

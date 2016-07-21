#include "stdafx.h"
#include "TaskThread.h"

bool HookRegisteredApp::ChangeActiveStatus(bool bActive)
{
	APPSTATE CurrentState;
	APPSTATE_REQUEST Req = _AS_GoSleep;

	if (bActive)
		Req = _AS_GoActive;

	if (IDriverState(m_hDevice, m_AppID, Req, &CurrentState))
		return true;
	
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
	ThreadParam*	pThreadParam = pThreadSync->m_pThreadParam;

	HANDLE hDevice = AppReg.Register();
	if (hDevice == INVALID_HANDLE_VALUE)
		return -1;

	pThreadParam->NewInternalData(hDevice, AppReg.GetAppID());

	HANDLE hStopEvent = pThreadSync->m_hStopEvent;

	if (!pThreadParam->AddFilters())
		return -1;

	pThreadSync->SetStartSucceed();

	HANDLE handles[3];
	handles[0] = hStopEvent;
	handles[1] = AppReg.m_hWhistleup;
	handles[2] = AppReg.m_hWFChanged;

	DWORD dwResult = WAIT_TIMEOUT;

	bool bExit = false;
	while (!bExit)
	{
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			pThreadParam->BeforeExit();
			bExit = true;
			break;

		case WAIT_OBJECT_0 + 1:
			pThreadParam->SingleEvent();
			break;

		case WAIT_OBJECT_0 + 2:
			// filters changed
			pThreadParam->FiltersChanged();
			break;
		}

		if (!bExit)
			dwResult = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
	}

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

	return true;
}

//+ ----------------------------------------------------------------------------------------

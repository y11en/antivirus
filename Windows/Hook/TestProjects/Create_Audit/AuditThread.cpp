#include "stdafx.h"

#include "AuditThread.h"

// -----------------------------------------------------------------------------------------
HANDLE g_hHeap = NULL;
LONG g_Mark = 1;

void* _MM_Alloc(unsigned int nSize)
{
	return HeapAlloc(g_hHeap, 0, nSize);
}

void* _MM_ReAlloc(void *pBuf, unsigned int nSize)
{
	return HeapReAlloc(g_hHeap, 0, pBuf, nSize);
}

void _MM_Free(void* p)
{
	HeapFree(g_hHeap, 0, p);
}

// -----------------------------------------------------------------------------------------
// Function name	: CopyThreadParam
// Description	    : coping data
// Return type		: void 
// Argument         : PTHREAD_PARAM pThreadParamTo
// Argument         : PTHREAD_PARAM pThreadParam
void CopyThreadParam(PTHREAD_PARAM pThreadParamTo, PTHREAD_PARAM pThreadParam)
{
	pThreadParamTo->m_hDevice = pThreadParam->m_hDevice;
	pThreadParamTo->m_hWhistleup = pThreadParam->m_hWhistleup;
	pThreadParamTo->m_hWFChanged = pThreadParam->m_hWFChanged;
	pThreadParamTo->m_AppReg.m_AppID = pThreadParam->m_AppReg.m_AppID;
	pThreadParamTo->m_SetVerdict = pThreadParam->m_SetVerdict;
	pThreadParamTo->m_pOSVer = pThreadParam->m_pOSVer;
}

// Function name	: GetEventData
// Description	    : get event data  from driver
// Return type		: BOOL  - TRUE if retrieving complete
// Argument         : PTHREAD_PARAM pThreadParam

BOOL GetEventData(PTHREAD_PARAM pThreadParam)
{
	DWORD BytesRet;
	DWORD BufferSize;
	
	BOOL bReq;
	
	pThreadParam->m_Event.m_AppID = pThreadParam->m_AppReg.m_AppID;
	
	// -----------------------------------------------------------------------------------------
	pThreadParam->m_Event.Mark = InterlockedIncrement(&g_Mark); 
	if (g_Mark == 0)
		InterlockedIncrement(&g_Mark); 
	BufferSize = sizeof(pThreadParam->m_pEventData);
	// -----------------------------------------------------------------------------------------
	
	bReq = DeviceIoControl(pThreadParam->m_hDevice, IOCTLHOOK_GetEvent, &(pThreadParam->m_Event), sizeof(GET_EVENT), &(pThreadParam->m_pEventData), BufferSize, &BytesRet, NULL);
	if (bReq == FALSE || BytesRet == 0)
	{
		IDriverSkipEvent(pThreadParam->m_hDevice, pThreadParam->m_AppReg.m_AppID);
	}
	else		
		return TRUE;
	
	return FALSE;
}
// -----------------------------------------------------------------------------------------


HANDLE StartClient(PTHREAD_PARAM pThreadParam)
{
	DWORD dwThreadID;
	HANDLE hWaiterThread = NULL;

	g_hHeap = GetProcessHeap();
	if (g_hHeap == NULL)
		return NULL;

	pThreadParam->m_SyncObject_ClientStart = CreateEvent(NULL, FALSE, FALSE, NULL);	
	if (pThreadParam->m_SyncObject_ClientStart == NULL)
		return NULL;

	pThreadParam->m_SyncObject_ClientExit = CreateEvent(NULL, FALSE, FALSE, NULL);	
	if (pThreadParam->m_SyncObject_ClientExit == NULL)
	{
		CloseHandle(pThreadParam->m_SyncObject_ClientStart);
		return NULL;
	}
	ResetEvent(pThreadParam->m_SyncObject_ClientStart);
	ResetEvent(pThreadParam->m_SyncObject_ClientExit);

	hWaiterThread = CreateThread(NULL, 0, EventWaiterThread, (void*) pThreadParam, 0, &dwThreadID);

	if (hWaiterThread != NULL)
		if (WaitForSingleObject(pThreadParam->m_SyncObject_ClientStart, INFINITE) != WAIT_OBJECT_0)
		{
			DbgPrint(1, "Client init failed!\n");	
			hWaiterThread = NULL;
		}
	
	return hWaiterThread;
}

void StopClient(HANDLE hThread, PTHREAD_PARAM pThreadParam)
{
	DbgPrint(1, "Stop client...\n");	
	if ((hThread != NULL) && (pThreadParam->m_hDevice != NULL))
	{
		SetEvent(pThreadParam->m_SyncObject_ClientExit);
		WaitForSingleObject(hThread, INFINITE);
	}
	DbgPrint(1, "Client stopped.\n");	
}


BOOL RegisterApplication(PTHREAD_PARAM pThreadParam)
{
	BOOL bRes = FALSE;

	PAPP_REGISTRATION pAppReg =  &pThreadParam->m_AppReg;

	DbgPrint(1, "Register app...\n", bRes);	
	
	CHAR WhistleupName[MAX_PATH];
	CHAR WFChangedName[MAX_PATH];

	pAppReg->m_CurProcId = GetCurrentProcessId();
	pAppReg->m_ClientFlags = _CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WATCHDOG_USEPAUSE;
	pAppReg->m_BlueScreenTimeout = 1/*DEADLOCKWDOG_TIMEOUT*/;
	pAppReg->m_CacheSize = 0;

	wsprintf(WhistleupName, "DMWU%d%d%d", GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount());
	wsprintf(WFChangedName, "DMWC%d%d%d", GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount());

	pThreadParam->m_hDevice = RegisterApp(pAppReg, pThreadParam->m_pOSVer,
										  &(pThreadParam->m_hWhistleup), &(pThreadParam->m_hWFChanged),
										  WhistleupName, WFChangedName);

	if (pThreadParam->m_hDevice != INVALID_HANDLE_VALUE)
		bRes = TRUE;	

	DbgPrint(1, "Register app done = %d\n", bRes);	
	return bRes;
}


// -----------------------------------------------------------------------------------------
// Function name	: EventWaiterThread
// Description	    : thread function for waiting event from driver
// Return type		: DWORD WINAPI 
// Argument         : void* pParam - PTHREAD_PARAM
DWORD WINAPI EventWaiterThread(void* pParam)
{
	DWORD wuNewData = RegisterWindowMessage("Audit_NewDataArrived");
	PTHREAD_PARAM pThreadParam = (PTHREAD_PARAM) pParam;
	
	PTHREAD_PARAM pNewParam;

	HDSTATE Activity;
	DWORD FailCount = 0;

	DbgPrint(1, "EventWaiterThreadID: %x\n", GetCurrentThreadId());

	if (RegisterApplication(pThreadParam) == FALSE)
	{
		DbgPrint(1, "Audit:: register app failed\n");
		CloseHandle(pThreadParam->m_SyncObject_ClientStart);
		return 0;
	}

	
	if (IDriverRegisterInvisibleThread(pThreadParam->m_hDevice, pThreadParam->m_AppReg.m_AppID) == FALSE)
	{
		DbgPrint(1, "Audit:: register invisible thread failed\n");
		UnRegisterApp(pThreadParam->m_hDevice, &pThreadParam->m_AppReg, FALSE, pThreadParam->m_pOSVer);
		CloseHandle(pThreadParam->m_SyncObject_ClientStart);
		return 0;
	}

//	APPSTATE CurrentState;
//	APPSTATE_REQUEST Req;
//!	Req = _AS_GoActive;
//	IDriverState(pThreadParam->m_hDevice, pThreadParam->m_AppReg.m_AppID, Req, &CurrentState);

	DbgPrint(1, "Audit:: Set thread priority to crititcal\n");
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	DWORD dwResult;

	SetEvent(pThreadParam->m_SyncObject_ClientStart);

	HANDLE Handles[2];
	dwResult = WAIT_TIMEOUT;
	
	Handles[0] = pThreadParam->m_SyncObject_ClientExit;
	Handles[1] = pThreadParam->m_hWhistleup;
	
	PEVENT_TRANSMIT pEvTrans = NULL;

	while (dwResult == WAIT_TIMEOUT) 
	{
		dwResult = WaitForMultipleObjects(2, Handles, FALSE, 2500);
		if (IDriverGetState(pThreadParam->m_hDevice, pThreadParam->m_AppReg.m_AppID, &Activity) == FALSE)
		{
			DbgPrint(1, "WatchDog: watch dog failed!\n");
			FailCount++;
			if (FailCount > 5)
			{
				DbgPrint(1, "WatchDog: detach detected! Client notified\n");
			}
		}
		else
			FailCount = 0;
		
		WCHAR url[1024];
		DWORD urlsize;
		if (dwResult == (WAIT_OBJECT_0 + 1))
		{
			dwResult = WAIT_TIMEOUT;
			
			while (Activity.QUnmarkedLen > 0)
			{
				Activity.QUnmarkedLen--;
				pNewParam = (PTHREAD_PARAM) _MM_Alloc(sizeof(_THREAD_PARAM));

				if (pNewParam == NULL)
				{
					IDriverSkipEvent(pThreadParam->m_hDevice, pThreadParam->m_AppReg.m_AppID);
					DbgPrint(1, "EventWaiterThread: Error during allocating memory for NewParam\n");
				}
				else
				{
					CopyThreadParam(pNewParam, pThreadParam);
					if (GetEventData(pNewParam) == TRUE)
					{
						pNewParam->m_SetVerdict.m_Verdict = Verdict_Default;	//answer will not be cached in drive - if you want this - change to Verdict_Pass
						pNewParam->m_SetVerdict.m_AppID = pNewParam->m_AppReg.m_AppID;
						pNewParam->m_SetVerdict.m_Mark = pNewParam->m_Event.Mark;
						pNewParam->m_SetVerdict.m_ExpTime = 0;

						pEvTrans = (PEVENT_TRANSMIT) pNewParam->m_pEventData;

						// query file name
						{
							{
								EVENT_OBJECT_REQUEST evRequest;
							
								ZeroMemory(&evRequest, sizeof(evRequest));

								evRequest.m_RequestType = _event_request_get_param;
								evRequest.m_Mark = pNewParam->m_Event.Mark;
								evRequest.m_AppID = pNewParam->m_AppReg.m_AppID;
								evRequest.m_Offset = _PARAM_OBJECT_URL_W;

								urlsize = sizeof(url) * sizeof(WCHAR);
								if (IDriverEventObjectRequest(pNewParam->m_hDevice, &evRequest, url, &urlsize))
								{
									OutputDebugStringW(url);
									OutputDebugString("\n");
								}
							}

							// read file
							{
								char buffer[sizeof(EVENT_OBJECT_REQUEST) + 4096];
								PEVENT_OBJECT_REQUEST pevRequest = (PEVENT_OBJECT_REQUEST) buffer;
								ULONG read_size;
							
								ZeroMemory(buffer, sizeof(buffer));

								pevRequest->m_RequestType = _event_request_read;
								pevRequest->m_Mark = pNewParam->m_Event.Mark;
								pevRequest->m_AppID = pNewParam->m_AppReg.m_AppID;
								pevRequest->m_Offset = 0;

								read_size = 4096;
								if (IDriverEventObjectRequest(pNewParam->m_hDevice, pevRequest, 
									buffer + sizeof(EVENT_OBJECT_REQUEST), &read_size))
								{

								}
							}

						}

						IDriverSetVerdict2(pNewParam->m_hDevice, &pNewParam->m_SetVerdict);

						_MM_Free(pNewParam);
					}
				}
			}
		}
	}
	DbgPrint(1, "EventWaiterThread: WaitForSingleObject failed (%x). Exit\n", dwResult);

	UnRegisterApp(pThreadParam->m_hDevice, &pThreadParam->m_AppReg, FALSE, pThreadParam->m_pOSVer);

	IDriverCacheClean(pThreadParam->m_hDevice, pThreadParam->m_AppReg.m_AppID);

	DbgPrint(1, "Audit:: EvThread closed\n");

	CloseHandle(pThreadParam->m_hWhistleup);
	CloseHandle(pThreadParam->m_hWFChanged);
	
	CloseHandle(pThreadParam->m_SyncObject_ClientStart);
	CloseHandle(pThreadParam->m_SyncObject_ClientExit);

	return 0;
}

BOOL AddFilters(PTHREAD_PARAM pThreadParam)
{
	return AddFSFilterW(pThreadParam->m_hDevice, pThreadParam->m_AppReg.m_AppID, 
		L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_MINIKAV, 1, 0, 0, PreProcessing, NULL);
}
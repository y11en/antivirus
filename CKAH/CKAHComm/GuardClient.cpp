// GuardClient.cpp: implementation of the CGuardClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuardClient.h"
#include <Hook/klpf_api.h>

#include "PFFilterID.h"

#pragma comment (lib, "fssync.lib" )

// время ожидания выхода потока-обработчика событий
#define GUARD_THREAD_TERM_WAIT_TIMEOUT INFINITE
#define QUEUE_THREAD_TERM_WAIT_TIMEOUT INFINITE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void* __cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	)
{	
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	return ptr;
};

void __cdecl
MemFree (
	PVOID pOpaquePtr,
	void** pptr
	)
{
	if (*pptr)
	{
		HeapFree( GetProcessHeap(), 0 , *pptr );
		*pptr = NULL;
	}
};

VOID
CGuardClient::DrvMemFree( void** ptr)
{
	MemFree( NULL, ptr );
}

CGuardClient::CGuardClient ()
{
	m_hThread = NULL;
	m_hEvQueueThread = NULL;
	m_bInitialState = TRUE;
	m_hShutDownEvent = NULL;
	m_bResident = FALSE;

	m_PendingSupport.m_hEventReply = CreateEvent (NULL, FALSE, FALSE, NULL);

	InitializeCriticalSection (&m_PendingSupport.m_csListSync);
	
	m_hEventQueueSemaphore = CreateSemaphore (NULL, 0, LONG_MAX, NULL);
	InitializeCriticalSection (&m_hcsEventQueue);

	m_hStopEvQueueDispatcher = CreateEvent (NULL, TRUE, FALSE, NULL);
	
	m_DriverContext = NULL;
}

CGuardClient::~CGuardClient ()
{
	CloseHandle (m_PendingSupport.m_hEventReply);
	DeleteCriticalSection (&m_PendingSupport.m_csListSync);

	CloseHandle (m_hEventQueueSemaphore);
	DeleteCriticalSection (&m_hcsEventQueue);

	CloseHandle (m_hStopEvQueueDispatcher);
}

DWORD WINAPI CGuardClient::EventQueueDispatcher (LPVOID pParam)
{
	CGuardClient* pThis = (CGuardClient*)pParam;
	assert (pThis);

	log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] Event queue dispatcher for client started"), PEL_INFO);

	HANDLE hEvents[2] = 
	{
		pThis->m_hStopEvQueueDispatcher,
		pThis->m_hEventQueueSemaphore
	};

	DRV_RegisterInvisibleThread( pThis->m_DriverContext );

	while (1)
	{
		switch (WaitForMultipleObjects (2, hEvents, FALSE, INFINITE))
		{
		case WAIT_OBJECT_0:
			{
				log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] Stop event signalled, removing pending events (client )"), PEL_INFO);

				EnterCriticalSection (&pThis->m_hcsEventQueue);

				std::list<EventQueueElem> &EventQueue = pThis->m_EventQueue;

				std::list<EventQueueElem>::const_iterator i;
				
				for (i = EventQueue.begin (); i != EventQueue.end (); ++i)
				{
					// для уменьшения значения семафора
					WaitForSingleObject (pThis->m_hEventQueueSemaphore, 0);

					const EventQueueElem &QueueElem = *i;

					ULONG EventFlags = 0;
					DRV_GetEventInfo( i->m_pMessage, NULL, NULL, NULL, NULL, &EventFlags, NULL );

					BOOL bAllow = TRUE;
					if (EventFlags & _EVENT_FLAG_AUTO_VERDICT_DISCARD)
						bAllow = FALSE;

					log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] Setting verdict %d for client , message = 0x%x"), PEL_INFO,
											bAllow, i->m_pMessage );

					DRV_SetVerdict( pThis->m_DriverContext, i->m_pMessage, bAllow, FALSE, 0 );

					MemFree( NULL, (void**) &i->m_pMessage );
				}
				
				EventQueue.clear ();

				LeaveCriticalSection (&pThis->m_hcsEventQueue);

				log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] All pending events are removed (client )"), PEL_INFO);
			}
			goto QuitDispatcher;
		case WAIT_OBJECT_0 + 1:
			{
				std::list<EventQueueElem> &EventQueue = pThis->m_EventQueue;

				PVOID pMessage = NULL;

				EnterCriticalSection (&pThis->m_hcsEventQueue);

				log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] Event queue dispatcher thread")
							_T(" for client %d is dispatching event (%d events remaining in queue)..."), PEL_INFO,
								pThis->m_dwAppID, EventQueue.size () - 1);

				if (!EventQueue.empty ())
				{
					EventQueueElem &QueueElem = EventQueue.front ();
					pMessage = QueueElem.m_pMessage;
					EventQueue.pop_front ();
				}

				LeaveCriticalSection (&pThis->m_hcsEventQueue);

				ULONG uExpTime = 0;
				VERDICT Verdict = Verdict_Discard;

				if (pMessage && // shutdown event is empty or not signaled
						(pThis->m_hShutDownEvent == NULL  || WaitForSingleObject (pThis->m_hShutDownEvent, 0) != WAIT_OBJECT_0) &&
						// and quit event is empty or not signaled
						(pThis->m_hStopEvQueueDispatcher == NULL || WaitForSingleObject (pThis->m_hStopEvQueueDispatcher, 0) != WAIT_OBJECT_0))
				{
					log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] Client is dispatching event with message 0x%x"), PEL_INFO,
										pMessage);
					Verdict = pThis->OnEvent( pMessage );
				}
				
				log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] Client %d has dispatched event"), PEL_INFO, pThis->m_dwAppID);
				
				if (Verdict_WaitVerdictFromTestEvent == Verdict)
				{
					log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] CHECK Verdict_WaitVerdictFromTestEvent !!!"), PEL_ERROR );
					DRV_SetVerdict( pThis->m_DriverContext, pMessage, TRUE, FALSE, 0 );
				}
				else
				{
					ULONG DrvMark = DRV_GetEventMark( pMessage );

					log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] SetVerdict for DrvMark 0x%x (VERDICT)%d, (ExpTime)%d)..."),
						PEL_INFO, DrvMark, (int)Verdict, uExpTime);

					BOOL bAllow = TRUE;
					if (Verdict_Discard == Verdict)
						bAllow = FALSE;

					HRESULT SetVerdictStatus = DRV_SetVerdict( pThis->m_DriverContext, pMessage, bAllow, FALSE, 0 );

					log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] IDriverSetVerdict returned status 0x%x, client %d"), PEL_INFO, SetVerdictStatus, pThis->m_dwAppID);
				}

				MemFree( NULL, (void**) &pMessage );
			}

			break;
		default:
			assert (0);
			break;
		}
	}

QuitDispatcher:

	DRV_UnRegisterInvisibleThread( pThis->m_DriverContext );

	log.WriteFormat (_T("[EVENTQUEUEDISPATCHER] Event queue dispatcher thread for client %d is exiting"), PEL_INFO, pThis->m_dwAppID);

	return 0;
}

void CGuardClient::RemoveElemsFromEventQueue (UUID *pUuid, VERDICT VerdictOfRemovedElems)
{
	RPC_STATUS status;

	EnterCriticalSection (&m_hcsEventQueue);

	std::list<EventQueueElem>::iterator i;

	BOOL bAllow = TRUE;
	if (Verdict_Discard == VerdictOfRemovedElems)
		bAllow = FALSE;
	
	for (i = m_EventQueue.begin (); i != m_EventQueue.end (); )
	{
		PSINGLE_PARAM pUuidParam = DRV_GetEventParam (i->m_pMessage, 0, PF_FILTER_USER_ID, 0);

		if (pUuidParam && UuidEqual ((UUID *)pUuidParam->ParamValue, pUuid, &status))
		{
			// для уменьшения значения семафора
			WaitForSingleObject (m_hEventQueueSemaphore, 0);

			DRV_SetVerdict( m_DriverContext, i->m_pMessage, bAllow, FALSE, 0 );
			MemFree( NULL, &i->m_pMessage );
			i = m_EventQueue.erase (i);
		}
		else
		{
			++i;
		}
	}
	
	LeaveCriticalSection (&m_hcsEventQueue);
}

static bool IsThreadAlive (HANDLE hHandle) 
{	
	if( hHandle != NULL )
	{
		DWORD dwExitCode = STILL_ACTIVE; 
		return GetExitCodeThread(hHandle, &dwExitCode) && dwExitCode == STILL_ACTIVE;
	}

	return false;
}

DWORD WINAPI CGuardClient::MainProc (LPVOID pParam)
{
	CGuardClient* pThis = (CGuardClient*)pParam;
	assert (pThis);

	log.WriteFormat (_T("[GUARDTHREAD] Thread for client %d is starting..."), PEL_INFO, pThis->m_dwAppID);

	DRV_RegisterInvisibleThread( pThis->m_DriverContext );

	HRESULT hResult = DRV_ChangeActiveStatus( pThis->m_DriverContext, pThis->m_bInitialState );
	log.WriteFormat (_T("[GUARDTHREAD] change client %d state result 0x%x"), PEL_ERROR, pThis->m_dwAppID, hResult);

	log.WriteFormat (_T("[GUARDTHREAD] Client %d enters wait loop"), PEL_INFO, pThis->m_dwAppID);

	while (TRUE)
	{
		log.WriteFormat (_T("[GUARDTHREAD] Client %d is waiting for events"), PEL_INFO, pThis->m_dwAppID);

		PVOID pMessage = NULL;
		hResult = DRV_PrepareMessage( pThis->m_DriverContext, &pMessage );
		if (!SUCCEEDED( hResult ))
		{
			log.WriteFormat (_T("[GUARDTHREAD] client %d preapre message error 0x%x"), PEL_ERROR, pThis->m_dwAppID, hResult);
			break;
		}

		ULONG MessageSize = 0;
		PVOID pEventHdr = NULL;
		hResult = DRV_GetMessage( pThis->m_DriverContext, &pMessage, &MessageSize, &pEventHdr );

		ULONG DrvMark = 0;
		if (SUCCEEDED( hResult ))
			DrvMark = DRV_GetEventMark( pMessage );

		log.WriteFormat (_T("[GUARDTHREAD] client %d get message status 0x%x (DrvMark 0x%x)"),
			PEL_ERROR, pThis->m_dwAppID, hResult, DrvMark);

		if (!SUCCEEDED( hResult ))
		{
			MemFree( pThis->m_DriverContext, &pMessage );
			break;
		}

		DRV_PendingMessage( pThis->m_DriverContext, pMessage, 30 );
		DRV_Yeild( pThis->m_DriverContext, pMessage, 60 * 60 * 24 * 30 );
		
		log.WriteFormat (_T("[GUARDTHREAD] Adding client %d event to event queue..."), PEL_INFO, pThis->m_dwAppID);

		EnterCriticalSection (&pThis->m_hcsEventQueue);

		bool bPushedOk = false;

		try
		{
			EventQueueElem QueueElem;
			QueueElem.m_pMessage = pMessage;
			pThis->m_EventQueue.push_back (QueueElem);
			bPushedOk = true;
		}
		catch(std::bad_alloc &)
		{
			log.WriteFormat (_T("[GUARDTHREAD] Exception caught while adding event to %d client event queue..."), PEL_INFO, pThis->m_dwAppID);
			bPushedOk = false;
			DRV_SetVerdict( pThis->m_DriverContext, pMessage, TRUE, FALSE, 0 );
			MemFree( pThis->m_DriverContext, &pMessage );
		}

		LeaveCriticalSection (&pThis->m_hcsEventQueue);

		if (bPushedOk)
		{
			LONG lPrevSemCount = 0;
			ReleaseSemaphore (pThis->m_hEventQueueSemaphore, 1, &lPrevSemCount);
			log.WriteFormat (_T("[GUARDTHREAD] Event queue for client %d contains %d elements"), PEL_INFO, pThis->m_dwAppID, lPrevSemCount + 1);
		}
	}

	log.WriteFormat (_T("[GUARDTHREAD] Waiting for client %d failed (0x%08x)"), PEL_ERROR, pThis->m_dwAppID, GetLastError ());
	log.WriteFormat (_T("[GUARDTHREAD] Client %d exiting wait loop"), PEL_INFO, pThis->m_dwAppID);

	::SetEvent (pThis->m_hStopEvQueueDispatcher);

	if(IsThreadAlive(pThis->m_hEvQueueThread))
	{
		log.WriteFormat (_T("[GUARDTHREAD] Waiting queue thread..."), PEL_INFO);
		if(WaitForSingleObject(pThis->m_hEvQueueThread, QUEUE_THREAD_TERM_WAIT_TIMEOUT) == WAIT_TIMEOUT)
		{
			log.WriteFormat (_T("[GUARDTHREAD] Queue thread is not responding"), PEL_WARNING);
			TerminateThread(pThis->m_hEvQueueThread, 0);
		}
		log.WriteFormat (_T("[GUARDTHREAD] Queue thread exited"), PEL_INFO);
	}

	log.WriteFormat (_T("[GUARDTHREAD] Unregistering client %d"), PEL_INFO, pThis->m_dwAppID);
	
	DRV_UnRegister( &pThis->m_DriverContext );

	log.WriteFormat (_T("[GUARDTHREAD] Thread for client %d exited"), PEL_INFO, pThis->m_dwAppID);

	return 0;
}

BOOL CGuardClient::Create (HANDLE hShutDownEvent, ULONG uAppID, ULONG uPriority, DWORD dwClientFlags, BOOL bState)
{
	log.WriteFormat (_T("[GUARDCREATE] Creating client %d (priority = %d, flags = %x, state = %d)"), PEL_INFO, uAppID, uPriority, dwClientFlags, bState);

	if (m_DriverContext)
	{
		log.WriteFormat (_T("[GUARDCREATE] Creation failed, device already present"), PEL_ERROR);
		return FALSE;
	}

	m_bInitialState = bState;
	m_dwAppID = uAppID;

	HRESULT hResult = DRV_Register (
		&m_DriverContext,
		m_dwAppID,
		uPriority,
		dwClientFlags,
		0,
		DEADLOCKWDOG_TIMEOUT,
		MemAlloc,
		MemFree,
		0
		);

	if (!SUCCEEDED( hResult ))
	{
		log.WriteFormat (_T("[GUARDTHREAD] Registration of client %d failed"), PEL_ERROR, m_dwAppID);
		assert (FALSE);
		return FALSE;
	}
	
	hResult = DRV_PrepareMessageQueue( m_DriverContext, 1 );
	if (!SUCCEEDED( hResult ))
	{
		log.WriteFormat (_T("[GUARDTHREAD] Prepare queue for client %d failed"), PEL_ERROR, m_dwAppID);
		assert (FALSE);
		DRV_UnRegister( &m_DriverContext );
		return FALSE;
	}

	log.WriteFormat (_T("[GUARDTHREAD] Client %d registered"), PEL_INFO, m_dwAppID);

	m_hShutDownEvent = hShutDownEvent;

	DWORD dwThreadId = 0;
	m_hThread = CreateThread (NULL, 0, MainProc, this, 0, &dwThreadId);

	if (NULL == m_hThread)
	{
		log.WriteFormat (_T("[GUARDCREATE] Failed to create thread (0x%08x)"), PEL_ERROR, GetLastError ());
		return FALSE;
	}

	dwThreadId = 0;
	m_hEvQueueThread = CreateThread (NULL, 0, EventQueueDispatcher, this, 0, &dwThreadId);

	if (NULL == m_hEvQueueThread)
	{
		StopClient (false); //TODO
		log.WriteFormat (_T("[GUARDCREATE] Creation of event queue dispather thread failed"), PEL_ERROR);
		return FALSE;
	}

	log.WriteFormat (_T("[GUARDCREATE] Client %d successfully created"), PEL_INFO, uAppID);

	return TRUE;
}

BOOL CGuardClient::IsCreated ()
{
	if (m_DriverContext)
		return TRUE;

	return FALSE;
}

ULONG CGuardClient::AddFilter (DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, DWORD dwFlags, PFILTER_PARAM pFirstParam, ...)
{
	ULONG FilterID = 0;

	va_list ArgPtr;
	PFILTER_PARAM pNextParam = pFirstParam;
	ULONG ParamsCount = 0;

	va_start (ArgPtr, pFirstParam);
	while (pNextParam)
	{
		pNextParam = va_arg (ArgPtr, PFILTER_PARAM);
		ParamsCount++;
	}
	va_end (ArgPtr);

	PFILTER_PARAM* pParams = NULL;
	if (ParamsCount)
	{
		pParams = (PFILTER_PARAM*) MemAlloc( NULL, sizeof(PFILTER_PARAM) * ParamsCount, 0);
		if (!pParams)
			return 0;
		ParamsCount = 0;
		pNextParam = pFirstParam;

		va_start (ArgPtr, pFirstParam);
		while (pNextParam)
		{
			pParams[ParamsCount] = pNextParam;
			ParamsCount++;

			pNextParam = va_arg (ArgPtr, PFILTER_PARAM);
		}
		va_end (ArgPtr);
	}

	ULONG FilterPos = 0;
	ULONG AddMethod = FLTCTL_ADD;

	HRESULT hResult = DRV_AddFilterEx (
		&FilterID,
		m_DriverContext,
		0,
		dwFlags,
		HookID,
		FunctionMj,
		FunctionMi,
		0,
		PreProcessing,
		AddMethod,
		FilterPos,
		pParams,
		ParamsCount
		);

	MemFree( NULL, (void**) &pParams );

	if (SUCCEEDED( hResult ))
		return FilterID;
	
	return FilterID;
}

ULONG CGuardClient::AddFilter (const CGuardFilter& filter, DWORD &dwOrderID)
{
	ULONG FilterID = 0;
	CGuardFilter temp_filter = filter;

	dwOrderID = dwOrderID == -1 ? (GetLastFilterOrder () + 1) : (dwOrderID + 1);
	temp_filter.AddParam (PF_FILTER_ORDER_ID, FLT_PARAM_NOP, sizeof (DWORD), (void*)&dwOrderID, _FILTER_PARAM_FLAG_NONE);

	PFILTER_TRANSMIT pFilter = temp_filter.AllocateFilter ();
	
	if (!pFilter)
		return 0;

	PFILTER_PARAM* pParams = NULL;
	if (pFilter->m_ParamsCount)
	{
		pParams = (PFILTER_PARAM*) MemAlloc( NULL, sizeof(PFILTER_PARAM) * pFilter->m_ParamsCount, 0);
		if (!pParams)
		{
			free (pFilter);
			return 0;
		}
		
		PFILTER_PARAM pNextParam = (PFILTER_PARAM) pFilter->m_Params;
		for (ULONG cou = 0; cou < pFilter->m_ParamsCount; cou++ )
		{
			pParams[cou] = pNextParam;

			pNextParam = (PFILTER_PARAM)((BYTE*)(pNextParam) + pNextParam->m_ParamSize + sizeof(FILTER_PARAM));
		}
	}

	ULONG FilterPos = 0;
	ULONG AddMethod = FLTCTL_ADD;

	HRESULT hResult = DRV_AddFilterEx (
		&FilterID,
		m_DriverContext,
		0,
		pFilter->m_Flags,
		pFilter->m_HookID,
		pFilter->m_FunctionMj,
		pFilter->m_FunctionMi,
		0,
		pFilter->m_ProcessingType,
		AddMethod,
		FilterPos,
		pParams,
		pFilter->m_ParamsCount
		);

	log.WriteFormat (_T("[DRIVEREXCHANGE] add filter result 0x%x (filterid = %d)"), PEL_INFO, hResult, FilterID );

	MemFree( NULL, (void**) &pParams );
	free (pFilter);

	if (SUCCEEDED( hResult ))
		return FilterID;

	return 0;
}

ULONG CGuardClient::AddFilterAfter (const ULONG uFltID, const CGuardFilter& filter)
{
	ULONG FilterID = 0;

	PFILTER_TRANSMIT pFilter = filter.AllocateFilter ();
	
	if (!pFilter)
		return 0;

	PFILTER_PARAM* pParams = NULL;
	if (pFilter->m_ParamsCount)
	{
		pParams = (PFILTER_PARAM*) MemAlloc( NULL, sizeof(PFILTER_PARAM) * pFilter->m_ParamsCount, 0);
		if (!pParams)
		{
			free (pFilter);
			return 0;
		}
		
		PFILTER_PARAM pNextParam = (PFILTER_PARAM) pFilter->m_Params;
		for (ULONG cou = 0; cou < pFilter->m_ParamsCount; cou++ )
		{
			pParams[cou] = pNextParam;

			pNextParam = (PFILTER_PARAM)((BYTE*)(pNextParam) + pNextParam->m_ParamSize + sizeof(FILTER_PARAM));
		}
	}

	ULONG FilterPos = uFltID;
	ULONG AddMethod = FLTCTL_ADD2POSITION;

	HRESULT hResult = DRV_AddFilterEx (
		&FilterID,
		m_DriverContext,
		0,
		pFilter->m_Flags,
		pFilter->m_HookID,
		pFilter->m_FunctionMj,
		pFilter->m_FunctionMi,
		0,
		pFilter->m_ProcessingType,
		AddMethod,
		FilterPos,
		pParams,
		pFilter->m_ParamsCount
		);

	log.WriteFormat (_T("[DRIVEREXCHANGE] add filter result 0x%x (filterid = %d)"), PEL_INFO, hResult, FilterID );

	MemFree( NULL, (void**) &pParams );
	free (pFilter);

	if (SUCCEEDED( hResult ))
		return FilterID;

	return 0;
}


BOOL CGuardClient::IsFiltersPresent ()
{
	PVOID pFilter = NULL;
	HRESULT hResult = DRV_GetFilterFirst( m_DriverContext, &pFilter );
	if (SUCCEEDED( hResult ))
	{
		MemFree( NULL, &pFilter );
		return TRUE;
	}

	return FALSE;
}


/*BOOL CGuardClient::DriverExchange (DWORD driverID, DWORD ioctl, void * pBuf, DWORD * dwBufSize, BOOL IsInBufferValid)
{
	if (NULL == (HANDLE)m_hDevice)
	{
		assert (FALSE);
		return FALSE;
	}

	log.WriteFormat (_T("[DRIVEREXCHANGE] Starting DriverExchange"), PEL_INFO);

	PEXTERNAL_DRV_REQUEST pRequest = NULL;
	BOOL status = FALSE;

	if (IsInBufferValid || (*dwBufSize < sizeof (EXTERNAL_DRV_REQUEST)))
	{
		log.WriteFormat (_T("[DRIVEREXCHANGE] Buffer too small - realloc"), PEL_INFO);

		pRequest = (PEXTERNAL_DRV_REQUEST)malloc (sizeof (EXTERNAL_DRV_REQUEST)+ *dwBufSize);

		if (!pRequest)
		{
			return FALSE;
		}

		pRequest->m_DrvID = driverID;
		pRequest->m_AppID = m_AppReg.m_AppID;
		pRequest->m_BufferSize = *dwBufSize;
		pRequest->m_IOCTL = ioctl;

		if (pBuf && (*dwBufSize > 0))
		{
			memcpy (pRequest->m_Buffer, pBuf, *dwBufSize);
		}
		

		status = IDriverExternalDrvRequest (m_hDevice, pRequest, pBuf, dwBufSize);

		free (pRequest);		
	}
	else
	{

		log.WriteFormat (_T("[DRIVEREXCHANGE] Buffer ok...IDriverExternalDrvRequest"), PEL_INFO);

		pRequest = (PEXTERNAL_DRV_REQUEST)pBuf;

		pRequest->m_DrvID = driverID;
		pRequest->m_AppID = m_AppReg.m_AppID;
		pRequest->m_BufferSize = *dwBufSize;
		pRequest->m_IOCTL = ioctl;

		log.WriteFormat (_T("[DRIVEREXCHANGE] IDriverExternalDrvRequest input buffer size = %d"), PEL_INFO, *dwBufSize);

		status = IDriverExternalDrvRequest (m_hDevice, pRequest, pBuf, dwBufSize);

		log.WriteFormat (_T("[DRIVEREXCHANGE] IDriverExternalDrvRequest output buffer size = %d"), PEL_INFO, *dwBufSize);
	}

	log.WriteFormat (_T("[DRIVEREXCHANGE] IDriverExternalDrvRequest return %d"), PEL_INFO, status);

	return status;
}*/

ULONG CGuardClient::AddFilter2Top (CGuardFilter &filter)
{
	PFILTER_TRANSMIT pFilter = filter.AllocateFilter ();
	
	if (!pFilter)
		return 0;

	PFILTER_PARAM* pParams = NULL;
	if (pFilter->m_ParamsCount)
	{
		pParams = (PFILTER_PARAM*) MemAlloc( NULL, sizeof(PFILTER_PARAM) * pFilter->m_ParamsCount, 0);
		if (!pParams)
		{
			free (pFilter);
			return 0;
		}
		
		PFILTER_PARAM pNextParam = (PFILTER_PARAM) pFilter->m_Params;
		for (ULONG cou = 0; cou < pFilter->m_ParamsCount; cou++ )
		{
			pParams[cou] = pNextParam;

			pNextParam = (PFILTER_PARAM)((BYTE*)(pNextParam) + pNextParam->m_ParamSize + sizeof(FILTER_PARAM));
		}
	}

	ULONG FilterPos = 0;
	ULONG AddMethod = FLTCTL_ADD2TOP;

	ULONG FilterID = 0;

	HRESULT hResult = DRV_AddFilterEx (
		&FilterID,
		m_DriverContext,
		0,
		pFilter->m_Flags,
		pFilter->m_HookID,
		pFilter->m_FunctionMj,
		pFilter->m_FunctionMi,
		0,
		pFilter->m_ProcessingType,
		AddMethod,
		FilterPos,
		pParams,
		pFilter->m_ParamsCount
		);

	log.WriteFormat (_T("[DRIVEREXCHANGE] add filter result 0x%x (filterid = %d)"), PEL_INFO, hResult, FilterID );

	MemFree( NULL, (void**) &pParams );
	free (pFilter);

	if (SUCCEEDED( hResult ))
		return FilterID;

	return 0;
}

BOOL CGuardClient::Enable ()
{
	HRESULT hResult = DRV_ChangeActiveStatus( m_DriverContext, TRUE );
	if (SUCCEEDED( hResult ))
	{
		log.WriteFormat (_T("[DRIVEREXCHANGE] app turned on "), PEL_ERROR);
		return TRUE;
	}

	log.WriteFormat (_T("[DRIVEREXCHANGE] Cannot turn app on"), PEL_ERROR);
	return FALSE;
}

BOOL CGuardClient::Disable ()
{
	HRESULT hResult = DRV_ChangeActiveStatus( m_DriverContext, FALSE );
	if (SUCCEEDED( hResult ))
	{
		log.WriteFormat (_T("[DRIVEREXCHANGE] app now in sleep mode"), PEL_ERROR);
		return TRUE;
	}

	log.WriteFormat (_T("[DRIVEREXCHANGE] Cannot sleep app"), PEL_ERROR);
	return FALSE;
}

BOOL CGuardClient::RemoveBlockFilters ()
{
	return RemoveFiltersWithParam (PF_BLOCK_FILTER);
}

BOOL CGuardClient::RemoveFiltersWithParam (ULONG nParam, bool bForceAll)
{
	HRESULT hResult;
	if(bForceAll)
	{
		hResult = DRV_FiltersClear( m_DriverContext );
		log.WriteFormat (_T("[DRIVEREXCHANGE] Clear filters complete status 0x%x"), PEL_ERROR, hResult );
	}
	else
	{
		FILTER_ID_LST lst_id;

		GetFiltersWithParam (nParam, lst_id, bForceAll);

		for (FILTER_ID_LST::iterator i = lst_id.begin (); i != lst_id.end (); ++i)
		{
			ULONG FilterId = *i;

			hResult = DRV_FilterDelete( m_DriverContext, FilterId );
			log.WriteFormat (_T("[DRIVEREXCHANGE] Filter %d delete status 0x%x"), PEL_ERROR, FilterId, hResult );
		}
	}

	return TRUE;
}

BOOL CGuardClient::RemoveAllFilters ()
{
	return RemoveFiltersWithParam (0, true);
}

BOOL CGuardClient::AddFilterList (const FLTLIST &flt_list)
{
	BOOL bStatus = FALSE;

	DWORD dwOrderID = -1;
	for (FLTLIST::const_iterator i = flt_list.begin (); i != flt_list.end (); i ++)
		bStatus |= !AddFilter (*i, dwOrderID);
	
	//if (!bStatus)
	//	SaveFilters ();

	return !bStatus;

}

BOOL CGuardClient::RemoveAllUserFilters (bool bForceAll)
{
	return RemoveFiltersWithParam (PF_FILTER_USER_ID, bForceAll);
}

BOOL CGuardClient::GetAllUserFilters (FLTLIST & flt_list, bool bForceAll)
{
	PVOID pFilter = NULL;

	HRESULT hResult = DRV_GetFilterFirst( m_DriverContext, &pFilter );
	while (SUCCEEDED( hResult ))
	{
		ULONG FilterId;
		DRV_GetFilterInfo( pFilter, &FilterId, NULL, NULL, NULL, NULL, NULL, NULL, NULL );

		PFILTER_PARAM pParam = DRV_GetFilterParam( pFilter, PF_FILTER_USER_ID );
		if (bForceAll || pParam)
			flt_list.insert (flt_list.end (), CGuardFilter( pFilter ));

		MemFree( NULL, &pFilter );

		hResult = DRV_FilterNext( m_DriverContext, FilterId, &pFilter );
	}

	return TRUE;
}


BOOL CGuardClient::GetFilter (const DWORD dwFilterID, CGuardFilter & filter)
{
	PVOID pFilter = NULL;

	HRESULT hResult = DRV_GetFilterFirst( m_DriverContext, &pFilter );
	while (SUCCEEDED( hResult ))
	{
		ULONG FilterId;
		DRV_GetFilterInfo( pFilter, &FilterId, NULL, NULL, NULL, NULL, NULL, NULL, NULL );

		if (FilterId == dwFilterID)
			break;

		MemFree( NULL, &pFilter );

		hResult = DRV_FilterNext( m_DriverContext, FilterId, &pFilter );
	}

	if (pFilter)
	{
		filter = CGuardFilter::CGuardFilter( pFilter );
		return TRUE;
	}

	return FALSE;
}

BOOL CGuardClient::RemoveUserFilter (const UUID & uidUsrID)
{
	FILTER_ID_LST lst_id;

	PVOID pFilter = NULL;

	HRESULT hResult = DRV_GetFilterFirst( m_DriverContext, &pFilter );
	while (SUCCEEDED( hResult ))
	{
		ULONG FilterId;
		DRV_GetFilterInfo( pFilter, &FilterId, NULL, NULL, NULL, NULL, NULL, NULL, NULL );

		PFILTER_PARAM pParam = DRV_GetFilterParam( pFilter, PF_FILTER_USER_ID );
		if (pParam)
		{
			RPC_STATUS status;
			if (UuidEqual ((UUID*)&uidUsrID, (UUID*)pParam->m_ParamValue, &status))
				lst_id.insert (lst_id.end (), FilterId);
		}

		MemFree( NULL, &pFilter );

		hResult = DRV_FilterNext( m_DriverContext, FilterId, &pFilter );
	}

	for (FILTER_ID_LST::iterator i = lst_id.begin (); i != lst_id.end (); i++)
	{
		ULONG FilterId = *i;
		
		hResult = DRV_FilterDelete( m_DriverContext, FilterId );
		log.WriteFormat (_T("[GUARDCLIENT] RemoveUserFilter %d result 0x%x"), PEL_ERROR, hResult, FilterId );
	}
	
	return TRUE;
}

BOOL CGuardClient::RemoveFilter (ULONG uFilterID)
{
	HRESULT hResult = DRV_FilterDelete( m_DriverContext, uFilterID );

	log.WriteFormat (_T("[GUARDCLIENT] del filter %d result 0x%x"), PEL_ERROR, hResult, uFilterID );

	if (SUCCEEDED( hResult))
		return TRUE;

	return FALSE;
}

DWORD CGuardClient::GetLastFilterOrder ()
{
	DWORD uFilterID = 0;
	
	PVOID pFilter = NULL;
	HRESULT hResult = DRV_GetFilterFirst( m_DriverContext, &pFilter );

	while (SUCCEEDED( hResult ))
	{
		ULONG FilterId = 0;
		DRV_GetFilterInfo( pFilter, &FilterId, NULL, NULL, NULL, NULL, NULL, NULL, NULL );

		PFILTER_PARAM pParam = DRV_GetFilterParam( pFilter, PF_FILTER_ORDER_ID );
		if (pParam)
			uFilterID = max (*(DWORD*)pParam->m_ParamValue, uFilterID);

		MemFree( NULL, &pFilter );

		hResult = DRV_FilterNext( m_DriverContext, FilterId, &pFilter );
	}

	return uFilterID;
}

BOOL CGuardClient::SaveFilters ()
{
	HRESULT hResult = DRV_SaveFilters( m_DriverContext );
	log.WriteFormat (_T("[GUARDCLIENT] save result 0x%x for client %d"), PEL_ERROR, hResult, m_dwAppID );
	return FALSE;
}

HANDLE CGuardClient::GetDriverContext ()
{
	return m_DriverContext;
}

void CGuardClient::StopClient (BOOL bResident)
{
	if (m_hThread)
	{
		log.WriteFormat (_T("[STOPGUARD] Trying to stop client %d..."), PEL_INFO, m_dwAppID);

		m_bResident = bResident;
		
		OnStopClient (bResident);
		
		DRV_BreakConnection( m_DriverContext );

		log.WriteFormat (_T("[STOPGUARD] Waiting client %d to stop..."), PEL_INFO, m_dwAppID);
		
		HANDLE hThreadsToWait[] = {m_hThread, m_hEvQueueThread};

		DWORD dwWaitStatus = ::WaitForMultipleObjects (2, hThreadsToWait, TRUE, GUARD_THREAD_TERM_WAIT_TIMEOUT);
		
		if (dwWaitStatus == WAIT_TIMEOUT)
		{
			SuspendThread (m_hThread);
			SuspendThread (m_hEvQueueThread);

			log.WriteFormat (_T("[STOPGUARD] Client %d is not responding"), PEL_INFO, m_dwAppID);

			// поставим вердикты на необработынные события
			std::list<EventQueueElem>::iterator i;
			for (i = m_EventQueue.begin (); i != m_EventQueue.end (); ++i)
			{
				log.WriteFormat (_T("[STOPGUARD] Setting pass verdict for client %d, message = 0x%x"), PEL_INFO, m_dwAppID, i->m_pMessage);
				DRV_SetVerdict( m_DriverContext, i->m_pMessage, TRUE, FALSE, 0 );
				MemFree( NULL, &i->m_pMessage );
			}
			//

			if (m_DriverContext)
			{			
				DRV_UnRegisterInvisibleThread( m_DriverContext );
				DRV_UnRegister( &m_DriverContext );
			}

			if (m_hThread)
			{
				if (IsThreadAlive (m_hThread))
					::TerminateThread (m_hThread, 0);
				CloseHandle (m_hThread);
				m_hThread = NULL;
			}

			if (m_hEvQueueThread)
			{
				if (IsThreadAlive (m_hEvQueueThread))
					::TerminateThread (m_hEvQueueThread, 0);
				CloseHandle (m_hEvQueueThread);
				m_hEvQueueThread = NULL;
			}
		}
		else
		{
			log.WriteFormat (_T("[STOPGUARD] Client %d is stopped normaly."), PEL_INFO, m_dwAppID);
			
			CloseHandle (m_hThread);
			m_hThread = NULL;

			CloseHandle (m_hEvQueueThread);
			m_hEvQueueThread = NULL;
		}

		m_EventQueue.clear ();
		::ResetEvent (m_hStopEvQueueDispatcher);
	}
}

void CGuardClient::GetFiltersWithParam (ULONG nParam, FILTER_ID_LST &filters, bool bForceAll)
{
	PVOID pFilter = NULL;

	HRESULT hResult = DRV_GetFilterFirst( m_DriverContext, &pFilter );
	while(SUCCEEDED( hResult ))
	{
		ULONG FilterId;
		DRV_GetFilterInfo( pFilter, &FilterId, NULL, NULL, NULL, NULL, NULL, NULL, NULL );

		if (bForceAll || DRV_GetFilterParam( pFilter, nParam ))
		{
			filters.insert (filters.end (), FilterId);
		}

		MemFree( NULL, &pFilter );

		hResult = DRV_FilterNext( m_DriverContext, FilterId, &pFilter );
	}
}

BOOL
CGuardClient::ChangeFilterParam (
	DWORD dwFilterID,
	ULONG nParam,
	ULONG dwParamFlags,
	FLT_PARAM_OPERATION ParamFlt,
	ULONG uParamSize,
	LPCVOID pParamValue
	)
{
	BOOL bRet = FALSE;

	PFILTER_PARAM pParam = (PFILTER_PARAM) MemAlloc( NULL, uParamSize + sizeof(FILTER_PARAM), 0 );
	if(!pParam)
		return FALSE;

	pParam->m_ParamFlags = dwParamFlags;
	pParam->m_ParamFlt = ParamFlt;
	pParam->m_ParamID = nParam;
	pParam->m_ParamSize = uParamSize;
	memcpy( pParam->m_ParamValue, pParamValue, uParamSize );

	HRESULT hResult = DRV_ChangeFilterParam( m_DriverContext, dwFilterID, pParam );
	if (SUCCEEDED( hResult ))
		bRet = TRUE;

	MemFree( NULL, (void**) &pParam );

	return bRet;
}

#include "stdafx.h"
#include "MCOUAsync.h"
#include <algorithm>
#include "OUMessage.h"
#include "Util.h"

static CAsyncProcessor *g_pAsyncProcessor = NULL;

bool StartAsyncProcessorThread()
{
	PR_TRACE((0, prtIMPORTANT, "Starting async processor thread..."));

	bool bRetValue = false;

	if(g_pAsyncProcessor)
	{
		TermAsyncProcessorThread();
		if(g_pAsyncProcessor)
		{
			PR_TRACE((0, prtIMPORTANT, "Async processor thread not started (previous instance)"));
			return false;
		}
	}

	g_pAsyncProcessor = new CAsyncProcessor();
	if(g_pAsyncProcessor && (bRetValue = g_pAsyncProcessor->Start()) == false)
		TermAsyncProcessorThread();

	if(bRetValue)
		PR_TRACE((0, prtIMPORTANT, "Async processor thread started successfulln"));
	else
		PR_TRACE((0, prtIMPORTANT, "Failed to start async processor threan"));

	return bRetValue;
}

bool PrepareTermAsyncProcessorThread()
{
	bool bRetValue = true;
	PR_TRACE((0, prtIMPORTANT, "Preparing to stop async processor thread..."));
	if(g_pAsyncProcessor)
		bRetValue = g_pAsyncProcessor->PrepareStop();
	PR_TRACE((0, prtIMPORTANT, "Stop of async processor thread prepared"));
	return bRetValue;
}

bool TermAsyncProcessorThread()
{
	PR_TRACE((0, prtIMPORTANT, "Stopping async processor thread..."));

	delete g_pAsyncProcessor;
	g_pAsyncProcessor = NULL;

	PR_TRACE((0, prtIMPORTANT, "Async processor thread stopped"));

	return true;
}

bool AddMessageToAsyncThread(LPENTRYID lpEntryId, ULONG ulEntryIdSize, LPMDB lpIMsgStore, bool bIsIncoming, HWND hOutlookWnd)
{
	if(lpEntryId == NULL || ulEntryIdSize == 0)
		return false;
	if(!g_pAsyncProcessor)
		return false;
	return g_pAsyncProcessor->AddMessage(lpEntryId, ulEntryIdSize, lpIMsgStore, bIsIncoming, hOutlookWnd);
}

//////////////////////////////////////////////////////////////////////////

CAsyncProcessor::CAsyncProcessor() :
	m_hWorkingThread(NULL)
{
	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!m_hStopEvent)
		PR_TRACE((0, prtIMPORTANT, "Failed to create stop event"));
	m_hQueueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hQueueEvent)
		PR_TRACE((0, prtIMPORTANT, "Failed to create queue event"));
	InitializeCriticalSection(&m_csQueueLock);
}

CAsyncProcessor::~CAsyncProcessor()
{
	Stop();

	if(m_hStopEvent)
		CloseHandle(m_hStopEvent);
	if(m_hQueueEvent)
		CloseHandle(m_hQueueEvent);
	DeleteCriticalSection(&m_csQueueLock);
}

struct CAsyncProcThreadParam
{
	CAsyncProcessor *pThis;
};

bool CAsyncProcessor::Start()
{
	if(m_hWorkingThread)
		return true; // already started
	if(!m_hStopEvent)
		return false;
	CAsyncProcThreadParam *pThreadParam = new CAsyncProcThreadParam;
	pThreadParam->pThis = this;	

	DWORD dwThreadId;
	m_hWorkingThread = CreateThread(NULL, 0, WorkingThread, pThreadParam, 0, &dwThreadId);
	if(m_hWorkingThread == NULL)
	{
		delete pThreadParam;
		PR_TRACE((0, prtIMPORTANT, "Failed to create async working thread (0x%08x)", GetLastError()));
	}

	return (m_hWorkingThread != NULL);
}

bool CAsyncProcessor::PrepareStop()
{
	if(!m_hStopEvent)
		return false;
	if(!m_hWorkingThread)
		return true;
	SetEvent(m_hStopEvent);
	return true;
}

bool CAsyncProcessor::Stop()
{
	if(!m_hStopEvent)
		return false;
	if(!m_hWorkingThread)
		return true;
	
	SetEvent(m_hStopEvent);
	if(WaitForSingleObject(m_hWorkingThread, ASYNC_THREAD_WAIT_COMPLETE_TIMEOUT) == WAIT_TIMEOUT)
	{
		PR_TRACE((0, prtIMPORTANT, "Unable to stop async processor thread, thread should be terminated"));
		TerminateThread(m_hWorkingThread, -1);
	}
	CloseHandle(m_hWorkingThread);
	m_hWorkingThread = NULL;

	return true;
}

bool CAsyncProcessor::AddMessage(LPENTRYID lpEntryId, ULONG ulEntryIdSize, LPMDB lpIMsgStore, bool bIsIncoming, HWND hOutlookWnd)
{
	if(lpEntryId == NULL || ulEntryIdSize == 0)
		return false;
	if(m_hStopEvent && WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0)
	{
		PR_TRACE((0, prtIMPORTANT, "Async processor exiting, message not added to queue"));
		return false;
	}

	PR_TRACE((0, prtIMPORTANT, "Adding message to queue..."));
	QueueElement element(std::vector<BYTE>((LPBYTE)lpEntryId, (LPBYTE)lpEntryId + ulEntryIdSize), lpIMsgStore, bIsIncoming, hOutlookWnd);
	EnterCriticalSection(&m_csQueueLock);
	if(std::find(m_Queue.begin(), m_Queue.end(), element) == m_Queue.end())
	{
		m_Queue.push_back(element);
		PR_TRACE((0, prtIMPORTANT, "Message added to queue, queue now contains %d messages", m_Queue.size()));
		SetEvent(m_hQueueEvent);
	}
	else
	{
		PR_TRACE((0, prtIMPORTANT, "Message already present in queue"));
	}
	LeaveCriticalSection(&m_csQueueLock);
	
	return true;
}

DWORD CALLBACK CAsyncProcessor::WorkingThread(void *pParam)
{
	HRESULT hr = g_pMAPIEDK->pMAPIInitialize(NULL);
	if(FAILED(hr))
		PR_TRACE((0, prtIMPORTANT, "[ASYNC] Failed to initialize MAPI (0x%08x)", hr));
	DWORD dwRetValue = ((CAsyncProcThreadParam *)pParam)->pThis->WorkingThreadImpl(pParam);
	g_pMAPIEDK->pMAPIUninitialize();

	return dwRetValue;
}

DWORD CAsyncProcessor::WorkingThreadImpl(void *pParam)
{
	CAsyncProcThreadParam *pThreadParam = (CAsyncProcThreadParam *)pParam;
	delete pThreadParam;
	pThreadParam = NULL;

	PR_TRACE((0, prtIMPORTANT, "[ASYNC] Async processor thread is waiting for events..."));

	HANDLE hWaitHandles[] = { m_hStopEvent, m_hQueueEvent };
	DWORD dwWaitResult;
	QueueElement element;
	while((dwWaitResult = WaitForMultipleObjects(sizeof(hWaitHandles) / sizeof(hWaitHandles[0]),
													hWaitHandles,
													FALSE,
													INFINITE)) != WAIT_OBJECT_0 + 0)
	{
		if(dwWaitResult == WAIT_OBJECT_0 + 1)
		{
			while(WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
			{
				EnterCriticalSection(&m_csQueueLock);
				if(m_Queue.empty())
				{
					PR_TRACE((0, prtIMPORTANT, "[ASYNC] Message queue is empty"));
					LeaveCriticalSection(&m_csQueueLock);
					break;
				}
	
				PR_TRACE((0, prtIMPORTANT, "[ASYNC] Processing message from queue, %d messages remain...", m_Queue.size() - 1));
				element = m_Queue.front();
				m_Queue.pop_front();
				LeaveCriticalSection(&m_csQueueLock);

				MCOU::MCState state = MCOU::CheckMCState();
				if(state != MCOU::mcsOPEnabledDuplex && state != MCOU::mcsOPEnabledIncoming)
				{
					PR_TRACE((0, prtIMPORTANT, "[ASYNC] Processing of message skipped, state is %d", state));
					continue;
				}

				LPMDB lpIMsgStore = element.GetIMsgStore();
				
				if(lpIMsgStore)
				{
					ULONG cbEntryID = 0;
					LPENTRYID lpEntryID = NULL;

					element.GetEntryID(cbEntryID, lpEntryID);

					LPMESSAGE lpMsg = NULL;
					ULONG ulObjType = 0;
					HRESULT hr = lpIMsgStore->OpenEntry(cbEntryID,
												lpEntryID,
												NULL,
												MAPI_BEST_ACCESS,
												&ulObjType,
												(LPUNKNOWN *)&lpMsg);

					if(hr == S_OK)
					{
						COUMessage Message;
						hr = Message.CreateFromIMessage(lpMsg, lpIMsgStore, element.GetOutlookHWND(), lpEntryID, cbEntryID);
						if(hr == S_OK)
						{
							// попробуем снять property, чтобы отсечь NTE_FAIL и т.п. (см. bug 22512)
							if(Message.CheckTestProperty())
								PR_TRACE((0, prtIMPORTANT, "Failed to get test property, message processing skipped"));
							else if(!Message.Process(element.GetIsIncoming(), true))
								PR_TRACE((0, prtIMPORTANT, "[ASYNC] Failed to check message"));
						}
						else
							PR_TRACE((0, prtIMPORTANT, "[ASYNC] Failed to create message object (0x%08x)", hr));
					}
					else
						PR_TRACE((0, prtIMPORTANT, "[ASYNC] Failed to open message (0x%08x)", hr));

					if(lpMsg)
						lpMsg->Release();

					PR_TRACE((0, prtIMPORTANT, "[ASYNC] Processing of queued message finished"));
				}
				else
					PR_TRACE((0, prtIMPORTANT, "[ASYNC] No message store"));

				if(lpIMsgStore)
					lpIMsgStore->Release();
			}
		}
		else
		{
			PR_TRACE((0, prtIMPORTANT, "[ASYNC] Unknon event (%d)...", dwWaitResult));
		}
	}

	PR_TRACE((0, prtIMPORTANT, "[ASYNC] Async processor thread exiting"));

	return 0;
}

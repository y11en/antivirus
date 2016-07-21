// ThCheckSelfCopy.cpp: implementation of the CThCheckSelfCopy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThCheckSelfCopy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CCheckSelfCopyQueue::PushEvent(PRequest_CheckSelfCopy pRequest)
{
	CAutoLock auto_lock(&m_Sync);

	m_SelfCopyQueue.push_back(pRequest);
}

PRequest_CheckSelfCopy CCheckSelfCopyQueue::PopEvent()
{
	CAutoLock auto_lock(&m_Sync);

	if (m_SelfCopyQueue.empty())
		return NULL;

	PRequest_CheckSelfCopy pRequest = m_SelfCopyQueue[0];
	m_SelfCopyQueue.erase(m_SelfCopyQueue.begin());

	return pRequest;
}

//+ ------------------------------------------------------------------------------------------

#include "ProcessRecognizer.h"
void CheckSelfCopyProc(PRequest_CheckSelfCopy pRequest)
{
	CProcessRecognizer recNew;
	recNew.RecognizeWithSizeCheck(pRequest->m_NewFileName.GetBuffer(0), pRequest->m_HashC.GetObjectSize());
	pRequest->m_NewFileName.ReleaseBuffer();

	TCHAR dbgmsg[4096];
	wsprintf(dbgmsg, _T("process '%s': check self copy to '%s'\n"), pRequest->m_ProcName, pRequest->m_NewFileName);
	OutputDebugString(dbgmsg);

	if (recNew.IsRecognized())
	{
		if (recNew.IsEQU(pRequest->m_HashC.GetHash()))
		{
			GlobalAlert();
			gpProcList->PushAlert_SelfCopy(pRequest->m_ProcessId, &pRequest->m_ProcName, 
				&pRequest->m_ImagePath, &pRequest->m_NewFileName);

		}
	}
}
//+ ------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD WINAPI CheckSelfCopyThread(LPVOID lpThreadParameter)
{
	ThreadSync* pThreadSync = (ThreadSync*) lpThreadParameter;
	if (pThreadSync == NULL)
		return -1;

	CThCheckSelfCopy* pThreadParam = (CThCheckSelfCopy*) pThreadSync->m_pThreadParam;

	HANDLE hStopEvent = pThreadSync->m_hStopEvent;
	HANDLE hSingleEvent = pThreadParam->Init();

	if (hSingleEvent == NULL)
		return -1;

	pThreadSync->SetStartSucceed();

	HANDLE handles[2];
	handles[0] = hStopEvent;
	handles[1] = hSingleEvent;

	DWORD dwResult = WAIT_TIMEOUT;

	PRequest_CheckSelfCopy pRequest;
	bool bExit = false;

	while (!bExit)
	{
		dwResult = WaitForMultipleObjects(sizeof(handles) / sizeof(handles[0]), handles, FALSE, 100);

		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			bExit = true;
			break;
		case WAIT_OBJECT_0 + 1:
		default:
			{
				// processing single check
				pRequest = pThreadParam->m_CheckSelfCopyQueue.PopEvent();
				if (pRequest != NULL)
				{
					switch (pRequest->m_dwID)
					{
					case 0:
						CheckSelfCopyProc(pRequest);
						break;
					case 1:
						gpProcList->PushAlert_TrojanActivity(pRequest->m_ProcessId, &pRequest->m_ProcName, &pRequest->m_ImagePath);
						break;
					}
					delete pRequest;
				}
			}
			break;
		}
	}

	pThreadParam->BeforeExit();
	return 0;
}


CThCheckSelfCopy::CThCheckSelfCopy()
{
	m_hNewEvent = NULL;
	m_bThreadExist = false;
}

CThCheckSelfCopy::~CThCheckSelfCopy()
{

}

bool CThCheckSelfCopy::StartLoop()
{
	ThreadSync ThSync(this);
	m_hThread = ThSync.StartThread(CheckSelfCopyThread, m_hStopEvent);
	if (m_hThread == INVALID_HANDLE_VALUE)
		return false;

	m_bThreadExist = true;
	return true;
}

void  CThCheckSelfCopy::StopLoop()
{
	SetEvent(m_hStopEvent);
}

HANDLE CThCheckSelfCopy::Init()
{
	m_hNewEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return m_hNewEvent;
}

void CThCheckSelfCopy::BeforeExit()
{
	m_bThreadExist = false;
	if (m_hNewEvent != NULL)
	{
		CloseHandle(m_hNewEvent);
		m_hNewEvent = NULL;
	}
}

void CThCheckSelfCopy::CheckSelfCopy(ULONG ProcessId, CHashContainer& HashC, CString *pProcName, CString* pImagePath, 
									 PWCHAR pwchNewFileName)
{
	if (m_bThreadExist == false)
		return;

	Request_CheckSelfCopy* pRequst = new Request_CheckSelfCopy;
	if (pRequst == NULL)
	{
		//! failed
		return;
	}

	pRequst->m_dwID = 0;
	pRequst->m_ProcessId = ProcessId;
	pRequst->m_ProcName = *pProcName;
	pRequst->m_ImagePath = *pImagePath;
	pRequst->m_NewFileName = pwchNewFileName;
	pRequst->m_HashC = HashC;

	m_CheckSelfCopyQueue.PushEvent(pRequst);

	SetEvent(m_hNewEvent);
}

void CThCheckSelfCopy::ShowTrojanActivity(ULONG ProcessId, CString *pProcName, CString* pImagePath)
{
	if (m_bThreadExist == false)
		return;

	Request_CheckSelfCopy* pRequst = new Request_CheckSelfCopy;
	if (pRequst == NULL)
	{
		//! failed
		return;
	}

	pRequst->m_dwID = 1;
	pRequst->m_ProcessId = ProcessId;
	pRequst->m_ProcName = *pProcName;
	pRequst->m_ImagePath = *pImagePath;

	m_CheckSelfCopyQueue.PushEvent(pRequst);

	SetEvent(m_hNewEvent);
}
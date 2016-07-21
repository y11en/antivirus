#include "stdafx.h"
#include "stdio.h"

#include "jobs.h"

#include "..\..\hook\hookspec.h"
#include "..\..\hook\funcs.h"

bool GetEventData(HANDLE hDevice, ULONG AppID, BYTE** pData, ULONG *pMark)
{
	DWORD BytesRet;
	BOOL bReq;

	static LONG Mark = 0;

	InterlockedIncrement(&Mark);
	if (Mark == 0)
		InterlockedIncrement(&Mark);

	ULONG EventDataSize = 4096;
	BYTE* pEventData = (BYTE*) HeapAlloc(GetProcessHeap(), 0, EventDataSize);
	if (pEventData != NULL)
	{
		*pMark = Mark;
		bReq = IDriverGetEvent(hDevice, AppID, Mark, pEventData, EventDataSize, &BytesRet);
		if (bReq == FALSE || BytesRet == 0)
		{
			DWORD BytesRet = 0;
			EventDataSize = 0;
			if(DeviceIoControl(hDevice, IOCTLHOOK_GetEventSize, &AppID, sizeof(AppID), &EventDataSize, sizeof(EventDataSize), &BytesRet, NULL))
			{
				HeapFree(GetProcessHeap(), 0, pEventData);
				pEventData = NULL;
				if (EventDataSize != 0)
				{
					pEventData = (BYTE*) HeapAlloc(GetProcessHeap(), 0, EventDataSize);
					if (pEventData != NULL)
					{
						bReq = IDriverGetEvent(hDevice, AppID, Mark, pEventData, EventDataSize, &BytesRet);
						if (bReq == FALSE || BytesRet == 0)
						{
							HeapFree(GetProcessHeap(),0, pEventData);
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
		return TRUE;
	}

	IDriverSkipEvent(hDevice, AppID);
	
	return FALSE;
}

//+ ------------------------------------------------------------------------------------------

Job_Process::Job_Process()
{
	
}

Job_Process::~Job_Process()
{
}

bool Job_Process::Start()
{
	return HookTask::Start(this);
}

bool Job_Process::AddFilters()
{
//	AddFSFilter2(m_hDevice, m_AppID, "", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO | FLT_A_DELETE_ON_MARK, 
//		FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileDispositionInformation, 0, PostProcessing, NULL, NULL);

	AddFSFilter2(m_hDevice, m_AppID, "FAR.EXE", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, 
		FLTTYPE_NFIOR, -1, -1, 0, PreProcessing, NULL, NULL);

	return true;
}
	
void Job_Process::BeforeExit()
{
}

void Job_Process::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	PEVENT_TRANSMIT pEventData = NULL;
	ULONG Mark;
	char msg[0x1000];
	PSINGLE_PARAM pUrl;

	while (true == GetEventData(m_hDevice, m_AppID, (BYTE**)&pEventData, &Mark))
	{
		pUrl = IDriverGetEventParam(pEventData, _PARAM_OBJECT_URL_W);

		wsprintf(msg, "Op: %d %d %d with %S\n", 
			pEventData->m_HookID, pEventData->m_FunctionMj, pEventData->m_FunctionMi, pUrl->ParamValue);
		OutputDebugString(msg);
		printf(msg);

		IDriverSetVerdict(m_hDevice, m_AppID, Verdict_Default, Mark, 0);

		HeapFree(GetProcessHeap(),0, pEventData);
	}
	
}

void Job_Process::FiltersChanged()
{
}
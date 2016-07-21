#include "stdafx.h"
#include "CheckJob.h"

#include "Hook\IDriver.h"

bool GetEventData(DrvEventList* pDrvEventList, HANDLE hDevice, ULONG AppID, BYTE** pData, ULONG *pMark)
{
	DWORD BytesRet;
	BOOL bReq;

	static LONG Mark = 0;

	InterlockedIncrement(&Mark);
	if (Mark == 0)
		InterlockedIncrement(&Mark);

	ULONG EventDataSize = 4096;
	BYTE* pEventData = pDrvEventList->Alloc(EventDataSize);
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
		return TRUE;
	}

	IDriverSkipEvent(hDevice, AppID);
	
	return FALSE;
}


CheckJob_System::CheckJob_System(DrvEventList* pDrvEventList)
{
	m_pDrvEventList = pDrvEventList;
}

CheckJob_System::~CheckJob_System()
{
}

bool CheckJob_System::Start()
{
	return HookTask::Start(this);
}

bool CheckJob_System::AddFilters()
{
	AddFSFilter2(m_hDevice, m_AppID, gpModuleName, 0, FLT_A_POPUP, FLTTYPE_SYSTEM, -1, -1, 0, PreProcessing, NULL, NULL);

	return true;
}
	
void CheckJob_System::SingleEvent()
{
	VERDICT Verdict = Verdict_Discard;
	BYTE* pEventData = NULL;
	ULONG Mark;
	if (false == GetEventData(m_pDrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
		m_pDrvEventList->SetError(_ERR_EVENT);		//event skipped
	else
	{
		if (false == m_pDrvEventList->AddNew(pEventData))
		{
			m_pDrvEventList->Free(pEventData);
			m_pDrvEventList->SetError(_ERR_INTERNAL);		// push failed
		}

		PEVENT_TRANSMIT pEvt = (PEVENT_TRANSMIT) pEventData;
		if (pEvt->m_HookID == FLTTYPE_SYSTEM && pEvt->m_FunctionMj == MJ_SYSTEM_CREATE_SECTION)
			Verdict = Verdict_Default;
		
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}
}

//+ ----------------------------------------------------------------------------------------

CheckJob_File::CheckJob_File(DrvEventList* pDrvEventList)
{
	m_bAllow = true;
	m_pDrvEventList = pDrvEventList;
}

CheckJob_File::~CheckJob_File()
{
}

bool CheckJob_File::Start()
{
	return HookTask::Start(this);
}

bool CheckJob_File::AddFilters()
{
	AddFSFilterW(m_hDevice, m_AppID, L"*" _DIRECT_VOLUME_ACCESS_NAME_W, gpModuleName, 0, FLT_A_DEFAULT, FLTTYPE_NFIOR, -1, -1, 0, PreProcessing, NULL);
	AddFSFilterW(m_hDevice, m_AppID, L"*$*", gpModuleName, 0, FLT_A_DEFAULT, FLTTYPE_NFIOR, -1, -1, 0, PreProcessing, NULL);
	AddFSFilter2(m_hDevice, m_AppID, gpModuleName, 0, FLT_A_POPUP, FLTTYPE_NFIOR, -1, -1, 0, PreProcessing, NULL, NULL);

	return true;
}
	
void CheckJob_File::SingleEvent()
{
	VERDICT Verdict = Verdict_Discard;
	BYTE* pEventData = NULL;
	ULONG Mark;
	if (false == GetEventData(m_pDrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
		m_pDrvEventList->SetError(_ERR_EVENT);		//event skipped
	else
	{
		if (false == m_pDrvEventList->AddNew(pEventData))
		{
			m_pDrvEventList->Free(pEventData);
			m_pDrvEventList->SetError(_ERR_INTERNAL);		// push failed
		}

		if (m_bAllow)
			Verdict = Verdict_Default;
		
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}
}

//+ ----------------------------------------------------------------------------------------

CheckJob_Reg::CheckJob_Reg(DrvEventList* pDrvEventList)
{
	m_bAllow = true;
	m_pDrvEventList = pDrvEventList;
}

CheckJob_Reg::~CheckJob_Reg()
{
}

bool CheckJob_Reg::Start()
{
	return HookTask::Start(this);
}

bool CheckJob_Reg::AddFilters()
{
	AddFSFilter2(m_hDevice, m_AppID, gpModuleName, 0, FLT_A_POPUP, FLTTYPE_REGS, -1, -1, 0, PreProcessing, NULL, NULL);

	return true;
}
	
void CheckJob_Reg::SingleEvent()
{
	VERDICT Verdict = Verdict_Discard;
	BYTE* pEventData = NULL;
	ULONG Mark;
	if (false == GetEventData(m_pDrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
		m_pDrvEventList->SetError(_ERR_EVENT);		//event skipped
	else
	{
		if (false == m_pDrvEventList->AddNew(pEventData))
		{
			m_pDrvEventList->Free(pEventData);
			m_pDrvEventList->SetError(_ERR_INTERNAL);		// push failed
		}

		if (m_bAllow)
			Verdict = Verdict_Default;
		
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}
}

//+ ----------------------------------------------------------------------------------------

CheckJob_Flt::CheckJob_Flt(DrvEventList* pDrvEventList, HANDLE hEvent)
{
	m_bAllow = true;
	m_pDrvEventList = pDrvEventList;
	m_hEvent = hEvent;
}

CheckJob_Flt::~CheckJob_Flt()
{
}

bool CheckJob_Flt::Start()
{
	return HookTask::Start(this);
}

void CheckJob_Flt::SingleEvent()
{
	VERDICT Verdict = Verdict_Discard;
	BYTE* pEventData = NULL;
	ULONG Mark;
	if (false == GetEventData(m_pDrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
		m_pDrvEventList->SetError(_ERR_EVENT);		//event skipped
	else
	{
		if (false == m_pDrvEventList->AddNew(pEventData))
		{
			m_pDrvEventList->Free(pEventData);
			m_pDrvEventList->SetError(_ERR_INTERNAL);		// push failed
		}

		if (m_bAllow)
			Verdict = Verdict_Pass;
		
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}
}

void CheckJob_Flt::FiltersChanged()
{
	Log.AddToLog(L"Change filter's queue detected");
	SetEvent(m_hEvent);
}
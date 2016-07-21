#include "stdafx.h"
#include "stdio.h"

#include "jobs.h"

#include "..\..\hook\hookspec.h"
#include "..\..\hook\funcs.h"

DWORD GetSubstringPosBM(BYTE* Str, DWORD lstrlen, BYTE* SubStr, DWORD SubStrLen)
{
	BYTE Table[0x100];
	int i;
	BYTE *PStr,*PEndStr,*PSubStr,*PEndSubStr;
	BYTE *PStrCur;
	if ((SubStrLen==0) || (lstrlen<SubStrLen)) 
		return -1;
	FillMemory(Table, sizeof(Table), SubStrLen);
	for(i = SubStrLen - 2; i >= 0; i--)
	{
		if (Table[SubStr[i]] == SubStrLen)
			Table[SubStr[i]] = (BYTE) (SubStrLen - i - 1);
	}
	PSubStr = PEndSubStr = SubStr+SubStrLen - 1;
	PStrCur = PStr=Str + SubStrLen - 1;
	PEndStr = Str + lstrlen;
	do {
		if((*PStr) == (*PSubStr))
		{
			if (PSubStr == SubStr) 
				return PStr - Str;
			PStr--;
			PSubStr--;
		} 
		else 
		{
			PSubStr = PEndSubStr;
			PStr = PStrCur += Table[(*PStrCur)];
		}
	} while (PStr<PEndStr);
	return -1;
}

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
	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
	
	BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
	
	BYTE Buf3[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param3 = (PFILTER_PARAM) Buf3;

	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param1->m_ParamFlt = FLT_PARAM_EUA;
	Param1->m_ParamID = _PARAM_OBJECT_DEST_ID;
	*(DWORD*)Param1->m_ParamValue = (DWORD) GetCurrentProcessId();
	Param1->m_ParamSize = sizeof(DWORD);

	Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
	Param3->m_ParamFlt = FLT_PARAM_EUA;
	Param3->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
	*(DWORD*)Param3->m_ParamValue = -1;
	Param3->m_ParamSize = sizeof(DWORD);
	
	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param3, NULL))
		return false;

	return true;
}
	
void Job_Process::BeforeExit()
{
}

void Job_Process::SingleEvent()
{
	VERDICT Verdict = Verdict_Discard;
	PEVENT_TRANSMIT pEventData = NULL;
	ULONG Mark;
	
	if (true == GetEventData(m_hDevice, m_AppID, (BYTE**)&pEventData, &Mark))
	{
		OutputDebugString("terminating\n");

		HeapFree(GetProcessHeap(),0, pEventData);
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}
	
}

void Job_Process::FiltersChanged()
{
}
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
	m_bSidReady = false;
	memset(m_SidBuf, 0, sizeof(m_SidBuf));	
}

Job_Process::~Job_Process()
{
}

bool Job_Process::Start()
{
	return HookTask::Start(this);
}

/*bool Job_Process::AddFilters()
{
	BYTE* buff[0x100];
	PFILTER_PARAM pParam = (PFILTER_PARAM) buff;
	
	BYTE* buffUrl[0x100];
	PFILTER_PARAM pParamUrl = (PFILTER_PARAM) buffUrl;

	BYTE* buffSid[sizeof(PFILTER_PARAM) + __SID_LENGTH];
	PFILTER_PARAM pParamSid = (PFILTER_PARAM) buffSid;

	pParam->m_ParamID = _PARAM_OBJECT_BYTEOFFSET;
	pParam->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pParam->m_ParamFlt = FLT_PARAM_EUA;
	pParam->m_ParamSize = sizeof(__int64);
	*(__int64*)pParam->m_ParamValue = 0;

	pParamUrl->m_ParamID = _PARAM_OBJECT_URL;
	pParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
	lstrcpy((PCHAR) pParamUrl->m_ParamValue, "*");
	pParamUrl->m_ParamSize = lstrlen((PCHAR) pParamUrl->m_ParamValue) + sizeof(CHAR);


	pParamSid->m_ParamID = _PARAM_OBJECT_SID;
	pParamSid->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pParamSid->m_ParamFlt = FLT_PARAM_CHECK_SID;
	pParamSid->m_ParamSize = __SID_LENGTH;
	memcpy(pParamSid->m_ParamValue, m_SidBuf, __SID_LENGTH);

	AddFSFilter2(m_hDevice, m_AppID, "", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, 
		FLTTYPE_DISK, IRP_MJ_WRITE, 0, 0, PreProcessing, NULL, pParamUrl, pParam, NULL);

	if (m_bSidReady)
	{
		OutputDebugString("new filters with sid!\n");
		AddFSFilter2(m_hDevice, m_AppID, "", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, 
			FLTTYPE_DISK, IRP_MJ_READ, 0, 0, PreProcessing, NULL, pParamUrl, pParam, pParamSid, NULL);
	}
	else
	{
		AddFSFilter2(m_hDevice, m_AppID, "", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, 
			FLTTYPE_DISK, IRP_MJ_READ, 0, 0, PreProcessing, NULL, pParamUrl, pParam, NULL);
	}
	
	return true;
}*/

bool Job_Process::AddFilters()
{
	BYTE* buffUrl[0x100];
	PFILTER_PARAM pParamUrl = (PFILTER_PARAM) buffUrl;

	BYTE* buffSid[sizeof(PFILTER_PARAM) + __SID_LENGTH];
	PFILTER_PARAM pParamSid = (PFILTER_PARAM) buffSid;

	pParamUrl->m_ParamID = _PARAM_OBJECT_URL;
	pParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
	lstrcpy((PCHAR) pParamUrl->m_ParamValue, "*");
	pParamUrl->m_ParamSize = lstrlen((PCHAR) pParamUrl->m_ParamValue) + sizeof(CHAR);

	pParamSid->m_ParamID = _PARAM_OBJECT_SID;
	pParamSid->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pParamSid->m_ParamFlt = FLT_PARAM_CHECK_SID;
	pParamSid->m_ParamSize = __SID_LENGTH;
	memcpy(pParamSid->m_ParamValue, m_SidBuf, __SID_LENGTH);

	if (m_bSidReady)
	{
		OutputDebugString("new filters with sid!\n");
		AddFSFilter2(m_hDevice, m_AppID, "", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, 
			FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL, pParamUrl, pParamSid, NULL);

	}
	else
	{
		AddFSFilter2(m_hDevice, m_AppID, "", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, 
			FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL, pParamUrl, NULL);
	}
	
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
	OutputDebugString("single event!\n");
	if (true == GetEventData(m_hDevice, m_AppID, (BYTE**)&pEventData, &Mark))
	{
		if (!m_bSidReady)
		{
			PSINGLE_PARAM pParamSid = IDriverGetEventParam(pEventData, _PARAM_OBJECT_SID);
			if (pParamSid && (__SID_LENGTH == pParamSid->ParamSize))
			{
				ResetFilters();
				m_bSidReady = true;
				memcpy(m_SidBuf, pParamSid->ParamValue, __SID_LENGTH);

				AddFilters();
			}
		}

		HeapFree(GetProcessHeap(),0, pEventData);
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}

}

/*void Job_Process::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	PEVENT_TRANSMIT pEventData = NULL;
	ULONG Mark;
	char msg[0x100];
	PSINGLE_PARAM pUrl;
	PSINGLE_PARAM pOffset;
	OutputDebugString("single event!\n");
	if (true == GetEventData(m_hDevice, m_AppID, (BYTE**)&pEventData, &Mark))
	{
		pUrl = IDriverGetEventParam(pEventData, _PARAM_OBJECT_URL);
		pOffset  = IDriverGetEventParam(pEventData, _PARAM_OBJECT_BYTEOFFSET);

		//if (GetSubstringPosBM(pUrl->ParamValue, pUrl->ParamSize, (BYTE*)"DR0", sizeof("DR0") - sizeof(CHAR)) != -1)
		{
			if (pEventData->m_FunctionMj == IRP_MJ_WRITE)
				wsprintf(msg, "write %s %d:%d\n", pUrl->ParamValue, 
				((LARGE_INTEGER*)pOffset->ParamValue)->HighPart,
				((LARGE_INTEGER*)pOffset->ParamValue)->LowPart);
			else
				wsprintf(msg, "read %s %d:%d\n", pUrl->ParamValue,
				((LARGE_INTEGER*)pOffset->ParamValue)->HighPart,
				((LARGE_INTEGER*)pOffset->ParamValue)->LowPart);
			
			OutputDebugString(msg);
			printf(msg);
		}

		HeapFree(GetProcessHeap(),0, pEventData);
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}

	if (!m_bSidReady)
	{
		PSINGLE_PARAM pParamSid = IDriverGetEventParam(pEventData, _PARAM_OBJECT_SID);
		if (pParamSid && (__SID_LENGTH == pParamSid->ParamSize))
		{
			ResetFilters();
			m_bSidReady = true;
			memcpy(m_SidBuf, pParamSid->ParamValue, __SID_LENGTH);

			AddFilters();
		}
	}
}*/

void Job_Process::FiltersChanged()
{
}

void Job_Process::ResetFilters()
{
	IDriverResetFilters(m_hDevice, m_AppID);
}
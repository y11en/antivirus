#include <windows.h>
#include "job_thread.h"

#ifdef _DEBUG
	#ifdef _NO_BLOCK
		#define __flt_flag	FLT_A_INFO
		#pragma message("----------------------- no block on action")
	#else
		#define __flt_flag	FLT_A_POPUP
	#endif // _NO_BLOCK
#else
	#define __flt_flag	FLT_A_POPUP
#endif // _DEBUG

CJob::~CJob()
{
}

bool CJob::Start()
{
	bool bRet = false;

	bRet = HookTask::Start(this);

	return bRet;
}



bool CJob::AddFilters()
{
	bool bRet = false;

	//////////////////////////////////////////////////////////////////////////
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

	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_MAP_TO_EVENT;
	Param2->m_ParamFlt = FLT_PARAM_NOP;
	Param2->m_ParamID = _PARAM_USER_FSDRVLIB_PROPROT;
	*(DWORD*)Param2->m_ParamValue = PROPROT_FLAG_TERMINATE;
	Param2->m_ParamSize = sizeof(DWORD);

	BYTE Buf4[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param4 = (PFILTER_PARAM) Buf4;
	
	ULONG ServicesExePid;
	ULONG ProcessId = IDriver_GetAllowedPids(&ServicesExePid);
	if (ProcessId)
	{
		bRet = true;
		IDriver_AddAllowedPidFilter(m_AppID, m_hDevice, ProcessId);
		if ((m_OsInfo.dwMajorVersion == 4) && (m_OsInfo.dwMinorVersion == 0)) // nt4 only
		{
			if (ServicesExePid)
				IDriver_AddAllowedPidFilter(m_AppID, m_hDevice, ServicesExePid);
		}
		
		Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
		Param3->m_ParamFlt = FLT_PARAM_AND;
		Param3->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
		*(DWORD*)Param3->m_ParamValue = _CONTEXT_OBJECT_FLAG_OPERATION_ON_SELF;
		Param3->m_ParamSize = sizeof(DWORD);
		
		Param4->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		Param4->m_ParamFlt = FLT_PARAM_AND;
		Param4->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
		*(DWORD*)Param4->m_ParamValue = PROCESS_TERMINATE;
		Param4->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY | FLT_A_INFO | FLT_A_DELETE_ON_MARK, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param2, Param3, Param4, NULL);

		Param4->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		Param4->m_ParamFlt = FLT_PARAM_AND;
		Param4->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
		*(DWORD*)Param4->m_ParamValue = PROCESS_VM_WRITE | PROCESS_DUP_HANDLE | PROCESS_CREATE_PROCESS | PROCESS_SET_INFORMATION | PROCESS_QUERY_INFORMATION;
		Param4->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param2, Param3, Param4, NULL);

		// query info
		Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		Param3->m_ParamFlt = FLT_PARAM_EUA;
		Param3->m_ParamID = _PARAM_OBJECT_CLIENTID1;
		*(DWORD*)Param3->m_ParamValue = (DWORD) GetCurrentProcessId();
		Param3->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_QUERY_INFO, 0, 0, PreProcessing, NULL, Param3, NULL);


		// dup hadle
		Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		Param3->m_ParamFlt = FLT_PARAM_EUA;
		Param3->m_ParamID = _PARAM_OBJECT_CLIENTID1;
		*(DWORD*)Param3->m_ParamValue = (DWORD) GetCurrentProcessId();
		Param3->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_DUPLICATE_OBJECT, 0, 0, PreProcessing, NULL, Param3, NULL);
	}

	//////////////////////////////////////////////////////////////////////////
	
	return bRet;
}

void CJob::BeforeExit()
{
}

void CJob::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	BYTE* pEventData = NULL;
	ULONG Mark;

	if (GetEventData(&m_DrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
	{
		PEVENT_TRANSMIT pEvt;

		DRV_EVENT_DESCRIBE DrvEvDescr;

		DrvEvDescr.m_hDevice = m_hDevice;
		DrvEvDescr.m_AppID = m_AppID;
		DrvEvDescr.m_Mark = Mark;
		DrvEvDescr.m_EventFlags = (ULONG) pEventData;

		pEvt = (PEVENT_TRANSMIT) pEventData;

		ProcessEvent(pEvt, &DrvEvDescr, &Verdict);

		if (!(pEvt->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
			IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);

		m_DrvEventList.Free(pEventData);
	}
}

//+ ------------------------------------------------------------------------------------------
void CJob::ProcessEvent(PEVENT_TRANSMIT pEvt, PDRV_EVENT_DESCRIBE pDrvEvDescr, PVERDICT pVerdict)
{
	WCHAR msg[MAX_PATH];
	wsprintf(msg, L"se: %S Mj: 0x%x\n", pEvt->m_ProcName, pEvt->m_FunctionMj);
	OutputDebugString(msg);

	if (MJ_SYSTEM_DUPLICATE_OBJECT == pEvt->m_FunctionMj)
	{
		PSINGLE_PARAM pParamSrc = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SOURCE_ID);
		PSINGLE_PARAM pParamDst = IDriverGetEventParam(pEvt, _PARAM_OBJECT_DEST_ID);

		if (pParamSrc && pParamDst)
		{
			ULONG srcPid = *(ULONG*)pParamSrc->ParamValue;
			ULONG dstPid = *(ULONG*)pParamDst->ParamValue;

			if (srcPid != dstPid)
			{
				*pVerdict = Verdict_Discard;

				wsprintf(msg, L"dup: %d -> %d. will deny\n", srcPid, dstPid);
				OutputDebugString(msg);
			}
		}
	}
}

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
	
	ADDITIONALSTATE AddState;
	if (IDriverGetAdditionalState(m_hDevice, &AddState))
	{
		if (AddState.DrvFlags & _DRV_FLAG_MINIFILTER)	// temporary not supported
			return false;
	}

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
	ULONG CsrssPid;
	ULONG ProcessId = IDriver_GetAllowedPids(&ServicesExePid, &CsrssPid);
	if (ProcessId)
	{
		bRet = true;
		IDriver_AddAllowedPidFilter(m_AppID, m_hDevice, ProcessId);
		if ((m_OsInfo.dwMajorVersion == 4) && (m_OsInfo.dwMinorVersion == 0))
		{
			// nt4 allow open process
		}
		else
		{
			if ((m_OsInfo.dwMajorVersion == 5) && (m_OsInfo.dwMinorVersion == 0)) // 2k
			{
				if (ServicesExePid)
					IDriver_AddAllowedPidFilter(m_AppID, m_hDevice, ServicesExePid);

				if (CsrssPid)
					IDriver_AddAllowedPidFilter(m_AppID, m_hDevice, CsrssPid);
			}
			
			Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
			Param3->m_ParamFlt = FLT_PARAM_AND;
			Param3->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
			*(DWORD*)Param3->m_ParamValue = _CONTEXT_OBJECT_FLAG_OPERATION_ON_SELF;
			Param3->m_ParamSize = sizeof(DWORD);
			
			Param4->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
			Param4->m_ParamFlt = FLT_PARAM_AND;
			Param4->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
			*(DWORD*)Param4->m_ParamValue = PROCESS_TERMINATE | PROCESS_SUSPEND_RESUME;
			Param4->m_ParamSize = sizeof(DWORD);
		
			AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY | FLT_A_INFO | FLT_A_DELETE_ON_MARK, FLTTYPE_SYSTEM, 
				MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param2, Param3, Param4, NULL);

			Param4->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
			Param4->m_ParamFlt = FLT_PARAM_AND;
			Param4->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
			*(DWORD*)Param4->m_ParamValue = PROCESS_VM_WRITE /*| PROCESS_DUP_HANDLE*/ | PROCESS_CREATE_PROCESS | PROCESS_SET_INFORMATION/* | PROCESS_QUERY_INFORMATION*/;
			Param4->m_ParamSize = sizeof(DWORD);
			
			AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, 
				MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param2, Param3, Param4, NULL);
		}

		// query info
		Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		Param3->m_ParamFlt = FLT_PARAM_EUA;
		Param3->m_ParamID = _PARAM_OBJECT_CLIENTID1;
		*(DWORD*)Param3->m_ParamValue = (DWORD) GetCurrentProcessId();
		Param3->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_QUERY_INFO, 0, 0, PreProcessing, NULL, Param3, NULL);

		// simple terminate info
		Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		Param3->m_ParamFlt = FLT_PARAM_EUA;
		Param3->m_ParamID = _PARAM_OBJECT_DEST_ID;
		*(DWORD*)Param3->m_ParamValue = (DWORD) GetCurrentProcessId();
		Param3->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY | FLT_A_INFO | FLT_A_DELETE_ON_MARK, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PreProcessing, NULL, Param3, NULL);


		// dup hadle
		Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		Param3->m_ParamFlt = FLT_PARAM_EUA;
		Param3->m_ParamID = _PARAM_OBJECT_CLIENTID1;
		*(DWORD*)Param3->m_ParamValue = (DWORD) GetCurrentProcessId();
		Param3->m_ParamSize = sizeof(DWORD);
		
		AddFSFilter2(m_hDevice, m_AppID, "*", 60 * 60, FLT_A_POPUP, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_DUPLICATE_OBJECT, 0, 0, PreProcessing, NULL, Param3, NULL);
	}

// interproc messages to invisible threads
	Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param3->m_ParamFlt = FLT_PARAM_EUA;
	Param3->m_ParamID = _PARAM_OBJECT_INVIS_TARGET;
	*(DWORD*)Param3->m_ParamValue = 1UL;
	Param3->m_ParamSize = sizeof(DWORD);
	AddFSFilter2(m_hDevice, m_AppID, "*", 60 * 60, FLT_A_DENY | FLT_A_POPUP, FLTTYPE_SYSTEM, 
		MJ_SYSTEM_INTERPROC_MESSAGE, 0, 0, PreProcessing, NULL, Param3, NULL);

// send input to invisible threads
	AddFSFilter2(m_hDevice, m_AppID, "*", 60 * 60, FLT_A_DENY | FLT_A_POPUP, FLTTYPE_SYSTEM, 
		MJ_SYSTEM_SENDINPUT, 0, 0, PreProcessing, NULL, NULL);

// FindWindow(Ex) does not find our windows
	Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param3->m_ParamFlt = FLT_PARAM_EUA;
	Param3->m_ParamID = _PARAM_OBJECT_INVIS_TARGET;
	*(DWORD*)Param3->m_ParamValue = 1UL;
	Param3->m_ParamSize = sizeof(DWORD);
	AddFSFilter2(m_hDevice, m_AppID, "*", 60 * 60, FLT_A_DENY | FLT_A_INFO, FLTTYPE_SYSTEM, 
		MJ_SYSTEM_FINDWINDOW, 0, 0, PostProcessing, NULL, Param3, NULL);

	//////////////////////////////////////////////////////////////////////////
	
	return bRet;
}

void CJob::BeforeExit()
{
}

void CJob::SingleEvent()
{
	VERDICT Verdict = Verdict_Discard;
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
BOOL CJob::ProcessNextInput(PINPUT pInput)
{
	BOOL bBlock = FALSE;

	if (pInput->type == INPUT_MOUSE)
	{
		PMOUSEINPUT pMi = &pInput->mi;
		POINT   Point = {0, 0};

		if (pMi->dwFlags & MOUSEEVENTF_MOVE)
		{
			DWORD	ScreenWidth, ScreenHeight;
			LONG	MoveX = 0, MoveY = 0;
			// virtual desktop used
			if (pMi->dwFlags & MOUSEEVENTF_VIRTUALDESK)
			{
				ScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
				ScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
			}
			else
			{
				ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
				ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
			}

			MoveX = (pMi->dx*ScreenWidth)/0xFFFFL;
			MoveY = (pMi->dy*ScreenHeight)/0xFFFFL;

			if (pMi->dwFlags & MOUSEEVENTF_ABSOLUTE)
			{
				Point.x = MoveX;
				Point.y = MoveY;
			}
			else
			{
				GetCursorPos(&Point);
				Point.x += MoveX;
				Point.y += MoveY;
			}
		}
		else
			GetCursorPos(&Point);

		HWND hWin = WindowFromPoint(Point);
		if (hWin)
		{
			DWORD ThreadID = GetWindowThreadProcessId(hWin, NULL);
			if (ThreadID)
			{
				DWORD bIsInvisibleTarget = IDriverIsInvisibleThread(m_hDevice, m_AppID, ThreadID);

				if (bIsInvisibleTarget)
					bBlock = TRUE;
			}
		}

	}

	return bBlock;
}

VERDICT CJob::ProcessSendInputs(PINPUT pInputs, DWORD nInputs)
{
	VERDICT Verdict = Verdict_NotFiltered;

	for (DWORD i = 0; i < nInputs; i++)
	{
		if (ProcessNextInput(&pInputs[i]))
		{
			Verdict = Verdict_Discard;
			break;
		}
	}

	return Verdict;
}

void CJob::ProcessEvent(PEVENT_TRANSMIT pEvt, PDRV_EVENT_DESCRIBE pDrvEvDescr, PVERDICT pVerdict)
{
	ULONG MsgId=0;
	ULONG pid = pEvt->m_ProcessID;

	*pVerdict = Verdict_NotFiltered;

	switch (pEvt->m_FunctionMj)
	{
	case MJ_SYSTEM_DUPLICATE_OBJECT:
		*pVerdict = Verdict_Default;
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
					pid = dstPid;
				}

			}

			break;
		}
	case MJ_SYSTEM_SENDINPUT:
		{
			MsgId = -1;

			PSINGLE_PARAM pParamInvisTarget = IDriverGetEventParam(pEvt, _PARAM_OBJECT_INVIS_TARGET);
			if (pParamInvisTarget)
			{
				DWORD bInvisTarget = *(DWORD *)pParamInvisTarget->ParamValue;
				if (bInvisTarget)
					*pVerdict = Verdict_Discard;
			}

			if (_PASS_VERDICT(*pVerdict))
			{
				PSINGLE_PARAM pParamInputs = IDriverGetEventParam(pEvt, _PARAM_OBJECT_BINARYDATA);
				PSINGLE_PARAM pParamNumInputs = IDriverGetEventParam(pEvt, _PARAM_OBJECT_PARAMCOUNT);
				if (pParamInputs && pParamNumInputs)
				{
					PINPUT pInputs = (PINPUT)pParamInputs->ParamValue;
					DWORD nInputs = *(DWORD *)pParamNumInputs->ParamValue;

					*pVerdict = ProcessSendInputs(pInputs, nInputs);
				}
			}

			break;
		}
	case MJ_SYSTEM_INTERPROC_MESSAGE:
		*pVerdict = Verdict_Default;
		{
			PSINGLE_PARAM pParamMsgId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_MSG_ID);

			if (pParamMsgId)
			{
				MsgId = *(PULONG)pParamMsgId->ParamValue;

				if (MsgId >= WM_USER)
					break;

/*				switch (MsgId)
				{
				case WM_QUERYENDSESSION:
				case WM_ENDSESSION:
				case WM_GETICON:
				case 0x0319:				// WM_APPCOMMAND
				case 0x031A:				// WM_THEMECHANGED
				case 0x003b:				// WM_CLIENTSHUTDOWN
					break;
				default:
					*pVerdict = Verdict_Discard;
				}
*/

				if (MsgId == WM_COMMAND
					|| (MsgId >= WM_KEYFIRST && MsgId <= WM_KEYLAST) 
					|| (MsgId >= WM_MOUSEFIRST && MsgId <= WM_MOUSELAST) 
					|| (MsgId == WM_CLOSE)
					|| (MsgId == WM_SYSCOMMAND)
					|| (MsgId == BM_CLICK)
					|| (MsgId == WM_SHOWWINDOW)
					|| (MsgId == WM_NCDESTROY)
					)
				{
					*pVerdict = Verdict_Discard;
				}
			}
		}

		break;
	case MJ_SYSTEM_TERMINATE_PROCESS:
	case MJ_SYSTEM_OPEN_PROCESS:
		*pVerdict = Verdict_Discard;
		break;
	}

	if (*pVerdict == Verdict_Discard)
	{
		BOOL bTrusted = m_pfnCallback(MsgId, pid, pEvt);
		if ( bTrusted )
			*pVerdict = Verdict_Pass;
	}
}

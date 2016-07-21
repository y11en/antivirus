#include "job_thread.h"
#include <tchar.h>

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
	bool bRet = HookTask::Start(this);

	DbPrint((DCB_SYSTEM, DL_IMPORTANT, _T("Activity Detector: task %s"), 
		bRet == true ? _T("started") : _T("start failed")));

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	return bRet;
}

bool CJob::AddProcFilters()
{
	bool bRet = !!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, __flt_flag, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_CREATE_PROCESS, 0, 0, PostProcessing, NULL, NULL);
	bRet &= !!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, __flt_flag, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_CREATE_PROCESS_EX, 0, 0, PostProcessing, NULL, NULL);
	bRet &= !!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, __flt_flag, FLTTYPE_SYSTEM, 
			MJ_EXIT_PROCESS, 0, 0, PostProcessing, NULL, NULL);
	bRet &= !!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, __flt_flag, FLTTYPE_SYSTEM, 
			MJ_CREATE_PROCESS_NOTIFY_EXTEND, 0, 0, PreProcessing, NULL, NULL);
	bRet &= !!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, __flt_flag, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PostProcessing, NULL, NULL);
	bRet &= !!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, __flt_flag, FLTTYPE_SYSTEM,
			MJ_SYSTEM_CREATE_THREAD, 0, 0, PostProcessing, NULL, NULL);
	
	return bRet;
}

bool CJob::AddRegFilters()
{
	bool bRet = !!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, __flt_flag, FLTTYPE_REGS, 
			Interceptor_SetValueKey, 0, 0, PostProcessing, NULL, NULL);
	
	return bRet;
}

//+ ------------------------------------------------------------------------------------------
bool CJob::AddFileFilters()
{
	/*bool bRet = true;

#define _env_length 0x200
	BYTE Buf1[sizeof(FILTER_PARAM) + _env_length];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;

	//+ ------------------------------------------------------------------------------------------
	//+ cleanup for modified files
	
	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
	Param1->m_ParamFlt = FLT_PARAM_WILDCARD;
	Param1->m_ParamID = _PARAM_OBJECT_URL_W;
	lstrcpy((PWCHAR) Param1->m_ParamValue, _T("*\\"));
	Param1->m_ParamSize = _WSTR_LEN_B((PWCHAR)Param1->m_ParamValue);

	BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;

	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param2->m_ParamFlt = FLT_PARAM_AND;
	Param2->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
	Param2->m_ParamSize = sizeof(DWORD);
	*(DWORD*)Param2->m_ParamValue = _CONTEXT_OBJECT_FLAG_MODIFIED;

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_NFIOR, 
			IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL, Param2, Param1, NULL))
	{
		ResetFilters();
		return false;
	}*/

	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;

	//+ ------------------------------------------------------------------------------------------
	//+ cleanup for modified files
	
	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param1->m_ParamFlt = FLT_PARAM_AND;
	Param1->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
	*(ULONG*)Param1->m_ParamValue = _CONTEXT_OBJECT_FLAG_FORWRITE | _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT;
	Param1->m_ParamSize = sizeof(ULONG);

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_PASS | FLT_A_POPUP, FLTTYPE_NFIOR, 
			IRP_MJ_CREATE, 0, 0, PreProcessing, NULL, Param1, NULL))
	{
		ResetFilters();
		return false;
	}

	// rename
	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_PASS | FLT_A_POPUP, FLTTYPE_NFIOR, 
			IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing, NULL, NULL))
	{
		ResetFilters();
		return false;
	}
	// delete

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_PASS | FLT_A_POPUP, FLTTYPE_NFIOR, 
			IRP_MJ_SET_INFORMATION, FileDispositionInformation, 0, PreProcessing, NULL, NULL))
	{
		ResetFilters();
		return false;
	}

	//- end setting job filters
	//- ------------------------------------------------------------------------------------------

	return true;
}

bool CJob::AddFilters()
{
	bool bRet = true;

//	bRet = AddProcFilters();
//	bRet &= AddRegFilters();
	bRet &= AddFileFilters();

	DbPrint((DCB_SYSTEM, DL_IMPORTANT, _T("Activity Detector: filters %s"),
		bRet == true ? _T("added") : _T("add failed")));

	return bRet;
}

void CJob::BeforeExit()
{
	DbPrint((DCB_SYSTEM, DL_IMPORTANT, _T("Activity Detector: before done...")));
}

void CJob::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	BYTE* pEventData = NULL;
	ULONG Mark;

	if (GetEventData(&m_DrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
	{
		PEVENT_TRANSMIT pEvt;

		DbPrint((DCB_DRIVER, DL_SPAM, _T("Activity Detector: single event")));

		pEvt = (PEVENT_TRANSMIT) pEventData;
		switch (pEvt->m_HookID)
		{
		case FLTTYPE_SYSTEM:
			ProcessEvent(pEvt);
			break;
		case FLTTYPE_REGS:
			RegistryEvent(pEvt);
			break;
		case FLTTYPE_NFIOR:
			FileEvent(pEvt);
			break;
		default:
			break;
		}

		if (!(pEvt->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
			IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);

		m_DrvEventList.Free(pEventData);
	}
}


//+ ------------------------------------------------------------------------------------------
void CJob::ProcessEvent(PEVENT_TRANSMIT pEvt)
{
	PSINGLE_PARAM pParamProcId;
	PSINGLE_PARAM pParamParentProcId;
	PSINGLE_PARAM pParamImagePath;
	PSINGLE_PARAM pParamProcHandle;
	PSINGLE_PARAM pParamSid;

	switch (pEvt->m_FunctionMj)
	{
	case MJ_SYSTEM_CREATE_PROCESS:
	case MJ_SYSTEM_CREATE_PROCESS_EX:
		DbPrint((DCB_DRIVER, DL_SPAM, _T("Event: Create process")));
		{
			pParamParentProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SOURCE_ID);
			pParamImagePath = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);
			pParamProcHandle = IDriverGetEventParam(pEvt, _PARAM_OBJECT_OBJECTHANDLE);
			
			if (pParamImagePath->ParamSize == 0 || pParamProcHandle == 0)
				break;
			
			pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_DEST_ID);
			pParamSid = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SID);
			
			ULONG ProcID = pParamProcId == NULL ? (ULONG) -1 : *(ULONG*)pParamProcId->ParamValue;
			
			m_ProcessList.AddNewProcess(*(ULONG*)pParamParentProcId->ParamValue, pEvt->m_ProcName, 
				ProcID, *(ULONG*)pParamProcHandle->ParamValue, (PWCHAR) pParamImagePath->ParamValue, (PSID) pParamSid->ParamValue);
			CSingleProcess* pParent = m_ProcessList.FindProcess(*(ULONG*)pParamParentProcId->ParamValue);
			if (pParent)
			{
				if (0 != _tcsicmp(pParent->m_pwchImagePath, (PWCHAR) pParamImagePath->ParamValue))
					m_ActivityCheck.CheckSelfCopy(pParent, (PWCHAR) pParamImagePath->ParamValue);
				pParent->Release();
			}
		}
		break;
	case MJ_CREATE_PROCESS_NOTIFY_EXTEND:
		DbPrint((DCB_DRIVER, DL_SPAM, _T("Event: Extend process notify")));
		{
			PSINGLE_PARAM pContext = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CONTEXT_FLAGS);
			PSINGLE_PARAM pComandLine = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_PARAM_W);
			if (pContext != NULL && pComandLine != NULL)
			{
				CSingleProcess *pProcess = m_ProcessList.FindProcess(pEvt->m_ProcessID);
				if (pProcess != NULL)
				{
					pProcess->m_pwchCmdLine = GetCopyStr((PWCHAR) pComandLine->ParamValue);
					CSingleProcess *pParent = pProcess->GetParent();
					if (pParent != NULL)
					{
						pParent->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Process running command: %s"), 
							(PWCHAR) pComandLine->ParamValue);
						pParent->Release();
					}
				}
				
				if (*(ULONG*)pContext->ParamValue & (CREATE_PROCESS_NOTIFY_FLAG_STDIN_REMOTE | CREATE_PROCESS_NOTIFY_FLAG_STDOUT_REMOTE))
				{
					if (pProcess == NULL)
					{
						DbPrint((DCB_SYSTEM, DL_WARNING, _T("BL: Process with redirected in/out - process not found")));
					}
					else
					{
						m_ActivityCheck.RedirectedOutput(pProcess, (PWCHAR) pComandLine->ParamValue);
					}
				}
				if (pProcess != NULL)
					pProcess->Release();
			}
		}
		break;
	case MJ_SYSTEM_CREATE_THREAD:
		DbPrint((DCB_DRIVER, DL_SPAM, _T("Event: Create thread")));
		{
			PSINGLE_PARAM pParamProcHandle = IDriverGetEventParam(pEvt, _PARAM_OBJECT_OBJECTHANDLE);
			PSINGLE_PARAM pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CLIENTID1);
			if (pParamProcHandle != NULL && pParamProcId != NULL)
			{
				if (*(ULONG*)pParamProcHandle->ParamValue != (ULONG)-1)
					m_ProcessList.UpdateProcessIdByHandle(*(ULONG*)pParamProcHandle->ParamValue, *(ULONG*)pParamProcId->ParamValue);
			}
		}
		break;

	case MJ_EXIT_PROCESS:
		DbPrint((DCB_DRIVER, DL_SPAM, _T("Event: exit process")));
		{
			pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SOURCE_ID);
			ULONG ProcID = pParamProcId == NULL ? (ULONG) -1 : *(ULONG*)pParamProcId->ParamValue;
			CSingleProcess *pProcess = m_ProcessList.FindProcess(ProcID);
			if (pProcess != NULL)
			{
				pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Process pid=%d exited"), ProcID);
				pProcess->Release();
			}
			else
			{
				m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Process pid=%d exited"), ProcID);
			}
			m_ProcessList.DeleteProcess(ProcID);
		}
		break;
	case MJ_SYSTEM_TERMINATE_PROCESS:
		{
			pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_DEST_ID);
			ULONG ProcID = pParamProcId == NULL ? (ULONG) -1 : *(ULONG*)pParamProcId->ParamValue;
			CSingleProcess *pProcess = m_ProcessList.FindProcess(ProcID);
			if (pProcess != NULL)
			{
				pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Process pid=%d terminated"), ProcID);
				pProcess->Release();
			}
			else
			{
				m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Process pid=%d terminated"), ProcID);
			}
			m_ProcessList.DeleteProcess(ProcID);
		}
		break;
	}
}

void CJob::RegistryEvent(PEVENT_TRANSMIT pEvt)
{
	PSINGLE_PARAM pParamKeyName;
	PSINGLE_PARAM pParamValueName;

	switch (pEvt->m_FunctionMj)
	{
	case Interceptor_SetValueKey:
		{
			PWCHAR pwchValueName = _T(""); // default value name
			PWCHAR pwchKeyName = NULL;
			if (!(pEvt->m_EventFlags & _EVENT_FLAG_POSTPROCESSING))
				DbPrint((DCB_DRIVER, DL_WARNING, _T("RegistryEvent: pre proc?")));

			pParamKeyName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);
			pParamValueName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_PARAM_W);
			if (!pParamKeyName)
			{
				DbPrint((DCB_DRIVER, DL_WARNING, _T("RegistryEvent: empty url")));
				break;
			}
			if (pParamValueName && pParamValueName->ParamSize)
				pwchValueName = (PWCHAR) pParamValueName->ParamValue;
			pwchKeyName = (PWCHAR) pParamKeyName->ParamValue;

			DbPrint((DCB_DRIVER, DL_SPAM, _T("RegistryEvent: check name %s\\%s"), 
				pwchKeyName, pwchValueName));

			if (m_ActivityCheck.IsP2PKey(pwchKeyName, pwchValueName))
			{
				CSingleProcess *pProcess = m_ProcessList.FindProcess(pEvt->m_ProcessID);
				if (NULL == pProcess)
				{
					DbPrint((DCB_BL, DL_WARNING, _T("BL: P2P key modify '%s\\%s' but process  pid=%d not found"), 
						pwchKeyName, pwchValueName, pEvt->m_ProcessID));
				}
				else
				{
					m_ActivityCheck.CheckP2PKey(pProcess, pwchKeyName, pwchValueName);
					pProcess->Release();
				}
			}

			if (m_RegCheck.IsAutoRunsKey(pwchKeyName, pwchValueName))
			{
				CSingleProcess *pProcess = m_ProcessList.FindProcess(pEvt->m_ProcessID);
				if (NULL == pProcess)
				{
					DbPrint((DCB_BL, DL_WARNING, _T("BL: autorun key modify '%s\\%s' but process  pid=%d not found"), 
						pwchKeyName, pwchValueName, pEvt->m_ProcessID));
				}
				else
				{
					m_ActivityCheck.CheckAutorunKey(pProcess, pwchKeyName, pwchValueName);
					pProcess->Release();
				}
			}
		}
		break;
	default:
		DbPrint((DCB_DRIVER, DL_SPAM, _T("Event: default!")));
		break;
	}
}

void CJob::FileEvent(PEVENT_TRANSMIT pEvt)
{
	PSINGLE_PARAM pParamUrl;
	pParamUrl = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);

	/*CSingleProcess *pProcess = m_ProcessList.FindProcess(pEvt->m_ProcessID);
	if (pProcess == NULL)
	{
		DbPrint((DCB_DRIVER, DL_NOTIFY, _T("BL: Process (pid=%d) not found. Was running before us? (copy to '%s')"), 
			pEvt->m_ProcessID,
			(PWCHAR) pParamUrl->ParamValue));
	}
	else
	{
		PWCHAR pwchFilename = (PWCHAR) pParamUrl->ParamValue;
		PWCHAR pwchFilePart = wcsrchr(pwchFilename, '\\');
		if (!pwchFilePart)
			pwchFilePart = pwchFilename;
		else
			pwchFilePart++;
		if (0 == lstrcmpi(pwchFilePart, _T("WIN.INI")))
		{
			PWCHAR pwchFilenameUpper = GetUpperStr( pwchFilename );
			if (pwchFilenameUpper)
			{
				WCHAR wcsString[MAX_PATH];
				if (GetPrivateProfileString(_T("windows"), _T("run"), _T(""), wcsString, countof(wcsString), pwchFilename))
					m_ActivityCheck.CheckAutorunKey(pProcess, pwchFilenameUpper, _T("run"), wcsString);
				if (GetPrivateProfileString(_T("windows"), _T("load"), _T(""), wcsString, countof(wcsString), pwchFilename))
					m_ActivityCheck.CheckAutorunKey(pProcess, pwchFilenameUpper, _T("load"), wcsString);
				FreeStr(pwchFilenameUpper);
			}
		}
		m_ActivityCheck.CheckSelfCopy(pProcess, pwchFilename);

		pProcess->Release();
	}*/

	WCHAR wcsString[MAX_PATH * 4];
	if (pEvt->m_FunctionMj == IRP_MJ_CREATE)
	{
		PSINGLE_PARAM pSingleContextFlags = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CONTEXT_FLAGS);
		if (pSingleContextFlags != NULL)
		{
			ULONG Flags = *(ULONG*) pSingleContextFlags->ParamValue;

			if (Flags & _CONTEXT_OBJECT_FLAG_DIRECTORY)
			{
				// create directory?
				wsprintf(wcsString, L"crd: %s\n", (PWCHAR) pParamUrl->ParamValue);
				OutputDebugString(wcsString);
			}
			else if (Flags & (_CONTEXT_OBJECT_FLAG_FORWRITE | _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT))
			{
				// will new or write existing?
				wsprintf(wcsString, L"ofw: %s\n", (PWCHAR) pParamUrl->ParamValue);
				OutputDebugString(wcsString);
			}
		}
	}
	if ((pEvt->m_FunctionMj == IRP_MJ_SET_INFORMATION) && (pEvt->m_FunctionMi == FileRenameInformation))
	{
		PSINGLE_PARAM pParamUrlDest = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_DEST_W);
		if (pParamUrlDest != NULL)
		{
			wsprintf(wcsString, L"ren: %s -> %s\n", (PWCHAR) pParamUrl->ParamValue, (PWCHAR) pParamUrlDest->ParamValue);
			OutputDebugString(wcsString);
		}
	}

	if ((pEvt->m_FunctionMj == IRP_MJ_SET_INFORMATION) && (pEvt->m_FunctionMi == FileDispositionInformation))
	{
		wsprintf(wcsString, L"del: %s\n", (PWCHAR) pParamUrl->ParamValue);
		OutputDebugString(wcsString);
	}
}
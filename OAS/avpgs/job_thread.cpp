#define PR_IMPEX_DEF

#include <Prague/pr_cpp.h>
#include <ProductCore/common/userban.h>

#include "job_thread.h"
#include "avpgimpl.h"

#include <Productcore/structs/procmonm.h>

#define _user_ban_new		0x00
#define _user_ban_send		0x01
#define _user_ban_ext		0x02


extern "C" DWORD g_dwTlsIndexEventDefinition;
extern LONG	g_HeapCounter;

tERROR pr_call OnAcc_ThreadCallback(tThreadCallType CallType, tPTR pCommonThreadContext, tPTR* ppThreadContext)
{
	tERROR err_ret = errOK;

	cThreadContext* pCommonContext = (cThreadContext*) pCommonThreadContext;

	switch (CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		{
			cThreadContext* pContext = new cThreadContext;
			*ppThreadContext = pContext;
			if (!pContext)
				err_ret = errUNEXPECTED;
			else
			{
				*pContext  = *pCommonContext;
				pCommonContext->m_pJob->SendMessage(pmc_AVPG, pm_AVPG_NOTIFY_THREAD_STARTED, NULL, NULL);
			}
		}
		break;
	case TP_CBTYPE_THREAD_DONE_CONTEXT:
		if (*ppThreadContext)
		{
			cThreadContext* pContext = (cThreadContext*) *ppThreadContext;
			*ppThreadContext = 0;

			pCommonContext->m_pJob->SendMessage(pmc_AVPG, pm_AVPG_NOTIFY_THREAD_STOPPED, NULL, NULL);
			
			delete pContext;
		}
		break;
	case TP_CBTYPE_THREAD_YIELD:
		if (*ppThreadContext)
		{
			cThreadContext* pContext = (cThreadContext*) *ppThreadContext;

			switch(pContext->m_ContextType)
			{
			case _e_tht_Work:
				break;

			case _e_tht_Delayed:
				pContext->m_pJob->ProcessDelayedEvent(false);
				break;
			
			case _e_tht_Notify:
				break;

			default:
				break;
			}
		}
		break;
	}

	return err_ret;
}

tERROR pr_call OnAcc_WorkTaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen)
{
	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;
	case TP_CBTYPE_TASK_PROCESS:
		{
			cThreadContext* pContext = (cThreadContext*) pThreadContext;

			switch(pContext->m_ContextType)
			{
			case _e_tht_Work:
			case _e_tht_Delayed:
				{
					if (pContext->m_pNotifyParamData != pContext->m_pCommon->m_pNotifyParamData || 
					pContext->m_NotifyParamDataSize != pContext->m_pCommon->m_NotifyParamDataSize)
					{
						pContext->m_pNotifyParamData = pContext->m_pCommon->m_pNotifyParamData;
						pContext->m_NotifyParamDataSize = pContext->m_pCommon->m_NotifyParamDataSize;

						pContext->m_pJob->SendMessage(pmc_AVPG, pm_AVPG_NOTIFY_FOREACHTHREAD, 
							pContext->m_pNotifyParamData, &pContext->m_NotifyParamDataSize);
					}
					
					PAVPGS_EVENT_DESCRIBE pEvDescr = *((PAVPGS_EVENT_DESCRIBE*) pTaskData);

					if (!pContext->m_pJob->m_bIsSkipEvents)
						pContext->m_pJob->ProcessEvent(pEvDescr);

					pContext->m_pJob->FreeEvDescr(pEvDescr);
				}
				break;
			
			case _e_tht_Notify:
				{
					PR_TRACE((g_root, prtERROR, "avpgs\tnotify task (pid %d)", GetCurrentProcessId()));
				}
				break;

			default:
				break;
			}
		}
		break;
	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}

tERROR pr_call OnAcc_ConnectTaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen)
{
	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;
	case TP_CBTYPE_TASK_PROCESS:
		{
			cThreadContext* pContext = (cThreadContext*) pThreadContext;

			if (pContext->m_pNotifyParamData != pContext->m_pCommon->m_pNotifyParamData || 
				pContext->m_NotifyParamDataSize != pContext->m_pCommon->m_NotifyParamDataSize)
			{
				pContext->m_pNotifyParamData = pContext->m_pCommon->m_pNotifyParamData;
				pContext->m_NotifyParamDataSize = pContext->m_pCommon->m_NotifyParamDataSize;

				pContext->m_pJob->SendMessage(pmc_AVPG, pm_AVPG_NOTIFY_FOREACHTHREAD, 
					pContext->m_pNotifyParamData, &pContext->m_NotifyParamDataSize);
			}
			
			PAVPGS_EVENT_DESCRIBE pEvDescr = *((PAVPGS_EVENT_DESCRIBE*) pTaskData);

			if (!pContext->m_pJob->m_bIsSkipEvents)
				pContext->m_pJob->DriveConnect_Message(pEvDescr->m_pEvt);

			pContext->m_pJob->FreeEvDescr(pEvDescr);
		}
		break;
	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}

// ---------------------------------------

CJob::CJob(hOBJECT hTask) :
		m_hTask(hTask), 
		m_ThPoolWork(0),
		m_ThPoolConnect(0),
		m_ThPoolDelayed(0),
		m_ThPoolNotify(0),
		m_CacheSize(),
		m_MaxThreadCount(1),
		m_bIsSkipEvents(true),
		m_EvSync(),
		m_PHashTree_Delayed(0),
		m_PHashTree_PostPone(0),
		m_WorkingMode(cAVPG_WORKING_MODE_ON_SMART),
		m_hFltDll(NULL),
		m_FilterGetDosName(NULL),
		m_LuidListSync(),
		m_hSecLib(NULL),
		m_fnLsaGetLogonSessionData(NULL),
		m_fnLsaFreeReturnBuffer(NULL),
		m_LuidCount(0),
		m_hNetApi(NULL),
		m_pfNetApiBufferFree(NULL),
		m_pfNetSessionEnum(NULL),
		m_bSkipBackupFiles(FALSE)

{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_PerfomanceFreq);

	HMODULE hModAdvAPI = GetModuleHandle("AdvAPI32.dll");
	if( hModAdvAPI )
	{
		*(void**)&m_pfSetThreadToken = GetProcAddress(hModAdvAPI, "SetThreadToken");

		if (m_pfSetThreadToken)
			*(void**)&m_pfOpenThreadToken = GetProcAddress(hModAdvAPI, "OpenThreadToken");
	}


	m_hFltDll = LoadLibrary("fltlib.dll");
	if (m_hFltDll)
		*(void**)&m_FilterGetDosName = GetProcAddress(m_hFltDll, "FilterGetDosName");

	m_hSecLib = ::LoadLibrary("Secur32.dll");
	if(m_hSecLib)
	{
		*(void**)&m_fnLsaGetLogonSessionData = GetProcAddress(m_hSecLib, "LsaGetLogonSessionData");
		*(void**)&m_fnLsaFreeReturnBuffer = GetProcAddress(m_hSecLib, "LsaFreeReturnBuffer");
	}

	if (!m_fnLsaGetLogonSessionData)
		PR_TRACE((g_root, prtIMPORTANT, "avpgs\tLsaGetLogonSessionData not resolved"));

	m_hNetApi = ::LoadLibrary("Netapi32.dll");
	if (m_hNetApi)
	{
		*(void**)&m_pfNetApiBufferFree = GetProcAddress(m_hNetApi, "NetApiBufferFree");
		if (m_pfNetApiBufferFree)
		{
			*(void**)&m_pfNetSessionEnum = GetProcAddress(m_hNetApi, "NetSessionEnum");
		}
	}
};

CJob::~CJob()
{
	if (m_ThPoolWork)
		m_ThPoolWork->sysCloseObject();

	if (m_ThPoolConnect)
		m_ThPoolConnect->sysCloseObject();

	if (m_ThPoolDelayed)
		m_ThPoolDelayed->sysCloseObject();
	
	if (m_ThPoolNotify)
		m_ThPoolNotify->sysCloseObject();

	m_EvSync.Lock();
	if (m_PHashTree_Delayed)
	{
		if (m_PHashTree_Delayed->m_ItemsCount)
		{
			PAVPGS_EVENT_DESCRIBE pEvDescr;
			PHashTreeItem pHashItem = _Hash_GetFirst(m_PHashTree_Delayed);
			while (pHashItem)
			{
				pEvDescr = (PAVPGS_EVENT_DESCRIBE) pHashItem->m_pUserData;
				FreeEvDescr(pEvDescr);

				pHashItem = pHashItem->m_pCommonNext;
			}
		}
		
		_Hash_Done(m_PHashTree_Delayed, 0);
		m_PHashTree_Delayed = 0;
	}

	if (m_PHashTree_PostPone)
	{
		if (m_PHashTree_PostPone->m_ItemsCount)
		{
			PAVPGS_EVENT_DESCRIBE pEvDescr;
			PHashTreeItem pHashItem = _Hash_GetFirst(m_PHashTree_PostPone);
			while (pHashItem)
			{
				pEvDescr = (PAVPGS_EVENT_DESCRIBE) pHashItem->m_pUserData;

				if (pEvDescr->m_bProcessing)
					pHashItem = pHashItem->m_pCommonNext;
				else
				{
					_Hash_DelItem(m_PHashTree_PostPone, pHashItem, 0);
					FreeEvDescr(pEvDescr);

					pHashItem = _Hash_GetFirst(m_PHashTree_PostPone);
				}
			}
		}
	}

	m_EvSync.UnLock();

	if (m_PHashTree_PostPone)
	{
		bool bExistData = true;
		while (bExistData)
		{
			m_EvSync.Lock();

			if (!_Hash_GetFirst(m_PHashTree_PostPone))
				bExistData = false;

			m_EvSync.UnLock();

			if (bExistData)
				Sleep(50);
		}

		_Hash_Done(m_PHashTree_PostPone, 0);
		m_PHashTree_PostPone = 0;
	}

	HookTask::Done();

	if (m_hFltDll)
	{
		FreeLibrary(m_hFltDll);
		m_hFltDll = NULL;
	}
	
	if (m_hSecLib)
	{
		FreeLibrary(m_hSecLib);
		m_hSecLib = NULL;
	}

	if (m_hNetApi)
	{
		FreeLibrary(m_hNetApi);
		m_hNetApi = NULL;
	}

	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tlocal allocs counter on exit %d (pid %d)", 
		g_HeapCounter, GetCurrentProcessId()));
}

void CJob::SetMaxThreadCount(DWORD maxthreads)
{
	if (!maxthreads)
		maxthreads = 1;

	m_MaxThreadCount = maxthreads;
	if (m_ThPoolWork)
		m_ThPoolWork->set_pgTP_MAX_THREADS(m_MaxThreadCount);
}

void CJob::SendMessage(tDWORD msg_cls, tDWORD msg_id, tPTR buf, tDWORD* pblen)
{
	m_hTask->sysSendMsg(msg_cls, msg_id, m_hTask, buf, pblen);
}

bool CJob::CreateThreadPool(cThreadPool** ppThPool, tDWORD MaxLen, tThreadPriority Priority, 
							tDWORD MinThreads, tDWORD MaxThreads, tDWORD IdleCount, tPTR pContext)
{
	tERROR error = m_hTask->sysCreateObject((hOBJECT*)ppThPool, IID_THREADPOOL, PID_THPOOLIMP);
	if (PR_FAIL(error))
	{
		*ppThPool = 0;
		return false;
	}

	(*ppThPool)->set_pgTP_NAME("AVPGS", sizeof("AVPGS"), cCP_ANSI);
	(*ppThPool)->set_pgTP_TASK_QUEUE_MAX_LEN(MaxLen);

	(*ppThPool)->set_pgTP_DEFAULT_THREAD_PRIORITY(Priority);
	(*ppThPool)->set_pgTP_MIN_THREADS(MinThreads);
	(*ppThPool)->set_pgTP_MAX_THREADS(MaxThreads);

	(*ppThPool)->set_pgTP_QUICK_EXECUTE(cTRUE);
	(*ppThPool)->set_pgTP_QUICK_DONE(cFALSE);

	(*ppThPool)->set_pgTP_MAX_IDLE_COUNT(IdleCount);
	(*ppThPool)->set_pgTP_INVISIBLE_FLAG(cTRUE);
	(*ppThPool)->set_pgTP_YIELD_TIMEOUT(5000);
	
	(*ppThPool)->set_pgTP_THREAD_CALLBACK(OnAcc_ThreadCallback);
	(*ppThPool)->set_pgTP_THREAD_CALLBACK_CONTEXT(pContext);

	error = (*ppThPool)->sysCreateObjectDone();
	if (PR_FAIL(error))
	{
		(*ppThPool)->sysCloseObject();
		*ppThPool = 0;
		return false;
	}

	return true;
}

bool CJob::Start()
{
	bool bRet = false;

	_Hash_Init_Library(_MM_Alloc, _MM_Free, _MM_Compare, 0);
	m_PHashTree_Delayed = _Hash_InitNew(8);
	m_PHashTree_PostPone = _Hash_InitNew(8);

	if (!m_PHashTree_Delayed || !m_PHashTree_PostPone)
		return false;

	m_CommonThreadContext.m_pJob = this;
	m_CommonThreadContext.m_pCommon = &m_CommonThreadContext;

	m_DelayedThreadContext.m_pJob = this;
	m_DelayedThreadContext.m_pCommon = &m_CommonThreadContext;
	m_DelayedThreadContext.m_ContextType = _e_tht_Delayed;

	m_NotifyThreadContext.m_pJob = this;
	m_NotifyThreadContext.m_pCommon = &m_CommonThreadContext;
	m_NotifyThreadContext.m_ContextType = _e_tht_Notify;
	

//+ ------------------------------------------------------------------------------------------
	bRet = CreateThreadPool(&m_ThPoolWork, max(2048, m_MaxThreadCount), TP_THREADPRIORITY_TIME_CRITICAL, 2, m_MaxThreadCount, 3, &m_CommonThreadContext);
	if (bRet)
		bRet = CreateThreadPool(&m_ThPoolConnect, 0, TP_THREADPRIORITY_NORMAL, 0, 1, 1, &m_CommonThreadContext);

	if (bRet)
		bRet = CreateThreadPool(&m_ThPoolDelayed, 4096, TP_THREADPRIORITY_ABOVE_NORMAL, 1, 1, (tDWORD) -1, &m_DelayedThreadContext);

	CreateThreadPool(&m_ThPoolNotify, 128, TP_THREADPRIORITY_TIME_CRITICAL, 0, 1, 1, NULL);


	//+ ------------------------------------------------------------------------------------------
	if (bRet)
		bRet = HookTask::Start(this);

	return bRet;
}

void CJob::BeforeExit()
{
}

tERROR CJob::AddInvisibleFile(tSTRING pcz)
{
	DWORD dwSite = 0;
	
	if (pcz == NULL)
	{
		FILTER_TRANSMIT _Filter;
		_Filter.m_AppID = m_AppID;
		_Filter.m_FltCtl = FLTCTL_RESET_FILTERS;

		if (IDriverFilterTransmit(m_hDevice, &_Filter, &_Filter, sizeof(_Filter), sizeof(_Filter)) == FALSE)
			return errUNEXPECTED;
		
		if (AddFilters())
			return errOK;

		return errNOT_OK;
	}

	if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		int dwlen = (lstrlen(pcz) + 2) * sizeof(WCHAR);
		PWCHAR pwUni = (PWCHAR) _MM_Alloc(sizeof(WCHAR) * dwlen);
		if (pwUni)
		{
			if (MultiByteToWideChar(CP_ACP, 0, pcz, -1, pwUni, dwlen) != 0)
			{
				AddFSFilterW(m_hDevice, m_AppID, pwUni, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, &dwSite);
				AddFSFilterW(m_hDevice, m_AppID, pwUni, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing, &dwSite);
				AddFSFilterW(m_hDevice, m_AppID, pwUni, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_FIOR, FastIoWrite, 0, 0, PreProcessing, &dwSite);
				AddFSFilterW(m_hDevice, m_AppID, pwUni, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_NORMAL, 0, PreProcessing, &dwSite);
				AddFSFilterW(m_hDevice, m_AppID, pwUni, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_MDL, 0, PreProcessing, &dwSite);
				
				AddFSFilterW(m_hDevice, m_AppID, pwUni, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing, &dwSite);
			}
			
			_MM_Free(pwUni);
		}
	}
	else
	{
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS,IFSFN_OPEN, 0, 0, PreProcessing, &dwSite);
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS,IFSFN_OPEN, 1, 0, PreProcessing, &dwSite);
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS,IFSFN_OPEN, 2, 0, PreProcessing, &dwSite);
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS,IFSFN_OPEN, 3, 0, PreProcessing, &dwSite);
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_I21,0x3d, 0, 0, PreProcessing, &dwSite);	//open
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_I21,0x71, 0x6c, 0, PreProcessing, &dwSite);	//LFN	+ EXT OPEN/CREATE
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_I21,0x3e, 0, 0, PreProcessing, &dwSite);	//int21 close
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_R3, 5, 0, 0, PreProcessing, &dwSite);	//CreateFileA
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_CLOSE, 0, 0, PostProcessing, &dwSite);
		AddFSFilter(m_hDevice, m_AppID, pcz, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_CLOSE, 2/*FINAL*/, 0, PostProcessing, &dwSite);
	}
	
	return errOK;
}

void FillParamNOP(PFILTER_PARAM Param, DWORD ParamFlags, ULONG ParamID)
{
	Param->m_ParamFlags = ParamFlags;
	Param->m_ParamFlt = FLT_PARAM_NOP;
	Param->m_ParamID = ParamID;
	lstrcpyW((PWCHAR) Param->m_ParamValue, L"*");
	Param->m_ParamSize = (lstrlenW(L"*") + 1) * sizeof(WCHAR);
}

bool CJob::IsExistFilters()
{
	char pFilterBuffer[0x1000];
	
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;
	
	_pFilter->m_AppID = m_AppID;
	_pFilter->m_FltCtl = FLTCTL_FIRST;
	
	if (IDriverFilterTransmit(m_hDevice, _pFilter, _pFilter, sizeof(pFilterBuffer), sizeof(pFilterBuffer)) == FALSE)
		return false;

	return true;
}


bool CJob::AddFilters()
{
	bool bRet = true;

	if (IsExistFilters())
		return true;
	
	if (VER_PLATFORM_WIN32_NT != m_OsInfo.dwPlatformId)
		m_WorkingMode = cAVPG_WORKING_MODE_ON_ACCESS;
	
	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tWorking mode %d (drv flags 0x%x)", m_WorkingMode, m_DrvFlags));

	if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
	{
		if (!m_FilterGetDosName)
			return false;

		BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(WCHAR) * 4];
		PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
		FillParamNOP(Param1, _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG, _PARAM_OBJECT_URL_W);

		BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(WCHAR) * 4];
		PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
		FillParamNOP(Param2, _FILTER_PARAM_FLAG_CACHABLE, _PARAM_OBJECT_VOLUME_NAME_W);

		//+ ban access filter
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_NFIOR, 
			IRP_MJ_CREATE, 0,  0, PreProcessing, NULL, NULL);
		//- ban access filter

		if (cAVPG_WORKING_MODE_ON_EXECUTE == m_WorkingMode ||
			cAVPG_WORKING_MODE_ON_SMART == m_WorkingMode
			|| cAVPG_WORKING_MODE_ON_ACCESS == m_WorkingMode)
		{
			AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM, 
				MJ_SYSTEM_CREATE_SECTION, 0,  0, PreProcessing, NULL, Param1, Param2, NULL);
		}

		if (cAVPG_WORKING_MODE_ON_EXECUTE != m_WorkingMode)
		{
			AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_NFIOR, 
				IRP_MJ_CREATE, 0,  0, PostProcessing, NULL, Param1, Param2, NULL);
			
			if (cAVPG_WORKING_MODE_ON_OPEN == m_WorkingMode)
			{
				PR_TRACE((g_root, prtIMPORTANT, "avpgs\tclose op skipped by working mode" ));
			}
			else
			{
				AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_NFIOR, 
					IRP_MJ_CLEANUP, 0,  0, PreProcessing, NULL, Param1, Param2, NULL);
			}
		}

		//+ reset cache item
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR, 
			IRP_MJ_WRITE, IRP_MN_NORMAL,  0, PostProcessing, NULL, Param1, Param2, NULL);

		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR, 
			IRP_MJ_WRITE, IRP_MN_MDL,  0, PostProcessing, NULL, Param1, Param2, NULL);
		//- reset cache item

		Param1->m_ParamID = _PARAM_OBJECT_URL_DEST_W;
		Param2->m_ParamID = _PARAM_OBJECT_VOLUME_NAME_DEST_W;
		AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR, 
			IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PostProcessing, NULL, Param1, Param2, NULL);
			
		AddFSFilterW(m_hDevice, m_AppID, L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_NFIOR, 
			IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_MOUNT_VOLUME, 0, PostProcessing, NULL);

		AddFSFilterW(m_hDevice, m_AppID, L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_RESETCACHE, FLTTYPE_NFIOR, 
			IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_UNMOUNT_VOLUME, 0, PostProcessing, NULL);

		return true;
	}

	if (cAVPG_WORKING_MODE_ON_EXECUTE != m_WorkingMode)
	{
		// Non operative files ------------------------------------------------------------------------------
		char* pcz;
		DWORD idx;
		DWORD cou = GetListNonOpFiles(&m_OsInfo);
		for (idx = 0; idx < cou; idx++)
		{
			pcz = GetNonOpFile(idx);
			if (AddInvisibleFile(pcz) != errOK)
				bRet = false;
		}
		
		NonOpFilesDone();
	}

	// Process filters
	if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{

		if ((cAVPG_WORKING_MODE_ON_SMART == m_WorkingMode)
			|| (cAVPG_WORKING_MODE_ON_EXECUTE == m_WorkingMode)
			|| (cAVPG_WORKING_MODE_ON_ACCESS == m_WorkingMode))
		{
			ULONG AccessFlg;
			BYTE Buf1[sizeof(FILTER_PARAM) + MAXPATHLEN + 1];
			BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(ULONG)];
			PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
			PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
			
			Param1->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG;
			Param1->m_ParamFlt = FLT_PARAM_NOP;
			Param1->m_ParamID = _PARAM_OBJECT_URL_W;
			lstrcpyW((PWCHAR) Param1->m_ParamValue, L"*");
			Param1->m_ParamSize = (lstrlenW(L"*") + 1) * sizeof(WCHAR);
			
			Param2->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			Param2->m_ParamFlt = FLT_PARAM_AND;
			Param2->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
			AccessFlg = SECTION_MAP_EXECUTE;
			*(ULONG*)Param2->m_ParamValue = AccessFlg;
			Param2->m_ParamSize = sizeof(ULONG);
			
			AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE,
				FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_SECTION, 0, 0, PreProcessing, NULL, Param1, Param2, NULL);
		}
	}
	else
	{
		// todo:
	}

	if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (cAVPG_WORKING_MODE_ON_EXECUTE != m_WorkingMode)
		{
			// -----------------------------------------------------------------------------------------
			// разрешенное по умолчанию!
			//!! Эти фильтры снимают лочку при обращении к бутам или к чему-то подобному. Разобраться!!!
			AddFSFilterW(m_hDevice, m_AppID, L"*$INDEX_ALLOCATION", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL);
			AddFSFilterW(m_hDevice, m_AppID, L"*$INDEX_ALLOCATION", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL);
			// а эти на ntfs только (остальные разделы снять
			AddFSFilterW(m_hDevice, m_AppID, L"*:\\System Volume Information\\tracking.log", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL);
			AddFSFilterW(m_hDevice, m_AppID, L"*:\\System Volume Information\\tracking.log", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL);
		}
		
		// -----------------------------------------------------------------------------------------
		if (cAVPG_WORKING_MODE_ON_EXECUTE != m_WorkingMode)
		{
			{
				// opening file with attributes
				ULONG AccessFlg;
				BYTE Buf1[sizeof(FILTER_PARAM) + MAXPATHLEN + 1];
				BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(ULONG)];
				PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
				PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
				
				Param1->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG;
				Param1->m_ParamFlt = FLT_PARAM_NOP;
				Param1->m_ParamID = _PARAM_OBJECT_URL_W;
				lstrcpyW((PWCHAR) Param1->m_ParamValue, L"*");
				Param1->m_ParamSize = (lstrlenW(L"*") + 1) * sizeof(WCHAR);
				
				Param2->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
				Param2->m_ParamFlt = FLT_PARAM_AND;
				Param2->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
				AccessFlg = FILE_READ_EA | FILE_WRITE_EA | FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES | DELETE;
				AccessFlg = ~AccessFlg;
				*(ULONG*)Param2->m_ParamValue = AccessFlg;
				Param2->m_ParamSize = sizeof(ULONG);
				
				AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 
					0, PreProcessing, NULL, Param1, Param2, NULL);
			}
			
			{
				BYTE Buf1[sizeof(FILTER_PARAM) + MAXPATHLEN + 1];
				PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
				
				Param1->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG;
				Param1->m_ParamFlt = FLT_PARAM_NOP;
				Param1->m_ParamID = _PARAM_OBJECT_URL_DEST_W;
				lstrcpyW((PWCHAR) Param1->m_ParamValue, L"*");
				Param1->m_ParamSize = (lstrlenW(L"*") + 1) * sizeof(WCHAR);
				
				AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_DROPCACHE, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 
					0, PreProcessing, NULL, Param1, NULL);
			}
		}
		
		AddFSFilterW(m_hDevice, m_AppID, L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO | FLT_A_RESETCACHE, FLTTYPE_NFIOR, IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_MOUNT_VOLUME, 0, PostProcessing, NULL);  
		AddFSFilterW(m_hDevice, m_AppID, L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO | FLT_A_RESETCACHE, FLTTYPE_NFIOR, IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_VERIFY_VOLUME, 0, PostProcessing, NULL);   
		
		// writing to file - auto pass and clear cache (FLT_A_INFO - переполняет очередь под стрессами)

		AddFSFilterW(m_hDevice, m_AppID, L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_NORMAL, 0, PreProcessing, NULL);
		AddFSFilterW(m_hDevice, m_AppID, L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_MDL, 0, PreProcessing, NULL);
		
		AddFSFilterW(m_hDevice, m_AppID, L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_FIOR, FastIoWrite, 0, 0, PreProcessing, NULL);
		
		//close file
		if ((cAVPG_WORKING_MODE_ON_OPEN != m_WorkingMode)
			&& (cAVPG_WORKING_MODE_ON_EXECUTE != m_WorkingMode))
		{
			AddFSFilterW(m_hDevice, m_AppID, L"\\\\*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_SAVE2CACHE | FLT_A_USECACHE | FLT_A_INFO, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL);
			AddFSFilterW(m_hDevice, m_AppID,     L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_SAVE2CACHE | FLT_A_USECACHE | FLT_A_INFO, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL);
		}
		
		// -----------------------------------------------------------------------------------------
	}
	else
	{
		// -----------------------------------------------------------------------------------------
		// разрешенное по умолчанию!
		// printing
		AddFSFilter(m_hDevice, m_AppID, "*\\PRN","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 0, 0, PreProcessing, NULL); 
		AddFSFilter(m_hDevice, m_AppID, "*\\PRN","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 1, 0, PreProcessing, NULL); 
		AddFSFilter(m_hDevice, m_AppID, "*\\PRN","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 2, 0, PreProcessing, NULL); 
		
		AddFSFilter(m_hDevice, m_AppID, "*\\LPT1","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 0, 0, PreProcessing, NULL); 
		AddFSFilter(m_hDevice, m_AppID, "*\\LPT1","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 1, 0, PreProcessing, NULL); 
		AddFSFilter(m_hDevice, m_AppID, "*\\LPT1","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 2, 0, PreProcessing, NULL); 
		
		AddFSFilter(m_hDevice, m_AppID, "*\\LPT2","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 0, 0, PreProcessing, NULL); 
		AddFSFilter(m_hDevice, m_AppID, "*\\LPT2","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 1, 0, PreProcessing, NULL); 
		AddFSFilter(m_hDevice, m_AppID, "*\\LPT2","*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 2, 0, PreProcessing, NULL); 

		// -----------------------------------------------------------------------------------------
		
		AddFSFilter(m_hDevice, m_AppID, "*\\", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 0, 0, PreProcessing, NULL);
		AddFSFilter(m_hDevice, m_AppID, "*\\", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 1, 0, PreProcessing, NULL);
		AddFSFilter(m_hDevice, m_AppID, "*\\", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 2, 0, PreProcessing, NULL);
		AddFSFilter(m_hDevice, m_AppID, "*\\", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_OPEN, 3, 0, PreProcessing, NULL);
		
		// VIK 05.02.02 15:00
		AddFSFilter(m_hDevice, m_AppID, "*\\", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_IFS, IFSFN_CLOSE, 0, 0, PostProcessing, NULL);
		AddFSFilter(m_hDevice, m_AppID, "\\\\.\\*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_R3, 5, 0, 0, PreProcessing, NULL);//CreateFileA

		// -----------------------------------------------------------------------------------------
		// рабочие фильтры
		// ring 3
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 0, 0, 0, PreProcessing, NULL);//	CreateProcessA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 1, 0, 0, PreProcessing, NULL);//WinExec
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 2, 0, 0, PreProcessing, NULL);//LoadModule
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 3, 0, 0, PreProcessing, NULL);//LoadLibraryA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 4, 0, 0, PreProcessing, NULL);//LoadLibraryExA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 5, 0, 0, PreProcessing, NULL);//CreateFileA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 6, 0, 0, PreProcessing, NULL);//OpenFile
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 7, 0, 0, PreProcessing, NULL);//_lopen
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 9, 0, 0, PreProcessing, NULL);//CopyFileA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 10, 0, 0, PreProcessing, NULL);//CopyFileExA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 11, 0, 0, PreProcessing, NULL);//MoveFileA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 12, 0, 0, PreProcessing, NULL);//MoveFileExA
		//13 //DeleteFileA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 14, 0, 0, PreProcessing, NULL);//SetFileAttributesA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 15, 0, 0, PreProcessing, NULL);//GetPrivateProfileIntA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 16, 0, 0, PreProcessing, NULL);//GetProfileIntA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 17, 0, 0, PreProcessing, NULL);//WritePrivateProfileSectionA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 18, 0, 0, PreProcessing, NULL);//GetPrivateProfileSectionA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 19, 0, 0, PreProcessing, NULL);//WriteProfileSectionA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 20, 0, 0, PreProcessing, NULL);//GetProfileSectionA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 21, 0, 0, PreProcessing, NULL);//GetPrivateProfileSectionNamesA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 22, 0, 0, PreProcessing, NULL);//WritePrivateProfileStringA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 23, 0, 0, PreProcessing, NULL);//GetPrivateProfileStringA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 24, 0, 0, PreProcessing, NULL);//WriteProfileStringA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 25, 0, 0, PreProcessing, NULL);//GetProfileStringA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 26, 0, 0, PreProcessing, NULL);//WritePrivateProfileStructA
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_R3, 27, 0, 0, PreProcessing, NULL);//GetPrivateProfileStructA
	
		// opening file
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_IFS, IFSFN_OPEN, 0, 0, PreProcessing, NULL);

		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_IFS, IFSFN_OPEN, 1, 0, PreProcessing, NULL);
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_IFS, IFSFN_OPEN, 2, 0, PreProcessing, NULL);
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_IFS, IFSFN_OPEN, 3, 0, PreProcessing, NULL);

		//rename
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_IFS, IFSFN_RENAME, 0, 0, PreProcessing, NULL);

		// writing to file
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_IFS, IFSFN_WRITE, 0, 0, PreProcessing, NULL);

		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO | FLT_A_RESETCACHE, FLTTYPE_IFS, IFSFN_CONNECT, 0, 0, PostProcessing, NULL);
		
		// VIK 05.02.02 15:00
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_SAVE2CACHE | FLT_A_USECACHE | FLT_A_INFO, FLTTYPE_IFS, IFSFN_CLOSE, 0, 0, PostProcessing, NULL);
		AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_SAVE2CACHE | FLT_A_USECACHE | FLT_A_INFO, FLTTYPE_IFS, IFSFN_CLOSE, 2, 0, PostProcessing, NULL); //close final


		{
			DWORD fltcount;
			typedef struct __TRI_DWORDA
			{
				DWORD Mj;
				DWORD Mi;
				DWORD Flg;
				PROCESSING_TYPE Processing;
			}_TRI_DWORDA;
			_TRI_DWORDA FltArr[] = {
				// open
				{0x0f, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//OPEN FCB
				{0x16, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//CREATE FCB
				{0x17, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//rename FCB
				{0x3c, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//CREATE
				{0x3d, 0,  FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},	//OPEN
				{0x56, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//rename
				{0x5a, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//CREATE TEMP
				{0x5b, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//CREATE NEW
				{0x6c, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//EXT OPEN/CREATE
				{0x71, 0x6c, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},//LFN	+ EXT OPEN/CREATE
				{0x71, 0x56, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},//LFN	+ rename

				{0x15, 0, FLT_A_DROPCACHE, PreProcessing},					//SEQUENTIAL WRITE FCB
				{0x22, 0, FLT_A_DROPCACHE, PreProcessing},					//WRITE RECORD FCB
				{0x28, 0, FLT_A_DROPCACHE, PreProcessing},					//WRITE BLOCK FCB
				{0x40, 0, FLT_A_DROPCACHE, PreProcessing},					//WRITE

				// exuecute
				{0x4b, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, PreProcessing},		//dos-execute

				// close
				{0x10, 0, /*FLT_A_POPUP | */FLT_A_USECACHE | FLT_A_SAVE2CACHE, PostProcessing},		//CLOSE FCB
				{0x3e, 0, /*FLT_A_POPUP | */FLT_A_USECACHE | FLT_A_SAVE2CACHE, PostProcessing}		//CLOSE
			};

			for (fltcount = 0; fltcount < sizeof(FltArr) / sizeof(_TRI_DWORDA); fltcount++)
			{
				AddFSFilter(m_hDevice, m_AppID, "*", "*", DEADLOCKWDOG_TIMEOUT, 
					FltArr[fltcount].Flg, FLTTYPE_I21, FltArr[fltcount].Mj, FltArr[fltcount].Mi, 0, 
					FltArr[fltcount].Processing, NULL);
			}
		}
	}
	
	return bRet;
}

bool CJob::RestartFilters()
{
	ResetFilters();
	return AddFilters();
}

void CJob::SingleEvent()
{
	tERROR err = errUNEXPECTED;
	PEVENT_TRANSMIT pEvt = NULL;

	ULONG Mark;
	tTaskId tid;

	VERDICT Verdict = Verdict_Default;

	bool bFoundInDelayed = false;
	
	if (GetEventData(&m_DrvEventList, m_hDevice, m_AppID, (BYTE**) &pEvt, &Mark))
	{
		PAVPGS_EVENT_DESCRIBE pEvDescr = NULL;
		_eEventOperation eop = GetFileOperation(pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi, m_OsInfo.dwPlatformId);
		_eEventOperation eop_real = eop;

		PSINGLE_PARAM pSingleObjectName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);
		
		ULONG ContextFlags = 0;
		PSINGLE_PARAM pSingleContextFlags = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CONTEXT_FLAGS);
		if (pSingleContextFlags && sizeof(ULONG) == pSingleContextFlags->ParamSize)
			ContextFlags = *(ULONG*) pSingleContextFlags->ParamValue;

		ULONG access_attr = 0;
		PSINGLE_PARAM pParamAccessAttr = IDriverGetEventParam(pEvt, _PARAM_OBJECT_ACCESSATTR);
		if (pParamAccessAttr && (sizeof(ULONG) == pParamAccessAttr->ParamSize))
			access_attr = *(ULONG*) pParamAccessAttr->ParamValue;

		PLUID pLuid = NULL;
		PSINGLE_PARAM pSingleLuid = IDriverGetEventParam(pEvt, _PARAM_OBJECT_LUID);
		if (pSingleLuid && (sizeof(LUID) == pSingleLuid->ParamSize))
			pLuid = (PLUID) pSingleLuid->ParamValue;

		if (m_OsInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
			access_attr = FILE_READ_DATA | FILE_EXECUTE | FILE_WRITE_DATA;

		if (m_bIsSkipEvents)
			eop = _op_other;
		else
		{
			if (pEvt->m_ProcessID == GetCurrentProcessId())
				eop = _op_other;

			if (IgnoreProcess_IsSkipThis(pEvt->m_ProcessID))
				eop = _op_other;
			else
			{
				if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
				{
					if (pEvt->m_EventFlags & _EVENT_FLAG_POSTPROCESSING)
					{
						if (m_bSkipBackupFiles)
						{
							if (_CONTEXT_OBJECT_FLAG_BACKUP & ContextFlags)
							{
								if ((FILE_EXECUTE | FILE_WRITE_DATA | FILE_APPEND_DATA) & access_attr)
								{
									// check file access
									PR_TRACE((g_root, prtERROR, "avpgs\tprocess %s %d backup access 0x%x to '%S'",
										pEvt->m_ProcName, pEvt->m_ProcessID, access_attr,
										pSingleObjectName->ParamValue));
								}
								else
								{
									eop = _op_other; // skip
								}
							}
						}
					}
					else
					{
						if (IRP_MJ_CREATE == pEvt->m_FunctionMj) // pre create - check user ban
						{
							eop = _op_other;					// skip processing by av

							if (pLuid)
							{
								cUserInfo UserInfo;
								
								UserInfo.m_Luid = pLuid->LowPart;
								UserInfo.m_LuidHP = pLuid->HighPart;

								if (IsUserBaned( &UserInfo ))
								{
									Verdict = Verdict_Discard;
								
									PR_TRACE((g_root, prtIMPORTANT, "avpgs\tdeny op on pre"));
								}
							}
						}
					}
				}
			}
		}

		/*if (_op_other != eop)
		{
			PR_TRACE((g_root, prtIMPORTANT, "avpgs\tincome op %d '%S' (context flags 0x%x)", eop,
				pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>",
				ContextFlags));
		}*/

		if (_op_close == eop)
		{
			if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
			{
				if (m_bSkipBackupFiles)
				{
					if (_CONTEXT_OBJECT_FLAG_BACKUP & ContextFlags)
					{
						if (_CONTEXT_OBJECT_FLAG_MODIFIED & ContextFlags)
						{
							PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\topened for backup but modified '%S'",
								pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));

						}
						else
							eop = _op_other; // opened not for write, check only modified files
					}
				}

				if ((_CONTEXT_OBJECT_FLAG_FORWRITE | _CONTEXT_OBJECT_FLAG_MODIFIED) & ContextFlags)
				{
				}
				else
				{
					eop = _op_other; // opened not for write, check only modified files
					PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tskip close not for write '%S' - context 0x%x",
						pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>",
						ContextFlags));
				}
			}
			else
			{
				if (pSingleObjectName)
				{
					if (pSingleObjectName->ParamSize > sizeof(WCHAR))
					{
						if ((((char*)pSingleObjectName->ParamValue)[0] == '\\'))
						{
							eop = _op_other;
							/*PR_TRACE((g_root, prtIMPORTANT, "avpgs\tskip event - close on network '%S'", 
								pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));*/
						}
					}
				}
			}
		}
		else if (_op_create == eop)
		{
			if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
			{
				if (ContextFlags & (_CONTEXT_OBJECT_FLAG_EXECUTE | _CONTEXT_OBJECT_FLAG_FORREAD))
				{

				}
				else
				{
					eop = _op_other;

					PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tskip by access attrib 0x%x '%S' (context flags 0x%x)", access_attr,
						pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>",
						ContextFlags));
				}
			}
			else
			{
				if (!access_attr)
				{
					if (!(access_attr & (FILE_READ_DATA | FILE_EXECUTE | FILE_WRITE_DATA)))
					{
						eop = _op_other;
				
						PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tskip by access attrib 0x%x '%S'", access_attr,
							pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));
					}
				}
			}
		}
		else if (_op_execute == eop)
		{
			if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
			{
				// already filed right parametes
			}
			else
			{
				eop = _op_create;

				access_attr = FILE_READ_DATA | FILE_EXECUTE;

				PSINGLE_PARAM pSinglePageProtection = IDriverGetEventParam(pEvt, _PARAM_OBJECT_PAGEPROTECTION);

				if (pSinglePageProtection && (pSinglePageProtection->ParamSize == sizeof(ULONG)))
				{
					ULONG PageProt = *(ULONG*) pSinglePageProtection->ParamValue;

					if (PageProt & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
					{

					}
					else
					{
						// load not for execute
						eop = _op_other;
					}
				}
			}
		}

		if (_op_create == eop || _op_close == eop)
		{
			if (_CONTEXT_OBJECT_FLAG_NETWORKTREE & ContextFlags)
			{
				/*PR_TRACE((g_root, prtNOTIFY, "avpgs\tskiping <%s> with NETWORKTREE flag  from %.32s(%d) - '%S'",
					eop == _op_create ? "create" :
						eop == _op_write ? "write" :
						"unknown",
					pEvt->m_ProcName, 
					pEvt->m_ProcessID, 
					pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));*/
				eop = _op_other;
			}
			else if (_CONTEXT_OBJECT_FLAG_DIRECTORY & ContextFlags)
			{
				/*PR_TRACE((g_root, prtNOTIFY, "avpgs\tskiping <%s> with DIRECTORY flag  from %.32s(%d) - '%S'",
					eop == _op_create ? "create" :
						eop == _op_write ? "write" :
						"unknown",
					pEvt->m_ProcName, 
					pEvt->m_ProcessID, 
					pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));*/
				eop = _op_other;
			}
		}

		PR_TRACE((g_root, prtNOTIFY, "avpgs\tcheck op %d (real %d) '%S' (context flags 0x%x)", eop, eop_real,
				pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>",
				ContextFlags));

		if (_op_other != eop)
			pEvDescr = (PAVPGS_EVENT_DESCRIBE) _MM_Alloc(sizeof(AVPGS_EVENT_DESCRIBE));

		if (pEvDescr)
		{
			pEvDescr->m_SetVerdict.m_Mark = Mark;
			pEvDescr->m_hDevice = m_hDevice;
			pEvDescr->m_pEvt = pEvt;
			pEvDescr->m_eop = eop;
			pEvDescr->m_hTokenImpersonate = NULL;
			pEvDescr->m_bProcessing = false;
			pEvDescr->m_bBreakProcessing = false;
			pEvDescr->m_bMiniKav = false;

			pEvDescr->m_SetVerdict.m_AppID = m_AppID;
			pEvDescr->m_SetVerdict.m_Verdict = Verdict_Default;
			pEvDescr->m_SetVerdict.m_ExpTime = 0;

			memset(&pEvDescr->m_CreatorLuid, 0, sizeof(LUID));
			memset(&pEvDescr->m_UserOverrideLuid, 0, sizeof(LUID));

			pEvDescr->m_PtrFN = NULL;

			/*PR_TRACE((g_root, prtIMPORTANT, "avpgs\tallocated for process op %d '%S'", eop,
				pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));*/

			if (_op_close == eop)
			{
				if (pLuid)
				{
					memcpy(&pEvDescr->m_CreatorLuid, pLuid, sizeof(LUID));

					PR_TRACE((g_root, prtNOTIFY, "avpgs\tclose luid 0x%x-0x%x",
						pEvDescr->m_CreatorLuid.HighPart, pEvDescr->m_CreatorLuid.LowPart));

					if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
					{
						if (_CONTEXT_OBJECT_FLAG_MODIFIED & ContextFlags)
						{
							pEvDescr->m_UserOverrideLuid = pEvDescr->m_CreatorLuid;
			
							PR_TRACE((g_root, prtNOTIFY, "avpgs\tclose modified override luid 0x%x-0x%x",
								pEvDescr->m_UserOverrideLuid.HighPart, pEvDescr->m_UserOverrideLuid.LowPart));
						}
					}
				}
			}

			if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
			{
				if (pEvt->m_EventFlags & _EVENT_FLAG_OBJECT_ATTENDED)
					pEvDescr->m_bMiniKav = true; // IO already opened - use mini-kav IO to read/write

				if (_op_close == eop)
				{
					bool bGoDelayed = false;

					if (!(_CONTEXT_OBJECT_FLAG_FORREAD & ContextFlags))
						bGoDelayed = true;
					else if (cAVPG_WORKING_MODE_ON_SMART == m_WorkingMode)
						bGoDelayed = true;

					if (bGoDelayed)
					{
						pEvt->m_EventFlags &= ~(_EVENT_FLAG_DEADLOCK_DETECTED | _EVENT_FLAG_RESCHEDULED);
						pEvDescr->m_bMiniKav = false; // unsafe

						PR_TRACE((g_root, prtIMPORTANT, "avpgs\tclose to dealyed'%S' - %s",
							pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>",
							_CONTEXT_OBJECT_FLAG_FORREAD & ContextFlags ? "smart" : "unsafe"));

						DRV_EVENT_DESCRIBE DrvEvDescrTmp;
						DrvEvDescrTmp.m_hDevice = pEvDescr->m_hDevice;
						DrvEvDescrTmp.m_AppID = pEvDescr->m_SetVerdict.m_AppID;
						DrvEvDescrTmp.m_Mark = pEvDescr->m_SetVerdict.m_Mark;
						DrvEvDescrTmp.m_EventFlags = 0;

						_HE_ImpersonateThread( &DrvEvDescrTmp );

						if (m_pfOpenThreadToken)
						{
							m_pfOpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &pEvDescr->m_hTokenImpersonate);
							m_pfSetThreadToken(NULL, NULL);
						}

						if (!(pEvt->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
						{
							IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 3);

							pEvt->m_EventFlags |= _EVENT_FLAG_DELETED_ON_MARK;
						}
					}
					else
					{
						// override eop for mini-filter (POPUP event)
						eop = _op_create;
					}
				}
			}
			
			switch (eop)
			{
			case _op_volume_verify:
				err = m_ThPoolConnect->AddTask(&tid, OnAcc_ConnectTaskCallback, &pEvDescr, sizeof(pEvDescr), 0);
				break;
			case _op_close:
				if (IsExistInPostPone(pEvDescr, false))
					err = errNOT_MATCHED;
				else
				{
					DWORD delaytime = 15;

					if (cAVPG_WORKING_MODE_ON_SMART == m_WorkingMode)
						delaytime = 10;

					PHashTreeItem pHashItemNew = NULL;
					if (AddToDelayedList(pEvDescr, m_PHashTree_Delayed, &pHashItemNew, delaytime, false))
					{
						PR_TRACE((g_root, prtIMPORTANT, "avpgs\t<close>  added to delayed %x from %.32s(%d) - '%S'",
							pHashItemNew,
							pEvt->m_ProcName, 
							pEvt->m_ProcessID, 
							pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));
						err = errOK;
					}
				}
				break;
			default:
				if (cAVPG_WORKING_MODE_ON_SMART == m_WorkingMode)
				{
					if (access_attr & FILE_WRITE_DATA || m_DrvFlags & _DRV_FLAG_MINIFILTER)
					{
						bFoundInDelayed = CheckDelayedList(pEvDescr, false);
						if (4 == pEvt->m_ProcessID || 8 == pEvt->m_ProcessID)
						{
							if (pLuid && IsUserRemote( pLuid ))
							{
								PR_TRACE((g_root, prtIMPORTANT, "avpgs\tsmart mode - network user, skip smart"));
								bFoundInDelayed = FALSE; // skip smart when network user
							}
						}
						
						if (bFoundInDelayed)
						{
							PR_TRACE((g_root, prtIMPORTANT, "avpgs\tsmart mode - found in delayed. op %d (real %d)",
								eop, eop_real));
							err = errNOT_MATCHED;
							break;
						}
					}
				}

				if (!bFoundInDelayed)
					CheckDelayedList(pEvDescr, true);
				
				if (IsExistInPostPone(pEvDescr, true))
				{
					/*PR_TRACE((g_root, prtNOTIFY, "avpgs\t<%s> exist in postpone, skip from %.32s(%d) - '%S'",
						eop == _op_create ? "create" :
							eop == _op_write ? "write" :
							"unknown",
						pEvt->m_ProcName, 
						pEvt->m_ProcessID, 
						pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));*/
					err = errNOT_MATCHED;
				}
				else
				{
					/*PR_TRACE((g_root, prtNOTIFY, "avpgs\t<%s> added scan task  from %.32s(%d) - '%S'", 
						eop == _op_create ? "create" :
							eop == _op_write ? "write" :
							"unknown",
						pEvt->m_ProcName, 
						pEvt->m_ProcessID, 
						pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));*/
					err = m_ThPoolWork->AddTask(&tid, OnAcc_WorkTaskCallback, &pEvDescr, sizeof(pEvDescr), 0);
				}
				break;
			}
		}

		if (PR_FAIL(err))
		{
			if (!(pEvt->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
				IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 30);

			m_DrvEventList.Free((BYTE*) pEvt);

			if (pEvDescr)
			{
				if (pEvDescr->m_hTokenImpersonate)
				{
					CloseHandle(pEvDescr->m_hTokenImpersonate);
					pEvDescr->m_hTokenImpersonate = NULL;
				}

				if (pEvDescr->m_PtrFN)
					_MM_Free(pEvDescr->m_PtrFN);

				_MM_Free(pEvDescr);
			}
		}
	}
}

void CJob::FreeEvDescr(PAVPGS_EVENT_DESCRIBE pEvDescr)
{
	if (pEvDescr->m_eop != _op_delayed)
	{
		if (pEvDescr->m_pEvt)
		{
			if (!(pEvDescr->m_pEvt->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
				IDriverSetVerdict2(pEvDescr->m_hDevice, &pEvDescr->m_SetVerdict);

			m_DrvEventList.Free((BYTE*) pEvDescr->m_pEvt);
		}
		else
			IDriverSetVerdict2(pEvDescr->m_hDevice, &pEvDescr->m_SetVerdict);
	}

	if (pEvDescr->m_hTokenImpersonate)
	{
		CloseHandle(pEvDescr->m_hTokenImpersonate);
		pEvDescr->m_hTokenImpersonate = NULL;
	}

	if (pEvDescr->m_PtrFN)
		_MM_Free(pEvDescr->m_PtrFN);

	_MM_Free(pEvDescr);
}

bool CJob::IsExistInPostPone(PAVPGS_EVENT_DESCRIBE pEvDescr, bool bStopPostpone)
{
	bool bFound = false;
	PSINGLE_PARAM pParam;

	bool bProcessing = false;

	m_EvSync.Lock();

	if (!m_PHashTree_PostPone || !m_PHashTree_PostPone->m_ItemsCount)
	{
		m_EvSync.UnLock();
		return false;
	}

	pParam = IDriverGetEventParam(pEvDescr->m_pEvt, _PARAM_OBJECT_URL_W);
	if (!pParam)
		pParam = IDriverGetEventParam(pEvDescr->m_pEvt, _PARAM_OBJECT_URL);
	
	if ((pParam != NULL) && (pParam->ParamSize != 0))
	{
		PHashTreeItem pHashItem = _Hash_Find(m_PHashTree_PostPone, pParam->ParamValue, pParam->ParamSize);
		if (pHashItem != 0)
		{
			bFound = true;
			if (bStopPostpone)
			{
				pEvDescr = (PAVPGS_EVENT_DESCRIBE) pHashItem->m_pUserData;
				if (pEvDescr && pEvDescr->m_bProcessing)
				{
					pEvDescr->m_bBreakProcessing = true;
					bProcessing = true;

					QueryPerformanceCounter((LARGE_INTEGER*) &pEvDescr->m_Timeout);
					pEvDescr->m_Timeout += 30 * m_PerfomanceFreq;
				}
			}
		}
	}
	
	m_EvSync.UnLock();

	if (bProcessing)
	{
		// wait, while processing stop
		Sleep(200);
	}

	return bFound;
}

bool CJob::CheckDelayedList(PAVPGS_EVENT_DESCRIBE pEvDescr, bool bFreeItem)
{
	bool bRet = false;
	PSINGLE_PARAM pParam;

	m_EvSync.Lock();

	if (!m_PHashTree_Delayed)
	{
		m_EvSync.UnLock();
		return false;
	}
	
	if (!m_PHashTree_Delayed->m_ItemsCount)
	{
		m_EvSync.UnLock();
		return false;
	}

	pParam = IDriverGetEventParam(pEvDescr->m_pEvt, _PARAM_OBJECT_URL_W);
	if (!pParam)
		pParam = IDriverGetEventParam(pEvDescr->m_pEvt, _PARAM_OBJECT_URL);
	
	if ((pParam != NULL) && (pParam->ParamSize != 0))
	{
		PHashTreeItem pHashItem = _Hash_Find(m_PHashTree_Delayed, pParam->ParamValue, pParam->ParamSize);
		if (pHashItem != 0)
		{
			PAVPGS_EVENT_DESCRIBE pEvDescrTmp = (PAVPGS_EVENT_DESCRIBE) pHashItem->m_pUserData;

			bRet = true;
			if (bFreeItem)
			{
				memcpy(&pEvDescr->m_CreatorLuid, &pEvDescrTmp->m_CreatorLuid, sizeof(LUID));

				_Hash_DelItem(m_PHashTree_Delayed, pHashItem, 0);
				FreeEvDescr(pEvDescrTmp);
			}
			else
			{
				if (pEvDescrTmp->m_pEvt)
				{
					if (pEvDescrTmp->m_pEvt->m_ProcessID != pEvDescr->m_pEvt->m_ProcessID)
					{
						PR_TRACE((g_root, prtNOTIFY, "avpgs\tfound in delayed stored pid %d, access pid %d - not matched process!", 
							pEvDescrTmp->m_pEvt->m_ProcessID,
							pEvDescr->m_pEvt->m_ProcessID));

						bRet = false;
					}
				}
			}
		}
	}
	
	m_EvSync.UnLock();
	
	return bRet;
}

bool
CJob::AddToDelayedList (
	PAVPGS_EVENT_DESCRIBE pEvDescr,
	PHashTree pTree,
	PHashTreeItem* ppHashItemNew,
	DWORD dwTimeShift,
	bool bIsPostPone)
{
	bool bRet = false;
	PSINGLE_PARAM pParam;
	PSINGLE_PARAM pParamW = NULL;
	
	QueryPerformanceCounter((LARGE_INTEGER*) &pEvDescr->m_Timeout);
	pEvDescr->m_Timeout += dwTimeShift * m_PerfomanceFreq;

	m_EvSync.Lock();	
	if (!pTree || pTree->m_ItemsCount > 2048)
	{
    	m_EvSync.UnLock();
		return bRet;
	}

	pParam = IDriverGetEventParam(pEvDescr->m_pEvt, _PARAM_OBJECT_URL_W);
	if (pParam == NULL)
		pParam = IDriverGetEventParam(pEvDescr->m_pEvt, _PARAM_OBJECT_URL);
	else
		pParamW = pParam;

	if ((!pParam) || (!pParam->ParamSize))
	{
		m_EvSync.UnLock();
		return bRet;
	}

	PHashTreeItem pHashItem = _Hash_Find(pTree, pParam->ParamValue, pParam->ParamSize);
	if (!pHashItem)
	{
		pHashItem = _Hash_AddItem(pTree, pParam->ParamValue, pParam->ParamSize, pEvDescr);
    	if (pHashItem)
		{
			bRet = true;
			*ppHashItemNew = pHashItem;
		}
	}
	
	m_EvSync.UnLock();

	if (m_bMultiProcessMode && !bIsPostPone && bRet && pParamW)
	{
		if (pEvDescr->m_CreatorLuid.HighPart || pEvDescr->m_CreatorLuid.LowPart)
		{
			cAvpgsDelayedNotify notify;
			notify.m_Luid = pEvDescr->m_CreatorLuid.LowPart;
			notify.m_LuidHP = pEvDescr->m_CreatorLuid.HighPart;

			if (PR_SUCC(notify.m_FileName.resize(pParamW->ParamSize)))
			{
				memcpy(notify.m_FileName.data(), pParamW->ParamValue, pParamW->ParamSize);

				PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tsend notify task (pid %d) - '%S'", 
					GetCurrentProcessId(), pParamW->ParamValue));

				m_hTask->sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_OBJCRINFO, m_hTask, &notify, SER_SENDMSG_PSIZE);
			}
		}
	}

	return bRet;
}

//+ ------------------------------------------------------------------------------------------

void CJob::DriveConnect_Message(PEVENT_TRANSMIT pEvt)
{
	tERROR error;
	hSTRING hStringTmp = NULL;

	tORIG_ID Origin = OID_AVP3_BOOT;

	memset(m_DrivesType, 0, sizeof(m_DrivesType));

	PSINGLE_PARAM pSingleObjectName;

	PSINGLE_PARAM pSingleVolumeName;
	pSingleVolumeName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_VOLUME_NAME_W);
	if (pSingleVolumeName)
	{
		error = CALL_SYS_ObjectCreateQuick(m_hTask, (hOBJECT*) &hStringTmp, IID_STRING, PID_ANY, 0);
		if (PR_FAIL(error))
			return;

		cStrObj strtmp;
		error = strtmp.assign(pSingleVolumeName->ParamValue, cCP_UNICODE, pSingleVolumeName->ParamSize);

		WCHAR dosname[4];
		if (S_OK != m_FilterGetDosName((LPCWSTR) pSingleVolumeName->ParamValue, dosname, sizeof(dosname) / sizeof(WCHAR)))
			memset(dosname, 0, sizeof(dosname));
		else
		{
			dosname[2] = L'\\';
			dosname[3] = 0;
		}
		
		CALL_String_SetCP(hStringTmp, cCP_UNICODE);
		hStringTmp->ImportFromBuff(NULL, const_cast <tWCHAR*> ( strtmp.data() ), strtmp.memsize (cCP_UNICODE), cCP_UNICODE, cSTRING_WHOLE);

		strtmp.assign(dosname, cCP_UNICODE, sizeof(dosname));

		if (dosname[0])
			CALL_SYS_PropertySet(hStringTmp, 0, propid_DosDeviceName, (LPSTR) strtmp.c_str(cCP_ANSI), strtmp.length());
		else
			CALL_SYS_PropertyDelete(hStringTmp, propid_DosDeviceName);

		error = CALL_SYS_PropertySet(hStringTmp, 0, propid_CustomOrigin, &Origin, sizeof(Origin));
		if (PR_SUCC(error))
			error = m_hTask->sysSendMsg(pmc_AVPG, pm_AVPG_PROCEED_OBJECT, hStringTmp, NULL, NULL);

		CALL_SYS_ObjectClose(hStringTmp);
		return;
	}


	pSingleObjectName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);
	if (pSingleObjectName == NULL)
		pSingleObjectName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL);
		
	if (pSingleObjectName == NULL)
		return;

	error = CALL_SYS_ObjectCreateQuick(m_hTask, (hOBJECT*) &hStringTmp, IID_STRING, PID_ANY, 0);
	if (PR_FAIL(error))
		return;

	if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		CALL_String_SetCP(hStringTmp, cCP_UNICODE);
	else
		CALL_String_SetCP(hStringTmp, cCP_ANSI);

	PCHAR pObjName = (PCHAR) pSingleObjectName->ParamValue;

	char Name[4];
	BOOL bDosName = FALSE;

	ZeroMemory(Name, sizeof(Name));
	Name[1] = ':'; Name[2] = '\\';

	if (pSingleObjectName->ParamID == _PARAM_OBJECT_URL_W)
	{
		char ansidrive[MAX_PATH];
		if(WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) pObjName, -1, ansidrive, sizeof(ansidrive), NULL, NULL))
			bDosName = __GetDosDeviceName(ansidrive, &(Name[0]));
	}
	else
		bDosName = __GetDosDeviceName(pObjName, &(Name[0]));
	
	if (bDosName)
		CALL_SYS_PropertySet(hStringTmp, 0, propid_DosDeviceName, Name, sizeof(Name));
	else
		CALL_SYS_PropertyDelete(hStringTmp, propid_DosDeviceName);


	error = CALL_SYS_PropertySet(hStringTmp, 0, propid_CustomOrigin, &Origin, sizeof(Origin));
	if (PR_SUCC(error))
	{
		PR_TRACE((g_root, prtIMPORTANT, "avpgs\tverify volume %s", Name));
		error = m_hTask->sysSendMsg(pmc_AVPG, pm_AVPG_PROCEED_OBJECT, hStringTmp, NULL, NULL);
	}

	CALL_SYS_ObjectClose(hStringTmp);
}

tERROR CJob::FillObjectName(cString* hctx, PEVENT_TRANSMIT pEvt, PSINGLE_PARAM pSingleObjectName, ULONG ContextFlags)
{
	tERROR error = errUNEXPECTED;
	PSINGLE_PARAM pSingleVolumeName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_VOLUME_NAME_W);
	PSINGLE_PARAM pSingleNetShareName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_NETSHARE_W);

	cStrObj strtmp;
	error = strtmp.assign(pSingleObjectName->ParamValue, cCP_UNICODE, pSingleObjectName->ParamSize);

	if (PR_SUCC(error) && pSingleVolumeName && pSingleVolumeName->ParamSize)
	{
		WCHAR dosname[4];
		if (S_OK == m_FilterGetDosName((LPCWSTR) pSingleVolumeName->ParamValue, dosname, sizeof(dosname) / sizeof(WCHAR)))
			strtmp.replace_one((LPCWSTR) pSingleVolumeName->ParamValue, fSTRING_COMPARE_CASE_INSENSITIVE, dosname);
	}

	if (ContextFlags & _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE)
	{
		strtmp.replace_one(L"\\Device\\LanmanRedirector", fSTRING_COMPARE_CASE_INSENSITIVE, L"\\");
		strtmp.replace_one(L"\\Device\\Mup", fSTRING_COMPARE_CASE_INSENSITIVE, L"\\");
	}

	if (PR_SUCC(error))
		error = hctx->ImportFromBuff(NULL, const_cast <tWCHAR*> ( strtmp.data() ), strtmp.memsize (cCP_UNICODE), cCP_UNICODE, cSTRING_WHOLE);

	return error;
}

tERROR
CJob::SetObjectContext (
	PAVPGS_EVENT_DESCRIBE pEvDescr,
	PDRV_EVENT_DESCRIBE pDrvEvDescr,
	cString* hctx,
	PSINGLE_PARAM* ppSingleObjectName
	)
{
	tERROR error = errOK;

	PEVENT_TRANSMIT pEvt = pEvDescr->m_pEvt;

	if ((pEvt->m_EventFlags & _EVENT_FLAG_DEADLOCK_DETECTED) || (pEvt->m_EventFlags & _EVENT_FLAG_RESCHEDULED))  // deadlock or reschedule 
		return errOPERATION_CANCELED;

	PSINGLE_PARAM pSingleObjectName;

	ULONG Flags = 0;

	pSingleObjectName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);
	if (!pSingleObjectName)
		pSingleObjectName = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL);

	if (!pSingleObjectName)
		return errOPERATION_CANCELED;

	if (!pSingleObjectName->ParamSize)
		return errOPERATION_CANCELED;

	/*if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (pEvDescr->m_eop == _op_close)
		{
			PSINGLE_PARAM pSingleObjectRefCount = IDriverGetEventParam(pEvt, _PARAM_OBJECT_REFCOUNT);
			if (pSingleObjectRefCount && pSingleObjectRefCount->ParamSize == sizeof(ULONG))
			{
				ULONG RefCount = *(ULONG*) pSingleObjectRefCount->ParamValue;
				if (RefCount > 2)
				{
					PR_TRACE((g_root, prtIMPORTANT, "avpgs\trefcount %d '%S'", RefCount, pSingleObjectName->ParamValue));
				}
			}
		}
	}*/

	tCODEPAGE cp;
	tDWORD imflg;
	if (pSingleObjectName->ParamID == _PARAM_OBJECT_URL_W)
	{
		cp = cCP_UNICODE;
		imflg = cSTRING_CONTENT_ONLY;
	}
	else
	{
		cp = cCP_ANSI;
		imflg = cSTRING_Z;
	}

	*ppSingleObjectName = pSingleObjectName;

	pDrvEvDescr->m_hDevice = pEvDescr->m_hDevice;
	pDrvEvDescr->m_AppID = pEvDescr->m_SetVerdict.m_AppID;
	pDrvEvDescr->m_Mark = pEvDescr->m_SetVerdict.m_Mark;
	pDrvEvDescr->m_EventFlags = pEvDescr->m_pEvt->m_EventFlags;

	hctx->propSetDWord(propid_EventDefinition, (DWORD) pDrvEvDescr);
	hctx->propSetDWord(propid_ThreadDefinition, (DWORD) pEvDescr);
	hctx->propSetDWord(propid_EventProcessID, (DWORD) pEvt->m_ProcessID);
	hctx->propSet(0, propid_ProcessDefinition, pEvt->m_ProcName, lstrlen(pEvt->m_ProcName) + 1);

	DWORD substrlen;
	substrlen = sizeof(L"\\\\TSCLIENT\\SCARD");					
	if (pSingleObjectName->ParamSize == substrlen)
	{
		if (!memcmp(pSingleObjectName->ParamValue, L"\\\\TSCLIENT\\SCARD", substrlen - sizeof(WCHAR)))
			return errOPERATION_CANCELED;
	}


	tBOOL bStreamed = cFALSE;
	PSINGLE_PARAM pSingleContextFlags = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CONTEXT_FLAGS);
	if (pSingleContextFlags)
	{
		Flags = *(ULONG*) pSingleContextFlags->ParamValue;

//		if (Flags & _CONTEXT_OBJECT_FLAG_DELETEPENDING)
//			return errOPERATION_CANCELED;

		/*if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
		{
			if (Flags & _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT)
				return errOPERATION_CANCELED;
		}*/
	
		if (Flags & _CONTEXT_OBJECT_FLAG_NETWORKTREE)
			return errOPERATION_CANCELED;
		
		if (Flags & _CONTEXT_OBJECT_FLAG_DIRECTORY)
			return errOPERATION_CANCELED;

		if (Flags & _CONTEXT_OBJECT_FLAG_NTFS)
		{
			bStreamed = cTRUE;
	
			if (!(m_DrvFlags & _DRV_FLAG_MINIFILTER))
			{
				if (pSingleObjectName)
				{
					substrlen = sizeof(L"\\$Extend\\$ObjId") - sizeof(WCHAR);
					DWORD pos = GetSubstringPosBM(pSingleObjectName->ParamValue, pSingleObjectName->ParamSize, (BYTE*) L"\\$Extend\\$ObjId", substrlen);
					if (pos != -1)
						return errOPERATION_CANCELED;

					substrlen = sizeof(L"\\$MFT") - sizeof(WCHAR);
					pos = GetSubstringPosBM(pSingleObjectName->ParamValue, pSingleObjectName->ParamSize, (BYTE*) L"\\$MFT", substrlen);
					if (pos != -1)
						return errOPERATION_CANCELED;
				}
			}				
		}

		bool bModified = false;

		if (_CONTEXT_OBJECT_FLAG_MODIFIED & Flags)
		{
			if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
				bModified = true;
			else
			{
				if (_op_close == pEvDescr->m_eop)
					bModified = true;
			}
		}

		if (bModified)
			hctx->propSetDWord(propid_ObjectModified, cTRUE);
	}

	hctx->propSetBool(propid_Streamed, bStreamed);
	
	if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
	{
		hctx->propSetBool(propid_Delayed, cFALSE);
		if (!pEvDescr->m_bMiniKav)
		{
			if (_op_execute != pEvDescr->m_eop)
			{
				PR_TRACE((g_root, prtIMPORTANT, "avpgs\tminifilter delayed processing"));
				hctx->propSetBool(propid_Delayed, cTRUE);
			}
		}
	}
	else
	{
		if ((_op_delayed == pEvDescr->m_eop) || (_op_close == pEvDescr->m_eop))
			hctx->propSetBool(propid_Delayed, cTRUE);
		else
			hctx->propSetBool(propid_Delayed, cFALSE);
	}

	if (m_DrvFlags & _DRV_FLAG_MINIFILTER)
		error = FillObjectName(hctx, pEvt, pSingleObjectName, Flags);
	else
		error = hctx->ImportFromBuff(NULL, pSingleObjectName->ParamValue, pSingleObjectName->ParamSize, cp, cSTRING_CONTENT_ONLY);
	
	return error;
}

bool CJob::IgnoreProcess_IsSkipThis(ULONG ProcessId)
{
	bool bFound = false;

	if (m_OsInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
		return false;

	m_EvSync.Lock();

	for (_tIgnorePidList::iterator _it = m_IgnorePidList.begin(); _it != m_IgnorePidList.end(); _it++)
	{
		IGNORE_PROC_LIST& ig = *_it;
		if (ig.m_ProcessId == ProcessId)
		{
			bFound = ig.m_bIgnoreTis;
			m_EvSync.UnLock();
			
			return bFound;
		}
	}

	m_EvSync.UnLock();

	cPRCMRequest req;
	tDWORD blen = sizeof(cPRCMRequest);
	
	req.m_Request = cPRCMRequest::_ePrcmn_ExclOpenFiles;
	req.m_ProcessId = ProcessId;

	tERROR error = m_hTask->sysSendMsg(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_GETACTIONMASK, m_hTask, &req, &blen);

	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tGetMaskResult for pid %d - %terr", ProcessId, error));

	IGNORE_PROC_LIST ignew;
	ignew.m_ProcessId = ProcessId;
	ignew.m_bIgnoreTis = false;

	if (error == errOK_DECIDED)
		ignew.m_bIgnoreTis = true;
	
	m_EvSync.Lock();
	m_IgnorePidList.push_back(ignew);
	m_EvSync.UnLock();

	return ignew.m_bIgnoreTis;
}

void CJob::IgnoreProcess_ResetList()
{
	m_EvSync.Lock();

	m_IgnorePidList.clear();

	m_EvSync.UnLock();
}

void CJob::ProcessEvent(PAVPGS_EVENT_DESCRIBE pEvDescr)
{
	if (_op_other == pEvDescr->m_eop)
		return;

	DRV_EVENT_DESCRIBE DrvEvDescr;
	
	PEVENT_TRANSMIT pEvt = pEvDescr->m_pEvt;
	
	//+ ------------------------------------------------------------------------------------------
	cString* hString = NULL;
	tERROR error = m_hTask->sysCreateObjectQuick((hOBJECT*) &hString, IID_STRING, PID_ANY, 0);
	if (PR_FAIL(error))
		hString = NULL;
	else
	{
		if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
			hString->SetCP(cCP_UNICODE);
		else
			hString->SetCP(cCP_ANSI);
	}

	cUserInfo UserInfo;
	cUserInfo UserBanInfo;
	if (hString)
	{
		PSINGLE_PARAM pSingleObjectName = NULL;
		error = SetObjectContext(pEvDescr, &DrvEvDescr, hString, &pSingleObjectName);
		
		if (PR_FAIL(error))
		{
			// skip
			if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
			{
				PR_TRACE((g_root, prtIMPORTANT, "avpgs\tskipped processing of scan task with %terr from %.32s(%d) - '%S'", 
					error,
					pEvt->m_ProcName, 
					pEvt->m_ProcessID, 
					pSingleObjectName ? (LPCWSTR) pSingleObjectName->ParamValue : L"<unknown>"));
			}
		}
		else
		{
			if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
			{
				PR_TRACE((g_root, prtIMPORTANT, "avpgs\t(pid %d) start processing scan task from %.32s(%d) - '%S' - op %d",
					GetCurrentProcessId(),
					pEvt->m_ProcName, 
					pEvt->m_ProcessID, 
					pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>",
					pEvDescr->m_eop));
			}
			else
			{
				PR_TRACE((g_root, prtNOTIFY, "avpgs\tstart processing scan task from %.32s(%d) - '%s' (%d-%d-%d)", 
					pEvt->m_ProcName, 
					pEvt->m_ProcessID, 
					pSingleObjectName ? (LPCSTR) pSingleObjectName->ParamValue : "<unknown>",
					pEvDescr->m_pEvt->m_HookID, pEvDescr->m_pEvt->m_FunctionMj, pEvDescr->m_pEvt->m_FunctionMi));
			}
			
			TlsSetValue(g_dwTlsIndexEventDefinition, &DrvEvDescr);

			tDWORD dwAvpgVerdict = cAVPG_RC_ALLOW_CACHE;

			if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
			{
				if (pEvDescr->m_hTokenImpersonate)
					m_pfSetThreadToken(NULL, pEvDescr->m_hTokenImpersonate);
				else
					_HE_ImpersonateThread(&DrvEvDescr);
				
				PSINGLE_PARAM pSingleSid = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SID);
				if (pSingleSid && pSingleSid->ParamSize)
				{
					if (PR_SUCC(UserInfo.m_Sid.resize(pSingleSid->ParamSize)))
					{
						memcpy(UserInfo.m_Sid.data(), pSingleSid->ParamValue, pSingleSid->ParamSize);
						
						SID* pSid = (SID*)UserInfo.m_Sid.data();
						PrintUserName(pSid, "sid filled, user:");
					}
				}

				PLUID pCreatorLuid = NULL;
				PLUID pUserOverrideLuid = NULL;
				
				if (pEvDescr->m_CreatorLuid.HighPart || pEvDescr->m_CreatorLuid.LowPart)
					pCreatorLuid = &pEvDescr->m_CreatorLuid;

				if (pEvDescr->m_UserOverrideLuid.HighPart || pEvDescr->m_UserOverrideLuid.LowPart)
					pUserOverrideLuid = &pEvDescr->m_UserOverrideLuid;

				if (SetUserAndBanInfo(hString, &UserInfo, &UserBanInfo, pCreatorLuid, pUserOverrideLuid))
				{
					if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
					{
						PWSTR pwsFN = (PWSTR) pSingleObjectName->ParamValue;

						PR_TRACE((g_root, prtNOTIFY, "avpgs\t'%S' access luid 0x%x-0x%0x, creator 0x%x-0x%0x (op %d)",
							pwsFN,
							UserInfo.m_LuidHP, UserInfo.m_Luid,
							UserBanInfo.m_LuidHP, UserBanInfo.m_Luid,
							pEvDescr->m_eop));
					}
					
					if (_op_create == pEvDescr->m_eop)
					{
						if (IsUserBaned(&UserInfo))
							dwAvpgVerdict = cAVPG_RC_DENY_ACCESS;
					}
				}
			}

			if (!(pEvt->m_EventFlags & FLT_A_INFO))
			{
				if (_op_delayed != pEvDescr->m_eop)
					IDriverYieldEvent(pEvDescr->m_hDevice, pEvDescr->m_SetVerdict.m_AppID, pEvDescr->m_SetVerdict.m_Mark, 3);
			}

			if (cAVPG_RC_ALLOW_CACHE == dwAvpgVerdict)
			{
				tDWORD blen = sizeof(dwAvpgVerdict);
				error = m_hTask->sysSendMsg(pmc_AVPG, pm_AVPG_PROCEED_OBJECT, hString, &dwAvpgVerdict, &blen);
			}
			else if (cAVPG_RC_DENY_ACCESS == dwAvpgVerdict)
			{
				PR_TRACE((g_root, prtIMPORTANT, "avpgs\tbanned user - deny in op"));
			}

			if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
			{
				PR_TRACE((g_root, prtNOTIFY, "avpgs\tVerdict %d for '%S'", 
					dwAvpgVerdict,
					pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));
			}
			
			bool bNeedDelete = false;

			switch (dwAvpgVerdict)
			{
			case cAVPG_RC_ALLOW_CACHE:
				{
					bool bNetworkFile = false;
					pEvDescr->m_SetVerdict.m_Verdict = Verdict_Pass;

					PSINGLE_PARAM pSingleParamFlag = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CONTEXT_FLAGS);
					ULONG flgs = 0;

					if (pSingleParamFlag)
						flgs = *(ULONG*)pSingleParamFlag->ParamValue;

					if (VER_PLATFORM_WIN32_NT == m_OsInfo.dwPlatformId)
					{
						if (flgs & _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE)
							bNetworkFile = true;
						else if ((((LPCWSTR)pSingleObjectName->ParamValue)[1] == '\\'))
							bNetworkFile = true;
					}
					else
					{
						if ((((char*)pSingleObjectName->ParamValue)[1] == '\\'))
							bNetworkFile = true;
						else
						{
							char drvletter[4];
							drvletter[0] = ((char*)pSingleObjectName->ParamValue)[0];
							drvletter[0] |= 0x20;
							
							if (flgs & _CONTEXT_OBJECT_FLAG_SRCREMOTE)
							{
								if (drvletter[0] >= 'a' && drvletter[0] <= 'z')
									m_DrivesType[drvletter[0] - 'a'] = DRIVE_REMOTE;
								bNetworkFile = true;
							}
							else
							{
								if (drvletter[0] >= 'a' && drvletter[0] <= 'z')
								{
									drvletter[1] = ':'; drvletter[2] = '\\'; drvletter[3] = 0;
									
									if (!m_DrivesType[drvletter[0] - 'a'])
										m_DrivesType[drvletter[0] - 'a'] = GetDriveType(drvletter);
									
									if (m_DrivesType[drvletter[0] - 'a'] == DRIVE_REMOTE)
										bNetworkFile = true;
								}
							}
						}
					}

					if (bNetworkFile)
						pEvDescr->m_SetVerdict.m_ExpTime = 2000;
				}
				break;

			case cAVPG_RC_ALLOW:
				pEvDescr->m_SetVerdict.m_Verdict = Verdict_Default;
				break;

			case cAVPG_RC_DENY_ACCESS:
				pEvDescr->m_SetVerdict.m_Verdict = Verdict_Discard;
				pEvDescr->m_SetVerdict.m_ExpTime = 2000;		//2 sec
				break;

			case cAVPG_RC_DELETE_OBJECT:
				pEvDescr->m_SetVerdict.m_Verdict = Verdict_Discard;
				bNeedDelete = TRUE;
				pEvDescr->m_SetVerdict.m_ExpTime = 2000;		//2 sec
				break;

			case cAVPG_RC_KILL_PROCESS:
				pEvDescr->m_SetVerdict.m_Verdict = Verdict_Kill;
				break;
				
			case cAVPG_RC_GO_DELAYED:
				pEvDescr->m_SetVerdict.m_Verdict = Verdict_Default;
				{
					PAVPGS_EVENT_DESCRIBE pEvDescrTmp = (PAVPGS_EVENT_DESCRIBE) _MM_Alloc(sizeof(AVPGS_EVENT_DESCRIBE));
					if (pEvDescrTmp)
					{
						memcpy(pEvDescrTmp, pEvDescr, sizeof(AVPGS_EVENT_DESCRIBE));
						pEvDescr->m_pEvt = NULL; // will free in processing pEvDescrTmp
						pEvDescrTmp->m_eop = _op_delayed;
						pEvDescrTmp->m_SetVerdict.m_Verdict = Verdict_Default;
						
						if (!pEvDescrTmp->m_hTokenImpersonate)
						{
							if (m_pfOpenThreadToken)
								m_pfOpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &pEvDescrTmp->m_hTokenImpersonate);
						}

						PHashTreeItem pHashItemNew = NULL;

						if (!AddToDelayedList(pEvDescrTmp, m_PHashTree_PostPone, &pHashItemNew, 20, true))
						{
							// can't add - free memory
							FreeEvDescr(pEvDescrTmp);
						}
						else
						{
							if (m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
							{
								PR_TRACE((g_root, prtIMPORTANT, "avpgs\tpostpone add %x from %.32s(%d) - '%S'", 
									pHashItemNew,
									pEvt->m_ProcName, 
									pEvt->m_ProcessID, 
									pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));
							}
						}
					}
				}
				break;
			default:
				break;
			}	// end switch dwAvpgVerdict

			if (bNeedDelete)
			{
				if (pSingleObjectName->ParamID == _PARAM_OBJECT_URL_W)
					DeleteFileW((PWCHAR) pSingleObjectName->ParamValue);
				else
					DeleteFile((char*) pSingleObjectName->ParamValue);

			}
		
			/*PR_TRACE((g_root, prtNOTIFY, "avpgs\tdone processing scan task - '%S'", 
				pSingleObjectName ? (LPCWSTR)pSingleObjectName->ParamValue : L"<unknown>"));*/
		}
		
		hString->sysCloseObject();
	}

	//+ ------------------------------------------------------------------------------------------

	TlsSetValue(g_dwTlsIndexEventDefinition, 0);

	if (m_pfSetThreadToken)
		m_pfSetThreadToken(NULL, NULL);
}

void CJob::SkipEvents()
{
	Sleep(250);
}

void CJob::ProcessDelayedEvent(bool bForce)
{
	PAVPGS_EVENT_DESCRIBE pEvDescr = (PAVPGS_EVENT_DESCRIBE) 1;
	
	__int64 PerfCounter;
	while (pEvDescr)
	{
		PHashTreeItem pHashItem;
		PHashTreeItem pHashItemPostPone = NULL;
		
		pEvDescr = 0;
		QueryPerformanceCounter((LARGE_INTEGER*) &PerfCounter);

		m_EvSync.Lock();
		//if (m_PHashTree_PostPone->m_ItemsCount || m_PHashTree_Delayed->m_ItemsCount)
		//	PR_TRACE((g_root, prtNOTIFY, "avpgs\tdelayed queues: postpone=%d, delayed=%d", m_PHashTree_PostPone->m_ItemsCount, m_PHashTree_Delayed->m_ItemsCount));
		if (!bForce)
		{
			//+ ------------------------------------------------------------------------------------------
			// postpone
			pHashItem = m_PHashTree_PostPone->m_pFirst;
			while (pHashItem)
			{
				pEvDescr = (PAVPGS_EVENT_DESCRIBE) pHashItem->m_pUserData;

				if (pEvDescr)
				{
					if ((!pEvDescr->m_Timeout) || (pEvDescr->m_Timeout < PerfCounter))
					{
						if (pEvDescr->m_bProcessing)
						{
							pEvDescr = 0; // go to delayed list
							break;
						}

						pHashItemPostPone = pHashItem;
						pEvDescr->m_bProcessing = true;
						PR_TRACE((g_root, prtIMPORTANT, "avpgs\tpostpone go %x", pHashItemPostPone));
						break;
					}
					else
						pEvDescr = 0;
				}

				pHashItem = pHashItem->m_pCommonNext;
			}

		//+ ------------------------------------------------------------------------------------------
		}
		
		if (!pEvDescr && m_PHashTree_Delayed)
		{
			int couskip = 0;
			// single delayed object
			pHashItem = m_PHashTree_Delayed->m_pFirst;
			while (pHashItem)
			{
				pEvDescr = (PAVPGS_EVENT_DESCRIBE) pHashItem->m_pUserData;

				if (pEvDescr)
				{
					if (m_bIsSkipEvents
						|| (!pEvDescr->m_Timeout)
						|| (pEvDescr->m_Timeout < PerfCounter)
						|| bForce)
					{
						PR_TRACE((g_root, prtIMPORTANT, "avpgs\tdelayed go %x (skip %d, force %d)", 
							pHashItem, m_bIsSkipEvents, bForce));
						_Hash_DelItem(m_PHashTree_Delayed, pHashItem, 0);
						break;
					}
					else
					{
						pEvDescr = 0;

						// так как таймаут один то можно очеред не мотать дальше
						couskip++;
						if (couskip > 8)
							break;
					}
				}

				pHashItem = pHashItem->m_pCommonNext;
			}
		}
//+ ------------------------------------------------------------------------------------------
		m_EvSync.UnLock();

		if (pEvDescr)
		{
			if (!m_bIsSkipEvents)
			{
				//PR_TRACE((g_root, prtIMPORTANT, "avpgs\t%s processing start %x", (pHashItemPostPone ? "postpone" : "delayed"), pHashItem));
				pEvDescr->m_bMiniKav = false;

				ProcessEvent(pEvDescr);
				//PR_TRACE((g_root, prtIMPORTANT, "avpgs\t%s processing done %x", (pHashItemPostPone ? "postpone" : "delayed"), pHashItem));
			}
			
			if (pHashItemPostPone)
			{
				m_EvSync.Lock();

				pEvDescr = (PAVPGS_EVENT_DESCRIBE) pHashItemPostPone->m_pUserData;

				if (m_bIsSkipEvents)
					pEvDescr->m_bBreakProcessing = false; // dont restart check this file

				if (pEvDescr->m_bBreakProcessing)
				{
					pEvDescr->m_bBreakProcessing = false;
					pEvDescr->m_bProcessing = false;
					pEvDescr = NULL; // leave
				}
				else
				{
					//PR_TRACE((g_root, prtIMPORTANT, "avpgs\tpostpone del %x", pHashItemPostPone));
					_Hash_DelItem(m_PHashTree_PostPone, pHashItemPostPone, 0);
				}

				m_EvSync.UnLock();
			}
			
			if (pEvDescr)
				FreeEvDescr(pEvDescr);
		}
	}
}

void CJob::ForceDelayedEvent()
{
	if (m_OsInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
		return;

	PR_TRACE((g_root, prtERROR, "avpgs\tForceDelayedEvent! (bSkip %d)", m_bIsSkipEvents));

	SendMessage(pmc_AVPG, pm_AVPG_NOTIFY_THREAD_STARTED, NULL, NULL);

	for (int cou = 0; cou < 128; cou++)
	{
		ProcessDelayedEvent(true);
	}

	SendMessage(pmc_AVPG, pm_AVPG_NOTIFY_THREAD_STOPPED, NULL, NULL);
}

bool CJob::FillLuidInfo(cUserInfo* pUserInfo, bool bTryToResolve)
{
	bool bRet = false;
	HANDLE hToken = NULL;
	if( !m_pfOpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_QUERY_SOURCE, TRUE, &hToken) || !hToken )
		return false;

	BYTE pInfo[1000]; tULONG nSize;
	if( !GetTokenInformation(hToken, TokenStatistics, pInfo, sizeof(pInfo), &nSize) )
	{
		CloseHandle(hToken);
		return false;
	}

	PLUID pLuid = &((TOKEN_STATISTICS*)pInfo)->AuthenticationId;

	bRet = FillUserInfoByLuid(pLuid, pUserInfo, bTryToResolve);

	CloseHandle(hToken);

	return bRet;
}

bool CJob::FillUserInfoByLuid(PLUID pLuid, cUserInfo* pUserInfo, bool bTryToResolve)
{
	pUserInfo->m_Luid = pLuid->LowPart;
	pUserInfo->m_LuidHP = pLuid->HighPart;

	if (FindExistingLuidInfo(pUserInfo))
		return true;

	if (!bTryToResolve)
		return false;

	PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tresolve luid 0x%x-0x%x", pUserInfo->m_LuidHP, pUserInfo->m_Luid));

	bool bResolved = false;
	
	pUserInfo->m_LocalUser = cFALSE;

	if(m_fnLsaGetLogonSessionData)
	{
		NTSTATUS ntStatus;
		PSECURITY_LOGON_SESSION_DATA sessionData = NULL;

		__try
		{
			ntStatus = m_fnLsaGetLogonSessionData(pLuid, &sessionData);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			ntStatus = -1;
		}

		if (ntStatus)
		{
			sessionData = NULL;
			PR_TRACE((g_root, prtIMPORTANT, "avpgs\tget session data failed. status 0x%x", ntStatus));
		}

		if( sessionData && sessionData->LogonDomain.Buffer )
		{
			pUserInfo->m_sUserName = sessionData->LogonDomain.Buffer;
			pUserInfo->m_sUserName += L"\\";
		}

		if (sessionData)
		{
			PR_TRACE((g_root, prtIMPORTANT, "avpgs\tresolve luid 0x%x-0x%x - type 0x%x", 
				pUserInfo->m_LuidHP, 
				pUserInfo->m_Luid,
				sessionData->LogonType));
			
			if (sessionData->UserName.Buffer)
				pUserInfo->m_sUserName += sessionData->UserName.Buffer;

			switch (sessionData->LogonType)
			{
			case Network:
				if(m_pfNetSessionEnum && sessionData->UserName.Buffer)
				{
					DWORD i, dwEntriesRead = 0, dwTotalEntries = 0, dwResumeHandle = 0;
					LPSESSION_INFO_10 pBuf = NULL, pTmpBuf;

					m_pfNetSessionEnum(NULL, NULL, sessionData->UserName.Buffer, 10,
						(LPBYTE*)&pBuf, -1, &dwEntriesRead, &dwTotalEntries, &dwResumeHandle);

					PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tsessions count %d", dwTotalEntries));
					if( !dwEntriesRead)
					{
					}
					else if( dwEntriesRead == 1 )
					{
						pUserInfo->m_sMachineName = (LPWSTR)pBuf->sesi10_cname;
						bResolved = true;
					}
					else if( dwEntriesRead > 1 )
					{
						FILETIME tmCurrent;
						GetSystemTimeAsFileTime(&tmCurrent);

						DWORD nSeconds = (DWORD)((*(LONGLONG*)&tmCurrent - sessionData->LogonTime.QuadPart) / 10000000);

						for(i = 0, pTmpBuf = pBuf; i < dwEntriesRead && pTmpBuf; i++, pTmpBuf++)
							if( max(nSeconds, pTmpBuf->sesi10_time) - min(nSeconds, pTmpBuf->sesi10_time) < 5 )
							{
								pUserInfo->m_sMachineName = (LPWSTR)pTmpBuf->sesi10_cname;
								bResolved = true;
								break;
							}
					}

					if( pBuf )
						m_pfNetApiBufferFree(pBuf);
				}
				break;
				
			case Interactive:
			default:
				{
					bResolved = true;
					PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tluid is local"));
					pUserInfo->m_LocalUser = cTRUE;
					pUserInfo->m_sMachineName = L"localhost";
				}

				break;
			}
		}

		if( sessionData && m_fnLsaFreeReturnBuffer )
			m_fnLsaFreeReturnBuffer(sessionData);
	}

	if (bResolved)
	{
		PR_TRACE((g_root, prtIMPORTANT, "avpgs\tnew luid resolved (0x%x-0x%x). machine %S", 
			pUserInfo->m_LuidHP, pUserInfo->m_Luid, pUserInfo->m_sMachineName.data()));

		AddNewLuidInfo(pUserInfo);
	}

	return true;
}

bool CJob::SetUserAndBanInfo(cString* hctx, cUserInfo* pUserInfo, cUserInfo* pUserBanInfo, PLUID pCreatorLuid, PLUID pUserOverrideLuid)
{
	if (m_OsInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
		return false;

	if (5 > m_OsInfo.dwMajorVersion)
		return false;

	if (pUserOverrideLuid)
	{
		if (!FillUserInfoByLuid( pUserOverrideLuid, pUserInfo, true))
			return false;
	}
	else
	{
		if (!FillLuidInfo(pUserInfo, /*false*/true))
			return false;
	}

	if (pCreatorLuid)
	{
		PR_TRACE((g_root, prtNOTIFY, "avpgs\tfill user by creator luid (0x%x-0x%x)", 
			pCreatorLuid->HighPart, pCreatorLuid->LowPart));

		FillUserInfoByLuid(pCreatorLuid, pUserBanInfo, true);
	}

	hctx->propSetPtr(propid_UserInfo, pUserInfo);

	if (pCreatorLuid)
		hctx->propSetPtr(propid_CrUserInfo, pUserBanInfo);
	else
	{
		PR_TRACE((g_root, prtNOTIFY, "avpgs\tno creator luid"));
	}

	return true;
}

bool
CJob::IsUserRemote (
	PLUID pLuid
	)
{
	cUserInfo UserInfo;
	
	UserInfo.m_Luid = pLuid->LowPart;
	UserInfo.m_LuidHP = pLuid->HighPart;

	if (FindExistingLuidInfo( &UserInfo ))
	{
		if (UserInfo.m_LocalUser)
			return FALSE;
	}

	return TRUE;
}

bool CJob::IsUserBaned(cUserInfo* pUserInfo)
{
	bool bBanned = false;
	
	bool bFirstBan = false;

	cUserBanList UnBanList;

	time_t current = cDateTime::now_utc();

	PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\tcheck user ban luid (0x%x-0x%x)", 
		pUserInfo->m_LuidHP, pUserInfo->m_Luid));

	m_LuidListSync.Lock();

	tUINT cou;
	tUINT cou_all = m_BanList.m_Users.count();
	for (cou = 0; cou < cou_all; cou++)
	{
		cUserBanItem& BanItem = m_BanList.m_Users[cou];

		if (current >= BanItem.m_StopTime)
		{
			if (_user_ban_send == BanItem.m_LocalFlags)
				UnBanList.m_Users.push_back(BanItem);

			m_BanList.m_Users.remove(cou);
			cou--;
			cou_all--;
			if (!cou_all)
				break;

			continue;
		}

		if ((BanItem.m_Luid == pUserInfo->m_Luid) && (BanItem.m_LuidHP == pUserInfo->m_LuidHP))
		{
			bBanned = true;

			if (_user_ban_new == BanItem.m_LocalFlags)
			{
				BanItem.m_LocalFlags = _user_ban_send;
				bFirstBan = true;
			}
		}
	}

	m_LuidListSync.UnLock();

	if (bBanned)
	{
		PR_TRACE((g_root, prtIMPORTANT, "avpgs\tuser luid (0x%x-0x%x)is banned", 
			pUserInfo->m_LuidHP, pUserInfo->m_Luid));
	}

	cou_all = UnBanList.m_Users.count();
	for (cou = 0; cou < cou_all; cou++)
	{
		cUserBanItem& BanItem = UnBanList.m_Users[cou];
		m_hTask->sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_ITEM_DEL, m_hTask, &BanItem, SER_SENDMSG_PSIZE);
	}

	return bBanned;
}

bool CJob::FindExistingLuidInfo(cUserInfo* pUserInfo)
{
	m_LuidListSync.Lock();

	for (_tLuidInfoList::iterator _it = m_LuidInfoList.begin(); _it != m_LuidInfoList.end(); _it++)
	{
		cUserInfo& luiditem = *_it;
		if ((luiditem.m_Luid == pUserInfo->m_Luid) && (luiditem.m_LuidHP == pUserInfo->m_LuidHP))
		{
			*pUserInfo = luiditem;

			m_LuidListSync.UnLock();

			return true;
		}
	}

	m_LuidListSync.UnLock();

	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tluid not found (0x%x-0x%x) in cache", 
		pUserInfo->m_LuidHP, pUserInfo->m_Luid));

	return false;
}

void CJob::EnumUsers()
{
	_tLuidInfoList	LuidInfoList;

	m_LuidListSync.Lock();
	LuidInfoList = m_LuidInfoList;
	m_LuidListSync.UnLock();

	for (_tLuidInfoList::iterator _it = LuidInfoList.begin(); _it != LuidInfoList.end(); _it++)
	{
		cUserInfo& luiditem = *_it;

		SendMessage(pmc_USERBAN, pm_USERNOTIFY_INCOME, &luiditem, SER_SENDMSG_PSIZE); // notify about new user income
	}
}

void CJob::AddNewLuidInfo(cUserInfo* pUserInfo)
{
	SendMessage(pmc_USERBAN, pm_USERNOTIFY_INCOME, pUserInfo, SER_SENDMSG_PSIZE); // notify about new user income

	m_LuidListSync.Lock();

	for (_tLuidInfoList::iterator _it = m_LuidInfoList.begin(); _it != m_LuidInfoList.end(); _it++)
	{
		cUserInfo& luiditem = *_it;
		if ((luiditem.m_Luid == pUserInfo->m_Luid) && (luiditem.m_LuidHP == pUserInfo->m_LuidHP))
		{
			 // already exist - delete and place new
			m_LuidCount--;
			m_LuidInfoList.erase(_it);			
			break;
		}
	}


	#pragma message(__FILE__LINE__ "TODO: CACHE LUID for server")
	if (m_LuidCount > 512)
	{
		m_LuidInfoList.clear();
		m_LuidCount = 0;
		PR_TRACE((g_root, prtIMPORTANT, "avpgs\treset luids cach"));
	}
	
	m_LuidCount++;	
	m_LuidInfoList.push_back(*pUserInfo);

	m_LuidListSync.UnLock();

	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tluid info cached (0x%x-0x%x) - %s", 
		pUserInfo->m_LuidHP, pUserInfo->m_Luid,
		pUserInfo->m_LocalUser ? "local" : "network"));

	return;
}

tERROR CJob::GetBanList(cUserBanList* pBanList)
{
//	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tGetBanList"));

	m_LuidListSync.Lock();
	
	pBanList->assign(m_BanList, true);

	m_LuidListSync.UnLock();

	time_t current = cDateTime::now_utc();

	int cou_all = pBanList->m_Users.count();
	for (int cou = 0; cou < cou_all; cou++)
	{
		cUserBanItem& BanItem = pBanList->m_Users[cou];

		BanItem.m_RemainTime = BanItem.m_StopTime - current;
	}
	
	return errOK_DECIDED;
}

tERROR CJob::SetBanList(cUserBanList* pBanList)
{
	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tSetBanList"));

	m_LuidListSync.Lock();
	
	m_BanList.assign(*pBanList, true);
	
	m_LuidListSync.UnLock();
	
	return errOK_DECIDED;
}

void CJob::PrintUserName(SID* pSid, char* pszmsg)
{
	return;

	/*
	may lock system - cache data!

	char szDomain[MAX_PATH];
	DWORD dwDomainLen = sizeof(szDomain);
	char szUser[MAX_PATH];
	DWORD dwUserLen = sizeof(szUser);

	if (IDriverGetUserFromSid(pSid, szDomain, &dwDomainLen, szUser, &dwUserLen))
	{
		PR_TRACE((g_root, prtNOT_IMPORTANT, "avpgs\t%s%s\\%s",
			pszmsg, szDomain, szUser));
	}
	*/
}

tERROR CJob::AddUserBan(cUserBanItem* pBanItem, bool bReSend)
{
	DeleteUserBan(pBanItem, bReSend);

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "avpgs\tAddBanItem (luid 0x%x-0x%x) - %s (process %d)",
		pBanItem->m_LuidHP, pBanItem->m_Luid,
		bReSend ? "resend" : "internal msg", GetCurrentProcessId()));

	SID* pSid = (SID*)pBanItem->m_Sid.data();
PrintUserName(pSid, "ban user:");

	m_LuidListSync.Lock();
	
	pBanItem->m_LocalFlags = _user_ban_ext;
	m_BanList.m_Users.push_back(*pBanItem);

	m_LuidListSync.UnLock();

	if (bReSend)
	{
		PR_TRACE((g_root, prtIMPORTANT, "avpgs\tsending AddBanItem..."));
		m_hTask->sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_ITEM_ADD, m_hTask, pBanItem, SER_SENDMSG_PSIZE);
	}

	return errOK_DECIDED;
}

tERROR CJob::DeleteUserBan(cUserBanItem* pBanItem, bool bReSend)
{
	tERROR error = errNOT_FOUND;

	PR_TRACE((g_root, prtIMPORTANT, "avpgs\tDeleteBanItem (luid 0x%x-0x%x) - %s",
		pBanItem->m_LuidHP, pBanItem->m_Luid,
		bReSend ? "resend" : "internal msg"));

	m_LuidListSync.Lock();
	
	int cou_all = m_BanList.m_Users.count();
	for (int cou = 0; cou < cou_all; cou++)
	{
		cUserBanItem& BanItem = m_BanList.m_Users[cou];
		if ((BanItem.m_Luid == pBanItem->m_Luid) && (BanItem.m_LuidHP == pBanItem->m_LuidHP))
		{
			m_BanList.m_Users.remove(cou);
			error = errOK_DECIDED;
			break;
		}
	}

	m_LuidListSync.UnLock();

	if (errOK_DECIDED == error)
	{
		if (bReSend)
		{
			PR_TRACE((g_root, prtIMPORTANT, "avpgs\tsending DeleteBanItem..."));
			m_hTask->sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_ITEM_DEL, m_hTask, pBanItem, SER_SENDMSG_PSIZE);
		}
	}

	return error;
}

void CJob::AddCrInfo(cAvpgsDelayedNotify* pCrInfo)
{
	PR_TRACE((g_root, prtNOTIFY, "avpgs\tincome notify task (pid %d) - '%S' (len %d)", 
		GetCurrentProcessId(), pCrInfo->m_FileName, pCrInfo->m_FileName.count()));

	PAVPGS_EVENT_DESCRIBE pEvDescr = NULL;
	m_EvSync.Lock();
	
	if (m_PHashTree_Delayed)
	{
		PHashTreeItem pHashItem = _Hash_Find(m_PHashTree_Delayed, pCrInfo->m_FileName.data(), pCrInfo->m_FileName.count());
		if (!pHashItem)
		{
			pEvDescr = (PAVPGS_EVENT_DESCRIBE) _MM_Alloc(sizeof(AVPGS_EVENT_DESCRIBE));
			if (pEvDescr)
			{
				memset(pEvDescr, 0, sizeof(AVPGS_EVENT_DESCRIBE));

				pEvDescr->m_eop = _op_other;

				pEvDescr->m_CreatorLuid.LowPart = pCrInfo->m_Luid;
				pEvDescr->m_CreatorLuid.HighPart = pCrInfo->m_LuidHP;

				pEvDescr->m_PtrFN = _MM_Alloc(pCrInfo->m_FileName.count());
				if (pEvDescr->m_PtrFN)
				{
					memcpy(pEvDescr->m_PtrFN, pCrInfo->m_FileName.data(), pCrInfo->m_FileName.count());
					QueryPerformanceCounter((LARGE_INTEGER*) &pEvDescr->m_Timeout);
					pEvDescr->m_Timeout += 20 * m_PerfomanceFreq;

					pHashItem = _Hash_AddItem(m_PHashTree_Delayed, pEvDescr->m_PtrFN, pCrInfo->m_FileName.count(), pEvDescr);
    				if (pHashItem)
					{
						pEvDescr = NULL;
					}
				}
			}
		}
	}

	m_EvSync.UnLock();

	if (pEvDescr)
	{
		if (pEvDescr->m_PtrFN)
			_MM_Free(pEvDescr->m_PtrFN);

		_MM_Free(pEvDescr);
	}
}

void CJob::SetMultiProcessMode()
{
	m_bMultiProcessMode = true;
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "avpgs\tMultiProcessMode (pid %d)",
		GetCurrentProcessId()));
}

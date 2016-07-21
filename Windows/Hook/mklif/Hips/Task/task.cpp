// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  18 July 2007,  12:01 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2007.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Martynenko
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end
// test from home


// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Includes for interface,  )
#include "task.h"

#include <Prague/prague.h>
#include <strsafe.h>


// AVP Prague stamp end
#include "hipsmanager.h"
#include "hipsrulemanager.h"

//	get events from driver
DWORD WINAPI JobThreadProc(LPVOID lpParameter);

//	processing user query
DWORD WINAPI UserInteractionProcessingProc(LPVOID lpParameter);

#define HKEY_LOCAL_MACHINE_N	L"HKEY_LOCAL_MACHINE"
#define HKLM_N					L"HKLM"
#define HKEY_USERS_N			L"HKEY_USERS"
#define HKEY_CURRENT_USER_N		L"HKEY_CURRENT_USER"
#define HKCU_N					L"HKCU"
#define HKEY_CURRENT_CONFIG_N	L"HKEY_CURRENT_CONFIG"
#define	HKEY_CLASSES_ROUTE_N	L"HKEY_CLASSES_ROOT"
#define HKCR_N					L"HKCR"

#define CURRENT_CONTROL_SET_N	L"\\SYSTEM\\CURRENTCONTROLSET"

#define REGISTRY_MACHINE_N		L"\\REGISTRY\\MACHINE"
#define REGISTRY_USER_N			L"\\REGISTRY\\USER"

////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::ObjectInit()
{
	//DebugBreak();
	tERROR error = errNOT_IMPLEMENTED;
	m_TaskState = TASK_STATE_STOPPED;

	m_pProcMonObj = 0;
	m_JobThreadHandle = 0;

	m_pHipsManager = new CHipsManager();
	m_pNetRulesManager= new CNetRulesManager();

	
	if (TRUE != m_pHipsManager->Initialize((void*)this))
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInit:: m_pHipsManager->Initialize FAIL !!!"));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );
	//m_pNetRulesManager= new CNetRulesManager();
//	tDWORD err=((cTaskManager*)m_tm)->GetService(TASKID_TM_ITSELF,(hOBJECT)this, AVP_PROFILE_NETWORK_WATCHER, (hOBJECT*)&m_pNetRulesManager->m_netwatch);
	//m_pNetRulesManager->init();
	//NetWatchTask->Init();
	//m_pNetRulesManager->m_netwatch=

	// Place your code here
#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif

	HRESULT hResult = MKL_ClientRegister(
		&m_pHipsManager->m_pClientContext, 
		AVPG_Driver_Specific,//KL_PDM,	//	???
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);
	if (IS_ERROR( hResult ))
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInit:: client registration failed, hresult 0x%x", hResult ));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	
	//	init thread pools for processing drv events (one thread for one event type)
	if (!m_pHipsManager->InitThreadPools())
	{
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	return errOK;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::ObjectInitDone()
{
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ObjectInitDone:: start" ));
	
	tERROR error;
	m_pNetRulesManager->ObjectInitDone((cObject *)this,m_tm);

	//DebugBreak();
	error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );


#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif

	ULONG ApiVersion = 0;
	ULONG AppId = 0;

	HRESULT hResult = MKL_GetDriverApiVersion(m_pHipsManager->m_pClientContext, &ApiVersion);
	if (SUCCEEDED(hResult))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ObjectInitDone:: Mklif API version: %d", ApiVersion));
	}
	else
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone Mklif API version FAIL (hResult = %x)", hResult));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	hResult = MKL_GetAppId(m_pHipsManager->m_pClientContext, &AppId);
	if (SUCCEEDED( hResult ))
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: Client registered with id: %d (0x%x)", AppId, AppId));
	}
	else
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_GetAppId FAIL (hResult = %x)", hResult));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	if (SUCCEEDED( hResult ))
	{
		DWORD FltId = 0;
		//	set filter to HIPS(file and registry) ask events
		hResult = MKL_AddFilter ( &FltId,
			m_pHipsManager->m_pClientContext,
			"*", DEADLOCKWDOG_TIMEOUT,  FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_HIPS, MI_SYSTEM_HIPS_ASK,
			0, PreProcessing, NULL, 0);
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(MJ_SYSTEM_HIPS, MI_SYSTEM_HIPS_ASK) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to HIPS(file and registry) log events
		hResult = MKL_AddFilter ( &FltId,
			m_pHipsManager->m_pClientContext,
			"*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_HIPS, MI_SYSTEM_HIPS_LOG ,
			0, PreProcessing, NULL, 0 );
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(MJ_SYSTEM_HIPS, MI_SYSTEM_HIPS_LOG) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to start process notify events
		////hResult = MKL_AddFilter( &FltId,
		////	m_pHipsManager->m_pClientContext,
		////	"*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO,
		////	FLTTYPE_SYSTEM, MJ_CREATE_PROCESS_NOTIFY, 0,
		////	0, PostProcessing, NULL, NULL );
		////if (FAILED(hResult))
		////{
		////	PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_CREATE_PROCESS_NOTIFY) FAIL (hResult = %x)", hResult));
		////	return errOBJECT_CANNOT_BE_INITIALIZED;
		////}
		//	set filter to process stopped notify
		hResult = MKL_AddFilter( &FltId,
			m_pHipsManager->m_pClientContext,
			"*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO,
			FLTTYPE_SYSTEM, MJ_EXIT_PROCESS, 0,
			0, PostProcessing, NULL, NULL );
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_EXIT_PROCESS) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to device control events
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext,
			"*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_KLFLTDEV, 0,
			0, PreProcessing, NULL,NULL );
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_KLFLTDEV) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to write process memory (for start proc defining)
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext,
			"*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_WRITE_PROCESS_MEMORY, 0,
			0, PreProcessing, NULL, NULL );
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_WRITE_PROCESS_MEMORY) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}

		////	set filter to terminate process
		//hResult = MKL_AddFilter(&FltId,
		//	m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
		//	FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS,
		//	0, 0, PreProcessing, NULL, NULL );
		//if (FAILED(hResult))
		//{
		//	PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS) FAIL (hResult = %x)", hResult));
		//	return errOBJECT_CANNOT_BE_INITIALIZED;
		//}
		//	set filter to terminate process
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_OPEN_PROCESS,
			0, 0, PreProcessing, NULL, NULL );
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_OPEN_PROCESS) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to SetWindowHook
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX,
			0, 0, PreProcessing, NULL, NULL );
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to MJ_SYSTEM_SET_THREAD_CONTEXT
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_SET_THREAD_CONTEXT,
			0, 0, PreProcessing, NULL, NULL );
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		////	set filter to MJ_SYSTEM_CREATE_THREAD
		//hResult = MKL_AddFilter(&FltId,
		//	m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
		//	FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_THREAD,
		//	0, 0, PostProcessing, NULL, NULL );	//???
		//if (FAILED(hResult))
		//{
		//	PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_THREAD) FAIL (hResult = %x)", hResult));
		//	return errOBJECT_CANNOT_BE_INITIALIZED;
		//}
		//	set filter to MJ_SYSTEM_CREATE_REMOTE_THREAD
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_REMOTE_THREAD,
			0, 0, PreProcessing, NULL, NULL );	//???
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_REMOTE_THREAD) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to shutdown windows
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_SHUTDOWN,
			0, 0, PreProcessing, NULL, NULL );	//???
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_SHUTDOWN) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to MJ_SYSTEM_KEYLOGGER_DETECTED
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_SYSTEM, MJ_SYSTEM_KEYLOGGER_DETECTED,
			0, 0, PostProcessing, NULL, NULL );	//???
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_SYSTEM, MJ_SYSTEM_KEYLOGGER_DETECTED) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		//	set filter to IRP_MJ_SET_INFORMATION
		hResult = MKL_AddFilter(&FltId,
			m_pHipsManager->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP,
			FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION,
			FileLinkInformation, 0, PreProcessing, NULL, NULL );	
		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_AddFilter(FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION) FAIL (hResult = %x)", hResult));
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}

		
	}
	hResult = MKL_ClientSync( m_pHipsManager->m_pClientContext );
	if (FAILED(hResult))
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: MKL_ClientSync FAIL (hResult = %x)", hResult));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	DWORD JobThreadID = 0;
	m_JobThreadHandle = CreateThread(0, 0, JobThreadProc, this/*m_pHipsManager*/, 0, &JobThreadID);
	if (m_JobThreadHandle == 0) 
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ObjectInitDone:: CreateJobThread fial err = %d", GetLastError()));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ObjectInitDone:: OK"));
	return errOK;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::ObjectPreClose()
{
	if(m_pNetRulesManager)
	{
		m_pNetRulesManager->ObjectPreClose();
		delete m_pNetRulesManager;
		m_pNetRulesManager=0;
	}
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	m_pHipsManager->m_IsNeedToStopAll = true;

#ifdef HIPS_DEBUG_WITHOUT_DRV
	if (m_pHipsManager)
		delete m_pHipsManager;
	m_pHipsManager = 0;
	return errOK;
#endif

	if (m_pHipsManager->m_pClientContext)
		MKL_ClientBreakConnection(m_pHipsManager->m_pClientContext);
	if (m_JobThreadHandle)
	{
		for (int i=0; i<10; i++)
		{
			if (WAIT_TIMEOUT != WaitForSingleObject(m_JobThreadHandle, 1000))
				break;
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::PreClose waiting for JobThread %d sec...", i));
		}
		CloseHandle(m_JobThreadHandle);
	}
	if (m_pHipsManager->m_pClientContext)
	{
		PR_TRACE((g_root, prtERROR, TR "unregister client"));
		MKL_ClientUnregister(&m_pHipsManager->m_pClientContext);
	}
	error = errOK;

	if (m_pHipsManager)
		delete m_pHipsManager;
	m_pHipsManager = 0;

	if (m_pProcMonObj)
		m_tm->ReleaseService(TASKID_TM_ITSELF,(hOBJECT)m_pProcMonObj);

	return error;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );
	//PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::MsgReceive:: p_msg_cls_id=0x%x, p_msg_id = 0x%x",
	//	p_msg_cls_id, p_msg_id));
	switch (p_msg_id)
	{
	case pmPROFILE_SETTINGS_MODIFIED:
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "pmPROFILE_SETTINGS_MODIFIED"));
			cSerializable* pSerializable = (cSerializable*)p_par_buf;
			if (p_par_buf_len != SER_SENDMSG_PSIZE || !p_par_buf)
				return errPARAMETER_INVALID;
			 cProfileBase* pProfileInfo = (cProfileBase*)pSerializable;
			if (!pSerializable->isBasedOn(cProfileBase::eIID))
				return errPARAMETER_INVALID;;
			if(p_msg_cls_id==pmcPROFILE && p_msg_id==pmPROFILE_SETTINGS_MODIFIED && 
				pProfileInfo->m_sName==AVP_PROFILE_NETWORK_WATCHER && m_pNetRulesManager)
			{
				m_pNetRulesManager->NetWatcherSettingsChanged(); 
				if(m_pNetRulesManager->m_settings.Changed())
				{
					m_pNetRulesManager->GeneratePacketRules();
					m_pNetRulesManager->GenerateAppRules();
				}
			}
			break;
		}
	case pm_PROCESS_MONITOR_PROCESS_START://pm_PROCESS_MONITOR_PROCESS_START_WITH_RATING:
		{
			if (p_msg_cls_id == pmc_PROCESS_MONITOR)
			{
				PR_TRACE((g_root, prtIMPORTANT, TR "pm_PROCESS_MONITOR_PROCESS_START_WITH_RATING"));
				if (!p_par_buf)
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "p_par_buf == NULL"));
					return errPARAMETER_INVALID;;
				}
				cSerializable* pSerializable = (cSerializable*)p_par_buf;
				if (!pSerializable->isBasedOn(cProcMonNotifyEx::eIID))
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "cSecurityRating::eIID FAIL"));
					return errPARAMETER_INVALID;;
				}
				cProcMonNotifyEx * pRating = (cProcMonNotifyEx*)pSerializable;
				if (m_pHipsManager->CheckRuleForAppNew(pRating, true))
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "CheckRuleForAppNew return true, send pmc_TASK_SETTINGS_CHANGED"));
					sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
				}
				else
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "CheckRuleForAppNew return false !!!"));
				}
			}
			break;
		}
	};
	return error;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::SetSettings( const cSerializable* p_settings )
{
	//m_pHipsManager->TestFunc();
	switch (m_TaskState)
	{
		case TASK_STATE_STOPPED:
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetSettings state is TASK_STATE_STOPPED"));
			break;
		case TASK_STATE_RUNNING:
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetSettings state is TASK_STATE_RUNNING"));
			break;
		case TASK_STATE_FAILED:
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetSettings state is TASK_STATE_FAILED"));
			break;
		case TASK_STATE_PAUSED:
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetSettings state is TASK_STATE_PAUSED"));
			break;
		default:
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetSettings state is %x", m_TaskState));
	};

	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetSettings:: start"));

	// Place your code here
	if (p_settings->isBasedOn(CHipsSettings::eIID))
	{
		DWORD Temp = GetTickCount();
		if(m_pNetRulesManager)
			m_pNetRulesManager->SetSettings(p_settings);
		error = m_pHipsManager->m_HipsRuleManager.m_Settings.assign(*p_settings, cFALSE);
		if (PR_SUCC(error))
		{
			if (m_TaskState == TASK_STATE_RUNNING)
			{
				m_pHipsManager->m_HipsRuleManager.ResolveRuleList();

				error = ReloadAllRulesInDrv();
				if (PR_FAIL(error))
				{
					PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetSettings ReloadAllRulesInDrv return %x", error));
				}
				else
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::SetSettings Reloading was %d(ms)", GetTickCount() - Temp));
				}
			}
			else
			{
				PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::SetSettings no need to reload, task not run yet"));
			}
		}
	}
	else
		error = errNOT_SUPPORTED;

	if (PR_FAIL(error))
	{
		PR_TRACE((g_root, prtERROR, TR "SetSettings %terr", error));
	}

	m_pHipsManager->TestFunc();
	return error;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::GetSettings:: start"));

	// Place your code here
	if (p_settings->isBasedOn(CHipsSettings::eIID))
		error = p_settings->assign(m_pHipsManager->m_HipsRuleManager.m_Settings, cTRUE );
	else
		error = errNOT_SUPPORTED;


	//PR_TRACE((g_root, prtERROR, TR "GetSettings %terr", error));
	return error;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here
	PR_TRACE((g_root, prtERROR, TR "AskAction %terr", error));
	return error;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHipsTask::SetState method" );

	// Place your code here
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::SetState request to change state: incoming value 0x%x", p_state));

	bool IsFltDevFail = false;
	HRESULT hResult = S_OK;
	tMsgHandlerDescr hdl[] = {
          { (hOBJECT)this, rmhLISTENER, pmcPROFILE, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
          { (hOBJECT)this, rmhLISTENER, pmPROFILE_SETTINGS_MODIFIED , IID_ANY, PID_ANY, IID_ANY, PID_ANY }, 
          { (hOBJECT)this, rmhLISTENER, pmc_PROCESS_MONITOR , IID_ANY, PID_ANY, IID_ANY, PID_ANY }, 
		  
	};

	m_pNetRulesManager->SetState(p_state);

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetState TASK_REQUEST_RUN"));

		if (!m_pHipsManager->m_HipsRuleManager.ResolveRuleList())
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetState ResolveRuleList() return %x", error));
			m_TaskState = TASK_STATE_FAILED;
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		
		if (PR_FAIL(error = ReloadAllRulesInDrv()))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetState ReloadAllRulesInDrv return %x", error));
			m_TaskState = TASK_STATE_FAILED;
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		if (warnFLT_DEV_ERR == error)
		{
			IsFltDevFail = true;
		}

		if (CheckProcessOnStart())
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::SetState CheckProcessOnStart() return true, send need to reload"));
			sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
		}

		#ifndef HIPS_DEBUG_WITHOUT_DRV
			hResult = MKL_ChangeClientActiveStatus( m_pHipsManager->m_pClientContext, TRUE );
		#endif

		if (!SUCCEEDED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetState MKL_ChangeClientActiveStatus(TRUE) FAIL %x", hResult));
			m_TaskState = TASK_STATE_FAILED;
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}

		if (CheckProcessOnStart())
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::SetState CheckProcessOnStart() return true, send need to reload"));
			sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
		}

		m_TaskState = TASK_STATE_RUNNING;
	
		error = m_tm->sysRegisterMsgHandlerList (hdl, countof(hdl));

		error = warnTASK_STATE_CHANGED;
		break;

	case TASK_REQUEST_PAUSE:
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetState TASK_REQUEST_PAUSE"));

		#ifndef HIPS_DEBUG_WITHOUT_DRV
			hResult = MKL_ChangeClientActiveStatus( m_pHipsManager->m_pClientContext, FALSE );
		#endif

		if (FAILED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetState MKL_ChangeClientActiveStatus return %x", hResult));
			m_TaskState = TASK_STATE_FAILED;
			return errUNEXPECTED;
		}

		m_TaskState = TASK_STATE_PAUSED;
		m_tm->sysUnregisterMsgHandlerList (hdl, countof(hdl));
		m_pNetRulesManager->SetState(p_state);
		error = warnTASK_STATE_CHANGED;
		break;

	case TASK_REQUEST_STOP:
		//	need to stop task
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::SetState TASK_REQUEST_STOP"));
		m_pHipsManager->m_IsNeedToStopAll = true;

		#ifndef HIPS_DEBUG_WITHOUT_DRV
			MKL_ChangeClientActiveStatus( m_pHipsManager->m_pClientContext, FALSE );
			MKL_ClientBreakConnection( m_pHipsManager->m_pClientContext);
		#endif
		if (m_JobThreadHandle)
		{
			for (int i=0; i<10; i++)
			{
				if (WAIT_TIMEOUT != WaitForSingleObject(m_JobThreadHandle, 1000))
					break;
				PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetState(Stop) waiting for JobThread %d sec...", i));
			}
			CloseHandle(m_JobThreadHandle);
			m_JobThreadHandle = NULL;
		}

		m_pHipsManager->FreeThreadPools();
		
		//	free internal rules
		m_TaskState = TASK_STATE_STOPPED;
		m_tm->sysUnregisterMsgHandlerList (hdl, countof(hdl));
		m_pNetRulesManager->SetState(p_state);

		error = warnTASK_STATE_CHANGED;
		break;
	default:
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetState unknown p_state = 0x%x", p_state));
		return errPARAMETER_INVALID;
	}
	

	tDWORD size = sizeof(error);
	error = warnFLT_DEV_ERR;
	PR_TRACE((g_root, prtERROR, TR "CHipsTask::SetState return OK, and send (changes, 0x%x, 0x%x)",
		m_TaskState,
		(IsFltDevFail) ? warnFLT_DEV_ERR : 0));

	sysSendMsg(pmc_TASK_STATE_CHANGED, m_TaskState, 0,
		(IsFltDevFail) ? &error : 0,
		(IsFltDevFail) ? &size : 0);
	return errOK;;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
tERROR CHipsTask::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	if (m_pHipsManager && p_statistics->isBasedOn(cHIPSStatistics::eIID))
		error = p_statistics->assign(m_pHipsManager->m_HIPSStatistics, cTRUE);
	else
		error = errNOT_SUPPORTED;
	
	if (PR_FAIL( error ))
	{
		PR_TRACE((g_root, prtERROR, TR "GetStatistics %terr", error));
	}
	return error;
}
////////////////////////////////////////////////////////////////////////////
//	prepare space in driver for new rule collection
////////////////////////////////////////////////////////////////////////////
DWORD CHipsTask::ResetRules()
{
	m_pNetRulesManager->ResetAppSeverityRule();

#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask:: before MKL_ResetClientRules(%x)", m_pHipsManager->m_pClientContext));
	HRESULT hRes = MKL_ResetClientRules(m_pHipsManager->m_pClientContext);
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask:: MKL_ResetClientRules return %x", hRes));
	
	
	return (hRes == S_OK) ? errOK : errNOT_OK;
}
////////////////////////////////////////////////////////////////////////////
//	apply rule collection in driver
////////////////////////////////////////////////////////////////////////////
DWORD CHipsTask::ApplyRules()
{
	m_pNetRulesManager->ApplyAppSeverityRule();

#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask:: before MKL_ApplyRules(%x)", m_pHipsManager->m_pClientContext));
	HRESULT hRes = MKL_ApplyRules(m_pHipsManager->m_pClientContext);
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask:: MKL_ApplyRules return %x", hRes));

	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask:: before MKL_KLFltDev_ApplyRules(%x)", m_pHipsManager->m_pClientContext));
	hRes = MKL_KLFltDev_ApplyRules(m_pHipsManager->m_pClientContext);
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask:: MKL_KLFltDev_ApplyRules return %x", hRes));


	return (hRes == S_OK) ? errOK : errNOT_OK;
}
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
bool CHipsTask::ConvertFilePathToNative(wchar_t * l_pSrcPath, wchar_t ** l_ppDstPath)
{
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::ConvertFilePathToNative start (%S)", l_pSrcPath));
	if (!l_pSrcPath || !l_ppDstPath)
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ConvertFilePathToNative INVALID ARG (l_pSrcPath = %x, l_ppDstPath = %x)",
			l_pSrcPath, l_ppDstPath));
		return false;
	}
	HRESULT hRes = S_OK;

	wchar_t VolumeName[3];
	wchar_t OutVolumeName[100];
	wchar_t * VariableStart = 0;
	wchar_t * VariableEnd = 0;
	wchar_t Variable[50];

	DWORD OutDeviceNameSize = 0;

	cStrObj TempStr;

	if ((wcslen(l_pSrcPath) > 3) &&
			 (VariableStart = wcschr(l_pSrcPath, L'%')) &&
			 (VariableEnd = wcschr(l_pSrcPath+1, L'%')))
	{
		wcsncpy(Variable, VariableStart, (VariableEnd - VariableStart + 1));
		Variable[(VariableEnd - VariableStart + 1)] = 0;

		TempStr = Variable;
		if (g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(TempStr), 0, 0) != errOK_DECIDED )
		{
			return false;
		}
		TempStr.check_last_slash(false);
		VariableEnd++;
		if (VariableEnd)
			TempStr += VariableEnd;
		l_pSrcPath = TempStr.c_str(cCP_UNICODE);
	}

	DWORD OutNativeFileNameSize = (wcslen(l_pSrcPath)+MAX_PATH) * sizeof(wchar_t);

	*l_ppDstPath = (wchar_t*)new BYTE[OutNativeFileNameSize];

	if (wcschr(l_pSrcPath, L'*') || wcschr(l_pSrcPath, L'?') || wcschr(l_pSrcPath, L'%'))
	{
		//	path with mask
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::ConvertFilePathToNative path with mask"));
		if (wcslen(l_pSrcPath) > 3 && l_pSrcPath[1] == ':' && l_pSrcPath[2] == '\\')
		{
			//	path start with drive (* is some where further)
			wcsncpy(VolumeName, l_pSrcPath, 2);
			VolumeName[2] = 0;
			OutDeviceNameSize = QueryDosDeviceW(
				VolumeName,
				OutVolumeName,
				sizeof(OutVolumeName)
				);

			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::ConvertFilePathToNative QueryDosDevice return %d (%S -> %S)",
				OutDeviceNameSize,
				VolumeName,
				OutVolumeName
				));
			if (OutDeviceNameSize == 0)
			{
				return false;
			}
			wcscpy_s(*l_ppDstPath, OutNativeFileNameSize / sizeof(wchar_t), OutVolumeName);
			wcscat_s(*l_ppDstPath, OutNativeFileNameSize / sizeof(wchar_t), l_pSrcPath + 2);
		}
		else
		{
			wcscpy_s(*l_ppDstPath, OutNativeFileNameSize / sizeof(wchar_t), l_pSrcPath);
		}
	}
	else
	{
		//	clear path
#ifdef HIPS_DEBUG_WITHOUT_DRV
		wcscpy_s(*l_ppDstPath, OutNativeFileNameSize / sizeof(wchar_t), l_pSrcPath);
		return true;
#endif

//		DebugBreak();
		hRes = MKL_QueryFileNativePath(
			m_pHipsManager->m_pClientContext,
			l_pSrcPath,
			*l_ppDstPath,
			&OutNativeFileNameSize);
		if (hRes != S_OK || OutNativeFileNameSize == 0)
		{
			if (*l_ppDstPath)
				delete [] *l_ppDstPath;
			*l_ppDstPath = 0;
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ConvertFilePathToNative FAIL - return %x (%S), OutNativeFileNameSize = %d",
					hRes,
					l_pSrcPath,
					OutNativeFileNameSize));
			return false;
		}
		(*l_ppDstPath)[OutNativeFileNameSize / sizeof(wchar_t)] = 0;
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::ConvertFilePathToNative return %x (%S -> %S)",
			hRes,
			l_pSrcPath,
			*l_ppDstPath));
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool CHipsTask::UserRegPath2Kernel(wchar_t * UserRegPath, wchar_t ** KernelRegPath)
{
	if (KernelRegPath == 0 || UserRegPath == 0)
		return false;

	KernelRegPath[0] = 0;

	DWORD UserPathLen = (DWORD)wcslen(UserRegPath);
	if (UserPathLen == 0)
		return false;
	UserPathLen += 100;
	*KernelRegPath = new wchar_t[UserPathLen]; //	100 - for prefix like \\REGISTRY\\MACHINE\\SOFTWARE\\CLASSES
	(*KernelRegPath)[0] = 0;

//	if (wcsrchr(UserRegPath, L'\\') == UserRegPath+UserPathLen-1)
//		UserRegPath[UserPathLen-1] = 0;

	BOOL bHKLMKey = FALSE;
	PWCHAR pStrRemainder = UserRegPath;
	do
	{
// MACHINE
		if (_wcsnicmp(UserRegPath, HKEY_LOCAL_MACHINE_N, wcslen(HKEY_LOCAL_MACHINE_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKEY_LOCAL_MACHINE_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\MACHINE");
			bHKLMKey = TRUE;

			break;
		}
		if (_wcsnicmp(UserRegPath, HKLM_N, wcslen(HKLM_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKLM_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\MACHINE");
			bHKLMKey = TRUE;

			break;
		}

// USER
		if (_wcsnicmp(UserRegPath, HKEY_USERS_N, wcslen(HKEY_USERS_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKEY_USERS_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\USER");

			break;
		}

		if (_wcsnicmp(UserRegPath, HKEY_CURRENT_USER_N, wcslen(HKEY_CURRENT_USER_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKEY_CURRENT_USER_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\USER\\*");

			break;
		}

		if (_wcsnicmp(UserRegPath, HKCU_N, wcslen(HKCU_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKCU_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\USER\\*");

			break;
		}

// CURRENT CONFIG
		if (_wcsnicmp(UserRegPath, HKEY_CURRENT_CONFIG_N, wcslen(HKEY_CURRENT_CONFIG_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKEY_CURRENT_CONFIG_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET???");

			break;
		}

// CLASSES ROUTE
		if (_wcsnicmp(UserRegPath, HKEY_CLASSES_ROUTE_N, wcslen(HKEY_CLASSES_ROUTE_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKEY_CLASSES_ROUTE_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\MACHINE\\SOFTWARE\\CLASSES");

			break;
		}

		if (_wcsnicmp(UserRegPath, HKCR_N, wcslen(HKCR_N)) == 0)
		{
			pStrRemainder = UserRegPath+wcslen(HKCR_N);
			wcscpy_s(*KernelRegPath, UserPathLen, L"\\REGISTRY\\MACHINE\\SOFTWARE\\CLASSES");

			break;
		}
	} while (FALSE);

	//	link copy
	if (bHKLMKey && _wcsnicmp(pStrRemainder, CURRENT_CONTROL_SET_N, wcslen(CURRENT_CONTROL_SET_N)) == 0)
	{
		pStrRemainder += wcslen(CURRENT_CONTROL_SET_N);
		wcscat_s(*KernelRegPath, UserPathLen, L"\\SYSTEM\\CONTROLSET???");
		wcscat_s(*KernelRegPath, UserPathLen, pStrRemainder);
	}
	else
		wcscat_s(*KernelRegPath, UserPathLen, pStrRemainder);

	return true;
}
////////////////////////////////////////////////////////////////////////////
//	add one rule to driver
////////////////////////////////////////////////////////////////////////////
bool CHipsTask::KernelRegPath2User(wchar_t ** l_ppUserRegPath, wchar_t * l_pKernelRegPath)
{
	if (l_ppUserRegPath == 0 || l_pKernelRegPath == 0)
		return false;
	PWCHAR RemainingStr = l_pKernelRegPath;
	DWORD size = wcslen(l_pKernelRegPath) + 10;
	*l_ppUserRegPath = new wchar_t[size];
	if (_wcsnicmp(l_pKernelRegPath, REGISTRY_MACHINE_N, wcslen(REGISTRY_MACHINE_N)) == 0)
	{
		wcscpy_s(*l_ppUserRegPath, size, HKEY_LOCAL_MACHINE_N);
		RemainingStr = l_pKernelRegPath+wcslen(REGISTRY_MACHINE_N);
	}
	else if (_wcsnicmp(l_pKernelRegPath, REGISTRY_USER_N, wcslen(REGISTRY_USER_N)) == 0)
	{
		wcscpy_s(*l_ppUserRegPath, size, HKEY_USERS_N);
		RemainingStr = l_pKernelRegPath+wcslen(REGISTRY_USER_N);
	}
	wcscat_s(*l_ppUserRegPath, size, RemainingStr);
	return true;
}
////////////////////////////////////////////////////////////////////////////
//	add one rule to driver
////////////////////////////////////////////////////////////////////////////
DWORD CHipsTask::AddRuleToDrv(SHipsRuleItem * l_pRule)
{
	//return TRUE;	
	//DebugBreak();
	if (l_pRule == 0) 
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::AddRuleToDrv l_pRule == 0"));
		return errNOT_OK;
	}

	if (((l_pRule->f_ResType == cResource::ehrtFile) || (l_pRule->f_ResType == cResource::ehrtReg)) &&
		(l_pRule->f_pAppName == 0 || l_pRule->f_pResName == 0))
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::AddRuleToDrv return HIPS_EC_INVALID_ARG, l_pRule->f_pAppName = %S, l_pRule->f_pResName = %S",
			l_pRule->f_pAppName,
			l_pRule->f_pResName));
		return errNOT_OK;
	}

	HRESULT hRes = S_OK;

	DWORD buffsize = 16;

	PR_TRACE((g_root, prtNOT_IMPORTANT,
		TR "CHipsTask::AddRuleToDrv before MKL_AddRule(\n\tm_pClientContext = %x\n\tpNativeAppName = %S\n\tpNativeResName = %S\n\trestype=%d\n\tSeverity=%d\n\tAccessMask=%x\n\tDevGUID=%S\n\tDevType=%S\n\tl_pRule=%d)",
		m_pHipsManager->m_pClientContext,
		l_pRule->f_pAppNativeName,// pNativeAppName,
		l_pRule->f_pResNativeName,// pNativeResName,
		l_pRule->f_ResType,
		l_pRule->f_Severity,
		l_pRule->f_AccessFlag,
		l_pRule->f_DevGuid,
		l_pRule->f_DevType,
		l_pRule->f_RuleID
		));

	DWORD TempState = CHipsSettings::ehsssAllow;
	switch (l_pRule->f_ResType)
	{
		case cResource::ehrtFile:
		case cResource::ehrtReg:
			{
				#ifdef HIPS_DEBUG_WITHOUT_DRV
					return errOK;
				#endif
				TempState = m_pHipsManager->m_HipsRuleManager.m_Settings.m_FileAndRegState;
				if (TempState != CHipsSettings::ehsssAllow)
				{
					hRes = MKL_AddApplRule(m_pHipsManager->m_pClientContext,
													l_pRule->f_pAppNativeName,//pNativeAppName, 
													l_pRule->f_pResNativeName,//pNativeResName,
													l_pRule->f_pResValue,
													0, 0, //buff, buffsize,
													//0/*l_pRule->f_pAppHash*/, 0/*HIPS_APP_HASH_SIZE*/,
													(TempState == CHipsSettings::ehsssDeny) ? HIPS_FLAG_DENY_ALL : l_pRule->f_AccessFlag,
													&(l_pRule->f_DrvRuleID),
													l_pRule->f_RuleGrID);
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::AddRuleToDrv MKL_AddApplRule return %x, f_DrvRuleID=%d", hRes, l_pRule->f_DrvRuleID));
				}
				else
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::AddRuleToDrv rule skipped by FileAndRegState"));
				}
				break;
			}
		case cResource::ehrtDevice:
			{
				#ifdef HIPS_DEBUG_WITHOUT_DRV
					return errOK;
				#endif
				REPLUG_STATUS status;
				TempState = m_pHipsManager->m_HipsRuleManager.m_Settings.m_FileAndRegState;
				if (TempState != CHipsSettings::ehsssAllow)
				{
					hRes = MKL_KLFltDev_SetRule(m_pHipsManager->m_pClientContext,
												l_pRule->f_DevGuid,
												l_pRule->f_DevType,
												(TempState == CHipsSettings::ehsssDeny) ? HIPS_FLAG_DENY : l_pRule->f_AccessFlag,
												&status);
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::AddRuleToDrv MKL_KLFltDev_SetRule return %x", hRes));
					if (hRes != S_OK)
					{
						return warnFLT_DEV_ERR;
					}
				}
				else
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsTask::AddRuleToDrv rule skipped by m_DevicesState"));
				}
				break;
			}
		case cResource::ehrtSeverity:
			{
				PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::AddRuleToDrv:: before SetAppSeverityRule (m_pNetRulesManager = 0x%x)",
					m_pNetRulesManager));
				if (!m_pNetRulesManager)
					break;
				m_pNetRulesManager->SetAppSeverityRule(
														l_pRule->f_pAppName,//  NativeName,
														l_pRule->f_Severity,
														l_pRule->f_AccessFlag
														);
				PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::AddRuleToDrv SetAppSeverityRule\n\tAppName = %S,\n\tSeverity = %d, AccessFlag = 0x%x",
					l_pRule->f_pAppName, l_pRule->f_Severity, l_pRule->f_AccessFlag));
				break;
			}
		default:
			{
				PR_TRACE((g_root, prtERROR, TR "CHipsTask:: unknown res type (%d)", l_pRule->f_ResType));
				//hRes = E_FAIL;
				break;
			}
	}

	return (hRes == S_OK) ? errOK : errNOT_OK;
}
////////////////////////////////////////////////////////////////////////////
//	reload all rules to driver from internal lists
////////////////////////////////////////////////////////////////////////////
DWORD CHipsTask::ReloadAllRulesInDrv()
{
	ResetRules();
	return ReloadRulesInDrv(&m_pHipsManager->m_HipsRuleManager.m_HipsRulesList);
}
////////////////////////////////////////////////////////////////////////////
//	reload all rules to driver from internal lists
////////////////////////////////////////////////////////////////////////////
DWORD CHipsTask::ReloadRulesInDrv(HipsRulesList * l_pHipsRulesList)
{
	//DebugBreak();

	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ReloadAllRulesInDrv:: start"));
	DWORD eRes = errOK;
	BOOL bRes = HIPS_EC_OK;
	pSHipsRuleItem pRule = 0;
	DWORD StartTimeAll = GetTickCount();
	DWORD RuleCount = 0;
	bool IsFltDevFail = false;
	bRes = m_pHipsManager->GetFirstRule(&pRule, l_pHipsRulesList);
	if (!HIPS_SUCCESS(bRes)) {
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ReloadAllRulesInDrv:: GetFirstRule fail"));
		return errNOT_OK;
	}
	if (bRes == HIPS_EC_NO_MORE_ELEMENTS)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ReloadAllRulesInDrv:: there are no rules"));
		return errOK;
	}
	//	reset rules in driver (prepare for new)
	DWORD StartTime = GetTickCount();
	//	eRes = ResetRules();	//	move to allreload
//	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ReloadAllRulesInDrv:: ResetRules = %d ms)", GetTickCount() - StartTime));

	if ( PR_FAIL( bRes )) {
		PR_TRACE((g_root, prtERROR, TR "CHipsTask::ReloadAllRulesInDrv:: ResetRules fail"));
		return bRes;
	}
	do {
		if (pRule->f_State == hrisEnable && 
			((pRule->f_ResType == cResource::ehrtFile) ||
			 (pRule->f_ResType == cResource::ehrtReg) ||
			 (pRule->f_ResType == cResource::ehrtDevice) ||
			 (pRule->f_ResType == cResource::ehrtSeverity)
			 ))
		{
			//	adding only enabled rules
			StartTime = GetTickCount();
			eRes = AddRuleToDrv(pRule);
			//PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ReloadAllRulesInDrv:: AddRuleToDrv = %d ms)", GetTickCount() - StartTime));
			
			if (warnFLT_DEV_ERR == eRes)
				IsFltDevFail = true;
			if (PR_FAIL(eRes))
			{
				PR_TRACE((g_root, prtERROR, TR "CHipsTask::ReloadAllRulesInDrv:: AddRuleToDrv fail (eRes = 0x%x)", eRes));
				break;	//???
				//if (HIPS_EC_INVALID_ARG != bRes)
				//	break;
			}
			RuleCount++;
		}
	} while (HIPS_EC_OK == (bRes = m_pHipsManager->GetNextRule(&pRule, l_pHipsRulesList)));

	if (HIPS_SUCCESS(bRes) && PR_SUCC(eRes)) {
		StartTime = GetTickCount();
		eRes = ApplyRules();
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ReloadAllRulesInDrv:: ApplyRules = %d ms)", GetTickCount() - StartTime));
		if (PR_FAIL(eRes)) 
		{
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::ReloadAllRulesInDrv:: ApplyRules fail"));
		}
	}

	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ReloadAllRulesInDrv:: ApplyRules OK (added RuleCount = %d, tiem = %d ms)", 
		RuleCount, GetTickCount() - StartTimeAll));

	if (IsFltDevFail)
	{
		eRes = warnFLT_DEV_ERR;
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::ReloadAllRulesInDrv:: ApplyRules IsFltDevFail is true, return warnFLT_DEV_ERR(0x%x)", warnFLT_DEV_ERR)); 
	}
	return eRes;
}


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CHipsTask::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CHipsTask, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Hips Task", 10 )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_HIPS,                               // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MEZHUEV,                            // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(CHipsTask)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CHipsTask::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_TM
// You have to implement propetry: pgTASK_STATE
// You have to implement propetry: pgTASK_SESSION_ID
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI JobThreadProc(LPVOID lpParameter)
{
	//return 0;
	if (lpParameter == 0)
	{
		return 0;
	}
	//DebugBreak();

	PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: start"));

	CHipsTask * pTask = (CHipsTask*)lpParameter;
	CHipsManager * pHipsManager = pTask->GetHipsManager();/*(CHipsManager*)lpParameter*/;
	HRESULT hResult = S_OK;
	PVOID pMessage = NULL;
	DWORD MessageSize = 0;
	PMKLIF_EVENT_HDR pEventHdr;

	PVOID pMessage2 = NULL;
	DWORD MessageSize2 = 0;
//	PMKLIF_EVENT_HDR pEventHdr2;

	MKLIF_REPLY_EVENT LogVerdict;
	memset(&LogVerdict, 0, sizeof(LogVerdict));
	LogVerdict.m_VerdictFlags = efVerdict_Allow;
	LogVerdict.m_ExpTime = 0;


	MKLIF_REPLY_EVENT PendingVerdict;
	memset( &PendingVerdict, 0, sizeof(PendingVerdict) );
	PendingVerdict.m_VerdictFlags = efVerdict_Pending;
	PendingVerdict.m_ExpTime = 2;

	MKLIF_REPLY_EVENT AllowVerdict;
	memset( &AllowVerdict, 0, sizeof(AllowVerdict) );
	AllowVerdict.m_VerdictFlags = efVerdict_Allow;
	AllowVerdict.m_ExpTime = 2;
	
	hResult = MKL_BuildMultipleWait( pHipsManager->m_pClientContext, 1 );	

	PSINGLE_PARAM pObject = 0;
	PSINGLE_PARAM pObjectEx = 0;
	//PSINGLE_PARAM pParamPID = 0;
	PSINGLE_PARAM pParamResult = 0;
	PSINGLE_PARAM pDesiredOperation = 0;
	PSINGLE_PARAM pRuleID = 0;

	ULONG ContextFlags = 0;
	PSINGLE_PARAM pParamContextFlags = 0;

	wchar_t * ProcName = 0;
	
	bool IsNeedToReloadRule = false;


	PMKLIF_REPLY_EVENT pVerdict = NULL;

	while (hResult == S_OK) 
	{
		IsNeedToReloadRule = false;
		//PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: before MKL_GetMessage"));
		hResult = MKL_GetMessage(pHipsManager->m_pClientContext, &pMessage, &MessageSize, &pEventHdr );
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: after MKL_GetMessage"));
		if (SUCCEEDED( hResult ))
		{
			//PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: before MKL_GetMultipleMessage"));
			do
			{
				hResult = MKL_GetMultipleMessage(
							pHipsManager->m_pClientContext,
							&pMessage,
							&MessageSize,
							&pEventHdr,
							1000/*INFINITE*/ );

			} while (E_PENDING == hResult);
			//PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: after MKL_GetMultipleMessage"));
		}

		if (!SUCCEEDED(hResult))
		{
			PR_TRACE(( g_root, prtERROR, TR "wrong state, hresult 0x%x (native err 0x%x). exit!", 
				hResult, MKL_GetLastNativeError(pHipsManager->m_pClientContext) ));

			PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: exit command"));
			pfMemFree( NULL, &pMessage );
			break;
		}

		PendingVerdict.m_ExpTime = 15;
		hResult = MKL_ReplyMessage( pHipsManager->m_pClientContext, pMessage, &PendingVerdict );
		if (!SUCCEEDED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "MKL_ReplyMessage return error = %x !!!", hResult));
			pfMemFree( NULL, &pMessage );

			if (hResult == 0x801f0020)
			{
				PR_TRACE((g_root, prtERROR, TR "time out,  message already skipped !!!"));
				hResult = S_OK;
			}
			continue;
		}

		//PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: m_FilterID = %d, m_FunctionMj = 0x%x, m_FunctionMi = 0x%x",
		//	pEventHdr->m_FilterID, pEventHdr->m_FunctionMj, pEventHdr->m_FunctionMi));

		pVerdict = &AllowVerdict;
		switch (pEventHdr->m_FunctionMj) {
			case MJ_CREATE_PROCESS_NOTIFY:
			{
				// process start
				PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: MJ_CREATE_PROCESS_NOTIFY"));
				ULONG ProcessId = 0;
				PWCHAR pwchProcessFileName = NULL;

				PSINGLE_PARAM pParamProcName = MKL_GetEventParam (pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE);

				if (pParamProcName && pParamProcName->ParamSize)
					pwchProcessFileName = (PWCHAR) pParamProcName->ParamValue;

				PSINGLE_PARAM pParamTmp = MKL_GetEventParam (
					pMessage,
					MessageSize,
					_PARAM_OBJECT_SOURCE_ID,
					FALSE
					);

				if (pParamTmp && pParamTmp->ParamSize == sizeof(ULONG))
					ProcessId = *(ULONG*) pParamTmp->ParamValue;

				if (ProcessId && pwchProcessFileName)
				{
					PR_TRACE(( g_root, prtIMPORTANT, TR "Process created: '%S' pid %d (parent pid %d)", 
						pwchProcessFileName,
						(ULONG) ProcessId,
						(ULONG) pEventHdr->m_ProcessId
						));

					BYTE ProcHash[HIPS_HASH_SIZE];
					PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "before MKL_QueryProcessHash (m_pClientContext = %x, ProcessId = %d)",
						pHipsManager->m_pClientContext,
						ProcessId
						));
					//	get proc hash by PID
					hResult = MKL_QueryProcessHash(pHipsManager->m_pClientContext, ProcessId, &ProcHash[0], HIPS_HASH_SIZE);
					PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "after MKL_QueryProcessHash (hResult = %x)", hResult));
					if (SUCCEEDED(hResult))
					{
						//	put request to thread pool
						tTaskId task_info;
						//	set verdict before add task
						PendingVerdict.m_ExpTime = 500;
						hResult = MKL_SetVerdict( pHipsManager->m_pClientContext, pMessage, &PendingVerdict);
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: MKL_SetVerdict(PendingVerdict(500)) return %x", hResult));
						hResult = S_OK;
						pVerdict = 0;	// !!!

						CallBackHipsStruct * Str = new CallBackHipsStruct;
						Str->m_pContext = pHipsManager->m_pClientContext;
						Str->m_pMessage = pMessage;
						Str->m_MessageSize = MessageSize;
						Str->m_pHipsManager = pHipsManager;
						Str->m_pHipsTask = pTask;

						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x",
							Str->m_pContext,
							Str->m_pMessage,
							Str->m_pHipsManager));

						if (pwchProcessFileName)
						{
							Str->m_SrcProcName = new wchar_t[wcslen(pwchProcessFileName) + 1];
							wcsncpy(Str->m_SrcProcName, pwchProcessFileName, wcslen(pwchProcessFileName) + 1);
						}
						memcpy(Str->m_ProcHash, ProcHash, HIPS_HASH_SIZE);

						if (PR_SUCC(pHipsManager->m_ThPoolProcNotify->AddTask (
															&task_info,
															ThreadProcNotifyCallback,
															&Str,
															sizeof(CallBackHipsStruct*),
															0
															)))
						{
							PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: AddTask OK"));
						}
						else
						{
							PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: AddTask FAIL"));
						}
					}
				}
				break;
			} // case MJ_CREATE_PROCESS_NOTIFY
			case MJ_SYSTEM_KLFLTDEV:
			{
				// plug device
				PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: MJ_SYSTEM_KLFLTDEV"));
				//	put request to thread pool
				tTaskId task_info;
				//	set verdict before add task
				PendingVerdict.m_ExpTime = 500;
				hResult = MKL_SetVerdict( pHipsManager->m_pClientContext, pMessage, &PendingVerdict);
				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: MKL_SetVerdict(PendingVerdict(500)) return %x", hResult));
				hResult = S_OK;
				pVerdict = 0;	// !!!

				CallBackHipsStruct * Str = new CallBackHipsStruct;
				Str->m_pContext = pHipsManager->m_pClientContext;
				Str->m_pMessage = pMessage;
				Str->m_MessageSize = MessageSize;
				Str->m_pHipsManager = pHipsManager;
				Str->m_pHipsTask = pTask;

				Str->m_FunctionMj = pEventHdr->m_FunctionMj;
				Str->m_FunctionMi = pEventHdr->m_FunctionMi;
				Str->m_SrcPID = pEventHdr->m_ProcessId;

				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x, Str->m_ProcessId = %d, Str->m_FunctionMj = %x",
					Str->m_pContext,
					Str->m_pMessage,
					Str->m_pHipsManager,
					Str->m_SrcPID,
					Str->m_FunctionMj));

				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: before AddTask"));
				if (PR_SUCC(pHipsManager->m_ThPoolOtherEv->AddTask (
													&task_info,
													ThreadOtherEvCallback,
													&Str,
													sizeof(CallBackHipsStruct*),
													0
													)))
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: AddTask OK"));
				}
				else
				{
					PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: AddTask FAIL"));
				}
				break;
			}
			case MJ_SYSTEM_WRITE_PROCESS_MEMORY:
			case MJ_SYSTEM_TERMINATE_PROCESS:
			case MJ_SYSTEM_OPEN_PROCESS:
			case MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX:
			case MJ_SYSTEM_SET_THREAD_CONTEXT:
			//case MJ_SYSTEM_CREATE_THREAD:
			case MJ_SYSTEM_CREATE_REMOTE_THREAD:
			case MJ_SYSTEM_SHUTDOWN:
			case MJ_SYSTEM_KEYLOGGER_DETECTED:
			case IRP_MJ_SET_INFORMATION:
			case MJ_EXIT_PROCESS:
			{
				//	write to memory
				//PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: MJ_SYSTEM_WRITE_PROCESS_MEMORY || MJ_SYSTEM_TERMINATE_PROCESS  (pMessage = %x, MessageSize = %x)",
				//	pMessage, MessageSize
				//	));

				//	put request to thread pool
				tTaskId task_info;
				//	set verdict before add task
				PendingVerdict.m_ExpTime = 500;
				hResult = MKL_SetVerdict( pHipsManager->m_pClientContext, pMessage, &PendingVerdict);
				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: MKL_SetVerdict(PendingVerdict(500)) return %x", hResult));
				hResult = S_OK;
				pVerdict = 0;	// !!!

				CallBackHipsStruct * Str = new CallBackHipsStruct;
				Str->m_pContext = pHipsManager->m_pClientContext;
				Str->m_pMessage = pMessage;
				Str->m_pHipsManager = pHipsManager;
				Str->m_pHipsTask = pTask;

				Str->m_FunctionMj = pEventHdr->m_FunctionMj;
				Str->m_FunctionMi = pEventHdr->m_FunctionMi;
				Str->m_MessageSize = MessageSize;
				Str->m_SrcPID = pEventHdr->m_ProcessId;

				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x, Str->m_ProcessId = %d, Str->m_FunctionMj = %x",
					Str->m_pContext,
					Str->m_pMessage,
					Str->m_pHipsManager,
					Str->m_SrcPID,
					Str->m_FunctionMj));

				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: before AddTask"));
				if (PR_SUCC(pHipsManager->m_ThPoolOtherEv->AddTask (
													&task_info,
													ThreadOtherEvCallback,
													&Str,
													sizeof(CallBackHipsStruct*),
													0
													)))
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: AddTask OK"));
				}
				else
				{
					PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: AddTask FAIL"));
				}
				break;
			}
			case MJ_SYSTEM_HIPS:
			{
				PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: MJ_SYSTEM_HIPS"));
				//	pid
				//pParamPID = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_CLIENTID1, FALSE );
				//	obj name (file name)
				pObject = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
				//	value name
				pObjectEx = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_PARAM_W, FALSE );
				//	rule id
				pRuleID = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_SOURCE_ID, FALSE );
				//	desired operation
				pDesiredOperation = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_ACCESSATTR, FALSE );
				//	result
				pParamResult = MKL_GetEventParam( pMessage, MessageSize, _PARAM_RET_STATUS, FALSE );

				pParamContextFlags = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_CONTEXT_FLAGS, FALSE );
				if (pParamContextFlags && (sizeof(ULONG) == pParamContextFlags->ParamSize))
					ContextFlags = *(PULONG) pParamContextFlags->ParamValue;


				//	ProcName
				ProcName = 0;
				hResult = MKL_GetProcessName(pHipsManager->m_pClientContext, pMessage, &ProcName);

				switch (pEventHdr->m_FunctionMi) {
					case MI_SYSTEM_HIPS_LOG: 
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: MI_SYSTEM_HIPS_LOG"));
						if (pObject && /*pParamPID && */pParamResult)
						{
							//wchar_t OutErrorMsg[100];
							//PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: *** ProcName=%S (PID=%d), Action = %S,  ResName = %S(%S), result = %S",
							//	ProcName,
							//	pEventHdr->m_ProcessId//;(pParamPID) ? *(DWORD*)pParamPID->ParamValue : 0,
							//	(pDesiredOperation) ? CHipsManager::GetStrDesiredOperation(*(DWORD*)pDesiredOperation->ParamValue, OutErrorMsg) : L"Unknown",
							//	(pObject) ? (wchar_t*)pObject->ParamValue : 0,
							//	(pObjectEx) ? (wchar_t*)pObjectEx->ParamValue : 0,
							//	(pParamResult) ? CHipsManager::GetStrRes(*(DWORD*)pParamResult->ParamValue) : 0
							//	));

							//	put request to thread pool
							tTaskId task_info;
							//	set verdict before add task
							PendingVerdict.m_ExpTime = 3;
							hResult = MKL_SetVerdict( pHipsManager->m_pClientContext, pMessage, &PendingVerdict);
							PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: MKL_SetVerdict(PendingVerdict(3)) return %x", hResult));
							hResult = S_OK;
							pVerdict = 0;	// !!!

							CallBackHipsStruct * Str = new CallBackHipsStruct;
							Str->m_pContext = pHipsManager->m_pClientContext;
							Str->m_pMessage = pMessage;
							Str->m_MessageSize = MessageSize;
							Str->m_ContextFlags = ContextFlags;
							Str->m_pHipsManager = pHipsManager;
							Str->m_pHipsTask = pTask;
							Str->m_SrcPID = pEventHdr->m_ProcessId;

							Str->m_RuleID = (pRuleID) ? *(DWORD*)pRuleID->ParamValue : 0;
							StrAssign(ProcName, &Str->m_SrcProcName, wcslen(ProcName) + 1);
							//Str->m_SrcPID = (pParamPID) ? *(DWORD*)pParamPID->ParamValue : 0;
							Str->m_Action = (pDesiredOperation) ? (*(DWORD*)pDesiredOperation->ParamValue) : 0;
							Str->m_Result = (pParamResult) ? (*(DWORD*)pParamResult->ParamValue) : 0;
							StrAssign((pObject) ? (wchar_t*)pObject->ParamValue : 0, &Str->m_ObjectName1);
							StrAssign((pObjectEx) ? (wchar_t*)pObjectEx->ParamValue : 0, &Str->m_ObjectName2);

							PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x",
								Str->m_pContext,
								Str->m_pMessage,
								Str->m_pHipsManager));

							if (PR_SUCC(pHipsManager->m_ThPoolLog->AddTask (
																&task_info,
																ThreadLogCallback,
																&Str,
																sizeof(CallBackHipsStruct*),
																0
																)))
							{
								PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: AddTask OK"));
							}
							else
							{
								PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: AddTask FAIL"));
							}
						}
						break;
					}//	case MI_SYSTEM_HIPS_LOG
					case MI_SYSTEM_HIPS_ASK:
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: MI_SYSTEM_HIPS_ASK"));
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::JobThreadProc:: need ask user, set PendingVerdict"));

						//	set verdict before add task
						PendingVerdict.m_ExpTime = 1000;
						hResult = MKL_SetVerdict( pHipsManager->m_pClientContext, pMessage, &PendingVerdict);
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: MKL_SetVerdict(PendingVerdict(1000)) return %x", hResult));
						hResult = S_OK;
						pVerdict = 0;	// !!!

						tTaskId task_info;
						CallBackHipsStruct * Str = new CallBackHipsStruct;
						Str->m_pContext = pHipsManager->m_pClientContext;
						Str->m_pMessage = pMessage;
						Str->m_MessageSize = MessageSize;
						Str->m_ContextFlags = ContextFlags;
						Str->m_pHipsManager = pHipsManager;
						Str->m_SrcPID = pEventHdr->m_ProcessId;

						Str->m_RuleID = (pRuleID) ? *(DWORD*)pRuleID->ParamValue : 0;
						Str->m_Action = (pDesiredOperation) ? (*(DWORD*)pDesiredOperation->ParamValue) : 0;
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x, Str->m_Action = %d",
							Str->m_pContext,
							Str->m_pMessage,
							Str->m_pHipsManager,
							Str->m_Action));
						//Str->m_SrcPID = (pParamPID) ? *(DWORD*)pParamPID->ParamValue : 0;

						wchar_t * pTempObjName = 0;
						pTempObjName = (pObject) ? (wchar_t*)pObject->ParamValue : 0;

						if (pTempObjName)
						{
							Str->m_ObjectName1 = new wchar_t[wcslen(pTempObjName) + 1];
							wcsncpy(Str->m_ObjectName1, pTempObjName, wcslen(pTempObjName) + 1);
						}

						if (PR_SUCC(pHipsManager->m_ThPoolInfo->AddTask (
															&task_info,
															ThreadInfoCallback,
															&Str,
															sizeof(CallBackHipsStruct*),
															0
															)))
						{
							PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::JobThreadProc:: AddTask OK"));
						}
						else
						{
							PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: AddTask FAIL"));
						}

					
						break;
					}// MI_SYSTEM_HIPS_ASK
					default:
					{
						PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: wrong m_FunctionMi(%d)",
							pEventHdr->m_FunctionMi));
					}
				};//switch (pEventHdr->m_FunctionMi)
				break;
			}//case MJ_SYSTEM_HIPS
			default:
			{
				PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: wrong m_FunctionMj(%d)",
					pEventHdr->m_FunctionMj));
			}
		}//switch (pEventHdr->m_FunctionMj)

		if (pVerdict)
		{
			hResult = MKL_SetVerdict( pHipsManager->m_pClientContext, pMessage, pVerdict );
			pfMemFree(NULL, &pMessage);
		}
		if (!SUCCEEDED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "MKL_SetVerdict(pVerdict) return error = %x", hResult));
			break;
		}
	}//while 

	if (!pHipsManager->m_IsNeedToStopAll)
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: unexpected exit, res=%08X", hResult));
		pTask->sysSendMsg(pmc_TASK_STATE_CHANGED, TASK_STATE_FAILED, 0, 0, 0);
	}

	PR_TRACE((g_root, prtERROR, TR "HIPS::JobThreadProc:: end"));
	return 0;
}
////////////////////////////////////////////////////////////////////////////
//	reload all rules to driver from internal lists
////////////////////////////////////////////////////////////////////////////
bool CHipsTask::CheckProcessOnStart()
{
	//DebugBreak();
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::CheckProcessOnStart:: start"));
#ifdef HIPS_DEBUG_WITHOUT_DRV
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsTask::CheckProcessOnStart:: HIPS_DEBUG_WITHOUT_DRV, return"));
	return false;
#endif
	tERROR error = errOK;
	if (m_pProcMonObj == 0)
	{
		error = m_tm->GetService(TASKID_TM_ITSELF,(hOBJECT)this, AVP_SERVICE_PROCESS_MONITOR, (cObject**)&m_pProcMonObj);
		if (error != errOK)
		{
			m_pProcMonObj=0;
			PR_TRACE((g_root, prtERROR, TR "CHipsTask::CheckProcessOnStart:: GetService FAIL !!!"));
			return false;
		}
	}


	PVOID pProcessInfo = NULL;
	ULONG ProcessInfoLen = 0;
	bool IsNeedToReloadRule = false;

	HRESULT hResult = MKL_QueryProcessesInfo(
		m_pHipsManager->m_pClientContext, &pProcessInfo, &ProcessInfoLen );

	PR_TRACE(( g_root, prtERROR, TR "CHipsTask::CheckProcessOnStart:: QueryProcessesInfo: 0x%x", hResult ));
	if (!SUCCEEDED(hResult) )
		return IsNeedToReloadRule;

	ULONG EnumContext = 0;
	ULONG ProcessId = 0;
	ULONG ParentProcessId = 0;
	LARGE_INTEGER StartTime;
	PWCHAR pwchImagePath = NULL;
	PWCHAR pwchBaseFolder = NULL;
	PWCHAR pwchCmdLine = NULL;

	StartTime.QuadPart = 0;

	BYTE ProcHash[HIPS_HASH_SIZE];

	while( true)
	{
		hResult = MKL_EnumProcessInfo (
			pProcessInfo,
			ProcessInfoLen,
			&EnumContext,
			&ProcessId,
			&ParentProcessId,
			&StartTime,
			&pwchImagePath,
			&pwchBaseFolder,
			&pwchCmdLine
			);

		if (!SUCCEEDED(hResult) )
			break;
		
		PR_TRACE(( g_root, prtERROR, TR "CHipsTask::CheckProcessOnStart::\tpid %d(parent %d) from '%S'", 
			ProcessId,
			ParentProcessId,
			pwchImagePath
			));
		
		memset(ProcHash, 0, HIPS_HASH_SIZE);
		PR_TRACE(( g_root, prtIMPORTANT, TR "before MKL_QueryProcessHash (m_pClientContext = %x, ProcessId = %d)",
			m_pHipsManager->m_pClientContext,
			ProcessId
			));
		hResult = MKL_QueryProcessHash(
			m_pHipsManager->m_pClientContext,
			ProcessId,
			&ProcHash[0],
			HIPS_HASH_SIZE);
		PR_TRACE(( g_root, prtIMPORTANT, TR "after MKL_QueryProcessHash (hResult = %x)", hResult));
		if (SUCCEEDED(hResult))
		{
			m_pHipsManager->m_LocalCash.AddNewProcess(ProcessId, true);

			cProcMonNotifyEx Rating;
			if (PR_FAIL(error = m_pProcMonObj->GetProcessInfoEx(ProcessId, &Rating.m_SecRating)))
			{
				PR_TRACE(( g_root, prtIMPORTANT, TR "GetProcessInfoEx FAIL !!!  (error = %x)", error));
				return false;
			}

			Rating.m_ProcessId = ProcessId;
			Rating.m_sImagePath = pwchImagePath;

			if (m_pHipsManager->CheckRuleForAppNew(&Rating, true) && (!IsNeedToReloadRule))
				IsNeedToReloadRule = true;
		}

		//PR_TRACE(( g_root, prtERROR, TR "\t\t'%S'", pwchBaseFolder ));
		//PR_TRACE(( g_root, prtERROR, TR "\t\t'%S'", pwchCmdLine ));

//		QueryModInfo( pInternal, ProcessId );
	}

	pfMemFree( NULL, &pProcessInfo );
	return IsNeedToReloadRule;
}

////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
bool CHipsTask::MakeFileName (
	PVOID pClientContext,
	PVOID pMessage,
	ULONG MessageSize,
	ULONG ContextFlags,
	PWCHAR* ppwchFileName,
	PULONG pFileNameLen,
	bool bDestination
	)
{
	PWCHAR pwchFileName = NULL;

	int flen = 0;
	int vlen = 0;

	ULONG ParamID = bDestination ? _PARAM_OBJECT_URL_DEST_W : _PARAM_OBJECT_URL_W;
	ULONG ParamVolumeID = bDestination ? _PARAM_OBJECT_VOLUME_NAME_DEST_W : _PARAM_OBJECT_VOLUME_NAME_W;

	WCHAR pwchDosName[MAX_PATH + 1];
	memset( pwchDosName, 0, sizeof(pwchDosName) );

	*ppwchFileName = NULL;
	*pFileNameLen = 0;

	PSINGLE_PARAM pParamFileName = MKL_GetEventParam( pMessage, MessageSize, ParamID, FALSE );
	if (pParamFileName && pParamFileName->ParamSize)
		pwchFileName = (PWCHAR) pParamFileName->ParamValue;

	if (!pwchFileName)
		return false;

	if (ContextFlags & _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE)
	{
		cStringObj strtmp = pwchFileName;
		strtmp.replace_one(L"\\Device\\LanmanRedirector", fSTRING_COMPARE_CASE_INSENSITIVE, L"\\");
		strtmp.replace_one(L"\\Device\\Mup", fSTRING_COMPARE_CASE_INSENSITIVE, L"\\");
		
		flen = (strtmp.length() + 1) * sizeof(WCHAR);
		*ppwchFileName = (PWCHAR) pfMemAlloc(NULL, flen, 0 );
		if (*ppwchFileName)
		{
			HRESULT hResult = StringCbCopyW (
				*ppwchFileName,
				flen,
				strtmp.data()
				);

			if (S_OK == hResult)
			{
				*pFileNameLen = flen;
				return true;
			}

			PR_TRACE(( g_root, prtERROR, TR "Cannot build network name - string ops failed" ));

			pfMemFree( NULL, (void**) ppwchFileName );
		 
			return false;
		}
	}
	
	PWCHAR pwchVolumeName = NULL;
	
	PSINGLE_PARAM pParamVolumeName = MKL_GetEventParam( pMessage, MessageSize, ParamVolumeID, FALSE );
	if (pParamVolumeName && pParamVolumeName->ParamSize)
	{
		pwchVolumeName = (PWCHAR) pParamVolumeName->ParamValue;

		if (S_OK == MKL_GetVolumeName( pClientContext, pwchVolumeName, pwchDosName, sizeof(pwchDosName) / sizeof(WCHAR)))
		{
			//PR_TRACE(( g_root, prtERROR, TR "volume '%S' from '%S'", pwchDosName, pwchVolumeName ));
			vlen = lstrlenW( pwchDosName );

			flen = vlen * sizeof(WCHAR) + pParamFileName->ParamSize - pParamVolumeName->ParamSize + sizeof(WCHAR);
			*ppwchFileName = (PWCHAR) pfMemAlloc(NULL, flen, 0 );

			if (*ppwchFileName)
			{
				PWCHAR pwchTmp = *ppwchFileName;

				memcpy( pwchTmp, pwchDosName, vlen * sizeof(WCHAR) );
				
				HRESULT hResult = StringCbCopyW (
					pwchTmp + vlen,
					flen - vlen * sizeof(WCHAR),
					pwchFileName + pParamVolumeName->ParamSize / sizeof(WCHAR) - 1
					);

				if (S_OK == hResult)
				{
					*pFileNameLen = flen;
					return true;
				}

				PR_TRACE(( g_root, prtERROR, TR "Cannot build name - string ops failed" ));

				pfMemFree( NULL, (void**) ppwchFileName );

				return false;
			}
		}
	}

	*ppwchFileName = (PWCHAR) pfMemAlloc(NULL, pParamVolumeName->ParamSize, 0 );
	 if (*ppwchFileName)
	 {
		HRESULT hResult = StringCbCopyW (
			*ppwchFileName,
			pParamVolumeName->ParamSize / sizeof(WCHAR),
			pwchFileName
			);

		if (S_OK == hResult)
		{
			*pFileNameLen = pParamVolumeName->ParamSize;
			return true;
		}

		PR_TRACE(( g_root, prtERROR, TR "Cannot build default name - string ops failed" ));

		pfMemFree( NULL, (void**) ppwchFileName );
	 }

	return false;
}

//DWORD WINAPI UserInteractionProcessingProc(LPVOID lpParameter)
//{
//	return 0;
//	if (lpParameter == 0)
//	{
//		return 0;
//	}
//
//
//	PR_TRACE((g_root, prtERROR, TR "HIPS::UserInteractionProcessingProc:: start"));
//
//	CHipsManager * pHipsManager = (CHipsManager*)lpParameter;
//
//	wchar_t AppPath[MAX_PATH * 2];
//	wchar_t ResName[MAX_PATH * 2];
//	wchar_t ValueName[MAX_PATH * 2];
//
//	wchar_t TempMesage[3000];
//
//	MKLIF_REPLY_EVENT Verdict;
//	memset( &Verdict, 0, sizeof(Verdict) );
//	Verdict.m_VerdictFlags = efVerdict_Allow;
//	Verdict.m_ExpTime = 2;
//	PVOID pMessage = 0;
//
//	while (!pHipsManager->m_IsNeedToStopAll)
//	{
//		if (pHipsManager->m_UserQueue.GetFromQueue(AppPath, ResName, ValueName, &pMessage))
//		{
//			wsprintfW(TempMesage, L" App: %s\r\n ResName: %s\r\n ResValue: %s\r\n Allow action?",
//				(AppPath[0] == 0) ? L"unknown" : AppPath,
//				(ResName[0] == 0) ? L"unknown" : ResName,
//				(ValueName[0] == 0) ? L"unknown" : ValueName);
//			if (IDYES == MessageBoxW(0, TempMesage, L"HIPS", MB_YESNO))
//			{
//				//	need allow
//				Verdict.m_VerdictFlags = efVerdict_Allow;
//			}
//			else
//			{
//				//	need deny
//				Verdict.m_VerdictFlags = efVerdict_Deny;
//			}
//			MKL_SetVerdict( pHipsManager->m_pClientContext, pMessage, &Verdict );
//			pfMemFree(NULL, &pMessage);
//		}
//	}
//
//	return 0;
//}


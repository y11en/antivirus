// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  13 September 2006,  10:55 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <pr_vtbl.h>
#include <iface/i_reg.h>
#include <plugin/p_sfdb.h>
#include "pdm2imp.h"
#include <plugin/p_pdm2rt.h>
#include <Updater/updater.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable PDM : public cTask 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	hOBJECT    m_hTM;     // --
	tTaskState m_TaskState; // --
	tDWORD     m_SessionID; // --
	tDWORD     m_TaskID;  // --
	sInternal  m_Internal; // --

	hSECUREFILEDB		m_hSFDB_KL;		//	handle to "white list" base
// AVP Prague stamp end

private:
	bool CreateThreadPool (
		cThreadPool** ppThPool,
		tDWORD MaxLen,
		tThreadPriority Priority,
		tDWORD MinThreads,
		tDWORD MaxThreads
		);

	tERROR CreatePDMObject();
	void DeletePDMObject();
	void WaitLastEvent();

	tERROR ReloadSFDB(hSECUREFILEDB hSFDB, cStringObj* pSFDBPath, const bool lockLoadBasesMutex);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(PDM)
};
// AVP Prague stamp end


bool
PDM::CreateThreadPool (
	cThreadPool** ppThPool,
	tDWORD MaxLen,
	tThreadPriority Priority,
	tDWORD MinThreads,
	tDWORD MaxThreads
	)
{
	tERROR error = sysCreateObject( (hOBJECT*)ppThPool, IID_THREADPOOL, PID_THPOOLIMP );
	
	if (PR_FAIL( error ))
	{
		*ppThPool = 0;
		return false;
	}

	(*ppThPool)->set_pgTP_TASK_QUEUE_MAX_LEN( MaxLen );

	(*ppThPool)->set_pgTP_DEFAULT_THREAD_PRIORITY( Priority );
	(*ppThPool)->set_pgTP_MIN_THREADS( MinThreads );
	(*ppThPool)->set_pgTP_MAX_THREADS( MaxThreads );

	(*ppThPool)->set_pgTP_QUICK_EXECUTE( cFALSE );
	(*ppThPool)->set_pgTP_QUICK_DONE( cFALSE );

	(*ppThPool)->set_pgTP_THREAD_CALLBACK( OnThreadInitCallback );
	(*ppThPool)->set_pgTP_THREAD_CALLBACK_CONTEXT( &m_Internal );
	(*ppThPool)->set_pgTP_NAME( L"PDM2", 0, cCP_UNICODE );

	error = (*ppThPool)->sysCreateObjectDone();
	
	if (PR_FAIL(error))
	{
		(*ppThPool)->sysCloseObject();
		*ppThPool = 0;

		return false;
	}

	return true;
}

tERROR
PDM::CreatePDMObject()
{
	PR_TRACE((g_root, prtERROR, TR "CreatePDMObject"));

	m_Internal.m_pPDM = (Pdm2rtImp*) pfMemAlloc( NULL, sizeof(Pdm2rtImp), 0 );
	if (m_Internal.m_pPDM)
	{
		m_Internal.m_pPDM->Pdm2rtImp::Pdm2rtImp( NULL, m_hSFDB_KL, m_Internal.m_pThis);
		return errOK;
	}

	return errNOT_ENOUGH_MEMORY;
}

void
PDM::DeletePDMObject()
{
	if (m_Internal.m_pPDM)
	{
		PR_TRACE((g_root, prtERROR, TR "DeletePDMObject"));

		m_Internal.m_pPDM->~Pdm2rtImp();
		pfMemFree( NULL, (void**) &m_Internal.m_pPDM );
	}
}

void
PDM::WaitLastEvent()
{
	PSingleEvent pse = NULL;

	MKLIF_REPLY_EVENT DrvVerdict;	// default answer
	memset( &DrvVerdict, 0, sizeof(DrvVerdict) );
	
	while(true)
	{
		pse = NULL;

		m_Internal.m_EventSync.LockSync();
		
		if (!IsListEmpty( &m_Internal.m_Events ))
		{
			PLIST_ENTRY Flink = m_Internal.m_Events.Flink;

			PEventRoot pRoot = NULL;
			while (Flink != &m_Internal.m_Events)
			{
				pRoot = CONTAINING_RECORD( Flink, EventRoot, m_List );
				Flink = Flink->Flink;

				if (!IsListEmpty( &pRoot->m_EventList ))
				{
					pse = CONTAINING_RECORD( pRoot->m_EventList.Flink, SingleEvent, m_List );

					RemoveEntryList( &pse->m_List );
				
					break;
				}
			}
		}

		m_Internal.m_EventSync.UnLockSync();

		if (!pse)
			break; // no events found
		else
		{
			HRESULT hResult = MKL_ReplyMessage( m_Internal.m_pClientContext, pse->m_pMessage, &DrvVerdict );
			if (!SUCCEEDED(hResult) )
			{
				PR_TRACE((g_root, prtERROR, TR "WaitLastEvent: skip event error (hResult): 0x%x", hResult));
			}
			
			pfMemFree( NULL, &pse->m_pMessage );
			pfMemFree(NULL, (void**) &pse );
		}
	}
}

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR PDM::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	// Place your code here

	PR_TRACE((g_root, prtERROR, TR "ObjectInit" ));

	m_Internal.m_pThis = (cObject*) this;

	InitializeListHead( &m_Internal.m_Events );

	#define _max_waiter_thread	8

	error = errOK;	
	HRESULT hResult = MKL_ClientRegister(
		&m_Internal.m_pClientContext, 
		KL_PDM,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (IS_ERROR( hResult))
	{
		error = errUNEXPECTED;
		PR_TRACE((g_root, prtERROR, TR "client registration failed, hresult 0x%x", hResult ));
	}
	else
	{
		hResult = MKL_BuildMultipleWait( m_Internal.m_pClientContext, _max_waiter_thread );
		if (IS_ERROR( hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "MKL_BuildMultipleWait failed, hresult 0x%x", hResult ));
			error = errUNEXPECTED;
		}
	}

	if (PR_SUCC( error) )
	{
		PR_TRACE((g_root, prtERROR, TR "internal 0x%x, context 0x%x",
			&m_Internal, m_Internal.m_pClientContext ));

		//////////////////////////////////////////////////////////////////////////
		PEventRoot pSystemProcess = (PEventRoot) pfMemAlloc( NULL, sizeof(EventRoot), 0 );
		if (pSystemProcess)
		{
			pSystemProcess->m_pid = 0;
			pSystemProcess->m_Thread = 0;
			InitializeListHead( &pSystemProcess->m_EventList );

			InsertHeadList( &m_Internal.m_Events, &pSystemProcess->m_List );
		}
		//////////////////////////////////////////////////////////////////////////

		if (!CreateThreadPool( &m_Internal.m_ThPoolDrv, 1, TP_THREADPRIORITY_TIME_CRITICAL, 1, 1)
			||
			!CreateThreadPool( &m_Internal.m_ThPoolEvents, 8, TP_THREADPRIORITY_TIME_CRITICAL, 3, _max_waiter_thread)
			||
			!CreateThreadPool( &m_Internal.m_ThPoolInfo, 0, TP_THREADPRIORITY_TIME_CRITICAL, 0, 1)
			)
		{
			if (m_Internal.m_ThPoolEvents)
			{
				m_Internal.m_ThPoolEvents->sysCloseObject();
				m_Internal.m_ThPoolEvents = 0;
			}

			if (m_Internal.m_ThPoolDrv)
			{
				m_Internal.m_ThPoolDrv->sysCloseObject();
				m_Internal.m_ThPoolDrv = 0;
			}

			if (m_Internal.m_ThPoolInfo)
			{
				m_Internal.m_ThPoolInfo->sysCloseObject();
				m_Internal.m_ThPoolInfo = 0;
			}
			
			PR_TRACE((g_root, prtERROR, TR "create thpools failed" ));

			return errOBJECT_CANNOT_BE_INITIALIZED;
		}

		//////////////////////////////////////////////////////////////////////////
	}
//<<
	if (PR_SUCC( error) )
	{
		error = sysCreateObjectQuick((hOBJECT*)&m_hSFDB_KL, IID_SECUREFILEDB, PID_SFDB);
		if ( PR_SUCC(error) )
			error = m_hSFDB_KL->propSetDWord( plSERIALIZE_OPTIONS, cSFDB_SERIALIZE_CHECKSUM | cSFDB_SERIALIZE_CRYPT );
		if (PR_FAIL(error))
		{
			PR_TRACE((g_root, prtERROR, TR "KL  cannot be initialized, %terr", error));
		}
	}
//>>
	PR_TRACE((g_root, prtERROR, TR "ObjectInit %terr 0x%x, hresult 0x%x", error, hResult ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR PDM::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here
	hOBJECT hProcMon = NULL;
	tERROR errtmp;
	
	errtmp = ((cTaskManager*)m_hTM)->GetService (
		TASKID_TM_ITSELF,
		(hOBJECT) this,
		AVP_SERVICE_PROCESS_MONITOR,
		(hOBJECT*) &hProcMon,
		cREQUEST_SYNCHRONOUS
		);

	PR_TRACE((g_root, prtERROR, TR "GetService procmon %terr", errtmp));

	ULONG ApiVersion = 0;
	ULONG AppId = 0;

	HRESULT hResult = MKL_GetDriverApiVersion( m_Internal.m_pClientContext, &ApiVersion );
	if (SUCCEEDED( hResult ))
		PR_TRACE((g_root, prtERROR, TR "Mklif API version: %d", ApiVersion));

	hResult = MKL_GetAppId ( m_Internal.m_pClientContext, &AppId );
	if (SUCCEEDED( hResult ))
		PR_TRACE(( g_root, prtERROR, TR "Client registered with id: %d (0x%x)", AppId, AppId ));

	tTaskId task_drv;
	tPTR pData = &m_Internal;

	error = m_Internal.m_ThPoolDrv->AddTask( &task_drv, ThreadDrvCallback, &pData, sizeof(pData), 0 );

	if (PR_SUCC(( error )))
	{
		ULONG FltId;

		ULONG ReqTimeout = 5; // 5 sec to set preverdict
		
		hResult = MKL_AddFilter( &FltId, m_Internal.m_pClientContext, "*", ReqTimeout, 
			FLT_A_INFO, FLTTYPE_SYSTEM, MJ_EXIT_PROCESS, 0,
			0, PostProcessing, NULL, NULL );

		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter( &FltId, m_Internal.m_pClientContext, "*", ReqTimeout, 
				FLT_A_INFO, FLTTYPE_SYSTEM, MJ_CREATE_PROCESS_NOTIFY, 0,
				0, PostProcessing, NULL, NULL );

		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter( &FltId, m_Internal.m_pClientContext, "*", ReqTimeout, 
				FLT_A_INFO, FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_THREAD, 0,
				0, PostProcessing, NULL, NULL );

		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter( &FltId, m_Internal.m_pClientContext, "*", ReqTimeout, 
				FLT_A_INFO, FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_THREAD_NOTIFY, 0,
				0, PostProcessing, NULL, NULL );

		if (SUCCEEDED( hResult ))
		{
			char param_buf[sizeof(FILTER_PARAM) + sizeof(ULONG)];
			PFILTER_PARAM pContextFlags = (PFILTER_PARAM) param_buf;
			
			pContextFlags->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
			pContextFlags->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			pContextFlags->m_ParamFlt = FLT_PARAM_AND;
			pContextFlags->m_ParamSize = sizeof(ULONG);
			*(ULONG*)pContextFlags->m_ParamValue = _CONTEXT_OBJECT_FLAG_MODIFIED;

			hResult = MKL_AddFilter ( &FltId, m_Internal.m_pClientContext, "*", ReqTimeout, 
				FLT_A_POPUP, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0,
				0, AnyProcessing, NULL, pContextFlags, NULL
				);
		}

		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter ( &FltId, m_Internal.m_pClientContext, "*", ReqTimeout, 
				FLT_A_POPUP, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation,
				0, AnyProcessing, NULL, NULL
				);

		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter( &FltId, m_Internal.m_pClientContext, "*", ReqTimeout, 
				FLT_A_POPUP, FLTTYPE_REGS, Interceptor_SetValueKey, 0,
				0, AnyProcessing, NULL, NULL );

		if (!SUCCEEDED( hResult ))
		{
			PR_TRACE(( g_root, prtERROR, TR "register filters failed: 0x%x", hResult ));
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		}
	}

	if (PR_SUCC( error ))
	{
		sysRegisterMsgHandler (
			pmc_PRODUCT,
			rmhLISTENER,
			sysGetRoot(),
			IID_ANY,
			PID_ANY,
			IID_ANY,
			PID_ANY
			);
		sysRegisterMsgHandler(pmc_UPDATER, rmhDECIDER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	}

	PR_TRACE((g_root, prtERROR, TR "ObjectInitDone %terr", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR PDM::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	error = errOK;

	PR_TRACE((g_root, prtERROR, TR "closing prague objects..."));

	if (m_Internal.m_ThPoolDrv)
	{
		m_Internal.m_ThPoolDrv->sysCloseObject();
		m_Internal.m_ThPoolDrv = NULL;
	}

	if (m_Internal.m_ThPoolEvents)
	{
		m_Internal.m_ThPoolEvents->sysCloseObject();
		m_Internal.m_ThPoolEvents = NULL;
	}

	if (m_Internal.m_ThPoolInfo)
	{
		m_Internal.m_ThPoolInfo->sysCloseObject();
		m_Internal.m_ThPoolInfo = NULL;
	}

	if (m_Internal.m_pClientContext)
	{
		PR_TRACE((g_root, prtERROR, TR "unregister client"));
		MKL_ClientUnregister( &m_Internal.m_pClientContext );
	}

	m_Internal.m_EventSync.LockSync();
	while (true)
	{
		if (IsListEmpty( &m_Internal.m_Events ))
			break;

		PLIST_ENTRY Flink = m_Internal.m_Events.Flink;

		PEventRoot pRoot = CONTAINING_RECORD( Flink, EventRoot, m_List );

		RemoveEntryList( &pRoot->m_List );

		PR_TRACE(( g_root, prtERROR, TR "free events root for pid %d", pRoot->m_pid ));

		pfMemFree( NULL, (void**) &pRoot );
	}
	
	m_Internal.m_EventSync.UnLockSync();

	PR_TRACE((g_root, prtERROR, TR "ObjectPreClose %terr", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR PDM::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectClose method" );

	// Place your code here

	PR_TRACE((g_root, prtERROR, TR "ObjectClose %terr (mem usage %d)", error, gMemCounter ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR PDM::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;//errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here
	PR_TRACE((g_root, prtERROR, TR "MsgReceive %terr", error));

	if ((pmc_PRODUCT == p_msg_cls_id) && (pm_PRODUCT_TRUSTEDAPPS_CHANGED == p_msg_id))
	{
		PR_TRACE( ( g_root, prtIMPORTANT, TR "refresh trusted" ) );
		if (m_Internal.m_pPDM)
			m_Internal.m_pPDM->RefreshTrusted();
	}
//<<
	if(p_msg_cls_id == pmc_UPDATER && p_msg_id == pm_COMPONENT_PREINSTALLED)
	{
		PR_TRACE((g_root, prtERROR, TR "MsgReceive pmc_UPDATER"));
		cSerializable *pSer = p_par_buf_len == SER_SENDMSG_PSIZE ? (cSerializable *)p_par_buf : NULL;
		cUpdaterUpdatedFileList *updatedFiles = pSer && pSer->getIID() == cUpdaterUpdatedFileList::eIID ? 
			reinterpret_cast<cUpdaterUpdatedFileList *>(pSer) : NULL;
		if(!updatedFiles) {
			PR_TRACE((g_root, prtERROR, TR "MsgReceive pmc_UPDATER:: updatedFiles == 0"));
			return errPARAMETER_INVALID;
		}
		// reload 'SFDB' if Proactive Defence Component (Behaviour Blocker) is updated
		if(updatedFiles->m_selectedComponentIdentidier == cUPDATE_CATEGORY_PDM_BB)
		{
			PR_TRACE((g_root, prtERROR, TR "MsgReceive pmc_UPDATER:: cUPDATE_CATEGORY_PDM_BB"));
			PR_TRACE((this, prtIMPORTANT, TR "Reloading databases..."));
			
			cStringObj sss("%Bases%\\pdmkl.dat");
			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sss), 0, 0);
			// возврат ошибки означает откат обновления!
			error = ReloadSFDB(m_hSFDB_KL, &sss, false);
			PR_TRACE((this, prtIMPORTANT, TR "Database reloaded (%terr)", error));
		}
        return error;
	}
//>>
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR PDM::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here
	if (p_settings->isBasedOn(cPDM2rtSettings::eIID))
		error = m_Internal.m_Settings.assign(*p_settings, cFALSE);
	else
		error = errNOT_SUPPORTED;

	//<<--
	cStringObj sss("%Bases%\\pdmkl.dat");

	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sss), 0, 0);

	ReloadSFDB(m_hSFDB_KL, &sss, true);
	//-->>

	PR_TRACE((g_root, prtERROR, TR "SetSettings %terr", error));

	return error;
}
// AVP Prague stamp end

tERROR PDM::ReloadSFDB(hSECUREFILEDB hSFDB, cStringObj* pSFDBPath, const bool lockLoadBasesMutex) 
{
	tERROR error;
	tDWORD dwUserVersion; // AKA RecordIdAlg
	tDWORD dwRecordIDSize;
	tDWORD dwRecordDataSize;

	cBasesLoadLocker LoadLocker((cObject *)this, cUPDATE_CATEGORY_PDM_BB,
        lockLoadBasesMutex ? cBasesLoadLocker::withLock : cBasesLoadLocker::noLock);


	error = hSFDB->Load(pSFDBPath->c_str(cCP_ANSI));
	if (PR_SUCC(error))
		error =  hSFDB->GetDBInfo(&dwUserVersion, &dwRecordIDSize, &dwRecordDataSize);
	if (PR_SUCC(error))
	{
		if (dwUserVersion != 1 || dwRecordIDSize != 8 || dwRecordDataSize != 8)
		{
			PR_TRACE((g_root, prtERROR, TR "SFDB loaded, but data format incompatible"));
			error = errOBJECT_INCOMPATIBLE;
		}
	}
	
	if(PR_FAIL(error))
	{
		PR_TRACE((g_root, prtERROR, TR "SFDB load failed, trying InitNew"));
		error = hSFDB->InitNew(1, 8, 8, 0);
		if (PR_FAIL(error))
			PR_TRACE((g_root, prtERROR, TR "SFDB InitNew failed with %terr", error));
		if (PR_SUCC(error))
			pSFDBPath->copy(hSFDB, plSFDB_FILEPATH);
	}
	PR_TRACE((this, prtIMPORTANT, TR "Loading SFDB from %S %terr", pSFDBPath->data(), error));
	return error;
}


// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR PDM::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	if (p_settings->isBasedOn(cPDM2rtSettings::eIID))
		error = p_settings->assign( m_Internal.m_Settings, cTRUE );
	else
		error = errNOT_SUPPORTED;

	PR_TRACE((g_root, prtERROR, TR "GetSettings %terr", error));
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR PDM::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here
	PR_TRACE((g_root, prtERROR, TR "AskAction %terr", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR PDM::SetState( tTaskRequestState p_state )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	PR_TRACE((g_root, prtERROR, TR "request to change state: incoming value 0x%x", p_state));

	HRESULT hResult;

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		error = CreatePDMObject();
		if (PR_SUCC(error))
		{
			m_Internal.m_bStop = false;
			hResult = MKL_ChangeClientActiveStatus( m_Internal.m_pClientContext, TRUE );
			RegisterProcessesOnStart( &m_Internal );

			if (SUCCEEDED( hResult ))
			{
				m_TaskState = TASK_STATE_RUNNING;
				error = errOK;
			}
			else
			{
				m_TaskState = TASK_STATE_FAILED;
				error = errOBJECT_CANNOT_BE_INITIALIZED;
			}
		}
		break;
	
	case TASK_REQUEST_PAUSE:
		hResult = MKL_ChangeClientActiveStatus( m_Internal.m_pClientContext, FALSE );
		
		if (SUCCEEDED( hResult ))
		{
			WaitLastEvent();
			
			DeletePDMObject();

			m_TaskState = TASK_STATE_PAUSED;
			error = errOK;
		}
		else
			error = errUNEXPECTED;
		break;

	case TASK_REQUEST_STOP:

		PR_TRACE((g_root, prtERROR, TR "Stopping..."));

		m_Internal.m_bStop = true;
		hResult = MKL_ChangeClientActiveStatus( m_Internal.m_pClientContext, FALSE );
		
		WaitLastEvent();
		DeletePDMObject();

		error = errOK;

		m_TaskState = TASK_STATE_STOPPED;
		break;

	default:
		return errPARAMETER_INVALID;
	}
	
	PR_TRACE((g_root, prtERROR, TR "SetState %terr", error));
	
	if (PR_FAIL(error))
	{
		PR_TRACE((g_root, prtERROR, TR "SetState failed %#x", error));
	}
	else
		error = warnTASK_STATE_CHANGED;


	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR PDM::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
	
	if (p_statistics->isBasedOn(cPDM2rtStatistics::eIID))
		error = p_statistics->assign(m_Internal.m_Statistics, cTRUE);
	else
		error = errNOT_SUPPORTED;
	
	if (PR_FAIL( error ))
	{
		PR_TRACE((g_root, prtERROR, TR "GetStatistics %terr", error));
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR PDM::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "pdm2 imp", 9 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, PDM, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, PDM, m_TaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, PDM, m_SessionID, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, PDM, m_TaskID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
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
		PID_PDM2RT,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		1/*VID_SOBKO*/,
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(PDM)-sizeof(cObjImpl),           // memory size
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

tERROR pr_call Task_Register( hROOT root ) { return PDM::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end




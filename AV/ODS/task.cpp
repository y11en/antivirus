// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  27 April 2005,  15:19 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end

#define IMPEX_IMPORT_LIB

// AVP Prague stamp begin( Includes for interface,  )
#include "task.h"
// AVP Prague stamp end

#include <Prague/iface/e_loader.h> 
#include <Prague/plugin/p_win32_nfio.h>

#include <Extract/plugin/p_windiskio.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_loader.h> 
	#include <Prague/plugin/p_win32_nfio.h>
#if defined (_WIN32)
	#include <Prague/plugin/p_fsdrvplgn.h>
	#include <Extract/plugin/p_windiskio.h>
#endif
IMPORT_TABLE_END

#include "process.h"

#define  DISK_PERFORMANCE_CHECK_PERIOD    500
#define  DISK_PERFORMANCE_SLEEP_TIME      3

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
tERROR cODSImpl::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here
	m_hScanThread = new CScannerThread(this, false);
	m_hCounterThread = new CScannerThread(this, true);

	if( PR_SUCC(error) && m_hTM && !m_hAVS )
		error = m_hTM->GetService(TASKID_TM_ITSELF, (cObject*)this, sid_TM_AVS, (cObject**)&m_hAVS);

	if( !m_hAVS )
		error = errOBJECT_CANNOT_BE_INITIALIZED;

	if( PR_SUCC(error) )
		error = m_hAVS->CreateSession(&m_avsSession, (hTASK)this, cFALSE, OBJECT_ORIGIN_GENERIC);

	if( PR_SUCC(error) )
		error = init(this, 2);

	if( PR_SUCC(error) )
	{
		tPROPID nPropTaskId;
		if( PR_SUCC(g_root->RegisterCustomPropId(&nPropTaskId, cpTASK_MANAGER_TASK_ID, pTYPE_DWORD)) )
			m_nTaskId = propGetDWord(nPropTaskId);
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR cODSImpl::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	if( m_avsSession )
		m_avsSession->SetState(TASK_REQUEST_STOP);

	delete m_hScanThread;
	delete m_hCounterThread;

	de_init();

	if( m_avsTreats )
		m_hAVS->ReleaseTreats(m_avsTreats);

	if( m_avsSession )
		m_avsSession->sysCloseObject();

	if( m_hAVS && m_hTM )
		m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hAVS);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
tERROR cODSImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
tERROR cODSImpl::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here
	if( !p_settings )
		return errPARAMETER_INVALID;

	if( m_nState != TASK_STATE_UNKNOWN )
	{
		if( p_settings->isBasedOn(cODSSettings::eIID) )
			m_Settings.m_nABSNumber = ((cODSSettings*)p_settings)->m_nABSNumber;

		return errNOT_SUPPORTED;
	}

	if( p_settings->isBasedOn(cODSSettings::eIID) )
	{
		m_Settings = *(cODSSettings*)p_settings;

		if( !m_Settings.m_bRegisterThreats && m_Settings.m_nScanAction == SCAN_ACTION_POSTPONE )
			m_Settings.m_nScanAction = SCAN_ACTION_ASKUSER;

		m_hScanThread->InitScanners();
		m_hCounterThread->InitScanners();
	}
	else if( p_settings->isBasedOn(cODSStatistics::eIID) )
	{
		cODSStatistics& pStat = *(cODSStatistics*)p_settings;
		m_hScanThread->ReinitState(pStat);
		m_hCounterThread->ReinitState(pStat);
	}
	else
		return errNOT_SUPPORTED;

	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
tERROR cODSImpl::GetSettings( cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	if ( p_settings )
		error = p_settings->assign( m_Settings, false );
	else
		error = errPARAMETER_INVALID;

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
tERROR cODSImpl::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errUNEXPECTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	if( p_actionInfo && p_actionInfo->isBasedOn(cODSRuntimeParams::eIID) )
	{
		if( p_actionId == cODSRuntimeParams::SET )
			error = m_Params.assign(*p_actionInfo, true);
		else if( p_actionId == cODSRuntimeParams::GET )
			error = p_actionInfo->assign(m_Params, true);
	}

	// Place your code here
	return error;
}
// AVP Prague stamp end

tERROR cODSImpl::CheckPostponeScan()
{
	if( m_Params.m_bEnableStopMode )
		return errOK;

	if( m_Settings.m_nScanAction != SCAN_ACTION_POSTPONE )
		return errOK;

	if( !m_nTaskId || m_nState != TASK_STATE_COMPLETED /*|| !m_bSomeUntreated*/ )
		return errOK;

	if( PR_FAIL(m_hAVS->GetTreats(m_nTaskId, &m_avsTreats)) )
		return errOK;

	cAskProcessTreats pRecords;
	pRecords.m_bSyncScan = cTRUE;
	pRecords.m_bAskPassword = m_Settings.m_bAskPassword;

	m_avsTreats->Process(FILTER_NONE, &pRecords);
	return errOK;
}

tDWORD cODSImpl::GetCurCompletion(tDWORD* tmFinish)
{
	if( tmFinish )
		*tmFinish = 0;

	if( !m_hCounterThread )
		return 0;

	if( !m_hCounterThread->m_bCompleted )
		return tmFinish ? m_nStartPersent + 1 : m_nStartPersent;

	if( tmFinish && m_hScanThread->m_nObjCount )
	{
		int nScanTime = PrGetTickCount() - m_hScanThread->m_nStartTicks;
		int nEstimatedTime = nScanTime / m_hScanThread->m_nObjCount * m_hCounterThread->m_nObjCount;

		if( nEstimatedTime < nScanTime )
			nEstimatedTime = nScanTime;

		if( m_nState & STATE_FLAG_ACTIVE )
			*tmFinish = (tDWORD)(m_hScanThread->m_tmStart + nEstimatedTime / 1000);
	}

	if( m_hScanThread->m_bCompleted || !m_hCounterThread->m_nObjCount )
		return 100;

	if( m_nState == TASK_STATE_COMPLETED )
		return 99;

	tDWORD nPersent = m_hScanThread->m_nObjCount * (100 - m_nStartPersent) /
					m_hCounterThread->m_nObjCount + m_nStartPersent;

	if( nPersent >= 100 )
		nPersent = 99;
	else if( nPersent <= 0 )
		nPersent = tmFinish ? m_nStartPersent + 1 : m_nStartPersent;

	return nPersent;
}

tERROR cODSImpl::OnTaskStarted()
{
	tERROR error;
	if( PR_FAIL(error = m_avsSession->SetState(TASK_REQUEST_RUN)) )
	{
		m_nState = TASK_STATE_STOPPED;
		return error;
	}

	sysSendMsg(pmc_TASK_STATE_CHANGED, TASK_STATE_RUNNING, 0, 0, 0);

	if( m_Settings.m_bActiveDisinfection )
		sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_START, 0, 0, 0);

	return errOK;
}

tERROR cODSImpl::OnTaskCompleted()
{
	if( m_nState != TASK_STATE_STOPPED )
		m_nState = TASK_STATE_COMPLETED;

	CheckPostponeScan();

	sysSendMsg(pmc_TASK_STATE_CHANGED, m_nState, 0, 0, 0);

	if( m_hScanThread->m_hToken )
		m_hScanThread->m_hToken->Revert();

	if( m_nState == TASK_STATE_COMPLETED && m_Params.m_bEnableStopMode )
		switch(m_Params.m_eStopMode)
		{
		case cODSRuntimeParams::stop_Shutdown:  PrShutDownSystem(SHUTDOWN_SHUTDOWN|SHUTDOWN_FORCE); break;
		case cODSRuntimeParams::stop_Sleep:     PrShutDownSystem(SHUTDOWN_STANDBY|SHUTDOWN_FORCE); break;
		case cODSRuntimeParams::stop_Hibernate: PrShutDownSystem(SHUTDOWN_HIBERNATE|SHUTDOWN_FORCE); break;
		case cODSRuntimeParams::stop_Restart:
			if( m_Settings.m_bActiveDisinfection )
				sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_REBOOT, 0, 0, 0);
			PrShutDownSystem(SHUTDOWN_RESTART|SHUTDOWN_FORCE);
			break;
		}

	return errOK;
}

// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
//! tERROR cODSImpl::SetState( tTaskState p_state )
tERROR cODSImpl::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	tDWORD nOldState= m_nState;

	if( nOldState != TASK_STATE_UNKNOWN || p_state != TASK_REQUEST_RUN )
		m_avsSession->SetState(p_state);

	switch( p_state )
	{
		case TASK_REQUEST_RUN:
			m_nState = TASK_STATE_RUNNING;
			if( nOldState == TASK_STATE_UNKNOWN )
			{
				if( m_Settings.m_bCountProgress )
					m_hCounterThread->Start();
				m_hScanThread->Start();
			}
			else
				sysSendMsg(pmc_TASK_STATE_CHANGED, TASK_STATE_RUNNING, 0, 0, 0);
			break;
		case TASK_REQUEST_PAUSE:
			if( m_nState != TASK_STATE_COMPLETED )
				m_nState = TASK_STATE_PAUSED;
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_nState, 0, 0, 0);
			break;
		case TASK_REQUEST_STOP:
			if( m_nState == TASK_STATE_COMPLETED )
				sysSendMsg(pmc_TASK_STATE_CHANGED, m_nState, 0, 0, 0);
			else
				m_nState = TASK_STATE_STOPPED;

			if( m_avsTreats )
				m_avsTreats->ProcessCancel();
			break;
		default:
			return errTASK_STATE_UNKNOWN;
	}
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
tERROR cODSImpl::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
	if( !p_statistics )
		return errPARAMETER_INVALID;

	if( p_statistics->isBasedOn(cODSStatistics::eIID) )
	{
		if( m_avsSession )
			m_avsSession->GetStatistic(p_statistics);

		cODSStatistics* pStat = (cODSStatistics*)p_statistics;

		pStat->m_nCurObject = m_hScanThread->m_nCurObject;
		pStat->m_nReinitCompletion = m_hScanThread->m_nReinitCompletion;
		pStat->m_nReinitMask = m_hScanThread->m_nReinitMask;
		pStat->m_sReinitName = m_hScanThread->m_sReinitName;

		if( m_hScanThread->m_bCompleted )
			pStat->m_strCurObject.erase();
	}

	if( PR_SUCC(error) && m_hCounterThread )
	{
		cTaskStatistics * pStat = (cTaskStatistics*)p_statistics;
		pStat->m_timeStart = (tDWORD)m_hScanThread->m_tmStart;
		pStat->m_nPercentCompleted = GetCurCompletion(&pStat->m_timeFinish);
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR cODSImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "ODS new implementation", 23 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, cODSImpl, m_hTM, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, cODSImpl, m_nState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, cODSImpl, m_nSessionId, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, cODSImpl, m_nTaskId, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, cODSImpl, m_hStorage, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( plAVS, cODSImpl, m_hAVS, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
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
		PID_ODS,                                // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MEZHUEV,                            // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(cODSImpl)-sizeof(cObjImpl),      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

	if( PR_SUCC(error) )
		error = CObjectScanner::Init(root);

	if( PR_SUCC(error) )
		root->ResolveImportTable(NULL, import_table, PID_ODS);

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return cODSImpl::Register(root); }
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end


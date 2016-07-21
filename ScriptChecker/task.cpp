// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  23 September 2004,  19:20 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- Script Checker
// Author      -- Pavlushchik
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ScriptChecker/plugin/p_sc.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end

#include <time.h>

#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_mutex.h>
#include <AV/iface/i_avs.h>
#include <Prague/pr_remote.h>
#include <AV/structs/s_avs.h>
#include <Prague/plugin/p_tempfile.h>

extern tPROPID propid_object_detect_counter;

//#define EICAR_TEXT "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*"
//#define EICAR_TEXT_SIZE 0x44

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable ScriptChecker : public cTask
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
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
	hTASKMANAGER m_hTM;     // --
	tTaskState   m_TaskState; // --
	tDWORD       m_SessionID; // --
// AVP Prague stamp end



	tERROR SendReport(cDetectObjectInfo* pData);

	tTaskState m_ReqTaskState;
	hMUTEX m_hMutex; // used as flag of active component
	hAVS   m_hAvs;
	hAVSSESSION m_hSession;
	cProtectionSettings m_ProtectionSettings;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(ScriptChecker)
};
// AVP Prague stamp end



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
tERROR ScriptChecker::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR ScriptChecker::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	if (PR_SUCC(error))
		error = sysRegisterMsgHandler(pmc_SC_SCAN, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if (PR_SUCC(error))
		error = sysCreateObject((hOBJECT*)&m_hMutex, IID_MUTEX);
	if (PR_SUCC(error))
		error = m_hMutex->set_pgOBJECT_NAME("KLSCRIPTCHECKER_PR_DEF9362421", 0);
	if (PR_SUCC(error))
		error = m_hMutex->set_pgGLOBAL(cTRUE);
	if (PR_SUCC(error))
		error = m_hMutex->sysCreateObjectDone();
	if (PR_SUCC(error))
		error = m_hTM->GetService(TASKID_TM_ITSELF, (cObject*)this, sid_TM_AVS, (cObject**)&m_hAvs);
	if (PR_SUCC(error))
		error = m_hAvs->CreateSession(&m_hSession, (hTASK)this, cFALSE, OBJECT_ORIGIN_SCRIPT);
	if (PR_SUCC(error))
		error = PRRegisterObject("SCTask", *this);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR ScriptChecker::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	PRUnregisterObject((hOBJECT)this);

	if ( m_hSession && PR_SUCC(sysCheckObject((hOBJECT)m_hSession)) )
		m_hSession->sysCloseObject();

	if ( m_hAvs && PR_SUCC(sysCheckObject((hOBJECT)m_hAvs)) )
		m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hAvs);

	if (m_hMutex)
		m_hMutex->sysCloseObject();

	return error;
}
// AVP Prague stamp end



tERROR ScriptChecker::SendReport(cDetectObjectInfo* pData)
{
	return sysSendMsg(pmc_TASK_REPORT, 0, NULL, pData, SER_SENDMSG_PSIZE);
}

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
tERROR ScriptChecker::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;

	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	if (p_msg_cls_id == pmc_SC_SCAN && p_msg_id == msg_SC_SCAN)
	{
		if (m_TaskState == TASK_STATE_PAUSED)
			return errOK;

		if( m_TaskState != TASK_STATE_RUNNING)
			return errOPERATION_CANCELED;

		if (PR_SUCC(error = sysCheckObject(p_ctx, IID_IO)))
		{
			hIO pScanIO = (hIO)p_ctx;
			cProtectionStatistics stat;

#ifdef _PRAGUE_TRACE_
			{
				tDWORD dwNameSize;
				tCODEPAGE cp;
				cp = pScanIO->propGetDWord(pgOBJECT_FULL_NAME_CP);
				if (PR_SUCC(pScanIO->propGetStr(&dwNameSize, pgOBJECT_FULL_NAME, NULL, 0, cp)))
				{
					tPTR pName;
					if (PR_SUCC(heapAlloc(&pName, dwNameSize)))
					{
						if (PR_SUCC(pScanIO->propGetStr(&dwNameSize, pgOBJECT_FULL_NAME, pName, dwNameSize, cCP_ANSI)))
						{
							PR_TRACE((this, prtNOTIFY, "sc\tmsg_SC_SCAN: %s", pName ));
							heapFree(pName);
						}
					}
				}
			}
#endif

			cScanProcessInfo ProcessInfo;
			error = m_hSession->ProcessObject((hOBJECT)pScanIO, &ProcessInfo, &m_ProtectionSettings, &stat);

			tBOOL bDetected = !!(ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::DETECTED);

			if (p_par_buf_len && *p_par_buf_len >= sizeof(tDWORD))
			{
				*(tDWORD*)p_par_buf = bDetected;
				*p_par_buf_len = sizeof(tDWORD);
			}
			
			if (pScanIO != (hIO)p_ctx)
				pScanIO->sysCloseObject(); // delete IO with EICAR

			if (bDetected)
			{
				PR_TRACE((this, prtIMPORTANT, "sc\tmsg_SC_SCAN: detected " ));
				return errACCESS_DENIED;
			}
			return errOK_DECIDED;
		}
	}

	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR ScriptChecker::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	if ( !p_settings )
		error = errPARAMETER_INVALID;
	else
		error = m_ProtectionSettings.assign( *p_settings, true );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR ScriptChecker::GetSettings( cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	error = p_settings->assign(m_ProtectionSettings,true);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR ScriptChecker::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR ScriptChecker::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	if( p_state != TASK_REQUEST_PAUSE )
		if( PR_FAIL(error = m_hSession->SetState(p_state)) )
			return error;

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		m_TaskState = TASK_STATE_RUNNING;
		break;
	case TASK_REQUEST_PAUSE:
		m_TaskState = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		m_TaskState = TASK_STATE_STOPPED;
		break;
	default:
		return errPARAMETER_INVALID;
	}

	return warnTASK_STATE_CHANGED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR ScriptChecker::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	error = m_hSession->GetStatistic(p_statistics);
//	PR_TRACE((this, prtSPAM,"GetStat result %terr with generic %d (threats %d)", error, ((cAVSStatistics*)p_statistics)->m_nNumGenericScaned, ((cAVSStatistics*)p_statistics)->m_nNumThreats));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR ScriptChecker::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, ScriptChecker, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, ScriptChecker, m_TaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, ScriptChecker, m_SessionID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
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
		PID_SC,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MIKE,                               // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(ScriptChecker)-sizeof(cObjImpl), // memory size
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

tERROR pr_call Task_Register( hROOT root ) { return ScriptChecker::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_ID
// AVP Prague stamp end




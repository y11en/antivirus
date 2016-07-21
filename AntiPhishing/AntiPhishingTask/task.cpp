// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  04 April 2007,  19:44 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>

#include <AntiPhishing/plugin/p_antiphishingtask.h>
#include <AntiPhishing/plugin/p_antiphishing.h>

#include <ProductCore/iface/i_taskmanager.h>
// AVP Prague stamp end



PR_NEW_DECLARE;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable AntiPhishingTask : public cTask 
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
	hTASKMANAGER m_hTM;                 // --
	tTaskState   m_nTaskState;          // --
	tDWORD       m_nSessionId;          // --
	tDWORD       m_nTaskId;             // --
	tDWORD       m_nParentId;           // --
	tBOOL        m_bStopIfNoAPDatabase; // --
// AVP Prague stamp end



	hOBJECT      m_hTrafficMonitor;
	hTASK        m_hAntiPhishingService;

	tERROR AntiPhishingTask::UpdateTaskStatus(cTaskInfo* pTaskInfo, tTaskState nTaskState);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(AntiPhishingTask)
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
tERROR AntiPhishingTask::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	m_bStopIfNoAPDatabase = cTRUE;

	return error;
}
// AVP Prague stamp end




// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR AntiPhishingTask::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	if (!m_hTM)
		return errOBJECT_CANNOT_BE_INITIALIZED;

	m_nTaskState = TASK_STATE_CREATED;

	sysRegisterMsgHandler(pmc_ANTI_PHISHING_TASK, rmhDECIDER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	error = m_hTM->GetService(TASKID_TM_ITSELF, *this, AVP_SERVICE_TRAFFICMONITOR_HTTP, &m_hTrafficMonitor, cREQUEST_SYNCHRONOUS);
	PR_TRACE((this, prtIMPORTANT, "AntiPhishingTask::ObjectInitDone => GetService(TRAFFIC_MONITOR) result %terr", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR AntiPhishingTask::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	if (m_hTrafficMonitor && PR_SUCC(sysCheckObject(m_hTrafficMonitor)))
	{
		m_hTM->ReleaseService(TASKID_TM_ITSELF, m_hTrafficMonitor);
		m_hTrafficMonitor = NULL;
	}

	if (m_hAntiPhishingService && PR_SUCC(sysCheckObject((hOBJECT)m_hAntiPhishingService)))
	{
		m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hAntiPhishingService);
		m_hAntiPhishingService = NULL;
	}

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
tERROR AntiPhishingTask::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	if (p_msg_cls_id == pmc_ANTI_PHISHING_TASK && p_msg_id == msg_ANTI_PHISHING_CHECK_URL)
	{
		cSerializable* pSerializable = (cSerializable*)p_par_buf;
		PR_TRACE((this, prtNOTIFY, "AntiPhishingTask\tmsg_ANTI_PHISHING_CHECK_URL received, m_nTaskState=%x (%s)", m_nTaskState, m_nTaskState == TASK_STATE_RUNNING ? "running" : "NOT running"));
		if (m_nTaskState != TASK_STATE_RUNNING)
		{
			return errOK;
		}

		if (p_par_buf_len != SER_SENDMSG_PSIZE || !p_par_buf)
			return errPARAMETER_INVALID;

		if (!pSerializable->isBasedOn(cAntiPhishingCheckRequest::eIID))
		{
			PR_TRACE((this, prtERROR, "AntiPhishingTask\tMsgReceive: msg_ANTI_PHISHING_CHECK_URL, not cAntiPhishingCheckRequest::eIID (%08X)", pSerializable->getIID()));
			return errOBJECT_INCOMPATIBLE;
		}

		error = m_hAntiPhishingService->AskAction(msg_ANTI_PHISHING_CHECK_URL, pSerializable);
		if (error == errOK)
			error = errOK_DECIDED;

		PR_TRACE((this, prtIMPORTANT, "AntiPhishingTask\tMsgReceive: msg_ANTI_PHISHING_CHECK_URL url=%S, result: %terr", ((cAntiPhishingCheckRequest*)p_par_buf)->m_sURL.data(), error));
		return error;
	}

	if (p_msg_cls_id == pmcPROFILE && p_msg_id == pmPROFILE_STATE_CHANDED && m_nTaskState == TASK_STATE_RUNNING)
	{
		cSerializable* pSerializable = (cSerializable*)p_par_buf;

		if (p_par_buf_len != SER_SENDMSG_PSIZE || !p_par_buf)
			return errPARAMETER_INVALID;

		if (!pSerializable->isBasedOn(cProfileBase::eIID))
		{
			PR_TRACE((this, prtERROR, "AntiPhishingTask\tMsgReceive: pmPROFILE_STATE_CHANDED, not cProfileBase::eIID (%08X)", pSerializable->getIID()));
			return errPARAMETER_INVALID;
		}

		cProfileBase* pProfileInfo = (cProfileBase*)pSerializable;
		if (pProfileInfo->m_sName == AVP_SERVICE_ANTIPHISHING || pProfileInfo->m_sName == AVP_SERVICE_TRAFFICMONITOR_HTTP)
		{
			PR_TRACE((this, prtNOTIFY, "AntiPhishingTask\tUpdateTaskStatus: %S->%x", pProfileInfo->m_sName.data(), pProfileInfo->m_nState));
			if (IS_TASK_FINISHED(pProfileInfo->m_nState))
				m_nTaskState = (tTaskState)pProfileInfo->m_nState;
			return sysSendMsg(pmc_TASK_STATE_CHANGED, m_nTaskState, NULL, NULL, NULL);
		}
		return errOK;
	}

	// forward service report messages to task report
	switch (p_msg_cls_id)
	{
	case pmcASK_ACTION:
	case pmc_EVENTS_NOTIFY:
	case pmc_EVENTS_IMPORTANT:
	case pmc_EVENTS_CRITICAL:
	case pmc_UPDATE_THREATS_LIST:
		return sysSendMsg(p_msg_cls_id, p_msg_id, p_ctx, p_par_buf, p_par_buf_len);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR AntiPhishingTask::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Task has no settings

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR AntiPhishingTask::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Task has no settings

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR AntiPhishingTask::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR AntiPhishingTask::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	if (m_nTaskState == TASK_STATE_FAILED)
	{
		PR_TRACE((this, prtIMPORTANT, "AntiPhishingTask\tSetState: request for %x, but ALREADY in FAILED state", p_state));
		return error = errOBJECT_BAD_INTERNAL_STATE;
	}

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		if (!m_hAntiPhishingService)
		{
			error = m_hTM->GetService(TASKID_TM_ITSELF, *this, AVP_SERVICE_ANTIPHISHING, (hOBJECT*)&m_hAntiPhishingService);
			PR_TRACE((this, prtIMPORTANT, "AntiPhishingTask::SetState(TASK_STATE_RUNNING) => GetService(ANTIPHISH_SVC) result %terr", error));
			if( PR_FAIL(error) )
				return error;
			sysRegisterMsgHandler(pmcASK_ACTION, rmhDECIDER, m_hAntiPhishingService, IID_ANY, PID_ANTIPHISHING, IID_ANY, PID_ANY );
			sysRegisterMsgHandler(pmc_EVENTS_CRITICAL, rmhDECIDER, m_hAntiPhishingService, IID_ANY, PID_ANTIPHISHING, IID_ANY, PID_ANY );
			sysRegisterMsgHandler(pmc_EVENTS_IMPORTANT, rmhDECIDER, m_hAntiPhishingService, IID_ANY, PID_ANTIPHISHING, IID_ANY, PID_ANY );
			sysRegisterMsgHandler(pmc_EVENTS_NOTIFY, rmhDECIDER, m_hAntiPhishingService, IID_ANY, PID_ANTIPHISHING, IID_ANY, PID_ANY );
		}
		if (m_bStopIfNoAPDatabase)
		{
			if (PR_FAIL(error = m_hAntiPhishingService->AskAction(msg_ANTI_PHISHING_CHECK_DATABASE, NULL)))
				return error;
		}
		m_nTaskState = TASK_STATE_RUNNING;
		break;
	case TASK_REQUEST_PAUSE:
		m_nTaskState = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		m_nTaskState = TASK_STATE_STOPPED;
		break;
	}

	PR_TRACE((this, prtIMPORTANT, "AntiPhishingTask\tSetState: new state is %x", m_nTaskState));
	return warnTASK_STATE_CHANGED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR AntiPhishingTask::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	if (m_hAntiPhishingService)
		error = m_hAntiPhishingService->GetStatistics(p_statistics);
	else
	{
		cAntiPhishingStatistics stat;
		error = p_statistics->assign(stat, false);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR AntiPhishingTask::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, AntiPhishingTask, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, AntiPhishingTask, m_nTaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, AntiPhishingTask, m_nSessionId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, AntiPhishingTask, m_nTaskId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_PARENT_ID, AntiPhishingTask, m_nParentId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plSTOP_IF_NO_AP_DATABASE, AntiPhishingTask, m_bStopIfNoAPDatabase, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
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
		PID_ANTIPHISHINGTASK,                   // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MIKE,                               // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(AntiPhishingTask)-sizeof(cObjImpl),// memory size
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

tERROR pr_call Task_Register( hROOT root ) { return AntiPhishingTask::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// AVP Prague stamp end




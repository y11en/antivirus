// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  17 November 2006,  14:47 --------
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
#include "advdisim.h"

#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>

#include <AV/plugin/p_advdis.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable AdvDis : public cTask 
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
	cInternal  m_Internal; // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(AdvDis)
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
tERROR AdvDis::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "ObjectInit" ));
	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR AdvDis::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "ObjectInitDone" ));

	error = sysRegisterMsgHandler (
		pmc_ADVANCED_DISINFECTION,
		rmhLISTENER,
		m_hTM,
		IID_ANY,
		PID_ANY,
		IID_ANY,
		PID_ANY
		);

	if (PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtDANGER, TR "sysRegisterMsgHandler fail %terr", error ));
	}
	else
	{
		if (m_Internal.Init( (cObject*) this ))
			error = errOK;
		else
		{
			PR_TRACE(( g_root, prtDANGER, TR "ObjectInitDone - fail (connect to driver)" ));
			error = errUNEXPECTED;
		}
	}

	PR_TRACE(( g_root, prtDANGER, TR "ObjectInitDone result %terr", error ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR AdvDis::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	error = errOK;
	PR_TRACE(( g_root, prtDANGER, TR "ObjectPreClose" ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR AdvDis::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectClose method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "ObjectClose" ));

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
tERROR AdvDis::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "MsgReceive 0x%x:%x", p_msg_cls_id, p_msg_id ));

	if (pmc_ADVANCED_DISINFECTION == p_msg_cls_id)
	{
		switch (p_msg_id)
		{
		case pm_ADVANCED_DISINFECTION_START:			//no params - deny any process start
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, TR "Advanced disinfection ON"));
			
			m_Internal.ProtectFileMask( L"*.com" );
			m_Internal.ProtectFileMask( L"*.exe" );
			m_Internal.ProtectFileMask( L"*.dll" );
			m_Internal.ProtectFileMask( L"*.dpl" );
			m_Internal.ProtectFileMask( L"*.sys" );
			m_Internal.ProtectFileMask( L"*.ocx" );
			m_Internal.ProtectFileMask( L"*.efi" );
			m_Internal.ProtectFileMask( L"*.drv" );
			m_Internal.ProtectFileMask( L"*.ini" );
			m_Internal.ProtectFileMask( L"*.cmd" );
			m_Internal.ProtectFileMask( L"*.bat" );

			break;
		case pm_ADVANCED_DISINFECTION_LOCK_FILE:		//hCtx = (hSTRING) filename to lock
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, TR "lock file"));
			break;
		case pm_ADVANCED_DISINFECTION_LOCK_REGKEY:		//hCtx = (hSTRING) key to lock
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, TR "lock registry"));
			{
				cStringObj strtmp((hSTRING) p_ctx);
				ReparseKeyName(&strtmp);

				//
				int len = strtmp.length();
				if (len)
				{
					if (strtmp[len - 1] == L'\\')
						strtmp.erase(len - 1);
					PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, TR "AdvDis: protect key '%S'", strtmp.data()));
					m_Internal.ProtectKey((PWCHAR) strtmp.data(), L"*");
				}
				else
				{
					PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, TR "AdvDis: invalid key - empty"));
				}
			}
			break;
		case pm_ADVANCED_DISINFECTION_REBOOT:			// no params - хавайся кто может 
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, TR "will reboot on adv-disinfect!"));
			break;
		}
	}


	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR AdvDis::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here
	error = errOK;
	PR_TRACE(( g_root, prtDANGER, TR "SetSettings" ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR AdvDis::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "GetSettings" ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR AdvDis::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "AskAction" ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR AdvDis::SetState( tTaskRequestState p_state )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "SetState to 0x%x", p_state ));

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		if (m_Internal.SetActive( true ))
		{
			m_TaskState = TASK_STATE_RUNNING;
			error = errOK;
		}
		else
		{
			m_TaskState = TASK_STATE_FAILED;
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		}
		break;
	
	case TASK_REQUEST_PAUSE:
		if (m_Internal.SetActive( false ))
		{
			m_TaskState = TASK_STATE_PAUSED;
			error = errOK;
		}
		else
		{
			m_TaskState = TASK_STATE_FAILED;
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		}
		break;

	case TASK_REQUEST_STOP:
		m_Internal.SetActive( false );
		m_TaskState = TASK_STATE_STOPPED;
		error = errOK;
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
tERROR AdvDis::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
	PR_TRACE(( g_root, prtDANGER, TR "GetStatistics" ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR AdvDis::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Advanced Disinfection for driver-minifilter", 44 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, AdvDis, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, AdvDis, m_TaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, AdvDis, m_SessionID, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, AdvDis, m_TaskID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
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
		PID_ADVDIS,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		1/*VID_SOBKO*/,
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(AdvDis)-sizeof(cObjImpl),        // memory size
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

tERROR pr_call Task_Register( hROOT root ) { return AdvDis::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Sunday,  22 April 2007,  17:06 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- thpoolimp
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- threadpool.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ThreadPool_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include "thpimpl.h"
#include <Prague/plugin/p_thpoolimp.h>
// AVP Prague stamp end



/************************************************************************/
#define _prop_set(_type, _val)\
	*out_size = sizeof(_type);\
	if (NULL != buffer)\
	{\
		if (sizeof(_type) != size)\
			error = errBAD_SIZE;\
		else\
			_val = *(_type*) buffer;\
	}

#define _prop_get(_type, _val)\
	*out_size = sizeof(_type);\
	if (NULL != buffer)\
	{\
		if (sizeof(_type) != size)\
			error = errBAD_SIZE;\
		else\
			*(_type*) buffer = _val;\
	}

/************************************************************************/


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CThreadPool : public cThreadPool 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call PropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ThreadPriorityRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ThreadPriorityWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call AllowTerminatingRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call AllowTerminatingWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ThreadCallbackRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ThreadCallbackWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ThreadCallbackContextRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ThreadCallbackContextWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call InvisibleFlagRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call InvisibleFlagWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call DelayedDoneRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call DelayedDoneWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call QuickDoneRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call QuickDoneWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call QuickExecuteRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call QuickExecuteWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call AddTask( tTaskId* p_pTaskId, tThreadTaskCallback p_cbTaskCallback, tPTR p_pTaskData, tDWORD p_DataLen, tDWORD p_TaskPriority );
	tERROR pr_call GetThreadState( tThreadState* p_pState, tTaskId p_TaskId );
	tERROR pr_call GetThreadContext( tTaskId p_TaskId, tPTR* p_ppTaskContext, tPTR* p_ppTaskData, tDWORD* p_pTaskDataLen, tThreadState* p_pState );
	tERROR pr_call ReleaseThreadContext( tTaskId p_TaskId );
	tERROR pr_call WaitForTaskDone( tTaskId p_TaskId );
	tERROR pr_call SetThreadPriorityByTask( tTaskId p_TaskId, tThreadPriority p_ThreadPriority );
	tERROR pr_call RemoveTaskFromQueue( tTaskId p_TaskId );
	tERROR pr_call WaitTaskDonenotify( tDWORD p_Timeout );
	tERROR pr_call CreateSeparateThread( tThreadTaskCallback p_cbSeparateFunc, tDWORD p_SeparateData );
	tERROR pr_call EnumTaskContext( tThreadTaskEnum p_EnumMode, tTaskId* p_pTaskId, tPTR* p_ppTaskContext, tPTR* p_ppTaskData, tDWORD* p_pTaskDatalen, tThreadState* p_pState );

// Data declaration
	THPoolData m_THPoolData; // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CThreadPool)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ThreadPool". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR CThreadPool::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::ObjectInit method" );

	// Place your code here
	error = errOK;
	THP_InitData(&m_THPoolData);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CThreadPool::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::ObjectInitDone method" );

	// Place your code here
	PR_ASSERT_MSG(m_THPoolData.m_ThreadPoolName[0] != 0, "ThreadPool doesn't have a name");
	if (THP_Start(&m_THPoolData))
		error = errOK;
	else
		error = errOBJECT_CANNOT_BE_INITIALIZED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CThreadPool::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::ObjectPreClose method" );

	// Place your code here
	error = errOK;
	THP_Stop(&m_THPoolData);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR CThreadPool::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ThreadPool::ObjectClose method" );

	// Place your code here
	THP_DoneData(&m_THPoolData);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_MIN_THREADS
tERROR CThreadPool::PropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method PropRead for property pgTP_MIN_THREADS" );

	*out_size = sizeof(tDWORD);
	
	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	if (NULL != buffer)
	{
		if (sizeof(tDWORD) > size)
			error = errBUFFER_TOO_SMALL;
		else
		{
			switch (prop)
			{
			case pgTP_MIN_THREADS:
				*(tDWORD*)buffer = m_THPoolData.m_MinThreads;
				break;
			case pgTP_MAX_THREADS:
				*(tDWORD*)buffer = m_THPoolData.m_MaxThreads;
				break;
			case pgTP_TERMINATE_TIMEOUT:
				*(tDWORD*)buffer = m_THPoolData.m_TerminateTimeout;
				break;
			case pgTP_YIELD_TIMEOUT:
				*(tDWORD*)buffer = m_THPoolData.m_YieldTimeout;
				break;
			case pgTP_TASK_QUEUE_MAX_LEN:
				*(tDWORD*)buffer = m_THPoolData.m_TaskQueue_MaxLen;
				break;
			case pgTP_MAX_IDLE_COUNT:
				*(tDWORD*)buffer = m_THPoolData.m_IdleCountMax;
				break;
			case pgTP_NAME:
				strncpy(buffer, m_THPoolData.m_ThreadPoolName, size);
				break;
			default:
				error = errPROPERTY_NOT_FOUND;
				break;
			}
		}
	}

	PR_TRACE_A2( this, "Leave *GET* method PropRead for property pgTP_MIN_THREADS, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_MIN_THREADS
tERROR CThreadPool::PropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method PropWrite for property pgTP_MIN_THREADS" );

	*out_size = sizeof(tDWORD);
	
	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	if (NULL != buffer)
	{
		if (sizeof(tDWORD) != size && prop != pgTP_NAME)
			error = errBAD_SIZE;
		else
		{
			switch (prop)
			{
			case pgTP_MIN_THREADS:
				m_THPoolData.m_MinThreads = *(tDWORD*)buffer;
				break;
			case pgTP_MAX_THREADS:
				 m_THPoolData.m_MaxThreads = *(tDWORD*)buffer;
				break;
			case pgTP_TERMINATE_TIMEOUT:
				m_THPoolData.m_TerminateTimeout = *(tDWORD*)buffer;
				break;
			case pgTP_YIELD_TIMEOUT:
				m_THPoolData.m_YieldTimeout = *(tDWORD*)buffer;
				break;
			case pgTP_TASK_QUEUE_MAX_LEN:
				m_THPoolData.m_TaskQueue_MaxLen = *(tDWORD*)buffer;
				break;
			case pgTP_MAX_IDLE_COUNT:
				m_THPoolData.m_IdleCountMax = *(tDWORD*)buffer;
				break;
			case pgTP_NAME:
				strncpy(m_THPoolData.m_ThreadPoolName, buffer, sizeof(m_THPoolData.m_ThreadPoolName));
				break;
			default:
				error = errPROPERTY_NOT_FOUND;
				break;
			}
		}
	}


	PR_TRACE_A2( this, "Leave *SET* method PropWrite for property pgTP_MIN_THREADS, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ThreadPriorityRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_DEFAULT_THREAD_PRIORITY
tERROR CThreadPool::ThreadPriorityRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ThreadPriorityRead for property pgTP_DEFAULT_THREAD_PRIORITY" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tThreadPriority, m_THPoolData.m_DefaultThreadPriority);

	PR_TRACE_A2( this, "Leave *GET* method ThreadPriorityRead for property pgTP_DEFAULT_THREAD_PRIORITY, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ThreadPriorityWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_DEFAULT_THREAD_PRIORITY
tERROR CThreadPool::ThreadPriorityWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ThreadPriorityWrite for property pgTP_DEFAULT_THREAD_PRIORITY" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tThreadPriority, m_THPoolData.m_DefaultThreadPriority);

	PR_TRACE_A2( this, "Leave *SET* method ThreadPriorityWrite for property pgTP_DEFAULT_THREAD_PRIORITY, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, AllowTerminatingRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_ALLOW_TERMINATING
tERROR CThreadPool::AllowTerminatingRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method AllowTerminatingRead for property pgTP_ALLOW_TERMINATING" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tBOOL, m_THPoolData.m_bAllowTerminating);

	PR_TRACE_A2( this, "Leave *GET* method AllowTerminatingRead for property pgTP_ALLOW_TERMINATING, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, AllowTerminatingWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_ALLOW_TERMINATING
tERROR CThreadPool::AllowTerminatingWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method AllowTerminatingWrite for property pgTP_ALLOW_TERMINATING" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tBOOL, m_THPoolData.m_bAllowTerminating);

	PR_TRACE_A2( this, "Leave *SET* method AllowTerminatingWrite for property pgTP_ALLOW_TERMINATING, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ThreadCallbackRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_THREAD_CALLBACK
tERROR CThreadPool::ThreadCallbackRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ThreadCallbackRead for property pgTP_THREAD_CALLBACK" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tThreadCallback, m_THPoolData.m_cbThread);

	PR_TRACE_A2( this, "Leave *GET* method ThreadCallbackRead for property pgTP_THREAD_CALLBACK, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ThreadCallbackWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_THREAD_CALLBACK
tERROR CThreadPool::ThreadCallbackWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ThreadCallbackWrite for property pgTP_THREAD_CALLBACK" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tThreadCallback, m_THPoolData.m_cbThread);

	PR_TRACE_A2( this, "Leave *SET* method ThreadCallbackWrite for property pgTP_THREAD_CALLBACK, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ThreadCallbackContextRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_THREAD_CALLBACK_CONTEXT
tERROR CThreadPool::ThreadCallbackContextRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ThreadCallbackContextRead for property pgTP_THREAD_CALLBACK_CONTEXT" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tPTR, m_THPoolData.m_pThreadData)

	PR_TRACE_A2( this, "Leave *GET* method ThreadCallbackContextRead for property pgTP_THREAD_CALLBACK_CONTEXT, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ThreadCallbackContextWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_THREAD_CALLBACK_CONTEXT
tERROR CThreadPool::ThreadCallbackContextWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ThreadCallbackContextWrite for property pgTP_THREAD_CALLBACK_CONTEXT" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tPTR, m_THPoolData.m_pThreadData);

	PR_TRACE_A2( this, "Leave *SET* method ThreadCallbackContextWrite for property pgTP_THREAD_CALLBACK_CONTEXT, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, InvisibleFlagRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_INVISIBLE_FLAG
tERROR CThreadPool::InvisibleFlagRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method InvisibleFlagRead for property pgTP_INVISIBLE_FLAG" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tBOOL, m_THPoolData.m_bSetInvisible)


	PR_TRACE_A2( this, "Leave *GET* method InvisibleFlagRead for property pgTP_INVISIBLE_FLAG, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, InvisibleFlagWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_INVISIBLE_FLAG
tERROR CThreadPool::InvisibleFlagWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method InvisibleFlagWrite for property pgTP_INVISIBLE_FLAG" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tBOOL, m_THPoolData.m_bSetInvisible);


	PR_TRACE_A2( this, "Leave *SET* method InvisibleFlagWrite for property pgTP_INVISIBLE_FLAG, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, DelayedDoneRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_DELAYED_DONE
tERROR CThreadPool::DelayedDoneRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method DelayedDoneRead for property pgTP_DELAYED_DONE" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tBOOL, m_THPoolData.m_bDelayedDone)

	PR_TRACE_A2( this, "Leave *GET* method DelayedDoneRead for property pgTP_DELAYED_DONE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, DelayedDoneWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_DELAYED_DONE
tERROR CThreadPool::DelayedDoneWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method DelayedDoneWrite for property pgTP_DELAYED_DONE" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tBOOL, m_THPoolData.m_bDelayedDone)


	PR_TRACE_A2( this, "Leave *SET* method DelayedDoneWrite for property pgTP_DELAYED_DONE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, QuickDoneRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_QUICK_DONE
tERROR CThreadPool::QuickDoneRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method QuickDoneRead for property pgTP_QUICK_DONE" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tBOOL, m_THPoolData.m_bQuickDone)


	PR_TRACE_A2( this, "Leave *GET* method QuickDoneRead for property pgTP_QUICK_DONE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, QuickDoneWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_QUICK_DONE
tERROR CThreadPool::QuickDoneWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method QuickDoneWrite for property pgTP_QUICK_DONE" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tBOOL, m_THPoolData.m_bQuickDone)


	PR_TRACE_A2( this, "Leave *SET* method QuickDoneWrite for property pgTP_QUICK_DONE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, QuickExecuteRead )
// Interface "ThreadPool". Method "Get" for property(s):
//  -- TP_QUICK_EXECUTE
tERROR CThreadPool::QuickExecuteRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method QuickExecuteRead for property pgTP_QUICK_EXECUTE" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_get(tBOOL, m_THPoolData.m_bQuickExecute)


	PR_TRACE_A2( this, "Leave *GET* method QuickExecuteRead for property pgTP_QUICK_EXECUTE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, QuickExecuteWrite )
// Interface "ThreadPool". Method "Set" for property(s):
//  -- TP_QUICK_EXECUTE
tERROR CThreadPool::QuickExecuteWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method QuickExecuteWrite for property pgTP_QUICK_EXECUTE" );

	cthp_autosync autoSync(&m_THPoolData.m_Lock, _thp_lock_w);
	_prop_set(tBOOL, m_THPoolData.m_bQuickExecute)

	PR_TRACE_A2( this, "Leave *SET* method QuickExecuteWrite for property pgTP_QUICK_EXECUTE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AddTask )
// Parameters are:
tERROR CThreadPool::AddTask( tTaskId* p_pTaskId, tThreadTaskCallback p_cbTaskCallback, tPTR p_pTaskData, tDWORD p_DataLen, tDWORD p_TaskPriority )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::AddTask method" );

	// Place your code here
	error = THP_AddTask(&m_THPoolData, p_pTaskId, p_cbTaskCallback, p_pTaskData, p_DataLen, p_TaskPriority);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetThreadState )
// Parameters are:
tERROR CThreadPool::GetThreadState( tThreadState* p_pState, tTaskId p_TaskId )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::GetThreadState method" );

	// Place your code here
	error = errOK;
	*p_pState = THP_GetThreadState(&m_THPoolData, p_TaskId);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetThreadContext )
// Parameters are:
tERROR CThreadPool::GetThreadContext( tTaskId p_TaskId, tPTR* p_ppTaskContext, tPTR* p_ppTaskData, tDWORD* p_pTaskDataLen, tThreadState* p_pState )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::GetThreadContext method" );

	// Place your code here
	error = THP_GetThreadContext(&m_THPoolData, p_TaskId, p_ppTaskContext, p_ppTaskData, p_pTaskDataLen, p_pState);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseThreadContext )
// Parameters are:
tERROR CThreadPool::ReleaseThreadContext( tTaskId p_TaskId )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::ReleaseThreadContext method" );

	// Place your code here
	THP_ReleaseThreadContext(&m_THPoolData, p_TaskId);
	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, WaitForTaskDone )
// Parameters are:
tERROR CThreadPool::WaitForTaskDone( tTaskId p_TaskId )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::WaitForTaskDone method" );

	// Place your code here
	error = THP_WaitForTaskDone(&m_THPoolData, p_TaskId);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetThreadPriorityByTask )
// Parameters are:
tERROR CThreadPool::SetThreadPriorityByTask( tTaskId p_TaskId, tThreadPriority p_ThreadPriority )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::SetThreadPriorityByTask method" );

	// Place your code here
	error = THP_SetThreadPriorityByTask(&m_THPoolData, p_TaskId, p_ThreadPriority);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, RemoveTaskFromQueue )
// Parameters are:
tERROR CThreadPool::RemoveTaskFromQueue( tTaskId p_TaskId )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::RemoveTaskFromQueue method" );

	// Place your code here
	error = THP_RemoveTaskFromQueue(&m_THPoolData, p_TaskId);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, WaitTaskDonenotify )
// Parameters are:
tERROR CThreadPool::WaitTaskDonenotify( tDWORD p_Timeout )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::WaitTaskDonenotify method" );

	error = THP_WaitTaskDoneNotify(&m_THPoolData, p_Timeout);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CreateSeparateThread )
// Parameters are:
tERROR CThreadPool::CreateSeparateThread( tThreadTaskCallback p_cbSeparateFunc, tDWORD p_SeparateData )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::CreateSeparateThread method" );

	// Place your code here
	error = THP_CreateSeparateThread(&m_THPoolData, p_cbSeparateFunc, p_SeparateData);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, EnumTaskContext )
// Parameters are:
tERROR CThreadPool::EnumTaskContext( tThreadTaskEnum p_EnumMode, tTaskId* p_pTaskId, tPTR* p_ppTaskContext, tPTR* p_ppTaskData, tDWORD* p_pTaskDatalen, tThreadState* p_pState )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ThreadPool::EnumTaskContext method" );

	// Place your code here
	error = THP_EnumTaskContext(&m_THPoolData, p_EnumMode, p_pTaskId, p_ppTaskContext, p_ppTaskData, p_pTaskDatalen, p_pState);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "ThreadPool". Register function
tERROR CThreadPool::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(ThreadPool_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ThreadPool_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Thread pool", 12 )
	mpLOCAL_PROPERTY_FN( pgTP_MIN_THREADS, FN_CUST(PropRead), FN_CUST(PropWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_MAX_THREADS, FN_CUST(PropRead), FN_CUST(PropWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_DEFAULT_THREAD_PRIORITY, FN_CUST(ThreadPriorityRead), FN_CUST(ThreadPriorityWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_ALLOW_TERMINATING, FN_CUST(AllowTerminatingRead), FN_CUST(AllowTerminatingWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_TERMINATE_TIMEOUT, FN_CUST(PropRead), FN_CUST(PropWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_YIELD_TIMEOUT, FN_CUST(PropRead), FN_CUST(PropWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_TASK_QUEUE_MAX_LEN, FN_CUST(PropRead), FN_CUST(PropWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_MAX_IDLE_COUNT, FN_CUST(PropRead), FN_CUST(PropWrite) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgTP_THREAD_CALLBACK, FN_CUST(ThreadCallbackRead), FN_CUST(ThreadCallbackWrite) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgTP_THREAD_CALLBACK_CONTEXT, FN_CUST(ThreadCallbackContextRead), FN_CUST(ThreadCallbackContextWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_INVISIBLE_FLAG, FN_CUST(InvisibleFlagRead), FN_CUST(InvisibleFlagWrite) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgTP_DELAYED_DONE, FN_CUST(DelayedDoneRead), FN_CUST(DelayedDoneWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_QUICK_DONE, FN_CUST(QuickDoneRead), FN_CUST(QuickDoneWrite) )
	mpLOCAL_PROPERTY_FN( pgTP_QUICK_EXECUTE, FN_CUST(QuickExecuteRead), FN_CUST(QuickExecuteWrite) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgTP_NAME, FN_CUST(PropRead), FN_CUST(PropWrite) )
mpPROPERTY_TABLE_END(ThreadPool_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(ThreadPool)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(ThreadPool)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(ThreadPool)
	mEXTERNAL_METHOD(ThreadPool, AddTask)
	mEXTERNAL_METHOD(ThreadPool, GetThreadState)
	mEXTERNAL_METHOD(ThreadPool, GetThreadContext)
	mEXTERNAL_METHOD(ThreadPool, ReleaseThreadContext)
	mEXTERNAL_METHOD(ThreadPool, WaitForTaskDone)
	mEXTERNAL_METHOD(ThreadPool, SetThreadPriorityByTask)
	mEXTERNAL_METHOD(ThreadPool, RemoveTaskFromQueue)
	mEXTERNAL_METHOD(ThreadPool, WaitTaskDonenotify)
	mEXTERNAL_METHOD(ThreadPool, CreateSeparateThread)
	mEXTERNAL_METHOD(ThreadPool, EnumTaskContext)
mEXTERNAL_TABLE_END(ThreadPool)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "ThreadPool::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_THREADPOOL,                         // interface identifier
		PID_THPOOLIMP,                          // plugin identifier
		0x00000000,                             // subtype identifier
		ThreadPool_VERSION,                     // interface version
		VID_SOBKO,                              // interface developer
		&i_ThreadPool_vtbl,                     // internal(kernel called) function table
		(sizeof(i_ThreadPool_vtbl)/sizeof(tPTR)),// internal function table size
		&e_ThreadPool_vtbl,                     // external function table
		(sizeof(e_ThreadPool_vtbl)/sizeof(tPTR)),// external function table size
		ThreadPool_PropTable,                   // property table
		mPROPERTY_TABLE_SIZE(ThreadPool_PropTable),// property table size
		sizeof(CThreadPool)-sizeof(cObjImpl),   // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ThreadPool(IID_THREADPOOL) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call ThreadPool_Register( hROOT root ) { return CThreadPool::Register(root); }
// AVP Prague stamp end



#include "thpimpl.h"

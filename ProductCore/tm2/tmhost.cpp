// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  30 March 2006,  12:36 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- tmhost.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define TmHost_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Includes for interface,  )
#include "tmhost.h"
// AVP Prague stamp end

static tPROPID m_prop_taskid;
static tPROPID m_prop_task_type;
static tPROPID m_prop_is_remote;
static tPROPID m_prop_profile;

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "TmHost". Static(shared) property table variables
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR cTmHostImpl::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "TmHost::ObjectInitDone method" );

	// Place your code here
	error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_REMOTE_GLOBAL, rmhLISTENER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	if( PR_SUCC(error) )
		error = AttachObject(*this);

	if( PR_SUCC(error) )
		RegisterObject(pgTM_HOST, *this);

	g_root->RegisterCustomPropId(&m_prop_taskid, cpTASK_MANAGER_TASK_ID, pTYPE_DWORD );
	g_root->RegisterCustomPropId(&m_prop_task_type, cpTASK_MANAGER_TASK_TYPE_NAME, pTYPE_STRING );
	g_root->RegisterCustomPropId(&m_prop_is_remote, cpTASK_MANAGER_TASK_IS_REMOTE, pTYPE_BOOL);
	g_root->RegisterCustomPropId(&m_prop_profile, cpTASK_MANAGER_PROFILE_NAME, pTYPE_STRING );

	if( EstablishConnections )
		EstablishConnections(pgTM_HOST);
	
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cTmHostImpl::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "TmHost::ObjectPreClose method" );

	// Place your code here
	UnregisterObject(*this);

	tDWORD i, c = m_tasks.size();
	for(i = 0; i < c; i++)
		if( !m_tasks[i].m_bService )
			close_object(m_tasks[i].m_task);

	for(i = 0; i < c; i++)
		if( m_tasks[i].m_bService )
			close_object(m_tasks[i].m_task);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
tERROR cTmHostImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "TmHost::MsgReceive method" );

	// Place your code here
	if( p_msg_cls_id == pmc_REMOTE_GLOBAL &&
		p_msg_id == pm_REMOTE_GLOBAL_TM_INITED )
	{
		cTaskManager* tm = (cTaskManager*)p_ctx;
		if( tm )
		{
			tDWORD i, c = m_tasks.size();
			for(i = 0; i < c; i++)
			{
				cObj* host = NULL, *task = m_tasks[i].m_task;
				tm->RegisterRemoteTask(*this, task, &m_tasks[i], &host);
				if( !host )
				{
					sysSendMsg( pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
					return errOK;
				}

				AttachObject(task, host);
			}

			AttachObject(*this, p_send_point);
		}
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, CreateTask )
// Parameters are:
tERROR cTmHostImpl::CreateTask( hOBJECT p_host, cSerializable* p_info, hOBJECT* p_task )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "TmHost::CreateTask method" );

	// Place your code here
	if( !p_host || !p_info || !p_info->isBasedOn(cTaskInfo::eIID) )
		return errPARAMETER_INVALID;

	cTaskInfo* info = (cTaskInfo*)p_info;

	if( PR_FAIL(error = p_host->sysCreateObject(p_task, info->m_iid, info->m_pid)) )
		return error;

	info->m_sType.copy(*p_task, m_prop_task_type);
	info->m_sProfile.copy(*p_task, m_prop_profile);
	(*p_task)->propSetDWord(m_prop_taskid, info->m_nParentId);
	(*p_task)->propSetBool(m_prop_is_remote, cTRUE);

	AttachObject(*p_task);

	cAutoCS locker(m_lock, true);

	cTHTask& htask = m_tasks.push_back(*p_task);
	htask.assign(*info, false);
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, DestroyTask )
// Parameters are:
tERROR cTmHostImpl::DestroyTask( hOBJECT p_task )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "TmHost::DestroyTask method" );

	// Place your code here
	cAutoCS locker(m_lock, true);

	tINT pos = m_tasks.find(p_task);
	if( pos == -1 )
		return errNOT_FOUND;

	m_tasks.remove(pos);
	locker.unlock();

	return DetachObject(p_task);
}
// AVP Prague stamp end

tERROR cTmHostImpl::AttachObject(cObj* obj, cObj* parent)
{
	if( !obj )
		return errUNEXPECTED;

	cObj* dad = obj->sysGetParent(IID_ANY);
	if( !parent )
	{
		AddRefObjectProxy(dad);
	}
	else if( parent != dad )
	{
		AddRefObjectProxy(parent);
		obj->sysSetParent(NULL, parent);
		ReleaseObjectProxy(dad);
	}
	return errOK;
}

tERROR cTmHostImpl::DetachObject(cObj* obj)
{
	if( !obj )
		return errUNEXPECTED;

	cObj* dad = obj->sysGetParent(IID_ANY);

	tERROR error = obj->sysCloseObject();

	if( dad )
		ReleaseObjectProxy(dad);

	return error;
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "TmHost". Register function
tERROR cTmHostImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(TmHost)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(TmHost)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(TmHost)
	mEXTERNAL_METHOD(TmHost, CreateTask)
	mEXTERNAL_METHOD(TmHost, DestroyTask)
mEXTERNAL_TABLE_END(TmHost)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "TmHost::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TMHOST,                             // interface identifier
		PID_TM,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		TmHost_VERSION,                         // interface version
		VID_MEZHUEV,                            // interface developer
		&i_TmHost_vtbl,                         // internal(kernel called) function table
		(sizeof(i_TmHost_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_TmHost_vtbl,                         // external function table
		(sizeof(e_TmHost_vtbl)/sizeof(tPTR)),   // external function table size
		NULL,                                   // property table
		0,                                      // property table size
		sizeof(cTmHostImpl)-sizeof(cObjImpl),   // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"TmHost(IID_TMHOST) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call TmHost_Register( hROOT root ) { return cTmHostImpl::Register(root); }
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end


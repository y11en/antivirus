// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  14 December 2004,  12:55 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- ipresolver.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IPRESOLVER_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "resip_imp.h"
#include <Prague/iface/i_string.h>
#include <AHTasks/plugin/p_resip.h>
// AVP Prague stamp end

#include <ProductCore/iptoa.h>

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cIPRES : public cIPRESOLVER /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call ResolveHost( tUINT* result, hSTRING p_HostName, tResolveMethod p_ResolveMethod );
	tERROR pr_call ResolveBreak( tUINT p_ResolveID );
	tERROR pr_call ResolveHostAtOnce( hSTRING p_HostName, cSerializable* p_IPs );
	tERROR pr_call ResolveIP( tUINT* result, cSerializable* p_IP, tResolveMethod p_ResolveMethod );

// Data declaration
	cThreadPool*   m_ThPool_Resolver;  // --
	cThreadPool*   m_ThPool_Continous; // --
	cMutex*        m_hSync;            // --
	sThreadContext m_Context;          // --
// AVP Prague stamp end



	bool CreateThreadPool(cThreadPool** ppThPool, tPTR pContext);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cIPRES)
private:
	bool m_bInCLose;
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IPRESOLVER". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR cIPRES::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ipresolver::ObjectInitDone method" );

	m_Context.m_Task = *this;
	InitializeListHead(&m_Context.m_List);
	m_Context.m_pList = &m_Context.m_List;
	m_Context.m_RequestIDNext = 1;

	error = sysCreateObjectQuick((hOBJECT*)&m_hSync, IID_MUTEX);
	if (PR_SUCC(error))
	{
		m_Context.m_hSync = m_hSync;

		if (!CreateThreadPool(&m_ThPool_Resolver, &m_Context))
			error = errOBJECT_CANNOT_BE_INITIALIZED;
	}

	if (PR_SUCC(error))
	{
		if (!CreateThreadPool(&m_ThPool_Continous, &m_Context))
			error = errOBJECT_CANNOT_BE_INITIALIZED;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cIPRES::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ipresolver::ObjectPreClose method" );

	// Place your code here
	error = errOK;

	m_hSync->Lock(cSYNC_INFINITE);

	m_bInCLose = true;

	m_hSync->Release();

	if (m_ThPool_Continous)
	{
		m_ThPool_Continous->sysCloseObject();
		m_ThPool_Continous = 0;
	}

	if (m_ThPool_Resolver)
	{
		m_ThPool_Resolver->sysCloseObject();
		m_ThPool_Resolver = 0;
	}

	m_hSync->Lock( cSYNC_INFINITE );

	PIPRESOLVE_LISTITEM pItem;
	while (true)
	{
		if (IsListEmpty( m_Context.m_pList ))
			break;

		pItem = (PIPRESOLVE_LISTITEM) RemoveHeadList( m_Context.m_pList );

		delete pItem;
	}

	m_hSync->Release();


	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR cIPRES::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IPRESOLVER::ObjectClose method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ResolveHost )
// Parameters are:
tERROR cIPRES::ResolveHost( tUINT* result, hSTRING p_HostName, tResolveMethod p_ResolveMethod )
{
	tERROR error = errNOT_IMPLEMENTED;
	tUINT  ret_val = 0;
	PR_TRACE_FUNC_FRAME( "ipresolver::ResolveHost method" );

	// Place your code here

	if (!p_HostName)
		return errPARAMETER_INVALID;
	
	if (p_ResolveMethod != cJUST_RESOLVE)
		return errNOT_IMPLEMENTED;

	error = errOK;

	PIPRESOLVE_LISTITEM pItem = NULL;

	m_hSync->Lock(cSYNC_INFINITE);

	if (m_bInCLose)
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	else
	{
		pItem = new IPRESOLVE_LISTITEM;

		if (pItem)
		{
			pItem->m_ResolveDirection = cTRUE;
			error = pItem->m_ResolveData.m_HostName.append(p_HostName);

			if (PR_FAIL(error))
			{
				delete pItem;
				pItem = NULL;
				error = errNOT_ENOUGH_MEMORY;
			}
			else
			{
				pItem->m_ResolveData.m_RequestID = m_Context.m_RequestIDNext;
				if (result)
					*result = pItem->m_ResolveData.m_RequestID;
				m_Context.m_RequestIDNext++;

				InsertHeadList(&m_Context.m_List, &pItem->m_List);
			}
		}
	}

	m_hSync->Release();
	
	tTaskId task;
	if (pItem)
	{
		tERROR errAddTask = m_ThPool_Resolver->AddTask(&task, ipres_ThreadTaskCallback, 0, 0, 0);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ResolveBreak )
// Parameters are:
tERROR cIPRES::ResolveBreak( tUINT p_ResolveID )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ipresolver::ResolveBreak method" );

	// Place your code here
	m_hSync->Lock(cSYNC_INFINITE);
	m_hSync->Release();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ResolveHostAtOnce )
// Parameters are:
tERROR cIPRES::ResolveHostAtOnce( hSTRING p_HostName, cSerializable* p_IPs )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IPRESOLVER::ResolveHostAtOnce method" );

	// Place your code here
	if (!p_IPs)
		return errPARAMETER_INVALID;

	if (!p_IPs->isBasedOn(cIPResolveResult::eIID))
		return errPARAMETER_INVALID;

	cIPResolveResult* pIPs = (cIPResolveResult*) p_IPs;
	pIPs->m_RequestID = -1;
	pIPs->m_HostName.append(p_HostName);

	ResolveOneHost((cIPResolveResult*) p_IPs);
	
	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ResolveIP )
// Parameters are:
tERROR cIPRES::ResolveIP( tUINT* result, cSerializable* p_IP, tResolveMethod p_ResolveMethod ) 
{
	tERROR error = errNOT_IMPLEMENTED;
	tUINT  ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IPRESOLVER::ResolveIP method" );

	// Place your code here
    if (!p_IP)
		return errPARAMETER_INVALID;

	if (!p_IP->isBasedOn(cIP::eIID))
		return errPARAMETER_INVALID;

    cIP& ip = *(cIP*)p_IP;
	PR_TRACE((0, prtERROR, "IPRES\t resolve ip request %s", cIPToA(ip)));

    if (!ip.IsValid())
		return errPARAMETER_INVALID;
	
	if (p_ResolveMethod != cJUST_RESOLVE)
		return errNOT_IMPLEMENTED;

	error = errOK;

	PIPRESOLVE_LISTITEM pItem = new IPRESOLVE_LISTITEM;

	if (!pItem)
		return errNOT_ENOUGH_MEMORY;
	
	pItem->m_ResolveDirection = cFALSE;
	pItem->m_ResolveData.m_IPs.push_back(ip);

	m_hSync->Lock(cSYNC_INFINITE);

	pItem->m_ResolveData.m_RequestID = m_Context.m_RequestIDNext;
	if (result)
		*result = pItem->m_ResolveData.m_RequestID;
	m_Context.m_RequestIDNext++;

	InsertHeadList(&m_Context.m_List, &pItem->m_List);
		
	m_hSync->Release();

	tTaskId task;
	tERROR errAddTask = m_ThPool_Resolver->AddTask(&task, ipres_ThreadTaskCallback, 0, 0, 0);

	if (PR_FAIL(errAddTask))
		error = errUNEXPECTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IPRESOLVER". Register function
tERROR cIPRES::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IPRESOLVER_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IPRESOLVER_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "IP resolver", 12 )
mpPROPERTY_TABLE_END(IPRESOLVER_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(IPRESOLVER)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(IPRESOLVER)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(IPRESOLVER)
	mEXTERNAL_METHOD(IPRESOLVER, ResolveHost)
	mEXTERNAL_METHOD(IPRESOLVER, ResolveBreak)
	mEXTERNAL_METHOD(IPRESOLVER, ResolveHostAtOnce)
	mEXTERNAL_METHOD(IPRESOLVER, ResolveIP)
mEXTERNAL_TABLE_END(IPRESOLVER)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "ipresolver::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IPRESOLVER,                         // interface identifier
		PID_RESIP,                              // plugin identifier
		0x00000000,                             // subtype identifier
		IPRESOLVER_VERSION,                     // interface version
		VID_SOBKO,                              // interface developer
		&i_IPRESOLVER_vtbl,                     // internal(kernel called) function table
		(sizeof(i_IPRESOLVER_vtbl)/sizeof(tPTR)),// internal function table size
		&e_IPRESOLVER_vtbl,                     // external function table
		(sizeof(e_IPRESOLVER_vtbl)/sizeof(tPTR)),// external function table size
		IPRESOLVER_PropTable,                   // property table
		mPROPERTY_TABLE_SIZE(IPRESOLVER_PropTable),// property table size
		sizeof(cIPRES)-sizeof(cObjImpl),        // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ipresolver(IID_IPRESOLVER) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call IPRESOLVER_Register( hROOT root ) { return cIPRES::Register(root); }
// AVP Prague stamp end



bool cIPRES::CreateThreadPool(cThreadPool** ppThPool, tPTR pContext)
{
	tERROR error = sysCreateObject((hOBJECT*)ppThPool, IID_THREADPOOL, PID_ANY);
	if (PR_FAIL(error))
	{
		*ppThPool = 0;
		return false;
	}
	(*ppThPool)->set_pgTP_NAME("IPRES", sizeof("IPRES"), cCP_ANSI);
	(*ppThPool)->set_pgTP_TASK_QUEUE_MAX_LEN(1024);

	(*ppThPool)->set_pgTP_MIN_THREADS(0);
	(*ppThPool)->set_pgTP_MAX_THREADS(5);

	(*ppThPool)->set_pgTP_QUICK_EXECUTE(cTRUE);
	(*ppThPool)->set_pgTP_QUICK_DONE(cTRUE);

	(*ppThPool)->set_pgTP_MAX_IDLE_COUNT((tDWORD) -1);
	(*ppThPool)->set_pgTP_INVISIBLE_FLAG(cTRUE);
	(*ppThPool)->set_pgTP_YIELD_TIMEOUT(5000);
	
	(*ppThPool)->set_pgTP_THREAD_CALLBACK(ipres_ThreadCallback);
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

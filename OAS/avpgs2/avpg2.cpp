// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  27 July 2007,  13:16 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- KAV
// Sub project -- OAS
// Purpose     -- Плагин для обработки событий из klif.sys
// Author      -- Rubin
// File Name   -- avpg2.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define AVPG2_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <OAS/plugin/p_avpgs2.h>
#include <Prague/pr_serializable.h>
#include <ProductCore/structs/s_trmod.h>
#include <ProductCore/structs/ProcMonM.h>
// AVP Prague stamp end

#ifdef _WIN32
#include "wavpg.h"
#elif defined(__APPLE__)
#include "mavpg.h"
#else 
#error unsupported platform
#endif

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cAVPGS2Impl : public cAVPG2 
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
	tERROR pr_call SetActivityMode( tAVPG2_CLIENT_ACTIVITY_MODE p_ActivityMode );
	tERROR pr_call SetWorkingMode( tAVPG2_WORKING_MODE p_WorkingMode );
	tERROR pr_call YieldEvent( hOBJECT p_EventObject, tDWORD p_YieldTimeout );
	tERROR pr_call ResetDriverCache();
	tERROR pr_call CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName );
	tERROR pr_call SetPreverdict( hOBJECT p_EventObject, tAVPG2_PROCESS_OBJECT_RESULT_FLAGS p_ResultFlags );
	tERROR pr_call ResetBanList();
	tERROR pr_call AddBan( cSerializable* p_User );

// Data declaration
	tDWORD                      m_WorkingThreadMaxCount; // --
	tAVPG2_CLIENT_ACTIVITY_MODE m_ActivityMode;          // --
	tAVPG2_WORKING_MODE         m_WorkingMode;           // --

// AVP Prague stamp end

	cAvpgImpl *m_pAvpgImpl;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cAVPGS2Impl)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AVPG2". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR cAVPGS2Impl::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::ObjectInit method" );

	// Place your code here
	error = errOK;
	
	m_WorkingThreadMaxCount = 60;
	m_ActivityMode = cAVPG2_CLIENT_ACTIVITY_MODE_SLEEP;
	m_WorkingMode = cAVPG2_WORKING_MODE_ON_SMART;

	m_pAvpgImpl = NULL;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR cAVPGS2Impl::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::ObjectInitDone method" );

	// Place your code here
	sysRegisterMsgHandler( pmc_PRODUCT, rmhLISTENER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	sysRegisterMsgHandler(pmc_PROCESS_MONITOR, rmhLISTENER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	try {
#ifdef _WIN32
		m_pAvpgImpl = new cWinAvpgImpl ( *this, m_WorkingMode, m_WorkingThreadMaxCount );
#elif defined(__APPLE__)
		m_pAvpgImpl = new cMacAvpgImpl ( *this, m_WorkingMode, m_WorkingThreadMaxCount );
#endif
	} catch( char * pstrException ) {
		PR_TRACE(( g_root, prtERROR, TR "create avpgs failure (%s)", pstrException ));
		m_pAvpgImpl = NULL;
	}

	if (m_pAvpgImpl)
		error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cAVPGS2Impl::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::ObjectPreClose method" );

	// Place your code here
	if (m_pAvpgImpl)
	{
		m_pAvpgImpl->SetActivityMode( cAVPG2_CLIENT_ACTIVITY_MODE_SLEEP );
		delete m_pAvpgImpl;
		m_pAvpgImpl = NULL;

		extern tLONG gMemCounter;

		PR_TRACE(( g_root, prtERROR, TR "memusage counter %d", gMemCounter ));
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
tERROR cAVPGS2Impl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::MsgReceive method" );

	// Place your code here
	if ((pmc_PRODUCT == p_msg_cls_id && pm_PRODUCT_TRUSTEDAPPS_CHANGED == p_msg_id) ||
		(pmc_PROCESS_MONITOR == p_msg_cls_id && pm_PROCESS_MONITOR_PROCESS_EXIT == p_msg_id) )
	{
		error = errOK;
		if (m_pAvpgImpl)
			m_pAvpgImpl->ClearTrustedProcessesCache();
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetActivityMode )
// Parameters are:
tERROR cAVPGS2Impl::SetActivityMode( tAVPG2_CLIENT_ACTIVITY_MODE p_ActivityMode )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::SetActivityMode method" );

	// Place your code here
	PR_TRACE(( g_root, prtERROR, TR "SetActivityMode 0x%x request ", p_ActivityMode ));
	if (m_pAvpgImpl)
		error = m_pAvpgImpl->SetActivityMode( p_ActivityMode );
	else
		error = errUNEXPECTED;

	if (PR_SUCC( error ))
	{
		m_ActivityMode = p_ActivityMode;
		PR_TRACE(( g_root, prtERROR, TR "SetActivityMode %d saved", m_ActivityMode ));
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetWorkingMode )
// Parameters are:
tERROR cAVPGS2Impl::SetWorkingMode( tAVPG2_WORKING_MODE p_WorkingMode )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::SetWorkingMode method" );

	// Place your code here
	PR_TRACE(( g_root, prtERROR, TR "SetWorkingMode 0x%x request", p_WorkingMode ));
	
	if (!m_pAvpgImpl)
		error = errUNEXPECTED;
	else
	{
		m_WorkingMode = p_WorkingMode;
		
		try{
#ifdef _WIN32
			m_pAvpgImpl->ChangeWorkingMode( m_WorkingMode );
#elif defined(__APPLE__)
			delete m_pAvpgImpl;
			m_pAvpgImpl = new cMacAvpgImpl (*this, m_WorkingMode, m_WorkingThreadMaxCount );
#endif // __APPLE__
			error = errOK;
			if (m_pAvpgImpl)
				m_pAvpgImpl->SetActivityMode( m_ActivityMode );
		} catch( char * pstrException ) {
			PR_TRACE(( g_root, prtERROR, TR "create avpgs failure during switch working mode(%s)", pstrException ));
			m_pAvpgImpl = NULL;
			error = errUNEXPECTED;
		}
	}

	PR_TRACE(( g_root, prtERROR, TR "SetWorkingMode 0x%x completed", p_WorkingMode ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, YieldEvent )
// Extended method comment
//    Нотификация для интерфейса что событие продолжает обрабатываться и его необходимо удерживать в очереди не меньше чем Timeout
// Behaviour comment
//    Так же рекомендуется обрабатывать системное сообщение pmc_PROCESSING - pm_PROCESSING_YIELD
// Parameters are:
tERROR cAVPGS2Impl::YieldEvent( hOBJECT p_EventObject, tDWORD p_YieldTimeout )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::YieldEvent method" );

	// Place your code here
	if (m_pAvpgImpl)
		error = m_pAvpgImpl->YieldEvent( p_EventObject, p_YieldTimeout );
	else
		error = errUNEXPECTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ResetDriverCache )
// Parameters are:
tERROR cAVPGS2Impl::ResetDriverCache()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::ResetDriverCache method" );

	// Place your code here
	if (m_pAvpgImpl)
		m_pAvpgImpl->ResetDriverCache();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CreateIO )
// Parameters are:
tERROR cAVPGS2Impl::CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::CreateIO method" );

	// Place your code here
	if (m_pAvpgImpl)
		error = m_pAvpgImpl->CreateIO( p_phObject, p_hObjectName );
	else
		error = errUNEXPECTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetPreverdict )
// Parameters are:
tERROR cAVPGS2Impl::SetPreverdict( hOBJECT p_EventObject, tAVPG2_PROCESS_OBJECT_RESULT_FLAGS p_ResultFlags )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::SetPreverdict method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ResetBanList )
// Parameters are:
tERROR cAVPGS2Impl::ResetBanList()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::ResetBanList method" );

	// Place your code here
	error = errUNEXPECTED;
	if (m_pAvpgImpl)
		error = m_pAvpgImpl->ResetBanList();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AddBan )
// Parameters are:
tERROR cAVPGS2Impl::AddBan( cSerializable* p_User )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG2::AddBan method" );

	// Place your code here
	if (!p_User || !p_User->isBasedOn( cUserInfo::eIID))
		error = errPARAMETER_INVALID;
	else
	{
		cUserInfo* pUserInfo = (cUserInfo*) p_User;
		error = errUNEXPECTED;
		if (m_pAvpgImpl)
			error = m_pAvpgImpl->AddBan( pUserInfo );
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AVPG2". Register function
tERROR cAVPGS2Impl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(AVPG2_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, AVPG2_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Interface for interaction with KLIF", 36 )
//	mpLOCAL_PROPERTY_BUF( pgWorkingThreadMaxCount, cAVPGS2Impl, m_WorkingThreadMaxCount, cPROP_BUFFER_READ_WRITE )
//	mpLOCAL_PROPERTY_BUF( pgClientActivityMode, cAVPGS2Impl, m_ActivityMode, cPROP_BUFFER_READ )
//	mpLOCAL_PROPERTY_BUF( pgWorkingMode, cAVPGS2Impl, m_WorkingMode, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgWorkingThreadMaxCount, cAVPGS2Impl, m_WorkingThreadMaxCount, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgClientActivityMode, cAVPGS2Impl, m_ActivityMode, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgWorkingMode, cAVPGS2Impl, m_WorkingMode, cPROP_BUFFER_READ )
mpPROPERTY_TABLE_END(AVPG2_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AVPG2)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(AVPG2)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AVPG2)
	mEXTERNAL_METHOD(AVPG2, SetActivityMode)
	mEXTERNAL_METHOD(AVPG2, SetWorkingMode)
	mEXTERNAL_METHOD(AVPG2, YieldEvent)
	mEXTERNAL_METHOD(AVPG2, ResetDriverCache)
	mEXTERNAL_METHOD(AVPG2, CreateIO)
	mEXTERNAL_METHOD(AVPG2, SetPreverdict)
	mEXTERNAL_METHOD(AVPG2, ResetBanList)
	mEXTERNAL_METHOD(AVPG2, AddBan)
mEXTERNAL_TABLE_END(AVPG2)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AVPG2::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AVPG2,                              // interface identifier
		PID_AVPGS2,                             // plugin identifier
		0x00000000,                             // subtype identifier
		AVPG2_VERSION,                          // interface version
		VID_RUBIN,                              // interface developer
		&i_AVPG2_vtbl,                          // internal(kernel called) function table
		(sizeof(i_AVPG2_vtbl)/sizeof(tPTR)),    // internal function table size
		&e_AVPG2_vtbl,                          // external function table
		(sizeof(e_AVPG2_vtbl)/sizeof(tPTR)),    // external function table size
		AVPG2_PropTable,                        // property table
		mPROPERTY_TABLE_SIZE(AVPG2_PropTable),  // property table size
		sizeof(cAVPGS2Impl)-sizeof(cObjImpl),   // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AVPG2(IID_AVPG2) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AVPG2_Register( hROOT root ) { return cAVPGS2Impl::Register(root); }
// AVP Prague stamp end




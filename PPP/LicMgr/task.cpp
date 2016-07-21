// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  12 September 2005,  17:40 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- LicMgr
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
#include <iface/i_reg.h>
#include <iface/i_taskmanager.h>
#include <plugin/p_licmgr.h>
// AVP Prague stamp end

#include <structs/s_licmgr.h>
#include <iface/i_io.h>
#include <iface/i_licensing.h>

#define IMPEX_IMPORT_LIB
    #include <plugin/p_win32_nfio.h>


#define err2prtl(err) (PR_SUCC(err) ? prtIMPORTANT : prtERROR)


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable LicMgr : public cTask 
{
private:
// Internal function declarations
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
	hTASKMANAGER m_hTM;        // --
	tTaskState   m_nTaskState; // --
	tDWORD       m_nTaskID;    // --
	hREGISTRY    m_hReg;       // --
// AVP Prague stamp end

	cLicMgrSettings m_Settings;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(LicMgr)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR LicMgr::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR LicMgr::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here

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
tERROR LicMgr::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR LicMgr::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	if( !p_settings || !p_settings->isBasedOn(cLicMgrSettings::eIID) )
		return errPARAMETER_INVALID;

	return m_Settings.assign(*p_settings, true);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR LicMgr::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR LicMgr::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR LicMgr::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	if( p_state != TASK_REQUEST_RUN )
		return errNOT_SUPPORTED;

	tERROR err = errOK;

	cLicensing* pLic = NULL;
	cAutoObj<cIO> pTmpIo;
	cStringObj strKeyFile;
	
	if( PR_SUCC(err) ) 
	{
		cStringObj &strOrigKeyFile = m_Settings.m_sFileName;
		
		PrGetTempDir(cAutoString(strKeyFile), NULL, cCP_ANSI);
		strKeyFile.check_last_slash();
		tDWORD nPos = strOrigKeyFile.find_last_of("\\/");
		strKeyFile += nPos != cStringObj::npos ? strOrigKeyFile.substr(nPos + 1) : strOrigKeyFile;
	}
	if( PR_SUCC(err) ) err = g_root->sysCreateObject(pTmpIo, IID_IO, PID_WIN32_NFIO);
	if( PR_SUCC(err) ) err = pTmpIo->set_pgOBJECT_ACCESS_MODE(fACCESS_WRITE);
	if( PR_SUCC(err) ) err = pTmpIo->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
	if( PR_SUCC(err) ) err = pTmpIo->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_DELETE|fOMODE_SHARE_DENY_WRITE);
	if( PR_SUCC(err) ) err = strKeyFile.copy(pTmpIo, pgOBJECT_FULL_NAME);
	if( PR_SUCC(err) ) err = pTmpIo->sysCreateObjectDone();
	if( PR_FAIL(err) )
	{
		err = PrGetTempFileName(cAutoString(strKeyFile), NULL, cCP_UNICODE);
		if( PR_SUCC(err) ) err = strKeyFile.copy(pTmpIo, pgOBJECT_FULL_NAME);
		if( PR_SUCC(err) ) err = pTmpIo->sysCreateObjectDone();
	}	
	PR_TRACE((g_root, err2prtl(err), "LicMgr\tSetState. Create file done. name %S, (%terr)", strKeyFile.data(), err));
	
	if( PR_SUCC(err) ) err = pTmpIo->SeekWrite(NULL, 0, m_Settings.m_aLicBody.data(), m_Settings.m_aLicBody.size());
	if( PR_FAIL(err) )
		PR_TRACE((g_root, prtERROR, "LicMgr\tSetState. Can't write to temp file (%terr)", err));
	
	
	if( PR_SUCC(err) )
	{
		err = m_hTM->GetService(0, *this, sid_TM_LIC, (hOBJECT*)&pLic);
		if( PR_FAIL(err) )
			PR_TRACE((g_root, prtERROR, "LicMgr\tSetState. Can't get licence service (%terr)", err));
	}
	if( PR_SUCC(err) )
	{
		err = pLic->AddKey((wchar_t *)strKeyFile.data(), m_Settings.m_bSecondary ? KAM_ADD : KAM_REPLACE);
		PR_TRACE((g_root, prtIMPORTANT, "LicMgr\tSetState. AddKey done (%terr), secondary = %d", err, m_Settings.m_bSecondary));
	}
	if( pLic )
		m_hTM->ReleaseService(0, (hOBJECT)pLic), pLic = NULL;

	tDWORD size = sizeof(err);
	sysSendMsg(pmc_TASK_STATE_CHANGED, PR_SUCC(err) ? TASK_STATE_COMPLETED : TASK_STATE_FAILED, NULL, &err, &size);

	return err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR LicMgr::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR LicMgr::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "License manager", 16 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, LicMgr, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, LicMgr, m_nTaskState, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgTASK_SESSION_ID, ((tDWORD)(0xffffffff)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, LicMgr, m_nTaskID, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, LicMgr, m_hReg, cPROP_BUFFER_READ_WRITE )
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
		PID_LICMGR,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_SOBKO,                              // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(LicMgr)-sizeof(cObjImpl),        // memory size
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

tERROR pr_call Task_Register( hROOT root ) { return LicMgr::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end




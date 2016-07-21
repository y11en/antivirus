// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  05 July 2007,  17:00 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_root.h>

#include <Extract/plugin/p_miniarc.h>
#include <Extract/plugin/p_minizip.h>

#include <Mail/plugin/p_spamtest.h>
#include <Mail/structs/s_spamtest.h>

#include <Updater/updater.h>
// AVP Prague stamp end


#include "kisaspam/kis/MessageInfo/MessageInfo.h"
#include "kisaspam/kis/DataBase.h"
#include "kisaspam/kis/SpamFilter.h"
#include <memory>

tERROR CreateFileIO(hIO& p, tSTRING filename)
{
	tERROR err = g_root->sysCreateObject((hOBJECT*)&p, IID_IO, PID_NATIVE_FIO, 0);
	if (PR_FAIL(err))
		return err;
	if (PR_FAIL(err = p->propSetStr(0, pgOBJECT_FULL_NAME, filename, strlen(filename), cCP_ANSI))
		|| PR_FAIL(err = p->sysCreateObjectDone()))
	{
		p->sysCloseObject();
		p = NULL;
	}
	return err;
}

#define PR_CREATEFILE_IO_INTERNAL(p, os, p_int, filename)	\
	if ( PR_SUCC(err) )	\
	if ( PR_SUCC(err = p->sysCreateObject((hOBJECT*)&os, IID_OS, PID_UNIVERSAL_ARCHIVER)))	\
	if ( PR_SUCC(err = os->propSetDWord(ppMINI_ARCHIVER_PID, PID_MINIZIP)))	\
	if ( PR_SUCC(err = os->sysCreateObjectDone()))	\
		err = os->IOCreate(&p_int, (hOBJECT)filename, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
#define PR_FREE(p)	  if (p) {p->sysCloseObject(); p = NULL;}
#define PR_RELEASE(p) if (p) {p->Release(); p = NULL;}
extern IStream * CreateIStreamIO(hIO p_io, tBOOL p_del_io); 

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CSpamtestService : public cTask {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
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
	cSpamtestSettings m_settings; // --
	hTASKMANAGER      m_tm;       // --
	tDWORD            m_state;    // --
// AVP Prague stamp end



protected:
	tERROR InitSpamtest(bool bLockLoadBases);
	void DoneSpamtest();


private:
	CDataBase m_filterDB;
	cCriticalSection* m_lock;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CSpamtestService)
};
// AVP Prague stamp end



tERROR CSpamtestService::InitSpamtest(bool bLockLoadBases)
{
	cBasesLoadLocker Locker((cObject *)this, cUPDATE_CATEGORY_AS,
        bLockLoadBases ? cBasesLoadLocker::withLock : cBasesLoadLocker::noLock);

	hIO io_gsg = NULL; IStream* lp_gsg = NULL;
	hIO io_pdb = NULL; IStream* lp_pdb = NULL;

	cERROR err = errOK;
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(m_settings.m_szBaseFolder), 0, 0);
	cStringObj szBaseGSG = m_settings.m_szBaseFolder;
	cStringObj szBasePDB = m_settings.m_szBaseFolder;
	cStringObj szBaseRecentTerms;
	szBaseGSG += "\\cfbase-s.gsg";
	szBasePDB += "\\profiles.pdb";
	if(m_settings.m_bUseRecentTerms)
	{
		szBaseRecentTerms = m_settings.m_szBaseFolder;
		szBaseRecentTerms += "\\rterms.trn";
	}
	PR_TRACE((this, prtIMPORTANT, "spmsvc\tInitializing spamtest (GSG: \"%S\", PDB: \"%S\")..."
		, szBaseGSG.data(), szBasePDB.data()));

	if ( m_settings.m_bUseGSG )
	{
		if (PR_SUCC(err = CreateFileIO(io_gsg, (tSTRING)szBaseGSG.c_str(cCP_ANSI))))
			lp_gsg = CreateIStreamIO(io_gsg, cFALSE);
		else
			PR_TRACE((this, prtERROR, "spmsvc\tFailed to create IO for gsg (%terr)", err));
	}
	if ( PR_SUCC(err) && m_settings.m_bUsePDB )
	{
		if (PR_SUCC(err = CreateFileIO(io_pdb, (tSTRING)szBasePDB.c_str(cCP_ANSI))))
			lp_pdb = CreateIStreamIO(io_pdb, cFALSE);
		else
			PR_TRACE((this, prtERROR, "spmsvc\tFailed to create IO for pdb (%terr)", err));
	}
	if ( PR_SUCC(err) )
	{
		cAutoCS cs(m_lock, true);
		if ( FAILED( m_filterDB.Init(
			lp_gsg, 
			lp_pdb,
			szBaseRecentTerms.c_str(cCP_ANSI)
			)))
		{
			m_filterDB.Done();
			err = errOBJECT_CANNOT_BE_INITIALIZED;
			PR_TRACE((this, prtERROR, "spmsvc\tFailed to init filter DB"));
		}
		else
			PR_TRACE((this, prtIMPORTANT, "spmsvc\tFilter DB succesfully initialized"));
	}
	PR_RELEASE(lp_gsg);
	PR_RELEASE(lp_pdb);
	PR_FREE(io_gsg);
	PR_FREE(io_pdb);
	return err;
}

void CSpamtestService::DoneSpamtest() 
{
	cAutoCS cs(m_lock, true);
	m_filterDB.Done();
	PR_TRACE((this, prtIMPORTANT, "spmsvc\tFilter DB released"));
}


// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CSpamtestService::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_SPAMTEST, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_UPDATER, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	if (PR_SUCC(error))
		PR_TRACE((this, prtIMPORTANT, "spmsvc\tSpamtest service succesfully created"));
	else
		PR_TRACE((this, prtERROR, "spmsvc\tSpamtest service failed to create (%terr)", error));

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
tERROR CSpamtestService::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	if ( m_state == TASK_STATE_RUNNING &&
			p_msg_cls_id == pmc_SPAMTEST && p_msg_id == msg_ISSPAMMESSAGE )
	{
		cAutoCS cs(m_lock, true);
		if ( m_filterDB.IsValid() )
		{
			PR_TRACE((this, prtNOTIFY, "spmsvc\tChecking a message for the spam..."));
			cSpamtestParams* hSpamtestParams = (cSpamtestParams*)p_par_buf;
			hSpamtestParams->m_os = p_ctx;
			MESSAGE_INFO_KIS info((hOS)hSpamtestParams->m_os);
			CSpamFilter filter(&m_filterDB);
			SPAM_TYPE uType = ST_NOT_SPAM;
			STATISTICS hStat;
			HRESULT hr = filter.Process(
				hSpamtestParams->m_bUsePDB,
				hSpamtestParams->m_bUseGSG,
				hSpamtestParams->m_bUseRecentTerms,
				&info, &uType, &hStat
				);
			if ( SUCCEEDED(hr) )
			{
				hSpamtestParams->m_result = uType;
				hSpamtestParams->m_facility = hStat.dwFacility;
				error = errOK_DECIDED;
				PR_TRACE((this, prtNOTIFY, "spmsvc\tMessage type is %i", UINT(uType)));
			}
			else
				PR_TRACE((this, prtERROR, "spmsvc\tSpamFilter process failed (%08x)", hr));
		}
	}
	if(p_msg_cls_id == pmc_UPDATER && p_msg_id == pm_COMPONENT_PREINSTALLED)
	{
		cSerializable *pSer = p_par_buf_len == SER_SENDMSG_PSIZE ? (cSerializable *)p_par_buf : NULL;
		cUpdaterUpdatedFileList *updatedFiles = pSer && pSer->getIID() == cUpdaterUpdatedFileList::eIID ?
			reinterpret_cast<cUpdaterUpdatedFileList *>(pSer) : NULL;
		if(!updatedFiles)
			return errPARAMETER_INVALID;

		// reinitialization in case Anti-Spam is updated
		if(updatedFiles->m_selectedComponentIdentidier == cUPDATE_CATEGORY_AS)
		{
			PR_TRACE((this, prtIMPORTANT, "spmsvc\tBases updated, loading it..."));
			error = InitSpamtest(false);
			PR_TRACE((this, prtIMPORTANT, "spmsvc\tDatabase reloaded (%terr)", error));
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CSpamtestService::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	error = m_settings.assign(*p_settings, false);

	if(PR_SUCC(error))
	{
		error = InitSpamtest(true);
		if(PR_FAIL(error))
		{
			m_state = TASK_STATE_FAILED;
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CSpamtestService::GetSettings( cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	error = p_settings->assign(m_settings, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CSpamtestService::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CSpamtestService::SetState( tTaskRequestState p_state ) {
	tERROR error = warnTASK_STATE_CHANGED;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	switch ( p_state )
	{
	case TASK_REQUEST_RUN:
		{
			PR_TRACE((this, prtIMPORTANT, "spmsvc\tStarting service..."));
			cAutoCS cs(m_lock, true);
			if ( !m_filterDB.IsValid() )
				error = InitSpamtest(true);
			if ( PR_FAIL(error) || !m_filterDB.IsValid() )
			{
				m_state = TASK_STATE_FAILED;
				error = errOBJECT_CANNOT_BE_INITIALIZED;
				PR_TRACE((this, prtERROR, "spmsvc\tFailed to start"));
			}
			else
			{
				m_state = TASK_STATE_RUNNING;
				PR_TRACE((this, prtIMPORTANT, "spmsvc\tService started"));
			}
		}
		break;
	case TASK_REQUEST_STOP:
		{
			PR_TRACE((this, prtIMPORTANT, "spmsvc\tStoping service..."));
			cAutoCS cs(m_lock, true);
			if ( m_filterDB.IsValid() )
				DoneSpamtest();
			m_state = TASK_STATE_COMPLETED;
			PR_TRACE((this, prtIMPORTANT, "spmsvc\tService stopped"));
		}
		break;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CSpamtestService::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CSpamtestService::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "0", 2 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CSpamtestService, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInitDone)
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
		IID_TASK,                             // interface identifier
		PID_SPAMTEST,                           // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                         // interface version
		VID_DENISOV,                            // interface developer
		&i_Task_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Task_vtbl,                         // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),   // external function table size
		Task_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable), // property table size
		sizeof(CSpamtestService)-sizeof(cObjImpl),// memory size
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

tERROR pr_call Task_Register( hROOT root ) { return CSpamtestService::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_STATE
// You have to implement propetry: pgTASK_SESSION_ID
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end




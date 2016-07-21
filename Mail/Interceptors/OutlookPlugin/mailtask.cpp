// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  18 May 2004,  13:03 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- mailtask.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define MailTask_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <plugin/p_outlookplugintask.h>
#include <iface/i_root.h>
#include <iface/i_reg.h>
#include <iface/i_task.h>
// AVP Prague stamp end



#include <structs/s_mc.h>
#include <structs/s_mc_oe.h>
#include <iface/i_task.h>
#include "../remote_mbl.h"


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable COutlookPluginTask : public cTask {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );
	tERROR pr_call Process( hOBJECT p_hTopIO, tTotalPrimaryAction* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut );

// Data declaration
	hREGISTRY     m_config;     // Настройки (не используются). Не путать с настройками, вдвигаемыми через SetSettings и предоставляемыми через GetSettings
	tDWORD        m_state;      // Текущее состояние задачи
	hTASKMANAGER  m_tm;         // Указатель на объект БизнесЛогики
	tDWORD        m_session_id; // Идентификатор сессии (не используется) при работе в терминальном режиме Windows
	cMCOESettings m_settings;   // Настройки траффик-монитора
// AVP Prague stamp end



private:
	cMCOEStatistics m_statistics;

	
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(COutlookPluginTask)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MailTask". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR COutlookPluginTask::ObjectInitDone() {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "MailTask::ObjectInitDone method" );

	// Place your code here
	tCHAR* plugin_list[] = {""};
	error = RemoteMBLInit(NULL, plugin_list, 0);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR COutlookPluginTask::ObjectPreClose() {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "MailTask::ObjectPreClose method" );

	// Place your code here
	error = RemoteMBLTerm();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR COutlookPluginTask::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::SetSettings method" );

	// Place your code here
	error = m_settings.assign(*p_settings, true);
	if ( m_state == TASK_STATE_RUNNING )
	{
		PRUnregisterObject(this);
		PRUnregisterObject(this);
		if ( PR_SUCC(error) && m_settings.m_bCheckIncoming ) error = PRRegisterObject("MC_OU_Incoming", this);
		if ( PR_SUCC(error) && m_settings.m_bCheckOutgoing ) error = PRRegisterObject("MC_OU_Outgoing", this);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR COutlookPluginTask::GetSettings( cSerializable* p_settings ) {
	tERROR error = errNOT_MATCHED;
	PR_TRACE_FUNC_FRAME( "MailTask::GetSettings method" );

	// Place your code here
	if ( p_settings )
	{
		if ( p_settings->isBasedOn(cMCSettings::eIID) )
		{
			// Перенаправляем запрос в MBL
			hMAILTASK hWorker = (hMAILTASK)RemoteMBLGet();
			if ( hWorker )
				error = hWorker->GetSettings(p_settings);
		}
		else
			*(cMCOESettings*)p_settings = m_settings;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR COutlookPluginTask::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "MailTask::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR COutlookPluginTask::SetState( tTaskState p_state ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::SetState method" );

	// Place your code here
	m_state = p_state;
	PRUnregisterObject(this);
	PRUnregisterObject(this);

	switch (m_state)
	{
	case TASK_REQUEST_RUN:
		if ( PR_SUCC(error) && m_settings.m_bCheckIncoming ) error = PRRegisterObject("MC_OU_Incoming", this);
		if ( PR_SUCC(error) && m_settings.m_bCheckOutgoing ) error = PRRegisterObject("MC_OU_Outgoing", this);
		break;
	case TASK_REQUEST_STOP:
		PRUnregisterObject(this);
		PRUnregisterObject(this);
		break;
	default:
		error = errTASK_STATE_UNKNOWN;
	}

	sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR COutlookPluginTask::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "MailTask::GetStatistics method" );

	// Place your code here
	*(cMCOEStatistics*)p_statistics = m_statistics;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Process )
// Parameters are:
//   "p_szMailTaskName"      : [IN] Уникальное имя задачи почтовой фильтрации
//                             См. mailchecker_types.h::cMBLParams::szMailTaskName
tERROR COutlookPluginTask::Process( hOBJECT p_hTopIO, tTotalPrimaryAction* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "MailTask::Process method" );

	// Place your code here
	error = sysSendMsg( pmc_MAILTASK_PROCESS, NULL, p_hTopIO, NULL, NULL);
	if ( PR_SUCC(error) )
		m_statistics.m_dwMailCount++;
//	hMAILTASK hWorker = (hMAILTASK)RemoteMBLGet();
//	if ( hWorker )
//	{
//		error = hWorker->Process( p_hTopIO, p_hTotalPrimaryAction, p_szMailTaskName, p_dwTimeOut );
//		m_statistics.m_dwMailCount++;
//	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "MailTask". Register function
tERROR COutlookPluginTask::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(MailTask_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, MailTask_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Interface Comment", 18 )
	mSHARED_PROPERTY( pgTASK_TYPE, ((tDWORD)(TASK_MC_OP)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_CONFIG, COutlookPluginTask, m_config, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, COutlookPluginTask, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, COutlookPluginTask, m_state, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, COutlookPluginTask, m_session_id, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(MailTask_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(MailTask)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(MailTask)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(MailTask)
	mEXTERNAL_METHOD(MailTask, SetSettings)
	mEXTERNAL_METHOD(MailTask, GetSettings)
	mEXTERNAL_METHOD(MailTask, AskAction)
	mEXTERNAL_METHOD(MailTask, SetState)
	mEXTERNAL_METHOD(MailTask, GetStatistics)
	mEXTERNAL_METHOD(MailTask, Process)
mEXTERNAL_TABLE_END(MailTask)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "MailTask::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_MAILTASK,                           // interface identifier
		PID_OUTLOOKPLUGINTASK,                  // plugin identifier
		0x00000000,                             // subtype identifier
		MailTask_VERSION,                       // interface version
		VID_DENISOV,                            // interface developer
		&i_MailTask_vtbl,                       // internal(kernel called) function table
		(sizeof(i_MailTask_vtbl)/sizeof(tPTR)), // internal function table size
		&e_MailTask_vtbl,                       // external function table
		(sizeof(e_MailTask_vtbl)/sizeof(tPTR)), // external function table size
		MailTask_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(MailTask_PropTable),// property table size
		sizeof(COutlookPluginTask)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"MailTask(IID_MAILTASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call MailTask_Register( hROOT root ) { return COutlookPluginTask::Register(root); }
// AVP Prague stamp end




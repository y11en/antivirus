// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  31 March 2006,  12:03 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- tm.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define TaskManager_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <plugin/p_tm.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Includes for interface,  )
#include "tm.h"
// AVP Prague stamp end

#include "i_fake_taskmanager.h"

#include <plugin/p_report.h>
#include <plugin/p_qb.h>
#include <plugin/p_netdetect.h>
#include <iface/i_tmclient.h>

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "TaskManager". Static(shared) property table variables
// AVP Prague stamp end

#define TM_COMMAND_IDLE_WORK      1
#define TM_COMMAND_EXIT_REQUEST   2

// Для облегчения чтения дампов, указатель на tm будем хранить в статической памяти
TMImpl* g_tm = NULL;

TMImpl::TMImpl()
{

}

tERROR  TMImpl::Init( tDWORD flags ) {return errOK;};
 tERROR  TMImpl::Exit( tDWORD cause ) {return errOK;}; // -- close TM session
 tERROR  TMImpl::GetProfileInfo( const tCHAR* profile_name, cSerializable* info ) {return errOK;}; // -- returns combined task profile, task settings or task schedule settings
 tERROR  TMImpl::SetProfileInfo( const tCHAR* profile_name, const cSerializable* info, hOBJECT client, tDWORD delay ) {return errOK;}; // -- replace combined task profile, task settings or task schedule settings for the profile
 tERROR  TMImpl::EnableProfile( const tCHAR* profile_name, tBOOL enabled, tBOOL recursive, hOBJECT client ) {return errOK;};
 tERROR  TMImpl::IsProfileEnabled( const tCHAR* profile_name, tBOOL* enabled ) {return errOK;};
 tERROR  TMImpl::DeleteProfile( const tCHAR* profile_name, hOBJECT client ) {return errOK;};
 tERROR  TMImpl::SetProfileState( const tCHAR* profile_name, tTaskRequestState state, hOBJECT client, tDWORD delay, tDWORD* task_id = NULL) {return errOK;};
 tERROR  TMImpl::GetProfileReport( const tCHAR* profile_name, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ) {return errOK;};
 tERROR  TMImpl::CloneProfile( const tCHAR* profile_name, cProfile* profile, tDWORD flags, hOBJECT client ) {return errOK;}; // -- clones profile
 tERROR  TMImpl::GetSettingsByLevel( const tCHAR* profile_name, tDWORD settings_level, tBOOL cumulative, cSerializable* settings ) {return errOK;}; // -- get settings by task type and settings level
 tERROR  TMImpl::GetProfilePersistentStorage( const tCHAR* profile_name, hOBJECT dad, hREGISTRY* storage, tBOOL rw ) {return errOK;};
 tERROR  TMImpl::OpenTask( tTaskId* task_id, const tCHAR* profile_name, tDWORD open_flags, hOBJECT client ) {return errOK;};
 tERROR  TMImpl::ReleaseTask( tTaskId task_id ) {return errOK;};
 tERROR  TMImpl::GetTaskState( tTaskId task_id, tTaskState* task_state ) {return errOK;}; // -- returns current task state
 tERROR  TMImpl::SetTaskState( tTaskId task_id, tTaskRequestState requested_state, hOBJECT client, tDWORD delay ) {return errOK;}; // -- requests task for state changing
 tERROR  TMImpl::GetTaskInfo( tTaskId task_id, cSerializable* info ) {return errOK;}; // -- returns asked info by task_id
 tERROR  TMImpl::SetTaskInfo( tTaskId task_id, const cSerializable* info ) {return errOK;}; // -- returns asked info by task_id
 tERROR  TMImpl::GetTaskReport( tTaskId task_id, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ) {return errOK;};
 tERROR  TMImpl::GetTaskPersistentStorage( tTaskId task_id, hOBJECT dad, hREGISTRY* storage, tBOOL rw ) {return errOK;};
 tERROR  TMImpl::AskAction( tTaskId task_id, tDWORD action_id, cSerializable* params ) {return errOK;}; // -- invokes extension method implemented by task
 tERROR  TMImpl::AskTaskAction( tTaskId task_id, tDWORD action_id, cSerializable* params ) {return errOK;}; // -- invokes extension method implemented by task
 tERROR  TMImpl::GetService( tTaskId task_id, hOBJECT client, const tCHAR* service_id, hOBJECT* service, tDWORD start_delay = 0)
 {
	 if ( !strcmp(service_id, AVP_SERVICE_TRAFFICMONITOR) )
	 {
			*service = traffic_Monitor;
			return errOK;
	 }
	 return errNOT_FOUND;
 };
 tERROR  TMImpl::ReleaseService( tTaskId task_id, hOBJECT service ) {return errOK;};
 tERROR  TMImpl::LockTaskSettings( hOBJECT task, cSerializable** settings, tBOOL for_read_only ) {return errOK;};
 tERROR  TMImpl::UnlockTaskSettings( hOBJECT task, cSerializable* settings, tBOOL modified ) {return errOK;};
 tERROR  TMImpl::RegisterRemoteTask( hOBJECT tmhost, hOBJECT task, cSerializable* info, hOBJECT* host ) {return errOK;};
 tERROR  TMImpl::RegisterClient( hOBJECT client ) {return errOK;};
 tERROR  TMImpl::UnregisterClient( hOBJECT client ) {return errOK;};
 tERROR  TMImpl::CleanupItems( tDWORD item_type, const tDATETIME* time_stamp, hOBJECT client ) {return errOK;}; // -- clean old reports, quarantine and backup items by time point
 tERROR  TMImpl::AddLicKey( hSTRING key_name ) {return errOK;}; // -- add license key
 tERROR  TMImpl::AddRecordToTaskReport( tTaskId task_id, tDWORD msg_cls, const cSerializable* record ) {return errOK;};
 tERROR  TMImpl::GetTaskReportInfo( tDWORD flags, tDWORD index, const cSerializable* info ) {return errOK;};
 tERROR TMImpl::DeleteTaskReports( tTaskId task_id, tDWORD flags ) {return errOK;};
// AVP Prague stamp end

 tERROR TMImpl::ObjectInit()
 {

	 return errOK;
 }


 tERROR TMImpl::ObjectInitDone()
 {
	 return errOK;
 }
 // AVP Prague stamp end

 // AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
 // Extended method comment
 //    Kernel warns object it is going to close all children
 // Behaviour comment
 //    Object takes all necessary "before preclosing" actions
 // Parameters are:
 tERROR TMImpl::ObjectPreClose()
 {
	 return errOK;
 }

 // AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
 tERROR TMImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
 {
	 return errOK;
 }


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "TaskManager". Register function
tERROR TMImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(TaskManager_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, TaskManager_VERSION )
	mpLOCAL_PROPERTY_BUF( pgTM_TRAFFICMONITOR_PTR, TMImpl, traffic_Monitor, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTEST_MANAGER_PTR, TMImpl, m_testManager,     cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
//  mpLOCAL_PROPERTY_BUF( pgTM_PRODUCT_CONFIG, TMImpl, m_config, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
//	mpLOCAL_PROPERTY_BUF( pgTM_IS_GUI_CONNECTED, TMImpl, m_bIsGuiConnected, cPROP_BUFFER_READ )
mpPROPERTY_TABLE_END(TaskManager_PropTable)
// AVP Prague stamp end

		
		
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(TaskManager)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(TaskManager)
// AVP Prague stamp end

		
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(TaskManager)
	mEXTERNAL_METHOD(TaskManager, Init)
	mEXTERNAL_METHOD(TaskManager, Exit)
	mEXTERNAL_METHOD(TaskManager, GetProfileInfo)
	mEXTERNAL_METHOD(TaskManager, SetProfileInfo)
	mEXTERNAL_METHOD(TaskManager, EnableProfile)
	mEXTERNAL_METHOD(TaskManager, IsProfileEnabled)
	mEXTERNAL_METHOD(TaskManager, DeleteProfile)
	mEXTERNAL_METHOD(TaskManager, SetProfileState)
	mEXTERNAL_METHOD(TaskManager, GetProfileReport)
	mEXTERNAL_METHOD(TaskManager, CloneProfile)
	mEXTERNAL_METHOD(TaskManager, GetSettingsByLevel)
	mEXTERNAL_METHOD(TaskManager, GetProfilePersistentStorage)
	mEXTERNAL_METHOD(TaskManager, OpenTask)
	mEXTERNAL_METHOD(TaskManager, ReleaseTask)
	mEXTERNAL_METHOD(TaskManager, GetTaskState)
	mEXTERNAL_METHOD(TaskManager, SetTaskState)
	mEXTERNAL_METHOD(TaskManager, GetTaskInfo)
	mEXTERNAL_METHOD(TaskManager, SetTaskInfo)
	mEXTERNAL_METHOD(TaskManager, GetTaskReport)
	mEXTERNAL_METHOD(TaskManager, GetTaskPersistentStorage)
	mEXTERNAL_METHOD(TaskManager, AskAction)
	mEXTERNAL_METHOD(TaskManager, AskTaskAction)
	mEXTERNAL_METHOD(TaskManager, GetService)
	mEXTERNAL_METHOD(TaskManager, ReleaseService)
	mEXTERNAL_METHOD(TaskManager, LockTaskSettings)
	mEXTERNAL_METHOD(TaskManager, UnlockTaskSettings)
	mEXTERNAL_METHOD(TaskManager, RegisterRemoteTask)
	mEXTERNAL_METHOD(TaskManager, RegisterClient)
	mEXTERNAL_METHOD(TaskManager, UnregisterClient)
	mEXTERNAL_METHOD(TaskManager, CleanupItems)
	mEXTERNAL_METHOD(TaskManager, AddLicKey)
	mEXTERNAL_METHOD(TaskManager, AddRecordToTaskReport)
	mEXTERNAL_METHOD(TaskManager, GetTaskReportInfo)
	mEXTERNAL_METHOD(TaskManager, DeleteTaskReports)
mEXTERNAL_TABLE_END(TaskManager)
// AVP Prague stamp end

		
// AVP Prague stamp begin( Registration call,  )
	PR_TRACE_FUNC_FRAME_( *root, "TaskManager::Register method", &error );
	
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASKMANAGER,                        // interface identifier
		PID_TM,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		TaskManager_VERSION,                    // interface version
		VID_MEZHUEV,                            // interface developer
		&i_TaskManager_vtbl,                    // internal(kernel called) function table
		(sizeof(i_TaskManager_vtbl)/sizeof(tPTR)),// internal function table size
		&e_TaskManager_vtbl,                    // external function table
		(sizeof(e_TaskManager_vtbl)/sizeof(tPTR)),// external function table size
		TaskManager_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(TaskManager_PropTable),// property table size
		sizeof(TMImpl)-sizeof(cObjImpl),        // memory size
		IFACE_SYSTEM                            // interface flags
	);
	
#ifdef _PRAGUE_TRACE_
	if( PR_FAIL(error) )
		PR_TRACE( (root,prtDANGER,"TaskManager(IID_TASKMANAGER) registered [%terr]",error) );
#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

	
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call TaskManager_Register( hROOT root ) { return TMImpl::Register(root); }
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgBL_PRODUCT_NAME
// You have to implement propetry: pgBL_PRODUCT_VERSION
// You have to implement propetry: pgBL_PRODUCT_PATH
// You have to implement propetry: pgBL_PRODUCT_DATA_PATH
// You have to implement propetry: pgBL_COMPANY_NAME
// AVP Prague stamp end


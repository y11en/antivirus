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

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __tm_cpp__17424619_7dc8_427c_bdad_11c910fb62b7 )
#define __tm_cpp__17424619_7dc8_427c_bdad_11c910fb62b7
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <plugin/p_tm.h>
// AVP Prague stamp end

#include <iface/e_ktrace.h>
#include <iface/i_taskmanager.h>
#include <iface/i_qb.h>
#include <iface/i_licensing.h>
#include <iface/i_scheduler.h>
#include <iface/i_mutex.h>
#include <iface/i_netdetect.h>
#include <iface/i_os.h>
#include <plugin/p_regmap.h>
#include <plugin/p_win32_nfio.h>
#include <plugin/p_cryptohelper.h>
#include <plugin/p_win32loader.h>

#include <pr_remote.h>
#include <pr_cpp.h>
#include <pr_time.h>
#include <iface/i_productlogic.h>
#include <iface/i_threadpool.h>
#include <iface/i_csect.h>
#include <iface/i_event.h>
#include <iface/i_report.h>
#include <iface/i_token.h>
#include <structs/s_taskmanager.h>
#include <structs/s_avs.h>
#include <structs/s_qb.h>
//#include "tm_util.h"

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h>
#include <plugin/p_string.h>


#include <stdio.h>

#include "../IMAP4UnitTests/TestManager.h"

// AVP Prague stamp begin( C++ class declaration,  )
struct TMImpl :
	public cTaskManager
{
private:
	TMImpl();

// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call Init( tDWORD p_flags );
	tERROR pr_call Exit( tDWORD p_cause );

// profile methods
	tERROR pr_call GetProfileInfo( const tCHAR* p_profile_name, cSerializable* p_info );
	tERROR pr_call SetProfileInfo( const tCHAR* p_profile_name, const cSerializable* p_info, hOBJECT p_client, tDWORD p_delay );
	tERROR pr_call EnableProfile( const tCHAR* p_profile_name, tBOOL p_enabled, tBOOL p_recursive, hOBJECT p_client );
	tERROR pr_call IsProfileEnabled( const tCHAR* p_profile_name, tBOOL* p_enabled );
	tERROR pr_call DeleteProfile( const tCHAR* p_profile_name, hOBJECT p_client );
	tERROR pr_call SetProfileState( const tCHAR* p_profile_name, tTaskRequestState p_state, hOBJECT p_client, tDWORD p_delay, tDWORD* p_task_id );
	tERROR pr_call GetProfileReport( const tCHAR* p_profile_name, const tCHAR* p_report_id, hOBJECT p_client, tDWORD p_access, hREPORT* p_report );
	tERROR pr_call CloneProfile( const tCHAR* p_profile_name, cProfile* p_profile, tDWORD p_flags, hOBJECT p_client );
	tERROR pr_call GetSettingsByLevel( const tCHAR* p_profile_name, tDWORD p_settings_level, tBOOL p_cumulative, cSerializable* p_settings );
	tERROR pr_call GetProfilePersistentStorage( const tCHAR* p_profile_name, hOBJECT p_dad, hREGISTRY* p_storage, tBOOL p_rw );

// task management
	tERROR pr_call OpenTask( tTaskId* p_task_id, const tCHAR* p_profile_name, tDWORD p_open_flags, hOBJECT p_client );
	tERROR pr_call ReleaseTask( tTaskId p_task_id );
	tERROR pr_call GetTaskState( tTaskId p_task_id, tTaskState* p_task_state );
	tERROR pr_call SetTaskState( tTaskId p_task_id, tTaskRequestState p_requested_state, hOBJECT p_client, tDWORD p_delay );
	tERROR pr_call GetTaskInfo( tTaskId p_task_id, cSerializable* p_info );
	tERROR pr_call SetTaskInfo( tTaskId p_task_id, const cSerializable* p_info );
	tERROR pr_call GetTaskReport( tTaskId p_task_id, const tCHAR* p_report_id, hOBJECT p_client, tDWORD p_access, hREPORT* p_report );
	tERROR pr_call GetTaskPersistentStorage( tTaskId p_task_id, hOBJECT p_dad, hREGISTRY* p_storage, tBOOL p_rw );

// task extention methods
	tERROR pr_call AskAction( tTaskId p_task_id, tDWORD p_action_id, cSerializable* p_params );
	tERROR pr_call AskTaskAction( tTaskId p_task_id, tDWORD p_action_id, cSerializable* p_params );
	tERROR pr_call GetService( tTaskId p_task_id, hOBJECT p_client, const tCHAR* p_service_id, hOBJECT* p_service, tDWORD p_start_delay );
	tERROR pr_call ReleaseService( tTaskId p_task_id, hOBJECT p_service );
	tERROR pr_call LockTaskSettings( hOBJECT p_task, cSerializable** p_settings, tBOOL p_for_read_only );
	tERROR pr_call UnlockTaskSettings( hOBJECT p_task, cSerializable* p_settings, tBOOL p_modified );
	tERROR pr_call RegisterRemoteTask( hOBJECT p_tmhost, hOBJECT p_task, cSerializable* p_info, hOBJECT* p_host );

// register client methods
	tERROR pr_call StartGUI(tSESSION session);
	tERROR pr_call RegisterClient( hOBJECT p_client );
	tERROR pr_call UnregisterClient( hOBJECT p_client );
	tERROR pr_call UnregisterClientEx(hOBJECT p_client, tBOOL bFailOver);

// maintenance methods
	tERROR pr_call CleanupItems( tDWORD p_item_type, const tDATETIME* p_time_stamp, hOBJECT p_client );
	tERROR pr_call AddLicKey( hSTRING p_key_name );

// -report methods
	tERROR pr_call AddRecordToTaskReport( tTaskId p_task_id, tDWORD p_msg_cls, const cSerializable* p_record );
	tERROR pr_call GetTaskReportInfo( tDWORD p_flags, tDWORD p_index, const cSerializable* p_info );
	tERROR pr_call DeleteTaskReports( tTaskId p_task_id, tDWORD p_flags );

// Data declaration

	hOBJECT traffic_Monitor;
	TestManager*	m_testManager;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION_NO_DELETE(TMImpl)
};
// AVP Prague stamp end

//  mDECLARE_INITIALIZATION_NO_DELETE(TMImpl)


// AVP Prague stamp begin( Header endif,  )
#endif // tm_cpp
// AVP Prague stamp end


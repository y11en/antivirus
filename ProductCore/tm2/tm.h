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
#include <iface/i_licensing.h>
#include <iface/i_scheduler.h>
#include <iface/i_mutex.h>
#include <iface/i_netdetect.h>
#include <iface/i_os.h>
#include <plugin/p_regmap.h>
#include <plugin/p_win32_nfio.h>
#include <plugin/p_cryptohelper.h>
#include <plugin/p_win32loader.h>
#include "tm_profile.h"
#include "tm_report.h"
#include <stdio.h>

// AVP Prague stamp begin( C++ class declaration,  )
struct TMImpl :
	public cTaskManager,
	public cTmProfile,
	public cThreadPoolBase,
	public CPRRemotePrague
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
	cRegistry* m_config;          // --
	tDWORD     m_bIsGuiConnected; // --
// AVP Prague stamp end

public:
	cProductLogic*    m_bl;
	cCriticalSection* m_pcs;
	cEvent*           m_stopev;
	cCryptoHelper*    m_hCryptoHelper;
	cTmReport         m_report;
	cCfgEx            m_policy;
	cScheduler*       m_scheduler;
	cLicensing*       m_lic;
	cNetDetect*       m_net_detector;

	tBOOL       m_isCreated;
	tDWORD      m_dwInitFlags;
	tBOOL       m_safeBoot;
	tBOOL       m_isClosing;
	tBOOL       m_isClosed;
	tDWORD      m_dwFuncLevel;
	tDWORD      m_dwUnique;

	tDWORD      m_dwEnabledPolicyBit;
	tDWORD      m_dwLevelPolicyBit;

	tDWORD      m_prop_taskid;
	tDWORD      m_prop_task_type;
	tDWORD      m_prop_profile;
	tDWORD      m_prop_this;

	struct cClient
	{
		cClient(tDWORD session_id = -1, hOBJECT client = NULL) : m_client(client), m_session_id(session_id), m_token(NULL){}

		bool operator == (const cClient& c){ return c.m_client ? c.m_client == m_client : c.m_session_id == m_session_id; }
		bool isvalid(){ return m_client != NULL || m_session_id != -1; }

		cObj*      m_client;
		tDWORD     m_session_id;
		cToken*    m_token;
	};

	cVector<cClient> m_clients;

	struct cHost
	{
		cHost(tDWORD pid = 0) : m_pid(pid){}
		bool operator == (const cHost& c){ return c.m_pid == m_pid; }

		cTmHost*   m_host;
		tDWORD     m_pid;
		tDWORD     m_ref;
	};

	cVector<cHost>   m_hosts;

public:
	cTmProfile* FindProfile(const tCHAR* profile, tDWORD task_id, hOBJECT task);

	cTaskHeader* InitTaskHeader(cSerializable* data, cObj* task);
	tERROR      AskActionEx(tDWORD action_id, cSerializable* params, hOBJECT sender);
	tERROR      AskClientAction(cObject* client, tDWORD actionId, cSerializable* actionInfo);
	tERROR      CloseClients();

	tERROR      InitProductLogic();
	tERROR      InitProfile(cTmProfile* pParent);
	tERROR      InitLicensing();
	tERROR      InitScheduler();
	tERROR      InitPolicy();

	tERROR      ProcessScheduledEvent(cScheduleParams* event);
	tERROR      CheckInet();

	tERROR      ApplyPolicy(cSerializable* data, cTmProfile* profile);
	tERROR      ApplyPolicy(cTmProfile* profile, cCfgEx* policy, bool bDelete = false);
	tERROR      ApplyPolicyEx(cTmProfile* profile, cCfgEx* policy);
	tERROR      GetPolicy(cCfgEx* policy);

	tBOOL       IsRemoteObj(hOBJECT client, tPROCESS& process);

	tERROR      OpenReport(const tCHAR* p_name, hOBJECT p_client, tDWORD p_access, cReport** p_report);

	bool        LoadData(cSerializable* data, cStrObj& key, const tCHAR* subkey = NULL, bool bPolicy = true);
	bool        SaveData(cSerializable* str, cStrObj& key, const tVOID* data, bool bPolicy = true);

	tERROR      CreateRemoteTask(cTmProfile* profile, cObj** task);
	tERROR      DestroyRemoteTask(cObj* task);
	tERROR      DestroyHost(cTmHost* host, tPROCESS pid);

	tDWORD      GetUniqueId();
	void        IdleWork();

	tERROR      on_command(tDWORD command, void* ctx);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION_NO_DELETE(TMImpl)
};
// AVP Prague stamp end

//  mDECLARE_INITIALIZATION_NO_DELETE(TMImpl)

inline cTmProfilePtr::cTmProfilePtr(TMImpl* tm, const tCHAR* name, cObj* client) : m_obj(tm->FindProfile(name, 0, client)){}
inline cTmProfilePtr::cTmProfilePtr(TMImpl* tm, tDWORD task_id) : m_obj(tm->FindProfile(NULL, task_id, NULL)){}
inline cTmProfilePtr::cTmProfilePtr(TMImpl* tm, hOBJECT object) : m_obj(tm->FindProfile(NULL, 0, object)){}

// AVP Prague stamp begin( Header endif,  )
#endif // tm_cpp
// AVP Prague stamp end


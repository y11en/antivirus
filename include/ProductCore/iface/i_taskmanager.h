// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  25 August 2006,  11:24 --------
// File Name   -- (null)i_taskmanager.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_taskmanager__39f0ae03_a580_49e2_adea_3eb2a1dbcabc )
#define __i_taskmanager__39f0ae03_a580_49e2_adea_3eb2a1dbcabc
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <ProductCore/structs/s_profiles.h>
// AVP Prague stamp end



#define cpTASK_MANAGER_TASK_ID        ((tSTRING)("cpTASK_MANAGER_TASK_ID"))        // task identifier. set to hTASK object
#define cpTASK_MANAGER_TASK_TYPE_NAME ((tSTRING)("cpTASK_MANAGER_TASK_TYPE_NAME")) // task type (string constant)
#define cpTASK_MANAGER_PROFILE_NAME   ((tSTRING)("cpTASK_MANAGER_PROFILE_NAME"))   // task profile name
#define cpTASK_MANAGER_TASK_IS_REMOTE ((tSTRING)("cpTASK_MANAGER_TASK_IS_REMOTE"))   // task profile name


// properties which are need to be implemented to be a service
#define pgSERVICE_SETTINGS     mPROPERTY_MAKE_GLOBAL( pTYPE_SERIALIZABLE, 0x00000400 )
#define pgSERVICE_SSTATISTICS  mPROPERTY_MAKE_GLOBAL( pTYPE_SERIALIZABLE, 0x00000401 )



// AVP Prague stamp begin( Interface comment,  )
// TaskManager interface implementation
// Short comments -- product task supervisor
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TASKMANAGER  ((tIID)(39))
// AVP Prague stamp end



#include <Prague/iface/i_string.h>
#include <Prague/iface/i_reg.h>

typedef tDWORD  tTaskId; // unique running task identifier

#define TASKID_TM_ITSELF     0
#define TASKID_UNK           ((tUINT)-1)


typedef enum enGetProfileInfoSource {
  gpi_FROM_RUNNING_TASK  = 1,
  gpi_FROM_REPORT        = 2,
  gpi_FROM_BOTH          = 3,
  gpi_FROM_CURRENT_STATE = 4,
} tGetProfileInfoSource;


typedef const tCHAR* tServiceId; //  --

#define sid_TM_AVS                          AVP_SERVICE_AVS             // = 1,
#define sid_TM_LIC                          AVP_SERVICE_LICENCE         // = 2,
#define sid_TM_QUARANTINE                   AVP_SERVICE_QUARANTINE      // = 3,
#define sid_TM_BACKUP                       AVP_SERVICE_BACKUP          // = 4,
#define sid_TM_SCHEDULER                    AVP_SERVICE_SCHEDULER       // = 6,

#define cQB_OBJECT_ID_QUARANTINE            0x051db913 //is a crc32 for "cQB_OBJECT_ID_QUARANTINE"
#define cQB_OBJECT_ID_BACKUP                0x77edb09e //is a crc32 for "cQB_OBJECT_ID_BACKUP"

#define pmc_QBSTORAGE_REPLICATED            0x7b783ebf //is a crc32 for "pmc_QBSTORAGE_REPLICATED"

struct cQbMessageReplicated
{
	tQWORD nObjectID;
	tDWORD nListID;
};

/*
// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tTaskId; // unique running task identifier
typedef tDWORD                         tServiceId; //  --
typedef tDWORD                         tTaskRequestState; // rquest for changing task state
typedef tDWORD                         tTaskState; // current task state
typedef tDWORD                         cProfile; //  --
typedef tDWORD                         cProfileEx; //  --
typedef tDWORD                         cProfileList; //  --
typedef tDWORD                         tGetProfileInfoSource; //  --
// AVP Prague stamp end



*/
#if defined ( __cplusplus )
  struct cReport;
  typedef cReport* hREPORT;
  struct cProfile;
  struct cProfileList;
  struct cTaskParams;
#else
  typedef struct tag_hREPORT hREPORT;
  typedef struct cProfile cProfile;
  typedef struct cProfileList cProfileList;
  typedef struct cTaskParams cTaskParams;
#endif

// ---
typedef enum enTaskRequestState {
  TASK_REQUEST_UNK   = 0x00,
  TASK_REQUEST_RUN   = 0x01,
  TASK_REQUEST_PAUSE = 0x02,
  TASK_REQUEST_STOP  = 0x04,
} tTaskRequestState;

#define IS_REQUEST_VALID(req) ( (((tTaskRequestState)(req))==TASK_REQUEST_RUN) || (((tTaskRequestState)(req))==TASK_REQUEST_PAUSE) || (((tTaskRequestState)(req))==TASK_REQUEST_STOP) )


#define STATE_FLAG_ACTIVE      0x100000
#define STATE_FLAG_OPERATIONAL 0x200000
#define STATE_FLAG_MALFUNCTION 0x400000

// ---
typedef enum enTaskState {
	TASK_STATE_UNK                 = TASK_REQUEST_UNK,
	TASK_STATE_UNKNOWN             = TASK_REQUEST_UNK,
	TASK_STATE_CREATED             = 0x10,
	TASK_STATE_STARTING            = 0x20 | TASK_REQUEST_RUN   | STATE_FLAG_ACTIVE | STATE_FLAG_OPERATIONAL,
	TASK_STATE_RUNNING             = 0x30 | TASK_REQUEST_RUN   | STATE_FLAG_ACTIVE | STATE_FLAG_OPERATIONAL,
	TASK_STATE_RUNNING_MALFUNCTION = 0x40 | TASK_REQUEST_RUN   | STATE_FLAG_ACTIVE | STATE_FLAG_OPERATIONAL | STATE_FLAG_MALFUNCTION,
	TASK_STATE_PAUSING             = 0x50 | TASK_REQUEST_PAUSE | STATE_FLAG_ACTIVE | STATE_FLAG_OPERATIONAL,
	TASK_STATE_PAUSED              = 0x60 | TASK_REQUEST_PAUSE | STATE_FLAG_ACTIVE,
	TASK_STATE_RESUMING            = 0x70 | TASK_REQUEST_RUN   | STATE_FLAG_ACTIVE | STATE_FLAG_OPERATIONAL,
	TASK_STATE_STOPPING            = 0x80 | TASK_REQUEST_STOP  | STATE_FLAG_ACTIVE | STATE_FLAG_OPERATIONAL,
	TASK_STATE_STOPPED             = 0x90 | TASK_REQUEST_STOP,
	TASK_STATE_COMPLETED           = 0xA0,
	TASK_STATE_FAILED              = 0xF0 | STATE_FLAG_MALFUNCTION,
} tTaskState;

#define IS_IN_STATE( st, check )   ((check) == ((tTaskState)(st)))

#define IS_TASK_STATE_VALID(st)    ( IS_REQUEST_VALID(st)                           || \
                                     IS_IN_STATE(st,TASK_STATE_RUNNING)             || \
                                     IS_IN_STATE(st,TASK_STATE_PAUSED)              || \
                                     IS_IN_STATE(st,TASK_STATE_RUNNING_MALFUNCTION) || \
                                     IS_IN_STATE(st,TASK_STATE_STOPPED)             || \
                                     IS_IN_STATE(st,TASK_STATE_COMPLETED)           || \
                                     IS_IN_STATE(st,TASK_STATE_FAILED)              || \
                                     IS_IN_STATE(st,TASK_STATE_CREATED)             || \
																		 IS_IN_STATE(st,TASK_STATE_CREATED_MALFUNCTION) || \
                                     IS_IN_STATE(st,TASK_STATE_STARTING)            || \
                                     IS_IN_STATE(st,TASK_STATE_PAUSING)             || \
                                     IS_IN_STATE(st,TASK_STATE_RESUMING)            || \
                                     IS_IN_STATE(st,TASK_STATE_STOPPING) )

#define IS_TASK_FINISHED(st)       ( IS_IN_STATE(st,TASK_STATE_STOPPED) ||   \
                                     IS_IN_STATE(st,TASK_STATE_COMPLETED) || \
                                     IS_IN_STATE(st,TASK_STATE_FAILED) )

#define IS_TASK_COMPLETED(st)      ( IS_IN_STATE(st,TASK_STATE_STOPPED) ||   \
                                     IS_IN_STATE(st,TASK_STATE_COMPLETED) )

#define IS_TASK_JUST_CREATED(st)   ( IS_IN_STATE(st,TASK_STATE_CREATED) )
#define IS_TASK_RUNNING(st)        ( ((tDWORD)(st)) & STATE_FLAG_ACTIVE )
#define IS_TASK_NOT_RUNNING(st)    ( IS_IN_STATE(st,TASK_STATE_CREATED) || IS_TASK_FINISHED(st) )


#if defined( _PRAGUE_TRACE_ )
	#if defined (__cplusplus)
		inline 
	#else
		static
	#endif
		const tCHAR* _task_state_name( tUINT state ) {
			switch( state ) {
				case TASK_STATE_UNK                 : return "TASK_STATE_UNK";
				case TASK_REQUEST_RUN               : return "TASK_REQUEST_RUN";
				case TASK_REQUEST_PAUSE             : return "TASK_REQUEST_PAUSE";
				//case TASK_REQUEST_STOP              : return "TASK_REQUEST_STOP"; // the same as TASK_STATE_STOPPED

				case TASK_STATE_RUNNING             : return "TASK_STATE_RUNNING";
				case TASK_STATE_PAUSED              : return "TASK_STATE_PAUSED";
				case TASK_STATE_RUNNING_MALFUNCTION : return "TASK_STATE_RUNNING_MALFUNCTION";
				
				case TASK_STATE_STOPPED             : return "TASK_STATE_STOPPED";
				case TASK_STATE_COMPLETED           : return "TASK_STATE_COMPLETED";
				case TASK_STATE_FAILED              : return "TASK_STATE_FAILED";
				case TASK_STATE_CREATED             : return "TASK_STATE_CREATED";
				case TASK_STATE_STARTING            : return "TASK_STATE_STARTING";
				case TASK_STATE_PAUSING             : return "TASK_STATE_PAUSING";
				case TASK_STATE_RESUMING            : return "TASK_STATE_RESUMING";
				case TASK_STATE_STOPPING            : return "TASK_STATE_STOPPING";
				default                             : return "unknown state";
			}
		}
#else
	#define _task_state_name(st) ""
#endif


// AVP Prague stamp begin( Interface constants,  )

// set of constants for parameter open_flags of OpenTask method
#define OPEN_TASK_MODE_CREATE          ((tDWORD)(0x80000000L)) //  --
#define OPEN_TASK_MODE_OPEN            ((tDWORD)(0x40000000L)) //  --
#define OPEN_TASK_MODE_OPEN_ALWAYS     ((tDWORD)(0xc0000000L)) //  --
#define OPEN_TASK_MODE_MASK            ((tDWORD)(0xc0000000L)) //  --
#define OPEN_TASK_MODE_CLOSE_TASK_IF_OWNER_DIED ((tDWORD)(0x20000000L)) //  --

// profile constants
#define fPROFILE_TEMPORARY             ((tDWORD)(0x00000001L)) //  --

// CleanItems constants
#define cCLEAN_REPORTS                 ((tDWORD)(1)) //  --
#define cCLEAN_BACKUP_ITEMS            ((tDWORD)(2)) //  --
#define cCLEAN_QUARANTINE_ITEMS        ((tDWORD)(4)) //  --
#define cCLEAN_QB_ITEMS                ((tDWORD)(cCLEAN_BACKUP_ITEMS|cCLEAN_QUARANTINE_ITEMS)) //  --

// Set.... delay constans
#define cREQUEST_SYNCHRONOUS           ((tDWORD)(0xffffffff)) //  --
#define cREQUEST_DELAY_UP_TO_INTERNET_CONNECTION ((tDWORD)(0x80000000)) //  --
#define cREQUEST_DELAY_MAKE_PERSISTENT ((tDWORD)(0x40000000)) //  --
#define cREQUEST_RESTART_PREVIOUS_TASK ((tDWORD)(0x20000000)) //  --
#define cREQUEST_DONOT_FORCE_RUN       ((tDWORD)(0x10000000)) //  --
#define cREQUEST_SYNCSTORE_DATA        ((tDWORD)(0x01000000)) //  --

// -- init flags
#define cTM_INIT_FLAG_DEFAULT          ((tDWORD)(1)) //  --
#define cTM_INIT_FLAG_PROFILES         ((tDWORD)(4)) //  --
#define cTM_INIT_FLAG_STARTGUI         ((tDWORD)(8)) //  --

// -working with reports
#define cREPORT_FILTER_NONE            ((tDWORD)(1)) //  --
#define cREPORT_FILTER_RECENT          ((tDWORD)(2)) //  --
#define cREPORT_RECORD_NEXT            ((tDWORD)(4)) //  --
#define cREPORT_RECORD_PREV            ((tDWORD)(8)) //  --
#define cREPORT_DELETE_FORCE           ((tDWORD)(16)) //  --
#define cREPORT_DELETE_PREV            ((tDWORD)(32)) //  --
#define cREPORT_DELETE_FILES_ONLY      ((tDWORD)(64)) //  --
// AVP Prague stamp end


// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmcPROFILE 0xc5f5a72e //

	#define pmPROFILE_STATE_CHANDED 0x00001000 // (4096) --

	#define pmPROFILE_CREATE 0x00001002 // (4098) --

	#define pmPROFILE_DELETE 0x00001003 // (4099) --

	#define pmPROFILE_MODIFIED 0x00001004 // (4100) --

	#define pmPROFILE_SETTINGS_MODIFIED 0x00001005 // (4101) --

	#define pmPROFILE_TASK_DISABLED_ON_RUN 0x00001006 // (4102) --

	#define pmPROFILE_LICENCE_CHECK_ERR 0x00001007 // (4103) --

	#define pmPROFILE_TASK_DISABLED_IN_SAFE_MODE 0x00001008 // (4104) -- task cannot be started in safe mode

	#define pmPROFILE_CANT_START 0x00001009 // (4105) -- 

// message class
//    parameters:
//    "ctx"         -- NULL
//    "msg_id"   -- correspondent action identifier
//    "pbuff"      -- pointer to cAskTaskAction (or descendant) structure
//    "pblen"     -- must be SER_SENDMSG_PSIZE constant
//
//    Field "m_nTaskId" of the cAskTaskAction structure must identify task.
//    Constant "TASKID_TM_ITSELF" identifies TaskManager
#define pmcASK_ACTION 0x57701ba6 //

// message class
#define pmcTM_STARTUP_SCHEDULE 0xf19d9841 //

// message class
#define pmcTM_EVENTS 0xa4094ca0 // TM events

	#define pmTM_EVENT_START 0x384e7d1c // (944667932) -- TM just started

	#define pmTM_EVENT_STARTUP_TASKS_GOING_TO_START 0xffff2a49 // (4294912585) -- TM is going to start startup tasks

	#define pmTM_EVENT_STARTUP_TASKS_STARTED 0xf927d6d6 // (4180137686) -- TM just started startup tasks

	#define pmTM_EVENT_OS_SAFE_MODE 0x208ad891 // (545970321) -- sent when product starts in safe mode

	#define pmTM_EVENT_GUI_CONNECTED 0x506e3ae5 // (1349401317) -- sent when some GUI is connected

	#define pmTM_EVENT_GUI_DISCONNECTED 0x387e4b05 // (947800837) --

	#define pmTM_EVENT_INET_CONNECTED 0xf647945a // (4131886170) --

	#define pmTM_EVENT_INET_DISCONNECTED 0x844afb85 // (2219506565) --

	#define pmTM_EVENT_TASK_SETTINGS_CHANGED 0x5a2b81f1 // (1512800753) --

	#define pmTM_EVENT_GOING_TO_STOP 0xcdf441e7 // (3455336935) --

	#define pmTM_EVENT_INSTALL 0xffff2a4a // (4294912586) --

	#define pmTM_EVENT_IDLE 0xffff2a4b // (4294912587) --

	#define pmTM_EVENT_CHECK_STOP_ALLOWED 0xffff2a4c // (4294912588) --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
#define errDO_IT_YOURSELF                        PR_MAKE_DECL_ERR(IID_TASKMANAGER, 0x001) // 0x80027001,-2147323903 (1) --
#define warnTASK_STATE_CHANGED                   PR_MAKE_DECL_WARN(IID_TASKMANAGER, 0x002) // 0x00027002,159746 (2) --
#define warnTASK_ALREADY_IN_STATE                PR_MAKE_DECL_WARN(IID_TASKMANAGER, 0x003) // 0x00027003,159747 (3) --
#define warnTASK_NEED_SAVE_SETTINGS              PR_MAKE_DECL_WARN(IID_TASKMANAGER, 0x004) // 0x00027004,159748 (4) -- settings must be saved
#define errTASK_CANNOT_GO_TO_STATE               PR_MAKE_DECL_ERR(IID_TASKMANAGER, 0x005) // 0x80027005,-2147323899 (5) --
#define errTASK_ABNORMAL_TERMINATION_ONCE        PR_MAKE_DECL_ERR(IID_TASKMANAGER, 0x006) // 0x80027006,-2147323898 (6) -- bf 19699: this state appears only once, on second start it transformed to errTASK_ABNORMAL_TERMINATION
#define errTASK_ABNORMAL_TERMINATION             PR_MAKE_DECL_ERR(IID_TASKMANAGER, 0x007) // 0x80027007,-2147323897 (7) --
// AVP Prague stamp end



#define warnDONT_CHANGE_TASKID                   PR_MAKE_DECL_ERR(IID_TASKMANAGER, 0x010)


// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTASKMANAGER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iTaskManagerVtbl;
typedef struct iTaskManagerVtbl iTaskManagerVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cTaskManager;
	typedef cTaskManager* hTASKMANAGER;
#else
	typedef struct tag_hTASKMANAGER 
	{
		const iTaskManagerVtbl* vtbl; // pointer to the "TaskManager" virtual table
		const iSYSTEMVtbl*      sys;  // pointer to the "SYSTEM" virtual table
	} *hTASKMANAGER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( TaskManager_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpTaskManager_Init)                        ( hTASKMANAGER _this, tDWORD flags ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_Exit)                        ( hTASKMANAGER _this, tDWORD cause ); // -- close TM session;

	// ----------- profile methods ----------
	typedef   tERROR (pr_call *fnpTaskManager_GetProfileInfo)              ( hTASKMANAGER _this, const tCHAR* profile_name, cSerializable* info ); // -- returns combined task profile, task settings or task schedule settings;
	typedef   tERROR (pr_call *fnpTaskManager_SetProfileInfo)              ( hTASKMANAGER _this, const tCHAR* profile_name, const cSerializable* info, hOBJECT client, tDWORD delay ); // -- replace combined task profile, task settings or task schedule settings for the profile;
	typedef   tERROR (pr_call *fnpTaskManager_EnableProfile)               ( hTASKMANAGER _this, const tCHAR* profile_name, tBOOL enabled, tBOOL recursive, hOBJECT client ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_IsProfileEnabled)            ( hTASKMANAGER _this, const tCHAR* profile_name, tBOOL* enabled ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_DeleteProfile)               ( hTASKMANAGER _this, const tCHAR* profile_name, hOBJECT client ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_SetProfileState)             ( hTASKMANAGER _this, const tCHAR* profile_name, tTaskRequestState state, hOBJECT client, tDWORD delay, tDWORD* task_id ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_GetProfileReport)            ( hTASKMANAGER _this, const tCHAR* profile_name, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_CloneProfile)                ( hTASKMANAGER _this, const tCHAR* profile_name, cProfile* profile, tDWORD flags, hOBJECT client ); // -- clones profile;
	typedef   tERROR (pr_call *fnpTaskManager_GetSettingsByLevel)          ( hTASKMANAGER _this, const tCHAR* profile_name, tDWORD settings_level, tBOOL cumulative, cSerializable* settings ); // -- get settings by task type and settings level;
	typedef   tERROR (pr_call *fnpTaskManager_GetProfilePersistentStorage) ( hTASKMANAGER _this, const tCHAR* profile_name, hOBJECT dad, hREGISTRY* storage, tBOOL rw ); // -- ;

	// ----------- task management ----------
	typedef   tERROR (pr_call *fnpTaskManager_OpenTask)                    ( hTASKMANAGER _this, tTaskId* task_id, const tCHAR* profile_name, tDWORD open_flags, hOBJECT client ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_ReleaseTask)                 ( hTASKMANAGER _this, tTaskId task_id ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_GetTaskState)                ( hTASKMANAGER _this, tTaskId task_id, tTaskState* task_state ); // -- returns current task state;
	typedef   tERROR (pr_call *fnpTaskManager_SetTaskState)                ( hTASKMANAGER _this, tTaskId task_id, tTaskRequestState requested_state, hOBJECT client, tDWORD delay ); // -- requests task for state changing;
	typedef   tERROR (pr_call *fnpTaskManager_GetTaskInfo)                 ( hTASKMANAGER _this, tTaskId task_id, cSerializable* info ); // -- returns asked info by task_id;
	typedef   tERROR (pr_call *fnpTaskManager_SetTaskInfo)                 ( hTASKMANAGER _this, tTaskId task_id, const cSerializable* info ); // -- returns asked info by task_id;
	typedef   tERROR (pr_call *fnpTaskManager_GetTaskReport)               ( hTASKMANAGER _this, tTaskId task_id, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_GetTaskPersistentStorage)    ( hTASKMANAGER _this, tTaskId task_id, hOBJECT dad, hREGISTRY* storage, tBOOL rw ); // -- ;

	// ----------- task extention methods ----------
	typedef   tERROR (pr_call *fnpTaskManager_AskAction)                   ( hTASKMANAGER _this, tTaskId task_id, tDWORD action_id, cSerializable* params ); // -- invokes extension method implemented by task;
	typedef   tERROR (pr_call *fnpTaskManager_AskTaskAction)               ( hTASKMANAGER _this, tTaskId task_id, tDWORD action_id, cSerializable* params ); // -- invokes extension method implemented by task;
	typedef   tERROR (pr_call *fnpTaskManager_GetService)                  ( hTASKMANAGER _this, tTaskId task_id, hOBJECT client, const tCHAR* service_id, hOBJECT* service, tDWORD start_delay ); // -- returns extension interface implemented by task;
	typedef   tERROR (pr_call *fnpTaskManager_ReleaseService)              ( hTASKMANAGER _this, tTaskId task_id, hOBJECT service ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_LockTaskSettings)            ( hTASKMANAGER _this, hOBJECT task, cSerializable** settings, tBOOL for_read_only ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_UnlockTaskSettings)          ( hTASKMANAGER _this, hOBJECT task, cSerializable* settings, tBOOL modified ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_RegisterRemoteTask)          ( hTASKMANAGER _this, hOBJECT tmhost, hOBJECT task, cSerializable* info, hOBJECT* host ); // -- ;

	// ----------- register client methods ----------
	typedef   tERROR (pr_call *fnpTaskManager_RegisterClient)              ( hTASKMANAGER _this, hOBJECT client ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_UnregisterClient)            ( hTASKMANAGER _this, hOBJECT client ); // -- ;

	// ----------- maintenance methods ----------
	typedef   tERROR (pr_call *fnpTaskManager_CleanupItems)                ( hTASKMANAGER _this, tDWORD item_type, const tDATETIME* time_stamp, hOBJECT client ); // -- clean old reports, quarantine and backup items by time point;
	typedef   tERROR (pr_call *fnpTaskManager_AddLicKey)                   ( hTASKMANAGER _this, hSTRING key_name ); // -- add license key;

	// ----------- -report methods ----------
	typedef   tERROR (pr_call *fnpTaskManager_AddRecordToTaskReport)       ( hTASKMANAGER _this, tTaskId task_id, tDWORD msg_cls, const cSerializable* record ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_GetTaskReportInfo)           ( hTASKMANAGER _this, tDWORD flags, tDWORD index, const cSerializable* info ); // -- ;
	typedef   tERROR (pr_call *fnpTaskManager_DeleteTaskReports)           ( hTASKMANAGER _this, tTaskId task_id, tDWORD flags ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iTaskManagerVtbl 
{
	fnpTaskManager_Init                         Init;
	fnpTaskManager_Exit                         Exit;

// ----------- profile methods ----------
	fnpTaskManager_GetProfileInfo               GetProfileInfo;
	fnpTaskManager_SetProfileInfo               SetProfileInfo;
	fnpTaskManager_EnableProfile                EnableProfile;
	fnpTaskManager_IsProfileEnabled             IsProfileEnabled;
	fnpTaskManager_DeleteProfile                DeleteProfile;
	fnpTaskManager_SetProfileState              SetProfileState;
	fnpTaskManager_GetProfileReport             GetProfileReport;
	fnpTaskManager_CloneProfile                 CloneProfile;
	fnpTaskManager_GetSettingsByLevel           GetSettingsByLevel;
	fnpTaskManager_GetProfilePersistentStorage  GetProfilePersistentStorage;

// ----------- task management ----------
	fnpTaskManager_OpenTask                     OpenTask;
	fnpTaskManager_ReleaseTask                  ReleaseTask;
	fnpTaskManager_GetTaskState                 GetTaskState;
	fnpTaskManager_SetTaskState                 SetTaskState;
	fnpTaskManager_GetTaskInfo                  GetTaskInfo;
	fnpTaskManager_SetTaskInfo                  SetTaskInfo;
	fnpTaskManager_GetTaskReport                GetTaskReport;
	fnpTaskManager_GetTaskPersistentStorage     GetTaskPersistentStorage;

// ----------- task extention methods ----------
	fnpTaskManager_AskAction                    AskAction;
	fnpTaskManager_AskTaskAction                AskTaskAction;
	fnpTaskManager_GetService                   GetService;
	fnpTaskManager_ReleaseService               ReleaseService;
	fnpTaskManager_LockTaskSettings             LockTaskSettings;
	fnpTaskManager_UnlockTaskSettings           UnlockTaskSettings;
	fnpTaskManager_RegisterRemoteTask           RegisterRemoteTask;

// ----------- register client methods ----------
	fnpTaskManager_RegisterClient               RegisterClient;
	fnpTaskManager_UnregisterClient             UnregisterClient;

// ----------- maintenance methods ----------
	fnpTaskManager_CleanupItems                 CleanupItems;
	fnpTaskManager_AddLicKey                    AddLicKey;

// ----------- -report methods ----------
	fnpTaskManager_AddRecordToTaskReport        AddRecordToTaskReport;
	fnpTaskManager_GetTaskReportInfo            GetTaskReportInfo;
	fnpTaskManager_DeleteTaskReports            DeleteTaskReports;
}; // "TaskManager" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( TaskManager_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION    mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTM_PRODUCT_CONFIG    mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001000 )
#define pgBL_PRODUCT_NAME      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001004 )
#define pgBL_PRODUCT_VERSION   mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001005 )
#define pgBL_PRODUCT_PATH      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001006 )
#define pgBL_PRODUCT_DATA_PATH mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001007 )
#define pgBL_COMPANY_NAME      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001008 )
#define pgTM_IS_GUI_CONNECTED  mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001009 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_TaskManager_Init( _this, flags )                                                                            ((_this)->vtbl->Init( (_this), flags ))
	#define CALL_TaskManager_Exit( _this, cause )                                                                            ((_this)->vtbl->Exit( (_this), cause ))
	#define CALL_TaskManager_GetProfileInfo( _this, profile_name, info )                                                     ((_this)->vtbl->GetProfileInfo( (_this), profile_name, info ))
	#define CALL_TaskManager_SetProfileInfo( _this, profile_name, info, client, delay )                                      ((_this)->vtbl->SetProfileInfo( (_this), profile_name, info, client, delay ))
	#define CALL_TaskManager_EnableProfile( _this, profile_name, enabled, recursive, client )                                ((_this)->vtbl->EnableProfile( (_this), profile_name, enabled, recursive, client ))
	#define CALL_TaskManager_IsProfileEnabled( _this, profile_name, enabled )                                                ((_this)->vtbl->IsProfileEnabled( (_this), profile_name, enabled ))
	#define CALL_TaskManager_DeleteProfile( _this, profile_name, client )                                                    ((_this)->vtbl->DeleteProfile( (_this), profile_name, client ))
	#define CALL_TaskManager_SetProfileState( _this, profile_name, state, client, delay, task_id )                           ((_this)->vtbl->SetProfileState( (_this), profile_name, state, client, delay, task_id ))
	#define CALL_TaskManager_GetProfileReport( _this, profile_name, report_id, client, access, report )                      ((_this)->vtbl->GetProfileReport( (_this), profile_name, report_id, client, access, report ))
	#define CALL_TaskManager_CloneProfile( _this, profile_name, profile, flags, client )                                     ((_this)->vtbl->CloneProfile( (_this), profile_name, profile, flags, client ))
	#define CALL_TaskManager_GetSettingsByLevel( _this, profile_name, settings_level, cumulative, settings )                 ((_this)->vtbl->GetSettingsByLevel( (_this), profile_name, settings_level, cumulative, settings ))
	#define CALL_TaskManager_GetProfilePersistentStorage( _this, profile_name, dad, storage, rw )                            ((_this)->vtbl->GetProfilePersistentStorage( (_this), profile_name, dad, storage, rw ))
	#define CALL_TaskManager_OpenTask( _this, task_id, profile_name, open_flags, client )                                    ((_this)->vtbl->OpenTask( (_this), task_id, profile_name, open_flags, client ))
	#define CALL_TaskManager_ReleaseTask( _this, task_id )                                                                   ((_this)->vtbl->ReleaseTask( (_this), task_id ))
	#define CALL_TaskManager_GetTaskState( _this, task_id, task_state )                                                      ((_this)->vtbl->GetTaskState( (_this), task_id, task_state ))
	#define CALL_TaskManager_SetTaskState( _this, task_id, requested_state, client, delay )                                  ((_this)->vtbl->SetTaskState( (_this), task_id, requested_state, client, delay ))
	#define CALL_TaskManager_GetTaskInfo( _this, task_id, info )                                                             ((_this)->vtbl->GetTaskInfo( (_this), task_id, info ))
	#define CALL_TaskManager_SetTaskInfo( _this, task_id, info )                                                             ((_this)->vtbl->SetTaskInfo( (_this), task_id, info ))
	#define CALL_TaskManager_GetTaskReport( _this, task_id, report_id, client, access, report )                              ((_this)->vtbl->GetTaskReport( (_this), task_id, report_id, client, access, report ))
	#define CALL_TaskManager_GetTaskPersistentStorage( _this, task_id, dad, storage, rw )                                    ((_this)->vtbl->GetTaskPersistentStorage( (_this), task_id, dad, storage, rw ))
	#define CALL_TaskManager_AskAction( _this, task_id, action_id, params )                                                  ((_this)->vtbl->AskAction( (_this), task_id, action_id, params ))
	#define CALL_TaskManager_AskTaskAction( _this, task_id, action_id, params )                                              ((_this)->vtbl->AskTaskAction( (_this), task_id, action_id, params ))
	#define CALL_TaskManager_GetService( _this, task_id, client, service_id, service, start_delay )                          ((_this)->vtbl->GetService( (_this), task_id, client, service_id, service, start_delay ))
	#define CALL_TaskManager_ReleaseService( _this, task_id, service )                                                       ((_this)->vtbl->ReleaseService( (_this), task_id, service ))
	#define CALL_TaskManager_LockTaskSettings( _this, task, settings, for_read_only )                                        ((_this)->vtbl->LockTaskSettings( (_this), task, settings, for_read_only ))
	#define CALL_TaskManager_UnlockTaskSettings( _this, task, settings, modified )                                           ((_this)->vtbl->UnlockTaskSettings( (_this), task, settings, modified ))
	#define CALL_TaskManager_RegisterRemoteTask( _this, tmhost, task, info, host )                                           ((_this)->vtbl->RegisterRemoteTask( (_this), tmhost, task, info, host ))
	#define CALL_TaskManager_RegisterClient( _this, client )                                                                 ((_this)->vtbl->RegisterClient( (_this), client ))
	#define CALL_TaskManager_UnregisterClient( _this, client )                                                               ((_this)->vtbl->UnregisterClient( (_this), client ))
	#define CALL_TaskManager_CleanupItems( _this, item_type, time_stamp, client )                                            ((_this)->vtbl->CleanupItems( (_this), item_type, time_stamp, client ))
	#define CALL_TaskManager_AddLicKey( _this, key_name )                                                                    ((_this)->vtbl->AddLicKey( (_this), key_name ))
	#define CALL_TaskManager_AddRecordToTaskReport( _this, task_id, msg_cls, record )                                        ((_this)->vtbl->AddRecordToTaskReport( (_this), task_id, msg_cls, record ))
	#define CALL_TaskManager_GetTaskReportInfo( _this, flags, index, info )                                                  ((_this)->vtbl->GetTaskReportInfo( (_this), flags, index, info ))
	#define CALL_TaskManager_DeleteTaskReports( _this, task_id, flags )                                                      ((_this)->vtbl->DeleteTaskReports( (_this), task_id, flags ))
#else // if !defined( __cplusplus )
	#define CALL_TaskManager_Init( _this, flags )                                                                            ((_this)->Init( flags ))
	#define CALL_TaskManager_Exit( _this, cause )                                                                            ((_this)->Exit( cause ))
	#define CALL_TaskManager_GetProfileInfo( _this, profile_name, info )                                                     ((_this)->GetProfileInfo( profile_name, info ))
	#define CALL_TaskManager_SetProfileInfo( _this, profile_name, info, client, delay )                                      ((_this)->SetProfileInfo( profile_name, info, client, delay ))
	#define CALL_TaskManager_EnableProfile( _this, profile_name, enabled, recursive, client )                                ((_this)->EnableProfile( profile_name, enabled, recursive, client ))
	#define CALL_TaskManager_IsProfileEnabled( _this, profile_name, enabled )                                                ((_this)->IsProfileEnabled( profile_name, enabled ))
	#define CALL_TaskManager_DeleteProfile( _this, profile_name, client )                                                    ((_this)->DeleteProfile( profile_name, client ))
	#define CALL_TaskManager_SetProfileState( _this, profile_name, state, client, delay, task_id )                           ((_this)->SetProfileState( profile_name, state, client, delay, task_id ))
	#define CALL_TaskManager_GetProfileReport( _this, profile_name, report_id, client, access, report )                      ((_this)->GetProfileReport( profile_name, report_id, client, access, report ))
	#define CALL_TaskManager_CloneProfile( _this, profile_name, profile, flags, client )                                     ((_this)->CloneProfile( profile_name, profile, flags, client ))
	#define CALL_TaskManager_GetSettingsByLevel( _this, profile_name, settings_level, cumulative, settings )                 ((_this)->GetSettingsByLevel( profile_name, settings_level, cumulative, settings ))
	#define CALL_TaskManager_GetProfilePersistentStorage( _this, profile_name, dad, storage, rw )                            ((_this)->GetProfilePersistentStorage( profile_name, dad, storage, rw ))
	#define CALL_TaskManager_OpenTask( _this, task_id, profile_name, open_flags, client )                                    ((_this)->OpenTask( task_id, profile_name, open_flags, client ))
	#define CALL_TaskManager_ReleaseTask( _this, task_id )                                                                   ((_this)->ReleaseTask( task_id ))
	#define CALL_TaskManager_GetTaskState( _this, task_id, task_state )                                                      ((_this)->GetTaskState( task_id, task_state ))
	#define CALL_TaskManager_SetTaskState( _this, task_id, requested_state, client, delay )                                  ((_this)->SetTaskState( task_id, requested_state, client, delay ))
	#define CALL_TaskManager_GetTaskInfo( _this, task_id, info )                                                             ((_this)->GetTaskInfo( task_id, info ))
	#define CALL_TaskManager_SetTaskInfo( _this, task_id, info )                                                             ((_this)->SetTaskInfo( task_id, info ))
	#define CALL_TaskManager_GetTaskReport( _this, task_id, report_id, client, access, report )                              ((_this)->GetTaskReport( task_id, report_id, client, access, report ))
	#define CALL_TaskManager_GetTaskPersistentStorage( _this, task_id, dad, storage, rw )                                    ((_this)->GetTaskPersistentStorage( task_id, dad, storage, rw ))
	#define CALL_TaskManager_AskAction( _this, task_id, action_id, params )                                                  ((_this)->AskAction( task_id, action_id, params ))
	#define CALL_TaskManager_AskTaskAction( _this, task_id, action_id, params )                                              ((_this)->AskTaskAction( task_id, action_id, params ))
	#define CALL_TaskManager_GetService( _this, task_id, client, service_id, service, start_delay )                          ((_this)->GetService( task_id, client, service_id, service, start_delay ))
	#define CALL_TaskManager_ReleaseService( _this, task_id, service )                                                       ((_this)->ReleaseService( task_id, service ))
	#define CALL_TaskManager_LockTaskSettings( _this, task, settings, for_read_only )                                        ((_this)->LockTaskSettings( task, settings, for_read_only ))
	#define CALL_TaskManager_UnlockTaskSettings( _this, task, settings, modified )                                           ((_this)->UnlockTaskSettings( task, settings, modified ))
	#define CALL_TaskManager_RegisterRemoteTask( _this, tmhost, task, info, host )                                           ((_this)->RegisterRemoteTask( tmhost, task, info, host ))
	#define CALL_TaskManager_RegisterClient( _this, client )                                                                 ((_this)->RegisterClient( client ))
	#define CALL_TaskManager_UnregisterClient( _this, client )                                                               ((_this)->UnregisterClient( client ))
	#define CALL_TaskManager_CleanupItems( _this, item_type, time_stamp, client )                                            ((_this)->CleanupItems( item_type, time_stamp, client ))
	#define CALL_TaskManager_AddLicKey( _this, key_name )                                                                    ((_this)->AddLicKey( key_name ))
	#define CALL_TaskManager_AddRecordToTaskReport( _this, task_id, msg_cls, record )                                        ((_this)->AddRecordToTaskReport( task_id, msg_cls, record ))
	#define CALL_TaskManager_GetTaskReportInfo( _this, flags, index, info )                                                  ((_this)->GetTaskReportInfo( flags, index, info ))
	#define CALL_TaskManager_DeleteTaskReports( _this, task_id, flags )                                                      ((_this)->DeleteTaskReports( task_id, flags ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iTaskManager 
	{
		virtual tERROR pr_call Init( tDWORD flags ) = 0;
		virtual tERROR pr_call Exit( tDWORD cause ) = 0; // -- close TM session
		virtual tERROR pr_call GetProfileInfo( const tCHAR* profile_name, cSerializable* info ) = 0; // -- returns combined task profile, task settings or task schedule settings
		virtual tERROR pr_call SetProfileInfo( const tCHAR* profile_name, const cSerializable* info, hOBJECT client, tDWORD delay ) = 0; // -- replace combined task profile, task settings or task schedule settings for the profile
		virtual tERROR pr_call EnableProfile( const tCHAR* profile_name, tBOOL enabled, tBOOL recursive, hOBJECT client ) = 0;
		virtual tERROR pr_call IsProfileEnabled( const tCHAR* profile_name, tBOOL* enabled ) = 0;
		virtual tERROR pr_call DeleteProfile( const tCHAR* profile_name, hOBJECT client ) = 0;
		virtual tERROR pr_call SetProfileState( const tCHAR* profile_name, tTaskRequestState state, hOBJECT client, tDWORD delay, tDWORD* task_id = NULL) = 0;
		virtual tERROR pr_call GetProfileReport( const tCHAR* profile_name, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ) = 0;
		virtual tERROR pr_call CloneProfile( const tCHAR* profile_name, cProfile* profile, tDWORD flags, hOBJECT client ) = 0; // -- clones profile
		virtual tERROR pr_call GetSettingsByLevel( const tCHAR* profile_name, tDWORD settings_level, tBOOL cumulative, cSerializable* settings ) = 0; // -- get settings by task type and settings level
		virtual tERROR pr_call GetProfilePersistentStorage( const tCHAR* profile_name, hOBJECT dad, hREGISTRY* storage, tBOOL rw ) = 0;
		virtual tERROR pr_call OpenTask( tTaskId* task_id, const tCHAR* profile_name, tDWORD open_flags, hOBJECT client ) = 0;
		virtual tERROR pr_call ReleaseTask( tTaskId task_id ) = 0;
		virtual tERROR pr_call GetTaskState( tTaskId task_id, tTaskState* task_state ) = 0; // -- returns current task state
		virtual tERROR pr_call SetTaskState( tTaskId task_id, tTaskRequestState requested_state, hOBJECT client, tDWORD delay ) = 0; // -- requests task for state changing
		virtual tERROR pr_call GetTaskInfo( tTaskId task_id, cSerializable* info ) = 0; // -- returns asked info by task_id
		virtual tERROR pr_call SetTaskInfo( tTaskId task_id, const cSerializable* info ) = 0; // -- returns asked info by task_id
		virtual tERROR pr_call GetTaskReport( tTaskId task_id, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ) = 0;
		virtual tERROR pr_call GetTaskPersistentStorage( tTaskId task_id, hOBJECT dad, hREGISTRY* storage, tBOOL rw ) = 0;
		virtual tERROR pr_call AskAction( tTaskId task_id, tDWORD action_id, cSerializable* params ) = 0; // -- invokes extension method implemented by task
		virtual tERROR pr_call AskTaskAction( tTaskId task_id, tDWORD action_id, cSerializable* params ) = 0; // -- invokes extension method implemented by task
		virtual tERROR pr_call GetService( tTaskId task_id, hOBJECT client, const tCHAR* service_id, hOBJECT* service, tDWORD start_delay = 0) = 0; // -- returns extension interface implemented by task
		virtual tERROR pr_call ReleaseService( tTaskId task_id, hOBJECT service ) = 0;
		virtual tERROR pr_call LockTaskSettings( hOBJECT task, cSerializable** settings, tBOOL for_read_only ) = 0;
		virtual tERROR pr_call UnlockTaskSettings( hOBJECT task, cSerializable* settings, tBOOL modified ) = 0;
		virtual tERROR pr_call RegisterRemoteTask( hOBJECT tmhost, hOBJECT task, cSerializable* info, hOBJECT* host ) = 0;
		virtual tERROR pr_call RegisterClient( hOBJECT client ) = 0;
		virtual tERROR pr_call UnregisterClient( hOBJECT client ) = 0;
		virtual tERROR pr_call CleanupItems( tDWORD item_type, const tDATETIME* time_stamp, hOBJECT client ) = 0; // -- clean old reports, quarantine and backup items by time point
		virtual tERROR pr_call AddLicKey( hSTRING key_name ) = 0; // -- add license key
		virtual tERROR pr_call AddRecordToTaskReport( tTaskId task_id, tDWORD msg_cls, const cSerializable* record ) = 0;
		virtual tERROR pr_call GetTaskReportInfo( tDWORD flags, tDWORD index, const cSerializable* info ) = 0;
		virtual tERROR pr_call DeleteTaskReports( tTaskId task_id, tDWORD flags ) = 0;
	};

	struct pr_abstract cTaskManager : public iTaskManager, public iObj 
	{

		OBJ_IMPL( cTaskManager );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTASKMANAGER()   { return (hTASKMANAGER)this; }

		hOBJECT pr_call get_pgTM_PRODUCT_CONFIG() { return (hOBJECT)getObj(pgTM_PRODUCT_CONFIG); }
		tERROR pr_call set_pgTM_PRODUCT_CONFIG( hOBJECT value ) { return setObj(pgTM_PRODUCT_CONFIG,(hOBJECT)value); }

		tERROR pr_call get_pgBL_PRODUCT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_NAME,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_NAME,buff,size,cp); }

		tERROR pr_call get_pgBL_PRODUCT_VERSION( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_VERSION,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_VERSION( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_VERSION,buff,size,cp); }

		tERROR pr_call get_pgBL_PRODUCT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_PATH,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_PATH,buff,size,cp); }

		tERROR pr_call get_pgBL_PRODUCT_DATA_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_DATA_PATH,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_DATA_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_DATA_PATH,buff,size,cp); }

		tERROR pr_call get_pgBL_COMPANY_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_COMPANY_NAME,buff,size,cp); }
		tERROR pr_call set_pgBL_COMPANY_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_COMPANY_NAME,buff,size,cp); }

		tBOOL pr_call get_pgTM_IS_GUI_CONNECTED() { return (tBOOL)getBool(pgTM_IS_GUI_CONNECTED); }
		tERROR pr_call set_pgTM_IS_GUI_CONNECTED( tBOOL value ) { return setBool(pgTM_IS_GUI_CONNECTED,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_taskmanager__39f0ae03_a580_49e2_adea_3eb2a1dbcabc
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(TaskManager_PROXY_DEFINITION)
#define TaskManager_PROXY_DEFINITION

PR_PROXY_BEGIN(TaskManager)
	PR_PROXY(TaskManager, Init,               PR_ARG1(tid_DWORD))
	PR_PROXY(TaskManager, Exit,               PR_ARG1(tid_DWORD))
	PR_PROXY(TaskManager, GetProfileInfo,     PR_ARG2(tid_STRING,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, SetProfileInfo,     PR_ARG4(tid_STRING,tid_SERIALIZABLE,tid_OBJECT,tid_DWORD))
	PR_PROXY(TaskManager, EnableProfile,      PR_ARG4(tid_STRING,tid_BOOL,tid_BOOL,tid_OBJECT))
	PR_PROXY(TaskManager, IsProfileEnabled,   PR_ARG2(tid_STRING,tid_BOOL|tid_POINTER))
	PR_PROXY(TaskManager, DeleteProfile,      PR_ARG2(tid_STRING,tid_OBJECT))
	PR_PROXY(TaskManager, GetSettingsByLevel, PR_ARG4(tid_STRING,tid_DWORD,tid_BOOL,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, OpenTask,           PR_ARG4(tid_DWORD|tid_POINTER,tid_STRING,tid_DWORD,tid_OBJECT))
	PR_PROXY(TaskManager, ReleaseTask,        PR_ARG1(tid_DWORD))
	PR_PROXY(TaskManager, GetTaskState,       PR_ARG2(tid_DWORD,tid_DWORD|tid_POINTER))
	PR_SCPRX(TaskManager, SetTaskState,       PR_ARG4(tid_DWORD,tid_DWORD,tid_OBJECT,tid_DWORD))
	PR_PROXY(TaskManager, GetTaskInfo,        PR_ARG2(tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, SetTaskInfo,        PR_ARG2(tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, DeleteTaskReports,  PR_ARG2(tid_DWORD,tid_BOOL))
	PR_PROXY(TaskManager, GetTaskReportInfo,  PR_ARG3(tid_DWORD,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, AskAction,          PR_ARG3(tid_DWORD,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, AskTaskAction,      PR_ARG3(tid_DWORD,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, GetService,         PR_ARG5(tid_DWORD,tid_OBJECT,tid_STRING,tid_OBJECT|tid_POINTER|prf_REF,tid_DWORD))
	PR_PROXY(TaskManager, ReleaseService,     PR_ARG2(tid_DWORD,tid_OBJECT|prf_DEREF))
	PR_PROXY(TaskManager, RegisterRemoteTask, PR_ARG4(tid_OBJECT,tid_OBJECT,tid_SERIALIZABLE,tid_OBJECT|tid_POINTER))
	PR_SCPRX(TaskManager, SetProfileState,    PR_ARG5(tid_STRING,tid_DWORD,tid_OBJECT,tid_DWORD,tid_DWORD|tid_POINTER))
	PR_PROXY(TaskManager, GetProfileReport,   PR_ARG5(tid_STRING,tid_STRING,tid_OBJECT,tid_DWORD,tid_OBJECT|tid_POINTER))
	PR_PROXY(TaskManager, CloneProfile,       PR_ARG4(tid_STRING,tid_SERIALIZABLE,tid_DWORD,tid_OBJECT))
	PR_PROXY(TaskManager, GetTaskReport,      PR_ARG5(tid_DWORD,tid_STRING,tid_OBJECT,tid_DWORD,tid_OBJECT|tid_POINTER))
	PR_PROXY(TaskManager, RegisterClient,     PR_ARG1(tid_OBJECT))
	PR_PROXY(TaskManager, UnregisterClient,   PR_ARG1(tid_OBJECT))
	PR_PROXY(TaskManager, CleanupItems,       PR_ARG3(tid_DWORD,tid_DATETIME|tid_POINTER,tid_OBJECT))
	PR_PROXY(TaskManager, AddLicKey,          PR_ARG1(tid_OBJECT))
	PR_PROXY(TaskManager, AddRecordToTaskReport,       PR_ARG3(tid_DWORD,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TaskManager, GetProfilePersistentStorage, PR_ARG4(tid_STRING,tid_OBJECT,tid_OBJECT|tid_POINTER,tid_BOOL))
	PR_PROXY(TaskManager, GetTaskPersistentStorage,    PR_ARG4(tid_DWORD,tid_OBJECT,tid_OBJECT|tid_POINTER,tid_BOOL))
PR_PROXY_END(TaskManager, IID_TASKMANAGER)

#endif // __PROXY_DECLARATION

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  04 March 2005,  14:18 --------
// File Name   -- (null)i_task.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_task__f5f8e0ed_fead_43e7_8210_c17a946de519 )
#define __i_task__f5f8e0ed_fead_43e7_8210_c17a946de519
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_reg.h>
#include <ProductCore/iface/i_taskmanager.h>
// AVP Prague stamp end



typedef enum enTaskType
{
	TASK_UNK     = 0, 
	TASK_OAS     = 1,
	TASK_ODS     = 2,  
	TASK_MC	     = 3,
	TASK_SC      = 5,   
	TASK_OG      = 6,   
	TASK_AVS     = 7,
	TASK_BL      = 8, 
	TASK_GUI     = 9, 
	TASK_QB      = 10,
	TASK_UPDATER = 11,
	TASK_QSCAN   = 12,
	TASK_MC_TM   = 13,
	TASK_MC_OP   = 14,
	TASK_PDM     = 15,
	TASK_IDS     = 16,
	TASK_AH      = 17,
	TASK_FW      = 18,
	TASK_STM     = 19,
	TASK_LAST    = 20,
} tTaskType;

typedef enum enSettingsId 
{
	SETTINGS_ID_UNKNOWN          = -1,
	SETTINGS_ID_DEFAULT          = 0,
	SETTINGS_ID_0                = SETTINGS_ID_DEFAULT,
	SETTINGS_ID_LAST             = 0x10000,
} tSettingsId;

#define TASK_ID_SINGLETON     (-1)

typedef tDWORD tActionId;
typedef tDWORD tSerializableId;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Task interface implementation
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TASK  ((tIID)(23))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
#define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
#define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
#define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //
// AVP Prague stamp end

#define pmc_EVENTS_NOTIFY                   0xe532519d
#define pmc_EVENTS_IMPORTANT                0x10f87d4c
#define pmc_EVENTS_CRITICAL                 0xbfca8487


// message class


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
#define errTASK_CANNOT_UPDATE_SETTINGS_ON_RUNTIME PR_MAKE_DECL_ERR(IID_TASK, 0x001) // 0x80017001,-2147389439 (1) --
#define errTASK_STATE_UNKNOWN                     PR_MAKE_DECL_ERR(IID_TASK, 0x002) // 0x80017002,-2147389438 (2) --
#define errTASK_CANNOT_UPDATE_SETTINGS_ON_STOPPED PR_MAKE_DECL_ERR(IID_TASK, 0x003) // 0x80017003,-2147389437 (3) --
#define errTASK_DISABLED                          PR_MAKE_DECL_ERR(IID_TASK, 0x005) // 0x80017005,-2147389435 (5) --
#define errTASK_LOGON_FAILED                      PR_MAKE_DECL_ERR(IID_TASK, 0x006) // 0x80017006,-2147389434 (6) --
#define errTASK_CANNOT_START_IN_SAFE_NODE         PR_MAKE_DECL_ERR(IID_TASK, 0x007) // 0x80017007,-2147389433 (7) --
#define errTASK_CANNOT_LOGON_IN_SAFE_MODE         PR_MAKE_DECL_ERR(IID_TASK, 0x008) // 0x80017008,-2147389432 (8) --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTASK;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iTaskVtbl;
typedef struct iTaskVtbl iTaskVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cTask;
	typedef cTask* hTASK;
#else
	typedef struct tag_hTASK {
		const iTaskVtbl*   vtbl; // pointer to the "Task" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hTASK;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Task_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpTask_SetSettings)   ( hTASK _this, const cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpTask_GetSettings)   ( hTASK _this, cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpTask_AskAction)     ( hTASK _this, tActionId actionId, cSerializable* actionInfo ); // -- ;
	typedef   tERROR (pr_call *fnpTask_SetState)      ( hTASK _this, tTaskRequestState state ); // -- ;
	typedef   tERROR (pr_call *fnpTask_GetStatistics) ( hTASK _this, cSerializable* statistics ); // -- ;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iTaskVtbl {
	fnpTask_SetSettings    SetSettings;
	fnpTask_GetSettings    GetSettings;
	fnpTask_AskAction      AskAction;
	fnpTask_SetState       SetState;
	fnpTask_GetStatistics  GetStatistics;
}; // "Task" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Task_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTASK_TM                 mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001002 )
#define pgTASK_STATE              mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgTASK_SESSION_ID         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
#define pgTASK_ID                 mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgTASK_PERSISTENT_STORAGE mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001006 )
#define pgTASK_PARENT_ID          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001007 )
// AVP Prague stamp end



#define pgTASK_BL pgTASK_TM


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Task_SetSettings( _this, settings )                    ((_this)->vtbl->SetSettings( (_this), settings ))
	#define CALL_Task_GetSettings( _this, settings )                    ((_this)->vtbl->GetSettings( (_this), settings ))
	#define CALL_Task_AskAction( _this, actionId, actionInfo )          ((_this)->vtbl->AskAction( (_this), actionId, actionInfo ))
	#define CALL_Task_SetState( _this, state )                          ((_this)->vtbl->SetState( (_this), state ))
	#define CALL_Task_GetStatistics( _this, statistics )                ((_this)->vtbl->GetStatistics( (_this), statistics ))
#else // if !defined( __cplusplus )
	#define CALL_Task_SetSettings( _this, settings )                    ((_this)->SetSettings( settings ))
	#define CALL_Task_GetSettings( _this, settings )                    ((_this)->GetSettings( settings ))
	#define CALL_Task_AskAction( _this, actionId, actionInfo )          ((_this)->AskAction( actionId, actionInfo ))
	#define CALL_Task_SetState( _this, state )                          ((_this)->SetState( state ))
	#define CALL_Task_GetStatistics( _this, statistics )                ((_this)->GetStatistics( statistics ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iTask {
		virtual tERROR pr_call SetSettings( const cSerializable* settings ) = 0;
		virtual tERROR pr_call GetSettings( cSerializable* settings ) = 0;
		virtual tERROR pr_call AskAction( tActionId actionId, cSerializable* actionInfo ) = 0;
		virtual tERROR pr_call SetState( tTaskRequestState state ) = 0;
		virtual tERROR pr_call GetStatistics( cSerializable* statistics ) = 0;
	};

	struct pr_abstract cTask : public iTask, public iObj {
		OBJ_IMPL( cTask );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTASK()   { return (hTASK)this; }

		hOBJECT pr_call get_pgTASK_TM() { return (hOBJECT)getObj(pgTASK_TM); }
		tERROR pr_call set_pgTASK_TM( hOBJECT value ) { return setObj(pgTASK_TM,(hOBJECT)value); }

		tDWORD pr_call get_pgTASK_STATE() { return (tDWORD)getDWord(pgTASK_STATE); }
		tERROR pr_call set_pgTASK_STATE( tDWORD value ) { return setDWord(pgTASK_STATE,(tDWORD)value); }

		tDWORD pr_call get_pgTASK_SESSION_ID() { return (tDWORD)getDWord(pgTASK_SESSION_ID); }
		tERROR pr_call set_pgTASK_SESSION_ID( tDWORD value ) { return setDWord(pgTASK_SESSION_ID,(tDWORD)value); }

		tDWORD pr_call get_pgTASK_ID() { return (tDWORD)getDWord(pgTASK_ID); }
		tERROR pr_call set_pgTASK_ID( tDWORD value ) { return setDWord(pgTASK_ID,(tDWORD)value); }

		hOBJECT pr_call get_pgTASK_PERSISTENT_STORAGE() { return (hOBJECT)getObj(pgTASK_PERSISTENT_STORAGE); }
		tERROR pr_call set_pgTASK_PERSISTENT_STORAGE( hOBJECT value ) { return setObj(pgTASK_PERSISTENT_STORAGE,(hOBJECT)value); }

		tDWORD pr_call get_pgTASK_PARENT_ID() { return (tDWORD)getDWord(pgTASK_PARENT_ID); }
		tERROR pr_call set_pgTASK_PARENT_ID( tDWORD value ) { return setDWord(pgTASK_PARENT_ID,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_task__f5f8e0ed_fead_43e7_8210_c17a946de519
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(Task_PROXY_DEFINITION)
#define Task_PROXY_DEFINITION

PR_PROXY_BEGIN(Task)
	PR_PROXY(Task, SetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(Task, GetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(Task, AskAction,        PR_ARG2(tid_DWORD,tid_SERIALIZABLE))
	PR_SCPRX(Task, SetState,         PR_ARG1(tid_DWORD))
	PR_PROXY(Task, GetStatistics,    PR_ARG1(tid_SERIALIZABLE))
PR_PROXY_END(Task, IID_TASK)

#endif // __PROXY_DECLARATION

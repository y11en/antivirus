// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  04 April 2007,  18:55 --------
// File Name   -- (null)i_avs.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_avs__d306fcf3_ec29_44a0_ab12_3b29b6970c55 )
#define __i_avs__d306fcf3_ec29_44a0_ab12_3b29b6970c55
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



#include "i_avssession.h"
#include "i_avstreats.h"


// AVP Prague stamp begin( Interface comment,  )
// AVS interface implementation
// AVP Prague stamp end



typedef struct {
  tSTRING FileType;
  tSTRING FileName;
} tUPDATE_LIST;

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AVS  ((tIID)(34))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_AVS 0xe7491cd3 //

	#define pm_THREATS_STATUS_CHANGED 0x00001003 // (4099) --

	#define pm_SETTINGS_CHANGED 0x00001000 // (4096) --

	#define pm_BASES_LOADED 0x00001002 // (4098) --

	#define pm_BASES_PRELOAD 0x00001005 // (4101) --

	#define pm_BASES_LOADFAILED 0x00001006 // (4102) --

// message class
#define pmc_MATCH_EXCLUDES 0x521d6e78 //

// message class
#define pmc_UPDATE_THREATS_LIST 0xeb3c8145 //

// message class
#define pmc_THREATS_ADD_TO_QUARANTINE 0xd6ef67a2 //

// message class
#define pmc_AVS_UPDATE 0x619a8dfc //

// message class
#define pm_AVS_VERIFY_PID_TO_KILL 0xf65cb62a //
// AVP Prague stamp end

#define pmc_AVS_OBJECT 0x51121368 //


// AVP Prague stamp begin( Interface defined errors,  )
#define errAVS_BASES_LOCK_FAILED                 PR_MAKE_DECL_ERR(IID_AVS, 0x001) // 0x80022001,-2147344383 (1) --
#define errAVS_BASES_SIGN_INVALID                PR_MAKE_DECL_ERR(IID_AVS, 0x002) // 0x80022002,-2147344382 (2) --
#define errAVS_BASES_SIZE_INVALID                PR_MAKE_DECL_ERR(IID_AVS, 0x003) // 0x80022003,-2147344381 (3) --
#define errAVS_BASES_CONFIG_NOT_FOUND            PR_MAKE_DECL_ERR(IID_AVS, 0x004) // 0x80022004,-2147344380 (4) --
#define errAVS_BASES_CONFIG_INVALID              PR_MAKE_DECL_ERR(IID_AVS, 0x005) // 0x80022005,-2147344379 (5) --
#define errAVS_INIT_ICHECKER2                    PR_MAKE_DECL_ERR(IID_AVS, 0x006) // 0x80022006,-2147344378 (6) --
#define errAVS_INIT_ICHECKERSTREAM               PR_MAKE_DECL_ERR(IID_AVS, 0x007) // 0x80022007,-2147344377 (7) --
#define errAVS_UPDATE_BASES_FAILED               PR_MAKE_DECL_ERR(IID_AVS, 0x008) // 0x80022008,-2147344376 (8) --
#define errAVS_UPDATE_PLUGINS_FAILED             PR_MAKE_DECL_ERR(IID_AVS, 0x009) // 0x80022009,-2147344375 (9) --
#define errAVS_INIT_DRIVER_FAILED                PR_MAKE_DECL_ERR(IID_AVS, 0x00a) // 0x8002200a,-2147344374 (10) --
#define warnAVS_LOAD_BASES                       PR_MAKE_DECL_WARN(IID_AVS, 0x00b) // 0x0002200b,139275 (11) --
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hAVS;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAVSVtbl;
typedef struct iAVSVtbl iAVSVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAVS;
	typedef cAVS* hAVS;
#else
	typedef struct tag_hAVS 
	{
		const iAVSVtbl*    vtbl; // pointer to the "AVS" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hAVS;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AVS_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAVS_SetSettings)   ( hAVS _this, const cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_GetSettings)   ( hAVS _this, cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_AskAction)     ( hAVS _this, tActionId actionId, cSerializable* actionInfo ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_SetState)      ( hAVS _this, tTaskState state );     // -- ;
	typedef   tERROR (pr_call *fnpAVS_GetStatistics) ( hAVS _this, cSerializable* statistics ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_Update)        ( hAVS _this, const cSerializable* update_info ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_CreateSession) ( hAVS _this, hAVSSESSION* result, hTASK task, tDWORD engine_flags, tDWORD scan_origin ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_GetTreats)     ( hAVS _this, tDWORD taskid, hAVSTREATS* treats ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_ReleaseTreats) ( hAVS _this, hAVSTREATS treats );    // -- ;
	typedef   tERROR (pr_call *fnpAVS_MatchExcludes) ( hAVS _this, const cSerializable* object_info ); // -- ;
	typedef   tERROR (pr_call *fnpAVS_ProcessCancel) ( hAVS _this, tDWORD process_id );    // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAVSVtbl 
{
	fnpAVS_SetSettings    SetSettings;
	fnpAVS_GetSettings    GetSettings;
	fnpAVS_AskAction      AskAction;
	fnpAVS_SetState       SetState;
	fnpAVS_GetStatistics  GetStatistics;
	fnpAVS_Update         Update;
	fnpAVS_CreateSession  CreateSession;
	fnpAVS_GetTreats      GetTreats;
	fnpAVS_ReleaseTreats  ReleaseTreats;
	fnpAVS_MatchExcludes  MatchExcludes;
	fnpAVS_ProcessCancel  ProcessCancel;
}; // "AVS" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AVS_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTASK_TYPE               mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgTASK_TM                 mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001002 )
#define pgTASK_STATE              mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgTASK_SESSION_ID         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
#define pgTASK_ID                 mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgTASK_PERSISTENT_STORAGE mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001006 )
#define pgTASK_PARENT_ID          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001007 )
// AVP Prague stamp end



#define pgTASK_BL pgTASK_TM


// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AVS_SetSettings( _this, settings )                                          ((_this)->vtbl->SetSettings( (_this), settings ))
	#define CALL_AVS_GetSettings( _this, settings )                                          ((_this)->vtbl->GetSettings( (_this), settings ))
	#define CALL_AVS_AskAction( _this, actionId, actionInfo )                                ((_this)->vtbl->AskAction( (_this), actionId, actionInfo ))
	#define CALL_AVS_SetState( _this, state )                                                ((_this)->vtbl->SetState( (_this), state ))
	#define CALL_AVS_GetStatistics( _this, statistics )                                      ((_this)->vtbl->GetStatistics( (_this), statistics ))
	#define CALL_AVS_Update( _this, update_info )                                            ((_this)->vtbl->Update( (_this), update_info ))
	#define CALL_AVS_CreateSession( _this, result, task, engine_flags, scan_origin )         ((_this)->vtbl->CreateSession( (_this), result, task, engine_flags, scan_origin ))
	#define CALL_AVS_GetTreats( _this, taskid, treats )                                      ((_this)->vtbl->GetTreats( (_this), taskid, treats ))
	#define CALL_AVS_ReleaseTreats( _this, treats )                                          ((_this)->vtbl->ReleaseTreats( (_this), treats ))
	#define CALL_AVS_MatchExcludes( _this, object_info )                                     ((_this)->vtbl->MatchExcludes( (_this), object_info ))
	#define CALL_AVS_ProcessCancel( _this, process_id )                                      ((_this)->vtbl->ProcessCancel( (_this), process_id ))
#else // if !defined( __cplusplus )
	#define CALL_AVS_SetSettings( _this, settings )                                          ((_this)->SetSettings( settings ))
	#define CALL_AVS_GetSettings( _this, settings )                                          ((_this)->GetSettings( settings ))
	#define CALL_AVS_AskAction( _this, actionId, actionInfo )                                ((_this)->AskAction( actionId, actionInfo ))
	#define CALL_AVS_SetState( _this, state )                                                ((_this)->SetState( state ))
	#define CALL_AVS_GetStatistics( _this, statistics )                                      ((_this)->GetStatistics( statistics ))
	#define CALL_AVS_Update( _this, update_info )                                            ((_this)->Update( update_info ))
	#define CALL_AVS_CreateSession( _this, result, task, engine_flags, scan_origin )         ((_this)->CreateSession( result, task, engine_flags, scan_origin ))
	#define CALL_AVS_GetTreats( _this, taskid, treats )                                      ((_this)->GetTreats( taskid, treats ))
	#define CALL_AVS_ReleaseTreats( _this, treats )                                          ((_this)->ReleaseTreats( treats ))
	#define CALL_AVS_MatchExcludes( _this, object_info )                                     ((_this)->MatchExcludes( object_info ))
	#define CALL_AVS_ProcessCancel( _this, process_id )                                      ((_this)->ProcessCancel( process_id ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAVS 
	{
		virtual tERROR pr_call SetSettings( const cSerializable* settings ) = 0;
		virtual tERROR pr_call GetSettings( cSerializable* settings ) = 0;
		virtual tERROR pr_call AskAction( tActionId actionId, cSerializable* actionInfo ) = 0;
		virtual tERROR pr_call SetState( tTaskState state ) = 0;
		virtual tERROR pr_call GetStatistics( cSerializable* statistics ) = 0;
		virtual tERROR pr_call Update( const cSerializable* update_info ) = 0;
		virtual tERROR pr_call CreateSession( hAVSSESSION* result, hTASK task, tDWORD engine_flags, tDWORD scan_origin ) = 0;
		virtual tERROR pr_call GetTreats( tDWORD taskid, hAVSTREATS* treats ) = 0;
		virtual tERROR pr_call ReleaseTreats( hAVSTREATS treats ) = 0;
		virtual tERROR pr_call MatchExcludes( const cSerializable* object_info ) = 0;
		virtual tERROR pr_call ProcessCancel( tDWORD process_id ) = 0;
	};

	struct pr_abstract cAVS : public iAVS, public iObj 
	{

		OBJ_IMPL( cAVS );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hAVS()   { return (hAVS)this; }

		tDWORD pr_call get_pgTASK_TYPE() { return (tDWORD)getDWord(pgTASK_TYPE); }
		tERROR pr_call set_pgTASK_TYPE( tDWORD value ) { return setDWord(pgTASK_TYPE,(tDWORD)value); }

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



// AVP Prague stamp begin( Header endif,  )
#endif // __i_avs__d306fcf3_ec29_44a0_ab12_3b29b6970c55
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(AVS_PROXY_DEFINITION)
#define AVS_PROXY_DEFINITION

PR_PROXY_BEGIN(AVS)
	PR_PROXY(AVS, SetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(AVS, GetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(AVS, AskAction,        PR_ARG2(tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(AVS, SetState,         PR_ARG1(tid_DWORD))
	PR_PROXY(AVS, GetStatistics,    PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(AVS, GetTreats,        PR_ARG2(tid_DWORD,tid_OBJECT|tid_POINTER|prf_REF))
	PR_PROXY(AVS, ReleaseTreats,    PR_ARG1(tid_OBJECT))
	PR_PROXY(AVS, ProcessCancel,    PR_ARG1(tid_DWORD))
PR_PROXY_END_CM(AVS, IID_AVS, IID_TASK)

#endif // __PROXY_DECLARATION

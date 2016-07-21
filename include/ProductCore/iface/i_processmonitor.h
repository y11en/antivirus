// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  19 December 2007,  11:38 --------
// File Name   -- (null)i_processmonitor.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_processmonitor__48732f19_0ff9_4dd2_91db_bd68f7774a74 )
#define __i_processmonitor__48732f19_0ff9_4dd2_91db_bd68f7774a74
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ProcessMonitor interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_PROCESSMONITOR  ((tIID)(49034))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



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



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hPROCESSMONITOR;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iProcessMonitorVtbl;
typedef struct iProcessMonitorVtbl iProcessMonitorVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cProcessMonitor;
	typedef cProcessMonitor* hPROCESSMONITOR;
#else
	typedef struct tag_hPROCESSMONITOR {
		const iProcessMonitorVtbl* vtbl; // pointer to the "ProcessMonitor" virtual table
		const iSYSTEMVtbl*         sys;  // pointer to the "SYSTEM" virtual table
	} *hPROCESSMONITOR;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( ProcessMonitor_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpProcessMonitor_SetSettings)            ( hPROCESSMONITOR _this, const cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_GetSettings)            ( hPROCESSMONITOR _this, cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_AskAction)              ( hPROCESSMONITOR _this, tActionId actionId, cSerializable* actionInfo ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_SetState)               ( hPROCESSMONITOR _this, tTaskRequestState state ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_GetStatistics)          ( hPROCESSMONITOR _this, cSerializable* statistics ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_CalcObjectHash)         ( hPROCESSMONITOR _this, hOBJECT object, tQWORD* hash ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_GetProcessInfo)         ( hPROCESSMONITOR _this, cSerializable* info ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_IsProcessTrusted)       ( hPROCESSMONITOR _this, cSerializable* data, tBOOL* result ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_IsImageActive)          ( hPROCESSMONITOR _this, const tWCHAR* pFileName, tDWORD* pdwFlags ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_RegisterEventHandler)   ( hPROCESSMONITOR _this, tPTR pEventHandler, tDWORD dwFlags ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_UnregisterEventHandler) ( hPROCESSMONITOR _this, tPTR pEventHandler ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_GetProcessInfoEx)       ( hPROCESSMONITOR _this, tQWORD pid, cSerializable* info ); // -- ;
	typedef   tERROR (pr_call *fnpProcessMonitor_GetFileInfoEx)          ( hPROCESSMONITOR _this, const tWCHAR* filename, cSerializable* info ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iProcessMonitorVtbl {
	fnpProcessMonitor_SetSettings             SetSettings;
	fnpProcessMonitor_GetSettings             GetSettings;
	fnpProcessMonitor_AskAction               AskAction;
	fnpProcessMonitor_SetState                SetState;
	fnpProcessMonitor_GetStatistics           GetStatistics;
	fnpProcessMonitor_CalcObjectHash          CalcObjectHash;
	fnpProcessMonitor_GetProcessInfo          GetProcessInfo;
	fnpProcessMonitor_IsProcessTrusted        IsProcessTrusted;
	fnpProcessMonitor_IsImageActive           IsImageActive;
	fnpProcessMonitor_RegisterEventHandler    RegisterEventHandler;
	fnpProcessMonitor_UnregisterEventHandler  UnregisterEventHandler;
	fnpProcessMonitor_GetProcessInfoEx        GetProcessInfoEx;
	fnpProcessMonitor_GetFileInfoEx           GetFileInfoEx;
}; // "ProcessMonitor" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( ProcessMonitor_PRIVATE_DEFINITION )
// AVP Prague stamp end



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



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_ProcessMonitor_SetSettings( _this, settings )                                           ((_this)->vtbl->SetSettings( (_this), settings ))
	#define CALL_ProcessMonitor_GetSettings( _this, settings )                                           ((_this)->vtbl->GetSettings( (_this), settings ))
	#define CALL_ProcessMonitor_AskAction( _this, actionId, actionInfo )                                 ((_this)->vtbl->AskAction( (_this), actionId, actionInfo ))
	#define CALL_ProcessMonitor_SetState( _this, state )                                                 ((_this)->vtbl->SetState( (_this), state ))
	#define CALL_ProcessMonitor_GetStatistics( _this, statistics )                                       ((_this)->vtbl->GetStatistics( (_this), statistics ))
	#define CALL_ProcessMonitor_CalcObjectHash( _this, object, hash )                                    ((_this)->vtbl->CalcObjectHash( (_this), object, hash ))
	#define CALL_ProcessMonitor_GetProcessInfo( _this, info )                                            ((_this)->vtbl->GetProcessInfo( (_this), info ))
	#define CALL_ProcessMonitor_IsProcessTrusted( _this, data, result )                                  ((_this)->vtbl->IsProcessTrusted( (_this), data, result ))
	#define CALL_ProcessMonitor_IsImageActive( _this, pFileName, pdwFlags )                              ((_this)->vtbl->IsImageActive( (_this), pFileName, pdwFlags ))
	#define CALL_ProcessMonitor_RegisterEventHandler( _this, pEventHandler, dwFlags )                    ((_this)->vtbl->RegisterEventHandler( (_this), pEventHandler, dwFlags ))
	#define CALL_ProcessMonitor_UnregisterEventHandler( _this, pEventHandler )                           ((_this)->vtbl->UnregisterEventHandler( (_this), pEventHandler ))
	#define CALL_ProcessMonitor_GetProcessInfoEx( _this, pid, info )                                     ((_this)->vtbl->GetProcessInfoEx( (_this), pid, info ))
	#define CALL_ProcessMonitor_GetFileInfoEx( _this, filename, info )                                   ((_this)->vtbl->GetFileInfoEx( (_this), filename, info ))
#else // if !defined( __cplusplus )
	#define CALL_ProcessMonitor_SetSettings( _this, settings )                                           ((_this)->SetSettings( settings ))
	#define CALL_ProcessMonitor_GetSettings( _this, settings )                                           ((_this)->GetSettings( settings ))
	#define CALL_ProcessMonitor_AskAction( _this, actionId, actionInfo )                                 ((_this)->AskAction( actionId, actionInfo ))
	#define CALL_ProcessMonitor_SetState( _this, state )                                                 ((_this)->SetState( state ))
	#define CALL_ProcessMonitor_GetStatistics( _this, statistics )                                       ((_this)->GetStatistics( statistics ))
	#define CALL_ProcessMonitor_CalcObjectHash( _this, object, hash )                                    ((_this)->CalcObjectHash( object, hash ))
	#define CALL_ProcessMonitor_GetProcessInfo( _this, info )                                            ((_this)->GetProcessInfo( info ))
	#define CALL_ProcessMonitor_IsProcessTrusted( _this, data, result )                                  ((_this)->IsProcessTrusted( data, result ))
	#define CALL_ProcessMonitor_IsImageActive( _this, pFileName, pdwFlags )                              ((_this)->IsImageActive( pFileName, pdwFlags ))
	#define CALL_ProcessMonitor_RegisterEventHandler( _this, pEventHandler, dwFlags )                    ((_this)->RegisterEventHandler( pEventHandler, dwFlags ))
	#define CALL_ProcessMonitor_UnregisterEventHandler( _this, pEventHandler )                           ((_this)->UnregisterEventHandler( pEventHandler ))
	#define CALL_ProcessMonitor_GetProcessInfoEx( _this, pid, info )                                     ((_this)->GetProcessInfoEx( pid, info ))
	#define CALL_ProcessMonitor_GetFileInfoEx( _this, filename, info )                                   ((_this)->GetFileInfoEx( filename, info ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iProcessMonitor {
		virtual tERROR pr_call SetSettings( const cSerializable* settings ) = 0;
		virtual tERROR pr_call GetSettings( cSerializable* settings ) = 0;
		virtual tERROR pr_call AskAction( tActionId actionId, cSerializable* actionInfo ) = 0;
		virtual tERROR pr_call SetState( tTaskRequestState state ) = 0;
		virtual tERROR pr_call GetStatistics( cSerializable* statistics ) = 0;
		virtual tERROR pr_call CalcObjectHash( hOBJECT object, tQWORD* hash ) = 0;
		virtual tERROR pr_call GetProcessInfo( cSerializable* info ) = 0;
		virtual tERROR pr_call IsProcessTrusted( cSerializable* data, tBOOL* result ) = 0;
		virtual tERROR pr_call IsImageActive( const tWCHAR* pFileName, tDWORD* pdwFlags ) = 0;
		virtual tERROR pr_call RegisterEventHandler( tPTR pEventHandler, tDWORD dwFlags ) = 0;
		virtual tERROR pr_call UnregisterEventHandler( tPTR pEventHandler ) = 0;
		virtual tERROR pr_call GetProcessInfoEx( tQWORD pid, cSerializable* info ) = 0;
		virtual tERROR pr_call GetFileInfoEx( const tWCHAR* filename, cSerializable* info ) = 0;
	};

	struct pr_abstract cProcessMonitor : public iProcessMonitor, public iObj {

		OBJ_IMPL( cProcessMonitor );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hPROCESSMONITOR()   { return (hPROCESSMONITOR)this; }

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
#endif // __i_processmonitor__48732f19_0ff9_4dd2_91db_bd68f7774a74
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(ProcessMonitor_PROXY_DEFINITION)
#define ProcessMonitor_PROXY_DEFINITION

PR_PROXY_BEGIN(ProcessMonitor)
	PR_PROXY(ProcessMonitor, SetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(ProcessMonitor, GetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(ProcessMonitor, AskAction,        PR_ARG2(tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(ProcessMonitor, SetState,         PR_ARG1(tid_DWORD))
	PR_PROXY(ProcessMonitor, GetStatistics,    PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(ProcessMonitor, CalcObjectHash,   PR_ARG2(tid_OBJECT,tid_QWORD|tid_POINTER))
	PR_PROXY(ProcessMonitor, GetProcessInfo,   PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(ProcessMonitor, IsProcessTrusted, PR_ARG2(tid_SERIALIZABLE,tid_BOOL|tid_POINTER))
	PR_PROXY(ProcessMonitor, GetProcessInfoEx, PR_ARG2(tid_QWORD,tid_SERIALIZABLE))
	PR_PROXY(ProcessMonitor, GetFileInfoEx,    PR_ARG2(tid_WSTRING,tid_SERIALIZABLE))
PR_PROXY_END_CM(ProcessMonitor, IID_PROCESSMONITOR, IID_TASK)

#endif // __PROXY_DECLARATION

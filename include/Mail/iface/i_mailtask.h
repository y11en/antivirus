// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  19 November 2004,  12:12 --------
// File Name   -- (null)i_mailtask.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_mailtask__93adab05_5dda_44c6_888d_a2eb98343433 )
#define __i_mailtask__93adab05_5dda_44c6_888d_a2eb98343433
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_reg.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// MailTask interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_MAILTASK  ((tIID)(40006))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



//! Перечень основных действий, производимых над письмом
//typedef enum enum_TotalPrimaryAction
//{
//	mailtpa_Pass = 0,					// Пропустить чистое сообщение.
//	mailtpa_PassAfterChange,			// Сообщение было изменено при проверке. Пропустить - на усмотрение перехватчика
//	mailtpa_PassAfterSkip,				// Что-то было пропущено при проверке. Пропустить - на усмотрение перехватчика
//	mailtpa_DeleteOrBlockMessage,		// Блокировать сообщение от клиента
//	mailtpa_RejectMessage,				// Отразить сообщение
//	mailtpa_PassMessageAsAttachment,	//TODO: А вот это не факт, что можно сделать...
//	mailtpa_LAST
//}	tTotalPrimaryAction;
typedef tDWORD tTotalPrimaryAction;


// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
#define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
#define pmc_MAILTASK_PROCESS 0x0fe9fb5a //

// message class
#define pmc_MAILCHECKER_PROCESS 0xd875646d //
// AVP Prague stamp end

// message class

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hMAILTASK;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iMailTaskVtbl;
typedef struct iMailTaskVtbl iMailTaskVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cMailTask;
	typedef cMailTask* hMAILTASK;
#else
	typedef struct tag_hMAILTASK {
		const iMailTaskVtbl* vtbl; // pointer to the "MailTask" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hMAILTASK;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( MailTask_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpMailTask_SetSettings)   ( hMAILTASK _this, const cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpMailTask_GetSettings)   ( hMAILTASK _this, cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpMailTask_AskAction)     ( hMAILTASK _this, tActionId actionId, cSerializable* actionInfo ); // -- ;
	typedef   tERROR (pr_call *fnpMailTask_SetState)      ( hMAILTASK _this, tTaskState state ); // -- ;
	typedef   tERROR (pr_call *fnpMailTask_GetStatistics) ( hMAILTASK _this, cSerializable* statistics ); // -- ;
	typedef   tERROR (pr_call *fnpMailTask_Process)       ( hMAILTASK _this, hOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut ); // -- Функция процессирования. Используется перехватчиком. Реентерабельна!;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iMailTaskVtbl {
	fnpMailTask_SetSettings    SetSettings;
	fnpMailTask_GetSettings    GetSettings;
	fnpMailTask_AskAction      AskAction;
	fnpMailTask_SetState       SetState;
	fnpMailTask_GetStatistics  GetStatistics;
	fnpMailTask_Process        Process;
}; // "MailTask" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( MailTask_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTASK_TYPE         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgTASK_CONFIG       mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001001 )
#define pgTASK_TM           mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001002 )
#define pgTASK_STATE        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgTASK_SESSION_ID   mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
// AVP Prague stamp end



#define pgTASK_BL pgTASK_TM

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_MailTask_SetSettings( _this, settings )                                                                ((_this)->vtbl->SetSettings( (_this), settings ))
	#define CALL_MailTask_GetSettings( _this, settings )                                                                ((_this)->vtbl->GetSettings( (_this), settings ))
	#define CALL_MailTask_AskAction( _this, actionId, actionInfo )                                                      ((_this)->vtbl->AskAction( (_this), actionId, actionInfo ))
	#define CALL_MailTask_SetState( _this, state )                                                                      ((_this)->vtbl->SetState( (_this), state ))
	#define CALL_MailTask_GetStatistics( _this, statistics )                                                            ((_this)->vtbl->GetStatistics( (_this), statistics ))
	#define CALL_MailTask_Process( _this, p_hTopIO, p_hTotalPrimaryAction, p_szMailTaskName, p_dwTimeOut )              ((_this)->vtbl->Process( (_this), p_hTopIO, p_hTotalPrimaryAction, p_szMailTaskName, p_dwTimeOut ))
#else // if !defined( __cplusplus )
	#define CALL_MailTask_SetSettings( _this, settings )                                                                ((_this)->SetSettings( settings ))
	#define CALL_MailTask_GetSettings( _this, settings )                                                                ((_this)->GetSettings( settings ))
	#define CALL_MailTask_AskAction( _this, actionId, actionInfo )                                                      ((_this)->AskAction( actionId, actionInfo ))
	#define CALL_MailTask_SetState( _this, state )                                                                      ((_this)->SetState( state ))
	#define CALL_MailTask_GetStatistics( _this, statistics )                                                            ((_this)->GetStatistics( statistics ))
	#define CALL_MailTask_Process( _this, p_hTopIO, p_hTotalPrimaryAction, p_szMailTaskName, p_dwTimeOut )              ((_this)->Process( p_hTopIO, p_hTotalPrimaryAction, p_szMailTaskName, p_dwTimeOut ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iMailTask {
		virtual tERROR pr_call SetSettings( const cSerializable* settings ) = 0;
		virtual tERROR pr_call GetSettings( cSerializable* settings ) = 0;
		virtual tERROR pr_call AskAction( tActionId actionId, cSerializable* actionInfo ) = 0;
		virtual tERROR pr_call SetState( tTaskRequestState state ) = 0;
		virtual tERROR pr_call GetStatistics( cSerializable* statistics ) = 0;
		virtual tERROR pr_call Process( hOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut ) = 0; // -- Функция процессирования. Используется перехватчиком. Реентерабельна!
	};

	struct pr_abstract cMailTask : public iMailTask, public iObj {
		OBJ_IMPL( cMailTask );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hMAILTASK()   { return (hMAILTASK)this; }

		tDWORD pr_call get_pgTASK_TYPE() { return (tDWORD)getDWord(pgTASK_TYPE); }
		tERROR pr_call set_pgTASK_TYPE( tDWORD value ) { return setDWord(pgTASK_TYPE,(tDWORD)value); }

		hOBJECT pr_call get_pgTASK_CONFIG() { return (hOBJECT)getObj(pgTASK_CONFIG); }
		tERROR pr_call set_pgTASK_CONFIG( hOBJECT value ) { return setObj(pgTASK_CONFIG,(hOBJECT)value); }

		hOBJECT pr_call get_pgTASK_TM() { return (hOBJECT)getObj(pgTASK_TM); }
		tERROR pr_call set_pgTASK_TM( hOBJECT value ) { return setObj(pgTASK_TM,(hOBJECT)value); }

		tDWORD pr_call get_pgTASK_STATE() { return (tDWORD)getDWord(pgTASK_STATE); }
		tERROR pr_call set_pgTASK_STATE( tDWORD value ) { return setDWord(pgTASK_STATE,(tDWORD)value); }

		tDWORD pr_call get_pgTASK_SESSION_ID() { return (tDWORD)getDWord(pgTASK_SESSION_ID); }
		tERROR pr_call set_pgTASK_SESSION_ID( tDWORD value ) { return setDWord(pgTASK_SESSION_ID,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_mailtask__93adab05_5dda_44c6_888d_a2eb98343433
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(MailTask_PROXY_DEFINITION)
#define MailTask_PROXY_DEFINITION

PR_PROXY_BEGIN(MailTask)
	PR_PROXY(MailTask, SetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(MailTask, GetSettings,      PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(MailTask, AskAction,        PR_ARG2(tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(MailTask, SetState,         PR_ARG1(tid_DWORD))
	PR_PROXY(MailTask, GetStatistics,    PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(MailTask, Process,          PR_ARG4(tid_OBJECT,tid_DWORD|tid_POINTER,tid_STRING,tid_DWORD))
PR_PROXY_END_CM(MailTask, IID_MAILTASK, IID_TASK)

#endif // __PROXY_DECLARATION

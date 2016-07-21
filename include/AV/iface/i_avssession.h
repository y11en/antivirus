// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  30 January 2007,  14:20 --------
// File Name   -- (null)i_avssession.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_avssession__ef808558_b1d7_4a39_9395_b1c6671367db )
#define __i_avssession__ef808558_b1d7_4a39_9395_b1c6671367db
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// AVSSession interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AVSSESSION  ((tIID)(49031))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_AVS_SESSION 0x01060d9d //

	#define pm_PROCESS_BEGIN 0x00001000 // (4096) --

	#define pm_PROCESS_END 0x00001001 // (4097) --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hAVSSESSION;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAVSSessionVtbl;
typedef struct iAVSSessionVtbl iAVSSessionVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAVSSession;
	typedef cAVSSession* hAVSSESSION;
#else
	typedef struct tag_hAVSSESSION 
	{
		const iAVSSessionVtbl* vtbl; // pointer to the "AVSSession" virtual table
		const iSYSTEMVtbl*     sys;  // pointer to the "SYSTEM" virtual table
	} *hAVSSESSION;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AVSSession_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAVSSession_PreProcessObject) ( hAVSSESSION _this, cSerializable* process_info, const cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpAVSSession_ProcessObject)    ( hAVSSESSION _this, hOBJECT object, cSerializable* process_info, const cSerializable* settings, cSerializable* statistics ); // -- ;
	typedef   tERROR (pr_call *fnpAVSSession_GetStatistic)     ( hAVSSESSION _this, cSerializable* statistics ); // -- ;
	typedef   tERROR (pr_call *fnpAVSSession_SetState)         ( hAVSSESSION _this, tTaskRequestState state ); // -- ;
	typedef   tERROR (pr_call *fnpAVSSession_ResetStatistic)   ( hAVSSESSION _this, cSerializable* statistics, tBOOL in ); // -- ;
	typedef   tERROR (pr_call *fnpAVSSession_RegisterScope)    ( hAVSSESSION _this, tDWORD* scope, const cSerializable* info ); // -- ;
	typedef   tERROR (pr_call *fnpAVSSession_ProcessStream)    ( hAVSSESSION _this, tDWORD chunk_type, tPTR chunk, tDWORD size, cSerializable* process_info, const cSerializable* settings, cSerializable* statistic ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAVSSessionVtbl 
{
	fnpAVSSession_PreProcessObject  PreProcessObject;
	fnpAVSSession_ProcessObject     ProcessObject;
	fnpAVSSession_GetStatistic      GetStatistic;
	fnpAVSSession_SetState          SetState;
	fnpAVSSession_ResetStatistic    ResetStatistic;
	fnpAVSSession_RegisterScope     RegisterScope;
	fnpAVSSession_ProcessStream     ProcessStream;
}; // "AVSSession" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AVSSession_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AVSSession_PreProcessObject( _this, process_info, settings )                                                 ((_this)->vtbl->PreProcessObject( (_this), process_info, settings ))
	#define CALL_AVSSession_ProcessObject( _this, object, process_info, settings, statistics )                                ((_this)->vtbl->ProcessObject( (_this), object, process_info, settings, statistics ))
	#define CALL_AVSSession_GetStatistic( _this, statistics )                                                                 ((_this)->vtbl->GetStatistic( (_this), statistics ))
	#define CALL_AVSSession_SetState( _this, state )                                                                          ((_this)->vtbl->SetState( (_this), state ))
	#define CALL_AVSSession_ResetStatistic( _this, statistics, in )                                                           ((_this)->vtbl->ResetStatistic( (_this), statistics, in ))
	#define CALL_AVSSession_RegisterScope( _this, scope, info )                                                               ((_this)->vtbl->RegisterScope( (_this), scope, info ))
	#define CALL_AVSSession_ProcessStream( _this, chunk_type, chunk, size, process_info, settings, statistic )                ((_this)->vtbl->ProcessStream( (_this), chunk_type, chunk, size, process_info, settings, statistic ))
#else // if !defined( __cplusplus )
	#define CALL_AVSSession_PreProcessObject( _this, process_info, settings )                                                 ((_this)->PreProcessObject( process_info, settings ))
	#define CALL_AVSSession_ProcessObject( _this, object, process_info, settings, statistics )                                ((_this)->ProcessObject( object, process_info, settings, statistics ))
	#define CALL_AVSSession_GetStatistic( _this, statistics )                                                                 ((_this)->GetStatistic( statistics ))
	#define CALL_AVSSession_SetState( _this, state )                                                                          ((_this)->SetState( state ))
	#define CALL_AVSSession_ResetStatistic( _this, statistics, in )                                                           ((_this)->ResetStatistic( statistics, in ))
	#define CALL_AVSSession_RegisterScope( _this, scope, info )                                                               ((_this)->RegisterScope( scope, info ))
	#define CALL_AVSSession_ProcessStream( _this, chunk_type, chunk, size, process_info, settings, statistic )                ((_this)->ProcessStream( chunk_type, chunk, size, process_info, settings, statistic ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAVSSession 
	{
		virtual tERROR pr_call PreProcessObject( cSerializable* process_info, const cSerializable* settings ) = 0;
		virtual tERROR pr_call ProcessObject( hOBJECT object, cSerializable* process_info, const cSerializable* settings, cSerializable* statistics ) = 0;
		virtual tERROR pr_call GetStatistic( cSerializable* statistics ) = 0;
		virtual tERROR pr_call SetState( tTaskRequestState state ) = 0;
		virtual tERROR pr_call ResetStatistic( cSerializable* statistics, tBOOL in ) = 0;
		virtual tERROR pr_call RegisterScope( tDWORD* scope, const cSerializable* info ) = 0;
		virtual tERROR pr_call ProcessStream( tDWORD chunk_type, tPTR chunk, tDWORD size, cSerializable* process_info, const cSerializable* settings, cSerializable* statistic ) = 0;
	};

	struct pr_abstract cAVSSession : public iAVSSession, public iObj 
	{

		OBJ_IMPL( cAVSSession );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hAVSSESSION()   { return (hAVSSESSION)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_avssession__ef808558_b1d7_4a39_9395_b1c6671367db
// AVP Prague stamp end




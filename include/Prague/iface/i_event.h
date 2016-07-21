// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  10 March 2004,  14:13 --------
// File Name   -- (null)i_event.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_event__c421972c_8b53_4742_9119_3b3ca8456924 )
#define __i_event__c421972c_8b53_4742_9119_3b3ca8456924
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Event interface implementation
// Short comments -- Event behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_EVENT  ((tIID)(18))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSYNC_EVENT;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iEventVtbl;
typedef struct iEventVtbl iEventVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cEvent;
	typedef cEvent* hSYNC_EVENT;
#else
	typedef struct tag_hSYNC_EVENT 
	{
		const iEventVtbl*  vtbl; // pointer to the "Event" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hSYNC_EVENT;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Event_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpEvent_Wait)     ( hSYNC_EVENT _this, tDWORD timeout );        // -- wait for event to be signaled;
	typedef   tERROR (pr_call *fnpEvent_SetState) ( hSYNC_EVENT _this, tBOOL signaled );        // -- unlocks synchronization object;
	typedef   tERROR (pr_call *fnpEvent_Pulse)    ( hSYNC_EVENT _this );                        // -- pulse event for manual reset events;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iEventVtbl 
{
	fnpEvent_Wait      Wait;
	fnpEvent_SetState  SetState;
	fnpEvent_Pulse     Pulse;
}; // "Event" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Event_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
#define pgJUST_OPEN         mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgMANUAL_RESET      mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001001 )
#define pgLAST_OS_ERROR     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgINITIAL_STATE     mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001003 )
#define pgGLOBAL            mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001005 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Event_Wait( _this, timeout )                ((_this)->vtbl->Wait( (_this), timeout ))
	#define CALL_Event_SetState( _this, signaled )           ((_this)->vtbl->SetState( (_this), signaled ))
	#define CALL_Event_Pulse( _this )                        ((_this)->vtbl->Pulse( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_Event_Wait( _this, timeout )                ((_this)->Wait( timeout ))
	#define CALL_Event_SetState( _this, signaled )           ((_this)->SetState( signaled ))
	#define CALL_Event_Pulse( _this )                        ((_this)->Pulse( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iEvent 
	{
		virtual tERROR pr_call Wait( tDWORD timeout ) = 0; // -- wait for event to be signaled
		virtual tERROR pr_call SetState( tBOOL signaled ) = 0; // -- unlocks synchronization object
		virtual tERROR pr_call Pulse() = 0; // -- pulse event for manual reset events
	};

	struct pr_abstract cEvent : public iEvent, public iObj {

		OBJ_IMPL( cEvent );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSYNC_EVENT()   { return (hSYNC_EVENT)this; }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tBOOL pr_call get_pgJUST_OPEN() { return (tBOOL)getBool(pgJUST_OPEN); }
		tERROR pr_call set_pgJUST_OPEN( tBOOL value ) { return setBool(pgJUST_OPEN,(tBOOL)value); }

		tBOOL pr_call get_pgMANUAL_RESET() { return (tBOOL)getBool(pgMANUAL_RESET); }
		tERROR pr_call set_pgMANUAL_RESET( tBOOL value ) { return setBool(pgMANUAL_RESET,(tBOOL)value); }

		tDWORD pr_call get_pgLAST_OS_ERROR() { return (tDWORD)getDWord(pgLAST_OS_ERROR); }
		tERROR pr_call set_pgLAST_OS_ERROR( tDWORD value ) { return setDWord(pgLAST_OS_ERROR,(tDWORD)value); }

		tBOOL pr_call get_pgINITIAL_STATE() { return (tBOOL)getBool(pgINITIAL_STATE); }
		tERROR pr_call set_pgINITIAL_STATE( tBOOL value ) { return setBool(pgINITIAL_STATE,(tBOOL)value); }

		tBOOL pr_call get_pgGLOBAL() { return (tBOOL)getBool(pgGLOBAL); }
		tERROR pr_call set_pgGLOBAL( tBOOL value ) { return setBool(pgGLOBAL,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_event__0ca3511d_2c95_421e_970b_73f998d267ca
// AVP Prague stamp end




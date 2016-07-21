// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  30 March 2006,  11:51 --------
// File Name   -- (null)i_tmhost.c
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_tmhost__a4d2f79d_e4cd_4c51_b6e1_f8bef403f242 )
#define __i_tmhost__a4d2f79d_e4cd_4c51_b6e1_f8bef403f242
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// TmHost interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TMHOST  ((tIID)(49035))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTMHOST;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iTmHostVtbl;
typedef struct iTmHostVtbl iTmHostVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cTmHost;
	typedef cTmHost* hTMHOST;
#else
	typedef struct tag_hTMHOST {
		const iTmHostVtbl* vtbl; // pointer to the "TmHost" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hTMHOST;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( TmHost_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpTmHost_CreateTask)  ( hTMHOST _this, hOBJECT host, cSerializable* info, hOBJECT* task ); // -- ;
	typedef   tERROR (pr_call *fnpTmHost_DestroyTask) ( hTMHOST _this, hOBJECT task );        // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iTmHostVtbl {
	fnpTmHost_CreateTask   CreateTask;
	fnpTmHost_DestroyTask  DestroyTask;
}; // "TmHost" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( TmHost_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_TmHost_CreateTask( _this, host, info, task )            ((_this)->vtbl->CreateTask( (_this), host, info, task ))
	#define CALL_TmHost_DestroyTask( _this, task )                       ((_this)->vtbl->DestroyTask( (_this), task ))
#else // if !defined( __cplusplus )
	#define CALL_TmHost_CreateTask( _this, host, info, task )            ((_this)->CreateTask( host, info, task ))
	#define CALL_TmHost_DestroyTask( _this, task )                       ((_this)->DestroyTask( task ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iTmHost {
		virtual tERROR pr_call CreateTask( hOBJECT host, cSerializable* info, hOBJECT* task ) = 0;
		virtual tERROR pr_call DestroyTask( hOBJECT task ) = 0;
	};

	struct pr_abstract cTmHost : public iTmHost, public iObj {
		OBJ_IMPL( cTmHost );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTMHOST()   { return (hTMHOST)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_tmhost__a4d2f79d_e4cd_4c51_b6e1_f8bef403f242
// AVP Prague stamp end


#if defined(__PROXY_DECLARATION) && !defined(TmHost_PROXY_DEFINITION)
#define TmHost_PROXY_DEFINITION

PR_PROXY_BEGIN(TmHost)
	PR_PROXY(TmHost, CreateTask,      PR_ARG3(tid_OBJECT,tid_SERIALIZABLE,tid_OBJECT|tid_POINTER|prf_REF))
	PR_PROXY(TmHost, DestroyTask,     PR_ARG1(tid_OBJECT|prf_DEREF))
PR_PROXY_END(TmHost, IID_TMHOST)

#endif // __PROXY_DECLARATION

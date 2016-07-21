// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  22 June 2005,  18:40 --------
// File Name   -- (null)i_tmclient.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_tmclient__f7055cfe_aba5_45e4_961c_43cc6c840820 )
#define __i_tmclient__f7055cfe_aba5_45e4_961c_43cc6c840820
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// TmClient interface implementation
// Short comments -- client of TaskManager
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TM_CLIENT  ((tIID)(45))
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
#define errDONT_WANT_TO_PROCESS                  PR_MAKE_DECL_ERR(IID_TM_CLIENT, 0x001) // 0x8002d001,-2147299327 (1) -- client do not want to process this request, valid for both OpenRequest and ProcessRequest
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTM_CLIENT;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iTmClientVtbl;
typedef struct iTmClientVtbl iTmClientVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cTmClient;
	typedef cTmClient* hTM_CLIENT;
#else
	typedef struct tag_hTM_CLIENT {
		const iTmClientVtbl* vtbl; // pointer to the "TmClient" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hTM_CLIENT;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( TmClient_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpTmClient_OpenRequest)    ( hTM_CLIENT _this, tDWORD request_id, tDWORD request_type, cSerializable* params ); // -- open request;
	typedef   tERROR (pr_call *fnpTmClient_ProcessRequest) ( hTM_CLIENT _this, tDWORD request_id, tDWORD request_type, cSerializable* params ); // -- open request;
	typedef   tERROR (pr_call *fnpTmClient_CloseRequest)   ( hTM_CLIENT _this, tDWORD request_id ); // -- close request by id;
	typedef   tERROR (pr_call *fnpTmClient_Exit)           ( hTM_CLIENT _this, tDWORD cause );   // -- ;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iTmClientVtbl {
	fnpTmClient_OpenRequest     OpenRequest;
	fnpTmClient_ProcessRequest  ProcessRequest;
	fnpTmClient_CloseRequest    CloseRequest;
	fnpTmClient_Exit            Exit;
}; // "TmClient" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( TmClient_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_TmClient_OpenRequest( _this, request_id, request_type, params )                 ((_this)->vtbl->OpenRequest( (_this), request_id, request_type, params ))
	#define CALL_TmClient_ProcessRequest( _this, request_id, request_type, params )              ((_this)->vtbl->ProcessRequest( (_this), request_id, request_type, params ))
	#define CALL_TmClient_CloseRequest( _this, request_id )                                      ((_this)->vtbl->CloseRequest( (_this), request_id ))
	#define CALL_TmClient_Exit( _this, cause )                                                   ((_this)->vtbl->Exit( (_this), cause ))
#else // if !defined( __cplusplus )
	#define CALL_TmClient_OpenRequest( _this, request_id, request_type, params )                 ((_this)->OpenRequest( request_id, request_type, params ))
	#define CALL_TmClient_ProcessRequest( _this, request_id, request_type, params )              ((_this)->ProcessRequest( request_id, request_type, params ))
	#define CALL_TmClient_CloseRequest( _this, request_id )                                      ((_this)->CloseRequest( request_id ))
	#define CALL_TmClient_Exit( _this, cause )                                                   ((_this)->Exit( cause ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iTmClient {
		virtual tERROR pr_call OpenRequest( tDWORD request_id, tDWORD request_type, cSerializable* params ) = 0; // -- open request
		virtual tERROR pr_call ProcessRequest( tDWORD request_id, tDWORD request_type, cSerializable* params ) = 0; // -- open request
		virtual tERROR pr_call CloseRequest( tDWORD request_id ) = 0; // -- close request by id
		virtual tERROR pr_call Exit( tDWORD cause ) = 0;
	};

	struct pr_abstract cTmClient : public iTmClient, public iObj {
		OBJ_IMPL( cTmClient );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTM_CLIENT()   { return (hTM_CLIENT)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_tmclient__f7055cfe_aba5_45e4_961c_43cc6c840820
// AVP Prague stamp end


#if defined(__PROXY_DECLARATION) && !defined(TmClient_PROXY_DEFINITION)
#define TmClient_PROXY_DEFINITION

PR_PROXY_BEGIN(TmClient)
	PR_PROXY(TmClient, OpenRequest,      PR_ARG3(tid_DWORD,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TmClient, ProcessRequest,   PR_ARG3(tid_DWORD,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(TmClient, CloseRequest,     PR_ARG1(tid_DWORD))
	PR_PROXY(TmClient, Exit,             PR_ARG1(tid_DWORD))
PR_PROXY_END(TmClient, IID_TM_CLIENT)

#endif // __PROXY_DECLARATION

// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  29 October 2004,  14:39 --------
// File Name   -- (null)i_avstreats.c
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_avstreats__07a5b6f4_09a4_4f91_a1b0_0b603dda45d3 )
#define __i_avstreats__07a5b6f4_09a4_4f91_a1b0_0b603dda45d3
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// AVSTreats interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AVSTREATS  ((tIID)(49033))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define FILTER_NONE                    ((tDWORD)(0)) //  --
#define FILTER_UNTREATED               ((tDWORD)(1)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hAVSTREATS;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAVSTreatsVtbl;
typedef struct iAVSTreatsVtbl iAVSTreatsVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAVSTreats;
	typedef cAVSTreats* hAVSTREATS;
#else
	typedef struct tag_hAVSTREATS {
		const iAVSTreatsVtbl* vtbl; // pointer to the "AVSTreats" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hAVSTREATS;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AVSTreats_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAVSTreats_GetTreatInfo)  ( hAVSTREATS _this, tDWORD filter, tDWORD pos, cSerializable* info ); // -- ;
	typedef   tERROR (pr_call *fnpAVSTreats_Process)       ( hAVSTREATS _this, tDWORD filter, const cSerializable* process_info ); // -- ;
	typedef   tERROR (pr_call *fnpAVSTreats_GetInfo)       ( hAVSTREATS _this, cSerializable* info ); // -- ;
	typedef   tERROR (pr_call *fnpAVSTreats_ProcessCancel) ( hAVSTREATS _this );                 // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAVSTreatsVtbl {
	fnpAVSTreats_GetTreatInfo   GetTreatInfo;
	fnpAVSTreats_Process        Process;
	fnpAVSTreats_GetInfo        GetInfo;
	fnpAVSTreats_ProcessCancel  ProcessCancel;
}; // "AVSTreats" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AVSTreats_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AVSTreats_GetTreatInfo( _this, filter, pos, info )               ((_this)->vtbl->GetTreatInfo( (_this), filter, pos, info ))
	#define CALL_AVSTreats_Process( _this, filter, process_info )                 ((_this)->vtbl->Process( (_this), filter, process_info ))
	#define CALL_AVSTreats_GetInfo( _this, info )                                 ((_this)->vtbl->GetInfo( (_this), info ))
	#define CALL_AVSTreats_ProcessCancel( _this )                                 ((_this)->vtbl->ProcessCancel( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_AVSTreats_GetTreatInfo( _this, filter, pos, info )               ((_this)->GetTreatInfo( filter, pos, info ))
	#define CALL_AVSTreats_Process( _this, filter, process_info )                 ((_this)->Process( filter, process_info ))
	#define CALL_AVSTreats_GetInfo( _this, info )                                 ((_this)->GetInfo( info ))
	#define CALL_AVSTreats_ProcessCancel( _this )                                 ((_this)->ProcessCancel( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAVSTreats {
		virtual tERROR pr_call GetTreatInfo( tDWORD filter, tDWORD pos, cSerializable* info ) = 0;
		virtual tERROR pr_call Process( tDWORD filter, const cSerializable* process_info ) = 0;
		virtual tERROR pr_call GetInfo( cSerializable* info ) = 0;
		virtual tERROR pr_call ProcessCancel() = 0;
	};

	struct pr_abstract cAVSTreats : public iAVSTreats, public iObj {
		OBJ_IMPL( cAVSTreats );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hAVSTREATS()   { return (hAVSTREATS)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_avstreats__07a5b6f4_09a4_4f91_a1b0_0b603dda45d3
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(AVSTreats_PROXY_DEFINITION)
#define AVSTreats_PROXY_DEFINITION

PR_PROXY_BEGIN(AVSTreats)
	PR_PROXY(AVSTreats, GetTreatInfo,     PR_ARG3(tid_DWORD,tid_DWORD,tid_SERIALIZABLE))
	PR_SCPRX(AVSTreats, Process,          PR_ARG2(tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(AVSTreats, GetInfo,          PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(AVSTreats, ProcessCancel,    PR_ARG0())
PR_PROXY_END(AVSTreats, IID_AVSTREATS)

#endif // __PROXY_DECLARATION

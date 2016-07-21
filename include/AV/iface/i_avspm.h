// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  30 March 2005,  16:10 --------
// File Name   -- (null)i_avspm.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_avspm__7697f0f6_f74c_4d58_8d47_d643bedff900 )
#define __i_avspm__7697f0f6_f74c_4d58_8d47_d643bedff900
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_serializable.h>


// AVP Prague stamp begin( Interface comment,  )
// AVSPM interface implementation
// Short comments -- AVS Performance Monitoring
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AVSPM  ((tIID)(49030))
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
	typedef tUINT hAVSPM;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAVSPMVtbl;
typedef struct iAVSPMVtbl iAVSPMVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAVSPM;
	typedef cAVSPM* hAVSPM;
#else
	typedef struct tag_hAVSPM 
	{
		const iAVSPMVtbl*  vtbl; // pointer to the "AVSPM" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hAVSPM;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AVSPM_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAVSPM_ProcessBegin)      ( hAVSPM _this, tPTR engine );                 // -- ;
	typedef   tERROR (pr_call *fnpAVSPM_ProcessEnd)        ( hAVSPM _this );                 // -- ;
	typedef   tERROR (pr_call *fnpAVSPM_ProcessYield)      ( hAVSPM _this );                 // -- ;
	typedef   tERROR (pr_call *fnpAVSPM_GetPerformance)    ( hAVSPM _this, cSerializable* stat ); // -- ;
	typedef   tERROR (pr_call *fnpAVSPM_GetProcessorUsage) ( hAVSPM _this, tDWORD* usage );  // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAVSPMVtbl 
{
	fnpAVSPM_ProcessBegin       ProcessBegin;
	fnpAVSPM_ProcessEnd         ProcessEnd;
	fnpAVSPM_ProcessYield       ProcessYield;
	fnpAVSPM_GetPerformance     GetPerformance;
	fnpAVSPM_GetProcessorUsage  GetProcessorUsage;
}; // "AVSPM" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AVSPM_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AVSPM_ProcessBegin( _this, engine )               ((_this)->vtbl->ProcessBegin( (_this, engine) ))
	#define CALL_AVSPM_ProcessEnd( _this )                         ((_this)->vtbl->ProcessEnd( (_this) ))
	#define CALL_AVSPM_ProcessYield( _this )                       ((_this)->vtbl->ProcessYield( (_this) ))
	#define CALL_AVSPM_GetPerformance( _this, stat )               ((_this)->vtbl->GetPerformance( (_this), stat ))
	#define CALL_AVSPM_GetProcessorUsage( _this, usage )           ((_this)->vtbl->GetProcessorUsage( (_this), usage ))
#else // if !defined( __cplusplus )
	#define CALL_AVSPM_ProcessBegin( _this, engine )               ((_this)->ProcessBegin(engine))
	#define CALL_AVSPM_ProcessEnd( _this )                         ((_this)->ProcessEnd( ))
	#define CALL_AVSPM_ProcessYield( _this )                       ((_this)->ProcessYield( ))
	#define CALL_AVSPM_GetPerformance( _this, stat )               ((_this)->GetPerformance( stat ))
	#define CALL_AVSPM_GetProcessorUsage( _this, usage )           ((_this)->GetProcessorUsage( usage ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAVSPM 
	{
		virtual tERROR pr_call ProcessBegin(tPTR engine) = 0;
		virtual tERROR pr_call ProcessEnd() = 0;
		virtual tERROR pr_call ProcessYield() = 0;
		virtual tERROR pr_call GetPerformance( cSerializable* stat ) = 0;
		virtual tERROR pr_call GetProcessorUsage( tDWORD* usage ) = 0;
	};

	struct pr_abstract cAVSPM : public iAVSPM, public iObj 
	{
		OBJ_IMPL( cAVSPM );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hAVSPM()   { return (hAVSPM)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_avspm__7697f0f6_f74c_4d58_8d47_d643bedff900
// AVP Prague stamp end




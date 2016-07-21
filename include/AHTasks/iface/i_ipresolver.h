// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  14 December 2004,  12:51 --------
// File Name   -- (null)i_ipresolver.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_ipresolver__c99422de_925a_435f_9da6_0058a4f746fb )
#define __i_ipresolver__c99422de_925a_435f_9da6_0058a4f746fb
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_serializable.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end







// AVP Prague stamp begin( Interface comment,  )
// IPRESOLVER interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_IPRESOLVER  ((tIID)(61011))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tResolveMethod; //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define cJUST_RESOLVE                  ((tResolveMethod)(0)) //  --
#define cTIMED_RESOLVE                 ((tResolveMethod)(1)) //  --
#define cCONTINOUS_RESOLVE             ((tResolveMethod)(2)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_IPRESOLVER_RESULT 0xd79fba8b //

	#define pm_RESOLVE_INFO 0x00001000 // (4096) --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hIPRESOLVER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iIPRESOLVERVtbl;
typedef struct iIPRESOLVERVtbl iIPRESOLVERVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cIPRESOLVER;
	typedef cIPRESOLVER* hIPRESOLVER;
#else
	typedef struct tag_hIPRESOLVER 
	{
		const iIPRESOLVERVtbl* vtbl; // pointer to the "IPRESOLVER" virtual table
		const iSYSTEMVtbl*     sys;  // pointer to the "SYSTEM" virtual table
	} *hIPRESOLVER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( IPRESOLVER_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpIPRESOLVER_ResolveHost)       ( hIPRESOLVER _this, tUINT* result, hSTRING HostName, tResolveMethod ResolveMethod ); // -- ;
	typedef   tERROR (pr_call *fnpIPRESOLVER_ResolveBreak)      ( hIPRESOLVER _this, tUINT ResolveID ); // -- ;
	typedef   tERROR (pr_call *fnpIPRESOLVER_ResolveHostAtOnce) ( hIPRESOLVER _this, hSTRING HostName, cSerializable* IPs ); // -- ;
	typedef   tERROR (pr_call *fnpIPRESOLVER_ResolveIP)         ( hIPRESOLVER _this, tUINT* result, cSerializable* IP, tResolveMethod ResolveMethod ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iIPRESOLVERVtbl 
{
	fnpIPRESOLVER_ResolveHost        ResolveHost;
	fnpIPRESOLVER_ResolveBreak       ResolveBreak;
	fnpIPRESOLVER_ResolveHostAtOnce  ResolveHostAtOnce;
	fnpIPRESOLVER_ResolveIP          ResolveIP;
}; // "IPRESOLVER" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( IPRESOLVER_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_IPRESOLVER_ResolveHost( _this, result, HostName, ResolveMethod )        ((_this)->vtbl->ResolveHost( (_this), result, HostName, ResolveMethod ))
	#define CALL_IPRESOLVER_ResolveBreak( _this, ResolveID )                             ((_this)->vtbl->ResolveBreak( (_this), ResolveID ))
	#define CALL_IPRESOLVER_ResolveHostAtOnce( _this, HostName, IPs )                    ((_this)->vtbl->ResolveHostAtOnce( (_this), HostName, IPs ))
	#define CALL_IPRESOLVER_ResolveIP( _this, result, ip, ResolveMethod )                ((_this)->vtbl->ResolveIP( (_this), result, ip, ResolveMethod ))
#else // if !defined( __cplusplus )
	#define CALL_IPRESOLVER_ResolveHost( _this, result, HostName, ResolveMethod )        ((_this)->ResolveHost( result, HostName, ResolveMethod ))
	#define CALL_IPRESOLVER_ResolveBreak( _this, ResolveID )                             ((_this)->ResolveBreak( ResolveID ))
	#define CALL_IPRESOLVER_ResolveHostAtOnce( _this, HostName, IPs )                    ((_this)->ResolveHostAtOnce( HostName, IPs ))
	#define CALL_IPRESOLVER_ResolveIP( _this, result, ip, ResolveMethod )                ((_this)->ResolveIP( result, ip, ResolveMethod ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iIPRESOLVER 
	{
		virtual tERROR pr_call ResolveHost( tUINT* result, hSTRING HostName, tResolveMethod ResolveMethod ) = 0;
		virtual tERROR pr_call ResolveBreak( tUINT ResolveID ) = 0;
		virtual tERROR pr_call ResolveHostAtOnce( hSTRING HostName, cSerializable* IPs ) = 0;
        virtual tERROR pr_call ResolveIP( tUINT* result, cSerializable * IP, tResolveMethod ResolveMethod ) = 0;
	};

	struct pr_abstract cIPRESOLVER : public iIPRESOLVER, public iObj {

		OBJ_IMPL( cIPRESOLVER );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hIPRESOLVER()   { return (hIPRESOLVER)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_ipresolver__c99422de_925a_435f_9da6_0058a4f746fb
// AVP Prague stamp end




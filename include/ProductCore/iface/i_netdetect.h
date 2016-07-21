// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  11 January 2006,  16:21 --------
// File Name   -- (null)i_netdetect.cpp
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_netdetect__b2115174_a025_40d4_b46b_df9fa3cada31 )
#define __i_netdetect__b2115174_a025_40d4_b46b_df9fa3cada31
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// NetDetect interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_NETDETECT  ((tIID)(58019))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define cINTERNET_CONNECTION_MODEM     ((tDWORD)(0x01)) //  --
#define cINTERNET_CONNECTION_LAN       ((tDWORD)(0x02)) //  --
#define cINTERNET_CONNECTION_PROXY     ((tDWORD)(0x04)) //  --
#define cINTERNET_RAS_INSTALLED        ((tDWORD)(0x10)) //  --
#define cINTERNET_CONNECTION_OFFLINE   ((tDWORD)(0x20)) //  --
#define cINTERNET_CONNECTION_CONFIGURED ((tDWORD)(0x40)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hNETDETECT;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iNetDetectVtbl;
typedef struct iNetDetectVtbl iNetDetectVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cNetDetect;
	typedef cNetDetect* hNETDETECT;
#else
	typedef struct tag_hNETDETECT 
	{
		const iNetDetectVtbl* vtbl; // pointer to the "NetDetect" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hNETDETECT;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( NetDetect_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpNetDetect_Detect)                    ( hNETDETECT _this );     // -- Check internet connection;
	typedef   tERROR (pr_call *fnpNetDetect_InternetGetConnectedState) ( hNETDETECT _this, tDWORD* pdwFlags ); // -- Receives cINTERNET_xxx flags;
	typedef   tERROR (pr_call *fnpNetDetect_GetLastIpAddrChange)       ( hNETDETECT _this, tDWORD* pdwLastChangeTime ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iNetDetectVtbl 
{
	fnpNetDetect_Detect                     Detect;
	fnpNetDetect_InternetGetConnectedState  InternetGetConnectedState;
	fnpNetDetect_GetLastIpAddrChange        GetLastIpAddrChange;
}; // "NetDetect" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( NetDetect_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_NetDetect_Detect( _this )                                               ((_this)->vtbl->Detect( (_this) ))
	#define CALL_NetDetect_InternetGetConnectedState( _this, pdwFlags )                  ((_this)->vtbl->InternetGetConnectedState( (_this), pdwFlags ))
	#define CALL_NetDetect_GetLastIpAddrChange( _this, pdwLastChangeTime )               ((_this)->vtbl->GetLastIpAddrChange( (_this), pdwLastChangeTime ))
#else // if !defined( __cplusplus )
	#define CALL_NetDetect_Detect( _this )                                               ((_this)->Detect( ))
	#define CALL_NetDetect_InternetGetConnectedState( _this, pdwFlags )                  ((_this)->InternetGetConnectedState( pdwFlags ))
	#define CALL_NetDetect_GetLastIpAddrChange( _this, pdwLastChangeTime )               ((_this)->GetLastIpAddrChange( pdwLastChangeTime ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iNetDetect 
	{
		virtual tERROR pr_call Detect() = 0; // -- Check internet connection
		virtual tERROR pr_call InternetGetConnectedState( tDWORD* pdwFlags ) = 0; // -- Receives cINTERNET_xxx flags
		virtual tERROR pr_call GetLastIpAddrChange( tDWORD* pdwLastChangeTime ) = 0;
	};

	struct pr_abstract cNetDetect : public iNetDetect, public iObj 
	{
		OBJ_IMPL( cNetDetect );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hNETDETECT()   { return (hNETDETECT)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_netdetect__b2115174_a025_40d4_b46b_df9fa3cada31
// AVP Prague stamp end




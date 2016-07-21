// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  25 December 2006,  10:58 --------
// File Name   -- (null)i_token.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_token__ba403cf0_e716_4a1d_ba0f_9f5132521dfb )
#define __i_token__ba403cf0_e716_4a1d_ba0f_9f5132521dfb
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// Token interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TOKEN  ((tIID)(99))
// AVP Prague stamp end



#include <Prague/iface/i_string.h>

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
	typedef tUINT hTOKEN;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iTokenVtbl;
typedef struct iTokenVtbl iTokenVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cToken;
	typedef cToken* hTOKEN;
#else
	typedef struct tag_hTOKEN {
		const iTokenVtbl*  vtbl; // pointer to the "Token" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hTOKEN;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Token_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpToken_Impersonate) ( hTOKEN _this, tDWORD type );          // -- ;
	typedef   tERROR (pr_call *fnpToken_Revert)      ( hTOKEN _this );                       // -- ;
	typedef   tERROR (pr_call *fnpToken_Logon)       ( hTOKEN _this, hSTRING account, hSTRING password ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iTokenVtbl {
	fnpToken_Impersonate  Impersonate;
	fnpToken_Revert       Revert;
	fnpToken_Logon        Logon;
}; // "Token" external virtual table prototype
// AVP Prague stamp end

enum enImpersonationType
{
	eitDefault = 0,
	eitAsAdmin = 1,
	eitAsUser = 2
};

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Token_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Token_Impersonate( _this, type )                  ((_this)->vtbl->Impersonate( (_this), type ))
	#define CALL_Token_Revert( _this )                             ((_this)->vtbl->Revert( (_this) ))
	#define CALL_Token_Logon( _this, account, password )           ((_this)->vtbl->Logon( (_this), account, password ))
#else // if !defined( __cplusplus )
	#define CALL_Token_Impersonate( _this, type )                  ((_this)->Impersonate( type ))
	#define CALL_Token_Revert( _this )                             ((_this)->Revert( ))
	#define CALL_Token_Logon( _this, account, password )           ((_this)->Logon( account, password ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iToken {
		virtual tERROR pr_call Impersonate( tDWORD type = 0) = 0;
		virtual tERROR pr_call Revert() = 0;
		virtual tERROR pr_call Logon( hSTRING account, hSTRING password ) = 0;
	};

	struct pr_abstract cToken : public iToken, public iObj {

		OBJ_IMPL( cToken );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTOKEN()   { return (hTOKEN)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_token__ba403cf0_e716_4a1d_ba0f_9f5132521dfb
// AVP Prague stamp end




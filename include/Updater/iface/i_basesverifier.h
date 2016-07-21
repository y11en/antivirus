// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  23 October 2007,  19:22 --------
// File Name   -- (null)i_basesverifier.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_basesverifier__59da8425_829b_4dbd_be07_f2274caa810a )
#define __i_basesverifier__59da8425_829b_4dbd_be07_f2274caa810a
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// BasesVerifier interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_BASESVERIFIER  ((tIID)(56017))
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
	typedef tUINT hBASESVERIFIER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iBasesVerifierVtbl;
typedef struct iBasesVerifierVtbl iBasesVerifierVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cBasesVerifier;
	typedef cBasesVerifier* hBASESVERIFIER;
#else
	typedef struct tag_hBASESVERIFIER 
	{
		const iBasesVerifierVtbl* vtbl; // pointer to the "BasesVerifier" virtual table
		const iSYSTEMVtbl*        sys;  // pointer to the "SYSTEM" virtual table
	} *hBASESVERIFIER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( BasesVerifier_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpBasesVerifier_VerifyBases) ( hBASESVERIFIER _this, hSTRING components, hSTRING blackListPath, tDATETIME* basesDate ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iBasesVerifierVtbl 
{
	fnpBasesVerifier_VerifyBases  VerifyBases;
}; // "BasesVerifier" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( BasesVerifier_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_BasesVerifier_VerifyBases( _this, components, blackListPath, basesDate )                   ((_this)->vtbl->VerifyBases( (_this), components, blackListPath, basesDate ))
#else // if !defined( __cplusplus )
	#define CALL_BasesVerifier_VerifyBases( _this, components, blackListPath, basesDate )                   ((_this)->VerifyBases( components, blackListPath, basesDate ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iBasesVerifier 
	{
		virtual tERROR pr_call VerifyBases( hSTRING components, hSTRING blackListPath, tDATETIME* basesDate ) = 0;
	};

	struct pr_abstract cBasesVerifier : public iBasesVerifier, public iObj 
	{

		OBJ_IMPL( cBasesVerifier );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hBASESVERIFIER()   { return (hBASESVERIFIER)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_basesverifier__59da8425_829b_4dbd_be07_f2274caa810a
// AVP Prague stamp end




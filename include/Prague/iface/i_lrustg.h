// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:17 --------
// File Name   -- (null)i_lrustg.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_lrustg__e30d2e5c_3394_48b0_ba92_3150a20972b1 )
#define __i_lrustg__e30d2e5c_3394_48b0_ba92_3150a20972b1
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// LRUstg interface implementation
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_LRUSTG  ((tIID)(64014))
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
	typedef tUINT hLRUSTG;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iLRUstgVtbl;
typedef struct iLRUstgVtbl iLRUstgVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cLRUstg;
	typedef cLRUstg* hLRUSTG;
#else
	typedef struct tag_hLRUSTG {
		const iLRUstgVtbl* vtbl; // pointer to the "LRUstg" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hLRUSTG;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( LRUstg_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpLRUstg_Read)  ( hLRUSTG _this, tDWORD int_block, tDWORD ext_block ); // -- ;
	typedef   tERROR (pr_call *fnpLRUstg_Write) ( hLRUSTG _this, tDWORD int_block, tQWORD ext_block ); // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iLRUstgVtbl {
	fnpLRUstg_Read   Read;
	fnpLRUstg_Write  Write;
}; // "LRUstg" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( LRUstg_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_LRUstg_Read( _this, int_block, ext_block )             ((_this)->vtbl->Read( (_this), int_block, ext_block ))
	#define CALL_LRUstg_Write( _this, int_block, ext_block )            ((_this)->vtbl->Write( (_this), int_block, ext_block ))
#else // if !defined( __cplusplus )
	#define CALL_LRUstg_Read( _this, int_block, ext_block )             ((_this)->Read( int_block, ext_block ))
	#define CALL_LRUstg_Write( _this, int_block, ext_block )            ((_this)->Write( int_block, ext_block ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iLRUstg {
		virtual tERROR pr_call Read( tDWORD int_block, tDWORD ext_block ) = 0;
		virtual tERROR pr_call Write( tDWORD int_block, tQWORD ext_block ) = 0;
	};

	struct pr_abstract cLRUstg : public iLRUstg, public iObj {

		OBJ_IMPL( cLRUstg );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hLRUSTG()   { return (hLRUSTG)this; }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_lrustg__37b14238_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end


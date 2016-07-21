// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:22 --------
// File Name   -- (null)i_compar.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_compar__e21c7760_36ca_4fa7_8a48_5a5ddeedbad5 )
#define __i_compar__e21c7760_36ca_4fa7_8a48_5a5ddeedbad5
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// Compare interface implementation
// Short comments -- compare interface
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_COMPARE  ((tIID)(64001))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define cCOMPARE_LESS                  ((tINT)(-1)) //  --
#define cCOMPARE_EQUIL                 ((tINT)(0)) //  --
#define cCOMPARE_GREATER               ((tINT)(1)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hCOMPARE;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iCompareVtbl;
typedef struct iCompareVtbl iCompareVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cCompare;
	typedef cCompare* hCOMPARE;
#else
	typedef struct tag_hCOMPARE {
		const iCompareVtbl* vtbl; // pointer to the "Compare" virtual table
		const iSYSTEMVtbl*  sys;  // pointer to the "SYSTEM" virtual table
	} *hCOMPARE;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Compare_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpCompare_Compare) ( hCOMPARE _this, tINT* result, tPTR data1, tDWORD size1, tPTR data2, tDWORD size2 ); // -- Compare two raw data blocks;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iCompareVtbl {
	fnpCompare_Compare  Compare;
}; // "Compare" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Compare_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgCOMPARE_SETTINGS  mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x00001000 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Compare_Compare( _this, result, data1, size1, data2, size2 )     ((_this)->vtbl->Compare( (_this), result, data1, size1, data2, size2 ))
#else // if !defined( __cplusplus )
	#define CALL_Compare_Compare( _this, result, data1, size1, data2, size2 )     ((_this)->Compare( result, data1, size1, data2, size2 ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iCompare {
		virtual tERROR pr_call Compare( tINT* result, tPTR data1, tDWORD size1, tPTR data2, tDWORD size2 ) = 0; // -- Compare two raw data blocks
	};

	struct pr_abstract cCompare : public iCompare, public iObj {

		OBJ_IMPL( cCompare );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hCOMPARE()   { return (hCOMPARE)this; }

		tERROR pr_call get_pgCOMPARE_SETTINGS( tPTR value, tDWORD size ) { return get(pgCOMPARE_SETTINGS,value,size); }
		tERROR pr_call set_pgCOMPARE_SETTINGS( tPTR value, tDWORD size ) { return set(pgCOMPARE_SETTINGS,value,size); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_compar__37b132fd_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end


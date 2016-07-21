// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:22 --------
// File Name   -- (null)i_methodinfo.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_methodinfo__dc5fba5e_8515_4d02_a0f1_d7eecd483cf2 )
#define __i_methodinfo__dc5fba5e_8515_4d02_a0f1_d7eecd483cf2
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface comment,  )
// MethodInfo interface implementation
// Short comments -- interface method type information
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_METHODINFO  ((tIID)(59))
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
	typedef tUINT hMETHODINFO;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iMethodInfoVtbl;
typedef struct iMethodInfoVtbl iMethodInfoVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cMethodInfo;
	typedef cMethodInfo* hMETHODINFO;
#else
	typedef struct tag_hMETHODINFO {
		const iMethodInfoVtbl* vtbl; // pointer to the "MethodInfo" virtual table
		const iSYSTEMVtbl*     sys;  // pointer to the "SYSTEM" virtual table
	} *hMETHODINFO;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( MethodInfo_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef void (*fnpMethodInfo_dummy)( hMETHODINFO _this );


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iMethodInfoVtbl {
	fnpMethodInfo_dummy  dummy;
}; // "MethodInfo" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( MethodInfo_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
#else // if !defined( __cplusplus )
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_methodinfo__558f78e4_c771_11d4_b764_00d0b7170e50
// AVP Prague stamp end


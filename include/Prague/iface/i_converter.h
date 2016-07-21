// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:22 --------
// File Name   -- (null)i_converter.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_converter__a6826b3b_71f9_462a_9272_4a5818e432cc )
#define __i_converter__a6826b3b_71f9_462a_9272_4a5818e432cc
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// Converter interface implementation
// Short comments -- Converter interface
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_CONVERTER  ((tIID)(58017))
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
	typedef tUINT hCONVERTER;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iConverterVtbl;
typedef struct iConverterVtbl iConverterVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cConverter;
	typedef cConverter* hCONVERTER;
#else
	typedef struct tag_hCONVERTER {
		const iConverterVtbl* vtbl; // pointer to the "Converter" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hCONVERTER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Converter_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpConverter_Convert) ( hCONVERTER _this, hOBJECT* result, hOBJECT hInputObj );    // -- Преобразовывает поток данных;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iConverterVtbl {
	fnpConverter_Convert  Convert;
}; // "Converter" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Converter_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Converter_Convert( _this, result, hInputObj )       ((_this)->vtbl->Convert( (_this), result, hInputObj ))
#else // if !defined( __cplusplus )
	#define CALL_Converter_Convert( _this, result, hInputObj )       ((_this)->Convert( result, hInputObj ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iConverter {
		virtual tERROR pr_call Convert( hOBJECT* result, hOBJECT hInputObj ) = 0; // -- Преобразовывает поток данных
	};

	struct pr_abstract cConverter : public iConverter, public iObj {

		OBJ_IMPL( cConverter );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hCONVERTER()   { return (hCONVERTER)this; }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_converter__33151549_9f83_4204_9f99_140e968173ee
// AVP Prague stamp end


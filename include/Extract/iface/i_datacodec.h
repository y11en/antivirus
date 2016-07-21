// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  10 February 2005,  19:00 --------
// File Name   -- (null)i_datacodec.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_datacodec__f1b42afa_f3ea_4e12_8ba4_b91c44b54f26 )
#define __i_datacodec__f1b42afa_f3ea_4e12_8ba4_b91c44b54f26
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// DataCodec interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_DATACODEC  ((tIID)(58025))
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
	typedef tUINT hDATACODEC;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iDataCodecVtbl;
typedef struct iDataCodecVtbl iDataCodecVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cDataCodec;
	typedef cDataCodec* hDATACODEC;
#else
	typedef struct tag_hDATACODEC {
		const iDataCodecVtbl* vtbl; // pointer to the "DataCodec" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hDATACODEC;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( DataCodec_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpDataCodec_Encode) ( hDATACODEC _this, tDWORD* result, const tBYTE* pDecodedData, tDWORD nDataSize, tBYTE* pEncodedBuffer, tDWORD nBufferSize ); // -- ;
	typedef   tERROR (pr_call *fnpDataCodec_Decode) ( hDATACODEC _this, tDWORD* result, const tBYTE* pEncodedData, tDWORD nDataSize, tBYTE* pDecodedBuffer, tDWORD nBufferSize ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iDataCodecVtbl {
	fnpDataCodec_Encode  Encode;
	fnpDataCodec_Decode  Decode;
}; // "DataCodec" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( DataCodec_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_DataCodec_Encode( _this, result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize )               ((_this)->vtbl->Encode( (_this), result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize ))
	#define CALL_DataCodec_Decode( _this, result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize )               ((_this)->vtbl->Decode( (_this), result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize ))
#else // if !defined( __cplusplus )
	#define CALL_DataCodec_Encode( _this, result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize )               ((_this)->Encode( result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize ))
	#define CALL_DataCodec_Decode( _this, result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize )               ((_this)->Decode( result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iDataCodec {
		virtual tERROR pr_call Encode( tDWORD* result, const tBYTE* pDecodedData, tDWORD nDataSize, tBYTE* pEncodedBuffer, tDWORD nBufferSize ) = 0;
		virtual tERROR pr_call Decode( tDWORD* result, const tBYTE* pEncodedData, tDWORD nDataSize, tBYTE* pDecodedBuffer, tDWORD nBufferSize ) = 0;
	};

	struct pr_abstract cDataCodec : public iDataCodec, public iObj {
		
		OBJ_IMPL( cDataCodec );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hDATACODEC()   { return (hDATACODEC)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_datacodec__f1b42afa_f3ea_4e12_8ba4_b91c44b54f26
// AVP Prague stamp end




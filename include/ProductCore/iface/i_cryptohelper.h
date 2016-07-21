// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  06 December 2006,  19:10 --------
// File Name   -- (null)i_cryptohelper.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_cryptohelper__2747cfb9_b156_43d3_a8f1_f9d3d6cb17bd )
#define __i_cryptohelper__2747cfb9_b156_43d3_a8f1_f9d3d6cb17bd
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// CryptoHelper interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_CRYPTOHELPER  ((tIID)(65001))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define PSWD_DECRYPT                   ((tDWORD)(1)) //  --
#define PSWD_SIMPLE                    ((tDWORD)(2)) //  --
#define PSWD_RECRYPT                   ((tDWORD)(4)) //  --
#define CRHLP_PERSISTENT_DATA_ID       ((tDWORD)(10000)) // ѕол€, имеющие data_id начина€ с этого, доступны в любой версии продукта
// AVP Prague stamp end




// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hCRYPTOHELPER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iCryptoHelperVtbl;
typedef struct iCryptoHelperVtbl iCryptoHelperVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cCryptoHelper;
	typedef cCryptoHelper* hCRYPTOHELPER;
#else
	typedef struct tag_hCRYPTOHELPER 
	{
		const iCryptoHelperVtbl* vtbl; // pointer to the "CryptoHelper" virtual table
		const iSYSTEMVtbl*       sys;  // pointer to the "SYSTEM" virtual table
	} *hCRYPTOHELPER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( CryptoHelper_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpCryptoHelper_Encode)        ( hCRYPTOHELPER _this, tDWORD* result, const tBYTE* pDecodedData, tDWORD nDataSize, tBYTE* pEncodedBuffer, tDWORD nBufferSize ); // -- ;
	typedef   tERROR (pr_call *fnpCryptoHelper_Decode)        ( hCRYPTOHELPER _this, tDWORD* result, const tBYTE* pEncodedData, tDWORD nDataSize, tBYTE* pDecodedBuffer, tDWORD nBufferSize ); // -- ;
	typedef   tERROR (pr_call *fnpCryptoHelper_CryptPassword) ( hCRYPTOHELPER _this, cSerString* password, tDWORD flags ); // -- ;
	typedef   tERROR (pr_call *fnpCryptoHelper_IsMSTrusted)   ( hCRYPTOHELPER _this, const tWCHAR* file_name ); // -- ;
	typedef   tERROR (pr_call *fnpCryptoHelper_StoreData)     ( hCRYPTOHELPER _this, tDWORD data_id, tPTR data, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpCryptoHelper_GetData)       ( hCRYPTOHELPER _this, tDWORD data_id, tPTR data, tDWORD* size ); // -- ;
	typedef   tERROR (pr_call *fnpCryptoHelper_GetOldData)    ( hCRYPTOHELPER _this, tDWORD data_id, tPTR data, tDWORD* size ); // -- ¬озвращает значение пол€ дл€ старой версии продукта, если такое существует;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iCryptoHelperVtbl 
{
	fnpCryptoHelper_Encode         Encode;
	fnpCryptoHelper_Decode         Decode;
	fnpCryptoHelper_CryptPassword  CryptPassword;
	fnpCryptoHelper_IsMSTrusted    IsMSTrusted;
	fnpCryptoHelper_StoreData      StoreData;
	fnpCryptoHelper_GetData        GetData;
	fnpCryptoHelper_GetOldData     GetOldData;
}; // "CryptoHelper" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( CryptoHelper_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgPRODUCT_VERSION   mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_CryptoHelper_Encode( _this, result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize )                  ((_this)->vtbl->Encode( (_this), result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize ))
	#define CALL_CryptoHelper_Decode( _this, result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize )                  ((_this)->vtbl->Decode( (_this), result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize ))
	#define CALL_CryptoHelper_CryptPassword( _this, password, flags )                                                        ((_this)->vtbl->CryptPassword( (_this), password, flags ))
	#define CALL_CryptoHelper_IsMSTrusted( _this, file_name )                                                                ((_this)->vtbl->IsMSTrusted( (_this), file_name ))
	#define CALL_CryptoHelper_StoreData( _this, data_id, data, size )                                                        ((_this)->vtbl->StoreData( (_this), data_id, data, size ))
	#define CALL_CryptoHelper_GetData( _this, data_id, data, size )                                                          ((_this)->vtbl->GetData( (_this), data_id, data, size ))
	#define CALL_CryptoHelper_GetOldData( _this, data_id, data, size )                                                       ((_this)->vtbl->GetOldData( (_this), data_id, data, size ))
#else // if !defined( __cplusplus )
	#define CALL_CryptoHelper_Encode( _this, result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize )                  ((_this)->Encode( result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize ))
	#define CALL_CryptoHelper_Decode( _this, result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize )                  ((_this)->Decode( result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize ))
	#define CALL_CryptoHelper_CryptPassword( _this, password, flags )                                                        ((_this)->CryptPassword( password, flags ))
	#define CALL_CryptoHelper_IsMSTrusted( _this, file_name )                                                                ((_this)->IsMSTrusted( file_name ))
	#define CALL_CryptoHelper_StoreData( _this, data_id, data, size )                                                        ((_this)->StoreData( data_id, data, size ))
	#define CALL_CryptoHelper_GetData( _this, data_id, data, size )                                                          ((_this)->GetData( data_id, data, size ))
	#define CALL_CryptoHelper_GetOldData( _this, data_id, data, size )                                                       ((_this)->GetOldData( data_id, data, size ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iCryptoHelper 
	{
		virtual tERROR pr_call Encode( tDWORD* result, const tBYTE* pDecodedData, tDWORD nDataSize, tBYTE* pEncodedBuffer, tDWORD nBufferSize ) = 0;
		virtual tERROR pr_call Decode( tDWORD* result, const tBYTE* pEncodedData, tDWORD nDataSize, tBYTE* pDecodedBuffer, tDWORD nBufferSize ) = 0;
		virtual tERROR pr_call CryptPassword( cSerString* password, tDWORD flags ) = 0;
		virtual tERROR pr_call IsMSTrusted( const tWCHAR* file_name ) = 0;
		virtual tERROR pr_call StoreData( tDWORD data_id, tPTR data, tDWORD size ) = 0;
		virtual tERROR pr_call GetData( tDWORD data_id, tPTR data, tDWORD* size ) = 0;
		virtual tERROR pr_call GetOldData( tDWORD data_id, tPTR data, tDWORD* size ) = 0; // -- ¬озвращает значение пол€ дл€ старой версии продукта, если такое существует
	};

	struct pr_abstract cCryptoHelper : public iCryptoHelper, public iObj 
	{

		OBJ_IMPL( cCryptoHelper );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hCRYPTOHELPER()   { return (hCRYPTOHELPER)this; }

		tERROR pr_call get_pgPRODUCT_VERSION( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgPRODUCT_VERSION,buff,size,cp); }
		tERROR pr_call set_pgPRODUCT_VERSION( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgPRODUCT_VERSION,buff,size,cp); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_cryptohelper__2747cfb9_b156_43d3_a8f1_f9d3d6cb17bd
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(CryptoHelper_PROXY_DEFINITION)
#define CryptoHelper_PROXY_DEFINITION

PR_PROXY_BEGIN(CryptoHelper)
	PR_PROXY(CryptoHelper, Encode,        PR_ARG3(tid_DWORD|tid_POINTER,tid_BINARY|prf_IN,tid_BINARY|prf_OUT))
	PR_PROXY(CryptoHelper, Decode,        PR_ARG3(tid_DWORD|tid_POINTER,tid_BINARY|prf_IN,tid_BINARY|prf_OUT))
	PR_PROXY(CryptoHelper, CryptPassword, PR_ARG2(tid_SERIALIZABLE,tid_DWORD))
	PR_SCPRX(CryptoHelper, IsMSTrusted,   PR_ARG1(tid_WSTRING))
	PR_PROXY(CryptoHelper, StoreData,     PR_ARG2(tid_DWORD,tid_BINARY|prf_IN))
	PR_PROXY(CryptoHelper, GetData,       PR_ARG2(tid_DWORD,tid_BINARY|prf_OUT|prf_SIZEPTR))
PR_PROXY_END(CryptoHelper, IID_CRYPTOHELPER)

#endif // __PROXY_DECLARATION

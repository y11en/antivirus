// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  18 October 2007,  15:24 --------
// File Name   -- (null)i_addressbookextractor.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_addressbookextractor__6476492c_d007_427f_842a_f6d836c93cb0 )
#define __i_addressbookextractor__6476492c_d007_427f_842a_f6d836c93cb0
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end



#include <Mail/structs/s_addressbookextract.h>

// AVP Prague stamp begin( Interface comment,  )
// AddressBookExtractor interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_ADDRESSBOOKEXTRACTOR  ((tIID)(56016))
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
	typedef tUINT hADDRESSBOOKEXTRACTOR;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAddressBookExtractorVtbl;
typedef struct iAddressBookExtractorVtbl iAddressBookExtractorVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAddressBookExtractor;
	typedef cAddressBookExtractor* hADDRESSBOOKEXTRACTOR;
#else
	typedef struct tag_hADDRESSBOOKEXTRACTOR {
		const iAddressBookExtractorVtbl* vtbl; // pointer to the "AddressBookExtractor" virtual table
		const iSYSTEMVtbl*               sys;  // pointer to the "SYSTEM" virtual table
	} *hADDRESSBOOKEXTRACTOR;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AddressBookExtractor_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAddressBookExtractor_IsEnabled)        ( hADDRESSBOOKEXTRACTOR _this, tBOOL* pbEnabled ); // -- ;
	typedef   tERROR (pr_call *fnpAddressBookExtractor_GetFolderName)    ( hADDRESSBOOKEXTRACTOR _this, tSIZE_T hFolder, hSTRING name ); // -- ;
	typedef   tERROR (pr_call *fnpAddressBookExtractor_GetChildFolders)  ( hADDRESSBOOKEXTRACTOR _this, tSIZE_T hFolder, cSerializable* pFolders ); // -- ;
	typedef   tERROR (pr_call *fnpAddressBookExtractor_GetFolderContent) ( hADDRESSBOOKEXTRACTOR _this, tSIZE_T hFolder, cSerializable* pAddresses ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAddressBookExtractorVtbl {
	fnpAddressBookExtractor_IsEnabled         IsEnabled;
	fnpAddressBookExtractor_GetFolderName     GetFolderName;
	fnpAddressBookExtractor_GetChildFolders   GetChildFolders;
	fnpAddressBookExtractor_GetFolderContent  GetFolderContent;
}; // "AddressBookExtractor" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AddressBookExtractor_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AddressBookExtractor_IsEnabled( _this, pbEnabled )                                           ((_this)->vtbl->IsEnabled( (_this), pbEnabled ))
	#define CALL_AddressBookExtractor_GetFolderName( _this, hFolder, name )                                   ((_this)->vtbl->GetFolderName( (_this), hFolder, name ))
	#define CALL_AddressBookExtractor_GetChildFolders( _this, hFolder, pFolders )                             ((_this)->vtbl->GetChildFolders( (_this), hFolder, pFolders ))
	#define CALL_AddressBookExtractor_GetFolderContent( _this, hFolder, pAddresses )                          ((_this)->vtbl->GetFolderContent( (_this), hFolder, pAddresses ))
#else // if !defined( __cplusplus )
	#define CALL_AddressBookExtractor_IsEnabled( _this, pbEnabled )                                           ((_this)->IsEnabled( pbEnabled ))
	#define CALL_AddressBookExtractor_GetFolderName( _this, hFolder, name )                                   ((_this)->GetFolderName( hFolder, name ))
	#define CALL_AddressBookExtractor_GetChildFolders( _this, hFolder, pFolders )                             ((_this)->GetChildFolders( hFolder, pFolders ))
	#define CALL_AddressBookExtractor_GetFolderContent( _this, hFolder, pAddresses )                          ((_this)->GetFolderContent( hFolder, pAddresses ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAddressBookExtractor {
		virtual tERROR pr_call IsEnabled( tBOOL* pbEnabled ) = 0;
		virtual tERROR pr_call GetFolderName( tSIZE_T hFolder, hSTRING name ) = 0;
		virtual tERROR pr_call GetChildFolders( tSIZE_T hFolder, cSerializable* pFolders ) = 0;
		virtual tERROR pr_call GetFolderContent( tSIZE_T hFolder, cSerializable* pAddresses ) = 0;
	};

	struct pr_abstract cAddressBookExtractor : public iAddressBookExtractor, public iObj {

		OBJ_IMPL( cAddressBookExtractor );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hADDRESSBOOKEXTRACTOR()   { return (hADDRESSBOOKEXTRACTOR)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_addressbookextractor__6476492c_d007_427f_842a_f6d836c93cb0
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:21 --------
// File Name   -- (null)i_hashct.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_hashct__d2405400_085b_4752_a64b_64313128b421 )
#define __i_hashct__d2405400_085b_4752_a64b_64313128b421
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// HashContainer interface implementation
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_HASHCONTAINER  ((tIID)(63010))
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
	typedef tUINT hHASHCONTAINER;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iHashContainerVtbl;
typedef struct iHashContainerVtbl iHashContainerVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cHashContainer;
	typedef cHashContainer* hHASHCONTAINER;
#else
	typedef struct tag_hHASHCONTAINER {
		const iHashContainerVtbl* vtbl; // pointer to the "HashContainer" virtual table
		const iSYSTEMVtbl*        sys;  // pointer to the "SYSTEM" virtual table
	} *hHASHCONTAINER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( HashContainer_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpHashContainer_GetHash)    ( hHASHCONTAINER _this, tQWORD qwHash, tPTR* ppData ); // -- Hash Value;
	typedef   tERROR (pr_call *fnpHashContainer_SetHash)    ( hHASHCONTAINER _this, tQWORD qwHash, tPTR pData ); // -- ;
	typedef   tERROR (pr_call *fnpHashContainer_DeleteHash) ( hHASHCONTAINER _this, tQWORD qwHash ); // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iHashContainerVtbl {
	fnpHashContainer_GetHash     GetHash;
	fnpHashContainer_SetHash     SetHash;
	fnpHashContainer_DeleteHash  DeleteHash;
}; // "HashContainer" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( HashContainer_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_HashContainer_GetHash( _this, qwHash, ppData )                   ((_this)->vtbl->GetHash( (_this), qwHash, ppData ))
	#define CALL_HashContainer_SetHash( _this, qwHash, pData )                    ((_this)->vtbl->SetHash( (_this), qwHash, pData ))
	#define CALL_HashContainer_DeleteHash( _this, qwHash )                        ((_this)->vtbl->DeleteHash( (_this), qwHash ))
#else // if !defined( __cplusplus )
	#define CALL_HashContainer_GetHash( _this, qwHash, ppData )                   ((_this)->GetHash( qwHash, ppData ))
	#define CALL_HashContainer_SetHash( _this, qwHash, pData )                    ((_this)->SetHash( qwHash, pData ))
	#define CALL_HashContainer_DeleteHash( _this, qwHash )                        ((_this)->DeleteHash( qwHash ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iHashContainer {
		virtual tERROR pr_call GetHash( tQWORD qwHash, tPTR* ppData ) = 0; // -- Hash Value
		virtual tERROR pr_call SetHash( tQWORD qwHash, tPTR pData ) = 0;
		virtual tERROR pr_call DeleteHash( tQWORD qwHash ) = 0;
	};

	struct pr_abstract cHashContainer : public iHashContainer, public iObj {

		OBJ_IMPL( cHashContainer );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hHASHCONTAINER()   { return (hHASHCONTAINER)this; }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_hashct__b3efd949_3d71_4cd2_9f4f_ba4373d13ff5
// AVP Prague stamp end


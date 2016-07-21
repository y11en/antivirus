// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:20 --------
// File Name   -- (null)i_hash.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_hash__ec5b97c2_6750_427b_bcfb_48a22e281e08 )
#define __i_hash__ec5b97c2_6750_427b_bcfb_48a22e281e08
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// Hash interface implementation
// Short comments -- Calculates hash for data
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_HASH  ((tIID)(58004))
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
	typedef tUINT hHASH;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iHashVtbl;
typedef struct iHashVtbl iHashVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cHash;
	typedef cHash* hHASH;
#else
	typedef struct tag_hHASH {
		const iHashVtbl*   vtbl; // pointer to the "Hash" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hHASH;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Hash_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpHash_Reset)   ( hHASH _this );                            // -- Подготовить объект для подсчета нового хэша;
	typedef   tERROR (pr_call *fnpHash_Update)  ( hHASH _this, tBYTE* pData, tDWORD dwSize ); // -- Обсчитать очередной блок данных;
	typedef   tERROR (pr_call *fnpHash_GetHash) ( hHASH _this, tBYTE* pHash, tDWORD dwSize ); // -- Взять значание хеша для обработанных данных;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iHashVtbl {
	fnpHash_Reset    Reset;
	fnpHash_Update   Update;
	fnpHash_GetHash  GetHash;
}; // "Hash" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Hash_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgHASH_INIT_VALUE   mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x00001000 )
#define pgHASH_SIZE         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Hash_Reset( _this )                           ((_this)->vtbl->Reset( (_this) ))
	#define CALL_Hash_Update( _this, pData, dwSize )           ((_this)->vtbl->Update( (_this), pData, dwSize ))
	#define CALL_Hash_GetHash( _this, pHash, dwSize )          ((_this)->vtbl->GetHash( (_this), pHash, dwSize ))
#else // if !defined( __cplusplus )
	#define CALL_Hash_Reset( _this )                           ((_this)->Reset( ))
	#define CALL_Hash_Update( _this, pData, dwSize )           ((_this)->Update( pData, dwSize ))
	#define CALL_Hash_GetHash( _this, pHash, dwSize )          ((_this)->GetHash( pHash, dwSize ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iHash {
		virtual tERROR pr_call Reset() = 0; // -- Подготовить объект для подсчета нового хэша
		virtual tERROR pr_call Update( tBYTE* pData, tDWORD dwSize ) = 0; // -- Обсчитать очередной блок данных
		virtual tERROR pr_call GetHash( tBYTE* pHash, tDWORD dwSize ) = 0; // -- Взять значание хеша для обработанных данных
	};

	struct pr_abstract cHash : public iHash, public iObj {

		OBJ_IMPL( cHash );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hHASH()   { return (hHASH)this; }

		tERROR pr_call get_pgHASH_INIT_VALUE( tPTR value, tDWORD size ) { return get(pgHASH_INIT_VALUE,value,size); }
		tERROR pr_call set_pgHASH_INIT_VALUE( tPTR value, tDWORD size ) { return set(pgHASH_INIT_VALUE,value,size); }

		tDWORD pr_call get_pgHASH_SIZE() { return (tDWORD)getDWord(pgHASH_SIZE); }
		tERROR pr_call set_pgHASH_SIZE( tDWORD value ) { return setDWord(pgHASH_SIZE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_hash__fa04b86f_c794_40ab_b33b_b5a02617886a
// AVP Prague stamp end


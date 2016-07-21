// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:17 --------
// File Name   -- (null)i_lru.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_lru__b11dcecc_49e9_4bde_bac7_fdb87a002e1b )
#define __i_lru__b11dcecc_49e9_4bde_bac7_fdb87a002e1b
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// LRU interface implementation
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_LRU  ((tIID)(64011))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tLRU_MODE; //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define cLRU_RDONLY                    ((tLRU_MODE)(0)) //  --
#define cLRU_RDWR_THROUGH              ((tLRU_MODE)(1)) //  --
#define cLRU_RDWR_BACK                 ((tLRU_MODE)(2)) //  --
#define cLRU_RDWR_DELAY                ((tLRU_MODE)(3)) //  --
#define cLRU_ERROR                     ((tDWORD)(0xFFFFFFFF)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hLRU;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iLRUVtbl;
typedef struct iLRUVtbl iLRUVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cLRU;
	typedef cLRU* hLRU;
#else
	typedef struct tag_hLRU {
		const iLRUVtbl*    vtbl; // pointer to the "LRU" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hLRU;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( LRU_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpLRU_Lock)   ( hLRU _this, tDWORD* result, tDWORD ext, tBOOL write );     // -- ;
	typedef   tERROR (pr_call *fnpLRU_Unlock) ( hLRU _this, tDWORD ext );                  // -- ;
	typedef   tERROR (pr_call *fnpLRU_Flush)  ( hLRU _this );                              // -- ;
	typedef   tERROR (pr_call *fnpLRU_Clear)  ( hLRU _this );                              // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iLRUVtbl {
	fnpLRU_Lock    Lock;
	fnpLRU_Unlock  Unlock;
	fnpLRU_Flush   Flush;
	fnpLRU_Clear   Clear;
}; // "LRU" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( LRU_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgLRU_SIZE          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgLRU_MODE          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgLRU_WRITE_ERROR   mPROPERTY_MAKE_GLOBAL( pTYPE_ERROR   , 0x00001002 )
#define pgLRU_HITS          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgLRU_MISSES        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_LRU_Lock( _this, result, ext, write ) ((_this)->vtbl->Lock( (_this), result, ext, write ))
	#define CALL_LRU_Unlock( _this, ext )              ((_this)->vtbl->Unlock( (_this), ext ))
	#define CALL_LRU_Flush( _this )                    ((_this)->vtbl->Flush( (_this) ))
	#define CALL_LRU_Clear( _this )                    ((_this)->vtbl->Clear( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_LRU_Lock( _this, result, ext, write ) ((_this)->Lock( result, ext, write ))
	#define CALL_LRU_Unlock( _this, ext )              ((_this)->Unlock( ext ))
	#define CALL_LRU_Flush( _this )                    ((_this)->Flush( ))
	#define CALL_LRU_Clear( _this )                    ((_this)->Clear( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iLRU {
		virtual tERROR pr_call Lock( tDWORD* result, tDWORD ext, tBOOL write ) = 0;
		virtual tERROR pr_call Unlock( tDWORD ext ) = 0;
		virtual tERROR pr_call Flush() = 0;
		virtual tERROR pr_call Clear() = 0;
	};

	struct pr_abstract cLRU : public iLRU, public iObj {

		OBJ_IMPL( cLRU );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hLRU()   { return (hLRU)this; }

		tDWORD pr_call get_pgLRU_SIZE() { return (tDWORD)getDWord(pgLRU_SIZE); }
		tERROR pr_call set_pgLRU_SIZE( tDWORD value ) { return setDWord(pgLRU_SIZE,(tDWORD)value); }

		tLRU_MODE pr_call get_pgLRU_MODE() { tLRU_MODE value = {0}; get(pgLRU_MODE,&value,sizeof(value)); return value; }
		tERROR pr_call set_pgLRU_MODE( tLRU_MODE value ) { return set(pgLRU_MODE,&value,sizeof(value)); }

		tERROR pr_call get_pgLRU_WRITE_ERROR() { return (tERROR)getDWord(pgLRU_WRITE_ERROR); }
		tERROR pr_call set_pgLRU_WRITE_ERROR( tERROR value ) { return setDWord(pgLRU_WRITE_ERROR,(tDWORD)value); }

		tDWORD pr_call get_pgLRU_HITS() { return (tDWORD)getDWord(pgLRU_HITS); }
		tERROR pr_call set_pgLRU_HITS( tDWORD value ) { return setDWord(pgLRU_HITS,(tDWORD)value); }

		tDWORD pr_call get_pgLRU_MISSES() { return (tDWORD)getDWord(pgLRU_MISSES); }
		tERROR pr_call set_pgLRU_MISSES( tDWORD value ) { return setDWord(pgLRU_MISSES,(tDWORD)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_lru__37b141e5_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end


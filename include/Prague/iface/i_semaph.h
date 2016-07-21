// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  10 March 2004,  14:13 --------
// File Name   -- (null)i_semaph.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_semaph__d8f8056f_d49c_4ec7_ae33_1ef374b74917 )
#define __i_semaph__d8f8056f_d49c_4ec7_ae33_1ef374b74917
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Semaphore interface implementation
// Short comments -- Semaphore behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_SEMAPHORE  ((tIID)(15))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSEMAPHORE;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iSemaphoreVtbl;
typedef struct iSemaphoreVtbl iSemaphoreVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cSemaphore;
	typedef cSemaphore* hSEMAPHORE;
#else
	typedef struct tag_hSEMAPHORE 
	{
		const iSemaphoreVtbl* vtbl; // pointer to the "Semaphore" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hSEMAPHORE;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Semaphore_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpSemaphore_Lock)      ( hSEMAPHORE _this, tDWORD timeout );     // -- locks synchronization object;
	typedef   tERROR (pr_call *fnpSemaphore_Release)   ( hSEMAPHORE _this );                     // -- unlocks synchronization object;
	typedef   tERROR (pr_call *fnpSemaphore_ReleaseEx) ( hSEMAPHORE _this, tDWORD count );       // -- unlocks synchronization object by indicated count;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iSemaphoreVtbl 
{
	fnpSemaphore_Lock       Lock;
	fnpSemaphore_Release    Release;
	fnpSemaphore_ReleaseEx  ReleaseEx;
}; // "Semaphore" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Semaphore_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
#define pgJUST_OPEN         mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgBAND_WIDTH        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgINITIAL_COUNT     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgCURRENT_COUNT     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
#define pgLAST_OS_ERROR     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgGLOBAL            mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001005 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Semaphore_Lock( _this, timeout )                  ((_this)->vtbl->Lock( (_this), timeout ))
	#define CALL_Semaphore_Release( _this )                        ((_this)->vtbl->Release( (_this) ))
	#define CALL_Semaphore_ReleaseEx( _this, count )               ((_this)->vtbl->ReleaseEx( (_this), count ))
#else // if !defined( __cplusplus )
	#define CALL_Semaphore_Lock( _this, timeout )                  ((_this)->Lock( timeout ))
	#define CALL_Semaphore_Release( _this )                        ((_this)->Release( ))
	#define CALL_Semaphore_ReleaseEx( _this, count )               ((_this)->ReleaseEx( count ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iSemaphore 
	{
		virtual tERROR pr_call Lock( tDWORD timeout ) = 0; // -- locks synchronization object
		virtual tERROR pr_call Release() = 0; // -- unlocks synchronization object
		virtual tERROR pr_call ReleaseEx( tDWORD count ) = 0; // -- unlocks synchronization object by indicated count
	};

	struct pr_abstract cSemaphore : public iSemaphore, public iObj {

		OBJ_IMPL( cSemaphore );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSEMAPHORE()   { return (hSEMAPHORE)this; }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tBOOL pr_call get_pgJUST_OPEN() { return (tBOOL)getBool(pgJUST_OPEN); }
		tERROR pr_call set_pgJUST_OPEN( tBOOL value ) { return setBool(pgJUST_OPEN,(tBOOL)value); }

		tDWORD pr_call get_pgBAND_WIDTH() { return (tDWORD)getDWord(pgBAND_WIDTH); }
		tERROR pr_call set_pgBAND_WIDTH( tDWORD value ) { return setDWord(pgBAND_WIDTH,(tDWORD)value); }

		tDWORD pr_call get_pgINITIAL_COUNT() { return (tDWORD)getDWord(pgINITIAL_COUNT); }
		tERROR pr_call set_pgINITIAL_COUNT( tDWORD value ) { return setDWord(pgINITIAL_COUNT,(tDWORD)value); }

		tDWORD pr_call get_pgCURRENT_COUNT() { return (tDWORD)getDWord(pgCURRENT_COUNT); }
		tERROR pr_call set_pgCURRENT_COUNT( tDWORD value ) { return setDWord(pgCURRENT_COUNT,(tDWORD)value); }

		tDWORD pr_call get_pgLAST_OS_ERROR() { return (tDWORD)getDWord(pgLAST_OS_ERROR); }
		tERROR pr_call set_pgLAST_OS_ERROR( tDWORD value ) { return setDWord(pgLAST_OS_ERROR,(tDWORD)value); }

		tBOOL pr_call get_pgGLOBAL() { return (tBOOL)getBool(pgGLOBAL); }
		tERROR pr_call set_pgGLOBAL( tBOOL value ) { return setBool(pgGLOBAL,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_semaph__46da2c7f_e864_4534_80cc_187414b6f168
// AVP Prague stamp end




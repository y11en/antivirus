// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  10 March 2004,  14:13 --------
// File Name   -- (null)i_mutex.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_mutex__a20ceeda_1049_4580_894d_cb17e96a36dc )
#define __i_mutex__a20ceeda_1049_4580_894d_cb17e96a36dc
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Mutex interface implementation
// Short comments -- Mutex behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_MUTEX  ((tIID)(14))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )

// ----------  timeout constant  ----------
#define cSYNC_INFINITE                 ((tDWORD)(((tDWORD)-1))) //  --
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
	typedef tUINT hMUTEX;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iMutexVtbl;
typedef struct iMutexVtbl iMutexVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cMutex;
	typedef cMutex* hMUTEX;
#else
	typedef struct tag_hMUTEX 
	{
		const iMutexVtbl*  vtbl; // pointer to the "Mutex" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hMUTEX;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Mutex_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpMutex_Lock)    ( hMUTEX _this, tDWORD timeout );        // -- locks synchronization object;
	typedef   tERROR (pr_call *fnpMutex_Release) ( hMUTEX _this );                        // -- unlocks synchronization object;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iMutexVtbl 
{
	fnpMutex_Lock     Lock;
	fnpMutex_Release  Release;
}; // "Mutex" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Mutex_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
#define pgJUST_OPEN         mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgINITIAL_OWNER     mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001001 )
#define pgLAST_OS_ERROR     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgGLOBAL            mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001005 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Mutex_Lock( _this, timeout )           ((_this)->vtbl->Lock( (_this), timeout ))
	#define CALL_Mutex_Release( _this )                 ((_this)->vtbl->Release( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_Mutex_Lock( _this, timeout )           ((_this)->Lock( timeout ))
	#define CALL_Mutex_Release( _this )                 ((_this)->Release( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iMutex 
	{
		virtual tERROR pr_call Lock( tDWORD timeout ) = 0; // -- locks synchronization object
		virtual tERROR pr_call Release() = 0; // -- unlocks synchronization object
	};

	struct pr_abstract cMutex : public iMutex, public iObj {

		OBJ_IMPL( cMutex );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hMUTEX()   { return (hMUTEX)this; }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tBOOL pr_call get_pgJUST_OPEN() { return (tBOOL)getBool(pgJUST_OPEN); }
		tERROR pr_call set_pgJUST_OPEN( tBOOL value ) { return setBool(pgJUST_OPEN,(tBOOL)value); }

		tBOOL pr_call get_pgINITIAL_OWNER() { return (tBOOL)getBool(pgINITIAL_OWNER); }
		tERROR pr_call set_pgINITIAL_OWNER( tBOOL value ) { return setBool(pgINITIAL_OWNER,(tBOOL)value); }

		tDWORD pr_call get_pgLAST_OS_ERROR() { return (tDWORD)getDWord(pgLAST_OS_ERROR); }
		tERROR pr_call set_pgLAST_OS_ERROR( tDWORD value ) { return setDWord(pgLAST_OS_ERROR,(tDWORD)value); }

		tBOOL pr_call get_pgGLOBAL() { return (tBOOL)getBool(pgGLOBAL); }
		tERROR pr_call set_pgGLOBAL( tBOOL value ) { return setBool(pgGLOBAL,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_mutex__c49dc111_4515_470c_bc1f_fe566b652da2
// AVP Prague stamp end




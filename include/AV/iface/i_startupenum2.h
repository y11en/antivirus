// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  22 December 2005,  18:49 --------
// File Name   -- (null)i_startupenum2.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_startupenum2__515c3e08_18f1_4786_84c1_83c8db7f421b )
#define __i_startupenum2__515c3e08_18f1_4786_84c1_83c8db7f421b
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// StartUpEnum2 interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_STARTUPENUM2  ((tIID)(54001))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_STARTUPENUM2 0x5d55e35e //

	#define mc_FILE_FOUNDED 0x00001000 // (4096) --

	#define mc_ERROR_OPERATION 0x00001001 // (4097) --

	#define mc_FILE_TO_DELETE 0x00001002 // (4098) --

// message class
#define pmc_ADVANCED_DISINFECTION 0xbcef66f3 //

	#define pm_ADVANCED_DISINFECTION_START 0xb722df91 // (3072515985) --

	#define pm_ADVANCED_DISINFECTION_LOCK_REGKEY 0xb722df92 // (3072515986) --

	#define pm_ADVANCED_DISINFECTION_LOCK_FILE 0xb722df93 // (3072515987) --

	#define pm_ADVANCED_DISINFECTION_REBOOT 0xb722df94 // (3072515988) --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSTARTUPENUM2;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iStartUpEnum2Vtbl;
typedef struct iStartUpEnum2Vtbl iStartUpEnum2Vtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cStartUpEnum2;
	typedef cStartUpEnum2* hSTARTUPENUM2;
#else
	typedef struct tag_hSTARTUPENUM2 
	{
		const iStartUpEnum2Vtbl* vtbl; // pointer to the "StartUpEnum2" virtual table
		const iSYSTEMVtbl*       sys;  // pointer to the "SYSTEM" virtual table
	} *hSTARTUPENUM2;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( StartUpEnum2_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpStartUpEnum2_StartEnum) ( hSTARTUPENUM2 _this, hOBJECT pRetObject, tBOOL bAdvancedDisinfection ); // -- ;
	typedef   tERROR (pr_call *fnpStartUpEnum2_CleanUp)   ( hSTARTUPENUM2 _this, hSTRING DelFName, hOBJECT pRetObject, tSTRING DetectedVirusName ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iStartUpEnum2Vtbl 
{
	fnpStartUpEnum2_StartEnum  StartEnum;
	fnpStartUpEnum2_CleanUp    CleanUp;
}; // "StartUpEnum2" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( StartUpEnum2_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME       mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000040 )
//! #define pgOBJECT_PATH       mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000044 )
//! #define pgOBJECT_FULL_NAME  mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000041 )
//! #define pgPLUGIN_CODEPAGE   mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000c5 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_StartUpEnum2_StartEnum( _this, pRetObject, bAdvancedDisinfection )                      ((_this)->vtbl->StartEnum( (_this), pRetObject, bAdvancedDisinfection ))
	#define CALL_StartUpEnum2_CleanUp( _this, DelFName, pRetObject, DetectedVirusName )                  ((_this)->vtbl->CleanUp( (_this), DelFName, pRetObject, DetectedVirusName ))
#else // if !defined( __cplusplus )
	#define CALL_StartUpEnum2_StartEnum( _this, pRetObject, bAdvancedDisinfection )                      ((_this)->StartEnum( pRetObject, bAdvancedDisinfection ))
	#define CALL_StartUpEnum2_CleanUp( _this, DelFName, pRetObject, DetectedVirusName )                  ((_this)->CleanUp( DelFName, pRetObject, DetectedVirusName ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iStartUpEnum2 
	{
		virtual tERROR pr_call StartEnum( hOBJECT pRetObject, tBOOL bAdvancedDisinfection ) = 0;
		virtual tERROR pr_call CleanUp( hSTRING DelFName, hOBJECT pRetObject, tSTRING DetectedVirusName ) = 0;
	};

	struct pr_abstract cStartUpEnum2 : public iStartUpEnum2, public iObj {

		OBJ_IMPL( cStartUpEnum2 );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSTARTUPENUM2()   { return (hSTARTUPENUM2)this; }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_PATH,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_PATH,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_FULL_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_FULL_NAME,buff,size,cp); }

		tCODEPAGE pr_call get_pgPLUGIN_CODEPAGE() { return (tCODEPAGE)getDWord(pgPLUGIN_CODEPAGE); }
		tERROR pr_call set_pgPLUGIN_CODEPAGE( tCODEPAGE value ) { return setDWord(pgPLUGIN_CODEPAGE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_startupenum2__515c3e08_18f1_4786_84c1_83c8db7f421b
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  23 January 2004,  13:44 --------
// File Name   -- (null)i_timer.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_timer__355227ef_eb8d_4174_9fdc_744983796d63 )
#define __i_timer__355227ef_eb8d_4174_9fdc_744983796d63
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Timer interface implementation
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TIMER  ((tIID)(58016))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_TIMER 0x9d841caa // (2642681002) --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTIMER;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iTimerVtbl;
typedef struct iTimerVtbl iTimerVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cTimer;
	typedef cTimer* hTIMER;
#else
	typedef struct tag_hTIMER 
	{
		const iTimerVtbl*  vtbl; // pointer to the "Timer" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hTIMER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Timer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef void (*fnpTimer_dummy)( hTIMER _this );


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iTimerVtbl 
{
	fnpTimer_dummy  dummy;
}; // "Timer" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Timer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgFREQUENCY         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgTIMER_ID          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgENABLED           mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001002 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
#else // if !defined( __cplusplus )
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)

	struct pr_abstract cTimer : private iEmpty, public iObj {

		OBJ_IMPL( cTimer );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTIMER()   { return (hTIMER)this; }

		tDWORD pr_call get_pgFREQUENCY() { return (tDWORD)getDWord(pgFREQUENCY); }
		tERROR pr_call set_pgFREQUENCY( tDWORD value ) { return setDWord(pgFREQUENCY,(tDWORD)value); }

		tDWORD pr_call get_pgTIMER_ID() { return (tDWORD)getDWord(pgTIMER_ID); }
		tERROR pr_call set_pgTIMER_ID( tDWORD value ) { return setDWord(pgTIMER_ID,(tDWORD)value); }

		tBOOL pr_call get_pgENABLED() { return (tBOOL)getBool(pgENABLED); }
		tERROR pr_call set_pgENABLED( tBOOL value ) { return setBool(pgENABLED,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_timer__355227ef_eb8d_4174_9fdc_744983796d63
// AVP Prague stamp end




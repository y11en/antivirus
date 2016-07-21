// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  13:35 --------
// File Name   -- (null)i_receiv.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_receiv__de415920_ed74_4ab5_89ca_2769e5d3d261 )
#define __i_receiv__de415920_ed74_4ab5_89ca_2769e5d3d261
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_iface.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// MsgReceiver interface implementation
// Short comments -- special object for the registering as message receiver
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_MSG_RECEIVER  ((tIID)(9))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
#define pmc_MSG_RECEIVER 0x531fd7e3 //

	#define pm_MR_OBJECT_PRECLOSE 0x45fac119 // (1174061337) --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hMSG_RECEIVER;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iMsgReceiverVtbl;
typedef struct iMsgReceiverVtbl iMsgReceiverVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cMsgReceiver;
	typedef cMsgReceiver* hMSG_RECEIVER;
#else
	typedef struct tag_hMSG_RECEIVER {
		const iMsgReceiverVtbl* vtbl; // pointer to the "MsgReceiver" virtual table
		const iSYSTEMVtbl*      sys;  // pointer to the "SYSTEM" virtual table
	} *hMSG_RECEIVER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( MsgReceiver_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef void (*fnpMsgReceiver_dummy)( hMSG_RECEIVER _this );


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iMsgReceiverVtbl {
	fnpMsgReceiver_dummy  dummy;
}; // "MsgReceiver" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( MsgReceiver_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgRECEIVE_PROCEDURE mPROPERTY_MAKE_GLOBAL( pTYPE_FUNC_PTR, 0x00001000 )
#define pgRECEIVE_CLIENT_ID mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001001 )
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

	struct pr_abstract cMsgReceiver : private iEmpty, public iObj {

		OBJ_IMPL( cMsgReceiver );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hMSG_RECEIVER()   { return (hMSG_RECEIVER)this; }

		tFUNC_PTR pr_call get_pgRECEIVE_PROCEDURE() { return (tFUNC_PTR)getPtr(pgRECEIVE_PROCEDURE); }
		tERROR pr_call set_pgRECEIVE_PROCEDURE( tFUNC_PTR value ) { return setPtr(pgRECEIVE_PROCEDURE,(tPTR)value); }

		tPTR pr_call get_pgRECEIVE_CLIENT_ID() { return (tPTR)getPtr(pgRECEIVE_CLIENT_ID); }
		tERROR pr_call set_pgRECEIVE_CLIENT_ID( tPTR value ) { return setPtr(pgRECEIVE_CLIENT_ID,(tPTR)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_receiv__3d7e3465_03be_437c_9cad_2dee82a7f67d
// AVP Prague stamp end




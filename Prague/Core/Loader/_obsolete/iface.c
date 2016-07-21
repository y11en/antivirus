// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  01 October 2001,  14:07 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- iface.c
// Additional info
//  This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define IFace_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include "iface.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IFace interface implementation
// Short comments -- returns interface info
// Extended comment -
//  returns interface info:
//    -- interface name (optional)
//    -- interface types
//    -- interface constants
//    -- interface error codes
//    -- interface properties
//    -- count of interface external methods
//    -- interface external methods by index
//       -- return value type
//       -- count of parameters
//       -- parameter type by index
//    -- count of implemented interface internal methods
//    -- interface internal methods
//       -- return value type
//       -- count of parameters
//       -- parameter type by index
//
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
typedef struct tag_hi_IFace {
	const iIFaceVtbl*  vtbl; // pointer to the "IFace" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
} *hi_IFace;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef void (*fnpIFace_dummy)( hIFACE _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iIFaceVtbl {
	fnpIFace_dummy  dummy;
}; // "IFace" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IFace". Global(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "IFace". Property table
mPROPERTY_TABLE(IFace_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, 1 )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, can't extract default value, 26 )
mPROPERTY_TABLE_END(IFace_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "IFace". Register function
tERROR pr_call IFace_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter \"IFace::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IFACE,                              // interface identifier
		PID_WIN32_PLUGIN_LOADER,                // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,                             // interface version
		VID_PETROVITCH,                         // interface developer
		NULL,                                   // internal(kernel called) function table
		0,                                      // internal function table size
		NULL,                                   // external function table
		0,                                      // external function table size
		IFace_PropTable,                        // property table
		mPROPERTY_TABLE_SIZE(IFace_PropTable),  // property table size
		0,                                      // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( error != errOK )
			PR_TRACE( (root,prtDANGER,"IFace(IID_IFACE) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"IFace::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end




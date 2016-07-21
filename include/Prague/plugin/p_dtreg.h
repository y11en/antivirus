// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  26 April 2004,  20:09 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- AVP manager
// Sub project -- DTReg
// Purpose     -- secure registry unterface based on DataTree technology
// Author      -- petrovitch
// File Name   -- plugin_dtreg.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_dtreg )
#define __public_plugin_dtreg
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_DTREG  ((tPID)(7))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, Registry )
// Registry interface implementation
// Short comments -- access to storage like registry

// properties
#define plINIT_FROM_BASE       mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )
#define plCASE_SENSITIVE_NAMES mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002001 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_dtreg
// AVP Prague stamp end




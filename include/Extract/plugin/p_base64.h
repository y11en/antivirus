// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  11 February 2005,  11:39 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- plugin_base64.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_base64 )
#define __public_plugin_base64
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Extract/iface/i_datacodec.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_BASE64  ((tPID)(58051))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, DataCodec )
// DataCodec interface implementation

// properties
#define plALPHABET    mPROPERTY_MAKE_LOCAL( pTYPE_BINARY  , 0x00002000 )
#define plUSE_PADDING mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_base64
// AVP Prague stamp end




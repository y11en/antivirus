// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  26 December 2006,  15:17 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- plugin_licensing60.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_licensing60 )
#define __public_plugin_licensing60
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_licensing.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_LICENSING60  ((tPID)(38013))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_GUSCHIN  ((tVID)(90))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Licensing )
// Licensing interface implementation
// Short comments --

// properties
#define plUSES_BLACKLIST mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )

// message class
//! #define pmc_ONLINE_LICENSING 0x1e028874 //

// message class
//! #define pmc_LICENSING 0x6124e161 //
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_licensing60
// AVP Prague stamp end




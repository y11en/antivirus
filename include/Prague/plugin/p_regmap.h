// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  17 January 2006,  18:47 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- presents key of some registry as registry itself
// Author      -- Doukhvalow
// File Name   -- plugin_regmap.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_regmap )
#define __public_plugin_regmap
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_REGMAP  ((tPID)(36))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Registry )
// Registry interface implementation
// Short comments -- access to storage like registry

// properties
#define plPARENT_REG     mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002000 )
#define plPARENT_KEY     mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plPARENT_KEY_STR mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002002 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_regmap
// AVP Prague stamp end




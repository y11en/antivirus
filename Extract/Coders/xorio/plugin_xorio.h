// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  27 February 2004,  15:48 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Rubin
// File Name   -- plugin_xorio.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_xorio )
#define __pligin_xorio
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_XORIO  ((tPID)(46004))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_RUBIN  ((tVID)(82))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// properties
#define ppXOR_BYTE    mPROPERTY_MAKE_LOCAL( pTYPE_BYTE    , 0x00002000 )
#define ppXOR_ORIGIN  mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002001 )
#define ppXOR_KEY     mPROPERTY_MAKE_LOCAL( pTYPE_BINARY  , 0x00002002 )
#define ppXOR_KEYSIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002003 )

// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_xorio
// AVP Prague stamp end




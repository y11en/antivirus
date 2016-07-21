// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  26 November 2002,  12:32 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- LZX Decompressor
// Author      -- Andy Nikishin
// File Name   -- plugin_lzxdecompressor.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_lzxdecompressor )
#define __pligin_lzxdecompressor
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_LZXDECOMPRESSOR  ((tPID)(63027))
// AVP Prague stamp end

#define errDECOMPRESS                            PR_MAKE_IMP_ERR( PID_LZXDECOMPRESSOR, 0x001 )


// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end


// AVP Prague stamp begin( Property table,  )
#define plWINDOW_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plRUN_INPUT_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_lzxdecompressor
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  04 October 2002,  15:41 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Multi part DMAP wrapper
// Author      -- Andy Nikishin
// File Name   -- plugin_mdmap.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_mdmap )
#define __pligin_mdmap
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_MDMAP  ((tPID)(63025))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end


// AVP Prague stamp begin( Property table,  )
#define plMAP_AREA_START  mPROPERTY_MAKE_LOCAL( pTYPE_QWORD   , 0x00002001 )
#define plMAP_AREA_SIZE   mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
#define plMAP_AREA_ORIGIN mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002003 )
#define plMAP_PROP_FATHER mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002004 )
#define plMAP_RESET_DATA  mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002005 )
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_mdmap
// AVP Prague stamp end




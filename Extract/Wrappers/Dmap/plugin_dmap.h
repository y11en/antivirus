// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  04 October 2002,  15:41 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- DMAP wrapper
// Author      -- Andy Nikishin
// File Name   -- plugin_dmap.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_dmap )
#define __pligin_dmap
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define DMAP_ID  ((tPID)(0x0000f61c))
#define PID_DMAP ((tPID)(0x0000f61c))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end

#define ppMAP_AREA_START    mPROPERTY_MAKE_PRIVATE(pTYPE_QWORD  , 0x00002001)
#define ppMAP_AREA_SIZE     mPROPERTY_MAKE_PRIVATE(pTYPE_QWORD  , 0x00002002)
#define ppMAP_AREA_ORIGIN   mPROPERTY_MAKE_PRIVATE(pTYPE_OBJECT, 0x00002003)
#define ppMAP_PROP_FATHER   mPROPERTY_MAKE_PRIVATE(pTYPE_OBJECT  , 0x00002004)



// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_dmap
// AVP Prague stamp end




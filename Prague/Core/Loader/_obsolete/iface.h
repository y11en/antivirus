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
// File Name   -- iface.h
// Additional info
//  This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __iface_h__b43c8ad9_ad8b_47cd_a6a7_bcee8f83bdc2 )
#define __iface_h__b43c8ad9_ad8b_47cd_a6a7_bcee8f83bdc2
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <iface/i_root.h>
#include <iface/i_iface.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_WIN32_PLUGIN_LOADER  ((tPID)(5))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
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
// AVP Prague stamp begin( Registration call prototype,  )
#if defined( __cplusplus )
extern "C" {
#endif

	tERROR pr_call IFace_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // iface_h
// AVP Prague stamp end




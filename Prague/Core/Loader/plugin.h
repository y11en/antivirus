// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  25 March 2002,  19:12 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- plugin.h
// Additional info
//  This implementation finds loadable prague plugins in disk folder
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __plugin_h__1c5fc4ec_8db0_42c1_ade2_888b5135a6ab )
#define __plugin_h__1c5fc4ec_8db0_42c1_ade2_888b5135a6ab
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "mod_load.h"

#include <Prague/pr_loadr.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_plugin.h>

#if defined (OLD_METADATA_SUPPORT)
#include "../IFaceInfo/AVPPveID.h"
#include "prop_util.h"
#endif // OLD_METADATA_SUPPORT
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_WIN32_PLUGIN_LOADER  ((tPID)(5))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Plugin interface implementation
// Short comments -- interface of load/unload plugins
// Extended comment -
//  Supplies facilities :
//    -- to load/unload certain plugin
//    -- to get shared interface properties implemented by plugin
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
#if defined( __cplusplus )
extern "C" {
#endif

  tERROR pr_call Plugin_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )

  // data structure "ModuleDATA" is described in "D:\Prague\Loader\plugin.c" source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plMODULE_INSTANCE mPROPERTY_MAKE_LOCAL( pTYPE_PTR   , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // plugin_h
// AVP Prague stamp end




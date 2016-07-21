// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  07 October 2002,  16:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Multi part DMAP wrapper
// Author      -- Andy Nikishin
// File Name   -- mdmap.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __mdmap_h__3e8b02c8_d363_42b6_91ba_f32deecbf55a )
#define __mdmap_h__3e8b02c8_d363_42b6_91ba_f32deecbf55a
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_mdmap.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/iface/m_dmap.h>

#include <string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define MDMAP_MAX_PATH                       ((tDWORD)(260)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IO_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure MDMAP_Data is declared in O:\Prague\Wrappers\MultiDMAP\mdmap.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // mdmap_h
// AVP Prague stamp end




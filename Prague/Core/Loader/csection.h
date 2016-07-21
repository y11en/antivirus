// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  23 October 2001,  19:04 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- csection.h
// Additional info
//  This implementation finds loadable prague plugins in disk folder
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __csection_h__a22fa97c_22ef_47cc_997c_696a184f2c35 )
#define __csection_h__a22fa97c_22ef_47cc_997c_696a184f2c35
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_csect.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_csect.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
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
// CriticalSection interface implementation
// Short comments -- Critical section behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call CriticalSection_Register( hROOT root );
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // csection_h
// AVP Prague stamp end




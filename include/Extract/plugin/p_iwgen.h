// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  14 April 2004,  12:00 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- IWorms Generic Detection
// Author      -- Pavlushchik
// File Name   -- plugin_iwgen.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_iwgen )
#define __public_plugin_iwgen
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_converter.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_IWGEN  ((tPID)(58044))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end

#define pm_IWGEN_SELF_MAILER_DETECTED 0xDC9328A9 // iwgen send this message with pmc_REMOTE_GLOBAL class with PID of detected process
#define pm_IWGEN_MASS_MAILER_DETECTED 0x78b8273a // mailchecker sends this message with pmc_REMOTE_GLOBAL class with PID of detected process


// AVP Prague stamp begin( Plugin public definitions, Converter )
// Converter interface implementation
// Short comments -- Converter interface
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_iwgen
// AVP Prague stamp end




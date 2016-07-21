// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  13 December 2004,  16:57 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- Not defined
// Author      -- Mike Pavlyushchik
// File Name   -- plugin_scheduler.cpp
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_scheduler )
#define __public_plugin_scheduler
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_scheduler.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_SCHEDULER  ((tPID)(58040))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, Scheduler )
// Scheduler interface implementation
// Short comments -- Simple Scheduler

// properties
#define plSETTINGS mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002000 )

// message class
//! #define pmc_SCHEDULER 0x66a39c45 //
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_scheduler
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  08 September 2005,  13:12 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- plugin_spamtest.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_spamtest )
#define __public_plugin_spamtest
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_SPAMTEST  ((tPID)(40030))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_DENISOV  ((tVID)(88))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation

// message class
#define pmc_SPAMTEST 0x0051b977 //

	#define msg_ISSPAMMESSAGE 0x00002000 // (8192) --
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_spamtest
// AVP Prague stamp end




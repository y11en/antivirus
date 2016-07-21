// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  28 June 2005,  13:55 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- plugin_trafficmonitor.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_trafficmonitor )
#define __public_plugin_trafficmonitor
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Mail/iface/i_mailtask.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_TRAFFICMONITOR  ((tPID)(40012))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_DENISOV  ((tVID)(88))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, MailInterceptorLoader )
// MailInterceptorLoader interface implementation
// Short comments --
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, MailTask )
// MailTask interface implementation
// Short comments --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_MAILTASK_PROCESS 0x0fe9fb5a //

// message class
//! #define pmc_MAILCHECKER_PROCESS 0xd875646d //
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_trafficmonitor
// AVP Prague stamp end




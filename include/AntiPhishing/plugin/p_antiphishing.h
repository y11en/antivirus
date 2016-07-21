// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  26 July 2005,  10:19 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- plugin_antiphishing.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_antiphishing )
#define __public_plugin_antiphishing
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_ANTIPHISHING  ((tPID)(58056))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation

// errors
// #define errTASK_DISABLED                         PR_MAKE_IMP_ERR(PID_ANTIPHISHING, 0x005) // 0x9e2c8005,-1641250811 (5) --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //

// message class
#define pmc_ANTI_PHISHING 0xd369d608 //

	#define msg_ANTI_PHISHING_CHECK_URL 0xd369d708 // (3546928904) --

	#define msg_ANTI_PHISHING_BAD_DATABASE 0xd369d709 // (3546928905) --

	#define msg_ANTI_PHISHING_CHECK_DATABASE 0xd369d70a // (3546928906) --

// AVP Prague stamp end



#include <AntiPhishing/structs/s_antiphishing.h>

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_antiphishing
// AVP Prague stamp end




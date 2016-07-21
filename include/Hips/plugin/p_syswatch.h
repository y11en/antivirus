// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Saturday,  13 October 2007,  18:27 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- System Watcher
// Purpose     -- Not defined
// Author      -- Mike Pavlyushchik
// File Name   -- plugin_syswatch.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_syswatch )
#define __public_plugin_syswatch
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_SYSWATCH  ((tPID)(58100))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation

// errors
//#define errTASK_DISABLED                         PR_MAKE_IMP_ERR(PID_SYSWATCH, 0x005) // 0x9e2f4005,-1641070587 (5) --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //
// AVP Prague stamp end

#include <ProductCore/report.h>

#include <kl_pushpack.h>
typedef struct tag_SystemWatcherData
{
	tQWORD   nEventCreated;
	tQWORD   nEventModified;
	tQWORD   nEventDeleted;
	tQWORD   nPidOwner;
} tSystemWatcherData;

#define cSYSWATCH_DATA 0x844A7312

#include <kl_poppack.h>

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_syswatch
// AVP Prague stamp end




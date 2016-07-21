// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  10 May 2005,  14:59 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- plugin_httpscan.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_httpscan )
#define __public_plugin_httpscan
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_HTTPSCAN  ((tPID)(49031))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MEZHUEV  ((tVID)(79))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation

// errors
//#define errTASK_DISABLED                         PR_MAKE_IMP_ERR(PID_HTTPSCAN, 0x005) // 0x9bf87005,-1678217211 (5) --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //

// message class
#define pmc_HTTPSCAN_PROCESS 0xb898104a //
// AVP Prague stamp end

#define pmc_HTTPSCAN_CANCEL_SSL 0x8b44b074 // Отказ от обработки SSL


// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_httpscan
// AVP Prague stamp end




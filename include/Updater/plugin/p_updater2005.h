// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  23 October 2007,  19:29 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- plugin_updater2005.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_updater2005 )
#define __public_plugin_updater2005
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
#include <Updater/iface/i_basesverifier.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_UPDATER2005  ((tPID)(38016))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_GUSCHIN  ((tVID)(90))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation
// Short comments --

// errors
#define errTASK_ALREADY_RUNNING                  PR_MAKE_IMP_ERR(PID_UPDATER2005, 0x008) // 0x99480008,-1723334648 (8) --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //

// message class
#define pmc_UPDATER2005_REPORT 0x43273c66 //

	#define pm_REP_UPDATE_END 0x00002001 // (8193) --

	#define pm_CORE_EVENT 0x00002003 // (8195) --

// message class
#define pmc_UPDATER2005_CRITICAL_REPORT 0x87cc73ae //

	#define pm_REP_UPDATE_END 0x00002001 // (8193) --

	#define pm_CORE_EVENT 0x00002003 // (8195) --
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, BasesVerifier )
// BasesVerifier interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_updater2005
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  11 July 2005,  15:16 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- plugin_popupchk.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_popupchk )
#define __public_plugin_popupchk
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_POPUPCHK  ((tPID)(58054))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation

// constants
#define POPUP_TASK_NAME                ((tSTRING)("PopupCheckTask")) //  --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //

// message class
#define pmc_POPUP_CHECK 0xbc10a222 //

	#define msg_POPUP_CHECK_URL 0xbc10a322 // (3155206946) --

	#define msg_POPUP_ALLOWED 0xbc10a323 // (3155206947) --

	#define msg_POPUP_ADD_TRUSTED_SITE 0xbc10a324 // (3155206948) --

	#define msg_POPUP_TEMPORARY_ALLOW_CB  0xbc10a325 // (3155206949) --
	#define msg_POPUP_TEMPORARY_ALLOW_CB2 0xbc10a326 // (3155206949) --
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_popupchk
// AVP Prague stamp end




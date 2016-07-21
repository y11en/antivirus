// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  02 December 2005,  10:59 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_bl.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_bl )
#define __public_plugin_bl
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_productlogic.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
#define PID_BL  ((tPID)(34))
#define PID_TM  ((tPID)(22))
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation
// Short comments --

// properties
#define plTASK_SETTINGS_ID mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plTASK_REPORT_NAME mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002001 )

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, TaskManager )
// TaskManager interface implementation
// Short comments -- product task supervisor

// message class
//! #define pmcPROFILE 0xc5f5a72e //

// message class
//    parameters:
//    "ctx"         -- NULL
//    "msg_id"   -- correspondent action identifier
//    "pbuff"      -- pointer to cAskTaskAction (or descendant) structure
//    "pblen"     -- must be SER_SENDMSG_PSIZE constant
//
//    Field "m_nTaskId" of the cAskTaskAction structure must identify task.
//    Constant "TASKID_TM_ITSELF" identifies TaskManager
//! #define pmcASK_ACTION 0x57701ba6 //
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, ProductLogic )
// ProductLogic interface implementation
// Short comments --

// message class
//! #define pmc_PRODUCT 0x2937151d //

// message class
//! #define pmc_REBOOT 0xb37f57b2 //

// message class
//! #define pmc_PRODUCT_UPDATE 0x0ac7ff79 //

// message class
//! #define pmc_PRODUCT_STARTUP_CHANGE 0xb88c6d59 // request for changing product startup state

// message class
//! #define pmc_PRODUCT_NOTIFICATION 0xa5b477be //

// message class
#define pmc_LOCALIZE_PRODUCT_NOTIFICATION 0x26d968a1 //
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Profile )
// Profile interface implementation
// Short comments --
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_bl
// AVP Prague stamp end


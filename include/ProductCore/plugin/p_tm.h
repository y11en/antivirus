// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  30 March 2006,  11:56 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- plugin_tm.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_tm )
#define __public_plugin_tm
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
#define PID_TM  ((tPID)(22))
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>

#include <ProductCore/environment.h>
#include <ProductCore/iface/i_task.h>
#include <ProductCore/iface/i_taskmanager.h>
#include <ProductCore/iface/i_tmhost.h>
// AVP Prague stamp end


#define REPORT_MAIN_FILE "report.rpt"
#define REPORT_LOG_FILE  "syslog.rpt"

// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MEZHUEV  ((tVID)(79))
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

// message class
//! #define pmcTM_STARTUP_SCHEDULE 0xf19d9841 //

// message class
//! #define pmcTM_EVENTS 0xa4094ca0 // TM events
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, TmHost )
// TmHost interface implementation
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation
// Short comments --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Component )
// Component interface implementation
// Short comments --
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, BL )
// BL interface implementation
// Short comments --

// properties

// message class
//! #define pmc_BL_TASK_SETTINGS_CHANGED 0x596092cb //
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_tm
// AVP Prague stamp end


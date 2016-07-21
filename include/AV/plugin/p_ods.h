// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  22 March 2007,  12:40 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- plugin_ods.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_ods )
#define __public_plugin_ods
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_ODS  ((tPID)(49022))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MEZHUEV  ((tVID)(79))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation
// Short comments --

// errors

// properties
#define plAVS mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// properties
#define plNTFS mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002000 )

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0x52e986b3 // класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
// AVP Prague stamp end



#define pmc_ODS_SCANNERS  0xa91c56ed

#define pm_MEMORY_SCANNED  0x16d822b4

// AVP Prague stamp begin( Plugin public definitions, Component )
// Component interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Service )
// Service interface implementation
// Short comments --
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_ods
// AVP Prague stamp end




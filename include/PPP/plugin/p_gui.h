// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  22 June 2005,  18:42 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_gui.cpp
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_gui )
#define __public_plugin_gui
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
#include <ProductCore/iface/i_tmclient.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_GUI  ((tPID)(19))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation
// Short comments --

// errors
//#define errTASK_CANNOT_UPDATE_SETTINGS_ON_RUNTIME PR_MAKE_IMP_ERR(PID_GUI, 0x001) // 0x90013001,-1878970367 (1) --
//#define errTASK_STATE_UNKNOWN                    PR_MAKE_IMP_ERR(PID_GUI, 0x002) // 0x90013002,-1878970366 (2) --

// properties
#define plGUI_USER_SETTINGS_PATH mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002001 )

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, TmClient )
// TmClient interface implementation
// Short comments -- client of TaskManager

// errors
//#define errDONT_WANT_TO_PROCESS                  PR_MAKE_IMP_ERR(PID_GUI, 0x001) // 0x90013001,-1878970367 (1) -- client do not want to process this request, valid for both OpenRequest and ProcessRequest

// properties
#define plGUI             mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002000 )
#define plTASK_SESSION_ID mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00001004 )
// AVP Prague stamp end

#define pm_TO_RESTART_GUI 0x2fd850fa


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, Component )
// Component interface implementation
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, GUI )
// GUI interface implementation
// Short comments --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_gui
// AVP Prague stamp end




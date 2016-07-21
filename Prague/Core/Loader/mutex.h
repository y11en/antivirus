// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Wednesday,  10 March 2004,  14:58 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- mutex.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __mutex_h__799dde3f_0fb3_48a1_a761_b0e462ee631a )
#define __mutex_h__799dde3f_0fb3_48a1_a761_b0e462ee631a
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_mutex.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define PID_WIN32_PLUGIN_LOADER  ((tPID)(5))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Mutex interface implementation
// Short comments -- Mutex behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Mutex_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Mutex. Inner data structure
//!
//! typedef struct tag_MutexData {
//! 	HANDLE handle;        // handle of the Windows object
//! 	char*  name;          // name of the object
//! 	tBOOL  open;          // do not try to create
//! 	tBOOL  initial_owner; // initially owned
//! 	tDWORD last_error;    // last win error
//! 	tBOOL  global;        // --
//! } MutexData;
//!
//!
// to get pointer to the structure MutexData from the hOBJECT obj
//! #define CUST_TO_MutexData(object)  ((MutexData*)( (((tPTR*)object)[2]) ))
  // data structure MutexData is declared in o:\prague\loader\mutex.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // mutex_c
// AVP Prague stamp end




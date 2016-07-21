// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Tuesday,  29 April 2003,  16:28 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_io.c
// Additional info
//    File input/output for Win32
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __win32_io_h__3ad6132a_88c8_4d83_9d75_e32a9df15151 )
#define __win32_io_h__3ad6132a_88c8_4d83_9d75_e32a9df15151
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/plugin/p_win32_nfio.h>

#include <windows.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
#define fACCESS_NONE                   ((tDWORD)(0x01000000L)) // object has to be closed
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
#if defined (__cplusplus)
//  extern "C" 
#endif
  tERROR pr_call IO_Register( hROOT root );
// AVP Prague stamp end



typedef union tag_QWord {
	tLONGLONG v;
	struct {
		tDWORD l;
		tLONG  h;
	};
} tQWord;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure Win32IOData is declared in o:\prague\nfio\win32_io.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
#define plNATIVE_HANDLE    mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002001 )
#define plEXCLUSIVE_ACCESS mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002002 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // win32_io_c
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  27 February 2003,  18:48 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- k_ifenum.h
// Additional info
//  Kernel interface implementations
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __k_ifenum_h__607bca26_7c08_4c08_967f_36922dc5cee4 )
#define __k_ifenum_h__607bca26_7c08_4c08_967f_36922dc5cee4
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_iface.h>
#include <Prague/iface/i_ifenum.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_root.h>
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
//! #define PID_KERNEL  ((tPID)(1))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IFaceEnum interface implementation
// Short comments -- registered interface enumerator interface
// Extended comment -
//  Supplies facility to enumerate registered interfaces
//  Also possible to get shared properties of registered iterface
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IFaceEnum_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure IFENUM_DATA is declared in O:\prague\kernel\k_ifenum.c source file
// Interface IFaceEnum. Inner data structure
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // k_ifenum_h
// AVP Prague stamp end




// -------- Monday,  18 September 2000,  15:27 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- list.h
// -------------------------------------------



#if !defined( __list_h__2ca52783_8d78_11d4_aec2_008048c9de90 )
#define __list_h__2ca52783_8d78_11d4_aec2_008048c9de90



#include <Prague/iface/i_root.h>
#include <Prague/iface/i_list.h>
#include <Prague/iface/i_heap.h>




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Interface constants )
// AVP Prague stamp end( List, Interface constants )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Plugin identifier )
#define PID_UTIL  ((tPID)(0x0000fa07))
// AVP Prague stamp end( List, Plugin identifier )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Vendor identifier )
#define VID_ANDREW  ((tVID)(64))
// AVP Prague stamp end( List, Vendor identifier )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Interface comment )
// --------------------------------------------
// --- 9e55aca2_6dda_11d4_b94e_008048ec2fc7 ---
// --------------------------------------------
// List interface implementation
// Extended comment
//
// AVP Prague stamp end( List, Interface comment )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Register call )
#if defined( __cplusplus )
extern "C"
{
#endif

  tERROR pr_call List_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( List, Register call )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Property table )
// AVP Prague stamp end( List, Property table )
// --------------------------------------------------------------------------------



#endif // list_h




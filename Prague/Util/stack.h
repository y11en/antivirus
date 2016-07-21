// -------- Monday,  18 September 2000,  15:27 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- stack.h
// -------------------------------------------



#if !defined( __stack_h__2ca52784_8d78_11d4_aec2_008048c9de90 )
#define __stack_h__2ca52784_8d78_11d4_aec2_008048c9de90



#include <Prague/iface/i_root.h>
#include <Prague/iface/i_stack.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Interface constants )
// AVP Prague stamp end( Stack, Interface constants )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Plugin identifier )
#define PID_UTIL  ((tPID)(0x0000fa07))
// AVP Prague stamp end( Stack, Plugin identifier )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Vendor identifier )
#define VID_ANDREW  ((tVID)(64))
// AVP Prague stamp end( Stack, Vendor identifier )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Interface comment )
// --------------------------------------------
// --- 9e55aca3_6dda_11d4_b94e_008048ec2fc7 ---
// --------------------------------------------
// Stack interface implementation
// Extended comment
//   
// AVP Prague stamp end( Stack, Interface comment )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

  tERROR pr_call Stack_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( Stack, Register call )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Property table )
// AVP Prague stamp end( Stack, Property table )
// --------------------------------------------------------------------------------



#endif // stack_h




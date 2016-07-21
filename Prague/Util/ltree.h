// -------- Monday,  18 September 2000,  15:27 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- ltree.h
// -------------------------------------------



#if !defined( __ltree_h__2ca52781_8d78_11d4_aec2_008048c9de90 )
#define __ltree_h__2ca52781_8d78_11d4_aec2_008048c9de90



#include <Prague/iface/i_root.h>
#include <Prague/iface/i_ltree.h>





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Interface constants )
// AVP Prague stamp end( LTree, Interface constants )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Plugin identifier )
#define PID_UTIL  ((tPID)(0x0000fa07))
// AVP Prague stamp end( LTree, Plugin identifier )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Vendor identifier )
#define VID_ANDREW  ((tVID)(64))
// AVP Prague stamp end( LTree, Vendor identifier )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Interface comment )
// --------------------------------------------
// --- 9e55aca0_6dda_11d4_b94e_008048ec2fc7 ---
// --------------------------------------------
// LTree interface implementation
// Extended comment
//   
// AVP Prague stamp end( LTree, Interface comment )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

  tERROR pr_call LTree_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( LTree, Register call )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Property table )
// AVP Prague stamp end( LTree, Property table )
// --------------------------------------------------------------------------------



#endif // ltree_h




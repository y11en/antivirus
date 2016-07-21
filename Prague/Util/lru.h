// -------- Thursday,  05 October 2000,  15:51 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- lru.h
// -------------------------------------------



#if !defined( __lru_h__30377a2b_9754_4ced_a1cc_7d22f0b41b71 )
#define __lru_h__30377a2b_9754_4ced_a1cc_7d22f0b41b71



#include <Prague/iface/i_root.h>
#include <Prague/iface/i_lru.h>




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LRU, Interface constants )
// AVP Prague stamp end( LRU, Interface constants )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LRU, Plugin identifier )
#define PID_UTIL  ((tPID)(0x0000fa07))
// AVP Prague stamp end( LRU, Plugin identifier )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LRU, Vendor identifier )
#define VID_ANDREW  ((tVID)(64))
// AVP Prague stamp end( LRU, Vendor identifier )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LRU, Interface comment )
// --------------------------------------------
// --- 9e55aca1_6dda_11d4_b94e_008048ec2fc7 ---
// --------------------------------------------
// LRU interface implementation
// Extended comment
//   
// AVP Prague stamp end( LRU, Interface comment )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LRU, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

	tERROR pr_call LRU_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( LRU, Register call )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LRU, Property table )
// AVP Prague stamp end( LRU, Property table )
// --------------------------------------------------------------------------------



#endif // lru_h




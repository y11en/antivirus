// -------- Friday,  27 October 2000,  15:18 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Universal Archiver
// Sub project -- Example
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- i_cab.h
// -------------------------------------------



#if !defined( __example_h__b3aec046_33d8_4542_a20e_bee42346bede )
#define __example_h__b3aec046_33d8_4542_a20e_bee42346bede



#include <Prague/iface/i_root.h>
#include <Extract/iface/i_minarc.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Interface constants )
// AVP Prague stamp end( MiniArchiver, Interface constants )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Plugin identifier )
#define PID_CAB  ((tPID)(0x0000f627))
// AVP Prague stamp end( MiniArchiver, Plugin identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( MiniArchiver, Vendor identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Interface comment )
// --------------------------------------------
// --- f533045c_723a_48c7_95ca_2aba12755d87 ---
// --------------------------------------------
// MiniArchiver interface implementation
// Short comments -- Mini Plugin for Universal Archiver
// Extended comment
//   
// AVP Prague stamp end( MiniArchiver, Interface comment )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

	tERROR pr_call MiniArchiver_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( MiniArchiver, Register call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Property table )
// AVP Prague stamp end( MiniArchiver, Property table )
// --------------------------------------------------------------------------------



#endif // example_h




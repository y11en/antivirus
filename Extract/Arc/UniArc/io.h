// -------- Monday,  23 October 2000,  15:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Unversal Extractor
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- io.h
// -------------------------------------------



#if !defined( __io_h__157b5e12_9296_48ce_abcd_a9572ea3c7a1 )
#define __io_h__157b5e12_9296_48ce_abcd_a9572ea3c7a1



#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Interface constants )
// AVP Prague stamp end( IO, Interface constants )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( IO, Vendor identifier )
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Interface comment )
// --------------------------------------------
// --- 74d331e1_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
// IO interface implementation
// Short comments -- input/output interface
// Extended comment
//   Defines behavior of input/output of an object
//   Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position. 
// AVP Prague stamp end( IO, Interface comment )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

	tERROR pr_call IO_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( IO, Register call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------



#endif // io_h




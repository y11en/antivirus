// -------- Friday,  22 December 2000,  11:31 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- uacback.h
// -------------------------------------------



#if !defined( __uacback_h__49787e0f_33a6_489a_9964_3cc2a3f4fb4c )
#define __uacback_h__49787e0f_33a6_489a_9964_3cc2a3f4fb4c



#include <Prague/iface/i_root.h>
#include <Extract/iface/i_uacall.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Interface constants )
// AVP Prague stamp end( UniArchiverCallback, Interface constants )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Plugin identifier )
//#define PID_PLUGIN  ((tPID)(0x0000f629))
// AVP Prague stamp end( UniArchiverCallback, Plugin identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( UniArchiverCallback, Vendor identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Interface comment )
// --------------------------------------------
// --- ee6a23e6_6292_4a74_bf65_60818b3fbe9f ---
// --------------------------------------------
// UniArchiverCallback interface implementation
// Short comments -- call Back interface to Uni Archiver to get access to internal lists
// AVP Prague stamp end( UniArchiverCallback, Interface comment )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

	tERROR pr_call UniArchiverCallback_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( UniArchiverCallback, Register call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Property table )
// AVP Prague stamp end( UniArchiverCallback, Property table )
// --------------------------------------------------------------------------------



#endif // uacback_h




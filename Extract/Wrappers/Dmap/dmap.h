// -------- Thursday,  20 July 2000,  19:44 --------
// Project -- Prague
// Sub project -- Direct mapper
// Purpose -- Mapping plugin
// ...........................................
// Author -- Andy Nikishin
// File Name -- dmap.h
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------



#if !defined( __dmap_h__ae43ea64_0cb9_4968_ba54_818cc1f10061 )
#define __dmap_h__ae43ea64_0cb9_4968_ba54_818cc1f10061



#include <Prague/iface/i_io.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin, Plugin identifier )
//#define DMAP_ID  ((tPID)(0x0000f61c))
// AVP Prague stamp end( Plugin, Plugin identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( Plugin, Vendor identifier )
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

//#define ppFILE_NAME         mPROPERTY_MAKE_PRIVATE(pTYPE_STRING , 0x00000001)
//#define ppFILE_MODE         mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD  , 0x00000002)


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Register call )
	tERROR pr_call IO_Register( hROOT root);
// AVP Prague stamp end( IO, Register call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------



#endif // dmap_h




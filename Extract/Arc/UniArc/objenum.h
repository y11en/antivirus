// -------- Monday,  23 October 2000,  15:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Unversal Extractor
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- objenum.h
// -------------------------------------------



#if !defined( __objenum_h__0c36ebbe_67b2_4878_9da4_d3582e227fae )
#define __objenum_h__0c36ebbe_67b2_4878_9da4_d3582e227fae



#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_root.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjEnum, Interface constants )
// AVP Prague stamp end( ObjEnum, Interface constants )
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjEnum, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( ObjEnum, Vendor identifier )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjEnum, Interface comment )
// --------------------------------------------
// --- 74d331e0_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
// ObjEnum interface implementation
// Short comments -- object enumerator
// Extended comment
//   Defines behavior of an object container/enumerator
//   
//   Supplies object enumerate functionality on one level of hierarchy only. 
//   For example: on logical drive object of this interface must enumerate objects in one folder.
//   
//   Also must support creating new IO object in the container. There are two ways to do it. First is three steps protocol:
//     -- ObjectCreate system method with folder object as parent
//     -- dictate all necessary properties for the new object
//     -- ObjectCreateDone
//   Second is CreateNew folder's method which is wrapper of above protocol.
//   Advise: when caller invokes ObjectCreateDone method folder object receives ChildDone notification and can do all necessary job to process it.
//   
// AVP Prague stamp end( ObjEnum, Interface comment )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjEnum, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

	tERROR pr_call ObjPtr_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( ObjEnum, Register call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjEnum, Property table )
// AVP Prague stamp end( ObjEnum, Property table )
// --------------------------------------------------------------------------------



#endif // objenum_h




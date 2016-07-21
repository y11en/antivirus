// -------- Wednesday,  09 August 2000,  13:39 --------
// Project -- AVP ver 4
// Sub project -- ZIP Extractor
// Purpose -- Encoded stream extractor
// ...........................................
// Author -- Andy Nikishin
// File Name -- dmap.h
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------



#if !defined( __dmap_h__c3a57385_d29c_4cee_8fd9_994f0cc3a118 )
#define __dmap_h__c3a57385_d29c_4cee_8fd9_994f0cc3a118



#include <Prague/iface/i_io.h>
#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_root.h>

#include "../Dmap/plugin_dmap.h"



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin, Plugin identifier )
#define PID_ZIPPASSWORDMAP  ((tPID)(0x0000f620))
// AVP Prague stamp end( Plugin, Plugin identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( Plugin, Vendor identifier )
// --------------------------------------------------------------------------------

#define errDECRIPT_WARNING PR_MAKE_IMP_WARN(PID_ZIPPASSWORDMAP, 0x001)


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

// See declaration in DMAP.H

//#define ppFILE_NAME         mPROPERTY_MAKE_PRIVATE(pTYPE_STRING, 0x00000001)
//#define ppFILE_MODE         mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,  0x00000002)
//#define ppMAP_AREA_START    mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,  0x00000003)
//#define ppMAP_AREA_SIZE     mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,  0x00000004)
//#define ppMAP_AREA_ORIGIN   mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,  0x00000005)
//#define ppMAP_PROP_FATHER   mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,  0x00000006)

#define ppMAP_PROP_PASSWORD mPROPERTY_MAKE_PRIVATE(pTYPE_PTR,    0x00002005)
#define ppMAP_PROP_CRC32    mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,  0x00002006)
#define ppMAP_PROP_DATE     mPROPERTY_MAKE_PRIVATE(pTYPE_WORD,   0x00002007)
#define ppMAP_PROP_TIME     mPROPERTY_MAKE_PRIVATE(pTYPE_WORD,   0x00002008)
#define ppMAP_PROP_ERRORCODE mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD, 0x00002009)
#define ppMAP_PROP_FLAG      mPROPERTY_MAKE_PRIVATE(pTYPE_WORD,  0x0000200a)


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



#endif // dmap_h




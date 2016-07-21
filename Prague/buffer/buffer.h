// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  31 January 2002,  11:13 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- buffer interface
// Purpose     -- plugin implements buffer interface
// Author      -- petrovitch
// File Name   -- buffer.h
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __buffer_h__688e44c2_1402_46ec_b61c_acd3cd56e6bc )
#define __buffer_h__688e44c2_1402_46ec_b61c_acd3cd56e6bc
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_buffer.h>
#include <m_utils.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#if !defined( PID_BUFFER )
	#define PID_BUFFER  ((tPID)(20))
#endif
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Buffer interface implementation
// Short comments -- piece of memory interface
// Extended comment -
//  Interface supplies methods to work with memory piece. It is compatible with IO interface, i.e. Method and property tables of both interfaces begin identically.
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
#if defined( __cplusplus )
extern "C" {
#endif

  tERROR pr_call Buffer_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

#if defined( __cplusplus )
extern "C" {
#endif

//typedef struct tag_hi_Buffer {
//	const iBufferVtbl* vtbl; // pointer to the "Buffer" virtual table
//	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
//	const tBYTE*       data; // data
//} *hi_Buffer;

#if defined( __cplusplus )
}
#endif

// }
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // buffer_h
// AVP Prague stamp end




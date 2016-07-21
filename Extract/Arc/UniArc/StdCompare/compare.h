// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  31 October 2002,  13:24 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- StdCompare
// Purpose     -- Standard Compare Interfaces
// Author      -- mike
// File Name   -- compare.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __compare_h__3ba12598_d58c_42e5_b7d4_8fa2d37b5c2c )
#define __compare_h__3ba12598_d58c_42e5_b7d4_8fa2d37b5c2c
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_stdcompare.h"

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_compar.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define SUBTYPE_COMPARE_QWORD          ((tDWORD)(1)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Compare interface implementation
// Short comments -- compare interface
//    Used as user defined interface in TREE and HDB interfaces for sorting and searching. May be implemented as static ( witout any settings or as normal interface
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Compare_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure  is declared in O:\Prague\StdCompare\compare.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // compare_h
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  07 October 2002,  13:37 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- hash Container
// Sub project -- Compare Plugin
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- compare.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __compare_h__cfcb5b47_f962_432c_9d13_37a045c247ae )
#define __compare_h__cfcb5b47_f962_432c_9d13_37a045c247ae
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_comparehashcont.h"

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_compar.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
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
  // data structure  is declared in O:\Prague\HashContainer\Compere\compare.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // compare_h
// AVP Prague stamp end

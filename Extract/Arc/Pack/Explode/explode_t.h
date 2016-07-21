// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  09 December 2002,  12:48 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Explode decompressor
// Author      -- Andy Nikishin
// File Name   -- explode_t.h
// Additional info
//    Explode decompressor. Uses for ZIP Archivers
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __explode_t_h__806a546c_9435_4f02_a268_dd5fdb30f9b0 )
#define __explode_t_h__806a546c_9435_4f02_a268_dd5fdb30f9b0
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_explode.h"

#include <Prague/iface/i_root.h>
#include <Extract/iface/i_transformer.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Transformer_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure ExplodeData is declared in O:\Prague\Extract\Explode\explode_t.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plEXPLODE_BIT_FLAG mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // explode_t_h
// AVP Prague stamp end




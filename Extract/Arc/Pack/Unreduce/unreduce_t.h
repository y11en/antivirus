// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  09 December 2002,  13:48 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- UnReduce decompressor
// Author      -- Andy Nikishin
// File Name   -- unreduce_t.h
// Additional info
//    UnReduce decompressor. Uses for ZIP
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __unreduce_t_h__98d6d7b5_7ade_4f4e_9730_e58076b5ed24 )
#define __unreduce_t_h__98d6d7b5_7ade_4f4e_9730_e58076b5ed24
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_unreduce.h"

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
  // data structure UnreduceData is declared in O:\Prague\Extract\Unreduce\unreduce_t.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plEXPLODE_COMPRESSION_FLAG mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // unreduce_t_h
// AVP Prague stamp end




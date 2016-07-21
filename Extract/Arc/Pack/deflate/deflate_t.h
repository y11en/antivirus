// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  10 December 2002,  12:29 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Archive support for KAV ver 4
// Sub project -- Not defined
// Purpose     -- Deflate packer plugin
// Author      -- Andy Nikishin
// File Name   -- deflate_t.h
// Additional info
//    Deflate packer plugin
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __deflate_t_h__91b17ac7_e721_4392_94da_93c8dbfb0031 )
#define __deflate_t_h__91b17ac7_e721_4392_94da_93c8dbfb0031
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_deflate.h"
#include "zlibt.h"
#include "zutil.h"

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>

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
  // data structure _tag_DeflateData is declared in O:\Prague\Packers\Deflate\deflate_t.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plDEFLATE_WINDOW_SIZE      mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002000 )
   #define plDEFLATE_WINDOW           mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002001 )
   #define plDEFLATE_WINDOW_MULTIPIER mPROPERTY_MAKE_LOCAL( pTYPE_INT     , 0x00002002 )
   #define plDEFLATE_INPUT_CRC        mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002005 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // deflate_t_h
// AVP Prague stamp end




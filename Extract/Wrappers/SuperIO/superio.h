// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  11 December 2002,  14:07 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- SuperIO
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- superio.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __superio_h__996e4267_c72e_462f_891b_1583f1554d37 )
#define __superio_h__996e4267_c72e_462f_891b_1583f1554d37
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_superio.h"

#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_string.h>

#include <Extract/iface/i_transformer.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- interface for IO buffering and conversion
//    Supplies IO interface for any data presentation which provides data converters.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IO_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure tSUPERIO_DATA is declared in O:\Praque\SuperIO\superio.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plINPUT_CONVERTER  mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002000 )
   #define plOUTPUT_CONVERTER mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002001 )
   #define plREAD_CHUCK_SIZE  mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // superio_h
// AVP Prague stamp end




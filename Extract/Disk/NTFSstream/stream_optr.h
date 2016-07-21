// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  25 September 2002,  12:52 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- plugin for NTFS stream operation
// Author      -- Doukhvalow
// File Name   -- stream_optr.h
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __stream_optr_h__839cd56c_b9e0_4757_a62b_460bf6a17838 )
#define __stream_optr_h__839cd56c_b9e0_4757_a62b_460bf6a17838
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_objptr.h>
#include "plugin_ntfsstream.h"
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include "NTFSStream.h"
#include "../../Prague/nfio/win32_io.h"
#include "stream_util.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call ObjPtr_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure OPtrData is declared in o:\prague\ntfsstream\stream_optr.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
   #define plBASE_FULL_NAME mPROPERTY_MAKE_LOCAL( pTYPE_STRING, 0x00002002 )
   #define plNATIVE_ERR     mPROPERTY_MAKE_LOCAL( pTYPE_DWORD, 0x00002000 )
   #define plBASE_HANDLE    mPROPERTY_MAKE_LOCAL( pTYPE_DWORD, 0x00002003 )
   #define plCREATE_EMPTY   mPROPERTY_MAKE_LOCAL( pTYPE_BOOL, 0x00002004 )
   #define plSTREAM_COUNT   mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002005 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // stream_optr_h
// AVP Prague stamp end




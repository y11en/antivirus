// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Tuesday,  20 January 2004,  16:36 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- IChecker
// Author      -- Pavlushchik
// File Name   -- ichecker2.c
// Additional info
//    Integrated Solution Lite
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __ichecker2_h__c2112943_1a74_4129_a9cf_ca3680c3dff1 )
#define __ichecker2_h__c2112943_1a74_4129_a9cf_ca3680c3dff1
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
#include <ProductCore/iface/i_ichecker2.h>
#include "plugin_ichecker2.h"
#include <Prague/iface/i_io.h>
#include <ProductCore/iface/i_sfdb.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
#define cICHECKER_ARCHIVES_OFF         ((tDWORD)(1)) // Отключает подсчет хеша для архивов
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IChecker interface implementation
// Short comments --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IChecker_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure ifsData is declared in O:\Prague\ISLite\iChecker2\ichecker2.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
#define plDB_PATHNAME       mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002000 )
#define plDB_USER_DATA_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plDB_USER_VERSION   mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // ichecker2_c
// AVP Prague stamp end




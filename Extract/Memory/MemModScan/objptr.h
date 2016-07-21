// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  13 March 2003,  16:17 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- objptr.h
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __objptr_h__85d9a6db_be7e_4f43_87b5_48d415e82fd0 )
#define __objptr_h__85d9a6db_be7e_4f43_87b5_48d415e82fd0
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_memmodscan.h"
#include "io.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call ObjPtr_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure ObjPtrPrivate is declared in O:\Prague\MemModScan\objptr.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plENUM_PID mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // objptr_h
// AVP Prague stamp end




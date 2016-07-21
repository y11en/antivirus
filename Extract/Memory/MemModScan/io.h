// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  09 April 2003,  11:35 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- io.h
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __io_h__69fcc51b_7f2b_4203_a550_269f55fed025 )
#define __io_h__69fcc51b_7f2b_4203_a550_269f55fed025
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_memmodscan.h"
#include "os.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IO_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure IOPrivate is declared in O:\Prague\MemModScan\io.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plIO_PID     mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002000 )
   #define plIO_HMODULE mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002001 )
   #define plSize       mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002002 )
   #define plImageBase  mPROPERTY_MAKE_LOCAL( pTYPE_LONGLONG    , 0x00002003 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // io_h
// AVP Prague stamp end




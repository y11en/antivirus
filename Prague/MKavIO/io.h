// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Thursday,  03 August 2006,  16:13 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- MiniKav
// Sub project -- Not defined
// Purpose     -- IO для чтения/записи в файл через драйвер MiniKav.sys
// Author      -- Sobko
// File Name   -- io.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __io_c__063c6eb0_688d_4c7c_affe_26a28fcd4898 )
#define __io_c__063c6eb0_688d_4c7c_affe_26a28fcd4898
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/plugin/p_mkavio.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0x52e986b3 // класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
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
  // data structure Private is declared in O:/Prague/MKavIO\io.c source file
  // data structure Private is declared in O:\Prague\MKavIO\io.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // io_c
// AVP Prague stamp end




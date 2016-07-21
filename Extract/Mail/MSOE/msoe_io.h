// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  22 November 2004,  14:34 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- msoe
// Sub project -- msoe
// Purpose     -- плагин для работы с Outlook Express
// Author      -- Mezhuev
// File Name   -- msoe_io.c
// Additional info
//    Данный плагин предназначен для работы со следующими объектами Outlook Express:
//    - почтовая система (идентифицируется по Identity, если не указан, то успользуется текущий)
//    - база сообщений в формате dbx.
//    - сообщение в формате RFC822 (eml файл)
//    - тела и аттачменты сообщения (message part)
//
//    Интерфейсы данного плагина не поддерживают идентификацию объектов по строке!!!
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __msoe_io_c__98a82a9f_abd9_4a19_ad92_ccb413313ecb )
#define __msoe_io_c__98a82a9f_abd9_4a19_ad92_ccb413313ecb
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/plugin/p_msoe.h>
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
// Interface IO. Inner data structure

typedef struct tag_OEIO_Data 
{
	tDWORD       OpenMode;       // --
	tDWORD       AccessMode;     // --
	tBOOL        fDeleteOnClose; // --
	hOS          hOS;            // --
	tDWORD       Origin;         // --
	PROEObject * ioclass;        // --
	tBOOL        fModified;      // --
} OEIO_Data;


// to get pointer to the structure OEIO_Data from the hOBJECT obj
#define CUST_TO_OEIO_Data(object)  ((OEIO_Data*)( (((tPTR*)object)[2]) ))
  // data structure OEIO_Data is declared in O:\Prague\msoe\msoe_io.c source file
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	OEIO_Data*         data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // msoe_io_c
// AVP Prague stamp end


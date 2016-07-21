// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  22 November 2004,  14:53 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- outloook
// Sub project -- MDB
// Purpose     -- плагин для почтовой системы Outlook
// Author      -- Mezhuev
// File Name   -- mdb_io.c
// Additional info
//    Данный плагин предназначен для работы со следующими объектами Outlook:
//    - почтовая система для указанного Profile, если не указан, то успользуется текущий.
//    - база сообщений (pst файл)
//    - сообщение (Outlook Message)
//
//    Плагин реализован на основе MAPI интерфейса.
//
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __mdb_io_c__fe067b3c_42eb_41d9_b8f6_9aebb7874186 )
#define __mdb_io_c__fe067b3c_42eb_41d9_b8f6_9aebb7874186
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_os.h>

#include <Extract/plugin/p_mdb.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0xda000001 // (3657433089) -- класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
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
  // data structure MDBIO_Data is declared in o:\prague\outlook\mdb_io.c source file
// Interface IO. Inner data structure

typedef struct tag_MDBIO_Data 
{
	IMessage *     message;        // --
	tBYTE          FillChar;       // --
	tDWORD         OpenMode;       // --
	tDWORD         AccessMode;     // --
	tBOOL          fDeleteOnClose; // --
	hOS            hOS;            // --
	tDWORD         Origin;         // --
	IMAPIFolder *  folder;         // --
	LPSPropValue   entry;          // --
	IMAPISession * session;        // --
	IMsgStore *    store;          // --
	tSTRING        profile;        // --
} MDBIO_Data;


// to get pointer to the structure MDBIO_Data from the hOBJECT obj
#define CUST_TO_MDBIO_Data(object)  ((MDBIO_Data*)( (((tPTR*)object)[2]) ))
//! #define CUST_TO_MDBIO_Data(object)  ((MDBIO_Data*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MDBIO_Data*        data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // mdb_io_c
// AVP Prague stamp end


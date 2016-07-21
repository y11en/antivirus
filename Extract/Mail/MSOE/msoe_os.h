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
// File Name   -- msoe_os.c
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
#if !defined( __msoe_os_c__c169ac70_b010_453e_ab69_1b3304632df3 )
#define __msoe_os_c__c169ac70_b010_453e_ab69_1b3304632df3
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/plugin/p_msoe.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define OS_TYPE_MAIL                   ((tDWORD)(0)) //
#define OS_TYPE_DBX                    ((tDWORD)(1)) //
#define OS_TYPE_MESSAGE                ((tDWORD)(2)) //
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
// Short comments -- object system interface
//    Interface defines behavior of an object system. It responds for
//      - enumerate IO and Folder objects by creating Folder objects.
//      - create and delete IO and Folder objects
//    There are two possible ways to create or open IO object on OS. First is three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call OS_Register( hROOT root );
// AVP Prague stamp end

// AVP Prague stamp begin( Data structure,  )
// Interface OS. Inner data structure

typedef struct tag_MSOE_Data 
{
	PROEObject * osclass;   // --
	hIO          hIOBase;   // --
	tDWORD       ostype;    // --
	tBOOL        fModified; // --
} MSOE_Data;


// to get pointer to the structure MSOE_Data from the hOBJECT obj
#define CUST_TO_MSOE_Data(object)  ((MSOE_Data*)( (((tPTR*)object)[2]) ))
  // data structure MSOE_Data is declared in O:\Prague\msoe\msoe_os.c source file
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_OS 
{
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MSOE_Data*         data; // pointer to the "OS" data structure
} *hi_OS;

// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // msoe_os_c
// AVP Prague stamp end


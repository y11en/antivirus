// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  29 October 2007,  15:17 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- outloook
// Sub project -- MDB
// Purpose     -- плагин для почтовой системы Outlook
// Author      -- Mezhuev
// File Name   -- mdb_os.c
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
#if !defined( __mdb_os_c__5b6660a6_53af_46a2_83cc_b4ab30b6e54e )
#define __mdb_os_c__5b6660a6_53af_46a2_83cc_b4ab30b6e54e
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
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Extract/plugin/p_mdb.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call OS_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface OS. Inner data structure

typedef struct tag_MDB_Data {
	tCHAR         name[MAX_PATH];     // --
	tCHAR         password[MAX_PATH]; // --
	tDWORD        type;               // --
	tBOOL         is_public_folders;  // --
	tBOOL         is_private_folders; // --
	tBOOL         is_read_only;       // --
	IProfAdmin*   prof_admin;         // --
	IMAPISession* session;            // --
	hObjPtr       tmpenum;            // --
	hIO           hRealIO;            // --
	hIO           hNativeIO;          // --
	tBOOL         fModified;          // --
	tDWORD        priority;           // --
	tBOOL         is_force_ui_suppress; // --
 	HANDLE        token;
 	HANDLE        tokenold;
//! 	tDWORD        priority;
} MDB_Data;


// to get pointer to the structure MDB_Data from the hOBJECT obj
#define CUST_TO_MDB_Data(object)  ((MDB_Data*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_OS {
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MDB_Data*          data; // pointer to the "OS" data structure
} *hi_OS;

// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // mdb_os_c
// AVP Prague stamp end




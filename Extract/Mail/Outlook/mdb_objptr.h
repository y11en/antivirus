// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  25 November 2004,  12:36 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- outloook
// Sub project -- MDB
// Purpose     -- плагин для почтовой системы Outlook
// Author      -- Mezhuev
// File Name   -- mdb_objptr.c
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
#if !defined( __mdb_objptr_c__2a293f6c_5542_4ecf_9102_5ed5696fcb02 )
#define __mdb_objptr_c__2a293f6c_5542_4ecf_9102_5ed5696fcb02
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/plugin/p_mdb.h>
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
// Interface ObjPtr. Inner data structure

typedef struct tag_MDBEnum_Data 
{
	tObjPtrState   ptrState; // --
	tBOOL          fFolder;  // --
	IMAPITable *   table;    // --
	IMAPISession * session;  // --
	hOS            hOS;      // --
	IMAPIProp *    object;   // --
	tDWORD         tbltype;  // --
	LPSRowSet      objdata;  // --
	tDWORD         Origin;   // --
	IMAPIFolder *  folder;   // --
	tDWORD         objtype;  // --
	LPSPropValue   objentry; // --
	IMsgStore *    store;    // --
	tSTRING        profile;  // --
} MDBEnum_Data;


// to get pointer to the structure MDBEnum_Data from the hOBJECT obj
#define CUST_TO_MDBEnum_Data(object)  ((MDBEnum_Data*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MDBEnum_Data*      data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // mdb_objptr_c
// AVP Prague stamp end


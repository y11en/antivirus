// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  29 October 2007,  15:23 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- outloook
// Sub project -- MDB
// Purpose     -- плагин для почтовой системы Outlook
// Author      -- Mezhuev
// File Name   -- plugin_mdb.c
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
#if !defined( __public_plugin_mdb )
#define __public_plugin_mdb
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_MDB  ((tPID)(49000))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MEZHUEV  ((tVID)(79))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, OS )
// OS interface implementation
// Short comments -- object system interface

// properties
#define plMDB_PASSWORD             mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002001 )
#define plMDB_IS_PUBLIC_FOLDERS    mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002003 )
#define plMDB_IS_PRIVATE_FOLDERS   mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002004 )
#define plMDB_IS_FORCE_UI_SUPPRESS mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002005 )

// message class
//! #define pmc_OS_VOLUME 0x75eb704a // сообщения о томах

// message class
#define pmc_OS_CHANGE 0x8c2643b4 //

	#define pm_OS_CHANGE 0x00002000 // (8192) --
// AVP Prague stamp end



#define pmc_MDB_IS_EXCHANGE_SUPPORTED 0x5f3f238d //

// AVP Prague stamp begin( Plugin public definitions, ObjPtr )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// properties
#define plMDB_MESSAGE mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002000 )

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0x52e986b3 // класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_mdb
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  25 August 2004,  12:59 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- btdisk
// Sub project -- Not defined
// Purpose     -- загрузочные cd/dvd
// Author      -- Sobko
// File Name   -- plugin_btdisk.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_btdisk )
#define __public_plugin_btdisk
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_BTDISK  ((tPID)(61018))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SOBKO  ((tVID)(67))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, OS )
// OS interface implementation
// Short comments -- object system interface

// message class
//! #define pmc_OS_VOLUME 0x75eb704a // сообщения о томах
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, ObjPtr )
// ObjPtr interface implementation
// Short comments -- Object Enumerator (Pointer to Object)
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0x52e986b3 // класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_btdisk
// AVP Prague stamp end




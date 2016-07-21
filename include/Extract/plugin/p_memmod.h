// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  12 February 2004,  13:22 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Process memory scan
// Purpose     -- Доступ к памяти процессов для операционных систем Windows 9x/NT
// Author      -- Sobko
// File Name   -- plugin_memmod.cpp
// Additional info
//    Триада интерфейсов позволяющая енумерировать процессы в памяти, читать и изменять данные в этой памяти
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_memmod )
#define __pligin_memmod
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_MEMSCAN  ((tPID)(61004))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SOBKO  ((tVID)(67))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, OS )
// OS interface implementation
// Short comments -- object system interface

// constants

// ----------  -Нотификационные сообщения  ----------
#define pmc_MEMORY_PROCESS             ((tDWORD)(0x730f3626)) // класс сообщения для нотификации о процессах операционной системы
#define pm_MEMORY_PROCESS_REQUEST_DELETE ((tDWORD)(0x28cedca3)) // запрос на остановку процесса

// message class
//! #define pmc_OS_VOLUME 0x00001000 // (4096) -- сообщения о томах
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, ObjPtr )
// ObjPtr interface implementation
// Short comments -- pointer to the object

// properties
#define plPID         mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plPARENT_PID  mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plTHREADS_NUM mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002002 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// properties
#define plProcessID        mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x000000c0 )
#define plIO_HANDLEPROCESS mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_memmod
// AVP Prague stamp end




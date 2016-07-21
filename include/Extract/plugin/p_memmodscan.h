// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  12 February 2004,  13:20 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- plugin_memmodscan.cpp
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_memmodscan )
#define __pligin_memmodscan
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_MEMMODSCAN  ((tPID)(61001))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SOBKO  ((tVID)(67))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, OS )
// OS interface implementation
// Short comments -- object system interface

// properties
#define plPID mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, ObjPtr )
// ObjPtr interface implementation
// Short comments -- pointer to the object

// properties
#define plENUM_PID mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002000 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// properties
#define plIO_PID     mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002000 )
#define plIO_HMODULE mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002001 )
#define plSize       mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002002 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_memmodscan
// AVP Prague stamp end




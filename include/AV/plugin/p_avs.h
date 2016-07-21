// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  30 January 2007,  14:16 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_avs.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_avs )
#define __public_plugin_avs
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include <AV/iface/i_avs.h>
#include <AV/iface/i_avssession.h>
#include <AV/iface/i_avstreats.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
#define PID_AVS  ((tPID)(15))
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, AVS )
// AVS interface implementation
// Short comments --

// message class
//! #define pmc_AVS 0xe7491cd3 //

// message class
//! #define pmc_MATCH_EXCLUDES 0x521d6e78 //

// message class
//! #define pmc_UPDATE_THREATS_LIST 0xeb3c8145 //
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, AVSSession )
// AVSSession interface implementation
// Short comments --

// message class
//! #define pmc_AVS_SESSION 0x01060d9d //
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, AVSTreats )
// AVSTreats interface implementation
// Short comments --

// constants
#define FILTER_NONE                    ((tDWORD)(0)) //  --
#define FILTER_UNTREATED               ((tDWORD)(1)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// properties
#define plKLAV_IO_OBJECT mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002000 )
#define plSUBST_IO_OBJECT mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002001 )

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0x52e986b3 // класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_avs
// AVP Prague stamp end


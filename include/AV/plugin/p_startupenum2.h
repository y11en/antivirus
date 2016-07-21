// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  03 July 2006,  11:46 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Alexey Antropov
// File Name   -- plugin_startupenum2.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_startupenum2 )
#define __public_plugin_startupenum2
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_decodeio.h>

#include <AV/iface/i_startupenum2.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_STARTUPENUM2  ((tPID)(54000))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ALEX  ((tVID)(74))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, StartUpEnum2 )
// StartUpEnum2 interface implementation
// Short comments --

// properties
#define plADVANCED_DISINFECTION mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )

// message class
//! #define pmc_STARTUPENUM2 0x5d55e35e //

// message class
//! #define pmc_ADVANCED_DISINFECTION 0xbcef66f3 //
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, DecodeIO )
// DecodeIO interface implementation
// Short comments -- input/output interface

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0x52e986b3 // класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
// AVP Prague stamp end

#define pmc_STARTUP_CURE_MESSAGES 0x8C42EE10

	#define pm_STARTUP_HOSTS_MODIFIED 0x8C42EE11

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_startupenum2
// AVP Prague stamp end




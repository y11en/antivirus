// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  08 April 2005,  18:39 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- plugin_mailwasher.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_mailwasher )
#define __public_plugin_mailwasher
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_MAILWASHER  ((tPID)(40027))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_DENISOV  ((tVID)(88))
// AVP Prague stamp end



#define pmc_MAIL_WASHER 0x79062818 /*is a crc32 for "pmc_MAIL_WASHER"*/ 
#define pm_SESSION_NEEDED	0x1e998e5a /*is a crc32 for "pm_SESSION_NEEDED"*/
#define pm_SESSION_BEGIN	0x731edc5a /*is a crc32 for "pm_SESSION_BEGIN"*/
#define pm_SESSION_CLOSE	0x1a9ac88b /*is a crc32 for "pm_SESSION_CLOSE"*/
#define pm_SESSION_END		0x777926e  /*is a crc32 for "pm_SESSION_END"*/
#define pm_MESSAGE_FOUND	0x7dced504 /*is a crc32 for "pm_MESSAGE_FOUND"*/
#define pm_MESSAGE_GETLIST	0xb6c14dae /*is a crc32 for "pm_MESSAGE_GETLIST"*/
#define pm_MESSAGE_REMOVE	0xedc86de6 /*is a crc32 for "pm_MESSAGE_REMOVE"*/
#define pm_MESSAGE_DECODE	0xa06c8b80 /*is a crc32 for "pm_MESSAGE_DECODE"*/
#define pm_MESSAGE_DELETED	0xbf60516a /*is a crc32 for "pm_MESSAGE_DELETED"*/
#define pm_CANCEL_REQUEST	0xd9f725dd /*is a crc32 for "pm_CANCEL_REQUEST"*/

// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_mailwasher
// AVP Prague stamp end




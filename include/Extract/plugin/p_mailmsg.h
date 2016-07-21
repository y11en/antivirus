// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  13 February 2004,  13:56 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- plugin_mailmsg.cpp
// Additional info
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
//
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_mailmsg )
#define __pligin_mailmsg
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>

#include <Extract/iface/i_transformer.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_MAILMSG  ((tPID)(49001))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MEZHUEV  ((tVID)(79))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, OS )
// OS interface implementation
// Short comments -- object system interface

// constants
#define MSG_CONTENT_PLAIN_BODY         ((tSTRING)("PlainBody")) //
#define MSG_CONTENT_RICH_BODY          ((tSTRING)("RichBody")) //
#define MSG_CONTENT_HTML_BODY          ((tSTRING)("HTMLBody")) //

// properties
#define plMAILMSG_MESSAGE    mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002000 )
#define plMAILMSG_ISEMBENDED mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )

// message class
//! #define pmc_OS_VOLUME 0x00001000 // (4096) -- сообщения о томах
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, ObjPtr )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
//! #define pmc_IO 0xda000001 // (3657433089) -- класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, Transformer )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_mailmsg
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Wednesday,  09 April 2003,  11:21 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_io.h
// Additional info
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __msg_io_h__82d1f490_569b_4e1b_9aaa_7ae5b2e8b471 )
#define __msg_io_h__82d1f490_569b_4e1b_9aaa_7ae5b2e8b471
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_mailmsg.h"

#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_string.h>

#include <Extract/iface/i_mailmsg.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IO_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_MsgIO_Data 
{
	tDWORD       Origin;         // --
	tDWORD       OpenMode;       // --
	tDWORD       AccessMode;     // --
	tBOOL        fDeleteOnClose; // --
	hOS          hOS;            // --
	IMessage *   message;        // --
	IAttach *    attach;         // --
	tDWORD       cntype;         // --
	tDWORD       attachnum;      // --
	IStream *    stream;         // --
	tDWORD       objmethod;      // --
	tBOOL        fModified;      // --
	IStorage *   storage;        // --
	ILockBytes * lockbyte;       // --
	IStorage *   tmpstg;         // --
	tBOOL        fSaveChanges;   // --
	hIO          superio;        // --
} MsgIO_Data;


// to get pointer to the structure MsgIO_Data from the hOBJECT obj
#define CUST_TO_MsgIO_Data(object)  ((MsgIO_Data*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // msg_io_h
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  19 March 2003,  13:36 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_objptr.h
// Additional info
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __msg_objptr_h__2d50e86f_43c9_446a_b002_23eb2ecf7782 )
#define __msg_objptr_h__2d50e86f_43c9_446a_b002_23eb2ecf7782
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
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
//    Интерфейс позволяет навигировать по содержимому Outlook сообщения (тела, аттачменты, вложенные сообщения, OLE объекты). Объект может быть создан следующими способами:
//    - вызов ObjectCreate с родителем OS.
//    - вызов PtrCreate интерфейса OS.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call ObjPtr_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_MsgEnum_Data 
{
	tDWORD       Origin;    // --
	hOS          hOS;       // --
	tDWORD       fFolder;   // --
	tDWORD       ptrState;  // --
	IMAPITable * table;     // --
	IAttach *    attach;    // --
	IMessage *   message;   // --
	tDWORD       cntype;    // --
	tDWORD       attachnum; // --
//! 	tDWORD       support;   // --
} MsgEnum_Data;


// to get pointer to the structure MsgEnum_Data from the hOBJECT obj
#define CUST_TO_MsgEnum_Data(object)  ((MsgEnum_Data*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // msg_objptr_h
// AVP Prague stamp end




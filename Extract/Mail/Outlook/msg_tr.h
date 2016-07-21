// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  20 December 2002,  16:22 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_tr.h
// Additional info
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __msg_tr_h__d5161d9b_62be_4396_8d95_6c9c5e2327fa )
#define __msg_tr_h__d5161d9b_62be_4396_8d95_6c9c5e2327fa
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_mailmsg.h"

#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_string.h>

#include <Extract/iface/i_transformer.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
//    Техническая реализация данного интерфейса. Предназначена для буферизации OLE Stream.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Transformer_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure

typedef struct tag_MsgTR_Data 
{
	hIO     io;     // --
	hSEQ_IO iseqio; // --
	hSEQ_IO oseqio; // --
} MsgTR_Data;


// to get pointer to the structure MsgTR_Data from the hOBJECT obj
#define CUST_TO_MsgTR_Data(object)  ((MsgTR_Data*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // msg_tr_h
// AVP Prague stamp end




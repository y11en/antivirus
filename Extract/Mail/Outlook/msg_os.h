// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  19 March 2003,  13:31 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_os.h
// Additional info
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __msg_os_h__733536b4_bfb1_4de5_82e3_b4e4f15f7f26 )
#define __msg_os_h__733536b4_bfb1_4de5_82e3_b4e4f15f7f26
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

#include <Extract/iface/i_mailmsg.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define MSG_CONTENT_PLAIN_BODY         ((tSTRING)("PlainBody")) //  --
#define MSG_CONTENT_RICH_BODY          ((tSTRING)("RichBody")) //  --
#define MSG_CONTENT_HTML_BODY          ((tSTRING)("HTMLBody")) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
//    Interface defines behavior of an object system. It responds for
//      - enumerate IO and Folder objects by creating Folder objects.
//      - create and delete IO and Folder objects
//    There are two possible ways to create or open IO object on OS. First is three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//    Interface defines behavior of an object system. It responds for
//      - enumerate IO and Folder objects by creating Folder objects.
//      - create and delete IO and Folder objects
//    There are two possible ways to create or open IO object on OS. First is three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
// Short comments -- object system interface
//    Интерфейс предназначен для построения объектной системы на основе Outlook сообщения.
//
//    Способы создания объекта OS:
//    - при инициализации нужно указать свойство MAILMSG_MESSAGE (IMAPIMessage*)
//    - указать объект MDB_IO в качестве родителя в вызове ObjectCreate
//
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call OS_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface OS. Inner data structure

typedef struct tag_Msg_Data 
{
	IMessage * message;        // --
	hIO        hIO;            // --
	tBOOL      isembended;     // --
	tBOOL      fSaveChanges;   // --
	hObjPtr    tmpenum;      // --
} Msg_Data;


// to get pointer to the structure Msg_Data from the hOBJECT obj
#define CUST_TO_Msg_Data(object)  ((Msg_Data*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plMAILMSG_MESSAGE    mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002000 )
   #define plMAILMSG_ISEMBENDED mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // msg_os_h
// AVP Prague stamp end




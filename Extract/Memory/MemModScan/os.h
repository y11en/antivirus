// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  09 April 2003,  12:03 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- os.h
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __os_h__d818582f_a435_4023_9901_0cd702581c14 )
#define __os_h__d818582f_a435_4023_9901_0cd702581c14
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_memmodscan.h"
#include "objptr.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_buffer.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include "../MemScan/plugin_memmod.h"
#include "../MemScan/objptr.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
// Short comments -- object system interface
//    Interface defines behavior of an object system. It responds for
//      - enumerate IO and Folder objects by creating Folder objects.
//      - create and delete IO and Folder objects
//    There are two possible ways to create or open IO object on OS. First is three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call OS_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure OsPrivate is declared in O:\Prague\MemModScan\os.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plPID mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // os_h
// AVP Prague stamp end




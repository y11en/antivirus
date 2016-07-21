// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  08 April 2003,  20:42 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Process memory scan
// Purpose     -- Доступ к памяти процессов для операционных систем Windows 9x/NT
// Author      -- Sobko
// File Name   -- os.h
// Additional info
//    Триада интерфейсов позволяющая енумерировать процессы в памяти, читать и изменять данные в этой памяти
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __os_h__9c14f611_ae09_40da_9ce7_ed836e314d65 )
#define __os_h__9c14f611_ae09_40da_9ce7_ed836e314d65
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_memmod.h"

#include <Prague/iface/i_string.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// -Нотификационные сообщения
#define pmc_MEMORY_PROCESS             ((tDWORD)(0x730f3626)) // класс сообщения для нотификации о процессах операционной системы
#define pm_MEMORY_PROCESS_REQUEST_DELETE ((tDWORD)(0x28cedca3)) // запрос на остановку процесса
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
  // data structure PM_OsPrivate is declared in O:\Prague\MemScan\os.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // os_h
// AVP Prague stamp end




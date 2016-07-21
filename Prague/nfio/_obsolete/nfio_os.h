// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  27 August 2002,  16:15 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Novell NW
// Sub project -- Not defined
// Purpose     -- NetWare native file IO
// Author      -- petrovitch
// File Name   -- nw_os.h
// Additional info
//    File input/output for Novell NetWare
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __nw_os_h__aa395e2b_b9d4_11d6_8d36_000347e693e8 )
#define __nw_os_h__aa395e2b_b9d4_11d6_8d36_000347e693e8
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#if defined (__MWERKS__)
	#include <nwfattr.h>
	#define MAX_PATH _MAX_PATH
#endif
	
#include <iface/i_root.h>
#include <iface/i_os.h>
#include "plugin_nfio.h"
#include <iface/i_string.h>
#include <iface/i_io.h>
#include <iface/i_objptr.h>
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
// data structure OS_Data is declared in D:\Prague\NFIO\nw_os.c source file
  typedef struct tag_OS_Data {
	int nfio_errno; // NW last error
} OS_Data;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plNATIVE_ERR mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end

#define MB(a)            ((tCHAR*)(a))
#define MBS               sizeof(tCHAR)

// AVP Prague stamp begin( Header endif,  )
#endif // nw_os_h
// AVP Prague stamp end

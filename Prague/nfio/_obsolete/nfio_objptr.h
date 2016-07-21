// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  30 April 2002,  12:17 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_objptr.h
// Additional info
//  File input/output for Win32
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __win32_objptr_h__15b2ee68_a848_4de0_90d5_698d9e82ed4c )
#define __win32_objptr_h__15b2ee68_a848_4de0_90d5_698d9e82ed4c
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <iface/i_root.h>
#include <iface/i_objptr.h>
#include <iface/i_string.h>
#include <iface/i_os.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_NW_NFIO	((tPID)(3))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
#if defined( __cplusplus )
//extern "C" {
#endif

	tERROR pr_call ObjPtr_Register( hROOT root );

#if defined( __cplusplus )
//}
#endif
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct tag_ObjPtrData {
	tCHAR            mask_buff[30]; // file mask to enumerate
	tUINT            mask_len;      // length of current mask buffer
	tCHAR*           mask;          // pointer to the amsk
	tCHAR*           full;          // full path string (with unicode prefix possible)
	tDWORD           full_len;      // length of the full buffer allocated
	tCHAR*           path;          // path without prefix
	tDWORD           name_len;      // length of the name
	DIR*			 dirp;          // find directory handle
	tObjPtrState     state;         // state of the pointer
	hOS              os;            // based on this object
	int              nfio_errno;    // last win32 error
	tDWORD           origin;        // object origin

	// Attributes, etc.
	struct dirent*   dentry;        // find data
	struct stat		 dstat;
	
} ObjPtrData;

#if defined( __cplusplus )
}
#endif

// get pointer to the structure ObjPtrData from the hOBJECT obj
#define CUST_TO_ObjPtrData(object)  ((ObjPtrData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
   #define plNATIVE_ERR mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
   #define plMASK_CP    mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // win32_objptr_h
// AVP Prague stamp end

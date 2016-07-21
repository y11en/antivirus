// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  20 April 2002,  13:19 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// Copyright (c) Kaspersky Labs. 1996-2000.
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- NW native file IO
// Author      -- petrovitch/tag
// File Name   -- nw_io.h
//  File input/output for NW
// Additional info
//   File input/output for NW

#if defined (__MWERKS__)
	#define PATH_SEPARATOR	'\\'
#else
	#define PATH_SEPARATOR	'/'
#endif	

// AVP Prague stamp begin( IO, Interface constants )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __win32_io_h__3ad6132a_88c8_4d83_9d75_e32a9df15151 )
#define __win32_io_h__3ad6132a_88c8_4d83_9d75_e32a9df15151
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <iface/i_root.h>
#include <iface/i_io.h>
#include <iface/i_string.h>
#include <iface/i_os.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_NW_NFIO  ((tPID)(3))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// --- 74d331e1_e06c_11d3_bf0e_00d0b7161fb8 ---
// IO interface implementation
// Extended comment -
// Short comments -- input/output interface
// Extended comment
//   Defines behavior of input/output of an object
//   Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
#if defined( __cplusplus )
extern "C" {
#endif

	tERROR pr_call IO_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct tag_NFIO_IOData {
	tCHAR*   full_name;          // full name buffer
	tDWORD   full_name_buff_len; // length of the buffer
	tSTRING  ppath;              // pointer to the full path
	tSTRING  pname;              // pointer to the file name
	int		 fd;             	 // file handle
	tQWORD   cp;                 // current file position
	tDWORD   open_mode;          // open mode
	tDWORD   access_mode;        // file attributes
	tQWORD   size;               // file size
	tDWORD   open_attr;          // open time file attributes
	tBOOL    del_on_close;       // must be deleted on close
	hOS      os;                 // owner os
	int		 nfio_errno;         	 // last errno
	tDWORD   origin;             // object origin
} NFIO_IOData;

#if defined( __cplusplus )
}
#endif

// get pointer to the structure Win32IOData from the hOBJECT obj
#define CUST_TO_NFIO_IOData(object)  ((NFIO_IOData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
   #define plNATIVE_ERR mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end

#define MB(a)            ((tCHAR*)(a))
#define MBS               sizeof(tCHAR)

#ifndef MAX_PATH
	#define MAX_PATH _MAX_PATH
#endif
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // nw_io_h
// AVP Prague stamp end

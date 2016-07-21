// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  13 May 2004,  20:13 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_objptr.c
// Additional info
//    File input/output for Win32
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __win32_objptr_c__122c7906_1193_43f3_b914_38e6a5ea092f )
#define __win32_objptr_c__122c7906_1193_43f3_b914_38e6a5ea092f
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/plugin/p_win32_nfio.h>

#include <wincompat.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- Object Enumerator (Pointer to Object)
//     Defines behavior of an object container/enumerator
//      Supplies object enumerate functionality on one level of hierarchy only.
//      For example: on logical drive object of this interface must enumerate objects in one folder.
//
//      Also must support creating new IO object in the container. There are two ways to do it. First is three steps protocol:
//         -- ObjectCreate system method with folder object as parent
//         -- dictate all necessary properties for the new object
//         -- ObjectCreateDone
//       Second is CreateNew folder's method which is wrapper of above protocol.
//       Advise: when caller invokes ObjectCreateDone method folder object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call ObjPtr_Register( hROOT root );
// AVP Prague stamp end


typedef struct tag_hi_ObjPtr *hi_ObjPtr;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure ObjPtrData is declared in o:\prague\nfio\win32_objptr.c source file
// Interface ObjPtr. Inner data structure
typedef struct tag_ObjPtrData {
	tCHAR            mask_buff[30]; // file mask to enumerate
	tUINT            mask_len;      // length of current mask buffer
	tCHAR*           mask;          // pointer to the amsk
	tBOOL            apply_mask_to_folders;        // is mask applied to folder names
	tBOOL            curr_mask_applied_to_folders; // is mask applied to folder names for current session
	tCHAR*           full;          // full path string (with unicode prefix possible)
	tDWORD           full_len;      // length of the full buffer allocated
	tCHAR*           m_ppath;       // pointer to path without prefix
	tDWORD           name_len;      // length of the name
	HANDLE           handle;        // find handle
	WIN32_FIND_DATAW fd;            // Win32 find data
	tObjPtrState     state;         // state of the pointer
	hOS              os;            // based on this object
	tDWORD           win32err;      // last win32 error
	tDWORD           origin;        // object origin
	tUINT            folder_step;   // is cTRUE when we are stepping through folders in 2 phase enumeration
	tBOOL            m_do_not_make_prefix; // don't make "\\?\" or "\\?\\UNC\" if possible
	tBOOL            m_dot_passed; // we have found one dot on zero index
	tUINT            m_index;
	tERROR          (*go_func)(hi_ObjPtr); // function for the next step

	#if defined( _PRAGUE_TRACE_ )
		tCHAR*         m_generated_mask;     // mask generated for lastest enumeration
		tUINT          m_generated_mask_len; // 
	#endif
} ObjPtrData;

// to get pointer to the structure ObjPtrData from the hOBJECT obj
#define CUST_TO_ObjPtrData(object)  ((ObjPtrData*)( (((tPTR*)object)[2]) ))
//! #define CUST_TO_ObjPtrData(object)  ((ObjPtrData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
struct tag_hi_ObjPtr {
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	ObjPtrData*        data; // pointer to the "ObjPtr" data structure
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // win32_objptr_c
// AVP Prague stamp end




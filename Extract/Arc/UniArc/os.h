// -------- Monday,  23 October 2000,  15:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Unversal Extractor
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- os.h
// -------------------------------------------



#if !defined( __os_h__31a0506d_a481_4e09_b5dd_ebedf8328354 )
#define __os_h__31a0506d_a481_4e09_b5dd_ebedf8328354



#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_list.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_tree.h>

#include <Extract/iface/i_minarc.h>


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Interface constants )
// AVP Prague stamp end( OS, Interface constants )
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( OS, Vendor identifier )
// --------------------------------------------------------------------------------

#define cUNIARC_PROPERTY "UniArcChecked" 


#define UNI_MSG_FILE_CLOSE_CHANGE    ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0001)
#define UNI_MSG_FILE_CLOSE           ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0002)
#define UNI_MSG_FILE_DELETE          ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0003)
#define UNI_MSG_FILE_NEW             ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0004)
#define UNI_MSG_FILE_RENAME          ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0005)
#define UNI_MSG_FILE_OPEN            ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0006)
#define UNI_MSG_FILE_DELETE_NO_CHECK ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0007)
#define UNI_MSG_FILE_DELETE_ON_CLOSE ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0008)
#define UNI_MSG_FILE_CLOSE_KEEP_OPEN ((PID_UNIVERSAL_ARCHIVER<<16) + 0x0009)
#define UNI_MSG_FILE_RW              ((PID_UNIVERSAL_ARCHIVER<<16) + 0x000A)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Interface comment )
// --------------------------------------------
// --- 213d8b50_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
// OS interface implementation
// Short comments -- object system interface
// Extended comment
//   Interface defines behavior of an object system. It responds for
//     - enumerate IO and Folder objects by creating Folder objects.
//     - create and delete IO and Folder objects
//   There are two possible ways to create or open IO object on OS. First is three steps protocol:
//     -- call ObjectCreate system method and get new IO (Folder) object
//     -- dictate properties of new object
//     -- call ObjectCreateDone system method and have working object
//   Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//   Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//   
// AVP Prague stamp end( OS, Interface comment )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Data structure )
// Interface OS. Inner data structure
typedef struct tag_Uni_OS_Data 
{
	hIO    hARC_IO;       // --
	tBOOL  IsArcModified; // --
	hTREE  hChangeTree;   // --
	hTREE  hDeletedTree;  // --
	hTREE  hNewTree;      // --
	hTREE  hOpenTree;     // --
	hLIST  hRenamedList;
	tQWORD qwMagiq;       // --
	hMINIARC hArc;
	tBOOL  bIsReadOnly;
	tCHAR* szListData;      // --	
	tCHAR* szNameBuffer;
	tCHAR* szNewNameBuffer;
	tBOOL  IsWritable;
	hOBJECT hBasedObject;
	tBOOL   bRealChange;
	tPID   pidMiniArc;
	tBOOL  bScanAllVolumes;
	tWORD  wDelimeter;
	tBOOL  bObjectInitDone;
    tERROR errObjectInitDone;
} OS_Uni_Data;
// AVP Prague stamp end( OS, Data structure )
// --------------------------------------------------------------------------------

// Defines for IsWritable
#define CAN_WRITE  1
#define CANT_WRITE 0
#define DONT_KNOW  2


// AVP Prague stamp begin( OS, object declaration )
typedef struct tag_hi_OS 
{
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	OS_Uni_Data*          data;   // pointer to the "OS" data structure
} *hi_OS;
// AVP Prague stamp end( OS, object declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif

	tERROR pr_call OS_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( OS, Register call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Property table )
// AVP Prague stamp end( OS, Property table )
// --------------------------------------------------------------------------------



#endif // os_h




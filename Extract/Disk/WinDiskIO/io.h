// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  08 July 2003,  14:38 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- „тение секторов дл€ операционной системы Windows NT
// Author      -- Sobko
// File Name   -- io.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __io_h__ec1e2349_b30b_4637_9425_ce94e2582599 )
#define __io_h__ec1e2349_b30b_4637_9425_ce94e2582599
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <IFace/i_root.h>
#include <IFace/i_io.h>
#include "plugin/p_windiskio.h"
#include <IFace/i_string.h>
#include <pr_oid.h>
#include <IFace/i_seqio.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// -- Disks types
#define cFILE_DEVICE_CD_ROM            ((tDWORD)(0x00000002)) //  --
#define cFILE_DEVICE_DISK              ((tDWORD)(0x00000007)) //  --
#define cFILE_DEVICE_NETWORK           ((tDWORD)(0x00000012)) //  --
#define cFILE_DEVICE_TAPE              ((tDWORD)(0x0000001f)) //  --
#define cFILE_DEVICE_UNKNOWN           ((tDWORD)(0x00000022)) //  --
#define cFILE_DEVICE_MASS_STORAGE      ((tDWORD)(0x0000002d)) //  --
#define cFILE_DEVICE_CHANGER           ((tDWORD)(0x00000030)) //  --
#define cFILE_DEVICE_DVD               ((tDWORD)(0x00000033)) //  --

// -Mmedia types
#define cMEDIA_TYPE_FIXED              ((tDWORD)(12)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IO_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure Private is declared in O:\Prague\WinDiskIO\io.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plPartitionType       mPROPERTY_MAKE_LOCAL( pTYPE_BYTE    , 0x00002000 )
   #define plBootIndicator       mPROPERTY_MAKE_LOCAL( pTYPE_BYTE    , 0x00002001 )
   #define plRecognizedPartition mPROPERTY_MAKE_LOCAL( pTYPE_BYTE    , 0x00002002 )
   #define plCylinders           mPROPERTY_MAKE_LOCAL( pTYPE_QWORD   , 0x00002003 )
   #define plTracksPerCylinder   mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002004 )
   #define plSectorsPerTrack     mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002005 )
   #define plBytesPerSector      mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002006 )
   #define plMediaType           mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002007 )
   #define plDriveType           mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002008 )
   #define plDriveNumber         mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002009 )
   #define plIsPartition         mPROPERTY_MAKE_LOCAL( pTYPE_BOOL   , 0x0000200a )
   #define plIsReadOnly          mPROPERTY_MAKE_LOCAL( pTYPE_BOOL   , 0x0000200b )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // io_h
// AVP Prague stamp end




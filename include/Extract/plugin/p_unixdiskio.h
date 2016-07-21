// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  20 May 2005,  18:12 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Low level disk I/O for Unix-like systems
// Author      -- Sychev
// File Name   -- plugin_unixdiskio.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_unixdiskio )
#define __public_plugin_unixdiskio
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_objptr.h>
#include <iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_UNIXDISKIO  ((tPID)(61010))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SYCHEV  ((tVID)(85))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, ObjPtr )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, IO )
// IO interface implementation
// Short comments -- input/output interface

// constants
#define cFILE_DEVICE_CD_ROM            ((tDWORD)(0x00000002)) //
#define cFILE_DEVICE_DISK              ((tDWORD)(0x00000007)) //
#define cFILE_DEVICE_NETWORK           ((tDWORD)(0x00000012)) //
#define cFILE_DEVICE_TAPE              ((tDWORD)(0x0000001f)) //
#define cFILE_DEVICE_UNKNOWN           ((tDWORD)(0x00000022)) //
#define cFILE_DEVICE_MASS_STORAGE      ((tDWORD)(0x0000002d)) //
#define cFILE_DEVICE_CHANGER           ((tDWORD)(0x00000030)) //
#define cFILE_DEVICE_DVD               ((tDWORD)(0x00000033)) //
#define cMEDIA_TYPE_FIXED              ((tDWORD)(12)) //
#define cMEDIA_TYPE_REMOVABLE          ((tDWORD)(11)) //

// properties
#define plPartitionType                                mPROPERTY_MAKE_LOCAL( pTYPE_BYTE    , 0x00002000 )
#define plBootIndicator                                mPROPERTY_MAKE_LOCAL( pTYPE_BYTE    , 0x00002001 )
#define plRecognizedPartition                          mPROPERTY_MAKE_LOCAL( pTYPE_BYTE    , 0x00002002 )
#define plCylinders                                    mPROPERTY_MAKE_LOCAL( pTYPE_QWORD   , 0x00002003 )
#define plTracksPerCylinder                            mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002004 )
#define plSectorsPerTrack                              mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002005 )
#define plBytesPerSector                               mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002006 )
#define plMediaType                                    mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002007 )
#define plDriveType                                    mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002008 )
#define plDriveNumber                                  mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002009 )
#define plIsPartition                                  mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x0000200a )

// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_unixdiskio
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{{ (tDATA)&exDiskExQueryLockStatus, PID_UNIXDISKIO, 0x53f7e5f5l }},
{{ (tDATA)&exDiskExLogical_Next, PID_UNIXDISKIO, 0x90fc065fl }},
{{ (tDATA)&exDiskExLogical_GetDriveType, PID_UNIXDISKIO, 0xe15c41b7l }},
{{ (tDATA)&exDiskExIsPartition, PID_UNIXDISKIO, 0x87596b74l }},
{{ (tDATA)&exDiskExQueryDriveID, PID_UNIXDISKIO, 0xd62a88d6l }},
{{ (tDATA)&exDiskExIsSubst, PID_UNIXDISKIO, 0x75bad56el }},

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <iface/impexhlp.h>

IMPEX_DECL tBOOL  IMPEX_FUNC(exDiskExQueryLockStatus)( tCHAR p_DrvName, tBOOL* p_pbLocked )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(exDiskExLogical_Next)( hSTRING p_hStrLogical )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(exDiskExLogical_GetDriveType)( hSTRING p_hStrLogical, tDWORD* p_pDriveType )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(exDiskExIsPartition)( tCHAR p_DrvName, tBOOL* p_pbIsPartition )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(exDiskExQueryDriveID)( tSTRING p_DrvName, tDWORD* p_DriveID, tDWORD p_DriveIdLen )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(exDiskExIsSubst)( hSTRING p_hStrLogical )IMPEX_INIT;
#endif
// AVP Prague stamp end




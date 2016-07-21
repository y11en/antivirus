// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  22 November 2005,  13:40 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- plugin_fsdrvplgn.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_fsdrvplgn )
#define __public_plugin_fsdrvplgn
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
// AVP Prague stamp end



#include <Prague/iface/i_string.h>


// AVP Prague stamp begin( Plugin identifier,  )
#define PID_FSDRVPLGN  ((tPID)(49027))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MEZHUEV  ((tVID)(79))
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_fsdrvplgn
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDWORD)&FSMoveFileEx, PID_FSDRVPLGN, 0xbfe7a919l },
{ (tDWORD)&FSGetDriveType, PID_FSDRVPLGN, 0xb47e23e2l },
{ (tDWORD)&FSGetDiskStat, PID_FSDRVPLGN, 0x4d68a298l },
{ (tDWORD)&FSIsProcessedOnReboot, PID_FSDRVPLGN, 0x30b16322l },
{ (tDWORD)&FSDirectRead, PID_FSDRVPLGN, 0x8eae006el },
{ (tDWORD)&FSDirectOpen, PID_FSDRVPLGN, 0x8eae006fl },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tERROR IMPEX_FUNC(FSMoveFileEx)( hSTRING p_strSrc, hSTRING p_strDst, tDWORD p_dwFlags )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FSGetDriveType)( hSTRING p_strDrive )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FSGetDiskStat)( tQWORD* p_pTimeCurrent, tQWORD* p_pTotalWaitTime )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FSIsProcessedOnReboot)( hSTRING p_hObj )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FSDirectRead)( tPTR p_NativeHandle, tQWORD p_Offset, tPTR p_Buffer, tDWORD p_BufferSize, tDWORD* p_RetSize )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FSDirectOpen)( tPTR* p_NativeHandle, tWCHAR* p_wName )IMPEX_INIT;
#endif
// AVP Prague stamp end




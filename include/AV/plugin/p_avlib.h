// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  13 January 2006,  10:24 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AV Engine
// Purpose     -- AV Library
// Author      -- Graf
// File Name   -- plugin_avlib.c
// Additional info
//    Реализует сервисные функции для антивируса, такие как IsProgram, которые предполагают периодическое обновление.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_avlib )
#define __public_plugin_avlib
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_AVLIB  ((tPID)(58901))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



#ifndef _AVLIB_CONSTS_
#define _AVLIB_CONSTS_
#define cAVLIB_EXECUTABLE_EXTENSION          (1<<0)
#define cAVLIB_EXECUTABLE_FORMAT             (1<<1)
#define cAVLIB_STREAM                        (1<<2)
#define cAVLIB_ARCHIVE_FORMAT                (1<<3)
//#define cAVLIB_FILE_FORMAT_MODULE          (1<<8)
//#define cAVLIB_FILE_FORMAT_MACRO           (1<<8)
//#define cAVLIB_FILE_FORMAT_SCRIPT          (1<<8)
//#define cAVLIB_FILE_FORMAT_EMBED           (1<<8)
//#define cAVLIB_FILE_FORMAT_MAIL_DATABASE   (1<<8)
//#define cAVLIB_FILE_FORMAT_ARCHIVE         (1<<8)
#endif

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_avlib
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDATA)&AVLIB_IsProgram, PID_AVLIB, 0xa70e49cal },
{ (tDATA)&AVLIB_IsExecutableExtension, PID_AVLIB, 0x9a9398c5l },
{ (tDATA)&AVLIB_IsExecutableFormat, PID_AVLIB, 0xb565f870l },
{ (tDATA)&AVLIB_IsExecutableExtensionBuff, PID_AVLIB, 0x2d30b9a2l },
{ (tDATA)&AVLIB_IsExecutableFormatBuff, PID_AVLIB, 0xa67cb748l },
{ (tDATA)&AVLIB_PswHeuristic, PID_AVLIB, 0xf01e4c22l },
{ (tDATA)&AVLIB_IsExecutableFormatLite, PID_AVLIB, 0xb262bb06l },
{ (tDATA)&AVLIB_IsExecutableFormatBuffLite, PID_AVLIB, 0xe3854973l },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_IsProgram)( hIO p_io, tDWORD* p_pdwResult )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_IsExecutableExtension)( hIO p_io, tDWORD* p_pdwResult )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_IsExecutableFormat)( hIO p_io, tDWORD* p_pdwResult )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_IsExecutableExtensionBuff)( tBYTE* p_pExt, tDWORD p_dwSize, tDWORD* p_pdwResult )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_IsExecutableFormatBuff)( tBYTE* p_pData, tDWORD p_dwSize, tDWORD* p_pdwResult )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_PswHeuristic)( hOS p_os, hObjPtr p_objptr, hIO* p_ioptr )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_IsExecutableFormatLite)( hIO p_io, tDWORD* p_pdwResult )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AVLIB_IsExecutableFormatBuffLite)( tBYTE* p_pData, tDWORD p_dwSize, tDWORD* p_pdwResult )IMPEX_INIT;
#endif
// AVP Prague stamp end




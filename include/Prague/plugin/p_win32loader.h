// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  17 February 2006,  15:06 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- plugin_win32loader.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_win32loader )
#define __public_plugin_win32loader
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_loader.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_iface.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/iface/i_semaph.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_event.h>
#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_token.h>
#include <Prague/iface/i_scheck.h>
#include <Prague/iface/i_schecklist.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
#define PID_WIN32LOADER  ((tPID)(5))
// AVP Prague stamp end

#define PID_WIN32LOADER_2LEVEL_CSECT  ((tPID)(16))


// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Loader )
// Loader interface implementation
// Short comments -- prague loadable plugin finder

// constants

#define PLUGIN_LOAD_TIMEOUT            ((tDWORD)(3000)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Plugin )
// Plugin interface implementation
// Short comments -- interface of load/unload plugins

// properties
#define plMODULE_INSTANCE mPROPERTY_MAKE_LOCAL( pTYPE_PTR   , 0x00002000 )
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, IFace )
// IFace interface implementation
// Short comments -- returns interface info
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Mutex )
// Mutex interface implementation
// Short comments -- Mutex behaviour
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Semaphore )
// Semaphore interface implementation
// Short comments -- Semaphore behaviour
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, CriticalSection )
// CriticalSection interface implementation
// Short comments -- Critical section behaviour

// properties
#define plALLOCATOR mPROPERTY_MAKE_LOCAL( pTYPE_FUNC_PTR, 0x00002000 )
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Event )
// Event interface implementation
// Short comments -- Event behaviour
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Heap )
// Heap interface implementation
// Short comments -- распределение, освобождение памяти

// properties
#define plREQUESTED_HEAP_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plWIN32_HEAP_SIZE     mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plALLOCATED_POINTERS  mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
#define plEARLY_INITIALIZED   mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002003 )
#define plGLOBAL_ALLOCATED    mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002004 )
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, Token )
// Token interface implementation
// Short comments --

// properties
#define plPROCESS_ID              mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plTRY_IMPERSONATE_TO_USER mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plTOKEN_TYPE              mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
// AVP Prague stamp end

#define TOKEN_TYPE_UNKNOWN        0
#define TOKEN_TYPE_USER           1

// AVP Prague stamp begin( Plugin public definitions, scheck )
// SCheck interface implementation
// Short comments -- check digitally signet objects

// properties
#define plDSKM_GLOBAL_APP_ID mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, SCheckList )
// SCheckList interface implementation
// Short comments --

// properties
#define plDSKM_LIST_OWNER mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin public definitions, scheck_list )
// scheck_list interface implementation
// Short comments --

// properties
#define plDSKM_LIST_OWNER mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface comment,  )

// Loader interface implementation
// Short comments -- prague loadable plugin finder
//    Supplies facility to find prague loadable plugins. Must create objects of "Plugin" interafce. One object to one loadable plugin


// Plugin interface implementation
// Short comments -- interface of load/unload plugins
//    Supplies facilities :
//      -- to load/unload certain plugin
//      -- to get shared interface properties implemented by plugin


// IFace interface implementation
// Short comments -- returns interface info
//    returns interface info:
//      -- interface name (optional)
//      -- interface types
//      -- interface constants
//      -- interface error codes
//      -- interface properties
//      -- count of interface external methods
//      -- interface external methods by index
//         -- return value type
//         -- count of parameters
//         -- parameter type by index
//      -- count of implemented interface internal methods
//      -- interface internal methods
//         -- return value type
//         -- count of parameters
//         -- parameter type by index
//


// Mutex interface implementation
// Short comments -- Mutex behaviour


// Semaphore interface implementation
// Short comments -- Semaphore behaviour


// CriticalSection interface implementation
// Short comments -- Critical section behaviour


// Event interface implementation
// Short comments -- Event behaviour


// Heap interface implementation
// Short comments -- распределение, освобождение памяти
//    Интерфейс предоставляет услуги по распределению, перераспределению и освобождению регионов памяти.
// AVP Prague stamp end

// AVP Prague stamp begin( ,  )

// Loader declarations

// Plugin declarations

// IFace declarations

// Mutex declarations

// Semaphore declarations

// CriticalSection declarations

// Event declarations

// Heap declarations

// Loader declarations

// Plugin declarations

// IFace declarations

// Mutex declarations

// Semaphore declarations

// CriticalSection declarations

// Event declarations

// Heap declarations

// Loader declarations

// Plugin declarations
#define plMODULE_INSTANCE     mPROPERTY_MAKE_LOCAL( pTYPE_PTR   , 0x00002000 )

// IFace declarations

// Mutex declarations

// Semaphore declarations

// CriticalSection declarations
#define plALLOCATOR           mPROPERTY_MAKE_LOCAL( pTYPE_FUNC_PTR, 0x00002000 )

// Event declarations

// Heap declarations
#define plREQUESTED_HEAP_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plWIN32_HEAP_SIZE     mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plALLOCATED_POINTERS  mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
#define plEARLY_INITIALIZED   mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002003 )
// AVP Prague stamp end

#define plWIN32_HEAP_TAG      mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002004 )


// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_win32loader
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDATA)&Now, PID_WIN32LOADER, 0xa941ea2fl },
{ (tDATA)&NowGreenwich, PID_WIN32LOADER, 0x3a5a2bbel },
{ (tDATA)&PrConvertDate, PID_WIN32LOADER, 0xe7894275l },
{ (tDATA)&SignCheck, PID_WIN32LOADER, 0x25f0bac6l },
{ (tDATA)&PrOutputDebugString, PID_WIN32LOADER, 0x316657e6l },
{ (tDATA)&PrOutputDebugString_v, PID_WIN32LOADER, 0x38b72636l },
{ (tDATA)&CheckStackAvail, PID_WIN32LOADER, 0x259d3150l },
{ (tDATA)&GetThreadId, PID_WIN32LOADER, 0xd758e9b3l },
{ (tDATA)&GetCallerId, PID_WIN32LOADER, 0x97bbdd5dl },
// ---------------------------
// debug info methods
{ (tDATA)&PrDbgGetCallerInfo, PID_WIN32LOADER, 0xe0bdadefl },
{ (tDATA)&PrDbgGetInfoEx, PID_WIN32LOADER, 0x9e6207c7l },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tERROR IMPEX_FUNC(Now)( tDATETIME* p_dt )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(NowGreenwich)( tDATETIME* p_dt )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(PrConvertDate)( const tDATETIME* p_dt )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(SignCheck)( hOBJECT p_obj, tBOOL p_use_default_key, tPTR* p_keys, tDWORD p_key_count, tBOOL p_all_of, tDWORD* p_sign_lib_error_code )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(PrOutputDebugString)( tCHAR* p_format )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(PrOutputDebugString_v)( tCHAR* p_format, tPTR p_params )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(CheckStackAvail)( tDWORD p_probe )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(GetThreadId)()IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(GetCallerId)( const tVOID* p_stack_ptr, tDWORD p_ids, tDWORD p_count )IMPEX_INIT;
// ---------------------------
// debug info methods
IMPEX_DECL tERROR IMPEX_FUNC(PrDbgGetCallerInfo)( tCHAR* p_szSkipModulesList, tDWORD p_dwSkipLevels, tCHAR* p_pBuffer, tDWORD p_dwSize )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(PrDbgGetInfoEx)( tDWORD p_dwAddress, tCHAR* p_szSkipModulesList, tDWORD p_dwSkipLevels, tDWORD p_dwInfoType, tCHAR* p_pBuffer, tDWORD p_dwSize, tDWORD* p_pFFrame )IMPEX_INIT;
#endif
// AVP Prague stamp end


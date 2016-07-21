// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  14 April 2005,  18:08 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sychev
// File Name   -- plugin_vmarea.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_vmarea )
#define __public_plugin_vmarea
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_simple.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_VMAREA  ((tPID)(43004))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SYCHEV  ((tVID)(85))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, SimpleObject )
// SimpleObject interface implementation
// Short comments -- an area of virtual memory

// constants
#define PROTECTION_NOACCESS            ((tDWORD)(0)) //  --
#define PROTECTION_READ                ((tDWORD)(1<<0)) //  --
#define PROTECTION_WRITE               ((tDWORD)(1<<1)) //  --
#define PROTECTION_EXECUTE             ((tDWORD)(1<<2)) //  --

// properties
#define plSIZE       mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plPROTECTION mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plADDRESS    mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002002 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_vmarea
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDATA)&vm_allocate, PID_VMAREA, 0xd6ec51edl },
{ (tDATA)&vm_deallocate, PID_VMAREA, 0xa428b8e8l },
{ (tDATA)&vm_protect, PID_VMAREA, 0x3bee350cl },
{ (tDATA)&vm_lock, PID_VMAREA, 0xba363ae3l },
{ (tDATA)&vm_unlock, PID_VMAREA, 0x20e59389l },
{ (tDATA)&vm_pagesize, PID_VMAREA, 0xe7d4c253l },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tPTR IMPEX_FUNC(vm_allocate)( tDWORD p_aSize, tDWORD p_aProtection, tPTR p_aStartAddress )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(vm_deallocate)( tPTR p_anAddress, tDWORD p_aSize )IMPEX_INIT;
IMPEX_DECL tBOOL IMPEX_FUNC(vm_protect)( tPTR p_anAddress, tDWORD p_aSize, tDWORD p_aProtection )IMPEX_INIT;
IMPEX_DECL tBOOL IMPEX_FUNC(vm_lock)( tPTR p_anAddress, tDWORD p_aSize )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(vm_unlock)( tPTR p_anAddress, tDWORD p_aSize )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(vm_pagesize)()IMPEX_INIT;
#endif
// AVP Prague stamp end




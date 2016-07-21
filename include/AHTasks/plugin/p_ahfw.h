// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  03 May 2007,  21:09 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- AHFW
// Sub project -- Not defined
// Purpose     -- задача firewall
// Author      -- Sobko
// File Name   -- plugin_ahfw.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_ahfw )
#define __public_plugin_ahfw
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_AHFW  ((tPID)(61021))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SOBKO  ((tVID)(67))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Task )
// Task interface implementation
// Short comments --

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_TASK_SETTINGS_CHANGED 0x214ebca6 //

// message class
//! #define pmc_TASK_OS_REBOOT_REQUEST 0x64e52db8 //
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_ahfw
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDATA)&ResolveNetworks, PID_AHFW, 0x56ca68ecl },
{ (tDATA)&FillApplicationChecksum, PID_AHFW, 0x9a69f221l },
{ (tDATA)&AddNetwork, PID_AHFW, 0xf5891b5fl },
{ (tDATA)&DeleteNetworkWithHidden, PID_AHFW, 0x77d28a6dl },
{ (tDATA)&DeleteNetwork, PID_AHFW, 0xa8a03034l },
{ (tDATA)&ChangeNetwork, PID_AHFW, 0xed45ffc6l },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tERROR IMPEX_FUNC(ResolveNetworks)( cSerializable* p_Networks )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FillApplicationChecksum)( cStrObj* p_FullPathName, tBYTE* p_CheckSumBuffer, tDWORD* p_CheckSumBufferLen )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(AddNetwork)( cSerializable* p_Networks, cSerializable* p_NewNet, tUINT p_InsertIndex )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(DeleteNetworkWithHidden)( cSerializable* p_Networks, tUINT p_Index )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(DeleteNetwork)( cSerializable* p_Networks, tUINT p_Index )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ChangeNetwork)( cSerializable* p_Networks, tUINT p_Index, cSerializable* p_NewNet )IMPEX_INIT;
#endif
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  23 January 2003,  19:02 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- registry.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __registry_h__fef49ac0_1725_4b09_9c76_a38e7a741420 )
#define __registry_h__fef49ac0_1725_4b09_9c76_a38e7a741420
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>

#include "plugin_win32_reg.h"

#include <windows.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// registry pseudo handles
#define cHKEY_CLASSES_ROOT             ((tRegKey)(0x80000000)) // HKEY classes root
#define cHKEY_CURRENT_USER             ((tRegKey)(0x80000001)) // HKEY current user
#define cHKEY_LOCAL_MACHINE            ((tRegKey)(0x80000002)) // HKEY local machine
#define cHKEY_USERS                    ((tRegKey)(0x80000003)) // HKEY users
#define cHKEY_PERFORMANCE_DATA         ((tRegKey)(0x80000004)) //
#define cHKEY_CURRENT_CONFIG           ((tRegKey)(0x80000005)) //
#define cHKEY_DYN_DATA                 ((tRegKey)(0x80000006)) //

// registry pseudo handle aliases
#define cHKCU                          ((tRegKey)(cHKEY_CURRENT_USER)) // HKEY current user
#define cHKLM                          ((tRegKey)(cHKEY_LOCAL_MACHINE)) // HKEY local machine
#define cHKCR                          ((tRegKey)(cHKEY_CLASSES_ROOT)) // HKEY classes root

// name length
#define cRegMaxName                    ((tDWORD)(260)) // maximum name length
// AVP Prague stamp end


// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Registry interface implementation
// Short comments -- access to storage like registry
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Registry_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure RegData is declared in O:\Prague\WinReg\registry.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plNATIVE_ERROR mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // registry_h
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  02 February 2006,  14:11 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- plugin_win32_reg.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_win32_reg )
#define __public_plugin_win32_reg
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_WIN32_REG  ((tPID)(6))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, Registry )
// Registry interface implementation
// Short comments -- access to storage like registry

// constants

// ----------  registry pseudo handles  ----------
#define cHKEY_CLASSES_ROOT             ((tRegKey)(0x80000000)) // HKEY classes root
#define cHKEY_CURRENT_USER             ((tRegKey)(0x80000001)) // HKEY current user
#define cHKEY_LOCAL_MACHINE            ((tRegKey)(0x80000002)) // HKEY local machine
#define cHKEY_USERS                    ((tRegKey)(0x80000003)) // HKEY users
#define cHKEY_PERFORMANCE_DATA         ((tRegKey)(0x80000004)) //
#define cHKEY_CURRENT_CONFIG           ((tRegKey)(0x80000005)) //
#define cHKEY_DYN_DATA                 ((tRegKey)(0x80000006)) //

// ----------  registry pseudo handle aliases  ----------
#define cHKCU                          ((tRegKey)(cHKEY_CURRENT_USER)) // HKEY current user
#define cHKLM                          ((tRegKey)(cHKEY_LOCAL_MACHINE)) // HKEY local machine
#define cHKCR                          ((tRegKey)(cHKEY_CLASSES_ROOT)) // HKEY classes root

// ----------  name length  ----------
#define cRegMaxName                    ((tDWORD)(260)) // maximum name length

// errors
#define errREG_IMPERSONATION                     PR_MAKE_IMP_ERR(PID_WIN32_REG, 0x007) // 0x90006007,-1879023609 (7) -- impersonation error

// properties
#define plNATIVE_ERROR mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plIMPERSONATE  mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002001 )
#define plWOW64_64KEY  mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002002 )
#define plWOW64_32KEY  mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002003 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_win32_reg
// AVP Prague stamp end




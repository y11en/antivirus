// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  13 March 2003,  15:32 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Process memory scan
// Purpose     -- Доступ к памяти процессов для операционных систем Windows 9x/NT
// Author      -- Sobko
// File Name   -- objptr.h
// Additional info
//    Триада интерфейсов позволяющая енумерировать процессы в памяти, читать и изменять данные в этой памяти
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __objptr_h__d08e60fa_fa09_482f_bbce_78fe54cdbfa8 )
#define __objptr_h__d08e60fa_fa09_482f_bbce_78fe54cdbfa8
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "io.h"
#include "plugin_memmod.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call ObjPtr_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure PM_ObjPtrPrivate is declared in O:\Prague\MemScan\objptr.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plPID         mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
   #define plPARENT_PID  mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
   #define plTHREADS_NUM mPROPERTY_MAKE_LOCAL( pTYPE_UINT    , 0x00002002 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // objptr_h
// AVP Prague stamp end




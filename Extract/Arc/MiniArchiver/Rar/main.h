// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  19 December 2002,  15:53 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- RAR Archiver
// Sub project -- rar
// Purpose     -- RAR Archives processor
// Author      -- Dmitry Glavatskikh
// File Name   -- main.h
// Additional info
// RAR Archives processor. Works together with UniArc
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __example_h__7d33dae7_9fdb_4fc4_971e_deaf311b4edd )
#define __example_h__7d33dae7_9fdb_4fc4_971e_deaf311b4edd
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_io.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_root.h>

#include <Extract/iface/i_minarc.h>
#include <Extract/iface/i_uacall.h>

#include "rar.h"
#include "./base/const.h"
#include "./base/proto.h"
// AVP Prague stamp end

#ifdef _DEBUG
# include <windows.h>
# include <stdio.h>
#endif
#include <string.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// MiniArchiver interface implementation
// Short comments -- Mini Plugin for Universal Archiver
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
  tERROR pr_call RAR_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure RAR_Data is declared in d:\prague\rar\example.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // example_h
// AVP Prague stamp end




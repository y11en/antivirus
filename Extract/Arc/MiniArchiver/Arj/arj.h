// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  18 December 2002,  14:00 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- arj.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __arj_h__b6030a1b_2aee_49df_8adc_51c4d356a851 )
#define __arj_h__b6030a1b_2aee_49df_8adc_51c4d356a851
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_arj.h"
#include "Hash/MD5/plugin_hash_md5.h"

#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Extract/iface/i_minarc.h>
#include <Extract/iface/i_uacall.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define MAX_FILENAME_LEN               ((tDWORD)(512)) //  --
#define MAX_PASSWORD_LEN               ((tDWORD)(512)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// MiniArchiver interface implementation
// Short comments -- Mini Plugin Arj
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call MiniArchiver_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure ArjData is declared in O:\Prague\MiniArchiver\Arj\arj.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // arj_h
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  19 October 2002,  18:34 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Integrated Solution Lite
// Purpose     -- Предназначен для хранения информации о дате проверки файлов
// Author      -- Andy Nikishin
// File Name   -- ifilesec.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __ifilesec_h__cb2f20fa_3e0b_4129_ac27_f3ce0919a02d )
#define __ifilesec_h__cb2f20fa_3e0b_4129_ac27_f3ce0919a02d
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <..\Include\IFace/i_root.h>
#include <..\Include\IFace/i_ifilesecurestatus.h>
#include "plugin_ifilesec.h"
#include <..\Include\IFace/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// iFileSecureStatus interface implementation
// Short comments --
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call iFileSecureStatus_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure ifsData is declared in O:\Prague\ISLite\iChecker\ifilesec.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plDB_PATHNAME mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // ifilesec_h
// AVP Prague stamp end




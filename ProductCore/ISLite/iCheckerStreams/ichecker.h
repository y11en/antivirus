// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  07 March 2003,  19:02 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- ichecker.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __ichecker_h__6b16024c_3ff9_4cce_90da_ae549a8e394c )
#define __ichecker_h__6b16024c_3ff9_4cce_90da_ae549a8e394c
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <IFace/i_root.h>
#include <IFace/i_ichecker2.h>
#include "plugin_ichstrms.h"
#include <IFace/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define warnNOT_FOUND                  PR_MAKE_IMP_ERR(PID_ICHSTRMS, 0x001)
#define cICS_PROP_HASH                 ((tSTRING)("ICS_PROP_HASH")) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IChecker interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IChecker_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure  is declared in O:\Prague\ISLite\iCheckerStreams\ichecker.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // ichecker_h
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  30 October 2002,  14:24 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Test
// Sub project -- Not defined
// Purpose     -- making images form io
// Author      -- Sobko
// File Name   -- io.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __io_h__95d84fb0_5bc6_4a19_8c80_7c64a8495e8d )
#define __io_h__95d84fb0_5bc6_4a19_8c80_7c64a8495e8d
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <iface/i_root.h>
#include <iface/i_io.h>
#include "plugin_substio.h"
#include <iface/i_string.h>
#include <iface/i_tree.h>
#include <iface/i_seqio.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define _SUBST_TAG                     ((tQWORD)(((tQWORD) 0x4F495F74 /*'OI_t'*/ << 32) + (tQWORD) 0x73627553 /*'sbuS'*/)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_SUBST_IO 0xda000002 // (3657433090) -- управление подставленными данными

	#define pm_SUBST_IO_SAVE_CONTEXT 0x00002000 // (8192) -- сохранить IO
	// context comment
	//    IO с которго будут считанны данные и свойства для подмены
	// data content comment
	//    указатель на tERROR

	#define pm_SUBST_IO_RESTORE_CONTEXT 0x00002001 // (8193) -- восстановить данные
	// context comment
	//    IO из которго восстановят сохранённые для подмены данные
	// data content comment
	//    указатель на tERROR

	#define pm_SUBST_IO_SET_COMMENT 0x00002002 // (8194) --
	// context comment
	//    hSTRING with comment
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call IO_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure IOPrivate is declared in O:\Prague\SubstIO\io.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // io_h
// AVP Prague stamp end




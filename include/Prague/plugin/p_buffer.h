// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Tuesday,  17 February 2004,  15:56 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- buffer interface
// Purpose     -- plugin implements buffer interface
// Author      -- petrovitch
// File Name   -- plugin_buffer.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_buffer )
#define __pligin_buffer
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_buffer.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_BUFFER  ((tPID)(20))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin public definitions, Buffer )
// Buffer interface implementation
// Short comments -- интерфейс доступа к региону памяти//eng:"piece of memory" interface

// properties
#define plPREALLOCATED_BUFFER      mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002000 )
#define plPREALLACATED_BUFFER_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_buffer
// AVP Prague stamp end




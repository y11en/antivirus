// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  10 August 2004,  11:37 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sychev
// File Name   -- plugin_filemapping.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_filemapping )
#define __pligin_filemapping
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_buffer.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_FILEMAPPING  ((tPID)(43001))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SYCHEV  ((tVID)(85))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Buffer )
// Buffer interface implementation
// Short comments -- интерфейс доступа к региону памяти//eng:"piece of memory" interface

// constants
#define fACCESS_COPY_ON_WRITE          ((tDWORD)(0x00000100L)) //  --
#define fHINT_NORMAL_ACCESS            ((tDWORD)(0)) //  --
#define fHINT_SEQUENTIAL_ACCESS        ((tDWORD)(1)) //  --
#define fHINT_RANDOM_ACCESS            ((tDWORD)(2)) //  --
#define fACCESS_EXECUTE                ((tDWORD)(0x00000200L)) //  --

// properties
#define plACCESS_HINT mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_filemapping
// AVP Prague stamp end




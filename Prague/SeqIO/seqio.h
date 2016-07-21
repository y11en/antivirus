// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  08 February 2002,  15:50 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Sequential File IO
// Purpose     -- Not defined
// Author      -- Andrew
// File Name   -- seqio.h
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __seqio_h__79a540e3_14ce_11d6_a106_0002b302d579 )
#define __seqio_h__79a540e3_14ce_11d6_a106_0002b302d579
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include "seqio.h"
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_SEQ_IO  ((tPID)(64000))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ANDREW  ((tVID)(64))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// SeqIO interface implementation
// Short comments -- Sequential input/output interface
// Extended comment -
//  Supplies facility sequential input/output on ordinary IO object
//  Object of the interface must have current position
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
#if defined( __cplusplus )
extern "C"
{
#endif

    tERROR pr_call SeqIO_Register( hROOT root );

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end



#define cDEFAULT_BUFFER_SIZE 4096

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface SeqIO. Inner data structure

#if defined( __cplusplus )
extern "C"
{
#endif

#define cWRITE_TEST_NONE   0
#define cWRITE_TEST_INSIDE 1
#define cWRITE_TEST_EOF    2
#define cWRITE_TEST_GAP    3

typedef struct tag_tSEQIO_DATA
{
    // IO related data
    tQWORD   file_size;        // file size
    tBYTE    file_size_valid;  // file size is syncronized with file
    tBYTE    file_eof;         // file pointer at EOF position

    // buffer related data
    tQWORD   buffer_offset;    // offset from buffer begin
    tUINT    buffer_rsize;     // current allocated buffer size
    tUINT    buffer_wsize;     // allowed to write up to this value
    tUINT    buffer_ptr;       // current buffer position
    tUINT    buffer_count;     // current buffer count
    tDWORD   prop_buffer_size; // --

    tBYTE    write_test;       // write file inside tested

    tSBYTE   byte_order;       // --
    tSBYTE   buffer_order;     // --

    tBYTE    byte_reverse;     // --
    tBYTE    buffer_reverse;   // --

    tBYTE    read_access;
    tBYTE    write_access;

    tBYTE    read_buffering;
    tBYTE    write_buffering;

    tBYTE    buffer_dirty;

    tBYTE    region_active;
    tQWORD   region_start;
    tQWORD   region_end;
    tQWORD   region_size;


    tBOOL    delete_on_close;  // internal state
    hIO      io;      // --
    tDWORD   access_mode;      // --
    tDWORD   availability;     // --
    tORIG_ID origin;           // --

    // petrovitch 16.05.03 (because MemoryRealloc became obsolete)
    //tBYTE    buffer[cDEFAULT_BUFFER_SIZE];
    tBYTE*   buffer;

} tSEQIO_DATA;

#if defined( __cplusplus )
}
#endif



//!    tBYTE   c_byte_order;
//!    tBYTE   c_stream_order;
//!    tBYTE   c_buffer_order;

    // buffer mode
//!    tBYTE   read_buffering;
//!    tBYTE   write_buffering;
//!    tBYTE   read_access;
//!    tBYTE   write_access;


//!}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

#if defined( __cplusplus )
extern "C"
{
#endif

typedef struct tag_hi_SeqIO
{
    const iSeqIOVtbl*  vtbl; // pointer to the "SeqIO" virtual table
    const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
    tSEQIO_DATA*       data; // pointer to the "SeqIO" data structure
} *hi_SeqIO;

#if defined( __cplusplus )
}
#endif

//!}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // seqio_h
// AVP Prague stamp end




// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  26 November 2002,  12:32 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- LZX Decompressor
// Author      -- Andy Nikishin
// File Name   -- unzlx.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __unzlx_h__be786bb7_c1bf_4b35_9947_0464ceddb037 )
#define __unzlx_h__be786bb7_c1bf_4b35_9947_0464ceddb037
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include "plugin_lzxdecompressor.h"

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>

#include <Extract/iface/i_transformer.h>
// AVP Prague stamp end


/* LZX stuff */

/* some constants defined by the LZX specification */
#define LZX_MIN_MATCH                (2)
#define LZX_MAX_MATCH                (257)
#define LZX_NUM_CHARS                (256)
#define LZX_BLOCKTYPE_INVALID        (0)   /* also blocktypes 4-7 invalid */
#define LZX_BLOCKTYPE_VERBATIM       (1)
#define LZX_BLOCKTYPE_ALIGNED        (2)
#define LZX_BLOCKTYPE_UNCOMPRESSED   (3)
#define LZX_PRETREE_NUM_ELEMENTS     (20)
#define LZX_ALIGNED_NUM_ELEMENTS     (8)   /* aligned offset tree #elements */
#define LZX_NUM_PRIMARY_LENGTHS      (7)   /* this one missing from spec! */
#define LZX_NUM_SECONDARY_LENGTHS    (249) /* length tree #elements */

/* LZX huffman defines: tweak tablebits as desired */
#define LZX_PRETREE_MAXSYMBOLS  (LZX_PRETREE_NUM_ELEMENTS)
#define LZX_PRETREE_TABLEBITS   (6)
#define LZX_MAINTREE_MAXSYMBOLS (LZX_NUM_CHARS + 50*8)
#define LZX_MAINTREE_TABLEBITS  (12)
#define LZX_LENGTH_MAXSYMBOLS   (LZX_NUM_SECONDARY_LENGTHS+1)
#define LZX_LENGTH_TABLEBITS    (12)
#define LZX_ALIGNED_MAXSYMBOLS  (LZX_ALIGNED_NUM_ELEMENTS)
#define LZX_ALIGNED_TABLEBITS   (7)

#define DECR_DATAFORMAT   (1)
#define DECR_ILLEGALDATA  (2)
#define DECR_NOMEMORY     (3)
#define DECR_CHECKSUM     (4)
#define DECR_INPUT        (5)
#define DECR_OUTPUT       (6)


#define LZX_LENTABLE_SAFETY (64) /* we allow length table decoding overruns */

#define LZX_DECLARE_TABLE(tbl) \
	tWORD tbl##_table[(1<<LZX_##tbl##_TABLEBITS) + (LZX_##tbl##_MAXSYMBOLS<<1)];\
tBYTE tbl##_len  [LZX_##tbl##_MAXSYMBOLS + LZX_LENTABLE_SAFETY]

typedef struct _tag_LZXstate 
{
    tBYTE *window;         /* the actual decoding window              */
    tDWORD window_size;     /* window size (32Kb through 2Mb)          */
    tDWORD actual_size;     /* window size when it was first allocated */
    tDWORD window_posn;     /* current offset within the window        */
    tDWORD R0, R1, R2;      /* for the LRU offset system               */
    tWORD main_elements;   /* number of main tree elements            */
    tINT   header_read;     /* have we started decoding at all yet?    */
    tWORD block_type;      /* type of this block                      */
    tDWORD block_length;    /* uncompressed length of this block       */
    tDWORD block_remaining; /* uncompressed bytes still left to decode */
    tDWORD frames_read;     /* the number of CFDATA blocks processed   */
    tLONG  intel_filesize;  /* magic header value used for transform   */
    tLONG  intel_curpos;    /* current offset in transform space       */
    tINT   intel_started;   /* have we seen any translatable data yet? */
	
	tBYTE extra_bits[56];
	tDWORD position_base[52];
    LZX_DECLARE_TABLE(PRETREE);
    LZX_DECLARE_TABLE(MAINTREE);
    LZX_DECLARE_TABLE(LENGTH);
    LZX_DECLARE_TABLE(ALIGNED);
}LZXstate;

// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure

typedef struct tag_LZXData 
{
	hSEQ_IO hInputSeqIo;     // --
	hSEQ_IO hOutputSeqIo;    // --
	tQWORD  qwOutputSize; // --
	tDWORD  dwWndSize;       // --
	tBYTE*  lpWindow;
	tBOOL   bDebugFlag;
	tBYTE*  outbuf;
	tDWORD  dwRead;
	tDWORD  dwInputSize;
    tDWORD  err;
	LZXstate lzx;
} LZXData;

// AVP Prague stamp end

/*
typedef struct _tag_CABstate 
{
	//	tBYTE* inbuf;
}CABstate ;
*/



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Transformer_Register( hROOT root );
// AVP Prague stamp end
#ifdef __cplusplus
extern "C" 	{
#endif
	tERROR LZXInit(LZXData* lcabstate, tBYTE* lpWindow, tDWORD dwWndSize);
	tINT LZXDecompress(LZXData* pContext, tBYTE* lpOutBuffer, tDWORD cbOutBufferSize , tINT Flag);
#ifdef __cplusplus
}
#endif
	





// AVP Prague stamp begin( Header endif,  )
#endif // unzlx_h
// AVP Prague stamp end




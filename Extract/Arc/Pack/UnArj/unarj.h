// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  04 January 2003,  16:47 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Decompressor for ARJ Archives
// Author      -- Andy Nikishin
// File Name   -- unarj.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __unarj_h__e4fb68db_bfbf_4598_a641_52203a9f6aa3 )
#define __unarj_h__e4fb68db_bfbf_4598_a641_52203a9f6aa3
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_unarj.h"

#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include <Extract/iface/i_transformer.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Transformer_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure UnArj_Data is declared in O:\Prague\Extract\UnArj\unarj.c source file
// AVP Prague stamp end

#define CHAR_BIT  (8)
#define DDICSIZ      26624
#define THRESHOLD    3
#define MAXMATCH   256
#define MAXDICBIT   16
#define UCHAR_MAX    255
#define CODE_BIT          16
#define NC          (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)
#define CTABLESIZE  4096
#define NP          (MAXDICBIT + 1)
#define CBIT         9
#define NT          (CODE_BIT + 3)
#define PBIT         5
#define TBIT         5
#if NT > NP
#define NPT NT
#else
#define NPT NP
#endif
	
#define CTABLESIZE  4096
#define PTABLESIZE   256
	
#define STRTP          9
#define STOPP         13
	
#define STRTL          0
#define STOPL          7
#define INBUFSIZ 0x1000

struct tag_UnArj_Data;
	
typedef tERROR  (*out_func)  (struct tag_UnArj_Data*,  tPTR Buffer, tDWORD size);
	
// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure
	

typedef struct tag_UnArj_Data 
{
	tQWORD  qwDecompressSize; // --
	hSEQ_IO hInputIo;         // --
	hSEQ_IO hOutputIo;        // --
	tDWORD  dwDecoMethod;     // --
	hHEAP   hHeap;            // --
	tERROR  error;            // --
	hOBJECT _Me;              // --
	tDWORD  dwYeldCounter;    // --
	tDWORD  dwDecoded;        // --
	out_func UnArjWrite;       // --
	tUINT incnt;
	tBYTE *inptr;
	tLONG compsize;
	tLONG origsize;
	tWORD bitbuf;
	tINT bitcount;
	tBYTE subbitbuf;
	tWORD getlen;
	tWORD getbuf;
	tWORD pt_table[PTABLESIZE];
	tWORD right[2 * NC - 1];
	tWORD left[2 * NC - 1];
	tBYTE c_len[NC];
	tWORD *c_table;//[CTABLESIZE];//    (8k)
	tBYTE text[DDICSIZ] ;//(24k)
	tBYTE inbuf[INBUFSIZ];
	tBYTE pt_len[NPT];
	tWORD blocksize;
	tLONG last_len;
} UnArj_Data;
	
	// AVP Prague stamp end
	

// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end

tBYTE* ArjNew(UnArj_Data* data, tDWORD dwBytes);
tVOID ArjDelete(UnArj_Data* data, tPTR lpdata);
tDWORD decode_main(UnArj_Data* data);
tDWORD decode_f(UnArj_Data* data);


#define	STORED    0
#define	DEFLATED1 1
#define	DEFLATED2 2
#define	DEFLATED3 3
#define	DEFLATED4 4


// AVP Prague stamp begin( Property table,  )
   #define plDECOMPRESSON_METHOD mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // unarj_h
// AVP Prague stamp end




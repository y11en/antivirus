// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  10 January 2003,  12:15 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- ARJ Archiver
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- arjpack.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __arjpack_h__6d2b60a4_ae1f_478e_8815_dd6bc9706586 )
#define __arjpack_h__6d2b60a4_ae1f_478e_8815_dd6bc9706586
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_arjpacker.h"

#include <Prague/iface/i_heap.h>
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
  // data structure ArjPackData is declared in O:\Prague\Packers\ArjPack\arjpack.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure

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
	
#define STRTL			0
#define STOPL			7
#define INBUFSIZ		0x1000
#define PUTBIT_SIZE		4096
	
#define FAR_BLK_MAX        469762048
#define MIN_BUFSIZ              2048
#define MAX_BUFSIZ       FAR_BLK_MAX
#define BUFSIZ_INCREMENT           6
#define DICSIZ                 26624
#define DICSIZ_MAX             32750
#define LONG_MAX        0x7FFFFFFFL
#define MAX_FILE_SIZE       LONG_MAX    /* Size of compressed file */
#define BUFSIZ_DEFAULT         16384

#define	STORED    0
#define	DEFLATED1 1
#define	DEFLATED2 2
#define	DEFLATED3 3
#define	DEFLATED4 4
	
	
	
typedef struct tag_ArjPackData 
{
	tQWORD  qwSize;    // --
	hSEQ_IO hInputIo;  // --
	hSEQ_IO hOutputIo; // --
	hOBJECT _Me;
	hHEAP hHeap;
	tDWORD dwMethod;
	tERROR error;
	tWORD right[2 * NC - 1];
	tWORD left[2 * NC - 1];
	tBYTE c_len[NC];
	tBYTE pt_len[NPT];
	tINT  dicbit;
	tLONG dwCompressed;
	tINT  bitcount;	
	tCHAR *out_buffer;
	tWORD bitbuf;
	tWORD p_freq[2*NP-1];
	tWORD pt_code[NPT];
	tWORD *c_freq;
	tWORD *c_code;
	tWORD *heap;
	tWORD len_cnt[17];
	tINT  depth;
	tBYTE *buf;
	tWORD *freq;
	tBYTE output_mask;
	tWORD output_pos;
	tWORD t_freq[2*NT-1];
	tINT  heapsize;
	tWORD *sortptr;
	tBYTE *len;
	tWORD fpcount;
	short *fpbuf;
	short *dtree;
	tWORD treesize;
	tBYTE *tree;
	tWORD numchars;
	tWORD dicsiz_cur;
	tWORD dicpos;
	tWORD tc_passes;
	short *ftree;
	tINT st_n;
	tUINT dic_alloc;
	tINT  out_bytes;
	tINT  out_avail;
	tWORD cpos;
	tUINT bufsiz;
	tLONG origsize;	
	tUINT current_bufsiz;	
	tINT s_depth;
} ArjPackData;

// AVP Prague stamp end
	
tVOID encode(ArjPackData* data, tINT method, tINT max_compression);
tVOID encode_f(ArjPackData* data);

// AVP Prague stamp begin( Header endif,  )
#endif // arjpack_h
// AVP Prague stamp end




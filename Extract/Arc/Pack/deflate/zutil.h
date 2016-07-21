#ifndef _Z_UTIL_H
#define _Z_UTIL_H

#include "zlibt.h"


        /* common constants */

#ifndef DEF_WBITS
#  define DEF_WBITS MAX_WBITS
#endif
/* default windowBits for decompression. MAX_WBITS is for compression only */
#define MAX_MEM_LEVEL 9

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

/* Diagnostic functions */
#ifdef _DEBUG
   extern tINT z_verbose;
   extern tVOID z_error    OF((tCHAR *m));
#  define Assert(cond,msg) //{if(!(cond)) Printf(msg);}
#  define Trace(x) ///{printf x ; printf("\n");}
#  define Tracev(x) //{printf x ;printf("\n");}
#  define Tracevv(x) //{printf x ;printf("\n");}
#  define Tracec(c,x) //{if (z_verbose>0 && (c)) fprintf x ;}
#  define Tracecv(c,x) //{if (z_verbose>1 && (c)) fprintf x ;}

//tPTR zcalloc OF((tPTR opaque, tUINT items, tUINT size, tINT Line));
//tVOID   zcfree  OF((tPTR opaque, tPTR ptr, tINT Line));

#define ZALLOC(strm, items, size) \
           (*((strm)->zAlloc))(strm, (strm)->opaque, (items), (size),__LINE__)
#define ZFREE(strm, addr)  (*((strm)->zfree))(strm, (strm)->opaque, (tPTR)(addr),__LINE__);addr=Z_NULL
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)

tPTR  zcalloc OF((z_streamp strm, tPTR opaque, tUINT items, tUINT size));
tVOID zcfree  OF((z_streamp strm, tPTR opaque, tPTR ptr));

#define ZALLOC(strm, items, size) \
           (*((strm)->zAlloc))(strm, (strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))(strm,(strm)->opaque, (tPTR)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}
#endif


typedef tDWORD (EXPORT *check_func) OF((tDWORD check, const tBYTE *buf,tUINT len));


#endif /* _Z_UTIL_H */

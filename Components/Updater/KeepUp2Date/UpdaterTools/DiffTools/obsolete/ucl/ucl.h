#ifndef _UCL_H_
#define _UCL_H_  "UCL based compression"

#include <stdlib.h>
#include <memory.h>
#include "../shared/types.h"
#include "../shared/diff.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------

#define getbyte(c)  ((c).ip < (c).in_end ? (unsigned) *((c).ip)++ : (-1))

// type redefinitions
typedef unsigned int         ucl_uint32;
typedef int                  ucl_int32;
typedef unsigned int         ucl_uint;
typedef int                  ucl_int;
typedef int                  ucl_bool;

#ifndef UINT_MAX
#define UINT_MAX (0xFFFFFFFFL)
#endif
#ifndef INT_MAX
#define INT_MAX  (0x7FFFFFFFL)
#endif
#ifndef INT_MIN
#define INT_MIN  (UINT_MAX -1)
#endif
#define UCL_UINT32_MAX       UINT_MAX
#define UCL_INT32_MAX        INT_MAX
#define UCL_INT32_MIN        INT_MIN
#define UCL_UINT_MAX         UINT_MAX
#define UCL_INT_MAX          INT_MAX
#define UCL_INT_MIN          INT_MIN

#define UCL_UINT32_C(c)     c ## U

#define ucl_byte             unsigned char
#define ucl_bytep            unsigned char*
#define ucl_charp            char*
#define ucl_voidp            void*
#define ucl_shortp           short*
#define ucl_ushortp          unsigned short*
#define ucl_uint32p          ucl_uint32*
#define ucl_int32p           ucl_int32*
#define ucl_uintp            ucl_uint*
#define ucl_intp             ucl_int*
#define ucl_voidpp           ucl_voidp*
#define ucl_bytepp           ucl_bytep*

// buffers size
#define N               (16*1024*1024UL)    // size of ring buffer
#define F               (1024*1024)         // upper limit for match length
#define THRESHOLD       (1)                 // lower limit for match length

#define M2_MAX_OFFSET   (0x500)

#define UCL_BYTE(x)       ((unsigned char) (x))
#define UCL_USHORT(x)     ((unsigned short) ((x) & 0xffff))

#define UCL_MAX(a,b)        ((a) >= (b) ? (a) : (b))
#define UCL_MIN(a,b)        ((a) <= (b) ? (a) : (b))
#define UCL_MAX3(a,b,c)     ((a) >= (b) ? UCL_MAX(a,c) : UCL_MAX(b,c))
#define UCL_MIN3(a,b,c)     ((a) <= (b) ? UCL_MIN(a,c) : UCL_MIN(b,c))

#define ucl_sizeof(type)    ((ucl_uint) (sizeof(type)))

#define UCL_HIGH(array)     ((ucl_uint) (sizeof(array)/sizeof(*(array))))

// this always fits into 16 bits
#define UCL_SIZE(bits)      (1u << (bits))
#define UCL_MASK(bits)      (UCL_SIZE(bits) - 1)

#define UCL_LSIZE(bits)     (1ul << (bits))
#define UCL_LMASK(bits)     (UCL_LSIZE(bits) - 1)

#define UCL_USIZE(bits)     ((ucl_uint) 1 << (bits))
#define UCL_UMASK(bits)     (UCL_USIZE(bits) - 1)

// Maximum value of a signed/unsigned type.
// Do not use casts, avoid overflows
#define UCL_STYPE_MAX(b)    (((1l  << (8*(b)-2)) - 1l)  + (1l  << (8*(b)-2)))
#define UCL_UTYPE_MAX(b)    (((1ul << (8*(b)-1)) - 1ul) + (1ul << (8*(b)-1)))

// error codes
#define UCL_E_OK                       (0)
#define UCL_E_ERROR                    (-1)
#define UCL_E_INVALID_ARGUMENT         (-2)
#define UCL_E_OUT_OF_MEMORY            (-3)
#define UCL_E_NOT_COMPRESSIBLE         (-4)
#define UCL_E_INPUT_OVERRUN            (-5)
#define UCL_E_OUTPUT_OVERRUN           (-6)
#define UCL_E_LOOKBEHIND_OVERRUN       (-7)
#define UCL_E_EOF_NOT_FOUND            (-8)
#define UCL_E_INPUT_NOT_CONSUMED       (-9)
#define UCL_E_OVERLAP_OVERRUN          (-10)

// calback proc
typedef struct sUCL_CALLBACK {
  tVOID (*CallBack)(tUINT, tUINT, tINT, tVOID* User);
  tVOID* User;
} tUCL_CALLBACK, *pUCL_CALLBACK;

// options struct
typedef struct sUCL_COMPRESS_CFG {
  tINT   bb_endian;
  tINT   bb_size;
  tUINT  max_offset;
  tUINT  max_match;
  tINT   s_level;
  tINT   h_level;
  tINT   p_level;
  tINT   c_flags;
  tUINT  m_size;
} tUCL_COMPRESS_CFG, *pUCL_COMPRESS_CFG;

// working struct
typedef struct sUCL_COMPRESS_CTX {
    int init;

    ucl_uint look;          // bytes in lookahead buffer

    ucl_uint m_len;
    ucl_uint m_off;

    ucl_uint last_m_len;
    ucl_uint last_m_off;

    const ucl_byte *bp;
    const ucl_byte *ip;
    const ucl_byte *in;
    const ucl_byte *in_end;
    ucl_byte *out;

    ucl_uint32 bb_b;
    unsigned bb_k;
    unsigned bb_c_endian;
    unsigned bb_c_s;
    unsigned bb_c_s8;
    ucl_byte *bb_p;
    ucl_byte *bb_op;

    struct sUCL_COMPRESS_CFG conf;
    ucl_uintp result;

    tUCL_CALLBACK* cb;

    ucl_uint textsize;      // text size counter
    ucl_uint codesize;      // code size counter
    ucl_uint printcount;    // counter for reporting progress every 1K bytes

    // some stats
    unsigned long lit_bytes;
    unsigned long match_bytes;
    unsigned long rep_bytes;
    unsigned long lazy;
} tUCL_COMPRESS_CTX, *pUCL_COMPRESS_CTX;

// sliding window

#define SWD_N             N
#define SWD_F             F
#define SWD_THRESHOLD     THRESHOLD

typedef ucl_uint          swd_uint;
#define SWD_UINT_MAX      UCL_UINT_MAX
#define SWD_UINT(x)       ((swd_uint)(x))

#define SWD_HSIZE         (N)//(N/4)
#define SWD_MAX_CHAIN     (F)
#define HEAD3(b,p) \
  (((0x9f5f*(((((tUINT)b[p]<<5)^b[p+1])<<5)^b[p+2]))>>5) & (SWD_HSIZE-1))

#define HEAD2(b,p)      (* (const ucl_ushortp) &(b[p]))
#define NIL2              SWD_UINT_MAX

typedef struct sSWD_CONFIG {
  ucl_uint try_lazy;
  ucl_uint good_length;
  ucl_uint max_lazy;
  ucl_uint nice_length;
  ucl_uint max_chain;
  ucl_uint32 flags;
  ucl_uint32 max_offset;
} tSWD_CONFIG, *pSWD_CONFIG;

typedef struct sUCL_SWD {
// public - "built-in"
    ucl_uint n;
    ucl_uint f;
    ucl_uint threshold;

// public - configuration
    ucl_uint max_chain;
    ucl_uint nice_length;
    ucl_bool use_best_off;
    ucl_uint lazy_insert;

// public - output
    ucl_uint m_len;
    ucl_uint m_off;
    ucl_uint look;
    int b_char;

// semi public
    tUCL_COMPRESS_CTX* c;
    ucl_uint m_pos;

// private
    const ucl_byte *dict;
    const ucl_byte *dict_end;
    ucl_uint dict_len;

// private
    ucl_uint ip;                // input pointer (lookahead)
    ucl_uint bp;                // buffer pointer
    ucl_uint rp;                // remove pointer
    ucl_uint b_size;

    unsigned char *b_wrap;

    ucl_uint node_count;
    ucl_uint first_rp;

    unsigned char *b;
    swd_uint *head3;
    swd_uint *succ3;
    swd_uint *best3;
    swd_uint *llen3;
    swd_uint *head2;
} tUCL_SWD, *pUCL_SWD;

#define s_head3(s,key)        s->head3[key]

// ----------------------------------------------------------------------------

// from ucl_mchw.c
tVOID bbPutBit (tUCL_COMPRESS_CTX* c, tUINT bit);
tVOID bbPutByte(tUCL_COMPRESS_CTX* c, tUINT b);
tINT  bbConfig(tUCL_COMPRESS_CTX* c, tINT endian, tINT bitsize);
tVOID bbWriteBits(tUCL_COMPRESS_CTX* c);
tVOID bbPutBit(tUCL_COMPRESS_CTX* c, tUINT bit);
tVOID bbPutByte(tUCL_COMPRESS_CTX* c, tUINT b);
tVOID bbFlushBits(tUCL_COMPRESS_CTX* c, tUINT filler_bit);
tINT  init_match (tUCL_COMPRESS_CTX* c, tUCL_SWD* s, tBYTE* dict,
                  tUINT dict_len, tUINT flags);
tINT  find_match (tUCL_COMPRESS_CTX* c,tUCL_SWD* s,tUINT this_len,tUINT skip);

// from ucl_swd.c
tINT  swd_init(tUCL_SWD* s, tBYTE* dict, tUINT dict_len);
tVOID swd_exit(tUCL_SWD* s);
tVOID swd_accept(tUCL_SWD *s, ucl_uint n);
tVOID swd_findbest(tUCL_SWD *s);
tVOID swd_getbyte(tUCL_SWD *s);

// fom ucl.c
tINT  UCL_Compress  (tBYTE* in, tUINT in_len, tBYTE* out, tUINT* out_len,
                     tUINT Code, tUINT* Delta, tUCL_CALLBACK* cb);

tINT  UCL_Decompress(struct sDIFF_HDR* Hdr, tBYTE* src, tUINT src_len,
                     tBYTE* dst, tUINT* dst_len);

// ----------------------------------------------------------------------------

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _UCL_H_

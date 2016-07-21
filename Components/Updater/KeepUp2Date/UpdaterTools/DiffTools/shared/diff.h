#ifndef _DIFF_H_
#define _DIFF_H_  "Main difference block creator"

#include <stdlib.h>
#include <memory.h>
#include "../shared/differr.h"
#include "../shared/common.h"
#include "../shared/types.h"
#include "../shared/crc32.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- */
#pragma pack(1)
/* ------------------------------------------------------------------------- */

#define DIFF_VERSION          2     /* unpacker version                     */
#define DIFF_FLG_PACK         1     /* need pack to avc                     */
#define DIFF_FLG_NAMEOFF2LEN  2     /* offs in name block converted to lens */
#define DIFF_FLG_TBLPACK      4     /* diff tables was packed (by lzma now) */
#define DIFF_FLG_SIGN30       8     /* have v3.0 sign (64 bytes long)       */
#define DIFF_FLG_TBLPACK_ZLIB 0x10  /* diff tables was packed (by zlib)     */
#define DIFF_FLG_TBLPACK_LZMA DIFF_FLG_TBLPACK

/* diff block header */
#define DIFF_SIGNATURE "DIFF"
typedef struct sDIFF_HDR {
  tBYTE   Sign[4];                               /* DIFF signature          */
  tDWORD  DiffCrc32;                             /* full diff checksum      */
  tDWORD  DiffSize;                              /* full diff size          */
  tWORD   Version;                               /* version to extract      */
  tWORD   Flags;                                 /* project flags           */
  tBYTE   BaseHash[8];                           /* hash of base file       */
  tBYTE   CodeHash[8];                           /* hash of result file     */
  tDWORD  BaseSize;                              /* base data size          */
  tDWORD  CodeSize;                              /* encoded data size       */
  tDWORD  TblUSize;                              /* table unpacked size     */
  tDWORD  TblUCrc32;                             /* table unpacked checksum */
  /* diff stream */
  /* if ( Flags & DIFF_FLG_SIGN30 ) tBYTE  CodeSign[SIGN_SIZE] */
  /* tBYTE DiffTable[TblUSize]                                 */
} tDIFF_HDR, *pDIFF_HDR;


#define DIFT_SIGNATURE "DIFT"
typedef struct sDIFF_TERM_HDR {
	tBYTE  Sign[4];         // DIFT signature
	tBYTE  Hash[16];        // file MD5 hash
} tDIFF_TERM_HDR;


/* ------------------------------------------------------------------------- */
#pragma pack()
/* ------------------------------------------------------------------------- */

tINT  MakeDiff(tCHAR* FNOld, tCHAR* FNNew, tCHAR* FNDiff, tUINT nFlags, tUINT nLimitSize, tUINT* pnResultSize);
tINT ApplyDiff(tCHAR* FNOld, tCHAR* FNDiff, tCHAR* FNNew);

/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _DIFF_H_ */

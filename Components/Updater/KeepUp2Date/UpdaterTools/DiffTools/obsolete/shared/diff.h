#ifndef _DIFF_H_
#define _DIFF_H_  "Main difference block creator"

#include <stdlib.h>
#include <memory.h>
#include "../ucl/ucl.h"
#include "../shared/differr.h"
#include "../shared/common.h"
#include "../shared/types.h"
#include "../shared/crc32.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
//#include <pshpack1.h>
#pragma pack(1)
// ----------------------------------------------------------------------------

#define MAX_THUNK_SIZE       (16*1024*1024)      // max input data size
#define DIFF_VERSION         (1)                 // unpacker version
#define DIFF_FLG_PACK        (1)                 // need pack to avc
#define DIFF_FLG_NAMEOFF2LEN (2)                 // offsets in name block converted to lens

// diff block header
typedef struct sDIFF_HDR {
  tBYTE   Sign[4];                               // DIFF signature
  tWORD   Version;                               // version to extract
  tWORD   Flags;                                 // project flags
  tDWORD  DiffPCrc;                              // packed diff crc-32
  tDWORD  DiffPSize;                             // packed diff data size
  tDWORD  DiffUCrc;                              // unpacked diff crc-32
  tDWORD  DiffUSize;                             // unpacked diff data size
  tDWORD  BaseSize;                              // base data size
  tDWORD  BaseCrc;                               // base data checksum
  tDWORD  CodeSize;                              // encoded data size
  tDWORD  CodeCrc;                               // encoded data checksum
  tDWORD  Delta;                                 // encode delta
  tBYTE   DigitalSign[SIGN_SIZE];                // digital signature
} tDIFF_HDR, *pDIFF_HDR;

// ----------------------------------------------------------------------------
//#include <poppack.h>
#pragma pack()
// ----------------------------------------------------------------------------

tINT  MakeDiff(tCHAR* FNOld, tCHAR* FNNew,  tCHAR* FNDiff);
tINT ApplyDiff(tCHAR* FNOld, tCHAR* FNDiff, tCHAR* FNNew);

// ----------------------------------------------------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DIFF_H_

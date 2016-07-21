#ifndef _LZMA_DECODE_H_
#define _LZMA_DECODE_H_  "LZMA Decompression Wrapper"

#include <memory.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* error codes */
#define LZMA_DECODE_ERR_OK        0
#define LZMA_DECODE_ERR_FAIL     -1
#define LZMA_DECODE_ERR_NOMEM    -2

/* decompression wrapper */
int LzmaDecodeMem2Mem(const void* Source, unsigned int SrcSize,
                      void*  Destination, unsigned int DstSize);

#ifdef __cplusplus
}
#endif

#endif /* _LZMA_DECODE_H_ */

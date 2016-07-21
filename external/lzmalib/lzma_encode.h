#ifndef _LZMA_ENCODE_H_
#define _LZMA_ENCODE_H_  "LZMA Compression Wrapper"

#include <memory.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* error codes */
#define LZMA_ENCODE_ERR_OK        0
#define LZMA_ENCODE_ERR_FAIL     -1
#define LZMA_ENCODE_ERR_NOMEM    -2

/* compression wrapper */
int LzmaEncodeMem2Mem(const void* Source, unsigned int   SrcSize,
                      void*  Destination, unsigned int* pDstSize);

#ifdef __cplusplus
}
#endif

#endif /* _LZMA_ENCODE_H_ */

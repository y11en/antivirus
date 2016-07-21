#ifndef _DIFF_LZMA_PACK_H_
#define _DIFF_LZMA_PACK_H_  "LZMA compression C wrapper by Dmitry Glavatskikh"

#include <memory.h>
#include <stdlib.h>
#include "../shared/differr.h"
#include "../shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif


tINT LzmaPackMem2Mem(tBYTE* BSrc, tUINT SrcSize, tBYTE* BDst, tUINT* PDstSize);

#ifdef __cplusplus
}
#endif

#endif /* _DIFF_LZMA_PACK_H_ */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include "differr.h"
#include "types.h"
#include "crc32.h"
#include "../avepack/AvcPacker.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SIGN_SIZE            (64)              /* 3.0 digital sign size */

/* ------------------------------------------------------------------------- */

tULONG GetFSize(FILE* IF);
tINT   GetBaseFile(tCHAR* FName, tBYTE** Buff, tUINT* Size,
                   tBOOL* Pack,  tBOOL* HasSign, tBYTE Sign[SIGN_SIZE], tUINT* pnPackFormat);

/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _COMMON_H_ */

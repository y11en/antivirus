#ifndef _DIFF_MD5_H_
#define _DIFF_MD5_H_

#include "types.h"

/* ------------------------------------------------------------------------- */

/* MD5 context. */
typedef struct sMD5_CTX {
  tDWORD  state[4];
  tDWORD  count[2];
  tBYTE   buffer[64];
} tMD5_CTX, *pMD5_CTX;

tVOID MD5Init   (tMD5_CTX* context);
tVOID MD5Update (tMD5_CTX* context, tBYTE* input, tUINT inputLen);
tVOID MD5Final  (tMD5_CTX* context, tBYTE* digest);

tVOID CalcDiffMD5Hash(tBYTE* Data, tUINT DataSize, tBYTE Hash[8]);

/* ------------------------------------------------------------------------- */

#endif /* _DIFF_MD5_H_ */

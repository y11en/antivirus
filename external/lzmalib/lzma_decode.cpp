/* LZMA decompression C wrapper by Dmitry Glavatskikh */

#include <memory.h>
#include <stdlib.h>
#include "lzma_config.h"
#include "lzma_decode.h"
#include "lzmasdk/C/7zip/Compress/LZMA_C/LzmaDecode.h"

/* ------------------------------------------------------------------------- */

extern "C" /* must be accessible from ANSI-C source */
int LzmaDecodeMem2Mem(const void* Source, unsigned int SrcSize,
                      void*  Destination, unsigned int DstSize)
{
  CLzmaDecoderState  LzmaState;
  unsigned char      PropData[5];
  int                lp, lc, pb;
  unsigned int       ProbSize;
  unsigned int       Written;
  unsigned int       Readed;
  CProb*             Probs;
  int                Res;

  /* check params */
  if ( !Source || !SrcSize || !Destination || !DstSize )
    return(LZMA_DECODE_ERR_FAIL);

  lc = LZMA_CONF_CONTEXTBITS;
  lp = LZMA_CONF_POSITIONBITS;
  pb = LZMA_CONF_STATEBITS;

  PropData[0] = (unsigned char)((pb * 9 * 5) + (lp * 9) + lc);
  PropData[1] = (unsigned char)(DstSize >>  0);
  PropData[2] = (unsigned char)(DstSize >>  8);
  PropData[3] = (unsigned char)(DstSize >> 16);
  PropData[4] = (unsigned char)(DstSize >> 24);

  memset(&LzmaState, 0, sizeof(CLzmaDecoderState));
  if ( LZMA_RESULT_OK != LzmaDecodeProperties(&LzmaState.Properties, PropData, sizeof(PropData)) )
    return(LZMA_DECODE_ERR_FAIL);

  /* alloc lzma prob's */
  ProbSize = (LzmaGetNumProbs(&LzmaState.Properties) * sizeof(CProb));
  if ( NULL == (Probs = (CProb*)malloc(ProbSize + 1024 + 16)) )
    return(LZMA_DECODE_ERR_NOMEM);

  /* unpack buffer */
  Readed = Written = 0;
  LzmaState.Probs = Probs;
  memset(Probs, 0, ProbSize + 1024);
  Res = LzmaDecode(&LzmaState, (unsigned char*)Source, SrcSize, &Readed,
    (unsigned char*)Destination, DstSize, &Written);

  /* check result */
  free(Probs); LzmaState.Probs = Probs = NULL;
  if ( Res != LZMA_RESULT_OK || Written != DstSize )
    return(LZMA_DECODE_ERR_FAIL);

  /* all ok */
  return(LZMA_DECODE_ERR_OK);
}

/* ------------------------------------------------------------------------- */

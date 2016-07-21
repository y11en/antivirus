// lzma_unp.cpp
//
// KLAVA wrapper for LZMA unpacker
//

#include <klava/klavdef.h>
#include <string.h>

#include "../../../external/lzmalib/lzma_config.h"
#include "../../../external/lzmalib/lzmasdk/C/7zip/Compress/LZMA_C/LzmaDecodeSize.c"

KLAV_ERR KLAV_LZMA_Decompress (
			hKLAV_Alloc allocator,
			const void * Source,
			uint32_t     SrcSize,
			void *       Destination,
			uint32_t     DstSize
		)
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
  if ( !allocator || !Source || !SrcSize || !Destination || !DstSize )
    return KLAV_EINVAL;

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
    return KLAV_ECORRUPT;

  /* alloc lzma prob's */
  ProbSize = (LzmaGetNumProbs(&LzmaState.Properties) * sizeof(CProb));
  if ( NULL == (Probs = (CProb*) (allocator->alloc (ProbSize + 1024 + 16))) )
    return KLAV_ENOMEM;

  /* unpack buffer */
  Readed = Written = 0;
  LzmaState.Probs = Probs;
  memset(Probs, 0, ProbSize + 1024);
  Res = LzmaDecode(&LzmaState, (unsigned char*)Source, SrcSize, &Readed,
    (unsigned char*)Destination, DstSize, &Written);

  /* check result */
  allocator->free (Probs);
  LzmaState.Probs = Probs = NULL;
 
  if ( Res != LZMA_RESULT_OK || Written != DstSize )
    return KLAV_ECORRUPT;

  /* all ok */
  return KLAV_OK;
}


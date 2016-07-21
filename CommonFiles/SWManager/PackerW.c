// SWPackerWrite.c - implementation of Packer serialization wrapper
// Writing part of a code
//
#include <string.h>
#include "Packer.h"
#include "AVPPort.h"
#include <byteorder.h>

static int Squeeze( void **pAlgoData,
                    char *pInBuff, DWORD nInLength, DWORD *nStartInCount,
                    char *pOutBuff, DWORD nOutLength, DWORD *nStartCount,
                    int *bFirstEnter, 
                    BufferProc pProc, void *pProcData,
                    DWORD nLookUpSize, int bFlush );


// ---
static SWM_PROC AVP_bool SWM_PARAM SWWriteProc( void* pBuffer, AVP_dword nSize, AVP_dword *pnOutSize, void* pUserData ) {
  AVP_bool bOk = 0;
  SWManager *pManager = ((SWM_StreamInfo *)pUserData)->pSWMHandle;
  if ( pManager ) {
		SWContext *pSWContext;

		pManager->lpVtbl->SWMEnterBufferProc( pManager );

		pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_PACKER_ID );
		if ( pSWContext ) {

			if ( !pSWContext->m_bProcessingCompleted ) {

				AVP_dword nLookUpSize = ((SWPackerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize;
				AVP_bool bFlushBuffers = !pBuffer && nSize == (AVP_dword)-1;

				AVP_dword  nOccSize = 0;
				void      *pCurrBuffer = pSWContext->m_pBuffer;

				void      *pPackBuffer = 0;
				AVP_dword  nPackSize   = 0;
				AVP_dword  nHistorySize  = 0;

				bOk = 1;

				if ( pBuffer ) {
					pPackBuffer = SWAllocator( nLookUpSize + nSize );

					if ( pPackBuffer ) {
						if ( ((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory ) {
							nHistorySize = ((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nHistorySize;
							memcpy( pPackBuffer, ((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory, 
											nHistorySize );
							nPackSize += nHistorySize;
						}

						memcpy( (char *)pPackBuffer + nPackSize, pBuffer, nSize );
						nPackSize += nSize;
					}
					else 
						bOk = 0;
				}

				if ( bOk ) {
					if ( pSWContext->m_bHeaderProcessed == 0 ) {
						AVP_SW_Header rcHeader;
						SWPackerInitData rcInitData;
						
          				WriteDWordPtr (&rcHeader.magic, AVP_SW_MAGIC);
          				WriteWordPtr (&rcHeader.id, SW_PACKER_ID);
						memcpy( pCurrBuffer, &rcHeader, sizeof(rcHeader) );
						nOccSize += sizeof(rcHeader);
						pCurrBuffer = (char *)pCurrBuffer + sizeof(rcHeader);

						WriteDWordPtr (&rcInitData.m_nLookUpSize, ((SWPackerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize);
						WriteWordPtr (&rcInitData.m_nAlgorithm, ((SWPackerData *)pSWContext->m_pData)->m_rcInit.m_nAlgorithm);
						memcpy( pCurrBuffer, &rcInitData, sizeof(SWPackerInitData) );

						nOccSize += sizeof(SWPackerInitData);
						pCurrBuffer = (char *)pCurrBuffer + sizeof(SWPackerInitData);

						pSWContext->m_bHeaderProcessed = 1;
					}
					else {
						pCurrBuffer = pSWContext->m_pCurrPosition;
						nOccSize = (AVP_dword)((char *)pSWContext->m_pCurrPosition - (char *)pSWContext->m_pBuffer);
					}       

					if ( bFlushBuffers ) {
						bOk = Squeeze( &((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pAlgorithmData,
													 pPackBuffer, nPackSize, (DWORD *)&nHistorySize,
													 pSWContext->m_pBuffer, pSWContext->m_nBufferSize, (DWORD *)&nOccSize,
													 (int *)&pSWContext->m_bProcessingStarted,
													 pSWContext->m_pSiblingProc, pUserData,
													 nLookUpSize, bFlushBuffers );

						if ( bOk )
							bOk = pSWContext->m_pSiblingProc( 0, (AVP_dword)-1, 0, pUserData );

						pSWContext->m_bProcessingCompleted = 1;
					}
					else {
						bOk = Squeeze( &((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pAlgorithmData,
													 pPackBuffer, nPackSize, (DWORD *)&nHistorySize,
													 pSWContext->m_pBuffer, pSWContext->m_nBufferSize, (DWORD *)&nOccSize,
													 (int *)&pSWContext->m_bProcessingStarted,
													 pSWContext->m_pSiblingProc, pUserData,
													 nLookUpSize, bFlushBuffers );

						if ( bOk ) {
							pSWContext->m_pCurrPosition = (char *)pSWContext->m_pBuffer + nOccSize;

							if ( pPackBuffer ) {
								if ( !((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory )
									((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory = SWAllocator( nLookUpSize );

								if ( ((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory ) {
									memcpy( ((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory, 
													(char *)pPackBuffer, 
													nHistorySize );
									((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nHistorySize = nHistorySize;
								}
								else
									bOk = 0;
							}
						}
					}
					if ( pnOutSize )
						*pnOutSize = nSize;

					SWLiberator( pPackBuffer );
				}
			}
			else
				bOk = pSWContext->m_pSiblingProc( pBuffer, nSize, pnOutSize, pUserData );
		}

		pManager->lpVtbl->SWMLeaveBufferProc( pManager );
  }

  return bOk;
}

// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Packer( void *pManager ) {
  SWControl *piSWControl = 0;

  SWPackerCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pWriteProc = SWWriteProc;
		if ( SWMAdvise(pManager, piSWControl) ) 
			return 1;

		piSWControl->lpVtbl->Release( piSWControl );
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#define  PutByte(byte, pCont)  pCont.gWorkBuff[pCont.gnIndexByte++ + 2] = (BYTE)(byte);

// ---
static int PutBytes( BYTE *pFrom, DWORD nCount, PackContext *pCont ) {
  int bOk = 1;
  while( bOk && nCount ) {
    int nPut = min( pCont->gnOCount - pCont->gnWrittenBytes, nCount );
    memcpy ( (char *)pCont->gpOutBuff + pCont->gnWrittenBytes, pFrom, nPut );
    pCont->gnWrittenBytes += nPut;
    nCount -= nPut;
    pFrom += nPut;
    if ( pCont->gnWrittenBytes == pCont->gnOCount ) {
      DWORD nOutSize;

      PackContext rcSaveContext;
      memcpy( &rcSaveContext, pCont, sizeof(rcSaveContext) );

      bOk = pCont->gpBufferProc( pCont->gpOutBuff, pCont->gnOCount, (AVP_dword *)&nOutSize, pCont->gpBufferProcData );

      memcpy( pCont, &rcSaveContext, sizeof(rcSaveContext) );

      bOk = bOk && nOutSize == pCont->gnOCount;

      pCont->gnWrittenBytes = 0;
    }
  }
  return !bOk;
}

// ---
static int PutBit( BYTE bit, PackContext *pCont ) {
  int bOk = 0;
  WORD outval = ReadWordPtr ((WORD*)pCont->gWorkBuff) >> 1;
  outval |= (bit & 1) ? 0x8000 : 0;
  WriteWordPtr ((WORD *)pCont->gWorkBuff, outval);

  if ( ++pCont->gnIndexBit == 0x10 ) {
    bOk = PutBytes( pCont->gWorkBuff, pCont->gnIndexByte + 2, pCont );
    pCont->gnIndexBit = pCont->gnIndexByte = 0;
  }
  return bOk;
}

// ---
static int PutRest( PackContext *pCont ) {
  int bOk = 0;
  WriteWordPtr ((WORD *)pCont->gWorkBuff, ReadWordPtr((WORD *)pCont->gWorkBuff) >> (0x10 - pCont->gnIndexBit));
  bOk = PutBytes( pCont->gWorkBuff, pCont->gnIndexByte + 2, pCont );
  return bOk;
}


// ---
static int Squeeze( void **pAlgoData,
                    char *pInBuff, DWORD nInLength, DWORD *nStartInCount,
                    char *pOutBuff, DWORD nOutLength, DWORD *nStartOutCount,
                    int *bStarted,
                    BufferProc pProc, void *pProcData,
                    DWORD nLookUpSize, int bFlush ) {

  DWORD      i = 0;
  short int  j;
  short int  nLength,   maxlen;
  DWORD      nStart,nBufferTop;
  int        nCharListSize = (nLookUpSize << 1) + (nLookUpSize >> 2);

  int        bOk = 1;
  short int  nSpan,
             s_minimum,
             i_minimum;
  int nBackCount = nLookUpSize;
  int nLookUpTop = nLookUpSize << 1;

	PackContext gCont;
	
  if ( *pAlgoData )
    memcpy( &gCont, *pAlgoData, sizeof(gCont) );
  else 
    memset( &gCont, 0, sizeof(gCont) );

  gCont.gpOutBuff = pOutBuff;

  gCont.gnWrittenBytes = *nStartOutCount;

  gCont.gnOCount = nOutLength;

  nBufferTop = nInLength;
  nStart = *nStartInCount;

  gCont.gpBufferProc = pProc;
  gCont.gpBufferProcData = pProcData;

  if ( bFlush ) {
    DWORD nOutSize;
    
    PackContext rcSaveContext;

    if ( PutBit(0, &gCont) ) { bOk = 0; goto ret;   }
    if ( PutBit(1, &gCont) ) { bOk = 0; goto ret;   }
    PutByte (0xff, gCont);
    PutByte (0, gCont);
    PutByte (0, gCont);
    if ( PutRest(&gCont) )  { bOk = 0; goto ret; }

    SWLiberator( gCont.gpLastOccurence );
    gCont.gpLastOccurence = 0;

    SWLiberator( gCont.gpCharList );
    gCont.gpCharList = 0;

    gCont.gnIndexBit = gCont.gnIndexByte = 0;

    memcpy( &rcSaveContext, &gCont, sizeof(rcSaveContext) );

    bOk = gCont.gpBufferProc( gCont.gpOutBuff, gCont.gnWrittenBytes, (AVP_dword *)&nOutSize, gCont.gpBufferProcData );

    memcpy( &gCont, &rcSaveContext, sizeof(rcSaveContext) );

    bOk = bOk && nOutSize == gCont.gnWrittenBytes;

    goto ret;
  }

  if ( !pInBuff ) 
    goto ret;
          
  if ( !gCont.gpLastOccurence )   {
    gCont.gpLastOccurence = (short *)SWAllocator(256 * sizeof(short));
		if ( gCont.gpLastOccurence ) {
			/* Заполнение массивов индексов */
			for ( i = 0; i < 256; i++ ) 
				gCont.gpLastOccurence[i] = -1;

			gCont.gpLastOccurence[(WORD)pInBuff[*nStartInCount]] = 0;
		}
  }

  if ( !gCont.gpCharList ) {
    gCont.gpCharList = (short *)SWAllocator(nCharListSize * sizeof(short));
		if ( gCont.gpCharList ) {
			for ( i = 0; i < (DWORD)nCharListSize; i++ )
				gCont.gpCharList[i] = -1;
		}
  }

	if ( !gCont.gpLastOccurence || !gCont.gpCharList ) {
    SWLiberator( gCont.gpLastOccurence );
    gCont.gpLastOccurence = 0;

    SWLiberator( gCont.gpCharList );
    gCont.gpCharList = 0;

		return 0;
	}

  if ( !*bStarted ) {
    if ( PutBit(1, &gCont) ) { bOk = 0; goto ret; }
    PutByte ( *pInBuff, gCont );

    nStart++;

    *bStarted = 1;
  }

  for ( i = nStart; i < nBufferTop; i++ ) {
    BYTE  *ptr = pInBuff + i;
    short int    s;
    gCont.gpCharList[i] = gCont.gpLastOccurence[ (WORD) *ptr ];
    gCont.gpLastOccurence[ (WORD) *ptr ] = (short)i;

    nLength = 0;
    nSpan = 0;
    maxlen = (short)(nBufferTop - i < 0x100 ? nBufferTop - i : 0x100);
    s_minimum = -(short)(i > (DWORD)nBackCount ? nBackCount : i);
    i_minimum =  (short)(i > (DWORD)nBackCount ? i-nBackCount : 0);

    /* Просмотр буфера назад по указателям */
    for ( j = gCont.gpCharList[i] ; j >= i_minimum; j = gCont.gpCharList[j] ) {
      short int l;

      if ( j < i_minimum ) break;
      s = (short)(j - i);

      for (l = 1; l < maxlen && *(ptr + l) == *(ptr + s + l); l++) ;

      if ( l > nLength ) {
        nLength = l;
        nSpan = s;
        if (l >= maxlen) break;
      }
    }
    if( nLength > maxlen ) nLength = maxlen;

    if ( (nSpan >= -0xff && nLength >= 2) || (nSpan < -0xff && nLength > 2) )   {
      if ( PutBit(0, &gCont) ) goto ret;

      for ( j = 1; j < nLength; j++ ) {
        i++;
        gCont.gpCharList[i] = gCont.gpLastOccurence[ (WORD) *(ptr+j) ];
        gCont.gpLastOccurence[ (WORD) *(ptr+j) ] = (short)i;
      }


      if ( nLength <= 5 && nSpan >= -0xff ) {
        nLength -= 2;
        if ( PutBit(0, &gCont) ) { bOk = 0; goto ret;}
        if ( PutBit((BYTE)(nLength >> 1), &gCont) ) { bOk = 0; goto ret; }
        if ( PutBit((BYTE)nLength, &gCont) ) { bOk = 0; goto ret; }
        PutByte( nSpan & 0xff, gCont );
      }
      else {
        if ( PutBit(1, &gCont) ) { bOk = 0; goto ret; }
        PutByte (nSpan & 0xff, gCont);
        if (nLength <= 9) {
          nLength = (nLength - 2) | (((WORD)nSpan >> 5) & ~0x7);
          PutByte( nLength, gCont );
        }
        else {
          PutByte(((WORD)nSpan >> 5) & ~0x7, gCont);
          PutByte(nLength-1, gCont); /* ! */
        }
      }
    }
    else {
      if ( PutBit(1, &gCont) ) { bOk = 0; goto ret; }
      PutByte ( pInBuff[i], gCont );
    }
    if ( i > (DWORD)nLookUpTop )    {
      int k;
      /* Коррекция указателей */
      for ( k=0; k<256; k++ )
        gCont.gpLastOccurence[k] = ( gCont.gpLastOccurence[k] < (short)nBackCount
																			? -1
																			: gCont.gpLastOccurence[k] - (short)nBackCount );

      for ( k=nBackCount; k<(int)nBufferTop; k++ ) 
         pInBuff[k-nBackCount] = pInBuff[k];

      for ( k=nBackCount; k<nCharListSize; k++ ) {
              gCont.gpCharList[k-nBackCount] = gCont.gpCharList[k] < (short)nBackCount 
																								? -1 
																								: gCont.gpCharList[k] - (short)nBackCount;
      }

      i -= nBackCount;
      nBufferTop -= nBackCount;
    }
  }

  if ( nInLength >= nLookUpSize ) {
		int nBackCount = nLookUpSize;

		if ( nBufferTop > nLookUpSize ) 
			nBackCount = nLookUpSize;
		else
			nBackCount = 0;

		for ( j=0; j<256; j++ )
			gCont.gpLastOccurence[j] = ( gCont.gpLastOccurence[j] < nBackCount 
																		? -1
																		: gCont.gpLastOccurence[j] - nBackCount );

		for ( j=nBackCount; j<(short)nBufferTop; j++ ) {
			pInBuff[j-nBackCount] = pInBuff[j];
			gCont.gpCharList[j-nBackCount] = gCont.gpCharList[j] < nBackCount 
																				? -1 
																				: gCont.gpCharList[j] - nBackCount;
		}

		*nStartInCount = nBufferTop - nBackCount;
  }
  else
    *nStartInCount = nBufferTop;

ret:

  *nStartOutCount = gCont.gnWrittenBytes;

  if ( !*pAlgoData )
    *pAlgoData = SWAllocator( sizeof(gCont) );

	if ( *pAlgoData )
		memcpy( *pAlgoData, &gCont, sizeof(gCont) );
	else {
    SWLiberator( gCont.gpLastOccurence );
    gCont.gpLastOccurence = 0;

    SWLiberator( gCont.gpCharList );
    gCont.gpCharList = 0;

		bOk = 0;
	}

  return bOk;
}




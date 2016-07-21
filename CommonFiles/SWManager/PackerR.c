// SWPackerRead.c - implementation of Packer serialization wrapper
// Reading part of a code
//
#include <string.h>
#include "Packer.h"
#include "AVPPort.h"
#include <byteorder.h>

static int Unsqueeze( void **pAlgoData,
											char *pInBuff, DWORD *nInFilled, DWORD nInLength, DWORD *nStartInCount,
										  char *pOutBuff, DWORD nOutLength, DWORD *nStartOutCount,
											int *bFirstEnter, int *bComleted,
										  BufferProc pProc, void *pProcData );


// ---
static AVP_bool SWM_PARAM SWAcceptBuffer( SWControl *This, SWManager *pManager, void *pBuffer, AVP_dword nSize, void* pUserData ) {
	AVP_bool bOk = 0;
	if ( pManager ) {
		if ( nSize == sizeof(AVP_SW_Header) ) {

			SWContext *pSWContext;

			pManager->lpVtbl->SWMEnterBufferProc( pManager );

			pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_PACKER_ID );
			if ( pSWContext ) {
				SWPackerInitData rcInitData;
				AVP_dword nOutSize;
				bOk = pSWContext->m_pSiblingProc( &rcInitData, sizeof(rcInitData), &nOutSize, pUserData ) &&
							nOutSize == sizeof(rcInitData);

				if ( bOk ) {
					rcInitData.m_nLookUpSize = ReadDWordPtr (&rcInitData.m_nLookUpSize);
					rcInitData.m_nAlgorithm = ReadWordPtr (&rcInitData.m_nAlgorithm);					
					memcpy( &((SWPackerData *)pSWContext->m_pData)->m_rcInit, &rcInitData, sizeof(SWPackerInitData) );
				}	
			}

			pManager->lpVtbl->SWMLeaveBufferProc( pManager );
		}
	}

	return bOk;
}


// ---
static SWM_PROC AVP_bool SWM_PARAM SWReadProc( void* pBuffer, AVP_dword nSize, AVP_dword *pnOutSize, void* pUserData ) {
	AVP_bool bOk = 0;
	SWManager *pManager = ((SWM_StreamInfo *)pUserData)->pSWMHandle;
	if ( pManager ) {
		if ( pBuffer ) {
			SWContext *pSWContext;

			pManager->lpVtbl->SWMEnterBufferProc( pManager );

			pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_PACKER_ID );
			if ( pSWContext ) {
				AVP_dword nOverHead = ((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nOverHead;

				if ( pSWContext->m_bProcessingCompleted ) {
					AVP_dword  nWriteNow = 0;
					if ( nOverHead ) {
						nWriteNow = min( nOverHead, nSize );
						memcpy( pBuffer, (char *)((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pCurrHistory - 
														 nOverHead, nWriteNow );
						((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nOverHead -= nWriteNow;
					}
					if ( pnOutSize )
						*pnOutSize = nWriteNow;
					bOk = 1; 
				}
				else {
					AVP_dword  nLookUpSize = ((SWPackerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize;
					AVP_dword  nHistorySize = nLookUpSize + nSize + gnGetOutSize;
					void      *pHistory = SWAllocator( nHistorySize );
					if ( pHistory ) {
						AVP_dword  nProcSize = (AVP_dword)((char *)pSWContext->m_pCurrPosition - (char *)pSWContext->m_pBuffer);
						AVP_dword  nOccSize = nLookUpSize;
						AVP_dword  nWriteNow = 0;

						if ( !((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory ) {
							((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory = pHistory;
						}
						else {
							memcpy( pHistory, (char *)((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pCurrHistory -
											nLookUpSize, nLookUpSize + nOverHead);
							SWLiberator( ((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory );
							((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory = pHistory;
						}

						((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pCurrHistory = (char *)pHistory + nLookUpSize;
						((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nHistorySize = nHistorySize;

						nHistorySize -= gnGetOutSize;
						nOccSize += nOverHead;

						bOk =Unsqueeze( &((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pAlgorithmData,
														pSWContext->m_pBuffer, (DWORD *)&pSWContext->m_nBufferFilled, pSWContext->m_nBufferSize, (DWORD *)&nProcSize,
														(char *)((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pCurrHistory + nOverHead, nHistorySize, (DWORD *)&nOccSize,
														(int *)&pSWContext->m_bProcessingStarted, (int *)&pSWContext->m_bProcessingCompleted,
														pSWContext->m_pSiblingProc, pUserData );

						nWriteNow = min(nOccSize - nLookUpSize, nSize);

						memcpy( pBuffer, (char *)((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pCurrHistory, nWriteNow );

						((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nOverHead = (nOccSize - nLookUpSize) - nWriteNow;

						((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pCurrHistory = 
										(char *)((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory + nHistorySize;
						
						if ( pnOutSize )
							*pnOutSize = nWriteNow;

						pSWContext->m_pCurrPosition = (char *)pSWContext->m_pBuffer + nProcSize;
					}
					else 
						bOk = 0;
				}
			}

			pManager->lpVtbl->SWMLeaveBufferProc( pManager );
		}
		else {
			if ( pnOutSize )
				*pnOutSize = 0;
			bOk = 1;
		}
	}

	return bOk;
}


// ---
static AVP_bool SWM_PARAM SWCreateInstance( SWControl *This, SWManager *pManager, SWControl **pSWControl ) {
	SWControl *piSWControl = 0;

	SWPackerCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pReadProc = SWReadProc;
		piSWControl->lpVtbl->SWAcceptBuffer	 = SWAcceptBuffer;
		piSWControl->lpVtbl->SWCreateInstance = SWCreateInstance;

		if ( SWMAdvise(pManager, piSWControl) ) {
			*pSWControl = piSWControl;
			return 1;
		}

		piSWControl->lpVtbl->Release( piSWControl );
	}

	return 0;
}


// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Unpacker( void *pManager ) {
	SWControl *piSWControl = 0;

	SWPackerCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pReadProc = SWReadProc;
		piSWControl->lpVtbl->SWAcceptBuffer	 = SWAcceptBuffer;
		piSWControl->lpVtbl->SWCreateInstance = SWCreateInstance;

		if ( SWMAdvise(pManager, piSWControl) ) 
			return 1;

		piSWControl->lpVtbl->Release( piSWControl );
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

// ---
static int GetBuffer( PackContext *pCont ) {
	int bOk = 1;
	if ( pCont->gnWrittenBytes >= *pCont->gnIFilled ) {
		DWORD nOutSize;

		PackContext rcSaveContext;
		memcpy( &rcSaveContext, pCont, sizeof(rcSaveContext) );

		bOk = pCont->gpBufferProc( pCont->gpInBuff, pCont->gnICount, (AVP_dword *)&nOutSize, pCont->gpBufferProcData );
		
		memcpy( pCont, &rcSaveContext, sizeof(rcSaveContext) );

		pCont->gnICount = nOutSize;
		*pCont->gnIFilled = nOutSize;
		pCont->gnWrittenBytes = 0;
	}
	return bOk;
}


// ---
static int GetBytes( BYTE *cByte, int nCount, PackContext *pCont ) {
	int i = 0;
	int bOk = 1;
	for ( ; nCount && bOk; nCount--, i++ ) {
		bOk = GetBuffer( pCont );
		if ( bOk && pCont->gnICount ) {
			*(cByte + i) = *(pCont->gpInBuff + pCont->gnWrittenBytes);
			pCont->gnWrittenBytes++;
		}
	}
	return bOk;
}

// ---
static int GetByte( BYTE *cByte, PackContext *pCont ) {
	return GetBytes( cByte, 1, pCont );
}


// ---
static int GetBit( WORD *wBit, PackContext *pCont ) {
  int bOk = 1;
  *wBit = pCont->gnNextWord & 1;
  if ( --pCont->gnCount == 0 ) {
		bOk = GetBytes( (BYTE *)&pCont->gnNextWord, 2, pCont );
		pCont->gnNextWord = ReadWordPtr (&pCont->gnNextWord);

		if ( bOk ) {
			pCont->gnCount = 0x10;
		}
  }
	else
    pCont->gnNextWord >>= 1;

  return bOk;
}

// ---
static int Unsqueeze( void **pAlgoData,
										  char *pInBuff, DWORD *pnInFilled, DWORD nInLength, DWORD *nStartInCount,
										  char *pOutBuff, DWORD nOutLength, DWORD *nStartOutCount,
											int *bStarted, int *bComleted,
										  BufferProc pProc, void *pProcData ) {
  short int nLength;
  short int nSpan;
	int bOk = 1;
	WORD wBit;
	BYTE cByte;

	PackContext gCont;

	if ( *pAlgoData )
		memcpy( &gCont, *pAlgoData, sizeof(gCont) );
	else 
		memset( &gCont, 0, sizeof(gCont) );

	gCont.gpOutBuff = pOutBuff;
	gCont.gpInBuff = pInBuff;

  gCont.gnWrittenBytes = *nStartInCount;

  gCont.gnOCount = nOutLength;
  gCont.gnICount = nInLength;
	gCont.gnIFilled = pnInFilled;

	gCont.gpBufferProc = pProc;
	gCont.gpBufferProcData = pProcData;

	if ( !*bStarted ) {
		GetBuffer( &gCont );

		gCont.gnNextWord = ReadWordPtr ((WORD*)gCont.gpInBuff);
		gCont.gnCount = 0x10;

		gCont.gnWrittenBytes += 2;

		*bStarted = 1;
	}

	while ( bOk && *nStartOutCount < gCont.gnOCount ) {
		if ( !(bOk = GetBit(&wBit, &gCont)) || !gCont.gnICount )
			break;

		if( wBit ) {
			if ( !(bOk = GetByte(&cByte, &gCont)) || !gCont.gnICount )
				break;
			
			*gCont.gpOutBuff++ = cByte;
			(*nStartOutCount)++;
			continue;
		}

		if ( !(bOk = GetBit(&wBit, &gCont)) || !gCont.gnICount )
			break;
		if( !wBit ) {
			if ( !(bOk = GetBit(&wBit, &gCont)) || !gCont.gnICount )
				break;

			nLength = wBit << 1;

			if ( !(bOk = GetBit(&wBit, &gCont)) || !gCont.gnICount )
				break;

			nLength |= wBit;
			nLength += 2;

			if ( !(bOk = GetByte(&cByte, &gCont)) || !gCont.gnICount )
				break;
			nSpan = cByte | 0xff00;
		} 
		else {
			if ( !(bOk = GetByte(&cByte, &gCont)) || !gCont.gnICount )
				break;
			nSpan = cByte;

			if ( !(bOk = GetByte(&cByte, &gCont)) || !gCont.gnICount )
				break;
			nLength = cByte;

			nSpan |= ((nLength & ~0x07)<<5) | 0xe000;
			nLength = (nLength & 0x07)+2;
			if ( nLength == 2 ) {
				if ( !(bOk = GetByte(&cByte, &gCont)) || !gCont.gnICount )
					break;
				nLength = cByte;

				if ( nLength == 0 ) {
					if ( bComleted )
						*bComleted = 1;
					break;// end mark of compreesed load module
				}
				if ( nLength == 1 )
			     continue;// segment change
				else
					nLength++;
			}
		}
		for( ; nLength>0; nLength--,gCont.gpOutBuff++ ) {
			*gCont.gpOutBuff = *(gCont.gpOutBuff + nSpan);
			(*nStartOutCount)++;
		}
	}

	*nStartInCount = gCont.gnWrittenBytes;

	if ( !gCont.gnICount && bComleted )
		*bComleted = 1;

	if ( !*pAlgoData )
		*pAlgoData = SWAllocator( sizeof(gCont) );

	if ( *pAlgoData )
		memcpy( *pAlgoData, &gCont, sizeof(gCont) );
	else
		bOk = 0;

	return bOk;
}



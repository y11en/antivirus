// SWCRCCheckerRead.c - implementation of CRCChecker serialization wrapper
// Reading part of a code
//
#include <string.h>
#include "Checker.h"
#include <AVPPort.h>
#include <byteorder.h>

// ---
static AVP_bool SWM_PARAM SWAcceptBuffer( SWControl *This, SWManager *pManager, void *pBuffer, AVP_dword nSize, void* pUserData ) {
	AVP_bool bOk = 0;
	if ( pManager ) {
		if ( nSize == (sizeof(AVP_SW_Header) ) ) {

			SWContext *pSWContext;

			pManager->lpVtbl->SWMEnterBufferProc( pManager );

			pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_CRCCHECKER_ID );
			if ( pSWContext ) {
				SWCRCCheckerInitData rcInitData;
				AVP_dword nOutSize;
				bOk = pSWContext->m_pSiblingProc( &rcInitData, sizeof(rcInitData), &nOutSize, pUserData ) &&
							nOutSize == sizeof(rcInitData);

				if ( bOk ) {
					rcInitData.m_nLookUpSize = ReadDWordPtr (&rcInitData.m_nLookUpSize);
					rcInitData.m_nAlgorithm = ReadWordPtr (&rcInitData.m_nAlgorithm);

					memcpy( &((SWCRCCheckerData *)pSWContext->m_pData)->m_rcInit, &rcInitData, sizeof(SWCRCCheckerInitData) );

					SWLiberator( pSWContext->m_pBuffer );
					pSWContext->m_nBufferSize   = ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize/* + sizeof(AVP_dword)*/;
					pSWContext->m_pBuffer				= SWAllocator( pSWContext->m_nBufferSize );
					pSWContext->m_pCurrPosition = pSWContext->m_pBuffer; 
				}
				
				bOk = !!pSWContext->m_pBuffer;
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
		bOk = 1;

		if ( pBuffer ) {
			SWContext *pSWContext;

			pManager->lpVtbl->SWMEnterBufferProc( pManager );

			pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_CRCCHECKER_ID );
			if ( pSWContext ) {
				AVP_dword  nSumOutSize = 0;
				AVP_dword nCRCCheckCRC = (AVP_dword)-1;
				AVP_dword nCRCCheckOldCRC = 0;

				if ( pSWContext->m_bProcessingCompleted ) {
					if ( pnOutSize )
						*pnOutSize = nSumOutSize;
				}
				else {
					AVP_dword  nLookUpSize = ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize;
					AVP_dword  nCountNow	 = pSWContext->m_nBufferFilled;
					AVP_dword  nOutSize		 = nSize;
					// Word! - don't forget about it!
					AVP_word  nCountReadNow = (AVP_word)nLookUpSize;

					while ( bOk && nSize && nOutSize && !pSWContext->m_bProcessingCompleted && (nCountReadNow >= (AVP_word)nLookUpSize) ) {
						AVP_dword nWriteNow = min( nCountNow, nSize );
						nCountReadNow = (AVP_word)nLookUpSize;

						if ( !nWriteNow ) {
							bOk = pSWContext->m_pSiblingProc( &nCRCCheckOldCRC, sizeof(nCRCCheckOldCRC), &nOutSize, pUserData ) &&
										(!nOutSize || nOutSize == sizeof(nCRCCheckOldCRC));
							nCRCCheckOldCRC = ReadDWordPtr (&nCRCCheckOldCRC);
							
							if ( bOk ) {
								bOk = pSWContext->m_pSiblingProc( &nCountReadNow, sizeof(nCountReadNow), &nOutSize, pUserData ) &&
											(!nOutSize || nOutSize == sizeof(nCountReadNow));
								nCountReadNow = ReadWordPtr (&nCountReadNow);
							}

							if ( bOk ) {
								bOk = pSWContext->m_pSiblingProc( pSWContext->m_pBuffer, nCountReadNow, &nOutSize, pUserData ) &&
											(!nOutSize || nOutSize == nCountReadNow);
							}

							pSWContext->m_bProcessingCompleted = !bOk || !nOutSize;
							
							if ( bOk && nOutSize ) {
								pSWContext->m_pCurrPosition = pSWContext->m_pBuffer;
								pSWContext->m_nBufferFilled = nOutSize;
								nCountNow	= pSWContext->m_nBufferFilled;

								nCRCCheckCRC = (AVP_dword)-1;
								nCRCCheckCRC = CountCRC( pSWContext->m_pBuffer, nCountNow, nCRCCheckCRC );
								bOk = (nCRCCheckCRC == nCRCCheckOldCRC);

								nWriteNow = min( nCountNow, nSize );
							}
						}

						if ( bOk && nOutSize ) {
							memcpy( pBuffer, pSWContext->m_pCurrPosition, nWriteNow );

							pBuffer = ((char*)pBuffer)+ nWriteNow;
							nSize						-= nWriteNow;
							nSumOutSize			+= nWriteNow;	
							nCountNow				-= nWriteNow;
							pSWContext->m_pCurrPosition = ((char *)(pSWContext->m_pCurrPosition)) + nWriteNow;
							pSWContext->m_nBufferFilled -= nWriteNow;
						}
						/*
						pSWContext->m_bProcessingCompleted = !bOk || 
																									(!pSWContext->m_nBufferFilled && nCountReadNow != nLookUpSize);
						*/
					}
				}

				if ( pnOutSize )
					*pnOutSize = nSumOutSize;
			}

			pManager->lpVtbl->SWMLeaveBufferProc( pManager );
		}
		else {
			if ( pnOutSize )
				*pnOutSize = 0;
		}
	}

	return bOk;
}


void SWM_PARAM SWCRCCheckerCreateSWInstance( SWControl **pSWControl );

// ---
static AVP_bool SWM_PARAM SWCreateInstance( SWControl *This, SWManager *pManager, SWControl **pSWControl ) {
	SWControl *piSWControl = 0;

	SWCRCCheckerCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pReadProc	= SWReadProc;
		piSWControl->lpVtbl->SWAcceptBuffer   = SWAcceptBuffer;
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
SWM_PROC AVP_bool SWM_PARAM SWM_Register_CRCInChecker( void *pManager ) {
	SWControl *piSWControl = 0;

	SWCRCCheckerCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pReadProc = SWReadProc;
		piSWControl->lpVtbl->SWAcceptBuffer   = SWAcceptBuffer;
		piSWControl->lpVtbl->SWCreateInstance = SWCreateInstance;

		if ( SWMAdvise(pManager, piSWControl) ) 
			return 1;

		piSWControl->lpVtbl->Release( piSWControl );
	}
	return 0;
}


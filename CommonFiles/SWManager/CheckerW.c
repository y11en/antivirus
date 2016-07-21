// SWCRCCheckerWrite.c - implementation of CRCChecker serialization wrapper
// Writing part of a code
//
#include <string.h>
#include "Checker.h"
#include "AVPPort.h"
#include <byteorder.h>

// ---
static SWM_PROC AVP_bool SWM_PARAM SWWriteProc( void* pBuffer, AVP_dword nSize, AVP_dword *pnOutSize, void* pUserData ) {
  AVP_bool bOk = 0;
  SWManager *pManager = ((SWM_StreamInfo *)pUserData)->pSWMHandle;
  if ( pManager ) {
    SWContext *pSWContext;

    pManager->lpVtbl->SWMEnterBufferProc( pManager );

    pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_CRCCHECKER_ID );
    if ( pSWContext ) {
      AVP_dword  nLookUpSize = ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize;
      AVP_dword  nCRCCheckCRC = ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcWork.m_nCRCCheckCRC;
      AVP_dword  nCRCCheckCount = ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcWork.m_nCRCCheckCount;
      AVP_dword nOutSize;

      bOk = 1;

      if ( pBuffer ) {
        if ( pSWContext->m_bHeaderProcessed == 0 ) {
          AVP_SW_Header rcHeader;
          WriteDWordPtr (&rcHeader.magic, AVP_SW_MAGIC);
          WriteWordPtr (&rcHeader.id, SW_CRCCHECKER_ID);
          
          bOk = pSWContext->m_pSiblingProc( &rcHeader, sizeof(rcHeader), &nOutSize, pUserData ) &&
                nOutSize == sizeof(rcHeader);

		  if ( bOk ) {
          	SWCRCCheckerInitData swcdata;
          	WriteDWordPtr (&swcdata.m_nLookUpSize, ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize);
          	WriteWordPtr (&swcdata.m_nAlgorithm, ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcInit.m_nAlgorithm);
          	
            bOk = pSWContext->m_pSiblingProc( &swcdata, sizeof(SWCRCCheckerInitData), &nOutSize, pUserData ) &&
																								nOutSize == sizeof(SWCRCCheckerInitData);
		  }																								

          pSWContext->m_bHeaderProcessed = 1;
        }
        if ( bOk ) {
          AVP_dword  nSumOutSize = 0;

          while ( bOk && nSize ) {
            AVP_dword nWriteNow = min( nLookUpSize - nCRCCheckCount, nSize );
            
            memcpy( pSWContext->m_pCurrPosition, pBuffer, nWriteNow );

            pBuffer = (char *)pBuffer + nWriteNow;
            pSWContext->m_pCurrPosition = ((char *)pSWContext->m_pCurrPosition) + nWriteNow;
            nSize                                           -= nWriteNow;
            nSumOutSize                     += nWriteNow;
            nCRCCheckCount  += nWriteNow;

            if ( nCRCCheckCount == nLookUpSize ) {
              // Word! - don't forget about it!
              AVP_word nCRCCheckWriteCount;
              
              WriteWordPtr (&nCRCCheckWriteCount, (AVP_word)nCRCCheckCount);
              WriteDWordPtr (&nCRCCheckCRC, CountCRC( pSWContext->m_pBuffer, nCRCCheckCount, nCRCCheckCRC) );

              bOk = bOk && pSWContext->m_pSiblingProc( &nCRCCheckCRC, sizeof(nCRCCheckCRC), &nOutSize, pUserData ) &&
                                                       nOutSize == sizeof(nCRCCheckCRC);
              bOk = bOk && pSWContext->m_pSiblingProc( &nCRCCheckWriteCount, sizeof(nCRCCheckWriteCount), &nOutSize, pUserData ) &&
                                                       nOutSize == sizeof(nCRCCheckWriteCount);
              bOk = bOk && pSWContext->m_pSiblingProc( pSWContext->m_pBuffer, nCRCCheckCount, &nOutSize, pUserData ) &&
                                                       nOutSize == nCRCCheckCount;

              nCRCCheckCount = 0;
              nCRCCheckCRC   = (AVP_dword)-1;
              pSWContext->m_pCurrPosition = pSWContext->m_pBuffer;
            }
          }

          if ( pnOutSize )
            *pnOutSize = nSumOutSize;
        }
      }
      else {
				if ( !pSWContext->m_bProcessingCompleted ) {
					if ( nSize == (AVP_dword)-1 ) {
						// Word! - don't forget about it!
						AVP_word nCRCCheckWriteCount;
		                
		                WriteWordPtr (&nCRCCheckWriteCount, (AVP_word)nCRCCheckCount);
		                WriteDWordPtr (&nCRCCheckCRC, CountCRC( pSWContext->m_pBuffer, nCRCCheckCount, nCRCCheckCRC) );

						bOk = bOk && pSWContext->m_pSiblingProc( &nCRCCheckCRC, sizeof(nCRCCheckCRC), &nOutSize, pUserData ) &&
																										 nOutSize == sizeof(nCRCCheckCRC);
						bOk = bOk && pSWContext->m_pSiblingProc( &nCRCCheckWriteCount, sizeof(nCRCCheckWriteCount), &nOutSize, pUserData ) &&
																										 nOutSize == sizeof(nCRCCheckWriteCount);
						bOk = bOk && pSWContext->m_pSiblingProc( pSWContext->m_pBuffer, nCRCCheckCount, &nOutSize, pUserData ) &&
																										 nOutSize == nCRCCheckCount;

						nCRCCheckCount = 0;
						nCRCCheckCRC   = (AVP_dword)-1;
						pSWContext->m_bProcessingCompleted = 1;
					}
				}

				bOk = bOk && pSWContext->m_pSiblingProc( pBuffer, nSize, pnOutSize, pUserData );
      }

      ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcWork.m_nCRCCheckCRC = nCRCCheckCRC;
      ((SWCRCCheckerData *)pSWContext->m_pData)->m_rcWork.m_nCRCCheckCount = nCRCCheckCount;
    }

    pManager->lpVtbl->SWMLeaveBufferProc( pManager );
  }

  return bOk;
}


void SWM_PARAM SWCRCCheckerCreateSWInstance( SWControl **pSWControl );

// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Register_CRCOutChecker( void *pManager ) {
  SWControl *piSWControl = 0;

  SWCRCCheckerCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pWriteProc = SWWriteProc;
		if ( SWMAdvise(pManager, piSWControl) ) 
			return 1;

		piSWControl->lpVtbl->Release( piSWControl );
	}

	return 0;
}


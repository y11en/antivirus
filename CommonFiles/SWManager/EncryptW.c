// SWEncrypterWrite.c - implementation of Encrypter serialization wrapper
// Writing part of a code
//
#include <string.h>
#include "Encrypt.h"
#include "AVPPort.h"
#include <byteorder.h>

// ---
static SWM_PROC AVP_bool SWM_PARAM SWWriteProc( void* pBuffer, AVP_dword nSize, AVP_dword *pnOutSize, void* pUserData ) {
  AVP_bool bOk = 0;
  SWManager *pManager = ((SWM_StreamInfo *)pUserData)->pSWMHandle;
  if ( pManager ) {
    SWContext *pSWContext;

    pManager->lpVtbl->SWMEnterBufferProc( pManager );

    pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_ENCRYPTER_ID );
    if ( pSWContext ) {
      if ( pBuffer ) {
        bOk = 1;
        if ( pSWContext->m_bHeaderProcessed == 0 ) {
          AVP_dword nOutSize;
          AVP_SW_Header rcHeader;
          WriteDWordPtr (&rcHeader.magic, AVP_SW_MAGIC);
          WriteWordPtr (&rcHeader.id, SW_ENCRYPTER_ID);
          
          bOk = pSWContext->m_pSiblingProc( &rcHeader, sizeof(rcHeader), &nOutSize, pUserData ) &&
                                  nOutSize == sizeof(rcHeader);

          if ( bOk ) {
			AVP_word nAlgorithm;
			WriteWordPtr (&nAlgorithm, ((SWEncrypterData *)pSWContext->m_pData)->m_rcInit.m_nAlgorithm);
            bOk = pSWContext->m_pSiblingProc( &nAlgorithm, sizeof(SWEncrypterInitData), &nOutSize, pUserData ) &&
																							nOutSize == sizeof(SWEncrypterInitData);
		  }																							

          pSWContext->m_bHeaderProcessed = 1;
        }
        if ( bOk ) {
          AVP_dword nBufferSize = max( pSWContext->m_nBufferSize, nSize );
          if ( nBufferSize > pSWContext->m_nBufferSize ) {
            SWLiberator( pSWContext->m_pBuffer );
            pSWContext->m_pBuffer = SWAllocator( nBufferSize );
            pSWContext->m_nBufferSize = nBufferSize;
						bOk = !!pSWContext->m_pBuffer;
          }
					if ( pSWContext->m_pBuffer )
						memcpy( pSWContext->m_pBuffer, pBuffer, nSize );

					if ( bOk )
						bOk = EDBinary( &((SWEncrypterData *)pSWContext->m_pData)->m_rcWork.m_pAlgorithmData, 
														pSWContext->m_pBuffer, nSize );

					if ( bOk )
            bOk = pSWContext->m_pSiblingProc( pSWContext->m_pBuffer, nSize, pnOutSize, pUserData );
        }
      }
      else
        bOk = pSWContext->m_pSiblingProc( pBuffer, nSize, pnOutSize, pUserData );
    }

    pManager->lpVtbl->SWMLeaveBufferProc( pManager );
  }

  return bOk;
}


void SWM_PARAM SWEncrypterCreateSWInstance( SWControl **pSWControl );

// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Encrypter( void *pManager ) {
  SWControl *piSWControl = 0;

  SWEncrypterCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pWriteProc = SWWriteProc;
		if ( SWMAdvise(pManager, piSWControl) ) 
			return 1;

		piSWControl->lpVtbl->Release( piSWControl );
	}
	return 0;
}


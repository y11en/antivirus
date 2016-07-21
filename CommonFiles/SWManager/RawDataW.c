// SWRawDataWrite.c - implementation of RawData serialization wrapper
// Writing part of a code
//
#include <string.h>
#include "RawData.h"
#include "AVPPort.h"
#include <byteorder.h>


// ---
static SWM_PROC AVP_bool SWM_PARAM SWWriteProc( void* pBuffer, AVP_dword nSize, AVP_dword *pnOutSize, void* pUserData ) {
  AVP_bool bOk = 0;
  SWManager *pManager = ((SWM_StreamInfo *)pUserData)->pSWMHandle;
  if ( pManager ) {
    SWContext *pSWContext;

    pManager->lpVtbl->SWMEnterBufferProc( pManager );

    pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_RAWDATA_ID );
    if ( pSWContext ) {
      bOk = 1;
      if ( pSWContext->m_bHeaderProcessed == 0 ) {
        AVP_dword nOutSize;
        AVP_SW_Header rcHeader;
		WriteDWordPtr (&rcHeader.magic, AVP_SW_MAGIC);
		WriteWordPtr (&rcHeader.id, SW_RAWDATA_ID);

        bOk = pSWContext->m_pSiblingProc( &rcHeader, sizeof(rcHeader), &nOutSize, pUserData ) &&
							nOutSize == sizeof(rcHeader);

        pSWContext->m_bHeaderProcessed = 1;
      }
      if ( bOk ) 
        bOk = pSWContext->m_pSiblingProc( pBuffer, nSize, pnOutSize, pUserData );
    }

    pManager->lpVtbl->SWMLeaveBufferProc( pManager );
  }

  return bOk;
}


void SWM_PARAM SWRawDataCreateSWInstance( SWControl **pSWControl );

// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Register_RawData( void *pManager ) {
	SWControl *piSWControl = 0;

	SWRawDataCreateSWInstance( &piSWControl );

	if ( piSWControl && piSWControl->m_pSWContext && piSWControl->lpVtbl ) {
		piSWControl->m_pSWContext->m_pWriteProc = SWWriteProc;
		if ( SWMAdvise(pManager, piSWControl) ) 
			return 1;

		piSWControl->lpVtbl->Release( piSWControl );
	}

	return 0;
}


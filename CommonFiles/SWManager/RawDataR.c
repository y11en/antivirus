// SWRawDataRead.c - implementation of RawData serialization wrapper
// Reading part of a code
//
#include <string.h>
#include "RawData.h"
#include "AVPPort.h"
#include <byteorder.h>

// ---
static AVP_bool SWM_PARAM SWAcceptBuffer( SWControl *This, SWManager *pManager, void *pBuffer, AVP_dword nBufferSize, void* pUserData ) {
	return 1;
}

// ---
static AVP_bool SWLoadBuffer( SWManager *pManager, SWContext *pSWContext, void* pBuffer, AVP_dword nSize, AVP_dword *pnOutSize, void* pUserData ) {
	AVP_bool bOk = 0;

	if ( pSWContext->m_bHeaderProcessed == 0 ) {
		AVP_SW_Header rcSWHeader;
		AVP_dword nOutSize;
		bOk = pSWContext->m_pSiblingProc( &rcSWHeader, sizeof(AVP_SW_Header), &nOutSize, pUserData );
		rcSWHeader.magic = ReadDWordPtr (&rcSWHeader.magic);
		rcSWHeader.id = ReadWordPtr (&rcSWHeader.id);		
		
		if ( nOutSize >= sizeof(AVP_SW_Header) ) {
			if ( rcSWHeader.magic == AVP_SW_MAGIC ) {
				if ( rcSWHeader.id == SW_RAWDATA_ID ||
						 rcSWHeader.id ==	AVP_SW_PID_DATA_TREE ) {
					
					bOk = pSWContext->m_pSiblingProc( pBuffer, nSize, &nOutSize, pUserData );
					
					pSWContext->m_bHeaderProcessed = 1;

					if ( pnOutSize )
						*pnOutSize = nOutSize;
				}
				else {
					bOk = pManager->lpVtbl->SWMAttachSW( pManager, rcSWHeader.id, SW_RAWDATA_ID, 
																			 &rcSWHeader, nOutSize, pUserData );

					if ( bOk ) 
						bOk = SWLoadBuffer( pManager, pSWContext, pBuffer, nSize, pnOutSize, pUserData );
				}
			}
			else
				bOk = 0;
		}
		else
			bOk = 0;
	}
	else 
		bOk = pSWContext->m_pSiblingProc( pBuffer, nSize, pnOutSize, pUserData );

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

			pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_RAWDATA_ID );
			if ( pSWContext ) {
				bOk = SWLoadBuffer( pManager, pSWContext, pBuffer, nSize, pnOutSize, pUserData );
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


void SWM_PARAM SWRawDataCreateSWInstance( SWControl **pSWControl );

// ---
static AVP_bool SWM_PARAM SWCreateInstance( SWControl *This, SWManager *pManager, SWControl **pSWControl ) {
	SWControl *piSWControl = 0;

	SWRawDataCreateSWInstance( &piSWControl );

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
SWM_PROC AVP_bool SWM_PARAM SWM_Register_UnRawData( void *pManager ) {
	SWControl *piSWControl = 0;

	SWRawDataCreateSWInstance( &piSWControl );

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


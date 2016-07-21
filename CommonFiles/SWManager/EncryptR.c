// SWEncrypterRead.c - implementation of Encrypter serialization wrapper
// Reading part of a code
//
#include <string.h>
#include "Encrypt.h"
#include "AVPPort.h"
#include <byteorder.h>

// ---
static AVP_bool SWM_PARAM SWAcceptBuffer( SWControl *This, SWManager *pManager, void *pBuffer, AVP_dword nSize, void* pUserData ) {
	AVP_bool bOk = 0;
	if ( pManager ) {
		if ( nSize >= sizeof(AVP_SW_Header) ) {

			SWContext *pSWContext;

			pManager->lpVtbl->SWMEnterBufferProc( pManager );

			pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_ENCRYPTER_ID );
			if ( pSWContext ) {
				SWEncrypterInitData rcInitData;
				AVP_dword nOutSize;
				bOk = pSWContext->m_pSiblingProc( &rcInitData, sizeof(rcInitData), &nOutSize, pUserData ) &&
							nOutSize == sizeof(rcInitData);

				if ( bOk )  {
					rcInitData.m_nAlgorithm = ReadWordPtr (&rcInitData.m_nAlgorithm);
					memcpy( &((SWEncrypterData *)pSWContext->m_pData)->m_rcInit, &rcInitData, sizeof(SWEncrypterInitData) );
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
		bOk = 1;

		if ( pBuffer ) {
			SWContext *pSWContext;

			pManager->lpVtbl->SWMEnterBufferProc( pManager );

			pSWContext = pManager->lpVtbl->SWMGetContext( pManager, SW_ENCRYPTER_ID );
			if ( pSWContext ) {
				AVP_dword nOutSize = pSWContext->m_nBufferFilled;

				AVP_dword nBufferSize = max( pSWContext->m_nBufferSize, nSize );
				if ( nBufferSize > pSWContext->m_nBufferSize ) {
					SWLiberator( pSWContext->m_pBuffer );
					pSWContext->m_pBuffer = SWAllocator( nBufferSize );
					pSWContext->m_nBufferSize = nBufferSize;
					bOk = !!pSWContext->m_pBuffer;
				}

				if ( bOk ) {
					bOk = pSWContext->m_pSiblingProc( pSWContext->m_pBuffer, nSize, &nOutSize, pUserData );

					if ( bOk && nOutSize ) {
						bOk = EDBinary( &((SWEncrypterData *)pSWContext->m_pData)->m_rcWork.m_pAlgorithmData, 
														pSWContext->m_pBuffer, nOutSize );

						if ( bOk )
							memcpy( pBuffer, pSWContext->m_pBuffer, nOutSize );
					}
					if ( pnOutSize )
						*pnOutSize = nOutSize;
				}
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


void SWM_PARAM SWEncrypterCreateSWInstance( SWControl **pSWControl );

// ---
static AVP_bool SWM_PARAM SWCreateInstance( SWControl *This, SWManager *pManager, SWControl **pSWControl ) {
	SWControl *piSWControl = 0;

	SWEncrypterCreateSWInstance( &piSWControl );

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
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Decrypter( void *pManager ) {
	SWControl *piSWControl = 0;

	SWEncrypterCreateSWInstance( &piSWControl );

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


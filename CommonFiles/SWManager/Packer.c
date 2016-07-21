// SWPacker.c - implementation of Packer serialization wrapper
// Main part of a code
//
#include <string.h>
#include "Packer.h"

static const AVP_dword gnLookUpSize = LOOKUP_SIZE;
const AVP_dword gnGetOutSize = (LOOKUP_SIZE >> 2);

const unsigned short SW_PACKER_ID = AVP_SW_PID_PACKER;

// ---
static unsigned int SWM_PARAM AddRef( SWControl *This ) {
  return ++This->m_cRef;
}

static void SWM_PARAM SWReleaseContext( SWControl *This);

// ---
static unsigned int SWM_PARAM Release( SWControl *This ) {
  if ( --This->m_cRef == 0 ) {
    SWReleaseContext( This );
    SWLiberator( This->lpVtbl );
    SWLiberator( This );
    return 0;
  }
  return This->m_cRef;
}


// ---
static AVP_dword SWM_PARAM SWGetId( SWControl *This ) {
  return SW_PACKER_ID;
}

// ---
static void SWM_PARAM SWReleaseContext( SWControl *This ) {
	if ( This->m_pSWContext ) {
		if ( (SWPackerData *)This->m_pSWContext->m_pData ) {
			SWLiberator( ((SWPackerData *)This->m_pSWContext->m_pData)->m_rcWork.m_pHistory );
			if ( ((SWPackerData *)This->m_pSWContext->m_pData)->m_rcWork.m_pAlgorithmData ) {
				SWLiberator( ((PackContext *)((SWPackerData *)This->m_pSWContext->m_pData)->m_rcWork.m_pAlgorithmData)->gpLastOccurence );
				SWLiberator( ((PackContext *)((SWPackerData *)This->m_pSWContext->m_pData)->m_rcWork.m_pAlgorithmData)->gpCharList );
			}
			SWLiberator( ((SWPackerData *)This->m_pSWContext->m_pData)->m_rcWork.m_pAlgorithmData );
		}
		SWLiberator( This->m_pSWContext->m_pData );
		SWLiberator( This->m_pSWContext->m_pBuffer );
		SWLiberator( This->m_pSWContext );
	}
}

// ---
static SWContext *SWM_PARAM SWCreateContext( void ) {
  SWContext *pSWContext = (SWContext *)SWAllocator( sizeof(SWContext) );

	if ( pSWContext ) {
		pSWContext->m_pBuffer               = SWAllocator( BUFFER_SIZE );
		pSWContext->m_pCurrPosition					= pSWContext->m_pBuffer; 
		pSWContext->m_nBufferSize           = BUFFER_SIZE;
		pSWContext->m_nBufferFilled         = 0;
		pSWContext->m_bHeaderProcessed			= 0;
		pSWContext->m_pSiblingProc					= 0;
		pSWContext->m_pWriteProc            = 0;
		pSWContext->m_pReadProc             = 0;
		pSWContext->m_nLevel                = 0;
		pSWContext->m_bProcessingCompleted	= 0;
		pSWContext->m_bProcessingStarted    = 0;
		pSWContext->m_pData                 = SWAllocator( sizeof(SWPackerData) );

		if ( !pSWContext->m_pBuffer || !pSWContext->m_pData ) {
			SWLiberator( pSWContext->m_pBuffer );
			SWLiberator( pSWContext->m_pData );
			SWLiberator( pSWContext );
			pSWContext = 0;
		}
		else {
			((SWPackerData *)pSWContext->m_pData)->m_rcInit.m_nLookUpSize = gnLookUpSize;
			((SWPackerData *)pSWContext->m_pData)->m_rcInit.m_nAlgorithm    = 0;

			((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pHistory = 0;
			((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pCurrHistory = 0;
			((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nOverHead = 0;
			((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_nHistorySize = gnLookUpSize;
			((SWPackerData *)pSWContext->m_pData)->m_rcWork.m_pAlgorithmData = 0;
		}
	}

  return pSWContext;
}


void SWM_PARAM SWPackerCreateSWInstance( SWControl **pSWControl );

// ---
static AVP_bool SWM_PARAM SWDuplicate( SWControl *This, SWControl **pSWControl ) {
  SWPackerCreateSWInstance( pSWControl );

	if ( *pSWControl ) {
		(*pSWControl)->lpVtbl->SWAcceptBuffer = This->lpVtbl->SWAcceptBuffer;
		(*pSWControl)->lpVtbl->SWCreateInstance = This->lpVtbl->SWCreateInstance;

		(*pSWControl)->m_pSWContext->m_pReadProc = This->m_pSWContext->m_pReadProc;
		(*pSWControl)->m_pSWContext->m_pWriteProc = This->m_pSWContext->m_pWriteProc;

		return 1;
	}
	return 0;
}

// ---
void SWM_PARAM SWPackerCreateSWInstance( SWControl **pSWControl ) {
  SWControl *piSWControl = (SWControl *)SWAllocator( sizeof(SWControl) );

	if ( piSWControl ) {
		piSWControl->m_cRef = 1;
		piSWControl->lpVtbl = (SWControlVtbl *)SWAllocator( sizeof(SWControlVtbl) );
		if ( piSWControl->lpVtbl ) {
			piSWControl->lpVtbl->AddRef = AddRef;
			piSWControl->lpVtbl->Release = Release;
			piSWControl->lpVtbl->SWGetId = SWGetId;
			piSWControl->lpVtbl->SWDuplicate = SWDuplicate;
			piSWControl->lpVtbl->SWAcceptBuffer = 0;
			piSWControl->lpVtbl->SWCreateInstance = 0;

			piSWControl->m_pSWContext = SWCreateContext();

			if ( !piSWControl->m_pSWContext ) {
				SWLiberator( piSWControl->lpVtbl );
				SWLiberator( piSWControl );
				piSWControl = 0;
			}
		}
		else {
			SWLiberator( piSWControl );
			piSWControl = 0;
		}
	}
  *pSWControl = piSWControl;
}




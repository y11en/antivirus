// SWRawData.c - implementation of RawData serialization wrapper
// Main part of a code
//
#include <string.h>
#include "Swmi.h"

const unsigned short SW_RAWDATA_ID = AVP_SW_PID_RAWDATA;

// ---
static unsigned int SWM_PARAM AddRef( SWControl *This ) {
  return ++This->m_cRef;
}


static void SWM_PARAM SWReleaseContext( SWControl *This );

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
  return SW_RAWDATA_ID;
}

// ---
static void SWM_PARAM SWReleaseContext( SWControl *This ) {
	if ( This->m_pSWContext ) {
		SWLiberator( This->m_pSWContext->m_pBuffer );
		SWLiberator( This->m_pSWContext );
	}
}


// ---
static SWContext *SWM_PARAM SWCreateContext( void ) {
  SWContext *pSWContext = (SWContext *)SWAllocator( sizeof(SWContext) );

	if ( pSWContext ) {
		pSWContext->m_pBuffer              = 0;
		pSWContext->m_pCurrPosition				 = 0; 
		pSWContext->m_nBufferSize          = 0;
		pSWContext->m_nBufferFilled        = 0;
		pSWContext->m_bHeaderProcessed		 = 0;
		pSWContext->m_pSiblingProc         = 0;
		pSWContext->m_pWriteProc           = 0;
		pSWContext->m_pReadProc            = 0;
		pSWContext->m_nLevel               = 0;
		pSWContext->m_bProcessingCompleted = 0;
		pSWContext->m_bProcessingStarted   = 0;
		pSWContext->m_pData                = 0;
	}
  return pSWContext;
}


void SWM_PARAM SWRawDataCreateSWInstance( SWControl **pSWControl );

// ---
static AVP_bool SWM_PARAM SWDuplicate( SWControl *This, SWControl **pSWControl ) {
  SWRawDataCreateSWInstance( pSWControl );

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
void SWM_PARAM SWRawDataCreateSWInstance( SWControl **pSWControl ) {
  SWControl *piSWControl = (SWControl *)SWAllocator( sizeof(SWControl) );

	if ( piSWControl ) {
		piSWControl->m_cRef = 1;
		piSWControl->lpVtbl = (SWControlVtbl *)SWAllocator( sizeof(SWControlVtbl) );
		if ( piSWControl->lpVtbl ) {
			piSWControl->lpVtbl->AddRef = AddRef;
			piSWControl->lpVtbl->Release = Release;
			piSWControl->lpVtbl->SWGetId = SWGetId;
			piSWControl->lpVtbl->SWAcceptBuffer = 0;
			piSWControl->lpVtbl->SWCreateInstance = 0;
			piSWControl->lpVtbl->SWDuplicate = SWDuplicate;

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



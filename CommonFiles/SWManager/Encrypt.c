// SWEncrypter.c - implementation of Encrypter serialization wrapper
// Main part of a code
//
#include <string.h>
#include "Encrypt.h"
#include <byteorder.h>

const unsigned short SW_ENCRYPTER_ID = AVP_SW_PID_ENCRYPTER;
const int            BUFFER_SIZE = 0x4000;

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
  return SW_ENCRYPTER_ID;
}

// ---
static void SWM_PARAM SWReleaseContext( SWControl *This ) {
	if ( This->m_pSWContext ) {
		if ( (SWEncrypterData *)This->m_pSWContext->m_pData ) {
			SWLiberator( ((SWEncrypterData *)This->m_pSWContext->m_pData)->m_rcWork.m_pAlgorithmData );
		}
		SWLiberator( This->m_pSWContext->m_pData );
		SWLiberator( This->m_pSWContext->m_pBuffer );
	}
  SWLiberator( This->m_pSWContext );
}


// ---
static SWContext *SWM_PARAM SWCreateContext( void ) {
  SWContext *pSWContext = (SWContext *)SWAllocator( sizeof(SWContext) );

	if ( pSWContext ) {
		pSWContext->m_pBuffer               = SWAllocator( BUFFER_SIZE );
		pSWContext->m_pCurrPosition					= 0; 
		pSWContext->m_nBufferSize           = BUFFER_SIZE;
		pSWContext->m_nBufferFilled         = 0;
		pSWContext->m_bHeaderProcessed			= 0;
		pSWContext->m_pSiblingProc					= 0;
		pSWContext->m_pWriteProc            = 0;
		pSWContext->m_pReadProc             = 0;
		pSWContext->m_nLevel                = 0;
		pSWContext->m_bProcessingCompleted	= 0;
		pSWContext->m_bProcessingStarted    = 0;
		pSWContext->m_pData                 = SWAllocator( sizeof(SWEncrypterData) );

		if ( !pSWContext->m_pBuffer || !pSWContext->m_pData ) {
			SWLiberator( pSWContext->m_pBuffer );
			SWLiberator( pSWContext->m_pData );
			SWLiberator( pSWContext );
			pSWContext = 0;
		}
		else {
			((SWEncrypterData *)pSWContext->m_pData)->m_rcInit.m_nAlgorithm = 0;
			((SWEncrypterData *)pSWContext->m_pData)->m_rcWork.m_pAlgorithmData = 0;
		}
	}
  return pSWContext;
}

void SWM_PARAM SWEncrypterCreateSWInstance( SWControl **pSWControl );

// ---
static AVP_bool SWM_PARAM SWDuplicate( SWControl *This, SWControl **pSWControl ) {
  SWEncrypterCreateSWInstance( pSWControl );

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
void SWM_PARAM SWEncrypterCreateSWInstance( SWControl **pSWControl ) {
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


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct EncryptData {
  AVP_word m_nEncryptKey;
  AVP_bool m_bOdd;
} EncryptData;


// Do not change this value - there are files encrypted with it
#define ENCRYPT_KEY (0x7773 ^ 0x4c4b) //('ws'^'LK')

// ---
SWM_PROC AVP_bool SWM_PARAM EDBinary( void **pAlgoData, unsigned char *pCryptBuff,  int nCryptBuffLen ) {

  EncryptData rcData;
  int         nSize; 
	AVP_bool bOk = 1;

  if ( *pAlgoData )
    memcpy( &rcData, *pAlgoData, sizeof(rcData) );
  else {
    rcData.m_nEncryptKey = (AVP_word)ENCRYPT_KEY;
    rcData.m_bOdd = 1;
  }

  if ( !rcData.m_bOdd ) {
    *pCryptBuff ^= (unsigned char)(rcData.m_nEncryptKey >> 8);
    nCryptBuffLen--;
    pCryptBuff++;
  }

  for ( nSize=nCryptBuffLen >> 1; nSize; nSize-- ) {
    int nBit = (rcData.m_nEncryptKey & 1);
    //*((AVP_word *)pCryptBuff) ^= rcData.m_nEncryptKey;
    WriteWordPtr ((AVP_word *)pCryptBuff, ReadWordPtr ((AVP_word *)pCryptBuff) ^ rcData.m_nEncryptKey);
    pCryptBuff = (unsigned char*)((AVP_word *)pCryptBuff + 1);
    rcData.m_nEncryptKey >>= 1;
    rcData.m_nEncryptKey |= ((nBit ^ (rcData.m_nEncryptKey & 1)) << 15);
  }

  rcData.m_bOdd = !(nCryptBuffLen % 2);
  if ( !rcData.m_bOdd ) 
    *pCryptBuff ^= (unsigned char)rcData.m_nEncryptKey;

  if ( !*pAlgoData )
    *pAlgoData = SWAllocator( sizeof(rcData) );

	if ( *pAlgoData )
		memcpy( *pAlgoData, &rcData, sizeof(rcData) );
	else
		bOk = 0;

	return bOk;
}


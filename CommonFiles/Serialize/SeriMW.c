// KLSerializeMemWrite.c - implementation of KLDataTreeSerialization
// Writing MEM part of a code
//
#include <string.h>

#include <Serialize/KLDTSer.h>

#define BUFFER_SIZE (0x4000)

extern void* (* DTAllocator)(AVP_uint);
extern void  (* DTLiberator)(void*);


// ---
typedef struct {
	AVP_char  *m_pBuffer;
	AVP_dword  m_nBufferSize;
	AVP_dword  m_nWritten;
} SaveToMemInfo;


#define VALID_INFO_BUFFER(a) (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_pBuffer && ((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_nBufferSize)
#define INFO_POINTER(a)			 (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_pBuffer)
#define INFO_SIZE(a)				 (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_nBufferSize)
#define INFO_WRITTEN(a)			 (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_nWritten)

// ---
AVP_bool SaveToMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData ) {
	AVP_bool  bOk = 1;

	if ( pBuffer ) {
		if ( !VALID_INFO_BUFFER(pUserData) || (INFO_WRITTEN(pUserData) + nSize) <= INFO_SIZE(pUserData) ) {
			if ( INFO_POINTER(pUserData) )
				memcpy( INFO_POINTER(pUserData) + INFO_WRITTEN(pUserData), pBuffer, nSize );

			INFO_WRITTEN(pUserData) += nSize;
		}
		else
			bOk = 0;
	}

	if ( bOk && pdwcbWritten )
		*pdwcbWritten = nSize;

	return bOk;
}



#define VALID_BUFFER (pBuffer && nBufferSize)


// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemoryWrite( AVP_char *pBuffer, 
																														AVP_dword nBufferSize,
																														HDATA hRootData,
																														const TCHAR *pszDescription,
																														SWM_Info *pSWMInfo, 
																														SWM_StreamInfo *pSWMStreamInfo, 
																														AVP_bool bWriteRawDataHeader ) {

	AVP_Serialize_Data rcSerData;
	SaveToMemInfo      rcSaveInfo;
	AVP_dword					 nSerSize;
	AVP_dword					 nWritten;
	AVP_char          *pTempBuffer = 0;
	AVP_dword					 nTempSize;

	BOOL bOk = TRUE;

	nWritten = KLDT_SerializeHeadersToMemory( pBuffer, nBufferSize, pszDescription, bWriteRawDataHeader );

	memset( &rcSerData, 0, sizeof(rcSerData) );

	//DATA_Serialize( hRootData, NULL, &rcSerData, NULL, 0, &nSerSize );
	//nTempSize = (max(nSerSize, BUFFER_SIZE) << 1) + nWritten;

	nTempSize = BUFFER_SIZE;
	
	pTempBuffer = DTAllocator( nTempSize );

	if ( pTempBuffer ) {
		/*
		rcSaveInfo.m_pBuffer		 = pBuffer ? pBuffer : pTempBuffer;
		rcSaveInfo.m_nBufferSize = nBufferSize ? nBufferSize : nTempSize;
		rcSaveInfo.m_nWritten		 = nWritten;
		*/
		rcSaveInfo.m_pBuffer		 = pBuffer;
		rcSaveInfo.m_nBufferSize = nBufferSize;
		rcSaveInfo.m_nWritten		 = nWritten;
		
		pSWMStreamInfo->pUserData = &rcSaveInfo;

		rcSerData.user_data = (void *)pSWMStreamInfo;
		rcSerData.proc = (Buffer_Proc)pSWMInfo->m_pProc;

		bOk = DATA_Serialize( hRootData, NULL, &rcSerData, 
													pTempBuffer, nTempSize, 
													(AVP_dword *)&nSerSize );

		if ( bOk )
			bOk = pSWMInfo->m_pProc( 0, (AVP_dword)-1, 0, (void *)pSWMStreamInfo );

		nWritten = rcSaveInfo.m_nWritten;

		if ( !bOk )
			_RPT0( _CRT_ASSERT, _T("Cannot write to memory") );

		DTLiberator( pTempBuffer );
	}
	else {
		_RPT0( _CRT_ASSERT, _T("Memory allocation error") );
		nWritten = 0;
	}

	return nWritten;
}


#undef VALID_BUFFER



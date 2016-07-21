// KLDeserializeMem.c - implementation of KLDataTreeSerialization
// Reading MEM part of a code
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
	AVP_dword  m_nRead;
} LoadFromMemInfo;


#define VALID_INFO_BUFFER(a) (a->m_pBuffer && a->m_nBufferSize)
#define VALID_BUFFER (pBuffer && nBufferSize)

// ---
AVP_bool LoadFromMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbRead, void* pUserData ) {
	if ( pBuffer ) {
		LoadFromMemInfo *pInfo = (LoadFromMemInfo *)((SWM_StreamInfo *)pUserData)->pUserData;
//		if ( !VALID_INFO_BUFFER(pInfo) /*|| pInfo->m_nRead + nSize <= pInfo->m_nBufferSize */) {
			nSize = min( nSize, pInfo->m_nBufferSize - pInfo->m_nRead );

			if ( pInfo->m_pBuffer )
				memcpy( pBuffer, pInfo->m_pBuffer + pInfo->m_nRead, nSize );

			pInfo->m_nRead += nSize;

			if ( pdwcbRead )
				*pdwcbRead = nSize;
/*
		}
		else {
			if ( pdwcbRead )
				*pdwcbRead = 0;
		}
*/
	}
	else {
		if ( pdwcbRead )
			*pdwcbRead = 0;
	}

	return 1;
}


// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeFromMemoryRead( AVP_char *pBuffer, 
																															 AVP_dword nBufferSize,
																															 HDATA *hRootData,
																															 SWM_Info *pSWMInfo, 
																															 SWM_StreamInfo *pSWMStreamInfo,
																															 AVP_bool bReadRawDataHeader ) {

	AVP_Serialize_Data	rcSerData;
	LoadFromMemInfo     rcLoadInfo;
	HDATA								hResultData;
	AVP_dword						nRead;
	AVP_dword						nWillBeRead;
	AVP_char           *pTempBuffer; 

	if ( !DTAllocator || !DTLiberator ) {
    //_RPT0( _CRT_ASSERT, _T("Library was not initiated") );
    return 0;
	}

	if ( !VALID_BUFFER ) {
		//_RPT0( _CRT_ASSERT, _T("Wrong parameters") );
		return 0;
	}

	nRead = KLDT_DeserializeHeadersFromMemory( pBuffer, nBufferSize, bReadRawDataHeader );

	pTempBuffer = DTAllocator( BUFFER_SIZE );

	if ( pTempBuffer ) {
		rcLoadInfo.m_pBuffer		 = pBuffer;
		rcLoadInfo.m_nBufferSize = nBufferSize;
		rcLoadInfo.m_nRead			 = nRead;

		pSWMStreamInfo->pUserData = &rcLoadInfo;

		memset( &rcSerData, 0, sizeof(rcSerData) );
		rcSerData.user_data = (void *)pSWMStreamInfo;
		rcSerData.proc = (Buffer_Proc)pSWMInfo->m_pProc;

		nWillBeRead = nBufferSize - nRead;
		hResultData = DATA_Deserialize( &rcSerData, pTempBuffer, BUFFER_SIZE, (AVP_dword *)&nWillBeRead );
		if ( !hResultData )	{
			_RPT0( _CRT_ASSERT, _T("Cannot deserialize data tree") );
			DTLiberator( pTempBuffer );
			return 0;
		}
		else {
			if ( *hRootData )
				DATA_Attach( *hRootData, NULL, hResultData, DATA_IF_ROOT_INCLUDE );
			else
				*hRootData = hResultData;
		}

		nRead += nWillBeRead;

		DTLiberator( pTempBuffer );
	}
	else {
		_RPT0( _CRT_ASSERT, _T("Memory allocation error") );
		nRead = 0;
	}

	return nRead;
}

// Deserialize data header from memory
// pBuffer        - pointer to deserialization buffer
// nBufferSize		- amount of bytes in buffer
// pKLHeader      - pointer to KL header structure (might be 0)
// ppszDescription - pointer to pointer to buffer (allocated by function)	for description (might be 0)
//                   Should be freed by "free" function
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeaderFromMemory( AVP_char *pBuffer, 
																																AVP_dword nBufferSize,
																																AVP_KL_Header *pKLHeader,
																																TCHAR **ppszDescription ) {


	if ( !DTAllocator || !DTLiberator ) {
    _RPT0( _CRT_ASSERT, _T("Library was not initiated") );
    return 0;
	}

	if ( !VALID_BUFFER )
		_RPT0( _CRT_ASSERT, _T("Wrong parameters") );

	return !!KLDT_DeserializeFullHeadersFromMemory( pBuffer, nBufferSize, pKLHeader, ppszDescription );

}

#undef VALID_BUFFER




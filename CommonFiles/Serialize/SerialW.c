// KLSerializeWrite.c - implementation of KLDataTreeSerialization
// Writing part of a code
//
#include <stdarg.h>
#include <string.h>

#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>

#define BUFFER_SIZE (0x4000)

extern void* (* DTAllocator)(AVP_uint);
extern void  (* DTLiberator)(void*);

// ---
AVP_bool SaveToFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData ) {
	AVP_bool  bOk = 1;
	AVP_dword dwcbWritten = 0;

	if ( pBuffer ) 
		bOk = AvpWriteFile( (HANDLE)((SWM_StreamInfo *)pUserData)->pUserData, pBuffer, nSize, (DWORD *)&dwcbWritten, NULL ) && dwcbWritten == nSize;

	if ( pdwcbWritten )
		*pdwcbWritten = dwcbWritten;

	return bOk;
}


// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeWrite( const TCHAR *pszFileName, 
																									 HDATA hRootData, 
																									 const TCHAR *pszDescription,
																									 SWM_Info *pSWMInfo, 
																									 SWM_StreamInfo *pSWMStreamInfo,
																									 AVP_bool bWriteRawDataHeader ) {
	HANDLE hFile;

	BOOL bOk = TRUE;

	if ( !DTAllocator || !DTLiberator ) {
    _RPT0( _CRT_ASSERT, _T("Library was not initiated") );
    return 0;
	}

	if ( !hRootData ) {
    _RPT0( _CRT_ASSERT, _T("Wrong input parameters") );
    return 0;
	}
  
  hFile = AvpCreateFile( pszFileName, GENERIC_READ | GENERIC_WRITE, 
                          FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL );

	if ( hFile == INVALID_HANDLE_VALUE ) {
    _RPT0( _CRT_ASSERT, _T("Cannot create file") );
    return 0;
	}

	bOk = KLDT_SerializeHeaders( hFile, pszDescription, bWriteRawDataHeader );

	if ( bOk ) {
	
		AVP_Serialize_Data rcSerData;
		AVP_dword nSerSize;
		AVP_dword nOutSize;
		AVP_dword nBufSize;
		char *pBuffer;

		pSWMStreamInfo->pUserData = hFile;

		memset( &rcSerData, 0, sizeof(rcSerData) );

		DATA_Serialize( hRootData, NULL, &rcSerData, NULL, 0, &nSerSize );

		nBufSize = min(nSerSize, BUFFER_SIZE);

		pBuffer = DTAllocator( nBufSize );

		if ( pBuffer ) {
			rcSerData.user_data = (void *)pSWMStreamInfo;
			rcSerData.proc = (Buffer_Proc)pSWMInfo->m_pProc;

			bOk = DATA_Serialize( hRootData, NULL, &rcSerData, pBuffer, nBufSize, &nOutSize ) && 
						nOutSize == nSerSize;

			if ( bOk )
				bOk = pSWMInfo->m_pProc( 0, (AVP_dword)-1, 0, (void *)pSWMStreamInfo );

			if ( !bOk )
				_RPT0( _CRT_ASSERT, _T("Cannot write file") );

			DTLiberator( pBuffer );
		}
		else {
			_RPT0( _CRT_ASSERT, _T("Memory allocation error") );
			bOk = FALSE;
		}
	}

	AvpCloseHandle( hFile );

	return bOk;
}



// KLDeserializeRead.c - implementation of KLDataTreeSerialization
// Reading part of a code
//
#include <stdarg.h>
#include <string.h>

#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>

#define BUFFER_SIZE (0x4000)

extern void* (* DTAllocator)(AVP_uint);
extern void  (* DTLiberator)(void*);

AVP_bool LoadFromFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *dwcbRead, void* pUserData ); 


#ifdef KLDT_REALISE_UNICODE
// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeReadW( const wchar_t *pszFileName, 
																										 HDATA *hRootData,
																										 SWM_Info *pSWMInfo, 
																										 SWM_StreamInfo *pSWMStreamInfo,
																										 AVP_bool bReadRawDataHeader) {
	DWORD		dwcbRead;
	BOOL		bOk;
	HANDLE	hFile;

	if ( !DTAllocator || !DTLiberator ) {
    _RPT0( _CRT_ASSERT, _T("Library was not initiated") );
    return 0;
	}

  hFile = AvpCreateFile( (const TCHAR *)pszFileName, GENERIC_READ, 
												 FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );

	if ( hFile == INVALID_HANDLE_VALUE ) {
    //_RPT0( _CRT_ASSERT, _T("Cannot open file") );
    return FALSE;
	}

	bOk = KLDT_DeserializeHeadersW( hFile, bReadRawDataHeader );

	if ( bOk ) {
		HDATA hResultData;
		AVP_Serialize_Data rcSData;
		char *pBuffer;

		pSWMStreamInfo->pUserData  = hFile;
		
		memset( &rcSData, 0, sizeof(rcSData) );
		
		dwcbRead = AvpGetFileSize( hFile, NULL );
		dwcbRead = min(dwcbRead, BUFFER_SIZE);

		pBuffer = DTAllocator( dwcbRead );

		if ( pBuffer ) {
			rcSData.user_data = (void *)pSWMStreamInfo;
			rcSData.proc = (Buffer_Proc)pSWMInfo->m_pProc;

			hResultData = DATA_Deserialize( &rcSData, pBuffer, dwcbRead, (AVP_dword *)&dwcbRead );

			if ( !hResultData ) {
				_RPT0( _CRT_ASSERT, _T("Cannot deserialize data tree") );
				bOk = FALSE;
			}
			else {
				if ( *hRootData )
					DATA_Attach( *hRootData, NULL, hResultData, DATA_IF_ROOT_INCLUDE );
				else
					*hRootData = hResultData;
			}
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
#endif



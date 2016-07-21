// KLDeserializeHeaders.c - implementation of KLDataTreeSerialization
// Reading part of a code - deserialization standard headers
//
#include <string.h>
#ifdef  __MWERKS__
	#include "myassrt.h"
#endif

#if !defined (__unix__) && !defined (__MWERKS__)
	#include <windows.h>
#endif

#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>

#if defined (__unix__)
	#include <byteorder.h>
#endif


extern void* (* DTAllocator)(AVP_uint);
extern void  (* DTLiberator)(void*);

#ifdef KLDT_REALISE_UNICODE
// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeadersW( HANDLE hFile,
																											  AVP_bool bReadRawDataHeader ) {
	DWORD dwcbRead;
	AVP_KL_Header rcKLHeader;
	BOOL bOk = TRUE;
	
	bOk = KLDT_DeserializeFullHeadersW( hFile, &rcKLHeader, NULL );
	
	if ( bOk && bReadRawDataHeader ) {
		AVP_SW_Header rcSWHeader;
		if ( !AvpReadFile(hFile, &rcSWHeader, sizeof(rcSWHeader), &dwcbRead, 0) ||
			dwcbRead != sizeof(rcSWHeader) ) {
			_RPT0( _CRT_ASSERT, _T("Cannot read file") );
			bOk = FALSE;
		}
	}
	
	return bOk;
}


// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeFullHeadersW( HANDLE hFile,
																													  AVP_KL_Header *pKLHeader,
																													  wchar_t **ppDescription ) {
	DWORD dwcbRead;
	BOOL bOk = TRUE;
	AVP_KL_Header rcKLHeader;
	
	if ( !pKLHeader )
		pKLHeader = &rcKLHeader;
	
	if ( bOk ) {
		if ( !AvpReadFile(hFile, &pKLHeader->magic, sizeof(pKLHeader->magic), &dwcbRead, 0) ||
			dwcbRead != sizeof(pKLHeader->magic) ) {
			_RPT0( _CRT_ASSERT, _T("Cannot read file") );
			bOk = FALSE;
		}
	}
	
#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->magic = ReadDWordPtr ((DWORD*) (&(pKLHeader->magic)));
#endif
	pKLHeader->version = 1;
	
	if ( bOk ) {
		if ( pKLHeader->magic != AVP_SW_MAGIC ) {
			_RPT0( _CRT_ASSERT, _T("Wrong file type") );
			bOk = FALSE;
		}
	}
	
	if ( bOk ) {
		if ( !AvpReadFile(hFile, &pKLHeader->descriptionLength, sizeof(pKLHeader->descriptionLength), &dwcbRead, 0) ||
			dwcbRead != sizeof(pKLHeader->descriptionLength) ) {
			_RPT0( _CRT_ASSERT, _T("Cannot read file") );
			bOk = FALSE;
		}
	}
	
#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->descriptionLength = ReadWordPtr (&(pKLHeader->descriptionLength));
#endif
	
	if ( bOk ) {
		char *pBuffer = DTAllocator( pKLHeader->descriptionLength + 1 );
		memset( pBuffer, 0,  pKLHeader->descriptionLength + 1 );
		if ( pBuffer ) {
			if ( !AvpReadFile(hFile, pBuffer, pKLHeader->descriptionLength, &dwcbRead, 0) ||
				dwcbRead != pKLHeader->descriptionLength ) {
				_RPT0( _CRT_ASSERT, _T("Cannot read file") );
				bOk = FALSE;
			}
			if ( ppDescription ) {
#if defined(_UNICODE)
				*ppDescription = (TCHAR *)pBuffer;
#else
				DWORD dwSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pBuffer, -1, 0, 0 );
				*ppDescription = DTAllocator( dwSize * sizeof(wchar_t) );
				MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pBuffer, -1, *ppDescription, dwSize );
#endif
			}
			else
				DTLiberator( pBuffer );
		}
		else {
			_RPT0( _CRT_ASSERT, _T("Memory allocation error") );
			bOk = FALSE;
		}
	}
	
	return bOk;
}
#endif


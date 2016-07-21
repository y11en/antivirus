// KLDeserializeHeaders.c - implementation of KLDataTreeSerialization
// Reading part of a code - deserialization standard headers
//
#include <string.h>

#if !defined (__unix__) && !defined (__MWERKS__)
	#include <windows.h>
#endif

#ifdef __MWERKS__
	#include "myassrt.h"
#endif	

#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>

#if defined (MACHINE_IS_BIG_ENDIAN)
	#include <byteorder.h>
#endif


extern void* (* DTAllocator)(AVP_uint);
extern void  (* DTLiberator)(void*);

// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeaders( HANDLE hFile,
																											 AVP_bool bReadRawDataHeader ) {
	DWORD dwcbRead;
	AVP_KL_Header rcKLHeader;
	BOOL bOk = TRUE;

	bOk = KLDT_DeserializeFullHeaders( hFile, &rcKLHeader, NULL );

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
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeFullHeaders( HANDLE hFile,
																													 AVP_KL_Header *pKLHeader,
																													 TCHAR **ppDescription ) {
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
		if ( pBuffer ) {
			memset( pBuffer, 0,  pKLHeader->descriptionLength + 1 );
			if ( pKLHeader->descriptionLength ) {
				if ( !AvpReadFile(hFile, pBuffer, pKLHeader->descriptionLength, &dwcbRead, 0) ||
						 dwcbRead != pKLHeader->descriptionLength ) {
					_RPT0( _CRT_ASSERT, _T("Cannot read file") );
					bOk = FALSE;
				}
			}
			if ( ppDescription )
				*ppDescription = (TCHAR *)pBuffer;
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




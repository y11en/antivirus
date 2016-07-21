// KLDeserializeHeadersMem.c - implementation of KLDataTreeSerialization
// Reading MEM part of a code
//
#include <string.h>

#include <Serialize/KLDTSer.h>

#if defined (MACHINE_IS_BIG_ENDIAN)
	#include <byteorder.h>
#endif /* defined (__unix__) */
	

extern void* (* DTAllocator)(AVP_uint);
extern void  (* DTLiberator)(void*);

#define VALID_BUFFER (pBuffer && nBufferSize)

// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeHeadersFromMemory( AVP_char *pBuffer, 
																																  AVP_dword nBufferSize,
																																  AVP_bool bReadRawDataHeader ) {

	AVP_KL_Header				rcKLHeader;
	AVP_dword						nRead = 0;
	AVP_dword						nWillBeRead;

	nRead = KLDT_DeserializeFullHeadersFromMemory( pBuffer, nBufferSize, &rcKLHeader, 0 );

	if ( bReadRawDataHeader ) {
		nWillBeRead = sizeof(AVP_SW_Header);
		if ( VALID_BUFFER && nRead + nWillBeRead <= nBufferSize ) {
			nRead += nWillBeRead;
		}
	}

	return nRead;
}


// Low level function deserialize standard headers from memory
// pBuffer         - pointer to deserialization buffer
// nBufferSize		 - amount of bytes in buffer
// pKLHeader       - pointer to KL header structure (might be 0)
// ppszDescription - pointer to pointer to buffer (allocated by function)	for description (might be 0)
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeFullHeadersFromMemory( AVP_char *pBuffer, 
																																			AVP_dword nBufferSize,
																																			AVP_KL_Header *pKLHeader,
																																			TCHAR **ppszDescription ) {
	AVP_dword						nRead = 0;
	AVP_dword						nWillBeRead;
	AVP_KL_Header				rcKLHeader;
	AVP_char					 *pPointer;

	pPointer = pBuffer;

	if ( !pKLHeader )
		pKLHeader = &rcKLHeader;

	nWillBeRead = sizeof( pKLHeader->magic );
	if ( VALID_BUFFER && nRead + nWillBeRead <= nBufferSize ) {
		nRead += nWillBeRead;
		memcpy( &pKLHeader->magic, pPointer, nWillBeRead );
		pPointer += nWillBeRead;
	}

#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->magic = ReadDWordPtr ((DWORD*) (&(pKLHeader->magic)));
#endif

	if ( pKLHeader->magic != AVP_SW_MAGIC ) {
		_RPT0( _CRT_ASSERT, _T("Wrong type") );
		return nRead;
	}

	pKLHeader->version = 1;

	nWillBeRead = sizeof( pKLHeader->descriptionLength );
	if ( VALID_BUFFER && nRead + nWillBeRead <= nBufferSize ) {
		nRead += nWillBeRead;
		memcpy( &pKLHeader->descriptionLength, pPointer, nWillBeRead );
		pPointer += nWillBeRead;
	}

#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->descriptionLength = ReadWordPtr (&(pKLHeader->descriptionLength));
#endif
	
	nWillBeRead = pKLHeader->descriptionLength;
	if ( VALID_BUFFER && nRead + nWillBeRead <= nBufferSize ) {
		if ( ppszDescription ) {
			*ppszDescription = DTAllocator( nWillBeRead + 1 );
			if ( *ppszDescription ) {
				memset( *ppszDescription, 0,  nWillBeRead + 1 );
				memcpy( *ppszDescription, pPointer, nWillBeRead );
			}
		}
		nRead += nWillBeRead;
		pPointer += nWillBeRead;
	}

	return nRead;
}

#undef VALID_BUFFER



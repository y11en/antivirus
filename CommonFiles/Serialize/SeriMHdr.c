// KLSerializeHeadersToMem.c - implementation of KLDataTreeSerialization
// KLSerializeMem.c - implementation of KLDataTreeSerialization
// Writing MEM part of a code
//
#include <string.h>

#include <Serialize/KLDTSer.h>
#include <byteorder.h>
#ifdef __MWERKS__
	#include <tchar.h>
#endif	


#define VALID_BUFFER (pBuffer && nBufferSize)

// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeHeadersToMemory( AVP_char *pBuffer, 
																														  AVP_dword nBufferSize,
																														  const TCHAR *pszDescription,
																															AVP_bool bWriteRawDataHeader ) {
	AVP_KL_Header rcKLHeader;
	AVP_dword     nWritten = 0;
	AVP_dword     nWillBeWritten;
	AVP_char     *pPointer = pBuffer;
	unsigned short wDescLen = (unsigned short)((pszDescription != 0) ? _tcsclen( pszDescription ) * sizeof(TCHAR): 0);

	WriteDWordPtr (&rcKLHeader.magic, AVP_SW_MAGIC);
	WriteWordPtr (&rcKLHeader.version, 1);
	WriteWordPtr (&rcKLHeader.descriptionLength, wDescLen);
  
	nWillBeWritten = sizeof( rcKLHeader.magic );
	if ( !VALID_BUFFER || nWritten + nWillBeWritten <= (AVP_dword)nBufferSize ) {
		nWritten += nWillBeWritten;
		if ( pBuffer ) {
			memcpy( pPointer, &rcKLHeader.magic, nWillBeWritten );
			pPointer += nWillBeWritten;
		}
	}

	nWillBeWritten = sizeof( rcKLHeader.descriptionLength );
	if ( !VALID_BUFFER || nWritten + nWillBeWritten <= (AVP_dword)nBufferSize ) {
		nWritten += nWillBeWritten;
		if ( pBuffer ) {
			memcpy( pPointer, &rcKLHeader.descriptionLength, nWillBeWritten );
			pPointer += nWillBeWritten;
		}
	}

	nWillBeWritten = rcKLHeader.descriptionLength;
	if ( nWillBeWritten && (!VALID_BUFFER || nWritten + nWillBeWritten <= (AVP_dword)nBufferSize) ) {
		nWritten += nWillBeWritten;
		if ( pBuffer ) {
			memcpy( pPointer, (void *)pszDescription, nWillBeWritten );
			pPointer += nWillBeWritten;
		}
	}


	if ( bWriteRawDataHeader ) {
		AVP_SW_Header rcSWHeader = { AVP_SW_MAGIC, AVP_SW_PID_RAWDATA };
		nWillBeWritten = sizeof(rcSWHeader);
		if ( !VALID_BUFFER || nWritten + nWillBeWritten <= (AVP_dword)nBufferSize ) {
			nWritten += nWillBeWritten;
			if ( pBuffer ) {
				memcpy( pPointer, (void *)&rcSWHeader, nWillBeWritten );
				pPointer += nWillBeWritten;
			}
		}
	}

	return nWritten;
}


#undef VALID_BUFFER



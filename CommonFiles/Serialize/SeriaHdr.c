// KLSerializeHeaders.c - implementation of KLDataTreeSerialization
// Writing part of a code - serialize standard headers
//
#include <string.h>

#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>
#include <byteorder.h>
#ifdef __MWERKS__
	#include <tchar.h>
#endif	

// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeHeaders( HANDLE hFile,
																									   const TCHAR *pszDescription,
																									   AVP_bool bWriteRawDataHeader ) {
	DWORD dwcbWritten;
	AVP_KL_Header rcKLHeader;
	AVP_bool bOk = TRUE;
	unsigned short wDescLen = (unsigned short)((pszDescription != 0) ? _tcsclen( pszDescription ) * sizeof(TCHAR): 0);

	WriteDWordPtr (&rcKLHeader.magic, AVP_SW_MAGIC);
	WriteWordPtr (&rcKLHeader.version, 1);
	WriteWordPtr (&rcKLHeader.descriptionLength, wDescLen);
	
	if ( !AvpWriteFile(hFile, &rcKLHeader.magic, sizeof(rcKLHeader.magic), &dwcbWritten, NULL) ||
			 dwcbWritten != sizeof(rcKLHeader.magic) ) {
    _RPT0( _CRT_ASSERT, _T("Cannot write file") );
		bOk = FALSE;
	}

	if ( bOk ) {
		if ( !AvpWriteFile(hFile, &rcKLHeader.descriptionLength, sizeof(rcKLHeader.descriptionLength), &dwcbWritten, NULL) ||
				 dwcbWritten != sizeof(rcKLHeader.descriptionLength) ) {
			_RPT0( _CRT_ASSERT, _T("Cannot write file") );
			bOk = FALSE;
		}
	}

	if ( bOk && wDescLen ) {
		if ( !AvpWriteFile(hFile, (void *)pszDescription, wDescLen, &dwcbWritten, NULL) ||
				 dwcbWritten != wDescLen ) {
				_RPT0( _CRT_ASSERT, _T("Cannot write file") );
				bOk = FALSE;
		}
	}

	if ( bOk && bWriteRawDataHeader ) {
		AVP_SW_Header rcSWHeader;
		
		WriteDWordPtr (&rcSWHeader.magic, AVP_SW_MAGIC);
		WriteWordPtr (&rcSWHeader.id, AVP_SW_PID_RAWDATA);

		if ( !AvpWriteFile(hFile, &rcSWHeader, sizeof(rcSWHeader), &dwcbWritten, NULL) || 
				 dwcbWritten != sizeof(rcSWHeader) ) {
				_RPT0( _CRT_ASSERT, _T("Cannot write file") );
				bOk = FALSE;
		}
	}

	return bOk;
}


// KLDeserialize.c - implementation of KLDataTreeSerialization
// Reading part of a code
//

#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>

extern void* (* DTAllocator)(AVP_uint);
extern void  (* DTLiberator)(void*);

AVP_bool LoadFromFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *dwcbRead, void* pUserData );

#ifdef KLDT_REALISE_UNICODE
// Deserialize data tree
// pszFileName    - pointer to in file name
// hRootData      - pointer to handle of tree root
//                  0  - handle of deserialized tree will be puted into it
//                  !0 - DATA_Attach to this handle will be completed 
// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeW( const wchar_t *pszFileName, HDATA *hRootData ) {

  SWM_Info rcOutInfo;
  SWM_StreamInfo rcStreamInfo = {0, 0};

  rcOutInfo.m_pProc = (BufferProc)LoadFromFileCallBack;

  return KLDT_DeserializeReadW( pszFileName, hRootData, &rcOutInfo, &rcStreamInfo, TRUE );
}


// Deserialize data header
// pszFileName    - pointer to in file name
// pKLHeader      - pointer to KL header structure (might be 0)
// ppszDescription - pointer to pointer to buffer (allocated by function)	for description (might be 0)
//                   Should be freed by "free" function
// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeaderW( const wchar_t *pszFileName, 
																											 AVP_KL_Header *pKLHeader,
																											 wchar_t **ppszDescription ) {
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

	bOk = KLDT_DeserializeFullHeadersW( hFile, pKLHeader, ppszDescription );

	AvpCloseHandle( hFile );

	return bOk;
}
#endif


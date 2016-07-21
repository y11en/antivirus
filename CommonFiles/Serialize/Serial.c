// KLSerialize.c - implementation of KLDataTreeSerialization
// Writing part of a code
//
#include <Serialize/KLDTSer.h>

AVP_bool SaveToFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData );

// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_Serialize( const TCHAR *pszFileName, 
                                              HDATA hRootData, 
                                              const TCHAR *pszDescription ) {

  SWM_Info rcOutInfo;
  SWM_StreamInfo rcStreamInfo = {0, 0};

  rcOutInfo.m_pProc = (BufferProc)SaveToFileCallBack;

  return KLDT_SerializeWrite( pszFileName, hRootData, pszDescription,&rcOutInfo, &rcStreamInfo,
															TRUE );
}



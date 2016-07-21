// KLSerialize.c - implementation of KLDataTreeSerialization
// Writing part of a code
//
#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>

AVP_bool SaveToFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData );

#ifdef KLDT_REALISE_UNICODE
// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeW( const wchar_t *pszFileName, 
                                               HDATA hRootData, 
                                               const wchar_t *pszDescription ) {

  SWM_Info rcOutInfo;
  SWM_StreamInfo rcStreamInfo = {0, 0};

  rcOutInfo.m_pProc = (BufferProc)SaveToFileCallBack;

  return KLDT_SerializeWriteW( pszFileName, hRootData, pszDescription,&rcOutInfo, &rcStreamInfo,
															TRUE );
}
#endif


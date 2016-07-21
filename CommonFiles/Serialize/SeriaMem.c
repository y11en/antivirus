// KLSerializeMem.c - implementation of KLDataTreeSerialization
// Writing MEM part of a code
//
#include <string.h>

#include <Serialize/KLDTSer.h>


AVP_bool SaveToMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData );


// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemory( AVP_char *pBuffer, 
																											 AVP_dword nBufferSize,
																											 HDATA hRootData, 
																											 const TCHAR *pszDescription ) {

  SWM_Info rcOutInfo;
  SWM_StreamInfo rcStreamInfo = {0, 0};

  rcOutInfo.m_pProc = (BufferProc)SaveToMemoryCallBack;

  return KLDT_SerializeToMemoryWrite( pBuffer, nBufferSize, hRootData, pszDescription,&rcOutInfo, &rcStreamInfo,
																			TRUE );
}





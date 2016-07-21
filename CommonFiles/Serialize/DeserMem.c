// KLDeserializeMem.c - implementation of KLDataTreeSerialization
// Reading MEM part of a code
//
#include <string.h>
#include <Serialize/KLDTSer.h>


AVP_bool LoadFromMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *dwcbRead, void* pUserData );

// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeFromMemory( AVP_char *pBuffer, 
																													 AVP_dword nBufferSize,
																													 HDATA *hRootData ) {

  SWM_Info rcOutInfo;
  SWM_StreamInfo rcStreamInfo = {0, 0};

  rcOutInfo.m_pProc = (BufferProc)LoadFromMemoryCallBack;

  return KLDT_DeserializeFromMemoryRead( pBuffer, nBufferSize, hRootData, &rcOutInfo, &rcStreamInfo, TRUE );
}





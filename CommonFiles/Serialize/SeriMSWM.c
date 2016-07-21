// KLSerializeToMemorySWM.c - implementation of KLDataTreeSerialization
// Writing part of a code	- serilization using SWM
//
#include <Serialize/KLDTSer.h>

extern void* (* DTAllocator)(AVP_size_t);
extern void  (* DTLiberator)(void*);

extern void  (* DTLocker)(void*);
extern void  (* DTUnlocker)(void*);
extern void* DTContext; 

AVP_bool SaveToMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData );

// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemoryUsingSWM( AVP_char *pBuffer, 
																															 AVP_dword nBufferSize,
																															 HDATA hRootData, 
																															 const TCHAR *pszDescription ) {
	
	AVP_dword nResult = 0;

	SWM_StreamInfo rcStreamInfo;

	SWM_Info rcInInfo = { SaveToMemoryCallBack };
	SWM_Info rcOutInfo = { 0 };
	void *pSWHandle;
	void *pSWManager;
		
	SWM_Init_LibraryEx( DTAllocator, DTLiberator, DTLocker, DTUnlocker, DTContext ); 
		
	pSWManager = SWM_Create_Manager();

	SWM_Register_Encrypter( pSWManager );
	SWM_Register_CRCOutChecker( pSWManager );
	SWM_Register_Packer( pSWManager );

	pSWHandle = SWM_Prepare_To_Write( pSWManager, &rcInInfo, &rcOutInfo );

	if ( pSWHandle && rcOutInfo.m_pProc ) {
		rcStreamInfo.pSWMHandle = pSWHandle;
		rcStreamInfo.pUserData  = 0;


		nResult = KLDT_SerializeToMemoryWrite( pBuffer, nBufferSize, hRootData, pszDescription,&rcOutInfo, &rcStreamInfo,
																					 FALSE );
	}

	SWM_Shut_Down_Library( pSWHandle );
	SWM_Shut_Down_Library( pSWManager );

	return nResult;
}



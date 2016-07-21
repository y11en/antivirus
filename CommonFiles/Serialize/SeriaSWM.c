// KLSerializeSWM.c - implementation of KLDataTreeSerialization
// Writing part of a code	- serilization using SWM
//
#include <Serialize/KLDTSer.h>

extern void* (* DTAllocator)(AVP_size_t);
extern void  (* DTLiberator)(void*);

extern void  (* DTLocker)(void*);
extern void  (* DTUnlocker)(void*);
extern void* DTContext; 

AVP_bool SaveToFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData );

// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeUsingSWM( const TCHAR *pszFileName, 
																											HDATA hRootData, 
																											const TCHAR *pszDescription ) {
	
	AVP_bool bOk = 0;

	SWM_StreamInfo rcStreamInfo;

	SWM_Info rcInInfo = { SaveToFileCallBack };
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

		bOk = KLDT_SerializeWrite( pszFileName, hRootData, pszDescription, &rcOutInfo, &rcStreamInfo,
																FALSE );
	}

	SWM_Shut_Down_Library( pSWHandle );
	SWM_Shut_Down_Library( pSWManager );

	return bOk;
}



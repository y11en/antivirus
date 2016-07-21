// KLDeserializeSWM.c - implementation of KLDataTreeSerialization
// Reading part of a code	- deserialize using SWM
//
#include <Serialize/KLDTSer.h>

extern void* (* DTAllocator)(AVP_size_t);
extern void  (* DTLiberator)(void*);

extern void  (* DTLocker)(void*);
extern void  (* DTUnlocker)(void*);
extern void* DTContext; 

AVP_bool LoadFromFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *dwcbRead, void* pUserData );


// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeUsingSWM( const TCHAR *pszFileName, 
																											  HDATA *hRootData ) {
	AVP_bool bOk = 0;

	SWM_StreamInfo rcStreamInfo;

	SWM_Info rcInInfo = { LoadFromFileCallBack };
	SWM_Info rcOutInfo = { 0 };
	void *pSWHandle;
	void *pSWManager;
	
	SWM_Init_LibraryEx( DTAllocator, DTLiberator, DTLocker, DTUnlocker, DTContext ); 

	pSWManager = SWM_Create_Manager();
	
	SWM_Register_CRCInChecker( pSWManager );
	SWM_Register_Decrypter( pSWManager );
	SWM_Register_Unpacker( pSWManager );

	pSWHandle = SWM_Prepare_To_Read( pSWManager, &rcInInfo, &rcOutInfo );

	if ( pSWHandle && rcOutInfo.m_pProc )	{
		rcStreamInfo.pSWMHandle = pSWHandle;
		rcStreamInfo.pUserData  = 0;


		bOk = KLDT_DeserializeRead( pszFileName, hRootData, &rcOutInfo, &rcStreamInfo, FALSE );
	}

	SWM_Shut_Down_Library( pSWHandle );
	SWM_Shut_Down_Library( pSWManager );

	return bOk;
}




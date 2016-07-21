// KLSerializeToMemoryCustomizeSWM.c - implementation of KLDataTreeSerialization
// Writing part of a code	- serilization using SWM
//
#include <Serialize/KLDTSer.h>

#ifdef __MWERKS__
	#include <tchar.h>
#endif	

extern void* (* DTAllocator)(AVP_size_t);
extern void  (* DTLiberator)(void*);

extern void  (* DTLocker)(void*);
extern void  (* DTUnlocker)(void*);
extern void* DTContext; 

AVP_bool SaveToMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData );

// ---
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemoryUsingCustomizedSWM( AVP_char *pBuffer, 
																																				AVP_dword nBufferSize,
																																				HDATA hRootData, 
																																				const TCHAR *pszDescription, 
																																				const TCHAR *pszCodeStr ) {

	AVP_dword nResult = 0;
	if ( pszCodeStr && *pszCodeStr ) {

		SWM_StreamInfo rcStreamInfo;

		SWM_Info rcInInfo = { SaveToMemoryCallBack };
		SWM_Info rcOutInfo = { 0 };
		void *pSWHandle;
		void *pSWManager;
		
		SWM_Init_LibraryEx( DTAllocator, DTLiberator, DTLocker, DTUnlocker, DTContext ); 

		pSWManager = SWM_Create_Manager();

		while( _tcsnextc(pszCodeStr) != L'\0' ) {
			switch ( _tcsnextc(pszCodeStr) ) { 
				case L'e' :
					SWM_Register_Encrypter( pSWManager );
					break;
				case L'c' :
					SWM_Register_CRCOutChecker( pSWManager );
					break;
				case L'p' :
					SWM_Register_Packer( pSWManager );
					break;
			}
			pszCodeStr = _tcsinc(pszCodeStr); 
		}

		pSWHandle = SWM_Prepare_To_Write( pSWManager, &rcInInfo, &rcOutInfo );

		if ( pSWHandle && rcOutInfo.m_pProc ) {
			rcStreamInfo.pSWMHandle = pSWHandle;
			rcStreamInfo.pUserData  = 0;


			nResult = KLDT_SerializeToMemoryWrite( pBuffer, nBufferSize, hRootData, pszDescription, &rcOutInfo, &rcStreamInfo,
																				 FALSE );
		}

		SWM_Shut_Down_Library( pSWHandle );
		SWM_Shut_Down_Library( pSWManager );

	}
	else
		nResult = KLDT_SerializeToMemory( pBuffer, nBufferSize, hRootData, pszDescription );

	return nResult;
}



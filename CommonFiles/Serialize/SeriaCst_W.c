// KLSerializeCustomizeSWM.c - implementation of KLDataTreeSerialization
// Writing part of a code	- serilization using SWM
//

#if !defined(__MWERKS__) && !defined(__unix__)
	#include <windows.h>
#endif	
#include <Serialize/KLDTSer.h>
#ifdef __MWERKS__
	#include <tchar.h>
#endif	


extern void* (* DTAllocator)(AVP_size_t);
extern void  (* DTLiberator)(void*);

extern void  (* DTLocker)(void*);
extern void  (* DTUnlocker)(void*);
extern void* DTContext; 

AVP_bool SaveToFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData );

#ifdef KLDT_REALISE_UNICODE
// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeUsingCustomizedSWMW( const wchar_t *pszFileName, 
																															   HDATA hRootData, 
																															   const wchar_t *pszDescription, 
																															   const wchar_t *pszCodeStr ) {

	AVP_bool bOk = 0;
	if ( pszCodeStr && *pszCodeStr ) {

		SWM_StreamInfo rcStreamInfo;

		SWM_Info rcInInfo = { SaveToFileCallBack };
		SWM_Info rcOutInfo = { 0 };
		void *pSWHandle;
		void *pSWManager;
		
		SWM_Init_LibraryEx( DTAllocator, DTLiberator, DTLocker, DTUnlocker, DTContext ); 

		pSWManager = SWM_Create_Manager();

#if defined(_UNICODE)
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
#else
		{
		DWORD dwSize = WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pszCodeStr, -1, NULL, 0, NULL, NULL );
		char *pszCodeStrA = DTAllocator( dwSize * sizeof(char) );
		char *pDestStr = pszCodeStrA;
		WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pszDescription, -1, pszCodeStrA, dwSize, NULL, NULL );
		while( _tcsnextc(pDestStr) != L'\0' ) {
			switch ( _tcsnextc(pDestStr) ) { 
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
			pDestStr = _tcsinc(pDestStr); 
		}
		DTLiberator( pszCodeStrA );
		}
#endif

		pSWHandle = SWM_Prepare_To_Write( pSWManager, &rcInInfo, &rcOutInfo );

		if ( pSWHandle && rcOutInfo.m_pProc ) {
			rcStreamInfo.pSWMHandle = pSWHandle;
			rcStreamInfo.pUserData  = 0;


			bOk = KLDT_SerializeWriteW( pszFileName, hRootData, pszDescription,&rcOutInfo, &rcStreamInfo,
																	FALSE );
		}

		SWM_Shut_Down_Library( pSWHandle );
		SWM_Shut_Down_Library( pSWManager );
	}
	else
		bOk = KLDT_SerializeW( pszFileName, hRootData, pszDescription );

	return bOk;
}
#endif


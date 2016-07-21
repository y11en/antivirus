/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_deser.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/10/2005 2:55:02 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmdefs.h"
#include "dskmi.h"

#include "../SWManager/Swm.h"
#include "../_AVPIO.h"

#define BUFFER_SIZE (0x4000)



//! \fn				: DSKM_ReadRegHeaderFromFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HANDLE hFile
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_REG_Header *pKLHeader
AVP_dword DSKMAPI DSKM_ReadRegHeaderFromFile( HDSKM hDSKM, HANDLE hFile, AVP_dword dwObjectsType, AVP_REG_Header *pKLHeader ) {
	DWORD dwcbRead;
	AVP_REG_Header rcKLHeader;

	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( !pIo ) 
		return DSKM_ERR_IO_NOT_INITIALIZED;

	if ( !pKLHeader )
		pKLHeader = &rcKLHeader;
	
	DSKMSetFilePointer( pIo, hFile, 0, 0, FILE_BEGIN );

	if ( !DSKMReadFile(pIo, hFile, &pKLHeader->uiMagic, sizeof(pKLHeader->uiMagic), &dwcbRead, 0) ||
				dwcbRead != sizeof(pKLHeader->uiMagic) ) {
		return DSKM_ERR_CANNOT_READ_REG_FILE;
	}
	
#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->uiMagic = ReadDWordPtr ((DWORD*) (&(pKLHeader->uiMagic)));
#endif
	
	if ( pKLHeader->uiMagic != (unsigned int)AVP_REG_MAGIC ) {
		return DSKM_ERR_INVALID_REG_FILE;
	}
	
	if ( !DSKMReadFile(pIo, hFile, &pKLHeader->uiObjType, sizeof(pKLHeader->uiObjType), &dwcbRead, 0) ||
				dwcbRead != sizeof(pKLHeader->uiObjType) ) {
		return DSKM_ERR_CANNOT_READ_REG_FILE;
	}
	
#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->uiObjType = ReadDWordPtr (&(pKLHeader->uiObjType));
#endif
	
//	if ( pKLHeader->uiVersion != (unsigned short)DSKM_REG_VERSION ) {
	if ( RHD_VERSION(pKLHeader->uiObjType) != (unsigned short)DSKM_REG_VERSION ) {
		return DSKM_ERR_INVALID_REG_FILE_VERSION;
	}
	
//	if ( dwObjectsType && pKLHeader->uiBaseType != (unsigned int)dwObjectsType ) {
	if ( dwObjectsType && RHD_BASETYPE(pKLHeader->uiObjType) != (unsigned int)dwObjectsType ) {
		return DSKM_ERR_INVALID_REG_FILE_OBJTYPE;
	}
	
	return DSKM_ERR_OK;
}



//! \fn				: LoadFromFileCallBack
//! \brief			:	
//! \return			: static AVP_bool 
//! \param          :  void* pBuffer
//! \param          : AVP_dword nSize
//! \param          : AVP_dword *dwcbRead
//! \param          : void* pUserData
static AVP_bool DSKM_LoadFromFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *dwcbRead, void* pUserData ) {
	AVP_bool  bOk = 1;
	
	if ( pBuffer )
		bOk = DSKMReadFile( ((DSKMCBP *)((SWM_StreamInfo *)pUserData)->pUserData)->m_pIo, ((DSKMCBP *)((SWM_StreamInfo *)pUserData)->pUserData)->m_hFile, pBuffer, nSize, (DWORD *)dwcbRead, NULL ) /*&& dwcbRead == nSize*/;
	else {
		if ( dwcbRead )
			*dwcbRead = 0;
	}
	return bOk;
}



//! \fn				: DSKMAPI DSKM_DeserializeRegHandle
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : HANDLE hFile
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA *phRootData
static AVP_dword DSKMAPI DSKM_DeserializeRegHandle( HDSKM hDSKM, HANDLE hFile, AVP_dword dwObjectsType, HDATA *phRootData ) {
	DSKM_TRY {
		DSKM_Io *pIo = DSKM_GetIO( hDSKM );
		if ( pIo ) {
			DSKMSetFilePointer( pIo, hFile, 0, 0, FILE_BEGIN );
			{
				DWORD		dwcbRead;
				AVP_REG_Header rcKLHeader;

				AVP_dword dwResult = DSKM_ReadRegHeaderFromFile( hDSKM, hFile, dwObjectsType, &rcKLHeader );
				if ( DSKM_OK(dwResult) && phRootData ) {
					AVP_bool bOk = 1;

					SWM_StreamInfo rcStreamInfo;
					
					SWM_Info rcInInfo = { DSKM_LoadFromFileCallBack };
					SWM_Info rcOutInfo = { 0 };
					void *pSWHandle;
					void *pSWManager;
					
					SWM_Init_Library( DSKMAllocator, DSKMLiberator ); 
					
					pSWManager = SWM_Create_Manager();
					
					SWM_Register_CRCInChecker( pSWManager );
					SWM_Register_Decrypter( pSWManager );
					SWM_Register_Unpacker( pSWManager );
					
					pSWHandle = SWM_Prepare_To_Read( pSWManager, &rcInInfo, &rcOutInfo );
					
					if ( pSWHandle && rcOutInfo.m_pProc )	{

						HDATA hResultData;
						AVP_Serialize_Data rcSData;
						DSKMCBP rcCBP = {pIo, hFile};
						char *pBuffer;
						
						rcStreamInfo.pSWMHandle = pSWHandle;
						rcStreamInfo.pUserData  = &rcCBP;
						
						ds_mset( &rcSData, 0, sizeof(rcSData) );
						
						dwcbRead = DSKMGetFileSize( pIo, hFile, NULL );
						dwcbRead = min(dwcbRead, BUFFER_SIZE);
						
						pBuffer = DSKMAllocator( dwcbRead );
						
						if ( pBuffer ) {
							rcSData.user_data = (void *)&rcStreamInfo;
							rcSData.proc = (Buffer_Proc)rcOutInfo.m_pProc;
							
							hResultData = DATA_Deserialize( &rcSData, pBuffer, dwcbRead, (AVP_dword *)&dwcbRead );
							
							if ( !hResultData ) {
								bOk = 0;
							}
							else {
//								DSKM_SetIsSpecialProp( hResultData, rcKLHeader.uiIsSpecial );
//								DSKM_SetObjTypeProp( hResultData, (AVP_dword)rcKLHeader.uiBaseType );
								DSKM_SetIsSpecialProp( hResultData, RHD_ISSPECIAL(rcKLHeader.uiObjType) );
								DSKM_SetObjTypeProp( hResultData, (AVP_dword)RHD_BASETYPE(rcKLHeader.uiObjType) );
								if ( *phRootData )
									DATA_Attach( *phRootData, 0, hResultData, DATA_IF_ROOT_INCLUDE );
								else
									*phRootData = hResultData;
							}

							DSKMLiberator( pBuffer );
						}
						else {
							bOk = 0;
						}
					}
					
					SWM_Shut_Down_Library( pSWHandle );
					SWM_Shut_Down_Library( pSWManager );
					
					if ( !bOk )
						dwResult = DSKM_ERR_CANNOT_READ_REG_FILE;

				}
				return dwResult;
			}
		}
		return DSKM_ERR_IO_NOT_INITIALIZED;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_DeserializeRegFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pRegFileName
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA *phRootData
AVP_dword DSKMAPI DSKM_DeserializeRegFile( HDSKM hDSKM, const AVP_char* pRegFileName, AVP_dword dwObjectsType, HDATA *phRootData ) {
	DSKM_TRY {
		DSKM_Io *pIo = DSKM_GetIO( hDSKM );
		if ( pIo ) {
			AVP_dword dwResult = DSKM_ERR_CANNOT_OPEN_REG_FILE;
			HANDLE	hFile = DSKMCreateFile( pIo, pRegFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
			if ( hFile != INVALID_HANDLE_VALUE ) {
				dwResult = DSKM_DeserializeRegHandle( hDSKM, hFile, dwObjectsType, phRootData );
				DSKMCloseHandle( pIo, hFile );
			}
			return dwResult;
		}
		return DSKM_ERR_IO_NOT_INITIALIZED;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_DeserializeRegHFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMFILE hDSFile
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA *hRootData
AVP_dword DSKMAPI DSKM_DeserializeRegHFile( HDSKM hDSKM, HDSKMFILE hDSFile, AVP_dword dwObjectsType, HDATA *hRootData ) {
	DSKM_TRY {
		if ( hDSFile->m_hFile == INVALID_HANDLE_VALUE ) {
			DSKM_OpenHFileHandle( hDSKM, hDSFile );
		}
		if ( hDSFile->m_hFile != INVALID_HANDLE_VALUE ) 
			return DSKM_DeserializeRegHandle( hDSKM, hDSFile->m_hFile, dwObjectsType, hRootData );
		return DSKM_ERR_CANNOT_OPEN_REG_FILE;
	}
	DSKM_EXCEPT
}


#define VALID_BUFFER (pBuffer && dwBufferSize)



//! \fn				: DSKM_ReadRegHeaderFromMemory
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  AVP_char *pBuffer
//! \param          : AVP_dword *pdwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack
//! \param          : void *pvCallBackParams
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_REG_Header *pKLHeader
//! \param          : AVP_dword *pdwRead
AVP_dword DSKMAPI DSKM_ReadRegHeaderFromMemory( AVP_char *pBuffer, AVP_dword *pdwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, AVP_dword dwObjectsType, AVP_REG_Header *pKLHeader, AVP_dword *pdwRead ) {

	AVP_dword						nRead = 0;
	AVP_dword						nWillBeRead;
	AVP_dword						dwBufferSize = *pdwBufferSize;
	AVP_char					 *pPointer;
	
	pPointer = pBuffer;

	if ( !VALID_BUFFER )
		return DSKM_ERR_INVALID_BUFFER;
		
	if ( pfnCallBack ) {
		if ( pfnCallBack(pvCallBackParams, 0, 0, DSKM_CB_OPEN) < 0 ) 
			return DSKM_ERR_CANNOT_READ_REG_FILE;
		dwBufferSize = *pdwBufferSize = pfnCallBack( pvCallBackParams, pBuffer, *pdwBufferSize, DSKM_CB_READ );
		if ( (int)dwBufferSize < 0 || dwBufferSize < sizeof(*pKLHeader) )
			return DSKM_ERR_CANNOT_READ_REG_FILE;
	}

	nWillBeRead = sizeof( pKLHeader->uiMagic );
	if ( nRead + nWillBeRead <= dwBufferSize ) {
		nRead += nWillBeRead;
		ds_mcpy( &pKLHeader->uiMagic, pPointer, nWillBeRead );
		pPointer += nWillBeRead;
	}
	
#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->uiMagic = ReadDWordPtr ((DWORD*) (&(pKLHeader->uiMagic)));
#endif
	
	if ( pKLHeader->uiMagic != (unsigned int)AVP_REG_MAGIC ) {
		return DSKM_ERR_INVALID_REG_FILE;
	}
	
	nWillBeRead = sizeof( pKLHeader->uiObjType );
	if ( nRead + nWillBeRead <= dwBufferSize ) {
		nRead += nWillBeRead;
		ds_mcpy( &pKLHeader->uiObjType, pPointer, nWillBeRead );
		pPointer += nWillBeRead;
	}
	
#if defined (MACHINE_IS_BIG_ENDIAN)
	pKLHeader->uiObjType = ReadDWordPtr (&(pKLHeader->uiObjType));
#endif
	
//	if ( pKLHeader->uiVersion != (unsigned short)DSKM_REG_VERSION ) {
	if ( RHD_VERSION(pKLHeader->uiObjType) != (unsigned short)DSKM_REG_VERSION ) {
		return DSKM_ERR_INVALID_REG_FILE_VERSION;
	}
	
//	if ( dwObjectsType && pKLHeader->uiBaseType != (unsigned int)dwObjectsType ) {
	if ( dwObjectsType && RHD_BASETYPE(pKLHeader->uiObjType) != (unsigned int)dwObjectsType ) {
		return DSKM_ERR_INVALID_REG_FILE_OBJTYPE;
	}
	
	if ( pdwRead )
		*pdwRead = nRead;

	return DSKM_ERR_OK;
}

// ---
typedef struct {
	AVP_char									 *m_pBuffer;
	AVP_dword										m_nBufferSize;
	pfnDSKM_GetBuffer_CallBack	m_pfnCallBack;
	void											 *m_pvCallBackParams;
	AVP_dword										m_nRead;
} LoadFromMemInfo;



//! \fn				: DSKM_LoadFromMemoryCallBack
//! \brief			:	
//! \return			: AVP_bool 
//! \param          :  void* pBuffer
//! \param          : AVP_dword nSize
//! \param          : AVP_dword *pdwcbRead
//! \param          : void* pUserData
AVP_bool DSKM_LoadFromMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbRead, void* pUserData ) {
	if ( pdwcbRead )
		*pdwcbRead = 0;

	if ( pBuffer ) {
		LoadFromMemInfo *pInfo = (LoadFromMemInfo *)((SWM_StreamInfo *)pUserData)->pUserData;
		if ( (pInfo->m_nRead + nSize > pInfo->m_nBufferSize) && pInfo->m_pfnCallBack ) {
			int nRead;
			AVP_dword dwSize = min( nSize, pInfo->m_nBufferSize - pInfo->m_nRead );
			if ( pInfo->m_pBuffer ) {
				ds_mcpy( pBuffer, pInfo->m_pBuffer + pInfo->m_nRead, dwSize );
				pBuffer = (AVP_byte *)pBuffer + dwSize;
			}
			
			pInfo->m_nRead = 0;

			nRead = pInfo->m_pfnCallBack( pInfo->m_pvCallBackParams, pInfo->m_pBuffer, pInfo->m_nBufferSize, DSKM_CB_READ );

			if ( nRead < 0 ) {
				pInfo->m_nBufferSize = 0;
				return 0;
			}
			
			pInfo->m_nBufferSize = nRead;

			if ( pdwcbRead )
				*pdwcbRead += dwSize;

			nSize -= dwSize;
		}

		nSize = min( nSize, pInfo->m_nBufferSize - pInfo->m_nRead );

		if ( pInfo->m_pBuffer )
			ds_mcpy( pBuffer, pInfo->m_pBuffer + pInfo->m_nRead, nSize );

		pInfo->m_nRead += nSize;

		if ( pdwcbRead )
			*pdwcbRead += nSize;
	}

	return 1;
}




//! \fn				: DSKM_DeserializeRegBuffer
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack
//! \param          : void *pvCallBackParams
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA *phRootData
AVP_dword DSKMAPI DSKM_DeserializeRegBuffer( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, AVP_dword dwObjectsType, HDATA *phRootData ) {
	DSKM_TRY {
		AVP_dword	nRead = 0;
		AVP_dword dwResult = DSKM_ERR_INVALID_BUFFER;
		AVP_REG_Header	rcKLHeader;

		if ( !VALID_BUFFER )
			return DSKM_ERR_INVALID_BUFFER;
		
		dwResult = DSKM_ReadRegHeaderFromMemory( pBuffer, &dwBufferSize, pfnCallBack, pvCallBackParams, dwObjectsType, &rcKLHeader, &nRead );
		
		if ( DSKM_OK(dwResult) && nRead && phRootData ) {
			SWM_Info rcInInfo = { DSKM_LoadFromMemoryCallBack };
			SWM_Info rcOutInfo = { 0 };
			void *pSWHandle;
			void *pSWManager;
			
			SWM_Init_Library( DSKMAllocator, DSKMLiberator ); 
			
			pSWManager = SWM_Create_Manager();
			
			SWM_Register_CRCInChecker( pSWManager );
			SWM_Register_Decrypter( pSWManager );
			SWM_Register_Unpacker( pSWManager );
			
			pSWHandle = SWM_Prepare_To_Read( pSWManager, &rcInInfo, &rcOutInfo );
			
			if ( pSWHandle && rcOutInfo.m_pProc ) {
				SWM_StreamInfo rcStreamInfo;
				
				AVP_Serialize_Data	rcSerData;
				LoadFromMemInfo     rcLoadInfo;
				HDATA								hResultData;
				AVP_dword						nWillBeRead;
				AVP_char           *pTempBuffer; 
				
				rcStreamInfo.pSWMHandle = pSWHandle;
				rcStreamInfo.pUserData  = 0;

				pTempBuffer = DSKMAllocator( BUFFER_SIZE );
				
				if ( pTempBuffer ) {
					rcLoadInfo.m_pBuffer					= pBuffer;
					rcLoadInfo.m_nBufferSize			= dwBufferSize;
					rcLoadInfo.m_pfnCallBack			= pfnCallBack;
					rcLoadInfo.m_pvCallBackParams = pvCallBackParams;
					rcLoadInfo.m_nRead						= nRead;
					
					rcStreamInfo.pUserData = &rcLoadInfo;
					
					ds_mset( &rcSerData, 0, sizeof(rcSerData) );
					rcSerData.user_data = (void *)&rcStreamInfo;
					rcSerData.proc = (Buffer_Proc)rcOutInfo.m_pProc;
					
					nWillBeRead = dwBufferSize - nRead;
					hResultData = DATA_Deserialize( &rcSerData, pTempBuffer, BUFFER_SIZE, (AVP_dword *)&nWillBeRead );
					if ( hResultData )	{
//						DSKM_SetIsSpecialProp( hResultData, rcKLHeader.uiIsSpecial );
//						DSKM_SetObjTypeProp( hResultData, (AVP_dword)rcKLHeader.uiBaseType );
						DSKM_SetIsSpecialProp( hResultData, RHD_ISSPECIAL(rcKLHeader.uiObjType) );
						DSKM_SetObjTypeProp( hResultData, (AVP_dword)RHD_BASETYPE(rcKLHeader.uiObjType) );

						if ( *phRootData )
							DATA_Attach( *phRootData, 0, hResultData, DATA_IF_ROOT_INCLUDE );
						else
							*phRootData = hResultData;

						dwResult = DSKM_ERR_OK;
					}
					
					DSKMLiberator( pTempBuffer );
				}
			}
			
			SWM_Shut_Down_Library( pSWHandle );
			SWM_Shut_Down_Library( pSWManager );
		}	

		if ( pfnCallBack ) 
			pfnCallBack( pvCallBackParams, 0, 0, DSKM_CB_CLOSE );
		return dwResult;
	}
	DSKM_EXCEPT
}

/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_hash.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/8/2005 2:51:27 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmdefs.h"
#include "../_AVPIO.h"
#include "dskmi.h"

#include "cl_util.h"

#define BUFFER_SIZE (0xffff / 2)


//! \fn				: 	DSKM_GetBuffer_CallBack
//! \brief			:	Callback подкачки буфера объекта
//! \return			: int - количество записанных байт
//! \param          : void *pParams - параметры функции
//! \param          : void *pvBuffer	- указатель на буфер чтения
//! \param          : AVP_dword dwBufferSize - размер буфера чтения
//! \param          : AVP_dword dwCommand - код команды (DSKM_CB_*)
static int DSKMAPI DSKM_GetBuffer_CallBack( void *pParams, void *pvBuffer, AVP_dword dwBufferSize, AVP_dword dwCommand ) {
	if ( dwCommand == DSKM_CB_OPEN ) {
		DSKMSetFilePointer( ((DSKMCBP *)pParams)->m_pIo, ((DSKMCBP *)pParams)->m_hFile, 0, 0, FILE_BEGIN );
		return 0;
	}
	if ( dwCommand == DSKM_CB_READ ) {
		DWORD dwRead;
		if ( DSKMReadFile(((DSKMCBP *)pParams)->m_pIo, ((DSKMCBP *)pParams)->m_hFile, pvBuffer, dwBufferSize, &dwRead, 0) )
			return dwRead;
	}
	return -1;
}



//! \fn				: DSKM_HashObjectByHandle
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HANDLE hFile
//! \param          : void **ppvObjectHash
//! \param          : AVP_dword *pdwObjectHashSize
//! \param          : HDATA hKeysData
static AVP_dword	DSKMAPI DSKM_HashObjectByHandle( HDSKM hDSKM, HANDLE hFile, void **ppvObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_CANNOT_CALC_OBJ_HASH;
	
	if ( hFile != INVALID_HANDLE_VALUE ) {
		DSKM_Io *pIo = DSKM_GetIO( hDSKM );
		dwResult = DSKM_ERR_IO_NOT_INITIALIZED;
		if ( pIo ) {
			DSKMSetFilePointer( pIo, hFile, 0, 0, FILE_BEGIN );
			dwResult = DSKM_ERR_CANNOT_CALC_OBJ_HASH;
			if ( DSKMGetFileSize(pIo, hFile, NULL) ) {
				AVP_byte *pBuffer = DSKMAllocator( BUFFER_SIZE );
				dwResult = DSKM_UNKNOWN_ERROR;
				if ( pBuffer ) {
					DSKMCBP rcCBP = {pIo, hFile};
					dwResult = DSKM_HashObjectByBufferImp( hDSKM, pBuffer, BUFFER_SIZE, DSKM_GetBuffer_CallBack, (void *)&rcCBP, ppvObjectHash, pdwObjectHashSize, hKeysData );
					DSKMLiberator( pBuffer );
				}
			}
		}
	}
	return dwResult;
}




//! \fn				: DSKM_HashObjectByHFileImp
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMFILE hObjFile
//! \param          : void **ppvObjectHash
//! \param          : AVP_dword *pdwObjectHashSize
//! \param          : HDATA hKeysData
AVP_dword	DSKMAPI DSKM_HashObjectByHFileImp( HDSKM hDSKM, HDSKMFILE hObjFile, void **ppvObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData ) {
	if ( hObjFile->m_hFile == INVALID_HANDLE_VALUE ) {
		DSKM_OpenHFileHandle( hDSKM, hObjFile );
	}
	if ( hObjFile->m_hFile != INVALID_HANDLE_VALUE ) 
		return DSKM_HashObjectByHandle( hDSKM, hObjFile->m_hFile, ppvObjectHash, pdwObjectHashSize, hKeysData );

	return DSKM_ERR_CANNOT_OPEN_FILE;
}




//! \fn				: DSKM_HashObjectByNameImp
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pObjectFileName
//! \param          : void **ppvObjectHash
//! \param          : AVP_dword *pdwObjectHashSize
//! \param          : HDATA hKeysData
AVP_dword	DSKMAPI DSKM_HashObjectByNameImp( HDSKM hDSKM, const AVP_char* pObjectFileName, void **ppvObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_IO_NOT_INITIALIZED;
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		HANDLE hFile = DSKMCreateFile( pIo, pObjectFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
		dwResult = DSKM_ERR_CANNOT_OPEN_FILE;
		if ( hFile != INVALID_HANDLE_VALUE ) {
			dwResult = DSKM_HashObjectByHandle( hDSKM, hFile, ppvObjectHash, pdwObjectHashSize, hKeysData );
			DSKMCloseHandle( pIo, hFile );
		}
	}
	return dwResult;
}





//! \fn				: DSKM_HashObjectByBufferImp
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pCallBack
//! \param          : void *pCallBackParams
//! \param          : void **ppvObjectHash
//! \param          : AVP_dword *pdwObjectHashSize
//! \param          : HDATA hKeysData
AVP_dword	DSKMAPI DSKM_HashObjectByBufferImp( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, void **ppvObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;

	if ( pBuffer && dwBufferSize ) {
		DSKMHash *prcHash = DSKMAllocator( sizeof(DSKMHash) );
		dwResult = DSKM_UNKNOWN_ERROR;
		if ( prcHash ) {
			AVP_size_t dwParam1 = (AVP_size_t)prcHash;

			ds_mset( prcHash, 0, sizeof(DSKMHash) );
			DSCL_CALL_HASH_INIT( dwResult, hDSKM, dwParam1/*prcHash*/, 0 );

			if ( pCallBack ) {
				if ( pCallBack(pCallBackParams, 0, 0, DSKM_CB_OPEN) >= 0 ) {
					int nResult = 0;
					int dwcbRead = 0;
					nResult = !(dwcbRead = pCallBack(pCallBackParams, pBuffer, dwBufferSize, DSKM_CB_READ));
					while( !nResult && dwcbRead >= 0 ) {
						DSCL_CALL_HASH_BUFFER( dwResult, hDSKM, dwParam1/*prcHash*/, pBuffer, dwcbRead );
						nResult = !(dwcbRead = pCallBack(pCallBackParams, pBuffer, dwBufferSize, DSKM_CB_READ));
					}
					dwResult = DSKM_ERR_OK;
					if ( dwcbRead < 0 ) 
						dwResult = DSKM_ERR_CALLBACK_READ_ERROR;

					pCallBack(pCallBackParams, 0, 0, DSKM_CB_CLOSE);
				}
				else
					dwResult = DSKM_ERR_CALLBACK_READ_ERROR;
			}
			else {
				DSCL_CALL_HASH_BUFFER( dwResult, hDSKM, dwParam1/*prcHash*/, pBuffer, dwBufferSize );
				dwResult = DSKM_ERR_OK;
			}

			if ( DSKM_OK(dwResult) ) {
				DSCL_CALL_HASH_END( dwResult, hDSKM, dwParam1/*prcHash*/ );
				dwResult = DSKM_ERR_OK;

				if ( pdwObjectHashSize )
					*pdwObjectHashSize = sizeof(prcHash->hash);
				if ( ppvObjectHash ) {
					if ( !*ppvObjectHash ) {
						*ppvObjectHash = DSKMAllocator( *pdwObjectHashSize );
					}
					if ( *ppvObjectHash ) {
						ds_mcpy( *ppvObjectHash, &prcHash->hash, *pdwObjectHashSize );
					}
				}
			}

			DSKMLiberator( prcHash );
		}
	}
	return dwResult;
}



//! \fn				: DSKM_HashBufferToHash
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void *pvPropBuffer
//! \param          : AVP_dword dwPropSize
//! \param          : void *pvObjectHash
//! \param          : AVP_dword pdwObjectHashSize
AVP_dword	DSKMAPI DSKM_HashBufferToHash( HDSKM hDSKM, void *pvPropBuffer, AVP_dword dwPropSize, void *pvObjectHash, AVP_dword pdwObjectHashSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	
	if ( pvPropBuffer && dwPropSize && pvObjectHash && pdwObjectHashSize ) {
		DSKMHash *prcHash = DSKMAllocator( sizeof(DSKMHash) );
		dwResult = DSKM_UNKNOWN_ERROR;
		if ( prcHash ) {
			AVP_size_t dwParam1 = (AVP_size_t)prcHash;
			HDATA hKeysData = (HDATA)hDSKM;
			
			ds_mset( prcHash, 0, sizeof(DSKMHash) );
			DSCL_CALL_HASH_INIT( dwResult, hDSKM, dwParam1/*prcHash*/, pvObjectHash );
			DSCL_CALL_HASH_BUFFER( dwResult, hDSKM, dwParam1/*prcHash*/, pvPropBuffer, dwPropSize );
			DSCL_CALL_HASH_END( dwResult, hDSKM, dwParam1/*prcHash*/ );

			dwResult = DSKM_ERR_OK;
			
			ds_mcpy( pvObjectHash, &prcHash->hash, pdwObjectHashSize );

			DSKMLiberator( prcHash );
		}
	}
	return dwResult;
}

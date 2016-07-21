/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	cl_hass.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	10/12/2005 3:12:14 PM
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

AVP_dword DSKMAPI DSKM_HashBufferFindSign( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, DSKMHash *prcHash, DSKMSign *prcSign, HDATA hKeysData );

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



//! \fn				: DSKM_HashObjectByHandleCheckSign
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HANDLE hFile
//! \param          : void **ppvObjectHash
//! \param          : AVP_dword *pdwObjectHashSize
//! \param          : HDATA hKeysData
static AVP_dword	DSKMAPI DSKM_HashObjectByHandleCheckSign( HDSKM hDSKM, HANDLE hFile, void **ppvObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData, AVP_bool bTreatAnotherSignAsError ) {
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
					dwResult = DSKM_HashObjectByBufferCheckSignImp( hDSKM, pBuffer, BUFFER_SIZE, DSKM_GetBuffer_CallBack, (void *)&rcCBP, ppvObjectHash, pdwObjectHashSize, hKeysData, bTreatAnotherSignAsError );
					DSKMLiberator( pBuffer );
				}
			}
		}
	}
	return dwResult;
}




//! \fn				: DSKM_HashObjectByNameCheckSignImp
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pObjectFileName
//! \param          : void **ppvObjectHash
//! \param          : AVP_dword *pdwObjectHashSize
//! \param          : HDATA hKeysData
AVP_dword	DSKMAPI DSKM_HashObjectByNameCheckSignImp( HDSKM hDSKM, const AVP_char* pObjectFileName, void **ppvObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData, AVP_bool bTreatAnotherSignAsError ) {
	AVP_dword dwResult = DSKM_ERR_IO_NOT_INITIALIZED;
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		HANDLE hFile = DSKMCreateFile( pIo, pObjectFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
		dwResult = DSKM_ERR_CANNOT_OPEN_FILE;
		if ( hFile != INVALID_HANDLE_VALUE ) {
			dwResult = DSKM_HashObjectByHandleCheckSign( hDSKM, hFile, ppvObjectHash, pdwObjectHashSize, hKeysData, bTreatAnotherSignAsError );
			DSKMCloseHandle( pIo, hFile );
		}
	}
	return dwResult;
}




//! \fn				: DSKM_HashObjectByBufferCheckSignImp
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
//! \param          : AVP_bool bTreatAnotherSignAsError
AVP_dword	DSKMAPI DSKM_HashObjectByBufferCheckSignImp( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, void **ppvObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData, AVP_bool bTreatAnotherSignAsError ) {
	DSKMSign *prcSign = DSKMAllocator( sizeof(DSKMSign) );
	DSKMHash *prcHash = DSKMAllocator( sizeof(DSKMHash) );
	AVP_dword dwResult = DSKM_ERR_HASH_NOT_FOUND;

	if ( prcSign && prcHash ) {
		dwResult = DSKM_HashBufferFindSign( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, prcHash, prcSign, hKeysData );
		
		if ( dwResult == DSKM_ERR_SIGN_NOT_FOUND || (DSKM_OK(dwResult) && !bTreatAnotherSignAsError) ) {
			if ( pdwObjectHashSize )
				*pdwObjectHashSize = sizeof(prcHash->hash);
			if ( ppvObjectHash ) {
				*ppvObjectHash = DSKMAllocator( *pdwObjectHashSize );
				if ( *ppvObjectHash ) {
					ds_mcpy( *ppvObjectHash, &prcHash->hash, *pdwObjectHashSize );
				}
			}
			dwResult = DSKM_ERR_OK;
		}
		else
			if ( DSKM_OK(dwResult) )
				dwResult = DSKM_ERR_ANOTHER_SIGN_FOUND;

		DSKMLiberator( prcSign );
		DSKMLiberator( prcHash );
	}

	return dwResult;
}




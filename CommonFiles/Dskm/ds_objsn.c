/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_objsn.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	9/30/2005 12:39:13 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"



//! \fn				:  DSKMAPI DSKM_ParList_AddFileAssociation
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pszObjRegFileName
//! \param          : AVP_dword dwObjRegFileNameSize
//! \param          : const AVP_char* pszKeyRegFileName
//! \param          : AVP_dword dwKeyRegFileNameNameSize
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddFileAssociation( HDSKMLIST hList, AVP_dword dwObjectsType, const AVP_char* pszObjRegFileName, AVP_dword dwObjRegFileNameSize, const AVP_char* pszKeyRegFileName, AVP_dword dwKeyRegFileNameNameSize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			AVP_dword dwPID = DSKM_ASSOC_OTYPE_ID( 0 );
			dwResult = DSKM_ERR_DATA_PROCESSING;
			dwPID = DSKM_FindUniquePID( (HDATA)hList, dwPID );
			if ( dwPID ) {
				hListObj = (HDSKMLISTOBJ)DATA_Add( (HDATA)hList, 0, dwPID, dwObjectsType, 0 );
				if ( hListObj ) {
					if ( pszObjRegFileName ) {
						if ( !dwObjRegFileNameSize )
							dwObjRegFileNameSize = ds_slen(pszObjRegFileName) + 1;
						DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_ASSOC_SIGN_FILE_ID, (AVP_size_t)pszObjRegFileName, dwObjRegFileNameSize );
					}
					if ( pszKeyRegFileName ) {
						if ( !dwKeyRegFileNameNameSize )
							dwKeyRegFileNameNameSize = ds_slen(pszKeyRegFileName) + 1;
						DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_ASSOC_KEY_FILE_ID, (AVP_size_t)pszKeyRegFileName, dwKeyRegFileNameNameSize );
					}
					dwResult = DSKM_ERR_OK;
				}
			}
		}

		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}




//! \fn				: DSKM_ParList_PrepareNamedObject
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwPID
//! \param          : const AVP_char* pszObjectName
//! \param          : AVP_dword dwObjectNameSize
static HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_PrepareNamedObject( HDSKMLIST hList, AVP_dword dwPID, const AVP_char* pszObjectName, AVP_dword dwObjectNameSize) {
	HDSKMLISTOBJ hListObj = 0;
	AVP_dword    dwResult = DSKM_ERR_DATA_PROCESSING;
	
	if ( !GET_AVP_PID_ID(dwPID) ) 
		dwPID = DSKM_FindUniquePID( (HDATA)hList, dwPID );
	if ( dwPID ) {
		if ( !dwObjectNameSize )
			dwObjectNameSize = ds_slen(pszObjectName) + 1;
		if ( hListObj = (HDSKMLISTOBJ)DATA_Add((HDATA)hList, 0, dwPID, (AVP_size_t)pszObjectName, dwObjectNameSize) ) {
			// Поставим ошибку сразу - если проверку хакнут обходом кода, ошибка останется
			DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_LIB_CODE_HACKED, 0 );
			dwResult = DSKM_ERR_OK;
		}
	}

	DSKM_ParList_SetLastError( hList, dwResult );

	return hListObj;
}



//! \fn				:  DSKMAPI DSKM_ParList_AddObjectTypeAssociationFile
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKMLIST hList
//! \param          : const AVP_char* pszAssocFileName
//! \param          : AVP_dword dwAssocFileNameSize
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectTypeAssociationFile( HDSKMLIST hList, const AVP_char* pszAssocFileName, AVP_dword dwAssocFileNameSize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;

		DSKM_ParList_SetLastError( hList, DSKM_ERR_INVALID_PARAMETER );
		if ( hList && pszAssocFileName ) {
			DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
			if ( DSKMAllocator && DSKMAllocator ) {
				AVP_dword dwPID = DSKM_ASSOC_FILE_ID( 0 );
				hListObj = DSKM_ParList_PrepareNamedObject( hList, dwPID, pszAssocFileName, dwAssocFileNameSize );
			}
		}
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}


//! \fn				: DSKM_ParList_AddNamedObject
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwObjectId
//! \param          : const AVP_char* pObjectName
//! \param          : AVP_dword dwObjectNameSize
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddNamedObject( HDSKMLIST hList, AVP_dword dwObjectId, const AVP_char* pObjectName, AVP_dword dwObjectNameSize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;

		DSKM_ParList_SetLastError( hList, DSKM_ERR_INVALID_PARAMETER );
		if ( hList && pObjectName ) {
			DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
			if ( DSKMAllocator && DSKMAllocator ) {
				AVP_dword dwPID = DSKM_OBJECT_NAME_ID( dwObjectId );
				hListObj = DSKM_ParList_PrepareNamedObject( hList, dwPID, pObjectName, dwObjectNameSize );
			}
		}
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}


//! \fn				: DSKM_ParList_AddObjectHash
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : void *pHashPtr
//! \param          : AVP_dword dwHashSize
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectHash( HDSKMLIST hList, AVP_dword dwObjectId, void *pHashPtr, AVP_dword dwHashSize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( hList && pHashPtr && dwHashSize ) {
			dwResult = DSKM_ERR_NOT_INITIALIZED;
			if ( DSKMAllocator && DSKMAllocator ) {
				AVP_dword dwPID = DSKM_OBJECT_HASH_ID( dwObjectId );
				dwResult = DSKM_ERR_DATA_PROCESSING;
				if ( !dwObjectId ) 
					dwPID = DSKM_FindUniquePID( (HDATA)hList, dwPID );

				hListObj = (HDSKMLISTOBJ)DATA_Add( (HDATA)hList, 0, dwPID, (AVP_size_t)pHashPtr, dwHashSize );
				if ( hListObj ) {
					// Поставим ошибку сразу - если проверку хакнут обходом кода, ошибка останется
					DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_LIB_CODE_HACKED, 0 );
					dwResult = DSKM_ERR_OK;
				}
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



//! \fn				: DSKM_ParList_AddNamedReg
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwRegId
//! \param          : const AVP_char* pRegName
//! \param          : AVP_dword dwRegNameSize
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddNamedReg( HDSKMLIST hList, AVP_dword dwRegId, const AVP_char* pRegName, AVP_dword dwRegNameSize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		DSKM_ParList_SetLastError( hList, DSKM_ERR_INVALID_PARAMETER );
		if ( hList && pRegName ) {
			DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
			if ( DSKMAllocator && DSKMAllocator ) {
				AVP_dword dwPID = DSKM_REG_NAME_ID( dwRegId );
				hListObj = DSKM_ParList_PrepareNamedObject( hList, dwPID, pRegName, dwRegNameSize );
			}
		}
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



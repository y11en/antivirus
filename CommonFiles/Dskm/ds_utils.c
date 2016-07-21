/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_utils.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/7/2005 4:53:38 PM
*		
*		Example:	
*		
*		
*		
*/		


#include <stdlib.h>
#include "dskm.h"
#include "dskmdefs.h"
#include "../_AVPIO.h"
#include "dskmi.h"


void* (* DSKMAllocator)(AVP_size_t) = 0;
void  (* DSKMLiberator)(void*) = 0;



//! \fn				: DSKM_DeInitLibrary
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : AVP_bool bDeInitSubLib
AVP_dword	DSKMAPI DSKM_DeInitLibrary( HDSKM hDSKM, AVP_bool bDeInitSubLib  ) {
	DSKM_TRY {
		if ( hDSKM ) {
			DSKM_Io *pIo = DSKM_GetIO( hDSKM );
			if ( pIo ) {
				pIo->lpVtbl->Release( pIo );
			}
			DSKM_DeInitCriptoLibrary( hDSKM );
			DATA_Remove( (HDATA)hDSKM, 0 );
		}
		if ( bDeInitSubLib )
			DATA_Deinit_Library();
		
		return DSKM_ERR_OK;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_GetIO
//! \brief			:	
//! \return			: DSKM_Io *DSKMAPI 
//! \param          : HDSKM hDSKM
DSKM_Io *DSKMAPI DSKM_GetIO( HDSKM hDSKM ) {
	DSKM_Io *pIo = 0;
	HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_IO_ID );
	if ( hProp ) {
		PROP_Get_Val( hProp, &pIo, sizeof(avpt_dword) );
	}
	return pIo;
}


//! \fn				: DSKMAPI DSKM_FindUniquePID
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDATA hParentData
//! \param          : AVP_dword nSourcePID
AVP_dword DSKMAPI DSKM_FindUniquePID( HDATA hParentData, AVP_dword nSourcePID ) {
	DSKM_TRY {
		static AVP_bool bRandInitialized = 0;
		
		AVP_dword nSAPID = GET_AVP_PID_APP(nSourcePID);
		AVP_dword nSType = GET_AVP_PID_TYPE(nSourcePID);
		AVP_dword nSArr  = GET_AVP_PID_ARR(nSourcePID);
		
		if ( !bRandInitialized ) {
			srand( (int)hParentData );
			bRandInitialized = 1;
		}
		
		while( 1 ) {
			AVP_dword nID = rand() % GET_AVP_PID_ID_MAX_VALUE;
			AVP_dword nPID = MAKE_AVP_PID( nID, nSAPID, nSType, nSArr );
			MAKE_ADDR1(nAddr, nPID); 
			if ( !DATA_Find(hParentData, nAddr) )
				return nPID;
		}
		
		return 0;
	}
	DSKM_EXCEPT_SIZE
}



//! \fn				: DSKMAPI DSKM_FindUniquePropPID
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDATA hParentData
//! \param          : AVP_dword nSourcePID
AVP_dword DSKMAPI DSKM_FindUniquePropPID( HDATA hParentData, AVP_dword nSourcePID ) {
	DSKM_TRY {
		static AVP_bool bRandInitialized = 0;
		
		AVP_dword nSAPID = GET_AVP_PID_APP(nSourcePID);
		AVP_dword nSType = GET_AVP_PID_TYPE(nSourcePID);
		AVP_dword nSArr  = GET_AVP_PID_ARR(nSourcePID);
		
		if ( !bRandInitialized ) {
			srand( (int)hParentData );
			bRandInitialized = 1;
		}
		
		while( 1 ) {
			AVP_dword nID = rand() % GET_AVP_PID_ID_MAX_VALUE;
			AVP_dword nPID = MAKE_AVP_PID( nID, nSAPID, nSType, nSArr );
			if ( !DATA_Find_Prop(hParentData, 0, nPID) )
				return nPID;
		}
		
		return 0;
	}
	DSKM_EXCEPT_SIZE
}





//! \fn				:  DSKM_GetPropValueAsString
//! \brief			:	
//! \return			: AVP_char *DSKMAPI 
//! \param          :  HPROP hProp
//! \param          : AVP_dword *dwStrSize
AVP_char *DSKMAPI  DSKM_GetPropValueAsString( HPROP hProp, AVP_dword *dwStrSize ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMLiberator ) {
			return 0;
		}
		else {
			char *pResult = 0;

			AVP_dword nID = PROP_Get_Id( hProp );
			AVP_dword nType = GET_AVP_PID_TYPE(nID);
			switch ( nType ) {
				case avpt_str    : 
				case avpt_bin    : {
					int nSize = PROP_Get_Val( hProp, 0, 0 );
					if ( nSize ) {
						pResult = DSKMAllocator( sizeof(AVP_byte) * nSize );
						*pResult = 0;
						PROP_Get_Val( hProp, pResult, nSize );
						if ( dwStrSize )
							*dwStrSize = nSize;
					}
				}
					break;
			}
			return pResult;
		}
	}
	DSKM_EXCEPT_SIZE
}

//! \fn				: DSKMAPI DSKM_CopyObjectsTree
//! \brief			:	
//! \return			: static HDATA 
//! \param          : HDATA hSrcData
HDATA DSKMAPI DSKM_CopyObjectsTree( HDATA hSrcData ) {
	HDATA hDstData = DATA_Add(0, 0, AVP_PID_NOTHING, 0, 0);
	HDATA hCurrData = DATA_Get_First( hSrcData, 0 );
	while ( hCurrData ) {
		AVP_dword dwPID = DATA_Get_Id( hCurrData, 0 );
		dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
		switch ( dwPID ) {
			case DSKM_OBJECT_NAME_ID(0) : 
			case DSKM_OBJECT_BUFF_PTR_ID(0) : 
			case DSKM_OBJECT_HASH_ID(0) : {
				HDATA hCopyData = DATA_Copy( hDstData, 0, hCurrData, DATA_IF_ROOT_INCLUDE );
				if ( hCopyData ) {
					HPROP hProp = DATA_Find_Prop( hCopyData, 0, DSKM_OBJECT_ERROR_ID );
					if ( !hProp ) {
						DATA_Add_Prop( hCopyData, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_HASH_NOT_FOUND, 0 );
					}
					hProp = DATA_Find_Prop( hCopyData, 0, DSKM_OBJECT_LISTOBJ_ID );
					if ( !hProp ) {
						DATA_Add_Prop( hCopyData, 0, DSKM_OBJECT_LISTOBJ_ID, (AVP_size_t)hCurrData, 0 );
					}
					else {
						PROP_Set_Val( hProp, (AVP_size_t)hCurrData, 0 );
					}
				}
				break;
			}
		}
		
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}
	return hDstData;
}



//! \fn				: DSKM_CheckObjectList
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKMLIST HDSKM hDSKM
//! \param          : hObjectsList
//! \param          : HDATA hKeysData
AVP_dword DSKMAPI DSKM_CheckObjectList( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	
	if ( hObjectsList ) {
		HDATA hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
		while ( hCurrData ) {
			AVP_dword dwPID = DATA_Get_Id( hCurrData, 0 );
			HPROP hErrProp = DATA_Find_Prop( hCurrData, 0, DSKM_OBJECT_ERROR_ID );
			if ( !hErrProp ) 
				DATA_Add_Prop( hCurrData, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_INVALID_SIGN, 0 );
			dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
			switch ( dwPID ) {
				case DSKM_OBJECT_NAME_ID(0) : {
					AVP_char *pObjectFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, 0), 0 );
					if ( pObjectFileName ) {
						dwResult = DSKM_CheckFile( hDSKM, hCurrData, pObjectFileName, hKeysData );
						DSKMLiberator( pObjectFileName );
					}
					break;
				}
				case DSKM_OBJECT_BUFF_PTR_ID(0) : {
					void *pBuffer;
					AVP_dword dwBufferSize;
					pfnDSKM_GetBuffer_CallBack pCallBack;
					void *pCallBackParams;
					
					DATA_Get_Val( hCurrData, 0, 0, &pBuffer, sizeof(pBuffer) );
					DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
					DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
					DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
					
					dwResult = DSKM_CheckObjectByBuffer( hDSKM, hCurrData, pBuffer, dwBufferSize, pCallBack, pCallBackParams, hKeysData );
					break;
				}
			}

			PROP_Set_Val( hErrProp, dwResult, 0 );
			
			hCurrData = DATA_Get_Next( hCurrData, 0 );
		}
	}
	
	return dwResult;
}




//! \fn				: DSKM_CheckRegFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDSKMFILE hRegFile
//! \param          : HDSKMFILE hKeyFile
//! \param          : void *pKeyData
//! \param          : AVP_dword dwKeyDataSize
AVP_dword DSKMAPI DSKM_CheckRegFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDSKMFILE hRegFile, HDSKMFILE hKeyFile, void *pKeyData, AVP_dword dwKeyDataSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;

	if ( hDSKM ) {
		void *pObjectHash = 0;
		AVP_dword dwObjectHashSize = 0;
		if (DSKM_OK(dwResult = DSKM_HashObjectByHFileImp(hDSKM, hRegFile, &pObjectHash, &dwObjectHashSize, (HDATA)hDSKM)) ) {
			// Искать хэш реестра в контексте
			AVP_bool bFound = 0;
			HDATA hCurrData = DATA_Get_First( (HDATA)hDSKM, 0 );
			dwResult = DSKM_ERR_HASH_NOT_FOUND;
			while ( hCurrData ) {
				AVP_dword dwRegHashSize = DATA_Get_Val( hCurrData, 0, 0, 0, 0 );
				if ( dwRegHashSize && dwRegHashSize == dwObjectHashSize ) {
					void *pRegHash = DSKMAllocator( dwRegHashSize );
					DATA_Get_Val( hCurrData, 0, 0, pRegHash, dwRegHashSize );
					if ( !ds_mcmp(pRegHash, pObjectHash, dwRegHashSize) ) {
						// Хэш найден
						bFound = 1;
					}
					DSKMLiberator( pRegHash );
					if ( bFound ) {
						dwResult = DSKM_ERR_OK;
						break;
					}
				}
				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
		}

		if ( DSKM_NOT_OK(dwResult) ) {
			HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			
			dwResult = DSKM_ERR_DATA_PROCESSING;
			if ( hKeysRootData ) {
				dwResult = DSKM_ERR_KEY_NOT_FOUND;
				
				if ( pKeyData && dwKeyDataSize ) {
					dwResult = DSKM_FindKeysTreeInBuffer( hDSKM, hObjectsList, dwObjectsType, pKeyData, dwKeyDataSize, hKeysRootData );
				}
				else {
					if ( hKeyFile )
						dwResult = DSKM_FindKeysTreeInFile( hDSKM, hObjectsList, dwObjectsType, hKeyFile, hKeysRootData );
				}

				if ( DSKM_OK(dwResult) ) {
					dwResult = DSKM_CheckHFile( hDSKM, hRegFile, hKeysRootData );
					// Добавить хэш реестра в контекст
					if ( DSKM_OK(dwResult) && pObjectHash ) {
						AVP_dword dwPID = DSKM_OBJECT_REG_HASH_ID(0);
						dwPID = DSKM_FindUniquePID( (HDATA)hDSKM, dwPID );
						if ( dwPID ) 
							DATA_Add( (HDATA)hDSKM, 0, dwPID, (AVP_size_t)pObjectHash, dwObjectHashSize );
					}
				}

				DATA_Remove( hKeysRootData, 0 );
			}
		}
		if ( pObjectHash )
			DSKMLiberator( pObjectHash );
	}

	return dwResult;
}



//! \fn				: DSKM_CheckRegObject
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hRegObject
//! \param          : HDATA hKeyReg
//! \param          : void *pKeyData
//! \param          : AVP_dword dwKeyDataSize
AVP_dword DSKMAPI DSKM_CheckRegObject( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hRegObject, HDATA hKeyReg, void *pKeyData, AVP_dword dwKeyDataSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	
	if ( hDSKM ) {
		void *pObjectHash = 0;
		AVP_dword dwObjectHashSize = 0;
		if (DSKM_OK(dwResult = DSKM_PrepareObjectHash(hDSKM, hRegObject, &pObjectHash, &dwObjectHashSize)) ) {
			// Искать хэш реестра в контексте
			AVP_bool bFound = 0;
			HDATA hCurrData = DATA_Get_First( (HDATA)hDSKM, 0 );
			dwResult = DSKM_ERR_HASH_NOT_FOUND;
			while ( hCurrData ) {
				AVP_dword dwPID = DATA_Get_Id( hCurrData, 0 );
				AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
				if ( dwPID_0 == DSKM_OBJECT_REG_HASH_ID(0) ) {
					AVP_dword dwRegHashSize = DATA_Get_Val( hCurrData, 0, 0, 0, 0 );
					if ( dwRegHashSize && dwRegHashSize == dwObjectHashSize ) {
						void *pRegHash = DSKMAllocator( dwRegHashSize );
						DATA_Get_Val( hCurrData, 0, 0, pRegHash, dwRegHashSize );
						if ( !ds_mcmp(pRegHash, pObjectHash, dwRegHashSize) ) {
							// Хэш найден
							bFound = 1;
						}
						DSKMLiberator( pRegHash );
						if ( bFound ) {
							dwResult = DSKM_ERR_OK;
							break;
						}
					}
				}
				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
		}
	
		if ( DSKM_NOT_OK(dwResult) ) {
			HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			
			dwResult = DSKM_ERR_DATA_PROCESSING;
			if ( hKeysRootData ) {
				dwResult = DSKM_ERR_KEY_NOT_FOUND;
				
				if ( pKeyData && dwKeyDataSize ) {
					dwResult = DSKM_FindKeysTreeInBuffer( hDSKM, hObjectsList, dwObjectsType, pKeyData, dwKeyDataSize, hKeysRootData );
				}
				else {
					dwResult = DSKM_FindKeyData( (HDATA)hObjectsList, dwObjectsType, hKeyReg, hKeysRootData )
						         ? DSKM_ERR_OK
										 : DSKM_ERR_KEY_NOT_FOUND;
				}
				
				if ( DSKM_OK(dwResult) ) {
					dwResult = DSKM_CheckObject( hDSKM, hRegObject, hKeysRootData );
					// Добавить хэш реестра в контекст
					if ( DSKM_OK(dwResult) && pObjectHash ) {
						AVP_dword dwPID = DSKM_OBJECT_REG_HASH_ID(0);
						dwPID = DSKM_FindUniquePID( (HDATA)hDSKM, dwPID );
						if ( dwPID ) 
							DATA_Add( (HDATA)hDSKM, 0, dwPID, (AVP_size_t)pObjectHash, dwObjectHashSize );
					}
				}
				
				DATA_Remove( hKeysRootData, 0 );
			}
		}
		if ( pObjectHash )
			DSKMLiberator( pObjectHash );
	}
	
	return dwResult;
}






//! \fn				: DSKMAPI DSKM_ProcessObjectsError
//! \brief			:	
//! \return			: void  
//! \param          :  HDATA hObjectsList
//! \param          : HDATA hNotProcessedObjects
//! \param          : AVP_dword dwResult
void  DSKMAPI DSKM_ProcessObjectsError( HDATA hObjectsList, HDATA hNotProcessedObjects, AVP_dword dwResult ) {
	HDATA hCurrListData = DATA_Get_First( hObjectsList, 0 );
	while ( hCurrListData ) {
		if ( DSKM_OK(DSKM_IsObject(hCurrListData)) ) {
			AVP_dword dwPID = DATA_Get_Id( hCurrListData, 0 );
			if ( !hNotProcessedObjects ) {
				// Установка ошибки на всех
				HPROP hErrProp = DATA_Find_Prop( hCurrListData, 0, DSKM_OBJECT_ERROR_ID );
				if ( !hErrProp ) 
					DATA_Add_Prop( hCurrListData, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
				else
					DATA_Set_Val( hCurrListData, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
			}
			else {
				AVP_bool bErrIsSet = 0;
				HDATA hCurrObjectsData = DATA_Get_First( hNotProcessedObjects, 0 );
				while ( hCurrObjectsData ) {
					AVP_dword dwObjPID = DATA_Get_Id( hCurrObjectsData, 0 );
					if ( dwObjPID == dwPID ) {
						HPROP hErrProp = DATA_Find_Prop( hCurrObjectsData, 0, DSKM_OBJECT_ERROR_ID );
						if ( hErrProp ) {
							PROP_Get_Val( hErrProp, &dwResult, sizeof(dwResult) );
							hErrProp = DATA_Find_Prop( hCurrListData, 0, DSKM_OBJECT_ERROR_ID );
							if ( !hErrProp ) {
								DATA_Add_Prop( hCurrListData, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
							}
							else {
								DATA_Set_Val( hCurrListData, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
							}
							bErrIsSet = 1;
						}
						break;
					}
					hCurrObjectsData = DATA_Get_Next( hCurrObjectsData, 0 );
				}
				if ( !bErrIsSet ) {
					// Значит объект был обработан - его нет в списке не-обработанных
					if ( DATA_Find_Prop(hCurrListData, 0, DSKM_OBJECT_ERROR_ID) )
						DATA_Remove_Prop_ID( hCurrListData, 0, DSKM_OBJECT_ERROR_ID );
				}
			}
		}
		
		hCurrListData = DATA_Get_Next( hCurrListData, 0 );
	}
}



//! \fn				: DSKM_FindCorrespondingListObject
//! \brief			:	
//! \return			: HDATA DSKMAPI 
//! \param          : HDATA hObjectToFind
HDATA DSKMAPI DSKM_FindCorrespondingListObject( HDATA hObjectToFind ) {
	HDATA hListData = 0;
	HPROP hProp = DATA_Find_Prop( hObjectToFind, 0, DSKM_OBJECT_LISTOBJ_ID );
	if ( hProp ) {
		PROP_Get_Val( hProp, &hListData, sizeof(HDATA) );
	}
	return hListData;
}


//! \fn				: DSKM_CreateHFile
//! \brief			:	
//! \return			: HDSKMFILE DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pFileName
HDSKMFILE DSKMAPI DSKM_CreateHFile( HDSKM hDSKM, const AVP_char* pFileName ) {
	HDSKMFILE hFile = DSKMAllocator( sizeof(DSKMFILE) );
	if ( hFile ) {
		hFile->m_pFileName = pFileName;
		hFile->m_hFile = INVALID_HANDLE_VALUE;
	}
	return hFile;
}



//! \fn				: DSKM_OpenHFileHandle
//! \brief			:	
//! \return			: void DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMFILE hFile
void DSKMAPI DSKM_OpenHFileHandle( HDSKM hDSKM, HDSKMFILE hFile )	{
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		hFile->m_hFile = DSKMCreateFile( pIo, hFile->m_pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
	}
}


//! \fn				: DSKM_ReuseHFile
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMFILE hFile
//! \param          : const AVP_char* pFileName
void	DSKMAPI DSKM_ReuseHFile( HDSKM hDSKM, HDSKMFILE hFile, const AVP_char* pFileName ) {
	if ( hFile ) {
		if ( hFile->m_hFile != INVALID_HANDLE_VALUE ) {
			DSKM_Io *pIo = DSKM_GetIO( hDSKM );
			if ( pIo ) {
				DSKMCloseHandle( pIo, hFile->m_hFile );
			}
		}
		hFile->m_pFileName = pFileName;
		hFile->m_hFile = INVALID_HANDLE_VALUE;
	}
}


//! \fn				: DSKM_CloseHFile
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMFILE hFile
void	DSKMAPI DSKM_CloseHFile( HDSKM hDSKM, HDSKMFILE hFile ) {
	if ( hFile->m_hFile != INVALID_HANDLE_VALUE ) {
		DSKM_Io *pIo = DSKM_GetIO( hDSKM );
		if ( pIo ) {
			DSKMCloseHandle( pIo, hFile->m_hFile );
		}
	}
	DSKMLiberator( hFile );
}



//! \fn				: DSKMAPI DSKM_SetIsSpecialProp
//! \brief			:	
//! \return			: void 
//! \param          :  HDATA hRegData
//! \param          : AVP_bool bSpecialReg
void DSKMAPI DSKM_SetIsSpecialProp( HDATA hRegData, AVP_bool bSpecialReg ) {
	HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_ISSPEC_ID );
	if ( bSpecialReg ) {
		if ( !hProp )
			DATA_Add_Prop( hRegData, 0, DSKM_KEYREG_ISSPEC_ID, 0, 0 );
	}
	else
		if ( hProp )
			DATA_Remove_Prop_H( hRegData, 0, hProp );
}



//! \fn				: DSKM_SetObjTypeProp
//! \brief			:	
//! \return			: void DSKMAPI 
//! \param          :  HDATA hRegData
//! \param          : AVP_dword dwObjType
void DSKMAPI DSKM_SetObjTypeProp( HDATA hRegData, AVP_dword dwObjType ) {
	HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_OBJTYPE_ID );
	if ( !hProp ) 
		DATA_Add_Prop( hRegData, 0, DSKM_KEYREG_OBJTYPE_ID, dwObjType, 0 );
}



//! \fn				: DSKM_IsObject
//! \brief			:	
//! \return			: void DSKMAPI 
//! \param          : HDATA hObjData
AVP_dword DSKMAPI DSKM_IsObject( HDATA hObjData ) {
	AVP_dword dwPID = DATA_Get_Id( hObjData, 0 );
	AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
	switch ( dwPID_0 ) {
		case DSKM_OBJECT_NAME_ID(0) : 
		case DSKM_OBJECT_BUFF_PTR_ID(0) : 
		case DSKM_OBJECT_HASH_ID(0) : 
		case DSKM_ASSOC_OTYPE_ID(0) :
		case DSKM_ASSOC_FILE_ID(0) :
		case DSKM_ASSOC_BUFF_PTR_ID(0) :
		case DSKM_REG_NAME_ID(0) : 
		case DSKM_REG_BUFF_PTR_ID(0) : {
			return DSKM_ERR_OK;
		}
	}
	return DSKM_ERR_INVALID_PARAMETER;
}

//! \fn				: * DSKM_Alloc
//! \brief			:	Рапределить память, используя систему распределения памяти переданную при инициировании
//! \return			: Указатель на рапределенный буфер
//! \param          : AVP_int iSize - размер
void * DSKMAPI DSKM_Alloc( AVP_int iSize ) {
	DSKM_TRY {
		return DSKMAllocator( iSize );
	}
	DSKM_EXCEPT_SIZE
}


//! \fn				: DSKM_Free
//! \brief			:	Удалить память, используя систему распределения памяти переданную при инициировании 
//! \return			: void
//! \param          : void *pvBuffer - буфер для удаления
void DSKMAPI DSKM_Free( void *pvBuffer ) {
	DSKM_TRY {
		DSKMLiberator( pvBuffer );
	}
	DSKM_EXCEPT_VOID
}


#if defined( DSKM_MEMUTIL )

// ---
unsigned int DSKMAPI ds_slen( const char* src ) {
	unsigned i;
	for( i=0; src[i] != 0; i++ );
	return i;
}

// ---
void* DSKMAPI ds_mcpy( void* dst, const void* src, unsigned size ) {
	unsigned i;
	void*  ret = dst;
	for( i=0; i<size; i++ )
		*((*(unsigned char**)&dst)++) = *((*(unsigned char**)&src)++);
	return ret;
}

// ---
void* DSKMAPI ds_mset( void* dst, unsigned fill, unsigned size ) {
	unsigned i;
	void*  ret = dst;
	for( i=0; i<size; i++ )
		*((*(unsigned char**)&dst)++) = fill;
	return ret;
}

// ---
int DSKMAPI ds_mcmp( const void* c1, const void* c2, unsigned size ) {
	unsigned i;
	int  ret = 0;
	for( i=0; i<size; i++ ) {
		ret = *((*(unsigned char**)&c1)++) - *((*(unsigned char**)&c2)++);
		if ( ret )
			return ret;
  }
	return 0;
}

#endif

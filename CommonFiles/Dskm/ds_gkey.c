/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_gkey.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/14/2005 4:06:43 PM
*		
*		Example:	
*		
*		
*		
*/		



#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"



//! \fn				:  DSKMAPI DSKM_SaveParamsToTypeNode
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKMLIST hObjectsList
//! \param          : HDATA hTypeNode
AVP_dword   DSKMAPI DSKM_SaveParamsToTypeNode( HDSKMLIST hObjectsList, HDATA hTypeNode ) {
	AVP_dword dwResult = DSKM_ERR_OK;
	if ( hObjectsList ) {
		HDATA hCurrParamData = DATA_Get_First( (HDATA)hObjectsList, 0 );
		while ( hCurrParamData && DSKM_OK(dwResult) ) {
			AVP_dword dwPID;
			AVP_dword dwPID_0;
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			dwPID = DATA_Get_Id( hCurrParamData, 0 );
			dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
			switch ( dwPID_0 ) {
				case DSKM_OBJECT_NAME_ID(0) : 
				case DSKM_OBJECT_BUFF_PTR_ID(0) : 
				case DSKM_OBJECT_HASH_ID(0) : 
				case DSKM_ASSOC_OTYPE_ID(0) : 
				case DSKM_ASSOC_FILE_ID(0) :
				case DSKM_ASSOC_BUFF_PTR_ID(0) :
				case DSKM_REG_NAME_ID(0) : 
				case DSKM_REG_BUFF_PTR_ID(0) : 
					dwResult = DSKM_ERR_OK;
					break;
				default : {
					switch ( GET_AVP_PID_APP(dwPID) ) {
						case DSKM_APP_DWORD_PARAM : {
							AVP_dword dwValue;
							DATA_Get_Val( hCurrParamData, 0, 0, &dwValue, sizeof(dwValue) );
							if ( DATA_Add_Prop(hTypeNode, 0, dwPID, dwValue, 0) )
								dwResult = DSKM_ERR_OK;
							break;
						}
						case DSKM_APP_STR_PARAM : 
						case DSKM_APP_BPTR_PARAM : {
							AVP_dword dwParamBufferSize = DATA_Get_Val( hCurrParamData, 0, 0, 0, 0 );
							if ( dwParamBufferSize ) {
								void *pParamBuffer = DSKMAllocator( dwParamBufferSize );
								if ( pParamBuffer ) {
									DATA_Get_Val( hCurrParamData, 0, 0, pParamBuffer, dwParamBufferSize );
									if ( DATA_Add_Prop(hTypeNode, 0, dwPID, (AVP_size_t)pParamBuffer, dwParamBufferSize) )
										dwResult = DSKM_ERR_OK;
									DSKMLiberator( pParamBuffer );
								}
							}
							break;
						}
					}
					break;
				}
			}
			hCurrParamData = DATA_Get_Next( hCurrParamData, 0 );
		}
	}
	return dwResult;
}



//! \fn				:  DSKMAPI DSKM_SaveKeyToKeyReg
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_dword dwKeyType
//! \param          : void *pKey
//! \param          : AVP_dword dwKeySize
//! \param          : const AVP_char* pKeyRegFileName
AVP_dword   DSKMAPI DSKM_SaveKeyToKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_dword dwKeyType, void *pKey, AVP_dword dwKeySize, const AVP_char* pKeyRegFileName ) {
	DSKM_TRY {
		if ( pKey && dwKeySize ) {
			if ( dwObjectsType ) {
				HDATA hRegRootData = 0;
				AVP_dword dwResult = DSKM_DeserializeRegFile( hDSKM, pKeyRegFileName, dwKeyType, &hRegRootData );
				
				if ( !hRegRootData && dwResult == DSKM_ERR_CANNOT_OPEN_REG_FILE )
					hRegRootData = DATA_Add( 0, 0, DSKM_KEY_PID, 0, 0 );

				dwResult = DSKM_ERR_DATA_PROCESSING;
				if ( hRegRootData ) {
					AVP_dword dwPID = DSKM_KEYREG_KEY_ID( 0 );
					dwPID = DSKM_FindUniquePID( (HDATA)hRegRootData, dwPID );
					
					if ( dwPID ) {
						HDATA hNode = DATA_Add( hRegRootData, 0, dwPID, (AVP_size_t)pKey, dwKeySize );
						if ( hNode ) {
							DATA_Add_Prop( hNode, 0, DSKM_KEYREG_OBJTYPE_ID, dwObjectsType, 0 );
							dwResult = DSKM_SaveParamsToTypeNode( hObjectsList, hNode );
						}
					}
					
					if ( DSKM_OK(dwResult) )
						dwResult = DSKM_SerializeRegToFile( hDSKM, dwKeyType, hRegRootData, pKeyRegFileName );
					
					DATA_Remove( hRegRootData, 0 );
				}
				
				return dwResult;
			}
			return DSKM_ERR_INVALID_OBJTYPE;
		}
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}



//! \fn				:  DSKMAPI DSKM_RemoveKeyFromKeyReg
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_dword dwKeyType
//! \param          : const AVP_char* pKeyRegFileName
AVP_dword   DSKMAPI DSKM_RemoveKeyFromKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_dword dwKeyType, const AVP_char* pKeyRegFileName ) {
	DSKM_TRY {
		dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, 0 );
		if ( dwObjectsType ) {
			HDATA hRegRootData = 0;
			AVP_dword dwResult = DSKM_DeserializeRegFile( hDSKM, pKeyRegFileName, dwKeyType, &hRegRootData );
			
			if ( hRegRootData ) {
				HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
				if ( !DSKM_FindKeyData((HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysRootData) ) {
					dwResult = DSKM_ERR_KEY_NOT_FOUND;
				}
				DATA_Remove( hKeysRootData, 0 );

				if ( DSKM_OK(dwResult) )
					dwResult = DSKM_SerializeRegToFile( hDSKM, dwKeyType, hRegRootData, pKeyRegFileName );
				
				DATA_Remove( hRegRootData, 0 );
			}
			
			return dwResult;
		}
		return DSKM_ERR_INVALID_OBJTYPE;
	}
	DSKM_EXCEPT
}


//! \fn				:  DSKMAPI DSKM_GetKeyFromKeyReg
//! \brief			:	Получить ключ из реестра ключей. 
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список параметров объектов, для которых предназначен ключ
//! \param          : AVP_dword dwObjectsType	- тип объектов, для которых предназначен ключ
//!                   (может быть = 0 - тогда должен быть задан файл ассоциаций)
//! \param          : AVP_dword dwKeyType - тип ключа (DSKM_OTYPE_KEY_*)
//! \param          : void **ppvKey - указатель на результирующий буфер ключа (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwKeySize - указатель на результирующий размер буфера ключа
//! \param          : AVP_dword *pdwKeyCount - указатель на количество ключей
//! \param          : const AVP_char* pszKeyRegFileName - имя файла реестра
AVP_dword   DSKMAPI DSKM_GetKeyFromKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_dword dwKeyType, void **ppvKey, AVP_dword *pdwKeySize, AVP_dword *pdwKeyCount, const AVP_char* pszKeyRegFileName ) {
	DSKM_TRY {
		if ( ppvKey && pdwKeySize && pdwKeyCount ) {
			*pdwKeySize = 0;
			*ppvKey = 0;
			*pdwKeyCount = 0;
			dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, 0 );
			if ( dwObjectsType ) {
				HDATA hRegRootData = 0;
				AVP_dword dwResult = DSKM_DeserializeRegFile( hDSKM, pszKeyRegFileName, dwKeyType, &hRegRootData );

				if ( hRegRootData ) {
					HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
					if ( !DSKM_FindKeyData((HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysRootData) ) {
						dwResult = DSKM_ERR_KEY_NOT_FOUND;
					}
					else {
						HDATA hCurrData = 0;
						DSKM_FTABLE_REMOVE(hKeysRootData);

						hCurrData = DATA_Get_First( hKeysRootData, 0 );
						while ( hCurrData ) {
							AVP_dword dwPublicKeySize = DATA_Get_Val( hCurrData, 0, 0, 0, 0 );
							if ( dwPublicKeySize ) {
								void *pPublicKey = DSKMAllocator( dwPublicKeySize );
								if ( pPublicKey ) {
									char *pResult = 0;
									DATA_Get_Val( hCurrData, 0, 0, pPublicKey, dwPublicKeySize );

									pResult = DSKMAllocator( *pdwKeySize + dwPublicKeySize );
									if ( *ppvKey ) {
										ds_mcpy( pResult, *ppvKey, *pdwKeySize );
										DSKMLiberator( *ppvKey );
									}
									ds_mcpy( pResult + *pdwKeySize, pPublicKey, dwPublicKeySize );

									*pdwKeySize += dwPublicKeySize;
									*ppvKey = pResult;
									*pdwKeyCount += 1;

									DSKMLiberator( pPublicKey );
								}
							}
							hCurrData = DATA_Get_Next( hCurrData, 0 );
						}
					}
					DATA_Remove( hRegRootData, 0 );

					dwResult = DSKM_ERR_OK;
				}

				return dwResult;
			}
			return DSKM_ERR_INVALID_OBJTYPE;
		}
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}

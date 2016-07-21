/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_check.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/15/2005 11:12:43 AM
*		
*		Example:	
*		
*		
*		
*/		

#if defined (_WIN32)
#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"
#include "../_avpff.h"

//! \fn				: DSKM_PrepareAssocData
//! \brief			:	
//! \return			: static HDATA 
//! \param          : HDSKMLIST hObjectsList
static HDATA DSKMAPI DSKM_PrepareAssocData( HDSKMLIST hObjectsList ) {
	if ( hObjectsList ) {
		HDATA hResData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
		HDATA hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
		while ( hCurrData ) {
			AVP_dword dwPID = DATA_Get_Id( hCurrData, 0 );
			dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
			switch ( dwPID ) {
				case DSKM_ASSOC_OTYPE_ID(0) :
					DATA_Copy( hResData, 0, hCurrData, DATA_IF_ROOT_INCLUDE );
					break;
				default :
					break;
			}

			hCurrData = DATA_Get_Next( hCurrData, 0 );
		}
		if ( DATA_Get_First(hResData, 0) )
			return hResData;

		DATA_Remove( hResData, 0 );
	}
	return 0;
}



//! \fn				: DSKM_CheckRegsChainByFile
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hAssocData
//! \param          : HDSKMFILE hRegFile
static AVP_dword 	DSKMAPI DSKM_CheckRegsChainByFile( HDSKM hDSKM, HDATA hAssocData, HDSKMFILE hRegFile ) {
	AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;
	HDATA hCurrData = DATA_Get_First( hAssocData, 0 );
	while ( hCurrData ) {
		// Искать узел, в котором RegFile == входному имени
		AVP_dword dwCurrRegFileNameSize;
		AVP_char *pCurrRegFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, DSKM_ASSOC_SIGN_FILE_ID), &dwCurrRegFileNameSize );
		if ( pCurrRegFileName ) {
			if ( !ds_mcmp(pCurrRegFileName, hRegFile->m_pFileName, dwCurrRegFileNameSize) ) {
				// Найден
				AVP_dword dwCurrObjectType = 0;
				// Взять его KeyFile
				AVP_char *pKeyFileName = 0;
				HPROP hProp = DATA_Find_Prop( hCurrData, 0, DSKM_ASSOC_KEY_FILE_ID );
				if ( hProp ) 
					pKeyFileName = DSKM_GetPropValueAsString( hProp, 0 );
				DATA_Get_Val( hCurrData, 0, 0, &dwCurrObjectType, sizeof(dwCurrObjectType) );
				if ( pKeyFileName ) {
					// Проверить KeyFile по цепи вверх
					HDSKMFILE hKeyFile = DSKM_CreateHFile( hDSKM, pKeyFileName );
					if ( hKeyFile ) {
						dwResult = DSKM_CheckRegsChainByFile( hDSKM, hAssocData, hKeyFile );
						if ( DSKM_OK(dwResult) ) {
							// Проверить текущий RegFile ключом из KeyFile 
							dwResult = DSKM_CheckRegFile( hDSKM, 0, dwCurrObjectType, hRegFile, hKeyFile, 0, 0 );
						}
						DSKM_CloseHFile( hDSKM, hKeyFile );
					}
					DSKMLiberator( pKeyFileName );
				}
				else {
					// Проверить подпись RegFile мастер-ключом
					dwResult = DSKM_CheckRegFile( hDSKM, 0, dwCurrObjectType, hRegFile, 0, g_pDSKMData, g_dwDSKMDataSize );
				}
				break;
			}
			if ( DSKM_OK(dwResult) ) {
				dwResult = DSKM_ERR_LIB_CODE_HACKED;
				// Проверить хакнутость нижнего кода проверки подписи
				// Функция должна вернуть DSKM_ERR_SIGN_NOT_FOUND
				if ( DSKM_CheckObjectByBuffer(hDSKM, 0, pCurrRegFileName, dwCurrRegFileNameSize, 0, 0, 0) == DSKM_ERR_SIGN_NOT_FOUND )
					dwResult = DSKM_ERR_OK;
			}
			DSKMLiberator( pCurrRegFileName );
		}
		
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}
	return dwResult;
}





//! \fn				: DSKM_CheckRegsChainByType
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hAssocData
//! \param          : HDATA hKeysData
static AVP_dword 	DSKMAPI DSKM_CheckRegsChainByType( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hAssocData, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;
	HDATA hCurrData = DATA_Get_First( hAssocData, 0 );
	while ( hCurrData ) {
		// Искать узел с заданным типом объекта.
		AVP_dword dwCurrObjectType = 0;
		DATA_Get_Val( hCurrData, 0, 0, &dwCurrObjectType, sizeof(dwCurrObjectType) );
		if ( dwCurrObjectType == dwObjectsType ) {
			// Найден
			// Взять его KeyFile
			AVP_char *pKeyFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, DSKM_ASSOC_KEY_FILE_ID), 0 );
			if ( pKeyFileName ) {
				HDSKMFILE hKeyFile = DSKM_CreateHFile( hDSKM, pKeyFileName );
				if ( hKeyFile ) {
					// Проверить KeyFile по цепи вверх
					dwResult = DSKM_CheckRegsChainByFile( hDSKM, hAssocData, hKeyFile );
					if ( DSKM_OK(dwResult) ) {
						dwResult = DSKM_FindKeysTreeInFile( hDSKM, hObjectsList, dwObjectsType, hKeyFile, hKeysData );
					}
					DSKM_CloseHFile( hDSKM, hKeyFile );
				}
				DSKMLiberator( pKeyFileName );
			}
			else {
				// Выдать мастер-ключ
				dwResult = DSKM_FindKeysTreeInBuffer( hDSKM, 0, dwObjectsType, g_pDSKMData, g_dwDSKMDataSize, hKeysData );
			}
		}
		
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}
	return dwResult;
}



//! \fn				: DSKM_CheckObjectsUsingHashRegByAssoc
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hAssocData
static AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegByAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hAssocData ) {
	AVP_bool bErrProcessed = 0;
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	if ( hAssocData ) {
		dwResult = DSKM_ERR_REG_NOT_FOUND;
		switch ( dwObjectsType ) {
			case DSKM_OTYPE_KEY_KL_PUBLIC : 
			case DSKM_OTYPE_KEY_OPERABLE_PUBLIC : 
			case DSKM_OTYPE_LICENCE_KEY :
			case DSKM_OTYPE_LICENCE_KEY_ORDER :
				break;
			default : {
				// Заданный тип объектов - кэшируемые
				// Копировать дерево объектов - будем удалять найденные объекты 
				HDATA hObjects = DSKM_CopyObjectsTree( (HDATA)hObjectsList );
				DSKM_FTABLE_ADD(hDSKM);
				if ( hObjects ) {
					HDSKMFILE hRegFile = DSKM_CreateHFile( hDSKM, 0 );
					HDSKMFILE hKeyFile = DSKM_CreateHFile( hDSKM, 0 );
					if ( hRegFile && hKeyFile ) {
						AVP_bool bBaseChecked = 0;
						// Сканировать файл ассоциаций
						HDATA hCurrData = DATA_Get_First( hAssocData, 0 );
						while ( hCurrData ) {
							// На каждом узле - взять тип объекта
							AVP_dword dwCurrObjectType = 0;
							DATA_Get_Val( hCurrData, 0, 0, &dwCurrObjectType, sizeof(dwCurrObjectType) );
							if ( dwCurrObjectType == dwObjectsType ) {
								// На каждом узле - взять имя файла реестра
								AVP_char *pCurrRegFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, DSKM_ASSOC_SIGN_FILE_ID), 0 );
								if ( pCurrRegFileName ) {
									// Взять его KeyFile
									AVP_char *pKeyFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, DSKM_ASSOC_KEY_FILE_ID), 0 );
									if ( pKeyFileName ) {
										// Проверить KeyFile по цепи вверх
										DSKM_ReuseHFile( hDSKM, hKeyFile, pKeyFileName );
										dwResult = DSKM_CheckRegsChainByFile( hDSKM, hAssocData, hKeyFile );
										if ( DSKM_OK(dwResult) ) {
											// Приворить реестр его ключевым файлом
											DSKM_ReuseHFile( hDSKM, hRegFile, pCurrRegFileName );
											dwResult = DSKM_CheckRegFile( hDSKM, hObjectsList, dwObjectsType, hRegFile, hKeyFile, 0, 0 );
											if ( DSKM_OK(dwResult) ) {
												if ( hRegFile->m_hFile == INVALID_HANDLE_VALUE ) {
													DSKM_Io *pIo = DSKM_GetIO( hDSKM );
													dwResult = DSKM_ERR_IO_NOT_INITIALIZED;
													if ( pIo ) {
														hRegFile->m_hFile = DSKMCreateFile( pIo, pCurrRegFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
													}
												}
												if ( hRegFile->m_hFile != INVALID_HANDLE_VALUE ) {
													AVP_REG_Header rcKLHeader;
													dwResult = DSKM_ReadRegHeaderFromFile( hDSKM, hRegFile->m_hFile, dwObjectsType, &rcKLHeader );
													if ( DSKM_OK(dwResult) ) {
														if ( bBaseChecked && !RHD_ISSPECIAL(rcKLHeader.uiObjType) )
															dwResult = DSKM_ERR_ANOTHER_BASE_REG_FOUND;
														else
															bBaseChecked = !RHD_ISSPECIAL(rcKLHeader.uiObjType);
													}
												}
												if ( DSKM_OK(dwResult) ) 
													// Проверить объекты по этому реестру
													dwResult = DSKM_CheckObjectsByHashRegFile( hDSKM, hObjects, dwObjectsType, hRegFile, 0 );
											}
										}
										DSKMLiberator( pKeyFileName );
									}
									DSKMLiberator( pCurrRegFileName );
								}
							}
							
							if ( !DATA_Get_First(hObjects, 0) )
								// Все объекты найдены
								break;
							hCurrData = DATA_Get_Next( hCurrData, 0 );
							if ( !hCurrData ) {
								dwResult = DSKM_ERR_NOT_ALL_OBJECTS_FOUND; 
								break;
							}
						}
						DSKM_CloseHFile( hDSKM, hKeyFile );
						DSKM_CloseHFile( hDSKM, hRegFile );
						bErrProcessed = 1;
						DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
					}
					DATA_Remove( hObjects, 0 );
				}
			}
		}
	}
	if ( DSKM_NOT_OK(dwResult) && !bErrProcessed )
		DSKM_ProcessObjectsError( (HDATA)hObjectsList, 0, dwResult );
	return dwResult;
}


//! \fn				: DSKM_CheckObjectsUsingInsideSignByAssoc
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hAssocData
static AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignByAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hAssocData ) {
	if ( hAssocData ) {
		DSKM_FTABLE_ADD(hDSKM);
		{
			// Проверить цепочку ключевых файлов вверх и найти нужный ключ
			HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			AVP_dword dwResult = DSKM_CheckRegsChainByType( hDSKM, hObjectsList, dwObjectsType, hAssocData, hKeysRootData );
			if ( DSKM_OK(dwResult) ) {
				// Проверить подпись реестра найденным ключом
				dwResult = DSKM_CheckObjectList( hDSKM, hObjectsList, hKeysRootData );
			}
			DATA_Remove( hKeysRootData, 0 );
			return dwResult;
		}
	}
	return DSKM_ERR_INVALID_PARAMETER;
}

//! \fn				: DSKM_CheckObjectsUsingHashRegsAssoc
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegsAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;
			
			HDATA hAssocData = DSKM_PrepareAssocData( hObjectsList );
			if ( hAssocData ) {
				dwResult = DSKM_ERR_INVALID_OBJTYPE;
				dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, 0 );
				if ( dwObjectsType ) {
					dwResult = DSKM_CheckObjectsUsingHashRegByAssoc( hDSKM, hObjectsList, dwObjectsType, hAssocData );
				}
				DATA_Remove( hAssocData, 0 );
			}
			return dwResult;
		}
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_CheckObjectsUsingInsideSignAndKeysAssoc
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignAndKeysAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList ) {
			AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;
			
			HDATA hAssocData = DSKM_PrepareAssocData( hObjectsList );
			if ( hAssocData ) {
				dwResult = DSKM_CheckObjectsUsingInsideSignByAssoc( hDSKM, hObjectsList, dwObjectsType, hAssocData );
				DATA_Remove( hAssocData, 0 );
			}
			return dwResult;
		}
		
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}
#endif // _WIN32

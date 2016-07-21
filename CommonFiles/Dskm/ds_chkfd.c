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


#include "dskm.h"
#include "dskmdefs.h"
#include "../_AVPIO.h"
#include "dskmi.h"
#include "cl_util.h"




//! \fn				: DSKMAPI DSKM_FindRegFile
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pRegFilesFolder
//! \param          : AVP_dword dwObjectsType
//! \param          : HANDLE *phFindData
//! \param          : HDSKMFILE hRegFile
//! \param          : AVP_bool *pbCheckSpecial
static AVP_dword DSKMAPI DSKM_FindRegFile( HDSKM hDSKM, const AVP_char* pRegFilesFolder, AVP_dword dwObjectsType, HANDLE *phFindData, HDSKMFILE hRegFile, AVP_bool *pbCheckSpecial ) {
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		AVP_bool bWork = 0;
		HANDLE   hFindFile;
		AVP_char pFileName[MAX_PATH<<2];

		if ( !phFindData )
			bWork = !!(hFindFile = DSKMFindFirstFile(pIo, pRegFilesFolder, pFileName, sizeof(pFileName)));
		else 
			if ( !*phFindData )
				bWork = !!(*phFindData = DSKMFindFirstFile(pIo, pRegFilesFolder, pFileName, sizeof(pFileName)));
			else
				bWork = DSKMFindNextFile( pIo, *phFindData, pFileName, sizeof(pFileName) );

		if ( bWork ) {
			AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;
			do {
				DSKM_ReuseHFile( hDSKM, hRegFile, pFileName );
				
				hRegFile->m_hFile = DSKMCreateFile( pIo, hRegFile->m_pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
				if ( hRegFile->m_hFile != INVALID_HANDLE_VALUE ) {
					AVP_REG_Header rcKLHeader;
					dwResult = DSKM_ReadRegHeaderFromFile( hDSKM, hRegFile->m_hFile, dwObjectsType, &rcKLHeader );
					if ( pbCheckSpecial && DSKM_OK(dwResult) ) {
						if ( *pbCheckSpecial && !RHD_ISSPECIAL(rcKLHeader.uiObjType) )
							dwResult = DSKM_ERR_ANOTHER_BASE_REG_FOUND;
						else
							*pbCheckSpecial = !RHD_ISSPECIAL(rcKLHeader.uiObjType);
					}
					if ( DSKM_OK(dwResult) ) {
						dwResult = DSKM_ERR_LIB_CODE_HACKED;
						// Проверить хакнутость нижнего кода проверки подписи
						// Функция должна вернуть DSKM_ERR_SIGN_NOT_FOUND
						if ( DSKM_CheckObjectByBuffer(hDSKM, 0, &rcKLHeader, sizeof(AVP_REG_Header), 0, 0, 0) == DSKM_ERR_SIGN_NOT_FOUND )
							dwResult = DSKM_ERR_OK;
						break;
					}
				}
			} while ( DSKMFindNextFile(pIo, (!phFindData ? hFindFile : *phFindData), pFileName, sizeof(pFileName)) );
			
			if ( !phFindData )
				DSKMFindClose( pIo, hFindFile);

			if ( DSKM_NOT_OK(dwResult) ) 
				DSKM_ReuseHFile( hDSKM, hRegFile, 0 );

			return dwResult;
		}
		return DSKM_ERR_REG_NOT_FOUND;
	}
	return DSKM_ERR_IO_NOT_INITIALIZED;
}





//! \fn				: DSKMAPI DSKM_FindKeyForObjectsType
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pRegFilesFolder
//! \param          : HDATA hKeysData
static AVP_dword DSKMAPI DSKM_FindKeyForObjectsType( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pRegFilesFolder, HDATA hKeysData ) {
	if ( pRegFilesFolder ) {
		AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;
		HDSKMFILE hLKKeyRegFile = DSKM_CreateHFile( hDSKM, 0 );
		if ( hLKKeyRegFile ) {
			DSKM_FTABLE_ADD(hDSKM);
			switch ( dwObjectsType ) {
				case DSKM_OTYPE_KEY_KL_PUBLIC : {
					// Заданный объект - реестр ЛК
					dwResult = DSKM_FindKeysTreeInBuffer( hDSKM, 0, DSKM_OTYPE_KEY_KL_PUBLIC, g_pDSKMData, g_dwDSKMDataSize, hKeysData );
					break;
				}
				case DSKM_OTYPE_KEY_OPERABLE_PUBLIC : {
					// Заданный объект - реестр операбельных ключей  
					// Найти реестр ключа ЛК 
					dwResult = DSKM_FindRegFile( hDSKM, pRegFilesFolder, DSKM_OTYPE_KEY_KL_PUBLIC, 0, hLKKeyRegFile, 0 );
					if ( DSKM_OK(dwResult) ) {
						// Проверить подпись реестра ключа ЛК	мастер-ключом
						dwResult = DSKM_CheckRegFile( hDSKM, 0, DSKM_OTYPE_KEY_KL_PUBLIC, hLKKeyRegFile, 0, g_pDSKMData, g_dwDSKMDataSize );
						if ( DSKM_OK(dwResult) ) {
							// Найти в реестре ключа ЛК сам ключ
							dwResult = DSKM_FindKeysTreeInFile( hDSKM, 0, DSKM_OTYPE_KEY_OPERABLE_PUBLIC, hLKKeyRegFile, hKeysData );
						}
					}
					break;
				}
				default : {
					// Найти реестр ключа ЛК 
					dwResult = DSKM_FindRegFile( hDSKM, pRegFilesFolder, DSKM_OTYPE_KEY_KL_PUBLIC, 0, hLKKeyRegFile, 0 );
					if ( DSKM_OK(dwResult) ) {
						// Проверить подпись реестра ключа ЛК	мастер-ключом
						dwResult = DSKM_CheckRegFile( hDSKM, 0, DSKM_OTYPE_KEY_KL_PUBLIC, hLKKeyRegFile, 0, g_pDSKMData, g_dwDSKMDataSize );
						if ( DSKM_OK(dwResult) ) {
							// Найти реестр операбельных ключей для заданных объектов
							HDSKMFILE hOperKeyRegFile = DSKM_CreateHFile( hDSKM, 0 );
							dwResult = DSKM_ERR_REG_NOT_FOUND;
							if ( hOperKeyRegFile ) {
								HANDLE hFindData = 0;
								dwResult = DSKM_FindRegFile( hDSKM, pRegFilesFolder, DSKM_OTYPE_KEY_OPERABLE_PUBLIC, &hFindData, hOperKeyRegFile, 0 );
								if ( DSKM_OK(dwResult) ) {
									while ( DSKM_OK(dwResult) ) {
										// Проверить подпись реестра операбельных ключей ключом ЛК
										dwResult = DSKM_CheckRegFile( hDSKM, 0, DSKM_OTYPE_KEY_OPERABLE_PUBLIC, hOperKeyRegFile, hLKKeyRegFile, 0, 0 );
										if ( DSKM_OK(dwResult) ) {
											// Найти в реестре операбельных ключей ключи для ВСЕХ типов объектов, подходящие под вектор параметров
											dwResult = DSKM_FindKeysTreeInFile( hDSKM, hObjectsList, 0, hOperKeyRegFile, hKeysData );
											if ( DSKM_OK(dwResult) ) 
												dwResult = DSKM_PrepareTypeAssocByKeys( hDSKM, hObjectsList, hKeysData);
										}
										dwResult = DSKM_FindRegFile( hDSKM, pRegFilesFolder, DSKM_OTYPE_KEY_OPERABLE_PUBLIC, &hFindData, hOperKeyRegFile, 0 );
									}

									dwResult = DSKM_ERR_OK;

									{
										DSKM_Io *pIo = DSKM_GetIO( hDSKM );
										if ( pIo ) {
											DSKMFindClose( pIo, hFindData );
										}
									}
								}
								DSKM_CloseHFile( hDSKM, hOperKeyRegFile );
							}
						}
					}
					break;
				}
			}
			DSKM_CloseHFile( hDSKM, hLKKeyRegFile );
			return dwResult;
		}
	}
	
	return DSKM_ERR_INVALID_PARAMETER;
}


//! \fn				: DSKM_CheckObjectsUsingHashRegByFolder
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pRegFilesFolder
static AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegByFolder( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pRegFilesFolder ) {
	AVP_bool bErrProcessed = 0;
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	if ( pRegFilesFolder ) {
			dwResult = DSKM_ERR_REG_NOT_FOUND;
			switch ( dwObjectsType ) {
				case DSKM_OTYPE_KEY_KL_PUBLIC : 
				case DSKM_OTYPE_KEY_OPERABLE_PUBLIC : 
				case DSKM_OTYPE_LICENCE_KEY :
				case DSKM_OTYPE_LICENCE_KEY_ORDER :
					break;
				default : {
					// Найти все операбельные ключи для ВСЕХ типов объектов, подходящие под вектор параметров
					HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
					dwResult = DSKM_FindKeyForObjectsType( hDSKM, hObjectsList, 0, pRegFilesFolder, hKeysRootData );
					if ( DSKM_OK(dwResult) ) {
						// Копировать дерево объектов - будем удалять найденные объекты 
						HDATA hObjects = DSKM_CopyObjectsTree( (HDATA)hObjectsList );
						if ( hObjects ) {
							HDATA hCollectionData = DATA_Add( 0, 0, DSKM_OBJ_TYPES_PROP_ID, 0, 0 );
							HPROP hCollectionProp = DATA_Find_Prop( hCollectionData, 0, 0 );
								
							dwResult = DSKM_ERR_INVALID_OBJTYPE;
							dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, hCollectionData );
							if ( dwObjectsType && PROP_Arr_Count(hCollectionProp) > 0 ) {
								HDSKMFILE hRegFile = DSKM_CreateHFile( hDSKM, 0 );
								if ( hRegFile ) {
									// Найти реестр хэшей объектов
									AVP_bool bBaseChecked = 0;
									HANDLE hFindData = 0;
									dwResult = DSKM_FindRegFile( hDSKM, pRegFilesFolder, dwObjectsType, &hFindData, hRegFile, &bBaseChecked );
									if ( DSKM_OK(dwResult) ) {
										while ( DSKM_OK(dwResult) ) {
											// Проверить подпись реестра хэшей объектов операбельным ключом
											dwResult = DSKM_CheckHFile( hDSKM, hRegFile, hKeysRootData );
											if ( DSKM_OK(dwResult) ) {
												// Проверить объекты по этому реестру
												dwResult = DSKM_CheckObjectsByHashRegFile( hDSKM, hObjects, dwObjectsType, hRegFile, hCollectionData );
											}
											else
												// Подпись не совпала - считаем, что реестр мы не использовали. Если он был базовый - забудем.
												bBaseChecked = 0;
											if ( DSKM_OK(dwResult) ) {
												if ( !DATA_Get_First(hObjects, 0) )
													// Все объекты найдены
													break;
												if ( DSKM_NOT_OK(DSKM_FindRegFile(hDSKM, pRegFilesFolder, dwObjectsType, &hFindData, hRegFile, &bBaseChecked)) ) {
													// Больше нет реестров заданного типа
													dwResult = DSKM_ERR_NOT_ALL_OBJECTS_FOUND; 
													break;
												}
											}
										}
										{
											DSKM_Io *pIo = DSKM_GetIO( hDSKM );
											if ( pIo ) {
												DSKMFindClose( pIo, hFindData );
											}
											else
												dwResult = DSKM_ERR_IO_NOT_INITIALIZED;
										}
									}
									DSKM_CloseHFile( hDSKM, hRegFile );

									DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
									bErrProcessed = 1;
								}
							}
							DATA_Remove( hCollectionData, 0 );

							if ( DATA_Get_First(hObjects, 0) ) {
								// Проверить оставшиеся объекты на подпись внутри самих
								DSKM_CheckObjectsByInsideSign( hDSKM, hObjects, hKeysRootData );
								DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
								bErrProcessed = 1;
								if ( !DATA_Get_First(hObjects, 0) ) {
									// Все объекты найдены
									dwResult = DSKM_ERR_OK;
								}
							}
							
							DATA_Remove( hObjects, 0 );
						}
					}
					DATA_Remove( hKeysRootData, 0 );
					break;
				}
			}
	}
	
	if ( DSKM_NOT_OK(dwResult) && !bErrProcessed )
		DSKM_ProcessObjectsError( (HDATA)hObjectsList, 0, dwResult );
	return dwResult;
}



//! \fn				: DSKM_CheckObjectsUsingInsideSignByFolder
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pRegFilesFolder
static AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignByFolder( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pRegFilesFolder ) {
	if ( pRegFilesFolder ) {
		HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
		AVP_dword dwResult = DSKM_FindKeyForObjectsType( hDSKM, hObjectsList, dwObjectsType, pRegFilesFolder, hKeysRootData );
		if ( DSKM_OK(dwResult) ) {
			dwResult = DSKM_CheckObjectList( hDSKM, hObjectsList, hKeysRootData );
		}
		DATA_Remove( hKeysRootData, 0 );
		return dwResult;
	}
	
	return DSKM_ERR_INVALID_PARAMETER;
}



//! \fn				: DSKM_CheckObjectsUsingHashRegsFolder
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pRegFilesFolder
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegsFolder( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pRegFilesFolder ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			AVP_dword dwResult = DSKM_CheckObjectsUsingHashRegByFolder( hDSKM, hObjectsList, dwObjectsType, pRegFilesFolder );
			return dwResult;
		}
		
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_CheckObjectsUsingInsideSignAndKeysFolder
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pRegFilesFolder
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignAndKeysFolder( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pRegFilesFolder ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			AVP_dword dwResult = DSKM_CheckObjectsUsingInsideSignByFolder( hDSKM, hObjectsList, dwObjectsType, pRegFilesFolder );
			return dwResult;
		}
		
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}

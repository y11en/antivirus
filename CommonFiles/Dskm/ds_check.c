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
#include "dskmi.h"
#include "cl_util.h"




//! \fn				: DSKM_CheckObjectsUsingHashReg
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDSKMFILE hRegFile
static AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDSKMFILE hRegFile ) {
	AVP_bool bErrProcessed = 0;
	AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;
	switch ( dwObjectsType ) {
		case DSKM_OTYPE_KEY_KL_PUBLIC : 
		case DSKM_OTYPE_KEY_OPERABLE_PUBLIC : 
		case DSKM_OTYPE_LICENCE_KEY :
		case DSKM_OTYPE_LICENCE_KEY_ORDER :
			break;
		default : {
			// Копировать дерево объектов - будем удалять найденные объекты 
			HDATA hObjects = DSKM_CopyObjectsTree( (HDATA)hObjectsList );
			if ( hObjects ) {
				HDATA hCollectionData = DATA_Add( 0, 0, DSKM_OBJ_TYPES_PROP_ID, 0, 0 );
				dwResult = DSKM_ERR_INVALID_OBJTYPE;
				dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, hCollectionData );
				if ( dwObjectsType ) {
					// Проверить объекты по заданному реестру
					dwResult = DSKM_CheckObjectsByHashRegFile( hDSKM, hObjects, dwObjectsType, hRegFile, hCollectionData );
					if ( DSKM_OK(dwResult) && DATA_Get_First(hObjects, 0) ) {
						// Не все объекты найдены
						dwResult = DSKM_ERR_NOT_ALL_OBJECTS_FOUND; 
					}
					DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
					DATA_Remove( hObjects, 0 );
					bErrProcessed = 1;
				}
				DATA_Remove( hCollectionData, 0 );
			}
			break;
		}
	}
	if ( DSKM_NOT_OK(dwResult) && !bErrProcessed )
		DSKM_ProcessObjectsError( (HDATA)hObjectsList, 0, dwResult );
	return dwResult;
}


//! \fn				: DSKM_CheckObjectsUsingHashRegFile
//! \brief			:	Проверить объекты через переданный реестр хэшей
//                Подпись реестра должна быть уже проверена (через DSKM_CheckObjectsUsingInsideSign*)
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки и параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszRegFileName	- файл реестра
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pRegFileName ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			AVP_dword dwResult = DSKM_UNKNOWN_ERROR;
			HDSKMFILE hRegFile = DSKM_CreateHFile( hDSKM, pRegFileName );
			DSKM_FTABLE_ADD(hDSKM);
			if ( hRegFile ) {
				dwResult = DSKM_CheckObjectsUsingHashReg( hDSKM, hObjectsList, dwObjectsType, hRegFile );

				DSKM_CloseHFile( hDSKM, hRegFile );
			}

			return dwResult;
		}
		
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_CheckObjectsUsingInsideSignAndKeyReg
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pszKeyRegFileName
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignAndKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszKeyRegFileName ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList ) {
			AVP_dword dwResult = DSKM_UNKNOWN_ERROR;
			HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			if ( hKeysRootData ) {
				switch ( dwObjectsType ) {
					case DSKM_OTYPE_KEY_KL_PUBLIC : {
						// Заданный объект - реестр ЛК
						dwResult = DSKM_FindKeysTreeInBuffer( hDSKM, hObjectsList, dwObjectsType, g_pDSKMData, g_dwDSKMDataSize, hKeysRootData );
						break;
					}
					default : {
						HDSKMFILE hRegFile = DSKM_CreateHFile( hDSKM, pszKeyRegFileName );
						if ( hRegFile ) {
							dwResult = DSKM_FindKeysTreeInFile( hDSKM, hObjectsList, dwObjectsType, hRegFile, hKeysRootData );
							DSKM_CloseHFile( hDSKM, hRegFile );
						}
						break;
					}
				}
				if ( DSKM_OK(dwResult) ) {
					dwResult = DSKM_CheckObjectList( hDSKM, hObjectsList, hKeysRootData );
				}
				DATA_Remove( hKeysRootData, 0 );
			}
			return dwResult;
		}
		
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}

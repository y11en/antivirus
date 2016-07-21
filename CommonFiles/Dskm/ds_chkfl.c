/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_chkfl.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/28/2005 11:41:42 AM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"



//! \fn				: DSKM_CheckObjectByInsideSign
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObject
//! \param          : HDATA hKeysData
AVP_dword  	DSKMAPI DSKM_CheckObjectByInsideSign( HDSKM hDSKM, HDATA hObject, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_CheckObject( hDSKM, hObject, hKeysData );
	if ( DSKM_OK(dwResult) ) {
		DATA_Remove( hObject, 0 );
	}
	else {
		// TT #30734
		//if ( dwResult != DSKM_ERR_SIGN_NOT_FOUND ) 
			DATA_Set_Val( hObject, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
	}
	return dwResult;
}

//! \fn				: DSKM_CheckObjectsByInsideSign
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObjects
//! \param          : HDATA hKeysData
AVP_dword  	DSKMAPI DSKM_CheckObjectsByInsideSign( HDSKM hDSKM, HDATA hObjects, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_DATA_PROCESSING;
	HDATA hCurrObjectData = DATA_Get_First( hObjects, 0 );
	while ( hCurrObjectData ) {
		HDATA hWorkObjectData = hCurrObjectData;
		HDATA hListObject = DSKM_FindCorrespondingListObject( hWorkObjectData );
		hCurrObjectData = DATA_Get_Next( hCurrObjectData, 0 );
		dwResult = DSKM_CheckObjectByInsideSign( hDSKM, hWorkObjectData, hKeysData );
		if ( DSKM_OK(dwResult) ) {
			AVP_dword dwSignedBy = DSKM_OBJ_SIGN_INOBJ;
			DSKM_ParList_SetObjectProp( 0, (HDSKMLISTOBJ)hListObject, DSKM_OBJ_SIGNED_BY, &dwSignedBy, sizeof(dwSignedBy) );
		}
	}
	return dwResult;
}



//! \fn				: DSKM_CheckObjectByHashRegData
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hRegData
//! \param          : HDATA hObject
AVP_dword DSKMAPI DSKM_CheckObjectByHashRegData( HDSKM hDSKM, HDATA hRegData, HDATA hObject ) {
	AVP_dword dwResult = DSKM_ERR_DATA_PROCESSING;
	// Расчитать хэш объекта
	void *pObjectHash = 0;
	AVP_dword dwObjectHashSize = 0;
	
	DATA_Set_Val( hObject, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_HASH_NOT_FOUND, 0 );
	
	dwResult = DSKM_PrepareObjectHash( hDSKM, hObject, &pObjectHash, &dwObjectHashSize );
	if ( DSKM_OK(dwResult) ) {
		// Искать хэш объекта в реестре
		HPROP hHashArrPop = DATA_Find_Prop( hRegData, 0, DSKM_OBJECT_REG_ARR_HASH_ID );
		dwResult = DSKM_ERR_HASH_NOT_FOUND;
		if ( hHashArrPop ) {
			void *pRegHash = DSKMAllocator( dwObjectHashSize );
			if ( pRegHash )	{
				AVP_bool bFound = 0;
				AVP_dword i = 0;
				AVP_dword dwCount = PROP_Arr_Count( hHashArrPop );
				for( ; i<dwCount && !bFound; i++ ) {
					AVP_dword dwRegHashSize = PROP_Arr_Get_Items( hHashArrPop, i, pRegHash, dwObjectHashSize );
					if ( dwRegHashSize && !ds_mcmp(pRegHash, pObjectHash, dwRegHashSize) ) {
						// Хэш найден
						/* Не будем контролировать - потери времени
						dwResult = DSKM_ERR_LIB_CODE_HACKED;
						DATA_Set_Val( hObject, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_LIB_CODE_HACKED, 0 );
						// Проверить хакнутость нижнего кода проверки подписи
						// Функция должна вернуть DSKM_ERR_SIGN_NOT_FOUND - в буфере нет подписи 
						if ( DSKM_CheckObjectByBuffer(hDSKM, 0, pRegHash, dwRegHashSize, 0, 0, 0) == DSKM_ERR_SIGN_NOT_FOUND ) {
							DATA_Remove( hObject, 0 );
							bFound = 1;
							dwResult = DSKM_ERR_OK;
						}
						*/
						DATA_Remove( hObject, 0 );
						bFound = 1;
						dwResult = DSKM_ERR_OK;
					}
				}
				DSKMLiberator( pRegHash );
			}
		}
		else {
			AVP_bool bFound = 0;
			void *pRegHash = DSKMAllocator( dwObjectHashSize );
			if ( pRegHash )	{
				HDATA hCurrRegData = DATA_Get_First( hRegData, 0 );
				while ( hCurrRegData && !bFound ) {
					AVP_dword dwRegHashSize = DATA_Get_Val( hCurrRegData, 0, 0, pRegHash, dwObjectHashSize );
					if ( dwRegHashSize && !ds_mcmp(pRegHash, pObjectHash, dwRegHashSize) ) {
						// Хэш найден
						/* Не будем контролировать - потери времени
						dwResult = DSKM_ERR_LIB_CODE_HACKED;
						DATA_Set_Val( hObject, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_LIB_CODE_HACKED, 0 );
						// Проверить хакнутость нижнего кода проверки подписи
						// Функция должна вернуть DSKM_ERR_SIGN_NOT_FOUND - в буфере нет подписи 
						if ( DSKM_CheckObjectByBuffer(hDSKM, 0, pRegHash, dwRegHashSize, 0, 0, 0) == DSKM_ERR_SIGN_NOT_FOUND ) {
							DATA_Remove( hObject, 0 );
							bFound = 1;
							dwResult = DSKM_ERR_OK;
						}
						*/
						DATA_Remove( hObject, 0 );
						bFound = 1;
						dwResult = DSKM_ERR_OK;
					}
					hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
				}
				DSKMLiberator( pRegHash );
			}
		}
		DSKMLiberator( pObjectHash );
	}
	else {
		DATA_Set_Val( hObject, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
	}

	return dwResult;
}



//! \fn				: DSKM_CheckObjectsByHashRegData
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObjects
//! \param          : HDATA hRegData
AVP_dword  	DSKMAPI DSKM_CheckObjectsByHashRegData( HDSKM hDSKM, HDATA hObjects, HDATA hRegData ) {
	AVP_dword dwResult = DSKM_ERR_DATA_PROCESSING;
	HDATA hCurrObjectData = DATA_Get_First( hObjects, 0 );
	while ( hCurrObjectData ) {
		HDATA hWorkObjectData = hCurrObjectData;
		AVP_dword dwSignedBy = 0;
		AVP_dword dwSizeofSignedBy = sizeof(dwSignedBy);
		DSKM_ParList_GetObjectProp( (HDSKMLIST)hObjects, (HDSKMLISTOBJ)hWorkObjectData, DSKM_OBJ_SIGNED_BY, &dwSignedBy, &dwSizeofSignedBy );
		hCurrObjectData = DATA_Get_Next( hCurrObjectData, 0 );
		if ( !dwSignedBy ) {
			// Не установлен тип ключа, которым подписан объект
			HDATA hListObject = DSKM_FindCorrespondingListObject( hWorkObjectData );
			dwResult = DSKM_CheckObjectByHashRegData( hDSKM, hRegData, hWorkObjectData );
			if ( DSKM_OK(dwResult) ) {
				AVP_dword dwSignedBy = DSKM_OBJ_SIGN_INREG;
				DSKM_ParList_SetObjectProp( 0, (HDSKMLISTOBJ)hListObject, DSKM_OBJ_SIGNED_BY, &dwSignedBy, sizeof(dwSignedBy) );
			}
		}
	}
	return dwResult;
}



//! \fn				: DSKM_CheckObjectsByHashRegFile
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObjects
//! \param          : AVP_dword dwObjectsType
//! \param          : HDSKMFILE hRegFile
//! \param          : HDATA hTypesCollectionData
AVP_dword  	DSKMAPI DSKM_CheckObjectsByHashRegFile( HDSKM hDSKM, HDATA hObjects, AVP_dword dwObjectsType, HDSKMFILE hRegFile, HDATA hTypesCollectionData ) {
	// Десериализовать реестр хэшей объектов
	HDATA hRegData = 0;
	AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;

	if ( hTypesCollectionData ) {
		HPROP hCollectionProp = DATA_Find_Prop( hTypesCollectionData, 0, 0 );
		int i = 0;
		int c = PROP_Arr_Count( hCollectionProp );
		dwResult = DSKM_ERR_REG_NOT_FOUND;
		for ( ; i<c; i++ ) {
			AVP_dword dwPropVal;
			PROP_Arr_Get_Items( hCollectionProp, i, &dwPropVal, sizeof(dwPropVal) );
			dwResult = DSKM_DeserializeRegHFile( hDSKM, hRegFile, dwPropVal, &hRegData );
			if ( DSKM_OK(dwResult) ) 
				break;
		}
	}
	else
		dwResult = DSKM_DeserializeRegHFile( hDSKM, hRegFile, dwObjectsType, &hRegData );

	if ( DSKM_OK(dwResult) ) {
		dwResult = DSKM_CheckObjectsByHashRegData( hDSKM, hObjects, hRegData );
		DATA_Remove( hRegData, 0 );
	}
	return dwResult; 
}

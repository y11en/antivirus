/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_hash.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/15/2005 2:10:21 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"


//! \fn				: DSKM_HashObjectByName
//! \brief			:	Вычислить хэш файла
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  const AVP_char* pszObjectFileName - имя файла для подсчета
//! \param          : void **ppvObjectHash - результирующий хэш	(распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByName( HDSKM hDSKM, const AVP_char* pObjectFileName, void **ppvObjectHash, AVP_dword *pdwObjectHashSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	if ( hDSKM && ppvObjectHash && pdwObjectHashSize ) {
		HDATA hObjData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
		DSKM_FTABLE_ADD(hObjData);
		*ppvObjectHash = 0;
		dwResult = DSKM_HashObjectByNameImp( hDSKM, pObjectFileName, ppvObjectHash, pdwObjectHashSize, hObjData );
		DATA_Remove( hObjData, 0 );
	}
	return dwResult;
}



//! \fn				: DSKM_HashObjectByBuffer
//! \brief			:	Вычислить хэш буферизованного объекта
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  void *pvBuffer - указатель на буфер
//! \param          : AVP_dword dwBufferSize - размер буфера
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера
//                    Может быть 0 - тогда считается, что буфер уже заполнен
//! \param          : void *pvCallBackParams	- параметры функции подкачки
//! \param          : void **ppvObjectHash - результирующий хэш	(распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByBuffer( HDSKM hDSKM, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, void **ppvObjectHash, AVP_dword *pdwObjectHashSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	if ( hDSKM && ppvObjectHash && pdwObjectHashSize ) {
		HDATA hObjData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
		DSKM_FTABLE_ADD(hObjData);
		*ppvObjectHash = 0;
		dwResult = DSKM_HashObjectByBufferImp( hDSKM, pvBuffer, dwBufferSize, pfnCallBack, pvCallBackParams, ppvObjectHash, pdwObjectHashSize, hObjData );
		DATA_Remove( hObjData, 0 );
	}
	return dwResult;
}

//! \fn				: DSKM_HashObjectByList
//! \brief			:	Вычислить хэш объекта из списка (первого!)
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : void **ppvObjectHash - результирующий хэш	(распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByList( HDSKM hDSKM, HDSKMLIST hObjectsList, void **ppvObjectHash, AVP_dword *pdwObjectHashSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	if ( hDSKM && ppvObjectHash && pdwObjectHashSize ) {
		HDATA hCurrData;

		*ppvObjectHash = 0;

		hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
		dwResult = DSKM_ERR_OK;
		while ( hCurrData && DSKM_OK(dwResult) ) {
			void *pObjectHash = 0;
			AVP_dword dwObjectHashSize = 0;
			if ( DSKM_OK(dwResult = DSKM_PrepareObjectHash(hDSKM, hCurrData, ppvObjectHash, pdwObjectHashSize)) ) {
				break;
			}
			if ( dwResult == DSKM_ERR_INVALID_OBJTYPE || dwResult == DSKM_ERR_HASH_NOT_FOUND )
				dwResult = DSKM_ERR_OK;

			hCurrData = DATA_Get_Next( hCurrData, 0 );
		}
	}
	return dwResult;
}



//! \fn				: DSKM_HashObjectByNameUsingGivenHashBuffer
//! \brief			:	Вычислить хэш файла
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  const AVP_char* pszObjectFileName - имя файла для подсчета
//! \param          : void *pvObjectHash - буфер под результирующий хэш
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByNameUsingGivenHashBuffer( HDSKM hDSKM, const AVP_char* pszObjectFileName, void *pvObjectHash, AVP_dword *pdwObjectHashSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	if ( hDSKM && pvObjectHash && pdwObjectHashSize && *pdwObjectHashSize >= HASH_SIZE ) {
		HDATA hObjData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
		DSKM_FTABLE_ADD(hObjData);
		dwResult = DSKM_HashObjectByNameImp( hDSKM, pszObjectFileName, &pvObjectHash, pdwObjectHashSize, hObjData );
		DATA_Remove( hObjData, 0 );
	}
	else {
		if ( pdwObjectHashSize) {
			*pdwObjectHashSize = HASH_SIZE;
			dwResult = DSKM_ERR_OK;
		}
	}
	return dwResult;
}



//! \fn				: DSKM_HashObjectByBufferGivenHashBuffer
//! \brief			:	Вычислить хэш буферизованного объекта
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  void *pvBuffer - указатель на буфер
//! \param          : AVP_dword dwBufferSize - размер буфера
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера
//                    Может быть 0 - тогда считается, что буфер уже заполнен
//! \param          : void *pvCallBackParams	- параметры функции подкачки
//! \param          : void *ppvObjectHash - буфер под результирующий хэш
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByBufferGivenHashBuffer( HDSKM hDSKM, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, void *pvObjectHash, AVP_dword *pdwObjectHashSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	if ( hDSKM && pvObjectHash && pdwObjectHashSize && *pdwObjectHashSize >= HASH_SIZE ) {
		HDATA hObjData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
		DSKM_FTABLE_ADD(hObjData);
		dwResult = DSKM_HashObjectByBufferImp( hDSKM, pvBuffer, dwBufferSize, pfnCallBack, pvCallBackParams, &pvObjectHash, pdwObjectHashSize, hObjData );
		DATA_Remove( hObjData, 0 );
	}
	else {
		if ( pdwObjectHashSize) {
			*pdwObjectHashSize = HASH_SIZE;
			dwResult = DSKM_ERR_OK;
		}
	}
	return dwResult;
}





//! \fn				: DSKM_HashAdditionObjectParams
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObjectData
//! \param          : void *pvObjectHash
//! \param          : AVP_dword dwObjectHashSize
AVP_dword DSKMAPI DSKM_HashAdditionObjectParams( HDSKM hDSKM, HDATA hObjectData, void *pvObjectHash, AVP_dword dwObjectHashSize ) {
	AVP_dword dwResult = DSKM_ERR_OK;
		
	HPROP hCurrProp = DATA_Get_First_Prop( hObjectData, 0 );
	while ( hCurrProp ) {
		AVP_dword dwCurrPID = PROP_Get_Id( hCurrProp );
		switch ( GET_AVP_PID_APP(dwCurrPID) ) {
			case DSKM_APP_OBJECT_HASHED_PROP : {
				AVP_dword dwPropSize = PROP_Get_Val( hCurrProp, 0, 0 );
				if ( dwPropSize ) {
					void *pvPropBuffer = DSKMAllocator( dwPropSize );
					if ( pvPropBuffer ) {
						PROP_Get_Val( hCurrProp, pvPropBuffer, dwPropSize );
						dwResult = DSKM_HashBufferToHash( hDSKM, pvPropBuffer, dwPropSize, pvObjectHash, dwObjectHashSize );
						DSKMLiberator( pvPropBuffer );
					}
				}
				break;
			}
		}
		hCurrProp = PROP_Get_Next( hCurrProp );
	}

	return dwResult;
}


//! \fn				: DSKM_PrepareObjectHash
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObjectData
//! \param          : void **ppvObjectHash
//! \param          : AVP_dword *pdwObjectHashSize
AVP_dword DSKMAPI DSKM_PrepareObjectHash( HDSKM hDSKM, HDATA hObjectData, void **ppvObjectHash, AVP_dword *pdwObjectHashSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_HASH_NOT_FOUND;

		HPROP hHashProp = DATA_Find_Prop( hObjectData, 0, DSKM_OBJECT_REG_HASH_ID(1) );
		if ( hHashProp ) {
			*pdwObjectHashSize = PROP_Get_Val( hHashProp, 0, 0 );
			if ( *pdwObjectHashSize ) {
				*ppvObjectHash = DSKMAllocator( *pdwObjectHashSize );
				if ( *ppvObjectHash ) {
					PROP_Get_Val( hHashProp, *ppvObjectHash, *pdwObjectHashSize );
					dwResult = DSKM_ERR_OK;
				}
			}
		}

		if ( DSKM_NOT_OK(dwResult) ) {
			AVP_dword dwPID;

			*pdwObjectHashSize = 0;

			dwPID = DATA_Get_Id( hObjectData, 0 );
			dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
			switch ( dwPID ) {
				case DSKM_OBJECT_NAME_ID(0) : 
				case DSKM_REG_NAME_ID(0) : {
					AVP_char *pObjectFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hObjectData, 0, 0), 0 );
					if ( pObjectFileName ) {
						DSKM_FTABLE_ADD(hObjectData);
						dwResult = DSKM_HashObjectByNameImp( hDSKM, pObjectFileName, ppvObjectHash, pdwObjectHashSize, hObjectData );
						DSKM_FTABLE_REMOVE(hObjectData);
						DSKMLiberator( pObjectFileName );
					}
					break;
				}
				case DSKM_OBJECT_BUFF_PTR_ID(0) : 
				case DSKM_REG_BUFF_PTR_ID(0) : {
					void *pBuffer;
					AVP_dword dwBufferSize;
					pfnDSKM_GetBuffer_CallBack pCallBack;
					void *pCallBackParams;
					
					DATA_Get_Val( hObjectData, 0, 0, &pBuffer, sizeof(pBuffer) );
					DATA_Get_Val( hObjectData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
					DATA_Get_Val( hObjectData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
					DATA_Get_Val( hObjectData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
					
					DSKM_FTABLE_ADD(hObjectData);
					dwResult = DSKM_HashObjectByBufferImp( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, ppvObjectHash, pdwObjectHashSize, hObjectData );
					DSKM_FTABLE_REMOVE(hObjectData);
					break;
				}
				case DSKM_OBJECT_HASH_ID(0) : {
					*pdwObjectHashSize = DATA_Get_Val( hObjectData, 0, 0, 0, 0 );
					if ( *pdwObjectHashSize ) {
						*ppvObjectHash = DSKMAllocator( *pdwObjectHashSize );
						if ( *ppvObjectHash ) {
							DATA_Get_Val( hObjectData, 0, 0, *ppvObjectHash, *pdwObjectHashSize );
							dwResult = DSKM_ERR_OK;
						}
					}
					break;
				}
			}
			if ( DSKM_OK(dwResult) ) {
				dwResult = DSKM_HashAdditionObjectParams( hDSKM, hObjectData, *ppvObjectHash, *pdwObjectHashSize );
				DATA_Add_Prop( hObjectData, 0, DSKM_OBJECT_REG_HASH_ID(1), (AVP_size_t)*ppvObjectHash, *pdwObjectHashSize );
			}
		}
		return dwResult;
	}
	DSKM_EXCEPT
}


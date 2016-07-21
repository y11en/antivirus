/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_erro.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/29/2005 6:40:15 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"


//! \fn				:  DSKMAPI DSKM_ParList_GetFirstObjectError
//! \brief			:	Получить первый объект с ошибкой обработки
//! \return			: Идентификатор объекта   
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : AVP_dword *pdwErrorCode	- указатель на код ошибки. Может быть 0.
AVP_dword   DSKMAPI DSKM_ParList_GetFirstObjectError( HDSKMLIST hList, AVP_dword *pdwErrorCode ) {
	DSKM_TRY {
		if ( hList ) {
			HDATA hCurrListData = DATA_Get_First( (HDATA)hList, 0 );
			while ( hCurrListData ) {
				if ( DSKM_OK(DSKM_IsObject(hCurrListData)) ) {
					HPROP hErrProp = DATA_Find_Prop( hCurrListData, 0, DSKM_OBJECT_ERROR_ID );
					if ( hErrProp ) {
						AVP_dword dwError;
						PROP_Get_Val( hErrProp, &dwError, sizeof(AVP_dword) );
						if ( DSKM_NOT_OK(dwError) ) {
							if ( pdwErrorCode )
								PROP_Get_Val( hErrProp, pdwErrorCode, sizeof(AVP_dword) );
							return GET_AVP_PID_ID(DATA_Get_Id(hCurrListData, 0));
						}
					}
				}
				
				hCurrListData = DATA_Get_Next( hCurrListData, 0 );
			}
		}
		return 0;
	}
	DSKM_EXCEPT_SIZE
}


//! \fn				:  DSKMAPI DSKM_ParList_GetNextObjectError
//! \brief			:	Получить следующий объект с ошибкой обработки
//! \return			: Идентификатор объекта или 0, если объектов больше нет.  
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : AVP_dword dwObjectId - идентификатор предыдущего объекта
//! \param          : AVP_dword *pdwErrorCode	- указатель на код ошибки. Может быть 0.
AVP_dword   DSKMAPI DSKM_ParList_GetNextObjectError( HDSKMLIST hList, AVP_dword dwObjectId, AVP_dword *pdwErrorCode ) {
	DSKM_TRY {
		if ( hList && dwObjectId ) {
			HDATA hCurrListData = DATA_Get_First( (HDATA)hList, 0 );
			while ( hCurrListData ) {
				if ( DSKM_OK(DSKM_IsObject(hCurrListData)) ) {
					AVP_dword dwPID = DATA_Get_Id( hCurrListData, 0 );
					if ( dwObjectId == GET_AVP_PID_ID(dwPID) ) {
						HDATA hNextListData = DATA_Get_Next( hCurrListData, 0 );
						while ( hNextListData ) {
							HPROP hErrProp = DATA_Find_Prop( hNextListData, 0, DSKM_OBJECT_ERROR_ID );
							if ( hErrProp ) {
								PROP_Get_Val( hErrProp, &dwPID, sizeof(AVP_dword) );
								if ( DSKM_NOT_OK(dwPID) ) {
									if ( pdwErrorCode )
										PROP_Get_Val( hErrProp, pdwErrorCode, sizeof(AVP_dword) );
									return GET_AVP_PID_ID(DATA_Get_Id(hNextListData, 0));
								}
							}
							hNextListData = DATA_Get_Next( hNextListData, 0 );
						}
					}
				}
				hCurrListData = DATA_Get_Next( hCurrListData, 0 );
			}
		}
		return 0;
	}
	DSKM_EXCEPT_SIZE
}


//! \fn				:  DSKMAPI DSKM_ParList_GetObject
//! \brief			:	Выдать информацию об объекте
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : AVP_dword dwObjectId - идентификатор объекта
//! \param          : void *pvObject	- указатель на буфер результата (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwObjectSize - указатель на размер буфера результата
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetObject( HDSKMLIST hList, AVP_dword dwObjectId, void *pvObject, AVP_dword *pdwObjectSize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( hList && dwObjectId ) {
				HDATA hCurrListData = DATA_Get_First( (HDATA)hList, 0 );
				dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
				while ( hCurrListData ) {
					if ( DSKM_OK(DSKM_IsObject(hCurrListData)) ) {
						AVP_dword dwPID = DATA_Get_Id( hCurrListData, 0 );
						if ( dwObjectId == GET_AVP_PID_ID(dwPID) ) {
							if ( pdwObjectSize ) {
								AVP_dword dwSize = DATA_Get_Val( hCurrListData, 0, 0, 0, 0 );
								if ( pvObject == 0 ) {
									*pdwObjectSize = dwSize;
									hListObj = (HDSKMLISTOBJ)hCurrListData;
									dwResult = DSKM_ERR_OK;
								}
								else {
									if ( *pdwObjectSize < dwSize ) {
										*pdwObjectSize = dwSize;
										dwResult = DSKM_ERR_INVALID_SIZE;
									}
									else {
										DATA_Get_Val( hCurrListData, 0, 0, pvObject, dwSize );
										*pdwObjectSize = dwSize;
										hListObj = (HDSKMLISTOBJ)hCurrListData;
										dwResult = DSKM_ERR_OK;
									}
								}
							}
							else {
								hListObj = (HDSKMLISTOBJ)hCurrListData;
								dwResult = DSKM_ERR_OK;
							}
							break;
						}
					}
					hCurrListData = DATA_Get_Next( hCurrListData, 0 );
				}
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}
	DSKM_EXCEPT_SIZE
}


//! \fn				:  DSKMAPI DSKM_ParList_GetObjectId
//! \brief			:	Выдать идентификатор объекта
//! \return			: Идентификатор объекта списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
AVP_dword   DSKMAPI DSKM_ParList_GetObjectId( HDSKMLIST hList, HDSKMLISTOBJ hListObject ) {
	DSKM_TRY {
		AVP_dword dwObjectId = 0;
		AVP_dword dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( hList && hListObject ) {
				HDATA hCurrListData = DATA_Get_First( (HDATA)hList, 0 );
				dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
				while ( hCurrListData && DSKM_NOT_OK(dwResult) ) {
					if ( DSKM_OK(DSKM_IsObject(hCurrListData)) ) {
						if ( (HDATA)hListObject == hCurrListData ) {
							AVP_dword dwPID = DATA_Get_Id( hCurrListData, 0 );
							dwObjectId = GET_AVP_PID_ID(dwPID);
							dwResult = DSKM_ERR_OK;
						}
					}
					hCurrListData = DATA_Get_Next( hCurrListData, 0 );
				}
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return dwObjectId;
	}
	DSKM_EXCEPT_SIZE
}

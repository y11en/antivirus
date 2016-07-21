/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_objsp.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	9/30/2005 12:37:19 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"



//! \fn				: DSKM_ParList_SetObjectProp
//! \brief			:	Установить свойство объекта операции (DSKM_OBJ_*)
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwPropId - идертификатор свойства
//! \param          : void *pvData - буфер с данными
//! \param          : AVP_dword dwDataSize - размер буфера
AVP_dword 	DSKMAPI DSKM_ParList_SetObjectProp( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwPropId, void *pvData, AVP_dword dwDataSize ) {
	DSKM_TRY {
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;

			if ( pvData ) {
				switch ( dwPropId ) {
					case DSKM_OBJ_EXTERNAL_PARAM_PTR  : {
						HPROP hProp = DATA_Find_Prop( (HDATA)hListObject, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID );
						if ( hProp ) {
							DATA_Remove_Prop_H( (HDATA)hListObject, 0, hProp );
							DATA_Add_Prop( (HDATA)hListObject, 0, DSKM_OBJECT_PROP_ID(dwPropId), (AVP_size_t)pvData, 0 );
							dwResult = DSKM_ERR_OK;
						}
						break;
					}
					case DSKM_OBJ_EXTERNAL_BUFFER_PTR : {
						AVP_dword dwPID = DATA_Get_Id( (HDATA)hListObject, 0 );
						switch ( GET_AVP_PID_APP(dwPID) ) {
							case DSKM_APP_OBJECT_BUFF_PARAM :
							case DSKM_APP_REG_BUFF_PARAM  :
							case DSKM_APP_OBJECT_ASSOC_BUFF : {
								HPROP hProp = DATA_Find_Prop( (HDATA)hListObject, 0, 0 );
								if ( hProp ) {
									PROP_Set_Val( hProp, (AVP_size_t)pvData, 0 );
									dwResult = DSKM_ERR_OK;
								}
								break;
							}
						}
						break;
					}
					default : {
						HPROP hProp = DATA_Find_Prop( (HDATA)hListObject, 0, DSKM_OBJECT_PROP_ID(dwPropId) );
						if ( hProp ) 
							DATA_Remove_Prop_H( (HDATA)hListObject, 0, hProp );

						DATA_Add_Prop( (HDATA)hListObject, 0, DSKM_OBJECT_PROP_ID(dwPropId), (AVP_size_t)pvData, dwDataSize );
						dwResult = DSKM_ERR_OK;
					}
					break;
				}
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return dwResult;
	}	
	DSKM_EXCEPT
}


//! \fn				: DSKM_ParList_GetObjectProp
//! \brief			:	Получить свойство объекта операции (DSKM_OBJ_* или любое свое до 0xffff)
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwPropId - идертификатор свойства
//! \param          : void *ppvData	- указатель на буфер результата
//! \param          : AVP_dword *pdwDataSize - указатель на размер буфера результата
AVP_dword 	DSKMAPI DSKM_ParList_GetObjectProp( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwPropId, void *pvData, AVP_dword *pdwDataSize ) {
	DSKM_TRY {
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( pdwDataSize ) {
				dwResult = DSKM_ERR_PROP_NOT_FOUND;
				switch ( dwPropId ) {
					case DSKM_OBJ_EXTERNAL_PARAM_PTR  :
					case DSKM_OBJ_EXTERNAL_BUFFER_PTR : {
						AVP_dword dwPID = DATA_Get_Id( (HDATA)hListObject, 0 );
						switch ( GET_AVP_PID_APP(dwPID) ) {
							case DSKM_APP_OBJECT_BUFF_PARAM :
							case DSKM_APP_REG_BUFF_PARAM  :
							case DSKM_APP_OBJECT_ASSOC_BUFF : {
								if ( pvData == 0 ) {
									*pdwDataSize = sizeof(void *);
									dwResult = DSKM_ERR_OK;
									break;
								}
								if ( *pdwDataSize < sizeof(void *) ) {
									*pdwDataSize = sizeof(void *);
									dwResult = DSKM_ERR_INVALID_SIZE;
								}
								else {
									switch ( dwPropId ) {
										case DSKM_OBJ_EXTERNAL_PARAM_PTR  : {
											DATA_Get_Val( (HDATA)hListObject, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, pvData, sizeof(void *) );
											*pdwDataSize = sizeof(void *);
											break;
										}
										case DSKM_OBJ_EXTERNAL_BUFFER_PTR : {
											DATA_Get_Val( (HDATA)hListObject, 0, 0, pvData, sizeof(void *) );
											*pdwDataSize = sizeof(void *);
											break;
										}
									}
									dwResult = DSKM_ERR_OK;
								}
								break;
							}
						}
						break;
					}
					case DSKM_OBJ_PROCESSING_ERROR : {
						if ( DSKM_OK(DSKM_IsObject((HDATA)hListObject)) ) {
							HPROP hErrProp = DATA_Find_Prop( (HDATA)hListObject, 0, DSKM_OBJECT_ERROR_ID );
							if ( hErrProp ) {
								PROP_Get_Val( hErrProp, &dwPropId, sizeof(AVP_dword) );
								if ( DSKM_NOT_OK(dwPropId) ) {
									if ( pvData == 0 ) {
										*pdwDataSize = sizeof(AVP_dword);
										dwResult = DSKM_ERR_OK;
										break;
									}
									if ( *pdwDataSize < sizeof(AVP_dword) ) {
										*pdwDataSize = sizeof(AVP_dword);
										dwResult = DSKM_ERR_INVALID_SIZE;
									}
									else {
										PROP_Get_Val( hErrProp, pvData, sizeof(AVP_dword) );
										dwResult = DSKM_ERR_OK;
									}
								}
							}
						}
						break;
					}
					default : {
						HPROP hProp = DATA_Find_Prop( (HDATA)hListObject, 0, DSKM_OBJECT_PROP_ID(dwPropId) );
						if ( hProp ) {
							AVP_dword dwSize = PROP_Get_Val( hProp, 0, 0 );
							if ( pvData == 0 ) {
								*pdwDataSize = dwSize;
								dwResult = DSKM_ERR_OK;
								break;
							}
							if ( *pdwDataSize < dwSize ) {
								*pdwDataSize = dwSize;
								dwResult = DSKM_ERR_INVALID_SIZE;
							}
							else {
								PROP_Get_Val( hProp, pvData, dwSize );
								*pdwDataSize = dwSize;
								dwResult = DSKM_ERR_OK;
							}
						}
					}
					break;
				}
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return dwResult;
	}	
	DSKM_EXCEPT
}


//! \fn				: DSKM_ParList_SetObjectProp
//! \brief			:	Установить свойство объекта, подлежащее замешиванию в хэш объекта
//!               Таких свойств может быть установлено любое количество. 
//!               Замешивание выполняется последовательно (в порядке добавления)
//!               ВАЖНО! Порадок следования свойств при подписывании и проверке должен совпадать!
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : void *pvData - буфер с данными
//! \param          : AVP_dword dwDataSize - размер буфера
AVP_dword 	DSKMAPI DSKM_ParList_SetObjectHashingProp( HDSKMLIST hList, HDSKMLISTOBJ hListObject, void *pvData, AVP_dword dwDataSize ) {
	DSKM_TRY {
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			AVP_dword dwPID = DATA_Get_Id( (HDATA)hListObject, 0 );
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			
			switch ( GET_AVP_PID_APP(dwPID) ) {
				case DSKM_APP_OBJECT_NAME_PARAM :
				case DSKM_APP_OBJECT_BUFF_PARAM :
				case DSKM_APP_OBJECT_HASH_PARAM	: {
					AVP_dword dwPID = DSKM_OBJECT_HASHED_PROP_ID( 0 );
					dwResult = DSKM_ERR_DATA_PROCESSING;
					dwPID = DSKM_FindUniquePropPID( (HDATA)hList, dwPID );
					if ( dwPID ) {
						DATA_Add_Prop( (HDATA)hListObject, 0, dwPID, (AVP_size_t)pvData, dwDataSize );
						dwResult = DSKM_ERR_OK;
					}
				}
				break;
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return dwResult;
	}	
	DSKM_EXCEPT
}

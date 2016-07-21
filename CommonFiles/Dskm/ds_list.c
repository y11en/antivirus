/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	params.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/6/2005 3:38:09 PM
*		
*		Example:	
*		
*		
*		
*/		

#include "dskm.h"
#include "dskmi.h"



//! \fn				: DSKMAPI DSKM_ParList_SetLastError
//! \brief			:	
//! \return			: static void 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwResult
void DSKMAPI DSKM_ParList_SetLastError( HDSKMLIST hList, AVP_dword dwResult ) {
	if ( hList ) {
		HPROP hErrProp = DATA_Find_Prop( (HDATA)hList, 0, DSKM_OBJECT_ERROR_ID );
		if ( !hErrProp ) {
			DATA_Add_Prop( (HDATA)hList, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
		}
		else {
			PROP_Set_Val( hErrProp, dwResult, 0 );
		}
	}
}



//! \fn				: DSKMAPI DSKM_ParList_GetLastError
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          : HDSKMLIST hList
AVP_dword DSKMAPI DSKM_ParList_GetLastError( HDSKMLIST hList ) {
	if ( hList ) {
		AVP_dword dwResult = DSKM_ERR_OK;
		HPROP hErrProp = DATA_Find_Prop( (HDATA)hList, 0, DSKM_OBJECT_ERROR_ID );
		if ( hErrProp ) {
			PROP_Get_Val( hErrProp, &dwResult, sizeof(dwResult) );
		}
		return dwResult;
	}
	return DSKM_ERR_INVALID_PARAMETER;
}




//! \fn				: DSKM_ParList_Create
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          : HDSKMLIST *hParList
AVP_dword		DSKMAPI DSKM_ParList_Create( HDSKMLIST *hParList ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}

		if ( hParList ) {
			if ( (*hParList = (HDSKMLIST)DATA_Add(0, 0, AVP_PID_NOTHING, 0, 0)) != 0 )
				return DSKM_ERR_OK;

			return DSKM_ERR_DATA_PROCESSING;
		}

		return DSKM_ERR_INVALID_PARAMETER;
	}	
	DSKM_EXCEPT
}

//! \fn				: DSKM_ParList_Delete
//! \brief			:	
//! \return			: AVP_dword 
//! \param          : HDSKMLIST hList
AVP_dword 	DSKMAPI DSKM_ParList_Delete( HDSKMLIST hList ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( DATA_Remove((HDATA)hList, 0) )
			return DSKM_ERR_OK;
		
		return DSKM_ERR_DATA_PROCESSING;
	}	
	DSKM_EXCEPT
}



//! \fn				: DSKM_ParList_RemoveObjectById
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwObjectId
AVP_dword 	DSKMAPI DSKM_ParList_RemoveObjectById( HDSKMLIST hList, AVP_dword dwObjectId ) {
	DSKM_TRY {
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			HDATA hCurrData = DATA_Get_First( (HDATA)hList, 0 );
			dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
			while ( hCurrData ) {
				HDATA hNextData = DATA_Get_Next( hCurrData, 0 );
				AVP_dword dwPID = DATA_Get_Id( hCurrData, 0 );
				switch ( GET_AVP_PID_APP(dwPID) ) {
					case DSKM_APP_OBJECT_NAME_PARAM :
					case DSKM_APP_OBJECT_BUFF_PARAM :
					case DSKM_APP_OBJECT_HASH_PARAM	:
					case DSKM_APP_REG_NAME_PARAM :
					case DSKM_APP_REG_BUFF_PARAM :
						if ( GET_AVP_PID_ID(dwPID) == dwObjectId ) {
							DATA_Remove( hCurrData, 0 );
							dwResult = DSKM_ERR_OK;
						}
						break;
				}
				hCurrData = hNextData;
			}
		}
	
	
		DSKM_ParList_SetLastError( hList, dwResult );
		return dwResult;
	}	
	DSKM_EXCEPT
}


//! \fn				: DSKM_ParList_RemoveObjectByH
//! \brief			:	Удалить объект из списка
//! \return			: Код ошибки 
//! \param          :  HDSKMLIST hList - список объектов
//! \param          : HDSKMLISTOBJ hListObject - объект для удаления
AVP_dword 	DSKMAPI DSKM_ParList_RemoveObjectByH( HDSKMLIST hList, HDSKMLISTOBJ hListObject ) {
	DSKM_TRY {
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			HDATA hCurrData = DATA_Get_First( (HDATA)hList, 0 );
			dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
			while ( hCurrData ) {
				if ( (HDATA)hListObject == hCurrData ) {
					DATA_Remove( hCurrData, 0 );
					dwResult = DSKM_ERR_OK;
					break;
				}
				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return dwResult;
	}	
	DSKM_EXCEPT
}



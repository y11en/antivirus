/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_objse.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	10/3/2005 12:37:33 PM
*		
*		Example:	
*		
*		
*		
*/		



#include "dskm.h"
#include "dskmi.h"


//! \fn				: DSKM_ParList_GetFirstObject
//! \brief			:	Получить первый объект/параметр списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetFirstObject( HDSKMLIST hList ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
			hListObj = (HDSKMLISTOBJ)DATA_Get_First( (HDATA)hList, 0 );
			while ( hListObj ) {
				if ( DSKM_OK(DSKM_IsObject((HDATA)hListObj)) ) {
					dwResult = DSKM_ERR_OK;
					break;
				}
				hListObj = (HDSKMLISTOBJ)DATA_Get_Next( (HDATA)hListObj, 0 );
			}
		}
			
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}


//! \fn				: DSKM_ParList_GetNextObject
//! \brief			:	Получить следующий объект/параметр списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hCurrObject - текущий объект
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetNextObject( HDSKMLIST hList, HDSKMLISTOBJ hCurrObject ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
			hListObj = (HDSKMLISTOBJ)DATA_Get_Next( (HDATA)hCurrObject, 0 );
			while ( hListObj ) {
				if ( DSKM_OK(DSKM_IsObject((HDATA)hListObj)) ) {
					dwResult = DSKM_ERR_OK;
					break;
				}
				hListObj = (HDSKMLISTOBJ)DATA_Get_Next( (HDATA)hListObj, 0 );
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}


//! \fn				: DSKM_ParList_GetFirstParam
//! \brief			:	Получить первый параметр списка/объекта
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          :  HDSKMLISTOBJ hObject - объект операции, параметры которого нужно перебрать
//                     Если 0 - перебираются параметры списка
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetFirstParam( HDSKMLIST hList, HDSKMLISTOBJ hObject ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
			if ( !hObject ) {
				hListObj = (HDSKMLISTOBJ)DATA_Get_First( (HDATA)hList, 0 );
			}
			else {
				if ( DSKM_OK(DSKM_IsObject((HDATA)hObject)) ) {
					hListObj = (HDSKMLISTOBJ)DATA_Get_First( (HDATA)hObject, 0 );
				}
			}
			while ( hListObj ) {
				if ( DSKM_NOT_OK(DSKM_IsObject((HDATA)hListObj)) ) {
					dwResult = DSKM_ERR_OK;
					break;
				}
				hListObj = (HDSKMLISTOBJ)DATA_Get_Next( (HDATA)hListObj, 0 );
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}


//! \fn				: DSKM_ParList_GetNextParam
//! \brief			:	Получить следующий объект/параметр списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hCurrParam - текущий объект
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetNextParam( HDSKMLIST hList, HDSKMLISTOBJ hCurrParam ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_OBJECT_NOT_FOUND;
			hListObj = (HDSKMLISTOBJ)DATA_Get_Next( (HDATA)hCurrParam, 0 );
			while ( hListObj ) {
				if ( DSKM_NOT_OK(DSKM_IsObject((HDATA)hListObj)) ) {
					dwResult = DSKM_ERR_OK;
					break;
				}
				hListObj = (HDSKMLISTOBJ)DATA_Get_Next( (HDATA)hListObj, 0 );
			}
		}
		
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



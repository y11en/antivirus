/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_objsb.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	9/30/2005 12:40:25 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"




//! \fn				: DSKM_ParList_PrepareBufferedObject
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwPID
//! \param          : const void *pvBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack
//! \param          : void *pvCallBackParams
static HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_PrepareBufferedObject( HDSKMLIST hList, AVP_dword dwPID, const void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams ) {
	HDSKMLISTOBJ hListObj = 0;
	AVP_dword    dwResult = DSKM_ERR_DATA_PROCESSING;
	
	if ( !GET_AVP_PID_ID(dwPID) ) 
		dwPID = DSKM_FindUniquePID( (HDATA)hList, dwPID );
	if ( dwPID ) {
		hListObj = (HDSKMLISTOBJ)DATA_Add( (HDATA)hList, 0, dwPID, (AVP_size_t)&pvBuffer, sizeof(void *) );
		if ( hListObj ) {
			DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_OBJECT_BUFF_SIZE_ID, dwBufferSize, 0 );
			DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_OBJECT_BUFF_PFN_ID, (AVP_size_t)&pfnCallBack,  sizeof(void *) );
			DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, (AVP_size_t)&pvCallBackParams, sizeof(void *) );
			// Поставим ошибку сразу - если проверку хакнут обходом кода, ошибка останется
			DATA_Add_Prop( (HDATA)hListObj, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_LIB_CODE_HACKED, 0 );
			dwResult = DSKM_ERR_OK;
		}
	}
	
	DSKM_ParList_SetLastError( hList, dwResult );
	
	return hListObj;
}



//! \fn				:  DSKMAPI DSKM_ParList_AddObjectTypeAssociationBufferedObject
//! \brief			:	Добавить буферизованный объект ассоциаций типов
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : void *pvBuffer - указатель на буфер (не может быть нулевым)
//! \param          : AVP_dword dwBufferSize - размер буфера (не может быть нулевым)
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера 
//!												(может быть нулевой - используется наполненный буфер)
//! \param          : void *pvCallBackParams - параметры функции подкачки
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectTypeAssociationBufferedObject( HDSKMLIST hList, const void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;

		DSKM_ParList_SetLastError( hList, DSKM_ERR_INVALID_PARAMETER );
		if ( hList && pvBuffer && dwBufferSize ) {
			DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
			if ( DSKMAllocator && DSKMAllocator ) {
				AVP_dword dwPID = DSKM_ASSOC_BUFF_PTR_ID( 0 );
				hListObj = DSKM_ParList_PrepareBufferedObject( hList, dwPID, pvBuffer, dwBufferSize, pfnCallBack, pvCallBackParams );
			}
		}
			
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



//! \fn				: DSKM_ParList_AddBufferedObject
//! \brief			:	
//! \return			: AVP_dword 
//! \param          :  HDSKMLIST hList
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack
//! \param          : void *pCallBackParams
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddBufferedObject( HDSKMLIST hList, AVP_dword dwObjectId, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		DSKM_ParList_SetLastError( hList, DSKM_ERR_INVALID_PARAMETER );
		if ( hList && pvBuffer && dwBufferSize ) {
			DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
			if ( DSKMAllocator && DSKMAllocator ) {
				AVP_dword dwPID = DSKM_OBJECT_BUFF_PTR_ID( dwObjectId );
				hListObj = DSKM_ParList_PrepareBufferedObject( hList, dwPID, pvBuffer, dwBufferSize, pfnCallBack, pvCallBackParams );
			}
		}
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



//! \fn				: DSKM_ParList_AddBufferedReg
//! \brief			:	
//! \return			: AVP_dword 
//! \param          :  HDSKMLIST hList
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack
//! \param          : void *pCallBackParams
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddBufferedReg( HDSKMLIST hList, AVP_dword dwRegId, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		DSKM_ParList_SetLastError( hList, DSKM_ERR_INVALID_PARAMETER );
		if ( hList && pvBuffer && dwBufferSize ) {
			DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
			if ( DSKMAllocator && DSKMAllocator ) {
				AVP_dword dwPID = DSKM_REG_BUFF_PTR_ID( dwRegId );
				hListObj = DSKM_ParList_PrepareBufferedObject( hList, dwPID, pvBuffer, dwBufferSize, pfnCallBack, pvCallBackParams );
			}
		}
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



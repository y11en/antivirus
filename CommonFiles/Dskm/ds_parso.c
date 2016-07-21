/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_parso.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	10/21/2005 3:37:27 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"


//! \fn				: DSKM_ParList_AddObjectParamRoot
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : HDSKMLISTOBJ hListObject
static HDATA 	DSKMAPI DSKM_ParList_AddObjectParamRoot( HDSKMLIST hList, HDSKMLISTOBJ hListObject ) {
	HDATA hParamRoot = 0;
	DSKM_ParList_SetLastError( hList, DSKM_ERR_DATA_PROCESSING );
	{
		MAKE_ADDR1(PRAddr, DSKM_PARAM_ROOT_ID);
		hParamRoot = DATA_Find( (HDATA)hListObject, PRAddr);
	}
	if ( !hParamRoot ) {
		hParamRoot = DATA_Add( (HDATA)hListObject, 0, DSKM_PARAM_ROOT_ID, 0, 0 );
	}
	return hParamRoot;
}



//! \fn				: DSKM_ParList_AddObjectStrParam
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwParamId
//! \param          : const AVP_char* pStrParam
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectStrParam( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwParamId, const AVP_char* pStrParam ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );

		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( dwParamId ) {
				HDATA hParamRoot = DSKM_ParList_AddObjectParamRoot( hList, hListObject );
				if ( hParamRoot && (hListObj = (HDSKMLISTOBJ)DATA_Add(hParamRoot, 0, DSKM_STR_PARAM_ID(dwParamId), (AVP_size_t)pStrParam, 0)) )
					dwResult = DSKM_ERR_OK;
			}
		}
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



//! \fn				: DSKM_ParList_AddObjectDWordParam
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwParamId
//! \param          : AVP_dword dwParam
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectDWordParam( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwParamId, AVP_dword dwParam ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( dwParamId ) {
				HDATA hParamRoot = DSKM_ParList_AddObjectParamRoot( hList, hListObject );
				if ( hParamRoot && (hListObj = (HDSKMLISTOBJ)DATA_Add(hParamRoot, 0, DSKM_DWORD_PARAM_ID(dwParamId), (AVP_size_t)dwParam, 0)) )
					dwResult = DSKM_ERR_OK;
			}
		}
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}




//! \fn				: DSKM_ParList_AddObjectBinaryPtrParam
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwParamId
//! \param          : void *pParamPtr
//! \param          : AVP_dword dwBinarySize
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectBinaryPtrParam( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwParamId, void *pParamPtr, AVP_dword dwBinarySize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		DSKM_ParList_SetLastError( hList, DSKM_ERR_NOT_INITIALIZED );
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( dwParamId ) {
				HDATA hParamRoot = DSKM_ParList_AddObjectParamRoot( hList, hListObject );
				if ( hParamRoot && (hListObj = (HDSKMLISTOBJ)DATA_Add(hParamRoot, 0, DSKM_BPTR_PARAM_ID(dwParamId), (AVP_size_t)pParamPtr, dwBinarySize)) ) {
					dwResult = DSKM_ERR_OK;
				}
			}
		}
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}





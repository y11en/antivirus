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



//! \fn				: DSKM_ParList_AddStrParam
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwParamId
//! \param          : const AVP_char* pStrParam
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddStrParam( HDSKMLIST hList, AVP_dword dwParamId, const AVP_char* pStrParam ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;

		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( dwParamId ) {
				dwResult = DSKM_ERR_DATA_PROCESSING;
				if ( ( hListObj = (HDSKMLISTOBJ)DATA_Add((HDATA)hList, 0, DSKM_STR_PARAM_ID(dwParamId), (AVP_size_t)pStrParam, 0) ) != 0 )
					dwResult = DSKM_ERR_OK;
			}
		}
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}



//! \fn				: DSKM_ParList_AddDWordParam
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwParamId
//! \param          : AVP_dword dwParam
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddDWordParam( HDSKMLIST hList, AVP_dword dwParamId, AVP_dword dwParam ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( dwParamId ) {
				dwResult = DSKM_ERR_DATA_PROCESSING;
				if ( ( hListObj = (HDSKMLISTOBJ)DATA_Add((HDATA)hList, 0, DSKM_DWORD_PARAM_ID(dwParamId), (AVP_size_t)dwParam, 0)) != 0 )
					dwResult = DSKM_ERR_OK;
			}
		}
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}




//! \fn				: DSKM_ParList_AddBinaryPtrParam
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKMLIST hList
//! \param          : AVP_dword dwParamId
//! \param          : void *pParamPtr
//! \param          : AVP_dword dwBinarySize
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddBinaryPtrParam( HDSKMLIST hList, AVP_dword dwParamId, void *pParamPtr, AVP_dword dwBinarySize ) {
	DSKM_TRY {
		HDSKMLISTOBJ hListObj = 0;
		AVP_dword    dwResult = DSKM_ERR_NOT_INITIALIZED;
		
		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( dwParamId ) 
				dwResult = DSKM_ERR_DATA_PROCESSING;
			if ( ( hListObj = (HDSKMLISTOBJ)DATA_Add( (HDATA)hList, 0, DSKM_BPTR_PARAM_ID(dwParamId), (AVP_size_t)pParamPtr, dwBinarySize)) != 0 ) {
				dwResult = DSKM_ERR_OK;
			}
		}
		DSKM_ParList_SetLastError( hList, dwResult );
		return hListObj;
	}	
	DSKM_EXCEPT_SIZE
}





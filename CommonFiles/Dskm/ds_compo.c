/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_compo.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	11/21/2005 11:23:02 AM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmdefs.h"
#include "dskmi.h"
#include "cl_util.h"

//! \fn				:  DSKMAPI DSKM_CompareObjectsIgnoreSign
//! \brief			:	Сравнить вместе ВСЕ объекты из списка
//                Cравниваются все объеты вместе, путем расчета и сравнения хэшей
//! \return			: Код ошибки - DSKM_ERR_OBJECTS_NOT_EQUAL, если объекты не совпадают  
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
AVP_dword   DSKMAPI DSKM_CompareObjectsIgnoreSign( HDSKM hDSKM, HDSKMLIST hObjectsList ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( hObjectsList && hDSKM ) {
			void *pComparedObjectHash = 0;
			AVP_dword dwComparedObjectHashSize = 0;

			HDATA hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
			dwResult = DSKM_ERR_OK;
			while ( hCurrData && DSKM_OK(dwResult)) {
				AVP_dword dwPID;

				void *pObjectHash = 0;
				AVP_dword dwObjectHashSize = 0;
				
				dwPID = DATA_Get_Id( hCurrData, 0 );
				dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
				switch ( dwPID ) {
					case DSKM_OBJECT_NAME_ID(0) : {
						AVP_char *pObjectFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, 0), 0 );
						DSKM_FTABLE_ADD(hCurrData);
						if ( pObjectFileName ) {
							if ( DSKM_NOT_OK(dwResult = DSKM_HashObjectByNameCheckSignImp(hDSKM, pObjectFileName, &pObjectHash, &dwObjectHashSize, hCurrData, FALSE)) ) {
								dwResult = DSKM_ERR_CANNOT_CALC_OBJ_HASH;
							}
							DSKMLiberator( pObjectFileName );
						}
						break;
					}
					case DSKM_OBJECT_BUFF_PTR_ID(0) : {
						void *pBuffer;
						AVP_dword dwBufferSize;
						pfnDSKM_GetBuffer_CallBack pCallBack;
						void *pCallBackParams;
						
						DATA_Get_Val( hCurrData, 0, 0, &pBuffer, sizeof(pBuffer) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
						
						DSKM_FTABLE_ADD(hCurrData);
						
						if ( DSKM_NOT_OK(dwResult=DSKM_HashObjectByBufferCheckSignImp(hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, &pObjectHash, &dwObjectHashSize, hCurrData, FALSE)) ) {
							dwResult = DSKM_ERR_CANNOT_CALC_OBJ_HASH;
						}
						break;
					}
					case DSKM_OBJECT_HASH_ID(0) : {
						dwObjectHashSize = DATA_Get_Val( hCurrData, 0, 0, 0, 0 );
						if ( dwObjectHashSize ) {
							pObjectHash = DSKMAllocator( dwObjectHashSize );
							if ( pObjectHash ) {
								DATA_Get_Val( hCurrData, 0, 0, pObjectHash, dwObjectHashSize );
							}
						}
						if ( !pObjectHash ) {
							dwResult = DSKM_ERR_CANNOT_CALC_OBJ_HASH;
						}
						break;
					}
				}
				if ( pObjectHash ) {
					if ( !pComparedObjectHash ) {
						pComparedObjectHash = DSKMAllocator( dwObjectHashSize );
						if ( pComparedObjectHash ) {
							ds_mcpy( pComparedObjectHash, pObjectHash, dwObjectHashSize );
							dwComparedObjectHashSize = dwObjectHashSize;
						}
					}
					if ( pComparedObjectHash ) {
						if ( dwComparedObjectHashSize != dwObjectHashSize || ds_mcmp(pComparedObjectHash, pObjectHash, dwObjectHashSize) ) {
							dwResult = DSKM_ERR_OBJECTS_NOT_EQUAL;
							break;
						}
					}

					DSKMLiberator( pObjectHash );

					pObjectHash = 0;
					dwObjectHashSize = 0;
				}
				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
			if ( pComparedObjectHash ) {
				DSKMLiberator( pComparedObjectHash );
			}
		}
		
		return dwResult;
	}
	DSKM_EXCEPT
}

/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_assch.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	7/8/2005 12:17:44 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"






//! \fn				: DSKMAPI DSKM_FindObjectType
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDATA hRegData
//! \param          : HDATA hParamList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool *pbFound
//! \param          : HDATA hCollectionData
static AVP_dword DSKMAPI DSKM_FindObjectType( HDATA hRegData, HDATA hParamList, AVP_dword dwObjectsType, AVP_bool *pbFound, HDATA hCollectionData ) {
	AVP_bool bFound = 0;
	HDATA hCurrRegData = DATA_Get_First( hRegData, 0 );
	while ( hCurrRegData ) {
		bFound = DSKM_FindParamList( hParamList, hCurrRegData );
		if ( bFound ) {
			DATA_Get_Val( hCurrRegData, 0, 0, &dwObjectsType, sizeof(dwObjectsType) );
			if ( hCollectionData ) {
				HPROP hProp = DATA_Find_Prop( hCollectionData, 0, 0 );
				if ( hProp ) {
					AVP_bool bFoundValue = 0;
					int i = 0;
					int c = PROP_Arr_Count(hProp);
					for ( ; i<c; i++ ) {
						AVP_dword dwPropVal;
						PROP_Arr_Get_Items( hProp, i, &dwPropVal, sizeof(dwPropVal) );
						if ( dwPropVal == dwObjectsType ) {
							bFoundValue = 1;
							break;
						}
					}
					if ( !bFoundValue )
						PROP_Arr_Set_Items(hProp, c, &dwObjectsType, sizeof(dwObjectsType) );
				}
			}
			else
				break;
		}
		hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
	}
	if ( pbFound )
		*pbFound = bFound;
	return dwObjectsType;
}




//! \fn				: DSKM_CheckObjectTypeByAssoc
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : HDATA hParamList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hCollectionData
AVP_dword  	DSKMAPI DSKM_CheckObjectTypeByAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hParamList, AVP_dword dwObjectsType, HDATA hCollectionData ) {
	if ( hCollectionData && dwObjectsType ) {
		HPROP hProp = DATA_Find_Prop( hCollectionData, 0, 0 );
		if ( hProp ) 
			PROP_Arr_Set_Items(hProp, 0, &dwObjectsType, sizeof(dwObjectsType) );
	}
	{
	  // Искать кэшированную ассоциацию 
		HDATA hASRegRootData = 0;
		MAKE_ADDR1(ASAddr, DSKM_AS_HEAD_PID);
		hASRegRootData = DATA_Find( (HDATA)hDSKM, ASAddr );
		if ( hASRegRootData ) {
			HDATA hCurrRegData = DATA_Get_First( hASRegRootData, 0 );
			while ( hCurrRegData ) {
				dwObjectsType = DSKM_FindObjectType( hCurrRegData, hParamList, dwObjectsType, 0, hCollectionData );
				hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
			}
			return dwObjectsType;
		}
		// Не нашли. Будем искать в списке параметров
		if ( hObjectsList ) {
			AVP_bool bFound = 0;
			AVP_dword dwCount = 1;
			HDATA hCurrData;
			AVP_dword dwResult;

			hASRegRootData = DATA_Add( (HDATA)hDSKM, 0, DSKM_AS_HEAD_PID, 0, 0 );
			
			hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
			while ( hCurrData ) {
				AVP_dword dwPID = DATA_Get_Id( hCurrData, 0 );
				dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
				switch ( dwPID ) {
					case DSKM_ASSOC_FILE_ID(0) : {
						AVP_char *pFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, 0), 0 );
						if ( pFileName ) {
							HDATA hRegData = 0;
							dwResult = DSKM_DeserializeRegFile( hDSKM, pFileName, DSKM_OTYPE_ASSOC_FILE, &hRegData );
							DSKMLiberator( pFileName );
							if ( DSKM_OK(dwResult) && hRegData ) {
								dwObjectsType = DSKM_FindObjectType( hRegData, hParamList, dwObjectsType, &bFound, hCollectionData );
								if ( DATA_Replace_App_ID(hRegData, 0, DSKM_AS_AID, DATA_IF_DOESNOT_GO_DOWN) &&
										 DATA_Replace_ID(hRegData, 0, (AVP_word)dwCount++) ) 
									DATA_Attach( hASRegRootData, 0, hRegData, DATA_IF_ROOT_INCLUDE );
							}
						}
						break;
					}
					case DSKM_ASSOC_BUFF_PTR_ID(0) : {
						void *pBuffer;
						AVP_dword dwBufferSize;
						pfnDSKM_GetBuffer_CallBack pCallBack;
						void *pCallBackParams;
						HDATA hRegData = 0;
						
						DATA_Get_Val( hCurrData, 0, 0, &pBuffer, sizeof(pBuffer) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
						
						dwResult = DSKM_DeserializeRegBuffer( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, DSKM_OTYPE_ASSOC_FILE, &hRegData );
						if ( DSKM_OK(dwResult) && hRegData ) {
							dwObjectsType = DSKM_FindObjectType( hRegData, hParamList, dwObjectsType, &bFound, hCollectionData );
							if ( DATA_Replace_App_ID(hRegData, 0, DSKM_AS_AID, DATA_IF_DOESNOT_GO_DOWN) &&
									 DATA_Replace_ID(hRegData, 0, (AVP_word)dwCount++) ) 
								DATA_Attach( hASRegRootData, 0, hRegData, DATA_IF_ROOT_INCLUDE );
						}
						break;
					}
					default :
						break;
				}
				
				if ( bFound && !hCollectionData )
					break;

				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
		}
	}
	return dwObjectsType;
}



//! \fn				:   DSKMAPI DSKM_SaveKeyParamsToTypeNode
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDATA hKeyData
//! \param          : HDATA hTypeNode
static AVP_dword   DSKMAPI DSKM_SaveKeyParamsToTypeNode( HDATA hKeyData, HDATA hTypeNode ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	HPROP hCurrKeyProp = DATA_Get_First_Prop( hKeyData, 0 );
	while ( hCurrKeyProp ) {
		AVP_dword dwPID;
		dwPID = PROP_Get_Id( hCurrKeyProp );
		switch ( GET_AVP_PID_APP(dwPID) ) {
			case DSKM_APP_DWORD_PARAM : {
				AVP_dword dwValue;
				PROP_Get_Val( hCurrKeyProp, &dwValue, sizeof(dwValue) );
				if ( DATA_Add_Prop(hTypeNode, 0, dwPID, dwValue, 0) )
					dwResult = DSKM_ERR_OK;
				break;
			}
			case DSKM_APP_STR_PARAM : {
				AVP_dword dwParamBufferSize = PROP_Get_Val( hCurrKeyProp, 0, 0 );
				if ( dwParamBufferSize ) {
					void *pParamBuffer = DSKMAllocator( dwParamBufferSize );
					if ( pParamBuffer ) {
						PROP_Get_Val( hCurrKeyProp, pParamBuffer, dwParamBufferSize );
						if ( DATA_Add_Prop(hTypeNode, 0, dwPID, (AVP_size_t)pParamBuffer, dwParamBufferSize) )
							dwResult = DSKM_ERR_OK;
						DSKMLiberator( pParamBuffer );
					}
				}
				break;
			}
			case DSKM_APP_BPTR_PARAM : {
				AVP_dword dwParamBufferSize = PROP_Get_Val( hCurrKeyProp, 0, 0 );
				void *pParamBuffer = DSKMAllocator( dwParamBufferSize );
				if ( pParamBuffer ) {
					PROP_Get_Val( hCurrKeyProp, pParamBuffer, dwParamBufferSize );
					if ( DATA_Add_Prop(hTypeNode, 0, DSKM_BIN_PARAM_ID(GET_AVP_PID_ID(dwPID)), (AVP_size_t)pParamBuffer, dwParamBufferSize) )
						dwResult = DSKM_ERR_OK;
					DSKMLiberator( pParamBuffer );
				}
				break;
			}
		}
		hCurrKeyProp = PROP_Get_Next( hCurrKeyProp );
	}
	return dwResult;
}




//! \fn				: DSKMAPI DSKM_FindParamPropList
//! \brief			:	
//! \return			: static AVP_bool 
//! \param          :  HDATA hParamList
//! \param          : HDATA hCurrRegData
static AVP_bool DSKMAPI DSKM_FindParamPropList( HDATA hParamList, HDATA hCurrRegData ) {
	AVP_bool bFound = 1;
	HPROP hCurrParamProp = DATA_Get_First_Prop( hParamList, 0 );
	while ( hCurrParamProp && bFound ) {
		AVP_dword dwPID = PROP_Get_Id( hCurrParamProp );
		switch ( GET_AVP_PID_APP(dwPID) ) {
			case DSKM_APP_DWORD_PARAM : 
			case DSKM_APP_STR_PARAM : 
			case DSKM_APP_BPTR_PARAM : {
				AVP_bool bFoundId = 0;
				HPROP hCurrRegProp = DATA_Get_First_Prop( hCurrRegData, 0 );
				while ( hCurrRegProp && bFound ) {
					AVP_dword dwCurrPID;
					dwCurrPID = PROP_Get_Id( hCurrRegProp );
					if ( (GET_AVP_PID_ID(dwCurrPID) == GET_AVP_PID_ID(dwPID)) && 
							 (GET_AVP_PID_APP(dwCurrPID) == GET_AVP_PID_APP(dwPID)) ) {
						bFoundId = 1;
						switch ( GET_AVP_PID_APP(dwPID) ) {
							case DSKM_APP_DWORD_PARAM : {
								AVP_dword dwParamValue;
								AVP_dword dwRegValue;
								PROP_Get_Val( hCurrRegProp, &dwRegValue, sizeof(dwRegValue) );
								PROP_Get_Val( hCurrParamProp, &dwParamValue, sizeof(dwParamValue) );
								if ( dwRegValue != dwParamValue ) {
									// Если не равны - значит "не равны"
									bFound = 0;
								}
								break;
							}
							case DSKM_APP_STR_PARAM : {
								AVP_dword dwParamStrSize = 0;
								AVP_char *pParamStr = DSKM_GetPropValueAsString( hCurrParamProp, &dwParamStrSize );
								AVP_dword dwRegStrSize = 0;
								AVP_char *pRegStr = DSKM_GetPropValueAsString( hCurrRegProp, &dwRegStrSize );
								if ( !((dwRegStrSize == dwParamStrSize) && !ds_mcmp(pParamStr, pRegStr, dwParamStrSize)) ) {
									// Если строки не одинаковой длины или не совпадают - значит "не равны"
									bFound = 0;
								}
								DSKMLiberator( pParamStr );
								DSKMLiberator( pRegStr );
								break;
							}
							case DSKM_APP_BPTR_PARAM : {
								void *pParamBuffer = 0;
								AVP_dword dwParamBufferSize = PROP_Get_Val( hCurrParamProp, 0, 0 );
								void *pRegBuffer;
								AVP_dword dwRegBufferSize;

								dwRegBufferSize = PROP_Get_Val( hCurrRegProp, 0, 0 );
								if ( dwRegBufferSize != dwParamBufferSize ) {
									// Если размеры не равны - значит "не равны"
									bFound = 0;
									break;
								}
								pRegBuffer = DSKMAllocator( dwRegBufferSize );
								if ( pRegBuffer ) {
									PROP_Get_Val( hCurrRegProp, pRegBuffer, dwRegBufferSize );

									pParamBuffer = DSKMAllocator( dwParamBufferSize );
									if ( pParamBuffer ) {
										PROP_Get_Val( hCurrParamProp, pParamBuffer, dwParamBufferSize );

										if ( ds_mcmp(pRegBuffer, pParamBuffer, dwRegBufferSize) ) {
											// Если несовпадают - значит "не равны"
											bFound = 0;
										}
										DSKMLiberator(pParamBuffer);
									}
									
									DSKMLiberator(pRegBuffer);
								}
								break;
							}
						}
					}
					hCurrRegProp = PROP_Get_Next( hCurrRegProp );
				}
				if ( !bFoundId )
					bFound = 0;
			}
		}
		
		hCurrParamProp = PROP_Get_Next( hCurrParamProp );
	}

	return bFound;
}





//! \fn				: DSKM_PrepareTypeAssocByKeys
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : HDATA hKeysData
AVP_dword  	DSKMAPI DSKM_PrepareTypeAssocByKeys( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hKeysData ) {
	HDATA hASRegRootData = 0;

	// Найти установлунную ассоциацию
	HDATA hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
	while ( hCurrData ) {
		AVP_dword dwPID = DATA_Get_Id( hCurrData, 0 );
		dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
		switch ( dwPID ) {
			case DSKM_ASSOC_FILE_ID(0) : 
			case DSKM_ASSOC_BUFF_PTR_ID(0) : 
				// Нашли. Дальше будем работать по ней
				return DSKM_ERR_OK;
		}
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}

	{
		// Проверить/создать головной узел
		MAKE_ADDR1(ASAddr, DSKM_AS_HEAD_PID);
		hASRegRootData = DATA_Find( (HDATA)hDSKM, ASAddr );
		if ( !hASRegRootData ) {
			hASRegRootData = DATA_Add( (HDATA)hDSKM, 0, DSKM_AS_HEAD_PID, 0, 0 );
			if ( hASRegRootData ) {
				HDATA hASKeyRootData = 0;
				AVP_dword dwPID = DSKM_AS_PID( 0 );
				dwPID = DSKM_FindUniquePID( (HDATA)hASRegRootData, dwPID );
				if ( dwPID ) {
					hASKeyRootData = DATA_Add( hASRegRootData, 0, dwPID, 0, 0 );
				}	
				if ( !hASKeyRootData )
					return DSKM_ERR_DATA_PROCESSING;
			}
		}
		if ( !hASRegRootData ) 
			return DSKM_ERR_DATA_PROCESSING;
	}

	hCurrData = DATA_Get_First( hKeysData, 0 );
	while ( hCurrData ) {
		HPROP hTypeProp = DATA_Find_Prop( hCurrData, 0, DSKM_KEYREG_OBJTYPE_ID );
		if ( hTypeProp ) {
			AVP_dword dwKeyObjType = 0;
			PROP_Get_Val( hTypeProp, &dwKeyObjType, sizeof(dwKeyObjType) );
			if ( dwKeyObjType ) {
				AVP_bool bFound = 0;
				HDATA hCurrAssDatas = DATA_Get_First( hASRegRootData, 0 );
				while ( hCurrAssDatas && !bFound ) {
					HDATA hCurrAssData = DATA_Get_First( hCurrAssDatas, 0 );
					while ( hCurrAssData ) {
						AVP_dword dwAssObjType = 0;
						DATA_Get_Val( hCurrAssData, 0, 0, &dwAssObjType, sizeof(dwAssObjType) );
						if ( dwKeyObjType == dwAssObjType ) {
							if ( DSKM_FindParamPropList(hCurrData, hCurrAssData) ) {
								bFound = 1;
								break;
							}
						}
						hCurrAssData = DATA_Get_Next( hCurrAssData, 0 );
					}
					hCurrAssDatas = DATA_Get_Next( hCurrAssDatas, 0 );
				}
				if ( !bFound ) {
					AVP_bool bFound = 0;
					HDATA hCurrAssDatas = DATA_Get_First( hASRegRootData, 0 );
					while ( hCurrAssDatas && !bFound ) {
						AVP_dword dwPID = DSKM_ASSOC_OTYPE_ID( 0 );
						dwPID = DSKM_FindUniquePID( hCurrAssDatas, dwPID );
						if ( dwPID ) {
							HDATA hNode = DATA_Add( hCurrAssDatas, 0, dwPID, dwKeyObjType, 0 );
							bFound = 1;
							if ( hNode ) {
								AVP_dword dwResult = DSKM_SaveKeyParamsToTypeNode( hCurrData, hNode );
								if ( DSKM_NOT_OK(dwResult) )
									DATA_Remove( hNode, 0 );
							}
							break;
						}	
						hCurrAssDatas = DATA_Get_Next( hCurrAssDatas, 0 );
					}
					if ( !bFound ) {
						AVP_dword dwPID = DSKM_AS_PID( 0 );
						dwPID = DSKM_FindUniquePID( (HDATA)hASRegRootData, dwPID );
						if ( dwPID ) {
							hCurrAssDatas = DATA_Add( hASRegRootData, 0, dwPID, 0, 0 );
							if ( hCurrAssDatas ) {
								HDATA hNode = DATA_Add( hCurrAssDatas, 0, dwPID, dwKeyObjType, 0 );
								if ( hNode ) {
									AVP_dword dwResult = DSKM_SaveKeyParamsToTypeNode( hCurrData, hNode );
									if ( DSKM_NOT_OK(dwResult) )
										DATA_Remove( hNode, 0 );
								}
							}
							if ( !hCurrAssDatas )
								return DSKM_ERR_DATA_PROCESSING;
						}	
					}
				}
			}
		}
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}

	return DSKM_ERR_OK;
}

/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_fkey.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/10/2005 2:41:12 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"




//! \fn				:  DSKMAPI DSKM_CountDataChildren
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          : HDATA hRootData
static AVP_dword  DSKMAPI DSKM_CountDataChildren( HDATA hRootData ) {
	AVP_dword dwCount = 0;
	HDATA hCurrData = DATA_Get_First( hRootData, 0 );
	while ( hCurrData ) {
		if ( DSKM_CLFUNC_TABLE_ID != DATA_Get_Id(hCurrData, 0) )
			dwCount++;
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}
	return dwCount;
}

//! \fn				: DSKM_FindParamList
//! \brief			:	
//! \return			: AVP_bool DSKMAPI 
//! \param          :  HDATA hParamList
//! \param          : HDATA hCurrRegData
AVP_bool DSKMAPI DSKM_FindParamList( HDATA hParamList, HDATA hCurrRegData ) {
	AVP_bool bFound = 1;
	if ( hParamList ) {
		HDATA hCurrParamData = DATA_Get_First( hParamList, 0 );
		while ( hCurrParamData && bFound ) {
			AVP_dword dwPID = DATA_Get_Id( hCurrParamData, 0 );
			AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
			switch ( dwPID_0 ) {
				case DSKM_OBJECT_NAME_ID(0) : 
				case DSKM_OBJECT_BUFF_PTR_ID(0) : 
				case DSKM_OBJECT_HASH_ID(0) : 
				case DSKM_ASSOC_OTYPE_ID(0) :
				case DSKM_ASSOC_FILE_ID(0) :
				case DSKM_ASSOC_BUFF_PTR_ID(0) :
				case DSKM_REG_NAME_ID(0) : 
				case DSKM_REG_BUFF_PTR_ID(0) : 
					break;
				default : {
					AVP_bool bStop = 0;
					HPROP hCurrRegProp = DATA_Get_First_Prop( hCurrRegData, 0 );
					while ( hCurrRegProp && bFound && !bStop ) {
						AVP_dword dwCurrPID;
						dwCurrPID = PROP_Get_Id( hCurrRegProp );
						if ( (GET_AVP_PID_ID(dwCurrPID) == GET_AVP_PID_ID(dwPID)) && 
								 (GET_AVP_PID_APP(dwCurrPID) == GET_AVP_PID_APP(dwPID)) ) {
							switch ( GET_AVP_PID_APP(dwPID) ) {
								case DSKM_APP_DWORD_PARAM : {
									AVP_dword dwParamValue;
									AVP_dword dwRegValue;
									PROP_Get_Val( hCurrRegProp, &dwRegValue, sizeof(dwRegValue) );
									DATA_Get_Val( hCurrParamData, 0, 0, &dwParamValue, sizeof(dwParamValue) );
									/*
									if ( dwRegValue && dwParamValue && dwRegValue != dwParamValue ) {
										// Если оба ненулевые и не равны - значит "не равны"
										bFound = 0;
									}
									// Если хотя бы один нулевой или равны -  значит "равны" 
									bStop = 1;
									*/
									if ( dwRegValue != dwParamValue ) {
										// Если не равны - значит "не равны"
										bFound = 0;
									}
									// Если равны -  значит "равны" 
									bStop = 1;
									break;
								}
								case DSKM_APP_STR_PARAM : {
									AVP_dword dwParamStrSize = 0;
									AVP_char *pParamStr = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrParamData, 0, 0), &dwParamStrSize );
									AVP_dword dwRegStrSize = 0;
									AVP_char *pRegStr = DSKM_GetPropValueAsString( hCurrRegProp, &dwRegStrSize );
									/*
									if ( !(pParamStr && pRegStr && 
												((dwRegStrSize == 2 && *pRegStr == '*') || (dwParamStrSize == 2 && *pParamStr == '*') ||
												(dwRegStrSize == dwParamStrSize && !ds_mcmp(pParamStr, pRegStr, dwParamStrSize)))) ) {
										bFound = 0;
									}
									*/
									if ( !((dwRegStrSize == dwParamStrSize) && !ds_mcmp(pParamStr, pRegStr, dwParamStrSize)) ) {
										// Если строки не одинаковой длины или не совпадают - значит "не равны"
										bFound = 0;
									}
									DSKMLiberator( pParamStr );
									DSKMLiberator( pRegStr );
									bStop = 1;
									break;
								}
								case DSKM_APP_BPTR_PARAM : {
									void *pParamBuffer = 0;
									AVP_dword dwParamBufferSize = DATA_Get_Val( hCurrParamData, 0, 0, 0, 0 );
									void *pRegBuffer;
									AVP_dword dwRegBufferSize;

									/*
									bStop = 1;
									
									if ( !!pParamBuffer != !!dwParamBufferSize ) {
										// Если НЕ "оба нулевые" или НЕ "оба не нулевые" - неправильные параметры
										bFound = 0;
										break;
									}
									
									dwRegBufferSize = PROP_Get_Val( hCurrRegProp, 0, 0 );
									if ( dwRegBufferSize && dwParamBufferSize && dwRegBufferSize != dwParamBufferSize ) {
										// Если размеры оба ненулевые и не равны - значит "не равны"
										bFound = 0;
										break;
									}
									
									if ( (!pParamBuffer && !dwParamBufferSize) || !dwRegBufferSize ) {
										// Если хотя бы один буфер нулевой = значит "равны"
										break;
									}
									
									pRegBuffer = DSKMAllocator( dwRegBufferSize );
									if ( pRegBuffer ) {
										PROP_Get_Val( hCurrRegProp, pRegBuffer, dwRegBufferSize );
										
										if ( ds_mcmp(pRegBuffer, pParamBuffer, dwRegBufferSize) ) {
											bFound = 0;
										}
										
										DSKMLiberator(pRegBuffer);
									}
									*/

									bStop = 1;

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
											DATA_Get_Val( hCurrParamData, 0, 0, pParamBuffer, dwParamBufferSize );

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
					break;
				}
			}
			
			hCurrParamData = DATA_Get_Next( hCurrParamData, 0 );
		}
	}

	return bFound;
}



//! \fn				: DSKM_FindKeyData
//! \brief			:	
//! \return			: HDATA DSKMAPI 
//! \param          :  HDATA hParamList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hRegRootData
//! \param          : HDATA hKeysTree
HDATA DSKMAPI DSKM_FindKeyData( HDATA hParamList, AVP_dword dwObjectsType, HDATA hRegRootData, HDATA hKeysTree ) {
	DSKM_TRY {
		AVP_bool bOnePresented = 0;
		HDATA hCurrRegData = DATA_Get_First( hRegRootData, 0 );

		if ( hKeysTree ) {
			DSKM_FTABLE_ADD(hKeysTree);
		}
		
		while ( hCurrRegData ) {
			AVP_bool bFound = 0;
			AVP_dword dwRegObjType = 0;
			DATA_Get_Val( hCurrRegData, 0, DSKM_KEYREG_OBJTYPE_ID, &dwRegObjType, sizeof(dwRegObjType) );

			if ( !dwObjectsType || dwRegObjType == dwObjectsType ) {
				bFound = 1;
				if ( hParamList ) {
					bFound = DSKM_FindParamList( hParamList, hCurrRegData );
				}
			}
			if ( bFound ) {
				if ( hKeysTree ) {
					AVP_dword dwPID = DATA_Get_Id( hCurrRegData, 0 );
					dwPID = DSKM_FindUniquePID( hKeysTree, dwPID );
					if ( dwPID ) {
						HDATA hCopyData = DATA_Copy( 0, 0, hCurrRegData, DATA_IF_ROOT_INCLUDE );
						DATA_Replace_ID( hCopyData, 0, (AVP_word)GET_AVP_PID_ID(dwPID) );
						DATA_Attach( hKeysTree, 0, hCopyData, DATA_IF_ROOT_INCLUDE );
						bOnePresented = 1;
					}
				}
				else {
					return hCurrRegData;
				}
			}

			hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
		}

		if ( hKeysTree && bOnePresented ) {
			return hKeysTree;
		}

		return 0;
	}
	DSKM_EXCEPT_SIZE
}



//! \fn				: DSKM_FindKeyInFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pKeyRegFileName
//! \param          : void **pKey
//! \param          : AVP_dword *pdwKeySize
AVP_dword DSKMAPI DSKM_FindKeyInFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pKeyRegFileName, void **pKey, AVP_dword *pdwKeySize ) {
	DSKM_TRY {
		HDATA hRegRootData = 0;
		AVP_dword dwResult = DSKM_DeserializeRegFile( hDSKM, pKeyRegFileName, 0, &hRegRootData );
		
		if ( DSKM_OK(dwResult) ) {
			HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			HDATA hKeyData = DSKM_FindKeyData( (HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysRootData );
			dwResult = DSKM_ERR_KEY_NOT_FOUND;
			if ( hKeyData ) {
				AVP_dword dwKeyCounts = DSKM_CountDataChildren( hKeysRootData );
				switch ( dwKeyCounts ) {
					case 0 :
						break;
					default :
						dwResult = DSKM_ERR_MORE_THAN_ONE_KEY;
						break;
					case 1 :
						dwResult = DSKM_PrepareTypeAssocByKeys( hDSKM, hObjectsList, hKeysRootData );
						if ( DSKM_OK(dwResult) ) {
							hKeyData = DATA_Get_First( hKeysRootData, 0 );

							if ( DSKM_CLFUNC_TABLE_ID == DATA_Get_Id(hKeyData, 0) )
								hKeyData = DATA_Get_Next( hKeyData, 0 );

							*pdwKeySize = DATA_Get_Val( hKeyData, 0, 0, 0, 0 );
							if ( *pdwKeySize ) {
								*pKey = DSKMAllocator( *pdwKeySize );
								if ( *pKey ) {
									DATA_Get_Val( hKeyData, 0, 0, *pKey, *pdwKeySize );
									dwResult = DSKM_ERR_OK;
								}
							}
						}
						break;
				}
			}
			DATA_Remove( hKeysRootData, 0 );
			DATA_Remove( hRegRootData, 0 );
		}

		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				: DSKM_FindKeyInBuffer
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : void *pvKeyRegBuffer
//! \param          : AVP_dword dwKeyRegBufferSize
//! \param          : void **pKey
//! \param          : AVP_dword *dwKeySize
AVP_dword DSKMAPI DSKM_FindKeyInBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize, void **pKey, AVP_dword *dwKeySize ) {
	DSKM_TRY {
		HDATA hRegRootData = 0;
		AVP_dword dwResult = DSKM_DeserializeRegBuffer( hDSKM, pvKeyRegBuffer, dwKeyRegBufferSize, 0, 0, 0, &hRegRootData );
		
		if ( DSKM_OK(dwResult) ) {
			HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			HDATA hKeyData = DSKM_FindKeyData( (HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysRootData );
			dwResult = DSKM_ERR_KEY_NOT_FOUND;
			if ( hKeyData ) {
				AVP_dword dwKeyCounts = DSKM_CountDataChildren( hKeysRootData );
				switch ( dwKeyCounts ) {
					case 0 :
						break;
					default :
						dwResult = DSKM_ERR_MORE_THAN_ONE_KEY;
						break;
					case 1 :
						dwResult = DSKM_PrepareTypeAssocByKeys( hDSKM, hObjectsList, hKeysRootData );
						if ( DSKM_OK(dwResult) ) {
							hKeyData = DATA_Get_First( hKeysRootData, 0 );
							
							if ( DSKM_CLFUNC_TABLE_ID == DATA_Get_Id(hKeyData, 0) )
								hKeyData = DATA_Get_Next( hKeyData, 0 );
							
							*dwKeySize = DATA_Get_Val( hKeyData, 0, 0, 0, 0 );
							if ( *dwKeySize ) {
								*pKey = DSKMAllocator( *dwKeySize );
								if ( *pKey ) {
									DATA_Get_Val( hKeyData, 0, 0, *pKey, *dwKeySize );
									dwResult = DSKM_ERR_OK;
								}
							}
						}
				}
			}
			DATA_Remove( hKeysRootData, 0 );
			DATA_Remove( hRegRootData, 0 );
		}
		
		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				: DSKM_FindKeysTreeInFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDSKMFILE hKeyFile
//! \param          : HDATA hKeysTree
AVP_dword DSKMAPI DSKM_FindKeysTreeInFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDSKMFILE hKeyFile, HDATA hKeysTree ) {
	DSKM_TRY {
		HDATA hRegRootData = 0;
		AVP_dword dwResult = DSKM_DeserializeRegHFile( hDSKM, hKeyFile, 0, &hRegRootData );
		
		if ( DSKM_OK(dwResult) ) {
			if ( !DSKM_FindKeyData((HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysTree) ) {
				dwResult = DSKM_ERR_KEY_NOT_FOUND;
			}
			
			DATA_Remove( hRegRootData, 0 );
		}
		
		return dwResult;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_FindKeysTreeInBuffer
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : void *pvKeyRegBuffer
//! \param          : AVP_dword dwKeyRegBufferSize
//! \param          : HDATA hKeysTree
AVP_dword DSKMAPI DSKM_FindKeysTreeInBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize, HDATA hKeysTree ) {
	DSKM_TRY {
		HDATA hRegRootData = 0;
		AVP_dword dwResult = DSKM_DeserializeRegBuffer( hDSKM, pvKeyRegBuffer, dwKeyRegBufferSize, 0, 0, 0, &hRegRootData );
		
		if ( DSKM_OK(dwResult) ) {
			if ( !DSKM_FindKeyData((HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysTree) ) {
				dwResult = DSKM_ERR_KEY_NOT_FOUND;
			}
		
			DATA_Remove( hRegRootData, 0 );
		}
		
		return dwResult;
	}
	DSKM_EXCEPT
}



//! \fn				: DSKM_FindKeysTreeInObject
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hKeyRegObject
//! \param          : HDATA hKeysTree
AVP_dword DSKMAPI DSKM_FindKeysTreeInObject( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hKeyRegObject, HDATA hKeysTree ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_KEY_NOT_FOUND;
		HDATA hRegRootData = DATA_Get_First( hKeyRegObject, 0 );
		if ( !hRegRootData ) {
			AVP_dword dwPID = DATA_Get_Id( hKeyRegObject, 0 );
			AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
			switch ( dwPID_0 ) {
				case DSKM_REG_NAME_ID(0) : {
					AVP_char *pRegFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hKeyRegObject, 0, 0), 0 );
					if ( pRegFileName ) {
						HDATA hRegRootData = 0;
						dwResult = DSKM_DeserializeRegFile( hDSKM, pRegFileName, dwObjectsType, &hRegRootData );
						DSKMLiberator( pRegFileName );
					}
					break;
				}
				case DSKM_REG_BUFF_PTR_ID(0) : {
					void *pBuffer;
					AVP_dword dwBufferSize;
					pfnDSKM_GetBuffer_CallBack pCallBack;
					void *pCallBackParams;
					HDATA hRegRootData = 0;
					
					DATA_Get_Val( hKeyRegObject, 0, 0, &pBuffer, sizeof(pBuffer) );
					DATA_Get_Val( hKeyRegObject, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
					DATA_Get_Val( hKeyRegObject, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
					DATA_Get_Val( hKeyRegObject, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
					
					dwResult = DSKM_DeserializeRegBuffer( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, dwObjectsType, &hRegRootData );
					break;
				}
			}
			if ( DSKM_OK(dwResult) && hRegRootData ) {
				if ( !DSKM_FindKeyData((HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysTree) ) {
					dwResult = DSKM_ERR_KEY_NOT_FOUND;
				}
				
				DATA_Remove( hRegRootData, 0 );
			}
		}
		else {
			if ( DSKM_FindKeyData((HDATA)hObjectsList, dwObjectsType, hRegRootData, hKeysTree) ) {
				dwResult = DSKM_ERR_OK;
			}
		}
		return dwResult;
	}
	DSKM_EXCEPT
}




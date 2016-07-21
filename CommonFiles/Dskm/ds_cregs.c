/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_cregs.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	9/19/2005 12:45:23 PM
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


// ---
typedef struct _tag_CObjectCheckParams {
	HDATA     m_hObjectsList;
	HDATA			m_hKeyRegsRoot;
	HDATA			m_hKeyReg;
	void     *m_pKeyData;
	AVP_dword m_dwKeyDataSize;
} CObjectCheckParams;


//! \fn				: DSKMAPI DSKM_CompareObjectsHash
//! \brief			:	
//! \return			: static AVP_bool 
//! \param          :  HDATA hObject
//! \param          : void *pvHash
//! \param          : AVP_dword dwHashSize
static AVP_bool DSKMAPI DSKM_CompareObjectsHash( HDATA hObject, void *pvHash, AVP_dword dwHashSize ) {
	AVP_bool bResult = 0;
	HPROP hProp = DATA_Find_Prop( hObject, 0, DSKM_OBJECT_REG_HASH_ID(0) );
	if ( hProp ) {
		AVP_dword dwObjectHashSize = PROP_Get_Val( hProp, 0, 0 );
		if ( dwObjectHashSize && dwObjectHashSize == dwHashSize ) {
			void *pObjectHash = DSKMAllocator( dwObjectHashSize );
			if ( pObjectHash ) {
				PROP_Get_Val( hProp, pObjectHash, dwHashSize );
				if ( !ds_mcmp(pObjectHash, pvHash, dwObjectHashSize) ) {
					bResult = 1;
				}
				DSKMLiberator( pObjectHash );
			}
		}
	}
	return bResult;
}



//! \fn				: DSKMAPI DSKM_CheckRegType
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDATA hObjData
//! \param          : HDATA hRegData
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_dword dwResult
static AVP_dword DSKMAPI DSKM_CheckRegType( HDATA hObjData, HDATA hRegData, AVP_dword dwObjectsType, AVP_dword dwResult ) {
	if ( DSKM_OK(dwResult) ) {
		HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_OBJTYPE_ID );
		if ( hProp ) {
			AVP_dword dwRegObjsType = 0;
			PROP_Get_Val( hProp, &dwRegObjsType, sizeof(dwRegObjsType) );
			if ( dwRegObjsType != dwObjectsType ) {
				DATA_Attach( hObjData, 0, hRegData, DATA_IF_ROOT_INCLUDE );
				dwResult = DSKM_ERR_INVALID_OBJTYPE;
			}
		}	
	}
	DATA_Set_Val( hObjData, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
	return dwResult;
}





//! \fn				: DSKMAPI DSKM_CheckCurrObject
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hCurrObjData
//! \param          : AVP_dword dwObjectsType
//! \param          : CObjectCheckParams *pCheckParams
//! \param          : AVP_dword dwResult
static AVP_dword DSKMAPI DSKM_CheckCurrObject( HDSKM hDSKM, HDATA hCurrObjData, AVP_dword dwObjectsType, CObjectCheckParams *pCheckParams, AVP_dword dwResult ) {
	if ( DSKM_OK(dwResult) ) {
		if ( pCheckParams->m_hKeyRegsRoot ) {
			HDATA hCurrKeyRegData = DATA_Get_First( pCheckParams->m_hKeyRegsRoot, 0 );
			dwResult = DSKM_ERR_REG_NOT_FOUND;
			while ( hCurrKeyRegData && DSKM_NOT_OK(dwResult) ) {
				dwResult = DSKM_CheckRegObject( hDSKM, (HDSKMLIST)pCheckParams->m_hObjectsList, dwObjectsType, hCurrObjData, hCurrKeyRegData, 0, 0 );
				hCurrKeyRegData = DATA_Get_Next( hCurrKeyRegData, 0 );
			}
		}
		else {
			if ( pCheckParams->m_hKeyReg ) {
				dwResult = DSKM_CheckRegObject( hDSKM, (HDSKMLIST)pCheckParams->m_hObjectsList, dwObjectsType, hCurrObjData, pCheckParams->m_hKeyReg, 0, 0 );
			}
			else {
				if ( pCheckParams->m_pKeyData && pCheckParams->m_dwKeyDataSize ) {
					dwResult = DSKM_CheckRegObject( hDSKM, (HDSKMLIST)pCheckParams->m_hObjectsList, dwObjectsType, hCurrObjData, 0, pCheckParams->m_pKeyData, pCheckParams->m_dwKeyDataSize );
				}
			}
		}
	}
	DATA_Set_Val( hCurrObjData, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
	return dwResult;
}

//! \fn				: DSKMAPI DSKM_FindRegObject
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA *phRegObjData
//! \param          : HDATA *phRegRootData
//! \param          : AVP_bool *pbCheckSpecial
//! \param          : CObjectCheckParams *pCheckParams
//! \param          : HDATA hObjectRegRootData
static AVP_dword DSKMAPI DSKM_FindRegObject( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA *phRegObjData, HDATA *phRegRootData, AVP_bool *pbCheckSpecial, CObjectCheckParams *pCheckParams, HDATA hObjectRegRootData ) {
	AVP_dword dwResult = DSKM_ERR_REG_NOT_FOUND;

	// Ранее проверенные и десерилизованные реестры подвешены к своим объектам - искать по типу
	HDATA hRegRootData = 0;
	HDATA hCurrObjData = DATA_Get_First( (HDATA)hObjectsList, 0 );
	while ( hCurrObjData ) {
		HDATA hRegData = DATA_Get_First( hCurrObjData, 0 );
		if ( hRegData ) {
			HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_OBJTYPE_ID );
			if ( hProp ) {
				AVP_dword dwRegObjsType = 0;
				PROP_Get_Val( hProp, &dwRegObjsType, sizeof(dwRegObjsType) );
				if ( dwRegObjsType == dwObjectsType ) {
					// Ранее подготовленный реестр найден - отдать готовый
					DATA_Detach( hRegData, 0 );
					DATA_Set_Val( hCurrObjData, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_OK, 0 );
					*phRegRootData = hRegData;
					*phRegObjData = hCurrObjData;
					return DSKM_ERR_OK;
				}
			}
		}
		
		dwResult = DSKM_ERR_REG_NOT_FOUND;
		hCurrObjData = DATA_Get_Next( hCurrObjData, 0 );
	}

	hCurrObjData = DATA_Get_First( (HDATA)hObjectsList, 0 );
	while ( hCurrObjData ) {
		HDATA hRegData = DATA_Get_First( hCurrObjData, 0 );
		if ( !hRegData ) {
			void *pObjectHash = 0;
			AVP_dword dwObjectHashSize = 0;

			if ( hObjectRegRootData ) {
				// Ранее проверенные и десерилизованные реестры подвешены к своим головам - искать по хэшу
				if ( DSKM_OK(DSKM_PrepareObjectHash(hDSKM, hCurrObjData, &pObjectHash, &dwObjectHashSize)) ) {
					HDATA hCurrRegData = DATA_Get_First( hObjectRegRootData, 0 );
					while ( hCurrRegData ) {
						if ( DSKM_CompareObjectsHash(hCurrRegData, pObjectHash, dwObjectHashSize) ) {
							// Реестр найден - вернуть ошибку
							DATA_Set_Val( hCurrObjData, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_OK, 0 );
							*phRegRootData = hCurrRegData;
							*phRegObjData = hCurrObjData;
							dwResult = DSKM_ERR_OK;
							break;
						}
						hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
					}

					if ( DSKM_OK(dwResult) ) {
						if ( pObjectHash )
							DSKMLiberator( pObjectHash );
						return DSKM_INT_ERR_EQUAL_HASH;
					}
				}
			}
			{
				AVP_dword dwPID = DATA_Get_Id( hCurrObjData, 0 );
				AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
				switch ( dwPID_0 ) {
					case DSKM_ASSOC_FILE_ID(0) :
					case DSKM_REG_NAME_ID(0) : {
						AVP_char *pRegFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrObjData, 0, 0), 0 );
						if ( pRegFileName ) {
							if ( pCheckParams ) {
								dwResult = DSKM_DeserializeRegFile( hDSKM, pRegFileName, dwObjectsType, 0 );
								dwResult = DSKM_CheckCurrObject( hDSKM, hCurrObjData, dwObjectsType, pCheckParams, dwResult );
								if ( DSKM_OK(dwResult) ) {
									dwResult = DSKM_DeserializeRegFile( hDSKM, pRegFileName, 0, &hRegRootData );
									dwResult = DSKM_CheckRegType( hCurrObjData, hRegRootData, dwObjectsType, dwResult );
								}
							}
							else {
								dwResult = DSKM_DeserializeRegFile( hDSKM, pRegFileName, 0, &hRegRootData );
								dwResult = DSKM_CheckRegType( hCurrObjData, hRegRootData, dwObjectsType, dwResult );
							}
							DSKMLiberator( pRegFileName );
						}
						break;
					}
					case DSKM_ASSOC_BUFF_PTR_ID(0) :
					case DSKM_REG_BUFF_PTR_ID(0) : {
						void *pBuffer;
						AVP_dword dwBufferSize;
						pfnDSKM_GetBuffer_CallBack pCallBack;
						void *pCallBackParams;
						
						DATA_Get_Val( hCurrObjData, 0, 0, &pBuffer, sizeof(pBuffer) );
						DATA_Get_Val( hCurrObjData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
						DATA_Get_Val( hCurrObjData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
						DATA_Get_Val( hCurrObjData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
						
						if ( pCheckParams ) {
							dwResult = DSKM_DeserializeRegBuffer( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, dwObjectsType, 0 );
							dwResult = DSKM_CheckCurrObject( hDSKM, hCurrObjData, dwObjectsType, pCheckParams, dwResult );
							if ( DSKM_OK(dwResult) ) {
								dwResult = DSKM_DeserializeRegBuffer( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, 0, &hRegRootData );
								dwResult = DSKM_CheckRegType( hCurrObjData, hRegRootData, dwObjectsType, dwResult );
							}
						}
						else {
							dwResult = DSKM_DeserializeRegBuffer( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, 0, &hRegRootData );
							dwResult = DSKM_CheckRegType( hCurrObjData, hRegRootData, dwObjectsType, dwResult );
						}
						break;
					}
				}
			}

			if ( DSKM_OK(dwResult) ) {
				if ( pbCheckSpecial ) {
					HPROP hProp = DATA_Find_Prop( hRegRootData, 0, DSKM_KEYREG_ISSPEC_ID );
					if ( *pbCheckSpecial && !hProp ) {
						dwResult = DSKM_ERR_ANOTHER_BASE_REG_FOUND;
						if ( hRegRootData )
							DATA_Remove( hRegRootData, 0 );
					}
					else
						*pbCheckSpecial = !hProp;
				}

				if ( DSKM_OK(dwResult) ) {
					*phRegRootData = hRegRootData;
					*phRegObjData = hCurrObjData;
					if ( pObjectHash ) {
						DATA_Add_Prop( hRegRootData, 0, DSKM_OBJECT_REG_HASH_ID(0), (AVP_size_t)pObjectHash, dwObjectHashSize );
					}
				}
				DATA_Set_Val( hCurrObjData, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
				if ( pObjectHash ) {
					DSKMLiberator( pObjectHash );
					pObjectHash = 0;
				}
				break;
			}
			else
				hRegRootData = 0;

			if ( pObjectHash ) {
				DSKMLiberator( pObjectHash );
				pObjectHash = 0;
			}
		}
		dwResult = DSKM_ERR_REG_NOT_FOUND;
		hCurrObjData = DATA_Get_Next( hCurrObjData, 0 );
	}

	return dwResult;
}



//! \fn				:  DSKMAPI DSKM_PrepareRegsSetImp
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hProcessedData
static AVP_dword  DSKMAPI DSKM_PrepareRegsSetImp( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hProcessedData ) {
	AVP_dword dwResult = DSKM_ERR_OK;
	HDATA hKLRegRootData = 0;
	HDATA hOPRegRootData = 0;
	// Найти реестр ключа ЛК 
	MAKE_ADDR1(KLAddr, DSKM_KL_KEY_PID);
	hKLRegRootData = DATA_Find( (HDATA)hDSKM, KLAddr );
	if ( !hKLRegRootData ) {
		HDATA hKLKeyRegObjData = 0;
		// Проверить подпись реестра ключа ЛК	мастер-ключом
		CObjectCheckParams rcCheckParams = {0, 0, 0, g_pDSKMData, g_dwDSKMDataSize};
		dwResult = DSKM_FindRegObject( hDSKM, hObjectsList, DSKM_OTYPE_KEY_KL_PUBLIC, &hKLKeyRegObjData, &hKLRegRootData, 0, &rcCheckParams, 0 );
		if ( DSKM_OK(dwResult) ) {
			if( DATA_Replace_App_ID(hKLRegRootData, 0, DSKM_KL_KID, DATA_IF_DOESNOT_GO_DOWN) ) {
				DATA_Attach( (HDATA)hDSKM, 0, hKLRegRootData, DATA_IF_ROOT_INCLUDE );
			}
			else
				dwResult = DSKM_ERR_DATA_PROCESSING;
				
			// Перекинуть в список обработанных
			DATA_Attach( hProcessedData, 0, hKLKeyRegObjData, DATA_IF_ROOT_INCLUDE );
		}
	}
	else {
		HDATA hKLRegRootData = 0;
		HDATA hKLKeyRegObjData = 0;
		// Проверить подпись реестра ключа ЛК	мастер-ключом
		CObjectCheckParams rcCheckParams = {0, 0, 0, g_pDSKMData, g_dwDSKMDataSize};
		dwResult = DSKM_FindRegObject( hDSKM, hObjectsList, DSKM_OTYPE_KEY_KL_PUBLIC, &hKLKeyRegObjData, &hKLRegRootData, 0, &rcCheckParams, 0 );
		if ( DSKM_OK(dwResult) ) {
			DATA_Remove( hKLRegRootData, 0 );
			DATA_Attach( hProcessedData, 0, hKLKeyRegObjData, DATA_IF_ROOT_INCLUDE );
		}
		if ( dwResult == DSKM_INT_ERR_EQUAL_HASH ) {
			// Перекинуть в список обработанных
			DATA_Attach( hProcessedData, 0, hKLKeyRegObjData, DATA_IF_ROOT_INCLUDE );
			dwResult = DSKM_ERR_OK;
		}
	}

/* Не используются файлы ассоциаций
	if ( DSKM_OK(dwResult) ) {
		HDATA hASRegRootData = 0;
		// Найти реестр ассоциаций 
		MAKE_ADDR1(ASAddr, DSKM_AS_HEAD_PID);
		hASRegRootData = DATA_Find( (HDATA)hDSKM, ASAddr );
		if ( !hASRegRootData ) {
			hASRegRootData = DATA_Add( (HDATA)hDSKM, 0, DSKM_AS_HEAD_PID, 0, 0 );
		}
		if ( hASRegRootData ) {
			HDATA hASRegObjData = 0;
			HDATA hASRegCurrData = 0;
			
			while (1) {
				dwResult = DSKM_FindRegObject( hDSKM, hObjectsList, DSKM_OTYPE_ASSOC_FILE, &hASRegObjData, &hASRegCurrData, 0, 0, hASRegRootData );
				if ( DSKM_OK(dwResult) ) {
					if ( DATA_Replace_App_ID(hASRegCurrData, 0, DSKM_AS_AID, DATA_IF_DOESNOT_GO_DOWN) ) {
						AVP_dword nPID = DATA_Get_Id( hASRegCurrData, 0 );
						nPID = DSKM_FindUniquePID( hASRegRootData, nPID );
						DATA_Replace_ID(hASRegCurrData, 0, (AVP_word)GET_AVP_PID_ID(nPID) );
						DATA_Attach( hASRegRootData, 0, hASRegCurrData, DATA_IF_ROOT_INCLUDE );
					}
					else
						DATA_Remove( hASRegCurrData, 0 );
					// Перекинуть в список обработанных
					DATA_Attach( hProcessedData, 0, hASRegObjData, DATA_IF_ROOT_INCLUDE );
				}
				else {
					if ( dwResult == DSKM_INT_ERR_EQUAL_HASH ) {
						// Перекинуть в список обработанных
						DATA_Attach( hProcessedData, 0, hASRegObjData, DATA_IF_ROOT_INCLUDE );
						dwResult = DSKM_ERR_OK;
					}
					else 
						break;
				}
			}
			dwResult = DSKM_ERR_OK;
		}
	}
*/
	if ( DSKM_OK(dwResult) ) {
		MAKE_ADDR1(OPAddr, DSKM_OP_KEY_HEAD_PID);
		hOPRegRootData = DATA_Find( (HDATA)hDSKM, OPAddr );
		if ( !hOPRegRootData ) {
			hOPRegRootData = DATA_Add( (HDATA)hDSKM, 0,  DSKM_OP_KEY_HEAD_PID, 0, 0 );
		}
		if ( hOPRegRootData ) {
			// Найти реестры операбельных ключей
			HDATA hOPRegData = 0;
			AVP_dword dwCount = 1;
			HDATA hOperKeyRegObjData = 0;
			// Проверить подпись реестра операбельных ключей ключом ЛК
			CObjectCheckParams rcCheckParams = {0, 0, hKLRegRootData, 0, 0};
			while ( 1 ) {
				dwResult = DSKM_FindRegObject( hDSKM, hObjectsList, DSKM_OTYPE_KEY_OPERABLE_PUBLIC, &hOperKeyRegObjData, &hOPRegData, 0, &rcCheckParams, hOPRegRootData );
				if ( DSKM_OK(dwResult) ) {
					if ( DATA_Replace_App_ID(hOPRegData, 0, DSKM_OP_KID, DATA_IF_DOESNOT_GO_DOWN) ) {
						AVP_dword nPID = DATA_Get_Id( hOPRegData, 0 );
						nPID = DSKM_FindUniquePID( hOPRegRootData, nPID );
						DATA_Replace_ID( hOPRegData, 0, (AVP_word)GET_AVP_PID_ID(nPID) );
						DATA_Attach( hOPRegRootData, 0, hOPRegData, DATA_IF_ROOT_INCLUDE );
						DSKM_PrepareTypeAssocByKeys( hDSKM, hObjectsList, hOPRegData );
						dwCount++;
					}
					else
						DATA_Remove( hOPRegData, 0 );

					// Перекинуть в список обработанных
					DATA_Attach( hProcessedData, 0, hOperKeyRegObjData, DATA_IF_ROOT_INCLUDE );
				}
				else {
					if ( dwResult == DSKM_INT_ERR_EQUAL_HASH ) {
						// Перекинуть в список обработанных
						DATA_Attach( hProcessedData, 0, hOperKeyRegObjData, DATA_IF_ROOT_INCLUDE );
						dwResult = DSKM_ERR_OK;
					}
					else 
						break;
				}
			}
			dwResult = DSKM_ERR_OK;
			if ( dwCount == 1 ) {
				if ( !DATA_Get_First(hOPRegRootData, 0) ) {
					DATA_Remove( hOPRegRootData, 0 );
					hOPRegRootData = 0;
					dwResult = DSKM_ERR_REG_NOT_FOUND;
				}
			}
		}
	}
	if ( DSKM_OK(dwResult) ) {
		AVP_bool bOneTypePresented = 0;
		
		HDATA hCollectionData = DATA_Add( 0, 0, DSKM_OBJ_TYPES_PROP_ID, 0, 0 );
		HPROP hCollectionProp = DATA_Find_Prop( hCollectionData, 0, 0 );
		dwResult = DSKM_ERR_INVALID_OBJTYPE;
		
		dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, hCollectionData );
		if ( dwObjectsType && PROP_Arr_Count(hCollectionProp) > 0 ) {
			int i = 0;
			int c = PROP_Arr_Count( hCollectionProp );
			dwResult = DSKM_ERR_REG_NOT_FOUND;
			for ( ; i<c; i++ ) {
				AVP_dword dwCurrObjectType;
				PROP_Arr_Get_Items( hCollectionProp, i, &dwCurrObjectType, sizeof(dwCurrObjectType) );

				if ( dwCurrObjectType ) {
					// Найти все реестры хэшей для данного типа объектов
					AVP_bool bBaseChecked = 0;
					AVP_bool bOnePresented = 0;

					HDATA hTypeRegRootData = 0;
					MAKE_ADDR1(HSAddr, DSKM_HS_KEY_PID(dwCurrObjectType));
					hTypeRegRootData = DATA_Find( (HDATA)hDSKM, HSAddr );

					// Если дерево реестров для типа есть, значит уже выполнялся поиск
					bOnePresented = !!hTypeRegRootData;

					dwResult = DSKM_ERR_REG_NOT_FOUND;
					while ( 1 ) {
						HDATA hHashRegObjData = 0;
						HDATA hHashRegRootData = 0;
						// Проверить подпись реестра операбельными ключами 
						CObjectCheckParams rcCheckParams = {(HDATA)hObjectsList, hOPRegRootData, 0, 0, 0};
						dwResult = DSKM_FindRegObject( hDSKM, hObjectsList, dwCurrObjectType, &hHashRegObjData, &hHashRegRootData, &bBaseChecked, &rcCheckParams, hTypeRegRootData );
						if ( DSKM_OK(dwResult) ) {
							// Вычислить хэш объекта
							void *pObjectHash = 0;
							AVP_dword dwObjectHashSize = 0;
							if ( DSKM_OK(dwResult = DSKM_PrepareObjectHash(hDSKM, hHashRegObjData, &pObjectHash, &dwObjectHashSize)) ) {
								// Создать головной узел дерева реестров заданного типа
								if ( !hTypeRegRootData )
									hTypeRegRootData = DATA_Add( (HDATA)hDSKM, 0,  DSKM_HS_KEY_PID(dwCurrObjectType), 0, 0 );

								if ( hTypeRegRootData )	{
									// Подключить реестр в дерево реестров
									// Если базовый - ID = 0
									AVP_dword dwPID = DSKM_HS_KEY_PID(0);
									HPROP hProp = DATA_Find_Prop( hHashRegRootData, 0, DSKM_KEYREG_ISSPEC_ID );
									if ( hProp ) {
										HDATA hCurrData = DATA_Get_First( hTypeRegRootData, 0 );
										while ( hCurrData ) {
											if ( DSKM_CompareObjectsHash(hCurrData, pObjectHash, dwObjectHashSize) ) {
												dwResult = DSKM_INT_ERR_EQUAL_HASH;
												break;
											}
											hCurrData = DATA_Get_Next( hCurrData, 0 );
										}

										if ( DSKM_OK(dwResult) ) {
										// Если не базовый и нет в списке - найти ID
											dwPID = DSKM_FindUniquePID( hTypeRegRootData, dwPID );
										}
									}
									else {
										// Если базовый - в дереве не должно быть такого реестра
										HDATA hFoundRootData = 0;
										MAKE_ADDR1(HSAddr, dwPID);
										hFoundRootData = DATA_Find( hTypeRegRootData, HSAddr );
										if ( hFoundRootData ) {
											if ( !DSKM_CompareObjectsHash(hFoundRootData, pObjectHash, dwObjectHashSize) ) 
												dwResult = DSKM_ERR_ANOTHER_BASE_REG_FOUND;
											else
												dwResult = DSKM_INT_ERR_EQUAL_HASH;
										}
									}
									if ( DSKM_OK(dwResult) ) {
										if ( DATA_Replace_App_ID(hHashRegRootData, 0, DSKM_HS_HID, DATA_IF_DOESNOT_GO_DOWN) &&
												 (!(AVP_word)GET_AVP_PID_ID(dwPID) || 
													DATA_Replace_ID(hHashRegRootData, 0, (AVP_word)GET_AVP_PID_ID(dwPID))) ) {
											DATA_Attach( hTypeRegRootData, 0, hHashRegRootData, DATA_IF_ROOT_INCLUDE );
											DATA_Add_Prop( hHashRegRootData, 0, DSKM_OBJECT_REG_HASH_ID(0), (AVP_size_t)pObjectHash, dwObjectHashSize );
										}
										else
											dwResult = DSKM_ERR_DATA_PROCESSING;
									}
								}
								bOnePresented = 1;
								if ( pObjectHash )
									DSKMLiberator( pObjectHash );
							}
							if ( dwResult == DSKM_INT_ERR_EQUAL_HASH )
								dwResult = DSKM_ERR_OK;

							// Перекинуть в список обработанных
							DATA_Attach( hProcessedData, 0, hHashRegObjData, DATA_IF_ROOT_INCLUDE );
						}
						else {
							if ( dwResult == DSKM_ERR_REG_NOT_FOUND && bOnePresented ) {
								dwResult = DSKM_ERR_OK;
								break;
							}
							if ( dwResult == DSKM_INT_ERR_EQUAL_HASH ) {
								// Перекинуть в список обработанных
								DATA_Attach( hProcessedData, 0, hHashRegObjData, DATA_IF_ROOT_INCLUDE );
								dwResult = DSKM_ERR_OK;
							}
							else 
								break;
						}
						if ( DSKM_NOT_OK(dwResult) ) 
							break;
					}
					bOneTypePresented += bOnePresented;
				}
			}
		}
		if ( bOneTypePresented ) {
			dwResult = DSKM_ERR_OK;
		}
		if ( dwResult == DSKM_ERR_REG_NOT_FOUND ) {
			dwResult = DSKM_ERR_OK;
		}
		DATA_Remove( hCollectionData, 0 );
	}

	{
		// Всем оставшимся в списке установить ошибку.
		HDATA hCurrObjectData = DATA_Get_First( (HDATA)hObjectsList, 0 );
		while ( hCurrObjectData ) {
			HDATA hWorkObjectData = hCurrObjectData;
			hCurrObjectData = DATA_Get_Next( hCurrObjectData, 0 );
			{
				AVP_dword dwPID = DATA_Get_Id( hWorkObjectData, 0 );
				AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
				switch ( dwPID_0 ) {
					case DSKM_ASSOC_FILE_ID(0)			:
					case DSKM_REG_NAME_ID(0)				: 
					case DSKM_ASSOC_BUFF_PTR_ID(0)	:
					case DSKM_REG_BUFF_PTR_ID(0)		: {
						HPROP hErrProp = DATA_Find_Prop( hWorkObjectData, 0, DSKM_OBJECT_ERROR_ID );
						if ( hErrProp ) {
							AVP_dword dwError;
							PROP_Get_Val( hErrProp, &dwError, sizeof(AVP_dword) );
							if ( dwError == DSKM_ERR_OK || dwError == DSKM_ERR_LIB_CODE_HACKED ) 
								PROP_Set_Val( hErrProp, DSKM_ERR_INVALID_OBJTYPE, 0 );
						}
						else
							DATA_Add_Prop( hWorkObjectData, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_INVALID_OBJTYPE, 0 );
						// Перекинуть в список обработанных
						DATA_Attach( hProcessedData, 0, hWorkObjectData, DATA_IF_ROOT_INCLUDE );
						break;
					}
				}
			}
		}
	}
	return dwResult;
}


//! \fn				: DSKM_PrepareRegsSet
//! \brief			:	Проверить набор реестров (задаются через функции DSKM_ParList_Add * Reg)
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список реестров для проверки
//! \param          : AVP_dword dwObjectsType	- тип объектов
AVP_dword  	DSKMAPI DSKM_PrepareRegsSet( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType ) {
	DSKM_TRY {
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			if ( DATA_Get_First((HDATA)hObjectsList, 0) ) {
				// Копировать дерево объектов - будем удалять из него
				HDATA hCopyData = DATA_Copy( 0, 0, (HDATA)hObjectsList, DATA_IF_ROOT_INCLUDE );
				HDATA hProcessedData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
				if ( hCopyData && hProcessedData ) {
					AVP_dword dwResult = DSKM_PrepareRegsSetImp( hDSKM, (HDSKMLIST)hCopyData, dwObjectsType, hProcessedData );
					DSKM_ProcessObjectsError( (HDATA)hObjectsList, hProcessedData, dwResult );
					DATA_Remove( hCopyData, 0 );
					DATA_Remove( hProcessedData, 0 );
					return dwResult;
				}
				return DSKM_ERR_DATA_PROCESSING;
			}
			return DSKM_ERR_OBJECT_NOT_FOUND;
		}
		
		return DSKM_ERR_INVALID_PARAMETER;
	}
	DSKM_EXCEPT
}


//! \fn				: DSKMAPI DSKM_CleanupHashingPropsOnObject
//! \brief			:	
//! \return			: static void 
//! \param          : HDATA hObjectData
static void DSKMAPI DSKM_CleanupHashingPropsOnObject( HDATA hObjectData ) {
	HPROP hCurrProp = DATA_Get_First_Prop( hObjectData, 0 );
	while ( hCurrProp ) {
		AVP_dword dwCurrPID = 0;
		HPROP hWorkProp = hCurrProp;
		hCurrProp = PROP_Get_Next( hCurrProp );
		dwCurrPID = PROP_Get_Id( hWorkProp );
		switch ( GET_AVP_PID_APP(dwCurrPID) ) {
			case DSKM_APP_OBJECT_HASHED_PROP : {
				DATA_Remove_Prop_H( hObjectData, 0, hWorkProp ); 
				break;
			}
		}
	}
}


//! \fn				: DSKMAPI DSKM_CleanupHashingPropsOnKeyRegs
//! \brief			:	
//! \return			: static void 
//! \param          : HDATA hObjects
static void DSKMAPI DSKM_CleanupHashingPropsOnKeyRegs( HDSKM hDSKM, HDATA hObjects ) {
	HDATA hCurrObjectData = DATA_Get_First( hObjects, 0 );
	while ( hCurrObjectData ) {
		AVP_dword dwResult = DSKM_ERR_INVALID_SIGN;
		AVP_dword dwPID = DATA_Get_Id( hCurrObjectData, 0 );
		AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
		switch ( dwPID_0 ) {
			case DSKM_OBJECT_NAME_ID(0) : {
				AVP_char *pRegFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrObjectData, 0, 0), 0 );
				if ( pRegFileName ) {
					HDSKMFILE hRegFile = DSKM_CreateHFile( hDSKM, pRegFileName );
					if ( hRegFile ) {
						AVP_REG_Header rcKLHeader;
						DSKM_OpenHFileHandle( hDSKM, hRegFile );
						if ( DSKM_OK(DSKM_ReadRegHeaderFromFile(hDSKM, hRegFile->m_hFile, 0, &rcKLHeader)) ) {
							DSKM_CleanupHashingPropsOnObject( hCurrObjectData );
						}
						DSKM_CloseHFile( hDSKM, hRegFile );
					}
					DSKMLiberator( pRegFileName );
				}
				break;
			}
			case DSKM_OBJECT_BUFF_PTR_ID(0) : {
				void *pBuffer;
				AVP_dword dwBufferSize;
				pfnDSKM_GetBuffer_CallBack pCallBack;
				void *pCallBackParams;
				HDATA hRegRootData = 0;
				AVP_REG_Header rcKLHeader;
				
				DATA_Get_Val( hCurrObjectData, 0, 0, &pBuffer, sizeof(pBuffer) );
				DATA_Get_Val( hCurrObjectData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
				DATA_Get_Val( hCurrObjectData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
				DATA_Get_Val( hCurrObjectData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
				
				if ( DSKM_OK(DSKM_ReadRegHeaderFromMemory(pBuffer, &dwBufferSize, pCallBack, pCallBackParams, 0, &rcKLHeader, 0)) ) {
					DSKM_CleanupHashingPropsOnObject( hCurrObjectData );
				}

				pCallBack( pCallBackParams, 0, 0, DSKM_CB_CLOSE );
				break;
			}
		}
		hCurrObjectData = DATA_Get_Next( hCurrObjectData, 0 );
	}
}

//! \fn				: DSKM_CheckObjectsUsingRegsSet
//! \brief			:	Проверить объекты, используя подготовленный набор реестров (DSKM_PrepareRegsSet)
//! \return			: Код ошибки 
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки, параметров и ассоциаций
//! \param          : AVP_dword dwObjectsType	- тип объектов
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingRegsSet( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType ) {
	DSKM_TRY {
		AVP_bool bErrProcessed = 0;
		AVP_dword dwResult = DSKM_ERR_NOT_INITIALIZED;

		if ( DSKMAllocator && DSKMAllocator ) {
			dwResult = DSKM_ERR_INVALID_PARAMETER;
			if ( hObjectsList && hDSKM ) {
					
				HDATA hObjects = DSKM_CopyObjectsTree( (HDATA)hObjectsList );
				dwResult = DSKM_ERR_DATA_PROCESSING;
				
				if ( hObjects ) {
					dwResult = DSKM_ERR_OBJECT_NOT_FOUND;

					if ( DATA_Get_First(hObjects, 0) ) {
						HDATA hCurrObjectData = 0;
						HDATA hKLRegRootData = 0;
						HDATA hOPRegRootData = 0;
						{
							MAKE_ADDR1(KLAddr, DSKM_KL_KEY_PID);
							hKLRegRootData = DATA_Find( (HDATA)hDSKM, KLAddr );
						}
						{
							MAKE_ADDR1(OPAddr, DSKM_OP_KEY_HEAD_PID);
							hOPRegRootData = DATA_Find( (HDATA)hDSKM, OPAddr );
						}
						// Проверить объекты по подготовленным реестрам хэшей. Типы берем из каждого объекта.
						hCurrObjectData = DATA_Get_First( hObjects, 0 );
						while ( hCurrObjectData ) {
							HDATA hParamRoot = 0;
							HDATA hWorkObjectData = hCurrObjectData;

							AVP_dword dwSignedBy = 0;
							AVP_dword dwSizeOfSignedBy = sizeof(dwSignedBy);
							DSKM_ParList_GetObjectProp( (HDSKMLIST)hObjects, (HDSKMLISTOBJ)hWorkObjectData, DSKM_OBJ_SIGNED_BY, &dwSignedBy, &dwSizeOfSignedBy );

							hCurrObjectData = DATA_Get_Next( hCurrObjectData, 0 );
							if ( !dwSignedBy ) {
								// Не установлен тип ключа, которым подписан объект
								{
									MAKE_ADDR1(PRAddr, DSKM_PARAM_ROOT_ID);
									hParamRoot = DATA_Find( hWorkObjectData, PRAddr);
								}

								if ( hParamRoot ) {
									HDATA hCollectionData = DATA_Add( 0, 0, DSKM_OBJ_TYPES_PROP_ID, 0, 0 );
									if ( hCollectionData ) {
										HDATA hListObject = DSKM_FindCorrespondingListObject( hWorkObjectData );
										HPROP hCollectionProp = DATA_Find_Prop( hCollectionData, 0, 0 );

										dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, hParamRoot, dwObjectsType, hCollectionData );
										if ( dwObjectsType && PROP_Arr_Count(hCollectionProp) > 0 ) {
											HDATA hHashRegsRootData = 0;
											int i = 0;
											int c = PROP_Arr_Count( hCollectionProp );
											dwResult = DSKM_ERR_REG_NOT_FOUND;
											for ( ; i<c; i++ ) {
												AVP_dword dwCurrObjectType;
												PROP_Arr_Get_Items( hCollectionProp, i, &dwCurrObjectType, sizeof(dwCurrObjectType) );
												{
													MAKE_ADDR1(HSAddr, DSKM_HS_KEY_PID(dwCurrObjectType));
													hHashRegsRootData = DATA_Find( (HDATA)hDSKM, HSAddr );
												}
												if ( hHashRegsRootData ) {
													HDATA hCurrData = DATA_Get_First( hHashRegsRootData, 0 );
													dwResult = DSKM_ERR_OK;
													while ( hCurrData && DSKM_OK(dwResult) ) {
														// Проверить объект по хэшу
														dwResult = DSKM_CheckObjectByHashRegData( hDSKM, hCurrData, hWorkObjectData );
														if ( DSKM_NOT_OK(dwResult) ) {
															// Проверить подписью в самом - операбельными.
															if ( hOPRegRootData ) {
																HDATA hCurrOPData = DATA_Get_First( (HDATA)hOPRegRootData, 0 );
																while ( hCurrOPData ) {
																	HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
																	if ( DSKM_FindKeyData(hParamRoot, dwCurrObjectType, hCurrOPData, hKeysRootData) ) {
																		dwResult = DSKM_CheckObjectByInsideSign( hDSKM, hWorkObjectData, hKeysRootData );
																		if ( DSKM_OK(dwResult) ) {
																			AVP_dword dwSignedBy = DSKM_OBJ_SIGN_INOBJ;
																			DSKM_ParList_SetObjectProp( hObjectsList, (HDSKMLISTOBJ)hListObject, DSKM_OBJ_SIGNED_BY, &dwSignedBy, sizeof(dwSignedBy) );
																		}
																	}
																	DATA_Remove( hKeysRootData, 0 );
																	if ( !DATA_Get_First(hObjects, 0) ) {
																		// Все объекты найдены
																		dwResult = DSKM_ERR_OK;
																		break;
																	}
																	hCurrOPData = DATA_Get_Next( hCurrOPData, 0 );
																}
															}
														}
														else {
															AVP_dword dwSignedBy = DSKM_OBJ_SIGN_INREG;
															DSKM_ParList_SetObjectProp( hObjectsList, (HDSKMLISTOBJ)hListObject, DSKM_OBJ_SIGNED_BY, &dwSignedBy, sizeof(dwSignedBy) );
														}
														if ( !DATA_Get_First(hObjects, 0) )
															// Все объекты найдены
															break;
														hCurrData = DATA_Get_Next( hCurrData, 0 );
														if ( !hCurrData ) {
															// Больше нет реестров заданного типа
															dwResult = DSKM_ERR_NOT_ALL_OBJECTS_FOUND; 
															break;
														}
													}
													if ( !DATA_Get_First(hObjects, 0) ) {
														// Все объекты найдены
														break;
													}
												}
											}
										}
										else {
											DATA_Set_Val( hWorkObjectData, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_INVALID_OBJTYPE, 0 );
											DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
											bErrProcessed = 1;
											DATA_Remove( hWorkObjectData, 0 );
										}

										DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
										bErrProcessed = 1;

										DATA_Remove( hCollectionData, 0 );
									}
								}
							}
						}

						if ( DATA_Get_First(hObjects, 0) ) {
							// Проверить объекты по подготовленным реестрам хэшей. Типы берем из листа.
							HDATA hCollectionData = DATA_Add( 0, 0, DSKM_OBJ_TYPES_PROP_ID, 0, 0 );
							if ( hCollectionData ) {
								HPROP hCollectionProp = DATA_Find_Prop( hCollectionData, 0, 0 );
								dwResult = DSKM_ERR_INVALID_OBJTYPE;
								
								dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, hCollectionData );
								if ( dwObjectsType && PROP_Arr_Count(hCollectionProp) > 0 ) {
									HDATA hHashRegsRootData = 0;
									int i = 0;
									int c = PROP_Arr_Count( hCollectionProp );
									dwResult = DSKM_ERR_REG_NOT_FOUND;
									for ( ; i<c; i++ ) {
										AVP_dword dwCurrObjectType;
										PROP_Arr_Get_Items( hCollectionProp, i, &dwCurrObjectType, sizeof(dwCurrObjectType) );
										{
											MAKE_ADDR1(HSAddr, DSKM_HS_KEY_PID(dwCurrObjectType));
											hHashRegsRootData = DATA_Find( (HDATA)hDSKM, HSAddr );
										}
										if ( hHashRegsRootData ) {
											HDATA hCurrData = DATA_Get_First( hHashRegsRootData, 0 );
											dwResult = DSKM_ERR_OK;
											while ( hCurrData && DSKM_OK(dwResult) ) {
												dwResult = DSKM_CheckObjectsByHashRegData( hDSKM, hObjects, hCurrData );
												if ( !DATA_Get_First(hObjects, 0) )
													// Все объекты найдены
													break;
												hCurrData = DATA_Get_Next( hCurrData, 0 );
												if ( !hCurrData ) {
													// Больше нет реестров заданного типа
													dwResult = DSKM_ERR_NOT_ALL_OBJECTS_FOUND; 
													break;
												}
											}
											DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
											bErrProcessed = 1;

											if ( !DATA_Get_First(hObjects, 0) )
												// Все объекты найдены
												break;
										}
									}
								}
								DATA_Remove( hCollectionData, 0 );
							}
						}
						
						if ( DATA_Get_First(hObjects, 0) ) {
							// Проверить оставшиеся объекты на подпись внутри самих
							dwResult = DSKM_ERR_NOT_ALL_OBJECTS_FOUND; 

							// Замешиваем имя в хэш всегда
							//DSKM_CleanupHashingPropsOnKeyRegs( hDSKM, hObjects );

							if ( DSKM_NOT_OK(dwResult) ) {
								HDATA hCurrObjectData = DATA_Get_First( hObjects, 0 );
								while ( hCurrObjectData ) {
									HDATA hWorkObjectData = hCurrObjectData;
									AVP_dword dwSignedBy = 0;
									AVP_dword dwSizeOfSignedBy = sizeof(dwSignedBy);
									DSKM_ParList_GetObjectProp( (HDSKMLIST)hObjects, (HDSKMLISTOBJ)hWorkObjectData, DSKM_OBJ_SIGNED_BY, &dwSignedBy, &dwSizeOfSignedBy );
									hCurrObjectData = DATA_Get_Next( hCurrObjectData, 0 );
									if ( dwSignedBy ) {
										// Установлен тип ключа, которым подписан объект
										HDATA hListObject = DSKM_FindCorrespondingListObject( hWorkObjectData );
										if ( dwSignedBy == DSKM_OTYPE_KEY_KL_PUBLIC ) {
											// Это KL
											if ( hKLRegRootData ) {
												HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
												if ( DSKM_FindKeyData((HDATA)hObjectsList, DSKM_OTYPE_KEY_OPERABLE_PUBLIC, hKLRegRootData, hKeysRootData) ) {
													dwResult = DSKM_CheckObjectByInsideSign( hDSKM, hWorkObjectData, hKeysRootData );
													if ( DSKM_OK(dwResult) ) {
														AVP_dword dwSignedBy = DSKM_OBJ_SIGN_INOBJ;
														DSKM_ParList_SetObjectProp( 0, (HDSKMLISTOBJ)hListObject, DSKM_OBJ_SIGNED_BY, &dwSignedBy, sizeof(dwSignedBy) );
													}
												}
												DATA_Remove( hKeysRootData, 0 );
											}
										}
										else {
											// Это какой-то операбельный
											if ( hOPRegRootData ) {
												HDATA hCurrOPData = DATA_Get_First( (HDATA)hOPRegRootData, 0 );
												while ( hCurrOPData ) {
													HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
													if ( DSKM_FindKeyData((HDATA)hObjectsList, dwSignedBy, hCurrOPData, hKeysRootData) ) {
														dwResult = DSKM_CheckObjectByInsideSign( hDSKM, hWorkObjectData, hKeysRootData );
														if ( DSKM_OK(dwResult) ) {
															AVP_dword dwSignedBy = DSKM_OBJ_SIGN_INOBJ;
															DSKM_ParList_SetObjectProp( 0, (HDSKMLISTOBJ)hListObject, DSKM_OBJ_SIGNED_BY, &dwSignedBy, sizeof(dwSignedBy) );
															DATA_Remove( hKeysRootData, 0 );
															break;
														}
													}
													DATA_Remove( hKeysRootData, 0 );
													hCurrOPData = DATA_Get_Next( hCurrOPData, 0 );
												}
											}
										}
									}
								}
								if ( !DATA_Get_First(hObjects, 0) ) {
									// Все объекты найдены
									dwResult = DSKM_ERR_OK;
								}
								DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
								bErrProcessed = 1;
							}
							if ( DSKM_NOT_OK(dwResult) ) {
								// Проверить операбельными все, что можно
								if ( hOPRegRootData ) {
									HDATA hCurrOPData = DATA_Get_First( (HDATA)hOPRegRootData, 0 );
									while ( hCurrOPData ) {
										HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
										if ( DSKM_FindKeyData((HDATA)hObjectsList, 0, hCurrOPData, hKeysRootData) ) {
											DSKM_CheckObjectsByInsideSign( hDSKM, hObjects, hKeysRootData );
										}
										DATA_Remove( hKeysRootData, 0 );
										if ( !DATA_Get_First(hObjects, 0) ) {
											// Все объекты найдены
											dwResult = DSKM_ERR_OK;
											break;
										}
										hCurrOPData = DATA_Get_Next( hCurrOPData, 0 );
									}
									DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
									bErrProcessed = 1;
								}
							}
							if ( DSKM_NOT_OK(dwResult) ) {
								// Проверить операбельные KL-ем
								if ( hKLRegRootData ) {
									HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
									if ( DSKM_FindKeyData((HDATA)hObjectsList, DSKM_OTYPE_KEY_OPERABLE_PUBLIC, hKLRegRootData, hKeysRootData) ) {
										DSKM_CheckObjectsByInsideSign( hDSKM, hObjects, hKeysRootData );
									}
									DATA_Remove( hKeysRootData, 0 );
									if ( !DATA_Get_First(hObjects, 0) ) {
										// Все объекты найдены
										dwResult = DSKM_ERR_OK;
									}
									DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
									bErrProcessed = 1;
								}
							}
							if ( DSKM_NOT_OK(dwResult) ) {
								// Проверить KL-ли мастером
								HDATA hKeysRootData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
								dwResult = DSKM_FindKeysTreeInBuffer( hDSKM, 0, DSKM_OTYPE_KEY_KL_PUBLIC, g_pDSKMData, g_dwDSKMDataSize, hKeysRootData );
								if ( DSKM_OK(dwResult) ) {
									dwResult = DSKM_ERR_NOT_ALL_OBJECTS_FOUND; 

									DSKM_CheckObjectsByInsideSign( hDSKM, hObjects, hKeysRootData );
									DATA_Remove( hKeysRootData, 0 );
									if ( !DATA_Get_First(hObjects, 0) ) {
										// Все объекты найдены
										dwResult = DSKM_ERR_OK;
									}
									DSKM_ProcessObjectsError( (HDATA)hObjectsList, hObjects, 0 );
									bErrProcessed = 1;
								}
							}
						}
						
						if ( !DATA_Get_First(hObjects, 0) ) {
							// Все объекты найдены
							dwResult = DSKM_ERR_OK;
						}
					}
					DATA_Remove( hObjects, 0 );
				}
			}
		}
		
		if ( DSKM_NOT_OK(dwResult) && !bErrProcessed )
			DSKM_ProcessObjectsError( (HDATA)hObjectsList, 0, dwResult );
		return dwResult;
	}
	DSKM_EXCEPT
}

//! \fn				:  DSKMAPI DSKM_IsRegsSetPrepared
//! \brief			:	Проверить - подготовлен ли набор реестров к проверке
//! \return			: Код ошибки 
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список объектов для проверки, параметров и ассоциаций
//! \param          : AVP_dword dwObjectsType	- тип объектов
//!										Если dwObjectsType == 0 - тип определяется через ассоциации (если заданы)
//!										Если dwObjectsType != 0 - ищется подготовленный реестр для данного типа 
//!										Если dwObjectsType == 0 - ищется первый подготовленный реестр любого типа 
AVP_dword   DSKMAPI DSKM_IsRegsSetPrepared( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_NOT_INITIALIZED;
		if ( DSKMAllocator && DSKMAllocator ) {
			// Найти реестр ключа ЛК 
			HDATA hKLRegRootData = 0;
			HDATA hOPRegRootData = 0;
			HDATA hTypeRegRootData = 0;
			{
				MAKE_ADDR1(KLAddr, DSKM_KL_KEY_PID);
				hKLRegRootData = DATA_Find( (HDATA)hDSKM, KLAddr );
			}
			{
				MAKE_ADDR1(OPAddr, DSKM_OP_KEY_HEAD_PID);
				hOPRegRootData = DATA_Find( (HDATA)hDSKM, OPAddr );
			}
			dwResult = DSKM_ERR_REGS_NOT_INITIALIZED;
			if ( hKLRegRootData && hOPRegRootData) {
				HDATA hTypeRegRootData = 0;
				if ( !dwObjectsType ) {
					HDATA hCollectionData = DATA_Add( 0, 0, DSKM_OBJ_TYPES_PROP_ID, 0, 0 );
					HPROP hCollectionProp = DATA_Find_Prop( hCollectionData, 0, 0 );
					dwResult = DSKM_ERR_INVALID_OBJTYPE;
					
					dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, hCollectionData );
					if ( dwObjectsType && PROP_Arr_Count(hCollectionProp) > 0 ) {
						int i = 0;
						int c = PROP_Arr_Count( hCollectionProp );
						dwResult = DSKM_ERR_REG_NOT_FOUND;
						for ( ; i<c; i++ ) {
							AVP_dword dwCurrObjectType;
							PROP_Arr_Get_Items( hCollectionProp, i, &dwCurrObjectType, sizeof(dwCurrObjectType) );
							{
							MAKE_ADDR1(HSAddr, DSKM_HS_KEY_PID(dwCurrObjectType));
							hTypeRegRootData = DATA_Find( (HDATA)hDSKM, HSAddr );
							}
							if ( hTypeRegRootData ) {
								break;
							}
						}
					}
					else {
						HDATA hCurrData = DATA_Get_First( (HDATA)hDSKM, 0 );
						while ( hCurrData && DSKM_OK(dwResult) ) {
							if ( DSKM_HS_HID == GET_AVP_PID_APP(DATA_Get_Id(hCurrData, 0)) ) {
								hTypeRegRootData = hCurrData;
								break;
							}
							hCurrData = DATA_Get_Next( hCurrData, 0 );
						}
					}
					DATA_Remove( hCollectionData, 0 );
				}
				else {
					HDATA hTypeRegRootData = 0;
					MAKE_ADDR1(HSAddr, DSKM_HS_KEY_PID(dwObjectsType));
					hTypeRegRootData = DATA_Find( (HDATA)hDSKM, HSAddr );
				}
				if ( hTypeRegRootData )
					dwResult = DSKM_ERR_OK;
			}
		}
		return dwResult;
	}
	DSKM_EXCEPT
}

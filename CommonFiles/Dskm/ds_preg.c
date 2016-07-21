/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_sign.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/6/2005 6:02:42 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"



#if 0
//! \fn				: CompareFunc
//! \brief			:	
//! \return			: static int 
//! \param          :  DWORD pA
//! \param          : DWORD pB
//! \param          : HDATA hUnsorted
static int CompareFunc( DWORD pA, DWORD pB, HDATA hUnsorted ) {
	int iResult = 0;
	if ( pA != pB ) {
		HDATA hDataA = 0;
		HDATA hDataB = 0;
		{
			MAKE_ADDR1(USAddrA, DSKM_OBJECT_REG_HASH_ID(pA));
			hDataA = DATA_Find(hUnsorted, USAddrA );
		}
		{
			MAKE_ADDR1(USAddrB, DSKM_OBJECT_REG_HASH_ID(pB));
			hDataB = DATA_Find(hUnsorted, USAddrB );
		}
		if ( hDataA && hDataB ) {
			AVP_dword dwHashSizeA = DATA_Get_Val( hDataA, 0, 0, 0, 0 );
			AVP_dword dwHashSizeB = DATA_Get_Val( hDataB, 0, 0, 0, 0 );
			if ( dwHashSizeA == dwHashSizeB ) {
				void *pHashA = DSKMAllocator( dwHashSizeA );
				void *pHashB = DSKMAllocator( dwHashSizeB );
				if ( pHashA && pHashB )	{
					DATA_Get_Val( hDataA, 0, 0, pHashA, dwHashSizeA );
					DATA_Get_Val( hDataB, 0, 0, pHashB, dwHashSizeB );
					iResult = ds_mcmp( pHashA, pHashB, dwHashSizeA );
				}
				if ( pHashA )	
					DSKMLiberator( pHashA );
				if ( pHashB )	
					DSKMLiberator( pHashB );
			}
		}
	}
	return iResult;
}


//! \fn				: void SwapItems
//! \brief			:	
//! \return			: static inline 
//! \param          :  DWORD *pA
//! \param          : DWORD *pB
static void SwapItems( DWORD *pA, DWORD *pB ) {
	*pA ^= *pB; *pB ^= *pA; *pA ^= *pB;
}


// ---
static void QuickSortRecursive( DWORD *pBase, int nLow, int nHight, HDATA hUnsorted ) {
	int   i,j;
	DWORD mod;
	
	i = nHight;
	j = nLow;
	
	mod = pBase[((int) ((nLow+nHight) / 2))];
	
	do {
		
		while ( CompareFunc(pBase[j], mod, hUnsorted) < 0 ) j++;
		while ( CompareFunc(pBase[i], mod, hUnsorted) > 0 ) i--;
		
		if ( i >= j ) {
			
			if ( i != j ) 
				SwapItems( &pBase[i], &pBase[j] );
			
			i--;
			j++;
		}
	} while (j <= i);
	
	if (nLow < i		 ) QuickSortRecursive( pBase, nLow, i     , hUnsorted );
	if (j		 < nHight) QuickSortRecursive( pBase, j   , nHight, hUnsorted );
}


//! \fn				: SortIndexArray
//! \brief			:	
//! \return			: static void 
//! \param          :  DWORD *pBase
//! \param          : int nHight
//! \param          : HDATA hUnsorted
static void SortIndexArray( DWORD *pBase, int nHight, HDATA hUnsorted ) {
	QuickSortRecursive( pBase, 0, nHight - 1, hUnsorted );
}




//! \fn				: SortHashTree
//! \brief			:	
//! \return			: static void 
//! \param          :  HDATA hUnsorted
//! \param          : HDATA hSorted
static void SortHashTree( HDATA hUnsorted, HDATA hSorted ) {
	int i = 0;
	AVP_dword dwID = 1;
	DWORD *pIndexArray = 0;
	HDATA hCurrData = DATA_Get_Last( hUnsorted, 0 );
	if ( hCurrData )
		dwID = GET_AVP_PID_ID(DATA_Get_Id(hCurrData, 0));

	pIndexArray = (DWORD *)DSKMAllocator( dwID * sizeof(DWORD) );
	for( ; i<(int)dwID; i++ )
		pIndexArray[i] = i + 1;
	
	SortIndexArray( pIndexArray, dwID, hUnsorted );
	
	for( i=0; i<(int)dwID; i++ ) {
		MAKE_ADDR1(USAddr, DSKM_OBJECT_REG_HASH_ID(pIndexArray[i]));
		hCurrData = DATA_Find( hUnsorted, USAddr );
		if ( hCurrData ) {
			DATA_Detach( hCurrData, 0 );
			DATA_Replace_ID( hCurrData, 0, (AVP_word)(i + 1) );
			DATA_Attach( hSorted, 0, hCurrData, DATA_IF_ROOT_INCLUDE );
		}
	}

	DSKMLiberator( pIndexArray );
}
#endif


#if 1
//! \fn				: DSKM_PrepareHashReg
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : HDATA hRegData
static 	AVP_dword DSKMAPI DSKM_PrepareHashReg( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hRegData ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_OK;
		HDATA hCurrData;
		AVP_dword dwID = 1;
		HPROP hArrPop = 0;
		
		hCurrData = DATA_Get_Last( hRegData, 0 );
		if ( hCurrData )
			dwID = GET_AVP_PID_ID(DATA_Get_Id(hCurrData, 0)) + 1;

		hArrPop = DATA_Find_Prop( hRegData, 0, DSKM_OBJECT_REG_ARR_HASH_ID );
		if ( !hArrPop ) {
			hArrPop = DATA_Add_Prop( hRegData, 0, DSKM_OBJECT_REG_ARR_HASH_ID, 0, 0 );
		}

		if ( !hArrPop ) {
			dwResult = DSKM_ERR_DATA_PROCESSING;
		}

		if ( DSKM_OK(dwResult) ) {
			hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
			while ( hCurrData && DSKM_OK(dwResult) ) {
				void *pObjectHash = 0;
				AVP_dword dwObjectHashSize = 0;
				if ( DSKM_OK(dwResult = DSKM_PrepareObjectHash(hDSKM, hCurrData, &pObjectHash, &dwObjectHashSize)) ) {
					PROP_Arr_Insert( hArrPop, PROP_ARR_LAST, pObjectHash, dwObjectHashSize );
					DSKMLiberator( pObjectHash );
				}
				if ( dwResult == DSKM_ERR_INVALID_OBJTYPE || dwResult == DSKM_ERR_HASH_NOT_FOUND )
					dwResult = DSKM_ERR_OK;

				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
		}
		return dwResult;
	}
	DSKM_EXCEPT
}
#endif



#if 0
//! \fn				: DSKM_PrepareHashReg
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : HDATA hRegData
static 	AVP_dword DSKMAPI DSKM_PrepareHashReg( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hRegData ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_OK;
		HDATA hCurrData;
		AVP_dword dwID = 1;
		
		hCurrData = DATA_Get_Last( hRegData, 0 );
		if ( hCurrData )
			dwID = GET_AVP_PID_ID(DATA_Get_Id(hCurrData, 0)) + 1;

		hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
		while ( hCurrData && DSKM_OK(dwResult) ) {
			void *pObjectHash = 0;
			AVP_dword dwObjectHashSize = 0;
			if ( DSKM_OK(dwResult = DSKM_PrepareObjectHash(hDSKM, hCurrData, &pObjectHash, &dwObjectHashSize)) ) {
				dwResult = DATA_Add(hRegData, 0, DSKM_OBJECT_REG_HASH_ID(dwID++), (AVP_size_t)pObjectHash, dwObjectHashSize)
					         ? DSKM_ERR_OK
									 : DSKM_ERR_DATA_PROCESSING;
				DSKMLiberator( pObjectHash );
			}
			if ( dwResult == DSKM_ERR_INVALID_OBJTYPE || dwResult == DSKM_ERR_HASH_NOT_FOUND )
				dwResult = DSKM_ERR_OK;

			hCurrData = DATA_Get_Next( hCurrData, 0 );
		}
/*
		if ( DSKM_OK(dwResult) ) {
			HDATA hSorted = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			SortHashTree( hRegData, hSorted );
			DATA_Copy( hRegData, 0, hSorted, DATA_IF_ROOT_EXCLUDE );
			DATA_Remove( hSorted, 0 );
		}
*/
		return dwResult;
	}
	DSKM_EXCEPT
}
#endif


//! \fn				:  DSKMAPI DSKM_SignObjectsToRegFileByKeyImp
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
//! \param          : void *pPrivateKey
//! \param          : AVP_dword dwKeySize
//! \param          : const AVP_char* pObjsRegFileName
static AVP_dword   DSKMAPI DSKM_SignObjectsToRegFileByKeyImp( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void *pPrivateKey, AVP_dword dwKeySize, const AVP_char* pObjsRegFileName ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_UNKNOWN_ERROR;

		{
			HDATA hRegData = 0;//DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			dwResult = DSKM_DeserializeRegFile( hDSKM, pObjsRegFileName, 0, &hRegData );
			if ( dwResult == DSKM_ERR_CANNOT_OPEN_REG_FILE ) {
				hRegData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
				if ( hRegData )
					dwResult = DSKM_ERR_OK;
			}
			else {
				if ( hRegData ) {
					HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_OBJTYPE_ID );
					if ( hProp ) {
						AVP_dword dwRegObjsType = 0;
						PROP_Get_Val( hProp, &dwRegObjsType, sizeof(dwRegObjsType) );
						if ( dwRegObjsType != dwObjectsType )
							dwResult = DSKM_ERR_INVALID_OBJTYPE;
					}
				}
			}
			if ( DSKM_OK(dwResult) ) 
				dwResult = DSKM_PrepareHashReg( hDSKM, hObjectsList, hRegData );
			if ( DSKM_OK(dwResult) ) {
				DSKM_SetIsSpecialProp( hRegData, bSpecialReg );
				DSKM_SetObjTypeProp( hRegData, dwObjectsType );

				dwResult = DSKM_SerializeRegToFile( hDSKM, dwObjectsType, hRegData, pObjsRegFileName );

				if ( DSKM_OK(dwResult) )
					dwResult = DSKM_SignFile( hDSKM, pObjsRegFileName, pPrivateKey, dwKeySize );

				DATA_Remove( hRegData, 0 );
			}
		}

		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				:  DSKMAPI DSKM_SignObjectsToRegFileByKey
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
//! \param          : void *pvKeyRegBuffer
//! \param          : AVP_dword dwKeyRegBufferSize
//! \param          : const AVP_char* pObjsRegFileName
AVP_dword   DSKMAPI DSKM_SignObjectsToRegFileByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize, const AVP_char* pObjsRegFileName ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			void *pPrivateKey = 0;
			AVP_dword dwPrivateKeySize;
			
			dwResult = DSKM_FindKeyInBuffer( hDSKM, hObjectsList, 0, pvKeyRegBuffer, dwKeyRegBufferSize, &pPrivateKey, &dwPrivateKeySize );
			if ( DSKM_OK(dwResult) ) {
				dwResult = DSKM_ERR_INVALID_OBJTYPE;
				dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, 0 );
				if ( dwObjectsType ) {
					dwResult = DSKM_SignObjectsToRegFileByKeyImp( hDSKM, hObjectsList, dwObjectsType, bSpecialReg, pPrivateKey, dwPrivateKeySize, pObjsRegFileName );
				}
			}
			else {
				if ( dwResult == DSKM_ERR_INVALID_BUFFER ) {
					dwResult = DSKM_SignObjectsToRegFileByKeyImp( hDSKM, hObjectsList, dwObjectsType, bSpecialReg, pvKeyRegBuffer, dwKeyRegBufferSize, pObjsRegFileName );
				}
			}
			
			if ( pPrivateKey ) {
				DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
				DSKMLiberator( pPrivateKey );
			}
		}
		
		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				:  DSKMAPI DSKM_SignObjectsToRegBufferByKeyImp
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
//! \param          : void *pPrivateKey
//! \param          : AVP_dword dwKeySize
//! \param          : void **pRegBuffer
//! \param          : AVP_dword *pdwRegBufferSize
static AVP_dword   DSKMAPI DSKM_SignObjectsToRegBufferByKeyImp( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void *pPrivateKey, AVP_dword dwKeySize, void **pRegBuffer, AVP_dword *pdwRegBufferSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_UNKNOWN_ERROR;
		
		{
			HDATA hRegData = 0;//DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			if ( *pRegBuffer && *pdwRegBufferSize ) {
				dwResult = DSKM_DeserializeRegBuffer( hDSKM, *pRegBuffer, *pdwRegBufferSize, 0, 0, 0, &hRegData );
				if ( DSKM_OK(dwResult) && hRegData ) {
					HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_OBJTYPE_ID );
					if ( hProp ) {
						AVP_dword dwRegObjsType = 0;
						PROP_Get_Val( hProp, &dwRegObjsType, sizeof(dwRegObjsType) );
						if ( dwRegObjsType != dwObjectsType )	{
							dwResult = DSKM_ERR_INVALID_OBJTYPE;
						}
					}
				}
			}
			else {
				hRegData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
				dwResult = DSKM_ERR_OK;
			}
			if ( DSKM_OK(dwResult) ) 
				dwResult = DSKM_PrepareHashReg( hDSKM, hObjectsList, hRegData );
			if ( DSKM_OK(dwResult) ) {
				if ( *pRegBuffer ) {
					DSKMLiberator( *pRegBuffer );
					*pRegBuffer = 0;
					*pdwRegBufferSize = 0;
				}
				
				DSKM_SetIsSpecialProp( hRegData, bSpecialReg );
				DSKM_SetObjTypeProp( hRegData, dwObjectsType );
				
				*pdwRegBufferSize = DSKM_SerializeRegToBuffer( hDSKM, dwObjectsType, hRegData, 0, 0 );
				if ( *pdwRegBufferSize ) {
					*pRegBuffer = DSKMAllocator( *pdwRegBufferSize );
					*pdwRegBufferSize = DSKM_SerializeRegToBuffer( hDSKM, dwObjectsType, hRegData, *(AVP_byte **)pRegBuffer, *pdwRegBufferSize );

					dwResult = DSKM_SignBuffer( hDSKM, pRegBuffer, pdwRegBufferSize, pPrivateKey, dwKeySize );
				}
				else 
					dwResult = DSKM_ERR_CANNOT_WRITE_REG_TOMEM;
				
				DATA_Remove( hRegData, 0 );
			}
		}
			
		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				:  DSKMAPI DSKM_SignObjectsToRegBufferByKey
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
//! \param          : void *pvKeyRegBuffer
//! \param          : AVP_dword dwKeyRegBufferSize
//! \param          : void **pRegBuffer
//! \param          : AVP_dword *pdwRegBufferSize
AVP_dword   DSKMAPI DSKM_SignObjectsToRegBufferByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize, void **pRegBuffer, AVP_dword *pdwRegBufferSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			void *pPrivateKey = 0;
			AVP_dword dwPrivateKeySize;
			
			dwResult = DSKM_FindKeyInBuffer( hDSKM, hObjectsList, 0, pvKeyRegBuffer, dwKeyRegBufferSize, &pPrivateKey, &dwPrivateKeySize );
			if ( DSKM_OK(dwResult) ) {
				dwResult = DSKM_ERR_INVALID_OBJTYPE;
				dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, 0 );
				if ( dwObjectsType ) {
					dwResult = DSKM_SignObjectsToRegBufferByKeyImp( hDSKM, hObjectsList, dwObjectsType, bSpecialReg, pPrivateKey, dwPrivateKeySize, pRegBuffer, pdwRegBufferSize );
				}
			}
			else {
				if ( dwResult == DSKM_ERR_INVALID_BUFFER ) {
					dwResult = DSKM_SignObjectsToRegBufferByKeyImp( hDSKM, hObjectsList, dwObjectsType, bSpecialReg, pvKeyRegBuffer, dwKeyRegBufferSize, pRegBuffer, pdwRegBufferSize );
				}
			}
			
			if ( pPrivateKey ) {
				DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
				DSKMLiberator( pPrivateKey );
			}
		}
		
		return dwResult;
	}
	DSKM_EXCEPT
}

			


//! \fn				:  DSKMAPI DSKM_SignObjectsToRegFileByKeyReg
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
//! \param          : const AVP_char* pKeyRegFileName
//! \param          : const AVP_char* pObjsRegFileName
AVP_dword   DSKMAPI DSKM_SignObjectsToRegFileByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, const AVP_char* pKeyRegFileName, const AVP_char* pObjsRegFileName ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}

		if ( hObjectsList && hDSKM ) {
			void *pPrivateKey = 0;
			AVP_dword dwPrivateKeySize;

			dwResult = DSKM_FindKeyInFile( hDSKM, hObjectsList, 0, pKeyRegFileName, &pPrivateKey, &dwPrivateKeySize );
			if ( DSKM_OK(dwResult) ) {
				dwResult = DSKM_ERR_INVALID_OBJTYPE;
				dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, 0 );
				if ( dwObjectsType ) {
					dwResult = DSKM_SignObjectsToRegFileByKeyImp( hDSKM, hObjectsList, dwObjectsType, bSpecialReg, pPrivateKey, dwPrivateKeySize, pObjsRegFileName );
				}
			}
			
			if ( pPrivateKey ) {
				DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
				DSKMLiberator( pPrivateKey );
			}
		}
			
		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				:  DSKMAPI DSKM_SignObjectsToRegBufferByKeyReg
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
//! \param          : const AVP_char* pKeyRegFileName
//! \param          : void **pRegBuffer
//! \param          : AVP_dword *pdwRegBufferSize
AVP_dword   DSKMAPI DSKM_SignObjectsToRegBufferByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, const AVP_char* pKeyRegFileName, void **pRegBuffer, AVP_dword *pdwRegBufferSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			void *pPrivateKey = 0;
			AVP_dword dwPrivateKeySize;
			
			dwResult = DSKM_FindKeyInFile( hDSKM, hObjectsList, dwObjectsType, pKeyRegFileName, &pPrivateKey, &dwPrivateKeySize );
			if ( DSKM_OK(dwResult) ) {
				dwResult = DSKM_ERR_INVALID_OBJTYPE;
				dwObjectsType = DSKM_CheckObjectTypeByAssoc( hDSKM, hObjectsList, (HDATA)hObjectsList, dwObjectsType, 0 );
				if ( dwObjectsType ) {
					dwResult = DSKM_SignObjectsToRegBufferByKeyImp( hDSKM, hObjectsList, dwObjectsType, bSpecialReg, pPrivateKey, dwPrivateKeySize, pRegBuffer, pdwRegBufferSize );
				}
			}
			
			if ( pPrivateKey ) {
				DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
				DSKMLiberator( pPrivateKey );
			}
		}
			
		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				: DSKM_HashObjectsToRegBuffer
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
//! \param          : void **ppvHashRegBuffer
//! \param          : AVP_dword *pdwHashRegBufferSize
AVP_dword	DSKMAPI DSKM_HashObjectsToRegBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void **ppvHashRegBuffer, AVP_dword *pdwHashRegBufferSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			
			HDATA hRegData = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
			dwResult = DSKM_PrepareHashReg( hDSKM, hObjectsList, hRegData );
			if ( DSKM_OK(dwResult) ) {
				DSKM_SetIsSpecialProp( hRegData, bSpecialReg );
				DSKM_SetObjTypeProp( hRegData, dwObjectsType );
				
				*pdwHashRegBufferSize = DSKM_SerializeRegToBuffer( hDSKM, dwObjectsType, hRegData, 0, 0 );
				if ( *pdwHashRegBufferSize ) {
					*ppvHashRegBuffer = DSKMAllocator( *pdwHashRegBufferSize );
					*pdwHashRegBufferSize = DSKM_SerializeRegToBuffer( hDSKM, dwObjectsType, hRegData, *(AVP_byte **)ppvHashRegBuffer, *pdwHashRegBufferSize );
					dwResult = DSKM_ERR_OK;
				}
				else 
					dwResult = DSKM_ERR_CANNOT_WRITE_REG_TOMEM;
				
				DATA_Remove( hRegData, 0 );
			}
		}
		
		return dwResult;
	}
	DSKM_EXCEPT
}





//! \fn				: DSKM_PrepareAndSignHash
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : void *pPrivateKey
//! \param          : AVP_dword dwKeySize
//! \param          : void **ppSignBuffer
//! \param          : AVP_dword *pdwSignBufferSize
static 	AVP_dword DSKMAPI DSKM_PrepareAndSignHash( HDSKM hDSKM, HDSKMLIST hObjectsList, void *pPrivateKey, AVP_dword dwKeySize, void **ppSignBuffer, AVP_dword *pdwSignBufferSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( hObjectsList ) {
			AVP_bool bFound = 0;
			HDATA hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
			while ( hCurrData && !bFound ) {
				AVP_dword dwPID;
				
				dwPID = DATA_Get_Id( hCurrData, 0 );
				dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
				switch ( dwPID ) {
					case DSKM_OBJECT_NAME_ID(0) : {
						AVP_char *pObjectFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, 0), 0 );
						DSKM_FTABLE_ADD(hCurrData);
						if ( pObjectFileName ) {
							void *pObjectHash = 0;
							AVP_dword dwObjectHashSize = 0;
							if ( DSKM_OK(dwResult = DSKM_HashObjectByNameCheckSignImp(hDSKM, pObjectFileName, &pObjectHash, &dwObjectHashSize, hCurrData, TRUE)) ) {
								DSKM_HashAdditionObjectParams( hDSKM, hCurrData, pObjectHash, dwObjectHashSize );
								if ( ppSignBuffer && pdwSignBufferSize ) {
									dwResult = DSKM_SignHash( hDSKM, pObjectHash, dwObjectHashSize, pPrivateKey, dwKeySize, ppSignBuffer, pdwSignBufferSize );
								}
								else {
									void *pSignBuffer = 0;
									AVP_dword dwSignBufferSize = 0;
									dwResult = DSKM_SignHash( hDSKM, pObjectHash, dwObjectHashSize, pPrivateKey, dwKeySize, &pSignBuffer, &dwSignBufferSize );
									if ( DSKM_OK(dwResult) ) {
										dwResult = DSKM_SignFileBySign( hDSKM, pObjectFileName, pSignBuffer, dwSignBufferSize );
										DSKMLiberator( pSignBuffer );
									}
								}
								DSKMLiberator( pObjectHash );
							}
							DSKMLiberator( pObjectFileName );

							bFound = 1;
						}
						break;
					}
					case DSKM_OBJECT_BUFF_PTR_ID(0) : {
						void *pObjectHash = 0;
						AVP_dword dwObjectHashSize = 0;
						
						void *pBuffer;
						AVP_dword dwBufferSize;
						pfnDSKM_GetBuffer_CallBack pCallBack;
						void *pCallBackParams;
						
						DATA_Get_Val( hCurrData, 0, 0, &pBuffer, sizeof(pBuffer) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
						
						DSKM_FTABLE_ADD(hCurrData);

						if ( DSKM_OK(dwResult=DSKM_HashObjectByBufferCheckSignImp(hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, &pObjectHash, &dwObjectHashSize, hCurrData, TRUE)) ) {
							DSKM_HashAdditionObjectParams( hDSKM, hCurrData, pObjectHash, dwObjectHashSize );
							dwResult = DSKM_SignHash( hDSKM, pObjectHash, dwObjectHashSize, pPrivateKey, dwKeySize, ppSignBuffer, pdwSignBufferSize );
							
							bFound = 1;
						}
						if ( pObjectHash )
							DSKMLiberator( pObjectHash );
						break;
					}
					case DSKM_OBJECT_HASH_ID(0) : {
						AVP_dword dwObjectHashSize = DATA_Get_Val( hCurrData, 0, 0, 0, 0 );
						if ( dwObjectHashSize ) {
							void *pObjectHash = DSKMAllocator( dwObjectHashSize );;
							if ( pObjectHash ) {
								DATA_Get_Val( hCurrData, 0, 0, pObjectHash, dwObjectHashSize );
								DSKM_HashAdditionObjectParams( hDSKM, hCurrData, pObjectHash, dwObjectHashSize );
								dwResult = DSKM_SignHash( hDSKM, pObjectHash, dwObjectHashSize, pPrivateKey, dwKeySize, ppSignBuffer, pdwSignBufferSize );
								
								bFound = 1;

								DSKMLiberator( pObjectHash );
							}
						}
						break;
					}
				}
				
				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
		}

		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				:  DSKMAPI DSKM_SignOneObjectToObjectByKey
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : void *pvKeyRegBuffer
//! \param          : AVP_dword dwKeyRegBufferSize
AVP_dword   DSKMAPI DSKM_SignOneObjectToObjectByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	
	if ( !DSKMAllocator || !DSKMAllocator ) {
    return DSKM_ERR_NOT_INITIALIZED;
	}
	
	if ( hObjectsList && hDSKM ) {
		void *pPrivateKey = 0;
		AVP_dword dwPrivateKeySize;
		
		dwResult = DSKM_FindKeyInBuffer( hDSKM, hObjectsList, dwObjectsType, pvKeyRegBuffer, dwKeyRegBufferSize, &pPrivateKey, &dwPrivateKeySize );
		if ( DSKM_OK(dwResult) ) {
			dwResult = DSKM_PrepareAndSignHash( hDSKM, hObjectsList, pPrivateKey, dwPrivateKeySize, 0, 0 );
		}
		else {
			if ( dwResult == DSKM_ERR_INVALID_BUFFER ) {
				dwResult = DSKM_PrepareAndSignHash( hDSKM, hObjectsList, pvKeyRegBuffer, dwKeyRegBufferSize, 0, 0 );
			}
		}
		
		if ( pPrivateKey ) {
			DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
			DSKMLiberator( pPrivateKey );
		}
	}
		
	return dwResult;
}





//! \fn				:  DSKMAPI DSKM_SignOneObjectToSignBufferByKey
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : void *pvKeyRegBuffer
//! \param          : AVP_dword dwKeyRegBufferSize
//! \param          : void **pSignBuffer
//! \param          : AVP_dword *dwSignBufferSize
AVP_dword   DSKMAPI DSKM_SignOneObjectToSignBufferByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize, void **pSignBuffer, AVP_dword *dwSignBufferSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	
	if ( !DSKMAllocator || !DSKMAllocator ) {
    return DSKM_ERR_NOT_INITIALIZED;
	}
	
		if ( hObjectsList && hDSKM ) {
		void *pPrivateKey = 0;
		AVP_dword dwPrivateKeySize;
		
		dwResult = DSKM_FindKeyInBuffer( hDSKM, hObjectsList, dwObjectsType, pvKeyRegBuffer, dwKeyRegBufferSize, &pPrivateKey, &dwPrivateKeySize );
		if ( DSKM_OK(dwResult) ) {
			dwResult = DSKM_PrepareAndSignHash( hDSKM, hObjectsList, pPrivateKey, dwPrivateKeySize, pSignBuffer, dwSignBufferSize );
		}
		else {
			if ( dwResult == DSKM_ERR_INVALID_BUFFER ) {
				dwResult = DSKM_PrepareAndSignHash( hDSKM, hObjectsList, pvKeyRegBuffer, dwKeyRegBufferSize, pSignBuffer, dwSignBufferSize );
			}
		}
		
		if ( pPrivateKey ) {
			DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
			DSKMLiberator( pPrivateKey );
		}
	}

	return dwResult;
}




//! \fn				:  DSKMAPI DSKM_SignOneObjectToObjectByKeyReg
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pKeyRegFileName
AVP_dword   DSKMAPI DSKM_SignOneObjectToObjectByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pKeyRegFileName ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			void *pPrivateKey = 0;
			AVP_dword dwPrivateKeySize;
			
			dwResult = DSKM_FindKeyInFile( hDSKM, hObjectsList, dwObjectsType, pKeyRegFileName, &pPrivateKey, &dwPrivateKeySize );
			if ( DSKM_OK(dwResult) )
				dwResult = DSKM_PrepareAndSignHash( hDSKM, hObjectsList, pPrivateKey, dwPrivateKeySize, 0, 0 );
			
			if ( pPrivateKey ) {
				DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
				DSKMLiberator( pPrivateKey );
			}
		}
			
		return dwResult;
	}
	DSKM_EXCEPT
}




//! \fn				:  DSKMAPI DSKM_SignOneObjectToSignBufferByKeyReg
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMLIST hObjectsList
//! \param          : AVP_dword dwObjectsType
//! \param          : const AVP_char* pKeyRegFileName
//! \param          : void **pSignBuffer
//! \param          : AVP_dword *dwSignBufferSize
AVP_dword   DSKMAPI DSKM_SignOneObjectToSignBufferByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pKeyRegFileName, void **pSignBuffer, AVP_dword *dwSignBufferSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			void *pPrivateKey = 0;
			AVP_dword dwPrivateKeySize;
			
			dwResult = DSKM_FindKeyInFile( hDSKM, hObjectsList, dwObjectsType, pKeyRegFileName, &pPrivateKey, &dwPrivateKeySize );
			if ( DSKM_OK(dwResult) )
				dwResult = DSKM_PrepareAndSignHash( hDSKM, hObjectsList, pPrivateKey, dwPrivateKeySize, pSignBuffer, dwSignBufferSize );

			if ( pPrivateKey ) {
				DSKM_RandomFillBuffer( hDSKM, pPrivateKey, dwPrivateKeySize );
				DSKMLiberator( pPrivateKey );
			}
		}
			
		return dwResult;
	}
	DSKM_EXCEPT
}


//! \fn				:  DSKMAPI DSKM_SignOneObjectToObjectBySignBuffer
//! \brief			:	Подписать один объект (первый в списке), используя переданную подпись.
//!               Соответствие подписи и объекта НЕ контролируется
//!               Если объект является файлом, то подпись к нему дописывается
//!               Если объект является буфером, то выдается новый буфер, как "старый+подпись"
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : void *pvSignBuffer - буфер с подписью объекта 
//! \param          : AVP_dword dwSignBufferSize - размер буфера с подписью
//! \param          : void **ppvSignedBuffer - подписанный буфер (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwSignedBufferSize - размер подписанного буфера
AVP_dword   DSKMAPI DSKM_SignOneObjectToObjectBySignBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, void *pvSignBuffer, AVP_dword dwSignBufferSize, void **ppvSignedBuffer, AVP_dword *pdwSignedBufferSize ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hObjectsList && hDSKM ) {
			AVP_bool bFound = 0;
			HDATA hCurrData = DATA_Get_First( (HDATA)hObjectsList, 0 );
			while ( hCurrData && !bFound ) {
				AVP_dword dwPID;
				
				dwPID = DATA_Get_Id( hCurrData, 0 );
				dwPID = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
				switch ( dwPID ) {
					case DSKM_OBJECT_NAME_ID(0) : {
						AVP_char *pObjectFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, 0), 0 );
						if ( pObjectFileName ) {
							dwResult = DSKM_SignFileBySign( hDSKM, pObjectFileName, pvSignBuffer, dwSignBufferSize );
								
							DSKMLiberator( pObjectFileName );
						}
						bFound = 1;
						break;
					}
					case DSKM_OBJECT_BUFF_PTR_ID(0) : {
						void *pObjectHash = 0;
						AVP_dword dwObjectHashSize = 0;
						
						void *pBuffer;
						AVP_dword dwBufferSize;
						pfnDSKM_GetBuffer_CallBack pCallBack;
						void *pCallBackParams;
						
						DATA_Get_Val( hCurrData, 0, 0, &pBuffer, sizeof(pBuffer) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
						DATA_Get_Val( hCurrData, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
						
						dwResult = DSKM_SignBufferBySign( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, pvSignBuffer, dwSignBufferSize, ppvSignedBuffer, pdwSignedBufferSize );
							
						bFound = 1;
						break;
					}
				}
				
				hCurrData = DATA_Get_Next( hCurrData, 0 );
			}
		}
	
		return dwResult;
	}
	DSKM_EXCEPT
}

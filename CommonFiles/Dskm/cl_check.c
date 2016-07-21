/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	cl_check.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/15/2005 1:32:16 PM
*		
*		Example:	
*		
*		
*		
*/		

#include "dskm.h"
#include "dskmdefs.h"
#include "../_AVPIO.h"
#include "dskmi.h"

#include "cl_util.h"

#define BUFFER_SIZE (0xffff / 2)


//! \fn				: DSKMAPI DSKM_CheckSignByKeysTree
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : DSKMSign *pSign
//! \param          : DSKMHash *pHash
//! \param          : HDATA hKeysData
static AVP_dword DSKMAPI DSKM_CheckSignByKeysTree( HDSKM hDSKM, DSKMSign *pSign, DSKMHash *pHash, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_SIGN_NOT_FOUND;
	if ( IS_END_OF_SIGN(&pSign->end) ) {
		HDATA hCurrData = DATA_Get_First( hKeysData, 0 );
		HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID );
		if ( !hProp ) {
			DATA_Add_Prop( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_INVALID_SIGN, 0 );
		}
		// Первый это таблица!
		hCurrData = DATA_Get_Next( hCurrData, 0 );
		while ( DSKM_NOT_OK((DATA_Get_Val((HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID, &dwResult, sizeof(dwResult)),dwResult)) && 
			      hCurrData ) {
			AVP_dword dwPublicKeySize = DATA_Get_Val( hCurrData, 0, 0, 0, 0 );
			if ( dwPublicKeySize ) {
				void *pPublicKey = DSKMAllocator( dwPublicKeySize );
				if ( pPublicKey ) {
					DATA_Get_Val( hCurrData, 0, 0, pPublicKey, dwPublicKeySize );
					DSCL_CALL_CHECK_SIGN(dwResult, hDSKM, pSign, pHash, pPublicKey, dwPublicKeySize);
					DSKMLiberator( pPublicKey );
				}
			}
/* TEST
			if ( DSKM_OK((DATA_Get_Val((HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID, &dwResult, sizeof(dwResult)),dwResult)) )
				break;
*/
			hCurrData = DATA_Get_Next( hCurrData, 0 );
		}
/* TEST
		if ( hCurrData ) {
			HPROP hCurrRegProp = 0;
			AVP_dword dwRegObjType = 0;
			DATA_Get_Val( hCurrData, 0, DSKM_KEYREG_OBJTYPE_ID, &dwRegObjType, sizeof(dwRegObjType) );
			hCurrRegProp = DATA_Get_First_Prop( hCurrData, 0 );
			while ( hCurrRegProp ) {
				AVP_dword dwCurrPID;
				dwCurrPID = PROP_Get_Id( hCurrRegProp );
				switch ( GET_AVP_PID_APP(dwCurrPID) ) {
					case DSKM_APP_DWORD_PARAM : {
						AVP_dword dwRegValue;
						PROP_Get_Val( hCurrRegProp, &dwRegValue, sizeof(dwRegValue) );
						break;
					}
					case DSKM_APP_STR_PARAM : {
						AVP_dword dwRegStrSize = 0;
						AVP_char *pRegStr = DSKM_GetPropValueAsString( hCurrRegProp, &dwRegStrSize );
						DSKMLiberator( pRegStr );
						break;
					}
					case DSKM_APP_BPTR_PARAM : {
						void *pRegBuffer;
						AVP_dword dwRegBufferSize;

						dwRegBufferSize = PROP_Get_Val( hCurrRegProp, 0, 0 );
						pRegBuffer = DSKMAllocator( dwRegBufferSize );
						if ( pRegBuffer ) {
							PROP_Get_Val( hCurrRegProp, pRegBuffer, dwRegBufferSize );
							DSKMLiberator(pRegBuffer);
						}
						break;
					}
				}
				hCurrRegProp = PROP_Get_Next( hCurrRegProp );
			}
			return dwResult;
		}
*/
	}
	return dwResult;
}



//! \fn				: DSKM_HashBufferFindSign
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pCallBack
//! \param          : void *pCallBackParams
//! \param          : DSKMHash *prcHash
//! \param          : DSKMSign *prcSign
//! \param          : HDATA hKeysData
AVP_dword DSKMAPI DSKM_HashBufferFindSign( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, DSKMHash *prcHash, DSKMSign *prcSign, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_BUFFER;
	if ( !hKeysData )
		hKeysData = (HDATA)hDSKM; 
	if ( pBuffer && dwBufferSize ) {
		AVP_size_t dwParam1;
		AVP_size_t dwParam2;

		ds_mset( prcSign, 0, sizeof(*prcSign) );
				
		if ( pCallBack ) {
			if ( pCallBack(pCallBackParams, 0, 0, DSKM_CB_OPEN) >= 0 ) {
				int nResult = 0;
				int dwcbRead;

				dwParam1 = (AVP_size_t)prcHash;
				DSCL_CALL_HASH_INIT(nResult, hDSKM, dwParam1/*prcHash*/, 0 );
				
				dwResult = DSKM_ERR_SIGN_NOT_FOUND;
				
				nResult = !(dwcbRead = pCallBack(pCallBackParams, pBuffer, dwBufferSize, DSKM_CB_READ));
				while( !nResult && dwcbRead >= 0 ) {
					AVP_byte *pSign = 0;
					DWORD     dwFoundLen = 0;
					AVP_byte *pFound = (AVP_byte *)pBuffer;
					dwParam1 = (AVP_size_t)&pSign;
					DSCL_CALL_FIND_SIGN( dwFoundLen, pBuffer, dwcbRead, dwParam1/*&pSign*/ );
					if ( pSign ) {
						int nTail = 0;
Found :
						nTail = dwcbRead - (int)(pSign - (AVP_byte *)pFound);
						dwParam2 = (AVP_size_t)prcHash;
						dwParam1 = (AVP_size_t)(pSign - (AVP_byte *)pFound);
						DSCL_CALL_HASH_BUFFER( nResult, hDSKM, dwParam2/*prcHash*/, pFound, dwParam1 );
						if ( nTail < sizeof(DSKMSign) ) {
							ds_mcpy( prcSign, pSign, nTail );
							pFound = (AVP_byte *)pBuffer;
							nResult = !(dwcbRead = pCallBack(pCallBackParams, pBuffer, dwBufferSize, DSKM_CB_READ));
							if ( !nResult && dwcbRead >= 0 && (DWORD)dwcbRead >= (sizeof(DSKMSign) - nTail) ) {
								ds_mcpy( ((AVP_byte *)prcSign) + nTail, pBuffer, (sizeof(DSKMSign) - nTail) );
								dwParam2 = (AVP_size_t)prcSign;
								dwParam1 = (AVP_size_t)&pSign;
								nResult	= sizeof(DSKMSign);
								DSCL_CALL_FIND_SIGN( nResult, dwParam2/*prcSign*/, nResult/*sizeof(DSKMSign)*/, dwParam1/*&pSign*/ );
								if ( pSign ) {
									if ( IS_END_OF_SIGN(&((DSKMSign*)pSign)->end) ) {
										ds_mcpy( prcSign, &pSign, sizeof(DSKMSign) );
										dwResult = DSKM_ERR_OK;
										break;
									}
								}
								dwParam1 = (AVP_size_t)prcHash;
								DSCL_CALL_HASH_BUFFER( nResult, hDSKM, dwParam1/*prcHash*/, dwParam2/*prcSign*/, nTail );

								dwParam1 = (AVP_size_t)&pSign;
								DSCL_CALL_FIND_SIGN( dwParam2, pBuffer, dwcbRead, dwParam1/*&pSign*/ );
								if ( pSign ) 
									goto Found;

								dwParam1 = (AVP_size_t)prcHash;
								DSCL_CALL_HASH_BUFFER( nResult, hDSKM, dwParam1/*prcHash*/, pBuffer, dwcbRead );
							}
							else {
								if ( dwcbRead ) {
									DSCL_CALL_HASH_BUFFER( nResult, hDSKM, dwParam2/*prcHash*/, pBuffer, dwcbRead );
								}
								else {
									dwParam1 = (AVP_size_t)prcSign;
									DSCL_CALL_HASH_BUFFER( nResult, hDSKM, dwParam2/*prcHash*/, dwParam1/*prcSign*/, nTail );
								}
								break;
							}
						}
						else {
							ds_mcpy( prcSign, pSign, sizeof(DSKMSign) );
							if ( IS_END_OF_SIGN(&prcSign->end) ) {
								dwResult = DSKM_ERR_OK;
								break;
							}
							DSCL_CALL_HASH_BUFFER( nResult, hDSKM, dwParam2/*prcHash*/, pSign, dwFoundLen );

							dwParam1 = (AVP_size_t)&pSign;
							pFound += dwFoundLen;
							dwcbRead -= dwFoundLen;
							DSCL_CALL_FIND_SIGN( dwFoundLen, pFound, dwcbRead, dwParam1/*&pSign*/ );
							if ( pSign ) 
								goto Found;
						}
					}
					else {
						dwParam2 = (AVP_size_t)prcHash;
						DSCL_CALL_HASH_BUFFER( nResult, hDSKM, dwParam2/*prcHash*/, pBuffer, dwcbRead );
					}
					
					nResult = !(dwcbRead = pCallBack(pCallBackParams, pBuffer, dwBufferSize, DSKM_CB_READ));
				}

				pCallBack( pCallBackParams, 0, 0, DSKM_CB_CLOSE );

				if ( dwcbRead >= 0 ) {
					dwParam2 = (AVP_size_t)prcHash;
					DSCL_CALL_HASH_END( dwParam1, hDSKM, dwParam2/*prcHash*/ );
				}
				else {
					return dwResult = DSKM_ERR_CALLBACK_READ_ERROR;
				}
			}
			else {
				return dwResult = DSKM_ERR_CALLBACK_READ_ERROR;
			}
		}
		else {
			AVP_byte *pSign = 0;
			dwParam2 = (AVP_size_t)prcHash;
			DSCL_CALL_HASH_INIT( dwResult, hDSKM, dwParam2/*prcHash*/, 0 );
			
			dwParam1 = (AVP_size_t)&pSign;
			DSCL_CALL_FIND_SIGN( dwResult, pBuffer, dwBufferSize, dwParam1/*&pSign*/ );

			dwResult = DSKM_ERR_SIGN_NOT_FOUND;
			
			if ( pSign ) {
				int nTail = dwBufferSize - (int)(pSign - (AVP_byte *)pBuffer);
				if ( nTail >= sizeof(DSKMSign) && IS_END_OF_SIGN(&((DSKMSign *)pSign)->end) ) {
					nTail = (int)(pSign - (AVP_byte *)pBuffer);
					DSCL_CALL_HASH_BUFFER( dwResult, hDSKM, dwParam2/*prcHash*/, pBuffer, nTail/*(pSign - (AVP_byte *)pBuffer)*/ );
					
					ds_mcpy( prcSign, pSign, sizeof(DSKMSign) );

					dwResult = DSKM_ERR_OK;
				}
			}

			if ( DSKM_NOT_OK(dwResult) ) {
				DSCL_CALL_HASH_BUFFER( dwParam1, hDSKM, dwParam2/*prcHash*/, pBuffer, dwBufferSize );
			}

			DSCL_CALL_HASH_END( dwParam1, hDSKM, dwParam2/*prcHash*/ );
		}
	}
	return dwResult;
}





//! \fn				: DSKM_CheckObjectByBuffer
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObject
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pCallBack
//! \param          : void *pCallBackParams
//! \param          : HDATA hKeysData
AVP_dword DSKMAPI DSKM_CheckObjectByBuffer( HDSKM hDSKM, HDATA hObject, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, HDATA hKeysData ) {
	DSKMSign *prcSign = DSKMAllocator( sizeof(DSKMSign) );
	DSKMHash *prcHash = DSKMAllocator( sizeof(DSKMHash) );
	AVP_dword dwResult = DSKM_ERR_HASH_NOT_FOUND;

	if ( prcSign && prcHash ) {
		if ( hObject ) {
			HPROP hHashProp = DATA_Find_Prop( hObject, 0, DSKM_CHECKED_OBJ_HASH_ID(1) );
			if ( hHashProp ) {
				AVP_dword dwObjectHashSize = PROP_Get_Val( hHashProp, 0, 0 );
				if ( dwObjectHashSize == sizeof(prcHash->hash) ) {
					PROP_Get_Val( hHashProp, &prcHash->hash, sizeof(prcHash->hash) );
					dwResult = DSKM_ERR_OK;
				}
			}
		}

		if ( DSKM_OK(dwResult) && hObject ) {
			HPROP hSignProp = DATA_Find_Prop( hObject, 0, DSKM_CHECKED_OBJ_SIGN_ID(2) );
			dwResult = DSKM_ERR_SIGN_NOT_FOUND;
			if ( hSignProp ) {
				AVP_dword dwObjectSignSize = PROP_Get_Val( hSignProp, 0, 0 );
				if ( dwObjectSignSize == sizeof(*prcSign) ) {
					PROP_Get_Val( hSignProp, prcSign, sizeof(*prcSign) );
					dwResult = DSKM_ERR_OK;
				}
			}
		}
		
		if ( DSKM_NOT_OK(dwResult) ) {
			dwResult = DSKM_HashBufferFindSign( hDSKM, pBuffer, dwBufferSize, pCallBack, pCallBackParams, prcHash, prcSign, hKeysData );
			if ( hObject ) {
				DSKM_HashAdditionObjectParams( hDSKM, hObject, prcHash->hash, sizeof(prcHash->hash) );

				DATA_Add_Prop( hObject, 0, DSKM_CHECKED_OBJ_HASH_ID(1), (AVP_size_t)&prcHash->hash, sizeof(prcHash->hash) );
				DATA_Add_Prop( hObject, 0, DSKM_CHECKED_OBJ_SIGN_ID(2), (AVP_size_t)prcSign, sizeof(*prcSign) );
			}
		}

		if ( DSKM_OK(dwResult) ) {
			dwResult = DSKM_CheckSignByKeysTree( hDSKM, prcSign, prcHash, hKeysData );
		}

		{
			HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID );
			if ( hProp ) {
				PROP_Get_Val( hProp, &dwResult, sizeof(dwResult) );
				DATA_Remove_Prop_H( (HDATA)hDSKM, 0, hProp );
			}
		}

		DSKMLiberator( prcSign );
		DSKMLiberator( prcHash );
	}
	return dwResult;
}



//! \fn				: 	DSKM_GetBuffer_CallBack
//! \brief			:	Callback подкачки буфера объекта
//! \return			: int - количество записанных байт
//! \param          : void *pParams - параметры функции
//! \param          : void *pvBuffer	- указатель на буфер чтения
//! \param          : AVP_dword dwBufferSize - размер буфера чтения
//! \param          : AVP_dword dwCommand - код команды (DSKM_CB_*)
static int DSKMAPI DSKM_GetBuffer_CallBack( void *pParams, void *pvBuffer, AVP_dword dwBufferSize, AVP_dword dwCommand ) {
	if ( dwCommand == DSKM_CB_OPEN ) {
		DSKMSetFilePointer( ((DSKMCBP *)pParams)->m_pIo, ((DSKMCBP *)pParams)->m_hFile, 0, 0, FILE_BEGIN );
		return 0;
	}
	if ( dwCommand == DSKM_CB_READ ) {
		DWORD dwRead;
		if ( DSKMReadFile(((DSKMCBP *)pParams)->m_pIo, ((DSKMCBP *)pParams)->m_hFile, pvBuffer, dwBufferSize, &dwRead, 0) )
			return dwRead;
	}
	return -1;
}




//! \fn				: DSKMAPI DSKM_CheckHandle
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObject
//! \param          : HANDLE hFile
//! \param          : HDATA hKeysData
static AVP_dword DSKMAPI DSKM_CheckHandle( HDSKM hDSKM, HDATA hObject, HANDLE hFile, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_SIGN;
	if ( hFile != INVALID_HANDLE_VALUE ) {
		DSKM_Io *pIo = DSKM_GetIO( hDSKM );
		dwResult = DSKM_ERR_IO_NOT_INITIALIZED;
		if ( pIo ) {
			DSKMSetFilePointer( pIo, hFile, 0, 0, FILE_BEGIN );
			{
				AVP_byte *pBuffer = DSKMAllocator( BUFFER_SIZE );
				dwResult = DSKM_UNKNOWN_ERROR;
				if ( pBuffer ) {
					DSKMCBP rcCBP = {pIo, hFile};
					dwResult = DSKM_CheckObjectByBuffer( hDSKM, hObject, pBuffer, BUFFER_SIZE, DSKM_GetBuffer_CallBack, (void *)&rcCBP, hKeysData );
					DSKMLiberator( pBuffer );
				}
			}
		}
	}
	return dwResult;
}



//! \fn				: DSKM_CheckFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObject
//! \param          : const AVP_char* pszFileName
//! \param          : HDATA hKeysData
AVP_dword DSKMAPI DSKM_CheckFile( HDSKM hDSKM, HDATA hObject, const AVP_char* pszFileName, HDATA hKeysData ) {
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		HANDLE hFile = DSKMCreateFile( pIo, pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
		
		if ( hFile != INVALID_HANDLE_VALUE ) {
			AVP_dword dwResult = DSKM_CheckHandle( hDSKM, hObject, hFile, hKeysData );
			DSKMCloseHandle( pIo, hFile );
			return dwResult;
		}
		return DSKM_ERR_CANNOT_OPEN_FILE;
	}
	return DSKM_ERR_IO_NOT_INITIALIZED;
}



//! \fn				: DSKM_CheckHFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDSKMFILE hFile
//! \param          : HDATA hKeysData
AVP_dword DSKMAPI DSKM_CheckHFile( HDSKM hDSKM, HDSKMFILE hFile, HDATA hKeysData ) {
	if ( hFile ) {
		if ( hFile->m_hFile == INVALID_HANDLE_VALUE ) {
			DSKM_OpenHFileHandle( hDSKM, hFile );
		}
		if ( hFile->m_hFile != INVALID_HANDLE_VALUE ) 
			return DSKM_CheckHandle( hDSKM, 0, hFile->m_hFile, hKeysData );
	}
	return DSKM_ERR_CANNOT_OPEN_FILE;
}



//! \fn				: DSKM_CheckObject
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : HDATA hObject
//! \param          : HDATA hKeysData
AVP_dword DSKMAPI DSKM_CheckObject( HDSKM hDSKM, HDATA hObject, HDATA hKeysData ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_SIGN;
	AVP_dword dwPID = DATA_Get_Id( hObject, 0 );
	AVP_dword dwPID_0 = MAKE_AVP_PID( 0, GET_AVP_PID_APP(dwPID), GET_AVP_PID_TYPE(dwPID), GET_AVP_PID_ARR(dwPID) );
	switch ( dwPID_0 ) {
		case DSKM_OBJECT_NAME_ID(0) : 
		case DSKM_REG_NAME_ID(0) : {
			AVP_char *pRegFileName = DSKM_GetPropValueAsString( DATA_Find_Prop(hObject, 0, 0), 0 );
			if ( pRegFileName ) {
				dwResult = DSKM_CheckFile( hDSKM, hObject, pRegFileName, hKeysData );
				DSKMLiberator( pRegFileName );
			}
			break;
		}
		case DSKM_OBJECT_BUFF_PTR_ID(0) : 
		case DSKM_REG_BUFF_PTR_ID(0) : {
			void *pBuffer;
			AVP_dword dwBufferSize;
			pfnDSKM_GetBuffer_CallBack pCallBack;
			void *pCallBackParams;
			
			DATA_Get_Val( hObject, 0, 0, &pBuffer, sizeof(pBuffer) );
			DATA_Get_Val( hObject, 0, DSKM_OBJECT_BUFF_SIZE_ID, &dwBufferSize, sizeof(dwBufferSize) );
			DATA_Get_Val( hObject, 0, DSKM_OBJECT_BUFF_PFN_ID, &pCallBack, sizeof(pCallBack) );
			DATA_Get_Val( hObject, 0, DSKM_OBJECT_BUFF_PFN_PARS_ID, &pCallBackParams, sizeof(pCallBackParams) );
			
			dwResult = DSKM_CheckObjectByBuffer( hDSKM, hObject, pBuffer, dwBufferSize, pCallBack, pCallBackParams, hKeysData );
			break;
		}
	}

	return dwResult;
}


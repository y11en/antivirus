/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_sign.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/8/2005 3:42:39 PM
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


#define BUFFER_SIZE (0xffff / 4)



//! \fn				: DSKMAPI DSKMCreateSign
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  HDSKM hDSKM
//! \param          : DSKMSign *pSign
//! \param          : DSKMHash *pHash
//! \param          : void *pKey
//! \param          : unsigned long dwKeySize
static AVP_dword DSKMAPI DSKMCreateSign( HDSKM hDSKM, DSKMSign *pSign, DSKMHash *pHash, void *pKey, unsigned long dwKeySize ) {
	AVP_dword dwResult = DSKM_ERR_CANNOT_CREATE_SIGN;
#ifdef DSKM_EMUL
	int i;
	AVP_byte sign[SIGN_BIN_LEN];
	for ( i=0; i<SIGN_BIN_LEN; ) {
		unsigned long j;
		for ( j=0; j<dwKeySize && i<SIGN_BIN_LEN; i++,j++ ) {
			sign[i] = pHash->hash[i] ^ ((unsigned char *)pKey)[j];
		}
	}
	i = BinToText( sign, SIGN_BIN_LEN, pSign->sign, SIGN_TXT_LEN );
	if ( i < SIGN_BIN_LEN )
		dwResult = DSKM_ERR_CANNOT_CREATE_SIGN;
	if ( DSKM_OK(dwResult) ) {
		SET_BEG_OF_SIGN(&pSign->begin);
		SET_END_OF_SIGN(&pSign->end);
	}
#else
	HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_CLCONT_C_ID );
	if ( hProp ) {
		struct  LR34_10_2001 *pCContext = 0;
		PROP_Get_Val( hProp, &pCContext, sizeof(pCContext) );
		if ( pCContext ) {
			if ( CrypC_LR34_10_2001_set_secret_key(pCContext, pKey) == RET_OK ) {
				AVP_byte sign[SIGN_FULL_LEN];
				sign[0] = SIGN_VERSION;
				sign[1] = SIGN_BIN_LEN;
				dwResult = (CrypC_LR34_10_2001_sign( pCContext, pHash->hash, sign + SIGN_AUX_SIZE) == RET_OK) ? DSKM_ERR_OK : DSKM_ERR_CANNOT_CREATE_SIGN;
				if ( DSKM_OK(dwResult) ) {
					int iConverted = BinToText( sign, sizeof(sign), pSign->sign, sizeof(pSign->sign) );
					if ( iConverted < SIGN_BIN_LEN )
						dwResult = DSKM_ERR_CANNOT_CREATE_SIGN;
					if ( DSKM_OK(dwResult) ) {
						SET_BEG_OF_SIGN(&pSign->begin);
						SET_END_OF_SIGN(&pSign->end);
					}
				}
				CrypC_LR34_10_2001_clear_secret_key( pCContext );
			}
		}
	}
#endif
	return dwResult;
}



//! \fn				: DSKM_SignFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pszFileName
//! \param          : void *pPrivateKey
//! \param          : AVP_dword dwKeySize
AVP_dword DSKMAPI DSKM_SignFile( HDSKM hDSKM, const AVP_char* pszFileName, void *pPrivateKey, AVP_dword dwKeySize ) {
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		HANDLE hFile = DSKMCreateFile( pIo, pszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
		
		if ( hFile != INVALID_HANDLE_VALUE ) {
			DSKMHash *prcHash = DSKMAllocator( sizeof(DSKMHash) );
			AVP_dword dwResult = DSKM_UNKNOWN_ERROR;
			if ( prcHash ) {
				DWORD dwFileSize;
				DWORD dwcbRead;
				int nResult = 0;
				
				AVP_byte *pBuffer = DSKMAllocator( BUFFER_SIZE );
				
				if ( pBuffer ) {
					dwResult = DSKM_ERR_OK;
					dwFileSize = DSKMGetFileSize( pIo, hFile, NULL );
					
					//DSKMHashInit( hDSKM, &rcHash );
					DSCL_SIGN_CALL_HASH_INIT( ((AVP_size_t)hDSKM), ((AVP_size_t)prcHash), 0 );
					
					while( !nResult && dwFileSize ) {
						AVP_dword dwNow = min( dwFileSize, BUFFER_SIZE );
						
						nResult = !(DSKMReadFile(pIo, hFile, pBuffer, dwNow, &dwcbRead, NULL) && dwcbRead == dwNow);
						//DSKMHashBuffer( hDSKM, &rcHash, pBuffer, dwNow );
						DSCL_SIGN_CALL_HASH_BUFFER( ((AVP_size_t)hDSKM), ((AVP_size_t)prcHash), ((AVP_size_t)pBuffer), ((AVP_size_t)dwNow) );
						
						dwFileSize -= dwNow;
					}
					
					DSKMLiberator( pBuffer );
				}
				
				//DSKMHashEnd( hDSKM, &rcHash );
				DSCL_SIGN_CALL_HASH_END( ((AVP_size_t)hDSKM), ((AVP_size_t)prcHash) );
				
				{
					DSKMSign *prcSign = DSKMAllocator( sizeof(DSKMSign) );
					dwResult = DSKM_UNKNOWN_ERROR;
					if ( prcSign ) {
						if ( DSKM_OK(dwResult=DSKMCreateSign(hDSKM, prcSign, prcHash, pPrivateKey, dwKeySize)) ) {
							DWORD dwWritten;
							DSKMWriteFile( pIo, hFile, prcSign, sizeof(*prcSign), &dwWritten, 0 );
							if ( dwWritten != sizeof(*prcSign) )
								dwResult = DSKM_ERR_CANNOT_WRITE_REG_FILE;
						}
						DSKMLiberator( prcSign );
					}
				}
				
				DSKMCloseHandle( pIo, hFile );
				
				DSKMLiberator( prcHash );
			}
			return dwResult;
		}
		return DSKM_ERR_CANNOT_OPEN_REG_FILE;
	}
	return 	DSKM_ERR_IO_NOT_INITIALIZED;
}




//! \fn				: DSKM_SignFileBySign
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pszFileName
//! \param          : void *pSign
//! \param          : AVP_dword dwSignSize
AVP_dword DSKMAPI DSKM_SignFileBySign( HDSKM hDSKM, const AVP_char* pszFileName, void *pSign, AVP_dword dwSignSize ) {
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		HANDLE hFile = DSKMCreateFile( pIo, pszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
		
		if ( hFile != INVALID_HANDLE_VALUE ) {
			AVP_dword dwResult = DSKM_ERR_CANNOT_WRITE_REG_FILE;
			DWORD dwFileSize = DSKMGetFileSize( pIo, hFile, NULL );
			
			if ( dwFileSize == DSKMSetFilePointer(pIo, hFile, 0, 0, FILE_END) )  {
				DWORD dwWritten;
				dwResult = DSKM_ERR_OK;
				DSKMWriteFile( pIo, hFile, pSign, dwSignSize, &dwWritten, 0 );
				if ( dwWritten != dwSignSize )
					dwResult = DSKM_ERR_CANNOT_WRITE_REG_FILE;
			}
				
			DSKMCloseHandle( pIo, hFile );
			
			return dwResult;
		}
		return DSKM_ERR_CANNOT_OPEN_REG_FILE;
	}
	return 	DSKM_ERR_IO_NOT_INITIALIZED;
}



//! \fn				: DSKM_SignBufferBySign
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : pfnDSKM_GetBuffer_CallBack pCallBack
//! \param          : void *pCallBackParams
//! \param          : void *pSign
//! \param          : AVP_dword dwSignSize
//! \param          : void **ppvSignedBuffer
//! \param          : AVP_dword *pdwSignedBufferSize
AVP_dword	DSKMAPI DSKM_SignBufferBySign( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, void *pSign, AVP_dword dwSignSize, void **ppvSignedBuffer, AVP_dword *pdwSignedBufferSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	
	if ( pBuffer && dwBufferSize ) {
		*pdwSignedBufferSize = 0;
		*ppvSignedBuffer = 0;
		if ( pCallBack ) {
			AVP_byte *pTemp;
			if ( pCallBack(pCallBackParams, 0, 0, DSKM_CB_OPEN) >= 0 ) {
				int nResult = 0;
				int dwcbRead = 0;
				dwResult = DSKM_ERR_OK;
				nResult = !(dwcbRead = pCallBack(pCallBackParams, pBuffer, dwBufferSize, DSKM_CB_READ));
				while( !nResult && dwcbRead >= 0 ) {
					if ( dwcbRead ) {
						pTemp = DSKMAllocator( dwcbRead + *pdwSignedBufferSize );
						if ( pTemp ) {
							if ( *ppvSignedBuffer ) {
								ds_mcpy( pTemp, *ppvSignedBuffer, *pdwSignedBufferSize );
								DSKMLiberator( *ppvSignedBuffer );
							}
							ds_mcpy( pTemp + *pdwSignedBufferSize, pBuffer, dwcbRead );
							*ppvSignedBuffer = pTemp;
							*pdwSignedBufferSize += dwcbRead;
						}
						else {
							dwResult = DSKM_UNKNOWN_ERROR;
							nResult = 1;
						}
					}
					nResult = !(dwcbRead = pCallBack(pCallBackParams, pBuffer, dwBufferSize, DSKM_CB_READ));
				}
				if ( DSKM_OK(dwResult) && dwcbRead < 0 ) 
					dwResult = DSKM_ERR_CALLBACK_READ_ERROR;

				pCallBack( pCallBackParams, 0, 0, DSKM_CB_CLOSE );
			}
			else
				dwResult = DSKM_ERR_CALLBACK_READ_ERROR;
			
			if ( DSKM_OK(dwResult) ) {
				dwResult = DSKM_UNKNOWN_ERROR;
				pTemp = DSKMAllocator( dwSignSize + *pdwSignedBufferSize );
				if ( pTemp ) {
					if ( *ppvSignedBuffer ) {
						ds_mcpy( pTemp, *ppvSignedBuffer, *pdwSignedBufferSize );
						DSKMLiberator( *ppvSignedBuffer );
					}
					ds_mcpy( pTemp + *pdwSignedBufferSize, pSign, dwSignSize );
					*ppvSignedBuffer = pTemp;
					*pdwSignedBufferSize += dwSignSize;
					dwResult = DSKM_ERR_OK;
				}
			}
		}
		else {
			dwResult = DSKM_UNKNOWN_ERROR;
			*ppvSignedBuffer = DSKMAllocator( dwSignSize + dwBufferSize );
			if ( *ppvSignedBuffer ) {
				ds_mcpy( *ppvSignedBuffer, pBuffer, dwBufferSize );
				ds_mcpy( (AVP_byte *)*ppvSignedBuffer + dwBufferSize, pSign, dwSignSize );
				*pdwSignedBufferSize = dwSignSize + dwBufferSize;
				dwResult = DSKM_ERR_OK;
			}
		}
	}
	return dwResult;
}


//! \fn				: DSKM_SignBuffer
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void **ppvRegBuffer
//! \param          : AVP_dword *pdwRegBufferSize
//! \param          : void *pPrivateKey
//! \param          : AVP_dword dwKeySize
AVP_dword DSKMAPI DSKM_SignBuffer( HDSKM hDSKM, void **ppvRegBuffer, AVP_dword *pdwRegBufferSize, void *pPrivateKey, AVP_dword dwKeySize ) {
	DSKMSign *prcSign = DSKMAllocator( sizeof(DSKMSign) );
	DSKMHash *prcHash = DSKMAllocator( sizeof(DSKMHash) );
	AVP_dword dwResult = DSKM_UNKNOWN_ERROR;
	if ( prcSign && prcHash ) {
		dwResult = DSKM_ERR_OK;

		//DSKMHashInit( hDSKM, &rcHash );
		DSCL_SIGN_CALL_HASH_INIT( ((AVP_size_t)hDSKM), ((AVP_size_t)prcHash), 0 );
		//DSKMHashBuffer( hDSKM, &rcHash, *ppvRegBuffer, *pdwRegBufferSize );
		DSCL_SIGN_CALL_HASH_BUFFER( ((AVP_size_t)hDSKM), ((AVP_size_t)prcHash), ((AVP_size_t)*ppvRegBuffer), ((AVP_size_t)*pdwRegBufferSize) );
		//DSKMHashEnd( hDSKM, &rcHash );
		DSCL_SIGN_CALL_HASH_END( ((AVP_size_t)hDSKM), ((AVP_size_t)prcHash) );

		if ( DSKM_OK(dwResult = DSKMCreateSign(hDSKM, prcSign, prcHash, pPrivateKey, dwKeySize)) ) {
			void *pSignBuffer = DSKMAllocator( *pdwRegBufferSize + sizeof(*prcSign) ); 
			dwResult = DSKM_UNKNOWN_ERROR;
			if ( pSignBuffer ) {
				ds_mcpy( pSignBuffer, *ppvRegBuffer, *pdwRegBufferSize );
				ds_mcpy( (unsigned char *)pSignBuffer + *pdwRegBufferSize, prcSign, sizeof(*prcSign) );
				*pdwRegBufferSize += sizeof(*prcSign);
				DSKMLiberator( *ppvRegBuffer );
				*ppvRegBuffer = pSignBuffer;
				dwResult = DSKM_ERR_OK;
			}
		}

		DSKMLiberator( prcSign );
		DSKMLiberator( prcHash );
	}
	return dwResult;
}



//! \fn				: DSKM_SignHash
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void *pObjectHash
//! \param          : AVP_dword dwObjectHashSize
//! \param          : void *pPrivateKey
//! \param          : AVP_dword dwKeySize
//! \param          : void **ppSignBuffer
//! \param          : AVP_dword *pdwSignBufferSize
AVP_dword DSKMAPI DSKM_SignHash( HDSKM hDSKM, void *pObjectHash, AVP_dword dwObjectHashSize, void *pPrivateKey, AVP_dword dwKeySize, void **ppSignBuffer, AVP_dword *pdwSignBufferSize ) {
	DSKMSign *prcSign = DSKMAllocator( sizeof(DSKMSign) );
	DSKMHash *prcHash = DSKMAllocator( sizeof(DSKMHash) );
	AVP_dword dwResult = DSKM_UNKNOWN_ERROR;
	if ( prcSign && prcHash ) {
		dwResult = DSKM_ERR_OK;
		ds_mcpy( prcHash->hash, pObjectHash, min(sizeof(prcHash->hash),dwObjectHashSize) );
		if ( DSKM_OK(dwResult = DSKMCreateSign( hDSKM, prcSign, prcHash, pPrivateKey, dwKeySize )) ) {
			dwResult = DSKM_UNKNOWN_ERROR;
			if ( ( *ppSignBuffer = DSKMAllocator( sizeof(*prcSign) ) ) != 0 ) {
				*pdwSignBufferSize = sizeof(*prcSign);
				ds_mcpy( *ppSignBuffer, prcSign, sizeof(*prcSign) );
				dwResult = DSKM_ERR_OK;
			}
		}

		DSKMLiberator( prcSign );
		DSKMLiberator( prcHash );
	}
	return dwResult;
}


//! \fn				: DSKM_RandomFillBuffer
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : void *pBuffer
//! \param          : AVP_dword dwBufferSize
AVP_dword	DSKMAPI DSKM_RandomFillBuffer( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize ) {
	AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
	
	if ( pBuffer && dwBufferSize ) {
		HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_CLCONT_R_ID );
		dwResult = DSKM_UNKNOWN_ERROR;
		if ( hProp ) {
			struct  RND_State    *pRContext = 0;
			PROP_Get_Val( hProp, &pRContext, sizeof(pRContext) );
			CrypC_Random_Fill( pRContext, pBuffer, dwBufferSize );
			dwResult = DSKM_ERR_OK;
		}
	}
	return dwResult;
}

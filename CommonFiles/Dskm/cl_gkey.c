/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	cl_gkey.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/15/2005 1:28:13 PM
*		
*		Example:	
*		
*		
*		
*/		



#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"




//! \fn				:  DSKMAPI DSKM_GenerateKeyPair
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : void **ppPrivateKey
//! \param          : AVP_dword *pdwPrivateKeySize
//! \param          : void **ppvPublicKey
//! \param          : AVP_dword *pdwPublicKeySize
AVP_dword   DSKMAPI DSKM_GenerateKeyPair( HDSKM hDSKM, void **ppPrivateKey, AVP_dword *pdwPrivateKeySize, void **ppvPublicKey, AVP_dword *pdwPublicKeySize ) {
	if ( ppPrivateKey && pdwPrivateKeySize && ppvPublicKey && pdwPublicKeySize ) {
#ifdef DSKM_EMUL
		AVP_dword i;

		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		*pdwPrivateKeySize = 10;
		*ppPrivateKey = DSKMAllocator( *pdwPrivateKeySize );
		
		*pdwPublicKeySize = 10;
		*ppvPublicKey = DSKMAllocator( *pdwPublicKeySize );

		for ( i=0; i<*pdwPublicKeySize; i++ ) {
			(*(AVP_byte **)ppPrivateKey)[i] = (AVP_byte)i;
			(*(AVP_byte **)ppvPublicKey)[i] = (AVP_byte)i;
		}
		
		return DSKM_ERR_OK;
#else
		*ppPrivateKey = 0;
		*pdwPrivateKeySize = 0;
		*ppvPublicKey = 0;
		*pdwPublicKeySize = 0;
		{
			AVP_dword dwResult = DSKM_ERR_CRIPTO_LIB;
			HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_CLCONT_C_ID );
			if ( hProp ) {
				struct  LR34_10_2001 *pCContext = 0;
				PROP_Get_Val( hProp, &pCContext, sizeof(pCContext) );
				if ( pCContext ) {
					if ( CrypC_LR34_10_2001_gen_secret_key(pCContext) == RET_OK && 
							 CrypC_LR34_10_2001_gen_public_key(pCContext)	== RET_OK ) {
						
						int iKeySize = 0;
						if ( CrypC_LR34_10_2001_get_secret_key(pCContext, 0, &iKeySize) == RET_OK ) {
							*ppPrivateKey = DSKMAllocator( iKeySize );
							if ( *ppPrivateKey ) {
								*pdwPrivateKeySize = iKeySize;
								if ( CrypC_LR34_10_2001_get_secret_key(pCContext, *ppPrivateKey, &iKeySize) != RET_OK ) {
									DSKMLiberator( *ppPrivateKey );
									*ppPrivateKey = 0;
									*pdwPrivateKeySize = 0;
								}
							}
						}
						if ( *ppPrivateKey ) {
							struct LPoint rcPub;
							if ( CrypC_LR34_10_2001_get_public_key(pCContext, &rcPub, &iKeySize) == RET_OK ) {
								*ppvPublicKey = DSKMAllocator( sizeof(rcPub.X) + sizeof(rcPub.Y) );
								if ( *ppvPublicKey ) {
									*pdwPublicKeySize = sizeof(rcPub.X) + sizeof(rcPub.Y);
									ds_mcpy( *ppvPublicKey, &rcPub, *pdwPublicKeySize );
									dwResult = DSKM_ERR_OK;
								}
							}
							if ( DSKM_NOT_OK(dwResult) ) {
								DSKMLiberator( *ppPrivateKey );
								*ppPrivateKey = 0;
								*pdwPrivateKeySize = 0;
							}
						}
					}
					CrypC_LR34_10_2001_clear_secret_key( pCContext );
					CrypC_LR34_10_2001_clear_public_key( pCContext );

					return dwResult;
				}
			}
		}
		return DSKM_ERR_NOT_INITIALIZED;
#endif
	}
	return DSKM_ERR_INVALID_PARAMETER;
}



/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	cl_init.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	7/5/2005 11:38:24 AM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"



//! \fn				: DSKM_InitCriptoLibrary
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          : HDSKM hDSKM
AVP_dword	DSKMAPI DSKM_InitCriptoLibrary( HDSKM hDSKM ) {
#ifdef DSKM_EMUL
	return DSKM_ERR_OK;
#else
	if ( hDSKM ) {
		struct  RND_State    *pRContext = DSKMAllocator( sizeof(struct RND_State) );
		struct  LR34_10_2001 *pCContext = DSKMAllocator( sizeof(struct LR34_10_2001) );
		if ( pRContext && pCContext ) {
			ds_mset( pRContext, 0, sizeof(struct RND_State) );
			ds_mset( pCContext, 0, sizeof(struct LR34_10_2001) );
			if ( DATA_Add_Prop((HDATA)hDSKM, 0, DSKM_CLCONT_R_ID, (AVP_size_t)&pRContext, sizeof(pRContext)) && 
				   DATA_Add_Prop((HDATA)hDSKM, 0, DSKM_CLCONT_C_ID, (AVP_size_t)&pCContext, sizeof(pCContext)) ) {
				if ( CrypC_LR34_10_2001_set_parms(pCContext, pRContext, 0, 32, 0, 32, 0, 0, 0) ) {
					DSKMLiberator( pRContext );
					DSKMLiberator( pCContext );
					return DSKM_ERR_NOT_INITIALIZED;
				}
				return DSKM_ERR_OK;
			}
		}
		return DSKM_ERR_NOT_INITIALIZED;
	}
	return DSKM_ERR_INVALID_PARAMETER;
#endif
}



//! \fn				: DSKM_DeInitCriptoLibrary
//! \brief			:	
//! \return			: DSKMAPI 
//! \param          : HDSKM hDSKM
AVP_dword	DSKMAPI DSKM_DeInitCriptoLibrary( HDSKM hDSKM ) {
#ifdef DSKM_EMUL
	return DSKM_ERR_OK;
#else
	if ( hDSKM ) {
		HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_CLCONT_C_ID );
		if ( hProp ) {
			struct  LR34_10_2001 *pCContext = 0;
			PROP_Get_Val( hProp, &pCContext, sizeof(pCContext) );
			if ( pCContext ) {
				CrypC_LR34_10_2001_clear( pCContext );
				DSKMLiberator( pCContext );
			}
		}
		hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_CLCONT_R_ID );
		if ( hProp ) {
			struct  RND_State    *pRContext = 0;
			PROP_Get_Val( hProp, &pRContext, sizeof(pRContext) );
			if ( pRContext )
				DSKMLiberator( pRContext );
		}
		return DSKM_ERR_OK;
	}
	return DSKM_ERR_INVALID_PARAMETER;
#endif
}

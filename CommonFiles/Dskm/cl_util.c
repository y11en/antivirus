/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	cl_util.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	7/4/2005 6:46:50 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"

#include "cl_util.h"

//#define SECURE_CALL
static void DSCLAPI DSKMSafeZoneBegin() {
}


#ifndef DSKM_EMUL
struct  COM_State PContext =
{
 /* Массив из 8 подстановок множества {0,...,15} криптосхемы ГОСТ 28147-89     */
	{{0X1,0XF,0XD,0X0,0X5,0X7,0XA,0X4,0X9,0X2,0X3,0XE,0X6,0XB,0X8,0XC},
	 {0XD,0XB,0X4,0X1,0X3,0XF,0X5,0X9,0X0,0XA,0XE,0X7,0X6,0X8,0X2,0XC},
	 {0X4,0XB,0XA,0X0,0X7,0X2,0X1,0XD,0X3,0X6,0X8,0X5,0X9,0XC,0XF,0XE},
	 {0X6,0XC,0X7,0X1,0X5,0XF,0XD,0X8,0X4,0XA,0X9,0XE,0X0,0X3,0XB,0X2},
	 {0X7,0XD,0XA,0X1,0X0,0X8,0X9,0XF,0XE,0X4,0X6,0XC,0XB,0X2,0X5,0X3},
	 {0X5,0X8,0X1,0XD,0XA,0X3,0X4,0X2,0XE,0XF,0XC,0X7,0X6,0X0,0X9,0XB},
	 {0XE,0XB,0X4,0XC,0X6,0XD,0XF,0XA,0X2,0X3,0X8,0X1,0X0,0X7,0X5,0X9},
	 {0X4,0XA,0X9,0X2,0XD,0X8,0X0,0XE,0X6,0XB,0X1,0XC,0X7,0XF,0X5,0X3}},
	/* Массив из 4 (перемноженных) подстановок множества {0,...,255} со сдвигами  */
	/* образов на позицию байта прообраза внутри двойного слова.                  */
	{0,0}
};
#endif


//! \fn				: DSKMHashInit
//! \brief			:	
//! \return			: AVP_dword DSCLAPI 
//! \param          :  HDSKM hDSKM
//! \param          : DSKMHash *pHash
#if defined( SECURE_CALL )
static 
#endif
AVP_dword DSCLAPI DSKMHashInit( HDSKM hDSKM, DSKMHash *pHash, void *pvInitialHash ) {
#ifdef DSKM_EMUL
	ds_mset( pHash, 0, sizeof(DSKMHash) );
#else
	ds_mset( pHash, 0, sizeof(DSKMHash) );
	ds_mcpy( &pHash->PContext, &PContext, sizeof(struct COM_State) );
	{
		HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_CLCONT_R_ID );
		if ( hProp ) {
			struct  RND_State    *pRContext = 0;
			PROP_Get_Val( hProp, &pRContext, sizeof(pRContext) );
			if ( pRContext ) {
				CrypC_R34_11_1994_init  ( &pHash->HContext, &pHash->PContext, pRContext, pvInitialHash );
			}
		}
	}
#endif
	return 0;
}


//! \fn				: DSKMHashBuffer
//! \brief			:	
//! \return			: AVP_dword DSCLAPI 
//! \param          :  HDSKM hDSKM
//! \param          : DSKMHash *pHash
//! \param          : void *pBuffer
//! \param          : AVP_dword dwNow
#if defined( SECURE_CALL )
static 
#endif
AVP_dword DSCLAPI DSKMHashBuffer( HDSKM hDSKM, DSKMHash *pHash, void *pBuffer, AVP_dword dwNow ) {
#ifdef DSKM_EMUL
	unsigned long i;
	for ( i=0; i<dwNow; i++ ) {
		pHash->hash[i%sizeof(pHash->hash)] += ((AVP_byte *)pBuffer)[i];
	}
#else
	CrypC_R34_11_1994_update( &pHash->HContext, pBuffer, dwNow );
#endif
	return 0;
}


//! \fn				: DSKMHashEnd
//! \brief			:	
//! \return			: AVP_dword DSCLAPI 
//! \param          :  HDSKM hDSKM
//! \param          : DSKMHash *pHash
#if defined( SECURE_CALL )
static 
#endif
AVP_dword DSCLAPI DSKMHashEnd( HDSKM hDSKM, DSKMHash *pHash ) {
#ifndef DSKM_EMUL
  CrypC_R34_11_1994_final ( &pHash->HContext, pHash->hash );
  CrypC_R34_11_1994_clear ( &pHash->HContext );
#endif
	return 0;
}


#if defined(DSKM_SECURE_CHECK_TAG)

#if !defined (MACHINE_IS_BIG_ENDIAN)

#define CMP(ptr,o) ((*(((unsigned char*)ptr)+o) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>(o * 8))) == DSKM_SIGN_CHAR_##o##_CR)

#else /*MACHINE_IS_BIG_ENDIAN*/

#define CMP(ptr,o) ((*(((unsigned char*)ptr)+o) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>(24 - (o * 8)))) == DSKM_SIGN_CHAR_##o##_CR)

#endif /*MACHINE_IS_BIG_ENDIAN*/

#else	/*DSKM_SECURE_CHECK_TAG*/

#define CMP(ptr,o) (*(((unsigned char*)ptr)+o) == DSKM_SIGN_CHAR_##o)

#endif /*DSKM_SECURE_CHECK_TAG*/


//! \fn				: DSCLAPI DSKMFindSign
//! \brief			:	
//! \return			: void * 
//! \param          :  void *pBuffer
//! \param          : AVP_dword dwSize
#if defined( SECURE_CALL )
static 
#endif
AVP_dword DSCLAPI DSKMFindSign( void *pBuffer, AVP_dword dwSize, void **ppvSign ) {
  AVP_byte * pBuf = (AVP_byte *)pBuffer;
  *ppvSign = 0;
  for( ; dwSize>=SIGN_TXT_BEG; dwSize--,pBuf++ ) {
    if ( IS_BEG_OF_SIGN(pBuf) ) {
      return (*ppvSign = pBuf),SIGN_TXT_BEG;
		}
	}
  if ( dwSize >= (SIGN_TXT_BEG - 1) ) {
    if ( CMP(pBuf,0) && CMP(pBuf,1) && CMP(pBuf,2) )
      return (*ppvSign = pBuf),(SIGN_TXT_BEG - 1);
    dwSize--;
    pBuf++;
	}
  if ( dwSize >= (SIGN_TXT_BEG - 2) ) {
    if ( CMP(pBuf,0) && CMP(pBuf,1) )
      return (*ppvSign = pBuf),(SIGN_TXT_BEG - 2);
    dwSize--;
    pBuf++;
	}
  if ( dwSize >= (SIGN_TXT_BEG - 3) ) {
    if ( CMP(pBuf,0) )
      return (*ppvSign = pBuf),(SIGN_TXT_BEG - 3);
	}
  return 0;
}



//! \fn				: DSCLAPI DSKMCheckSign
//! \brief			:	
//! \return			: unsigned int 
//! \param          :  HDSKM hDSKM
//! \param          : DSKMSign *pSign
//! \param          : DSKMHash *pHash
//! \param          : void *pKey
//! \param          : AVP_dword dwKeySize
#if defined( SECURE_CALL )
static 
#endif
AVP_dword DSCLAPI DSKMCheckSign( HDSKM hDSKM, DSKMSign *pSign, DSKMHash *pHash, void *pKey, AVP_dword dwKeySize ) {
	AVP_byte sign[SIGN_FULL_LEN + 2];
	TextToBin( pSign->sign, sizeof(pSign->sign)/*SIGN_TXT_LEN*/, sign, SIGN_FULL_LEN + 2 );
#ifdef DSKM_EMUL
	{
		int i;
		HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID );
		if ( !hProp ) {
			DATA_Add_Prop( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID, DSKM_ERR_INVALID_SIGN, 0 );
		}
		for ( i=0; i<SIGN_BIN_LEN; ) {
			unsigned long j;
			for ( j=0; j<dwKeySize && i<SIGN_BIN_LEN; i++,j++ ) {
				sign[i] ^= ((unsigned char *)pKey)[j];
			}
		}
		AVP_dword dwResult = !ds_mcmp(pHash->hash, pSign->sign, sizeof(pSign->sign)) == RET_OK ? DSKM_ERR_OK : DSKM_ERR_INVALID_SIGN;
		DATA_Set_Val( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID, dwResult, 0 );
	}
#else
	{
		//DWORD dwErrors[2] = {	DSKM_ERR_OK, DSKM_ERR_INVALID_SIGN };
		struct LPoint rcPub;
		HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID );
		ds_mset( &rcPub, 0, sizeof(rcPub) );
		//ds_mcpy( &rcPub.Z, dwErrors, sizeof(dwErrors) );
		if ( !hProp ) {
			DATA_Add_Prop( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID, rcPub.Type = DSKM_ERR_INVALID_SIGN, 0 );
		}
		hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_CLCONT_C_ID );
		if ( hProp && sign[0] == SIGN_VERSION && sign[1] == SIGN_BIN_LEN ) {
			struct  LR34_10_2001 *pCContext = 0;
			PROP_Get_Val( hProp, &pCContext, sizeof(pCContext) );
			if ( pCContext ) {
				ds_mcpy( &rcPub, pKey, sizeof(rcPub.X) + sizeof(rcPub.Y) );
				rcPub.Type = Affi;
				if ( CrypC_LR34_10_2001_set_public_key(pCContext, &rcPub) == RET_OK ) {
					AVP_dword dwPid = DSKM_OBJECT_ERROR_IDA(CrypC_LR34_10_2001_verify(pCContext, pHash->hash, sign + SIGN_AUX_SIZE));
					MAKE_ADDR3(eAddr, DSKM_CLFUNC_TABLE_ID, DSKM_OBJECT_ERROR_ID, dwPid );
					DATA_Get_Val( (HDATA)hDSKM, eAddr, 0, &rcPub.Type, sizeof(rcPub.Type) );
					//rcPub.Type = rcPub.Z[!!CrypC_LR34_10_2001_verify(pCContext, pHash->hash, sign + SIGN_AUX_SIZE)];
					CrypC_LR34_10_2001_clear_public_key( pCContext );
				}
			}
		}
		DATA_Set_Val( (HDATA)hDSKM, 0, DSKM_OBJECT_ERROR_ID, rcPub.Type, 0 );
		ds_mset(&rcPub, 0, sizeof(rcPub));
		ds_mset(pKey, 0, dwKeySize);
	}
#endif
	return 0;
}

static void DSCLAPI DSKMSafeZoneEnd() {
}


#if defined( _WIN32 )
#pragma warning( push )
#pragma warning( disable : 4113 )
#pragma warning( disable : 4047 )
#pragma warning( disable : 4028 )
#endif

void *g_dwFTable[] = {
	ds_mcpy					,
	ds_mset					,
	DSKMHashInit		,	// 2
	DSKMHashBuffer	,	// 3
	ds_slen					,
	DSKMHashEnd			,	// 5
	DSKMFindSign		,	// 6
	DSKMCheckSign		,	// 7
	ds_mcmp					,
};

#if defined( _WIN32 )
#pragma warning( pop ) 
#endif

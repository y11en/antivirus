#ifndef __KLKEYREC_H__
#define __KLKEYREC_H__

////////////////////////////////////////////////////////////////////
//
//  KLKey.h
//  Definitons the entry point for the Key Recover LIB
//  AVP key recovering stuff
//  Win32 CPP w\ MFC
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <Property/Property.h>

#if !defined (__unix__)
	#define  KLKR_PARAM 
#else
	#include <windows.h>
	#define  KLKR_PARAM  __cdecl
#endif

#define KLKR_PROC

// Init library
// Must be called from context each thread uses it
// pfMemAlloc - pointer to memory-alloc function
// pfMemFree  - pointer to memory-free function
KLKR_PROC AVP_bool  KLKR_PARAM  KLKR_Init_Library( void* (*pfMemAlloc)(AVP_uint), void (*pfMemFree)(void*) );

// Shut down library
// Must be called from context each thread uses it
KLKR_PROC void      KLKR_PARAM  KLKR_Shutdown_Library();


KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_FindFirstKeyFile( const TCHAR *pFolderPath );
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_FindNextKeyFile();

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_SetKeyFileName( const TCHAR *pFullKeyFileName );

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyFullFileName( TCHAR *pKeyFileName );
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyFileName( TCHAR *pKeyFileName );
KLKR_PROC HDATA   	KLKR_PARAM  KLKR_GetKeyHandle();
KLKR_PROC HDATA   	KLKR_PARAM  KLKR_SetKeyHandle( HDATA hKeyData );

KLKR_PROC AVP_dword KLKR_PARAM  KLKR_GetKeyFileSizeI();

KLKR_PROC AVP_dword KLKR_PARAM  KLKR_GetKeyVersionI();
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyVersionS( TCHAR *pVersion );
KLKR_PROC AVP_int   KLKR_PARAM  KLKR_GetKeyVersionSL( TCHAR *pVersion, AVP_int iSize );

KLKR_PROC AVP_dword KLKR_PARAM  KLKR_GetKeyLifeSpanI();
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyLifeSpanS( TCHAR *pLifeSpan );
KLKR_PROC AVP_int   KLKR_PARAM  KLKR_GetKeyLifeSpanSL( TCHAR *pLifeSpan, AVP_int iSize );

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyExpDateS( TCHAR *pExpDate, AVP_bool *bExpired );
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyExpDateExS( TCHAR *pExpDate, AVP_bool *bExpired );
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyExpDateExExS( TCHAR *pExpDate, AVP_bool *bExpired, AVP_bool bRegister );
KLKR_PROC AVP_int 	KLKR_PARAM  KLKR_GetKeyExpDateExExSL( TCHAR *pExpDate, AVP_int iSize, AVP_bool *bExpired, AVP_bool bRegister );
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyExpDateExExI( AVP_date *pKeyDate, AVP_bool *bExpired, AVP_bool bRegister );
	
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyIsTrialI();
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyIsOEMI();
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyIsUpgradeI();

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyHasSupportI();

KLKR_PROC AVP_dword KLKR_PARAM  KLKR_GetKeyLicenceCountI();
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyLicenceCountS( TCHAR *pLicCount );
KLKR_PROC AVP_int 	KLKR_PARAM  KLKR_GetKeyLicenceCountSL( TCHAR *pLicCount, AVP_int iSize );

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyLicenceInfoS( TCHAR*pLicenceInfo );
KLKR_PROC AVP_int   KLKR_PARAM  KLKR_GetKeyLicenceInfoSL( TCHAR*pLicenceInfo, AVP_int iSize );

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeySupportInfoS( TCHAR* pSupportInfo );
KLKR_PROC AVP_int   KLKR_PARAM  KLKR_GetKeySupportInfoSL( TCHAR* pSupportInfo, AVP_int iSize );

KLKR_PROC AVP_bool  KLKR_PARAM  KLKR_GetKeySerialNumberI( AVP_dword *pnNumb1, AVP_dword *pnNumb2, AVP_dword *pnNumb3 );
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeySerialNumberS( TCHAR *pSerialNumb );
KLKR_PROC AVP_int 	KLKR_PARAM  KLKR_GetKeySerialNumberSL( TCHAR *pSerialNumb, AVP_int iSize );

KLKR_PROC AVP_dword	KLKR_PARAM  KLKR_GetKeyPLPositionI();
KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyPLPositionS( TCHAR *pPLPosition );
KLKR_PROC AVP_int 	KLKR_PARAM  KLKR_GetKeyPLPositionSL( TCHAR *pPLPosition, AVP_int iSize );

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyPLPositionNameS( TCHAR *pPLPositionName );
KLKR_PROC AVP_int 	KLKR_PARAM  KLKR_GetKeyPLPositionNameSL( TCHAR *pPLPositionName, AVP_int iSize );

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyDescriptionS( TCHAR *pDescription );
KLKR_PROC AVP_int 	KLKR_PARAM  KLKR_GetKeyDescriptionSL( TCHAR *pDescription, AVP_int iSize );

KLKR_PROC AVP_bool	KLKR_PARAM  KLKR_GetKeyProductionDateS( TCHAR *pProdDate );
KLKR_PROC AVP_int 	KLKR_PARAM  KLKR_GetKeyProductionDateSL( TCHAR *pProdDate, AVP_int iSize );

KLKR_PROC AVP_dword KLKR_PARAM  KLKR_GetKeyHasProductI( long nProductID );

KLKR_PROC AVP_dword KLKR_PARAM  KLKR_GetLicenceTypeI();

KLKR_PROC AVP_bool  KLKR_PARAM  KLKR_GetKeyIsToProduct( long nProductID );

#endif

/*
    ct_funcs.h

    Functions of the Cryptographic Toolkit.
    Version 2.1

    Last changes: 20.11.96 16:49

    Copyright (c) LAN Crypto. All Rights Reserved.
*/

#include "CT_STRCT.h"

#ifdef __cplusplus
extern "C" {
#endif

CTAPIPREFIX CT_PTR          CTAPI CT_Version( void );
CTAPIPREFIX CT_PTR          CTAPI CT_BuildDate( void );
CTAPIPREFIX CT_INT          CTAPI CT_ErasePrivateKeyTM( CT_INT );

CTAPIPREFIX CTN_Signature * CTAPI CTN_SignConstructor( void );
CTAPIPREFIX void            CTAPI CTN_SignInitVariables( CTN_Signature * );
CTAPIPREFIX void            CTAPI CTN_SignDestructor( CTN_Signature * );

CTAPIPREFIX CTN_Notary *    CTAPI CTN_NotConstructor( void );
CTAPIPREFIX void            CTAPI CTN_NotInitVariables( CTN_Notary * );
CTAPIPREFIX void            CTAPI CTN_NotDestructor( CTN_Notary * );

CTAPIPREFIX CT_INT          CTAPI CTN_GetPrivateKeyFile( CTN_Signature *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTN_FillPrivateKeyStructure( CTN_Signature * );
CTAPIPREFIX CT_INT          CTAPI CTN_FillPublicKeyStructure( CTN_Signature *sign, CT_ULONG flags, CTN_Notary *not );

CTAPIPREFIX void            CTAPI CTN_CreateNewPrivateKey( CTN_Signature *, CT_PTR, CT_INT );
CTAPIPREFIX CT_INT          CTAPI CTN_SavePrivateKeyStructureToFile( CTN_Signature *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTN_ErasePrivateKeyFile( CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTN_SavePrivateKeyStructureToTM( CTN_Signature *, CT_INT );
CTAPIPREFIX CT_INT          CTAPI CTN_GetPrivateKeyFromTM( CTN_Signature *, CT_INT, CT_PTR );

CTAPIPREFIX void            CTAPI CTN_ModifyAdditionKey( CTN_PrivateKey *, CT_PTR, CT_INT );

CTAPIPREFIX CT_INT          CTAPI CTN_CreatePublicKeyFile( CTN_Signature *, CT_ULONG, CT_UINT, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTN_ReadPublicKeyFile( CTN_Notary *, CT_PTR );

CTAPIPREFIX CT_INT          CTAPI CTN_RetrieveInfoFromVault( CTN_Notary *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTN_CreateVaultFile( CTN_Notary *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTN_ReadPublicKeyFromVault( CTN_Notary *, CT_PTR, CT_ULONG );
CTAPIPREFIX CT_INT          CTAPI CTN_WritePublicKeyToVault( CTN_Notary *, CT_UINT );
CTAPIPREFIX CT_INT          CTAPI CTN_FindFirstPublicKeyInVault( CTN_Notary * );
CTAPIPREFIX CT_INT          CTAPI CTN_FindNextPublicKeyInVault( CTN_Notary * );
CTAPIPREFIX CT_INT          CTAPI CTN_RemovePublicKeyFromVault( CTN_Notary *, CT_PTR, CT_ULONG );

CTAPIPREFIX CT_INT          CTAPI CTN_CreateFullPublicKeyFile( CTN_Notary *, CT_UINT, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTN_ReadFullPublicKeyFile( CTN_Notary *, CT_PTR );

CTAPIPREFIX void            CTAPI CTN_SHAInit( CTN_Hash * );
CTAPIPREFIX void            CTAPI CTN_SHABuffer( CTN_Hash *, CT_PTR, CT_INT );
CTAPIPREFIX void            CTAPI CTN_SHAEnd( CTN_Hash * );

CTAPIPREFIX void            CTAPI CTN_R50Init( CTN_Hash * );
CTAPIPREFIX void            CTAPI CTN_R50Buffer( CTN_Hash *, CT_PTR, CT_INT );
CTAPIPREFIX void            CTAPI CTN_R50End( CTN_Hash * );

CTAPIPREFIX CT_INT          CTAPI CTN_HashInit( CTN_Hash *, CT_ULONG );
CTAPIPREFIX CT_INT          CTAPI CTN_HashBuffer( CTN_Hash *, CT_PTR, CT_INT );
CTAPIPREFIX CT_INT          CTAPI CTN_HashEnd( CTN_Hash * );

CTAPIPREFIX void            CTAPI CTN_DSASign( CTN_Signature * );
CTAPIPREFIX CT_INT          CTAPI CTN_DSANot( CTN_Notary * );

CTAPIPREFIX void            CTAPI CTN_GOSTSign( CTN_Signature * );
CTAPIPREFIX CT_INT          CTAPI CTN_GOSTNot( CTN_Notary * );

CTAPIPREFIX void            CTAPI CTN_LANSSign( CTN_Signature * );
CTAPIPREFIX CT_INT          CTAPI CTN_LANSNot( CTN_Notary * );

CTAPIPREFIX CT_INT          CTAPI CTN_Sign( CTN_Signature *, CT_ULONG );
CTAPIPREFIX CT_INT          CTAPI CTN_Not( CTN_Notary * );

CTAPIPREFIX CT_INT          CTAPI CTN_SignFile( CTN_Signature *, CT_PTR, CT_ULONG, CT_UINT, CTBACKPROC );
CTAPIPREFIX CT_INT          CTAPI CTN_IsSigned( CT_PTR, CTN_SignedFileInfo * );
CTAPIPREFIX CT_INT          CTAPI CTN_NotFile( CT_PTR, CT_PTR, CT_UINT, CT_INT, CTN_NotResult [], CTBACKPROC );
CTAPIPREFIX CT_INT          CTAPI CTN_RemoveSignaturesFromFile( CT_PTR, CTBACKPROC );

CTAPIPREFIX CTV_Vesta *     CTAPI CTV_VestaConstructor( void );
CTAPIPREFIX void            CTAPI CTV_VestaInitVariables( CTV_Vesta * );
CTAPIPREFIX void            CTAPI CTV_VestaDestructor( CTV_Vesta * );

CTAPIPREFIX CT_INT          CTAPI CTV_GetPrivateKeyFile( CTV_Vesta *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTV_FillPrivateKeyStructure( CTV_Vesta * );
CTAPIPREFIX void            CTAPI CTV_CreateNewKeys( CTV_Vesta *, CT_PTR, CT_INT );
CTAPIPREFIX CT_INT          CTAPI CTV_SavePrivateKeyStructureToFile( CTV_Vesta *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTV_ErasePrivateKeyFile( CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTV_SavePrivateKeyStructureToTM( CTV_Vesta *, CT_INT );
CTAPIPREFIX CT_INT          CTAPI CTV_GetPrivateKeyFromTM( CTV_Vesta *, CT_INT, CT_PTR );

CTAPIPREFIX void            CTAPI CTV_ModifyAdditionKey( CTV_PrivateKey *, CT_PTR, CT_INT );

CTAPIPREFIX CT_INT          CTAPI CTV_CreatePublicKeyFile( CTV_Vesta *, CT_ULONG, CT_UINT, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTV_SavePublicKeyToPublicKeyFile( CTV_Vesta *, CT_UINT, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTV_ReadPublicKeyFile( CTV_Vesta *, CT_PTR );

CTAPIPREFIX CT_INT          CTAPI CTV_RetrieveInfoFromVault( CTV_Vesta *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTV_CreateVaultFile( CTV_Vesta *, CT_PTR );
CTAPIPREFIX CT_INT          CTAPI CTV_ReadPublicKeyFromVaultByShortName( CTV_Vesta *, CT_PTR, CT_ULONG );
CTAPIPREFIX CT_INT          CTAPI CTV_ReadPublicKeyFromVault( CTV_Vesta *, CT_PTR, CT_ULONG );
CTAPIPREFIX CT_INT          CTAPI CTV_WritePublicKeyToVault( CTV_Vesta *, CT_UINT );
CTAPIPREFIX CT_INT          CTAPI CTV_FindFirstPublicKeyInVault( CTV_Vesta * );
CTAPIPREFIX CT_INT          CTAPI CTV_FindNextPublicKeyInVault( CTV_Vesta * );
CTAPIPREFIX CT_INT          CTAPI CTV_RemovePublicKeyFromVault( CTV_Vesta *, CT_PTR, CT_ULONG  );

CTAPIPREFIX CT_INT          CTAPI CTV_SetCryptoKey( CTV_Vesta *, CT_PTR, CT_ULONG );
CTAPIPREFIX CT_INT          CTAPI CTV_SetDecryptoKey( CTV_Vesta *, CT_PTR, CT_ULONG );

CTAPIPREFIX CT_INT          CTAPI CTV_CryptoInit( CTV_Vesta *, CT_ULONG, CT_UINT );
CTAPIPREFIX CT_INT          CTAPI CTV_CryptoBuffer( CTV_Vesta *, CT_PTR, CT_INT );

CTAPIPREFIX CT_INT          CTAPI CTV_CopyPublicKeyToWorkGroupStructure( CTV_Vesta *, CTV_WorkGroupKey * );
CTAPIPREFIX CT_INT          CTAPI CTV_WDecryptoInit( CTV_Vesta *, CT_ULONG, CT_INT, CTV_WorkGroupKey [] );
CTAPIPREFIX CT_INT          CTAPI CTV_WCryptoInit( CTV_Vesta *, CT_ULONG, CT_INT, CTV_WorkGroupKey [] );

CTAPIPREFIX CT_INT          CTAPI CTV_DecryptoInit( CTV_Vesta *, CT_ULONG, CT_UINT );
CTAPIPREFIX CT_INT          CTAPI CTV_DecryptoBuffer( CTV_Vesta *, CT_PTR, CT_INT );

CTAPIPREFIX CT_INT          CTAPI CTV_CryptoFile( CTV_Vesta *, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_ULONG, CT_UINT, CTBACKPROC );
CTAPIPREFIX CT_INT          CTAPI CTV_IsEncrypted( CT_PTR, CTV_EncryptedFileInfo * );
CTAPIPREFIX CT_INT          CTAPI CTV_DecryptoFile( CTV_Vesta *, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_UINT, CTBACKPROC );

#ifdef _DLL
CTAPIPREFIX CT_PTR 			CTAPI CT_LastErrorGetFileName( void );
CTAPIPREFIX CT_INT 			CTAPI CT_LastErrorGetErrNo( void );
#endif
#ifdef __cplusplus
}
#endif


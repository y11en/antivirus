/*
    ct_strct.h

    Structures for the Cryptographic Toolkit.
    Version 2.1

    Last changes: 20.11.96 15:04

    Copyright (c) LAN Crypto. All Rights Reserved.
*/

#include "CT_DEFS.h"
#include "CT_TYPS.h"

#ifndef __CT_STRUCTURES__
#define __CT_STRUCTURES__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CT_s_ErrNo
    {
        CT_PTR              FileName;
        CT_INT              ErrNo;
    } CT_ErrNo;

typedef struct CT_s_Date
    {
        CT_INT              Day;
        CT_INT              Month;
        CT_INT              Year;
    } CT_Date;

typedef struct CTV_s_PrivateKey
    {
        CT_UCHAR            UserSN[CT_V_LEN_SN];
        CT_UCHAR            ShortName[CT_V_LEN_SHORTNAME];
        CT_UCHAR            UserName[CT_V_LEN_USERNAME];
        CT_UCHAR            EncryptedUserSN[2*CT_V_LEN_ONLYSN];
        CT_UCHAR            FileName[CT_LEN_MAXFILENAME];
        CT_UINT             NKeys;
        CT_UCHAR            XKey[CT_V_LEN_MAXXKEY];
        CT_UCHAR            PublicKey[CT_V_LEN_PUBLICKEY];
        CT_UCHAR            OldXKey[CT_V_LEN_MAXXKEY];
        CT_UCHAR            OldPublicKey[CT_V_LEN_PUBLICKEY];
        CT_UINT             XLen;
        CT_UINT             Flags;
        CT_ULONG            Methods;
        CT_UINT             NetWork;
        CT_ULONG            CrcKeyDisk;
        CT_ULONG            CrcTouchMem;
        CT_Date             CreateDate;
        CT_Date             FinishDate;
        CT_UCHAR            AdditionKey[CT_V_LEN_ADDITION_KEY];
        CT_UCHAR            DiskAdditionKey[CT_V_LEN_DISK_ADDITION_KEY];
        CT_UCHAR            TouchAdditionKey[CT_V_LEN_TOUCH_ADDITION_KEY];
    } CTV_PrivateKey;

typedef struct CTV_s_PublicKey
    {
        CT_UCHAR            UserSN[CT_V_LEN_SN];
        CT_UCHAR            UserName[CT_V_LEN_USERNAME];
        CT_UCHAR            ShortName[CT_V_LEN_SHORTNAME];
        CT_UCHAR            EncryptedUserSN[CT_V_LEN_ONLYSN];
        CT_ULONG            Methods;
        CT_Date             FinishDate;
        CT_UCHAR            PublicKey[CT_V_LEN_PUBLICKEY];
    } CTV_PublicKey;

typedef struct CTV_s_VaultInformation
    {
        CT_UCHAR            FileName[CT_LEN_MAXFILENAME];
        CT_ULONG            HTable[CT_LEN_FF_HASH_TABLE];
        CT_ULONG            NUsers;
        CT_ULONG            CUser;
    } CTV_VaultInfo;

typedef struct CTV_s_VaultList
    {
        CTV_VaultInfo       Vault;
        struct              CTV_s_VaultList *next;
    } CTV_VaultList;

typedef struct CTV_s_TransferInformation
    {
        CT_ULONG            Method;
        CT_ULONG            AddKey[2];
        CT_UCHAR            SenderShortName[CT_V_LEN_SHORTNAME];
        CT_UCHAR            ReceiverUserSN[CT_V_LEN_SN];
        CT_UCHAR            ReceiverShortName[CT_V_LEN_SHORTNAME];
        CT_UINT             CrcPubReceiver;
        CT_UINT             CrcPubSender;
    } CTV_TransfInfo;

typedef struct CTV_s_WorkGroupKey
    {
        CT_UCHAR            UserSN[CT_V_LEN_SN];
        CT_UCHAR            PublicKey[CT_V_LEN_PUBLICKEY];
        CT_UINT             CrcPub;
    } CTV_WorkGroupKey;

typedef struct CT_V_s_Vesta_2M
    {
        CT_UINT             X[31];
        CT_UCHAR            G[32];
        CT_UCHAR            H[2];
        CT_UINT             posg;
        CT_UINT             V;
        CT_UINT             W;
    } u_Vesta_2M;

typedef struct CT_V_s_FEAL
    {
        CT_UCHAR            Key[36];
    } u_FEAL;

typedef struct CT_V_s_DES
    {
        CT_INT              EncDecF;
        CT_INT              KeyLeng;
        CT_INT              Mode;
        CT_UCHAR            InitBlock[8];
        CT_ULONG            Kn1[32];
        CT_ULONG            Kn2[32];
        CT_ULONG            Kn3[32];
    } u_DES;

typedef struct CT_V_s_G28147
    {
        CT_UCHAR            Key[32];
        CT_ULONG            N1;
        CT_ULONG            N2;
    } u_G28147;

typedef struct CT_V_s_SAFER
    {
        CT_UCHAR            Key[104];
    } u_SAFER;

typedef struct CT_V_s_WICKER
	{
		CT_INT				KeyLength;
    	CT_ULONG			IV[4];
		CT_ULONG			KS[44];
	}u_WICKER;

typedef struct CTV_s_Vesta
    {
        CTV_PrivateKey      Private;
        CTV_PublicKey       Public;
        CTV_VaultInfo       Vault;
        CTV_TransfInfo      Transfer;
        CT_UINT             flags;
        CT_PTR              Password;
        CT_UCHAR            PrivateKeyBuffer[CT_V_LEN_PRIVATEKEYFILE];
        CT_UCHAR            buffer[CT_V_MAXLEN_WORKBUFFER];
        CT_ULONG            Method;
        union
        {
            u_Vesta_2M      Vesta_2M;
            u_FEAL          FEAL;
            u_DES           DES;
            u_G28147        G28147;
            u_SAFER         SAFER;
			u_WICKER		Wicker;
        } CT_V_u_Vesta;
    } CTV_Vesta;

typedef struct CTV_s_CTV_EncryptedFileInfo
    {
        CT_INT              Version;
        CT_UCHAR            SenderShortName[CT_V_LEN_SHORTNAME];
        CT_UCHAR            ReceiverUserSN[CT_V_LEN_SN];
        CT_INT              KeyLength;
        CT_UCHAR            FileName[CT_LEN_MAXFILENAME];
        CT_ULONG            Method;
        CT_INT              WorkGroups;
        CT_UINT             NUsers;
        CT_INT              EncryptedFileType;
    } CTV_EncryptedFileInfo;

typedef struct CTN_s_PrivateKey
    {
        CT_UCHAR            UserSN[CT_N_LEN_SN];
        CT_UCHAR            UserName[CT_N_LEN_USERNAME];
        CT_UCHAR            EncryptedUserSN[2*CT_N_LEN_ONLYSN];
        CT_UCHAR            FileName[CT_LEN_MAXFILENAME];
        CT_UINT             NKeys;
        CT_UCHAR            XKey[CT_N_LEN_MAXXKEY];
        CT_UINT             XLen;
        CT_UINT             Flags;
        CT_ULONG            Methods;
        CT_UCHAR            KKey[CT_N_LEN_KKEY];
        CT_ULONG            SHSt[5];
        CT_ULONG            CrcKeyDisk;
        CT_ULONG            CrcTouchMem;
        CT_Date             CreateDate;
        CT_Date             FinishDate;
        CT_UCHAR            AdditionKey[CT_N_LEN_ADDITION_KEY];
        CT_UCHAR            DiskAdditionKey[CT_N_LEN_DISK_ADDITION_KEY];
        CT_UCHAR            TouchAdditionKey[CT_N_LEN_TOUCH_ADDITION_KEY];
    } CTN_PrivateKey;

typedef struct CTN_s_PublicKey
    {
        CT_UCHAR            UserSN[CT_N_LEN_SN];
        CT_UCHAR            UserName[CT_N_LEN_USERNAME];
        CT_UCHAR            EncryptedUserSN[CT_N_LEN_ONLYSN];
        CT_ULONG            Methods;
        CT_Date             FinishDate;
        CT_UCHAR            B1[CT_N_LEN_SIGN];
        CT_UCHAR            B2[CT_N_LEN_SIGN];
        CT_UCHAR            B3[CT_N_LEN_SIGN];
        CT_UCHAR            B4[CT_N_LEN_SIGN];
    } CTN_PublicKey;

typedef struct CTN_s_VaultInformation
    {
        CT_UCHAR            FileName[CT_LEN_MAXFILENAME];
        CT_ULONG            HTable[CT_LEN_FF_HASH_TABLE];
        CT_ULONG            NUsers;
        CT_ULONG            CUser;
    } CTN_VaultInfo;

typedef struct CTN_s_VaultList
    {
        CTN_VaultInfo       Vault;
        struct              CTN_s_VaultList *next;
    } CTN_VaultList;

typedef struct CTN_s_NotResult
    {
        CT_UCHAR            UserSN[CT_N_LEN_SN];
        CT_UCHAR            UserName[CT_N_LEN_USERNAME];
        CT_ULONG            Method;
        CT_INT              Result;
    } CTN_NotResult;

typedef struct CT_N_s_R50
    {
        CT_UCHAR            hp[32];
        CT_UCHAR            sp[32];
        CT_UCHAR            l[32];
        CT_UCHAR            s[32];
        CT_UCHAR            buffer[32];
        CT_INT              ptrbuffer;
    } u_R50;

typedef struct CT_N_s_SHA
    {
        CT_ULONG            HV[5];
        CT_ULONG            W[16];
        CT_UINT             posh;
        CT_UINT             poshash;
    } u_SHA;

typedef struct CT_N_s_Hash
    {
        CT_UCHAR            HashValue[CT_N_LEN_MAXHASH];
        CT_ULONG            HashLen;
        CT_ULONG            Method;
        CT_UINT             flags;
        union
        {
            u_SHA           SHA;
            u_R50           R50;
        } CT_N_u_Hash;
    } CTN_Hash;

typedef struct CTN_s_Mark
    {
        CT_UCHAR            UserSN[CT_N_LEN_SN];
        CT_UCHAR            r[CT_N_LEN_MAXRS];
        CT_UCHAR            s[CT_N_LEN_MAXRS];
        CT_ULONG            RSType;
    } CTN_Mark;

typedef struct CTN_s_Signature
    {
        CTN_PrivateKey      Private;
        CTN_Hash            Hash;
        CTN_Mark            RS;
        CT_UINT             flags;
        CT_PTR              Password;
        CT_UCHAR            PrivateKeyBuffer[CT_N_LEN_PRIVATEKEYFILE];
        CT_UCHAR            buffer[CT_N_MAXLEN_WORKBUFFER];
    } CTN_Signature;

typedef struct CTN_s_Notarius
    {
        CTN_VaultInfo       Vault;
        CTN_PublicKey       Public;
        CTN_Hash            Hash;
        CTN_Mark            RS;
        CT_UINT             flags;
        CT_UCHAR            buffer[CT_N_MAXLEN_WORKBUFFER];
    } CTN_Notary;

typedef struct CTN_s_CTV_SignedFileInfo
    {
        CT_ULONG            Method;
        CT_INT              KeyLength;
    } CTN_SignedFileInfo;

#ifndef _DLL
extern CTAPIPREFIX CT_ErrNo CT_LastError;
#endif

#ifdef __cplusplus
}
#endif
#endif


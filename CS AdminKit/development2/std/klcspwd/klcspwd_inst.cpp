/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspwd_inst.cpp
 * \author	Andrew Kazachkov
 * \date	27.10.2004 16:05:23
 * \brief	
 * 
 */

#include <stdlib.h>

#include "std/klcspwd/klcspwd.h"
#include "std/klcspwd/klcspwdr.h"
#include "std/klcspwd/prtstrg.h"
#include "std/klcspwd/crypto.h"

namespace KLCSPWD
{
    static int SelectKeyType(
                        key_type_t      nKeyType,
                        const char*&    pPubKeyName,
                        const char*&    pPrvKeyName)
    {
        switch(nKeyType)
        {
        case KT_HOSTKEY:
            pPubKeyName = KLCSPWD::c_szLocalPubKey;
            pPrvKeyName = KLCSPWD::c_szLocalPrvKey;
            break;
        case KT_GLOBKEY:
            pPubKeyName = KLCSPWD::c_szGlobalPubKey;
            pPrvKeyName = KLCSPWD::c_szGlobalPrvKey;
			break;
        case KT_SRVRKEY:
            pPubKeyName = KLCSPWD::c_szServerPubKey;
            pPrvKeyName = KLCSPWD::c_szServerPrvKey;
            break;
        default:
            return -1;
        };
        return 0;
    };

    static int InstallDataProtection(key_type_t nKeyType)
    {
        void*   pPublicKey = NULL;
        size_t  nPublicKey = 0;
        void*   pPrivateKey = NULL;
        size_t  nPrivateKey = 0;
        ;
        const char* pPubKeyName = NULL;
        const char* pPrvKeyName = NULL;

        int nResult = SelectKeyType(nKeyType, pPubKeyName, pPrvKeyName);
        if(nResult)
            goto onCleanup;

        nResult = KLCSPWD::GenRsaKey(pPublicKey, nPublicKey, pPrivateKey, nPrivateKey);
        if(nResult)
            goto onCleanup;

        nResult = KLCSPWD::PutData(pPubKeyName, pPublicKey, nPublicKey);
        if(nResult)
            goto onCleanup;

        nResult = KLCSPWD::PutData(pPrvKeyName, pPrivateKey, nPrivateKey);
        if(nResult)
            goto onCleanup;
onCleanup:
        KLCSPWD::Free(pPublicKey, nPublicKey);
        KLCSPWD::Free(pPrivateKey, nPrivateKey);
        return nResult;

    }

    static int UninstallDataProtection(key_type_t nKeyType)
    {
        const char* pPubKeyName = NULL;
        const char* pPrvKeyName = NULL;

        int nResult = SelectKeyType(nKeyType, pPubKeyName, pPrvKeyName);
        if(nResult)
            return nResult;

        int nResult1 = KLCSPWD::PutData(pPubKeyName, NULL, 0);
        int nResult2 = KLCSPWD::PutData(pPrvKeyName, NULL, 0);
        return nResult1 ? nResult1 : nResult2;
    };
    
    KLCSPWD_DECL int UninstallDataProtection2(key_type_t nKeyType)
    {
        return UninstallDataProtection(nKeyType);
    };

    KLCSPWD_DECL int InstallDataProtection2(key_type_t nKeyType)
    {
        return InstallDataProtection(nKeyType);
    };


    KLCSPWD_DECL bool IsDataProtectionInstalled()
    {
        void*   pData = NULL;
        size_t  nData = 0;
        int nResult = 0;
        nResult = GetData(c_szLocalPubKey, pData, nData);
        Free(pData, nData);
        return nResult == 0;
    };


    KLCSPWD_DECL int InstallDataProtection()
    {
        bool bDoInstall = false;
        {
            void*   pPublicKey = NULL;
            size_t  nPublicKey = 0;
            void*   pPrivateKey = NULL;
            size_t  nPrivateKey = 0;
            bDoInstall = (0 != KLCSPWD::GetData( KLCSPWD::c_szLocalPubKey, 
                                                 pPublicKey, 
                                                 nPublicKey))    ||
                        (0 != KLCSPWD::GetData(  KLCSPWD::c_szLocalPrvKey, 
                                                 pPrivateKey, 
                                                 nPrivateKey));
            Free(pPublicKey, nPublicKey);
            Free(pPrivateKey, nPrivateKey);
        };
        return bDoInstall ? InstallDataProtection(KT_HOSTKEY) : 0;
    };

    KLCSPWD_DECL int UninstallDataProtection()
    {
        return 0;
        //return UninstallDataProtection(KT_HOSTKEY);
    };
    
    KLCSPWD_DECL int InstallServerKey()
    {
        bool bDoInstall = false;
        {
            void*   pPublicKey = NULL;
            size_t  nPublicKey = 0;
            void*   pPrivateKey = NULL;
            size_t  nPrivateKey = 0;
            bDoInstall = (0 != KLCSPWD::GetData( KLCSPWD::c_szGlobalPubKey, 
                                                 pPublicKey, 
                                                 nPublicKey))    ||
                        (0 != KLCSPWD::GetData(  KLCSPWD::c_szGlobalPrvKey, 
                                                 pPrivateKey, 
                                                 nPrivateKey));
            Free(pPublicKey, nPublicKey);
            Free(pPrivateKey, nPrivateKey);
        };
        return bDoInstall ? InstallDataProtection(KT_GLOBKEY) : 0;
    }

    KLCSPWD_DECL int UninstallServerKey()
    {
        return UninstallDataProtection(KT_GLOBKEY);
    }

    KLCSPWD_DECL int InstallSettingsServerKey()
    {
        bool bDoInstall = false;
        {
            void*   pPublicKey = NULL;
            size_t  nPublicKey = 0;
            void*   pPrivateKey = NULL;
            size_t  nPrivateKey = 0;
            bDoInstall = (0 != KLCSPWD::GetData( KLCSPWD::c_szServerPubKey, 
                                                 pPublicKey, 
                                                 nPublicKey))    ||
                        (0 != KLCSPWD::GetData(  KLCSPWD::c_szServerPrvKey, 
                                                 pPrivateKey, 
                                                 nPrivateKey));
            Free(pPublicKey, nPublicKey);
            Free(pPrivateKey, nPrivateKey);
        };
        return bDoInstall ? InstallDataProtection(KT_SRVRKEY) : 0;
    };

    KLCSPWD_DECL int UninstallSettingsServerKey()
    {
        return UninstallDataProtection(KT_SRVRKEY);
    };
};
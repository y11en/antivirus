/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspwd.cpp
 * \author	Andrew Kazachkov
 * \date	27.10.2004 16:05:15
 * \brief	
 * 
 */

#include <stdlib.h>
#ifndef N_PLAT_NLM
 #include <memory.h>
#else
 #include <string.h>
#endif

#include "std/klcspwd/klcspwd.h"
#include "std/klcspwd/prtstrg.h"
#include "std/klcspwd/crypto.h"
#include "std/klcspwd/klcspwdr.h"
#include "std/klcspwd/errorcodes.h"

namespace KLCSPWD
{
    const void*     c_pHostPrefix = &c_szHostPrefix[0];
    const size_t    c_nHostPrefix = c_nPrefix;

    int UnprotectData(
                    const void* pProtectedData, 
                    size_t      nProtectedData, 
                    void*&      pData,
                    size_t&     nData)
    {
        if(!pProtectedData || !nProtectedData )
            return KLCSPWD_BADARG;
        if(pData || nData)
            return KLCSPWD_BADARG;
        if(nProtectedData <= c_nPrefix)
            return KLCSPWD_BADARG;
        ;
        int nResult = 0;
        ;
        void*   pPrivateKey = NULL;
        size_t  nPrivateKey = 0;
        void*   pPlainText = NULL;
        size_t  nPlainText = 0;
        {
            if(memcmp(pProtectedData, c_szHostPrefix, c_nPrefix) == 0)
                nResult = KLCSPWD::GetData(KLCSPWD::c_szLocalPrvKey, pPrivateKey, nPrivateKey);
            else if(memcmp(pProtectedData, c_szGlobPrefix, c_nPrefix) == 0)
                nResult = KLCSPWD::GetData(KLCSPWD::c_szGlobalPrvKey, pPrivateKey, nPrivateKey);
            else if(memcmp(pProtectedData, c_szSrvrPrefix, c_nPrefix) == 0)
                nResult = KLCSPWD::GetData(KLCSPWD::c_szServerPrvKey, pPrivateKey, nPrivateKey);           
            else
                nResult = KLCSPWD_BADARG;
            if(nResult)
                goto onCleanup;
            nResult = KLCSPWD::DecryptData(
                                        pPrivateKey,
                                        nPrivateKey,
                                        c_nHostPrefix,
                                        pProtectedData,
                                        nProtectedData,
                                        pPlainText,
                                        nPlainText);
            if(nResult)
                goto onCleanup;
            pData = pPlainText;
            nData = nPlainText;
            pPlainText = NULL;
            nPlainText = 0;
            nResult = 0;
        };
    onCleanup:
        Free(pPrivateKey, nPrivateKey);
        Free(pPlainText, nPlainText);
        return nResult;
    };

    int ProtectDataLocally(
                    const void* pData,
                    size_t      nData,
                    void*&      pProtectedData,
                    size_t&     nProtectedData)
    {
        if(!pData && nData)
            return KLCSPWD_BADARG;
        if(pProtectedData || nProtectedData)
            return KLCSPWD_BADARG;
        ;
        int nResult = 0;
        ;
        void*   pPublicKey = NULL;
        size_t  nPublicKey = 0;
        void*   pCipherText = NULL;
        size_t  nCipherText = 0;
        {
            nResult = KLCSPWD::GetData(KLCSPWD::c_szLocalPubKey, pPublicKey, nPublicKey);
            if(nResult)
                goto onCleanup;
            nResult = KLCSPWD::EncryptData(
                                    pPublicKey,
                                    nPublicKey,
                                    c_pHostPrefix,
                                    c_nHostPrefix,
                                    pData,
                                    nData,
                                    pCipherText,
                                    nCipherText);
            if(nResult)
                goto onCleanup;
            pProtectedData = pCipherText;
            nProtectedData = nCipherText;
            pCipherText = NULL;
            nCipherText = 0;
            nResult = 0;
        };
    onCleanup:
        Free(pPublicKey, nPublicKey);
        Free(pCipherText, nCipherText);
        return nResult;
    };

    KLCSPWD_DECL void Initialize()
    {
        initialize(true);
    };
    
    KLCSPWD_DECL void Deinitialize()
    {
        deinitialize();
    }

    int ProtectDataForKey(
                    const void*   pData,
                    size_t        nData,
                    const void*   pPublicKey,
                    size_t        nPublicKey,
                    key_type_t    nKeyType,
                    void*&        pProtectedData,
                    size_t&       nProtectedData)
    {
        const char* pPrefix = NULL;
        switch(nKeyType)
        {
        case KT_HOSTKEY:
            pPrefix = c_szHostPrefix;
            break;
        case KT_GLOBKEY:
            pPrefix = c_szGlobPrefix;
            break;
        case KT_SRVRKEY:
            pPrefix = c_szSrvrPrefix;
            break;
        default:
            return KLCSPWD_BADARG;            
        };
        return EncryptData(
                pPublicKey,
                nPublicKey,
                pPrefix,
                c_nPrefix,
                pData,
                nData,
                pProtectedData,
                nProtectedData);
    };
};

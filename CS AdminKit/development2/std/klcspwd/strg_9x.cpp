/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	strg_9x.cpp
 * \author	Andrew Kazachkov
 * \date	01.11.2004 11:11:49
 * \brief	
 * 
 */

#include "std/klcspwd/klcspwd_common.h"

#ifdef KLSTD_WIN9X
#include <std/win32/klos_win32v40.h>

#include <wincrypt.h>

#include "std/klcspwd/crypto.h"
#include "std/klcspwd/prtstrg.h"

#include "avp/parray.h"
#include "avp/text2bin.h"
#include "AVPRegID.h"

namespace KLCSPWD
{
    BOOL CreateContainer(LPCTSTR szContainerName, HCRYPTPROV* phProv)
    {
        HCRYPTPROV hProv=NULL;
        BOOL bResult = FALSE;
        ;
        bResult = CryptAcquireContext(
                                &hProv,
                                szContainerName,
                                MS_DEF_PROV,
                                PROV_RSA_FULL,
                                CRYPT_NEWKEYSET|CRYPT_MACHINE_KEYSET);
        if(!bResult)
            goto onCleanup;
        ;
        if(phProv)
        {
            *phProv = hProv;
            hProv = NULL;
        };
        ;
    onCleanup:
        DWORD dwError = bResult ? 0 : GetLastError();
        if(hProv)
            CryptReleaseContext(hProv, 0);
        SetLastError(dwError);
        return bResult;
    };

    BOOL CreateExchangeKeyPair(HCRYPTPROV hProv, HCRYPTKEY* phKey)
    {
        HCRYPTKEY hKey = NULL;
        BOOL bResult = FALSE;
        ;
        bResult = CryptGenKey(hProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hKey);
        if(!bResult)
            goto onCleanup;
        ;
        if(phKey)
        {
            *phKey = hKey;
            hKey = NULL;
        };
        ;
    onCleanup:
        DWORD dwError = bResult ? 0 : GetLastError();
        if(hKey)
            CryptDestroyKey(hKey);
        SetLastError(dwError);
        return bResult;
    };

    INT ProtectDataCAPI(const char* szKeyName, const void* pData, size_t nData, void** ppData, size_t* pnData)
    {
        USES_CONVERSION;
        ;
        HCRYPTPROV hProv=NULL;
        BOOL bResult = FALSE;
        ;
        HCRYPTKEY hExchangeKey = NULL;
        HCRYPTKEY hSessionKey = NULL;
        void*   pTempBuffer = NULL;
        DWORD   dwTempBuffer = 0;
        void*   pSessionKeyBlob = NULL;
        DWORD   dwSessionKeyBlob = 0;
        void*   pResultBuffer = NULL;
        size_t  nResultBuffer = 0;
        {
            if(!(bResult = CryptAcquireContext(
                                    &hProv,
                                    A2CT(szKeyName),
                                    MS_DEF_PROV,
                                    PROV_RSA_FULL,
                                    CRYPT_MACHINE_KEYSET)))
            {
                DWORD dwError = GetLastError();
                if(dwError == NTE_BAD_KEYSET || dwError == NTE_KEYSET_NOT_DEF)
                {
                    bResult = CreateContainer(A2CT(szKeyName), &hProv);
                    if(!bResult)
                        goto onCleanup;
                }
                else
                    goto onCleanup;
            };
            ;
            if(!(bResult = CryptGetUserKey(hProv, AT_KEYEXCHANGE, &hExchangeKey)))
            {
                DWORD dwError = GetLastError();
                if(dwError == NTE_NO_KEY)
                {
                    bResult = CreateExchangeKeyPair(hProv, &hExchangeKey);
                    if(!bResult)
                        goto onCleanup;
                }
                else
                    goto onCleanup;
            };
            ;   
            bResult = CryptGenKey(hProv, CALG_RC4, CRYPT_EXPORTABLE, &hSessionKey);
            if(!bResult)
                goto onCleanup;
            ;
            //determining size of output buffer is redundant because RC4 is a stream cipher but as general it's required
            DWORD dwTempBufferSize = nData;
            bResult = CryptEncrypt(hSessionKey, NULL, TRUE, 0, NULL, &dwTempBufferSize, dwTempBufferSize);
            if(!bResult)
                goto onCleanup;
            pTempBuffer = malloc(dwTempBufferSize);
            dwTempBuffer = nData;
            memcpy(pTempBuffer, pData, nData);
            bResult = CryptEncrypt(hSessionKey, NULL, TRUE, 0, (PBYTE)pTempBuffer, &dwTempBuffer, dwTempBufferSize);
            if(!bResult)
                goto onCleanup;
            ;
            bResult = CryptExportKey(hSessionKey, hExchangeKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlob);
            if(!bResult)
                goto onCleanup;
            pSessionKeyBlob = malloc(dwSessionKeyBlob);
            bResult = CryptExportKey(hSessionKey, hExchangeKey, SIMPLEBLOB, 0, (PBYTE)pSessionKeyBlob, &dwSessionKeyBlob);
            if(!bResult)
                goto onCleanup;
            ;
            nResultBuffer = dwTempBuffer + dwSessionKeyBlob + sizeof(dwSessionKeyBlob) + sizeof(dwTempBuffer);
            pResultBuffer = malloc(nResultBuffer);
            PBYTE p = (PBYTE)pResultBuffer;
            memcpy(p, &dwSessionKeyBlob, sizeof(dwSessionKeyBlob));
            p += sizeof(dwSessionKeyBlob);
            memcpy(p, pSessionKeyBlob, dwSessionKeyBlob);
            p += dwSessionKeyBlob;
            memcpy(p, &dwTempBuffer, sizeof(dwTempBuffer));
            p += sizeof(dwTempBuffer);
            memcpy(p, pTempBuffer, dwTempBuffer);
            p += dwTempBuffer;
            _ASSERTE(p  == nResultBuffer + PBYTE(pResultBuffer));
            *ppData = pResultBuffer;
            *pnData = nResultBuffer;
            pResultBuffer = NULL;
        };
    onCleanup:
        DWORD dwError = bResult ? 0 : GetLastError();
        if(hSessionKey)
            CryptDestroyKey(hSessionKey);
        if(hExchangeKey)
            CryptDestroyKey(hExchangeKey);
        if(hProv)
            CryptReleaseContext(hProv, 0);
        Free(pTempBuffer, dwTempBuffer);
        Free(pSessionKeyBlob, dwSessionKeyBlob);
        Free(pResultBuffer, nResultBuffer);
        return (int)dwError;
    };

    INT UnprotectDataCAPI(const char* szKeyName, const void* pResultBuffer, size_t nResultBuffer, void** ppData, size_t* pnData)
    {
        USES_CONVERSION;
        ;
        HCRYPTPROV hProv=NULL;
        BOOL bResult = FALSE;
        ;
        HCRYPTKEY hExchangeKey = NULL;
        HCRYPTKEY hSessionKey = NULL;
        void*   pTempBuffer = NULL;
        DWORD   dwTempBuffer = 0;
        void*   pSessionKeyBlob = NULL;
        DWORD   dwSessionKeyBlob = 0;
        void*   pData = NULL;
        DWORD   dwData = 0;
        {
            const BYTE* p = (const BYTE*)pResultBuffer;
            memcpy(&dwSessionKeyBlob, p, sizeof(dwSessionKeyBlob));
            p += sizeof(dwSessionKeyBlob);
            pSessionKeyBlob = malloc(dwSessionKeyBlob);
            memcpy(pSessionKeyBlob, p, dwSessionKeyBlob);
            p += dwSessionKeyBlob;
            memcpy(&dwData, p, sizeof(dwData));
            p += sizeof(dwData);
            pData = malloc(dwData);
            memcpy(pData, p, dwData);
            p += dwData;
            _ASSERTE(p == PBYTE(pResultBuffer) + nResultBuffer);
            ;
            bResult = CryptAcquireContext(
                                    &hProv,
                                    A2CT(szKeyName),
                                    MS_DEF_PROV,
                                    PROV_RSA_FULL,
                                    CRYPT_MACHINE_KEYSET);
            if(!bResult)
                goto onCleanup;
            ;
            bResult = CryptGetUserKey(hProv, AT_KEYEXCHANGE, &hExchangeKey);
            if(!bResult)
                goto onCleanup;
            ;   
            bResult = CryptImportKey(hProv, (PBYTE)pSessionKeyBlob, dwSessionKeyBlob, hExchangeKey, 0, &hSessionKey);
            if(!bResult)
                goto onCleanup;
            ;
            //determining size of output buffer is redundant because RC4 is a stream cipher but as general it's required
            pTempBuffer = malloc(dwData);
            dwTempBuffer = dwData;
            memcpy(pTempBuffer, pData, dwData);
            bResult = CryptDecrypt(hSessionKey, NULL, TRUE, 0, (PBYTE)pTempBuffer, &dwTempBuffer);
            if(!bResult)
                goto onCleanup;
            ;
            *ppData = pTempBuffer;
            *pnData = dwTempBuffer;
            pTempBuffer = NULL;
        };
    onCleanup:
        DWORD dwError = bResult ? 0 : GetLastError();
        if(hSessionKey)
            CryptDestroyKey(hSessionKey);
        if(hExchangeKey)
            CryptDestroyKey(hExchangeKey);
        if(hProv)
            CryptReleaseContext(hProv, 0);
        Free(pTempBuffer, dwTempBuffer);
        Free(pSessionKeyBlob, dwSessionKeyBlob);
        Free(pData, dwData);
        return (int)dwError;
    };
    TCHAR c_szRegKeyName[] = AVP_REGKEY_COMPONENTS_FULL _SEP_ _T("34\\.core\\.independent\\dataprotection");

    int PutData(const char* szKey, const void* pData, size_t nData)
    {
        USES_CONVERSION;
        int nResult = 0;
        void*   pProtectedData = NULL;
        size_t  nProtectedData = 0;
        {
            nResult = ProtectDataCAPI(
                            szKey,
                            pData,
                            nData,
                            &pProtectedData,
                            &nProtectedData);
            if(nResult)
                goto onCleanup;
            CRegKey key;
            if(pData)
            {
                nResult = key.Create(
                        HKEY_LOCAL_MACHINE,
                        c_szRegKeyName,
                        REG_NONE,
                        REG_OPTION_NON_VOLATILE,
                        KEY_WRITE|KEY_READ,
                        NULL,
                        NULL);
                if(nResult)
                    goto onCleanup;
                nResult = RegSetValueEx(
                            key,
                            A2CT(szKey),
                            NULL,
                            REG_BINARY,
                            (CONST BYTE*)pProtectedData,
                            nProtectedData);
                if(nResult)
                    goto onCleanup;
            }
            else
            {
                RegDeleteKey(HKEY_LOCAL_MACHINE, c_szRegKeyName);
            };
        };
    onCleanup:
        Free(pProtectedData, nProtectedData);
        return nResult;
    };

    int GetData(const char* szKey, void*& pData, size_t& nData)
    {
        USES_CONVERSION;
        int nResult = 0;
        void*   pProtectedData = NULL;
        DWORD   dwProtectedData = 0;
        void*   pUnprotectedData = NULL;
        size_t  nUnprotectedData = 0;
        {
            CRegKey key;
            nResult = key.Open(
                    HKEY_LOCAL_MACHINE,
                    c_szRegKeyName,
                    KEY_READ);
            if(nResult)
                goto onCleanup;
            nResult = RegQueryValueEx(
                        key,
                        A2CT(szKey),
                        NULL,
                        NULL,
                        NULL,
                        &dwProtectedData);
            if(dwProtectedData)
                nResult = 0;
            if(nResult)
                goto onCleanup;
            pProtectedData = malloc(dwProtectedData);
            nResult = RegQueryValueEx(
                        key,
                        A2CT(szKey),
                        NULL,
                        NULL,
                        (BYTE*)pProtectedData,
                        &dwProtectedData);
            if(nResult)
                goto onCleanup;
            nResult = UnprotectDataCAPI(
                        szKey,
                        pProtectedData,
                        dwProtectedData,
                        &pUnprotectedData,
                        &nUnprotectedData);
            if(nResult)
                goto onCleanup;
            pData = pUnprotectedData;            
            nData = nUnprotectedData;
            pUnprotectedData = NULL;
            nUnprotectedData = 0;
        };
    onCleanup:
        Free(pProtectedData, dwProtectedData);
        Free(pUnprotectedData, nUnprotectedData);
        return nResult;
    };
};

#endif
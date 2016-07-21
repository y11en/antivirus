/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	strg_nt.cpp
 * \author	Andrew Kazachkov
 * \date	29.10.2004 10:47:35
 * \brief	
 * 
 */

#include "std/klcspwd/klcspwd_common.h"

#ifdef KLSTD_WINNT

#include <std/win32/klos_win32v40.h>
#include <Ntsecapi.h>
#include "./conv.h"
#include "std/klcspwd/prtstrg.h"

#include "avp/parray.h"
#include "avp/text2bin.h"

namespace KLCSPWD
{
    int UnicodeStringToBinary(
                    const wchar_t*  szwString, 
                    void*&          pData,
                    size_t&         nData)
    {
        if(!szwString)
            return (int)ERROR_BAD_ARGUMENTS;
        KLCSPWD_CW2A    szText = szwString;
        const size_t    nText = strlen(szText);
        size_t    nBlock = T2B_LENGTH(nText);
        CArrayPointer<char> pBlock;
        pBlock.Allocate(nBlock);
        if(!(char*)pBlock)
            return (int)ERROR_NOT_ENOUGH_MEMORY;
        memset((char*)pBlock, 0, nBlock);
        if(!(nBlock = TextToBin(szText, nText, pBlock, nBlock)))
            return (int)ERROR_BAD_ARGUMENTS;
        ;
        nData = nBlock;
        pData = pBlock.Relinquish();
        return 0;
    };

    int BinaryToUnicodeString(
                    const void*             pData,
                    size_t                  nData,
                    CArrayPointer<wchar_t>& pStringW)
    {
        const size_t nBlock = B2T_LENGTH(nData);
        CArrayPointer<char> pBlock;
        pBlock.Allocate(nBlock + 1);
        if(!(char*)pBlock)
            return (int)ERROR_NOT_ENOUGH_MEMORY;
        memset((char*)pBlock, 0, nBlock + 1);
        if( !BinToText(pData, nData, pBlock, nBlock))
            return (int)ERROR_BAD_ARGUMENTS;
        ;
        KLCSPWD_CA2W data(pBlock);
        pStringW.Allocate(wcslen(data) + 1);
        if(!(wchar_t*)pStringW)
            return (int)ERROR_NOT_ENOUGH_MEMORY;
        wcscpy(pStringW, data);
        return 0;
    };

    int MakeKeyString(const char* szKey, CArrayPointer<wchar_t>& pKeyName)
    {
        KLCSPWD_CA2W Data(szKey);
        const size_t nData = wcslen(Data);
        const wchar_t c_szwPrefix[] = L"L$";
        const size_t c_nPrefix = KLCSPWD_COUNTOF(c_szwPrefix) - 1;
        pKeyName.Allocate(c_nPrefix + nData + 1);
        if(!(wchar_t*)pKeyName)
            return (int)ERROR_NOT_ENOUGH_MEMORY;
        wcscpy(pKeyName, c_szwPrefix);
        wcscat(pKeyName, Data);
        return 0;
    }

    int PutData(const char* szKey, const void* pData, size_t nData)
    {
        NTSTATUS ntsResult = ERROR_SUCCESS;
        LSA_HANDLE lsahPolicyHandle = NULL;
        {
            LSA_OBJECT_ATTRIBUTES lsaObjectAttributes;
            ZeroMemory( &lsaObjectAttributes, sizeof( lsaObjectAttributes ) );        
            ntsResult =  ::LsaOpenPolicy(
                        NULL, //Name of the target system (NULL = local).
                        & lsaObjectAttributes, // Reserved parameter.
                        POLICY_CREATE_SECRET, //Desired access permissions.
                        & lsahPolicyHandle);//Receives the policy handle.            
            if( ntsResult != ERROR_SUCCESS )
                goto onCleanup;
            CArrayPointer<wchar_t> pKeyName;
            ntsResult = MakeKeyString(szKey, pKeyName);
            if( ntsResult != ERROR_SUCCESS )
                goto onCleanup;
            LSA_UNICODE_STRING lucKeyName;            
            lucKeyName.Buffer = pKeyName;
            lucKeyName.Length = wcslen(pKeyName) * sizeof(wchar_t);
            lucKeyName.MaximumLength = lucKeyName.Length;

            CArrayPointer<wchar_t> pValue;
            if(pData)
            {
                ntsResult = BinaryToUnicodeString(pData, nData, pValue);
                if( ntsResult != ERROR_SUCCESS )
                    goto onCleanup;
            };
            LSA_UNICODE_STRING lucPrivateData;
            if( !pValue || !pValue[0] )
            {
                lucPrivateData.Buffer = NULL;
                lucPrivateData.Length = 0;
                lucPrivateData.MaximumLength = 0;
            }
            else
            {
                lucPrivateData.Buffer = pValue;
                lucPrivateData.Length = wcslen(pValue) * sizeof(wchar_t);
                lucPrivateData.MaximumLength = lucPrivateData.Length;
            };

            // Store the private data.
            ntsResult =
                ::LsaStorePrivateData(
                    lsahPolicyHandle,   // handle to a Policy object
                    & lucKeyName,    // key to identify the data
                    !pData ? NULL : &lucPrivateData // the private data
                );
            if( ntsResult != ERROR_SUCCESS )
                goto onCleanup;

        };
    onCleanup:
        _ASSERTE(ntsResult == 0);
        if(lsahPolicyHandle)
            ::LsaClose( lsahPolicyHandle );
        return ntsResult;
    };

    int GetData(const char* szKey, void*& pData, size_t& nData)
    {
        NTSTATUS ntsResult = ERROR_SUCCESS;
        LSA_HANDLE lsahPolicyHandle = NULL;
        LSA_UNICODE_STRING* plucPrivateData = NULL;
        {
            LSA_OBJECT_ATTRIBUTES lsaObjectAttributes;
            ZeroMemory( &lsaObjectAttributes, sizeof( lsaObjectAttributes ) );        
            ntsResult =  ::LsaOpenPolicy(
                        NULL, //Name of the target system (NULL = local).
                        & lsaObjectAttributes, // Reserved parameter.
                        POLICY_GET_PRIVATE_INFORMATION, //Desired access permissions.
                        & lsahPolicyHandle);//Receives the policy handle.            
            if( ntsResult != ERROR_SUCCESS )
                goto onCleanup;
            CArrayPointer<wchar_t> pKeyName;
            ntsResult = MakeKeyString(szKey, pKeyName);
            if( ntsResult != ERROR_SUCCESS )
                goto onCleanup;
            LSA_UNICODE_STRING lucKeyName;            
            lucKeyName.Buffer = pKeyName;
            lucKeyName.Length = wcslen(pKeyName) * sizeof(wchar_t);
            lucKeyName.MaximumLength = lucKeyName.Length;            

            // Store the private data.
            ntsResult =
                ::LsaRetrievePrivateData(
                    lsahPolicyHandle,   // handle to a Policy object
                    & lucKeyName,    // key to identify the data
                    & plucPrivateData // the private data
                );
            //one user faced with the problem when Windows returned ERROR_SUCCESS and plucPrivateData was NULL
            if(!plucPrivateData)
                ntsResult = E_FAIL;
            ;
            if( ntsResult != ERROR_SUCCESS )
                goto onCleanup;

            const size_t nValue = plucPrivateData->Length/sizeof(wchar_t) + 1;
            CArrayPointer<wchar_t> pValue;
            pValue.Allocate(nValue);
            memset(pValue, 0, nValue*sizeof(wchar_t));
            memcpy(pValue, plucPrivateData->Buffer, plucPrivateData->Length);
            ntsResult = UnicodeStringToBinary(pValue, pData, nData);
            if( ntsResult != ERROR_SUCCESS )
                goto onCleanup;
        };
    onCleanup:
        if(lsahPolicyHandle)
            ::LsaClose( lsahPolicyHandle );
        if(plucPrivateData)
            ::LsaFreeMemory(plucPrivateData);
        return ntsResult;
    };
};
#endif
/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parstrdata.cpp
 * \author	Andrew Kazachkov
 * \date	10.09.2004 10:01:18
 * \brief	
 * 
 */

#include <std/base/klstd.h>
#include <std/par/parstrdata.h>

#include "std/memory/klmem.h"

#ifdef _WIN32
    #pragma intrinsic( memcmp, memcpy)
#endif

namespace KLPAR
{
    string_t::string_t()
        :   m_pData(NULL)
        ,   m_pStaticData(NULL)
    {;};

    string_t::string_t(const wchar_t* pData)
        :   m_pData((pData && pData[0]) ? AllocateString(pData) : NULL)
        ,   m_pStaticData(NULL)
    {;};

    string_t::string_t(const strdata_stat_t* pData)
        :   m_pData(NULL)
        ,   m_pStaticData(pData)
    {
        KLSTD_ASSERT(m_pStaticData != NULL);
    };

    string_t::string_t(const string_t& data)
        :   m_pData(NULL)
        ,   m_pStaticData(NULL)
    {
        assign(data);
    };

    void string_t::assign(const string_t& data)
    {
        clear();
        if(data.m_pData)
        {
            m_pData = data.m_pData;
            KLSTD_InterlockedIncrement(&m_pData->m_lCntRef);
        }
        else
        {
            m_pData = AllocateString(
                                data.m_pStaticData
                                    ?   data.m_pStaticData->m_szwString
                                    :   L"",
                                data.m_pStaticData
                                    ?   data.m_pStaticData->m_nLength
                                    :   0);
        };
    };
    
    wchar_t* string_t::LockBuffer(size_t nCharsWithoutNull)
    {
        clear();
        //ReleaseString(m_pData);
        m_pData = AllocateString(NULL, nCharsWithoutNull);
        //m_pStaticData = NULL;
        return &m_pData->m_data[0];
    };

    void string_t::UnlockBuffer()
    {
        m_pData->m_data[m_pData->m_lLength] = 0;
    };

    void string_t::clear()
    {
        ReleaseString(m_pData);
        m_pData = NULL;
        m_pStaticData = NULL;
    };

    string_t::~string_t()
    {
        ReleaseString(m_pData);
        //m_pData = NULL;
        //m_pStaticData = NULL;
    };

    string_t::string_data_t* string_t::AllocateString(const wchar_t* pData, size_t nSize)
    {
        const wchar_t* pString = pData ? pData : L"";
        const size_t nLength = (nSize == -1) ? my_wcslen(pString) : nSize;
        string_data_t* pResult = (string_data_t*)KLSTD_AllocTmp(
                                                    sizeof(_string_data_t) + 
                                                    sizeof(wchar_t)*(nLength + 1));
        if(!pResult)
        {
            KLSTD_THROW(KLSTD::STDE_NOMEMORY);
        };
        pResult->m_lLength = nLength;
        pResult->m_lCntRef = 1;
        //wcsncpy(&pResult->m_data[0], pString, nLength);
        if(nLength)
            memcpy(&pResult->m_data[0], pString, sizeof(wchar_t)*nLength);
        pResult->m_data[nLength] = 0;
        ;
        return pResult;
    };

    void string_t::ReleaseString(string_t::string_data_t* pData)
    {
        KLSTD_ASSERT(!pData || pData->m_lCntRef > 0);
        if(pData && KLSTD_InterlockedDecrement(&pData->m_lCntRef) == 0)
            KLSTD_FreeTmp(pData);
    }

    /*
    int KLSTD_FASTCALL string_t::compare(const string_t& data) const
    {
        return wcscmp(get(), data.get());
    };
    */
};

#ifdef _WIN32

KLCSC_DECL __declspec(naked)
int __fastcall klstd_wcscmp (
        const wchar_t * src,
        const wchar_t * dst)
{
    __asm
    {
        mov         eax,ecx 
    label_401012:
        mov         cx,word ptr [eax] 
        cmp         cx,word ptr [edx] 
        jne         label_401037 
        test        cx,cx 
        je          label_401034
        mov         cx,word ptr [eax+2] 
        cmp         cx,word ptr [edx+2] 
        jne         label_401037
        add         eax,4 
        add         edx,4 
        test        cx,cx 
        jne         label_401012
    label_401034:
        xor         eax,eax 
        ret
    label_401037:
        sbb         eax,eax 
        sbb         eax,0FFFFFFFFh 
        ret
    };
};

KLCSC_DECL __declspec(naked)
size_t __fastcall my_wcslen (
        const wchar_t * wcs)
{
    __asm
    {
        lea         edx,[ecx+2] 
    lable_401053:
        mov         ax,word ptr [ecx] 
        add         ecx,2 
        test        ax,ax 
        jne         lable_401053 
        sub         ecx,edx 
        sar         ecx,1 
        mov         eax,ecx 
        ret
    };
};

#else

KLCSC_DECL int KLSTD_FASTCALL klstd_wcscmp (
        const wchar_t * src,
        const wchar_t * dst)
{
    return wcscmp(src, dst);
};

KLCSC_DECL size_t KLSTD_FASTCALL my_wcslen (
        const wchar_t * wcs)
{
    return wcslen(wcs);
};

#endif
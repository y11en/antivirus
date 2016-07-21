#include <stddef.h>
#include <std/base/klstd.h>
#include <std/err/klerrors.h>
#include <std/base/klstdutils.h>
#include <std/memory/klmem.h>

namespace KLSTD
{
    const size_t c_nStrSign = 0x0F4B78C3;

    struct wstr_data_t
    {
        size_t  sign;
        size_t  len;
        wchar_t data[1];
    };

    AKWSTR AllocWSTR(const wchar_t* szw, size_t n)
    {
        size_t nRealSize =  offsetof(wstr_data_t, data) + (n + 1)*sizeof(wchar_t);
        wstr_data_t* p = (wstr_data_t*)KLSTD_AllocTmp(nRealSize);
        KLSTD_CHKMEM(p);
        p->sign = c_nStrSign;
        p->len = n;
        if(szw)
            wcsncpy(p->data, szw, n);
        p->data[n] = 0;
        return &p->data[0];
    };

    wstr_data_t* Ptr2Data(KLSTD::AKWSTR wstr, bool bThrow = true)
    {
        wstr_data_t* pResult = NULL;
        if(wstr)
        {
#ifdef _WIN32
            __try
            {
#endif
                pResult = (wstr_data_t*)(void*)(((char*)wstr) - offsetof(wstr_data_t, data));
                if(c_nStrSign != pResult->sign || int(pResult->len) < 0 )
                    pResult = NULL;
#ifdef _WIN32
            }
            __except(1)
            {
                __asm int 3;
                pResult = NULL;
            };
#endif
        };
        if(wstr && !pResult && bThrow)
        {
            KLSTD_THROW(KLSTD::STDE_BADFORMAT);
        };
        return pResult;
    };

    KLCSC_DECL void ConvertStrArr(
                    std::vector<std::wstring>&  vecNames, 
                    KLSTD::AKWSTRARR&           arr)
    {
        KLSTD::klwstrarr_t res(vecNames.size());
        for(size_t i = 0; i < vecNames.size(); ++i)
        {
            KLSTD::klwstr_t x(vecNames[i].c_str());
            res.setat(i, x.detach());
        };
        arr = res.detach();
    };
};

KLSTD::AKWSTR KLSTD_AllocWSTR(const wchar_t* szw)
{
    return KLSTD::AllocWSTR(szw, wcslen(szw));
};

KLSTD::AKWSTR KLSTD_AllocWSTRLen(const wchar_t* szw, size_t nLen)
{
    return KLSTD::AllocWSTR(szw, nLen);
};

size_t KLSTD_GetWSTRLen(const KLSTD::AKWSTR wstr)
{
    KLSTD::wstr_data_t* pData = KLSTD::Ptr2Data(wstr);    
    return pData?pData->len:0;
};

void KLSTD_FreeWSTR(KLSTD::AKWSTR wstr)
{
    KLSTD::wstr_data_t* pData = KLSTD::Ptr2Data(wstr, false);
    if(pData)
    {
        pData->sign = 0;
        KLSTD_FreeTmp(pData);
    };
};

int KLSTD_CompareWSTR(const KLSTD::AKWSTR wstr1, const KLSTD::AKWSTR wstr2)
{
    KLSTD::wstr_data_t* pData1 = KLSTD::Ptr2Data(wstr1);
    KLSTD::wstr_data_t* pData2 = KLSTD::Ptr2Data(wstr2);
    return wcscmp(
                pData1
                    ?   pData1->data
                    :   L"", 
                pData2
                    ?   pData2->data
                    :   L"");
};

KLCSC_DECL void KLSTD_AllocArrayWSTR(size_t size, KLSTD::AKWSTRARR& arr)
{
    KLSTD_CHK(arr.m_pwstr, NULL == arr.m_pwstr);
    KLSTD_CHK(arr.m_nwstr, 0 == arr.m_nwstr);
    const size_t nByteLen = (size + 1) * sizeof(KLSTD::AKWSTR);
    arr.m_pwstr = (KLSTD::AKWSTR*)KLSTD_AllocTmp(nByteLen);
    KLSTD_CHKMEM(arr.m_pwstr);
    arr.m_nwstr = size;
    memset(arr.m_pwstr, 0, nByteLen);
};

KLCSC_DECL void KLSTD_FreeArrayWSTR(KLSTD::AKWSTRARR& arr)
{
    if(arr.m_pwstr)
    {
        for(size_t i = 0; i < arr.m_nwstr; ++i)
        {
            if(arr.m_pwstr[i])
            {
                KLSTD_FreeWSTR(arr.m_pwstr[i]);
                arr.m_pwstr[i] = NULL;
            };
        };
        KLSTD_FreeTmp(arr.m_pwstr);
        arr.m_pwstr = NULL;
    };
    if(arr.m_nwstr)
        arr.m_nwstr = 0;
};


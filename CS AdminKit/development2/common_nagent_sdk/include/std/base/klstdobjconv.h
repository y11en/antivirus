#ifndef __KLSTD_OBJCONV_H__
#define __KLSTD_OBJCONV_H__

#include <std/base/klbase.h>
#include <std/base/klstdutils.h>
#include <string>
#include <vector>

namespace KLSTD
{
    inline void ConvertArray2Vector(
            const KLSTD::klwstrarr_t&       arrData,
            std::vector< std::wstring >&    vecData)
    {            
        const size_t size = arrData.size();
        vecData.clear();
        vecData.reserve(size);
        for(size_t i = 0; i < size; ++i)
            vecData.push_back(arrData[i]?arrData[i]:L"");
    };

    inline void ConvertVector2Array(
            const std::vector< std::wstring >&  vecData,
            KLSTD::klwstrarr_t&                 arrData)
    {
        KLSTD::klwstrarr_t arrResult;
        const size_t size = vecData.size();
        arrResult.create(size);
        for(size_t i = 0; i < size; ++i)
        {
            klwstr_t str;
            str.assign(vecData[i].c_str());
            arrResult.setat(i, str.detach());
        };
        arrData.attach(arrResult.detach());
    };
};

#endif //__KLSTD_OBJCONV_H__

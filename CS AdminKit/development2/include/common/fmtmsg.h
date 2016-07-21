#ifndef __KL_FMTMSG_H__
#define __KL_FMTMSG_H__

#include <cstdlib>
#include <string>
#include <climits>

namespace KLLOC
{
    //! deprecated, use KLSTD::c_nStdAutoArgs
    const size_t c_nAutoArgs = UINT_MAX;

    //! deprecated, use KLSTD_FormatMessage instead
    bool DoFormatMessage(
                    const wchar_t*  szwTemplate,
                    const wchar_t** pszwArgs,
                    size_t          nArgs,
                    std::wstring&   wstrResult);
};

#endif //__KL_FMTMSG_H__

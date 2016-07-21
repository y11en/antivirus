/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	error2string.h
 * \author	Andrew Kazachkov
 * \date	04.06.2004 16:11:06
 * \brief	
 * 
 */

#ifndef __KL_ERROR2STRING_H__
#define __KL_ERROR2STRING_H__

#include <ostream>
#include "std/err/error.h"

namespace KLERR
{
    class WriteError
    {
    public:
        WriteError(KLERR::Error* pError, bool bNoDevInfo = false)
            :   m_pError(pError)
            ,   m_bNoDevInfo(bNoDevInfo)
        {;};
    public:
        KLSTD::CAutoPtr<KLERR::Error>   m_pError;
        const bool                      m_bNoDevInfo;
    };

    std::wostream& operator << (std::wostream& os, const WriteError& addr);

    KLSTD_NOTHROW void ErrReportFailure(
                        KLERR::Error*       pError,
                        bool                bIsError,
                        const wchar_t*      szwLocModule,
                        unsigned            idLoc,
                        const wchar_t*      szwArg1 = L"",
                        const wchar_t*      szwArg2 = L"",
                        const wchar_t*      szwArg3 = L"",
                        const wchar_t*      szwArg4 = L"",
                        const wchar_t*      szwArg5 = L"",
                        const wchar_t*      szwArg6 = L"",
                        const wchar_t*      szwArg7 = L"",
                        const wchar_t*      szwArg8 = L"",
                        const wchar_t*      szwArg9 = L"") throw();
    
    KLSTD_NOTHROW void ErrReportFailure2(
                        const wchar_t*      szwInternalData,
                        KLERR::Error*       pError,
                        bool                bIsError,
                        const wchar_t*      szwLocModule,
                        unsigned            idLoc,
                        const wchar_t*      szwArg1 = L"",
                        const wchar_t*      szwArg2 = L"",
                        const wchar_t*      szwArg3 = L"",
                        const wchar_t*      szwArg4 = L"",
                        const wchar_t*      szwArg5 = L"",
                        const wchar_t*      szwArg6 = L"",
                        const wchar_t*      szwArg7 = L"",
                        const wchar_t*      szwArg8 = L"",
                        const wchar_t*      szwArg9 = L"") throw();
};
#endif //__KL_ERROR2STRING_H__

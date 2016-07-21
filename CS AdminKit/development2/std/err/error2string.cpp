/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	error2string.cpp
 * \author	Andrew Kazachkov
 * \date	04.06.2004 16:12:35
 * \brief	
 * 
 */

#include "std/err/error.h"
#include "std/fmt/stdfmtstr.h"
#include "std/err/error_localized.h"
#include "std/err/errlocids.h"
#include "std/err/errloc_intervals.h"
#include "std/conv/klconv.h"
#include "std/err/error2string.h"
#include "std/base/klstdutils.h"
#include "common/reporter.h"
#include "common/reporter2.h"

#include <sstream>

using namespace std;

#ifdef KLERR_NO_LOCALISATION
namespace
{
    inline std::wstring Int2Str(unsigned x, bool bDec = true)
    {
        wchar_t szBuffer[64];
        _ultow(x, szBuffer, bDec?10:16);
        return szBuffer;
    };

    inline std::wstring Int2Str(int x, bool bDec = true)
    {
        wchar_t szBuffer[64];
        _ltow(x, szBuffer, bDec?10:16);
        return szBuffer;
    };
};
#endif

namespace KLERR
{
    std::wostream& operator << (std::wostream& os, const WriteError& addr)
    {
        KLERR::Error* pError = const_cast<KLERR::Error*>((const KLERR::Error*)addr.m_pError);
        if(pError)
        {
        #ifndef KLERR_NO_LOCALISATION
            
            os << L"#" << pError->GetId() << L" (" << pError->GetErrorSubcode() << L") ";

            KLSTD::klwstr_t wstrLocString;
            if( KLERR_IsErrorLocalized(pError) &&
                KLERR_LocFormatErrorString(pError, wstrLocString.outref()) )
            {
                os << (const wchar_t*)wstrLocString;
            }
            else if(pError->GetMsg() && pError->GetMsg()[0])
                os << pError->GetMsg();
            ;
            if(!addr.m_bNoDevInfo)
            {
                os << KLSTD_RPT_DELIMITER;
                ;
                KLSTD::klwstr_t wstrFormat;
                KLERR_FindLocString(
                                KLERR_LOCMOD_STD, 
                                KLSTD::STDEL_ERROR_INFO,
                                wstrFormat.outref(), 
                                L"Error information: %1/%2 (%3), %4, %5");
                wchar_t szwCode[16], szwSubCode[16], szwLine[16];
                _ltow(pError->GetId(), szwCode, 10);
                _ltow(pError->GetErrorSubcode(), szwSubCode, 10);
                _ltow(pError->GetLine(), szwLine, 10);
                KLSTD_A2CW2 wstrFile(pError->GetFileName());
                const wchar_t* pArgs[] = 
                {
                    szwCode,
                    szwSubCode,
                    pError->GetMsg(),
                    (const wchar_t*)wstrFile,
                    szwLine
                };
                KLSTD::klwstr_t wstrMessage;
                KLSTD_FormatMessage(
                            wstrFormat.c_str(),
                            pArgs,
                            KLSTD_COUNTOF(pArgs),
                            wstrMessage.outref());

                os << (const wchar_t*)wstrMessage << endl;
            };
        #else
            os << 
                L"\tCode=" << Int2Str(pError->GetId()) << endl <<
                L"\tSubcode=" << Int2Str(pError->GetErrorSubcode()) << endl <<
                L"\tMessage='" << pError->GetMsg() << L"'" << endl;
            if(!addr.m_bNoDevInfo)
            {
                os << KLSTD_RPT_DELIMITER << 
                L"\tFile='" << (const wchar_t*)KLSTD_A2CW2(pError->GetFileName()) << L"'" << endl <<
                L"\tLine=" << Int2Str(pError->GetLine());
            };
        #endif
        };
        return os;
    };
    
#ifndef KLERR_NO_LOCALISATION

    KLSTD_NOTHROW void ErrReportFailure(
                        KLERR::Error*       pError,
                        bool                bIsError,
                        const wchar_t*      szwLocModule,
                        unsigned            idLoc,
                        const wchar_t*      szwArg1,
                        const wchar_t*      szwArg2,
                        const wchar_t*      szwArg3,
                        const wchar_t*      szwArg4,
                        const wchar_t*      szwArg5,
                        const wchar_t*      szwArg6,
                        const wchar_t*      szwArg7,
                        const wchar_t*      szwArg8,
                        const wchar_t*      szwArg9) throw()
    {
        ErrReportFailure2(
                    L"", 
                    pError,
                    bIsError,
                    szwLocModule,
                    idLoc,
                    szwArg1,
                    szwArg2,
                    szwArg3,
                    szwArg4,
                    szwArg5,
                    szwArg6,
                    szwArg7,
                    szwArg8,
                    szwArg9);
    };

    KLSTD_NOTHROW void ErrReportFailure2(
                        const wchar_t*      szwInternalData,
                        KLERR::Error*       pError,
                        bool                bIsError,
                        const wchar_t*      szwLocModule,
                        unsigned            idLoc,
                        const wchar_t*      szwArg1,
                        const wchar_t*      szwArg2,
                        const wchar_t*      szwArg3,
                        const wchar_t*      szwArg4,
                        const wchar_t*      szwArg5,
                        const wchar_t*      szwArg6,
                        const wchar_t*      szwArg7,
                        const wchar_t*      szwArg8,
                        const wchar_t*      szwArg9) throw()
    {
        KLSTD::klwstr_t wstrFormat;
        KLERR_FindLocString(
                        szwLocModule, 
                        idLoc,
                        wstrFormat.outref(), 
                        L"");

        const wchar_t* pArgs[] = 
        {
            szwArg1,
            szwArg2,
            szwArg3,
            szwArg4,
            szwArg5,
            szwArg6,
            szwArg7,
            szwArg8,
            szwArg9
        };

        KLSTD::klwstr_t wstrMessage;
        KLSTD_FormatMessage(
                        wstrFormat.c_str(), 
                        pArgs, 
                        KLSTD_COUNTOF(pArgs), 
                        wstrMessage.outref());

        std::wostringstream os;
        os << (const wchar_t*)wstrMessage << std::endl << 
            WriteError(pError, false) << std::endl << KLSTD_RPT_DELIMITER << L"Extra data: " << szwInternalData;
        
        if(bIsError)
            KLSTD_ReportError(os.str().c_str());
        else
            KLSTD_ReportWarning(os.str().c_str());
    };
#endif
};

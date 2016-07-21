#include <std/base/klstd.h>
#include <std/thr/locks.h>
#include <common/reporter.h>

#define KLCS_MODULENAME L"KLSTD"

namespace KLSTD
{
    extern KLSTD::Reporter*     g_pReporter;
    extern KLSTD::ObjectLock*   g_plckReporter;
};

using namespace KLSTD;

KLCSC_DECL KLSTD_NOTHROW void KLSTD_SetGlobalReporter(
                                    KLSTD::Reporter* pReporter) throw()
{
    KLERR_BEGIN
        if(g_plckReporter)
        {
            g_plckReporter->Disallow();
            g_plckReporter->Wait();
            g_pReporter = pReporter;
            g_plckReporter->Allow();
        };
    KLERR_ENDT(1)
};


KLCSC_DECL KLSTD_NOTHROW bool KLSTD_ReportError(
                                    const wchar_t* szwMessage) throw()
{
    bool bResult = false;
    KLSTD::CAutoObjectLock aol(*g_plckReporter);
    if(aol && g_pReporter)
    {
        g_pReporter->ReportError(szwMessage);
        bResult = true;
    };
    return bResult;
};

KLCSC_DECL KLSTD_NOTHROW bool KLSTD_ReportInfo(
                                    const wchar_t* szwMessage) throw()
{
    bool bResult = false;
    KLSTD::CAutoObjectLock aol(*g_plckReporter);
    if(aol && g_pReporter)
    {
        g_pReporter->ReportInfo(szwMessage);
        bResult = true;
    };
    return bResult;
};

KLCSC_DECL KLSTD_NOTHROW bool KLSTD_ReportWarning(
                                    const wchar_t* szwMessage) throw()
{
    bool bResult = false;
    KLSTD::CAutoObjectLock aol(*g_plckReporter);
    if(aol && g_pReporter)
    {
        g_pReporter->ReportWarning(szwMessage);
        bResult = true;
    };
    return bResult;
};

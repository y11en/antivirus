/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	tracer.h
 * \author	Andrew Kazachkov
 * \date	21.11.2003 14:21:12
 * \brief	
 * 
 */

#ifndef __KL_TRACER_H__
#define __KL_TRACER_H__

#ifdef _WIN32
#include <tchar.h>
#endif
#include <string>
#include <std/trc/trace.h>

#ifdef _DEBUG
    #define KLSTD_DEFAULT_TRACE_FLAGS               \
                (   KLSTD::TF_PRINT_DATETIME|       \
                    KLSTD::TF_PRINT_MODULE|         \
                    KLSTD::TF_PRINT_THREAD_ID|      \
                    KLSTD::TF_ENABLE_DEBUGOUT|      \
                    KLSTD::TF_PRINT_MILLISECONDS    )
#else
    #define KLSTD_DEFAULT_TRACE_FLAGS               \
                (   KLSTD::TF_PRINT_DATETIME|       \
                    KLSTD::TF_PRINT_MODULE|         \
                    KLSTD::TF_PRINT_THREAD_ID|      \
                    KLSTD::TF_PRINT_MILLISECONDS    )
#endif

KLCSC_DECL void KLSTD_GetTraceInfoA(
                        int             argc,
                        char**          argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        std::wstring&   wstrTraceFile,
                        long&           nTracelevel);

KLCSC_DECL void KLSTD_GetTraceInfoW(
                        int             argc,
                        wchar_t**       argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        std::wstring&   wstrTraceFile,
                        long&           nTracelevel);

KLSTD_INLINEONLY void KLSTD_GetTraceInfo(
                        int             argc,
                        char**          argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        std::wstring&   wstrTraceFile,
                        long&           nTracelevel)
{
    KLSTD_GetTraceInfoA(
                    argc,
                    argv,
                    szwProduct,
                    szwVersion,
                    wstrTraceFile,
                    nTracelevel);
}

KLSTD_INLINEONLY void KLSTD_GetTraceInfo(
                        int             argc,
                        wchar_t**       argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        std::wstring&   wstrTraceFile,
                        long&           nTracelevel)
{
    KLSTD_GetTraceInfoW(
                    argc,
                    argv,
                    szwProduct,
                    szwVersion,
                    wstrTraceFile,
                    nTracelevel);
}

enum
{
    KLSTD_DBGEXCPT_PROCESSEXCP  =   1,
    KLSTD_DBGEXCPT_SHOWMSGBOXES =   2,
    KLSTD_DBGEXCPT_DODUMP   =   4
};

KLCSC_DECL void KLSTD_GetDiagOptionsA(
                        int             argc,
                        char**          argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        AVP_dword&      dwExcptFlags);

KLCSC_DECL void KLSTD_GetDiagOptionsW(
                        int             argc,
                        wchar_t**       argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        AVP_dword&      dwExcptFlags);

KLSTD_INLINEONLY void KLSTD_GetDiagOptions(
                        int             argc,
                        char**          argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        AVP_dword&      dwExcptFlags)
{
    KLSTD_GetDiagOptionsA(argc, argv, szwProduct, szwVersion, dwExcptFlags);
}

KLSTD_INLINEONLY void KLSTD_GetDiagOptions(
                        int             argc,
                        wchar_t**       argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        AVP_dword&      dwExcptFlags)
{
    KLSTD_GetDiagOptionsW(argc, argv, szwProduct, szwVersion, dwExcptFlags);
}

#endif //__KL_TRACER_H__

#include "std/trc/tracewatch.h"
#include "std/trc/tracer.h"
#include "std/conv/klconv.h"
#include "std/stress/st_prefix.h"
#include "common/trace_versions.h"

#define KLCS_MODULENAME L"TRACEWATCH"

namespace KLSTD
{
    CTraceWatch::CTraceWatch()
        :   m_hEvent(NULL)
        ,   m_nTraceLevel(0)
        ,   m_bTraceStarted(false)
        ,   m_argc(0)
        ,   m_argv(NULL)
        ,   m_dwFlags(0)
    {
        m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    };

    CTraceWatch::~CTraceWatch()
    {
        Destroy();
    };

    KLSTD_NOTHROW void CTraceWatch::Create(
                    const std::wstring& wstrProduct,
                    const std::wstring& wstrVersion,
                    int                 argc,
                    TCHAR**             argv,
                    AVP_dword           dwFlags) throw()
    {
    KLERR_TRY
        m_dwFlags = dwFlags;
        m_wstrProduct = wstrProduct;
        m_wstrVersion = wstrVersion;
        m_argc = argc;
        m_argv = argv;
        ;
        LONG lResult = m_key.Open(
                    HKEY_LOCAL_MACHINE,
                    KLSTD_StGetProductKey(
                        wstrProduct.c_str(), 
                        wstrVersion.c_str()).c_str(), 
                    KEY_READ);
        if(lResult)
        {
            KLSTD_THROW_LASTERROR_CODE2(lResult);
        };
        ;
        ResetNotifier();
    KLERR_CATCH(pError)
        KLSTD_ASSERT(!"KLSTD::CTraceWatch::Create failed");
    KLERR_ENDTRY
        KLERR_IGNORE(UpdateTracing(true));    
    };

    KLSTD_NOTHROW void CTraceWatch::Destroy() throw()
    {
    KLERR_TRY
        if(m_hEvent)
        {
            CloseHandle(m_hEvent);
            m_hEvent = NULL;
        };
        if(m_bTraceStarted)
        {
            KLERR_IGNORE(KLSTD::StopTrace());
            m_bTraceStarted = false;
        };
    KLERR_CATCH(pError)
        KLSTD_ASSERT(!"KLSTD::CTraceWatch::Destroy failed");
    KLERR_ENDTRY
    };

    void CTraceWatch::ResetNotifier()
    {
        if(!ResetEvent(m_hEvent))
        {
            KLSTD_THROW_LASTERROR2();
        };
        long lResult = RegNotifyChangeKeyValue(
                                m_key, 
                                TRUE,
                                REG_NOTIFY_CHANGE_NAME|
                                    REG_NOTIFY_CHANGE_LAST_SET,
                                m_hEvent,
                                TRUE);
        if(lResult)
        {
            KLSTD_THROW_LASTERROR_CODE2(lResult);
        };
    };

    KLSTD_NOTHROW void CTraceWatch::Check()throw()
    {
        if(!m_hEvent || !(HKEY)m_key)
            return;
        KLERR_TRY
            if(WaitForSingleObject(m_hEvent, 0) == WAIT_OBJECT_0)
            {
                KLSTD_TRACE0(3, L"Registry key changed!!!\n");
                ResetNotifier();
                UpdateTracing();
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
            KLSTD_ASSERT(!"CTraceWatch::Check");
        KLERR_ENDTRY
    };
    
    static void TraceOS()
    {
        OSVERSIONINFO osv;
        KLSTD_ZEROSTRUCT(osv);
        osv.dwOSVersionInfoSize  = sizeof(osv);
        if(GetVersionEx(&osv))
        {
            KLSTD_TRACE4(1, (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
                                ?   L"Operating system: "
                                    L"Microsoft Windows NT %u.%u, build %u, %ls\n"
                                :   L"Operating system: "
                                    L"Microsoft Windows 9x %u.%u, build %u, %ls\n",
                            osv.dwMajorVersion,
                            osv.dwMinorVersion,
                            osv.dwBuildNumber,
                            (const wchar_t*)KLSTD_T2CW2(osv.szCSDVersion));
        };
    };

    void CTraceWatch::UpdateTracing(bool bFirstTime)
    {
        std::wstring    wstrTraceFile;
        long            nTraceLevel = 0;
        KLSTD_GetTraceInfo(
                            m_argc,
                            m_argv,
                            m_wstrProduct.c_str(),
                            m_wstrVersion.c_str(), 
                            wstrTraceFile,
                            nTraceLevel);
        if(wstrTraceFile.empty())
            nTraceLevel = 0;
        ;
        bool bDoRestartTraces = false;
        if(bFirstTime)
            bDoRestartTraces = true;
        else
            bDoRestartTraces = (    wstrTraceFile != m_wstrTraceFile ||
                                    nTraceLevel != m_nTraceLevel);
        ;
        if(bDoRestartTraces)
        {
            if(m_bTraceStarted)
            {
                KLERR_IGNORE(KLSTD::StopTrace());
                m_bTraceStarted = false;
            };
            if(nTraceLevel != 0)
            {
                KLERR_IGNORE(KLSTD::StartTrace( 
                                    nTraceLevel, 
                                    KLSTD_W2CA2(wstrTraceFile.c_str()), 
                                    m_dwFlags));
                m_bTraceStarted = true;
                KLSTD::Trace(   1, 
                                KLCS_MODULENAME,
                                L"\n\n"
                                L"\tTraceFile: '%ls'\n"
                                L"\tTraceLevel = %u,"
                                L"\tTraceFlags = 0x%X\n\n", 
                                wstrTraceFile.c_str(),
                                nTraceLevel,
                                m_dwFlags);
                TraceOS();
                TraceVersions(1);
            };
            m_nTraceLevel = nTraceLevel;
            m_wstrTraceFile = wstrTraceFile;
        };
    };
};

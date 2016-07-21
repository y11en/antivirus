#include "./NTService.h"
#include <process.h>

class CNTServiceThreadReporter
{
public:
    CNTServiceThreadReporter(CNTService* pX, DWORD dwMaxTmt = INFINITE, bool bNeedStop = true)
        :   m_bStopFlag(false)
        ,   m_hThread(NULL)
        ,   m_pX(pX)
        ,   m_dwMaxTmt(dwMaxTmt)
        ,   m_bNeedStop(bNeedStop)
    {        
        ATLTRACE(_T("\nCNTServiceThreadReporter\n"));
        KLSTD_TRACE1(1, L"\nCNTServiceThreadReporter(dwMaxTmt=%d)\n", dwMaxTmt);
        unsigned foo = 0;
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, thread_proc, this, 0, &foo);
        if(!m_hThread)
        {
            KLSTD_TRACE0(1, L"Failed to create helper thread !!!\n");
            m_pX->ReportStatus(SERVICE_STOP_PENDING, 60000);
        };
    };
    ~CNTServiceThreadReporter()
    {
        ATLTRACE(_T("~CNTServiceThreadReporter\n\n"));
        KLSTD_TRACE0(1, L"~CNTServiceThreadReporter\n\n");
        m_bStopFlag = true;
        if(m_hThread)
            WaitForSingleObject(m_hThread, INFINITE);
    };
protected:    
    static unsigned __stdcall thread_proc( void * p)
    {
        CNTServiceThreadReporter* pThis = (CNTServiceThreadReporter*)p;
        const DWORD dwTickCount0 = GetTickCount();
        for(size_t i = 0; !pThis->m_bStopFlag; ++i)
        {
            if( pThis->m_dwMaxTmt != INFINITE &&  
                GetTickCount() >= dwTickCount0 + pThis->m_dwMaxTmt)
            {
                if(pThis->m_bNeedStop)
                {
                    KLSTD_TRACE0(1, L"CNTServiceThreadReporter: ReportStatus(SERVICE_STOPPED)\n");
                    pThis->m_pX->ReportStatus(SERVICE_STOPPED);
                };
                TerminateProcess(GetCurrentProcess(), -3);
            }
            else
            {
                Sleep(1000);
                if(pThis->m_bNeedStop && i % 3 == 0)
                {
                    KLSTD_TRACE0(1, L"CNTServiceThreadReporter: ReportStatus(SERVICE_STOP_PENDING, 5000)\n");
                    pThis->m_pX->ReportStatus(SERVICE_STOP_PENDING, 5000);
                };
            };
        };
        return 0;
    };
    CNTService*     m_pX;
    HANDLE          m_hThread;
    const DWORD     m_dwMaxTmt;
    volatile bool   m_bStopFlag;
    const bool      m_bNeedStop;
};

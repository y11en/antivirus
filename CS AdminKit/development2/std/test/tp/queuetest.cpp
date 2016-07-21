/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	queuetest.cpp
 * \author	Andrew Kazachkov
 * \date	13.04.2005 13:17:58
 * \brief	
 * 
 */

#include "queuetest.h"
#include "testmodule.h"

#define KLCS_MODULENAME L"QUEUETEST"

void CQueueTest::Initialize(KLTST2::TestData2* pTestData)
{
	::KLSTD_Initialize();
	::KLTP_Initialize();
}

KLSTD_NOTHROW void CQueueTest::Deinitialize() throw()
{
	::KLTP_Deinitialize();
	::KLSTD_Deinitialize();
}

long CQueueTest::Run()
{
	bool bResult=true;
	KLERR_TRY
        Do();
	KLERR_CATCH(pError)
		bResult=false;
	KLERR_ENDTRY
	return bResult?0:(-1);
}

void CQueueTest::OnCmd(int)
{
    KLSTD_InterlockedIncrement(&m_lReceived);
};

unsigned long KLSTD_THREADDECL CQueueTest::ThreadFunction(void *arpg)
{
    CQueueTest* pThis = (CQueueTest*)arpg;
    for(size_t i = 0; ; ++i)
    {
        if(!pThis->m_pQueue->Send(1))
            break;
        KLSTD_InterlockedIncrement(&pThis->m_lSent);
        if(i && (i % 23))
            KLSTD_Sleep(10);
    };        
    return 0;
};

unsigned long KLSTD_THREADDECL CQueueTest::RunnerFunction(void *arpg)
{
    KLCMDQUEUE::CmdQueue<int, int, CQueueTest>* pQueue =  
        (KLCMDQUEUE::CmdQueue<int, int, CQueueTest>*)arpg;
    pQueue->Run();
    return 0;
};

void CQueueTest::Do()
{
    for(size_t i = 0; i < 2; ++i)
    {
        m_lSent = m_lReceived = 0;
        KLSTD::CPointer<KLSTD::Thread> pTh1, pTh2, pTh3, pTh4;
        KLSTD_CreateThread(&pTh1);
        KLSTD_CreateThread(&pTh2);
        KLSTD_CreateThread(&pTh3);
        KLSTD_CreateThread(&pTh4);
        KLSTD::CPointer<KLSTD::Thread> pRunner;
        KLERR_TRY
            m_pQueue = NULL;
            m_pQueue =  
                    new KLCMDQUEUE::CmdQueue<int, int, CQueueTest>(NULL, (i & 1));
            m_pQueue->Create(this, &CQueueTest::OnCmd);
            pTh1->Start(L"1", ThreadFunction, this);
            pTh2->Start(L"2", ThreadFunction, this);
            pTh3->Start(L"3", ThreadFunction, this);
            pTh4->Start(L"4", ThreadFunction, this);
            if(i & 1)
            {
                KLSTD_CreateThread(&pRunner);
                pRunner->Start(L"runner", RunnerFunction, m_pQueue);
            };
            KLSTD_Sleep(10000);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            m_pQueue->Destroy();            
            if(pTh1)
                pTh1->Join();
            if(pTh2)
                pTh2->Join();
            if(pTh3)
                pTh3->Join();
            if(pTh4)
                pTh4->Join();
            if(pRunner)
                pRunner->Join();
        KLERR_ENDTRY
    };
};

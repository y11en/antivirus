/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file tptest.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Тест библиотеки TP.
 *
 */

#include "tptest.h"

#include "std/trc/trace.h"
#include "std/tp/threadspool.h"

#include "testmodule.h"

#define KLCS_MODULENAME L"TPTEST"

class ThreadsPoolTestWorker : public KLSTD::KLBaseImpl<KLTP::ThreadsPool::Worker>
{
public:
	ThreadsPoolTestWorker( bool createSem = false, bool runOnce = false )
	{
		if ( createSem )
		{
			KLSTD_CreateSemaphore( &m_pSem, 0 );
		}
		m_runOnce = runOnce;
		m_counter = 0;
	}

	virtual int RunWorker( KLTP::ThreadsPool::WorkerId wId )
	{
		if ( m_pSem ) 
		{
			if ( !m_runOnce ) 
			{
				m_counter++; 
				if ( m_counter==2 ) m_pSem->Wait();
			}
			else
				m_pSem->Wait();
		}
		return 0;
	}

	int									m_counter;
	KLSTD::CAutoPtr<KLSTD::Semaphore>	m_pSem;
	bool								m_runOnce;

};

void CThreadPoolTest::Initialize(KLTST2::TestData2* pTestData)
{
	::KLSTD_Initialize();
	::KLTP_Initialize();
}

KLSTD_NOTHROW void CThreadPoolTest::Deinitialize() throw()
{ 
	::KLTP_Deinitialize();
	::KLSTD_Deinitialize();
}
	
//!\brief Запускает тест и возвращает результаты выполнения теста.
long CThreadPoolTest::Run( )
{
	KLTP::ThreadsPool::WorkerId wId;
	KLSTD::CAutoPtr<ThreadsPoolTestWorker> pWorker;
	pWorker.Attach( new ThreadsPoolTestWorker() );

	KLTP_GetThreadsPool()->AddWorker( &wId, L"ThreadsPoolTestWorkerName", pWorker );
	KLTP_GetThreadsPool()->DeleteWorker( wId );

	pWorker = 0;
	pWorker.Attach( new ThreadsPoolTestWorker( true, false ) );

	KLTP_GetThreadsPool()->AddWorker( &wId, L"ThreadsPoolTestWorkerName", pWorker, KLTP::ThreadsPool::RunNormal,
		true, 1000 );

	KLTP_GetThreadsPool()->SuspendWorker( wId );

	pWorker->m_pSem->Post();

	KLTP_GetThreadsPool()->ResumeWorker( wId );

	KLTP_GetThreadsPool()->DeleteWorker( wId );

	pWorker = 0;
	pWorker.Attach( new ThreadsPoolTestWorker( true, true ) );

	KLTP_GetThreadsPool()->AddWorker( &wId, L"ThreadsPoolTestWorkerName", pWorker, 
		KLTP::ThreadsPool::RunOnce );
	pWorker->m_pSem->Post();

	KLTP_GetThreadsPool()->DeleteWorker( wId );

	return 0;
}

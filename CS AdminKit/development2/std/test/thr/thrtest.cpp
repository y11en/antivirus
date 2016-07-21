/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file thrtest.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Тест библиотеки THR.
 *
 */

#include "thrtest.h"

#include "std/thr/thread.h"
#include "std/trc/trace.h"
#include "std/time/kltime.h"
#include "std/thrstore/threadstore.h"

#include "testmodule.h"

#define KLCS_MODULENAME L"THRTEST"

unsigned long KLSTD_THREADDECL ThreadTestThreadFunc( void *arpg )
{
	long *counter = (long *)arpg;	

	KLSTD_TRACE0(1, L"ThreadTestThreadFunc: change current thread priority (low)\n");
	KLSTD_SetCurrentThreadPriority( KLSTD::Thread::PriorityLow );
		KLSTD_TRACE0(1, L"ThreadTestThreadFunc: change current thread priority (high)\n");
	KLSTD_SetCurrentThreadPriority( KLSTD::Thread::PriorityHigh );
	KLSTD_TRACE0(1, L"ThreadTestThreadFunc: change current thread priority (normal)\n");
	KLSTD_SetCurrentThreadPriority( KLSTD::Thread::PriorityNormal );

	unsigned long sysCount = KLSTD::GetSysTickCount();
	if ( sysCount==0 ) 
	{
		KLSTD_TRACE0(1, L"KLSTD::GetSysTickCount error ( in ThreadTestThreadFunc ): return 0\n");
		return (0);
	}
	KLSTD_Sleep( 1000 );

	if ( (KLSTD::GetSysTickCount()-sysCount) < 900  )	
	{
		KLSTD_TRACE0(1, L"KLSTD_Sleep error ( in ThreadTestThreadFunc ): time elapsed too small\n");
		return (0);
	}

	int thrId = KLSTD_GetCurrentThreadId();
	if ( thrId==0 ) 
	{
		KLSTD_TRACE0(1, L"KLSTD_GetCurrentThreadId error ( in ThreadTestThreadFunc ): return 0\n");
		return (0);
	}
	else (*counter)++;	
	return 1;
}

void CThreadTest::Initialize(KLTST2::TestData2* pTestData)
{
	::KLSTD_Initialize();
}

KLSTD_NOTHROW void CThreadTest::Deinitialize() throw()
{
	::KLSTD_Deinitialize();
}
	
//!\brief Запускает тест и возвращает результаты выполнения теста.
long CThreadTest::Run( )
{
	int thrId = KLSTD_GetCurrentThreadId();
	if ( thrId==0 ) 
	{
		KLSTD_TRACE0(1, L"CThreadTest::Run: KLSTD_GetCurrentThreadId error: return 0\n");
		return (0);
	}

	KLSTD::Thread *thread = NULL;

	KLSTD_CreateThread( &thread );
	KLSTD_CHKMEM( thread );

	long counter = 10;
	thread->Start( L"TestTHRName", &ThreadTestThreadFunc, &counter );
	bool joinRes = thread->Join();
	if ( joinRes==false )
	{
		KLSTD_TRACE0(1, L"CThreadTest::Run: Join error: return false\n");
		return (-1);
	}

	if ( counter!=11 )
	{
		KLSTD_TRACE0(1, L"CThreadTest::Run: thread function error\n");
		return (-1);
	}

	delete thread;

	return 1;
}


struct CSThreadArg
{
	CSThreadArg()
	{
		inProcess = false;
		counter = 0;
		pCricSec = NULL;
		pSem = NULL;
	}
	KLSTD::CAutoPtr<KLSTD::CriticalSection> pCricSec;
	KLSTD::CAutoPtr<KLSTD::Semaphore>		pSem;	
	bool	inProcess;
	int		counter;
};

const int c_LoopMaxCount = 10;
const int c_NumberOfThread = 3;

unsigned long KLSTD_THREADDECL CriticalSectionTestThreadFunc( void *arpg )
{
	CSThreadArg *threadArg = (CSThreadArg *)arpg;
	if ( threadArg==NULL )
	{
		KLSTD_TRACE0(1, L"CriticalSectionTestThreadFunc error: threadArg==NULL\n");
		return (0);
	}

	threadArg->pCricSec->Enter();
	threadArg->pCricSec->Enter();
	threadArg->pCricSec->Leave();
	threadArg->pCricSec->Leave();

	for( int counter = 0; counter < c_LoopMaxCount; counter++ )
	{
		{
			KLSTD::AutoCriticalSection autoUnlock( threadArg->pCricSec );
			
			if ( threadArg->inProcess ) 
			{
				KLSTD_TRACE0(1, L"CriticalSectionTestThreadFunc error: struct allready in process\n");
				return (-1);
			}
			
			threadArg->inProcess = true;
			threadArg->counter++;
			KLSTD_Sleep( 10 );
			threadArg->inProcess = false;
		}
		KLSTD_Sleep( 10 );
	}

	return 1;
}


void CCriticalSectionTest::Initialize(KLTST2::TestData2* pTestData)
{
	::KLSTD_Initialize();
}

KLSTD_NOTHROW void CCriticalSectionTest::Deinitialize() throw()
{
	::KLSTD_Deinitialize();
}
	
//!\brief Запускает тест и возвращает результаты выполнения теста.
long CCriticalSectionTest::Run( )
{
	KLSTD::Thread *pThreads[c_NumberOfThread];
	CSThreadArg threadsArgs[c_NumberOfThread];

	int tc = 0;
	for( tc = 0; tc < c_NumberOfThread; tc++ )
	{
		KLSTD_CreateCriticalSection( &threadsArgs[tc].pCricSec );

		pThreads[tc] = NULL;
		KLSTD_CreateThread( &pThreads[tc] );
		KLSTD_CHKMEM( pThreads[tc] );
	
		pThreads[tc]->Start( L"TestTHRName", &CriticalSectionTestThreadFunc, &threadsArgs[tc] );
	}

	for( tc = 0; tc < c_NumberOfThread; tc++ )
	{		
		bool joinRes = pThreads[tc]->Join();
		if ( joinRes==false )
		{
			KLSTD_TRACE0(1, L"CCriticalSectionTest::Run: Join error: return false\n");
			return (-1);
		}
	}

	int sumCounter = 0;
	for( tc = 0; tc < c_NumberOfThread; tc++ )
	{	
		sumCounter += threadsArgs[tc].counter;
		delete pThreads[tc];
	}

	if ( sumCounter!=(c_LoopMaxCount*c_NumberOfThread) )
	{
		KLSTD_TRACE0(1, L"CCriticalSectionTest::Run error: wrong counter\n");
		return (-1);
	}
	
	return 1;
}

unsigned long KLSTD_THREADDECL SemaphoreTestThreadFunc( void *arpg )
{
	CSThreadArg *threadArg = (CSThreadArg *)arpg;
	if ( threadArg==NULL )
	{
		KLSTD_TRACE0(1, L"SemaphoreTestThreadFunc error: threadArg==NULL\n");
		return (0);
	}

	bool res = threadArg->pSem->Wait();
	if ( res==false )
	{
		KLSTD_TRACE0(1, L"SemaphoreTestThreadFunc error: wait error\n");
		return (0);
	}

	return 1;
}


void CSemaphoreTest::Initialize(KLTST2::TestData2* pTestData)
{
	::KLSTD_Initialize();
}

KLSTD_NOTHROW void CSemaphoreTest::Deinitialize() throw()
{ 
	::KLSTD_Deinitialize();
}
	
//!\brief Запускает тест и возвращает результаты выполнения теста.
long CSemaphoreTest::Run( )
{
	KLSTD::Semaphore *pSem;
	KLSTD_CreateSemaphore( &pSem, 1 );
	
	bool res = pSem->Wait();
	if ( res==false )
	{
		KLSTD_TRACE0(1, L"CSemaphoreTest::Run error: wait error\n");
		return (-1);
	}

	res = pSem->Wait( 10 );
	if ( res==true )
	{
		KLSTD_TRACE0(1, L"CSemaphoreTest::Run error: second wait error\n");
		return (-1);
	}

	delete pSem;	


	KLSTD::Thread *pThreads[c_NumberOfThread];
	CSThreadArg threadsArgs[c_NumberOfThread];

	int tc = 0;
	for( tc = 0; tc < c_NumberOfThread; tc++ )
	{
		KLSTD_CreateSemaphore( &threadsArgs[tc].pSem, 0 );

		pThreads[tc] = NULL;
		KLSTD_CreateThread( &pThreads[tc] );
		KLSTD_CHKMEM( pThreads[tc] );
	
		pThreads[tc]->Start( L"TestTHRName", &SemaphoreTestThreadFunc, &threadsArgs[tc] );
	}

	for( tc = 0; tc < c_NumberOfThread; tc++ )
	{	
		threadsArgs[tc].pSem->Post();
	}

	for( tc = 0; tc < c_NumberOfThread; tc++ )
	{		
		bool joinRes = pThreads[tc]->Join();
		if ( joinRes==false )
		{
			KLSTD_TRACE0(1, L"CSemaphoreTest::Run: Join error: return false\n");
			return (-1);
		}
	}
	
	return 1;
}

const wchar_t c_ThreadStoreName[] = L"THRSTORETEST";

void CThreadStoreTest::Initialize(KLTST2::TestData2* pTestData)
{
	::KLSTD_Initialize();
}

KLSTD_NOTHROW void CThreadStoreTest::Deinitialize() throw()
{ 
	::KLSTD_Deinitialize();
}
	
//!\brief Запускает тест и возвращает результаты выполнения теста.
long CThreadStoreTest::Run( )
{
	KLSTD::CAutoPtr<KLSTD::CriticalSection> pCricSec;
	KLSTD_CreateCriticalSection( &pCricSec );

	KLSTD_GetGlobalThreadStore()->AddObject( c_ThreadStoreName, pCricSec );

	bool res = KLSTD_GetGlobalThreadStore()->IsObjectHere( c_ThreadStoreName );

	if ( res==false )
	{
		KLSTD_TRACE0(1, L"CThreadStoreTest::Run: IsObjectHere error: return false\n");
		return (-1);
	}

	KLSTD::CAutoPtr<KLSTD::CriticalSection> pTestCric;
	KLSTD_GetGlobalThreadStore()->GetStoredObject( c_ThreadStoreName, (KLSTD::KLBase **)&pTestCric );
	if ( !(pTestCric==pCricSec) )
	{
		KLSTD_TRACE0(1, L"CThreadStoreTest::Run: GetStoredObject error\n");
		return (-1);
	}

	pTestCric = 0;
	KLSTD_GetGlobalThreadStore()->RemoveObject( c_ThreadStoreName, (KLSTD::KLBase **)&pTestCric );
	res = KLSTD_GetGlobalThreadStore()->IsObjectHere( c_ThreadStoreName );

	if ( res )
	{
		KLSTD_TRACE0(1, L"CThreadStoreTest::Run: IsObjectHere error: return true\n");
		return (-1);
	}

	KLSTD_GetGlobalThreadStore()->AddObject( c_ThreadStoreName, pCricSec );

	KLSTD_GetGlobalThreadStore()->CleanupThreadObjects();

	res = KLSTD_GetGlobalThreadStore()->IsObjectHere( c_ThreadStoreName );

	if ( res )
	{
		KLSTD_TRACE0(1, L"CThreadStoreTest::Run: IsObjectHere after CleanupThreadObjects error: return true\n");
		return (-1);
	}


	return 1;
}

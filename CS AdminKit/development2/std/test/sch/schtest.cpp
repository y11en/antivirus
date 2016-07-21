/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file schtest.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Тест библиотеки Scheduler.
 *
 */

#include "schtest.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef N_PLAT_NLM
#  include <nwthread.h>
#include <nwconio.h>
#endif

#include "std/base/kldefs.h"
#include "std/thr/sync.h"
#include "std/trc/trace.h"
#include "build/general.h"
#include "std/err/klerrors.h"
#include "testmodule.h"
#include "helpers.h"

#include "std/sch/scheduler.h"
#include "std/sch/millisecondsschedule.h"
#include "std/sch/everyweekschedule.h"
#include "std/sch/everymonthschedule.h"
#include "std/sch/everydayschedule.h"
#include "std/sch/taskparams.h"
#include "std/sch/taskresults.h"
#include "std/sch/task.h"


#define TestSleep(msec) KLSTD_Sleep(msec)

#ifdef _MBCS
#define MAX_NUMBER_OF_THREAD	3
#define MIN_NUMBER_OF_THREAD	2
#define NUMBER_OF_TESTS_TASKS   10
#else
#define MAX_NUMBER_OF_THREAD	10
#define MIN_NUMBER_OF_THREAD	5
#define NUMBER_OF_TESTS_TASKS   100
#endif

#include "testmodule.h"

struct TestDescription {
	TestDescription( int t, const wchar_t *s )
	{
		testId = t;
		testName = s;
	}
	int			testId;
	const wchar_t *testName;
};

static const wchar_t *SCH_MODULE_TEST_NAME	= L"Scheduler";

struct TestTaskParams;

KLSCH::Errors AddSchedulerTasks( TestTaskParams **paramsArray,int &paramsArraySize,
								KLSCH::Scheduler *scheduler, int numberOfTask, int sleepTimeout,
								int &err_line  );

KLSCH::Errors DeleteSchedulerTasks( TestTaskParams **pArray, int &paramsArraySize,
								   KLSCH::Scheduler *scheduler,
								int &err_line );

KLSCH::Errors CheckTasksWithIteratorAndDelete( TestTaskParams **pArray, int &paramsArraySize,
								KLSCH::Scheduler *scheduler, bool deleteAfterCheck,
								bool checkOnlyHanging, bool checkOnlySuspend, int &err_line );

KLSCH::Errors CheckTaskExecutionResults( TestTaskParams *paramsArray, int paramsArraySize, 
								KLSCH::Scheduler *scheduler,
								int &err_line );

KLSCH::Errors CheckSuspendTasksParams( TestTaskParams *paramsArray, int paramsArraySize,
								KLSCH::Scheduler *scheduler, bool notWorkCheck,
								int &err_line );

KLSCH::Errors SuspendSchedulerTasks( TestTaskParams *paramsArray, int paramsArraySize,
								KLSCH::Scheduler *scheduler,
								int &err_line );

KLSCH::Errors ResumeSchedulerTasks( TestTaskParams *paramsArray, int paramsArraySize,
								KLSCH::Scheduler *scheduler,
								int &err_line );

struct ThreadDesc;

void CreateWorkingThreads( ThreadDesc **tDescArray, int &threadDescArraySize,
						  KLSCH::Scheduler *scheduler, int numberOfThreads );
double StopWorkingThreads( ThreadDesc *tDescArray, int threadDescArraySize );

void DeInit();


void CTaskFunctionality::Initialize(KLTST2::TestData2* pTestData)
{
}

KLSTD_NOTHROW void CTaskFunctionality::Deinitialize() throw()
{
}
	
//!\brief Запускает тест и возвращает результаты выполнения теста.
long CTaskFunctionality::Run( )
{ 
  int err_line = 0;
  KLSCH::Scheduler *scheduler = KLSCH_CreateScheduler();
  KLSCH::Errors err;
  TestTaskParams *paramsArray = NULL;
  int paramsArraySize;

  err = AddSchedulerTasks( &paramsArray, paramsArraySize, scheduler, 100, 0, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  err = CheckTasksWithIteratorAndDelete( &paramsArray, paramsArraySize, scheduler, true, false, false, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  delete scheduler;

  return 0;
}


void CExecution::Initialize(KLTST2::TestData2* pTestData)
{
}

KLSTD_NOTHROW void CExecution::Deinitialize() throw()
{
}

long CExecution::Run()
{
  int err_line = 0;
  KLSCH::Scheduler *scheduler = KLSCH_CreateScheduler();
  KLSCH::Errors err;
  TestTaskParams *paramsArray = NULL;
  int paramsArraySize;
  ThreadDesc *tDescArray = NULL;
  int threadDescArraySize;

  CreateWorkingThreads( &tDescArray, threadDescArraySize, scheduler, MIN_NUMBER_OF_THREAD );

  err = AddSchedulerTasks( &paramsArray, paramsArraySize, scheduler, NUMBER_OF_TESTS_TASKS, 0, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }		

#if defined(N_PLAT_NLM) || defined(__unix)
  TestSleep( 60000 ); // даем время выполниться все задачам
#else
  TestSleep( 8100 ); // даем время выполниться все задачам
#endif
		
  StopWorkingThreads( tDescArray, threadDescArraySize );

  err = CheckTaskExecutionResults( paramsArray, paramsArraySize, scheduler, err_line );
		
  if ( err!=KLSCH::ERR_NONE ) {			
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
                __FILE__, __LINE__, err_line );
    return -1L;		
  }

  err = DeleteSchedulerTasks( &paramsArray, paramsArraySize, scheduler, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }
  delete scheduler;

  return 0;
}


void CIterator::Initialize(KLTST2::TestData2* pTestData)
{
}

KLSTD_NOTHROW void CIterator::Deinitialize() throw()
{
}

long CIterator::Run()
{
  int err_line = 0;
  KLSCH::Scheduler *scheduler = KLSCH_CreateScheduler();
  KLSCH::Errors err;
  TestTaskParams *paramsArray = NULL;
  int paramsArraySize;
  ThreadDesc *tDescArray = NULL;
  int threadDescArraySize;
	
  err = AddSchedulerTasks( &paramsArray, paramsArraySize, scheduler, 10, 8000, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;
  }

  err = CheckTasksWithIteratorAndDelete( &paramsArray, paramsArraySize, scheduler, false, false, false, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  CreateWorkingThreads( &tDescArray, threadDescArraySize, scheduler, 10 );

  TestSleep( 30000 ); // даем время для запуска всех потоков

  err = CheckTasksWithIteratorAndDelete( &paramsArray, paramsArraySize, scheduler, false, true, false, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }
		
  StopWorkingThreads( tDescArray, threadDescArraySize );		

  err = DeleteSchedulerTasks( &paramsArray, paramsArraySize, scheduler, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  delete scheduler;

  return 0;
}


void CSuspendResume::Initialize(KLTST2::TestData2* pTestData)
{
}

KLSTD_NOTHROW void CSuspendResume::Deinitialize() throw()
{
}

long CSuspendResume::Run()
{
  int err_line = 0;
  KLSCH::Scheduler *scheduler = KLSCH_CreateScheduler();
  KLSCH::Errors err;
  TestTaskParams *paramsArray = NULL;
  int paramsArraySize;
  ThreadDesc *tDescArray = NULL;
  int threadDescArraySize;
			
  err = AddSchedulerTasks( &paramsArray, paramsArraySize, scheduler, NUMBER_OF_TESTS_TASKS, 1, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }		

  err = SuspendSchedulerTasks( paramsArray, paramsArraySize, scheduler, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  err = CheckTasksWithIteratorAndDelete( &paramsArray, paramsArraySize, scheduler, false, false, true, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  CreateWorkingThreads( &tDescArray, threadDescArraySize, scheduler, MIN_NUMBER_OF_THREAD );
#if defined(N_PLAT_NLM) || defined(__unix) || defined(_MBCS)
  TestSleep( 10000 ); // даем время для запуска всех потоков
#  else
	#ifdef N_PLAT_NLM
		TestSleep( 5000 ); // даем время для запуска всех потоков
	#else
		TestSleep( 1000 ); // даем время для запуска всех потоков
	#endif
#  endif

  // проверяем что ни одна задача не запустилась
  err = CheckSuspendTasksParams( paramsArray, paramsArraySize, scheduler, true, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  err = ResumeSchedulerTasks( paramsArray, paramsArraySize, scheduler, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }		

#ifdef N_PLAT_NLM
  TestSleep( 40000 ); // даем время для запуска всех задач
#else
  TestSleep( 23100 ); // даем время для запуска всех задач
#endif
  // проверяем что все задачи запустились
  err = CheckSuspendTasksParams( paramsArray, paramsArraySize, scheduler, false, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  StopWorkingThreads( tDescArray, threadDescArraySize );		
		
  err = DeleteSchedulerTasks( &paramsArray, paramsArraySize, scheduler, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  // тестируем Suspend планировшика
  err = AddSchedulerTasks( &paramsArray, paramsArraySize, scheduler, NUMBER_OF_TESTS_TASKS, 1, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }		

  scheduler->Suspend();

  CreateWorkingThreads( &tDescArray, threadDescArraySize, scheduler, MIN_NUMBER_OF_THREAD );

  TestSleep( 1000 ); // даем время для запуска всех потоков

  // проверяем что ни одна задача не запустилась
  err = CheckSuspendTasksParams( paramsArray, paramsArraySize, scheduler, true, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  scheduler->Resume();

#if defined(N_PLAT_NLM) || defined(__unix)
  TestSleep( 40000 ); // даем время для запуска всех задач
#else
  TestSleep( 5100 ); // даем время для запуска всех задач
#endif


  // проверяем что все задачи запустились
  err = CheckSuspendTasksParams( paramsArray, paramsArraySize, scheduler, false, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  StopWorkingThreads( tDescArray, threadDescArraySize );		
		
  err = DeleteSchedulerTasks( &paramsArray, paramsArraySize, scheduler, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }		

  delete scheduler;

  return 0;
}


void CPerformance::Initialize(KLTST2::TestData2* pTestData)
{
}

KLSTD_NOTHROW void CPerformance::Deinitialize() throw()
{
}

long CPerformance::Run()
{
  int err_line = 0;
  KLSCH::Scheduler *scheduler = KLSCH_CreateScheduler();
  KLSCH::Errors err;
  const int NUMBER_OF_THREAD = 3;
  TestTaskParams *paramsArray = NULL;
  int paramsArraySize;
  ThreadDesc *tDescArray = NULL;
  int threadDescArraySize;

  CreateWorkingThreads( &tDescArray, threadDescArraySize, scheduler, NUMBER_OF_THREAD );

  err = AddSchedulerTasks( &paramsArray, paramsArraySize, scheduler, 600, (-1), err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }		

  TestSleep( 5100 ); // даем время выполниться всем задачам

  double numberOfCalls = StopWorkingThreads( tDescArray, threadDescArraySize );

  KLTRACE1( L"SCH_TEST", L"GetMAXNumberOfCalls - %0.2f\n", numberOfCalls * NUMBER_OF_THREAD );

  err = DeleteSchedulerTasks( &paramsArray, paramsArraySize, scheduler, err_line );
  if ( err!=KLSCH::ERR_NONE ) {
    KLTRACE1( L"SCH_TEST", L"Error in sch_test. File - '%hs', Line - %d err_line - %d.\n",
              __FILE__, __LINE__, err_line );
    return -1L;		
  }

  delete scheduler;

  return 0;
}

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Task.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Набор тестовых функций.
 *
 */

#include <iostream>

using namespace std;

#ifdef WIN32
# include <stdio.h>
# include <conio.h>
#endif

#include "std/thr/thread.h"
#include "std/thr/sync.h"
#include "std/trc/trace.h"
#include "std/time/kltime.h"

#include "std/sch/scheduler.h"
#include "std/sch/millisecondsschedule.h"
#include "std/sch/everyweekschedule.h"
#include "std/sch/everymonthschedule.h"
#include "std/sch/everydayschedule.h"
#include "std/sch/taskparams.h"
#include "std/sch/taskresults.h"
#include "std/sch/task.h"

#ifdef N_PLAT_NLM
#include <nwmalloc.h>
#endif

// глобальные параметры задач ( используется для получение результатов работы тестов )
struct TestTaskParams {

	int prevTickCounter;
	int taskNum;
	int timeOut;
	int numberOfCalls;
	int numberErrorsOfTimeOut;
	int lastCallTimeSec;
	int lastCallTimeMSec;
	int sleepTimeout;

	struct tm firstStartTime;
	int firstStartTimeSec;
	int firstStartTimeMSec;
};

const int TIME_DELTA_VALUE	= 500; // 0,5 sec

class TaskCallbackParams : public KLSCH::TaskParams
{
public:
	TaskCallbackParams( TestTaskParams *p )
	{
		par = p;

		par->prevTickCounter = 0;		
		par->numberOfCalls = 0;
		par->numberErrorsOfTimeOut = 0;
	}

	TaskCallbackParams( const TaskCallbackParams& tcp )
	{
		par = tcp.par;
	}

	// функция копироваия
	virtual KLSCH::TaskParams *Clone()
	{
		return new TaskCallbackParams( *this );
	}

	TestTaskParams *par;
};

class TaskCallbackResult : public KLSCH::TaskResults
{
public:
	TaskCallbackResult( TestTaskParams *p )
	{
		par = p;
		taskId = p->taskNum;		
	}

	TaskCallbackResult( const TaskCallbackResult& tcr )
	{
		par = tcr.par;
		taskId = tcr.taskId;
	}

	// функция копироваия
	virtual KLSCH::TaskResults *Clone()
	{
		return new TaskCallbackResult( *this );
	}

	TestTaskParams *par;
	int taskId;
};



KLSCH::Errors UniversalTaskCallback(const KLSCH::TaskParams* tParams, 
									KLSCH::TaskResults **tResult )
{
	TaskCallbackParams *tcParams = (TaskCallbackParams *)tParams;

	if ( tcParams->par->prevTickCounter==0 ) 
	{
		tcParams->par->prevTickCounter = KLSTD::GetSysTickCount();

		time_t now = time(NULL);
		struct tm infoT;
		KL_LOCALTIME( now, infoT );
		char timeStr[128];
		strftime( timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", &infoT );


		KLTRACE5( L"SCH_TEST", L"========================================Task %3d First start - %5d thread - %d current time - %s\n", 
			tcParams->par->taskNum, tcParams->par->prevTickCounter, *tResult, timeStr ) ;		

		tcParams->par->lastCallTimeSec	= now;
		tcParams->par->lastCallTimeMSec	= KLSTD::GetCurrentMilliseconds();
	}
	else
	{
		int cutTcount = KLSTD::GetSysTickCount();
//		KLTRACE5( L"SCH_TEST", L"========================================Task %3d Difference - %5d thread - %d\n", 
//			tcParams->par->taskNum, cutTcount-tcParams->par->prevTickCounter, *tResult );

		if ( (cutTcount-tcParams->par->prevTickCounter)>(tcParams->par->timeOut+TIME_DELTA_VALUE) )
		{
			KLTRACE5( L"SCH_TEST", L"=================================================Error of timeout============= Task %d Diff - %d TimeOut - %d\n", 
				tcParams->par->taskNum, cutTcount-tcParams->par->prevTickCounter, tcParams->par->timeOut );

			tcParams->par->numberErrorsOfTimeOut++;			
		}
	

		tcParams->par->prevTickCounter = cutTcount;
	}

	tcParams->par->numberOfCalls++;

	if ( tcParams->par->sleepTimeout>0 ) {
      KLSTD_Sleep( tcParams->par->sleepTimeout );
    }

	*tResult = new TaskCallbackResult( tcParams->par );

	return KLSCH::ERR_NONE;
}

struct WorkingThreadParams 
{
	KLSCH::Scheduler	*workingScheduler;
	bool stopThread;
	double maxNumberOfCalls;
	int threadNum;
	void *taskParams;
};

unsigned long KLSTD_THREADDECL scheduler_worker_thread(void *s)
{
	WorkingThreadParams *tParams = (WorkingThreadParams *)s;

	int threadNum = tParams->threadNum;
	int callCounter = 0;

	KLTRACE5( L"SCH_TEST", L"scheduler_worker_thread - %d started...\n", threadNum );

	long saveTickCount = KLSTD::GetSysTickCount();
	int saveCallCount = 0;

#ifdef N_PLAT_NLM
    // ThreadSwitchLowPriority();
#endif
	while(1)
	{



		KLSCH::TaskResults *res = (KLSCH::TaskResults *)threadNum;
		KLSCH::Errors err = tParams->workingScheduler->WaitTaskExecution( &res, 
                                                                          tParams->taskParams ); // 1000  - ptr -)

		if ( err==KLSCH::ERR_NO_MORE_TASKS || err==KLSCH::ERR_SUSPEND_MODE )
		{
			KLTRACE5( L"SCH_TEST", L"WaitTaskExecution error - %d\n", err  );
			if ( tParams->stopThread ) break;
			continue;
		}	

		if ( err==KLSCH::ERR_NONE )
			if ( res!=NULL) delete res;

		if ( tParams->stopThread ) break;

		callCounter++;

		if ( callCounter % 1000 == 0 ) 
		{
			double numOfCalls = double(callCounter - saveCallCount) / (double(KLSTD::GetSysTickCount() - saveTickCount) / 1000 );				
			KLTRACE5( L"SCH_TEST", L"Thread - %d Call counter - %d Call per second - %0.2f\n", threadNum, callCounter,
				 numOfCalls );
			if ( numOfCalls>tParams->maxNumberOfCalls ) tParams->maxNumberOfCalls=numOfCalls;

			saveTickCount = KLSTD::GetSysTickCount();
			saveCallCount = callCounter;
			if ( tParams->stopThread ) break;
		}
	}

    KLTRACE5( L"SCH_TEST", L"Thread near finish - %d Call counter - %d\n", threadNum, callCounter );
    

	return 0;
}


void DeInit( TestTaskParams **paramsArray, int &paramsArraySize );


KLSCH::Errors AddSchedulerTasks( TestTaskParams **pArray,int &paramsArraySize,
								KLSCH::Scheduler *scheduler, int numberOfTask, int sleepTimeout,
								int &err_line  )
{
	DeInit( pArray, paramsArraySize );

	*pArray = new TestTaskParams[numberOfTask];
	TestTaskParams *paramsArray = *pArray;
	paramsArraySize = numberOfTask;

	scheduler->SetWaitTime( 1000 );

	for( int taskCounter = 0; taskCounter < paramsArraySize; taskCounter++ )
	{
		int edCounter = 2, ewCounter = 2, emCounter = 2;

		KLSCH::MillisecondsSchedule *mlSchedule = KLSCH_CreateMillisecondsSchedule();
		KLSCH::EverydaySchedule		*edSchedule = KLSCH_CreateEverydaySchedule();
		KLSCH::EveryWeekSchedule	*ewSchedule = KLSCH_CreateEveryWeekSchedule();
		KLSCH::EveryMonthSchedule	*emSchedule = KLSCH_CreateEveryMonthSchedule();

		KLSCH::Task *myTask = KLSCH_CreateTask();

		time_t now = time(NULL);
		struct tm infoT = *localtime( &now );

		paramsArray[taskCounter].firstStartTimeMSec = 0;
		paramsArray[taskCounter].firstStartTimeSec = 0;
		paramsArray[taskCounter].timeOut = 1000;
		paramsArray[taskCounter].sleepTimeout = sleepTimeout;
			
		if ( taskCounter % 4 == 0 || sleepTimeout!=0 ) 
		{			
			const int coof = 50;

			if ( sleepTimeout==(-1) )
				mlSchedule->SetPeriod( 0 ); // для performance теста
			else
				mlSchedule->SetPeriod( (taskCounter+1)*coof );
			//mlSchedule.SetPeriod( 10000000 );

			myTask->SetSchedule( mlSchedule );
			if ( sleepTimeout!=(-1) ) myTask->SetStartDeltaTimeout( 5 );

			paramsArray[taskCounter].firstStartTimeMSec = (taskCounter+1)*coof;
			paramsArray[taskCounter].timeOut = (taskCounter+1)*coof;

			paramsArray[taskCounter].firstStartTimeSec = now;
			paramsArray[taskCounter].firstStartTimeSec += ((taskCounter+1)*coof)/KLSCH::c_MillisecondsInSecond;
		}		

		if ( taskCounter % 4 == 1 && sleepTimeout==0 ) 
		{
			edCounter++;
			if ( edCounter>7 ) edCounter=2;

			if ( infoT.tm_sec + edCounter > 59 ) infoT.tm_sec = 0, infoT.tm_min++;
			if ( infoT.tm_min > 59 ) infoT.tm_hour++, infoT.tm_min=0;
			if ( infoT.tm_hour > 23 ) infoT.tm_mday++;

			infoT.tm_sec += edCounter;

			edSchedule->SetExecutionTime( infoT.tm_hour, infoT.tm_min, infoT.tm_sec );

			myTask->SetSchedule( edSchedule );
		}

		if ( taskCounter % 4 == 2 && sleepTimeout==0 ) 
		{
			ewCounter++;
			if ( ewCounter>7 ) ewCounter=2;

			if ( infoT.tm_sec + ewCounter > 59 ) infoT.tm_sec = 0, infoT.tm_min++;
			if ( infoT.tm_min > 59 ) infoT.tm_hour++, infoT.tm_min=0;
			if ( infoT.tm_hour > 23 ) infoT.tm_mday++;

			infoT.tm_sec += ewCounter;

			ewSchedule->SetExecutionTime( infoT.tm_wday, infoT.tm_hour, infoT.tm_min, 
				infoT.tm_sec );

			myTask->SetSchedule( ewSchedule );
		}

		if ( taskCounter % 4 == 3 && sleepTimeout==0 ) 
		{
			emCounter++;
			if ( emCounter>7 ) emCounter=2;

			if ( infoT.tm_sec + emCounter > 59 ) infoT.tm_sec = 0, infoT.tm_min++;
			if ( infoT.tm_min > 59 ) infoT.tm_hour++, infoT.tm_min=0;
			if ( infoT.tm_hour > 23 ) infoT.tm_mday++;

			infoT.tm_sec += emCounter;

			emSchedule->SetExecutionTime( infoT.tm_mday, infoT.tm_hour, infoT.tm_min, 
				infoT.tm_sec );

			myTask->SetSchedule( emSchedule );
		}

		if ( sleepTimeout!=0 )
			myTask->SetMaxExecutionTime( sleepTimeout / 10 );

		paramsArray[taskCounter].firstStartTime = infoT;

		myTask->SetCallback( UniversalTaskCallback );		
		
		paramsArray[taskCounter].taskNum = taskCounter+1;
		
		TaskCallbackParams tcp( &paramsArray[taskCounter] );
		myTask->SetTaskParams( &tcp );

		myTask->SetTaskId( paramsArray[taskCounter].taskNum );

		KLSCH::TaskId tId;
		KLSCH::Errors err = scheduler->AddTask( myTask, tId );

		delete myTask;
		delete mlSchedule;
		delete edSchedule;
		delete ewSchedule;
		delete emSchedule;

		if ( tId!=paramsArray[taskCounter].taskNum )
		{
			err_line  = __LINE__;
			return KLSCH::ERR_TASK_ID;		
		}		
	}

	return KLSCH::ERR_NONE;
}

KLSCH::Errors DeleteSchedulerTasks( TestTaskParams **pArray, int &paramsArraySize,
								   KLSCH::Scheduler *scheduler,
								int &err_line )
{
	TestTaskParams *paramsArray = *pArray;
	for( int taskCounter = 0; taskCounter < paramsArraySize; taskCounter++ )
	{
		KLSCH::Errors err = scheduler->DelTask( paramsArray[taskCounter].taskNum );

		if ( err!=KLSCH::ERR_NONE ) 
		{
			err_line = __LINE__;
			return err;		
		}		
	}

	DeInit( pArray, paramsArraySize );

	return KLSCH::ERR_NONE;
}

/*!\brief Функция получает Iterator из Scheduler'а и производить контроль задач
*		затем удалят все задачи и вновь проверяет
*/
KLSCH::Errors CheckTasksWithIteratorAndDelete( TestTaskParams **pArray, int &paramsArraySize,
								KLSCH::Scheduler *scheduler, bool deleteAfterCheck,
								bool checkOnlyHanging, bool checkOnlySuspend, int &err_line )
{	
	KLSCH::Errors err;
	const KLSCH::Task *task;
	bool hasMoreTasks = false;
	int taskCounter = 0;
	TestTaskParams *paramsArray = *pArray;

	if ( !checkOnlyHanging )
	{
		KLSCH::TasksIterator *it = KLSCH_CreateTasksIterator();
		err = scheduler->CreateTasksIterator( it );
		if ( err!=KLSCH::ERR_NONE ) 
		{
			err_line = __LINE__;
			return err;
		}
		
		it->Reset();		
		
		do
		{
			it->Next( &task, hasMoreTasks );
			if ( task!=NULL )
			{	
				KLSCH::Task::State tState;
				KLSCH::TaskId tId;				
				const TaskCallbackParams *tParams;

				taskCounter++;			
				
				
				task->GetTaskId( tId );
				task->GetTaskParams( (const KLSCH::TaskParams **)&tParams );
				task->GetCurrentState( tState );
								
				if ( tId!=tParams->par->taskNum )
				{
					err_line = __LINE__;
					return KLSCH::ERR_TASK_ID;
				}		
				if ( checkOnlySuspend )
				{
					if ( tState!=KLSCH::Task::Suspended ) 
					{
						err_line = __LINE__;
						return KLSCH::ERR_TASK_ID;
					}		
				}
			}
		}while( hasMoreTasks );
		
		if ( taskCounter!=paramsArraySize )
		{
			err_line = __LINE__;
			return KLSCH::ERR_TEST;
		}
		
		err = scheduler->CreateHangingTasksIterator( it );
		if ( err!=KLSCH::ERR_NONE ) 
		{
			err_line = __LINE__;
			return err;
		}
		
		it->Reset();
		it->Next( &task, hasMoreTasks );
		
		if ( task!=NULL ) 
		{
			err_line = __LINE__;
			return KLSCH::ERR_TEST;
		}

		if ( deleteAfterCheck )
		{			
			// удаляем все задачи ( задачи не должны удалиться физически )
			err = DeleteSchedulerTasks( pArray, paramsArraySize, scheduler, err_line );
			if ( err!=KLSCH::ERR_NONE ) 
			{
				return err;
			}
			
			err = scheduler->CreateTasksIterator( it );
			it->Reset();	
			
			do
			{
				it->Next( &task, hasMoreTasks );
				if ( task!=NULL )
				{			
					taskCounter++;			
					KLSCH::Task::State curState;				
					
					task->GetCurrentState( curState );
					if ( curState!=KLSCH::Task::Deleted )
					{
						err_line = __LINE__;
						return KLSCH::ERR_TASK_ID;
					}		
				}
			}while( hasMoreTasks );
		}

		delete it;
	} // после данной строчки все итераторы уничтожаются
	
	if ( deleteAfterCheck || checkOnlyHanging )
	{
		KLSCH::TasksIterator *it = KLSCH_CreateTasksIterator();
		
		if ( checkOnlyHanging )
			err = scheduler->CreateHangingTasksIterator( it );
		else	
			err = scheduler->CreateTasksIterator( it );
		
		if ( err!=KLSCH::ERR_NONE ) 
		{
			err_line = __LINE__;
			return err;
		}
		
		// больше задач не должно быть
		it->Reset();
		
		if ( !checkOnlyHanging )
		{
			it->Next( &task, hasMoreTasks );
			
			if ( task!=NULL ) 
			{
				err_line = __LINE__;
				return KLSCH::ERR_TEST;
			}
		}
		else
		{
			do
			{
				it->Next( &task, hasMoreTasks );
				if ( task!=NULL )
				{			
					taskCounter++;			
					KLSCH::Task::State curState;				
					
					task->GetCurrentState( curState );
					if ( curState!=KLSCH::Task::Running )
					{
						err_line = __LINE__;
						return KLSCH::ERR_TASK_ID;
					}		
				}
			}while( hasMoreTasks );
			
			if ( taskCounter!=paramsArraySize )
			{
				KLTRACE1( L"SCH_TEST", L" taskCounter - %d paramsArraySize - %d\n", taskCounter, paramsArraySize );
				err_line = __LINE__;
				return KLSCH::ERR_TEST;
			}
		}
		delete it;
	}

	return KLSCH::ERR_NONE;
}

//\brief Данная функция возвращает описание задачи
KLSCH::Errors GetTaskPtr( KLSCH::Scheduler *scheduler, const KLSCH::Task **t, KLSCH::TaskId tId, int &err_line )
{
	KLSCH::TasksIterator *it = KLSCH_CreateTasksIterator();
	KLSCH::Errors err = scheduler->CreateTasksIterator( it );
	if ( err!=KLSCH::ERR_NONE ) {
		err_line = __LINE__;
		return err;	}
	
	it->Reset();	
	*t = NULL;
	bool hasMoreTasks = false;
	do
	{
		it->Next( t, hasMoreTasks );
		if ( t!=NULL )
		{		
			KLSCH::TaskId curtId;			
			
			(*t)->GetTaskId( curtId );			
			
			if ( curtId==tId )
			{
				break;
			}		
		}
	}while( hasMoreTasks );

	delete it;

	return KLSCH::ERR_NONE;
}

KLSCH::Errors
CheckTaskExecutionResults( TestTaskParams *paramsArray, int paramsArraySize, 
                           KLSCH::Scheduler *scheduler,
                           int &err_line )
{
	for( int taskCounter = 0; taskCounter < paramsArraySize; taskCounter++ )
	{
		if ( paramsArray[taskCounter].numberOfCalls==0 ) {
          err_line = __LINE__; return KLSCH::ERR_TEST;
        }
		
		if ( paramsArray[taskCounter].numberErrorsOfTimeOut!=0 ) {		
          int to = TIME_DELTA_VALUE;
          KLTRACE1( L"SCH_TEST", L"Timeout for start of tasks in scheduler test was excedeed. Task nume - %d Time out limit - %d\n", 
					taskCounter, to );
		}

		time_t fst = mktime( &paramsArray[taskCounter].firstStartTime );
		if ( paramsArray[taskCounter].firstStartTimeSec != 0 ) {
          fst = paramsArray[taskCounter].firstStartTimeSec;
        }

		/*if ( (paramsArray[taskCounter].lastCallTimeSec - fst) > 2 ) {
          wcerr << "Num - " << paramsArray[taskCounter].taskNum
                << " diff - "
                << (paramsArray[taskCounter].lastCallTimeSec - fst) << endl;
        }
        */
			//err_line = __LINE__; return KLSCH::ERR_TEST; }
		
		// проверяем саму задачу		
		KLSCH::Task::State currState;
		const KLSCH::Task *task;
		const KLSCH::Schedule *schedule;

		KLSCH::Errors err = GetTaskPtr( scheduler, &task, paramsArray[taskCounter].taskNum, err_line );
		if ( err!=KLSCH::ERR_NONE ) { return err; }

		if ( task==NULL ) {
          err_line = __LINE__;
          return KLSCH::ERR_TEST;
        }
		
		task->GetCurrentState( currState );
		if ( currState!=KLSCH::Task::Scheduled ) {
          err_line = __LINE__;
          return KLSCH::ERR_TEST;
        }

		task->GetSchedule( &schedule );

		/*if ( dynamic_cast<const KLSCH::EverydaySchedule*>(schedule)!=NULL ) 
		{
			const KLSCH::EverydaySchedule *edScheduler = dynamic_cast<const KLSCH::EverydaySchedule*>(schedule);
		}

		if ( dynamic_cast<const KLSCH::EveryWeekSchedule*>(schedule)!=NULL ) 
		{
			const KLSCH::EveryWeekSchedule *edScheduler = dynamic_cast<const KLSCH::EveryWeekSchedule*>(schedule);
		}

		if ( dynamic_cast<const KLSCH::EveryMonthSchedule*>(schedule)!=NULL ) 
		{
			const KLSCH::EveryMonthSchedule *edScheduler = dynamic_cast<const KLSCH::EveryMonthSchedule*>(schedule);
		}*/
	}
	return KLSCH::ERR_NONE;
}

KLSCH::Errors CheckSuspendTasksParams( TestTaskParams *paramsArray, int paramsArraySize,
								KLSCH::Scheduler *scheduler, bool notWorkCheck,
								int &err_line )
{
	for( int taskCounter = 0; taskCounter < paramsArraySize; taskCounter++ )
	{
		if ( notWorkCheck )
		{
			if ( paramsArray[taskCounter].numberOfCalls!=0 ) { 			
				err_line = __LINE__; return KLSCH::ERR_TEST; }
		} else {
			if ( paramsArray[taskCounter].numberOfCalls==0 ) { 			
				err_line = __LINE__; return KLSCH::ERR_TEST; }
		}
	}
	return KLSCH::ERR_NONE;
}

KLSCH::Errors SuspendSchedulerTasks( TestTaskParams *paramsArray, int paramsArraySize,
								KLSCH::Scheduler *scheduler,
								int &err_line )
{
	for( int taskCounter = 0; taskCounter < paramsArraySize; taskCounter++ )
	{
		KLSCH::Errors err = scheduler->SuspendTask( paramsArray[taskCounter].taskNum );

		if ( err!=KLSCH::ERR_NONE ) 
		{
			err_line = __LINE__;
			return err;		
		}		
	}
	return KLSCH::ERR_NONE;
}

KLSCH::Errors ResumeSchedulerTasks( TestTaskParams *paramsArray, int paramsArraySize,
								KLSCH::Scheduler *scheduler,
								int &err_line )
{
	for( int taskCounter = 0; taskCounter < paramsArraySize; taskCounter++ )
	{
		KLSCH::Errors err = scheduler->ResumeTask( paramsArray[taskCounter].taskNum );

		if ( err!=KLSCH::ERR_NONE ) 
		{
			err_line = __LINE__;
			return err;		
		}		
	}
	return KLSCH::ERR_NONE;
}

struct ThreadDesc
{
	KLSTD::Thread *thread;
	WorkingThreadParams threadParams;
};

void CreateWorkingThreads( ThreadDesc **tDescArray, int &threadDescArraySize,
						  KLSCH::Scheduler *scheduler, int numberOfThreads )
{
	*tDescArray = new ThreadDesc[numberOfThreads];	
	ThreadDesc *threadDescArray = *tDescArray;

	threadDescArraySize = numberOfThreads;
	
	for( int tc = 0; tc<numberOfThreads; tc++ ) 
	{		
        threadDescArray[tc].threadParams.workingScheduler = scheduler;
		threadDescArray[tc].threadParams.maxNumberOfCalls = 0;
		threadDescArray[tc].threadParams.stopThread = false;
		threadDescArray[tc].threadParams.threadNum = tc;
		threadDescArray[tc].threadParams.taskParams = (void *)tc;		

		threadDescArray[tc].thread = NULL;
		KLSTD_CreateThread( &threadDescArray[tc].thread );

		if ( threadDescArray[tc].thread==NULL ) 
			KLTRACE1( L"SCH_TEST", L"CreateThread thread - %d created handle - %d...\n", tc,
				threadDescArray[tc].thread );
		else {
          threadDescArray[tc].thread->Start( L"SCH test thread", scheduler_worker_thread, 
                                             (void *)&threadDescArray[tc].threadParams );
        }
	}
}

double StopWorkingThreads( ThreadDesc *tDescArray, int threadDescArraySize )
{
	ThreadDesc *threadDescArray = tDescArray;

	double maxCalls = 0;
	for( int tc = 0; tc<threadDescArraySize; tc++ )
	{
		threadDescArray[tc].threadParams.stopThread = true;

		threadDescArray[tc].thread->Join();

		if ( threadDescArray[tc].threadParams.maxNumberOfCalls>maxCalls )
			maxCalls = threadDescArray[tc].threadParams.maxNumberOfCalls;

		delete threadDescArray[tc].thread;
	}		

	delete []threadDescArray;

	return maxCalls;
}

void DeInit( TestTaskParams **paramsArray, int &paramsArraySize )
{
	if ( *paramsArray!=NULL ) delete [](*paramsArray);
	paramsArraySize = 0;
	*paramsArray = NULL;
}



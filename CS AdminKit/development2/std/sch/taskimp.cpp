/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TaskImp.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализацией класса задачи для запуска модулем-планировщиком.
 *
 */

#include <ctime>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "std/thr/sync.h"
#include "std/trc/trace.h"
#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "std/sch/schedule.h"
#include "std/sch/taskparams.h"
#include "std/time/kltime.h"
#include "std/sch/millisecondsschedule.h"

#include "taskimp.h"
#include "private.h"


//!\brief Вспомогательный класс для изменения приоритета потока
class SchedulerThreadPriority
{
public:
	SchedulerThreadPriority() { settingNewPriorityFlag = false; }

	void SetNewPriority( KLSCH::Task::TaskThreadPriority ttPriority )
	{
#ifdef _WIN32
		// сохраняем текущий приоритет потока
		threadPrevPriority = GetThreadPriority( GetCurrentThread() );
		settingNewPriorityFlag = true;

		int newThreadPriority = THREAD_PRIORITY_NORMAL;
		if ( ttPriority==KLSCH::Task::TP_Low )
			newThreadPriority = THREAD_PRIORITY_LOWEST;
		if ( ttPriority==KLSCH::Task::TP_Hight )
			newThreadPriority = THREAD_PRIORITY_HIGHEST;

		SetThreadPriority( GetCurrentThread(), newThreadPriority );
#endif	
	}

	void RestorePrioroty()
	{
#ifdef _WIN32
		if ( settingNewPriorityFlag )
		{
			SetThreadPriority( GetCurrentThread(), threadPrevPriority );
			settingNewPriorityFlag = false;
		}
#endif
	}

protected:
	int		threadPrevPriority;
	bool	settingNewPriorityFlag;
};

namespace KLSCH {

			
	enum State {
			Created,			///< Задача только что создана и еще не попала в Scheduler
			Scheduled,			///< Задача находиться в состоянии ожидания вызова
			WaitForRunning,		///< Данная задача ожидает запуска
			Running,			///< Задача выполняется
			Suspended,			///< Задача времено приостановлена ( см. Scheduler::SuspendTask )
			Completed,			///< Задача уже выполнелась и больше зупускаться не будет ( на основе ее расписания )
			Deleted				///< Задача удалена ( физически из списка будет удалена позже )
		};

	// используется для вывода состояний задачи ( только для отладки )
	char STATE_STR_DESC[10][32] = { "Created", "Scheduled", "WaitForRunning", "Running",
								"Suspended", "Completed", "Deleted" };


	//! \brief Конструкторы.
	TaskImp::TaskImp( )
	{		
		InitializeVars();
	}

	TaskImp::TaskImp( TaskId id, TaskClassId cid )
	{
		InitializeVars();
		tId = id;
		cId = cid;
	}

	const TaskImp &TaskImp::operator = ( const TaskImp &t )
	{
		tId = t.tId;
		cId = t.cId;
		maxExecuteTime	= t.maxExecuteTime;

		if ( t.schedule!=NULL ) t.schedule->Clone( &schedule );
		else schedule = NULL;
		if ( t.params!=NULL ) params = t.params->Clone( );
		else params = NULL;

		nextExeTime = t.nextExeTime; nextExemsec = t.nextExemsec;			
		lastExeTime = t.lastExeTime; lastExemsec = t.lastExemsec;

		state		= t.state;		

		startSysTick = t.startSysTick;
		func		 = t.func;
		func2		 = t.func2;

		threadCntx   = t.threadCntx;
		delAfterRun	 = t.delAfterRun;

		startDeltaTimeout = t.startDeltaTimeout;
		preparativeTimeout = t.preparativeTimeout;
		preparativeStart = t.preparativeStart;
		if ( t.accessorySchedule!=NULL ) t.accessorySchedule->Clone( &accessorySchedule );
		else accessorySchedule = NULL;
		
		taskThreadPriority = t.taskThreadPriority;

		return *this;
	}

	void TaskImp::InitializeVars()				//!< Инициализирует внутренние переменные
	{
		tId = 0;
		cId = 0;
		maxExecuteTime = c_Infinite;
		schedule	= NULL;
		params		= NULL;

		nextExeTime = nextExemsec = c_Undefined;
		lastExeTime = lastExemsec = c_Undefined;

		state		= Created;		

		startSysTick = 0;
		func		 = NULL;
		func2		 = NULL;
		threadCntx	 = NULL;
		startDeltaTimeout = 0;
		preparativeTimeout = 0;
		preparativeStart = false;
		accessorySchedule = NULL;
		taskThreadPriority = TP_Normal;

		delAfterRun  = false;
	}

	//!\brief Деструктор. Удаляет объект Schedule.
	TaskImp::~TaskImp( )
	{
		if ( schedule!=NULL ) delete schedule;
		if ( params!=NULL ) delete params;
		if ( accessorySchedule!=NULL ) delete accessorySchedule;
	}

    //!\brief Устанавливает идентификатор задачн.
	Errors TaskImp::SetTaskId( TaskId id )
	{
		tId = id;
		return ERR_NONE;
	}	
	
	//!\brief Возвращает идентификатор задачн.
	Errors TaskImp::GetTaskId( TaskId& id ) const
	{
		id = tId;
		return ERR_NONE;
	}	
	
	//!\brief Устанавливает идентификатор класса задачн.
	Errors TaskImp::SetClassId( TaskClassId cid )
	{
		cId = cid;
		return ERR_NONE;
	}	
	
	//!\brief Возвращает идентификатор класса задачн.
	Errors TaskImp::GetClassId( TaskClassId& cid ) const
	{
		cid = cId;
		return ERR_NONE;
	}	
	
	//!\brief Возвращает текущие состояние задачи.
	Errors TaskImp::GetCurrentState( TaskImp::State& s ) const
	{
		s = state;
		return ERR_NONE;
	}
	
	//!\brief Устанавливает расписание для запуска задачи.
	Errors TaskImp::SetSchedule( Schedule* s )
	{
		if ( schedule!=NULL ) delete schedule;
		s->Clone( &schedule );		
		return ERR_NONE;
	}
	
	//!\brief Возвращает указатель на расписание для запуска задачи.
	Errors TaskImp::GetSchedule( const Schedule ** s) const
	{
		*s = schedule;
		return ERR_NONE;
	}
	
	//!\brief Устанавливает callback для запуска задачи.
	Errors TaskImp::SetCallback( TaskCallback cb)
	{
		func = cb;
		return ERR_NONE;
	}

	//!\brief Устанавливает расширенную callback для запуска задачи.
	Errors TaskImp::SetCallback2( TaskCallback2 cb)
	{
		func2 = cb;
		return ERR_NONE;
	}
	
	//!\brief Возвращает установленный callback для запуска задачи.
	Errors TaskImp::GetCallback( TaskCallback& cb ) const
	{
		cb = func;
		return ERR_NONE;
	}
	
	//!\brief Устанавливает параметры для запуска задачи.
	Errors TaskImp::SetTaskParams( TaskParams* par )
	{
		if ( par==NULL ) return ERR_INVALID_OBJECT;
		params = par->Clone(); // копируем параметры
		return ERR_NONE;
	}
	
	//!\brief Возвращает копию параметров для запуска задачи.
	Errors TaskImp::GetTaskParams( const TaskParams** par ) const
	{
		*par = params;
		return ERR_NONE;
	}

	//!\brief Устанавливает диапазона в котором может колибаться время запуска задачи (msec).
    Errors TaskImp::SetStartDeltaTimeout( int timeOut )
	{
		if ( timeOut<0 ) return ERR_TIME_INTERVAL;
		
		startDeltaTimeout = timeOut;
		return ERR_NONE;
	}

    //!\brief Возвращает диапазона в котором может колибаться время запуска задачи (msec).
    Errors TaskImp::GetStartDeltaTimeout(int& timeOut) const
	{
		timeOut = startDeltaTimeout;
		return ERR_NONE;
	}
	
	//!\brief Устанавливает timeout для подготовительного запуска
    Errors TaskImp::SetPreparativeStartTimeout( int timeout )
	{
		if ( timeout<0 ) return ERR_TIME_INTERVAL;
				
		preparativeTimeout = timeout;
		if ( preparativeTimeout>0 ) preparativeStart = true;
		else preparativeStart = false;
		
		if ( accessorySchedule!=NULL ) delete accessorySchedule;
		accessorySchedule = NULL;

		return ERR_NONE;
	}

	//!\brief Возвращяет timeout для подготовительного запуска
	Errors TaskImp::GetPreparativeStartTimeout( int &timeout ) const
	{
		timeout = preparativeTimeout;		
		return ERR_NONE;
	}
	
	//!\brief Устанавливает максимальное время выполнения задачи (msec).
	Errors TaskImp::SetMaxExecutionTime( int maxTime /* = 10000*/ )
	{
		maxExecuteTime = maxTime;
		return ERR_NONE;
	}
	
	//!\brief Возвращает максимальное время выполнения задачи (msec).
	Errors TaskImp::GetMaxExecutionTime(int& maxTime) const
	{
		maxTime = maxExecuteTime;
		if ( maxTime<0 ) maxTime = 0;
		return ERR_NONE;
	}

	//!\brief Возвращает время выполенения текущей задачи.
    Errors TaskImp::GetRunningTime( int &msec )
	{
		msec = 0;
		if ( state!=Running || startSysTick==0 ) return ERR_TASK_STATE;
		
		msec = (KLSTD::GetSysTickCount()-startSysTick);
		
		return ERR_NONE;
	}
	
	//!\brief Устанавливает последнее время выполнения задачи.
	Errors TaskImp::SetLastExecutionTime( time_t sec /*= 0*/,  int msec /*= 0*/ )
	{
		lastExeTime = sec;
		lastExemsec = msec;

		return ERR_NONE;
	}
	
	//!\brief Возвращает последнее время выполнения задачи.
	Errors TaskImp::GetLastExecutionTime( time_t& sec, int& msec ) const
	{
		sec = lastExeTime;
		msec = lastExemsec;
		if ( sec<0 ) sec = 0;
		if ( msec<0 ) msec = 0;

		return ERR_NONE;
	}
	
	//!\brief Возвращает следующеzе запланированное время выполнения задачи.
	Errors TaskImp::GetNextExecutionTime( time_t& sec, int& msec )
	{
		if ( schedule==NULL ) return ERR_SCHEDULE;	

		if ( nextExeTime==c_Undefined )
			CalculateNextExecutionTime();

		sec = nextExeTime;
		msec = nextExemsec;
		if ( sec<0 ) sec = 0;
		if ( msec<0 ) msec = 0;

		return ERR_NONE;
	}

	//!\brief Устанавливает флаг удаления задачи сразу после ее выполнения.
    Errors TaskImp::SetDelAfterRunFlag( bool delFlag )
	{
		delAfterRun = delFlag;
		return ERR_NONE;
	}


	//!\brief Возвращяет флаг удаления задачи сразу после ее выполнения.
    Errors TaskImp::GetDelAfterRunFlag( bool &delFlag ) const
	{
		delFlag = delAfterRun;
		return ERR_NONE;
	}

	//!\brief Устанавливате приоритет потока в котором должна выполняться задача.
	Errors TaskImp::SetTaskThreadPriority( TaskThreadPriority ttPriority )
	{
		taskThreadPriority = ttPriority;
		return ERR_NONE;
	}
	
	//!\brief Возвращает приоритет потока в котором должна выполняться задача.
	Errors TaskImp::GetTaskThreadPriority( TaskThreadPriority &ttPriority ) const
	{
		ttPriority = taskThreadPriority;
		return ERR_NONE;
	}
	
	//!\brief Возвращает контекст потока, в котором производиться выполнение задачи.
	Errors TaskImp::GetThreadContext( void **tCntx ) const
	{
		*tCntx = threadCntx;

		return ERR_NONE;
	}

	//!\brief Функция создания копии объекта
	void TaskImp::Clone( Task **ppTask ) const
	{
		*ppTask = KLSCH_CreateTask();

		*((TaskImp *)*ppTask) = *this;
	}
	
	//!\brief Возвращает указатель на расписание для запуска задачи для
	//		 дальнейшей модификации.
	Errors TaskImp::GetWritableSchedule( Schedule ** s)
	{
		*s = schedule;
		return ERR_NONE;
	}

	void TaskImp::CalculateNextExecutionTime( time_t currTime ) //!< Расчитывает следующие время запуска
	{
		int startDelta = 0;
		if ( currTime==0 ) time( &currTime );

		if ( delAfterRun )
		{
			time_t firstExeSec;	int firstExeMSec;
			bool runMissed;
			schedule->GetFirstExecutionTime( firstExeSec, firstExeMSec );
			schedule->GetRunMissedFlag( runMissed );

			if ( ( lastExeTime!=c_Undefined && lastExeTime!=0 && lastExeTime >= firstExeSec ) 
				|| ( firstExeSec < currTime && !runMissed ) )
			{
				nextExeTime = ULONG_MAX;	// задача RunOnce уже была запущенна или ее время уже истекло
				return;
			}
		}

		if ( startDeltaTimeout>0 ) 
		{
			startDelta = KLSTD_Random( 0, startDeltaTimeout );
		}
		
		//KLTRACE4( L"SCH", L"TaskImp::CalculateNextExecutionTime startDelta - %d startDeltaTimeout - %d\n", 
		//			startDelta, startDeltaTimeout );
		if ( preparativeStart && accessorySchedule!=NULL )
		{
			time_t saveNextExeTime = nextExeTime;			

			accessorySchedule->CalculateNextExecutionTime( nextExeTime, nextExemsec, currTime, 
				lastExeTime, lastExemsec, startDelta );
			if ( accessorySchedule!=NULL ) delete accessorySchedule;
			accessorySchedule = NULL;
			preparativeStart = false;

			// Делаем поправку времени запуска задачи если время запуска предварительной задачи
			// было позже чем точно по расписанию ( данный код сохраняет временный промежуток между
			// предварительным и основным запуском задачи во всех случаях )
			long prepSec = preparativeTimeout / c_MillisecondsInSecond;
			long delta;
			if ( (saveNextExeTime + prepSec) < currTime )
			{
				delta = prepSec;
			}
			else
			{
				delta = (saveNextExeTime + prepSec) - currTime;
			}
			nextExeTime = currTime + delta;

		}
		else
		{
			schedule->CalculateNextExecutionTime( nextExeTime, nextExemsec, currTime, 
				lastExeTime, lastExemsec, startDelta );
			if ( preparativeTimeout>0 ) preparativeStart = true;
		}
		
		if ( preparativeStart && nextExeTime==c_Undefined && accessorySchedule==NULL )
			preparativeStart = false;

		if ( preparativeStart )
		{
			// предварительный запуск ( корректируем время )
			long prepSec = preparativeTimeout / c_MillisecondsInSecond;
			time_t saveNextExeTime = nextExeTime;
			time_t saveNextExemsec = nextExemsec;

			if ( prepSec > nextExeTime ) nextExeTime = 0;
			else nextExeTime -= prepSec;

			if ( nextExeTime<currTime ) nextExeTime = currTime;

			// создаем вспомогательное расписание
			if ( accessorySchedule!=NULL ) delete accessorySchedule;
			accessorySchedule = NULL;
			accessorySchedule = KLSCH_CreateMillisecondsSchedule();
			accessorySchedule->SetPeriod(0);
			accessorySchedule->SetFirstExecutionTime( saveNextExeTime, saveNextExemsec );
		}

		Trace( 4, "Next execution time has been calculated. TaskId - %d nextExeTime - %d nextExemsec - %d lastExeTime - %d lastExeMSec - %d\n", 
			tId, nextExeTime, nextExemsec, lastExeTime, lastExemsec  );
	}

	/** Расчитывает следующие время запуска после Suspend состояния */
	void TaskImp::CalculateNextTimeAfterSuspend( )
	{
		bool runMiss = false;
		if ( schedule ) schedule->GetRunMissedFlag( runMiss );

		if ( !runMiss ) CalculateNextExecutionTime( ); // только для не runMissed задач
	}

	/** class helper functions */	
	void TaskImp::SetCurrState( State newState ) //!< Устанавливает текущее состояние задачи
	{		
		Trace( 4, "Task state has been changed. TaskId - %d OldState - '%s' NewState - '%s'\n", 
			tId, STATE_STR_DESC[state], STATE_STR_DESC[newState] );

		if ( state==Deleted ) return; // нельзя изменить статус задачи если она уже Deleted

		state = newState;

		if ( state==Suspended )
		{
			bool runMiss = false;
			if ( schedule ) schedule->GetRunMissedFlag( runMiss );

			// сбрасываем задачу в первоначальное состояние
			if ( !runMiss ) 
			{
				nextExeTime = c_Undefined;
				nextExemsec = c_Undefined;
				SetLastExecutionTime( c_Undefined, c_Undefined ); 			
			}
		}
	}
	
	void TaskImp::SetThreadContext( void *tCntx ) //!< Устанавливает контекст потока для задачи
	{
		threadCntx = tCntx;
	}

	void TaskImp::Call( TaskResults** result, bool &delTask )    //!< Вызывает callback задачи
	{
		TaskThreadPriority saveTp = taskThreadPriority;
		SchedulerThreadPriority threadPriority;

		if ( saveTp!=TP_Normal ) {
			// устанавливаем приоритет выполнения
			threadPriority.SetNewPriority( saveTp );
		}
		
		startSysTick = KLSTD::GetSysTickCount();

		if ( nextExeTime!=c_Undefined && !preparativeStart )
			lastExeTime = nextExeTime, lastExemsec = nextExemsec;

		if ( func2!=NULL )
		{
			TaskOptions to(preparativeStart,preparativeTimeout);
			func2( params, &to, result );
		}
		else
		{
			func( params, result );
		}

		if ( !preparativeStart ) delTask = delAfterRun;

		if ( saveTp!=TP_Normal ) {
			// востанавливаем приоритет потока
			threadPriority.RestorePrioroty();
		}

		Trace( 4, "Call task. TaskId - %d lastExeTime - %d preparativeStart - %d\n", tId, preparativeStart, 
			lastExeTime );

		startSysTick = 0;
	}

	bool TaskImp::IsTaskHanging() const		//!< Определяет является ли данная задача завишей
	{
		if ( state!=Running || startSysTick==0 ) return false;

		if ( maxExecuteTime==c_Infinite ) return false;

		if ( KLSTD::DiffTickCount(KLSTD::GetSysTickCount(),startSysTick)>(unsigned)maxExecuteTime ) return true;
		else return false;
	}
	
}	// end namespace KLSCH

/*
	\brief Метод для создания объекта Task
*/
KLSCH::Task *KLSCH_CreateTask()
{
	return new KLSCH::TaskImp();
}

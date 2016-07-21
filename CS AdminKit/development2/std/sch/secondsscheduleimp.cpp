/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file SecondsScheduleImp.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализацией класса расписание SecondsScheduleImp для задачи 
 *
 */

#include "std/trc/trace.h"

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "secondsscheduleimp.h"

namespace KLSCH {

	const long c_day96Time = 345600000; // 96 часов 1000*60*60*96

	//!\brief Конструтор.
	SecondsScheduleImp::SecondsScheduleImp()
	{
		period = 0;
		periodCounter = c_Undefined;
	}

	//!\brief Конструтор копирования.	
	SecondsScheduleImp::SecondsScheduleImp( const SecondsScheduleImp &schedule )
	{
		firstStart		= schedule.firstStart;
		period			= schedule.period;
		periodCounter	= schedule.periodCounter;
	}
	
	//!\brief Устанавливает период выполнения задачи.
	Errors SecondsScheduleImp::SetPeriod( int sec /* = 0 */ )
	{
		period = sec;
		return ERR_NONE;
	}
	
	//!\brief Возвращает период выполнения задачи.
	Errors SecondsScheduleImp::GetPeriod( int& sec ) const		
	{
		sec = period;
		return ERR_NONE;
	}

	//!\brief Содает копию объекта.
	Errors SecondsScheduleImp::Clone( Schedule **newCopy )
	{
		*newCopy = new SecondsScheduleImp( *this );
		(*newCopy)->CopyFrom( this ); // копирование базового класса
		return ERR_NONE;
	}
	
	//!\brief Фунция расчета времени следущего запуска.		
	Errors SecondsScheduleImp::CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
		time_t lastExeSec, int lastExeMsec, int startDelta )
	{
		if ( period<0 ) return ERR_SCHEDULE;

		//nextTime = currTime;		
		//msec = period;
		if ( periodCounter==c_Undefined || lastExeSec==c_Undefined )
		{	
			time_t executionTimeBeforeCurrentSec;
			time_t executionTimeAfterCurrentSec;

			GetExecutionTimes( currTime, executionTimeBeforeCurrentSec,
				executionTimeAfterCurrentSec, lastExeSec, lastExeMsec );

			if ( (lastExeSec!=c_Undefined && lastExeSec!=0) && lastExeSec<currTime && runMissed &&
				 executionTimeBeforeCurrentSec>lastExeSec ) // запуск пропущенной задачи
			{
				firstStart = executionTimeBeforeCurrentSec;	// запускаем одну пропущеную задачу								
			}
			else
			{
				if ( ( firstExeTime!=c_Undefined && firstExeTime!=0 ) &&  
					executionTimeBeforeCurrentSec==firstExeTime && ( lastExeSec==c_Undefined || lastExeSec==0 ) )
					firstStart = executionTimeBeforeCurrentSec;	
				else
					firstStart = executionTimeAfterCurrentSec;				
			}
		}
		else
			periodCounter += period;
		
		nextTime	= firstStart + periodCounter;
		msec		= startDelta;
		
		int compareDelta = 30;	// 30 secs
		if ( period > compareDelta ) compareDelta = period;

		if ( ( CompareTimes( currTime, 0, nextTime + compareDelta, 0 ) > 0  // пропущено более одного запуска
			 || ( lastExeSec!=c_Undefined && lastExeSec > currTime ) ) // время сильно ушло назад
			 && period!=0 )
		{
			periodCounter = c_Undefined;
			if ( lastExeSec > currTime ) lastExeSec = lastExeMsec = c_Undefined;

			return CalculateNextExecutionTime( nextTime, msec, currTime,
				lastExeSec, lastExeMsec, startDelta );
		}

		{
			// trace info
			struct tm infoT;
			KL_LOCALTIME( nextTime, infoT );
			char timeStr[128];
			strftime( timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", &infoT );

			if ( lastExeSec!=c_Undefined )
			{	
				KL_LOCALTIME( lastExeSec, infoT );
				char timeStrLast[128];
				strftime( timeStrLast, sizeof(timeStrLast), "%a %b %d %H:%M:%S %Y", &infoT );
				KLTRACE3( L"SCH", L"SecondsSchedulerImp::CalculateNextExecutionTime nextTime - %hs nextTime - %d period - %d period counter - %d lastExeSec - %hs\n", 
					timeStr, nextTime, period, periodCounter, timeStrLast );
			}
		}

		return ERR_NONE;
	}

	void SecondsScheduleImp::GetExecutionTimes( time_t currTime, time_t &executionTimeBeforeCurrentSec,			
			time_t &executionTimeAfterCurrentSec, time_t lastExeSec, int lastExeMsec )
	{
		periodCounter = 0;

		// производим расчет ближайщего времени запуска периодической задачи до текущего и после
		// для расчета используем установленное время первого запуска задачи
		if ( ( firstExeTime!=c_Undefined && firstExeTime!=0 ) || 
			 ( lastExeSec!=c_Undefined && lastExeSec!=0 ) )
		{
			time_t fTime = firstExeTime;

			if ( lastExeSec!=c_Undefined && lastExeSec!=0  )
			{
				executionTimeBeforeCurrentSec = lastExeSec,
				executionTimeAfterCurrentSec = lastExeSec;
			}
			else
			{
				executionTimeBeforeCurrentSec = firstExeTime,
				executionTimeAfterCurrentSec = firstExeTime;
			}			

			if ( fTime>currTime ) return;
			if ( period==0 )
			{
				if ( executionTimeBeforeCurrentSec==c_Undefined 
					|| executionTimeBeforeCurrentSec==0 )
				{
					executionTimeBeforeCurrentSec = currTime;
					executionTimeAfterCurrentSec = currTime + period;
				}
				return;
			}

			periodCounter = period;

			while( executionTimeAfterCurrentSec <= currTime )
			{
				executionTimeBeforeCurrentSec = executionTimeAfterCurrentSec;				

				executionTimeAfterCurrentSec = fTime + periodCounter;
				periodCounter += period;
			}

			periodCounter = 0;
		}
		else
		{
			executionTimeBeforeCurrentSec = currTime;
			executionTimeAfterCurrentSec = currTime + period;

			return;
		}	
	}
	
}	// end namespace KLSCH

/*
	\brief Метод для создания объекта SecondsSchedule
*/
KLSCH::SecondsSchedule *KLSCH_CreateSecondsSchedule()
{
	return new KLSCH::SecondsScheduleImp;
}

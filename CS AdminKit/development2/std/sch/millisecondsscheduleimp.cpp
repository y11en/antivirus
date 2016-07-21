/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file MillisecondsScheduleImp.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализацией класса расписание MillisecondsScheduleImp для задачи 
 *
 */

#include "std/trc/trace.h"

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "millisecondsscheduleimp.h"

namespace KLSCH {

	const long c_day96Time = 345600000; // 96  часов  1000*60*60*96

	//!\brief Конструтор.
	MillisecondsScheduleImp::MillisecondsScheduleImp()
	{
		period = 0;
		periodCounter = c_Undefined;
	}

	//!\brief Конструтор копирования.	
	MillisecondsScheduleImp::MillisecondsScheduleImp( const MillisecondsScheduleImp &schedule )
	{
		firstStart		= schedule.firstStart;
		period			= schedule.period;
		periodCounter	= schedule.periodCounter;
	}
	
	//!\brief Устанавливает период выполнения задачи.
	Errors MillisecondsScheduleImp::SetPeriod( int msec /* = 0 */ )
	{
		period = msec;
		return ERR_NONE;
	}
	
	//!\brief Возвращает период выполнения задачи.
	Errors MillisecondsScheduleImp::GetPeriod( int& msec ) const		
	{
		msec = period;
		return ERR_NONE;
	}

	//!\brief Содает копию объекта.
	Errors MillisecondsScheduleImp::Clone( Schedule **newCopy )
	{
		*newCopy = new MillisecondsScheduleImp( *this );
		(*newCopy)->CopyFrom( this ); // копирование базового класса
		return ERR_NONE;
	}
	
	//!\brief Фунция расчета времени следущего запуска.		
	Errors MillisecondsScheduleImp::CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
		time_t lastExeSec, int lastExeMsec, int startDelta )
	{
		if ( period<0 ) return ERR_SCHEDULE;

		//nextTime = currTime;		
		//msec = period;
		if ( periodCounter==c_Undefined || lastExeSec==c_Undefined )
		{	
			time_t executionTimeBeforeCurrentSec; int beforeMsec; 
			time_t executionTimeAfterCurrentSec; int AfterMsec;

			GetExecutionTimes( currTime, executionTimeBeforeCurrentSec, beforeMsec,
				executionTimeAfterCurrentSec, AfterMsec, lastExeSec, lastExeMsec );

			if ( lastExeSec!=c_Undefined && lastExeSec<currTime && runMissed &&
				CompareTimes( executionTimeBeforeCurrentSec,  beforeMsec, lastExeSec, lastExeMsec ) > 0 ) // запуск пропущенной задачи
			{
				firstStart = executionTimeBeforeCurrentSec;	// запускаем одну пропущеную задачу				
				periodCounter = beforeMsec;
			}
			else
			{
				firstStart = executionTimeAfterCurrentSec;
				periodCounter = AfterMsec;
			}
		}
		else
			periodCounter += period;

		if ( startDelta>0 && period!=0 ) {
			if ( startDelta>period ) startDelta = startDelta % period;			
		}
		
		msec		= periodCounter + startDelta; 
		nextTime	= firstStart;		
		
		if ( periodCounter>c_day96Time ) // четверо суток
		{
			// сбрасываем periodCounter каждые четверо суток
			firstStart += periodCounter / c_MillisecondsInSecond;
			periodCounter = periodCounter % c_MillisecondsInSecond;
		}
		
		int compareDelta = 30000;	// 30 secs
		if ( period > compareDelta ) compareDelta = period;

		if ( ( CompareTimes( currTime, 0, nextTime, ( msec + compareDelta ) ) > 0  // пропущено более одного запуска
			 || ( lastExeSec!=c_Undefined && lastExeSec > currTime ) ) // время сильно ушло назад
			 && period!=0 )
		{
			periodCounter = c_Undefined;
			if ( lastExeSec > currTime ) lastExeSec = lastExeMsec = c_Undefined;

			return CalculateNextExecutionTime( nextTime, msec, currTime,
				lastExeSec, lastExeMsec, startDelta );
		}

		if ( nextTime>=0 )
		{
			// trace info
			struct tm infoT;
			KL_LOCALTIME( nextTime, infoT );
			char timeStr[128];
			strftime( timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", &infoT );

			KLTRACE3( L"SCH", L"MillisecondsScheduleImp::CalculateNextExecutionTime nextTime - %hs nextTime - %d period - %d period counter - %d\n", 
					timeStr, nextTime, period, periodCounter );
		}

		return ERR_NONE;
	}

	void MillisecondsScheduleImp::GetExecutionTimes( time_t currTime, time_t &executionTimeBeforeCurrentSec,
			int &beforeMsec, 
			time_t &executionTimeAfterCurrentSec,
			int &AfterMsec, 
			time_t lastExeSec, int lastExeMsec )
	{
		periodCounter = 0;

		// производим расчет ближайщего времени запуска периодической задачи до текущего и после
		// для расчета используем установленное время первого запуска задачи
		if ( ( firstExeTime!=c_Undefined && firstExemsec!=c_Undefined && firstExeTime!=0 ) || 
			 ( lastExeSec!=c_Undefined && lastExeSec!=0 ) )
		{
			if ( lastExeSec!=c_Undefined && lastExeSec!=0  )
			{
				executionTimeBeforeCurrentSec = lastExeSec;
				beforeMsec = lastExeMsec;

				executionTimeAfterCurrentSec = lastExeSec;
				AfterMsec = lastExeMsec;				
			}
			else
			{
				executionTimeBeforeCurrentSec = firstExeTime;
				beforeMsec = firstExemsec;

				executionTimeAfterCurrentSec = firstExeTime;
				AfterMsec = firstExemsec;				
			}

			periodCounter = period;

			if ( (firstExeTime!=c_Undefined && firstExeTime>currTime) || period==0 ) return;

			int currnetDeltaMsec = AfterMsec;
			while( CompareTimes( executionTimeAfterCurrentSec,  AfterMsec, currTime, currnetDeltaMsec ) <= 0 )
			{
				executionTimeBeforeCurrentSec = executionTimeAfterCurrentSec;
				beforeMsec = AfterMsec;

				periodCounter += period;
				AfterMsec = periodCounter;

				if ( periodCounter>c_day96Time ) // четверо суток
				{
					// сбрасываем periodCounter каждые четверо суток
					executionTimeAfterCurrentSec += periodCounter / c_MillisecondsInSecond;
					executionTimeBeforeCurrentSec += beforeMsec / c_MillisecondsInSecond;
					periodCounter = periodCounter % c_MillisecondsInSecond;
					AfterMsec = periodCounter;
					beforeMsec = beforeMsec % c_MillisecondsInSecond;
				}
			}

			periodCounter = 0;
		}
		else
		{
			executionTimeBeforeCurrentSec = currTime;
			beforeMsec = 0;

			executionTimeAfterCurrentSec = currTime;
			AfterMsec = period;

			periodCounter = period;

			return;
		}	
	}
	
}	// end namespace KLSCH

/*
	\brief Метод для создания объекта MillisecondsSchedule
*/
KLSCH::MillisecondsSchedule *KLSCH_CreateMillisecondsSchedule()
{
	return new KLSCH::MillisecondsScheduleImp;
}

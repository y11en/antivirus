/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EveryMonthScheduleImp.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализации класса расписания задачи 
 *        для запуска модулем-планировщиком еженедельно.
 *
 */

#include "std/trc/trace.h"

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "private.h"

#include "everymonthscheduleimp.h"

namespace KLSCH {

	//!\brief Конструтор.
	EveryMonthScheduleImp::EveryMonthScheduleImp()
	{
		day		= c_Undefined;
		hours	= c_Undefined;
		min     = c_Undefined;
		sec     = c_Undefined;

		monthsPeriod = 1;
	}

	//!\brief Конструтор копирования.	
	EveryMonthScheduleImp::EveryMonthScheduleImp( const EveryMonthScheduleImp &schedule )
	{
		day		= schedule.day;
		hours	= schedule.hours;
		min		= schedule.min;
		sec		= schedule.sec;
		monthsPeriod = schedule.monthsPeriod;
	}
	
	//!\brief Устанавливает период выполнения задачи.
	Errors EveryMonthScheduleImp::SetExecutionTime( int d, int h, int m, int s )
	{
		if ( d<1 || d>31 )	return ERR_TIME_INTERVAL;
		if ( h<0 || h>23 )	return ERR_TIME_INTERVAL;
		if ( m<0 || m>59 )	return ERR_TIME_INTERVAL;
		if ( s<0 || s>59 )	return ERR_TIME_INTERVAL;

		day		= d;
		hours   = h;
		min     = m;
		sec     = s;

		return ERR_NONE;
	}
	
	//!\brief Возвращает период выполнения задачи.
	Errors EveryMonthScheduleImp::GetExecutionTime( int& d, int& h, int& m, int& s ) const
	{
		if ( day==c_Undefined ) return ERR_SCHEDULE;
		
		d		= day;
		h		= hours;
		m		= min;
		s		= sec;

		return ERR_NONE;
	}
	
	//!\brief Устанавливает период выполнения задачи в месяцах
	Errors EveryMonthScheduleImp::SetPeriod( int months )
	{
		if ( months<=0 ) return ERR_SCHEDULE;

		monthsPeriod = months;

		return ERR_NONE;
	}

    //!\brief Возвращает период выполнения задачи в месяцах
    Errors EveryMonthScheduleImp::GetPeriod( int& months ) const
	{
		months = monthsPeriod;

		return ERR_NONE;
	}

	//!\brief Содает копию объекта.
	Errors EveryMonthScheduleImp::Clone( Schedule **newCopy )
	{
		*newCopy = new EveryMonthScheduleImp( *this );
		(*newCopy)->CopyFrom( this ); // копирование базового класса
		return ERR_NONE;
	}

	
	//!\brief Фунция расчета времени следущего запуска.		
	Errors EveryMonthScheduleImp::CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
		time_t lastExeSec, int lastExeMsec, int startDelta )
	{
		const long c_day24Time = 86400000; // 24 часа 1000*60*60*24
		if ( day==c_Undefined ) return ERR_SCHEDULE;

		struct tm startTm;
		
		if ( lastExeSec>=0 && runMissed )
		{
			// используем предедущее время запуска
			KL_LOCALTIME( lastExeSec, startTm );
		}			
		else
			// используем текущее время для расчета
			KL_LOCALTIME( currTime, startTm );			

		if ( startDelta>0 ) {
			if ( startDelta>c_day24Time ) startDelta = startDelta % c_day24Time;
		}

		int daysInMonth = GetDaysInMonth( startTm );

		bool find = false;
		time_t saveNextTime = nextTime;
		while ( !find )
		{
			startTm.tm_mday	= day;
			// корректируем дату запуска
			if ( startTm.tm_mday>daysInMonth ) startTm.tm_mday = daysInMonth;
			
			// избегаем переводов на лет/зим время
			mktime( &startTm );

			startTm.tm_hour = hours;
			startTm.tm_min  = min;
			startTm.tm_sec  = sec;
			
			nextTime = mktime( &startTm );
			if ( nextTime==(-1) ) return ERR_SCHEDULE;
			
			msec = startDelta;

			if ( !find )
			{
				if ( nextTime>=currTime && ( lastExeSec==c_Undefined || nextTime>lastExeSec ) ) 				
				{
					if ( lastExeSec != c_Undefined && lastExeSec != 0 && runMissed &&
						lastExeSec >= firstExeTime )
					{
						if ( saveNextTime>lastExeSec && saveNextTime<=currTime ) 
							nextTime = saveNextTime;
					}
					
					find = true;
				}
				
				// проверяем условние времени первого запуска задачи
				if ( firstExeTime != c_Undefined && nextTime<firstExeTime && nextTime!=(-1) )
					find = false;

				if ( !find )
				{					
					startTm.tm_mon += monthsPeriod;  // сдвигаем еще на период вперед
					if ( startTm.tm_mon>11 )
						startTm.tm_year += startTm.tm_mon / 12, startTm.tm_mon = startTm.tm_mon % 12;

					daysInMonth = GetDaysInMonth( startTm );
				}
			}

			saveNextTime = nextTime;
		}

		if ( lifeTime!=c_Undefined && nextTime > lifeTime ) return ERR_SCHEDULE_EXPIRED;
		{
			// trace info
			struct tm infoT;
			KL_LOCALTIME( nextTime, infoT );
			char timeStr[128];
			strftime( timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", &infoT );
			KLTRACE3( L"SCH", L"EveryWeekSchedule::CalculateNextExecutionTime nextTime - %hs msec - %d\n", timeStr, msec );
		}

		return ERR_NONE;	
	}

	int EveryMonthScheduleImp::GetDaysInMonth( struct tm &monthTm )
	{
		struct tm currentMonthTm, nextMonthTm;		
		currentMonthTm = monthTm;
		nextMonthTm = monthTm;		
		nextMonthTm.tm_mon++;

		time_t currentMonthTime = mktime( &currentMonthTm );
		time_t nextMonthTime = mktime( &nextMonthTm );

		time_t diffMonthTime = nextMonthTime - currentMonthTime;

		struct tm diffTm;
		KL_LOCALTIME( diffMonthTime, diffTm );
		int days = diffTm.tm_yday;

		return days;
	}
	

}	// end namespace KLSCH

/*
	\brief Метод для создания объекта MillisecondsSchedule
*/
KLSCH::EveryMonthSchedule *KLSCH_CreateEveryMonthSchedule()
{
	return new KLSCH::EveryMonthScheduleImp;
}

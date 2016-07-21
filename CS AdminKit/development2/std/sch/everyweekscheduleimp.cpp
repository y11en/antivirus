/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EveryWeekScheduleImp.cpp
 * \author ƒамир Ўи€фетдинов
 * \date 2002
 * \brief ‘айл с реализации класса расписани€ задачи 
 *        дл€ запуска модулем-планировщиком еженедельно.
 *
 */

#include "std/trc/trace.h"

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "private.h"

#include "everyweekscheduleimp.h"

namespace KLSCH {

	//!\brief  онструтор.
	EveryWeekScheduleImp::EveryWeekScheduleImp()
	{
		day		= c_Undefined;
		hours	= c_Undefined;
		min     = c_Undefined;
		sec     = c_Undefined;
		weeksPeriod = 1;
	}

	//!\brief  онструтор копировани€.	
	EveryWeekScheduleImp::EveryWeekScheduleImp( const EveryWeekScheduleImp &schedule )
	{
		day		= schedule.day;
		hours	= schedule.hours;
		min		= schedule.min;
		sec		= schedule.sec;
		weeksPeriod = schedule.weeksPeriod;
	}
	
	//!\brief ”станавливает период выполнени€ задачи.
	Errors EveryWeekScheduleImp::SetExecutionTime( int d, int h, int m, int s )
	{
		if ( d<0 || d>6 )	return ERR_TIME_INTERVAL;
		if ( h<0 || h>23 )	return ERR_TIME_INTERVAL;
		if ( m<0 || m>59 )	return ERR_TIME_INTERVAL;
		if ( s<0 || s>59 )	return ERR_TIME_INTERVAL;

		day		= d;
		hours   = h;
		min     = m;
		sec     = s;

		return ERR_NONE;
	}
	
	//!\brief ¬озвращает период выполнени€ задачи.
	Errors EveryWeekScheduleImp::GetExecutionTime( int& d, int& h, int& m, int& s ) const
	{
		if ( day==c_Undefined ) return ERR_SCHEDULE;
		
		d		= day;
		h		= hours;
		m		= min;
		s		= sec;

		return ERR_NONE;
	}
	
	//!\brief ”станавливает период выполнени€ задачи в недел€х
	Errors EveryWeekScheduleImp::SetPeriod( int weeks )
	{
		if ( weeks<=0 ) return ERR_SCHEDULE;

		weeksPeriod = weeks;

		return ERR_NONE;
	}

    //!\brief ¬озвращает период выполнени€ задачи в недел€х
    Errors EveryWeekScheduleImp::GetPeriod( int& weeks ) const
	{
		weeks = weeksPeriod;

		return ERR_NONE;
	}


	//!\brief —одает копию объекта.
	Errors EveryWeekScheduleImp::Clone( Schedule **newCopy )
	{
		*newCopy = new EveryWeekScheduleImp( *this );
		(*newCopy)->CopyFrom( this ); // копирование базового класса
		return ERR_NONE;
	}
	
	//!\brief ‘унци€ расчета времени следущего запуска.		
	Errors EveryWeekScheduleImp::CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
		time_t lastExeSec, int lastExeMsec, int startDelta )
	{
		const long c_day24Time = 86400000; // 24 часа 1000*60*60*24
		if ( day==c_Undefined ) return ERR_SCHEDULE;

		struct tm startTm;		
		
		if ( lastExeSec != c_Undefined && lastExeSec != 0 && runMissed )
		{
			// используем предедущее врем€ запуска				
			KL_LOCALTIME( lastExeSec, startTm );	
		}			
		else
			// используем текущее врем€ дл€ расчета
			KL_LOCALTIME( currTime, startTm );

		// расчитываем разницу между дн€ми недели
		int dayDelta = day - startTm.tm_wday;

		if ( startDelta>0 ) {
			if ( startDelta>c_day24Time ) startDelta = startDelta % c_day24Time;
		}

		bool find = false;
		time_t saveNextTime = nextTime;
		while ( !find )
		{
			// избегаем переводов на лет/зим врем€
			mktime( &startTm );
			
			startTm.tm_mday	+= dayDelta;
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

				// провер€ем условние времени первого запуска задачи
				if ( firstExeTime != c_Undefined && nextTime<firstExeTime && nextTime!=(-1) )
					find = false;

				if ( !find )
				{
					startTm.tm_mday -= dayDelta;
					startTm.tm_mday += 7 * weeksPeriod; // сдвигаем еще на период вперед
				}
			}

			saveNextTime = nextTime;
		}

		/*// возможно врем€ уже прошло ( также сдвинет на одину неделю перед вызовом предыдущей задачи )
		{
			long schTime = day * 1000000 + hours * 10000 + min * 100 + sec;
			long lastTime = startTm.tm_wday * 1000000 + startTm.tm_hour * 10000 + startTm.tm_min * 100 + startTm.tm_sec;
			
			if ( schTime<=lastTime ) dayDelta += 7; // сдвинем врем€ запуска на неделю вперед

			if ( runMissed && lastExeSec==0 ) // значение 0 используетс€ только дл€ запуска одной пропушенной задачи 
											  // ( например задача в начале недели )
				startTm = *localtime( &currTime ); // получаем текущее врем€
		}
		
		bool find = false;
		while ( !find )
		{

			startTm.tm_mday	+= dayDelta;
			startTm.tm_hour = hours;
			startTm.tm_min  = min;
			startTm.tm_sec  = sec;
			
			nextTime = mktime( &startTm );
			msec = startDelta;
			find = true;

			// провер€ем условние времени первого запуска задачи
			if ( firstExeTime>=0 && nextTime<firstExeTime && nextTime!=(-1) )
			{
				find = false;
				startTm.tm_mday -= dayDelta;
				startTm.tm_mday += 7; // сдвигаем еще на неделю вперед
			}
		};

		if ( nextTime==(-1) ) return ERR_SCHEDULE;*/

		if ( lifeTime != c_Undefined && lifeTime != 0 && nextTime > lifeTime ) return ERR_SCHEDULE_EXPIRED;

		{
			// trace info
			struct tm infoT;
			KL_LOCALTIME( nextTime, infoT );
			char timeStr[128];
			strftime( timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", &infoT );
			KLTRACE3( L"SCH", L"EveryWeekSchedule::CalculateNextExecutionTime nextTime - %hs nextTime - %d msec - %d\n", timeStr, nextTime, msec );
		}

		return ERR_NONE;
	}
	
}	// end namespace KLSCH

/*
	\brief ћетод дл€ создани€ объекта MillisecondsSchedule
*/
KLSCH::EveryWeekSchedule *KLSCH_CreateEveryWeekSchedule()
{
	return new KLSCH::EveryWeekScheduleImp;
}

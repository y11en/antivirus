/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file weeksdaysscheduleImp.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализации класса расписания задачи 
 *        для запуска модулем-планировщиком в определенные дни недели..
 *
 */

#include "std/trc/trace.h"

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "private.h"

#include "weeksdaysscheduleimp.h"

namespace KLSCH {

	//!\brief Конструтор.
	WeeksDaysScheduleImp::WeeksDaysScheduleImp()
	{
		hours	= c_Undefined;
		min     = c_Undefined;
		sec     = c_Undefined;		
		weeksPeriod = 1;
		daysBitMask = c_Undefined;
	}

	//!\brief Конструтор копирования.	
	WeeksDaysScheduleImp::WeeksDaysScheduleImp( const WeeksDaysScheduleImp &schedule )
	{
		days	= schedule.days;
		hours	= schedule.hours;
		min		= schedule.min;
		sec		= schedule.sec;		
		weeksPeriod = schedule.weeksPeriod;
		daysBitMask = schedule.daysBitMask;
	}
	
	//!\brief Устанавливает период выполнения задачи.
	Errors WeeksDaysScheduleImp::SetExecutionTime( int h, int m, int s,
								 int daysMask )
	{	
		if ( h<0 || h>23 )	return ERR_TIME_INTERVAL;
		if ( m<0 || m>59 )	return ERR_TIME_INTERVAL;
		if ( s<0 || s>59 )	return ERR_TIME_INTERVAL;

		hours   = h;
		min     = m;
		sec     = s;		
		daysBitMask = daysMask;

		days.clear();
		for( int i = 0; i < 7; i++ )
		{
			bool setBitFlag = ((daysMask & 0x1)!=0);
			daysMask = daysMask >> 1;

			if ( setBitFlag ) days.push_back( i );
		}

		return ERR_NONE;
	}

	//!\brief Возвращает период выполнения задачи.
	Errors WeeksDaysScheduleImp::GetExecutionTime( int& h, int& m, int& s,
			int &daysMask ) const
	{
		h		= hours;
		m		= min;
		s		= sec;
		daysMask = daysBitMask;

		return ERR_NONE;
	}

	//!\brief Устанавливает период выполнения задачи в неделях
	Errors WeeksDaysScheduleImp::SetPeriod( int weeks )
	{
		if ( weeks<=0 ) return ERR_SCHEDULE;

		weeksPeriod = weeks;

		return ERR_NONE;
	}

    //!\brief Возвращает период выполнения задачи в неделях
    Errors WeeksDaysScheduleImp::GetPeriod( int& weeks ) const
	{
		weeks = weeksPeriod;

		return ERR_NONE;
	}

	//!\brief Содает копию объекта.
	Errors WeeksDaysScheduleImp::Clone( Schedule **newCopy )
	{
		*newCopy = new WeeksDaysScheduleImp( *this );
		(*newCopy)->CopyFrom( this ); // копирование базового класса
		return ERR_NONE;
	}
	
	//!\brief Фунция расчета времени следущего запуска.		
	Errors WeeksDaysScheduleImp::CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
		time_t lastExeSec, int lastExeMsec, int startDelta )
	{
		if ( days.size()==0 ) return ERR_SCHEDULE;

		const long c_day24Time = 86400000; // 24 часа 1000*60*60*24		

		struct tm startTm;		
		
/*		if ( lastExeSec != c_Undefined && lastExeSec != 0 && runMissed )
		{
			// используем предедущее время запуска				
			KL_LOCALTIME( lastExeSec, startTm );	
		}			
		else
*/
		// используем текущее время для расчета
		KL_LOCALTIME( currTime, startTm );

		// расчитываем разницу между днями недели
		int prevDayDelta = -(INT_MAX), nextDayDelta = INT_MAX;
		std::vector<int>::iterator it = days.begin();
		for( ; it != days.end(); ++it )
		{
			int dayDelta = (*it) - startTm.tm_wday;

			if ( dayDelta==0 ) 
			{
				bool bPrevDayFlag = false;
				if ( hours<startTm.tm_hour ) bPrevDayFlag = true;
				else if ( hours==startTm.tm_hour && min<startTm.tm_min) bPrevDayFlag = true;
				else if ( hours==startTm.tm_hour && min==startTm.tm_min && sec<startTm.tm_sec) bPrevDayFlag = true;

				if ( bPrevDayFlag ) prevDayDelta = 0;
				else nextDayDelta = 0;
			}
			else
			{
				if ( dayDelta<0 && dayDelta>prevDayDelta ) prevDayDelta = dayDelta;
				else if ( dayDelta<nextDayDelta ) nextDayDelta = dayDelta;
			}

		}

		if ( nextDayDelta==INT_MAX ) nextDayDelta = prevDayDelta, prevDayDelta = -(INT_MAX);

		//int dayDelta = day - startTm.tm_wday;

		if ( startDelta>0 ) {
			if ( startDelta>c_day24Time ) startDelta = startDelta % c_day24Time;
		}

		bool find = false;
		time_t saveNextTime = nextTime;		
		time_t savePrevNextTime = ULONG_MAX;		
		time_t nextPrevTime;

		struct tm startPrevTm;
		memcpy( &startPrevTm, &startTm, sizeof(startTm) );

		while ( !find )
		{
			// избегаем переводов на лет/зим время
			mktime( &startTm );
			
			startTm.tm_mday	+= nextDayDelta;
			startTm.tm_hour = hours, startTm.tm_min  = min, startTm.tm_sec  = sec;
			
			nextTime = mktime( &startTm );
			if ( nextTime==(-1) ) return ERR_SCHEDULE;

			if ( prevDayDelta != -(INT_MAX) )
			{
				mktime( &startPrevTm );
			
				startPrevTm.tm_mday	+= prevDayDelta;
				startPrevTm.tm_hour = hours, startPrevTm.tm_min  = min, startPrevTm.tm_sec  = sec;
			
				nextPrevTime = mktime( &startPrevTm );
				if ( nextPrevTime==(-1) ) return ERR_SCHEDULE;
			}
			
			msec = startDelta;

			if ( !find )
			{
				if ( nextTime>=currTime && ( lastExeSec==c_Undefined || nextTime>lastExeSec ) ) 
				{
					if ( runMissed && prevDayDelta != -(INT_MAX) && nextPrevTime<=currTime )
					{
						if ( lastExeSec != c_Undefined && lastExeSec != 0 &&
							lastExeSec >= firstExeTime && nextPrevTime>lastExeSec )
						{
							nextTime = nextPrevTime;
						}
						else
						{
							if ( lastExeSec==c_Undefined || lastExeSec == 0 ) {
								nextTime = nextPrevTime;
							}
						}
					}

					
					find = true;
				}

				// проверяем условние времени первого запуска задачи
				if ( firstExeTime != c_Undefined && nextTime<firstExeTime && nextTime!=(-1) )
					find = false;

				if ( !find )
				{
					startTm.tm_mday -= nextDayDelta;
					startTm.tm_mday += 7 * weeksPeriod; // сдвигаем еще на период вперед

					startPrevTm.tm_mday -= prevDayDelta;
					startPrevTm.tm_mday += 7 * weeksPeriod; // сдвигаем еще на период вперед					
				}
			}

			saveNextTime = nextTime;
		}

		if ( lifeTime != c_Undefined && lifeTime != 0 && nextTime > lifeTime ) return ERR_SCHEDULE_EXPIRED;

		{
			// trace info
			struct tm infoT;
			KL_LOCALTIME( nextTime, infoT );
			char timeStr[128];
			strftime( timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", &infoT );
			KLTRACE3( L"SCH", L"WeeksDaysSchedule::CalculateNextExecutionTime nextTime - %hs nextTime - %d msec - %d\n", timeStr, nextTime, msec );
		}

		return ERR_NONE;
	}
	
}	// end namespace KLSCH

/*
	\brief Метод для создания объекта WeeksDaysSchedule
*/
KLSCH::WeeksDaysSchedule *KLSCH_CreateWeeksDaysSchedule()
{
	return new KLSCH::WeeksDaysScheduleImp;
}

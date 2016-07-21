 /*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Common.cpp
 * \author ƒамир Ўи€фетдинов
 * \date 2002
 * \brief –елаизаиц€ общих функций дл€ модул€ Scheduler.
 *
 */

#include <ctime>

#include "std/sch/common.h"

namespace KLSCH {

	//!\brief ‘ункци€ дл€ сравени€ time_t + msec.
	int CompareTimes( time_t t1sec, int t1msec, time_t t2sec, int t2msec )
	{
		if ( t1msec==c_Undefined ) return 1;
		if ( t1sec==t2sec ) 
		{			
			return (t1msec - t2msec);
		}
		else 
		{
			if ( t1msec>=c_MillisecondsInSecond )			
				t1sec += t1msec / c_MillisecondsInSecond, t1msec %= c_MillisecondsInSecond;
			
			if ( t2msec>=c_MillisecondsInSecond )			
				t2sec += t2msec / c_MillisecondsInSecond, t2msec %= c_MillisecondsInSecond;
			
			return (t1sec - t2sec);
		}
	}

	//!\brief ‘ункци€ дл€ расчета времени ожидани€ ( в милисекундах ) до указанного времени.
	unsigned long CalculateWaitingTime( time_t tsec, int tmsec, time_t currTime,
		int currTimeMSec )
	{
		if ( tsec<0 )  return (unsigned long)c_Infinite;
		if ( tmsec<0 ) return (unsigned long)c_Infinite;

		int sec = tsec;
		int res = 0;

		if ( tmsec>=c_MillisecondsInSecond )
		{
			sec += tmsec / c_MillisecondsInSecond;
			tmsec %= c_MillisecondsInSecond;
		}
		sec -= currTime;
		if ( sec<0 ) res = 0;
		else
		{
			res = ((sec * c_MillisecondsInSecond) + tmsec) - currTimeMSec + 10; // 10 Msec дл€ того что бы точно перескочить нужную секунду
			if ( res<0 ) res = 0;
		}

		return res;
	}

} // nasmesapce KLSCH

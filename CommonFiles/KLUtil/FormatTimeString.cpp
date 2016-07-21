#include "stdafx.h"
#include "inl/tmutil.h"
#include "FormatTimeString.h"

namespace KLUTIL
{

int GetHumanDaysBetween(const CSysTime& tEventPrev, const CSysTime& tEventNext)
{
	/* Старый алгоритм (выдаст между вчера 19:00 и сегодня 20:00 -> 2 дня, а не 1!)
	int nDaysAgo = (tEventNext - tEventPrev).GetFullDays();
	
	// adding "human" taste
	bool bEventInThePast = tEventNext > tEventPrev;
	KLUTIL::CSysTime tFullDays = tEventPrev + KLUTIL::CTimeInterval(nDaysAgo);
	if (CLocTime(tFullDays).GetDay() != CLocTime(tEventNext).GetDay())
		nDaysAgo += bEventInThePast ? 1 : -1;
	*/

	// Новый алгоритм (считает кол-во переходов через 0:00)
	CLocTime prev(tEventPrev), next(tEventNext);
	prev.SetHour(12);
	prev.SetMinute(0);
	prev.SetSecond(0);
	prev.SetMilliseconds(0);
	next.SetHour(12);
	next.SetMinute(0);
	next.SetSecond(0);
	next.SetMilliseconds(0);

	int nDaysAgo = (next - prev).GetFullDays();
	
	return nDaysAgo;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int GetHumanDaysAgo(const CSysTime& tEventData)
{
	return GetHumanDaysBetween(tEventData, CSysTime::GetCurrentTime());
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CStdString GetHumanDaysAgoString ( const CSysTime& tEventData, CStdString DateTimeString )
{
	if  ( tEventData.IsInvalid() )
	{
		return _T("");
	}
	
	if  ( DateTimeString.IsEmpty () )
	{
		DateTimeString = tEventData.GetDateTimeString();
	}
	
	int nDays = GetHumanDaysAgo ( tEventData );
	
	if (nDays<0)
		return DateTimeString;
	
	return GetHumanDaysIntervalString ( tEventData, DateTimeString );
	
}

CStdString GetHumanDaysIntervalString ( const CSysTime& tEventData, CStdString DateTimeString )
{
	CStdString str ( DateTimeString );
	
	Get_n_DaysStringEx ( tEventData, str, DateTimeString );
	return str;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int Get_n_DaysStringEx ( const CSysTime& tEventData, CStdString & szDays, CStdString DateTimeString )
{
	if  ( tEventData.IsInvalid() )
	{
		return -1;
	}
	
	if  ( DateTimeString.IsEmpty () )
	{
		DateTimeString = tEventData.GetDateTimeString();
	}
	
	int nDays = GetHumanDaysAgo ( tEventData );
	
	if  ( nDays == 0 )
	{
		szDays.Format ( IDS_COMMON_TODAY, ( LPCSTR ) DateTimeString );
	}
	else if  ( nDays == 1 )
	{
		szDays.Format ( IDS_COMMON_YESTERDAY, ( LPCSTR ) DateTimeString );
	}
	else if  ( nDays == -1 )
	{
		szDays.Format ( IDS_COMMON_TOMORROW, ( LPCSTR ) DateTimeString );
	}
	else
	{
		if  ( nDays > 0 )
		{
			long nTailSeculary = nDays%100;
			long nTailDecade = nTailSeculary%10;
			long nDecades = nTailSeculary/10;
			
			//2-4, 22-24, 32-34,...92-94.
			if  ( nTailDecade >= 2 && nTailDecade <= 4 && nDecades != 1 )
			{
				szDays.Format ( IDS_COMMON_DAYS_AGO1, nDays, ( LPCSTR ) DateTimeString );
			}
			else if  ( nTailDecade == 1 && nDecades != 1 )
			{
				szDays.Format ( IDS_COMMON_DAYS_AGO2, nDays, ( LPCSTR ) DateTimeString );
			}
			else
			{
				szDays.Format ( IDS_COMMON_DAYS_AGO3, nDays, ( LPCSTR ) DateTimeString );
			}
		}
		else
		{
			nDays *= -1;
			long nTailSeculary = nDays % 100;
			long nTailDecade = nTailSeculary % 10;
			long nDecades = nTailSeculary / 10;
			
			//2-4, 22-24, 32-34,...92-94.
			if  ( nTailDecade >= 2 && nTailDecade <= 4 && nDecades != 1 )
			{
				szDays.Format ( IDS_COMMON_DAYS_AGO4, nDays, ( LPCSTR ) DateTimeString );
			}
			else if ( nTailDecade == 1 && nDecades != 1 )
			{
				szDays.Format ( IDS_COMMON_DAYS_AGO5, nDays, ( LPCSTR ) DateTimeString );
			}
			else
			{
				szDays.Format ( IDS_COMMON_DAYS_AGO6, nDays, ( LPCSTR ) DateTimeString );
			}
		}
	}
	return nDays;
}

} // namespace KLUTIL
#ifndef __TMUTIL_H__
#define __TMUTIL_H__

#include <assert.h>
#include <time.h>


/// constants for convertion to FILETIME. Filetime is structure is a 64-bit
///  value representing the number of 100-nanosecond intervals since January
const __int64 FILETIME_IN_SECOND = 10000000I64;
const __int64 FILETIME_IN_HOUR = 3600*FILETIME_IN_SECOND;
const __int64 FILETIME_IN_DAY = 24*FILETIME_IN_HOUR;
const __int64 FILETIME_IN_WEEK = 7*FILETIME_IN_DAY;

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline void UnixTimeToFileTime ( time_t t, LPFILETIME pft )
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;
	
	ll = Int32x32To64 ( t, 10000000 ) + 116444736000000000;
	pft -> dwLowDateTime = (DWORD)ll;
	pft -> dwHighDateTime = (DWORD)(ll >> 32);
} 

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline void UnixTimeToLocalTime ( time_t t, LPSYSTEMTIME pst )
{
	tm* loc = localtime(&t);
	if (loc && pst)
	{
		pst->wYear = loc->tm_year + 1900;
		pst->wMonth = loc->tm_mon + 1;
		pst->wDayOfWeek = loc->tm_wday;
		pst->wDay = loc->tm_mday;
		pst->wHour = loc->tm_hour;
		pst->wMinute = loc->tm_min;
		pst->wSecond = loc->tm_sec;
		pst->wMilliseconds = 0;
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline void UnixTimeToSystemTime ( time_t t, LPSYSTEMTIME pst )
{
	_FILETIME ft;
	
	UnixTimeToFileTime ( t, &ft );
	
	FileTimeToSystemTime ( &ft, pst );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline time_t LocalTimeToUnixTime(const SYSTEMTIME &sysTime, int nDST = -1)
{
	struct tm atm;
	atm.tm_sec = (int)sysTime.wSecond;
	atm.tm_min = (int)sysTime.wMinute;
	atm.tm_hour = (int)sysTime.wHour;
	assert((int)sysTime.wDay >= 1 && (int)sysTime.wDay <= 31);
	atm.tm_mday = (int)sysTime.wDay;
	assert((int)sysTime.wMonth >= 1 && (int)sysTime.wMonth <= 12);
	atm.tm_mon = (int)sysTime.wMonth - 1;        // tm_mon is 0 based
	assert((int)sysTime.wYear >= 1900);
	atm.tm_year = (int)sysTime.wYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;
	time_t rettime = mktime(&atm);
	return rettime;
}

inline const SYSTEMTIME operator+(const SYSTEMTIME &left, const SYSTEMTIME &right)
{
	const time_t resultUnix = LocalTimeToUnixTime(left) + LocalTimeToUnixTime(right);
	SYSTEMTIME resultSystem;
	UnixTimeToLocalTime(resultUnix, &resultSystem);
	return resultSystem;
}

inline const SYSTEMTIME operator+(const SYSTEMTIME &left, const time_t &right)
{
	const time_t resultUnix = LocalTimeToUnixTime(left) + right;
	SYSTEMTIME resultSystem;
	UnixTimeToLocalTime(resultUnix, &resultSystem);
	return resultSystem;
}


#endif
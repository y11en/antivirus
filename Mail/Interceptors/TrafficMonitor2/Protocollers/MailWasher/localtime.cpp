#include <sys/types.h>
#include <time.h>
#include <stdio.h>

#include "localtime.h"

#define DAY_MIN         (24 * HOUR_MIN) /* minutes in a day */
#define HOUR_MIN        60              /* minutes in an hour */
#define MIN_SEC         60              /* seconds in a minute */

static char *montab[12] = {
"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static char *weektab[7] = {
"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};


int CLocalTime::CalcGmtOffs(time_t time)
{
#if defined (WIN32)
	struct tm lt = *localtime(&time);
	struct tm gmt = *gmtime(&time);
#else
	struct tm lt;
	localtime_r( &time, &lt );
	struct tm gmt;
	gmtime_r(&time, &gmt);
#endif
	int gmtoff;

    gmtoff = (lt.tm_hour - gmt.tm_hour) * HOUR_MIN + lt.tm_min - gmt.tm_min;
    if (lt.tm_year < gmt.tm_year)
        gmtoff -= DAY_MIN;
    else if (lt.tm_year > gmt.tm_year)
        gmtoff += DAY_MIN;
    else if (lt.tm_yday < gmt.tm_yday)
        gmtoff -= DAY_MIN;
    else if (lt.tm_yday > gmt.tm_yday)
        gmtoff += DAY_MIN;
    if (lt.tm_sec <= gmt.tm_sec - MIN_SEC)
        gmtoff -= 1;
    else if (lt.tm_sec >= gmt.tm_sec + MIN_SEC)
        gmtoff += 1;

	return gmtoff;
}

int CLocalTime::getGmtOff()
{
	return CalcGmtOffs(time(NULL));
}

std::string CLocalTime::getRfc822Date()
{
	char rfcdate[50];
	char gmtbuffer[50];
	time_t now = ::time(NULL);
	int gmtoff = CalcGmtOffs(now);

#if defined (WIN32)
	struct tm lt = *localtime(&now);
#else
	struct tm lt;
	localtime_r( &now, &lt );
#endif
	_snprintf(gmtbuffer, 50, "%+03d%02d", (int) (gmtoff / HOUR_MIN), (int) (abs(gmtoff) % HOUR_MIN));
	_snprintf(rfcdate, 50, "%s, %d %s %d %02d:%02d:%02d ", weektab[lt.tm_wday], lt.tm_mday, montab[lt.tm_mon], lt.tm_year+1900, lt.tm_hour, lt.tm_min, lt.tm_sec);
	std::string result;

	result = rfcdate;
	result += gmtbuffer;

	return result;
}

std::string CLocalTime::getDate()
{
	char rfcdate[50];
	time_t now = ::time(NULL);

#if defined (WIN32)
	struct tm lt = *localtime(&now);
#else
	struct tm lt;
	localtime_r( &now, &lt );
#endif

	_snprintf(rfcdate, 50, "%02d.%02d.%4d", lt.tm_mday, lt.tm_mon, lt.tm_year+1900);
	std::string result;

	result = rfcdate;

	return result;
}

std::string CLocalTime::getTime()
{
	char rfcdate[50];
	time_t now = ::time(NULL);

#if defined (WIN32)
	struct tm lt = *localtime(&now);
#else
	struct tm lt;
	localtime_r( &now, &lt );
#endif

	_snprintf(rfcdate, 50, "%02d:%02d", lt.tm_hour, lt.tm_min);
	std::string result;

	result = rfcdate;

	return result;
}

std::string CLocalTime::getRfc822Date(time_t _time)
{
	char rfcdate[50];
	char gmtbuffer[50];
	time_t now = _time;
	int gmtoff = CalcGmtOffs(now);

#if defined (WIN32)
	struct tm lt = *localtime(&now);
#else
	struct tm lt;
	localtime_r( &now, &lt );
#endif
	_snprintf(gmtbuffer, 50, "%+03d%02d", (int) (gmtoff / HOUR_MIN), (int) (abs(gmtoff) % HOUR_MIN));
	_snprintf(rfcdate, 50, "%s, %d %s %d %02d:%02d:%02d ", weektab[lt.tm_wday], lt.tm_mday, montab[lt.tm_mon], lt.tm_year+1900, lt.tm_hour, lt.tm_min, lt.tm_sec);
	std::string result;

	result = rfcdate;
	result += gmtbuffer;

	return result;
}

#include "../ichecker2/npr.h"

#include <time.h>

#define cINTERVALS_SECOND (100000000)
#define cINTERVALS_DAY    (((tQWORD)100000000)*60*60*24)
#define cDAYS_CYCLE       (146097)

typedef tQWORD tiDT;

#define TNORM(time) ((time != NULL) ? *((tiDT*)(time)) : 0)
static tBYTE monthday[12] = { 30, 27, 30, 29, 30, 29, 30, 30, 29, 30, 29 ,30 };

static tBOOL Leap(tDWORD year)
{
    return (!(year & 3)) && ( (year % 100) || ( !(year % 400) ) );
}

static tDWORD Days(tDWORD year)
{
    return year*365+year/4-year/100+year/400;
}

/* static tUINT DayNo(tiDT idt, tDWORD *yeardays, tDWORD *year) */
/* { */
/*     tDWORD days; */
/*     tDWORD y1; */
/*     tDWORD y2; */
/*     tDWORD ydays; */

/*     days = (tDWORD)(idt / cINTERVALS_DAY);  // days always fit in dword; */

/*     y1 = (days/cDAYS_CYCLE); // in full cycles */
/*     days -= y1*cDAYS_CYCLE; */

/*     y2 = (days % cDAYS_CYCLE) / 365;     // in last cycle */

/*     ydays = Days(y2); */

/*     if ( days < ydays ) */
/*     { */
/*         y2--; */
/*         ydays = Days(y2); */
/*     } */

/*     if ( year != NULL) */
/*     { */
/*         *year = y2 + y1*400+1; */
/*     } */

/*     if ( yeardays != NULL ) */
/*     { */
/*         *yeardays = days - ydays; */
/*     } */
/*     return days; */
/* } */

tERROR DTSet(tDT *dt, tDWORD year, tDWORD month, tDWORD day, tDWORD hour, tDWORD minute, tDWORD second, tDWORD  ns)
{
    tERROR error = errDT_OUT_OF_RANGE;

    if ( dt != NULL )
    {
        tDWORD days;        // days always smaller than DWORD
        tDWORD secs;
        int leap;

        tiDT *idt;
        idt = (tiDT*)(dt);

        leap = Leap(year);
        // check date
        month--;
        day--;
        if ( month > 11 ) goto abort;
        if ( day > monthday[month] )
        {
            if ( month != 1 || (!leap) ) goto abort;
            if ( day != 28 ) goto abort;
        }
        if ( hour >= 24 ) goto abort;
        if ( minute >= 60 ) goto abort;
        if ( second >= 60 ) goto abort;
        ns /= 10;
        if ( ns >= cINTERVALS_SECOND ) goto abort;

        days = Days(year)+(((month+1)*7826)>>8)+(2-leap)*(month < 2)+day-397;
        secs = hour*3600+minute*60+second;
        *idt = days*cINTERVALS_DAY;
		*idt += (tiDT)secs*cINTERVALS_SECOND;
		*idt += ns/10;
		error = errOK;
    }
    else
    {
        error = errPARAMETER_INVALID;
    }
    abort:
    return error;

}

tERROR Now( tDT* dt ) 
{
	time_t ltime = time(0);
	struct tm * st = localtime (&ltime);

	if ( !dt )
		return errPARAMETER_INVALID;
	if ( !DTSet )
		return errUNEXPECTED;
	
	return DTSet (dt, st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec, 0 );
}

tERROR DTIntervalGet(tDT *dt1, tDT *dt2, tQWORD *intervals, tQWORD ns)
{
    tERROR error = errDT_DIFF_NEGATIVE;
    tiDT idt1;
    tiDT idt2;

    if ( ns == 0 || intervals == NULL )
    {
        error = errPARAMETER_INVALID;
    }
    else
    {
        idt1 = TNORM(dt1);
        idt2 = TNORM(dt2);

        if ( idt2 <= idt1 )
        {
            *intervals = idt1-idt2;

            if ( ns == 100 )
            {
                *intervals /= 10;
            }
            else if ( ns != 10 )
            {
				
                //*intervals = *intervals * 10 / ns; // BUG: possible overflow when *10 (Mike&Graf, 12.09.2002)
				*intervals = (*intervals / ns ) * 10 ;
            }
            error = errOK;
        }
    }
    return error;
}

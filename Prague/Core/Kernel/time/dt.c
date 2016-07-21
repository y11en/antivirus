
// --------------------------------------------------------------------------------
// -------- 20.03.02 14:42            --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Time
// Purpose     -- Not defined
// Author      -- Andrew
// File Name   -- prtime.c


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )

#include <Prague/prague.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktrace.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktime.h>
// AVP Prague stamp end

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

static tUINT DayNo(tiDT idt, tDWORD *yeardays, tDWORD *year)
{
    tDWORD days;
    tDWORD y1;
    tDWORD y2;
    tDWORD ydays;

    days = (tDWORD)(idt / cINTERVALS_DAY);  // days always fit in dword;

    y1 = (days/cDAYS_CYCLE); // in full cycles
    days -= y1*cDAYS_CYCLE;

    y2 = (days % cDAYS_CYCLE) / 365;     // in last cycle

    ydays = Days(y2);

    if ( days < ydays )
    {
        y2--;
        ydays = Days(y2);
    }

    if ( year != NULL)
    {
        *year = y2 + y1*400+1;
    }

    if ( yeardays != NULL )
    {
        *yeardays = days - ydays;
    }
    return days;
}

static tERROR GetDate(tiDT *idt, tDWORD *year, tDWORD *month, tDWORD *day, tDWORD *yday)
{
    tDWORD days;
    tDWORD m;
    tDWORD y2;
    int leap;


    DayNo(*idt,&days,&y2);
    if ( year != NULL ) *year = y2;

    if ( yday != NULL ) *yday = days;

    leap = Leap(y2);
    if ( days-leap > 58 )
    {
        // from march first
        m = (((days+2-leap)<<8)+128)/7826;
    }
    else
    {
        m = (days > 30);
    }

    m++;
    if ( month != NULL ) *month = m;

    days -= ((m*7826)>>8)-33+(2-leap)*(m<3)+leap;

    if ( day != NULL )  *day = days;

    // less then day calculation
    *idt %= cINTERVALS_DAY;

    return errOK;
}

tERROR IMPEX_FUNC(DTSet)(tDT *dt, tDWORD year, tDWORD month, tDWORD day, tDWORD hour, tDWORD minute, tDWORD second, tDWORD  ns)
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
		*idt += ns/*/10*/;		// fix by Mike&Sobko 28/06/04
		error = errOK;
    }
    else
    {
        error = errPARAMETER_INVALID;
    }
    abort:
    return error;

}

tERROR IMPEX_FUNC(DTGet)(const tDT *dt, tDWORD *year, tDWORD *month, tDWORD *day, tDWORD *hour, tDWORD *minute, tDWORD *second, tDWORD *ns)
{
    tDWORD secs;
    tiDT idt;

    idt = TNORM(dt);

    GetDate(&idt,year,month,day,NULL);

    if ( ns != NULL ) *ns = (tDWORD)((idt % cINTERVALS_SECOND)*10);
    secs = (tDWORD)(idt / cINTERVALS_SECOND);

    if ( second != NULL ) *second = secs % 60;
    secs /= 60;
    if ( minute != NULL ) *minute = secs % 60;
    if ( hour != NULL ) *hour = secs / 60;

    return errOK;
}

tINT   IMPEX_FUNC(DTCmp)(const tDT *dt1, const tDT *dt2)
// -1, 0, 1
{
    tINT rcode;
    tiDT idt1;
    tiDT idt2;

    idt1 = TNORM(dt1);
    idt2 = TNORM(dt2);

    if ( idt1 < idt2 )
    {
        rcode = -1;
    }
    else if ( idt1 == idt2 )
    {
        rcode = 0;
    }
    else
    {
        rcode = 1;
    }
    return rcode;
}

tERROR IMPEX_FUNC(DTCpy)(tDT *dest, const tDT *src)
// if src == NULL, clear 'dest'
{
    tERROR error = errOK;

    if ( dest != NULL )
    {
         *((tiDT*)(dest)) = TNORM(src);
    }
    else
    {
        error = errPARAMETER_INVALID;
    }
    return error;
}

tUINT IMPEX_FUNC(DTWeekDay)(const tDT *dt, tBOOL sunday)
{
    tDWORD dayno;
    dayno = DayNo(TNORM(dt),NULL,NULL);
    if ( sunday ) dayno++;
    return dayno % 7;
}

tUINT IMPEX_FUNC(DTWeekNo)(const tDT *dt, tBOOL sunday)
{
    tDWORD day;
    tDWORD yearday;
    day = DayNo(TNORM(dt),&yearday,NULL);
    if ( sunday ) day+=1;
    yearday += (day-yearday) % 7;
    return yearday / 7+1;
}

tUINT IMPEX_FUNC(DTDayNo)(const tDT *dt)
{
    tDWORD day;
    DayNo(TNORM(dt),&day,NULL);
    return day+1;
}

tERROR IMPEX_FUNC(DTIntervalShift)(tDT *dt, tLONGLONG intervals, tQWORD ns)
{
	tERROR error = errPARAMETER_INVALID;
	tiDT idt;
	tiDT idt1 = 0;


	if ( dt != NULL )
	{
		idt = *((tiDT*)(dt));

		if ( ns == 100 )
		{
			idt1 = intervals * 10;
		}
		else if ( ns != 10 )
		{
			//idt1 = intervals * ns; // possible overflow here (Mike, 12.09.2002)
			//idt1 /= 10;
			//ns /= 10;
			//idt1 = intervals * ns; // BUG: precision decrease
			idt1 = (intervals * (ns / 10)) + ((ns % 10) * intervals) / 10;
		}
		else
		{
			idt1 = intervals;
		}
		idt += idt1;

		//if ( idt < idt1 )
		//{
		//	error = errDT_OUT_OF_RANGE;
		//}
		//else
		//{

			*((tiDT*)(dt)) = idt;
			error = errOK;
		//}
	}
	return error;
}

tERROR IMPEX_FUNC(DTIntervalGet)(const tDT *dt1, const tDT *dt2, tLONGLONG *intervals, tQWORD ns)
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
		tBOOL bNegative = cFALSE;
		idt1 = TNORM(dt1);
		idt2 = TNORM(dt2);

		if ( idt2 <= idt1 )
		{
			*intervals = idt1-idt2;
		}
		else 
		{
			*intervals = idt2-idt1;
			bNegative = cTRUE;
			//*intervals = -*intervals; // Mike - error occures below tLONGLONG/QWORD => QWORD
		}

		if ( ns == 100 )
		{
			*intervals /= 10;
		}
		else if ( ns != 10 )
		{

			//*intervals = *intervals * 10 / ns; // BUG: possible overflow when *10 (Mike&Graf, 12.09.2002)
			//*intervals = (*intervals / ns ) * 10 ; // BUG: precision decrease - (230 / 100) * 10 = 20 (Vik&Mike 22.12.03)
			//														(230 * 10) / 100 = 23!
			*intervals = ((*intervals / ns ) * 10) + (((*intervals % ns) * 10) / ns);
		}
		if (bNegative)
			*intervals = -*intervals;
		error = errOK;
	}
	return error;
}

tERROR IMPEX_FUNC(DTDiffGet)(const tDT *dt1, const tDT *dt2, tDWORD *year, tDWORD *month, tDWORD *day, tDWORD *hour, tDWORD *minute, tDWORD *second)
{
    tERROR error = errDT_DIFF_NEGATIVE;
    tiDT idt1;
    tiDT idt2;
    tiDT diff;

    idt1 = TNORM(dt1);
    idt2 = TNORM(dt2);

    if ( idt2 <= idt1 )
    {
        if ( year != NULL || month != NULL )
        {
            tDWORD y1, m1, d1, yd1;
            tDWORD y2, m2, d2, yd2;

            GetDate(&idt1, &y1, &m1, &d1, &yd1);
            GetDate(&idt2, &y2, &m2, &d2, &yd2);

            diff = idt1 - idt2;

            if ( idt1 < idt2 && (hour != NULL || minute != NULL || second != NULL) )
            {
                diff += cINTERVALS_DAY; // add one day to h:m:s
                d1--;                   // sub ine day from date (can be 0)
            }

            if ( month != NULL )
            {

                if ( d1 < d2 )
                {
                    // first correction
                    m1--;

                    if ( m1 == 0 )
                    {
                        y1--;
                        m1 = 12;
                    }

                    if ( m1 == 2 && Leap(y1) )
                    {
                        d1 += 29;
                    }
                    else
                    {
                        d1 += monthday[m1-1]+1;
                    }
                }

                if ( d1 < d2 )
                {
                    // second correction
                    m1--;
                    if ( m1 == 0 )
                    {
                        y1--;
                        m1 = 12;
                    }

                    if ( m1 == 2 && Leap(y1) )
                    {
                        d1 += 29;
                    }
                    else
                    {
                        d1 += monthday[m1-1]+1;
                    }
                }

                d1 -= d2;

                if ( m1 < m2 )
                {
                    m1 += 12;
                    y1--;
                }

                m1 -= m2;
                y1 -= y2;

                if ( year != NULL )
                {
                    *year = y1;
                }
                else
                {
                    m1 += y1*12;
                }

                if ( month != NULL )
                {
                    *month = m1;
                }

            }
            else
            {
                if ( yd1 < yd2 )
                {
                    y1--;
                    yd1 += 365+Leap(y1);
                }

                y1 -= y2;
                d1 = yd1 - yd2;

                *year = y1;
            }

            if ( day != NULL )
            {
                *day = d1;
            }
            else
            {
                diff += (d1-1) * cINTERVALS_DAY;
            }
        }
        else
        {
            diff = idt1 - idt2;

            if ( day != NULL )
            {
                *day = (tDWORD)(diff / cINTERVALS_DAY);
                diff %= cINTERVALS_DAY;
            }
        }


        diff /= cINTERVALS_SECOND; // remove nanoseconds

        if ( hour != NULL )
        {
            *hour = (tDWORD)(diff / 3600);
            diff %= 3600;
        }

        if ( minute != NULL )
        {
            *minute = (tDWORD)(diff / 60);
            diff %= 60;
        }

        if ( second != NULL )
        {
            *second = (tDWORD)(diff);
        }

        error = errOK;
    }
    return error;
}

tERROR IMPEX_FUNC(DTTimeShift)(tDT *dt, tINT hour, tINT minute, tINT second)
{
    tERROR error = errOK;

    if ( dt != NULL )
    {
        *((tiDT*)(dt)) += (tLONGLONG)((tLONGLONG)hour*3600 + (tLONGLONG)minute*60 + second)*cINTERVALS_SECOND;
    }
    else
    {
        error = errPARAMETER_INVALID;
    }
    return error;
}

tERROR IMPEX_FUNC(DTDateShift)(tDT *dt, tINT year, tINT month, tINT day)
{
    tERROR error = errPARAMETER_INVALID;

    if ( dt != NULL )
    {
        tDWORD iyear;
        tINT   imonth;
        tDWORD iday;
        tDWORD days;
        int leap;

        GetDate((tiDT*)(dt),&iyear,&imonth,&iday,NULL);

        if ( year < 0 )
        {
            iyear -= (tDWORD)(-year);
        }
        else
        {
            iyear += (tDWORD)(year);
        }

        if ( month < 0 )
        {
            imonth -= (tDWORD)(-month);
        }
        else
        {
            imonth += (tDWORD)(month);
        }

		if ( imonth <= 0 )
		{
			iyear += imonth / 12 - 1;
			imonth = imonth  % 12 + 12;
		}

		imonth--;

        if ( imonth > 11 )
        {
            iyear += imonth / 12;
            imonth %= 12;
        }

		leap = Leap(iyear);

		iday--;

        if ( iday > monthday[imonth] )
        {
            if ( imonth == 1 && leap && iday >= 28 )
            {
                iday = 28;
            }
            else
            {
                iday = monthday[imonth];
            }
        }

        days = 365*iyear+iyear/4-iyear/100+iyear/400+((imonth+1)*3057)/100+(2-leap)*(imonth < 2)+iday-397;

        *((tiDT*)(dt)) += cINTERVALS_DAY*days;

        if ( day )
        {
            *((tiDT*)(dt)) += (tLONGLONG)(day)*cINTERVALS_DAY;
        }
        error = errOK;
    }
    return error;
}


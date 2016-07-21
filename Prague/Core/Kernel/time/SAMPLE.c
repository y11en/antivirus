
// -------------------------------------------
// -------- 25.03.02 15:56 -------------------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Time
// Purpose     -- Not defined
// Author      -- Andrew
// File Name   -- sample.c

// converters sample

#include <prague.h>
#include <iface/l_time.h>
#include <windows.h>
#include <time.h>

tERROR DT2FileTime(tDT *dt, tQWORD *filetime)
{
    tDT origin;

    DTSet(&origin, 1601, 1, 1,0,0,0,0);           // set FILETIME origin
    DTIntervalGet(dt, &origin, filetime, 100); // calc diff in 100 ns intervals

    return errOK;
}

tERROR FileTime2DT(tDT *dt, tQWORD *filetime)
{

    DTSet(dt, 1601, 1, 1,0,0,0,0);           // set FILETIME origin
    DTIntervalShift(dt, *filetime, 100);

    return errOK;

}

tERROR DT2SystemTime(tDT *dt, SYSTEMTIME *st)
{
    tDWORD year  ;
    tDWORD month ;
    tDWORD day   ;
    tDWORD hour  ;
    tDWORD minute;
    tDWORD second;
    tDWORD ns    ;

    DTGet(dt, &year, &month, &day, &hour, &minute, &second, &ns);

    st->wYear   = (WORD)(year  );
    st->wMonth  = (WORD)(month );
    st->wDay    = (WORD)(day   );
    st->wHour   = (WORD)(hour  );
    st->wMinute = (WORD)(minute);
    st->wSecond = (WORD)(second);
    st->wMilliseconds = (WORD)(ns / 1000000);

    return errOK;

}

tERROR SystemTime2DT(tDT *dt, SYSTEMTIME *st)
{
    DTSet(dt, st->wYear, st->wMonth, st->wDay, st->wHour, st->wMinute, st->wSecond, (tDWORD)(st->wMilliseconds) *1000000);
    return errOK;
}

tERROR DT2Time_t(tDT *dt, time_t *time)
{
    tDT origin;
    tDWORD secs;

    DTSet(&origin, 1970, 1, 1, 0,0,0,0); // time_t origin
    DTDiffGet(dt,&origin,NULL,NULL,NULL,NULL,NULL, &secs);
    *time = (time_t)(secs);
    return errOK;
}

tERROR Time_t2DT(tDT *dt, time_t *time)
{
    DTSet(dt, 1970, 1, 1, 0,0,0,0); // time_t origin
    DTTimeShift(dt,0,0,*time);
    return errOK;
}

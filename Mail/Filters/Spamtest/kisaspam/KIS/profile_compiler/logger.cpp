/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2005, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: logger.cpp                                                            *
 * Created: Sun Jun  2 20:35:15 2002                                           *
 * Desc: Logger facility for KASP.                                             *
 * --------------------------------------------------------------------------- *
 * Victor Troitsky
 *******************************************************************************/

/**
 * \file  logger.cpp
 * \brief Logger facility for KASP.
 *
 * XXX Needed to make locks for writing to logs by different processes.
 */
//#include "client-filter/librules/commhdr.h"
#include "StdAfx.h"
#pragma hdrstop

#include "LogFile.h"


#include <stdlib.h>
#include <stdio.h>

#ifndef _WIN32
#include <syslog.h>
#include <sys/uio.h>
#include <unistd.h>
#else
#include <io.h>
#include <process.h>

//#define getpid _getpid
//#define snprintf _snprintf
//#define vsnprintf _vsnprintf

//#include <utility>
//#include <hash_map>
#endif //_WIN32

#include <sys/types.h>

#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <smtp-filter/common/logger.h>

#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

extern "C"
int logger_vprintf( unsigned int mask, const char *format, va_list argptr )
{
    char buf[8192]; 
    buf[sizeof(buf)-1] = '\x0';
	
    int log_level = LOG_DEBUG;
					
    if(mask & LOGGER_ERROR)
        log_level = LOG_DEBUG;
    else if(mask & LOGGER_CRITICAL)
        log_level = LOG_INFO;
    else if(mask & LOGGER_INFO)
        log_level = LOG_DEBUG;
    else if(mask & LOGGER_WARNING)
        log_level = LOG_DEBUG;
	
#ifdef WIN32
    _vsnprintf(buf, sizeof(buf)-1, format, argptr);
#else
    vsnprintf(buf, sizeof(buf)-1, format, argptr);
#endif

    WriteToLog(log_level, buf);
    return 0;
}

int logger_printf(unsigned int mask, const char *format, ... )
{
    va_list args;
    int res = 0;

    va_start(args, format);

    res = logger_vprintf(mask, format, args);

    va_end(args);

    return res;
}

int fltlog_flood(const char *format, ... )
{
    va_list args;
    int res = 0;

    va_start(args, format);

    res = logger_vprintf(LOGGER_VERBOSE_FLOOD | LOGGER_DEBUG, format, args);

    va_end(args);

    return res;
}

int fltlog_debug(const char *format, ... )
{
    va_list args;
    int res = 0;

    va_start(args, format);

    res = logger_vprintf(LOGGER_VERBOSE_EXTREME | LOGGER_DEBUG, format, args);

    va_end(args);

    return res;
}

int fltlog_info(const char *format, ... )
{
    va_list args;
    int res = 0;

    va_start(args, format);

    res = logger_vprintf(LOGGER_VERBOSE_HIGH | LOGGER_INFO, format, args);

    va_end(args);

    return res;
}

int fltlog_warning(const char *format, ... )
{
    va_list args;
    int res = 0;

    va_start(args, format);

    res = logger_vprintf(LOGGER_VERBOSE_INFORMATIVE | LOGGER_WARNING, format, args);

    va_end(args);

    return res;
}

int fltlog_error(const char *format, ... )
{
    va_list args;
    int res = 0;

    va_start(args, format);

    res = logger_vprintf(LOGGER_VERBOSE_NORMAL | LOGGER_ERROR, format, args);

    va_end(args);

    return res;
}

int fltlog_crit(const char *format, ... )
{
    va_list args;
    int res = 0;

    va_start(args, format);

    res = logger_vprintf(LOGGER_VERBOSE_NORMAL | LOGGER_CRITICAL, format, args);

    va_end(args);

    return res;
}


/*
 * <eof logger.cpp>
 */


void WriteToLog(int type, const char *str1, const char *str2, const char *str3, const char *str4, const char *str5, const char *str6)
{
}

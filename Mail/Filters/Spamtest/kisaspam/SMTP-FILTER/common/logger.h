/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: logger.h                                                              *
 * Created: Sat Jun  1 14:21:48 2002                                           *
 * Desc: Logger facility for filter                                            *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  logger.h
 * \brief Logger facility for filter
 */

#ifndef __logger_h__
#define __logger_h__

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define LOGGER_DEBUG     0x00000001UL
#define LOGGER_INFO      0x00000002UL
#define LOGGER_CRITICAL  0x00000004UL
#define LOGGER_ERROR     0x00000008UL
#define LOGGER_WARNING   0x00000010UL
#define LOGGER_STATS     0x00000040UL

/*
 * LOGGER_USER added to suppor user-side log messages (via DoSysLog in profiles).
 */
#define LOGGER_USER      0x00000020UL

/* without LOGGER_USER --- it is correct */
#define LOGGER_ALL       (LOGGER_DEBUG | LOGGER_INFO | LOGGER_CRITICAL | LOGGER_ERROR | LOGGER_WARNING)

#define LOGGER_MASK_ALL  0x0000FFFFUL

#define LOGGER_VERBOSE_NORMAL       0x00010000UL
#define LOGGER_VERBOSE_INFORMATIVE  0x00020000UL
#define LOGGER_VERBOSE_HIGH         0x00030000UL
#define LOGGER_VERBOSE_EXTREME      0x00040000UL
#define LOGGER_VERBOSE_FLOOD        0x00050000UL

#define LOGGER_VERBOSE_MASK         0x000F0000UL

    /*
     * Note, than in "add" functions mask used only for LOGGER_MASK_ALL definitions.
     * But in logger_printf you can set any combination of LOGGER_* flags.
     */

#define LOGGER_OPEN_NORMAL 0x00000000UL
#define LOGGER_OPEN_PID    0x00000001UL

#ifndef _WIN32
    int  logger_open(const char *id_str, unsigned int flags);
#else
	int logger_open(int s, const char *id_str, unsigned int flags);
#endif //_WIN32

    /* added file closed by logger at logger_close() call */
    int  logger_add_file(int fd, unsigned int mask);
    int  logger_remove_file(int fd);
    
    int  logger_add_syslog(unsigned int mask);
    int  logger_set_syslog_mask(unsigned int mask);
    int  logger_remove_syslog();

    int  logger_set_verbose_level(unsigned int level);

    int  logger_printf(unsigned int mask, const char *format, ... );

    int  logger_printf_ex(const char *file_id, unsigned int msg_id, int level, const char *format, ... );

#define LOGID(N) "[" MY_LOG_FILEID N "]: "
#define LOGIDNONE "[NOLOGID]: "

    int  fltlog_flood  (const char *format, ... );
    int  fltlog_debug  (const char *format, ... );
    int  fltlog_info   (const char *format, ... );
    int  fltlog_warning(const char *format, ... );
    int  fltlog_error  (const char *format, ... );
    int  fltlog_crit   (const char *format, ... );

    void logger_close();

#ifdef __cplusplus
}
#endif

#endif /* __logger_h__ */

/*
 * <eof logger.h>
 */

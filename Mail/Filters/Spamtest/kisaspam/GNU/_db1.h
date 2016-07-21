/****************************************************************************
 * Proj: Content filtration                                                 *
 * ------------------------------------------------------------------------ *
 * File: bdb1.h                                                             *
 * Desc: Include file for berkeley db 1 (which may be in different places   *
 *       at different OS)                                                   *
 * ------------------------------------------------------------------------ *
 * Andrey L. Kalinin, andrey@kalinin.ru                                     *
 ****************************************************************************/

#ifndef __db1_compat_h__
#define __db1_compat_h__

#if defined(WIN32) || defined(_WIN32) || defined(OS_WIN32) || defined(_MSC_VER)
#include <fcntl.h>
#include <gnu/db-1.85/include/db.h>
#define DB_BINARY     O_BINARY
#else

#define DB_BINARY     0x0000
#if defined (OS_FREEBSD) || defined(OS_UNIX)
#include <db.h>
#elif defined(OS_LINUX)
#include <gnu/db-1.85/include/db.h>
/*#include <gnu/db-1.85/include/db.h>*/
#else
#include <gnu/db-1.85/include/db.h>
#endif

#endif

#endif /* __db1_compat_h__ */

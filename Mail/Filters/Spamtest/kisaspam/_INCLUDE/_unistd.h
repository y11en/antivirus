/* Copyright (C) 2001-2003 Ashmanov & Partners company, Moscow, Russia
 * All rigts reserved.
 *
 * PROJECT: PCOMMON
 * NAME:    Common unix compatibility API
 *
 * COMPILER: MS VC++ 6.0, GCC     TARGET: Win32,FreeBSD-x86,Linux-x86
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 06.11.01: v 1.00 created by AlexIV
 *==========================================================================
 */
#ifndef __UNISTD_h__
#define __UNISTD_h__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_MSC_VER)
#include <unistd.h>
#else

#include <stddef.h>

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;
int getopt(int argc, char * const *argv, const char *optstring);

size_t strlcpy(char *dst, const char *src, size_t dstsize);

#endif /* _MSC_VER */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*__UNISTD_h__*/

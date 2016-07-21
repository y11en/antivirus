/* $Id: nix_types.c,v 1.1 2004/12/30 14:09:32 alexiv Exp $ 
 * */
#include "StdPch.h"

#ifdef _WIN32 

#include <stdio.h>
#include <errno.h>
#include <WinSock.h>
#include <nix_types.h>

#include <_unistd.h>
#include <_time.h>
#include <nix_types.h>

#ifdef USE_CUSTOM_HEAP
#include "mem_heap.h"
#endif


#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN    16
#endif

static const char * inet_ntop_v4 (const void *src, char *dst, size_t size)
{
    const char digits[] = "0123456789";
    int i;
    struct in_addr *addr = (struct in_addr *)src;
    u_long a = ntohl(addr->s_addr);
    const char *orig_dst = dst;

    if (size < INET_ADDRSTRLEN) {
	errno = ENOSPC;
	return NULL;
    }
    for (i = 0; i < 4; ++i) {
	int n = (a >> (24 - i * 8)) & 0xFF;
	int non_zerop = 0;

	if (non_zerop || n / 100 > 0) {
	    *dst++ = digits[n / 100];
	    n %= 100;
	    non_zerop = 1;
	}
	if (non_zerop || n / 10 > 0) {
	    *dst++ = digits[n / 10];
	    n %= 10;
	    non_zerop = 1;
	}
	*dst++ = digits[n];
	if (i != 3)
	    *dst++ = '.';
    }
    *dst++ = '\0';
    return orig_dst;
}

const char * inet_ntop(int af, const void *src, char *dst, size_t size)
{
    switch (af) {
    case AF_INET :
	return inet_ntop_v4 (src, dst, size);
    default :
	errno = WSAEAFNOSUPPORT;
	return NULL;
    }
}


int inet_pton(int af, const char *src, void *dst)
{
	struct in_addr *addr;
    if (af != AF_INET) {
	errno = EAFNOSUPPORT;
	return -1;
    }
	addr = dst;
	addr->s_addr = inet_addr(src);
	return (addr->s_addr == INADDR_NONE) ? 0 : 1;
}

int inet_aton(const char *cp, struct in_addr *addr)
{
	addr->s_addr = inet_addr(cp);
	return (addr->s_addr == INADDR_NONE) ? 0 : 1;
}

size_t strlcpy(char *dst, const char *src, size_t dstsize)
{
	if(strlen(src) > dstsize - 1)
	{
		strncpy(dst, src, dstsize - 1);
		dst[dstsize - 1] = 0;
	}
	else
		strcpy(dst, src);
	return strlen(src);
}

/* FILETIME of Jan 1 1970 00:00:00. */
static const unsigned __int64 epoch = 116444736000000000L;

/*
 * timezone information is stored outside the kernel so tzp isn't used anymore.
 */

int
gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	FILETIME	file_time;
	SYSTEMTIME	system_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long) (system_time.wMilliseconds * 1000);

	return 0;
}


/* Functions to capsule or serve linux style function
 * for Windows Visual C++ 
 */
/*
int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
        // Get the time, if they want it 
        if (time_Info != NULL) {
                time_Info->tv_sec = time(NULL);
                time_Info->tv_usec = timeGetTime()*1000;
        }
        // Get the timezone, if they want it 
        if (timezone_Info != NULL) {
                _tzset();
                timezone_Info->tz_minuteswest = _timezone;
                timezone_Info->tz_dsttime = _daylight;
        }
        // And return 
        return 0;
}

*/


#endif // _WIN32

#ifndef __TIME_H
#define __TIME_H

# include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

char* _strptime(char *buf, char *fmt, struct tm *tm);

#ifdef _WIN32
#if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};
#endif
int gettimeofday(struct timeval * tp, struct timezone * tzp);
#endif

#ifdef __cplusplus
};
#endif

#endif /*__TIME_H*/

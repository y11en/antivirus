
#ifndef __COMMON_HEADER_FILE__
#define __COMMON_HEADER_FILE__

#include <windows.h>
#include <tchar.h>

#define KASP

#define  LOG (s)
#define  LOG2 (s1, s2)
#define  LOG3 (s1, s2, s3)
#define  LOG4 (s1, s2, s3, s4)

#define __byte_swap_long(x) (((x) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | ((x) >> 24))
#define ntohl(x) __byte_swap_long(x)

//#include "../_include/export.h"

#ifndef MAX_GSG2_SIZE
    #define MAX_GSG2_SIZE (300*1024)
#endif

#endif//__COMMON_HEADER_FILE__
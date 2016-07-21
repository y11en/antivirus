#include <time.h>
#define KLDUMP_LOG_SIG  'UDLK'

struct KLDUMP_LOG_HEADER
{
    unsigned int sig;
#if _MSC_VER >= 1300
    __time32_t   time;
#else
    time_t       time;
#endif
    unsigned int reserved0;
    unsigned int reserved1;
};

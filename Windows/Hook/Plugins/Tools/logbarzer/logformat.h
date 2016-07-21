#include <time.h>
#define KLDUMP_LOG_SIG  'UDLK'

#pragma pack(push, 1)
struct KLDUMP_LOG_HEADER
{
    unsigned int sig;
    time_t time;
    unsigned int reserved0;
    unsigned int reserved1;
};

struct KLDUMP_LOG_PKT_HEADER
{
    unsigned long isIncoming;
    unsigned long PacketSize;
};
#pragma pack(pop)
#include "../../klsdk/kl_ipaddr.h"

// Banomet API

#define BANOMET_PLUGIN_NAME "Banomet"

#pragma pack( push, 1 )


#define MSG_BAN_HOST        0x1

typedef struct _UNBAN_ITEM {
    unsigned long   HostIp;
    unsigned long   Mask;
} UNBAN_ITEM, *PUNBAN_ITEM;
#define MSG_UNBAN_HOST      0x2


typedef struct _BAN_LIST_ITEM {
    ULONG       banned_ip;      // забанненый хост
    
    __int64     ban_from;       // когда был забанен
} BAN_LIST_ITEM, *PBAN_LIST_ITEM;
#define MSG_GET_BANNED_LIST 0x3


typedef struct _UNBAN_ITEM2 {
    ip_addr_t   HostIp;
    
    ip_addr_t   Mask;    

} UNBAN_ITEM2, *PUNBAN_ITEM2;
#define MSG_UNBAN_HOST2     0x4


typedef struct _BAN_LIST_ITEM2 {    
    ip_addr_t   banned_ip;

    __int64     ban_from;       // когда был забанен
} BAN_LIST_ITEM2, *PBAN_LIST_ITEM2;
#define MSG_GET_BANNED_LIST2 0x5


#pragma pack( pop )
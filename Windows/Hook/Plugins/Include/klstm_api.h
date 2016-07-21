// Stealth mode API

#define STEALTH_MODE_PLUGIN_NAME    "Stealth Mode plugin"

#include "../../klsdk/kl_ipaddr.h"

#pragma pack ( push, 1 )

// ioctl message ids
#define STEALTH_RUN     0x1

#define STEALTH_STOP    0x2

typedef struct _SET_STEALTH_STRUCT {
	char	MacAddress[6];
	ULONG	isSet;
} SET_STEALTH_STRUCT, *PSET_STEALTH_STRUCT;

// Message = SET_STEALTH_STRUCT
#define STEALTH_SET_STEALTH	0x3


typedef struct _STEALTH_NET  {
	ULONG	Net;
	ULONG	Mask;
} STEALTH_NET, *PSTEALTH_NET;

// добавляет сеть "видимости"
// Message = STEALTH_NET
#define STEALTH_ADD_NET		0x4

// удаляет сеть
// Message = STEALTH_NET
#define STEALTH_REMOVE_NET	0x5

typedef struct _STEALTH_NET_LIST {
	ULONG		Count;
	STEALTH_NET Net[0];
} STEALTH_NET_LIST, *PSTEALTH_NET_LIST;
#define STEALTH_GET_NET_LIST 0x6


typedef struct _STEALTH_NET_REACTION {
	bool		UseStmForNet;
} STEALTH_NET_REACTION, *PSTEALTH_NET_REACTION;
#define STEALTH_SET_NET_DEFAULT_REACTION 0x7


typedef struct _STEALTH_NET2  {
    ip_addr_t	Net;
    ip_addr_t	Mask;
} STEALTH_NET2, *PSTEALTH_NET2;

// добавляет сеть "видимости"
// Message = STEALTH_NET2
#define STEALTH_ADD_NET2		0x8

// удаляет сеть
// Message = STEALTH_NET2
#define STEALTH_REMOVE_NET2	    0x9

typedef struct _STEALTH_NET_LIST2 {
	ULONG		 Count;
	STEALTH_NET2 Net[0];
} STEALTH_NET_LIST2, *PSTEALTH_NET_LIST2;
#define STEALTH_GET_NET_LIST2   0xA


#pragma pack ( pop )
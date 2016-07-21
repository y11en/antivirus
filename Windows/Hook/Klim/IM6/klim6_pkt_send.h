#ifndef _KLIM6_PKT_SEND_
#define _KLIM6_PKT_SEND_

#include "..\klim_pkt.h"

typedef struct _KLIM6_SEND_PACKET {
    PMS_FILTER          Filter;
    PNET_BUFFER_LIST    NetBufferList;
    NDIS_PORT_NUMBER    PortNumber;
    ULONG               SendFlags;
} KLIM6_SEND_PACKET, *PKLIM6_SEND_PACKET;

VOID 
OutPkt_Init();

PKLIM_PACKET
OutPkt_Allocate( PMS_FILTER         pFilter,
                 PNET_BUFFER_LIST   pNetBufferList,
                 PNET_BUFFER        pNetBuffer,
                 NDIS_PORT_NUMBER   PortNumber,
                 ULONG              SendFlags
               );



#endif //_KLIM6_PKT_SEND_
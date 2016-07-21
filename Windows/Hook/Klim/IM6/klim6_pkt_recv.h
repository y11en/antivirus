#ifndef _KLIM6_PKT_RECV_
#define _KLIM6_PKT_RECV_

#include "..\klim_pkt.h"

typedef struct _KLIM6_RECV_PACKET {
    PMS_FILTER          Filter;
    PNET_BUFFER_LIST    NetBufferList;
    NDIS_PORT_NUMBER    PortNumber;
    ULONG               ReceiveFlags;
} KLIM6_RECV_PACKET, *PKLIM6_RECV_PACKET;

VOID 
InPkt_Init();

PKLIM_PACKET
InPkt_Allocate( PMS_FILTER pFilter,
                PNET_BUFFER_LIST pNetBufferList,
                NDIS_PORT_NUMBER PortNumber,
                ULONG            ReceiveFlags
              );



#endif //_KLIM6_PKT_RECV_ 

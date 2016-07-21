#ifndef _KLIM5_RECV_PACKETS__
#define _KLIM5_RECV_PACKETS__

#include "..\klim_pkt.h"

#define IN_NDIS_PKT_TAG '20pg'

typedef struct _KLIM_RECV_NDIS_PACKET {
    PNDIS_PACKET NdisPacket;
    PNDIS_PACKET OrigPacket;
    PADAPT  Adapt;
} KLIM_RECV_NDIS_PACKET, *PKLIM_RECV_NDIS_PACKET;

VOID
InPkt_Init();


NDIS_STATUS
InPkt_Ref( PVOID pktConext );

NDIS_STATUS
InPkt_Deref( PVOID pktConext );

ULONG
InPkt_GetRefCount( PVOID pktConext );

ULONG
InPkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count );

ULONG
InPkt_GetSize( PVOID pktContext );
//-------------------------------------------------

VOID
InPkt_Initialize(                 
                 PKLIM_PACKET pKlimPacket, 
                     PNDIS_PACKET pNdisPacket,
                     PADAPT pAdapter);

PKLIM_PACKET
InPkt_Allocate(                
                PNDIS_PACKET pNdisPacket,
                PADAPT pAdapter);

#endif
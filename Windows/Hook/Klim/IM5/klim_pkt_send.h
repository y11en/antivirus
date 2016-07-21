#ifndef _KLIM5_SEND_PACKETS__
#define _KLIM5_SEND_PACKETS__

typedef struct _KLIM_SEND_NDIS_PACKET {
    PNDIS_PACKET NdisPacket;
    PADAPT       Adapt;
    PNDIS_PACKET OrigPacket;
} KLIM_SEND_PACKET, *PKLIM_SEND_PACKET;

VOID
OutPkt_Initialize();

PKLIM_PACKET
OutNdisPkt_Allocate( 
                    PNDIS_PACKET pNdisPacket,
                    PADAPT pAdapter);

VOID
OutNdisPkt_Initialize( PKLIM_PACKET pKlimPacket, 
                      PNDIS_PACKET pNdisPacket,
                     PADAPT pAdapter);


#endif
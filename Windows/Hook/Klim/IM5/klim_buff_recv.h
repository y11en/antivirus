#ifndef _KLIM5_RECV_BUFFERS_H__
#define _KLIM5_RECV_BUFFERS_H__

#include "..\klim_pkt.h"

#define IN_BUFF_PKT_TAG '10pg'

#pragma pack( push, 4 )

typedef struct _KLIM_RECV_BUFF_PACKET {
    ULONG HeaderBufferSize;
    ULONG LookAheadBufferSize;
    PCHAR HeaderBuffer;
    PCHAR LookAheadBuffer;

    PADAPT  Adapt;

    CHAR  PlainPacket[0];  // Существует только когда RefCount > 1
} KLIM_RECV_BUFF_PACKET, *PKLIM_RECV_BUFF_PACKET;

#pragma pack ( pop )

VOID
InBuffPkt_Init();

NDIS_STATUS
InBuffPkt_Ref( PVOID pktConext );

NDIS_STATUS
InBuffPkt_Deref( PVOID pktConext );

ULONG
InBuffPkt_GetRefCount( PVOID pktConext );

ULONG
InBuffPkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count );

ULONG
InBuffPkt_GetSize( PVOID pktContext );
//-------------------------------------------------

VOID
InBuffPkt_Initialize( PKLIM_PACKET pKlimPacket, 
                     PVOID HeaderBuffer, 
                     ULONG HeaderBufferSize, 
                     PVOID LookAheadBuffer, 
                     ULONG LookAheadBufferSize,
                     PADAPT pAdapter);

PKLIM_PACKET
InBuffPkt_Allocate( 
                   PVOID HeaderBuffer, 
                   ULONG HeaderBufferSize, 
                   PVOID LookAheadBuffer, 
                   ULONG LookAheadBufferSize,
                   PADAPT pAdapter);

PKLIM_PACKET
InBuffPkt_Allocate2( 
                   PNDIS_PACKET NdisPacket,
                   PADAPT pAdapter);

#endif
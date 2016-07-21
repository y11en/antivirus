#include "precomp.h"
#include "..\klim_pkt.h"
#pragma hdrstop

extern NDIS_HANDLE     InPktPacketPool;
extern NDIS_HANDLE     InPktBufferPool;

VOID
InBuffPkt_Init()
{    
}

ULONG
InBuffPkt_GetSize( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;
    PKLIM_RECV_BUFF_PACKET pkt = (PKLIM_RECV_BUFF_PACKET)p->Packet;

    return pkt->HeaderBufferSize + pkt->LookAheadBufferSize;
}

ULONG
InBuffPkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;
    PKLIM_RECV_BUFF_PACKET pkt = (PKLIM_RECV_BUFF_PACKET)p->Packet;    
    ULONG bToCopy = count;

    if ( pkt->HeaderBufferSize + pkt->LookAheadBufferSize - from < count )
        bToCopy = pkt->HeaderBufferSize + pkt->LookAheadBufferSize - from;

    /*
    // 1. Copy HeaderBuffer Data
    if ( from < pkt->HeaderBufferSize )
    {
        RtlCopyMemory ( Buffer, (PCHAR)pkt->HeaderBuffer + from, pkt->HeaderBufferSize - from );
        dwCopied = pkt->HeaderBufferSize - from;
    }

    // 2. Copy LookAheadBuffer Data
    RtlCopyMemory ( (PCHAR)Buffer + dwCopied, pkt->LookAheadBuffer, count - dwCopied );

    dwCopied += count - dwCopied;
    */
    RtlCopyMemory( (PCHAR)Buffer, pkt->PlainPacket + from, bToCopy );

    return bToCopy;
}

NDIS_STATUS
InBuffPkt_Ref( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;

    InterlockedIncrement( &p->RefCount );

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
InBuffPkt_Deref( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;
    PKLIM_RECV_BUFF_PACKET pkt = (PKLIM_RECV_BUFF_PACKET)p->Packet;
    PADAPT pAdapt = pkt->Adapt;

    InterlockedDecrement( &p->RefCount );

    if ( p->RefCount == 0 )
    {
        switch ( p->Status )
        {
        case KLIM_PKT_PASS:
            {
                pAdapt->IndicateRcvComplete = TRUE;
                if (pAdapt->MiniportHandle == NULL)
                {
                    break;
                }
                switch (pAdapt->Medium)
                {
                case NdisMedium802_3:
                case NdisMediumWan:
                    NdisMEthIndicateReceive(pAdapt->MiniportHandle,
                        p,
                        pkt->HeaderBuffer,
                        pkt->HeaderBufferSize,
                        pkt->LookAheadBuffer,
                        pkt->LookAheadBufferSize,
                        pkt->LookAheadBufferSize);
                    break;

                case NdisMedium802_5:
                    NdisMTrIndicateReceive(pAdapt->MiniportHandle,
                        p,
                        pkt->HeaderBuffer,
                        pkt->HeaderBufferSize,
                        pkt->LookAheadBuffer,
                        pkt->LookAheadBufferSize,
                        pkt->LookAheadBufferSize);
                    break;

#if FDDI
                case NdisMediumFddi:
                    NdisMFddiIndicateReceive(pAdapt->MiniportHandle,
                        p,
                        pkt->HeaderBuffer,
                        pkt->HeaderBufferSize,
                        pkt->LookAheadBuffer,
                        pkt->LookAheadBufferSize,
                        pkt->LookAheadBufferSize);
                    break;
#endif
                default:
                    ASSERT(FALSE);
                    break;
                }
            }
            break;
        
        case KLIM_PKT_BLOCK:
            break;   
        }
        
        ExFreeToNPagedLookasideList( &pkt->Adapt->InBufList, p );        
    }

    return NDIS_STATUS_SUCCESS;
}

ULONG
InBuffPkt_GetRefCount( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;    

    return p->RefCount;
}

PKLIM_PACKET
InBuffPkt_Allocate( 
                   PVOID HeaderBuffer, 
                   ULONG HeaderBufferSize, 
                   PVOID LookAheadBuffer, 
                   ULONG LookAheadBufferSize,
                   PADAPT pAdapter)
{
    PKLIM_PACKET pKlimPacket = ExAllocateFromNPagedLookasideList( &pAdapter->InBufList );

    if ( pKlimPacket )
    {
        PKLIM_RECV_BUFF_PACKET pkt = (PKLIM_RECV_BUFF_PACKET)pKlimPacket->Packet;

        pkt->HeaderBufferSize = HeaderBufferSize;
        pkt->LookAheadBufferSize = LookAheadBufferSize;
        pkt->HeaderBuffer = pkt->PlainPacket;
        pkt->LookAheadBuffer = pkt->HeaderBuffer + pkt->HeaderBufferSize;

        if ( HeaderBuffer )
            RtlCopyMemory( pkt->HeaderBuffer, HeaderBuffer, HeaderBufferSize );

        if ( LookAheadBuffer )
            RtlCopyMemory( pkt->LookAheadBuffer, LookAheadBuffer, LookAheadBufferSize );

        pKlimPacket->Ref = InBuffPkt_Ref;
        pKlimPacket->Deref = InBuffPkt_Deref;
        pKlimPacket->GetRefCount = InBuffPkt_GetRefCount;
        pKlimPacket->GetSize = InBuffPkt_GetSize;
        pKlimPacket->GetData = InBuffPkt_GetData;
        pKlimPacket->SetStatus = KlimPktSetStatus;

        pKlimPacket->Status = KLIM_PKT_PASS;
        RtlCopyMemory( pKlimPacket->LocalMac, pAdapter->CurrentMacAddr, 6 );
        pKlimPacket->IfIndex = 0;

        pKlimPacket->RefCount = 0;
        pkt->Adapt = pAdapter;
    }

    return pKlimPacket;
}

VOID
InBuffPkt_Initialize( PKLIM_PACKET pKlimPacket, 
                     PVOID HeaderBuffer, 
                     ULONG HeaderBufferSize, 
                     PVOID LookAheadBuffer, 
                     ULONG LookAheadBufferSize,
                     PADAPT pAdapter)
{
    PKLIM_RECV_BUFF_PACKET pkt = (PKLIM_RECV_BUFF_PACKET)pKlimPacket->Packet;

    pkt->HeaderBufferSize = HeaderBufferSize;
    pkt->LookAheadBufferSize = LookAheadBufferSize;
    pkt->HeaderBuffer = HeaderBuffer;
    pkt->LookAheadBuffer = LookAheadBuffer;

    pKlimPacket->RefCount = 0;
    pKlimPacket->Status = KLIM_PKT_PASS;
    RtlCopyMemory( pKlimPacket->LocalMac, pAdapter->CurrentMacAddr, 6 );
    pKlimPacket->IfIndex = 0;

    pKlimPacket->Ref = InBuffPkt_Ref;
    pKlimPacket->Deref = InBuffPkt_Deref;
    pKlimPacket->GetRefCount = InBuffPkt_GetRefCount;
    pKlimPacket->GetSize = InBuffPkt_GetSize;
    pKlimPacket->GetData = InBuffPkt_GetData;
    pKlimPacket->SetStatus = KlimPktSetStatus;
}

PKLIM_PACKET
InBuffPkt_Allocate2( 
                    PNDIS_PACKET NdisPacket,
                    PADAPT pAdapter)
{
    PKLIM_PACKET pKlimPacket = ExAllocateFromNPagedLookasideList( &pAdapter->InBufList );

    if ( pKlimPacket )
    {
        PKLIM_RECV_BUFF_PACKET pkt = (PKLIM_RECV_BUFF_PACKET)pKlimPacket->Packet;
        ULONG PacketSize;

        NDIS_STATUS Status;
        PNDIS_PACKET ndis_pkt;
        PNDIS_BUFFER ndis_buff;
        ULONG dwCopied;

        NdisQueryPacket( NdisPacket, NULL, NULL, NULL, &PacketSize );

        pkt->HeaderBufferSize = 0xE;
        pkt->LookAheadBufferSize = PacketSize - pkt->HeaderBufferSize;
        pkt->HeaderBuffer = pkt->PlainPacket;
        pkt->LookAheadBuffer = pkt->HeaderBuffer + pkt->HeaderBufferSize;

        NdisAllocatePacket( &Status, &ndis_pkt, InPktPacketPool );

        if ( Status != NDIS_STATUS_SUCCESS )
        {
            ExFreeToNPagedLookasideList( &pAdapter->InBufList, pKlimPacket );
            return NULL;
        }

        NdisAllocateBuffer( &Status, &ndis_buff, InPktBufferPool, pkt->PlainPacket, pAdapter->MaxFrameSize );

        if ( Status != NDIS_STATUS_SUCCESS )
        {
            NdisFreePacket( ndis_pkt );
            ExFreeToNPagedLookasideList( &pAdapter->InBufList, pKlimPacket );
            return NULL;
        }

        NdisChainBufferAtFront( ndis_pkt, ndis_buff );

        NdisCopyFromPacketToPacket( ndis_pkt, 0, PacketSize, NdisPacket, 0, &dwCopied );

        NdisUnchainBufferAtFront( ndis_pkt, &ndis_buff );

        NdisFreeBuffer ( ndis_buff );
        NdisFreePacket ( ndis_pkt );


        pKlimPacket->RefCount = 0;
        pkt->Adapt = pAdapter;

        pKlimPacket->Status = KLIM_PKT_PASS;
        RtlCopyMemory( pKlimPacket->LocalMac, pAdapter->CurrentMacAddr, 6 );
        pKlimPacket->IfIndex = 0;

        pKlimPacket->Ref = InBuffPkt_Ref;
        pKlimPacket->Deref = InBuffPkt_Deref;
        pKlimPacket->GetRefCount = InBuffPkt_GetRefCount;
        pKlimPacket->GetSize = InBuffPkt_GetSize;
        pKlimPacket->GetData = InBuffPkt_GetData;
        pKlimPacket->SetStatus = KlimPktSetStatus;
    }

    return pKlimPacket;
}
#include "precomp.h"
#include "..\klim_pkt.h"
#pragma hdrstop

NDIS_HANDLE     InPktPacketPool;
NDIS_HANDLE     InPktBufferPool;

VOID
InPkt_Init()
{
    NDIS_STATUS Status;    

    NdisAllocatePacketPool( &Status, &InPktPacketPool, 200, sizeof ( PVOID ) * 4 );
    NdisAllocateBufferPool( &Status, &InPktBufferPool, 200 );
}

NDIS_STATUS
InPkt_Ref( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;

    if ( 1 < InterlockedIncrement( &p->RefCount ) )
    {
        // кто-то зареференсил пакет.

    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
InPkt_Deref( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;
    PKLIM_RECV_NDIS_PACKET pkt = (PKLIM_RECV_NDIS_PACKET)p->Packet;

    PADAPT pAdapt = pkt->Adapt;

    if ( 0 == InterlockedDecrement( &p->RefCount ) )
    {
        KIRQL Irql;

        KeRaiseIrql(DISPATCH_LEVEL, &Irql);

        switch ( p->Status )
        {
        case KLIM_PKT_PASS:
            // статус пакета NDIS_STATUS_SUCCESS
            // нужно показать свой пакет. ѕосле этого будут приходить MiniportReturnPackets.
            NdisMIndicateReceivePacket(pAdapt->MiniportHandle, &pkt->NdisPacket, 1);
        	break;
        case KLIM_PKT_BLOCK:
            NdisReturnPackets( &pkt->OrigPacket, 1 );
            NdisDprFreePacket( pkt->NdisPacket );
            break;
        }

        KeLowerIrql(Irql);

        ExFreeToNPagedLookasideList( &pAdapt->InPktList, p );                
    }

    return NDIS_STATUS_SUCCESS;
}

ULONG
InPkt_GetRefCount( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;    

    return p->RefCount;
}

ULONG
InPkt_GetSize( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;
    PKLIM_RECV_NDIS_PACKET pkt = (PKLIM_RECV_NDIS_PACKET)p->Packet;

    ULONG Size;

    NdisQueryPacket(pkt->NdisPacket, NULL, NULL, NULL, &Size );

    return Size;
}

ULONG
InPkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count )
{
    ULONG dwCopied;
    NDIS_STATUS Status;
    PNDIS_PACKET NdisPacket;
    PNDIS_BUFFER NdisBuffer;

    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;
    PKLIM_RECV_NDIS_PACKET pkt = (PKLIM_RECV_NDIS_PACKET)p->Packet;

    NdisAllocatePacket( &Status, &NdisPacket, InPktPacketPool );

    NdisAllocateBuffer( &Status, &NdisBuffer, InPktBufferPool, Buffer, count );

    NdisChainBufferAtFront( NdisPacket, NdisBuffer );

    NdisCopyFromPacketToPacket( NdisPacket, 0, count, pkt->NdisPacket, from, &dwCopied );

    NdisUnchainBufferAtFront( NdisPacket, &NdisBuffer );

    NdisFreeBuffer ( NdisBuffer );
    NdisFreePacket ( NdisPacket );

    return dwCopied;
}


PKLIM_PACKET
InPkt_Allocate( 
                    PNDIS_PACKET pNdisPacket,
                    PADAPT pAdapter)
{
    PKLIM_PACKET pKlimPacket = ExAllocateFromNPagedLookasideList( &pAdapter->InPktList );

    if ( pKlimPacket )
    {
        PKLIM_RECV_NDIS_PACKET pkt = (PKLIM_RECV_NDIS_PACKET)pKlimPacket->Packet;
        pkt->Adapt = pAdapter;        

        pkt->NdisPacket = pNdisPacket;
        pKlimPacket->RefCount = 0;

        pKlimPacket->Status = KLIM_PKT_PASS;
        RtlCopyMemory( pKlimPacket->LocalMac, pAdapter->CurrentMacAddr, 6 );
        pKlimPacket->IfIndex = 0;

        pKlimPacket->Ref = InPkt_Ref;
        pKlimPacket->Deref = InPkt_Deref;
        pKlimPacket->GetRefCount = InPkt_GetRefCount;
        pKlimPacket->GetSize = InPkt_GetSize;
        pKlimPacket->GetData = InPkt_GetData;
        pKlimPacket->SetStatus = KlimPktSetStatus;
    }


    return pKlimPacket;
}
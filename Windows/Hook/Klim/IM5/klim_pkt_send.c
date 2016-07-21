#include "precomp.h"
#pragma hdrstop

NDIS_HANDLE     OutNdisPktPacketPool;
NDIS_HANDLE     OutNdisPktBufferPool;


VOID
OutPkt_Initialize()
{
    NDIS_STATUS Status;    

    NdisAllocatePacketPool( &Status, &OutNdisPktPacketPool, 100, sizeof ( PVOID ) * 4 );
    NdisAllocateBufferPool( &Status, &OutNdisPktBufferPool, 100 );
}

ULONG
OutNdisPkt_GetSize( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;
    PKLIM_SEND_PACKET pkt = (PKLIM_SEND_PACKET)p->Packet;

    ULONG Size;

    NdisQueryPacket(pkt->NdisPacket, NULL, NULL, NULL, &Size );

    return Size;
}

ULONG
OutNdisPkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count )
{
    ULONG dwCopied;
    NDIS_STATUS Status;
    PNDIS_PACKET NdisPacket;
    PNDIS_BUFFER NdisBuffer;

    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;
    PKLIM_RECV_NDIS_PACKET pkt = (PKLIM_RECV_NDIS_PACKET)p->Packet;

    NdisAllocatePacket( &Status, &NdisPacket, OutNdisPktPacketPool );

    NdisAllocateBuffer( &Status, &NdisBuffer, OutNdisPktBufferPool, Buffer, count );

    NdisChainBufferAtFront( NdisPacket, NdisBuffer );

    NdisCopyFromPacketToPacket( NdisPacket, 0, count, pkt->NdisPacket, from, &dwCopied );

    NdisUnchainBufferAtFront( NdisPacket, &NdisBuffer );

    NdisFreeBuffer ( NdisBuffer );
    NdisFreePacket ( NdisPacket );

    return dwCopied;
}

NDIS_STATUS
OutNdisPkt_Ref( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;
    PKLIM_SEND_PACKET pkt = (PKLIM_SEND_PACKET)p->Packet;

    if ( 1 < InterlockedIncrement( &p->RefCount ) )
    {
        // кто-то зареференсил пакет.

    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
OutNdisPkt_Deref( PVOID pktConext )
{
    NDIS_STATUS Status = NDIS_STATUS_PENDING;
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;
    PKLIM_SEND_PACKET pkt = (PKLIM_SEND_PACKET)p->Packet;
    PSEND_RSVD SendRsvd = (PSEND_RSVD)(pkt->NdisPacket->ProtocolReserved);

    if ( 0 == InterlockedDecrement( &p->RefCount ) )
    {
        KIRQL Irql;

        KeRaiseIrql(DISPATCH_LEVEL, &Irql);

//#ifdef NDIS51
        
//#else
        switch ( p->Status )
        {
        case KLIM_PKT_PASS:
            {
                // NdisSend( &Status, pkt->Adapt->BindingHandle, pkt->NdisPacket );
                NdisSend(&Status,
                    pkt->Adapt->BindingHandle,
                    pkt->NdisPacket);


                if (Status != NDIS_STATUS_PENDING)
                {                    
                    NdisIMCopySendCompletePerPacketInfo ( SendRsvd->OriginalPkt, pkt->NdisPacket );

                    NdisFreePacket(pkt->NdisPacket);

                    ADAPT_DECR_PENDING_SENDS(pkt->Adapt);

                    NdisMSendComplete(ADAPT_MINIPORT_HANDLE(pkt->Adapt),
                        SendRsvd->OriginalPkt,
                        Status);
                }
            }
            break;
        case KLIM_PKT_BLOCK:
            {                
                NdisIMCopySendCompletePerPacketInfo ( SendRsvd->OriginalPkt, pkt->NdisPacket );

                NdisMSendComplete(ADAPT_MINIPORT_HANDLE(pkt->Adapt),
                    SendRsvd->OriginalPkt,
                    NDIS_STATUS_FAILURE);

                NdisFreePacket(pkt->NdisPacket);

                Status = NDIS_STATUS_NOT_ACCEPTED;

                ADAPT_DECR_PENDING_SENDS(pkt->Adapt);
            }
            break;
        default:
            break;
        }
// #endif

        KeLowerIrql(Irql);
        
        ExFreeToNPagedLookasideList( &pkt->Adapt->OutPktList, p );        
    }    

    return Status;
}

ULONG
OutNdisPkt_GetRefCount( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;   
    PKLIM_SEND_PACKET pkt = (PKLIM_SEND_PACKET)p->Packet;

    return p->RefCount;
}

PKLIM_PACKET
OutNdisPkt_Allocate( 
                   PNDIS_PACKET pNdisPacket,
                   PADAPT pAdapter)
{
    PKLIM_PACKET pKlimPacket = ExAllocateFromNPagedLookasideList( &pAdapter->OutPktList );

    if ( pKlimPacket )
    {        
        PKLIM_SEND_PACKET pkt = (PKLIM_SEND_PACKET)pKlimPacket->Packet;
        PSEND_RSVD SendRsvd = (PSEND_RSVD)(pNdisPacket->ProtocolReserved);

        pkt->Adapt = pAdapter;
        pkt->OrigPacket = SendRsvd->OriginalPkt;

        pkt->NdisPacket = pNdisPacket;
        pKlimPacket->RefCount = 0;
        RtlCopyMemory( pKlimPacket->LocalMac, pAdapter->CurrentMacAddr, 6 );
        pKlimPacket->IfIndex = 0;

        pKlimPacket->Ref = OutNdisPkt_Ref;
        pKlimPacket->Deref = OutNdisPkt_Deref;
        pKlimPacket->GetRefCount = OutNdisPkt_GetRefCount;
        pKlimPacket->GetSize = OutNdisPkt_GetSize;
        pKlimPacket->GetData = OutNdisPkt_GetData;
        pKlimPacket->SetStatus = KlimPktSetStatus;

        pKlimPacket->Status = KLIM_PKT_PASS;
    }

    return pKlimPacket;
}

VOID
OutNdisPkt_Initialize( PKLIM_PACKET pKlimPacket, 
                      PNDIS_PACKET pNdisPacket,
                      PADAPT pAdapter)
{
    PKLIM_SEND_PACKET pkt = (PKLIM_SEND_PACKET)pKlimPacket->Packet;

    pkt->Adapt = pAdapter;
    pkt->NdisPacket = pNdisPacket;
    pKlimPacket->RefCount = 0;
    RtlCopyMemory( pKlimPacket->LocalMac, pAdapter->CurrentMacAddr, 6 );

    pKlimPacket->Ref = OutNdisPkt_Ref;
    pKlimPacket->Deref = OutNdisPkt_Deref;
    pKlimPacket->GetRefCount = OutNdisPkt_GetRefCount;
    pKlimPacket->GetSize = OutNdisPkt_GetSize;
    pKlimPacket->GetData = OutNdisPkt_GetData;
    pKlimPacket->SetStatus = KlimPktSetStatus;

    pKlimPacket->Status = KLIM_PKT_PASS;
}
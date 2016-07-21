#include "precomp.h"

VOID
InPkt_Init()
{
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
    PKLIM6_RECV_PACKET pkt = (PKLIM6_RECV_PACKET)p->Packet;

    PMS_FILTER pFilter = pkt->Filter;
    ULONG ReturnFlags;

    if ( 0 == InterlockedDecrement( &p->RefCount ) )
    {
        KIRQL Irql;

        KeRaiseIrql(DISPATCH_LEVEL, &Irql);

        switch ( p->Status )
        {
        case KLIM_PKT_PASS:
            // статус пакета NDIS_STATUS_SUCCESS
            // нужно показать свой пакет. ѕосле этого будут приходить MiniportReturnPackets.
            NdisFIndicateReceiveNetBufferLists(
                pFilter->FilterHandle,
                pkt->NetBufferList,
                pkt->PortNumber, 
                1,
                (pkt->ReceiveFlags & ~NDIS_RECEIVE_FLAGS_RESOURCES) | NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL );

            break;
        case KLIM_PKT_BLOCK:

            if (NDIS_TEST_RECEIVE_CAN_PEND(pkt->ReceiveFlags))
            {   
                ReturnFlags = NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL;

                NdisFReturnNetBufferLists(pFilter->FilterHandle, pkt->NetBufferList, ReturnFlags);
            }
            else
            {
                filterFreeNetBufferList(pFilter, pkt->NetBufferList);
            }
            break;
        }

        KeLowerIrql(Irql);

        ExFreeToNPagedLookasideList( &pFilter->InPktList, p );                
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
    PKLIM6_RECV_PACKET pkt = (PKLIM6_RECV_PACKET)p->Packet;

    if (pkt->NetBufferList && pkt->NetBufferList->FirstNetBuffer)
    {
        return pkt->NetBufferList->FirstNetBuffer->DataLength;
    }

    return 0;
}

ULONG
InPkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count )
{
    ULONG               BytesCopied = 0;
    PMDL                pMdl;
    PNET_BUFFER_LIST    pNetBufList;

    PKLIM_PACKET        p = (PKLIM_PACKET)pktContext;
    PKLIM6_RECV_PACKET  pkt = (PKLIM6_RECV_PACKET)p->Packet;
    PMS_FILTER          pFilter = pkt->Filter;

    pMdl = NdisAllocateMdl(pFilter->FilterHandle, Buffer, count);
    if (pMdl)
    {
        pNetBufList = NdisAllocateNetBufferAndNetBufferList(
            pFilter->NetBufferListPool,
            0,                              // ContextSize
            0,                              // ContextBackfill
            pMdl,                           // MdlChain
            0,                              // DataOffset
            count);                         // DataLength
        if (pNetBufList)
        {
            NdisCopyFromNetBufferToNetBuffer(pNetBufList->FirstNetBuffer, 0, count, pkt->NetBufferList->FirstNetBuffer, from, &BytesCopied);

            NdisFreeNetBufferList(pNetBufList);
        }
        NdisFreeMdl(pMdl);
    }
    return BytesCopied;
}

NTSTATUS
Klim6QueryPacket( PVOID pktContext, PVOID Buffer, ULONG Size, ULONG* dwRet )
{
    if ( dwRet )
        *dwRet = 0;
    return STATUS_UNSUCCESSFUL;
}


PKLIM_PACKET
InPkt_Allocate( PMS_FILTER        pFilter,
               PNET_BUFFER_LIST  pNetBufferList,
               NDIS_PORT_NUMBER  PortNumber,
               ULONG             ReceiveFlags
               )
{
    PNET_BUFFER_LIST        NetBufferList;
    PKLIM_PACKET            pKlimPacket = 0;
    ULONG                   TotalLength;
    ULONG                   BytesCopied;
    NTSTATUS                NtStatus;
    PKLIM6_RECV_PACKET      pkt = 0;

    do
    {
        pKlimPacket = ExAllocateFromNPagedLookasideList( &pFilter->InPktList );

        if (!pKlimPacket)
        {
            break;
        }

        if (NDIS_TEST_RECEIVE_CANNOT_PEND(ReceiveFlags))
        {
            TotalLength = pNetBufferList->FirstNetBuffer->DataLength;
            NetBufferList = filterAllocateNetBufferList(pFilter, TotalLength); 

            if (!NetBufferList)
            {
                ExFreeToNPagedLookasideList( &pFilter->InPktList, pKlimPacket );                
                pKlimPacket = 0;
                break;
            }

            NtStatus = NdisCopyFromNetBufferToNetBuffer(NetBufferList->FirstNetBuffer,
                0,
                TotalLength,
                pNetBufferList->FirstNetBuffer,
                0,
                &BytesCopied);

            if (NtStatus != STATUS_SUCCESS)
            {
                DEBUGP(DL_FATAL, ("ReceiveNetBufferList: Open %p, failed to"
                    " copy the data, %d bytes\n", pOpenContext, TotalLength));
                //
                // Free the NetBufferList and memory allocate before
                //
                filterFreeNetBufferList(pFilter, NetBufferList);

                ExFreeToNPagedLookasideList( &pFilter->InPktList, pKlimPacket );                
                pKlimPacket = 0;

                break;   
            }

            FILTER_ASSERT(BytesCopied == TotalLength);

            NetBufferList->FirstNetBuffer->DataLength = BytesCopied;
            NetBufferList->SourceHandle = pFilter->FilterHandle;
        }
        else
        {
            NetBufferList = pNetBufferList;
        }

        pkt = (PKLIM6_RECV_PACKET)pKlimPacket->Packet;
        pkt->Filter = pFilter;
        pkt->NetBufferList = NetBufferList;
        pkt->PortNumber = PortNumber;
        pkt->ReceiveFlags = ReceiveFlags & ~NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL;

        pKlimPacket->RefCount = 0;
        RtlCopyMemory( pKlimPacket->LocalMac, pFilter->CurrentMacAddr, 6 );
        pKlimPacket->IfIndex = pFilter->MiniportIfIndex;

        pKlimPacket->Status = KLIM_PKT_PASS;

        pKlimPacket->Ref = InPkt_Ref;
        pKlimPacket->Deref = InPkt_Deref;
        pKlimPacket->GetRefCount = InPkt_GetRefCount;
        pKlimPacket->GetSize = InPkt_GetSize;
        pKlimPacket->GetData = InPkt_GetData;
        pKlimPacket->SetStatus = KlimPktSetStatus;        
    }
    while(FALSE);

    return pKlimPacket;
}
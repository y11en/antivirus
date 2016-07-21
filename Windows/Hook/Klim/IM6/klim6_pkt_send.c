#include "precomp.h"

VOID 
OutPkt_Init()
{
}

NDIS_STATUS
OutPkt_Ref( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;

    if ( 1 < InterlockedIncrement( &p->RefCount ) )
    {
        // кто-то зареференсил пакет.

    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
OutPkt_Deref( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;
    PKLIM6_SEND_PACKET pkt = (PKLIM6_SEND_PACKET)p->Packet;

    PMS_FILTER pFilter = pkt->Filter;
    ULONG SendCompleteFlags;

    if ( 0 == InterlockedDecrement( &p->RefCount ) )
    {
        KIRQL Irql;

        KeRaiseIrql(DISPATCH_LEVEL, &Irql);

        switch ( p->Status )
        {
        case KLIM_PKT_PASS:

            NdisFSendNetBufferLists(
                        pFilter->FilterHandle, 
                        pkt->NetBufferList, 
                        pkt->PortNumber, 
                        pkt->SendFlags | NDIS_SEND_FLAGS_DISPATCH_LEVEL );

            break;
        case KLIM_PKT_BLOCK:

            if (filterIsMineNetBufferList(pFilter, pkt->NetBufferList))
            {
                filterFreeNetBufferList(pFilter, pkt->NetBufferList);
            }
            else
            {
                SendCompleteFlags = NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL;
                
                NdisFSendNetBufferListsComplete(
                            pFilter->FilterHandle,
                            pkt->NetBufferList,
                            SendCompleteFlags );
                
            }

            break;

        }

        KeLowerIrql(Irql);

        ExFreeToNPagedLookasideList( &pFilter->OutPktList, p );                
    }

    return NDIS_STATUS_SUCCESS;
}

ULONG
OutPkt_GetRefCount( PVOID pktConext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktConext;    

    return p->RefCount;
}

ULONG
OutPkt_GetSize( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;    	
    PKLIM6_SEND_PACKET pkt = (PKLIM6_SEND_PACKET)p->Packet;

    if (pkt->NetBufferList && pkt->NetBufferList->FirstNetBuffer)
    {
        return pkt->NetBufferList->FirstNetBuffer->DataLength;
    }

    return 0;
}

ULONG
OutPkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count )
{
    ULONG               BytesCopied = 0;
    PMDL                pMdl;
    PNET_BUFFER_LIST    pNetBufList;

    PKLIM_PACKET        p = (PKLIM_PACKET)pktContext;    	
    PKLIM6_SEND_PACKET  pkt = (PKLIM6_SEND_PACKET)p->Packet;
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


PKLIM_PACKET
OutPkt_Allocate( PMS_FILTER        pFilter,
                 PNET_BUFFER_LIST  pNetBufferList,
                 PNET_BUFFER	   pNetBuffer,
                 NDIS_PORT_NUMBER  PortNumber,
                 ULONG             SendFlags
                )
{
    PNET_BUFFER_LIST        NetBufferList;
    PKLIM_PACKET            pKlimPacket = 0;
    ULONG                   TotalLength;
    ULONG                   BytesCopied;
    NTSTATUS                NtStatus;
    PKLIM6_SEND_PACKET      pkt = 0;

    do
    {
        pKlimPacket = ExAllocateFromNPagedLookasideList( &pFilter->OutPktList );

        if (!pKlimPacket)
        {
            break;
        }

        if (pNetBuffer)
        {
            NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO info;
            TotalLength = pNetBuffer->DataLength;
            NetBufferList = filterAllocateNetBufferList(pFilter, TotalLength); 

            if (!NetBufferList)
            {
                ExFreeToNPagedLookasideList( &pFilter->OutPktList, pKlimPacket );                
                pKlimPacket = 0;
                break;
            }

            NtStatus = NdisCopyFromNetBufferToNetBuffer(NetBufferList->FirstNetBuffer,
                                                        0,
                                                        TotalLength,
                                                        pNetBuffer,
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

                ExFreeToNPagedLookasideList( &pFilter->OutPktList, pKlimPacket );                
                pKlimPacket = 0;

                break;   
            }

            FILTER_ASSERT(BytesCopied == TotalLength);
            
            NetBufferList->FirstNetBuffer->DataLength = BytesCopied;
            NetBufferList->SourceHandle = pFilter->FilterHandle;

            info = *(PNDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO)&NET_BUFFER_LIST_INFO (pNetBufferList, TcpIpChecksumNetBufferListInfo);
            
            NET_BUFFER_LIST_INFO (NetBufferList, TcpIpChecksumNetBufferListInfo) = 
                NET_BUFFER_LIST_INFO (pNetBufferList, TcpIpChecksumNetBufferListInfo);

            NET_BUFFER_LIST_INFO (NetBufferList, NetBufferListProtocolId) = 
                NET_BUFFER_LIST_INFO (pNetBufferList, NetBufferListProtocolId);
        }
        else
        {
            NetBufferList = pNetBufferList;
            NetBufferList->Status = NDIS_STATUS_PENDING;
        }

        pkt = (PKLIM6_SEND_PACKET)pKlimPacket->Packet;
        pkt->Filter = pFilter;
        pkt->NetBufferList = NetBufferList;
        pkt->PortNumber = PortNumber;
        pkt->SendFlags = SendFlags & ~NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL;

        pKlimPacket->RefCount = 0;
        RtlCopyMemory( pKlimPacket->LocalMac, pFilter->CurrentMacAddr, 6 );
        pKlimPacket->IfIndex = pFilter->MiniportIfIndex;

        pKlimPacket->Status = KLIM_PKT_PASS;

        pKlimPacket->Ref = OutPkt_Ref;
        pKlimPacket->Deref = OutPkt_Deref;
        pKlimPacket->GetRefCount = OutPkt_GetRefCount;
        pKlimPacket->GetSize = OutPkt_GetSize;
        pKlimPacket->GetData = OutPkt_GetData;
        pKlimPacket->SetStatus = KlimPktSetStatus;
    }
    while(FALSE);

    return pKlimPacket;
}


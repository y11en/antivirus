#include "precomp.h"
#include "sendpkt.h"
#pragma hdrstop

extern NDIS_HANDLE     OutNdisPktPacketPool;
extern NDIS_HANDLE     OutNdisPktBufferPool;

unsigned char BroadcastMac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

unsigned short ip_sum(unsigned short *ptr,int nbytes) 
{
    register long           sum;
    USHORT                 oddbyte;
    register USHORT        answer;

    sum = 0;
    while (nbytes > 1)  
    {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) 
    {
        oddbyte = 0;
        *((UCHAR *) &oddbyte) = *(UCHAR *)ptr;   /* one byte only */
        sum += oddbyte;
    }

    sum  = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);                 
    answer = (USHORT) ~sum;
    return(answer);
}

BOOLEAN
IsZeroMac( char MacAddr[6] )
{
    if ( MacAddr[0] == 0 && MacAddr[1] == 0 &&
         MacAddr[2] == 0 && MacAddr[3] == 0 &&
         MacAddr[4] == 0 && MacAddr[5] == 0 )
    {
        return TRUE;
    }
    return FALSE;
}

ULONG
KlimSendPacket( PVOID pPacketBuf, ULONG Size )
{
    NDIS_STATUS Status;
    PMS_FILTER   pFilter;    
    PLIST_ENTRY  pLE;
    PNET_BUFFER_LIST        NetBufferList, NetBufferLists = NULL;

    PMDL                pMdl;
    PNET_BUFFER_LIST    pNetBufList;
    ULONG               BytesCopied = 0;

    PETH_HEADER eth_base  = (PETH_HEADER)pPacketBuf;

    FILTER_ACQUIRE_LOCK(&FilterListLock, FALSE);

    for ( pLE = FilterModuleList.Flink; pLE != &FilterModuleList; pLE = pLE->Flink )
    {
        PVOID   pVA = ExAllocatePoolWithTag(NonPagedPool, Size, '1234') ;
        PETH_HEADER eth  = (PETH_HEADER)pVA;
        PIP_HEADER  ip   = (PIP_HEADER) ( (char*)pVA + sizeof ( ETH_HEADER ) );
        pFilter = CONTAINING_RECORD( pLE, MS_FILTER, FilterModuleLink );

        if ( !IsZeroMac(eth_base->srcMac) )
        {
            // если пробит конкретный MAC, то пакет отправляем с фиксированного интерфейса.
            if ( 6 != RtlCompareMemory(eth->srcMac, pFilter->CurrentMacAddr, 6 ) )
            {
                ExFreePool( pVA );
                continue;
            }
        }

        RtlCopyMemory( pVA, pPacketBuf, Size );

        RtlCopyMemory ( eth->srcMac, pFilter->CurrentMacAddr, 6 );
        ip->srcIP           = pFilter->CurrentIP;

        ip->HeaderChecksum  = ip_sum( (USHORT*)ip, IP_HEADER_SIZE);
        
        NetBufferList = filterAllocateNetBufferList(pFilter, Size); 

        pMdl = NdisAllocateMdl(pFilter->FilterHandle, pVA, Size);

        pNetBufList = NdisAllocateNetBufferAndNetBufferList(
                                    pFilter->NetBufferListPool,
                                    0,                              // ContextSize
                                    0,                              // ContextBackfill
                                    pMdl,                           // MdlChain
                                    0,                              // DataOffset
                                    Size);                         // DataLength

        NdisCopyFromNetBufferToNetBuffer( NetBufferList->FirstNetBuffer, 0, Size, pNetBufList->FirstNetBuffer, 0, &BytesCopied);

        NdisFreeNetBufferList(pNetBufList);

        NdisFreeMdl(pMdl);

        // Запоминаем pFilter. перед отправкой перебить на pFilter->FilterHandle
        NetBufferList->SourceHandle = pFilter;

        /*
        NdisFSendNetBufferLists(
            pFilter->FilterHandle, 
            NetBufferList, 
            pFilter->PortArray.Ports[0].PortNumber, 
            NDIS_SEND_FLAGS_DISPATCH_LEVEL );
            */
        NetBufferList->Next = NetBufferLists;
        NetBufferLists = NetBufferList;


    }
    
    FILTER_RELEASE_LOCK(&FilterListLock, FALSE);

    while ( NetBufferLists )
    {   
        NetBufferList = NetBufferLists->Next;
        NetBufferLists->Next = 0;

        pFilter = NetBufferLists->SourceHandle;
        NetBufferLists->SourceHandle = pFilter->FilterHandle;

        NdisFSendNetBufferLists(
            pFilter->FilterHandle, 
            NetBufferLists, 
            pFilter->PortArray.Ports[0].PortNumber, 
            NDIS_SEND_FLAGS_DISPATCH_LEVEL );

        NetBufferLists = NetBufferList;
    }    

    return 0;
}
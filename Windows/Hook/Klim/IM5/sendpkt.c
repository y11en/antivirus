#include "precomp.h"
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
    PNDIS_PACKET NdisPacket;
    PNDIS_BUFFER NdisBuffer;
    PADAPT       *ppCursor;
    PSEND_RSVD   SendRsvd;

    PETH_HEADER eth_base  = (PETH_HEADER)pPacketBuf;

    PSEND_RSVD   Packets = NULL;
    PSEND_RSVD   Next;

    NdisAcquireSpinLock(&GlobalLock);

    for (ppCursor = &pAdaptList; *ppCursor != NULL; ppCursor = &(*ppCursor)->Next)
    {        
        PVOID   pVA = ExAllocatePoolWithTag(NonPagedPool, Size, '1234') ;

        if ( pVA )
        {
            PETH_HEADER eth  = (PETH_HEADER)pVA;
            PIP_HEADER  ip   = (PIP_HEADER) ( (char*)pVA + sizeof ( ETH_HEADER ) );

            if ( !IsZeroMac(eth_base->srcMac) )
            {
                // если пробит конкретный MAC, то пакет отправляем с фиксированного интерфейса.
                if ( 0 != memcmp(eth->srcMac, (*ppCursor)->CurrentMacAddr, 6 ) )
                {
                    ExFreePool( pVA );
                    continue;
                }
            }

            RtlCopyMemory( pVA, pPacketBuf, Size );

            NdisAllocatePacket( &Status, &NdisPacket, OutNdisPktPacketPool );

            if ( Status != NDIS_STATUS_SUCCESS )
            {
                return 0;
            }

            NdisAllocateBuffer( &Status, &NdisBuffer, OutNdisPktBufferPool, pVA, Size );

            if ( Status != NDIS_STATUS_SUCCESS )
            {
                NdisFreeBuffer( NdisBuffer );
                ExFreePool( pVA );
                return 0;
            }

            
            RtlCopyMemory ( eth->srcMac, (*ppCursor)->CurrentMacAddr, 6 );

            ip->srcIP           = (*ppCursor)->CurrentIP;

            ip->HeaderChecksum  = ip_sum( (USHORT*)ip, IP_HEADER_SIZE);

            NdisChainBufferAtFront( NdisPacket, NdisBuffer );

            SendRsvd = (PSEND_RSVD)(NdisPacket->ProtocolReserved);
            SendRsvd->Tag           = KLIM_PKT_HAND_MADE;
            SendRsvd->SendHandle    = (*ppCursor)->BindingHandle;
            SendRsvd->OriginalPkt   = NdisPacket;
            
            SendRsvd->Next = Packets;
            Packets = SendRsvd;

            /*
            NdisSend( &Status, (*ppCursor)->BindingHandle, NdisPacket );

            if ( Status != NDIS_STATUS_PENDING )
            {            
                PVOID           pVA;
                UINT           Length;

                NdisUnchainBufferAtFront( NdisPacket, &NdisBuffer );

                NdisQueryBuffer( NdisBuffer, &pVA, &Length );

                NdisFreeBuffer(NdisBuffer);
                ExFreePool ( pVA );

                NdisFreePacket ( NdisPacket );
            }
            */
        }        
    }

    NdisReleaseSpinLock(&GlobalLock);


    while ( Packets )
    {
        Next = Packets->Next;
        NdisPacket = Packets->OriginalPkt;

        NdisSend( &Status, Packets->SendHandle, NdisPacket );

        if ( Status != NDIS_STATUS_PENDING )
        {            
            PVOID           pVA;
            UINT           Length;

            NdisUnchainBufferAtFront( NdisPacket, &NdisBuffer );

            NdisQueryBuffer( NdisBuffer, &pVA, &Length );

            NdisFreeBuffer(NdisBuffer);
            ExFreePool ( pVA );

            NdisFreePacket ( NdisPacket );
        }

        Packets = Next;
    }

    return 0;
}
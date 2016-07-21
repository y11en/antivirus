#include "CustomPacket.h"
#include "..\g_custom_packet.h"
#include "..\g_cp_proto.h"

USHORT	dwLastID = 1;

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

KLIM_PKT_STATUS
ProcessInPacket( PVOID pkt )
{
    ULONG       PktSize;
    ETH_HEADER eth;
    IP_HEADER  ip;
    PCP_PACKET*	Packet;
    char        Buffer[100];
    char LocalMac[6];

    Packet = (PCP_PACKET*)Buffer;

    if ( ext_KlimTable == NULL )
        return KLIM_PKT_PASS;
    
    ext_KlimTable->GetData( pkt, &eth, 0, sizeof(eth) );

    if ( eth.Type == htons ( ETH_P_IP ) )
    {
        PktSize = ext_KlimTable->GetSize( pkt );
        ext_KlimTable->GetData( pkt, &ip, sizeof(eth), IP_HEADER_SIZE );
        
        if ( ip.Protocol == CP_PROTOCOL )
        {
            ULONG from = 0xE + ip.Ihl * sizeof ( ULONG );
            ext_KlimTable->GetData( pkt, Packet, from, min ( (PktSize - from), 100) );            

            if ( Packet->version != PCP_VERSION )
                return KLIM_PKT_BLOCK;

            ext_KlimTable->GetLocalMacFromPacket( pkt, LocalMac );

            if ( Packet->type == PCP_REQUEST_PACKET )
                PCP_RecvRequest( LocalMac, &eth, Packet );

            // if ( CurrentTime > g_StopTime )
            //    return KL_SUCCESS;

            if ( Packet->type == PCP_ANSWER_PACKET )
                PCP_RecvAnswer( LocalMac, &eth, Packet);

            ext_KlimTable->SetStatus( pkt, KLIM_PKT_BLOCK );
            return KLIM_PKT_BLOCK;
        }
    }

    return KLIM_PKT_PASS;
}

KLIM_PKT_STATUS
ProcessOutPacket( PVOID pkt )
{
    return KLIM_PKT_PASS;
}


KLSTATUS
CP_Init()
{    
    return KL_SUCCESS;
}


KLSTATUS
CP_AddAdapter( CP_ADAPTER_INFO* pAI )
{
    // все адаптеры, через которые мы рассылаем, находятся уже в Klim-е
    return KL_UNSUPPORTED;
}

KLSTATUS
CP_SetBroadcast(CP_IFACE_INFO* pII, ULONG Count )
{
    if ( ext_KlimTable )
    {
        for ( ULONG i = 0; i < Count; i++ )
        {   
            ext_KlimTable->SetBroadcastIp( pII[i].MacAddr, pII[i].IpAddr );
        }

        return KL_SUCCESS;
    }
    
    return KL_UNSUCCESSFUL;
}

KLSTATUS
CP_Done()
{
    return KL_SUCCESS;
}

KLSTATUS
CP_Send(char* Packet, ULONG PacketSize)
{
    return KL_SUCCESS;
}

unsigned char BroadcastMac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

KLSTATUS
CP_SendLicBuffer(char* Buffer, ULONG BufferSize)
{
    ULONG   PacketSize = sizeof ( ETH_HEADER) + IP_HEADER_SIZE + BufferSize;
    PVOID   pVA = KL_MEM_ALLOC ( PacketSize + 100 );

    if ( pVA == NULL )
        return KL_UNSUCCESSFUL;

    RtlZeroMemory( pVA, PacketSize + 100 );

    PETH_HEADER eth  = (PETH_HEADER)pVA;
    PIP_HEADER  ip   = (PIP_HEADER) ( (char*)pVA + sizeof ( ETH_HEADER ) );
    PVOID       data = (PVOID)      ( (char*)ip  + IP_HEADER_SIZE );

    //RtlCopyMemory ( eth->srcMac, OI->CurrentMacAddr, sizeof ( OI->CurrentMacAddr ) );
    RtlCopyMemory ( eth->dstMac, BroadcastMac , sizeof( BroadcastMac ) );
    eth->Type       = htons ( ETH_P_IP );

    // ip->srcIP           = OI->CurrentIP;
    ip->dstIP           = 0xFFFFFFFF;
    ip->Ihl             = 5;
    ip->Version         = 4;
    ip->Protocol        = CP_PROTOCOL;
    ip->TimeToLive      = 255;
    ip->TypeOfService   = 0;
    ip->Flg_FragOffs    = 0;
    ip->TotalLength     = htons ( ((USHORT)( IP_HEADER_SIZE + BufferSize)) );
    ip->Identification  = htons ( dwLastID++ );
    ip->HeaderChecksum  = 0;
    ip->HeaderChecksum  = ip_sum( (USHORT*)ip, IP_HEADER_SIZE);        

    RtlCopyMemory ( data, Buffer, BufferSize );

    if ( ext_KlimTable )
        ext_KlimTable->SendPacket( pVA, PacketSize );

    KL_MEM_FREE( pVA );

    return KL_SUCCESS;
}

bool
IsLocalMac( char Mac[6] )
{
    if ( ext_KlimTable )
    {
        return ( TRUE == ext_KlimTable->IsLocalMac( Mac ) );
    }
    return true;
}
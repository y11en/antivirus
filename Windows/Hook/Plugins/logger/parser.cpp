#include "kldef.h"
#include "parser.h"
#include "descriptions.h"

PLUGIN_CALLBACK ParserTable[] = {	
    { CB_PACKET_RECEIVE,       CKl_PacketParcer::ReceiveFrame,          PRIORITY_NORMAL },
    { PACKET_IP,               CKl_PacketParcer::ReceiveIP,             PRIORITY_NORMAL }    
};

CKl_PacketParcer::CKl_PacketParcer()
{
    AddTable(ParserTable, sizeof ( ParserTable) / sizeof (PLUGIN_CALLBACK) );        
    m_Description   = DESCRIPTON_PACKET_PARSER;
    Ref();
}

CKl_PacketParcer::~CKl_PacketParcer()
{
    Unregister();
    
    Deref();    
}

KLSTATUS
KL_CALLBACK
CKl_PacketParcer::ReceiveFrameDispatch(PVOID pktContext, PVOID  plContext)
{
    return KL_DELAY_REQUEST;
}

KLSTATUS
KL_CALLBACK
CKl_PacketParcer::ReceiveFrame(PVOID pktContext, PVOID  plContext)
{
    BEGIN_DATA_PROCESSING(plContext);

    KLSTATUS            klStatus    = KL_SUCCESS;
    CKl_PacketParcer*   _This       = (CKl_PacketParcer*)plContext;

    PETH_HEADER      EthHdr = (PETH_HEADER)((PKLICK_INFO)(_This->m_ImportTable))->GetFrameHeaderPtr(pktContext);
    
	if ( EthHdr == NULL )
	{
		END_DATA_PROCESSING();
		return KL_SUCCESS;
	}   

    switch( htons( EthHdr->Type ) ) 
    {
    case ETH_P_IP:
        klStatus = _This->EventNotify( PACKET_IP, sizeof(void*), (void*)&DataPacket );        
    	break;
    case ETH_P_ARP:
        klStatus = _This->EventNotify( PACKET_ARP, sizeof(void*), (void*)&DataPacket );
    	break;
    default:
        break;
    }

    END_DATA_PROCESSING();
    return klStatus;
}

KLSTATUS
KL_CALLBACK
CKl_PacketParcer::ReceiveIP(PVOID plContext, ULONG DataSize, PVOID pData)
{
    KLSTATUS            klStatus    = KL_SUCCESS;
    CKl_PacketParcer*   MyParser    = (CKl_PacketParcer*)plContext;
    CKl_DataPacket*     DataPacket  = (CKl_DataPacket*)pData;

    IP_HEADER           IpHeader;

    // если пакет не полный, то парсер его разобрать не сможет. Пропустим.
    if ( !DataPacket->m_isComplete )
        return KL_SUCCESS;

    if ( KL_SUCCESS != DataPacket->GetIpHdr((char*)&IpHeader) )
    {
        return KL_UNSUCCESSFUL;
    }
    
    DataPacket->m_IpHdr = &IpHeader;

    if (IpHeader.Version != 4)
        return KL_SUCCESS;

    switch( IpHeader.Protocol ) 
    {
    case IP_PROTO_TCP:        
        klStatus = MyParser->EventNotify( PROTOCOL_TCP, sizeof(DataPacket), (void*)DataPacket );
    	break;
    case IP_PROTO_UDP:        
        klStatus = MyParser->EventNotify( PROTOCOL_UDP, sizeof(DataPacket), (void*)DataPacket );
    	break;
    case IP_PROTO_ICMP:
        klStatus = MyParser->EventNotify( PROTOCOL_ICMP, sizeof(DataPacket), (void*)DataPacket );
        break;
    default:
        klStatus = MyParser->EventNotify( CHECK_IP_DATA, sizeof (void*), (void*)DataPacket );
        break;
    }

    return klStatus;
}

KLSTATUS
KL_CALLBACK
CKl_PacketParcer::ReceiveTCP(PVOID plContext, ULONG DataSize, PVOID pData)
{
    KLSTATUS            klStatus    = KL_SUCCESS;
    CKl_PacketParcer*   MyParser    = (CKl_PacketParcer*)plContext;
    CKl_DataPacket*     DataPacket  = (CKl_DataPacket*)pData;

    TCP_HEADER          TcpHeader;
    
    DataPacket->GetBuffer((char*)&TcpHeader, 0, sizeof ( TCP_HEADER ) );
    DataPacket->m_DataSize      = DataPacket->GetPacketSize() - DataPacket->m_IpOffset - sizeof( TCP_HEADER );
    DataPacket->m_DataOffset    = sizeof( TCP_HEADER );
    DataPacket->m_TcpHdr        = &TcpHeader;

    klStatus = MyParser->EventNotify( CHECK_TCP_DATA, sizeof (void*), (void*)DataPacket );

    return klStatus;
}

KLSTATUS
KL_CALLBACK
CKl_PacketParcer::ReceiveUDP(PVOID plContext, ULONG DataSize, PVOID pData)
{
    KLSTATUS            klStatus    = KL_SUCCESS;
    CKl_PacketParcer*   MyParser    = (CKl_PacketParcer*)plContext;
    CKl_DataPacket*     DataPacket  = (CKl_DataPacket*)pData;

    UDP_HEADER          UdpHeader;
    
    DataPacket->GetBuffer((char*)&UdpHeader, 0, sizeof ( UDP_HEADER ) );
    DataPacket->m_DataSize = DataPacket->GetPacketSize() - DataPacket->m_IpOffset - sizeof( UDP_HEADER );
    DataPacket->m_DataOffset    = sizeof( UDP_HEADER );
    DataPacket->m_UdpHdr    = &UdpHeader;

    klStatus = MyParser->EventNotify( CHECK_UDP_DATA, sizeof (void*), (void*)DataPacket );

    return klStatus;
}

KLSTATUS
KL_CALLBACK
CKl_PacketParcer::ReceiveICMP(PVOID plContext, ULONG DataSize, PVOID pData)
{    
    CKl_PacketParcer*   MyParser = (CKl_PacketParcer*)plContext;

    return KL_SUCCESS;
}
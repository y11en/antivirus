// Protocols.h: interface for the Protocols class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOLS_H__517BD0F2_5F83_4191_82E8_D71B003E80B2__INCLUDED_)
#define AFX_PROTOCOLS_H__517BD0F2_5F83_4191_82E8_D71B003E80B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma pack( push, 1)

#define ETH_PROTO_IP        0x0008
#define ETH_PROTO_IPV6      0xdd86

#define IP_PROTO_TCP        0x06
#define IP_PROTO_UDP        0x11
#define IP_PROTO_ICMP       0x01
#define IP_PROTO_ICMPV6     0x3A

typedef struct _ethhdr 
{
	unsigned char	h_dest[6];	/* destination eth addr	*/
	unsigned char	h_source[6];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
	unsigned char	data[0];
}EthHdr, *PEthHdr;

typedef struct	_IP_HEADER {	
	UCHAR	Ihl		: 4;
	UCHAR	Version : 4;
	
	UCHAR	TypeOfService;
	USHORT	TotalLength;
	USHORT  Identification;
	USHORT	Flg_FragOffs;
	UCHAR	TimeToLive;
	UCHAR	Protocol;
	USHORT	HeaderChecksum;
	ULONG	srcIP;
	ULONG	dstIP;
} IP_HEADER, *PIP_HEADER;

#define IPV6_HDR_TRAFFIC_CLASS(_p_hdr_) (((_p_hdr_)->TrafficClassHi<<4) | (_p_hdr_)->TrafficClassLo )
#define IPV6_HDR_FLOW_LABEL(_p_hdr_) (((ULONG)(_p_hdr_)->FlowLabelHi<<16) | (ULONG)ntohs((_p_hdr_)->FlowLabelLo) )

typedef struct _IPV6_HEADER {
    
    UCHAR   TrafficClassHi : 4;    
    UCHAR   Version     : 4;

    UCHAR   FlowLabelHi   : 4;
    UCHAR   TrafficClassLo : 4;
        
    USHORT   FlowLabelLo;

    USHORT  PayloadLength;
    UCHAR   NextHeader;
    UCHAR   HopLimit;
    USHORT   SrcIP[8];
    USHORT   DstIP[8];
} IPV6_HEADER, *PIPV6_HEADER;


typedef struct _TCP_HEADER {
	USHORT	srcPort;
	USHORT	dstPort;
	ULONG	SeqNumber;
	ULONG	AckNumber;
	
	USHORT	res1:4;
	USHORT	DataOffset:4;
	USHORT	fin:1;
	USHORT	syn:1;
	USHORT	rst:1;
	USHORT	psh:1;
	USHORT	ack:1;
	USHORT	urg:1;
	USHORT	ece:1;
	USHORT	cwr:1;
	
	USHORT	Window;
	USHORT	CheckSum;
	USHORT	UrgPointer;
} TCP_HEADER, *PTCP_HEADER;

typedef struct _UDP_HEADER {
	USHORT	srcPort;
	USHORT	dstPort;
	USHORT	Length;
	USHORT	CheckSum;
} UDP_HEADER, *PUDP_HEADER;

#pragma pack( pop )

#endif // !defined(AFX_PROTOCOLS_H__517BD0F2_5F83_4191_82E8_D71B003E80B2__INCLUDED_)

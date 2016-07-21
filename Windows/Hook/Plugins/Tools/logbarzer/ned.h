#include <windows.h>

#define ETH_HEADER_LENGTH	0xE

#define ETH_P_LOOP	0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_ECHO	0x0200		/* Ethernet Echo packet		*/
#define ETH_P_PUP	0x0400		/* Xerox PUP packet		*/
#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_X25	0x0805		/* CCITT X.25			*/
#define ETH_P_ARP	0x0806		/* Address Resolution packet	*/
#define	ETH_P_BPQ	0x08FF		/* G8BPQ AX.25 Ethernet Packet	[ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_DEC       0x6000          /* DEC Assigned proto           */
#define ETH_P_DNA_DL    0x6001          /* DEC DNA Dump/Load            */
#define ETH_P_DNA_RC    0x6002          /* DEC DNA Remote Console       */
#define ETH_P_DNA_RT    0x6003          /* DEC DNA Routing              */
#define ETH_P_LAT       0x6004          /* DEC LAT                      */
#define ETH_P_DIAG      0x6005          /* DEC Diagnostics              */
#define ETH_P_CUST      0x6006          /* DEC Customer use             */
#define ETH_P_SCA       0x6007          /* DEC Systems Comms Arch       */
#define ETH_P_RARP      0x8035		/* Reverse Addr Res packet	*/
#define ETH_P_ATALK	0x809B		/* Appletalk DDP		*/
#define ETH_P_AARP	0x80F3		/* Appletalk AARP		*/
#define ETH_P_IPX	0x8137		/* IPX over DIX			*/
#define ETH_P_IPV6	0x86DD		/* IPv6 over bluebook		*/
#define ETH_P_PPP_DISC	0x8863		/* PPPoE discovery messages     */
#define ETH_P_PPP_SES	0x8864		/* PPPoE session messages	*/
#define ETH_P_ATMMPOA	0x884c		/* MultiProtocol Over ATM	*/
#define ETH_P_ATMFATE	0x8884		/* Frame-based ATM Transport*/

#pragma pack( push, 1)
typedef struct _ETH_HEADER {
	UCHAR	dstMac[6];	/* destination eth addr	*/
	UCHAR	srcMac[6];	/* source ether addr	*/
	USHORT	Type;		/* packet type ID field	*/
} ETH_HEADER, *PETH_HEADER;

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
    UCHAR   reserved[40];
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

#define IP_PROTO_TCP        0x06
#define IP_PROTO_UDP        0x11
#define IP_PROTO_ICMP       0x01
#define IP_PROTO_ICMPV6     0x3A

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

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/

typedef struct _ICMP_HEADER {
	UCHAR	Type;
	UCHAR	Code;
	USHORT	CheckSum;
	union 
	{
		struct 
		{
			USHORT	Id;
			USHORT	Sequence;
		} echo;
		struct
		{
			ULONG	Address;
			ULONG	enh_hdr;
		} redirect;


	}	Params;	
} ICMP_HEADER, *PICMP_HEADER;

#pragma pack( pop )

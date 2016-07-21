#ifndef _KLIM5_SEND_PACKET_H__
#define _KLIM5_SEND_PACKET_H__

ULONG
KlimSendPacket( PVOID pPacketBuf, ULONG Size );

#ifndef htons
#define htons(_x_) (USHORT)(((_x_)<<8)|((_x_)>>8))
#endif

#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/

#pragma pack( push, 1)
typedef struct _ETH_HEADER {
    UCHAR	dstMac[6];	/* destination eth addr	*/
    UCHAR	srcMac[6];	/* source ether addr	*/
    USHORT	Type;		/* packet type ID field	*/
} ETH_HEADER, *PETH_HEADER;

#define     IP_HEADER_SIZE  20

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

#pragma pack (pop)

#endif
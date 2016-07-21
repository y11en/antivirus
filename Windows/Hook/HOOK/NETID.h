#ifndef _NETID_
#define _NETID_

#define FIELD_ID(Type, Size, Code)	( \
	(ULONG)(Type<<16) | ((ULONG)(Size<<8))&0x0000FF00 | (BYTE)Code )


#define MJ_PACKET_ANY_DIRECTION		(ULONG)0xFFFFFFFF //default
#define MJ_PACKET_OUT				(ULONG)0x00000001 //For outgoing packet
#define MJ_PACKET_IN				(ULONG)0x00000002 //For incoming packet
#define MJ_PACKET_THROUGH			(ULONG)0x00000004 //For pass-through packet (if network adapter use promiscuous mode)


#define Mi_ETH				0x0001
//reserved 2 - 15 for others mediums (Token Ring, FDDI, PPP, ...)

#define Mi_ARP				0x0010
#define Mi_IP				0x0020
//reserved 0x20 - 0xF0 for others protocols (RARP, IPX, ...)

#define Mi_TCP				0x0100
#define Mi_UDP				0x0200
#define Mi_ICMP				0x0300

//packet field ID's
#define ID_ETH_DEST			FIELD_ID(Mi_ETH, 6, 1)
#define ID_ETH_SOURCE		FIELD_ID(Mi_ETH, 6, 2)
#define ID_ETH_PROTO		FIELD_ID(Mi_ETH, 2, 3)
#define ID_ETH_DSAP			FIELD_ID(Mi_ETH, 0, 4)
#define ID_ETH_SSAP			FIELD_ID(Mi_ETH, 0, 5)
#define ID_ETH_LLC_CONTROL	FIELD_ID(Mi_ETH, 0, 6)
#define ID_ETH_LLC_MODE		FIELD_ID(Mi_ETH, 0, 7)
#define ID_ETH_SNAP_OUI		FIELD_ID(Mi_ETH, 0, 8)
#define ID_ETH_SNAP_TYPE	FIELD_ID(Mi_ETH, 0, 9)

#define ID_ARP_HRD_TYPE				FIELD_ID(Mi_ARP, 0, 1)
#define ID_ARP_PRO_TYPE				FIELD_ID(Mi_ARP, 0, 2)
#define ID_ARP_HRD_SIZE				FIELD_ID(Mi_ARP, 0, 3)
#define ID_ARP_PRO_SIZE				FIELD_ID(Mi_ARP, 0, 4)
#define ID_ARP_COMMAND				FIELD_ID(Mi_ARP, 0, 5)
#define ID_ARP_SENDER_MAC_ADDRESS	FIELD_ID(Mi_ARP, 0, 6)
#define ID_ARP_SENDER_IP_ADDRESS	FIELD_ID(Mi_ARP, 0, 7)
#define ID_ARP_TARGET_MAC_ADDRESS	FIELD_ID(Mi_ARP, 0, 8)
#define ID_ARP_TARGET_IP_ADDRESS	FIELD_ID(Mi_ARP, 0, 9)

#define ID_IP_HDR_LENGTH			FIELD_ID(Mi_IP, 0, 1)
#define ID_IP_VERSION				FIELD_ID(Mi_IP, 0, 2)
#define ID_IP_TYPE_OF_SERVICE		FIELD_ID(Mi_IP, 0, 3)
#define ID_IP_TOTAL_LENGTH			FIELD_ID(Mi_IP, 0, 4)
#define ID_IP_IDENTIFICATION		FIELD_ID(Mi_IP, 0, 5)
#define ID_IP_FRAGMENTATION_FLAG	FIELD_ID(Mi_IP, 0, 6)
#define ID_IP_FRAGMENTATION_OFFSET	FIELD_ID(Mi_IP, 0, 7)
#define ID_IP_TIME_TO_LIVE			FIELD_ID(Mi_IP, 0, 8)
#define ID_IP_PROTOCOL				FIELD_ID(Mi_IP, 0, 9)
#define ID_IP_SRC_ADDRESS			FIELD_ID(Mi_IP, 0, 10)
#define ID_IP_DST_ADDRESS			FIELD_ID(Mi_IP, 0, 11)
#define ID_IP_OPTIONS				FIELD_ID(Mi_IP, 0, 12)
#define ID_IP_OPCODE				FIELD_ID(Mi_IP, 0, 13)

#define ID_TCP_SRC_PORT				FIELD_ID(Mi_TCP, 0, 1)
#define ID_TCP_DST_PORT				FIELD_ID(Mi_TCP, 0, 2)
#define ID_TCP_SEQ_NUMBER			FIELD_ID(Mi_TCP, 0, 3)
#define ID_TCP_ASK_SEQ_NUMBER		FIELD_ID(Mi_TCP, 0, 4)
#define ID_TCP_DATA_OFFSET			FIELD_ID(Mi_TCP, 0, 5)
#define ID_TCP_URG					FIELD_ID(Mi_TCP, 0, 6)
#define ID_TCP_ACK					FIELD_ID(Mi_TCP, 0, 7)
#define ID_TCP_PSH					FIELD_ID(Mi_TCP, 0, 8)
#define ID_TCP_RST					FIELD_ID(Mi_TCP, 0, 9)
#define ID_TCP_SYN					FIELD_ID(Mi_TCP, 0, 10)
#define ID_TCP_FIN					FIELD_ID(Mi_TCP, 0, 11)
#define ID_TCP_WIN					FIELD_ID(Mi_TCP, 0, 12)

#define ID_UDP_SRC_PORT				FIELD_ID(Mi_UDP, 0, 1)
#define ID_UDP_DST_PORT				FIELD_ID(Mi_UDP, 0, 2)
#define ID_UDP_TOTAL_LENGTH			FIELD_ID(Mi_UDP, 0, 3)

#define ID_ICMP_TYPE				FIELD_ID(Mi_ICMP, 0, 1)
#define ID_ICMP_CODE				FIELD_ID(Mi_ICMP, 0, 2)
#define ID_ICMP_UNI_CODE			FIELD_ID(Mi_ICMP, 2, 3)
#endif //_NETID_


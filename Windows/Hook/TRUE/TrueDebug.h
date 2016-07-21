#ifndef __TRUE_DEBUG_H_
#define __TRUE_DEBUG_H_

//extern ULONG DebugTrueMask;
//extern ULONG DebugTrueLevel;


char g_ExtName[] = "True parser"

//debug categories
#define DTRUE_UNKNOWN			0
#define DTRUE_ETH_DIX		1
#define DTRUE_ETH_II		2
#define DTRUE_ETH_SNAP		3
#define DTRUE_IPv4			4
#define DTRUE_ARP			5
#define DTRUE_TCP			6
#define DTRUE_UDP			7
#define DTRUE_ICMP			8


#define DC_TRUE_UNKNOWN			(1L << DTRUE_UNKNOWN)
#define DC_TRUE_ETH_DIX		(1L << DTRUE_ETH_DIX)
#define DC_TRUE_ETH_II		(1L << DTRUE_ETH_II)
#define DC_TRUE_ETH_SNAP	(1L << DTRUE_ETH_SNAP)
#define DC_TRUE_IPv4		(1L << DTRUE_IPv4)
#define DC_TRUE_ARP			(1L << DTRUE_ARP)
#define DC_TRUE_TCP			(1L << DTRUE_TCP)
#define DC_TRUE_UDP			(1L << DTRUE_UDP)
#define DC_TRUE_ICMP		(1L << DTRUE_ICMP)



CHAR *TrueCatName[] = {
		"Unknown",
		"ETH_DIX",
		"ETH_II",
		"ETH_SNAP",
		"IPv4",
		"ARP",
		"TCP",
		"UDP",
		"ICMP"
};

#endif //__TRUE_DEBUG_H_
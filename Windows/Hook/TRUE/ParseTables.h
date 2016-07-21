#ifndef _PARSE_TABLES_
#define _PARSE_TABLES_

#include "Protocols\Eth.h"
#include "Protocols\IPv4.h"
#include "Protocols\ARP.h"
#include "Protocols\TCP.h"
#include "Protocols\Udp.h"
#include "Protocols\icmp.h"
#include "NtBased.h"




/*
static ParseUnit	T_SEND_PARAM[]= {
	{1, NULL, NULL},
	{0, sendParams, NULL}};

static ParseUnit	T_IP_PROT[]= {
	{3, NULL, NULL},
	{IP_P_TCP, protTCP, T_SEND_PARAM},
	{IP_P_UDP, protUDP, T_SEND_PARAM},
	{IP_P_ICMP, protICMP, T_SEND_PARAM}};
*/

/*static ParseUnit	T_RESULT[]= {
	{1, NULL, NULL},
	{

*/
static ParseUnit	T_CHECK[]= 
{
	{1, NULL,		NULL	},
	{0, ProPacket,	NULL	}
};
static ParseUnit	T_TCP_LEVEL[]= 
{
	{3,			NULL,		NULL},
	{IP_P_TCP,	protTCP,	T_CHECK},
	{IP_P_UDP,	protUDP,	T_CHECK},
	{IP_P_ICMP, protICMP,	T_CHECK}
};


static ParseUnit	T_IP_LEVEL[]= 
{
	{1,			NULL,		NULL		},
	{ETH_P_IP,	protIP,		T_TCP_LEVEL	}
};

static ParseUnit	T_ETH_EXT[]= 
{	
	{3,						NULL,		NULL		},
	{MJ_ID_ETH_RAW_802_3,	protIPX,	T_CHECK		},
	{MJ_ID_ETH_LLC_802_3,	protLLC,	T_IP_LEVEL	},
	{MJ_ID_ETH_SNAP,		protSNAP,	T_IP_LEVEL	}
};

static ParseUnit	T_ETH_TYPES[]= 
{
	{1,					NULL,			T_IP_LEVEL	},
	{MJ_ID_ETH_802_3,	protEth_802_3,	T_ETH_EXT	}
};


static ParseUnit	ROOT[] = 
{
	{2,			NULL,		NULL		},
	{MT_Eth,	protEth,	T_ETH_TYPES	},
	{MT_Eth,	protTrap,	NULL		}
};


#endif //_PARSE_TABLES_

#ifndef _KLOP_CUSTOM_PACKET_H__
#define _KLOP_CUSTOM_PACKET_H__

#include "kldef.h"
#include "kl_object.h"
#include "klstatus.h"
#include "..\hook\klop_api.h"
#include "..\hook\klim_api.h"

#define     IP_HEADER_SIZE  20

bool
IsLocalMac( char Mac[6] );

KLIM_PKT_STATUS
ProcessInPacket( PVOID pkt );

KLIM_PKT_STATUS
ProcessOutPacket( PVOID pkt );

extern PKLIM_TABLE ext_KlimTable;

#endif
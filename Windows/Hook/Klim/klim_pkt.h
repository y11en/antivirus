#ifndef _KLIM_PACKETS_H__
#define _KLIM_PACKETS_H__

#ifdef _WIN64
#pragma pack ( push, 8 )
#endif

#include "ndis.h"

#ifdef _WIN64
#pragma pack ( pop )
#endif


#include "..\hook\klim_api.h"

#define KLIM_PKT_SEND_NDIS_PKT   0x1
#define KLIM_PKT_RECV_NDIS_PKT   0x2
#define KLIM_PKT_RECV_BUFF_PKT   0x3



typedef struct _KLIM_PACKET {
    ULONG   PktType;
    ULONG   RefCount;
    KLIM_PKT_STATUS Status;  // когда RefCount == 0, то учитывает этот статус
    LIST_ENTRY ListEntry;
    ULONG   isAllocated;  // в Deref память нужно освобождать....
    char    LocalMac[6]; // вообще-то это нужно для клопа
    ULONG   IfIndex;

    RefPktHandler   Ref;
    DerefPktHandler Deref;
    GetRefCountHandler GetRefCount;
    GetSizeHandler  GetSize;
    GetDataHandler  GetData;
    SetStatusHandler SetStatus;

    CHAR    Packet[0];
} KLIM_PACKET, *PKLIM_PACKET;


extern
VOID
KlimPktSetStatus( PVOID pktContext, KLIM_PKT_STATUS Status );


extern 
BOOLEAN
KlimGetLocalMac( PVOID pktContext, char MacAddr[6] );

#endif

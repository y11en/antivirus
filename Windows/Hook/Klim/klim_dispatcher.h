#ifndef _KLIM_DISPATCHER_H__
#define _KLIM_DISPATCHER_H__

#include "..\hook\klim_api.h"
#include "klim_pkt.h"
#include "..\KLSDK\klim_sync.h"

#define CHECK_SIZE( _size_, _min_ ) \
if ( (_size_) < (_min_) ) \
{       \
    ntStatus = STATUS_BUFFER_TOO_SMALL; \
    dwRet = (_min_) ; \
    break; \
}

VOID
KlimTable_Reset();

NTSTATUS
KlimDispatcher(
               IN PDEVICE_OBJECT    DeviceObject,
               IN PIRP              Irp
               );

NTSTATUS
SendIoctl( ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* pRet  );


extern KLIM_TABLE KlimTable;

ULONG
Pkt_GetSize( PVOID pktContext );

ULONG
Pkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count );

NDIS_STATUS
Pkt_Ref( PVOID pktContext );

NDIS_STATUS
Pkt_Deref( PVOID pktContext );

ULONG
Pkt_GetRefCount( PVOID pktContext );

VOID
Pkt_SetStatus( PVOID pktContext, KLIM_PKT_STATUS Status );

NTSTATUS
Pkt_QueryPacket( PVOID pktContext, KlimInformation Info, PVOID Buffer, ULONG Size, ULONG* dwRet );

#define MAX_KLIM_PLUGINS 100
#define TABLE2_VERSION  1

typedef struct _KLIM_CALLBACK {
    LIST_ENTRY  ListEntry;
    ProcessPacketHandler ProcessPkt;
    ULONG       Priority;
    UCHAR       IsEnabled;
} KLIM_CALLBACK, *PKLIM_CALLBACK;

typedef struct KLIM_TABLE2 {
    ULONG       Version;
    
    KSPIN_LOCK  InHandlersLock;    
    LIST_ENTRY  InHandlers;

    LIST_ENTRY  OutHandlers;
    KSPIN_LOCK  OutHandlersLock;    
} KLIM_TABLE2, *PKLIM_TABLE2;

extern KLIM_TABLE2 KlimTable2;

void
Klim2TableInit();

void
Klim2ProcessHandlers( PKLIM_PACKET pkt, PLIST_ENTRY HandlersList, PKSPIN_LOCK pLock );

void
AttachCallbacks(PKLIM_CALLBACK_STRUCT cb, PLIST_ENTRY List, PKSPIN_LOCK Lock );

BOOLEAN
IsLocalMac( char Mac[6] );

void
SetBroadcastIP(char Mac[6], ULONG IP);

#endif
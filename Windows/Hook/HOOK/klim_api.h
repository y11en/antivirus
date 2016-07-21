#ifndef _KLIM_API_H__
#define _KLIM_API_H__

#define KLIM_API_VERSION  0x4

#define KLIM_FILE_DEVICE_SECTION    0x00008005

#define KLIM_IOCTL_INDEX            0x841

typedef ULONG KLIM_PKT_STATUS;

#define KLIM_PKT_PASS   0x1
#define KLIM_PKT_BLOCK  0x2
#define KLIM_PKT_DELAY  0x3

#define KLIM_EVENT_NAME L"\\KlimEvent"

//#include "ndis.h"

typedef
KLIM_PKT_STATUS
(*ProcessPacketHandler) ( PVOID pktContext);

typedef
NDIS_STATUS
(*RefPktHandler) ( PVOID pktContext );

typedef
NDIS_STATUS
(*DerefPktHandler) ( PVOID pktContext );

typedef
ULONG
(*GetRefCountHandler) ( PVOID pktContext );

// возврящает размер пакета
typedef
ULONG
(*GetSizeHandler) ( PVOID pktContext );

typedef
VOID
(*SetStatusHandler) ( PVOID pktContext, KLIM_PKT_STATUS Status );

// возвращает размер скопированных данных.
typedef
ULONG
(*GetDataHandler) ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count );

typedef
BOOLEAN
(*GetLocalMacHandler) ( PVOID pktContext, char Mac[6] );

typedef
ULONG
(*SendRawPacket)( PVOID pPacketBuf, ULONG Size );

typedef
BOOLEAN
(*f_IsLocalMac)( char Mac[6] );

typedef
void
(*f_SetBroadcastIp)( char Mac[6], ULONG IP );

typedef enum {
    KlimUndefined = 0,
    KlimQueryMac,
    KlimQueryIF
} KlimInformation;

typedef
NTSTATUS
(*QueryKlimPacket)( PVOID pktContext, KlimInformation Info, PVOID Buffer, ULONG Size, ULONG* dwRet );

// outBuffer : ULONG
#define KLIM_GET_VERSION CTL_CODE(KLIM_FILE_DEVICE_SECTION,				\
						KLIM_IOCTL_INDEX,								\
						METHOD_BUFFERED,								\
						FILE_ANY_ACCESS)

typedef struct _KLIM_TABLE {
    ULONG Version;
    ProcessPacketHandler PreProcessInPkt;
    ProcessPacketHandler PreProcessOutPkt;    

    ProcessPacketHandler ProcessInPkt;
    ProcessPacketHandler ProcessOutPkt;  

    RefPktHandler   Ref;
    DerefPktHandler Deref;
    GetRefCountHandler GetRefCount;
    GetSizeHandler  GetSize;
    GetDataHandler  GetData;
    SetStatusHandler SetStatus;

    GetLocalMacHandler GetLocalMacFromPacket;


    SendRawPacket   SendPacket;
    f_IsLocalMac    IsLocalMac;
    f_SetBroadcastIp SetBroadcastIp;

    QueryKlimPacket QueryPacket;
} KLIM_TABLE, *PKLIM_TABLE;

#define KLIMPKT_QUERY_MAC           0x1
#define KLIMPKT_QUERY_IF_INDEX      0x2

// InBuffer = &pKlinTable;
#define KLIM_GET_TABLE CTL_CODE(KLIM_FILE_DEVICE_SECTION,				\
						KLIM_IOCTL_INDEX + 1,							\
						METHOD_BUFFERED,								\
						FILE_ANY_ACCESS)

#define KLIM_SEND_PKT CTL_CODE(KLIM_FILE_DEVICE_SECTION,				\
						KLIM_IOCTL_INDEX + 2,							\
						METHOD_BUFFERED,								\
						FILE_ANY_ACCESS)

// чем больше значение приоритета, тем раньше вызовется калбэк на прием пакета,
// и тем позже вызовется калбэк на отправку пакета.
#define KLIM_PRIORITY_ZERO  0
#define KLIM_PRIORITY_KLICK 1000
#define KLIM_PRIORITY_KLOP  2000

typedef struct _KLIM_CALLBACK_STRUCT {
    ULONG                Priority;
    ProcessPacketHandler ProcessPkt;
} KLIM_CALLBACK_STRUCT, *PKLIM_CALLBACK_STRUCT;

typedef struct _KLIM_REGISTER_CB {

    KLIM_CALLBACK_STRUCT InCallback;
    KLIM_CALLBACK_STRUCT OutCallback;

} KLIM_REGISTER_CB,*PKLIM_REGISTER_CB;
#define KLIM_REGISTER_CALLBACKS CTL_CODE(KLIM_FILE_DEVICE_SECTION,				\
						KLIM_IOCTL_INDEX + 3,							\
						METHOD_BUFFERED,								\
						FILE_ANY_ACCESS)

#endif
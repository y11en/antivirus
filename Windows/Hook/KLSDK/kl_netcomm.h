#ifndef _NETWORK_DRIVER_COMMUNICATIONS_H__
#define _NETWORK_DRIVER_COMMUNICATIONS_H__

#include "kldef.h"
#include "kl_ipv6.h"

typedef struct _TDI_COMPLETION_CONTEXT
{
    KEVENT kCompleteEvent;

} TDI_COMPLETION_CONTEXT, *PTDI_COMPLETION_CONTEXT;

typedef struct _TDI_HANDLE {
    HANDLE hConnection;
    HANDLE hTransport;

    PFILE_OBJECT pfoConnection;
    PFILE_OBJECT pfoTransport;

} TDI_HANDLE, *PTDI_HANDLE;

#ifndef STATUS_CONTINUE_COMPLETION
#define STATUS_CONTINUE_COMPLETION  STATUS_SUCCESS
#endif

NTSTATUS TdiFuncs_InitializeTransportHandles( USHORT AddrType, PTDI_HANDLE pTdiHandle);

void TdiFuncs_FreeHandles(PTDI_HANDLE pTdiHandle);

NTSTATUS TdiExample_ClientEventReceive(
                                       PVOID TdiEventContext, 
                                       CONNECTION_CONTEXT ConnectionContext, 
                                       ULONG ReceiveFlags, 
                                       ULONG  BytesIndicated, 
                                       ULONG  BytesAvailable, 
                                       ULONG  *BytesTaken, 
                                       PVOID  Tsdu, 
                                       PIRP  *IoRequestPacket);

NTSTATUS 
TdiFuncs_OpenTransportAddress(
                USHORT           AddrType,
                HANDLE*         pTdiHandle, 
                PFILE_OBJECT    *pFileObject
                );

NTSTATUS 
TdiFuncs_OpenConnection(
                USHORT AddrType,
                PHANDLE         pTdiHandle, 
                PFILE_OBJECT    *pFileObject
                );

NTSTATUS 
TdiFuncs_AssociateTransportAndConnection(
                HANDLE          hTransportAddress, 
                PFILE_OBJECT    pfoConnection
                );


NTSTATUS 
TdiFuncs_Connect(
                PFILE_OBJECT pfoConnection, 
                _ip_addr_t* rAddress, 
                USHORT rPort
                );


NTSTATUS 
TdiFuncs_Send(
                PFILE_OBJECT    pfoConnection, 
                PVOID           pData, 
                UINT            uiSendLength, 
                UINT            *pDataSent
                );

NTSTATUS 
TdiFuncs_SetEventHandler(
                PFILE_OBJECT    pfoTdiFileObject, 
                LONG            InEventType, 
                PVOID           InEventHandler, 
                PVOID           InEventContext
                );

NTSTATUS 
TdiFuncs_Disconnect(
                PFILE_OBJECT    pfoConnection
                );

NTSTATUS 
TdiFuncs_DisAssociateTransportAndConnection(
                PFILE_OBJECT    pfoConnection
                );

NTSTATUS 
TdiFuncs_CloseTdiOpenHandle(
                HANDLE          hTdiHandle, 
                PFILE_OBJECT    pfoTdiFileObject);

#endif
#ifdef _WIN64
#pragma pack ( push, 8 )
#endif

#include "ntddk.h"
#include "ndis.h"

#ifdef _WIN64
#pragma pack ( pop )
#endif

#include "klim_dispatcher.h"
#include "klim_pkt.h"
#include "IM5\sendpkt.h"

#include "..\hook\klick_api.h"

KLIM_TABLE KlimTable;

KLIM_TABLE2 KlimTable2;

ULONG g_InCount = 10;

KLIM_PKT_STATUS
KlimInStub ( PVOID pktContext )
{   
    Klim2ProcessHandlers( pktContext, &KlimTable2.InHandlers, &KlimTable2.InHandlersLock );
    
    return KLIM_PKT_PASS;

    KlimTable.SetStatus ( pktContext, KLIM_PKT_PASS );

    return KLIM_PKT_PASS;
}

ULONG g_OutCount = 10;

KLIM_PKT_STATUS
KlimOutStub ( PVOID pktContext )
{
    Klim2ProcessHandlers( pktContext, &KlimTable2.OutHandlers, &KlimTable2.OutHandlersLock );

    return KLIM_PKT_PASS;

    KlimTable.SetStatus ( pktContext, KLIM_PKT_PASS );

    return KLIM_PKT_PASS;
}

VOID
KlimTable_Reset()
{
    KlimTable.Version = KLIM_API_VERSION;
    KlimTable.ProcessInPkt = KlimInStub;
    KlimTable.PreProcessInPkt = KlimInStub;
    KlimTable.ProcessOutPkt = KlimOutStub;
    KlimTable.PreProcessOutPkt = KlimOutStub;
    
    KlimTable.Ref = Pkt_Ref;
    KlimTable.Deref = Pkt_Deref;
    KlimTable.GetRefCount = Pkt_GetRefCount;
    KlimTable.GetSize = Pkt_GetSize;
    KlimTable.GetData = Pkt_GetData;
    KlimTable.SetStatus = Pkt_SetStatus;
    KlimTable.IsLocalMac = IsLocalMac;
    KlimTable.GetLocalMacFromPacket = KlimGetLocalMac;
    
    KlimTable.SendPacket = KlimSendPacket;
    KlimTable.SetBroadcastIp = SetBroadcastIP;
    KlimTable.QueryPacket = Pkt_QueryPacket;
}

NTSTATUS
FindDevice( PCWSTR Name, PDEVICE_OBJECT* pDevObj, PFILE_OBJECT* pFileObj )
{
    NTSTATUS ntStatus;
    PDEVICE_OBJECT DevObj = NULL;
    PFILE_OBJECT fo;
    HANDLE hDriver;
    UNICODE_STRING DeviceName;	
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK ioStatus;	

    RtlInitUnicodeString( &DeviceName, Name );

    InitializeObjectAttributes( &oa, &DeviceName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    if ( NT_SUCCESS ( ntStatus = ZwCreateFile( 
        &hDriver, 
        SYNCHRONIZE | FILE_ANY_ACCESS, 
        &oa, 
        &ioStatus, 
        NULL, 
        0, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        FILE_OPEN,  
        FILE_SYNCHRONOUS_IO_NONALERT, 
        NULL, 
        0) ) )
    {	
        if ( NT_SUCCESS ( ntStatus = ObReferenceObjectByHandle( 
            hDriver, 
            FILE_READ_DATA, 
            NULL,
            KernelMode, 
            (PVOID*)&fo, 
            NULL ) ))
        {
            DevObj = IoGetRelatedDeviceObject( fo );
        }
        else
        {
            ntStatus = STATUS_UNSUCCESSFUL;
        }

        ZwClose( hDriver );
    }
    else
    {
        ntStatus = STATUS_UNSUCCESSFUL;				
    }

    *pDevObj = DevObj;
    *pFileObj = fo;

    return ntStatus;
}

NTSTATUS
SendIoctl( ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* pRet  )
{
    PIRP				Irp;
    KEVENT				Event;
    IO_STATUS_BLOCK		ioStatus;
    NTSTATUS			ntStatus;
    PIO_STACK_LOCATION  irpSp;
    PDEVICE_OBJECT pDevObj;
    PFILE_OBJECT   pFileObj;
    ULONG dwRet = 0;

    PAGED_CODE();

    ntStatus = FindDevice( L"\\Device\\klick", &pDevObj, &pFileObj  );

    if ( ntStatus != STATUS_SUCCESS )
    {
        return ntStatus;
    }

    KeInitializeEvent( &Event, NotificationEvent, FALSE);

    Irp = IoBuildDeviceIoControlRequest( 
        MessageID,
        pDevObj,
        pInBuffer,
        InBufferSize,
        pOutBuffer,
        OutBufferSize,
        TRUE,
        &Event,
        &ioStatus);	

    if ( Irp ) 
    {
        irpSp = IoGetNextIrpStackLocation( Irp );        
        irpSp->FileObject = pFileObj;
        ntStatus = IoCallDriver ( (PDEVICE_OBJECT)pDevObj, Irp );

        if ( ntStatus  ==  STATUS_PENDING ) 
        {
            KeWaitForSingleObject( &Event, Executive, KernelMode, FALSE, NULL );

            ntStatus = ioStatus.Status;            
        }
        
        dwRet = (ULONG)ioStatus.Information;
    }

    if ( pRet )
        *pRet = dwRet;

    return ntStatus;
}

NTSTATUS
KlimDispatcher(
           IN PDEVICE_OBJECT    DeviceObject,
           IN PIRP              Irp
           )
{
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG nIoctl = IrpSp->Parameters.DeviceIoControl.IoControlCode;
    PVOID ioBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inbSize = IrpSp->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outbSize= IrpSp->Parameters.DeviceIoControl.OutputBufferLength;
    ULONG dwRet = 0;

    if ( IrpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL ||
         IrpSp->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL )
    {
        switch ( nIoctl )
        {
        case KLIM_GET_VERSION:
            CHECK_SIZE( outbSize, sizeof (ULONG) );
            *(ULONG*)ioBuffer = KLIM_API_VERSION;
            dwRet = sizeof ( ULONG );
            break;

        case KLIM_GET_TABLE:
            {                
                CHECK_SIZE( outbSize, sizeof (PVOID) );

                *(PVOID*)ioBuffer = &KlimTable;
                //pTable->pPtChars = NULL;
                //pTable->pMpChars = NULL;

                dwRet = sizeof ( PVOID );                
            }

            break;

        case KLIM_SEND_PKT:
            {
                KlimSendPacket( ioBuffer, inbSize );
            }
            break;

        case KLIM_REGISTER_CALLBACKS:
            {
                PKLIM_REGISTER_CB RegCB = (PKLIM_REGISTER_CB)ioBuffer;
                CHECK_SIZE( inbSize, sizeof (KLIM_REGISTER_CB) );                

                AttachCallbacks( &RegCB->InCallback,  &KlimTable2.InHandlers,  &KlimTable2.InHandlersLock );

                RegCB->OutCallback.Priority = 0xFFFFFFFF - RegCB->OutCallback.Priority;
                AttachCallbacks( &RegCB->OutCallback, &KlimTable2.OutHandlers, &KlimTable2.OutHandlersLock );
            }
            break;

        default:
            break;
        }
    }

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = dwRet;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return ntStatus;
}

ULONG
Pkt_GetSize( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;
    return p->GetSize( p );
}

ULONG
Pkt_GetData ( PVOID pktContext, PVOID Buffer, ULONG from, ULONG count )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    return p->GetData( p, Buffer, from, count );
}

NDIS_STATUS
Pkt_Ref( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    return p->Ref( p );
}

NDIS_STATUS
Pkt_Deref( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    return p->Deref( p );
}

ULONG
Pkt_GetRefCount( PVOID pktContext )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    return p->GetRefCount( p );
}

VOID
Pkt_SetStatus( PVOID pktContext, KLIM_PKT_STATUS Status )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    p->SetStatus( p, Status );
}

NTSTATUS
Pkt_QueryPacket( PVOID pktContext, KlimInformation Info, PVOID Buffer, ULONG Size, ULONG* dwRet )
{
    NTSTATUS    Status = STATUS_SUCCESS;
    ULONG   dwReturned = 0;
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    switch ( Info )
    {
    case KlimQueryMac:
        dwReturned = 6;
        
        if ( Size < 6 )
        {
            Status = STATUS_BUFFER_TOO_SMALL;            
            break;
        }
        RtlCopyMemory( Buffer, p->LocalMac, 6 );
        break;
    case KlimQueryIF:
        dwReturned = 4;
        if ( Size < 4 )
        {
            Status = STATUS_BUFFER_TOO_SMALL;
            break;
        }
        *(ULONG*)Buffer = p->IfIndex;
        break;
    default:
        Status = STATUS_UNSUCCESSFUL;
        break;
    }

    if ( dwRet )
        *dwRet = dwReturned;

    return Status;
}

//-------------------------------------------------------------------------------------

void
Klim2TableInit()
{
    RtlZeroMemory( &KlimTable2, sizeof ( KlimTable2 ) );

    KlimTable2.Version = TABLE2_VERSION;
    
    InitializeListHead( &KlimTable2.InHandlers );
    KeInitializeSpinLock( &KlimTable2.InHandlersLock );    
    
    InitializeListHead( &KlimTable2.OutHandlers );
    KeInitializeSpinLock( &KlimTable2.OutHandlersLock );    
}

void
Klim2ProcessHandlers( PKLIM_PACKET pkt, PLIST_ENTRY HandlersList, PKSPIN_LOCK pLock )
{
    PLIST_ENTRY pLE;
    KIRQL   OldIrql;
    PKLIM_CALLBACK KlimCallback;
    KLIM_PKT_STATUS KlimStatus;

    KeAcquireSpinLock( pLock, &OldIrql );

    if ( !IsListEmpty( HandlersList ) )
    {
        for ( pLE = HandlersList->Flink; pLE != HandlersList; pLE = pLE->Flink )
        {
            KlimCallback = CONTAINING_RECORD( pLE, KLIM_CALLBACK, ListEntry );

            KeReleaseSpinLock( pLock, OldIrql );

            KlimStatus = KlimCallback->ProcessPkt( pkt );

            KeAcquireSpinLock( pLock, &OldIrql );

            if ( KlimStatus == KLIM_PKT_BLOCK )
            {
                pkt->SetStatus( pkt, KLIM_PKT_BLOCK );
                break;
            }
        }
    }

    KeReleaseSpinLock( pLock, OldIrql );

    return;
}

void
AttachCallbacks(PKLIM_CALLBACK_STRUCT cb, PLIST_ENTRY List, PKSPIN_LOCK Lock )
{
    PKLIM_CALLBACK pCallback;    
    KIRQL OldIrql;    
    
    KeAcquireSpinLock( Lock, &OldIrql );

    pCallback = ExAllocatePoolWithTag( NonPagedPool, sizeof ( KLIM_CALLBACK ), 'klim' );

    if ( pCallback )
    {
        PLIST_ENTRY pInsertList = List;
        pCallback->Priority     = cb->Priority;
        pCallback->ProcessPkt   = cb->ProcessPkt;
        pCallback->IsEnabled    = TRUE;

        if ( !IsListEmpty( List ))
        {
            PLIST_ENTRY pLE;    
            PKLIM_CALLBACK pListCallback;

            for ( pLE = List->Flink; pLE != List; pLE = pLE->Flink )
            {
                pListCallback = CONTAINING_RECORD( pLE, KLIM_CALLBACK, ListEntry );

                if ( pListCallback->Priority < pCallback->Priority )
                {
                    // pCallback нужно вставить после pListCallback;
                    pInsertList = pListCallback->ListEntry.Flink;                    
                    break;
                }                    
            }
        }        
        
        InsertTailList( pInsertList, &pCallback->ListEntry );
    }

    KeReleaseSpinLock( Lock, OldIrql );
}


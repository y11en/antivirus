#ifdef _WIN64
#pragma pack ( push, 8 )
#endif

#include "ndis.h"

#ifdef _WIN64
#pragma pack ( pop )
#endif


#include "klim_sync.h"
#include "..\hook\klim_api.h"

HANDLE g_KlimSyncEvent = NULL;

HANDLE
KlimSyn_CreateEvent()
{
    NTSTATUS ntStatus;
    OBJECT_ATTRIBUTES                  oa;
    UNICODE_STRING                     us;
    HANDLE                              EventHandle;

    RtlInitUnicodeString(&us, KLIM_EVENT_NAME );
    InitializeObjectAttributes( &oa, &us, OBJ_CASE_INSENSITIVE, NULL, NULL );
    
    ntStatus = ZwCreateEvent( &EventHandle, EVENT_ALL_ACCESS, &oa, NotificationEvent, FALSE );

    if ( ntStatus == STATUS_SUCCESS )
    {
        // Евент создался. раньше его не существовало.
        return EventHandle;
    }

    ntStatus = ZwOpenEvent( &EventHandle, EVENT_ALL_ACCESS, &oa );

    if ( ntStatus == STATUS_SUCCESS )
    {
        // Евент открылся. его кто-то создал 
        return EventHandle;
    }

    return NULL;
}

void
KlimSyn_CloseEvent( HANDLE Event )
{
    if ( Event != (HANDLE)-1 )
    {
        ZwClose( Event );
    }
}

void
KlimSyn_WaitEvent( HANDLE Event )
{
    ZwWaitForSingleObject( Event, FALSE, NULL );
}

void
KlimSyn_SetEvent( HANDLE Event )
{
    ZwSetEvent( Event, NULL );
}

void 
KlinSyn_WaitThread(PKLIN_SYN_THREAD_CTX ThreadCtx)
{
    KlimSyn_WaitEvent( g_KlimSyncEvent );

    ThreadCtx->Callback( ThreadCtx->CallbackCtx );

    ExFreePool( ThreadCtx );

    PsTerminateSystemThread(STATUS_SUCCESS);
}

void
KlinSyn_StartWaitThread( KlimSynCallback Callback, PVOID CallbackCtx )
{
    HANDLE  ThreadHandle;
    OBJECT_ATTRIBUTES   oa;
    PKLIN_SYN_THREAD_CTX ThreadCtx;
    InitializeObjectAttributes(&oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

    ThreadCtx = ExAllocatePool( NonPagedPool, sizeof ( KLIN_SYN_THREAD_CTX ) );

    if ( ThreadCtx )
    {        
        ThreadCtx->Callback     = Callback;
        ThreadCtx->CallbackCtx  = CallbackCtx;

        if ( NT_SUCCESS( PsCreateSystemThread( 
                                &ThreadHandle, 
                                THREAD_ALL_ACCESS, 
                                &oa, 
                                NULL, 
                                NULL, 
                                (PKSTART_ROUTINE)KlinSyn_WaitThread, ThreadCtx )) )
        {
            if ( STATUS_SUCCESS == ObReferenceObjectByHandle( 
                                                ThreadHandle, 
                                                THREAD_ALL_ACCESS, 
                                                NULL, 
                                                KernelMode, 
                                                &ThreadCtx->pObject, 
                                                NULL ) )
            {
                // ObDereferenceObject( ThreadHandle );
            }
            
            ZwClose(ThreadHandle);
            return;
        }
    }
}